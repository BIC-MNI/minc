/* ----------------------------- MNI Header -----------------------------------
@NAME       : save_transferred_object.c
@DESCRIPTION: Routine to save data object.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 28, 1997 (Peter Neelin)
@MODIFIED   : 
 * $Log: save_transferred_object.c,v $
 * Revision 6.1  1999-10-29 17:51:58  neelin
 * Fixed Log keyword
 *
 * Revision 6.0  1997/09/12 13:24:27  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:25:26  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:06:20  neelin
 * Release of minc version 0.4
 *
 * Revision 1.1  1997/03/04  20:56:47  neelin
 * Initial revision
 *
@COPYRIGHT  :
              Copyright 1997 Peter Neelin, McConnell Brain Imaging Centre, 
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

#include <dicomserver.h>

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
   int study_id, acquisition_id, image_id;
   Acr_File *afp;
   FILE *fp;
   Acr_Status status;
   Acr_VR_encoding_type vr_encoding;
   Acr_byte_order byte_order;
   static int file_counter = 0;

   /* Get the VR encoding state and byte order */
   element = acr_get_group_element_list(group_list);
   vr_encoding = acr_get_element_vr_encoding(element);
   byte_order = acr_get_element_byte_order(element);

   /* Get data info */
   get_identification_info(group_list,
                           &(data_info->study_id), &(data_info->acq_id),
                           &(data_info->rec_num), &(data_info->image_type));

   /* Get number of echos, echo number, number of dynamic scans and 
      dynamic_scan_number */
   data_info->num_echoes =
      acr_find_int(group_list, SPI_Number_of_echoes, 1);
   data_info->echo_number =
      acr_find_int(group_list, ACR_Echo_number, 1);
   data_info->num_dyn_scans =
      acr_find_int(group_list, ACR_Acquisitions_in_series, 1);
   data_info->dyn_scan_number =
      acr_find_int(group_list, ACR_Series, 1);

   /* Look for patient name */
   element = acr_find_group_element(group_list, ACR_Patient_name);
   if (element != NULL) {
      string_to_filename(acr_get_element_string(element), patient_name,
                         sizeof(patient_name));
   }
   if ((element == NULL) || (strlen(patient_name) == 0))
      (void) strcpy(patient_name, "unknown");

   /* Look for study and image numbers */
   study_id = data_info->study_id;
   acquisition_id = data_info->acq_id;
   image_id = acr_find_int(group_list, ACR_Image, 0);

   /* Create the new file name */
   (void) sprintf(temp_name, "%s-%04d-%s_%d_%d_%d.dcm", 
                  file_prefix, file_counter++, patient_name, study_id, 
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
      acr_set_vr_encoding(afp, vr_encoding);
      acr_set_byte_order(afp, byte_order);

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
      acr_file_free(afp);
      (void) fclose(fp);
   }

   /* Copy the name */
   *new_file_name = strdup(temp_name);

   return;

}
