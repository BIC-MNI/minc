/* ----------------------------- MNI Header -----------------------------------
@NAME       : use_the_files.c
@DESCRIPTION: Code to do something with the files copied through GYROCOM.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 23, 1993 (Peter Neelin)
@MODIFIED   : $Log: use_the_files.c,v $
@MODIFIED   : Revision 1.3  1993-11-30 14:42:34  neelin
@MODIFIED   : Copies to minc format.
@MODIFIED   :
 * Revision 1.2  93/11/25  13:27:23  neelin
 * Working version.
 * 
 * Revision 1.1  93/11/23  14:12:58  neelin
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

#include <sys/types.h>
#include <unistd.h>
#include <gcomserver.h>

/* ----------------------------- MNI Header -----------------------------------
@NAME       : use_the_files
@INPUT      : num_files - number of image files
              file_list - list of file names
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to do something with the files.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 23, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void use_the_files(int num_files, char *file_list[], 
                          Data_Object_Info data_info[])
{
   int ifile;
   extern int Do_logging;
   char **acq_file_list;
   int num_acq_files;
   int *used_file;
   int found_first;
   int cur_study;
   int cur_acq;
   int exit_status;
   char *output_file_name;

   /* Allocate space for acquisition file list */
   acq_file_list = MALLOC(num_files * sizeof(*acq_file_list));
   used_file = MALLOC(num_files * sizeof(*used_file));
   for (ifile=0; ifile < num_files; ifile++)
      used_file[ifile] = FALSE;

   /* Separate files into acquisitions. Loop until we don't find any 
      more acquisitions */
   do {

      /* Loop through files, looking for an acquisition */
      found_first = FALSE;
      num_acq_files = 0;
      for (ifile=0; ifile < num_files; ifile++) {
         if (used_file[ifile]) continue;
         if (!found_first) {
            found_first = TRUE;
            cur_study = data_info[ifile].study_id;
            cur_acq = data_info[ifile].acq_id;
            used_file[ifile] = TRUE;
         }
         else if ((data_info[ifile].study_id == cur_study) &&
                  (data_info[ifile].acq_id == cur_acq)) {
            used_file[ifile] = TRUE;
         }
         if (used_file[ifile]) {
            acq_file_list[num_acq_files] = file_list[ifile];
            num_acq_files++;
         }
      }

      /* Use the files for this acquisition */

      if (found_first) {

         /* Create minc file */
         exit_status = gyro_to_minc(num_acq_files, acq_file_list, NULL,
                                    FALSE, OUTPUT_MINC_DIR, 
                                    &output_file_name);
         if (exit_status != EXIT_SUCCESS) {
            (void) fprintf(stderr, "Error creating minc file %s.\n",
                           output_file_name);
         }

         /* Change the ownership */
         (void) chown(output_file_name, (uid_t) MINC_FILE_OWNER, 
                      (gid_t) MINC_FILE_GROUP);

         /* Print out the file names */
         if (Do_logging >= LOW_LOGGING) {
            (void) fprintf(stderr, "\nFiles copied:\n");
            for (ifile=0; ifile < num_acq_files; ifile++) {
               (void) fprintf(stderr, "     %s\n", acq_file_list[ifile]);
            }
         }

      }

   } while (found_first);

   /* Free acquisition file list */
   FREE(acq_file_list);
   FREE(used_file);

   return;
}
