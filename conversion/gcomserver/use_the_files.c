/* ----------------------------- MNI Header -----------------------------------
@NAME       : use_the_files.c
@DESCRIPTION: Code to do something with the files copied through GYROCOM.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 23, 1993 (Peter Neelin)
@MODIFIED   : $Log: use_the_files.c,v $
@MODIFIED   : Revision 1.6  1994-01-14 11:37:37  neelin
@MODIFIED   : Fixed handling of multiple reconstructions and image types. Add spiinfo variable with extra info (including window min/max). Changed output
@MODIFIED   : file name to include reconstruction number and image type number.
@MODIFIED   :
 * Revision 1.5  94/01/11  12:37:29  neelin
 * Modified handling of output directory and user id.
 * Defaults are current dir and no chown.
 * Read from file /usr/local/lib/gcomserver.<hostname>
 * 
 * Revision 1.4  93/12/10  15:35:41  neelin
 * Improved file name generation from patient name. No buffering on stderr.
 * Added spi group list to minc header.
 * Optionally read a defaults file to get output minc directory and owner.
 * 
 * Revision 1.3  93/11/30  14:42:34  neelin
 * Copies to minc format.
 * 
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

/* Function prototypes */
int gethostname (char *name, int namelen);


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
   int cur_rec;
   int cur_imgtyp;
   int exit_status;
   char *output_file_name;
   char file_prefix[256];
   char output_default_file[256];
   char hostname[256];
   int output_uid, output_gid;
   FILE *fp;

   /* Look for defaults file */
   file_prefix[0] = '\0';
   output_uid = output_gid = INT_MIN;
   (void) gethostname(hostname, sizeof(hostname) - 1);
   (void) sprintf(output_default_file, "%s%s", 
                  OUTPUT_DEFAULT_FILE, hostname);
   if ((fp=fopen(output_default_file, "r")) != NULL) {
      (void) fscanf(fp, "%s %d %d", file_prefix, &output_uid, &output_gid);
      (void) fclose(fp);
   }

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
            cur_rec = data_info[ifile].rec_num;
            cur_imgtyp = data_info[ifile].image_type;
            used_file[ifile] = TRUE;
         }
         else if ((data_info[ifile].study_id == cur_study) &&
                  (data_info[ifile].acq_id == cur_acq) &&
                  (data_info[ifile].rec_num == cur_rec) &&
                  (data_info[ifile].image_type == cur_imgtyp)) {
            used_file[ifile] = TRUE;
         }
         if (used_file[ifile]) {
            acq_file_list[num_acq_files] = file_list[ifile];
            num_acq_files++;
         }
      }

      /* Use the files for this acquisition */

      if (found_first) {

         /* Print out the file names */
         if (Do_logging >= HIGH_LOGGING) {
            (void) fprintf(stderr, "\nFiles copied:\n");
            for (ifile=0; ifile < num_acq_files; ifile++) {
               (void) fprintf(stderr, "     %s\n", acq_file_list[ifile]);
            }
         }

         /* Create minc file */
         exit_status = gyro_to_minc(num_acq_files, acq_file_list, NULL,
                                    FALSE, file_prefix, 
                                    &output_file_name);

         /* Change the ownership */
         if (exit_status == EXIT_SUCCESS) {
            if (Do_logging >= LOW_LOGGING) 
               (void) fprintf(stderr, "Created minc file %s.\n",
                              output_file_name);
            if ((output_uid != INT_MIN) && (output_gid != INT_MIN)) {
               (void) chown(output_file_name, (uid_t) output_uid,
                            (gid_t) output_gid);
            }
         }

      }

   } while (found_first);

   /* Free acquisition file list */
   FREE(acq_file_list);
   FREE(used_file);

   return;
}
