/* ----------------------------- MNI Header -----------------------------------
@NAME       : reply.c
@DESCRIPTION: Routines for dealing with spi messages.
@GLOBALS    : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : $Log: reply.c,v $
@MODIFIED   : Revision 1.1  1993-11-23 14:12:06  neelin
@MODIFIED   : Initial revision
@MODIFIED   :
---------------------------------------------------------------------------- */

#include <gcomserver.h>

static int Do_logging = TRUE;

/* ----------------------------- MNI Header -----------------------------------
@NAME       : print_message_contents
@INPUT      : input_message
@OUTPUT     : (nothing)
@RETURNS    : (nothing)
@DESCRIPTION: Dump out message contents to stderr.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void print_message_contents(Acr_Message input_message)
{
   Acr_Group group;
   Acr_Element element;

   /* Loop over groups */
   group = acr_get_message_group_list(input_message);
   while (group != NULL) {

      /* Print the group id */
      (void) fprintf(stderr, "\nGroup 0x%04x :\n\n", 
                     acr_get_group_group(group));

      /* Loop over elements */
      element = acr_get_group_element_list(group);
      while (element != NULL) {

         /* Print the element id */
         (void) fprintf(stderr, "     0x%04x  0x%04x  length = %d\n",
                        acr_get_element_group(element),
                        acr_get_element_element(element),
                        (int) acr_get_element_length(element));

         element = acr_get_element_next(element);
      }

      group = acr_get_group_next(group);
   }

   /* Flush the buffer */
   (void) fflush(stderr);

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : spi_reply
@INPUT      : input_message
@OUTPUT     : (nothing)
@RETURNS    : output_message
@DESCRIPTION: Composes reply for SPI messages
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Message spi_reply(Acr_Message input_message)
{
   Acr_Group group;
   Acr_Element element;
   Acr_Message output_message;
   short his_message_id = 0;
   short my_message_id = 0;
   char *his_station_id = "CANPHIMR000000";
   char *my_station_id =  "CANMNIPT000101";
   short his_session_id = 0;
   int acr_command = ACR_UNKNOWN_COMMAND;
   int spi_command = SPI_UNKNOWN_COMMAND;

   /* Get the command */
   group = acr_get_message_group_list(input_message);
   element = acr_find_group_element(group, ACR_Command);
   if (element != NULL) 
      acr_command = acr_get_element_short(element);
   element = acr_find_group_element(group, SPI_Command);
   if (element != NULL) 
      spi_command = acr_get_element_short(element);

   /* Get the appropriate reply command */
   if (acr_command == CANCELq) {
      acr_command = CANCELp;
   }
   else {
      acr_command = SENDp;
      switch (spi_command) {
      case GCBEGINq:
         spi_command = GCBEGINp; break;
      case READYq:
         spi_command = READYp; break;
      case SENDq:
         spi_command = SENDp; break;
      case GCENDq:
         spi_command = GCENDp; break;
      default:
         spi_command = SENDp; break;
      }
   }

   /* Get information from input_message */
   element = acr_find_group_element(group, ACR_Message_id);
   if (element != NULL)
      his_message_id = acr_get_element_short(element);
   element = acr_find_group_element(group, ACR_Initiator);
   if (element != NULL)
      his_station_id = acr_get_element_string(element);
   

   /* Compose the reply */
   output_message = acr_create_message();

   /* Acr-nema group */
   group = acr_create_group(ACR_MESSAGE_GID);
   acr_group_add_element(group,
      acr_create_element_long(ACR_Length_to_eom, (short) 0));
   acr_group_add_element(group,
      acr_create_element_string(ACR_Recognition_code, ACR_RECOGNITION_CODE));
   acr_group_add_element(group,
      acr_create_element_short(ACR_Command, (short) acr_command));
   acr_group_add_element(group,
      acr_create_element_short(ACR_Message_id, my_message_id));
   acr_group_add_element(group,
      acr_create_element_short(ACR_Message_id_brt, his_message_id));
   acr_group_add_element(group,
      acr_create_element_string(ACR_Initiator, my_station_id));
   acr_group_add_element(group,
      acr_create_element_string(ACR_Receiver, his_station_id));
   acr_group_add_element(group,
      acr_create_element_short(ACR_Status, (short) ACR_SUCCESS));
   acr_message_add_group(output_message, group);

   /* SPI group */
   if (acr_command != CANCELq) {
      group = acr_create_group(SPI_MESSAGE_GID);
      acr_group_add_element(group,
         acr_create_element_string(SPI_Recognition_code, 
                                   SPI_RECOGNITION_CODE));
      acr_group_add_element(group,
         acr_create_element_short(SPI_Command, (short) spi_command));
      acr_group_add_element(group,
         acr_create_element_short(SPI_Session_id, his_session_id));
      acr_message_add_group(output_message, group);
   }

   return output_message;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : gcbegin_reply
@INPUT      : input_message
@OUTPUT     : num_files - number of files needed
@RETURNS    : output_message
@DESCRIPTION: Responds to GCBEGINq message
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Message gcbegin_reply(Acr_Message input_message, int *num_files)
{
   Acr_Message output_message;
   Acr_Group group;
   Acr_Element element;

   /* Print log message */
   if (Do_logging) {
      (void) fprintf(stderr, "\n\nReceived GCBEGINq message:\n");
      print_message_contents(input_message);
   }

   /* Get the group list */
   group = acr_get_message_group_list(input_message);

   /* Get the number of files */
   element = acr_find_group_element(group, SPI_Nr_data_objects);
   if (element != NULL)
      *num_files = acr_get_element_short(element);
   else
      *num_files = 1;

   /* Create the reply */
   output_message = spi_reply(input_message);

   /* Get the SPI group in the reply (2nd in list) */
   group = acr_get_group_next(acr_get_message_group_list(output_message));

   /* Add the number of data objects */
   acr_group_add_element(group,
      acr_create_element_short(SPI_Nr_data_objects, *num_files));

   return output_message;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ready_reply
@INPUT      : input_message
@OUTPUT     : (nothing)
@RETURNS    : output_message
@DESCRIPTION: Responds to READYq message
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Message ready_reply(Acr_Message input_message)
{
   Acr_Message output_message;

   /* Print log message */
   if (Do_logging) {
      (void) fprintf(stderr, "\n\nReceived READYq message:\n");
      print_message_contents(input_message);
   }

   /* Create the reply */
   output_message = spi_reply(input_message);

   return output_message;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : send_reply
@INPUT      : input_message
              file_prefix
@OUTPUT     : new_file_name (must be freed by caller)
@RETURNS    : output_message
@DESCRIPTION: Responds to SENDq message
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Message send_reply(Acr_Message input_message, char *file_prefix,
                              char **new_file_name)
{
   Acr_Message output_message;
   Acr_Group group;
   size_t new_file_length;
   char temp_name[256];

   /* Print log message */
   if (Do_logging) {
      (void) fprintf(stderr, "\n\nReceived SENDq message:\n");
      print_message_contents(input_message);
   }

   /* Get the group list */
   group = acr_get_message_group_list(input_message);

   /* Create the new file name */
   (void) sprintf(temp_name, "%s____", file_prefix);
   *new_file_name = strdup(temp_name);

   /* Create the reply */
   output_message = spi_reply(input_message);

   return output_message;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : gcend_reply
@INPUT      : input_message
@OUTPUT     : (nothing)
@RETURNS    : output_message
@DESCRIPTION: Responds to GCENDq message
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Message gcend_reply(Acr_Message input_message)
{
   Acr_Message output_message;

   /* Print log message */
   if (Do_logging) {
      (void) fprintf(stderr, "\n\nReceived GCENDq message:\n");
      print_message_contents(input_message);
   }

   /* Create the reply */
   output_message = spi_reply(input_message);

   return output_message;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : cancel_reply
@INPUT      : input_message
@OUTPUT     : (nothing)
@RETURNS    : output_message
@DESCRIPTION: Responds to CANCELq message
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Message cancel_reply(Acr_Message input_message)
{
   Acr_Message output_message;

   /* Print log message */
   if (Do_logging) {
      (void) fprintf(stderr, "\n\nReceived CANCELq message:\n");
      print_message_contents(input_message);
   }

   /* Create the reply */
   output_message = spi_reply(input_message);

   return output_message;
}

