/* mincstats.c
 *
 * Andrew Janke - rotor@cmr.uq.edu.au
 * Centre for Magnetic Resonance
 * University of Queensland, Australia
 *
 * $Log: mincstats.c,v $
 * Revision 1.11  2002-09-05 00:41:57  rotor
 * ----------------------------------------------------------------------
 * Fixed clash of C/L arguments in mincstats (-max and -max_bins)
 *    -max_bins has now been changed to -int_max_bins
 *
 * Committing in .
 *
 * Modified Files:
 *  mincstats.c
 * ----------------------------------------------------------------------
 *
 * Revision 1.10  2002/04/08 21:46:34  jgsled
 * fixed problem where mincstats segmentation fault when trying to close a NULL file pointer
 *
 * Revision 1.9  2002/01/09 13:23:16  neelin
 * Removed extraneous newline for histogram output with -quiet turned on.
 *
 * Revision 1.8  2001/12/11 14:36:00  neelin
 * Added -discrete_histogram and -integer_histogram, as well as
 * -world_only options.
 *
 * Revision 1.7  2001/12/10 14:11:45  neelin
 * Obtained speed improvement by only doing CoM summing when needed.
 *
 * Revision 1.6  2001/12/06 21:54:25  neelin
 * Check for -quiet when printing volume and mask ranges.
 *
 * Revision 1.5  2001/12/06 21:48:16  neelin
 * Significant modifications to get mincstats working. Also added support
 * for multiple ranges in the volume and the mask. Added -binvalue and
 * -maskbinvalue options.
 *
 * Revision 1.4  2001/12/05 17:20:13  neelin
 * Lots of fixes to get it working. Also fixed up centre of mass calculation
 * and display.
 *
 * Revision 1.2  2001/11/28 21:59:39  neelin
 * Significant modifications. Removed dependencies on volume_io.
 * Added support for centre-of-mass calculation.
 * Compiles but crashes under linux.
 *
 * Revision 1.1  2001/11/28 21:54:08  neelin
 * *** empty log message ***
 *
 *
 * Thu Feb  1 17:16:21 EST 2001 - completed filename checking and other 
 *                                   mundane stuff - first release 1.0
 * Wed Jan 31 14:33:30 EST 2001 - finished -entropy, -median and -histogram
 * Fri Jan 19 15:25:44 EST 2001 - created first version from minccount as a 
 *                                   mirror of Alex Zijdenbos + John Sleds 
 *                                   volume_stats proggy with less memory 
 *                                   overhead
 * Original version - 1999 sometime..
 *
 * A few notes on the stats in here.
 * Median   - This is a "histogram median" based upon calculating 
 *               the volume of histogram above and below the median
 *               Thus the more bins the more accurate the approximation
 * Majority - This is the centre of the largest bin in the histogram
 * BiModalT - The Bi-Modal Threshold calculated using the method described in
 *               Otsu N, "A Threshold Selection Method from Grey-level Histograms"
 *               IEEE Trans on Systems, Man and Cybernetics. 1979, 9:1;62-66.
 * Entropy  - This is what is called "Shannon Entropy" of a histogram
 *               H(x) = - Sum(P(i) * log2(P(i))
 *               Where P(i) is the bin probability
 * PctT     - The threshold needed for a particular "Critical percentage" of
 *               of a histogram.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <ParseArgv.h>
#include <minc_def.h>
#include <voxel_loop.h>

#ifndef TRUE
#  define TRUE  1
#  define FALSE 0
#endif

#define SQR(x)    ((x)*(x))

#define WORLD_NDIMS 3

#define DEFAULT_BOOLEAN (-1)

#define BINS_DEFAULT 2000

/* Double_Array structure */
typedef struct {
   int numvalues;
   double *values;
} Double_Array;

/* Stats structure */
typedef struct {
      double vol_range[2];
      double mask_range[2];
      float  *histogram;
      double hvoxels;
      double vvoxels;
      double volume;
      double vol_per;
      double hist_per;
      double min;
      double max;
      double sum;
      double sum2;
      double mean;
      double variance;
      double stddev;
      double voxel_com_sum[WORLD_NDIMS];
      double voxel_com[WORLD_NDIMS];
      double world_com[WORLD_NDIMS];
      double median;
      double majority;
      double biModalT;
      double pct_T;
      double entropy;
} Stats_Info;

/* Function prototypes */
void do_math(void *caller_data, long num_voxels, int input_num_buffers, int input_vector_length,
                    double *input_data[], int output_num_buffers, int output_vector_length,
                    double *output_data[], Loop_Info *loop_info);
void do_stats(double value, long index[], Stats_Info *stats);
void print_result(char* title, double result);
long get_minc_nvoxels(int mincid);
double get_minc_voxel_volume(int mincid);
void get_minc_attribute(int mincid, char *varname, char *attname, 
                       int maxvals, double vals[]);
int get_minc_ndims(int mincid);
void find_minc_spatial_dims(int mincid, int space_to_dim[], int dim_to_space[]);
void get_minc_voxel_to_world(int mincid, 
                             double voxel_to_world[WORLD_NDIMS][WORLD_NDIMS+1]);
void normalize_vector(double vector[]);
void transform_coord(double out_coord[], 
                     double transform[WORLD_NDIMS][WORLD_NDIMS+1],
                     double in_coord[]);
void print_com(Stats_Info *stats);
int get_double_list(char *dst, char *key, char *nextarg);
void verify_range_options(Double_Array *min, Double_Array *max, 
                          Double_Array *range, Double_Array *binvalue);
void init_stats(Stats_Info *stats, int hist_bins);
void free_stats(Stats_Info *stats);

/* Argument variables */
int max_buffer_size_in_kb = 4 * 1024;

static int    verbose        = FALSE;
static int    quiet          = FALSE;
static int    clobber        = FALSE;
static int    ignoreNaN      = DEFAULT_BOOLEAN;
static double fillvalue      = -DBL_MAX;

static int    All            = FALSE;
static int    Vol_Count      = FALSE;
static int    Vol_Per        = FALSE;
static int    Vol            = FALSE;
static int    Min            = FALSE;
static int    Max            = FALSE;
static int    Sum            = FALSE;
static int    Sum2           = FALSE;
static int    Mean           = FALSE;
static int    Variance       = FALSE;
static int    Stddev         = FALSE;
static int    CoM            = FALSE;
static int    World_Only     = FALSE;

static int    Hist           = FALSE;
static int    Hist_Count     = FALSE;
static int    Hist_Per       = FALSE;
static int    Median         = FALSE;
static int    Majority       = FALSE;
static int    BiModalT       = FALSE;
static int    PctT           = FALSE;
static double pctT           = 0.0;
static int    Entropy        = FALSE;
  
static Double_Array vol_min = {0, NULL};
static Double_Array vol_max = {0, NULL};
static Double_Array vol_range = {0, NULL};
static Double_Array vol_binvalue = {0, NULL};
static int          num_ranges;

char          *mask_file;
static Double_Array mask_min = {0, NULL};
static Double_Array mask_max = {0, NULL};
static Double_Array mask_range = {0, NULL};
static Double_Array mask_binvalue = {0, NULL};
static int          num_masks;

char          *hist_file;
static int     hist_bins     = BINS_DEFAULT;
static double  hist_sep;
static double  hist_range[2] = {-DBL_MAX, DBL_MAX};
static int     discrete_histogram = FALSE;
static int     integer_histogram = FALSE;
static int     max_bins      = 10000;
  
/* Global Variables to store info for stats */
Stats_Info **stats_info = NULL;
double voxel_volume;
double nvoxels;
int    space_to_dim[WORLD_NDIMS] = {-1, -1, -1};
int    dim_to_space[MAX_VAR_DIMS];
int    file_ndims = 0;


/* Argument table - no I don't like 80 column format, we have 21" monitors so why not use them? */
ArgvInfo argTable[] = {
  {NULL,          ARGV_HELP,    (char*)NULL, (char*)NULL,       "General options:"                          },
  {"-verbose",    ARGV_CONSTANT,(char*)TRUE, (char*)&verbose,   "Print out extra information."              },
  {"-quiet",      ARGV_CONSTANT,(char*)TRUE, (char*)&quiet,     "Print requested values only."              },
  {"-clobber",    ARGV_CONSTANT,(char*)TRUE,  (char*)&clobber,   "Clobber existing files."                   },
  {"-noclobber",  ARGV_CONSTANT,(char*)FALSE, (char*)&clobber,   "Do not clobber existing files (default)."                   },
  {"-max_buffer_size_in_kb",
                  ARGV_INT,     (char*)1,    (char*)&max_buffer_size_in_kb,
                                                                "maximum size of internal buffers."         },
 
  {NULL,          ARGV_HELP,    (char*)NULL, (char*)NULL,       "\nVoxel selection options:"                },
  {"-floor", ARGV_FUNC, (char *) get_double_list, (char *) &vol_min,
   "Ignore voxels below this value (list)"},
  {"-ceil", ARGV_FUNC, (char *) get_double_list, (char *) &vol_max,
   "Ignore voxels above this value (list)"},
  {"-range", ARGV_FUNC, (char *) get_double_list, (char *) &vol_range,
   "Ignore voxels outside this range (list)"},
  {"-binvalue", ARGV_FUNC, (char *) get_double_list, (char *) &vol_binvalue,
   "Include voxels within 0.5 of this value (list)"},
  {"-mask",       ARGV_STRING,  (char*)1,    (char*)&mask_file, "<mask.mnc> Use mask file for calculations."},
  {"-mask_floor", ARGV_FUNC, (char *) get_double_list, (char *) &mask_min,
   "Exclude mask voxels below this value (list)"},
  {"-mask_ceil", ARGV_FUNC, (char *) get_double_list, (char *) &mask_max,
   "Exclude mask voxels above this value (list)"},
  {"-mask_range", ARGV_FUNC, (char *) get_double_list, (char *) &mask_range,
   "Exclude voxels outside this range (list)"},
  {"-mask_binvalue", ARGV_FUNC, (char *) get_double_list, (char *) &mask_binvalue,
   "Include mask voxels within 0.5 of this value (list)"},
  {"-ignore_nan", ARGV_CONSTANT,(char*)TRUE, (char*)&ignoreNaN, "Exclude NaN values from stats (default)."       },
  {"-include_nan",ARGV_CONSTANT,(char*)FALSE,(char*)&ignoreNaN, "Treat NaN values as zero."       },
  {"-replace_nan",ARGV_FLOAT,(char*)1,(char*)&fillvalue, "Replace NaNs with specified value."       },
  
  {NULL,          ARGV_HELP,    (char*)NULL, (char*)NULL,       "\nHistogram Options:"                      },
  {"-histogram",  ARGV_STRING,  (char*)1,    (char*)&hist_file, "<hist_file> Compute histogram."            },
  {"-hist_bins",  ARGV_INT,     (char*)1,    (char*)&hist_bins, "<number> of bins in each histogram."       },
  {"-bins",       ARGV_INT,     (char*)1,    (char*)&hist_bins, "synonym for -hist_bins."                   },
  {"-hist_floor", ARGV_FLOAT,   (char*)1,    (char*)&hist_range[0],"Histogram floor value. (incl)"             },
  {"-hist_ceil",  ARGV_FLOAT,   (char*)1,    (char*)&hist_range[1], "Histogram ceiling value. (incl)"           },
  {"-hist_range", ARGV_FLOAT,   (char*)2,    (char*)&hist_range,"Histogram floor and ceiling. (incl)"       },
  {"-discrete_histogram", ARGV_CONSTANT,   (char*)TRUE,    (char*)&discrete_histogram,"Match histogram bins to data discretization"       },
  {"-integer_histogram", ARGV_CONSTANT,   (char*)TRUE,    (char*)&integer_histogram,"Set histogram bins to unit width"       },
  {"-int_max_bins", ARGV_INT,   (char*)1,    (char*)&max_bins,"Set maximum number of histogram bins for integer histograms"       },

  {NULL,          ARGV_HELP,    (char*)NULL, (char*)NULL,       "\nStatistics (Printed in this order)"      },
  {"-all",        ARGV_CONSTANT,(char*)TRUE, (char*)&All,       "all statistics (default)."                 },
  {"-none",       ARGV_CONSTANT,(char*)TRUE, (char*)&Vol_Count, "synonym for -count. (from volume_stats)"   },
  {"-count",      ARGV_CONSTANT,(char*)TRUE, (char*)&Vol_Count, "# of voxels."                              },
  {"-percent",    ARGV_CONSTANT,(char*)TRUE, (char*)&Vol_Per,   "percentage of valid voxels."               },
  {"-volume",     ARGV_CONSTANT,(char*)TRUE, (char*)&Vol,       "volume (in mm3)."                          },
  {"-min",        ARGV_CONSTANT,(char*)TRUE, (char*)&Min,       "minimum value."                            },
  {"-max",        ARGV_CONSTANT,(char*)TRUE, (char*)&Max,       "maximum value."                            },
  {"-sum",        ARGV_CONSTANT,(char*)TRUE, (char*)&Sum,       "sum."                                      },
  {"-sum2",       ARGV_CONSTANT,(char*)TRUE, (char*)&Sum2,      "sum of squares."                           },
  {"-mean",       ARGV_CONSTANT,(char*)TRUE, (char*)&Mean,      "mean value."                               },
  {"-variance",   ARGV_CONSTANT,(char*)TRUE, (char*)&Variance,  "variance."                                 },
  {"-stddev",     ARGV_CONSTANT,(char*)TRUE, (char*)&Stddev,    "standard deviation."                       },
  {"-CoM",        ARGV_CONSTANT,(char*)TRUE, (char*)&CoM,       "centre of mass of the volume."                    },
  {"-com",        ARGV_CONSTANT,(char*)TRUE, (char*)&CoM,       "centre of mass of the volume."                    },
  {"-world_only", ARGV_CONSTANT,(char*)TRUE, (char*)&World_Only,"print CoM in world coords only."                    },
  
  {NULL,          ARGV_HELP,    (char*)NULL, (char*)NULL,       "\nHistogram Dependant Statistics:"         },
  {"-hist_count", ARGV_CONSTANT,(char*)TRUE, (char*)&Hist_Count,"# of voxels portrayed in Histogram."       },
  {"-hist_percent",
                  ARGV_CONSTANT,(char*)TRUE, (char*)&Hist_Per,  "percentage of histogram voxels."           },
  {"-median",     ARGV_CONSTANT,(char*)TRUE, (char*)&Median,    "median value."                             },
  {"-majority",   ARGV_CONSTANT,(char*)TRUE, (char*)&Majority,  "most frequently occurring histogram bin."  },
  {"-biModalT",   ARGV_CONSTANT,(char*)TRUE, (char*)&BiModalT,  "value separating a volume into 2 classes." },
  {"-pctT",       ARGV_FLOAT,   (char*)1,    (char*)&pctT,      "<%> threshold at the supplied % of data."  },
  {"-entropy",    ARGV_CONSTANT,(char*)TRUE, (char*)&Entropy,   "entropy of the volume."                    },
  
  {NULL,          ARGV_HELP,    (char*)NULL, (char*)NULL,       "\nOther Statistics:"                       },
  
  {NULL, ARGV_HELP,NULL, NULL, ""},
  {NULL, ARGV_END, NULL, NULL, NULL}
  };


int main(int argc, char *argv[])
{
   char          **infiles;
   int             nfiles;
   Loop_Options   *loop_options;
   int            mincid, imgid;
   int            idim;
   int            irange, imask;
   double         real_range[2], valid_range[2];
   nc_type        datatype;
   int            is_signed;
   double         voxel_to_world[WORLD_NDIMS][WORLD_NDIMS+1];
   Stats_Info     *stats;
   FILE           *FP;
   double         scale, voxmin, voxmax;

   /* Get arguments */
   if (ParseArgv(&argc, argv, argTable, 0) || (argc != 2)) {
      (void) fprintf(stderr, "\nUsage: %s [options] <infile.mnc>\n", argv[0]);
      (void) fprintf(stderr,   "       %s -help\n\n", argv[0]);
      exit(EXIT_FAILURE);
   }
   nfiles = argc - 1;
   infiles = &argv[1];
   infiles[1] = &mask_file[0];
   
   if (infiles[1] != NULL) {
      nfiles++;
   }

   /* Check for NaN options */
   if (ignoreNaN == DEFAULT_BOOLEAN) {
      ignoreNaN = (fillvalue != -DBL_MAX);
   }
   if (ignoreNaN && fillvalue == -DBL_MAX) {
      fillvalue = 0.0;
   }

   /* Check range options: not over-specified and put values 
      in vol_min/vol_max */
   verify_range_options(&vol_min, &vol_max, &vol_range, &vol_binvalue);
   num_ranges = vol_min.numvalues;

   /* Check mask range options: not over-specified and put values 
      in mask_min/mask_max */
   verify_range_options(&mask_min, &mask_max, &mask_range, &mask_binvalue);
   num_masks = mask_min.numvalues;

   /* Check histogramming options */
   if ((discrete_histogram && integer_histogram) ||
       ((discrete_histogram || integer_histogram) && 
        (hist_bins != BINS_DEFAULT))) {
      (void) fprintf(stderr, "Please specify only one of -discrete_histogram, -integer_histogram or -bins\n");
      exit(EXIT_FAILURE);
   }

   /* init PctT boolean before checking */
   if (pctT > 0.0) {
      PctT = TRUE;
      pctT /= 100;
   }

   /* if nothing selected, do everything */
   if (!Vol_Count && !Vol_Per && !Vol && !Min && !Max && !Sum && !Sum2 && 
       !Mean && !Variance && !Stddev && !Hist_Count && !Hist_Per && 
       !Median && !Majority && !BiModalT && !PctT && !Entropy && !CoM) {
      All = TRUE;
      Hist = TRUE;
   }
      
   if ((hist_file != NULL) || Hist_Count || Hist_Per || 
       Median || Majority || BiModalT || PctT || Entropy) {
      Hist = TRUE;
   }
   if (hist_bins <= 0) Hist = FALSE;
      
   /* do checking on arguments */
   if (hist_bins < 1) {
      (void) fprintf(stderr, "%s: Must have one or more bins for a histogram\n", argv[0]);
      exit(EXIT_FAILURE);
   }
   
   if (access(infiles[0], F_OK) != 0 ) {
      (void) fprintf(stderr, "%s: Couldn't find %s\n", argv[0], infiles[0]);
      exit(EXIT_FAILURE);
   }
   
   if (infiles[1] != NULL && access(infiles[1], F_OK) != 0 ) {
      (void) fprintf(stderr, "%s: Couldn't find mask file: %s\n", argv[0], infiles[1]);
      exit(EXIT_FAILURE);
   }
   
   if (hist_file != NULL && !clobber && access(hist_file, F_OK) != -1 ) {
      (void) fprintf(stderr, "%s: Histogram %s exists! (use -clobber to overwrite)\n", argv[0], hist_file);
      exit(EXIT_FAILURE);
   }

   /* Open the file to get some information */
   mincid = miopen(infiles[0], NC_NOWRITE);
   imgid = ncvarid(mincid, MIimage);
   nvoxels = get_minc_nvoxels(mincid);
   voxel_volume = get_minc_voxel_volume(mincid);
   (void) miget_datatype(mincid, imgid, &datatype, &is_signed);
   (void) miget_image_range(mincid, real_range);
   (void) miget_valid_range(mincid, imgid, valid_range);
   file_ndims = get_minc_ndims(mincid);
   find_minc_spatial_dims(mincid, space_to_dim, dim_to_space);
   get_minc_voxel_to_world(mincid, voxel_to_world);

   /* Check whether discrete histogramming makes sense - i.e. not
      floating-point. Silently ignore the option if it does not make sense. */
   if (datatype == NC_FLOAT || datatype == NC_DOUBLE) {
      discrete_histogram = FALSE;
   }
   
   /* set up the histogram definition, if needed */
   if (Hist) {

      if (hist_range[0] == -DBL_MAX) { 
         if (vol_min.numvalues == 1 && vol_min.values[0] != -DBL_MAX) 
            hist_range[0] = vol_min.values[0];
         else
            hist_range[0] = real_range[0];
      }
         
      if (hist_range[1] == DBL_MAX) { 
         if (vol_max.numvalues == 1 && vol_max.values[0] != DBL_MAX) 
            hist_range[1] = vol_max.values[0];
         else
            hist_range[1] = real_range[1];
      }

      if (discrete_histogram) {

         /* Convert histogram range to voxel values and round, then
            convert back. */
         scale = (real_range[1] == real_range[0]) ? 0.0 :
            (valid_range[1]-valid_range[0]) / (real_range[1]-real_range[0]);
         voxmin = rint((hist_range[0] - real_range[0])*scale + valid_range[0]);
         voxmax = rint((hist_range[1] - real_range[0])*scale + valid_range[0]);
         if (real_range[1] != real_range[0])
            scale = 1.0/scale;
         hist_range[0] = (voxmin - valid_range[0]) * scale + real_range[0];
         hist_range[1] = (voxmax - valid_range[0]) * scale + real_range[0];

         /* Figure out number of bins and bin width */
         hist_bins = voxmax - voxmin;
         if (hist_bins <= 0) {
            hist_sep = 1.0;
            hist_bins = 0;
         }
         else {
            hist_sep = (hist_range[1] - hist_range[0])/hist_bins;
         }

         /* Shift the ends of the histogram down and up by half a bin
            and add one to the number of bins */
         hist_range[0] -= hist_sep/2.0;
         hist_range[1] += hist_sep/2.0;
         hist_bins++;
      }
      else if (integer_histogram) {

         /* Add and subtract the 0.01 in order to ensure that a range that
            is already properly specified stays that way. Ie. [-0.5,255.5]
            does not change, regardless of the type of rounding done to .5 */
         hist_range[0] = (int) rint(hist_range[0] + 0.01);
         hist_range[1] = (int) rint(hist_range[1] - 0.01);
         hist_bins = hist_range[1] - hist_range[0] + 1.0;
         hist_range[0] -= 0.5;
         hist_range[1] += 0.5;
         hist_sep = 1.0;
      }
      else {
         hist_sep = (hist_range[1] - hist_range[0])/hist_bins;
      }

      if ((discrete_histogram || integer_histogram) &&
          (hist_bins > max_bins)) {
         (void) fprintf(stderr, "Too many bins in histogram (%d) - please increase -max_bins if appropriate\n", hist_bins);
         exit(EXIT_FAILURE);
      }
      
   }
   
   /* Initialize the stats structure */
   stats_info = MALLOC(num_ranges * sizeof(*stats_info));
   for (irange=0; irange < num_ranges; irange++) {
      stats_info[irange] = MALLOC(num_masks * sizeof(**stats_info));
      for (imask=0; imask < num_masks; imask++) {
         stats = &stats_info[irange][imask];
         init_stats(stats, hist_bins);
         stats->vol_range[0] = vol_min.values[irange];
         stats->vol_range[1] = vol_max.values[irange];
         stats->mask_range[0] = mask_min.values[imask];
         stats->mask_range[1] = mask_max.values[imask];
      }
   }

   /* Do math */
   loop_options = create_loop_options();
   set_loop_first_input_mincid(loop_options, mincid);
   set_loop_verbose(loop_options, verbose);
   set_loop_buffer_size(loop_options, (long)1024 * max_buffer_size_in_kb);
   voxel_loop(nfiles, infiles, 0, NULL, NULL, 
              loop_options, do_math, NULL);
   free_loop_options(loop_options);

   /* Open the histogram file if it will be needed */
   if (hist_file == NULL) {
      FP = NULL;
   }
   else {
      FP = fopen(hist_file, "w");
      if (FP == NULL) {
         perror("Error opening histogram file");
         exit(EXIT_FAILURE);
      }
   }

   /* Loop over ranges and masks, calculating results */
   for (irange=0; irange < num_ranges; irange++) {
      for (imask=0; imask < num_masks; imask++) {

         stats = &stats_info[irange][imask];

         stats->vol_per  = stats->vvoxels / nvoxels * 100;
         stats->hist_per = stats->hvoxels / nvoxels * 100;
         stats->mean     = 
            (stats->vvoxels > 0) ? stats->sum/stats->vvoxels : 0.0;
         stats->variance = 
            (stats->vvoxels > 1) ? 
            (stats->sum2 - SQR(stats->sum)/stats->vvoxels)/(stats->vvoxels-1) 
            : 0.0;
         stats->stddev   = sqrt(stats->variance);
         stats->volume  = voxel_volume * stats->vvoxels;
         for (idim=0; idim < WORLD_NDIMS; idim++) {
            if (stats->sum != 0.0) 
               stats->voxel_com[idim] = 
                  stats->voxel_com_sum[idim] / stats->sum;
            else
               stats->voxel_com[idim] = 0.0;
         }
         transform_coord(stats->world_com, voxel_to_world, stats->voxel_com);
   
         /* Do the histogram calculations */
         if (Hist) {
            int    c;
            float  *hist_centre;
            float  *pdf;                 /* probability density Function */
            float  *cdf;                 /* cumulative density Function  */
      
            int    majority_bin = 0;
            int    median_bin   = 0;
            int    pctt_bin     = 0;
            int    bimodalt_bin = 0;

            /* BiModal Threshold variables*/
            double zero_moment  = 0.0;
            double first_moment = 0.0;
            double var          = 0.0;
            double max_var      = 0.0;

            /* Allocate space for histograms */
            hist_centre = CALLOC(hist_bins, sizeof(float));
            pdf =         CALLOC(hist_bins, sizeof(float));
            cdf =         CALLOC(hist_bins, sizeof(float));
            if (hist_centre == NULL || pdf == NULL || cdf == NULL) {
               (void) fprintf(stderr, "Memory allocation error\n");
               exit(EXIT_FAILURE);
            }

            for (c=0; c < hist_bins; c++) {
               hist_centre[c] = (c*hist_sep) + hist_range[0] + (hist_sep/2);
               pdf[c] = (stats->hvoxels > 0) ? 
                  stats->histogram[c]/stats->hvoxels : 0.0;
               if (c == 0) { cdf[c] = pdf[c];            }
               else        { cdf[c] = cdf[c-1] + pdf[c]; }

               /* Majority */
               if (stats->histogram[c] > stats->histogram[majority_bin]) {
                  majority_bin = c; 
               }
         
               /* Entropy */
               if (stats->histogram[c] > 0.0) {
                  stats->entropy -= pdf[c] * (log(pdf[c])/log(2.0));
               }
         
               /* Histogram Median */
               if (cdf[c] < 0.5) {
                  median_bin = c;
               }
         
               /* BiModal Threshold */
               if (c > 0) {
                  zero_moment  += pdf[c];
                  first_moment += hist_centre[c]*pdf[c];
            
                  var = SQR((stats->mean * zero_moment) - first_moment) / 
                     (zero_moment * (1 - zero_moment));

                  if (var > max_var) {
                     bimodalt_bin = c;
                     max_var = var;
                  }
               }
         
               /* pct Threshold */
               if (cdf[c] < pctT) {
                  pctt_bin = c;
               }
            }
         
            /* median */
            if (median_bin == 0)
               stats->median = 0.5 * pdf[median_bin] *  hist_sep;
            else
               stats->median = ((double)median_bin + (0.5 - cdf[median_bin]) 
                                * pdf[median_bin + 1]) *  hist_sep;
      
            stats->majority = hist_centre[majority_bin];
            stats->biModalT = hist_centre[bimodalt_bin];
      
            /* pct Threshold */
            if (pctt_bin == 0)
               stats->pct_T = pctT * pdf[pctt_bin] *  hist_sep;
            else
               stats->pct_T = ((double)pctt_bin + (pctT - cdf[pctt_bin]) 
                               * pdf[pctt_bin + 1]) *  hist_sep;
      
            /* output the histogram */
            if (hist_file != NULL) {
         
               (void) fprintf(FP, "# histogram for: %s\n",     infiles[0]);
               (void) fprintf(FP, "#  mask file:    %s\n",     infiles[1]);
               if (stats->vol_range[0] != -DBL_MAX ||
                   stats->vol_range[1] !=  DBL_MAX) {
                  (void) fprintf(FP, "#  volume range: %g  %g\n", stats->vol_range[0], stats->vol_range[1]);
               }
               if (stats->mask_range[0] != -DBL_MAX ||
                   stats->mask_range[1] !=  DBL_MAX) {
                  (void) fprintf(FP, "#  mask range:   %g  %g\n", stats->mask_range[0], stats->mask_range[1]);
               }
               (void) fprintf(FP, "#  domain:       %g  %g\n", hist_range[0], hist_range[1]);
               (void) fprintf(FP, "#  entropy:      %g\n",     stats->entropy);;
               (void) fprintf(FP, "# bin centres                 counts\n");
               for (c=0; c < hist_bins; c++)
                  (void) fprintf(FP, "  %-20.10g  %12g\n", hist_centre[c], stats->histogram[c]);
               (void) fprintf(FP, "\n");
            }

            /* Free the space */
            FREE(hist_centre);
            FREE(pdf);
            FREE(cdf);

         }  /* end histogram calculations */

         /* Print range of data allowed */
         if (verbose || (num_ranges > 1 && !quiet)) {
            (void) fprintf(stdout, "Included Range:    %g   %g\n", stats->vol_range[0], stats->vol_range[1]);
         }
         if (verbose || (num_masks > 1 && !quiet)) {
            (void) fprintf(stdout, "Mask Range:        %g   %g\n", stats->mask_range[0], stats->mask_range[1]);
         }

         /* Print warnings about ranges */
         if (!quiet && real_range[0] != stats->min && 
             stats->vol_range[0] == -DBL_MAX &&
             stats->mask_range[0] == -DBL_MAX) { 
            (void) fprintf(stderr, "*** %s - reported min (%g) doesn't equal header (%g)\n", argv[0], stats->min, real_range[0]);
         } 
         if (!quiet && real_range[1] != stats->max && 
             stats->vol_range[1] == DBL_MAX && 
             stats->mask_range[1] == DBL_MAX) { 
            (void) fprintf(stderr, "*** %s - reported max (%g) doesn't equal header (%g)\n", argv[0], stats->max, real_range[1]);
         }

         /* Output stats */
         if (Hist) {
            if (verbose) {
               (void) fprintf(stdout, "Histogram Range:   %g\t%g\n", hist_range[0], hist_range[1]);
               (void) fprintf(stdout, "Histogram bins:    %i  of Width (separation): %f\n", hist_bins, hist_sep);
            }
         }
      
         if (All && !quiet)       { (void) fprintf(stdout, "File:              %s\n", infiles[0]);}
         if (All && !quiet)       { (void) fprintf(stdout, "Mask file:         %s\n", infiles[1]);}
         if (All && !quiet)       {    print_result("Total voxels:      ", nvoxels  );     }
         if (All || Vol_Count)    {    print_result("# voxels:          ", stats->vvoxels  );     }
         if (All || Vol_Per)      {    print_result("% of total:        ", stats->vol_per  );     }
         if (All || Vol)          {    print_result("Volume (mm3):      ", stats->volume   );     }
         if (All || Min)          {    print_result("Min:               ", stats->min      );     }
         if (All || Max)          {    print_result("Max:               ", stats->max      );     }
         if (All || Sum)          {    print_result("Sum:               ", stats->sum      );     }
         if (All || Sum2)         {    print_result("Sum^2:             ", stats->sum2     );     }
         if (All || Mean)         {    print_result("Mean:              ", stats->mean     );     }
         if (All || Variance)     {    print_result("Variance:          ", stats->variance );     }
         if (All || Stddev)       {    print_result("Stddev:            ", stats->stddev   );     }
         if (All || CoM)          {
            print_com(stats);
         }
   
         if (Hist){
            if (All && !quiet)    {(void) fprintf(stdout,"\nHistogram:         %s\n", hist_file); }
            if (All && !quiet)    {    print_result("Total voxels:      ", nvoxels  );     }
            if (All || Hist_Count){    print_result("# voxels:          ", stats->hvoxels  );     }
            if (All || Hist_Per)  {    print_result("% of total:        ", stats->hist_per );     }
            if (All || Median)    {    print_result("Median:            ", stats->median   );     }
            if (All || Majority)  {    print_result("Majority:          ", stats->majority );     }
            if (All || BiModalT)  {    print_result("BiModalT:          ", stats->biModalT );     }
            if (All || PctT)      {    
               char str[100];
               (void) sprintf(str, "PctT [%3d%%]:       ", (int)(pctT * 100));
               print_result(str,              stats->pct_T);
            }
            if (All || Entropy)   {    print_result("Entropy :          ", stats->entropy  );     }
            if (!quiet) { (void) fprintf(stdout, "\n"); }
         }

      }   /* End of loop over masks */

   }   /* End of loop over ranges */

   /* Close the histogram file */
   if (FP != NULL)
     (void) fclose(FP);

   /* Free things up */
   for (irange=0; irange < num_ranges; irange++) {
      for (imask=0; imask < num_masks; imask++) {
         free_stats(&stats_info[irange][imask]);
      }
      FREE(stats_info[irange]);
   }
   FREE(stats_info);

   return EXIT_SUCCESS;
}

void do_math(void *caller_data, long num_voxels, 
             int input_num_buffers, int input_vector_length,
             double *input_data[], 
             int output_num_buffers, int output_vector_length,
             double *output_data[], Loop_Info *loop_info)
/* ARGSUSED */
{
   long ivox;
   long index[MAX_VAR_DIMS];
   int imask, irange;
   double mask_min, mask_max;
   Stats_Info *stats;

   /* Loop through the voxels - a bit of optimization in case we 
      have a brain-dead compiler*/
   if (mask_file != NULL) {
      for (irange=0; irange < num_ranges; irange++) {
         for (imask=0; imask < num_masks; imask++) {
            stats = &stats_info[irange][imask];
            mask_min = stats->mask_range[0];
            mask_max = stats->mask_range[1];
            if (CoM || All) {
               for (ivox=0; ivox < num_voxels*input_vector_length; ivox++) {
                  if ((input_data[1][ivox] >= mask_min) && 
                      (input_data[1][ivox] <= mask_max)) {
                     get_info_voxel_index(loop_info, ivox, file_ndims, index);
                     do_stats(input_data[0][ivox], index, stats);
                  }
               }
            }
            else {
               for (ivox=0; ivox < num_voxels*input_vector_length; ivox++) {
                  if ((input_data[1][ivox] >= mask_min) && 
                      (input_data[1][ivox] <= mask_max)) {
                     do_stats(input_data[0][ivox], NULL, stats);
                  }
               }
            }
         }
      }
   }
      
   else {
      for (irange=0; irange < num_ranges; irange++) {
         stats = &stats_info[irange][0];
         if (CoM || All) {
            for (ivox=0; ivox < num_voxels*input_vector_length; ivox++) {
               get_info_voxel_index(loop_info, ivox, file_ndims, index);
               do_stats(input_data[0][ivox], index, stats);
            }
         }
         else {
            for (ivox=0; ivox < num_voxels*input_vector_length; ivox++) {
               do_stats(input_data[0][ivox], NULL, stats);
            }
         }
      }
   }
      
   return;
}
   
void do_stats(double value, long index[], Stats_Info *stats)
{
   int idim;
   int hist_index, dim_index;

   /* Check for NaNs */
   if (value == -DBL_MAX) {
      if (ignoreNaN)
         value = fillvalue;
      else
         return;
   }

   /* Collect stats if we are within range */
   if ((value >= stats->vol_range[0]) && (value <= stats->vol_range[1])) {
      stats->vvoxels++;
      stats->sum  += value;
      stats->sum2 += SQR(value);

      if (value < stats->min) { stats->min = value; }
      if (value > stats->max) { stats->max = value; }

      /* Get voxel index */
      if (CoM || All) {
         for (idim=0; idim < WORLD_NDIMS; idim++) {
            dim_index = space_to_dim[idim];
            if (dim_index >= 0) {
               stats->voxel_com_sum[idim] += value * index[dim_index];
            }
         }
      }
      
      if (Hist && (value >= hist_range[0]) && (value <= hist_range[1])) {
         /*lower limit <= value < upper limit */
         hist_index = (int)floor((value - hist_range[0])/hist_sep);
         if (hist_index >= hist_bins) hist_index = hist_bins-1;
         stats->histogram[hist_index]++;
         stats->hvoxels++;
      }
   }
}
   
void print_result(char* title, double result)
{
   if (!quiet){ (void) fprintf(stdout, "%s", title); }
   (void) fprintf(stdout, "%.10g\n", result);
}

/* Get the number of voxels in the file - this is the total number,
   not just spatial dimensions */
long get_minc_nvoxels(int mincid)
{
   int imgid, dim[MAX_VAR_DIMS];
   int idim, ndims;
   long nvoxels, length;

   /* Get the dimension ids for the image variable */
   imgid = ncvarid(mincid, MIimage);
   (void) ncvarinq(mincid, imgid, NULL, NULL, &ndims, dim, NULL);

   /* Loop over them to get the total number of voxels */
   nvoxels = 1;
   for (idim=0; idim < ndims; idim++) {
      (void) ncdiminq(mincid, dim[idim], NULL, &length);
      nvoxels *= length;
   }

   return nvoxels;
}

/* Get the volume of a spatial voxel */
double get_minc_voxel_volume(int mincid)
{
   int imgid, dim[MAX_VAR_DIMS];
   int idim, ndims;
   double volume, step;
   char dimname[MAX_NC_NAME];

   /* Get the dimension ids for the image variable */
   imgid = ncvarid(mincid, MIimage);
   (void) ncvarinq(mincid, imgid, NULL, NULL, &ndims, dim, NULL);

   /* Loop over them to get the total spatial volume */
   volume = 1.0;
   for (idim=0; idim < ndims; idim++) {

      /* Get the name and check that this is a spatial dimension */
      (void) ncdiminq(mincid, dim[idim], dimname, NULL);
      if ((dimname[0] == '\0') ||
          (strcmp(&dimname[1], "space") != 0) ||
          !(dimname[0] == 'x' || dimname[0] == 'y' || dimname[0] == 'z')) {
         continue;
      }

      /* Get the step attribute of the coordinate variable */
      step = 1.0;
      get_minc_attribute(mincid, dimname, MIstep, 1, &step);

      /* Make sure that it is positive and calculate the volume */
      if (step < 0.0) step = -step;
      volume *= step;
   }

   return volume;
}

/* Get a double attribute from a minc file */
void get_minc_attribute(int mincid, char *varname, char *attname, 
                        int maxvals, double vals[])
{
   int varid;
   int old_ncopts;
   int att_length;

   if (!mivar_exists(mincid, varname)) return;
   varid = ncvarid(mincid, varname);
   old_ncopts = ncopts; ncopts = 0;
   (void) miattget(mincid, varid, attname, NC_DOUBLE, maxvals, vals, 
                   &att_length);
   ncopts = old_ncopts;
}

/* Get the total number of image dimensions in a minc file */
int get_minc_ndims(int mincid)
{
   int imgid;
   int ndims;

   imgid = ncvarid(mincid, MIimage);
   (void) ncvarinq(mincid, imgid, NULL, NULL, &ndims, NULL, NULL);

   return ndims;
}

/* Get the mapping from spatial dimension - x, y, z - to file dimensions
   and vice-versa. */
void find_minc_spatial_dims(int mincid, int space_to_dim[], int dim_to_space[])
{
   int imgid, dim[MAX_VAR_DIMS];
   int idim, ndims, world_index;
   char dimname[MAX_NC_NAME];

   /* Set default values */
   for (idim=0; idim < 3; idim++)
      space_to_dim[idim] = -1;
   for (idim=0; idim < MAX_VAR_DIMS; idim++)
      dim_to_space[idim] = -1;

   /* Get the dimension ids for the image variable */
   imgid = ncvarid(mincid, MIimage);
   (void) ncvarinq(mincid, imgid, NULL, NULL, &ndims, dim, NULL);

   /* Loop over them to find the spatial ones */
   for (idim=0; idim < ndims; idim++) {

      /* Get the name and check that this is a spatial dimension */
      (void) ncdiminq(mincid, dim[idim], dimname, NULL);
      if ((dimname[0] == '\0') || (strcmp(&dimname[1], "space") != 0)) {
         continue;
      }

      /* Look for the spatial dimensions*/
      switch (dimname[0]) {
         case 'x':
            world_index = 0;
            break;
         case 'y':
            world_index = 1;
            break;
         case 'z':
            world_index = 2;
            break;
         default:
            world_index = 0;
            break;
      }
      space_to_dim[world_index] = idim;
      dim_to_space[idim] = world_index;
   }
}

/* Get the voxel to world transform (for column vectors) */
void get_minc_voxel_to_world(int mincid, 
                             double voxel_to_world[WORLD_NDIMS][WORLD_NDIMS+1])
{
   int idim, jdim;
   double dircos[WORLD_NDIMS];
   double step, start;
   char *dimensions[] = {MIxspace, MIyspace, MIzspace};

   /* Zero the matrix */
   for (idim=0; idim < WORLD_NDIMS; idim++) {
      for (jdim=0; jdim < WORLD_NDIMS+1; jdim++) 
         voxel_to_world[idim][jdim] = 0.0;
   }

   for (jdim=0; jdim < WORLD_NDIMS; jdim++) {

      /* Set default values */
      step = 1.0;
      start = 0.0;
      for (idim=0; idim < WORLD_NDIMS; idim++)
         dircos[idim] = 0.0;
      dircos[jdim] = 1.0;

      /* Get the attributes */
      get_minc_attribute(mincid, dimensions[jdim], MIstart, 1, &start);
      get_minc_attribute(mincid, dimensions[jdim], MIstep,  1, &step);
      get_minc_attribute(mincid, dimensions[jdim], MIdirection_cosines, 
                         WORLD_NDIMS, dircos);
      normalize_vector(dircos);

      /* Put them in the matrix */
      for (idim=0; idim < WORLD_NDIMS; idim++) {
         voxel_to_world[idim][jdim] = step * dircos[idim];
         voxel_to_world[idim][WORLD_NDIMS] +=  start * dircos[idim];
      }

   }

}

void normalize_vector(double vector[])
{
   int idim;
   double magnitude;
 
   magnitude = 0.0;
   for (idim=0; idim < WORLD_NDIMS; idim++) {
      magnitude += (vector[idim] * vector[idim]);
   }
   magnitude = sqrt(magnitude);
   if (magnitude > 0.0) {
      for (idim=0; idim < WORLD_NDIMS; idim++) {
         vector[idim] /= magnitude;
      }
   }
}

/* Prints out centre of mass with correct file order */
void print_com(Stats_Info *stats)
{
   char *spatial_codes[WORLD_NDIMS] = {"x", "y", "z"}; /* In x,y,z order */
   int idim;
   int first;

   /* Print out voxel coord info */
   if (!World_Only) {
      if (!quiet) {
         (void) fprintf(stdout, "CoM_voxel(");
         first = TRUE;
         for (idim=0; idim < MAX_VAR_DIMS; idim++) {
            if (dim_to_space[idim] >= 0) {
               if (first) first = FALSE;
               else (void) fprintf(stdout, ",");
               (void) fprintf(stdout, "%s", spatial_codes[dim_to_space[idim]]);
            }
         }
         (void) fprintf(stdout, "):  ");
      }
      first = TRUE;
      for (idim=0; idim < MAX_VAR_DIMS; idim++) {
         if (dim_to_space[idim] >= 0) {
            if (first) first = FALSE;
            else (void) fprintf(stdout, " ");
            (void) fprintf(stdout, "%.10g", stats->voxel_com[dim_to_space[idim]]);
         }
      }
      (void) fprintf(stdout, "\n");
   }

   /* Print out world coord info */
   if (!quiet) {
      (void) fprintf(stdout, "CoM_real(x,y,z):   ");
   }
   (void) fprintf(stdout, "%.10g %.10g %.10g\n", 
                  stats->world_com[0], 
                  stats->world_com[1], 
                  stats->world_com[2]);
}

/* Transforms a coordinate through a linear transform */
void transform_coord(double out_coord[], 
                     double transform[WORLD_NDIMS][WORLD_NDIMS+1],
                     double in_coord[])
{
   int idim, jdim;
   double homogeneous_coord[WORLD_NDIMS+1];

   for (idim=0; idim < WORLD_NDIMS; idim++) {
      homogeneous_coord[idim] = in_coord[idim];
   }
   homogeneous_coord[WORLD_NDIMS] = 1.0;

   for (idim=0; idim < WORLD_NDIMS; idim++) {
      out_coord[idim] = 0.0;
      for (jdim=0; jdim < WORLD_NDIMS+1; jdim++) {
         out_coord[idim] += transform[idim][jdim] * homogeneous_coord[jdim];
      }
   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_double_list
@INPUT      : dst - client data passed by ParseArgv
              key - matching key in argv
              nextarg - argument following key in argv
@OUTPUT     : (none)
@RETURNS    : TRUE since nextarg is used.
@DESCRIPTION: Gets a list (array) of double values.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : March 8, 1995 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
int get_double_list(char *dst, char *key, char *nextarg)
{
#define VECTOR_SEPARATOR ','

   int num_elements;
   int num_alloc;
   double *double_list;
   double dvalue;
   char *cur, *end, *prev;
   Double_Array *double_array;

   /* Check for a following argument */
   if (nextarg == NULL) {
      (void) fprintf(stderr, 
                     "\"%s\" option requires an additional argument\n",
                     key);
      exit(EXIT_FAILURE);
   }

   /* Get pointers to array variables */
   double_array = (Double_Array *) dst;

   /* Set up pointers to end of string and first non-space character */
   end = nextarg + strlen(nextarg);
   cur = nextarg;
   while (isspace(*cur)) cur++;
   num_elements = 0;
   num_alloc = 0;
   double_list = NULL;

   /* Loop through string looking for doubles */
   while (cur!=end) {

      /* Get double */
      prev = cur;
      dvalue = strtod(prev, &cur);
      if (cur == prev) {
         (void) fprintf(stderr, 
            "expected vector of doubles for \"%s\", but got \"%s\"\n", 
                        key, nextarg);
         exit(EXIT_FAILURE);
      }

      /* Add the value to the list */
      num_elements++;
      if (num_elements > num_alloc) {
         num_alloc += 20;
         if (double_list == NULL) {
            double_list = 
               MALLOC(num_alloc * sizeof(*double_list));
         }
         else {
            double_list = 
               REALLOC(double_list, num_alloc * sizeof(*double_list));
         }
      }
      double_list[num_elements-1] = dvalue;

      /* Skip any spaces */
      while (isspace(*cur)) cur++;

      /* Skip an optional comma */
      if (*cur == VECTOR_SEPARATOR) cur++;

   }

   /* Update the global variables */
   double_array->numvalues = num_elements;
   if (double_array->values != NULL) {
      FREE(double_array->values);
   }
   double_array->values = double_list;

   return TRUE;
}

/* Check range options and set appropriate values. At least one
   value will be set up for min and max. */
void verify_range_options(Double_Array *min, Double_Array *max, 
                          Double_Array *range, Double_Array *binvalue)
{
   int            overspecified = FALSE;
   int            min_defaults, max_defaults;
   int            num_values;
   int            ivalue;

   /* Check the min and max */
   if (min->numvalues != 0 && max->numvalues != 0 &&
       min->numvalues != max->numvalues) {
      (void) fprintf(stderr, "Number of floor ceil values differs\n");
      exit(EXIT_FAILURE);
   }
   num_values = min->numvalues;
   if (num_values == 0) num_values = max->numvalues;

   /* Check for range */
   if (range->numvalues > 0) {
      if (num_values == 0) num_values = range->numvalues/2;
      else overspecified = TRUE;
   }

   /* Check for binvalue */
   if (binvalue->numvalues > 0) {
      if (num_values == 0) num_values = binvalue->numvalues;
      else overspecified = TRUE;
   }

   /* Print an error if too many options have been given */
   if (overspecified) {
      (void) fprintf(stderr, "Set only one of floor/ceil, range or binvalue\n");
      exit(EXIT_FAILURE);
   }

   /* Double-check that we got the sizes right */
   if ((min->numvalues > 0 && min->numvalues != num_values) ||
       (max->numvalues > 0 && max->numvalues != num_values)) {
      (void) fprintf(stderr, "Problem with range specification\n");
      exit(EXIT_FAILURE);
   }

   /* Check if we are setting default values. Make sure that at least one
      value is set */
   if (num_values <= 0) {
      num_values = 1;
      min_defaults = max_defaults = TRUE;
   }
   else {
      min_defaults = (min->numvalues == 0 && max->numvalues > 0);
      max_defaults = (max->numvalues == 0 && min->numvalues > 0);
   }

   /* Allocate the space */
   if (min->numvalues <= 0) {
      min->numvalues = num_values;
      min->values = MALLOC(num_values * sizeof(double));
   }
   if (max->numvalues <= 0) {
      max->numvalues = num_values;
      max->values = MALLOC(num_values * sizeof(double));
   }
   if (min->values == NULL || max->values == NULL) {
      (void) fprintf(stderr, "Memory allocation error\n");
      exit(EXIT_FAILURE);
   }

   /* Set defaults, if needed */
   if (min_defaults) {
      for (ivalue=0; ivalue < num_values; ivalue++)
         min->values[ivalue] = -DBL_MAX;
   }
   if (max_defaults) {
      for (ivalue=0; ivalue < num_values; ivalue++)
         max->values[ivalue] = DBL_MAX;
   }

   /* Set min and max from range, if needed */
   if (range->numvalues > 0) {

      /* Check for odd number of values - they should be in pairs */
      if ((vol_range.numvalues % 2) != 0) {
         (void) fprintf(stderr, "Specify range values in pairs (even number)\n");
         exit(EXIT_FAILURE);
      }

      /* Loop over values */
      for (ivalue=0; ivalue*2+1 < range->numvalues; ivalue++) {
         min->values[ivalue] = range->values[ivalue*2];
         max->values[ivalue] = range->values[ivalue*2+1];
      }

   }

   /* Set min and max from binvalue, if needed */
   if (binvalue->numvalues > 0) {
      for (ivalue=0; ivalue < binvalue->numvalues; ivalue++) {
         min->values[ivalue] = binvalue->values[ivalue] - 0.5;
         max->values[ivalue] = binvalue->values[ivalue] + 0.5;
      }
   }

}

/* Initialiaze a Stats_Info structure */
void init_stats(Stats_Info *stats, int hist_bins)
{
   stats->vol_range[0] = -DBL_MAX;
   stats->vol_range[1] =  DBL_MAX;
   stats->mask_range[0] = -DBL_MAX;
   stats->mask_range[1] =  DBL_MAX;
   if (Hist && hist_bins > 0) {
      stats->histogram = CALLOC(hist_bins, sizeof(float));
      if (stats->histogram == NULL) {
         (void) fprintf(stderr, "Memory allocation error\n");
         exit(EXIT_FAILURE);
      }
   }
   else {
      stats->histogram = NULL;
   }
   stats->hvoxels          = 0.0;    /* number of voxels in histogram  */
   stats->vvoxels          = 0.0;    /* number of valid voxels         */
   stats->volume           = 0.0;
   stats->vol_per          = 0.0;
   stats->hist_per         = 0.0;
   stats->min              = DBL_MAX;
   stats->max              = -DBL_MAX;
   stats->sum              = 0.0; 
   stats->sum2             = 0.0;
   stats->mean             = 0.0;
   stats->variance         = 0.0;
   stats->stddev           = 0.0;
   stats->voxel_com_sum[0] = 0.0;
   stats->voxel_com_sum[1] = 0.0;
   stats->voxel_com_sum[2] = 0.0;
   stats->voxel_com[0]     = 0.0;
   stats->voxel_com[1]     = 0.0;
   stats->voxel_com[2]     = 0.0;
   stats->world_com[0]     = 0.0;
   stats->world_com[1]     = 0.0;
   stats->world_com[2]     = 0.0;
   stats->median           = 0.0;
   stats->majority         = 0.0;
   stats->biModalT         = 0.0;
   stats->pct_T            = 0.0;
   stats->entropy          = 0.0;
}

/* Free things from a Stats_Info structure */
void free_stats(Stats_Info *stats)
{
   if (stats->histogram != NULL)
      FREE(stats->histogram);
}
