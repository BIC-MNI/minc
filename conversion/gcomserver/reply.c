/* ----------------------------- MNI Header -----------------------------------
@NAME       : reply.c
@DESCRIPTION: Routines for dealing with spi messages.
@GLOBALS    : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : 
 * $Log: reply.c,v $
 * Revision 6.5  2001-03-19 18:38:35  neelin
 * Set implementation UID to have prefix specified in config file.
 *
 * Revision 6.4  2000/02/22 00:06:34  neelin
 * Added printing of project name into log file.
 *
 * Revision 6.3  2000/01/31 13:57:39  neelin
 * Added keyword to project file to allow definition of the local AEtitle.
 * A simple syntax allows insertion of the host name into the AEtitle.
 *
 * Revision 6.2  1999/10/29 17:52:05  neelin
 * Fixed Log keyword
 *
 * Revision 6.1  1997/09/12 23:13:28  neelin
 * Added ability to convert gyrocom images to dicom images.
 *
 * Revision 6.0  1997/09/12  13:23:50  neelin
 * Release of minc version 0.6
 *
 * Revision 5.1  1997/09/11  13:09:40  neelin
 * Added more complicated syntax for project files so that different things
 * can be done to the data. The old syntax is still supported.
 *
 * Revision 5.0  1997/08/21  13:24:50  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:01:07  neelin
 * Release of minc version 0.4
 *
 * Revision 3.0  1995/05/15  19:31:44  neelin
 * Release of minc version 0.3
 *
 * Revision 2.1  1995/02/14  18:12:26  neelin
 * Added project names and defaults files (using volume name).
 * Added process id to log file name.
 * Moved temporary files to subdirectory.
 *
 * Revision 2.0  1994/09/28  10:35:32  neelin
 * Release of minc version 0.2
 *
 * Revision 1.4  94/09/28  10:34:51  neelin
 * Pre-release
 * 
 * Revision 1.3  93/11/25  13:26:58  neelin
 * Working version.
 * 
 * Revision 1.2  93/11/24  12:09:32  neelin
 * Changed to use new acr-nema dump function. spi_reply returns a group list
 * instead of a message.
 * 
 * Revision 1.1  93/11/23  14:12:06  neelin
 * Initial revision
 * 
@COPYRIGHT  :
              Copyright 1993 Peter Neelin, McConnell Brain Imaging Centre, 
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

#include <gcomserver.h>

extern int Do_logging;

/* ----------------------------- MNI Header -----------------------------------
@NAME       : make_message
@INPUT      : group_list
@OUTPUT     : (nothing)
@RETURNS    : output message.
@DESCRIPTION: Convert a group list into a message.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 24, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Message make_message(Acr_Group group_list)
{
   Acr_Group next_group, group;
   Acr_Message output_message;

   /* Create the output message */
   output_message = acr_create_message();

   /* Loop through groups, adding them to the message */
   group = group_list;
   while (group != NULL) {
      next_group = acr_get_group_next(group);
      acr_set_group_next(group, NULL);
      acr_message_add_group(output_message, group);
      group = next_group;
   }

   return output_message;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : spi_reply
@INPUT      : input_message
@OUTPUT     : (nothing)
@RETURNS    : group_list for output message.
@DESCRIPTION: Composes reply for SPI messages
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Group spi_reply(Acr_Message input_message)
{
   Acr_Group group, group_list;
   Acr_Element element;
   unsigned short his_message_id = 0;
   static unsigned short my_message_id = 0;
   char *his_station_id = "CANPHIMR000000";
   char *my_station_id =  "CANMNIPT010101";
   unsigned short my_session_id = 0;
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

   /* Compose the reply. Increment message id number and increment
      session id number if GCBEGINp */
   my_message_id++;
   if ((acr_command == SENDp) && (spi_command == GCBEGINp))
      my_session_id++;

   /* Acr-nema group */
   group_list = group = acr_create_group(ACR_MESSAGE_GID);
   acr_group_add_element(group,
      acr_create_element_long(ACR_Length_to_eom, (unsigned short) 0));
   acr_group_add_element(group,
      acr_create_element_string(ACR_Recognition_code, ACR_RECOGNITION_CODE));
   acr_group_add_element(group,
      acr_create_element_short(ACR_Command, (unsigned short) acr_command));
   acr_group_add_element(group,
      acr_create_element_short(ACR_Message_id, my_message_id));
   acr_group_add_element(group,
      acr_create_element_short(ACR_Message_id_brt, his_message_id));
   acr_group_add_element(group,
      acr_create_element_string(ACR_Initiator, my_station_id));
   acr_group_add_element(group,
      acr_create_element_string(ACR_Receiver, his_station_id));
   acr_group_add_element(group,
      acr_create_element_short(ACR_Dataset_type,
                               (unsigned short) ACR_NULL_DATASET));
   acr_group_add_element(group,
      acr_create_element_short(ACR_Status, 
                               (unsigned short) ACR_SUCCESS));

   /* SPI group */
   if (acr_command == SENDp) {
      acr_set_group_next(group, acr_create_group(SPI_MESSAGE_GID));
      group = acr_get_group_next(group);
      acr_group_add_element(group,
         acr_create_element_string(SPI_Recognition_code, 
                                   SPI_RECOGNITION_CODE));
      acr_group_add_element(group,
         acr_create_element_short(SPI_Command, (unsigned short) spi_command));
      acr_group_add_element(group,
         acr_create_element_long(SPI_Status, (long) ACR_SUCCESS));
      acr_group_add_element(group,
         acr_create_element_short(SPI_Session_id, my_session_id));
      acr_group_add_element(group,
         acr_create_element_short(SPI_Dataset_type,
                                  (unsigned short) ACR_NULL_DATASET));
   }

   return group_list;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_ae_title
@INPUT      : title_format - string to be copied into ae_title, with 
                 substitutions. Currently, only %% (%) and %H (hostname in
                 uppercase) are recognized.
              max_ae_len - maximum length of output AE title. Does not
                 include terminating NUL
@OUTPUT     : ae_title - new AE title with substitutions
@RETURNS    : (nothing)
@DESCRIPTION: Routine to create an AE title give a format string that specifies
              substitutions:
                 %H = hostname in uppercase (first part only)
                 %% = %
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 17, 2000 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
void set_ae_title(char *ae_title, char *title_format, int max_ae_len)
{
   char host_name[128];
   int iin, iout, iname;

   /* Check if local AE title format is defined */
   if ((title_format == NULL) || (strlen(title_format) == 0)) {
      (void) strncpy(ae_title, "GCOMTODICOM", max_ae_len);
   }
   else {

      /* Copy the format */
      for (iin=0, iout=0; (title_format[iin]!='\0') && (iout<max_ae_len); 
           iin++, iout++) {

         /* Check for special formats */
         if (title_format[iin] == '%') {

            /* Get host name */
            if (title_format[iin+1] == 'H') {
               if (gethostname(host_name, sizeof(host_name)-1) != 0) {
                  host_name[0] = '\0';
               }

               /* Take only first part and convert to uppercase */
               for (iname=0; ((iname < max_ae_len-iout) &&
                              (host_name[iname] != '.') &&
                              (host_name[iname] != '\0')); iname++) {
                  ae_title[iout+iname] = 
                     (char) toupper((int) host_name[iname]);
               }
               iout += iname-1;

            }

            /* Replace %% with % */
            else if (title_format[iin+1] == '%') {
               ae_title[iout] = '%';
            }

            /* Copy other formats as is */
            else {
               ae_title[iout] = title_format[iin];
               iin--;
            }
            iin++;
         }

         /* Copy normal characters */
         else {
            ae_title[iout] = title_format[iin];
         }

      }

      /* Terminate the string */
      ae_title[iout] = '\0';

   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : gcbegin_reply
@INPUT      : input_message
@OUTPUT     : num_files - number of files needed
              project_name - name to use for project file
              project_info
@RETURNS    : output_message
@DESCRIPTION: Responds to GCBEGINq message
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Message gcbegin_reply(Acr_Message input_message, int *num_files,
                                 char **project_name, 
                                 Project_File_Info *project_info)
{
   Acr_Group group, group_list;
   Acr_Element element;
   char operator_string[512];
   char implementation_uid[65];
   char ae_title[MAX_AE_LEN+1];
   void *ptr;
   int index;

   /* Print log message */
   if (Do_logging >= HIGH_LOGGING) {
      (void) fprintf(stderr, "\n\nReceived GCBEGINq message:\n");
      acr_dump_message(stderr, input_message);
   }

   /* Free project_name string if needed */
   if (*project_name != NULL) FREE(*project_name);

   /* Get the group list */
   group_list = acr_get_message_group_list(input_message);

   /* Get the number of files */
   element = acr_find_group_element(group_list, SPI_Nr_data_objects);
   if (element != NULL)
      *num_files = acr_get_element_short(element);
   else
      *num_files = 1;

   /* Get the project name from the volume name */
   *project_name = strdup(acr_find_string(group_list, SPI_Volume_name, ""));

   /* Print out the project name */
   (void) fprintf(stderr, "Requested project name is \"%s\"\n", *project_name);

   /* Create the reply */
   group_list = spi_reply(input_message);

   /* Add to the reply */
   group = acr_get_group_next(group_list);
   acr_group_add_element(group,
      acr_create_element_short(SPI_Delay_time, (unsigned short) 0));
   acr_group_add_element(group,
      acr_create_element_short(SPI_Nr_data_objects, *num_files));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Operator_text, "OK"));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Log_info, "OK"));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Volume_name, "PET"));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Creation_date, "1993.11.23"));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Creation_time, "10:00:0000"));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Volume_type, "FOREIGN"));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Volume_status, "OPEN"));
   acr_group_add_element(group,
      acr_create_element_numeric(SPI_Space_left, (double) 1000000));
   acr_group_add_element(group,
      acr_create_element_numeric(SPI_Nr_exams, (double) 10));
   acr_group_add_element(group,
      acr_create_element_numeric(SPI_Nr_images, (double) 200));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Closing_date, "2010.11.23"));
   acr_group_add_element(group,
      acr_create_element_numeric(SPI_Perc_space_used, (double) 20));

   /* Check that the project file is okay - if not get a string listing 
      possible project names and send it back */
   operator_string[0] = '\0';
   if (read_project_file(*project_name, project_info)) {

      /* Print an error message */
      (void) fprintf(stderr, "Unknown project \"%s\"\n", *project_name);

      /* Set a message for the operator */
      (void) sprintf(operator_string, 
                     "Unknown volume, try remote DOR volume: ");
      index = strlen(operator_string);
      get_project_option_string(&operator_string[index],
                                sizeof(operator_string) - index);

   }

   /* Check for a dicom-type project */
   else if (project_info->type == PROJECT_DICOM) {

      /* Get the AE title */
      set_ae_title(ae_title, project_info->info.dicom.LocalAEtitle, 
                   MAX_AE_LEN);

      /* Write message in log file */
      (void) fprintf(stderr, "Connecting to host %s, port %s\n",
                     project_info->info.dicom.hostname,
                     project_info->info.dicom.port);
      (void) fprintf(stderr, "with AE title %s and local AE title %s\n",
                     project_info->info.dicom.AEtitle, ae_title);

      /* Set the implementation uid if a prefix is given */
      if (strlen(project_info->info.dicom.UIDprefix) > 0) {
         (void) sprintf(implementation_uid, "%s.100.1.1", 
                        project_info->info.dicom.UIDprefix);
         acr_set_implementation_uid(implementation_uid);
      }

      /* Make the dicom connection */
      if (!acr_open_dicom_connection(project_info->info.dicom.hostname,
                                     project_info->info.dicom.port,
                                     project_info->info.dicom.AEtitle,
                                     ae_title,
                                     ACR_MR_IMAGE_STORAGE_UID,
                                     ACR_IMPLICIT_VR_LITTLE_END_UID,
                                     &project_info->info.dicom.afpin,
                                     &project_info->info.dicom.afpout)) {
         (void) sprintf(operator_string, "Unable to connect to host %s",
                        project_info->info.dicom.hostname);
      }

   }

   /* Handle any error */
   if (operator_string[0] != '\0') {

      /* Set the project name to NULL as a signal to the caller */
      FREE(*project_name);
      *project_name = NULL;

      /* Set an error status */
      element = acr_find_group_element(group_list, ACR_Status);
      if (element != NULL) {
         ptr = acr_get_element_data(element);
         *((unsigned short *) ptr) = ACR_REFUSED;
      }
      element = acr_find_group_element(group_list, SPI_Status);
      if (element != NULL) {
         ptr = acr_get_element_data(element);
         *((long *) ptr) = SPI_MEDIUM_NOT_AVAIL;
      }

      /* Set a message for the operator */
      index = strlen(operator_string);
      if ((index % 2) != 0) {
         operator_string[index] = ' ';
         operator_string[index+1] = '\0';
      }
      element = acr_find_group_element(group_list, SPI_Operator_text);
      if (element != NULL) {
         ptr = acr_get_element_data(element);
         if (ptr != NULL) FREE(ptr);
         acr_set_element_data(element, strlen(operator_string),
                              strdup(operator_string));
      }
   }


   return make_message(group_list);

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
   Acr_Group group_list, group;

   /* Print log message */
   if (Do_logging >= HIGH_LOGGING) {
      (void) fprintf(stderr, "\n\nReceived READYq message:\n");
      acr_dump_message(stderr, input_message);
   }

   /* Create the reply */
   group_list = spi_reply(input_message);

   /* Add to the reply */
   group = acr_get_group_next(group_list);
   acr_group_add_element(group,
      acr_create_element_short(SPI_Delay_time, (unsigned short) 0));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Operator_text, "OK"));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Log_info, "OK"));

   return make_message(group_list);

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
public Acr_Message send_reply(Acr_Message input_message)
{
   Acr_Group group_list, group;

   /* Print log message */
   if (Do_logging >= HIGH_LOGGING) {
      (void) fprintf(stderr, "\n\nReceived SENDq message:\n");
      acr_dump_message(stderr, input_message);
   }

   /* Create the reply */
   group_list = spi_reply(input_message);

   /* Add to the reply */
   group = acr_get_group_next(group_list);
   acr_group_add_element(group,
      acr_create_element_string(SPI_Operator_text, "OK"));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Log_info, "OK"));

   return make_message(group_list);

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
   Acr_Group group_list, group;

   /* Print log message */
   if (Do_logging >= HIGH_LOGGING) {
      (void) fprintf(stderr, "\n\nReceived GCENDq message:\n");
      acr_dump_message(stderr, input_message);
   }

   /* Create the reply */
   group_list = spi_reply(input_message);

   /* Add to the reply */
   group = acr_get_group_next(group_list);
   acr_group_add_element(group,
      acr_create_element_string(SPI_Operator_text, "OK"));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Log_info, "OK"));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Volume_name, "PET"));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Creation_date, "1993.11.23"));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Creation_time, "10:00:0000"));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Volume_type, "FOREIGN"));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Volume_status, "OPEN"));
   acr_group_add_element(group,
      acr_create_element_numeric(SPI_Space_left, (double) 1000000));
   acr_group_add_element(group,
      acr_create_element_numeric(SPI_Nr_exams, (double) 10));
   acr_group_add_element(group,
      acr_create_element_numeric(SPI_Nr_images, (double) 200));
   acr_group_add_element(group,
      acr_create_element_string(SPI_Closing_date, "2010.11.23"));
   acr_group_add_element(group,
      acr_create_element_numeric(SPI_Perc_space_used, (double) 20));

   return make_message(group_list);

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
   /* Print log message */
   if (Do_logging >= HIGH_LOGGING) {
      (void) fprintf(stderr, "\n\nReceived CANCELq message:\n");
      acr_dump_message(stderr, input_message);
   }

   /* Create the reply */
   return make_message(spi_reply(input_message));

}

