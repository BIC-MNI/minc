/* ----------------------------- MNI Header -----------------------------------
@NAME       : dicomreader.c
@DESCRIPTION: Program to convert dicom files to minc
@GLOBALS    : 
@CREATED    : June 2001 (Rick Hoge)
@MODIFIED   : 
 * $Log: dicomreader.c,v $
 * Revision 1.3  2008-01-17 02:33:01  rotor
 *  * removed all rcsids
 *  * removed a bunch of ^L's that somehow crept in
 *  * removed old (and outdated) BUGS file
 *
 * Revision 1.2  2008/01/12 19:08:14  stever
 * Add __attribute__ ((unused)) to all rcsid variables.
 *
 * Revision 1.1.1.1  2003/08/15 19:52:55  leili
 * Leili's dicom server for sonata
 *
 * Revision 1.3  2002/03/22 00:38:08  rhoge
 * Added progress bar, wait for children at end, updated feedback statements
 *
 * Revision 1.2  2002/03/19 13:13:56  rhoge
 * initial working mosaic support - I think time is scrambled though.
 *
 * Revision 1.1  2001/12/31 17:26:21  rhoge
 * adding file to repository- compiles without warning and converts non-mosaic
 * Numa 4 files. 
 * Will probably not work for Numa 3 files yet.
 *
---------------------------------------------------------------------------- */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include "dicomserver.h"

extern char *minc_history; /* Global for minc history */
char *pname; /* program name */
File_Type file_type = UNDEF ; /* type of input files */

/* function prototypes */
private int ima_sort_function(const void *entry1, const void *entry2);
private int dcm_sort_function(const void *entry1, const void *entry2);
private int print_file_info( int ix, Data_Object_Info *info);
public int progress(long index, int end, char *message);

#define EXTREME_LOGGING 10 /* rhoge */

/* Do we do logging? */
int Do_logging = 0;
int NoFork = 0;

/* Do we keep files or are they temporary? */
static int Keep_files = 
#ifndef KEEP_FILES
   FALSE;
#else
   TRUE;
#endif

/* Globals for handling connection timeouts */
int Connection_timeout = FALSE;
Acr_File *Alarmed_afp = NULL;

int main(int argc, char *argv[])
{
   long ifile;
   long max_group;

   Acr_Group group_list;
   int exit_status;
   char **file_list;
   char **acq_file_list;
   Data_Object_Info **file_info_list;
   Data_Object_Info **acq_file_info_list;
   int num_files, num_files_alloc;
   int num_acq_files;
   FILE *fptemp;
   char last_file_name[256]; // delete?
   char *project_name = NULL;
   int process_files, have_extra_file;
   pid_t parent_pid, child_pid;
   int statptr;

   /* added by rhoge */
   int ix;
   int UseArgDir = 1;
   char OutDir[128];

   /* Get server process id */
   parent_pid = getpid();

   /* Create minc history string */
   {
      char *string;
      string = "dicomserver";
      minc_history = time_stamp(1, &string);
   }

   /* get program name */

   pname = argv[0];

   if (argc<2) {
     usage();
   }

  /* read in all the input pars and file names */

   for (ix = 1; ix<argc; ix++) {

     if (!strncmp(argv[ix],"-help",5)) { 
       usage();
     } 
     else if (!strncmp(argv[ix],"-log",4)) { 
       ix++;
       Do_logging = atoi(argv[ix]);
     }
     else if (!strncmp(argv[ix],"-nofork",5)) { 
       NoFork = 1;
     } 
     else if (!strncmp(argv[ix],"-",1)) { 
       (void) fprintf(stderr,"ERROR:  input arg `%s' not recognized\n",
		      argv[ix]);
       exit(EXIT_FAILURE);
     }
     else { /* assume rest of args are the file names and destination */

       /* Get space for file lists */
       num_files = argc - ix - 1;
       file_list = MALLOC((size_t) num_files * sizeof(*file_list));
       file_info_list = MALLOC(num_files * sizeof(*file_info_list));

       ifile = 0;
       for ( ; ix < argc - 1; ix++) {
	 file_list[ifile] = strdup(argv[ix]);
	 ifile++;
       }
       strcpy(OutDir,argv[argc-1]); 
       (void) strcat(OutDir, "/"); /* make sure path ends with slash */
     }
   } // end of loop over input args

   /* figure out what kind of files we have -
    * supported types are:
    *
    *  SIEMENS_SPI (Siemens .ima format - Numaris 3.5)
    *  SIEMENS_DCM (Siemens DICOM - Numaris 4)
    *
    * if not all same type, return an error */

   printf("Checking file types...   ");

   for (ifile = 0; ifile < num_files; ifile++) {

     char dicm_test_string[5];

     fptemp = fopen(file_list[ifile], "r");
     if (fptemp == NULL) {
       fprintf(stderr,"Error opening file %s!\n",file_list[ifile]);
       exit(EXIT_FAILURE);
     }

     /* Numaris 4 DICOM file? if so, bytes 129-132 will 
	contain the string `DICM' */

     fseek(fptemp,128,SEEK_SET);
     fread(dicm_test_string,1,4,fptemp);
     dicm_test_string[4] = (char) '\0';

     if (!strncmp(dicm_test_string,"DICM",4)) {
       if (file_type == IMA) {
	 fprintf(stderr,"ERROR:  mixed file types\n");
	 exit(EXIT_FAILURE);
       } else {
	 file_type = N4DCM;
	 // should make check of all files optional
	 if (1) {
	   printf("assuming remaining files are N4DICOM!\n");
	   break; // break out of file checking loop
	 } else {
	   progress(ifile, num_files, "Checking file types");
	 }
       }
     } else if (strstr(file_list[ifile],".ima")!=NULL) {
       if (file_type == N4DCM) {
	 fprintf(stderr,"ERROR:  mixed file types\n");
	 exit(EXIT_FAILURE);
       } else {
	 file_type = IMA;
       }
     } else {
       fprintf(stderr,"ERROR:  File %s is unkown type\n",file_list[ifile]);
       exit(EXIT_FAILURE);
     } // end of file type check

     (void) fclose(fptemp);
   } // end of loop over files to check for mixed file types

   // now loop over all files, getting basic info on each

   for (ifile = 0; ifile < num_files; ifile++) {

     char message[20];
     sprintf(message,"Parsing %d files",num_files);
     progress(ifile, num_files, message);

     // allocate space for the current entry to file_info_list
     file_info_list[ifile] = MALLOC(sizeof(*file_info_list[ifile]));
     file_info_list[ifile]->file_index = ifile;

     if (file_type == N4DCM) {

       // read up to but not including pixel data
       max_group = ACR_ACTUAL_IMAGE_GID - 1;
       group_list = read_numa4_dicom(file_list[ifile], max_group);

     } else if (file_type == IMA) {
       group_list = siemens_to_dicom(file_list[ifile], TRUE);
       if (group_list == NULL) {
	 fprintf(stderr,"Error reading groups from file %s!\n",
		 file_list[ifile]);
	 exit(EXIT_FAILURE);
       }
     }

     // get some preliminary info from group_list
     // (which should have been `corrected' in read_xxxx_dicom
     parse_dicom_groups(group_list, file_info_list[ifile]);

     // put the file name into the info list
     file_info_list[ifile]->file_name = strdup(file_list[ifile]);

     /* print out info about file */
     print_file_info(ifile,file_info_list[ifile]);

     // Delete the group list now that we're done with it
     acr_delete_group_list(group_list);

   } // end of loop over files to get basic info

   printf("Sorting files...   ");

   if (file_type == N4DCM) {
     // sort the files based on acquisition number
     qsort(file_info_list, num_files, sizeof(file_info_list[0]),
	   dcm_sort_function);

   } else if (file_type == IMA) {
     // sort the files based on file name
     // (could also use dcm_sort_function, but would have 
     // to use ACR_Image instead of ACR_Acquisition
     qsort(file_list, num_files, sizeof(file_list[0]),
	   ima_sort_function);
   }

   printf("Done sorting files.\n");

   /* Get space for acquisition file lists */
   num_files_alloc = FILE_ALLOC_INCREMENT;
   acq_file_list = MALLOC((size_t) num_files_alloc * sizeof(*acq_file_list));
   acq_file_info_list = MALLOC(num_files_alloc * sizeof(*acq_file_info_list));

   /* Loop over files, processing by acquisition */ 

   printf("Processing files, one series at a time...\n");

   num_acq_files = 1;
   for (ifile = 0; ifile < num_files; ifile++) {

     // Wait for any children that have finished 
     while ((child_pid=wait3(&statptr, WNOHANG, NULL)) > 0) {}

     // If there are children, slow down the processing
     if (child_pid == 0) {
       (void) sleep((unsigned int) SERVER_SLEEP_TIME);
     }

     /* Set flags indicating whether we should do anything with the files
	and whether the file lists contain an extra file */
     process_files = FALSE;
     have_extra_file = FALSE;

     /* Extend acquisition file list if necessary */
     if (num_acq_files >= num_files_alloc) {
       num_files_alloc = num_acq_files + FILE_ALLOC_INCREMENT;
       acq_file_list = REALLOC(acq_file_list, 
			   num_files_alloc * sizeof(*acq_file_list));
       acq_file_info_list = 
	 REALLOC(acq_file_info_list, 
		 num_files_alloc * sizeof(*acq_file_info_list));
     }
     acq_file_list[num_acq_files-1] = NULL;
     acq_file_info_list[num_acq_files-1] = 
       MALLOC(sizeof(*acq_file_info_list[num_acq_files-1]));

     acq_file_list[num_acq_files-1] = strdup(file_info_list[ifile]->file_name);

     if (file_type == N4DCM) {
       /* read up to but not including pixel data */
       max_group = ACR_ACTUAL_IMAGE_GID - 1;
       group_list = read_numa4_dicom(acq_file_list[num_acq_files-1],max_group);
       
     } else if (file_type == IMA) {
       group_list = siemens_to_dicom(acq_file_list[num_acq_files-1], TRUE);
       if (group_list == NULL) {
	 fprintf(stderr,"Error reading groups from file %s!\n",
		 acq_file_list[num_acq_files-1]);
	 exit(EXIT_FAILURE);
       } 
     }
     parse_dicom_groups(group_list, acq_file_info_list[num_acq_files-1]);

     // put the file name into the info list
     acq_file_info_list[num_acq_files-1]->file_name = 
       strdup(acq_file_list[num_acq_files-1]);

     // print some file info (junk)
     print_file_info(num_acq_files,acq_file_info_list[num_acq_files-1]);

     // Check whether we have reached the end of a group of files
     if (num_acq_files > 1) {
       if ((acq_file_info_list[num_acq_files-1]->study_id != 
	    acq_file_info_list[0]->study_id) ||
	   (acq_file_info_list[num_acq_files-1]->acq_id != 
	    acq_file_info_list[0]->acq_id)) {

	 process_files = TRUE;
	 have_extra_file = TRUE;
       } else if (ifile == num_files-1) {
	 // we're at the last file
	 process_files = TRUE;
       }
     }
     
     // Delete the group list now that we're done with it
     acr_delete_group_list(group_list);

     // Use the files if we have a complete acquisition
     if (process_files) {

       // Check for file from next acquisition
       if (have_extra_file) num_acq_files--;

       printf("Converting data for series %d (%s:  %d files)...\n",
	      acq_file_info_list[0]->acq_id,
	      acq_file_info_list[0]->protocol_name,
	      num_acq_files);

       if (NoFork) {
	 child_pid = 0;
       } else {
	 // Fork child to process the files
	 child_pid = fork();
       }

       if (child_pid > 0) {      // Parent process
	 // printf("[Parent]: Forked process to create minc file.\n");
       }                         // Error forking
       else if (child_pid < 0) {
	 fprintf(stderr, 
		 "Error forking child to create minc file\n");
	 return;
       }
       else {                    // Child process

	 // process the files (same function as server)
	 use_the_files(project_name, num_acq_files, acq_file_list, 
		       acq_file_info_list,UseArgDir,OutDir);

	 // Print message about child finishing
	 if (NoFork) {
	   //printf("[Parent]: Minc creation process finished.\n");
	   printf("File creation complete for Series %d.\n",
		  acq_file_info_list[0]->acq_id);
	 } else {
	   //printf("[Child]: Minc creation process finished.\n");
	   printf("  File creation complete for Series %d.\n",
		  acq_file_info_list[0]->acq_id);
	 }

	 if (!NoFork) {
	   // Exit from child, if forked
	   exit(EXIT_SUCCESS);
	 }
	 
       }         // End of child process

       // ----------------------------------------------------
       // if we get here we must be the parent, who goes 
       // along happily continuing to eat files
       // ----------------------------------------------------

       // Reset the lists for a new series
       free_list(num_acq_files, acq_file_list, acq_file_info_list); 

       // check to see if the last series ended by
       // running into a new series:
       if (have_extra_file) {
	 // move most recent file info to first entry in array,
	 // in preparation for next series

	 // note that num_acq_files has already been decremented,
	 // so we do not need to subtract 1 to convert to array index
	 acq_file_list[0] = acq_file_list[num_acq_files];
	 acq_file_info_list[0] = acq_file_info_list[num_acq_files];

	 acq_file_list[num_acq_files] = NULL;
	 acq_file_info_list[num_acq_files] = NULL;
       }
       //       num_acq_files = (have_extra_file ? 1 : 0);
       num_acq_files = (have_extra_file ? 2 : 1);
     } else {  
       // we're not processing the files yet - just increment counter
       num_acq_files++;
     }
   }        // end of loop over files

   // Wait for child processes if we've been forking.
   // We sleep in the checking loop to reduce parent CPU usage
   if (!NoFork) {
     printf("  (waiting for child processes to finish...)\n");
     while ((child_pid=wait3(&statptr, WNOHANG, NULL)) >= 0) {sleep(1);}
   }
   printf("Done processing files.\n");

   /* Save name of first file in last set */
   if ((num_acq_files > 0) && (acq_file_list[0] != NULL)) {
      last_file_name[sizeof(last_file_name) - 1] = '\0';
      (void) strncpy(last_file_name,acq_file_list[0],sizeof(last_file_name)-1);
   }
   else {
      last_file_name[0] = '\0';
   }

   /* Clean up files, if needed */
   if (0) {
     if (num_acq_files > 0) {
       cleanup_files(num_acq_files, acq_file_list);
       free_list(num_acq_files, acq_file_list, acq_file_info_list);
       num_acq_files = 0;
     }
   }
   FREE(acq_file_list);
   FREE(acq_file_info_list);
   
   /* Print final message */

   exit_status = EXIT_SUCCESS;

   exit(exit_status);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : cleanup_files
@INPUT      : num_files - number of files in list
              file_list - array of file names
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Removes files.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void cleanup_files(int num_files, char *file_list[])
{
   int i;

   if (Keep_files) return;

   for (i=0; i < num_files; i++) {
      if (file_list[i] != NULL) {
         (void) remove(file_list[i]);
      }
   }

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : free_list
@INPUT      : num_files - number of files in list
              file_list - array of file names
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Frees up things pointed to in pointer arrays. Does not free
              the arrays themselves.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void free_list(int num_files, char **file_list, 
                      Data_Object_Info **file_info_list)
{
   int i;

   for (i=0; i < num_files; i++) {
      if (file_list[i] != NULL) {
         FREE(file_list[i]);
      }
      if (file_info_list[i] != NULL) {
         FREE(file_info_list[i]);
      }
   }

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ima_sort_function
@INPUT      : entry1
              entry2
@OUTPUT     : (none)
@RETURNS    : -1, 0, 1 for lt, eq, gt
@DESCRIPTION: Function to compare two ima file names
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June 2001 (Rick Hoge)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int ima_sort_function(const void *entry1, const void *entry2)
{

  char * const *value1 = entry1;
  char * const *value2 = entry2;

  int session1,series1,image1;
  int session2,series2,image2;
  
  sscanf(*value1,"%d-%d-%d.ima",&session1,&series1,&image1);
  sscanf(*value2,"%d-%d-%d.ima",&session2,&series2,&image2);
  
  if (session1 < session2) return -1;
  else if (session1 > session2) return 1;
  else if (series1 < series2) return -1;
  else if (series1 > series2) return 1;
  else if (image1 < image2) return -1;
  else if (image1 > image2) return 1;
  else return 0;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : dcm_sort_function
@INPUT      : entry1
              entry2
@OUTPUT     : (none)
@RETURNS    : -1, 0, 1 for lt, eq, gt
@DESCRIPTION: Function to compare two dcm series numbers
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June 2001 (Rick Hoge)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int dcm_sort_function(const void *entry1, const void *entry2)
{

  Data_Object_Info **file_info_list1 = (Data_Object_Info **) entry1;
  Data_Object_Info **file_info_list2 = (Data_Object_Info **) entry2;

  // make a sort-able session ID number:  date.time
  double session1 = (*file_info_list1)->study_date +
    (*file_info_list1)->study_time / 1e6;
  double session2 = (*file_info_list2)->study_date +
    (*file_info_list2)->study_time / 1e6;

  // series index
  int series1 = (*file_info_list1)->acq_id;
  int series2 = (*file_info_list2)->acq_id;

  // frame index
  int frame1 = (*file_info_list1)->dyn_scan_number;
  int frame2 = (*file_info_list2)->dyn_scan_number;

  // image index
  int image1 = (*file_info_list1)->global_image_number;
  int image2 = (*file_info_list2)->global_image_number;

  if (session1 < session2) return -1;
  else if (session1 > session2) return 1;
  else if (series1 < series2) return -1;
  else if (series1 > series2) return 1;
  else if (frame1 < frame2) return -1;
  else if (frame1 > frame2) return 1;
  else if (image1 < image2) return -1;
  else if (image1 > image2) return 1;
  else return 0;
}

private int print_file_info( int ix, Data_Object_Info *info) {

  return 0;

  // printf("SPI_Parameter_file_name = %s\n",
  //  acr_find_string(group_list, SPI_Parameter_file_name, ""));
  // printf("SPI_Order_of_slices                 = %s\n",
  //  acr_find_string(group_list, SPI_Order_of_slices, ""));
  
  printf("%4s  %18s %15s %8s %6s %8s %8s %3s %3s %3s %3s %3s %3s %4s %4s %4s %5s %16s\n",
	 "ix","file","study id","date","time","serialno","acq",
	 "nec","iec","ndy","idy","nsl","isl","acol","rcol","mrow","img#",
	 "seq");
  
  /*       ix  file stu  dat tim  sn acq nec iec ndy idy nsl isl */
  printf("%4d: %18s %.6f %8d %6d %8d %8d %3d %3d %3d %3d %3d %3d %4d %4d %4d %5d %16s\n\n",
	 ix,
	 info->file_name,
	 info->study_id,
	 info->study_date,
	 info->study_time,
	 info->scanner_serialno,
	 info->acq_id,
	 info->num_echoes,
	 info->echo_number,
	 info->num_dyn_scans,
	 info->dyn_scan_number,
	 info->num_slices_nominal,
	 info->slice_number,
	 info->acq_cols,
	 info->rec_cols,
	 info->num_mosaic_rows,
	 info->global_image_number,
	 info->sequence_name);
  
}

void usage (void) {
  fprintf(stderr,
	  "\nUsage:  dicomreader [options] file1 file2 file3 ... destdir\n");
  fprintf(stderr,"\noptions:\n");
  fprintf(stderr,"  -help           : print this informative message\n");
  fprintf(stderr,"  -log <0|1|2|3>  : set logging level (default=0)\n");
  fprintf(stderr,"  -nofork         : don't fork to create minc files\n");
  exit(EXIT_FAILURE);
}
