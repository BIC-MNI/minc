/* ----------------------------- MNI Header -----------------------------------
@NAME       : siemens_dicom_to_minc.c
@DESCRIPTION: Code to convert a list of Siemens dicom files to minc 
              format.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 28, 1997 (Peter Neelin)
@MODIFIED   : 
 * $Log: siemens_dicom_to_minc.c,v $
 * Revision 6.1  1999-10-29 17:51:59  neelin
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

extern int Do_logging;

/* ----------------------------- MNI Header -----------------------------------
@NAME       : siemens_dicom_to_minc
@INPUT      : num_files - number of image files
              file_list - list of file names
              minc_file - name of output minc file (NULL means make one
                 up)
              clobber - if TRUE, then open the output with NC_CLOBBER
              file_prefix - string providing any directory or prefix 
                 for internally generated filename (if it is a directory,
                 then it must contain the last "/")
@OUTPUT     : output_file_name - returns a pointer to an internal area
                 containing the file name of the created file if minc_file
                 is NULL, or simply a pointer to minc_file. If NULL, then
                 nothing is returned.
@RETURNS    : EXIT_SUCCESS if no error, EXIT_FAILURE on error.
@DESCRIPTION: Routine to convert a list of Siemens dicom files to minc 
              format.
@METHOD     : 
@GLOBALS    : Do_logging
@CALLS      : 
@CREATED    : November 25, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int siemens_dicom_to_minc(int num_files, char *file_list[], 
                        char *minc_file, int clobber,
                        char *file_prefix, char **output_file_name)
{
   Acr_Group group_list;
   int max_group;
   File_Info *file_info;
   General_Info general_info;
   Image_Data image;
   int icvid;
   int ifile;
   Mri_Index imri;
   char *out_file_name;
   int isep;

   /* Allocate space for the file information */
   file_info = MALLOC(num_files * sizeof(*file_info));

   /* Last group needed for first pass */
   max_group = ACR_ACTUAL_IMAGE_GID - 1;

   /* Add all control characters as numeric array separators to handle 
      odd behaviour with Siemens dicom files */
   for (isep=0; isep < 31; isep++) {
      (void) acr_element_numeric_array_separator(isep);
   }

   /* Initialize some values for general info */
   general_info.initialized = FALSE;
   general_info.group_list = NULL;
   for (imri=0; imri < MRI_NDIMS; imri++) {
      general_info.indices[imri] = NULL;
      general_info.coordinates[imri] = NULL;
   }

   /* Loop through file list getting information */
   for (ifile=0; ifile < num_files; ifile++) {

      /* Read the file */
      group_list = read_siemens_dicom(file_list[ifile], max_group);

      /* Get file-specific information */
      get_file_info(group_list, &file_info[ifile], &general_info);

      /* Delete the group list */
      acr_delete_group_list(group_list);

      /* Print log message if not using file */
      if (!file_info[ifile].valid) {
         if (Do_logging >= LOW_LOGGING) {
            (void) fprintf(stderr, "Not using file %s\n",
                           file_list[ifile]);
         }

      }

   }

   /* Sort the dimensions */
   sort_dimensions(&general_info);

   /* Create the output file */
   if (general_info.initialized) {
      icvid = create_minc_file(minc_file, clobber, &general_info,
                               file_prefix, &out_file_name);
   }
   if (output_file_name != NULL)
      *output_file_name = out_file_name;

   /* Check that we found the general info and that the minc file was
      created okay */
   if ((!general_info.initialized) || (icvid == MI_ERROR)) {
      if (general_info.initialized) {
         (void) fprintf(stderr, "Error creating minc file %s.\n",
                        out_file_name);
      }
      free_info(&general_info, file_info, num_files);
      FREE(file_info);
      return EXIT_FAILURE;
   }

   /* We now read all groups */
   max_group = 0;

   /* Loop through the files again and put images into the minc file */
   for (ifile=0; ifile < num_files; ifile++) {

      /* Check that we have a valid file */
      if (!file_info[ifile].valid) {
         continue;
      }

      /* Read the file */
      group_list = read_siemens_dicom(file_list[ifile], max_group);

      /* Get image */
      get_siemens_dicom_image(group_list, &image);

      /* Save the image and any other information */
      save_minc_image(icvid, &general_info, &file_info[ifile], &image);

      /* Delete the group list */
      acr_delete_group_list(group_list);

      /* Free the image data */
      if ((image.data != NULL) && (image.free)) FREE(image.data);

   }

   /* Close the output file */
   close_minc_file(icvid);

   /* Free the general_info and file_info stuff */
   free_info(&general_info, file_info, num_files);
   FREE(file_info);

   return EXIT_SUCCESS;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_siemens_dicom
@INPUT      : filename - name of siemens dicom file to read
              max_group - maximum group number to read
@OUTPUT     : (none)
@RETURNS    : group list read in from file
@DESCRIPTION: Routine to read in a group list from a file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 25, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Group read_siemens_dicom(char *filename, int max_group)
{
   FILE *fp;
   Acr_File *afp;
   Acr_Group group_list;

   /* Open the file */
   fp = fopen(filename, "r");
   if (fp == NULL) return NULL;

   /* Connect to input stream */
   afp=acr_file_initialize(fp, 0, acr_stdio_read);

   /* Read in group list */
   (void) acr_input_group_list(afp, &group_list, max_group);

   /* Close the file */
   acr_file_free(afp);
   (void) fclose(fp);

   return group_list;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : free_info
@INPUT      : general_info
              file_info
              num_files
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to free contents of general and file info structures.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 26, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
/* ARGSUSED */
public void free_info(General_Info *general_info, File_Info *file_info, 
                      int num_files)
{
   Mri_Index imri;

   /* Free the general info pointers */
   for (imri=0; imri < MRI_NDIMS; imri++) {
      if (general_info->indices[imri] != NULL) {
         FREE(general_info->indices[imri]);
      }
      if (general_info->coordinates[imri] != NULL) {
         FREE(general_info->coordinates[imri]);
      }
   }

   /* Free the group list */
   if (general_info->group_list != NULL) {
      acr_delete_group_list(general_info->group_list);
   }

   return;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : search_list
@INPUT      : value
              list
              list_length
              starting_point - point from which search should start
@OUTPUT     : (none)
@RETURNS    : Index in list where value is found, or -1 is value not found.
@DESCRIPTION: Routine to search a list for a value, returning the index
              into the list. If the value is not found, then -1 is returned.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 28, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int search_list(int value, int list[], int list_length, 
                       int starting_point)
{
   int index;

   /* Check list length and starting point */
   if (list_length <= 0) return -1;
   if ((starting_point >= list_length) || (starting_point < 0)) {
      starting_point = 0;
   }

   /* Loop over indices, wrapping at the end of the list */
   index = starting_point;
   do {
      if (list[index] == value) return index;
      index++;
      if (index >= list_length) index = 0;
   } while (index != starting_point);

   /* If we get to here, we didn't find the value */
   return -1;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : sort_dimensions
@INPUT      : general_info
@OUTPUT     : general_info
@RETURNS    : (nothing)
@DESCRIPTION: Routine to sort the MRI dimensions according to their 
              coordinates. It also fills in the step and start values for 
              the SLICE dimension.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 28, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void sort_dimensions(General_Info *general_info)
{
   Mri_Index imri;
   Sort_Element *sort_array;
   int nvalues, ival, jval;
   int reverse_array;

   /* Sort the dimensions, if needed */
   for (imri = 0; imri < MRI_NDIMS; imri++) {
      if (general_info->size[imri] > 1) {

         /* Set up the array for sorting */
         nvalues = general_info->size[imri];
         sort_array = MALLOC(nvalues * sizeof(*sort_array));
         for (ival=0; ival < nvalues; ival++) {
            sort_array[ival].identifier = general_info->indices[imri][ival];
            sort_array[ival].original_index = ival;
            sort_array[ival].value = general_info->coordinates[imri][ival];
         }

         /* Sort the array */
         qsort((void *) sort_array, (size_t) nvalues, sizeof(*sort_array), 
               dimension_sort_function);

         /* Figure out if we should reverse the array to keep something 
            similar to the original ordering */
         reverse_array = (sort_array[0].original_index > 
                          sort_array[nvalues-1].original_index);

         /* Copy the information back into the appropriate arrays */
         for (ival=0; ival < nvalues; ival++) {
            jval = (reverse_array ? nvalues - ival - 1 : ival);
            general_info->indices[imri][ival] = sort_array[jval].identifier;
            general_info->coordinates[imri][ival] = sort_array[jval].value;
         }

         /* Free the array */
         FREE(sort_array);

         /* Update slice step and start */
         if (imri == SLICE) {
            if (general_info->coordinates[imri][0] != 
                general_info->coordinates[imri][nvalues-1]) {
               general_info->step[general_info->slice_world] = 
                  (general_info->coordinates[imri][nvalues-1] -
                   general_info->coordinates[imri][0]) / 
                      ((double) general_info->size[imri] - 1.0);
            }
            general_info->start[general_info->slice_world] =
               general_info->coordinates[imri][0];
         }

      }       /* If size > 1 */
   }          /* Loop over dimensions */

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : dimension_sort_function
@INPUT      : v1, v2 - values to compare
@OUTPUT     : (none)
@RETURNS    : -1, 0 or 1 if v1 < v2, v1 == v2 or v1 > v2
@DESCRIPTION: Function to compare to array elements for sorting. Elements are
              compared first on value, then on their original array index
              (this tries to preserve the original sequence).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 28, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int dimension_sort_function(const void *v1, const void *v2)
{
   Sort_Element *value1, *value2;

   value1 = (Sort_Element *) v1;
   value2 = (Sort_Element *) v2;

   if (value1->value < value2->value)
      return -1;
   else if (value1->value > value2->value)
      return 1;
   else if (value1->original_index < value2->original_index)
      return -1;
   else if (value1->original_index > value2->original_index)
      return 1;
   else
      return 0;
}

