/* ----------------------------- MNI Header -----------------------------------
@NAME       : save_transferred_object.c
@DESCRIPTION: Routine to save data object.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 24, 1993 (Peter Neelin)
@MODIFIED   : $Log: save_transferred_object.c,v $
@MODIFIED   : Revision 1.1  1993-11-25 13:27:00  neelin
@MODIFIED   : Initial revision
@MODIFIED   :
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
@RETURNS    : (nothing)
@DESCRIPTION: Routine to save the object in a file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 24, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void save_transferred_object(Acr_Group group_list, char *file_prefix,
                                    char **new_file_name)
{
   Acr_Group group;
   Acr_Element element;
   char temp_name[256];
   char patient_name[256];
   char *study_id = "0";
   char *acquisition_id = "0";
   char *image_id = "0";
   Acr_File *afp;
   FILE *fp;
   Acr_Status status;

   /* Look for patient name */
   element = acr_find_group_element(group_list, ACR_Patient_name);
   if (element != NULL) {
      (void) sscanf(acr_get_element_string(element), "%s", patient_name);
   }
   if ((element == NULL) || (strlen(patient_name) == 0))
      (void) strcpy(patient_name, "unknown");

   /* Look for study and image numbers */
   element = acr_find_group_element(group_list, ACR_Study);
   if (element != NULL)
      study_id = acr_get_element_string(element);
   element = acr_find_group_element(group_list, ACR_Acquisition);
   if (element != NULL)
      acquisition_id = acr_get_element_string(element);
   element = acr_find_group_element(group_list, ACR_Image);
   if (element != NULL)
      image_id = acr_get_element_string(element);

   /* Create the new file name */
   (void) sprintf(temp_name, "%s-%s_%s_%s_%s", 
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
