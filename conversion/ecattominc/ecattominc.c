/* ----------------------------- MNI Header -----------------------------------
@NAME       : ecattominc
@INPUT      : argc, argv - command line arguments
@OUTPUT     : (none)
@RETURNS    : error status
@DESCRIPTION: Converts a CTI ECAT file to a minc format file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 3, 1996 (Peter Neelin)
@MODIFIED   : 
 * $Log: ecattominc.c,v $
 * Revision 6.7  2008-04-11 05:16:02  rotor
 *  * added config.h to ecattominc
 *  * removed minc_globdef.h from Makefile.am
 *
 * Revision 6.6  2008/01/17 02:33:01  rotor
 *  * removed all rcsids
 *  * removed a bunch of ^L's that somehow crept in
 *  * removed old (and outdated) BUGS file
 *
 * Revision 6.5  2008/01/12 19:08:14  stever
 * Add __attribute__ ((unused)) to all rcsid variables.
 *
 * Revision 6.4  2005/01/19 19:46:01  bert
 * Changes from Anthonin Reilhac
 *
 *
 * Revision 7.0  2004/08/13           Anthonin Reilhac
 * Portage of the code under linux environment
 *    - little / big Indian conversion
 *    - the vax conversion routines are now included within the distribution
 *    - MALLOC, REALLOC and FREE macros definined within the ecat_file.h body
 *      MALLOC and REALLOC use the malloc_check and realloc_check in ecat_file.c
 *      which are not private(static) anymore
 *      mni_def.h is not used anymore and 
 * Fixed x and y flipping when y size is odd.
 * Did not show up before since normal x and y size are even
 *
 * Revision 6.3  2000/09/08 18:17:15  neelin
 * Fixed swapping of x and y sizes when getting dimensions sizes from ecat file.
 * This has not previously shown up since normal ECAT images are square.
 *
 * Revision 6.2  1999/11/09 13:44:56  neelin
 * Year 2000 fixes for scan date in minc file.
 *
 * Revision 6.1  1999/10/29 17:52:01  neelin
 * Fixed Log keyword
 *
 * Revision 6.0  1997/09/12 13:24:22  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:25:21  neelin
 * Release of minc version 0.5
 *
 * Revision 4.1  1997/05/16  18:21:37  neelin
 * Changed calculation of z filter width to use BinSize rather than
 * PlaneSeparation.
 *
 * Revision 4.0  1997/05/07  20:06:04  neelin
 * Release of minc version 0.4
 *
 * Revision 1.2  1996/03/26  15:58:18  neelin
 * Various changes including changed coordinates in x and y and computing
 * FWHM from cutoff frequency.
 *
 * Revision 1.1  1996/01/18  14:52:14  neelin
 * Initial revision
 *
@COPYRIGHT  :
              Copyright 1996 Peter Neelin, McConnell Brain Imaging Centre, 
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <time.h>
#include <ParseArgv.h>
#include <time_stamp.h>
#include <minc.h>
/*#include <def_mni.h>    modif anthonin */
#include "ecat_file.h"



/* Type declarations */
typedef struct {
   char *name;
   char *values;
} ecat_header_data_type;

typedef struct {
   int nslices;
   int low_slice;
   int high_slice;
   double scan_time;
   double time_width;
   double half_life;
   double zstart;
   double zstep;
   double decay_correction;
   char isotope[16];
   int image_xsize;
   int image_ysize;
   int ordered_frame;
   int *ordered_slices;
   char image_type[16];
} frame_info_type;

typedef struct {
   int low_frame;
   int high_frame;
   int num_slices;
   int max_nslices;
   int max_xsize;
   int max_ysize;
   double xstart;
   double ystart;
   double xstep;
   double ystep;
   double xwidth;
   double ywidth;
   double zwidth;
   int decay_corrected;
   char img_units[16];
   char patient_name[40];
   char patient_sex[8];
   long patient_age;
   char patient_birthdate[40];
   char study_id[40];
   char start_time[40];
   long start_year;
   long start_month;
   long start_day;
   long start_hour;
   long start_minute;
   double start_seconds;
   char tracer[40];
   char injection_time[40];
   long injection_hour;
   long injection_minute;
   double injection_seconds;
   double injection_dose;
   int septa_retracted;
   ecat_header_data_type *main_field_list;
   int num_main_fields;
   ecat_header_data_type *subhdr_field_list;
   int num_subhdr_fields;
} general_info_type;

typedef struct {
   double sort_key;
   void *sort_value;
} sort_type;

/* Function declarations */
void usage_error(char *progname);
int get_frame_info(Ecat_file *ecat_fp, int slice_range[2],
                   int num_frames, frame_info_type *frame_info, 
                   general_info_type *general_info);
void sort_slices(int sort_over_time, int num_frames, 
                 frame_info_type *frame_info,
                 general_info_type *general_info);
int sortcmp(const void *val1, const void *val2);
int setup_minc_file(int mincid, int write_byte_data, int copy_all_header,
                    int ndims, long count[], int num_frames,
                    frame_info_type *frame_info,
                    general_info_type *general_info,
                    char *blood_file);
int get_slice(Ecat_file *ecat_fp, int frame_num, int slice_num, 
              long *pixel_max, double *image_max, short *image, 
              frame_info_type *frame_info,
              general_info_type *general_info);
int write_minc_slice(double scale, int write_byte_data,
                     int mincid, int icvid, 
                     int ndims,long start[], long count[], 
                     short *image, int image_xsize, int image_ysize,
                     long pixel_max, double image_max,
                     double scan_time, double time_width, double zpos);
double decay_correction(double scan_time, double measure_time, 
                        double start_time, double half_life);
void CreateBloodStructures (int mincHandle, int bloodHandle);
void FillBloodStructures (int mincHandle, int bloodHandle);

/* Constants */
#define TRUE 1
#define FALSE 0
#define MAX_DIMS 4
#define ECAT_ACTIVITY "ACTIVITY"
#define ECAT_CALIB_UNITS_UNKNOWN 0
#define ECAT_CALIB_UNITS_BECQUEREL 1
#define ECAT_CALIB_UNITS_CPS 3
#define MM_PER_CM 10.0
#define BECQUEREL_PER_NCURIE 37
#define BECQUEREL_PER_MCURIE (BECQUEREL_PER_NCURIE * 1e6)
#define NCURIE_PER_CC_STRING "nCi/cc"
#define SECONDS_PER_HOUR 3600
#define DEFAULT_RANGE INT_MIN
#define MINIMUM_HALFLIFE 0.1
#define FWHM_SCALE_FOR_HANN 1.082


/* we don't need mni_def anymore*/

/*#define MALLOC(size) ((void *) malloc(size))
#define FREE(ptr) free( (void *) ptr)
#define REALLOC(ptr, size) ((void *) realloc(ptr, size))*/


/* Main program */

int main(int argc, char *argv[])
{
   /* Variables for arguments */
   static int write_byte_data = TRUE;
   static int clobber = FALSE;
   static int verbose = TRUE;
   static int decay_correct = TRUE;
   static int slice_range[2] = {DEFAULT_RANGE, DEFAULT_RANGE};
   static int copy_all_header = TRUE;
   static char *blood_file = NULL;
   static int frame_range[2] = {DEFAULT_RANGE, DEFAULT_RANGE};

   /* Argument option table */
   static ArgvInfo argTable[] = {
      {"-byte", ARGV_CONSTANT, (char *) TRUE, (char *) &write_byte_data,
          "Write out data as bytes (default)."},
      {"-short", ARGV_CONSTANT, (char *) FALSE, (char *) &write_byte_data,
          "Write out data as short integers."},
      {"-decay_correct", ARGV_CONSTANT, (char *) TRUE, (char *) &decay_correct,
          "Do decay correction on images (default)."},
      {"-nodecay_correct", ARGV_CONSTANT, (char *) FALSE, 
          (char *) &decay_correct, "Don't do decay correction."},
      {"-clobber", ARGV_CONSTANT, (char *) TRUE, (char *) &clobber,
          "Overwrite existing file."},
      {"-noclobber", ARGV_CONSTANT, (char *) FALSE, (char *) &clobber,
          "Don't overwrite existing file (default)."},
      {"-verbose", ARGV_CONSTANT, (char *) TRUE, (char *) &verbose,
          "List files as they are converted (default)"},
      {"-quiet", ARGV_CONSTANT, (char *) FALSE, (char *) &verbose,
          "Do not list files as they are converted."},
      {"-slices", ARGV_INT, (char *) 2, (char *) slice_range,
          "Range of slices to copy (counting from 0)."},
      {"-frames", ARGV_INT, (char *) 2, (char *) frame_range,
          "Range of frames to copy (counting from 0)."},
      {"-frame", ARGV_INT, (char *) 1, (char *) frame_range,
          "Single frame to copy (counting from 0)."},
      {"-small_header", ARGV_CONSTANT, (char *) FALSE, 
          (char *) &copy_all_header,
          "Copy only basic header information."},
      {"-all_header", ARGV_CONSTANT, (char *) TRUE, (char *) &copy_all_header,
          "Copy all header information (default)."},
      {"-bloodfile", ARGV_STRING, (char *) 1, (char *) &blood_file,
          "Insert blood data from this file."},
      {NULL, ARGV_END, NULL, NULL, NULL}
   };

   /* Other variables */
   char *pname; /*name of the present command ->argv[0]*/
   char *mincfile; /*name of the minc file = output*/
   char *ecat_filename; /*name of the ecat7 file = input*/
   int num_frames;
   int num_bed_positions;
   int sort_over_time;
   frame_info_type *frame_info;
   general_info_type *general_info;
   long count[MAX_DIMS], start[MAX_DIMS];
   int ndims;
   int mincid, icvid, varid;
   int islice, iframe, i, slice_num, ifield, frame_num, low_frame, high_frame;
   long pixel_max;
   double image_max;
   double scale;
   short *image;
   Ecat_file *ecat_fp;
   int status;
   char *tm_stamp; /******** pk for minchistory*/
   double first_z, last_z, zstep;

   /* Get time stamp */
   tm_stamp = time_stamp(argc, argv);

   /* Check arguments */
   pname = argv[0];
   if (ParseArgv(&argc, argv, argTable, 0) || (argc != 3)) {
      usage_error(pname);
   }

   /* Check the slice range */
   if (slice_range[0] == DEFAULT_RANGE) {
      slice_range[0] = 0;
      slice_range[1] = INT_MAX;
   }
   else if (slice_range[1] == DEFAULT_RANGE) {
      slice_range[1] = slice_range[0];
   }
   if ((slice_range[0] < 0) || (slice_range[1] < 0) ||
       (slice_range[1] < slice_range[0])) {
      (void) fprintf(stderr, "%s: Error in slice range: %d to %d.\n",
                     pname, slice_range[0], slice_range[1]);
      exit(EXIT_FAILURE);
   }

   /* Check the frame range */
   if (frame_range[0] == DEFAULT_RANGE) {
      frame_range[0] = 0;
      frame_range[1] = INT_MAX;
   }
   else if (frame_range[1] == DEFAULT_RANGE) {
      frame_range[1] = frame_range[0];
   }
   if ((frame_range[0] < 0) || (frame_range[1] < 0) ||
       (frame_range[1] < frame_range[0])) {
      (void) fprintf(stderr, "%s: Error in frame range: %d to %d.\n",
                     pname, frame_range[0], frame_range[1]);
      exit(EXIT_FAILURE);
   }

   /* Get file names */
   ecat_filename = argv[1];
   mincfile = argv[2];

   /* Open the ECAT file */
   if ((ecat_fp=ecat_open(ecat_filename))==NULL) {
      (void) fprintf(stderr, "%s: Error opening file %s.\n",
                     pname, ecat_filename);
      exit(EXIT_FAILURE);
   }

   

   /* Get number of frames and bed positions to see if we are varying over
      time or interleaving slices */
   num_frames = ecat_get_num_frames(ecat_fp);
   num_bed_positions = ecat_get_num_bed_positions(ecat_fp);
   sort_over_time = TRUE;
   if ((num_frames > 1) && (num_bed_positions > 1)) {
      (void) fprintf(stderr, 
                     "%s: Cannot handle multiple frames and bed positions.\n",
                     pname);
      exit(EXIT_FAILURE);
   }
   if (num_bed_positions > 1) {
      num_frames = num_bed_positions;
      sort_over_time = FALSE;
   }

   /* Print log message */
   if (verbose) {
      (void) fprintf(stderr, "Reading headers.\n");
   }

   /* Get frame range */
   low_frame = 0; 
   high_frame = num_frames - 1;
   if (frame_range[0] > low_frame)
      low_frame = frame_range[0];
   if (frame_range[1] < high_frame)
      high_frame = frame_range[1];
   if (low_frame > high_frame)
      low_frame = high_frame;
   num_frames = high_frame - low_frame + 1;

   /* Read the files to get basic information */
   general_info=MALLOC(sizeof(*general_info));
   frame_info = MALLOC(num_frames * sizeof(*frame_info));
   general_info->low_frame = low_frame;
   general_info->high_frame = high_frame;
   status=get_frame_info(ecat_fp, slice_range,
                         num_frames, frame_info, general_info);
   if (status >= 0) {
      (void) fprintf(stderr, "%s: Error reading ECAT file %s on frame %d.\n", 
                     pname, ecat_filename, status);
      exit(EXIT_FAILURE);
   }

   /* Allocate space for ordered slice list if sorting over z position */
   if (!sort_over_time) {
      for (iframe=0; iframe<num_frames; iframe++) {
         frame_info[iframe].ordered_slices = 
            MALLOC(frame_info[iframe].nslices * sizeof(int));
      }
   }

   /* Sort the slices */
   sort_slices(sort_over_time, num_frames, 
               frame_info, general_info);
 
   /* Setup the minc file */
   if (sort_over_time && (num_frames>1)) {
      ndims = 4;
      count[0]=num_frames;
      count[1]=general_info->max_nslices;
   }
   else {
      ndims=3;
      count[0]=general_info->num_slices;
   }
   count[ndims-1] = general_info->max_xsize;
   count[ndims-2] = general_info->max_ysize;
   mincid = micreate(mincfile, (clobber ? NC_CLOBBER : NC_NOCLOBBER));
   (void) miattputstr(mincid, NC_GLOBAL, MIhistory, tm_stamp);
   icvid=setup_minc_file(mincid, write_byte_data, copy_all_header,
                         ndims, count, num_frames,
                         frame_info, general_info, blood_file);
   if (icvid==MI_ERROR) {
      (void) fprintf(stderr, 
                     "%s: Error setting up minc file %s.\n",
                     pname, mincfile);
      exit(EXIT_FAILURE);
   }

   /* Initialize minc start and count vectors */
   for (i=0; i<ndims-2; i++) count[i]=1;
   (void) miset_coords(ndims, (long) 0, start);

   /* Set up values for decay correction */
   scale = 1.0;

   /* Allocate an image buffer */
   image = MALLOC(general_info->max_xsize * general_info->max_ysize *
                  sizeof(short));

   /* Print log message */
   if (verbose) {
      (void) fprintf(stderr, "Copying frames:");
      (void) fflush(stderr);
   }

   /* Loop through files */
   for (iframe=0; iframe<num_frames; iframe++) {

      /* Print log message */
      if (verbose) {
         (void) fprintf(stderr, ".");
         (void) fflush(stderr);
      }

      /* Get decay correction (scan time is already measured from injection
         time) */
      if (decay_correct && !general_info->decay_corrected &&
          (strcmp(frame_info[iframe].image_type, ECAT_ACTIVITY) == 0)) {
         scale = decay_correction(frame_info[iframe].scan_time, 
                                  frame_info[iframe].time_width,
                                  0.0,
                                  frame_info[iframe].half_life);
      }
      else if (!decay_correct && general_info->decay_corrected) {
         scale = 1.0 / frame_info[iframe].decay_correction;
      }
      else {
         scale = 1.0;
      }

      /* Loop through slices */
      for (islice = 0; islice < frame_info[iframe].nslices; islice++) {

         /* Set the minc coordinates */
         if (sort_over_time && (num_frames>1)) {
            start[0]=frame_info[iframe].ordered_frame;
            start[1]=islice;
         }
         else if (sort_over_time) 
            start[0]=islice;
         else
            start[0]=frame_info[iframe].ordered_slices[islice];
         count[ndims-1] = frame_info[iframe].image_xsize;
         count[ndims-2] = frame_info[iframe].image_ysize;

         /* Copy the slice */
         slice_num = islice + frame_info[iframe].low_slice;
         frame_num = iframe + general_info->low_frame;
         if (get_slice(ecat_fp, frame_num, slice_num, 
                       &pixel_max, &image_max, image,
                       &frame_info[iframe], general_info) ||
             write_minc_slice(scale, write_byte_data,
                              mincid, icvid, ndims, start, count, image, 
                              frame_info[iframe].image_xsize, 
                              frame_info[iframe].image_ysize, 
                              pixel_max, image_max,
                              frame_info[iframe].scan_time,
                              frame_info[iframe].time_width,
                              frame_info[iframe].zstep * 
                              (double) slice_num +
                              frame_info[iframe].zstart)) {
            (void) fprintf(stderr, 
                           "%s: Error copying slice %d from frame %d.\n",
                           pname, slice_num, frame_num);
            exit(EXIT_FAILURE);
         }

      }        /* End slice loop */


   }         /* End frame loop */

   /* Write out average z step and start for irregularly spaced slices */
   if ((ndims!=MAX_DIMS) && (num_frames>1)) {
      start[0] = 0;
      varid = ncvarid(mincid, MIzspace);
      (void) mivarget1(mincid, varid, start, NC_DOUBLE, NULL,
                       &first_z);
      start[0] = general_info->num_slices - 1;
      (void) mivarget1(mincid, varid, start, NC_DOUBLE, NULL,
                       &last_z);
      if (start[0] > 0)
         zstep = (last_z - first_z) / ((double) start[0]);
      else
         zstep = 1.0;
      (void) miattputdbl(mincid, varid, MIstep, zstep);
      (void) miattputdbl(mincid, varid, MIstart, first_z);
   }

   /* Close minc file */
   (void) miattputstr(mincid, ncvarid(mincid, MIimage), MIcomplete, MI_TRUE);
   (void) miclose(mincid);

   /* Write out log message */
   if (verbose) {
      (void) fprintf(stderr, "Done\n");
      (void) fflush(stderr);
   }

   FREE(image);
   if (!sort_over_time) {
      for (iframe=0; iframe<num_frames; iframe++) {
         FREE(frame_info[iframe].ordered_slices);
      }
   }
   FREE(frame_info);
   for (ifield=0; ifield < general_info->num_main_fields; ifield++) {
      FREE(general_info->main_field_list[ifield].name);
      FREE(general_info->main_field_list[ifield].values);
   }
   for (ifield=0; ifield < general_info->num_subhdr_fields; ifield++) {
      FREE(general_info->subhdr_field_list[ifield].name);
      FREE(general_info->subhdr_field_list[ifield].values);
   }
   FREE(general_info->main_field_list);
   FREE(general_info->subhdr_field_list);
   FREE(general_info);

   exit(EXIT_SUCCESS);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : usage_error
@INPUT      : progname - program name
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Prints a usage error message and exits.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 3, 1996 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
void usage_error(char *progname)
{
   (void) fprintf(stderr, 
             "\nUsage: %s [<options>] <infile> <outfile.mnc>\n", progname);
   (void) fprintf(stderr,   
               "       %s [-help]\n\n", progname);

   exit(EXIT_FAILURE);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_frame_info
@INPUT      : ecat_fp - file pointer for ecat file
              slice_range - 2-component array giving range of slices
              num_frames - number of frames
@OUTPUT     : frame_info - array of structures containing information
                 about each frame.
              general_info - general information about the file.
@RETURNS    : (-1) if no error occurs, otherwise, the index
              of the first frame that could not be read.
@DESCRIPTION: Reads information for frame in the ECAT file
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 4, 1996 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
int get_frame_info(Ecat_file *ecat_fp, int slice_range[2],
                   int num_frames, frame_info_type *frame_info, 
                   general_info_type *general_info)
{
   static char *the_months[]=
      {NULL, "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", 
          "Aug", "Sep", "Oct", "Nov", "Dec"};
   int *num_slices, *max_nslices, *max_xsize, *max_ysize;
   int start_day, start_month, start_year;
   int start_hour, start_minute, start_seconds;
   frame_info_type *fip;
   int lvalue;
   double fvalue;
   char svalue[ECAT_MAX_STRING_LENGTH];
   int iframe, ifield, imult, num_fields, iheader, curframe;
   int length, newlength, multiplicity;
   struct tm *tm_ptr;
   ecat_header_data_type *field_list;
   Ecat_field_name field;
   char *description;
   time_t the_time;
   char *ptr;
   int isotope_name_okay;
   int septa_state;
   double cutoff, binsize;

   /* Initialize number of slices */
   num_slices = &(general_info->num_slices);
   max_nslices = &(general_info->max_nslices);
   max_xsize = &(general_info->max_xsize);
   max_ysize = &(general_info->max_ysize);
   *num_slices = 0;
   *max_nslices = 0;
   *max_xsize = *max_ysize = 0;
   general_info->decay_corrected = FALSE;

   /* Loop through files, reading information */
   for (iframe=0; iframe<num_frames; iframe++) {

      /* Get pointer to frame_info structure */
      fip = &frame_info[iframe];
      curframe = iframe + general_info->low_frame;

      /* Get number of slices */
      fip->low_slice = 0; 
      fip->high_slice = ecat_get_num_planes(ecat_fp) - 1;
      if (slice_range[0] > fip->low_slice)
         fip->low_slice = slice_range[0];
      if (slice_range[1] < fip->high_slice)
         fip->high_slice = slice_range[1];
      if (fip->low_slice > fip->high_slice)
         fip->low_slice = fip->high_slice;
      fip->nslices = fip->high_slice - fip->low_slice + 1;
      *num_slices += fip->nslices;
      if (fip->nslices > *max_nslices) 
         *max_nslices = fip->nslices;

      /* Get image width */
      if (ecat_get_subhdr_value(ecat_fp, curframe, 0, ECAT_X_Dimension, 0,
                                &lvalue, NULL, NULL)) return curframe;
      fip->image_xsize = lvalue;
      if (lvalue > *max_xsize) *max_xsize = lvalue;
      if (ecat_get_subhdr_value(ecat_fp, curframe, 0, ECAT_Y_Dimension, 0,
                                &lvalue, NULL, NULL)) return curframe;
      fip->image_ysize = lvalue;
      if (lvalue > *max_ysize) *max_ysize = lvalue;

      /* Get frame start time (in seconds) */
      if (ecat_get_subhdr_value(ecat_fp, curframe, 0, ECAT_Frame_Start_Time, 0,
                                &lvalue, NULL, NULL)) return curframe;
      fip->scan_time = (double) lvalue / 1000.0;
      
      /* Get length of frame (in seconds) */
      if (ecat_get_subhdr_value(ecat_fp, curframe, 0, ECAT_Frame_Duration, 0,
                                &lvalue, NULL, NULL)) return curframe;
      fip->time_width = (double) lvalue / 1000.0;

      /* Get scan type */
      if (ecat_get_main_value(ecat_fp, ECAT_Calibration_Units, 0,
                              &lvalue, NULL, NULL)) return curframe;
      if ((lvalue == ECAT_CALIB_UNITS_BECQUEREL) ||
          (lvalue == ECAT_CALIB_UNITS_UNKNOWN) ||
          (lvalue == ECAT_CALIB_UNITS_CPS)) {
         (void) strcpy(fip->image_type, ECAT_ACTIVITY);
      }
      else {
         (void) strcpy(fip->image_type, "");
      }

      /* Get isotope and half-life */
      if (ecat_get_main_value(ecat_fp, ECAT_Isotope_Name, 0,
                              NULL, NULL, fip->isotope))
         return curframe;
      if (ecat_get_main_value(ecat_fp, ECAT_Isotope_Halflife, 0,
                              NULL, &fip->half_life, NULL))
         return curframe;

      /* Check that they are reasonable */
      isotope_name_okay = TRUE;
      for (ptr=fip->isotope; 
           (*ptr != '\0') && (ptr < &fip->isotope[sizeof(fip->isotope)]); 
           ptr++) {
         if (!isprint((int) *ptr)) {
            isotope_name_okay = FALSE;
         }
      }
      if (ptr == fip->isotope)
         isotope_name_okay = FALSE;
      if (!isotope_name_okay || (fip->half_life < MINIMUM_HALFLIFE)) {
         (void) fprintf(stderr, "Ignoring bad isotope name or half-life.\n");
         fip->isotope[0] = '\0';
         fip->half_life = 0.0;
      }

      /* Get z start and step (correct start for non-zero first slice */
      if (ecat_get_main_value(ecat_fp, ECAT_Plane_Separation, 0,
                              NULL, &fip->zstep, NULL)) return curframe;
      fip->zstep *= -MM_PER_CM;
      if (ecat_get_num_bed_positions(ecat_fp) <= 1) {
         if (ecat_get_main_value(ecat_fp, ECAT_Init_Bed_Position, 0,
                                 NULL, &fip->zstart, NULL)) return curframe;
      }
      else {
         if (ecat_get_main_value(ecat_fp, ECAT_Bed_Position, curframe,
                                 NULL, &fip->zstart, NULL)) return curframe;
      }
      fip->zstart *= -MM_PER_CM;
      fip->zstart += fip->low_slice * fip->zstep;

      /* Check to see if file has been decay corrected */
      fvalue = 1.0;
      (void) ecat_get_subhdr_value(ecat_fp, curframe, 0, 
                                   ECAT_Decay_Corr_Fctr, 0,
                                   NULL, &fvalue, NULL);
      if (fvalue <= 0.0) fvalue = 1.0;
      fip->decay_correction = fvalue;
      if (fip->decay_correction != 1.0) {
         general_info->decay_corrected = TRUE;
      }

      /* Get general information from first frame */
      if (iframe==0) {

         /* Get pixel sizes */
         if (ecat_get_subhdr_value(ecat_fp, curframe, 0, ECAT_X_Pixel_Size, 0,
                                   NULL, &general_info->xstep, NULL))
            return curframe;
         if (ecat_get_subhdr_value(ecat_fp, curframe, 0, ECAT_Y_Pixel_Size, 0,
                                   NULL, &general_info->ystep, NULL))
            return curframe;
         general_info->xstep *= MM_PER_CM;
         general_info->ystep *= MM_PER_CM;

         /* Get location of first voxel */
         if (ecat_get_subhdr_value(ecat_fp, curframe, 0, ECAT_X_Offset, 0,
                                   NULL, &general_info->xstart, NULL))
            return curframe;
         if (ecat_get_subhdr_value(ecat_fp, curframe, 0, ECAT_Y_Offset, 0,
                                   NULL, &general_info->ystart, NULL))
            return curframe;
         general_info->xstart *= -MM_PER_CM;
         general_info->ystart *= MM_PER_CM;
         general_info->xstart -= 
            general_info->xstep * ((double) fip->image_xsize - 1.0) / 2.0;
         general_info->ystart -= 
            general_info->ystep * ((double) fip->image_ysize - 1.0) / 2.0;

         /* Get resolution in each direction (or zero if not found) */
         general_info->xwidth = general_info->ywidth = 
            general_info->zwidth = -1.0;
         (void) ecat_get_subhdr_value(ecat_fp, curframe, 0, ECAT_X_Resolution,
                                      0, NULL, &general_info->xwidth, NULL);
         (void) ecat_get_subhdr_value(ecat_fp, curframe, 0, ECAT_Y_Resolution,
                                      0, NULL, &general_info->ywidth, NULL);
         (void) ecat_get_subhdr_value(ecat_fp, curframe, 0, ECAT_Z_Resolution,
                                      0, NULL, &general_info->zwidth, NULL);
         general_info->xwidth *= MM_PER_CM;
         general_info->ywidth *= MM_PER_CM;
         general_info->zwidth *= MM_PER_CM;

         /* If resolution is not found, then use cutoff frequency and 
            assume FWHM for Hann filter */
         cutoff = -1.0;
         binsize = -1.0;
         (void) ecat_get_subhdr_value(ecat_fp, curframe, 0, 
                                      ECAT_Rfilter_Cutoff,
                                      0, NULL, &cutoff, NULL);
         if (cutoff <= 0) {
            (void) ecat_get_subhdr_value(ecat_fp, curframe, 0, 
                                         ECAT_Filter_Cutoff_Frequency,
                                         0, NULL, &cutoff, NULL);
         }
         (void) ecat_get_main_value(ecat_fp, ECAT_Bin_Size, 0,
                                    NULL, &binsize, NULL);
         binsize *= MM_PER_CM;
         if ((general_info->xwidth <= 0.0) && 
             (cutoff > 0.0) && (binsize > 0.0)) {
            general_info->xwidth = FWHM_SCALE_FOR_HANN * binsize / cutoff;
         }
         if ((general_info->ywidth <= 0.0) && 
             (cutoff > 0.0) && (binsize > 0.0)) {
            general_info->ywidth = FWHM_SCALE_FOR_HANN * binsize / cutoff;
         }
         if ((general_info->zwidth <= 0.0) &&
             !ecat_get_subhdr_value(ecat_fp, curframe, 0, 
                                    ECAT_Zfilter_Cutoff,
                                    0, NULL, &cutoff, NULL)) {
            general_info->zwidth = FWHM_SCALE_FOR_HANN * binsize / cutoff;
         }

         /* Get image range and units */
         if (ecat_get_main_value(ecat_fp, ECAT_Calibration_Units, 0,
                                 &lvalue, NULL, NULL)) return curframe;
         if (lvalue == ECAT_CALIB_UNITS_BECQUEREL) {
            (void) strcpy(general_info->img_units, NCURIE_PER_CC_STRING);
         }
         else {
            (void) strcpy(general_info->img_units, "");
         }

         /* Get patient information */
         if (ecat_get_main_value(ecat_fp, ECAT_Patient_Name, 0, 
                                 NULL, NULL, general_info->patient_name))
            return curframe;
         if (ecat_get_main_value(ecat_fp, ECAT_Patient_Sex, 0, 
                                 NULL, NULL, general_info->patient_sex))
            return curframe;
         switch (general_info->patient_sex[0]) {
         case 1:
         case 'M':
            (void) strcpy(general_info->patient_sex, MI_MALE);
            break;
         case 2:
         case 'F':
            (void) strcpy(general_info->patient_sex, MI_FEMALE);
            break;
         default:
            (void) strcpy(general_info->patient_sex, MI_OTHER);
            break;
         }
         if (ecat_get_main_value(ecat_fp, ECAT_Patient_Age, 0,
                                 &lvalue, NULL, NULL)) 
            general_info->patient_age = -1;
         else
            general_info->patient_age = lvalue;
         if (!ecat_get_main_value(ecat_fp, ECAT_Patient_Birth_Date, 0,
                                  &lvalue, NULL, NULL)) {
            /* Try to get the right birthday by adding half a day, using
               UTC and rounding down. This works because field stores 
               birthday at 0:00 converted using the local scanner 
               timezone. */
            the_time = (time_t) (lvalue + 12 * SECONDS_PER_HOUR);
            tm_ptr = gmtime(&the_time);
            (void) sprintf(general_info->patient_birthdate,
                           "%d-%s-%d", tm_ptr->tm_mday, 
                           the_months[tm_ptr->tm_mon+1], 
                           tm_ptr->tm_year+1900);
         }
         else {
            general_info->patient_birthdate[0] = '\0';
         }

         /* Get study information */
         if (ecat_get_main_value(ecat_fp, ECAT_Study_Type, 0,
                                 NULL, NULL, general_info->study_id))
            return curframe;
         if (!ecat_get_main_value(ecat_fp, ECAT_Scan_Start_Time, 0,
                                  &lvalue, NULL, NULL)) {
            the_time = (time_t) lvalue;
            tm_ptr = localtime(&the_time);
            general_info->start_day = tm_ptr->tm_mday;
            general_info->start_month = tm_ptr->tm_mon + 1;
            general_info->start_year = tm_ptr->tm_year + 1900;
            general_info->start_hour = tm_ptr->tm_hour;
            general_info->start_minute = tm_ptr->tm_min;
            general_info->start_seconds = tm_ptr->tm_sec;
         }
         else {
            if (ecat_get_main_value(ecat_fp, ECAT_Scan_Start_Day, 0,
                                    &start_day, NULL, NULL) ||
                ecat_get_main_value(ecat_fp, ECAT_Scan_Start_Month, 0,
                                    &start_month, NULL, NULL) ||
                ecat_get_main_value(ecat_fp, ECAT_Scan_Start_Year, 0,
                                    &start_year, NULL, NULL) ||
                ecat_get_main_value(ecat_fp, ECAT_Scan_Start_Hour, 0,
                                    &start_hour, NULL, NULL) ||
                ecat_get_main_value(ecat_fp, ECAT_Scan_Start_Minute, 0,
                                    &start_minute, NULL, NULL) ||
                ecat_get_main_value(ecat_fp, ECAT_Scan_Start_Second, 0,
                                    &start_seconds, NULL, NULL)) {
               return curframe;
            }
            start_year += 1900;
            if (start_year < 1950) start_year += 100;
            general_info->start_day = start_day;
            general_info->start_month = start_month;
            general_info->start_year = start_year;
            general_info->start_hour = start_hour;
            general_info->start_minute = start_minute;
            general_info->start_seconds = start_seconds;
         }
         (void) sprintf(general_info->start_time, "%d-%s-%d %d:%d:%d",
                        (int) general_info->start_day,
                        the_months[general_info->start_month],
                        (int) general_info->start_year,
                        (int) general_info->start_hour,
                        (int) general_info->start_minute,
                        (int) general_info->start_seconds);
         if ((int) strlen(fip->isotope) > 0) {
            if (ecat_get_main_value(ecat_fp, ECAT_Radiopharmaceutical, 0,
                                    NULL, NULL, general_info->tracer))
               return curframe;
         }
         else {
            general_info->tracer[0] = '\0';
         }
         if (!ecat_get_main_value(ecat_fp, ECAT_Dose_Start_Time, 0,
                                  &lvalue, NULL, NULL) && (lvalue != 0)) {
            the_time = (time_t) lvalue;
            (void) strcpy(general_info->injection_time, 
                          ctime(&the_time));
            tm_ptr = localtime(&the_time);
            general_info->injection_hour = tm_ptr->tm_hour;
            general_info->injection_minute = tm_ptr->tm_min;
            general_info->injection_seconds = tm_ptr->tm_sec;
         }
         else {
            general_info->injection_time[0] = '\0';
         }
         if (!ecat_get_main_value(ecat_fp, ECAT_Dosage, 0,
                                  NULL, &general_info->injection_dose, NULL)) {
            general_info->injection_dose /= BECQUEREL_PER_MCURIE;
         }
         else {
            general_info->injection_dose = -1.0;
         }

         /* Get septa state */
         septa_state = 0;
         if (!ecat_get_main_value(ecat_fp, ECAT_Septa_State, 0, 
                                  &septa_state, NULL, NULL)) {
            general_info->septa_retracted = (septa_state == 1);
         }

         /* Get list of header values */
         for (iheader=0; iheader < 2; iheader++) {

            /* Get space for first field */
            field_list = MALLOC(sizeof(*field_list));

            /* Loop through fields */
            ifield = 0;
            num_fields = 0;
            do {

               /* Get next field */
               if (iheader == 0) {
                  field = ecat_list_main(ecat_fp, ifield);
                  description = 
                     ecat_get_main_field_description(ecat_fp, field);
                  multiplicity = 
                     ecat_get_main_field_length(ecat_fp, field);
               }
               else {
                  field = ecat_list_subhdr(ecat_fp, ifield);
                  description = 
                     ecat_get_subhdr_field_description(ecat_fp, field);
                  multiplicity = 
                     ecat_get_subhdr_field_length(ecat_fp, field);
               }
               ifield++;

               /* Check for end of list */
               if ((field == ECAT_No_Field) || (description == NULL) ||
                   (multiplicity <= 0))
                  continue;

               /* Save the description */
               field_list[num_fields].name = strdup(description);

               /* Get space for the values */
               field_list[num_fields].values = NULL;
               length = 0;

               /* Loop through multiplicity */
               for (imult=0; imult < multiplicity; imult++) {

                  /* Get value */
                  svalue[0] = '\0';
                  if (iheader == 0) {
                     (void) ecat_get_main_value(ecat_fp, field, imult, 
                                                NULL, NULL, svalue);
                  }
                  else {
                     (void) ecat_get_subhdr_value(ecat_fp, curframe, 0, 
                                                  field, imult, 
                                                  NULL, NULL, svalue);
                  }

                  /* Save it */
                  if (field_list[num_fields].values == NULL) {
                     field_list[num_fields].values = strdup(svalue);
                     length = strlen(svalue);
                  }
                  else {
                     newlength = length + strlen(svalue) + 1;
                     field_list[num_fields].values = 
                        REALLOC(field_list[num_fields].values, 
                                (size_t) newlength + 1);
                     field_list[num_fields].values[length] = '\n';
                     (void) strcpy(&field_list[num_fields].values[length+1], 
                                   svalue);
                     length = newlength;
                  }

               }       /* End of loop over multiplicity */


               /* Get space for next field */
               field_list = REALLOC(field_list, 
                                    (num_fields+2) * sizeof(field_list[0]));

               /* Increment counter */
               num_fields++;

            } while (field != ECAT_No_Field);

            /* Save the list */
            if (iheader == 0) {
               general_info->main_field_list=field_list;
               general_info->num_main_fields=num_fields;
            }
            else {
               general_info->subhdr_field_list=field_list;
               general_info->num_subhdr_fields=num_fields;
            }

         }  /* Loop over headers */
            
      }  /* If first file */

   }   /* Loop over files */

   return -1;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : sort_slices
@INPUT      : sort_over_time - boolean indicating whether sort should be
                 over time or z position.
              num_frames - number of frames
              frame_info - array of frame information.
              general_info - general information about files. 
@OUTPUT     : frame_info - modified to give slice ordering information.
@RETURNS    : (nothing)
@DESCRIPTION: Sorts the slices for the output file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 12, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
void sort_slices(int sort_over_time, int num_frames, 
                 frame_info_type *frame_info,
                 general_info_type *general_info)
{
   int iframe, islice, isort, num_sort, slice_num;
   /* Variables for sorting */
   sort_type *sort_array;
   struct {
      int file;
      int slice;
   } *slice_ptr;
   frame_info_type *file_ptr;

   /* Allocate array for sorting */
   num_sort = (sort_over_time ? num_frames : general_info->num_slices);
   sort_array = MALLOC (num_sort * sizeof(*sort_array));

   /* Are we sorting over time or z position */
   if (sort_over_time) {
      /* Go through the files */
      for (iframe=0; iframe<num_frames; iframe++) {
         sort_array[iframe].sort_key = frame_info[iframe].scan_time;
         sort_array[iframe].sort_value = &frame_info[iframe];
      }
   }
   /* Otherwise we are sorting over z position */
   else {
      /* Go through the files and slices */
      isort=0;
      for (iframe=0; iframe<num_frames; iframe++) {
         for (islice = 0; islice < frame_info[iframe].nslices; islice++) {
            slice_num = islice + frame_info[iframe].low_slice;
            sort_array[isort].sort_key = frame_info[iframe].zstart +
               slice_num * frame_info[iframe].zstep;
            slice_ptr = MALLOC(sizeof(*slice_ptr));
            slice_ptr->file = iframe;
            slice_ptr->slice = islice;
            sort_array[isort].sort_value = slice_ptr;
            isort++;
         }
      }
   }

   /* Sort the slices */
   qsort(sort_array, num_sort, sizeof(*sort_array), sortcmp);

   /* Loop through sorted list */
   for (isort=0; isort<num_sort; isort++) {
      if (sort_over_time) {
         file_ptr=sort_array[isort].sort_value;
         file_ptr->ordered_frame = isort;
      }
      else {
         slice_ptr = sort_array[isort].sort_value;
         iframe = slice_ptr->file;
         islice = slice_ptr->slice;
         frame_info[iframe].ordered_slices[islice] = isort;
         FREE(slice_ptr);
      }
   }

   /* Free the sorting array */
   FREE(sort_array);

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : sortcmp
@INPUT      : val1 - first value
              val2 - second value
@OUTPUT     : (none)
@RETURNS    : 0 if values are the same, -1 if val1->sort_key < val2->sort_key
              and +1 if val1->sort_key > val2->sort_key.
@DESCRIPTION: Compares two double precision values. If they are the same,
              then return 0. If val1 < val2, return -1. If val1 > val2, 
              return +1.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 12, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
int sortcmp(const void *val1, const void *val2)
{

   if (((sort_type *)val1)->sort_key < 
       ((sort_type *)val2)->sort_key) 
      return -1;
   else if (((sort_type *)val1)->sort_key > 
            ((sort_type *)val2)->sort_key)
      return 1;
   else return 0;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : setup_minc_file
@INPUT      : mincid - id of minc file
              write_byte_data - boolean indicating whether data should be
                 written as bytes (TRUE) or shorts (FALSE).
              copy_all_header - boolean indicating whether all of the 
                 header information should be copied or not.
              ndims - number of dimensions for minc file
              count - lengths of dimensions minc file
              num_frames - number of frames.
              frame_info - array of information about frames.
              general_info - general information about file.
              blood_file - name of blood file containing data to include.
@OUTPUT     : (nothing)
@RETURNS    : Image conversion variable id or MI_ERROR if an error occurs.
@DESCRIPTION: Initializes the header of the minc file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 12, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
int setup_minc_file(int mincid, int write_byte_data, int copy_all_header,
                    int ndims, long count[], int num_frames,
                    frame_info_type *frame_info,
                    general_info_type *general_info,
                    char *blood_file)
{
   static char *dim_names_array[]={MItime, MIzspace, MIyspace, MIxspace};
   char **dim_names;
   static char *dimwidth_names_array[]={
      MItime_width, MIzspace_width, MIyspace_width, MIxspace_width};
   char **dimwidth_names;
   int dim[MAX_DIMS];
   int img, imgmax, imgmin, dimvarid, widvarid, icv, varid, ecat_var;
   int bloodid;
   int idim, ifield, num_fields, iheader, iframe;
   double vrange[2];
   char varname[MAX_NC_NAME];
   ecat_header_data_type *field_list;
   double dimwidths[MAX_DIMS];
   double *frame_times;
   double *frame_lengths;

   /* Set up dimension arrays for looping */
   dim_names = dim_names_array + MAX_DIMS - ndims;
   dimwidth_names = dimwidth_names_array + MAX_DIMS - ndims;
   for (idim=0; idim < ndims; idim++) {
      switch (idim + MAX_DIMS - ndims) {
      case 0: dimwidths[idim] = 1.0; break;
      case 1: dimwidths[idim] = general_info->zwidth; break;
      case 2: dimwidths[idim] = general_info->ywidth; break;
      case 3: dimwidths[idim] = general_info->xwidth; break;
      }
   }

   /* Create the dimensions */
   for (idim=0; idim<ndims; idim++) {
      dim[idim]=ncdimdef(mincid, dim_names[idim], count[idim]);
      dimvarid=micreate_std_variable(mincid, dim_names[idim], NC_DOUBLE, 
                                     (((idim==0) && (num_frames>1)) ? 1 : 0), 
                                     &dim[idim]);
      if (dimwidths[idim] > 0) {
         widvarid=micreate_std_variable(mincid, dimwidth_names[idim], 
                                        NC_DOUBLE, 
                                        ((strcmp(dim_names[idim], MItime)==0)
                                         ? 1 : 0), &dim[idim]);
      }
      else {
         widvarid = MI_ERROR;
      }

      /* Add attributes to the dimension variables */
      if (strcmp(dim_names[idim], MIzspace)==0) {
         /* Write out step and start. We will rewrite this for irregularly
            spaced files */
         (void) miattputdbl(mincid, dimvarid, MIstep, 
                            frame_info[0].zstep);
         (void) miattputdbl(mincid, dimvarid, MIstart, 
                            frame_info[0].zstart);
         (void) miattputstr(mincid, dimvarid, MIunits, "mm");
         (void) miattputstr(mincid, dimvarid, MIspacetype, MI_NATIVE);
         if (widvarid != MI_ERROR) {
            (void) miattputdbl(mincid, widvarid, MIwidth, 
                               (double) general_info->zwidth);
            (void) miattputstr(mincid, widvarid, MIunits, "mm");
            (void) miattputstr(mincid, widvarid, MIfiltertype, MI_GAUSSIAN);
         }
      }
      else if (strcmp(dim_names[idim], MIyspace)==0) {
         (void) miattputstr(mincid, dimvarid, MIunits, "mm");
         (void) miattputdbl(mincid, dimvarid, MIstart, 
                            (double) general_info->ystart);
         (void) miattputdbl(mincid, dimvarid, MIstep, 
                            (double) general_info->ystep);
         (void) miattputstr(mincid, dimvarid, MIspacetype, MI_NATIVE);
         if (widvarid != MI_ERROR) {
            (void) miattputdbl(mincid, widvarid, MIwidth, 
                               (double) general_info->ywidth);
            (void) miattputstr(mincid, widvarid, MIfiltertype, MI_GAUSSIAN);
         }
      }
      else if (strcmp(dim_names[idim], MIxspace)==0) {
         (void) miattputstr(mincid, dimvarid, MIunits, "mm");
         (void) miattputdbl(mincid, dimvarid, MIstart, 
                            (double) general_info->xstart);
         (void) miattputdbl(mincid, dimvarid, MIstep, 
                            (double) general_info->xstep);
         (void) miattputstr(mincid, dimvarid, MIspacetype, MI_NATIVE);
         if (widvarid != MI_ERROR) {
            (void) miattputdbl(mincid, widvarid, MIwidth, 
                               (double) general_info->xwidth);
            (void) miattputstr(mincid, widvarid, MIfiltertype, MI_GAUSSIAN);
         }
      }
      else if (strcmp(dim_names[idim], MItime)==0) {
         (void) miattputstr(mincid, dimvarid, MIunits, "seconds");
         (void) miattputstr(mincid, widvarid, MIunits, "seconds");
      }
   }

   /* Create the image variable */
   if (write_byte_data) {
      img=micreate_std_variable(mincid, MIimage, NC_BYTE, ndims, dim);
      (void) miattputstr(mincid, img, MIsigntype, MI_UNSIGNED);
      vrange[0]=0; vrange[1]=255;
   }
   else {
      img=micreate_std_variable(mincid, MIimage, NC_SHORT, ndims, dim);
      (void) miattputstr(mincid, img, MIsigntype, MI_SIGNED);
      vrange[0] = -32000;
      vrange[1] = 32000;
   }
   (void) ncattput(mincid, img, MIvalid_range, NC_DOUBLE, 2, vrange);
   (void) miattputstr(mincid, img, MIcomplete, MI_FALSE);

   /* Create the image max and min variables */
   imgmax=micreate_std_variable(mincid, MIimagemax, NC_DOUBLE, ndims-2, dim);
   imgmin=micreate_std_variable(mincid, MIimagemin, NC_DOUBLE, ndims-2, dim);
   (void) miattputstr(mincid, imgmax, MIunits,
                      general_info->img_units);
   (void) miattputstr(mincid, imgmin, MIunits,
                      general_info->img_units);

   /* Create the image conversion variable */
   icv=miicv_create();
   (void) miicv_setint(icv, MI_ICV_TYPE, NC_SHORT);

   /* Save patient info */
   varid = micreate_group_variable(mincid, MIpatient);
   (void) miattputstr(mincid, varid, MIfull_name, 
                      general_info->patient_name);
   (void) miattputstr(mincid, varid, MIsex, 
                      general_info->patient_sex);
   if (general_info->patient_age > 0)
      (void) ncattput(mincid, varid, MIage, NC_LONG, 1, 
                      &general_info->patient_age);
   if ((int) strlen(general_info->patient_birthdate) > 0)
      (void) miattputstr(mincid, varid, MIbirthdate, 
                         general_info->patient_birthdate);

   /* Save study info */
   varid = micreate_group_variable(mincid, MIstudy);
   (void) miattputstr(mincid, varid, MImodality, MI_PET);
   (void) miattputstr(mincid, varid, MImanufacturer, "CTI");
   (void) miattputstr(mincid, varid, MIstudy_id,
                      general_info->study_id);
   (void) miattputstr(mincid, varid, MIstart_time,
                      general_info->start_time);
   (void) ncattput(mincid, varid, MIstart_year, NC_LONG, 1,
                   &general_info->start_year);
   (void) ncattput(mincid, varid, MIstart_month, NC_LONG, 1,
                   &general_info->start_month);
   (void) ncattput(mincid, varid, MIstart_day, NC_LONG, 1,
                   &general_info->start_day);
   (void) ncattput(mincid, varid, MIstart_hour, NC_LONG, 1,
                   &general_info->start_hour);
   (void) ncattput(mincid, varid, MIstart_minute, NC_LONG, 1,
                   &general_info->start_minute);
   (void) ncattput(mincid, varid, MIstart_seconds, NC_DOUBLE, 1, 
                   &general_info->start_seconds);

   /* Save acquisition info */
   varid = micreate_group_variable(mincid, MIacquisition);
   if ((int) strlen(frame_info[0].isotope) > 0) {
      (void) miattputstr(mincid, varid, MIradionuclide,
                         frame_info[0].isotope);
   }
   if (frame_info[0].half_life > 0.0) {
      (void) miattputdbl(mincid, varid, MIradionuclide_halflife,
                         (double) frame_info[0].half_life);
   }
   if ((int) strlen(general_info->tracer) > 0) {
      (void) miattputstr(mincid, varid, MItracer, 
                         general_info->tracer);
   }
   if ((int) strlen(general_info->injection_time) > 0) {
      (void) miattputstr(mincid, varid, MIinjection_time,
                         general_info->injection_time);
      (void) ncattput(mincid, varid, MIinjection_hour, NC_LONG, 1, 
                      &general_info->injection_hour);
      (void) ncattput(mincid, varid, MIinjection_minute, NC_LONG, 1,
                      &general_info->injection_minute);
      (void) ncattput(mincid, varid, MIinjection_seconds, NC_DOUBLE, 1,
                      &general_info->injection_seconds);
   }
   if (general_info->injection_dose > 0.0) {
      (void) ncattput(mincid, varid, MIinjection_dose, NC_DOUBLE, 1,
                      &general_info->injection_dose);
      (void) miattputstr(mincid, varid, MIdose_units, "mCurie");
   }

   /* Save the septa state in a special ECAT variable, along with frame
      starts and lengths if we are not creating a time dimension. */
   varid = ncvardef(mincid, "ecat_acquisition", NC_LONG, 0, NULL);
   (void) miattputstr(mincid, varid, MIvartype, MI_GROUP);
   (void) miattputstr(mincid, varid, MIvarid, 
                      "ECAT-specific acquisition information");
   (void) miadd_child(mincid, ncvarid(mincid, MIrootvariable), varid);
   (void) miattputstr(mincid, varid, "septa_retracted", 
                      (general_info->septa_retracted ? MI_TRUE : MI_FALSE));
   if (ndims < MAX_DIMS) {
      frame_times = MALLOC(sizeof(*frame_times) * num_frames);
      frame_lengths = MALLOC(sizeof(*frame_lengths) * num_frames);
      for (iframe=0; iframe < num_frames; iframe++) {
         frame_times[iframe] = frame_info[iframe].scan_time;
         frame_lengths[iframe] = frame_info[iframe].time_width;
      }
      (void) ncattput(mincid, varid, "frame_times", NC_DOUBLE, num_frames,
                      frame_times);
      (void) ncattput(mincid, varid, "frame_lengths", NC_DOUBLE, num_frames,
                      frame_lengths);
   }

   /* If we want all of the values from the header, get them */
   if (copy_all_header) {

      for (iheader=0; iheader < 2; iheader++) {

         /* Set up values for either header */
         if (iheader == 0) {
            (void) strcpy(varname, "ecat-main");
            field_list = general_info->main_field_list;
            num_fields = general_info->num_main_fields;
         }
         else {
            (void) strcpy(varname, "ecat-subhdr");
            field_list = general_info->subhdr_field_list;
            num_fields = general_info->num_subhdr_fields;
         }

         /* Create a variable for ECAT fields */
         ecat_var = ncvardef(mincid, varname, NC_LONG, 0, NULL);
         (void) miattputstr(mincid, ecat_var, MIvartype, MI_GROUP);
         (void) miattputstr(mincid, ecat_var, MIvarid, "MNI ECAT variable");
         (void) miadd_child(mincid, ncvarid(mincid, MIrootvariable), ecat_var);

         /* Loop through fields */
         for (ifield=0; ifield < num_fields; ifield++){
            (void) miattputstr(mincid, ecat_var, field_list[ifield].name,
                               field_list[ifield].values);
         }

      }   /* Loop over headers */

   }   /* If copy_all_header */

   /* Open the blood file and create the variables if needed */
   if (blood_file != NULL) {
      bloodid = ncopen(blood_file, NC_NOWRITE);
      CreateBloodStructures(mincid, bloodid);
   }

   /* Attach the icv */
   (void) ncsetfill(mincid, NC_NOFILL);
   (void) ncendef(mincid);
   (void) miicv_attach(icv, mincid, img);

   /* Copy the blood data */
   if (blood_file != NULL) {
      FillBloodStructures(mincid, bloodid);
      ncclose(bloodid);
   }

   return icv;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_slice
@INPUT      : ecat_fp - file pointer for file
              frame_num - number of frame
              slice_num - slice to copy
              frame_info - information on frame
              general_info - general file information
@OUTPUT     : pixel_max - maximum pixel value
              image_max - real value to which pixel_max corresponds
              image - the image
@RETURNS    : Returns TRUE if an error occurs.
@DESCRIPTION: Gets an image from the file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 20, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
int get_slice(Ecat_file *ecat_fp, int frame_num, int slice_num, 
              long *pixel_max, double *image_max, short *image, 
              frame_info_type *frame_info,
              general_info_type *general_info)
     /* ARGSUSED */
{
   long npix, in, off;
   int pmax;
   int lvalue;
   short temp;
   double scale, global_scale;

   /* Get the image from the file */
   if (ecat_get_image(ecat_fp, frame_num, slice_num, image)) return TRUE;
   
   /* Flip the image to give positive x & y axes */
   npix = frame_info->image_xsize * frame_info->image_ysize;

   for(in = 0; in < npix/2; in++) {
     off = npix - in - 1;
     temp = image[off];
     image[off] = image[in];
     image[in] = temp;     
   }
   /* Get image and pixel max */
   if (ecat_get_subhdr_value(ecat_fp, frame_num, slice_num, 
                             ECAT_Image_Max, 0, &pmax, NULL, NULL) ||
       ecat_get_subhdr_value(ecat_fp, frame_num, slice_num, 
                             ECAT_Scale_Factor, 0, NULL, &scale, NULL))
      return TRUE;
   if (!ecat_get_main_value(ecat_fp, ECAT_Calibration_Factor, 0, 
                            NULL, &global_scale, NULL) && 
       (global_scale > 0.0)) {
      if (!ecat_get_main_value(ecat_fp, ECAT_Calibration_Units, 0,
                               &lvalue, NULL, NULL) &&
          (lvalue == ECAT_CALIB_UNITS_BECQUEREL)) {
         global_scale /= BECQUEREL_PER_NCURIE;
      }
   }
   else if (ecat_get_subhdr_value(ecat_fp, frame_num, slice_num,
                                  ECAT_Calibration_Factor, 0, 
                                  NULL, &global_scale, NULL)) {
      global_scale = 1.0;
   }
   *pixel_max = pmax;
   *image_max = (double) pmax * scale * global_scale;

   return FALSE;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : write_minc_slice
@INPUT      : scale - scale for decay correcting image
              write_byte_data - boolean indicating whether data should be
                 written as bytes (TRUE) or shorts (FALSE).
              mincid - id of minc file
              icvid - id of image conversion variable
              start - coordinate of slice in minc file
              count - edge lengths of image to write in minc file
              image - pointer to image buffer
              image_xsize, image_ysize - dimensions of image
              pixel_max - maximum pixel value
              image_max - real value to which pixel_max corresponds
              scan_time - time of slice
              time_width - time width of slice
              zpos - z position of slice
              frame_info - information on frame
              general_info - general file information
@OUTPUT     : (nothing)
@RETURNS    : Returns TRUE if an error occurs.
@DESCRIPTION: Writes out the image to the minc file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 12, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
int write_minc_slice(double scale, int write_byte_data,
                     int mincid, int icvid, 
                     int ndims, long start[], long count[], 
                     short *image, int image_xsize, int image_ysize,
                     long pixel_max, double image_max,
                     double scan_time, double time_width, double zpos)
     /*ARGSUSED*/
{
   double pixmin, pixmax;
   long ipix, npix;
   double maximum, minimum;

   /* Search for pixel max and min */
   npix = image_xsize * image_ysize;
   pixmin = pixmax = image[0];
   for (ipix=1; ipix<npix; ipix++) {
      if (image[ipix]>pixmax) pixmax = image[ipix];
      if (image[ipix]<pixmin) pixmin = image[ipix];
   }

   /* Get image max and min */
   maximum = image_max * pixmax / (double) pixel_max;
   minimum = image_max * pixmin / (double) pixel_max;

   /* Change valid range on icv */
   (void) miicv_detach(icvid);
   (void) miicv_setdbl(icvid, MI_ICV_VALID_MAX, (double) pixmax);
   (void) miicv_setdbl(icvid, MI_ICV_VALID_MIN, (double) pixmin);
   (void) miicv_attach(icvid, mincid, ncvarid(mincid, MIimage));

   /* Calculate real max and min */
   maximum = maximum * scale;
   minimum = minimum * scale;

   /* Write out the image */
   (void) miicv_put(icvid, start, count, image);

   /* Write out image max and min */
   (void) ncvarput1(mincid, ncvarid(mincid, MIimagemax), start, &maximum);
   (void) ncvarput1(mincid, ncvarid(mincid, MIimagemin), start, &minimum);

   /* Write out time and z position */
   if (ndims == MAX_DIMS) {
      (void) mivarput1(mincid, ncvarid(mincid, MItime), start, 
                       NC_DOUBLE, NULL, &scan_time);
      (void) mivarput1(mincid, ncvarid(mincid, MItime_width), start,
                       NC_DOUBLE, NULL, &time_width);
   }
   (void) mivarput1(mincid, ncvarid(mincid, MIzspace), &start[ndims-3],
                    NC_DOUBLE, NULL, &zpos);

   return FALSE;
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : decay_correction
@INPUT      : scan_time - time of beginning of sample
              measure_time - length of sample
              start_time - time to which we should decay correct
              half_life - half life of isotope
@OUTPUT     : (nothing)
@RETURNS    : decay correction scaling factor
@DESCRIPTION: Calculates the decay correction needed to get equivalent counts
              at start_time. Correction assumes constant activity (without
              decay) over measure_time.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 21, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
double decay_correction(double scan_time, double measure_time, 
                        double start_time, double half_life)
{
   double mean_life;
   double measure_correction;
   double decay;

   /* Check for negative half_life and calculate mean life */
   if (half_life <= 0.0) return 1.0;
   mean_life = half_life/ log(2.0);

   /* Normalize scan time and measure_time */
   scan_time = (scan_time - start_time) / mean_life;
   measure_time /= mean_life;

   /* Calculate correction for decay over measuring time (assuming a
      constant activity). Check for possible rounding errors. */
   if ((measure_time*measure_time/2.0) < DBL_EPSILON) {
      measure_correction = 1.0 - measure_time/2.0;
   }
   else {
      measure_correction = (1.0 - exp(-measure_time)) / fabs(measure_time);
   }

   /* Calculate decay */
   decay = exp(-scan_time) * measure_correction;
   if (decay<=0.0) decay = DBL_MAX;
   else decay = 1.0/decay;

   return decay;
}

