/* ----------------------------- MNI Header -----------------------------------
@NAME       : use_the_files.c
@DESCRIPTION: Code to do something with the files copied through dicom.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 28, 1997 (Peter Neelin)
@MODIFIED   : 
 * $Log: use_the_files.c,v $
 * Revision 1.1  2003-08-15 19:52:55  leili
 * Initial revision
 *
 * Revision 1.8  2002/04/26 03:27:03  rhoge
 * fixed MrProt problem - replaced fixed lenght char array with malloc
 *
 * Revision 1.7  2002/03/22 19:19:36  rhoge
 * Numerous fixes -
 * - handle Numaris 4 Dicom patient name
 * - option to cleanup input files
 * - command option
 * - list-only option
 * - debug mode
 * - user supplied name, idstr
 * - anonymization
 *
 * Revision 1.6  2002/03/19 22:10:16  rhoge
 * removed time sorting for N4DCM mosaics - time is random for mosaics
 *
 * Revision 1.5  2001/12/31 18:27:21  rhoge
 * modifications for dicomreader processing of Numaris 4 dicom files - at
 * this point code compiles without warning, but does not deal with
 * mosaiced files.  Also will probably not work at this time for Numaris
 * 3 .ima files.  dicomserver may also not be functional...
 *
 * Revision 1.4  2001/04/19 19:09:04  rhoge
 * added macro to turn off echos/frames in one file
 *
 * Revision 1.3  2001/02/26 06:16:00  rhoge
 * modified to allow specification of destination directory on command line
 *
 * Revision 1.2  2000/12/11 17:44:49  rhoge
 * added 1 to indices in debugging statements for consistency with number
 * of elements
 *
 * Revision 1.1.1.1  2000/11/30 02:13:15  rhoge
 * imported sources to CVS repository on amoeba
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
/* Commented out by rhoge, put back in by Leili */
//int gethostname (char *name, size_t namelen); 

File_Type file_type;
char command_line[512];
char *pname;

/* ----------------------------- MNI Header -----------------------------------
@NAME       : use_the_files
@INPUT      : project_name - name to use for project file
              num_files - number of image files
              file_list - list of file names
	      UseArgDir - flag to use command line OutDir destination
	      OutDir - destination directory
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
                          Data_Object_Info *data_info[],
			  int UseArgDir,char *OutDir)
     /* last three args added by rhoge */
{
   int ifile;
   int num_acq_files;
   extern int Do_logging;
   char **acq_file_list;
   int *used_file;
   int found_first;
   double cur_study;
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
   char string[512];
   FILE *fp;
   (void) fprintf(stderr, "The pname is: %s\n", pname);
   /* The if statment commented out by Leili forcing the old-style project_file to be used since it is invoked by the server*/ 
   //if (!strncmp(pname,"dicomserver",11)) {
     // use old-style project_file if invoked by server
     (void) read_project_file(project_name, file_prefix, 
			      &output_uid, &output_gid,
			      command_line, (int) sizeof(command_line));
     //}

   if (UseArgDir) { // if an output directory name has been 
                    // provided on the command line
     //(void) fprintf(stderr, "The output directory name has been provided \n");
     strcpy(file_prefix,OutDir);
   } else if (!strncmp(project_name,"/",1)) { // AEtitle is destination
     //(void) fprintf(stderr, "The output directory is the one specified in the AEtitle \n");
     strcpy(file_prefix,project_name);
   }

   if (Do_logging > HIGH_LOGGING) { // debugging
     fprintf(stderr,
	     "project_name:  [%s]    file_prefix:  [%s]\n",
	     project_name,file_prefix);
   }

   // Allocate space for acquisition file list
   acq_file_list = MALLOC(num_files * sizeof(*acq_file_list));
   used_file = MALLOC(num_files * sizeof(*used_file));
   for (ifile=0; ifile < num_files; ifile++)
      used_file[ifile] = FALSE;

   do {

     // Loop through files, looking for an acquisition

     // file groups should already have been sorted into acquisitions
     // in calling program 

     // this code is in a `do while' loop because we may 

     found_first = FALSE;
     num_acq_files = 0;
     for (ifile=0; ifile < num_files; ifile++) {
       
       if (used_file[ifile]) continue;
       if (!found_first) {
	 
	 /* found first file:  set all current attributes like
	    study id, acq id, rec num(?), image type, echo number,
	    dyn scan number, flag for multiple echoes, flag for
	    multiple time points the flag input file as `used' */ 
	 
	 found_first = TRUE;
	 cur_study = data_info[ifile]->study_id;
	 cur_acq = data_info[ifile]->acq_id;
	 cur_rec = data_info[ifile]->rec_num;
	 cur_imgtyp = data_info[ifile]->image_type;
	 cur_echo = data_info[ifile]->echo_number;
	 cur_dyn_scan = data_info[ifile]->dyn_scan_number;

	 /* note that if there are only two echos OR only two time
	    points, we turn off dyn_scans_in_one_file on the
	    presumption that the user is more likely to treat them
	    as separate acquisitions than a scan with a time
	    dimension (turned off!!) */
	 
#define MIN_FRAMES_PER_FILE 1
	 
	 echoes_in_one_file = 
	   (data_info[ifile]->num_echoes > MIN_FRAMES_PER_FILE);
	 dyn_scans_in_one_file = 
	   (data_info[ifile]->num_dyn_scans > MIN_FRAMES_PER_FILE);
	 
	 used_file[ifile] = TRUE;
       }
       
       /* otherwise check if attributes of the new input file match those
	  of the current output context and flag input file as `used' */
       
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
	 
	 /* if input file is flagged as `used', then add its index
	    to the list of files for this acquisition (and increment
	    counter) */

	 acq_file_list[num_acq_files] = file_list[ifile];
	 num_acq_files++;
       }
     } // rhoge: end of loop over files
     
     // Use the files for this acquisition
     
     if (found_first) {
       
       // Print out the file names 
       if (Do_logging >= HIGH_LOGGING) {
	 (void) fprintf(stderr, "\nFiles copied:\n");
	 for (ifile=0; ifile < num_acq_files; ifile++) {
	   (void) fprintf(stderr, "     %s\n", acq_file_list[ifile]);
	 }
       }

       // Create minc file
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
	 printf("-Applying command '%s' to output file...  ",command_line);
	 (void) fflush(stdout);
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
	 printf("Done.\n");
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


