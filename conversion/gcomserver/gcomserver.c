/* ----------------------------- MNI Header -----------------------------------
@NAME       : gcomserver.c
@DESCRIPTION: Program to receive images from Philips GYROSCAN.
@GLOBALS    : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : $Log: gcomserver.c,v $
@MODIFIED   : Revision 1.1  1993-11-23 14:11:36  neelin
@MODIFIED   : Initial revision
@MODIFIED   :
---------------------------------------------------------------------------- */

#include <gcomserver.h>

typedef enum {
   WAITING_FOR_GROUP, WAITING_FOR_OBJECT, READY_FOR_OBJECT, 
   END_OF_GROUP, DISCONNECTING
} Server_state;

/* Do we keep files or are they temporary? */
static int Keep_files = TRUE;

int main(int argc, char *argv[])
{
   char *pname;
   Acr_File *afpin, *afpout;
   Acr_Status status;
   Server_state state;
   int acr_command, spi_command;
   Acr_Element element;
   Acr_Group group_list;
   Acr_Message input_message, output_message;
   int exit_status;
   char **file_list;
   int num_files;
   int cur_file;
   char file_prefix_string[256] = "gcomserver";
   char *file_prefix = file_prefix_string;
   FILE *fptemp;

   /* Re-open stderr */
   (void) freopen("gcomserver.log", "w", stderr);

   /* Print message at start */
   pname = argv[0];
   (void) fprintf(stderr, "%s: Started gyrocom server.\n", pname);

   /* Make connection */
   open_connection(argc, argv, &afpin, &afpout);

   /* Create file prefix. Create the temporary file to avoid file name 
      clashes */
   if (! Keep_files) {
      (void) tmpnam(file_prefix);
      fptemp = fopen(file_prefix, "w");
      if (fptemp == NULL) {
         (void) fprintf(stderr, "%s: Unable to create temporary file.\n",
                        pname);
         exit(EXIT_FAILURE);
      }
      (void) fclose(fptemp);
   }

   /* Loop while reading messages */
   state = WAITING_FOR_GROUP;
   while ((status=spi_input_message(afpin, &input_message)) == ACR_OK) {

      /* Get group list */
      group_list = acr_get_message_group_list(input_message);

      /* Get ACR-NEMA command */
      element = acr_find_group_element(group_list, ACR_Command);
      if (element == NULL) 
         acr_command = ACR_UNKNOWN_COMMAND;
      else 
         acr_command = acr_get_element_short(element);

      /* Get SPI command */
      element = acr_find_group_element(group_list, SPI_Command);
      if (element == NULL) 
         spi_command = SPI_UNKNOWN_COMMAND;
      else 
         spi_command = acr_get_element_short(element);

      /* Check command */
      if (acr_command == SENDq) {

         /* Deal with command */
         switch (spi_command) {

            /* Begin group copy */
         case GCBEGINq:
            if (state != WAITING_FOR_GROUP) {
               status = ACR_PROTOCOL_ERROR;
               state = DISCONNECTING;
               break;
            }
            output_message = gcbegin_reply(input_message, &num_files);
            file_list = MALLOC((size_t) num_files * sizeof(*file_list));
            cur_file = 0;
            state = WAITING_FOR_OBJECT;
            break;

            /* Ready */
         case READYq:
            if (state != WAITING_FOR_OBJECT) {
               status = ACR_PROTOCOL_ERROR;
               state = DISCONNECTING;
               break;
            }
            output_message = ready_reply(input_message);
            state = READY_FOR_OBJECT;
            break;

            /* Send */
         case SENDq:
            if ((state != WAITING_FOR_OBJECT) &&
                 (state != READY_FOR_OBJECT)) {
               status = ACR_PROTOCOL_ERROR;
               state = DISCONNECTING;
               break;
            }
            output_message = send_reply(input_message, file_prefix,
                                        &file_list[cur_file]);
            cur_file++;
            if (cur_file >= num_files) {
               state = END_OF_GROUP;
            }
            else {
               state = WAITING_FOR_OBJECT;
            }
            break;

            /* GCENDq */
         case GCENDq:
            if (state != END_OF_GROUP) {
               status = ACR_PROTOCOL_ERROR;
               state = DISCONNECTING;
               break;
            }
            /* Do something with the files */
            use_the_files(num_files, file_list);
            /* Remove the temporary files */
            cleanup_files(num_files, file_list);
            free_list(num_files, file_list);
            /* Create the output message */
            output_message = gcend_reply(input_message);
            break;

            /* Unknown command */
         default:
            status = ACR_PROTOCOL_ERROR;
            state = DISCONNECTING;
            break;
         }        /* End of switch on spi_command */

      }        /* End of if acr_command == SENDq */

      /* Handle cancel command */
      else if (acr_command == CANCELq) {
         if ((state == WAITING_FOR_OBJECT) || (state == READY_FOR_OBJECT)) {
            cleanup_files(num_files, file_list);
            free_list(num_files, file_list);
         }
         output_message = cancel_reply(input_message);
         state = WAITING_FOR_GROUP;
      }

      /* Illegal command */
      else {
         status = ACR_PROTOCOL_ERROR;
         state = DISCONNECTING;
      }

      /* Delete input message */
      acr_delete_message(input_message);

      /* Check for disconnection */
      if (state == DISCONNECTING) {
         break;
      }

      /* Send reply */
      status = spi_output_message(afpout, output_message);

      if (status != ACR_OK) break;

   }        /* End of loop over messages */

   /* Clean up files, if needed */
   if ((state == WAITING_FOR_OBJECT) || (state == READY_FOR_OBJECT)) {
      cleanup_files(num_files, file_list);
      free_list(num_files, file_list);
   }
   
   /* Remove the file prefix file */
   if (! Keep_files) {
      cleanup_files(1, &file_prefix);
   }

   /* Print final message */
   switch (status) {
   case ACR_OK:
   case ACR_END_OF_INPUT:
      exit_status = EXIT_SUCCESS;
      (void) fprintf(stderr, "%s: Finished transfer.\n", pname);
      break;
   case ACR_PROTOCOL_ERROR:
      exit_status = EXIT_FAILURE;
      (void) fprintf(stderr, "%s: Protocol error. Disconnecting.\n", pname);
      break;
   case ACR_OTHER_ERROR:
      exit_status = EXIT_FAILURE;
      (void) fprintf(stderr, "%s: I/O error. Disconnecting.\n", pname);
      break;
   default:
      exit_status = EXIT_FAILURE;
      (void) fprintf(stderr, "%s: Unknown error. Disconnecting.\n", pname);
      break;
   }

   exit(exit_status);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : cleanup_files
@INPUT      : num_files - number of files in list
              file_list - array of file names
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Removes files.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void cleanup_files(int num_files, char *file_list[])
{
   int i;

   if (Keep_files) return;

   for (i=0; i < num_files; i++) {
      (void) remove(file_list[i]);
   }

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : free_list
@INPUT      : num_files - number of files in list
              file_list - array of file names
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Frees strings in file list
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void free_list(int num_files, char **file_list)
{
   int i;

   for (i=0; i < num_files; i++) {
      if ( file_list[i] != NULL) {
         FREE(file_list[i]);
      }
   }
   FREE(file_list);

   return;
}

