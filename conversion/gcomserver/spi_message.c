/* ----------------------------- MNI Header -----------------------------------
@NAME       : spi_message.c
@DESCRIPTION: File containing routines for getting spi messages
@GLOBALS    : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : $Log: spi_message.c,v $
@MODIFIED   : Revision 1.5  1994-04-07 11:03:32  neelin
@MODIFIED   : Changed error handling to be more explicit about errors.
@MODIFIED   : When the server terminates due to an error, a message is printed to /dev/log.
@MODIFIED   : Changed handling of file cleanup.
@MODIFIED   :
 * Revision 1.4  93/11/30  14:42:32  neelin
 * Copies to minc format.
 * 
 * Revision 1.3  93/11/25  13:27:21  neelin
 * Working version.
 * 
 * Revision 1.2  93/11/24  12:10:06  neelin
 * 
 * Revision 1.1  93/11/23  14:12:47  neelin
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

/* SPI end-of-message string */
static char end_of_message[] = {'E', 'O', 'M', '\0'};
static int length_of_eom = 
   sizeof(end_of_message) / sizeof(end_of_message[0]);

/* ----------------------------- MNI Header -----------------------------------
@NAME       : input_end_of_message
@INPUT      : afp
@OUTPUT     : (none)
@RETURNS    : status of input
@DESCRIPTION: Reads in a spi end of message sequence.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 24, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Status input_end_of_message(Acr_File *afp)
{
   Acr_Status status;
   int ch;
   int i;

   status = ACR_OK;
   for (i=0; i < length_of_eom; i++) {
      ch = acr_getc(afp);
      if (ch != end_of_message[i]) {

         /* Kludge to handle EOS */
         if ((i != 2) || (ch != 'S')) {
            status = ACR_HIGH_LEVEL_ERROR;
         }

      }
   }

   return status;

}

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
   Acr_Element element;
   Acr_Group group_list, group;
   char *string;

   /* Read message */
   status = acr_input_message(afp, message);

   if (status != ACR_OK) return status;

   /* Read in end-of-message code */
   status = input_end_of_message(afp);

   /* Check for ACR-NEMA recognition code */
   group_list = acr_get_message_group_list(*message);
   string = ACR_RECOGNITION_CODE;
   element = acr_find_group_element(group_list, ACR_Recognition_code);
   if ((element == NULL) || 
       strncmp(string, acr_get_element_string(element), strlen(string)) != 0) {
      status = ACR_HIGH_LEVEL_ERROR;
      return status;
   }

   /* Check for SPI recognition code, if needed */
   group = acr_get_group_next(group_list);
   if ((group != NULL) && (acr_get_group_group(group) == SPI_MESSAGE_GID)) {
      string = SPI_RECOGNITION_CODE;
      element = acr_find_group_element(group_list, SPI_Recognition_code);
      if ((element == NULL) || 
          strncmp(string, acr_get_element_string(element), 
                  strlen(string)) != 0) {
         status = ACR_HIGH_LEVEL_ERROR;
         return status;
      }
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

   /* Flush the buffer */
   if (acr_file_flush(afp) == EOF) {
      status = ACR_OTHER_ERROR;
      return status;
   }

   return status;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : spi_input_data_object
@INPUT      : afp
@OUTPUT     : group_list
@RETURNS    : status of input
@DESCRIPTION: Reads in a spi data object
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 24, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Status spi_input_data_object(Acr_File *afp, Acr_Group *group_list)
{
   Acr_Group next, cur;
   Acr_Element element;
   Acr_Status status;
   int data_set_type;
   char *data_set_subtype;
   int final_group;
   int itype;
   static struct {
      char *name;
      int subtype_group;
   } subtype_list[] = {
      SPI_SCANDATA_OBJECT, SPI_SCANDATA_GID,
      SPI_SPECTRADATA_OBJECT, SPI_SPECTRADATA_GID,
      SPI_RAWDATA_OBJECT, SPI_RAWDATA_GID,
      SPI_LABELDATA_OBJECT, SPI_LABELDATA_GID};
   static int num_subtypes = sizeof(subtype_list) / sizeof(subtype_list[0]);

   /* Input the object up to the identifying group */
   status = acr_input_group_list(afp, group_list, ACR_IDENTIFYING_GID);
   if (status != ACR_OK) return status;

   /* Locate the last group in the list */
   cur = *group_list;
   if (cur == NULL) return ACR_HIGH_LEVEL_ERROR;
   while (acr_get_group_next(cur) != NULL) 
      cur = acr_get_group_next(cur);

   /* Look for data set type */
   element = acr_find_group_element(*group_list, ACR_Data_set_type);
   if (element == NULL) return ACR_HIGH_LEVEL_ERROR;
   data_set_type = acr_get_element_short(element);

   /* If we have an image, then read up to image */
   if (data_set_type == ACR_IMAGE_OBJECT) {

      /* Read up to image group */
      status = acr_input_group_list(afp, &next, ACR_IMAGE_GID);
      acr_set_group_next(cur, next);
      while (acr_get_group_next(cur) != NULL)
         cur = acr_get_group_next(cur);
      if (status != ACR_OK) return status;

      /* Get image actual group id */
      element = acr_find_group_element(cur, ACR_Image_location);
      if (element == NULL) return ACR_HIGH_LEVEL_ERROR;
      final_group = acr_get_element_short(element);

   }

   /* If this is not an image object, then find out type of object and 
      read up to last group */
   else {

      /* Look for data set sub-type */
      element = acr_find_group_element(*group_list, ACR_Data_set_subtype);
      if (element == NULL) return ACR_HIGH_LEVEL_ERROR;
      data_set_subtype = acr_get_element_string(element);

      /* Which data set is it? */
      for (itype=0; itype < num_subtypes; itype++) {
         if (strncmp(data_set_subtype, subtype_list[itype].name, 
                     strlen(subtype_list[itype].name)) == 0) {
            final_group = subtype_list[itype].subtype_group;
            break;
         }
      }

   }

   /* Read up to the final group */
   status = acr_input_group_list(afp, &next, final_group);
   acr_set_group_next(cur, next);
   while (acr_get_group_next(cur) != NULL)
      cur = acr_get_group_next(cur);

   /* Check that we got the last group */
   if (status != ACR_OK) return status;
   if (acr_get_group_group(cur) != final_group) 
      return ACR_HIGH_LEVEL_ERROR;

   /* Read end of message sequence */
   status = input_end_of_message(afp);

   return status;
}
