/* ----------------------------- MNI Header -----------------------------------
@NAME       : use_the_files.c
@DESCRIPTION: Code to do something with the files copied through GYROCOM.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 23, 1993 (Peter Neelin)
@MODIFIED   : 
 * $Log: use_the_files.c,v $
 * Revision 6.1  1999-10-29 17:52:05  neelin
 * Fixed Log keyword
 *
 * Revision 6.0  1997/09/12 13:23:50  neelin
 * Release of minc version 0.6
 *
 * Revision 5.1  1997/09/11  13:09:40  neelin
 * Added more complicated syntax for project files so that different things
 * can be done to the data. The old syntax is still supported.
 *
 * Revision 5.0  1997/08/21  13:24:50  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:01:07  neelin
 * Release of minc version 0.4
 *
 * Revision 3.0  1995/05/15  19:31:44  neelin
 * Release of minc version 0.3
 *
 * Revision 2.1  1995/02/14  18:12:26  neelin
 * Added project names and defaults files (using volume name).
 * Added process id to log file name.
 * Moved temporary files to subdirectory.
 *
 * Revision 2.0  1994/09/28  10:35:37  neelin
 * Release of minc version 0.2
 *
 * Revision 1.9  94/09/28  10:34:53  neelin
 * Pre-release
 * 
 * Revision 1.8  94/05/24  15:09:53  neelin
 * Break up multiple echoes or time frames into separate files for 2 echoes
 * or 2 frames (put in 1 file for more).
 * Changed units of repetition time, echo time, etc to seconds.
 * Save echo times in dimension variable when appropriate.
 * Changed to file names to end in _mri.mnc.
 * 
 * Revision 1.7  94/03/15  14:25:49  neelin
 * Changed image-max/min to use fp_scaled_max/min instead of ext_scale_max/min
 * Added acquisition:comments attribute
 * Changed reading of configuration file to allow execution of a command on
 * the minc file.
 * 
 * Revision 1.6  94/01/14  11:37:37  neelin
 * Fixed handling of multiple reconstructions and image types. Add spiinfo variable with extra info (including window min/max). Changed output
 * file name to include reconstruction number and image type number.
 * 
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
   int cur_echo;
   int cur_dyn_scan;
   int echoes_in_one_file;
   int dyn_scans_in_one_file;
   int exit_status;
   char *output_file_name;
   char *file_prefix;
   char *command_line;
   int output_uid, output_gid;
   char string[512];
   FILE *fp;
   Project_File_Info project_info;

   /* Look for defaults file */
   (void) read_project_file(project_name, &project_info);
   file_prefix = project_info.info.directory.file_prefix;
   output_uid = project_info.info.directory.output_uid;
   output_gid = project_info.info.directory.output_gid;
   command_line = project_info.info.directory.command_line;

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
            cur_echo = data_info[ifile].echo_number;
            cur_dyn_scan = data_info[ifile].dyn_scan_number;
            echoes_in_one_file = (data_info[ifile].num_echoes > 2);
            dyn_scans_in_one_file = (data_info[ifile].num_dyn_scans > 2);
            used_file[ifile] = TRUE;
         }
         else if ((data_info[ifile].study_id == cur_study) &&
                  (data_info[ifile].acq_id == cur_acq) &&
                  (data_info[ifile].rec_num == cur_rec) &&
                  (data_info[ifile].image_type == cur_imgtyp) &&
                  ((data_info[ifile].echo_number == cur_echo) ||
                   echoes_in_one_file) &&
                  ((data_info[ifile].dyn_scan_number == cur_dyn_scan) ||
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
         exit_status = gyro_to_minc(num_acq_files, acq_file_list, NULL, FALSE, 
                                    file_prefix, &output_file_name);

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

   return;
}
