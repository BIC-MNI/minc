/* ----------------------------- MNI Header -----------------------------------
@NAME       : reply.c
@DESCRIPTION: Routines for dealing with spi messages.
@GLOBALS    : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : $Log: reply.c,v $
@MODIFIED   : Revision 1.2  1993-11-24 12:09:32  neelin
@MODIFIED   : Changed to use new acr-nema dump function. spi_reply returns a group list
@MODIFIED   : instead of a message.
@MODIFIED   :
 * Revision 1.1  93/11/23  14:12:06  neelin
 * Initial revision
 * 
---------------------------------------------------------------------------- */

#include <gcomserver.h>

static int Do_logging = TRUE;

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
   unsigned short his_message_id = 1;
   unsigned short my_message_id = 1;
   char *his_station_id = "CANPHIMR000000";
   char *my_station_id =  "CANMNIPT010101";
   unsigned short my_session_id = 1;
   int num_files = 1;
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
   element = acr_find_group_element(group, SPI_Nr_data_objects);
   if (element != NULL)
      num_files = acr_get_element_short(element);

   /* Compose the reply */

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
         acr_create_element_short(SPI_Session_id, my_session_id));
      if (spi_command == GCBEGINp) {
         acr_group_add_element(group,
            acr_create_element_short(SPI_Nr_data_objects, num_files));
      }
      acr_group_add_element(group,
         acr_create_element_string(SPI_Operator_text, "OK"));
      acr_group_add_element(group,
         acr_create_element_string(SPI_Log_info, "OK"));
   }

   return group_list;
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
   Acr_Group group, group_list;
   Acr_Element element;

   /* Print log message */
   if (Do_logging) {
      (void) fprintf(stderr, "\n\nReceived GCBEGINq message:\n");
      acr_dump_message(stderr, input_message);
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
   group_list = spi_reply(input_message);

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

   /* Print log message */
   if (Do_logging) {
      (void) fprintf(stderr, "\n\nReceived READYq message:\n");
      acr_dump_message(stderr, input_message);
   }

   /* Create the reply */
   return make_message(spi_reply(input_message));

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
   Acr_Group group;
   char temp_name[256];

   /* Print log message */
   if (Do_logging) {
      (void) fprintf(stderr, "\n\nReceived SENDq message:\n");
      acr_dump_message(stderr, input_message);
   }

   /* Get the group list */
   group = acr_get_message_group_list(input_message);

   /* Create the new file name */
   (void) sprintf(temp_name, "%s____", file_prefix);
   *new_file_name = strdup(temp_name);

   /* Create the reply */
   return make_message(spi_reply(input_message));

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

   /* Print log message */
   if (Do_logging) {
      (void) fprintf(stderr, "\n\nReceived GCENDq message:\n");
      acr_dump_message(stderr, input_message);
   }

   /* Create the reply */
   return make_message(spi_reply(input_message));

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
   if (Do_logging) {
      (void) fprintf(stderr, "\n\nReceived CANCELq message:\n");
      acr_dump_message(stderr, input_message);
   }

   /* Create the reply */
   return make_message(spi_reply(input_message));

}

