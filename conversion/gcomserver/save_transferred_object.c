/* ----------------------------- MNI Header -----------------------------------
@NAME       : save_transferred_object.c
@DESCRIPTION: Routine to save data object.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 24, 1993 (Peter Neelin)
@MODIFIED   : $Log: save_transferred_object.c,v $
@MODIFIED   : Revision 1.3  1993-12-10 15:35:16  neelin
@MODIFIED   : Improved file name generation from patient name. No buffering on stderr.
@MODIFIED   : Added spi group list to minc header.
@MODIFIED   : Optionally read a defaults file to get output minc directory and owner.
@MODIFIED   :
 * Revision 1.2  93/11/30  14:42:28  neelin
 * Copies to minc format.
 * 
 * Revision 1.1  93/11/25  13:27:00  neelin
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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : save_transferred_object
@INPUT      : group_list - list of acr-nema groups that make up object
              file_prefix - prefix for file names
@OUTPUT     : new_file_name - name for newly created file
              data_info - information about data object
@RETURNS    : (nothing)
@DESCRIPTION: Routine to save the object in a file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 24, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void save_transferred_object(Acr_Group group_list, char *file_prefix,
                                    char **new_file_name,
                                    Data_Object_Info *data_info)
{
   Acr_Group group;
   Acr_Element element;
   char temp_name[256];
   char patient_name[256];
   int study_id = 0;
   int acquisition_id = 0;
   char *image_id = "0";
   Acr_File *afp;
   FILE *fp;
   Acr_Status status;

   /* Get data info */
   element = acr_find_group_element(group_list, ACR_Study);
   if (element != NULL)
      data_info->study_id = (int) acr_get_element_numeric(element);
   else
      data_info->study_id = 0;
   element = acr_find_group_element(group_list, ACR_Acquisition);
   if (element != NULL)
      data_info->acq_id = (int) acr_get_element_numeric(element);
   else
      data_info->acq_id = 0;

   /* Look for patient name */
   element = acr_find_group_element(group_list, ACR_Patient_name);
   if (element != NULL) {
      string_to_filename(acr_get_element_string(element), patient_name,
                         sizeof(patient_name));
   }
   if ((element == NULL) || (strlen(patient_name) == 0))
      (void) strcpy(patient_name, "unknown");

   /* Look for study and image numbers */
   element = acr_find_group_element(group_list, ACR_Study);
   if (element != NULL)
      study_id = (int) acr_get_element_numeric(element);
   element = acr_find_group_element(group_list, ACR_Acquisition);
   if (element != NULL)
      acquisition_id = (int) acr_get_element_numeric(element);
   element = acr_find_group_element(group_list, ACR_Image);
   if (element != NULL)
      image_id = acr_get_element_string(element);

   /* Create the new file name */
   (void) sprintf(temp_name, "%s-%s_%d_%d_%s", 
                  file_prefix, patient_name, study_id, 
                  acquisition_id, image_id);

   /* Create the file and write out the data */
   fp = fopen(temp_name, "w");
   if (fp == NULL) {
      (void) fprintf(stderr, "Error opening file for write: %s\n",
                     temp_name);
   }
   else {
      /* Set up the output stream */
      afp = acr_file_initialize(fp, 0, acr_stdio_write);

      /* Loop over groups */
      group = group_list;
      status = ACR_OK;
      while ((group != NULL) && (status == ACR_OK)) {

         /* Write out the group */
         status = acr_output_group(afp, group);
         if (status != ACR_OK) {
            (void) fprintf(stderr, "Error writing file %s\n",
                           temp_name);
         }
         group = acr_get_group_next(group);

      }

      /* Close the file */
      (void) acr_file_free(afp);
      (void) fclose(fp);
   }

   /* Copy the name */
   *new_file_name = strdup(temp_name);

   return;

}
