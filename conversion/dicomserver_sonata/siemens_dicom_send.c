/* ----------------------------- MNI Header -----------------------------------
@NAME       : siemens_dicom_send.c
@DESCRIPTION: Program to convert siemens vision internal format files to
              dicom and send them to a remote server.
@METHOD     : 
@GLOBALS    : 
@CREATED    : July 8, 1997 (Peter Neelin) re-write by Rick Hoge
@MODIFIED   : $Log: siemens_dicom_send.c,v $
@MODIFIED   : Revision 1.3  2008-01-17 02:33:01  rotor
@MODIFIED   :  * removed all rcsids
@MODIFIED   :  * removed a bunch of ^L's that somehow crept in
@MODIFIED   :  * removed old (and outdated) BUGS file
@MODIFIED   :
@MODIFIED   : Revision 1.2  2008/01/12 19:08:14  stever
@MODIFIED   : Add __attribute__ ((unused)) to all rcsid variables.
@MODIFIED   :
@MODIFIED   : Revision 1.1.1.1  2003/08/15 19:52:55  leili
@MODIFIED   : Leili's dicom server for sonata
@MODIFIED   :
@MODIFIED   : Revision 1.9  2001/10/19 13:08:39  rhoge
@MODIFIED   : - fixed problem for single slice acq loop scans in siemens_dicom_send
@MODIFIED   : - added diffusion pulse seq in ima2mnc, restored hard-coded path
@MODIFIED   :
@MODIFIED   : Revision 1.8  2001/04/19 19:33:10  rhoge
@MODIFIED   : added multi-echo support, basic testing.  Changed TEST to MYDEBUG
@MODIFIED   :
@MODIFIED   : Revision 1.7  2000/12/17 01:04:40  rhoge
@MODIFIED   : clean up log format
@MODIFIED   :
@MODIFIED   : Revision 1.6  2000/12/17 01:02:58  rhoge
@MODIFIED   : Made some debugging statements conditional on TEST macro, 
@MODIFIED   : in course of changes to restore Sun function after 
@MODIFIED   : byte-aligmnent fixes (padding) for Linux port
@MODIFIED   :
@MODIFIED   : Revision 1.5  2000/12/14 22:56:40  rhoge
@MODIFIED   : removed DBL_MAX - not defined on sun (replaced with 1.0 in
@MODIFIED   : acr_find_double, line 502)
@MODIFIED   :
@MODIFIED   : Revision 1.4  2000/12/11 20:04:35  rhoge
@MODIFIED   : got rid of printf statements for time correction
@MODIFIED   :
@MODIFIED   : Revision 1.3  2000/12/11 19:27:25  rhoge
@MODIFIED   : fix for leading zeros if hms less than two digits in time
@MODIFIED   :
@MODIFIED   : Revision 1.2  2000/12/11 17:43:01  rhoge
@MODIFIED   : added frame time correction - code compiles and runs, but order
@MODIFIED   : problem may not yet be fixed
@MODIFIED   :
@MODIFIED   : Revision 1.1.1.1  2000/11/30 02:05:54  rhoge
@MODIFIED   : imported sources to CVS repository on amoeba
@MODIFIED   :
 *
 * Revision 1.9  1999/08/03  17:42:55  neelin
 * Added ability to handle multiple input directories.
 *
 * Revision 1.8  1998/11/17  16:13:29  neelin
 * Changes to log messages to ensure that 100 percent is only displayed
 * when transfer is complete.
 *
 * Revision 1.7  1998/11/17  14:49:22  neelin
 * Modified help comment.
 *
 * Revision 1.6  1998/11/17  14:00:12  neelin
 * Changed logging to print percentage with overstrike for terminals.
 *
 * Revision 1.5  1998/11/16  19:48:43  neelin
 * Added support for breaking up mosaic images and sending the pieces one
 * at a time.
 *
 * Revision 1.4  1998/11/13  16:02:09  neelin
 * Modifications to support packed images and asynchronous transfer.
 *
 * Revision 1.3  1998/02/20  17:37:55  neelin
 * Removed debugging statements.
 *
 * Revision 1.2  1997/11/04  14:31:30  neelin
 * *** empty log message ***
 *
 * Revision 1.1  1997/08/11  12:50:53  neelin
 * Initial revision
 *
---------------------------------------------------------------------------- */

#include <stdio.h>
#include <math.h> /* for slice position hacks */
#include <stdlib.h>
#include <limits.h>
#include <dirent.h>
#include <string.h>
#include <malloc.h>
#include <ParseArgv.h>

#include <acr_nema.h>
#include <dicom_client_routines.h>

/* Constants */
#ifndef public
#  define public
#endif
#ifndef private
#  define private static
#endif
#ifndef EXIT_SUCCESS
#  define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
#  define EXIT_FAILURE 1
#endif
#define ALLOC_INCREMENT 100
#define LOG_PERCENT (2.0)

/* Dicom definitions */
#define ACR_MR_IMAGE_STORAGE_UID       "1.2.840.10008.5.1.4.1.1.4"
#define ACR_IMPLICIT_VR_LITTLE_END_UID "1.2.840.10008.1.2"
#define ACR_EXPLICIT_VR_LITTLE_END_UID "1.2.840.10008.1.2.1"
#define ACR_EXPLICIT_VR_BIG_END_UID    "1.2.840.10008.1.2.2"
#define MY_AE_TITLE "MGH_CLIENT"

/* rhoge: debugging definitions */
/* #define MYDEBUG */

/* Define element id's */

DEFINE_ELEMENT(static, ACR_Slice_thickness            , 0x0018, 0x0050, DS);
DEFINE_ELEMENT(static, ACR_Rows                       , 0x0028, 0x0010, US);
DEFINE_ELEMENT(static, ACR_Columns                    , 0x0028, 0x0011, US);
DEFINE_ELEMENT(static, ACR_Pixel_size                 , 0x0028, 0x0030, DS);
DEFINE_ELEMENT(static, ACR_Bits_allocated             , 0x0028, 0x0100, US);
DEFINE_ELEMENT(static, ACR_Image                      , 0x7fe0, 0x0010, OW);

DEFINE_ELEMENT(static, SPI_Sequence_File_Name         , 0x0019, 0x1511, LT);
DEFINE_ELEMENT(static, SPI_Image_position             , 0x0021, 0x1160, DS);
DEFINE_ELEMENT(static, SPI_Image_normal               , 0x0021, 0x1161, DS);
DEFINE_ELEMENT(static, SPI_Image_distance             , 0x0021, 0x1163, DS);
DEFINE_ELEMENT(static, SPI_Current_slice_number       , 0x0021, 0x1342, IS);
DEFINE_ELEMENT(static, SPI_Slice_distance_factor      , 0x0021, 0x1344, DS);

/* added by rhoge for acquisition loop with mosaic overflow
 * note:  on the Sonata, {acquisition,run,series,scan} is called 
 * Study in the header! beware of confusion... */

DEFINE_ELEMENT(static, ACR_Study                      , 0x0020, 0x0010, IS);
DEFINE_ELEMENT(static, ACR_Series                     , 0x0020, 0x0011, IS);
DEFINE_ELEMENT(static, ACR_Acquisitions_in_series     , 0x0020, 0x1001, IS);
DEFINE_ELEMENT(static, ACR_Echo_number                , 0x0018, 0x0086, IS);

DEFINE_ELEMENT(static, SPI_Number_of_averages         , 0x0018, 0x0083, DS);
DEFINE_ELEMENT(static, SPI_Number_of_slices_nom       , 0x0021, 0x1340, IS);
DEFINE_ELEMENT(static, SPI_Number_of_slices_cur       , 0x0021, 0x1341, IS);
DEFINE_ELEMENT(static, SPI_Number_of_fourier_lines_nominal,0x0019, 0x1220, IS);
DEFINE_ELEMENT(static, SPI_Number_of_echoes           , 0x0021, 0x1370, IS);

DEFINE_ELEMENT(static, ACR_Study_time            , 0x0008, 0x0030, TM);
DEFINE_ELEMENT(static, ACR_Repetition_time       , 0x0018, 0x0080, DS);
DEFINE_ELEMENT(static, ACR_Acquisition_time      , 0x0008, 0x0032, TM);

/* Typedefs */
typedef struct {
   int key;
   char *name;
   int dirindex;
} Sort_entry;

typedef struct {
   int packed;
   int mosaic_seq;
   int size[2];
   int big[2];
   int grid[2];
   int pixel_size;
   Acr_Element big_image;
   Acr_Element small_image;
   int sub_images;
   int first_image;
   double normal[3];
   double step[3];
   double position[3];
} Multi_Image;

typedef struct Mosaic_Info {
   char *match_string;
   int size[2];
   struct Mosaic_Info *next;
} *Mosaic_Info;

/* Function prototypes */
public Acr_Group siemens_to_dicom(char *filename, int read_image);
public void update_coordinate_info(Acr_Group group_list);
public int process_mosaic_args(char *dst, char *key, int argc, char **argv);
public Mosaic_Info get_mosaic_info(Mosaic_Info mosaic_info_list, 
                                   char *protocol);
public int get_directory(char *dst, char *key, char *nextarg);
public void get_file_names(int num_dirs, char **directory_list, 
                           int patient_number,
                           int first_image, int last_image,
                           int *num_files, char ***file_list,
                           int **file_dirindex_list);
private int sort_function(const void *entry1, const void *entry2);
public void free_file_names(char *fullpath, char *file_list[], int num_files);
public int multi_image_init(Acr_Group group_list, 
                            Multi_Image *multi_image);
public void multi_image_modify_group_list(Acr_Group group_list, 
                                          Multi_Image *multi_image,
                                          int iimage);
public void multi_image_cleanup(Acr_Group group_list, 
                                Multi_Image *multi_image);

/* Variables for argument parsing */
Mosaic_Info mosaic_info_list = NULL;
int max_outstanding = 10;
char **directory_list = NULL;
int num_dirs = 0;
int num_dirs_alloc = 0;

/* Argument table */
ArgvInfo argTable[] = {
   {"-mosaic", ARGV_GENFUNC, (char *) process_mosaic_args, 
       (char *) &mosaic_info_list,
       "Add a mosaic protocol and subimage size to the list.\n"
"\tUsage: -mosaic <protocol string> <m> <n>\n"
"\tIf the protocol name begins with a /, then an exact match is done.\n"
"\tIf it contains an extension, then a match on filename alone is done.\n"
"\tOtherwise, a prefix match is done.\n"
"\tParent directories can be specified for a prefix match.\n"
"\tExamples:\n"
"\t\t-mosaic /home/user/protcols/mosaic/myproto64_version1.ekc 64 64\n"
"\t\t-mosaic myproto64 64 64\n"
"\t\t-mosaic myproto64_version1.ekc 64 64\n"
"\t\t-mosaic mosaic/myproto64_ 64 64\n"
"\t\t-mosaic mosaic/myproto64_version1.ekc 64 64\n"
"\t\t-mosaic mosaic/ 64 64\n"
 },
   {"-directory", ARGV_FUNC, (char *) get_directory, (char *) NULL,
       "Specify a directory to search (instead of positional option)."},
   {"-max_outstanding", ARGV_INT, (char *) 1, (char *) &max_outstanding,
       "Specify max outstanding replies for asynchronous transfer."},
   {(char *) NULL, ARGV_END, (char *) NULL, (char *) NULL,
       (char *) NULL}
};

/* Main program */

int main(int argc, char *argv[])
{
   Acr_Group group_list;
   char *pname;
   char *host;
   char *port;
   char *AEtitle;
   int  iarg;
   int  patient_number;
   int  first_image;
   int  last_image;
   char **file_list;
   int *file_dirindex_list;
   int ifile, num_files, idir;
   Acr_File *afpin, *afpout;
   char *fullpath;
   int maxlength, maxdirlength, len;
   int iimage;
   Multi_Image multi_image;
   char *log_separator;
   int needed_args;

   /* stuff added by rhoge for acquisition loop with mosaic overflow */

   int idim;
   int imosaic;
   int iecho;
   int num_mosaics;
   int acqframe;
   int num_acqframes;
   int num_echos;
   int echo;
   int num_slices;
   int num_sub_images;
   int nimages;
   int num_special_files;
   int num_mosaic_elements;
   double top_slice_position[3];
   int current_series;
   int current_slice_index;
   double scan_start_time;
   double start_time_sec;
   double start_hours;
   double start_minutes;
   double start_seconds;
   double frame_hours;
   double frame_minutes;
   double frame_seconds;
   double frame_hours_oflow;
   double frame_minutes_oflow;
   double frame_seconds_oflow;
   double rel_frame_time_sec;
   double abs_frame_time_sec;
   char   abs_frame_time_hms[100];
   

   /* Check arguments. */
   /* Ugly hack here to keep backwards compatibility, but also allow -directory
      specification. needed_args stores the minimum number of args needed,
      depending on whether the -directory option was specified. */
   pname = argv[0];
   if (ParseArgv(&argc, argv, argTable, 0) || 
       (argc < (needed_args = (directory_list == NULL ? 6:5))) || 
       (argc > needed_args+2)) {
      (void) fprintf(stderr, 
         "Usage: %s host port AEtitle [directory] patient_num [first [last]]\n", 
                     pname);
      return EXIT_FAILURE;
   }
   iarg=1;
   host = argv[iarg++];
   port = argv[iarg++];
   AEtitle = argv[iarg++];
   if (directory_list == NULL) {
      num_dirs_alloc = 1;
      directory_list = malloc(sizeof(*directory_list) * num_dirs_alloc);
      directory_list[num_dirs] = argv[iarg++];
      num_dirs++;
   }
   patient_number = atoi(argv[iarg++]);
   first_image = ((argc < needed_args+1) ? 0 : atoi(argv[iarg++]));
   last_image = ((argc < needed_args+2) ? INT_MAX : atoi(argv[iarg++]));

   /* Check range */
   if ((last_image < first_image) || (first_image < 0)) {
      (void) fprintf(stderr, 
         "Bad range of images: use +ve values with last >= first\n");
      return EXIT_FAILURE;
   }

   /* Get file names */
   get_file_names(num_dirs, directory_list, 
                  patient_number, first_image, last_image, 
                  &num_files, &file_list, &file_dirindex_list);
   if (num_files <= 0) {
      (void) fprintf(stderr, "No files to send.\n");
      return EXIT_FAILURE;
   }

   
   /* Get space for file names */
   maxlength = 0;
   for (ifile = 0; ifile < num_files; ifile++) { 
      len = strlen(file_list[ifile]);
      if (len > maxlength) maxlength = len;
   }
   maxdirlength = 0;
   for (idir = 0; idir < num_dirs; idir++) {
      len = strlen(directory_list[idir]);
      if (len > maxdirlength) maxdirlength = len;
   }
   fullpath = malloc(maxdirlength + maxlength + 2);

   /* Make dicom connection */
   if (!acr_open_dicom_connection(host, port, AEtitle, MY_AE_TITLE,
                                  ACR_MR_IMAGE_STORAGE_UID,
				  ACR_IMPLICIT_VR_LITTLE_END_UID,
                                  &afpin, &afpout)) {
      free_file_names(fullpath, file_list, num_files);
      return EXIT_FAILURE;
   }

   /* Set maximum number of outstanding responses */
   acr_set_client_max_outstanding(afpin, max_outstanding);

   /* Loop over the input files, sending them one at a time */

   ifile = 0;

   while (ifile < num_files) {

     /* get next dicom group */

     /************** START of group read unit - function? ********/

     /* Get file name */
     idir = file_dirindex_list[ifile];
     (void) sprintf(fullpath, "%s/%s", 
		    directory_list[idir], file_list[ifile]);
     
     printf("processing file %s (%d of %d)\n",
	    file_list[ifile],
	    ifile+1,num_files);
     (void) fflush(stdout);

     /* Read data */
     group_list = siemens_to_dicom(fullpath, TRUE);
     if (group_list == NULL) continue;

     /* Look for multi-image files (many images in one) */
     num_mosaic_elements = multi_image_init(group_list, &multi_image);

#ifdef MYDEBUG
     printf("num_mosaic_elements = %d\n",num_mosaic_elements); 
#endif

     /************** END of group read unit - function? ********/

     /* determine number of `acqframes' */

     if (multi_image.mosaic_seq) {

       num_acqframes = acr_find_int(group_list,
				    SPI_Number_of_averages, 1);
     } else {
       num_acqframes = 1;
     }

#ifdef MYDEBUG
     printf("num_acqframes = %d\n",num_acqframes); 
#endif

     /* determine real number of slices */

     num_slices = acr_find_int(group_list, SPI_Number_of_slices_cur, 1);

     num_echos = acr_find_int(group_list, SPI_Number_of_echoes, 1);

#ifdef MYDEBUG
     printf("num_echos = %d\n",num_echos); 
#endif

     /* determine number of mosaics per volume */

     num_mosaics = (int)ceil((float)num_slices/(float)num_mosaic_elements);

     /* determine total number of `special' files expected for this
        series */

     num_special_files = num_acqframes * num_mosaics * num_echos;

#ifdef MYDEBUG
     printf("num_mosaics = %d\n",num_mosaics);
     printf("num_special_files = %d\n",num_special_files);
#endif

     current_series =
       acr_find_int(group_list, ACR_Study, 0);

#ifdef MYDEBUG
     printf("current_series = %d\n",current_series);
#endif

     /* add loop for echos, since diffusion scans may use echo loop
	for multiple encodings.  Loop order will usually be 
	acq, mosaic/slice, echo */

     for (iecho = 0;  iecho < num_echos; iecho++) {

       for (imosaic = 0;  imosaic < num_mosaics; imosaic++) {

	 current_slice_index = 
	   acr_find_int(group_list, SPI_Current_slice_number, 1);
	 
#ifdef MYDEBUG
     printf("current_slice_index = %d\n",current_slice_index);
#endif

	 for (acqframe = 0; acqframe < num_acqframes; acqframe++) {
	   
	   /* check if we hit a new series or slice unexpectedly early -
	      this indicates an aborted run */
	   
	   if ( acr_find_int(group_list,ACR_Study,0) !=
		current_series ) {
	     
	     /* if we hit a new SERIES too early, that's it.  We break
		out of all loops (acqframe and imosaic), clean up the
		group, and start over again at the top of
		while(ifile<num_files).  Note that we *don't* decrement
		ifile as the group will be re-read at the top of the
		while loop with the correct file index as-is.  */
	     
	     printf("WARNING:  new series too early; assuming aborted run for series %d.\n",current_series);
	     
	     acqframe = num_acqframes;
	     imosaic = num_mosaics;
	     multi_image_cleanup(group_list, &multi_image);
	     acr_delete_group_list(group_list);
	     break;
	     
	   } else if ( acr_find_int(group_list, SPI_Current_slice_number, 1) !=
		       current_slice_index ) {
	     
	     /* if we hit a new SLICE too early, we've passed the last
		frame in an overflow situation.  We then bump up the
		mosaic index, reset the frame counter (we're now at the
		first frame of the next overflow mosaic), and update the
		current slice index */
	     
	     printf("WARNING:  new mosaic too early; assuming aborted run for series %d.\n",current_series);
	     
	     imosaic++;
	     acqframe = 0;
	     current_slice_index = 
	       acr_find_int(group_list, SPI_Current_slice_number, 1);
	     
	   }
	   
	   /* debug statements for diffusion/echo loop scans? */
#ifdef MYDEBUG
	   printf("slice = %d\n",
		  acr_find_int(group_list, SPI_Current_slice_number, 1));
	   
	   printf("echo = %d\n",
		  acr_find_int(group_list, ACR_Echo_number, 1));
#endif
	   
	   /******* process .ima file ********/
	   
	   /******** START of acquisition loop correction section *******/
	   
	   /* note:  we _could_ correct the number of signal averages,
	      but this may be useful on the receive side so we'll leave
	      it equal to the number of dynamic scans */
	   
	   /* also, we only want to do this if looks like there really
	      are multiple acqframes (the acquisition loop has been used
	      for dynamic scanning).  Otherwise, we could clobber  valid
	      dynamic scan info for a measurement loop scan */
	   
	   if (num_acqframes>1) {
	     
	     /* Number of dynamic scans */
	     acr_insert_numeric(&group_list, ACR_Acquisitions_in_series,
				num_acqframes);
	     
	     /* now we have to fix the frame times, since these may be used
		in sorting the frames at the receive end */
	     
	     /* start of scanning run */
	     scan_start_time = 
	       atof(acr_find_string(group_list, ACR_Study_time, ""));
	     
	     /* extract hours, minutes, seconds */
	     start_hours = (int) (scan_start_time/(double)10000);
	     scan_start_time -= start_hours * (10000.0);
	     start_minutes = (int) (scan_start_time/(double)100);
	     scan_start_time -= start_minutes * (100.0);
	     start_seconds = scan_start_time;
	     
	     /* convert start time to seconds from start of day */
	     start_time_sec = 
	       start_hours*3600.0 +
	       start_minutes*60.0 +
	       start_seconds;
	     
	     /* compute frame time (from start) based on TR */
	     rel_frame_time_sec = acqframe * 
	       (acr_find_double(group_list, ACR_Repetition_time,1.0)/1000);
	     
	     /* now add times in seconds to get absolute frame time */
	     abs_frame_time_sec = rel_frame_time_sec + start_time_sec;
	     
	     /* convert absolute frame time in sec back to hms */
	     frame_hours     = floor(abs_frame_time_sec/3600.0);
	     abs_frame_time_sec  -= frame_hours * 3600.0;
	     frame_minutes   = floor(abs_frame_time_sec/60.0);
	     abs_frame_time_sec  -= frame_minutes * 60.0;
	     frame_seconds   = abs_frame_time_sec;
	     
	     /* check for day overflow (hours>23)*/
	     frame_hours = ((int)frame_hours)%24;
	     
	     /* write frame time to string */
	     sprintf(abs_frame_time_hms,"%02.0f%02.0f%06.3f",
		     frame_hours,
		     frame_minutes,
		     frame_seconds);
	     
	     /* now write the proper frame time into the group */
	     
	     acr_insert_string(&group_list, ACR_Acquisition_time, 
			       abs_frame_time_hms);
	     
	     /* current frame index */
	     acr_insert_numeric(&group_list, ACR_Series,acqframe);
	     
	   }
	   
	   /******** END of acquisition loop correction section *******/
	   
	   /* determine number of sub-images in mosaic */
	   
	   if (multi_image.mosaic_seq) {
	     
	     if (imosaic == num_mosaics-1) {
	       
	       num_sub_images = num_slices-(num_mosaics-1)*num_mosaic_elements;
	       
	     } else {
	       
	       num_sub_images = num_mosaic_elements;
	       
	     }

#ifdef MYDEBUG
     printf("num_sub_images = %d\n",num_sub_images); 
#endif
	     
	   } else { /* non-mosaic */
	     
	     num_sub_images = 1;
	     
	   }
	   
	   /********* Start of slice position correction Section ********/
	   
	   if (multi_image.mosaic_seq && num_sub_images > 1) {
	     
	     /* before 7:00pm Tuesday, March 28 2000: 
		
		position of first mosaic was ok, but overflow was either
		off by number of overflow slices (for interleaved
		excitation order) or correct (for ascending) */
	     
	     /* after 7:00pm Tuesday, March 28 2000: 
		
		position of .ima file is that of LAST slice in mosaic
		this must be corrected to give FIRST slice.
		
		This was originally done so that SPI_Current_slice_number
		would give the number of slices in the mosaic.
		Unfortunately, this only works for ascending order on the
		new system.  The SPL could therefore be modified to just
		assign the position of the first sub-mosaic to the .ima
		file.  This would probably be more consistent with other
		sites */
	     
	     /* note that the code below depends *only* on the slice
		position of the first mosaic being equal to that of the
		last slice, so it should work for either case (it is
		the slice position of subsequent `overflow' mosaics that
		has been variable) */
	     
	     if (imosaic==0) {
	       
	       for (idim=0; idim < 3; idim++) {
		 multi_image.position[idim] -= 
		   (double) (num_sub_images-1) * multi_image.step[idim];
		 
		 top_slice_position[idim]=multi_image.position[idim];
	       }
	       
	     } else {
	       
	       for (idim=0; idim < 3; idim++) {
		 multi_image.position[idim] = 
		   top_slice_position[idim] +
		   (double)imosaic*(double)num_mosaic_elements*
		   multi_image.step[idim];
	       }
	     }
	   } /* end of if (multi_image.mosaic_seq) */
	   
	   /********* End of slice position correction Section ********/
	   
	   /* Now send all the sub-images */
	   for (iimage=0; iimage < num_sub_images; iimage++) {
	     
	     
	     /* Modify the group list for this image */
	     multi_image_modify_group_list(group_list, &multi_image, iimage);
	     
	     /* correct slice index if mosaic */
	     if (multi_image.mosaic_seq) {
	       
	       acr_insert_numeric(&group_list, SPI_Current_slice_number,
				  imosaic*num_mosaic_elements+iimage+1);
	       
	     }
	     
	     /* Send the image */
	     if (!acr_send_group_list(afpin, afpout, group_list, 
				      ACR_MR_IMAGE_STORAGE_UID)) {
	       multi_image_cleanup(group_list, &multi_image);
	       acr_delete_group_list(group_list);
	       acr_close_dicom_connection(afpin, afpout);
	       free_file_names(fullpath, file_list, num_files);
	       return EXIT_FAILURE;
	     }
	     
	   }
	   
	   /* Clean up */
	   
	   multi_image_cleanup(group_list, &multi_image);
	   
	   acr_delete_group_list(group_list);
	   
	   ifile++;
	   
	   if (ifile == num_files) {
	     
	     /* we've hit the last file - let's get out of here!  (this
		might happen if the last run in the file list is
		aborted) */
	     
	     acqframe = num_acqframes;
	     imosaic = num_mosaics;
	     break;
	     
	     
	   } else if (num_special_files > 1) {	 
	     
	     /* get next dicom group */
	     
	     /************** START of group read unit - function? ********/
	     
	     /* Get file name */
	     idir = file_dirindex_list[ifile];
	     (void) sprintf(fullpath, "%s/%s", 
			    directory_list[idir], file_list[ifile]);
	     
	     printf("processing file %s (%d of %d)\n",
		    file_list[ifile],
		    ifile+1,num_files);
	     (void) fflush(stdout);
	     
	     /* Read data */
	     group_list = siemens_to_dicom(fullpath, TRUE);
	     if (group_list == NULL) continue;
	     
	     /* Look for multi-image files (many images in one) */
	     num_mosaic_elements = multi_image_init(group_list, &multi_image);
	     
	     /************** END of group read unit - function? ********/
	     
	   } /* end of if (num_special_files > 1) */
	   
	 } /* end of loop over acqframes */
	 
       } /* end of loop over mosaics */
       
     } /* end of loop over echos */
     
   } /* end of loop over files */


   (void) printf("Done sending files.\n");
   (void) fflush(stdout);

   /* Release the association and free stuff*/
   acr_close_dicom_connection(afpin, afpout);

   free_file_names(fullpath, file_list, num_files);

   return EXIT_SUCCESS;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : process_mosaic_args
@INPUT      : dst - arguments from ParseArgv
              key
              argc
              argv
@OUTPUT     : argv
@RETURNS    : Number of arguments left
@DESCRIPTION: Routine to process the arguments for the -mosaic option.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 13, 1998 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int process_mosaic_args(char *dst, char *key, int argc, char **argv)
{
   Mosaic_Info *mosaic_info_list, tail, new;
   char *match_string;
   char *ptr;
   int size[2];
   int iarg;

   /* Get pointer to mosaic info list */
   mosaic_info_list = (Mosaic_Info *) dst;

   /* Check number of arguments */
   if (argc < 3) {
      (void) fprintf(stderr, "\"%s\" option requires 3 arguments\n", key);
      return -1;
   }

   /* Get arguments */
   match_string = argv[0];
   if (match_string == NULL) {
      (void) fprintf(stderr, "Null string passed to \"%s\"\n", key);
      return -1;
   }
   size[0] = strtol(argv[1], &ptr, 0);
   if ((ptr == argv[1]) || (size[0] < 1)) {
      (void) fprintf(stderr, "\"%s\" takes a positive integer (%s)\n",
                     key, argv[1]);
      return -1;
   }
   size[1] = strtol(argv[2], &ptr, 0);
   if ((ptr == argv[2]) || (size[1] < 1)) {
      (void) fprintf(stderr, "\"%s\" takes a positive integer (%s)\n",
                     key, argv[2]);
      return -1;
   }

   /* Update the argv list. We copy one extra element, to get the NULL */
   argc -= 3;
   for (iarg = 0; iarg <= argc; iarg++) {
      argv[iarg] = argv[iarg+3];
   }

   /* Create a new entry and save the values */
   new = (Mosaic_Info) malloc(sizeof(*new));
   new->match_string = match_string;
   new->size[0] = size[0];
   new->size[1] = size[1];
   new->next = NULL;

   /* Add the entry to the list */
   if (*mosaic_info_list == NULL) {
      *mosaic_info_list = new;
   }
   else {
      tail = *mosaic_info_list;
      while (tail->next != NULL) {
         tail = tail->next;
      }
      tail->next = new;
   }

   return argc;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_mosaic_info
@INPUT      : mosaic_info_list - pointer to beginning of list
              protocol - protocol name
@OUTPUT     : (none)
@RETURNS    : Pointer to matching mosaic info structure or NULL
@DESCRIPTION: Routine to find a mosaic info structure that matches the
              given protocol name. If none is found, then NULL is returned.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 13, 1998 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Mosaic_Info get_mosaic_info(Mosaic_Info mosaic_info_list, 
                                   char *protocol)
{
#define DIRECTORY_SEP '/'
#define EXTENSION_SEP '.'

   Mosaic_Info info;
   char *ptr;
   int have_extension;
   int num_dirs;
   int nchars_to_match;
   int protolen;

   /* Loop over list */
   for (info=mosaic_info_list; info != NULL; info = info->next) {

      /* Check for an exact match */
      if (info->match_string[0] == DIRECTORY_SEP) {
         if (strcmp(protocol, info->match_string) == 0) {
            break;
         }
      }

      /* Otherwise look for a match with part of the protocol string */
      else {

         /* Figure out how many directories are given in the match string
            and whether there is an extension */
         have_extension = FALSE;
         num_dirs = 0;
         for (ptr = &info->match_string[strlen(info->match_string)];
              ptr != info->match_string;
              ptr--) {

            /* Extension can only occur on filename part */
            if ((num_dirs == 0) && (*ptr == EXTENSION_SEP)) {
               have_extension = TRUE;
            }

            /* Count directory separators, but ignore repeats */
            if ((*ptr == DIRECTORY_SEP) && (*(ptr+1) != DIRECTORY_SEP)) {
               num_dirs++;
            }
         }

         /* Figure out where to start match on protocol string by counting
            directories */
         for (ptr = &protocol[strlen(protocol)-1]; ptr != protocol; ptr--) {
            if ((*ptr == DIRECTORY_SEP) && (*(ptr+1) != DIRECTORY_SEP)) {
               num_dirs--;
            }
            if (num_dirs < 0) break;
         }
         if (*ptr == DIRECTORY_SEP) ptr++;

         /* Match whole string or just prefix, depending on extension */
         nchars_to_match = strlen(info->match_string);
         protolen = strlen(ptr);
         if (have_extension && (protolen > nchars_to_match)) {
            nchars_to_match = protolen;
         }

         /* Do the match */
         if (strncmp(ptr, info->match_string, nchars_to_match) == 0) {
            break;
         }

      }    /* If exact match, else */

   }      /* End of loop */

   /* Return the match */
   return info;

}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_directory
@INPUT      : dst - client data passed by ParseArgv
              key - matching key in argv
              nextarg - argument following key in argv
@OUTPUT     : (none)
@RETURNS    : TRUE since nextarg is used.
@DESCRIPTION: Gets directory name from command line and adds it to the list.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 3, 1999 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int get_directory(char *dst, char *key, char *nextarg)
     /* ARGSUSED */
{
   /* Check for a following argument */
   if (nextarg == NULL) {
      (void) fprintf(stderr, 
                     "\"%s\" option requires an additional argument\n",
                     key);
      exit(EXIT_FAILURE);
   }

   /* Check whether we need more space for the directory name */
   if (num_dirs >= num_dirs_alloc) {
      num_dirs_alloc += 10;
      if (directory_list == NULL)
         directory_list = malloc(sizeof(*directory_list) * num_dirs_alloc);
      else
         directory_list = realloc(directory_list,
                                  sizeof(*directory_list) * num_dirs_alloc);
   }

   /* Add the new directory */
   directory_list[num_dirs] = nextarg;
   num_dirs++;

   return TRUE;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_file_names
@INPUT      : num_dirs - number of directories in list
              directory_list - list of directories in which to look
              patient_number - number found at beginning of file name
              first_image - first number in range
              last_image - last number in range
@OUTPUT     : num_files - number of files found
              file_list - list of files to be sentn
@RETURNS    : (nothing)
@DESCRIPTION: Routine to find all files in directory and of the
              form "<patient number>-<scan number>-<image number>.ima" and 
              with <image number> in the range given by first_image and
              last_image. The files are sorted by image number.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : July 11, 1997 (Peter Neelin)
@MODIFIED   : August 3, 1999 (P.N.)
---------------------------------------------------------------------------- */
public void get_file_names(int num_dirs, char **directory_list, 
                           int patient_number,
                           int first_image, int last_image,
                           int *num_files, char ***file_list,
                           int **file_dirindex_list)
{
   DIR *dir;
   struct dirent *dirent;
   char *filename;
   int patid, acqid, imgid;
   char ch;
   Sort_entry *sorting_files;
   int num_entries, num_alloc, ifile, idir;

   /* Set initial values */
   *num_files = 0;
   *file_list = NULL;
   *file_dirindex_list = NULL;

   /* Set up array for file names */
   num_entries = 0;
   num_alloc = ALLOC_INCREMENT;
   sorting_files = malloc(sizeof(*sorting_files) * num_alloc);

   /* Loop over directories */
   for (idir=0; idir < num_dirs; idir++) {

      /* Open the directory and find appropriate files */
      if ((dir = opendir(directory_list[idir])) == NULL) {
         (void) fprintf(stderr, "Error opening directory \"%s\": ", 
                        directory_list[idir]);
         perror(NULL);
         continue;
      }

      /* Loop over entries */
      while ((dirent = readdir(dir)) != NULL) {

         /* Get filename */
         filename = dirent->d_name;

         /* Parse the file name */
         if (sscanf(filename, "%d-%d-%d.ima%c", 
                    &patid, &acqid, &imgid, &ch) != 3) 
            continue;

         /* Check that we want this file */
         if ((patid != patient_number) || (imgid < first_image) ||
             (imgid > last_image)) 
            continue;

         /* Add more space if needed */
         if (num_entries >= num_alloc) {
            num_alloc += ALLOC_INCREMENT;
            sorting_files = realloc(sorting_files, 
                                    sizeof(*sorting_files) * num_alloc);
         }

         /* Add the entry and its number for sorting */
         sorting_files[num_entries].key = imgid;
         sorting_files[num_entries].name = strdup(dirent->d_name);
         sorting_files[num_entries].dirindex = idir;

         /* Increment the number of entries */
         num_entries++;

      }     /* End of loop over files in directory */

      /* Close the directory */
      (void) closedir(dir);

   }     /* End of loop over directories */

   /* Check the number of entries */
   if (num_entries <= 0) return;

   /* Sort the files */
   qsort(sorting_files, num_entries, sizeof(sorting_files[0]),
         sort_function);

   /* Copy the file names into an array */
   *num_files = num_entries;
   *file_list = malloc(sizeof(**file_list) * (num_entries + 1));
   *file_dirindex_list = malloc(sizeof(**file_dirindex_list) 
                                * (num_entries + 1));
   (*file_list)[num_entries] = NULL;
   (*file_dirindex_list)[num_entries] = -1;
   for (ifile=0; ifile < num_entries; ifile++) {
      (*file_list)[ifile] = sorting_files[ifile].name;
      (*file_dirindex_list)[ifile] = sorting_files[ifile].dirindex;
   }

   /* Free the sorting structure */
   free(sorting_files);

   return;
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : sort_function
@INPUT      : entry1
              entry2
@OUTPUT     : (none)
@RETURNS    : -1, 0, 1 for lt, eq, gt
@DESCRIPTION: Function to compare two numbers for sorting files.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : July 11, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int sort_function(const void *entry1, const void *entry2)
{
   int value1, value2;

   value1 = ((Sort_entry *) entry1)->key;
   value2 = ((Sort_entry *) entry2)->key;

   if (value1 < value2) return -1;
   else if (value1 > value2) return 1;
   else return 0;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : free_file_names
@INPUT      : fullpath
              file_list
              num_files
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to free file name strings
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : September 19, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void free_file_names(char *fullpath, char *file_list[], int num_files)
{
   int ifile;

   free(fullpath);
   for (ifile=0; ifile < num_files; ifile++) {
      free(file_list[ifile]);
   }
   free(file_list);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : multi_image_init
@INPUT      : group_list - group list as read in from file
@OUTPUT     : multi_image - structure containing information to be used
                 for modifying group list in loop over images
@RETURNS    : number of images to be processed
@DESCRIPTION: Routine to set up loop over multiple images in an input file.
@METHOD     : 
@GLOBALS    : mosaic_info_list
@CALLS      : 
@CREATED    : November 6, 1998 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int multi_image_init(Acr_Group group_list, 
                            Multi_Image *multi_image)
{
   int group_id, element_id;
   int last_image, grid_size;
   long new_image_size;
   void *data;
   Acr_Element element;
   char string[256];
   int idim;
   double pixel_spacing[2], separation;
   char *protocol;
   Mosaic_Info mosaic_info;

   /* Find the protocol name and look for a mosaic info structure */
   protocol = acr_find_string(group_list, SPI_Sequence_File_Name, "");
   mosaic_info = get_mosaic_info(mosaic_info_list, protocol);

   /* Check whether we need to do anything */
   multi_image->mosaic_seq = (mosaic_info != NULL);
   multi_image->packed = (mosaic_info != NULL);

   if (!multi_image->packed) return 1;

   /* Get some basic image information */
   multi_image->big[0] = acr_find_int(group_list, ACR_Columns, 1);
   multi_image->big[1] = acr_find_int(group_list, ACR_Rows, 1);
   multi_image->pixel_size = 
      (acr_find_int(group_list, ACR_Bits_allocated, 16)-1) / 8 + 1;

   /* Get the image size */
   multi_image->size[0] = mosaic_info->size[0];
   multi_image->size[1] = mosaic_info->size[1];

   /* Get the grid shape, checking that it is not too big if specified */
   multi_image->grid[0] = multi_image->big[0] / multi_image->size[0];
   multi_image->grid[1] = multi_image->big[1] / multi_image->size[1];
   if ((multi_image->grid[0] < 1) || (multi_image->grid[0] < 1)) {
      (void) fprintf(stderr, "Grid too small: %d x %d\n",
                     multi_image->grid[0], multi_image->grid[1]);
      exit(EXIT_FAILURE);
   }

   /* Check whether we need to do anything (1x1 grid may be the whole image) */
   grid_size = multi_image->grid[0] * multi_image->grid[1];
   if ((grid_size == 1) &&
       (multi_image->size[0] == multi_image->big[0]) &&
       (multi_image->size[1] == multi_image->big[1])) {
     /* had to remove this as now ANY images acquired with 
	the mosaic sequence need special treatment */
     multi_image->packed = FALSE;
      return 1;
   }

   /* Steal the image element from the group list */
   multi_image->big_image = acr_find_group_element(group_list, ACR_Image);

   if (multi_image->big_image == NULL) {
      (void) fprintf(stderr, "Couldn't find an image\n");
      exit(EXIT_FAILURE);
   }
   group_id = acr_get_element_group(multi_image->big_image);
   element_id = acr_get_element_element(multi_image->big_image);
   acr_group_steal_element(acr_find_group(group_list, group_id),
                           multi_image->big_image);

   /* Add a small image */
   new_image_size = 
      multi_image->size[0] * multi_image->size[1] * multi_image->pixel_size;
   data = malloc((size_t) new_image_size);
   multi_image->small_image = 
      acr_create_element(group_id, element_id,
                         acr_get_element_vr(multi_image->big_image),
                         new_image_size, data);
   acr_set_element_vr(multi_image->small_image,
      acr_get_element_vr(multi_image->big_image));
   acr_set_element_byte_order(multi_image->small_image,
      acr_get_element_byte_order(multi_image->big_image));
   acr_set_element_vr_encoding(multi_image->small_image,
      acr_get_element_vr_encoding(multi_image->big_image));
   acr_insert_element_into_group_list(&group_list, multi_image->small_image);

   /* Update the pixel size */
   acr_insert_short(&group_list, ACR_Rows, multi_image->size[1]);
   acr_insert_short(&group_list, ACR_Columns, multi_image->size[0]);

   /* Get image image index info */
   last_image = acr_find_int(group_list, SPI_Current_slice_number, 1);

   /* sub_images is now just the number of mosaic elements, even if
      they don't all contain slices */

   multi_image->sub_images = multi_image->grid[0] * multi_image->grid[1];

   /* NOTE:  multi_image->first_image is unkown now, since last_image
      is no longer necessarily correct.  We update slice numbers in
      main() now.

      multi_image->first_image = last_image -
	 multi_image->sub_images + 1; */

   /* Fiddle the pixel size */
   element = acr_find_group_element(group_list, ACR_Pixel_size);
   if ((element != NULL) &&
       (acr_get_element_numeric_array(element, 2, pixel_spacing) == 2)) {
      pixel_spacing[0] *= 
         (double) multi_image->big[0] / (double) multi_image->size[0];
      pixel_spacing[1] *= 
         (double) multi_image->big[1] / (double) multi_image->size[1];
      (void) sprintf(string, "%.15g\\%.15g", 
                     pixel_spacing[0], pixel_spacing[1]);
      acr_insert_string(&group_list, ACR_Pixel_size, string);
   }

   /* Get step between slices */
   separation = acr_find_double(group_list, ACR_Slice_thickness, 1.0) *
      (1.0 + acr_find_double(group_list, SPI_Slice_distance_factor,
			     0.0));

   element = acr_find_group_element(group_list, SPI_Image_normal);
   if ((element == NULL) ||
       (acr_get_element_numeric_array(element, 3, multi_image->normal) != 3)) {
      multi_image->normal[0] = 0.0;
      multi_image->normal[1] = 0.0;
      multi_image->normal[2] = 1.0;
   }
   for (idim=0; idim < 3; idim++) {
      multi_image->step[idim] = separation * multi_image->normal[idim];
   }

   /* Get position and correct to first slice */
   element = acr_find_group_element(group_list, SPI_Image_position);
   if ((element == NULL) ||
       (acr_get_element_numeric_array(element, 3, 
                                      multi_image->position) != 3)) {
      multi_image->position[0] = 0.0;
      multi_image->position[1] = 0.0;
      multi_image->position[2] = 0.0;
   }

   /* sub_images may be wrong here if there is overflow, so I'm
      commenting this out and moving this step to a point in main()
      where multi_image->sub_images has been corrected */

   /*   for (idim=0; idim < 3; idim++) {
	multi_image->position[idim] -= 
	(double) (multi_image->sub_images-1) * multi_image->step[idim];
	} */

   /* Return number of sub-images in this image */
   return multi_image->sub_images;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : multi_image_modify_group_list
@INPUT      : group_list - group list as read in from file
              multi_image - structure containing information to be used
                 for modifying group list in loop over images
              iimage - image number to process (counting from zero).
@OUTPUT     : group_list
@RETURNS    : (nothing)
@DESCRIPTION: Routine to modify the group list to use a new image.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 6, 1998 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void multi_image_modify_group_list(Acr_Group group_list, 
                                          Multi_Image *multi_image,
                                          int iimage)
{
   int irow, ibyte, idim, nbyte;
   int isub, jsub;
   char *new, *old;
   long old_offset, new_offset;
   double position[3], distance;
   char string[256];

   /* Check whether we need to do anything */
   if (!multi_image->packed) return;

   /* Check the image number */
   if ((iimage < 0) || (iimage > multi_image->sub_images)) {
      (void) fprintf(stderr, "Invalid image number to send: %d of %d\n",
                     iimage, multi_image->sub_images);
      exit(EXIT_FAILURE);
   }

   /* Figure out the sub-image indices */
   isub = iimage % multi_image->grid[0];
   jsub = iimage / multi_image->grid[0];

   /* Get pointers */
   old = acr_get_element_data(multi_image->big_image);
   new = acr_get_element_data(multi_image->small_image);

   /* Copy the image */
   nbyte = multi_image->size[0] * multi_image->pixel_size;
   for (irow=0; irow < multi_image->size[1]; irow++) {
      old_offset = isub * multi_image->size[0] +
         (jsub * multi_image->size[1] + irow) * multi_image->big[0];
      old_offset *= multi_image->pixel_size;
      new_offset = (irow * multi_image->size[0]) * multi_image->pixel_size;
      for (ibyte=0; ibyte < nbyte; ibyte++) {
         new[new_offset + ibyte] = old[old_offset + ibyte];
      }
   }

   /* Reset the byte order and VR encoding. This will be modified on each
      send according to what the connection needs. */
   acr_set_element_byte_order(multi_image->small_image,
      acr_get_element_byte_order(multi_image->big_image));
   acr_set_element_vr_encoding(multi_image->small_image,
      acr_get_element_vr_encoding(multi_image->big_image));

   /* Update the index */
   acr_insert_numeric(&group_list, SPI_Current_slice_number, 
                      (double) (iimage + multi_image->first_image));

   /* Update the position */
   distance = 0.0;
   for (idim=0; idim < 3; idim++) {
      position[idim] = multi_image->position[idim] + 
         (double) iimage * multi_image->step[idim];
      distance += position[idim] * multi_image->normal[idim];
   }

   (void) sprintf(string, "%.15g\\%.15g\\%.15g",
                  position[0], position[1], position[2]);
   acr_insert_string(&group_list, SPI_Image_position, string);
   acr_insert_numeric(&group_list, SPI_Image_distance, distance);
   update_coordinate_info(group_list);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : multi_image_cleanup
@INPUT      : group_list - group list as read in from file
              multi_image - structure containing information to be used
                 for modifying group list in loop over images
@OUTPUT     : multi_image
@RETURNS    : (nothing)
@DESCRIPTION: Routine to clean up after doing multi-image stuff.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 6, 1998 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void multi_image_cleanup(Acr_Group group_list, 
                                Multi_Image *multi_image)
/* ARGSUSED */
{

   if (!multi_image->packed) return;

   acr_delete_element(multi_image->big_image);
}


