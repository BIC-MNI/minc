/* ----------------------------- MNI Header -----------------------------------
@NAME       : use_the_files.c
@DESCRIPTION: Code to do something with the files copied through dicom.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 28, 1997 (Peter Neelin)
@MODIFIED   : 
 * $Log: use_the_files.c,v $
 * Revision 6.2  2002-12-07 13:02:28  neelin
 * Fixed prototype for gethostname
 *
 * Revision 6.1  1999/10/29 17:52:00  neelin
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
 * Revision 1.2  1997/03/11  13:10:48  neelin
 * Working version of dicomserver.
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

#include <sys/types.h>
#include <unistd.h>
#include <dicomserver.h>

/* Function prototypes */
int gethostname (char *name, size_t namelen);


/* ----------------------------- MNI Header -----------------------------------
@NAME       : use_the_files
@INPUT      : project_name - name to use for project file
              num_files - number of image files
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
public void use_the_files(char *project_name,
                          int num_files, char *file_list[], 
                          Data_Object_Info *data_info[])
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
   int cur_echo;
   int cur_dyn_scan;
   int echoes_in_one_file;
   int dyn_scans_in_one_file;
   int exit_status;
   char *output_file_name;
   char file_prefix[256];
   int output_uid, output_gid;
   char command_line[512];
   char string[512];
   FILE *fp;

   /* Look for defaults file */
   (void) read_project_file(project_name, file_prefix, 
                            &output_uid, &output_gid,
                            command_line, (int) sizeof(command_line));

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
            cur_study = data_info[ifile]->study_id;
            cur_acq = data_info[ifile]->acq_id;
            cur_rec = data_info[ifile]->rec_num;
            cur_imgtyp = data_info[ifile]->image_type;
            cur_echo = data_info[ifile]->echo_number;
            cur_dyn_scan = data_info[ifile]->dyn_scan_number;
            echoes_in_one_file = (data_info[ifile]->num_echoes > 2);
            dyn_scans_in_one_file = (data_info[ifile]->num_dyn_scans > 2);
            used_file[ifile] = TRUE;
         }
         else if ((data_info[ifile]->study_id == cur_study) &&
                  (data_info[ifile]->acq_id == cur_acq) &&
                  (data_info[ifile]->rec_num == cur_rec) &&
                  (data_info[ifile]->image_type == cur_imgtyp) &&
                  ((data_info[ifile]->echo_number == cur_echo) ||
                   echoes_in_one_file) &&
                  ((data_info[ifile]->dyn_scan_number == cur_dyn_scan) ||
                   dyn_scans_in_one_file)) {
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
         exit_status = siemens_dicom_to_minc(num_acq_files, acq_file_list, 
                                             NULL, FALSE, file_prefix, 
                                             &output_file_name);

         if (exit_status != EXIT_SUCCESS) continue;

         /* Print log message */
         if (Do_logging >= LOW_LOGGING) {
            (void) fprintf(stderr, "Created minc file %s.\n",
                           output_file_name);
         }

         /* Invoke a command on the file (if requested) and get the 
            returned file name */
         if (strlen(command_line) > (size_t) 0) {
            (void) sprintf(string, "%s %s", command_line, output_file_name);
            if ((fp=popen(string, "r")) != NULL) {
               (void) fscanf(fp, "%s", output_file_name);
               if (pclose(fp) != EXIT_SUCCESS) {
                  (void) fprintf(stderr, 
                                 "Error executing command\n   \"%s\"\n",
                                 string);
               }
               else if (Do_logging >= LOW_LOGGING) {
                  (void) fprintf(stderr, 
                     "Executed command \"%s\",\nproducing file %s.\n",
                                 string, output_file_name);
               }
            }
            else {
               (void) fprintf(stderr, "Error executing command \"%s\"\n",
                              string);
            }
         }

         /* Change the ownership */
         if ((output_uid != INT_MIN) && (output_gid != INT_MIN)) {
            (void) chown(output_file_name, (uid_t) output_uid,
                         (gid_t) output_gid);
         }

      }

   } while (found_first);

   /* Free acquisition file list */
   FREE(acq_file_list);
   FREE(used_file);

}
