/* ----------------------------- MNI Header -----------------------------------
@NAME       : scxtominc
@INPUT      : argc, argv - command line arguments
@OUTPUT     : (none)
@RETURNS    : error status
@DESCRIPTION: Converts scanditronix format files to a minc format file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 11, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */

#ifndef lint
static char rcsid[]="$Header: /private-cvsroot/minc/conversion/scxtominc/scxtominc.c,v 1.5 1993-07-13 16:49:13 neelin Exp $";
#endif

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <ParseArgv.h>
#include <time_stamp.h>
#include <scx_file.h>
#include <minc.h>
#include <minc_def.h>
#include "isotope_list.h"

/* Macro definitions */
#undef MALLOC
#undef REALLOC
#undef FREE
#define  MALLOC( n_items, type ) \
   ( (void *) malloc( (size_t) (n_items) * sizeof(type) ) )
#define  REALLOC( ptr, n_items, type ) \
   ( (void *) realloc( (void *) ptr, (size_t) (n_items) * sizeof(type) ) )
#define  FREE( ptr ) \
   free( (void *) ptr )

/* Type declarations */
typedef struct {
   char name[8];
   int mult;
   nc_type type;
   void *att_vector;
} scx_mnem_list_type;

typedef struct {
   int nslices;
   int low_slice;
   int high_slice;
   double scan_time;
   double time_width;
   double half_life;
   double zstart;
   double zstep;
   char isotope[16];
   int image_size;
   int ordered_file;
   int *ordered_slices;
} scx_file_info_type;

typedef struct {
   int num_scx_slices;
   int max_nslices;
   int max_size;
   float zwidth;
   float xystep;
   float xywidth;
   long vmax;
   char img_units[16];
   char patient_name[32];
   char patient_sex[8];
   long patient_age;
   char study_id[40];
   char start_time[40];
   long start_year;
   long start_month;
   long start_day;
   long start_hour;
   long start_minute;
   float start_seconds;
   char tracer[10];
   char injection_time[40];
   long injection_hour;
   long injection_minute;
   float injection_seconds;
   float injection_dose;
   scx_mnem_list_type *mnem_list;
   int num_mnems;
} scx_general_info_type;

typedef struct {
   double sort_key;
   void *sort_value;
} scx_sort_type;

/* Function declarations */
void usage_error(char *progname);
int get_scx_file_info(int num_scx_files, char **scx_files, 
                      int slice_range[2],
                      scx_file_info_type *scx_file_info, 
                      scx_general_info_type *scx_general_info);
void sort_scx_slices(int sort_over_time, int num_scx_files, 
                     scx_file_info_type *scx_file_info,
                     scx_general_info_type *scx_general_info);
int sortcmp(const void *val1, const void *val2);
int setup_minc_file(int mincid, int write_byte_data, int copy_all_header,
                    int ndims, long count[], int num_scx_files,
                    scx_file_info_type *scx_file_info,
                    scx_general_info_type *scx_general_info);
int write_minc_slice(double scale, int write_byte_data,
                     int mincid, int icvid, 
                     int ndims,long start[], long count[], 
                     short *image, int image_size, 
                     long pixel_max, float image_max,
                     double scan_time, double time_width, double zpos);
int get_scx_slice(scx_file *scx_fp, int slice_num, 
                  long *pixel_max, float *image_max, short *image, 
                  scx_file_info_type *scx_file_info,
                  scx_general_info_type *scx_general_info);
double decay_correction(double scan_time, double measure_time, 
                        double start_time, double half_life);

/* Constants */
#define TRUE 1
#define FALSE 0
#define MAX_DIMS 4
#define HOURS_PER_DAY 24
#define MIN_PER_HOUR 60
#define SEC_PER_MIN 60

/* Scanditronix mnemonics used */
#define SCX_NCS  "NCS"
#define SCX_IMFM "IMFM"
#define SCX_DATY "DATY"
#define SCX_DATM "DATM"
#define SCX_DATD "DATD"
#define SCX_TIM  "TIM"
#define SCX_TIMH "TIMH"
#define SCX_TIMM "TIMM"
#define SCX_TIMS "TIMS"
#define SCX_TIHU "TIHU"
#define SCX_ITM  "ITM"
#define SCX_ITMH "ITMH"
#define SCX_ITMM "ITMM"
#define SCX_ITMS "ITMS"
#define SCX_MTM  "MTM"
#define SCX_ISO  "ISO"
#define SCX_CLV  "CLV"
#define SCX_CDI  "CDI"
#define SCX_CSZ  "CSZ"
#define SCX_PXS  "PXS"
#define SCX_MAX  "MAX"
#define SCX_IMUN "IMUN"
#define SCX_MAG  "MAG"
#define SCX_FWD  "FWD"
#define SCX_PNM  "PNM"
#define SCX_SEX  "SEX"
#define SCX_AGE  "AGE"
#define SCX_RIN  "RIN"
#define SCX_CAR  "CAR"
#define SCX_ACT  "ACT"

/* Main program */

int main(int argc, char *argv[])
{
   /* Variables for arguments */
   static int write_byte_data=TRUE;
   static int sort_over_time=TRUE;
   static int clobber=TRUE;
   static int verbose=TRUE;
   static int decay_correct=TRUE;
   static int slice_range[2]={0, 9999};
   static int copy_all_header=FALSE;

   /* Argument option table */
   static ArgvInfo argTable[] = {
      {"-byte", ARGV_CONSTANT, (char *) TRUE, (char *) &write_byte_data,
          "Write out data as bytes (default)."},
      {"-short", ARGV_CONSTANT, (char *) FALSE, (char *) &write_byte_data,
          "Write out data as short integers."},
      {"-time", ARGV_CONSTANT, (char *) TRUE, (char *) &sort_over_time,
          "Keep time ordering of data (default)."},
      {"-zposition", ARGV_CONSTANT, (char *) FALSE, (char *) &sort_over_time,
          "Sort data according to z position."},
      {"-decay_correct", ARGV_CONSTANT, (char *) TRUE, (char *) &decay_correct,
          "Do decay correction on images (default)."},
      {"-nodecay_correct", ARGV_CONSTANT, (char *) FALSE, 
          (char *) &decay_correct, "Don't do decay correction."},
      {"-clobber", ARGV_CONSTANT, (char *) TRUE, (char *) &clobber,
          "Overwrite existing file (default)."},
      {"-noclobber", ARGV_CONSTANT, (char *) FALSE, (char *) &clobber,
          "Don't overwrite existing file."},
      {"-verbose", ARGV_CONSTANT, (char *) TRUE, (char *) &verbose,
          "List files as they are converted (default)"},
      {"-quiet", ARGV_CONSTANT, (char *) FALSE, (char *) &verbose,
          "Do not list files as they are converted."},
      {"-small_header", ARGV_CONSTANT, (char *) FALSE, 
          (char *) &copy_all_header,
          "Copy only basic header information (default)."},
      {"-all_header", ARGV_CONSTANT, (char *) TRUE, (char *) &copy_all_header,
          "Copy all scanditronix header information."},
      {"-slices", ARGV_INT, (char *) 2, (char *) slice_range,
          "Range of slices to copy."},
      {NULL, ARGV_END, NULL, NULL, NULL}
   };

   /* Other variables */
   char *pname;
   char *mincfile;
   char **scx_files;
   int num_scx_files;
   scx_file_info_type *scx_file_info;
   scx_general_info_type *scx_general_info;
   long count[MAX_DIMS], start[MAX_DIMS];
   int ndims;
   int mincid, icvid;
   int islice, ifile, i, slice_num;
   long pixel_max;
   float image_max;
   double scale;
   short *image;
   scx_file *scx_fp;
   int status;
   char *tm_stamp;

   /* Get time stamp */
   tm_stamp = time_stamp(argc, argv);

   /* Check arguments */
   pname = argv[0];
   if (ParseArgv(&argc, argv, argTable, 0) || (argc < 3)) {
      usage_error(pname);
   }

   /* Check the slice range */
   if ((slice_range[0] < 0) || (slice_range[1] < 0) ||
       (slice_range[1] < slice_range[0])) {
      (void) fprintf(stderr, "%s: Error in slice range: %d to %d.\n",
                     pname, slice_range[0], slice_range[1]);
   }

   /* Get file names */
   mincfile = argv[argc-1];
   argv[argc-1] = NULL;    /* Null-terminate scx file list */
   num_scx_files = argc - 2;
   scx_files = &argv[1];

   /* Print log message */
   if (verbose) {
      (void) fprintf(stderr, "Reading headers.\n");
   }

   /* Read the files to get basic information */
   scx_general_info=MALLOC(1, *scx_general_info);
   scx_file_info = MALLOC(num_scx_files, *scx_file_info);
   status=get_scx_file_info(num_scx_files, scx_files, slice_range,
                            scx_file_info, scx_general_info);
   if (status >= 0) {
      (void) fprintf(stderr, "%s: Error reading scanditronix file %s.\n", 
                     pname, scx_files[status]);
      exit(EXIT_FAILURE);
   }

   /* Allocate space for ordered slice list if sorting over z position */
   if (!sort_over_time) {
      for (ifile=0; ifile<num_scx_files; ifile++) {
         scx_file_info[ifile].ordered_slices = 
            MALLOC(scx_file_info[ifile].nslices, int);
      }
   }

   /* Sort the slices */
   sort_scx_slices(sort_over_time, num_scx_files, 
                   scx_file_info, scx_general_info);
 
   /* Setup the minc file using the first scanditronix file */
   if (sort_over_time && (num_scx_files>1)) {
      ndims = 4;
      count[0]=num_scx_files;
      count[1]=scx_general_info->max_nslices;
   }
   else {
      ndims=3;
      count[0]=scx_general_info->num_scx_slices;
   }
   count[ndims-1] = count[ndims-2] = scx_general_info->max_size;
   mincid = nccreate(mincfile, (clobber ? NC_CLOBBER : NC_NOCLOBBER));
   (void) miattputstr(mincid, NC_GLOBAL, MIhistory, tm_stamp);
   icvid=setup_minc_file(mincid, write_byte_data, copy_all_header,
                         ndims, count, num_scx_files,
                         scx_file_info, scx_general_info);
   if (icvid==MI_ERROR) {
      (void) fprintf(stderr, 
                     "%s: Error setting up minc file %s from scx file %s.\n",
                     pname, mincfile, scx_files[0]);
      exit(EXIT_FAILURE);
   }

   /* Initialize minc start and count vectors */
   for (i=0; i<ndims-2; i++) count[i]=1;
   (void) miset_coords(ndims, (long) 0, start);

   /* Set up values for decay correction */
   scale = 1.0;

   /* Allocate an image buffer */
   image = MALLOC(scx_general_info->max_size * scx_general_info->max_size, 
                  short);

   /* Print log message */
   if (verbose) {
      (void) fprintf(stderr, "Copying files:\n");
   }

   /* Loop through files */
   for (ifile=0; ifile<num_scx_files; ifile++) {

      /* Open the scanditronix file */
      if ((scx_fp=scx_open(scx_files[ifile]))==NULL) {
         (void) fprintf(stderr, "%s: Error re-opening file %s.\n",
                        pname, scx_files[0]);
         exit(EXIT_FAILURE);
      }

      /* Print log message */
      if (verbose) {
         (void) fprintf(stderr, "   %s\n", scx_files[ifile]);
      }

      /* Get decay correction (scan time is already measured from injection
         time) */
      if (decay_correct) 
         scale = decay_correction(scx_file_info[ifile].scan_time, 
                                  scx_file_info[ifile].time_width,
                                  0.0,
                                  scx_file_info[ifile].half_life);
      else
         scale = 1.0;
      
      /* Loop through slices */
      for (islice = 0; islice < scx_file_info[ifile].nslices; islice++) {

         /* Set the minc coordinates */
         if (sort_over_time && (num_scx_files>1)) {
            start[0]=scx_file_info[ifile].ordered_file;
            start[1]=islice;
         }
         else if (sort_over_time) 
            start[0]=islice;
         else
            start[0]=scx_file_info[ifile].ordered_slices[islice];
         count[ndims-1] = count[ndims-2] = scx_file_info[ifile].image_size;

         /* Copy the slice */
         slice_num = islice + scx_file_info[ifile].low_slice;
         if (get_scx_slice(scx_fp, slice_num, &pixel_max, &image_max,
                           image, &scx_file_info[ifile], scx_general_info) ||
             write_minc_slice(scale, write_byte_data,
                              mincid, icvid, ndims, start, count, 
                              image, scx_file_info[ifile].image_size, 
                              pixel_max, image_max,
                              scx_file_info[ifile].scan_time,
                              scx_file_info[ifile].time_width,
                              scx_file_info[ifile].zstep * (double) slice_num +
                              scx_file_info[ifile].zstart)) {
            (void) fprintf(stderr, "%s: Error copying slice from file %s.\n",
                           pname, scx_files[ifile]);
            exit(EXIT_FAILURE);
         }

      }        /* End slice loop */

      /* Close the scanditronix file */
      scx_close(scx_fp);

   }         /* End file loop */

   (void) miattputstr(mincid, ncvarid(mincid, MIimage), MIcomplete, MI_TRUE);
   (void) ncclose(mincid);

   FREE(image);
   if (!sort_over_time) {
      for (ifile=0; ifile<num_scx_files; ifile++) {
         FREE(scx_file_info[ifile].ordered_slices);
      }
   }
   FREE(scx_file_info);
   FREE(scx_general_info);

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
@CREATED    : January 11, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
void usage_error(char *progname)
{
   (void) fprintf(stderr, 
             "\nUsage: %s [<options>] <infile> [...] <outfile>\n", progname);
   (void) fprintf(stderr,   
               "       %s [-help]\n\n", progname);

   exit(EXIT_FAILURE);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_scx_file_info
@INPUT      : num_scx_files - number of scanditronix files.
              scx_files - array of scanditronix file names.
              slice_range - 2-component array giving range of slices
@OUTPUT     : scx_file_info - array of structures containing information
                 about each file.
              scx_general_info - general information about the scx files.
@RETURNS    : (-1) if no error occurs, otherwise, the index (in scx_file)
              of the first file that could not be read.
@DESCRIPTION: Reads information from each scanditronix file given by scx_files.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 12, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
int get_scx_file_info(int num_scx_files, char **scx_files, 
                      int slice_range[2],
                      scx_file_info_type *scx_file_info, 
                      scx_general_info_type *scx_general_info)
{
   static char *the_months[]=
      {NULL, "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", 
          "Aug", "Sep", "Oct", "Nov", "Dec"};
   int *num_scx_slices, *max_nslices, *max_size;
   scx_file *fp;
   scx_file_info_type *sfip;
   long lvalue, timh, timm, tims, tihu, daty, datm, datd;
   float fvalue;
   char svalue[40];
   struct tm the_time;
   double inj_time;
   int ifile, i, imnem, imult, length;
   scx_mnem_types mtype;
   scx_mnem_list_type *mnem_list;
   void *att_vector;

   /* Initialize number of scanditronix slices */
   num_scx_slices = &(scx_general_info->num_scx_slices);
   max_nslices = &(scx_general_info->max_nslices);
   max_size = &(scx_general_info->max_size);
   *num_scx_slices = 0;
   *max_nslices = 0;
   *max_size = 0;

   /* Loop through files, reading information */
   for (ifile=0; ifile<num_scx_files; ifile++) {

      /* Open file */
      fp=scx_open(scx_files[ifile]);
      if (fp==NULL) return ifile;
      sfip = &scx_file_info[ifile];

      /* Get number of slices */
      if (scx_get_mnem(fp, SCX_NCS, 0, &lvalue, NULL, NULL)) return ifile;
      sfip->low_slice = 0; 
      sfip->high_slice = lvalue - 1;
      if (slice_range[0] > sfip->low_slice)
         sfip->low_slice = slice_range[0];
      if (slice_range[1] < sfip->high_slice)
         sfip->high_slice = slice_range[1];
      if (sfip->low_slice > sfip->high_slice)
         sfip->low_slice = sfip->high_slice;
      sfip->nslices = sfip->high_slice - sfip->low_slice + 1;
      *num_scx_slices += sfip->nslices;
      if (sfip->nslices > *max_nslices) 
         *max_nslices = sfip->nslices;

      /* Get image width */
      if (scx_get_mnem(fp, SCX_IMFM, 0, &lvalue, NULL, NULL)) return ifile;
      sfip->image_size = lvalue;
      if (lvalue > *max_size) *max_size = lvalue;

      /* Get time (in seconds) */
      if (scx_get_mnem(fp, SCX_DATY, 0, &daty, NULL, NULL) ||
          scx_get_mnem(fp, SCX_DATM, 0, &datm, NULL, NULL) ||
          scx_get_mnem(fp, SCX_DATD, 0, &datd, NULL, NULL) ||
          scx_get_mnem(fp, SCX_TIMH, 0, &timh, NULL, NULL) ||
          scx_get_mnem(fp, SCX_TIMM, 0, &timm, NULL, NULL) ||
          scx_get_mnem(fp, SCX_TIMS, 0, &tims, NULL, NULL) ||
          scx_get_mnem(fp, SCX_TIHU, 0, &tihu, NULL, NULL)) {
         return ifile;
      }
      the_time.tm_sec  = tims;
      the_time.tm_min  = timm;
      the_time.tm_hour = timh;
      the_time.tm_mday = datd;
      the_time.tm_mon  = datm;
      the_time.tm_year = daty;
      the_time.tm_isdst= FALSE;
      sfip->scan_time = 
         (double) mktime(&the_time) + (double) tihu/100.0;

      /* Get injection time (in seconds) (use date from before and check
         for time before inj_time) */
      if (scx_get_mnem(fp, SCX_ITMH, 0, &timh, NULL, NULL) ||
          scx_get_mnem(fp, SCX_ITMM, 0, &timm, NULL, NULL) ||
          scx_get_mnem(fp, SCX_ITMS, 0, &tims, NULL, NULL)) {
         return ifile;
      }
      the_time.tm_sec  = tims;
      the_time.tm_min  = timm;
      the_time.tm_hour = timh;
      the_time.tm_isdst= FALSE;
      inj_time = (double) mktime(&the_time);

      /* Set scan time to be from injection time. Check for scans over 
         midnight (cannot deal with scans longer than 1 day */
      sfip->scan_time -= inj_time;
      if (sfip->scan_time < 0.0)
         sfip->scan_time += HOURS_PER_DAY * MIN_PER_HOUR * SEC_PER_MIN;

      /* Get length of frame (in seconds) */
      if (scx_get_mnem(fp, SCX_MTM, 0, NULL, &fvalue, NULL))
         return ifile;
      sfip->time_width = fvalue;

      /* Get isotope and half-life */
      if (scx_get_mnem(fp, SCX_ISO, 0, NULL, NULL, 
                       sfip->isotope))
         return ifile;
      for (i=0; isotope_list[i].name !=NULL; i++) {
         if (strncmp(isotope_list[i].name, sfip->isotope, 
                     strlen(isotope_list[i].name))==0)
            break;
      }
      sfip->half_life = isotope_list[i].half_life;

      /* Get z start and step (correct start for non-zero first slice */
      if (scx_get_mnem(fp, SCX_CDI, 0, NULL, &fvalue, NULL)) return ifile;
      sfip->zstep = fvalue;
      if (scx_get_mnem(fp, SCX_CLV, 0, NULL, &fvalue, NULL)) return ifile;
      sfip->zstart = fvalue + sfip->low_slice * sfip->zstep;

      /* Get general information from first scx file */
      if (ifile==0) {
         if (scx_get_mnem(fp, SCX_CSZ, 0, 
                          NULL, &scx_general_info->zwidth, NULL))
            return MI_ERROR;
         if (scx_get_mnem(fp, SCX_PXS, 0,
                          NULL, &scx_general_info->xystep, NULL))
            return MI_ERROR;
         if (scx_get_mnem(fp, SCX_FWD, 0, 
                          NULL, &scx_general_info->xywidth, NULL))
            return MI_ERROR;
         if (scx_get_mnem(fp, SCX_MAX, 0, 
                          &scx_general_info->vmax, NULL, NULL))
            return MI_ERROR;
         if ((scx_general_info->vmax<=0) || (scx_general_info->vmax>32767))
            scx_general_info->vmax=32000;
         if (scx_get_mnem(fp, SCX_IMUN, 0, 
                          NULL, NULL, scx_general_info->img_units))
            return MI_ERROR;
         if (scx_get_mnem(fp, SCX_PNM, 0,
                          NULL, NULL, scx_general_info->patient_name)) 
            return MI_ERROR;
         if (scx_get_mnem(fp, SCX_SEX, 0,
                          NULL, NULL, scx_general_info->patient_sex)) 
            return MI_ERROR;
         if (scx_general_info->patient_sex[0]=='M')
            (void) strcpy(scx_general_info->patient_sex, MI_MALE);
         else if (scx_general_info->patient_sex[0]=='F')
            (void) strcpy(scx_general_info->patient_sex, MI_FEMALE);
         else
            (void) strcpy(scx_general_info->patient_sex, MI_OTHER);
         if (scx_get_mnem(fp, SCX_AGE, 0,
                          &scx_general_info->patient_age, NULL, NULL))
            return MI_ERROR;
         if (scx_get_mnem(fp, SCX_RIN, 0,
                          NULL, NULL, scx_general_info->study_id)) 
            return MI_ERROR;
         if (scx_get_mnem(fp, SCX_DATY, 0,
                          &scx_general_info->start_year, NULL, NULL)) 
            return MI_ERROR;
         if (scx_get_mnem(fp, SCX_DATM, 0,
                          &scx_general_info->start_month, NULL, NULL)) 
            return MI_ERROR;
         if (scx_get_mnem(fp, SCX_DATD, 0,
                          &scx_general_info->start_day, NULL, NULL)) 
            return MI_ERROR;
         if (scx_get_mnem(fp, SCX_TIMH, 0,
                          &scx_general_info->start_hour, NULL, NULL)) 
            return MI_ERROR;
         if (scx_get_mnem(fp, SCX_TIMM, 0,
                          &scx_general_info->start_minute, NULL, NULL)) 
            return MI_ERROR;
         if (scx_get_mnem(fp, SCX_TIMS, 0,
                          NULL, &scx_general_info->start_seconds, NULL)) 
            return MI_ERROR;
         if (scx_get_mnem(fp, SCX_TIHU, 0,
                          NULL, &fvalue, NULL)) 
            return MI_ERROR;
         scx_general_info->start_seconds += fvalue/100.0;
         if (scx_get_mnem(fp, SCX_TIM, 0, NULL, NULL, svalue))
            return MI_ERROR;
         (void) sprintf(scx_general_info->start_time, "%d-%s-%d %s",
                        (int) scx_general_info->start_day,
                        the_months[scx_general_info->start_month],
                        (int) scx_general_info->start_year+1900,
                        svalue);
         if (scx_get_mnem(fp, SCX_CAR, 0,
                          NULL, NULL, scx_general_info->tracer)) 
            return MI_ERROR;
         if (scx_get_mnem(fp, SCX_ITM, 0,
                          NULL, NULL, scx_general_info->injection_time)) 
            return MI_ERROR;
         if (scx_get_mnem(fp, SCX_ITMH, 0,
                          &scx_general_info->injection_hour, NULL, NULL)) 
            return MI_ERROR;
         if (scx_get_mnem(fp, SCX_ITMM, 0,
                          &scx_general_info->injection_minute, NULL, NULL)) 
            return MI_ERROR;
         if (scx_get_mnem(fp, SCX_ITMS, 0,
                          NULL, &scx_general_info->injection_seconds, NULL)) 
            return MI_ERROR;
         if (scx_get_mnem(fp, SCX_ACT, 0,
                          NULL, &scx_general_info->injection_dose, NULL)) 
            return MI_ERROR;

         /* Get list of header values */

         /* Get space for first mnemonic */
         mnem_list = MALLOC(1, *mnem_list);

         /* Loop through mnemonics */
         for (imnem=0; 
              scx_list_mnems(fp, imnem, mnem_list[imnem].name, 
                             &mnem_list[imnem].mult, &mtype)!=NULL;
              imnem++){

            /* Get space for attributes (handle strings differently) */
            switch (mtype) {
            case scx_string:
               mnem_list[imnem].type = NC_CHAR;
               length = 2;
               att_vector = MALLOC(length, char);
               *((char *) att_vector) = '\0';
               break;
            case scx_long:
               mnem_list[imnem].type = NC_LONG;
               att_vector = MALLOC(mnem_list[imnem].mult, long);
               break;
            case scx_float:
               mnem_list[imnem].type = NC_FLOAT;
               att_vector = MALLOC(mnem_list[imnem].mult, float); 
               break;
            }
         
            /* Loop through multiplicity */
            for (imult=0; imult < mnem_list[imnem].mult; imult++) {
               if (scx_get_mnem(fp, mnem_list[imnem].name, imult, 
                                &lvalue, &fvalue, svalue))
                  return MI_ERROR;
               switch (mtype) {
               case scx_string:
                  if (imult>0) {
                     *((char *) att_vector + length - 2) = '\n';
                     *((char *) att_vector + length - 1) = '\0';
                     length += 1;
                  }
                  length += strlen(svalue);
                  att_vector = REALLOC(att_vector, length, char);
                  att_vector = strcat((char *) att_vector, svalue);
                  break;
               case scx_long:
                  *(((long *) att_vector) + imult) = lvalue;
                  break;
               case scx_float:
                  *(((float *) att_vector) + imult) = fvalue;
                  break;
               }
               
            }  /* Loop over multiplicity */

            /* Save length of string */
            if (mtype==scx_string) {
               mnem_list[imnem].mult = length - 1;
            }
            
            /* Save pointer to attributes */
            mnem_list[imnem].att_vector = att_vector;

            /* Get space for next mnemonic */
            mnem_list = REALLOC(mnem_list, imnem+2, *mnem_list);
         }
         scx_general_info->mnem_list=mnem_list;
         scx_general_info->num_mnems=imnem;
            
      }  /* If first file */

      /* Close file */
      scx_close(fp);

   }   /* Loop over files */

   return -1;
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : sort_scx_slices
@INPUT      : sort_over_time - boolean indicating whether sort should be
                 over time or z position.
              num_scx_files - number of scanditronix files.
              scx_file_info - array of scanditronix file information.
              scx_general_info - general information about scx files. 
@OUTPUT     : scx_file_info - modified to give slice ordering information.
@RETURNS    : (nothing)
@DESCRIPTION: Sorts the scanditronix slices for the output file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 12, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
void sort_scx_slices(int sort_over_time, int num_scx_files, 
                     scx_file_info_type *scx_file_info,
                     scx_general_info_type *scx_general_info)
{
   int ifile, islice, isort, num_sort, slice_num;
   /* Variables for sorting */
   scx_sort_type *scx_sort;
   struct {
      int file;
      int slice;
   } *slice_ptr;
   scx_file_info_type *file_ptr;

   /* Allocate array for sorting */
   num_sort = (sort_over_time) ? num_scx_files : 
                                 scx_general_info->num_scx_slices;
   scx_sort = MALLOC (num_sort, *scx_sort);

   /* Are we sorting over time or z position */
   if (sort_over_time) {
      /* Go through the files */
      for (ifile=0; ifile<num_scx_files; ifile++) {
         scx_sort[ifile].sort_key = scx_file_info[ifile].scan_time;
         scx_sort[ifile].sort_value = &scx_file_info[ifile];
      }
   }
   /* Otherwise we are sorting over z position */
   else {
      /* Go through the files and slices */
      isort=0;
      for (ifile=0; ifile<num_scx_files; ifile++) {
         for (islice = 0; islice < scx_file_info[ifile].nslices; islice++) {
            slice_num = islice + scx_file_info[ifile].low_slice;
            scx_sort[isort].sort_key = scx_file_info[ifile].zstart +
               slice_num * scx_file_info[ifile].zstep;
            slice_ptr = MALLOC(1, *slice_ptr);
            slice_ptr->file = ifile;
            slice_ptr->slice = islice;
            scx_sort[isort].sort_value = slice_ptr;
            isort++;
         }
      }
   }

   /* Sort the slices */
   qsort(scx_sort, num_sort, sizeof(*scx_sort), sortcmp);

   /* Loop through sorted list */
   for (isort=0; isort<num_sort; isort++) {
      if (sort_over_time) {
         file_ptr=scx_sort[isort].sort_value;
         file_ptr->ordered_file = isort;
      }
      else {
         slice_ptr = scx_sort[isort].sort_value;
         ifile = slice_ptr->file;
         islice = slice_ptr->slice;
         scx_file_info[ifile].ordered_slices[islice] = isort;
         FREE(slice_ptr);
      }
   }

   /* Free the sorting array */
   FREE(scx_sort);

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

   if (((scx_sort_type *)val1)->sort_key < 
       ((scx_sort_type *)val2)->sort_key) 
      return -1;
   else if (((scx_sort_type *)val1)->sort_key > 
            ((scx_sort_type *)val2)->sort_key)
      return 1;
   else return 0;

}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : setup_minc_file
@INPUT      : mincid - id of minc file
              write_byte_data - boolean indicating whether data should be
                 written as bytes (TRUE) or shorts (FALSE).
              copy_all_header - boolean indicating whether all of the 
                 scanditronix header information should be copied or not.
              ndims - number of dimensions for minc file
              count - lengths of dimensions minc file
              scx_filename - name of scanditronix file for header values
              num_scx_files - number of scanditronix files.
              scx_file_info - array of information on scanditronix files.
              scx_general_info - general information about scx files
@OUTPUT     : (nothing)
@RETURNS    : Image conversion variable id or MI_ERROR if an error occurs.
@DESCRIPTION: Initializes the header of the minc file using information from
              the scanditronix file and the other structures.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 12, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
int setup_minc_file(int mincid, int write_byte_data, int copy_all_header,
                    int ndims, long count[], int num_scx_files,
                    scx_file_info_type *scx_file_info,
                    scx_general_info_type *scx_general_info)
{
   static char *dim_names_array[]={MItime, MIzspace, MIyspace, MIxspace};
   char **dim_names;
   static char *dimwidth_names_array[]={
      MItime_width, MIzspace_width, MIyspace_width, MIxspace_width};
   char **dimwidth_names;
   int dim[MAX_DIMS];
   int img, imgmax, imgmin, dimvarid, widvarid, icv, varid, scx_var;
   int idim, imnem;
   double vrange[2];

   /* Create the dimensions */
   dim_names = dim_names_array + MAX_DIMS - ndims;
   dimwidth_names = dimwidth_names_array + MAX_DIMS - ndims;
   for (idim=0; idim<ndims; idim++) {
      dim[idim]=ncdimdef(mincid, dim_names[idim], count[idim]);
      dimvarid=micreate_std_variable(mincid, dim_names[idim], NC_DOUBLE, 
                                     ((idim==0) && (num_scx_files>1)) ? 1 : 0, 
                                     &dim[idim]);
      widvarid=micreate_std_variable(mincid, dimwidth_names[idim], NC_DOUBLE, 
                                     (strcmp(dim_names[idim], MItime)==0) ? 
                                        1 : 0, 
                                     &dim[idim]);

      /* Add attributes to the dimension variables */
      if (strcmp(dim_names[idim], MIzspace)==0) {
         if ((ndims==MAX_DIMS) || (num_scx_files==1)) {
            (void) miattputdbl(mincid, dimvarid, MIstep, 
                               scx_file_info[0].zstep);
            (void) miattputdbl(mincid, dimvarid, MIstart, 
                               scx_file_info[0].zstart);
         }
         (void) miattputstr(mincid, dimvarid, MIunits, "mm");
         (void) miattputstr(mincid, dimvarid, MIspacetype, MI_NATIVE);
         (void) miattputdbl(mincid, widvarid, MIwidth, 
                            (double) scx_general_info->zwidth);
         (void) miattputstr(mincid, widvarid, MIunits, "mm");
         (void) miattputstr(mincid, widvarid, MIfiltertype, MI_GAUSSIAN);
      }
      else if ((strcmp(dim_names[idim], MIyspace)==0) ||
               (strcmp(dim_names[idim], MIxspace)==0)) {
         (void) miattputstr(mincid, dimvarid, MIunits, "mm");
         (void) miattputdbl(mincid, dimvarid, MIstep, 
                            (double) scx_general_info->xystep);
         (void) miattputstr(mincid, dimvarid, MIspacetype, MI_NATIVE);
         (void) miattputdbl(mincid, widvarid, MIwidth, 
                            (double) scx_general_info->xywidth);
         (void) miattputstr(mincid, widvarid, MIfiltertype, MI_GAUSSIAN);
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
      vrange[0] = -scx_general_info->vmax;
      vrange[1] = scx_general_info->vmax;
   }
   (void) ncattput(mincid, img, MIvalid_range, NC_DOUBLE, 2, vrange);
   (void) miattputstr(mincid, img, MIcomplete, MI_FALSE);

   /* Create the image max and min variables */
   imgmax=micreate_std_variable(mincid, MIimagemax, NC_DOUBLE, ndims-2, dim);
   imgmin=micreate_std_variable(mincid, MIimagemin, NC_DOUBLE, ndims-2, dim);
   (void) miattputstr(mincid, imgmax, MIunits,
                      scx_general_info->img_units);
   (void) miattputstr(mincid, imgmin, MIunits,
                      scx_general_info->img_units);

   /* Create the image conversion variable */
   icv=miicv_create();
   (void) miicv_setint(icv, MI_ICV_TYPE, NC_SHORT);
   (void) miicv_setdbl(icv, MI_ICV_VALID_MAX, 
                       (double) scx_general_info->vmax);
   (void) miicv_setdbl(icv, MI_ICV_VALID_MIN, 
                       (double) -scx_general_info->vmax);

   /* Save patient info */
   varid = micreate_group_variable(mincid, MIpatient);
   (void) miattputstr(mincid, varid, MIfull_name, 
                      scx_general_info->patient_name);
   (void) miattputstr(mincid, varid, MIsex, 
                      scx_general_info->patient_sex);
   (void) ncattput(mincid, varid, MIage, NC_LONG, 1, 
                   &scx_general_info->patient_age);

   /* Save study info */
   varid = micreate_group_variable(mincid, MIstudy);
   (void) miattputstr(mincid, varid, MImodality, MI_PET);
   (void) miattputstr(mincid, varid, MImanufacturer, "Scanditronix");
   (void) miattputstr(mincid, varid, MIstudy_id,
                      scx_general_info->study_id);
   (void) miattputstr(mincid, varid, MIstart_time,
                      scx_general_info->start_time);
   (void) ncattput(mincid, varid, MIstart_year, NC_LONG, 1,
                   &scx_general_info->start_year);
   (void) ncattput(mincid, varid, MIstart_month, NC_LONG, 1,
                   &scx_general_info->start_month);
   (void) ncattput(mincid, varid, MIstart_day, NC_LONG, 1,
                   &scx_general_info->start_day);
   (void) ncattput(mincid, varid, MIstart_hour, NC_LONG, 1,
                   &scx_general_info->start_hour);
   (void) ncattput(mincid, varid, MIstart_minute, NC_LONG, 1,
                   &scx_general_info->start_minute);
   (void) ncattput(mincid, varid, MIstart_seconds, NC_FLOAT, 1, 
                   &scx_general_info->start_seconds);

   /* Save acquisition info */
   varid = micreate_group_variable(mincid, MIacquisition);
   (void) miattputstr(mincid, varid, MIradionuclide,
                      scx_file_info[0].isotope);
   if (scx_file_info[0].half_life > 0.0) {
      (void) miattputdbl(mincid, varid, MIradionuclide_halflife,
                         (double) scx_file_info[0].half_life);
   }
   (void) miattputstr(mincid, varid, MItracer, 
                      scx_general_info->tracer);
   (void) miattputstr(mincid, varid, MIinjection_time,
                      scx_general_info->injection_time);
   (void) ncattput(mincid, varid, MIinjection_hour, NC_LONG, 1, 
                   &scx_general_info->injection_hour);
   (void) ncattput(mincid, varid, MIinjection_minute, NC_LONG, 1,
                   &scx_general_info->injection_minute);
   (void) ncattput(mincid, varid, MIinjection_seconds, NC_FLOAT, 1,
                   &scx_general_info->injection_seconds);
   (void) ncattput(mincid, varid, MIinjection_dose, NC_FLOAT, 1,
                   &scx_general_info->injection_dose);
   (void) miattputstr(mincid, varid, MIdose_units, "mCurie");

   /* If we want all of the values from the scanditronix header, get them */
   if (copy_all_header) {

      /* Create a variable for scx mnemonics */
      scx_var = ncvardef(mincid, "scanditronix", NC_LONG, 0, NULL);
      (void) miattputstr(mincid, scx_var, MIvartype, MI_GROUP);
      (void) miattputstr(mincid, scx_var, MIvarid, "MNI SCX variable");
      (void) miadd_child(mincid, ncvarid(mincid, MIrootvariable), scx_var);

      /* Loop through mnemonics */
      for (imnem=0; imnem<scx_general_info->num_mnems; imnem++){
         ncattput(mincid, scx_var, 
                  scx_general_info->mnem_list[imnem].name,
                  scx_general_info->mnem_list[imnem].type,
                  scx_general_info->mnem_list[imnem].mult,
                  scx_general_info->mnem_list[imnem].att_vector);
      } /* Loop through mnemonics */
   }   /* If copy_all_header */

   /* Attach the icv */
   (void) ncendef(mincid);
   (void) miicv_attach(icv, mincid, img);

   return icv;
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_scx_slice
@INPUT      : scx_fp - file pointer for scanditronix file
              slice_num - slice to copy
              scx_file_info - information on scanditronix file.
              scx_general_info - general scx file information
@OUTPUT     : pixel_max - maximum pixel value
              image_max - real value to which pixel_max corresponds
              image - scanditronix image
@RETURNS    : Returns TRUE if an error occurs.
@DESCRIPTION: Gets a scanditronix image.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 20, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
int get_scx_slice(scx_file *scx_fp, int slice_num, 
                  long *pixel_max, float *image_max, short *image, 
                  scx_file_info_type *scx_file_info,
                  scx_general_info_type *scx_general_info)
{       /* ARGSUSED */
   long npix, ix, iy, y_offset, off1, off2;
   short temp;

   /* Get the image from the scanditronix file */
   if (scx_get_image(scx_fp, slice_num, image)) return TRUE;

   /* Flip the scanditronix image to give positive x & y axes */
   npix = scx_file_info->image_size * scx_file_info->image_size;
   for (iy=0; iy<scx_file_info->image_size/2; iy++) {
      y_offset = iy * scx_file_info->image_size;
      for (ix=0; ix<scx_file_info->image_size; ix++) {
         off1 = y_offset + ix;
         off2 = npix - off1 - 1;
         temp = image[off1];
         image[off1] = image[off2];
         image[off2] = temp;
      }
   }

   /* Get image and pixel max */
   if (scx_get_mnem(scx_fp, SCX_MAG, slice_num, NULL, image_max, NULL))
      return TRUE;
   if (scx_get_mnem(scx_fp, SCX_MAX, 0, pixel_max, NULL, NULL)) return TRUE;
   if ((*pixel_max<=0) || (*pixel_max>32767)) *pixel_max = 32000;

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
              pixel_max - maximum pixel value
              image_max - real value to which pixel_max corresponds
              scan_time - time of slice
              time_width - time width of slice
              zpos - z position of slice
              scx_file_info - information on scanditronix file.
              scx_general_info - general scx file information
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
                     int ndims,long start[], long count[], 
                     short *image, int image_size, 
                     long pixel_max, float image_max,
                     double scan_time, double time_width, double zpos)
{
   double pixmin, pixmax;
   long ipix, npix;
   double maximum, minimum;

   /* If converting to byte data, look for pixel max/min */
   if (write_byte_data) {

      /* Search for pixel max and min */
      npix = image_size * image_size;
      pixmin = pixmax = image[0];
      for (ipix=1; ipix<npix; ipix++) {
         if (image[ipix]>pixmax) pixmax = image[ipix];
         if (image[ipix]<pixmin) pixmin = image[ipix];
      }

      /* Get image max and min */
      maximum = image_max * pixmax / (double) pixel_max;
      minimum = image_max * pixmin / (double)pixel_max;

      /* Change valid range on icv */
      (void) miicv_detach(icvid);
      (void) miicv_setdbl(icvid, MI_ICV_VALID_MAX, (double) pixmax);
      (void) miicv_setdbl(icvid, MI_ICV_VALID_MIN, (double) pixmin);
      (void) miicv_attach(icvid, mincid, ncvarid(mincid, MIimage));
      
   }

   /* Not converting data to bytes */
   else {
      maximum = image_max;
      minimum = -image_max;
   }
   maximum = maximum * scale;
   minimum = minimum * scale;

   /* Write out the image */
   (void) miicv_put(icvid, start, count, image);

   /* Write out image max and min */
   (void) ncvarput1(mincid, ncvarid(mincid, MIimagemax), start, &maximum);
   (void) ncvarput1(mincid, ncvarid(mincid, MIimagemin), start, &minimum);

   /* Write out time and z position */
   if (ndims==MAX_DIMS) {
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

