/* ----------------------------- MNI Header -----------------------------------
@NAME       : gyro_to_minc.c
@DESCRIPTION: Code to convert a list of gyroscan acr-nema files to minc 
              format.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 25, 1993 (Peter Neelin)
@MODIFIED   : $Log: gyro_to_minc.c,v $
@MODIFIED   : Revision 5.0  1997-08-21 13:24:50  neelin
@MODIFIED   : Release of minc version 0.5
@MODIFIED   :
 * Revision 4.0  1997/05/07  20:01:07  neelin
 * Release of minc version 0.4
 *
 * Revision 3.0  1995/05/15  19:31:44  neelin
 * Release of minc version 0.3
 *
 * Revision 2.3  1995/02/14  18:12:26  neelin
 * Added project names and defaults files (using volume name).
 * Added process id to log file name.
 * Moved temporary files to subdirectory.
 *
 * Revision 2.2  1995/02/08  19:31:47  neelin
 * Moved ARGSUSED statements for irix 5 lint.
 *
 * Revision 2.1  1994/10/20  13:50:12  neelin
 * Write out direction cosines to support rotated volumes.
 * Store single slices as 1-slice volumes (3D instead of 2D).
 * Changed storing of minc history (get args for gyrotominc).
 *
 * Revision 2.0  94/09/28  10:35:28  neelin
 * Release of minc version 0.2
 * 
 * Revision 1.5  94/09/28  10:35:01  neelin
 * Pre-release
 * 
 * Revision 1.4  94/01/18  13:36:11  neelin
 * Added command line options to gyrotominc and fixed error message bug in
 * gyro_to_minc.
 * 
 * Revision 1.3  94/01/14  11:37:21  neelin
 * Fixed handling of multiple reconstructions and image types. Add spiinfo variable with extra info (including window min/max). Changed output
 * file name to include reconstruction number and image type number.
 * 
 * Revision 1.2  93/12/08  09:13:27  neelin
 * Delete group list.
 * 
 * Revision 1.1  93/11/30  14:41:20  neelin
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
@NAME       : gyro_to_minc
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
@DESCRIPTION: Routine to convert a list of gyroscan acr-nema files to minc 
              format.
@METHOD     : 
@GLOBALS    : Do_logging
@CALLS      : 
@CREATED    : November 25, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int gyro_to_minc(int num_files, char *file_list[], 
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

   /* Allocate space for the file information */
   file_info = MALLOC(num_files * sizeof(*file_info));

   /* Last group needed for first pass */
   max_group = SPI_ACTUAL_IMAGE_GID - 1;

   /* Initialize some values for general info */
   general_info.initialized = FALSE;
   general_info.group_list = NULL;
   for (imri=0; imri < MRI_NDIMS; imri++) 
      general_info.position[imri] = NULL;

   /* Loop through file list getting information */
   for (ifile=0; ifile < num_files; ifile++) {

      /* Read the file */
      group_list = read_gyro(file_list[ifile], max_group);

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
      group_list = read_gyro(file_list[ifile], max_group);

      /* Get image */
      get_gyro_image(group_list, &image);

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
      if (general_info->position[imri] != NULL) {
         FREE(general_info->position[imri]);
      }
   }

   /* Free the group list */
   if (general_info->group_list != NULL) {
      acr_delete_group_list(general_info->group_list);
   }

   return;

}
