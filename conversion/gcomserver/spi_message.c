/* ----------------------------- MNI Header -----------------------------------
@NAME       : spi_message.c
@DESCRIPTION: File containing routines for getting spi messages
@GLOBALS    : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : $Log: spi_message.c,v $
@MODIFIED   : Revision 1.1  1993-11-23 14:12:47  neelin
@MODIFIED   : Initial revision
@MODIFIED   :
---------------------------------------------------------------------------- */

#include <gcomserver.h>

/* SPI end-of-message string */
static char end_of_message[] = {'E', 'O', 'M', '\0'};
static int length_of_eom = 
   sizeof(end_of_message) / sizeof(end_of_message[0]);

/* ----------------------------- MNI Header -----------------------------------
@NAME       : spi_input_message
@INPUT      : afp
@OUTPUT     : message
@RETURNS    : status of input
@DESCRIPTION: Reads in a spi message
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Status spi_input_message(Acr_File *afp, Acr_Message *message)
{
   Acr_Status status;
   int i, ch;
   Acr_Element element;
   Acr_Group group_list;
   char *string;

   /* Read message */
   status = acr_input_message(afp, message);

   if (status != ACR_OK) return status;

   /* Read in end-of-message code */
   for (i=0; i < length_of_eom; i++) {
      ch = acr_getc(afp);
      if (ch != end_of_message[i]) {
         status = ACR_PROTOCOL_ERROR;
      }
   }

   /* Check for recognition codes */
   group_list = acr_get_message_group_list(*message);
   string = ACR_RECOGNITION_CODE;
   element = acr_find_group_element(group_list, ACR_Recognition_code);
   if ((element == NULL) || 
       strncmp(string, acr_get_element_string(element), strlen(string)) != 0) {
      status = ACR_PROTOCOL_ERROR;
      return status;
   }
   string = SPI_RECOGNITION_CODE;
   element = acr_find_group_element(group_list, SPI_Recognition_code);
   if ((element == NULL) || 
       strncmp(string, acr_get_element_string(element), strlen(string)) != 0) {
      status = ACR_PROTOCOL_ERROR;
      return status;
   }

   return status;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : spi_output_message
@INPUT      : afp
              message
@OUTPUT     : (nothing)
@RETURNS    : status of output
@DESCRIPTION: Writes out a spi message
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Status spi_output_message(Acr_File *afp, Acr_Message message)
{
   Acr_Status status;
   int i, ch;

   /* Write message */
   status = acr_output_message(afp, message);

   if (status != ACR_OK) return status;

   /* Write out end-of-message code */
   for (i=0; i < length_of_eom; i++) {
      ch = acr_putc(end_of_message[i], afp);
      if (ch == EOF) {
         status = ACR_OTHER_ERROR;
         return status;
      }
   }

   return status;
}

