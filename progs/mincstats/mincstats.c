/* mincstats.c
 *
 * Andrew Janke - rotor@cmr.uq.edu.au
 * Center for Magnetic Resonance
 * University of Queensland, Australia
 *
 * $Log: mincstats.c,v $
 * Revision 1.2  2001-11-28 21:59:39  neelin
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
 * Majority - This is the center of the largest bin in the histogram
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
#include <ParseArgv.h>
#include <minc_def.h>
#include <voxel_loop.h>

#ifndef TRUE
#  define TRUE  1
#  define FALSE 0
#endif

#define SQR(x)    ((x)*(x))

#define WORLD_NDIMS 3

/* Structure for window information */
typedef struct {
   int     num_constants;
   double  constants[2];
   int     propagate_nan;
   double  illegal_value;
} Math_Data;

/* Function prototypes */
void do_math(void *caller_data, long num_voxels, int input_num_buffers, int input_vector_length,
                    double *input_data[], int output_num_buffers, int output_vector_length,
                    double *output_data[], Loop_Info *loop_info);
void do_stats(double value, long index[]);
void print_result(char* title, double result);
long get_minc_nvoxels(int mincid);
double get_minc_voxel_volume(int mincid);
void get_minc_attribute(int mincid, char *varname, char *attname, 
                       int maxvals, double vals[]);
int get_minc_ndims(int mincid);
void find_minc_spatial_dims(int mincid, int space_to_dim[]);
void get_minc_voxel_to_world(int mincid, 
                             double voxel_to_world[WORLD_NDIMS][WORLD_NDIMS+1]);
void normalize_vector(double vector[]);

/* Argument variables */
int max_buffer_size_in_kb = 4 * 1024;

static int    verbose        = FALSE;
static int    quiet          = FALSE;
static int    clobber        = FALSE;
static int    ignoreNaN      = TRUE;

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

static int    Hist           = FALSE;
static int    Hist_Count     = FALSE;
static int    Hist_Per       = FALSE;
static int    Median         = FALSE;
static int    Majority       = FALSE;
static int    BiModalT       = FALSE;
static int    PctT           = FALSE;
static double pctT           = 0.0;
static int    Entropy        = FALSE;
  
static double  vol_range[2]  = {-DBL_MAX, DBL_MAX};

char          *mask_file;
static double  mask_range[2] = {-DBL_MAX, DBL_MAX};

float         *histogram;
char          *hist_file;
static int     hist_bins     = 2000;
static double  hist_sep;
static double  hist_range[2] = {-DBL_MAX, DBL_MAX};
  
/* Global Variables to store results of stats -                 */
double nvoxels  =  0.0;       /* number of voxels               */
double hvoxels  =  0.0;       /* number of voxels in histogram  */
double vvoxels  =  0.0;       /* number of valid voxels         */
double volume   =  0.0;
double vol_per  =  0.0;
double hist_per =  0.0;
double min      =  DBL_MAX;
double max      = -DBL_MAX;
double sum      =  0.0; 
double sum2     =  0.0;
double mean     =  0.0;
double variance =  0.0;
double stddev   =  0.0;
double com_sum[WORLD_NDIMS]      = {0.0, 0.0, 0.0};
double com[WORLD_NDIMS]          = {0.0, 0.0, 0.0};
int    space_to_dim[WORLD_NDIMS] = {-1, -1, -1};
int    file_ndims = 0;
double median   =  0.0;
double majority =  0.0;
double biModalT =  0.0;
double pct_T    =  0.0;
double entropy  =  0.0;


/* Argument table - no I don't like 80 column format, we have 21" monitors so why not use them? */
ArgvInfo argTable[] = {
  {NULL,          ARGV_HELP,    (char*)NULL, (char*)NULL,       "General options:"                          },
  {"-verbose",    ARGV_CONSTANT,(char*)TRUE, (char*)&verbose,   "Print out extra information."              },
  {"-quiet",      ARGV_CONSTANT,(char*)TRUE, (char*)&quiet,     "Print requested values only."              },
  {"-clobber",    ARGV_CONSTANT,(char*)TRUE, (char*)&clobber,   "Clobber existing files."                   },
  {"-max_buffer_size_in_kb",
                  ARGV_INT,     (char*)1,    (char*)&max_buffer_size_in_kb,
                                                                "maximum size of internal buffers."         },
 
  {NULL,          ARGV_HELP,    (char*)NULL, (char*)NULL,       "\nVoxel selection options:"                },
  {"-floor",      ARGV_FLOAT,   (char*)1,    (char*)&vol_range[0], "Ignore voxels below this value. (incl)"    },
  {"-ceil",       ARGV_FLOAT,   (char*)1,    (char*)&vol_range[1],  "Ignore voxels above this value. (incl)"    },
  {"-range",      ARGV_FLOAT,   (char*)2,    (char*)&vol_range, "Ignore voxels outside the range. (incl)"   },
#ifdef NOT_YET_IMPLEMENTED
  {"-binvalue",   ARGV_STRING,  (char*)1,    (char*)&vol_binvalue, "Include voxels within 0.5 of value (accepts a space or comma-separated list).\n"    },
#endif
  {"-mask",       ARGV_STRING,  (char*)1,    (char*)&mask_file, "<mask.mnc> Use mask file for calculations."},
  {"-mask_floor", ARGV_FLOAT,   (char*)1,    (char*)&mask_range[0],"Mask floor value. (incl)"                  },
  {"-mask_ceil",  ARGV_FLOAT,   (char*)1,    (char*)&mask_range[1], "Mask ceiling value. (incl)"                },
  {"-mask_range", ARGV_FLOAT,   (char*)2,    (char*)&mask_range,"Mask floor and ceiling. (incl)"            },
#ifdef NOT_YET_IMPLEMENTED
  {"-mask_binvalue", ARGV_STRING,  (char*)1,    (char*)mask_binvalue, "Include mask voxels within 0.5 of value (accepts a space or comma-separated list).\n"    },
#endif
  {"-include_nan",ARGV_CONSTANT,(char*)FALSE,(char*)&ignoreNaN, "Treat NaN values as zero."       },
  
  {NULL,          ARGV_HELP,    (char*)NULL, (char*)NULL,       "\nHistogram Options:"                      },
  {"-histogram",  ARGV_STRING,  (char*)1,    (char*)&hist_file, "<hist_file> Compute histogram."            },
  {"-hist_bins",  ARGV_INT,     (char*)1,    (char*)&hist_bins, "<number> of bins in each histogram."       },
  {"-bins",       ARGV_INT,     (char*)1,    (char*)&hist_bins, "synonym for -hist_bins."                   },
  {"-hist_floor", ARGV_FLOAT,   (char*)1,    (char*)&hist_range[0],"Histogram floor value. (incl)"             },
  {"-hist_ceil",  ARGV_FLOAT,   (char*)1,    (char*)&hist_range[1], "Histogram ceiling value. (incl)"           },
  {"-hist_range", ARGV_FLOAT,   (char*)2,    (char*)&hist_range,"Histogram floor and ceiling. (incl)"       },

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
  {"-var",        ARGV_CONSTANT,(char*)TRUE, (char*)&Variance,  "variance."                                 },
  {"-stddev",     ARGV_CONSTANT,(char*)TRUE, (char*)&Stddev,    "standard deviation."                       },
  {"-CoM",        ARGV_CONSTANT,(char*)TRUE, (char*)&CoM,       "centre of mass of the volume."                    },
  
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
   Math_Data       math_data;
   Loop_Options   *loop_options;
   int            mincid;
   int            idim;
   double         real_range[2];
   double         voxel_to_world[WORLD_NDIMS][WORLD_NDIMS+1];

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

   /* if nothing selected, do everything */
   if (!Vol_Count && !Vol_Per && !Vol && !Min && !Max && !Sum && !Sum2 && 
       !Mean && !Variance && !Stddev && !Hist_Count && !Hist_Per && 
       !Median && !Majority && !BiModalT && !PctT && !Entropy) {
      All = TRUE;
      Hist = TRUE;
   }
      
   if ((hist_file != NULL) || Hist_Count || Hist_Per || 
       Median || Majority || BiModalT || PctT || Entropy) {
      Hist = TRUE;
   }
      
   if (pctT > 0.0) { PctT = TRUE; pctT /= 100; }
   
   /* do checking on arguments */
   if (hist_bins < 1) {
      fprintf(stderr, "%s: Must have one or more bins for a histogram\n", argv[0]);
      exit(EXIT_FAILURE);
   }
   
   if (access(infiles[0], F_OK) != 0 ) {
      fprintf(stderr, "%s: Couldn't find %s\n", argv[0], infiles[0]);
      exit(EXIT_FAILURE);
   }
   
   if (infiles[1] != NULL && access(infiles[1], F_OK) != 0 ) {
      fprintf(stderr, "%s: Couldn't find mask file: %s\n", argv[0], infiles[1]);
      exit(EXIT_FAILURE);
   }
   
   if (hist_file != NULL && !clobber && access(hist_file, F_OK) != -1 ) {
      fprintf(stderr, "%s: Histogram %s exists! (use -clobber to overwrite)\n", argv[0], hist_file);
      exit(EXIT_FAILURE);
   }

   /* Open the file to get some information */
   mincid = miopen(infiles[0], NC_NOWRITE);
   nvoxels = get_minc_nvoxels(mincid);
   volume = get_minc_voxel_volume(mincid);
   miget_image_range(mincid, real_range);
   file_ndims = get_minc_ndims(mincid);
   find_minc_spatial_dims(mincid, space_to_dim);
   get_minc_voxel_to_world(mincid, voxel_to_world);
   
   if (verbose) {
      fprintf(stdout, "Volume Range:     %g\t%g\n", vol_range[0], vol_range[1]);
      fprintf(stdout, "Mask Range:       %g\t%g\n", mask_range[0], mask_range[1]);
   }
      
   /* set up the histogram if we are doing one */
   if (Hist) {
      histogram = (float*)calloc(hist_bins, sizeof(float));
      
      if (hist_range[0] == -DBL_MAX) { 
         if (vol_range[0] != -DBL_MAX) { hist_range[0] = vol_range[0];  }
         else                          { hist_range[0] = real_range[0]; }
      }
         
      if (hist_range[1] == DBL_MAX) {
         if (vol_range[1] !=  DBL_MAX) { hist_range[1] = vol_range[1];  }
         else                          { hist_range[1] = real_range[1]; }
      }
         
      hist_sep = (hist_range[1] - hist_range[0])/hist_bins;
      
      if (verbose) {
         fprintf(stdout, "Histogram Range:  %g\t%g\n", hist_range[0], hist_range[1]);
         fprintf(stdout, "Histogram bins:   %i  of Width (separation): %f\n", hist_bins, hist_sep);
      }
   }
   
   /* Do math */
   loop_options = create_loop_options();
   set_loop_first_input_mincid(loop_options, mincid);
   set_loop_verbose(loop_options, verbose);
   set_loop_buffer_size(loop_options, (long)1024 * max_buffer_size_in_kb);
   voxel_loop(nfiles, infiles, 0, NULL, NULL, 
              loop_options, do_math, (void *)&math_data);
   free_loop_options(loop_options);
   
   vol_per  = vvoxels / nvoxels * 100;
   hist_per = hvoxels / nvoxels * 100;
   mean     = sum/vvoxels;
   variance = sum2/vvoxels - SQR(mean);
   stddev   = sqrt(variance);
   for (idim=0; idim < WORLD_NDIMS; idim++) {
      if (sum != 0.0) 
         com[idim] = com_sum[idim] / sum;
      else
         com[idim] = 0.0;
   }
   
   /* Do the histogram calculations */
   if (Hist) {
      int    c;
      float  hist_center[hist_bins];
      float  pdf[hist_bins];                 /* probability density Function */
      float  cdf[hist_bins];                 /* cumulative density Function  */
      
      int    majority_bin = 0;
      int    median_bin   = 0;
      int    pctt_bin     = 0;
      int    bimodalt_bin = 0;
      
      /* BiModal Threshold variables*/
      double zero_moment  = 0.0;
      double first_moment = 0.0;
      double var          = 0.0;
      double max_var      = 0.0;
      
      for (c=0; c < hist_bins; c++) {
         hist_center[c] = (c*hist_sep) + (hist_sep/2);
         pdf[c] = histogram[c]/hvoxels;
         if (c == 0) { cdf[c] = pdf[c];            }
         else        { cdf[c] = cdf[c-1] + pdf[c]; }
         
         /* Majority */
         if (histogram[c] > histogram[majority_bin]) {
            majority_bin = c; 
         }
         
         /* Entropy */
         if (histogram[c] > 0.0) {
            entropy -= pdf[c] * (log(pdf[c])/log(2.0));
         }
         
         /* Histogram Median */
         if (cdf[c] < 0.5) {
            median_bin = c;
         }
         
         /* BiModal Threshold */
         if (c > 0) {
            zero_moment  += pdf[c];
            first_moment += hist_center[c]*pdf[c];
            
            var = SQR((mean * zero_moment) - first_moment) / 
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
         median = 0.5 * pdf[median_bin] *  hist_sep;
      else
         median = ((double)median_bin + (0.5 - cdf[median_bin]) 
                   * pdf[median_bin + 1]) *  hist_sep;
      
      majority = hist_center[majority_bin];
      biModalT = hist_center[bimodalt_bin];
      
      /* pct Threshold */
      if (pctt_bin == 0)
         pct_T = pctT * pdf[pctt_bin] *  hist_sep;
      else
         pct_T = ((double)pctt_bin + (pctT - cdf[pctt_bin]) 
                  * pdf[pctt_bin + 1]) *  hist_sep;
      
      /* output the histogram */
      if (hist_file != NULL) {
         
         FILE *FP = fopen(hist_file, "w");
         fprintf(FP, "# histogram for: %s\n",     infiles[0]);
         fprintf(FP, "#  mask file:    %s\n",     infiles[1]);
         fprintf(FP, "#  domain:       %g  %g\n", hist_range[0], hist_range[1]);
         fprintf(FP, "#  entropy:      %g\n",     entropy);;
         fprintf(FP, "#  bin centers     counts\n");
         for (c=0; c < hist_bins; c++)
            fprintf(FP, "  %f       %f\n", hist_center[c], histogram[c]);
         fclose(FP);
      }
   }  /* end histogram calculations */
   
   if (!quiet && real_range[0] != min && vol_range[0] == -DBL_MAX) { 
      fprintf(stderr, "*** %s - reported min (%g) doesn't equal header (%g)\n", argv[0], min, real_range[0]);
   } 
   if (!quiet && real_range[1] != max && vol_range[0] == DBL_MAX) { 
      fprintf(stderr, "*** %s - reported max (%g) doesn't equal header (%g)\n", argv[0], max, real_range[1]);
   }
      
   /* Output stats */
   if (All && !quiet)       { fprintf(stdout, "File:         %s\n", infiles[0]);}
   if (All && !quiet)       { fprintf(stdout, "Mask file:    %s\n", infiles[1]);}
   if (All && !quiet)       {    print_result("Total voxels: ", nvoxels  );     }
   if (All || Vol_Count)    {    print_result("# voxels:     ", vvoxels  );     }
   if (All || Vol_Per)      {    print_result("% of total:   ", vol_per  );     }
   if (All || Vol)          {    print_result("Volume (mm3): ", volume   );     }
   if (All || Min)          {    print_result("Min:          ", min      );     }
   if (All || Max)          {    print_result("Max:          ", max      );     }
   if (All || Sum)          {    print_result("Sum:          ", sum      );     }
   if (All || Sum2)         {    print_result("Sum^2:        ", sum2     );     }
   if (All || Mean)         {    print_result("Mean:         ", mean     );     }
   if (All || Variance)     {    print_result("Variance:     ", variance );     }
   if (All || Stddev)       {    print_result("Stddev:       ", stddev   );     }
   if (All || CoM)          {
      if (!quiet) {
         (void) fprintf(stdout, "CoM:          ");
      }
      (void) fprintf(stdout, "%.10g %.10g %.10g\n", com[0], com[1], com[2]);
   }
   
   if (Hist){
      if (All && !quiet)    {fprintf(stdout,"\nHistogram:    %s\n", hist_file); }
      if (All && !quiet)    {    print_result("Total voxels: ", nvoxels  );     }
      if (All || Hist_Count){    print_result("# voxels:     ", hvoxels  );     }
      if (All || Hist_Per)  {    print_result("% of total:   ", hist_per );     }
      if (All || Median)    {    print_result("Median:       ", median   );     }
      if (All || Majority)  {    print_result("Majority:     ", majority );     }
      if (All || BiModalT)  {    print_result("BiModalT:     ", biModalT );     }
      if (All || PctT)      {    char str[100];
                                 sprintf(str, "PctT [%3d%%]:  ", (int)(pctT * 100));
                                 print_result(str,              pct_T);         }
      if (All || Entropy)   {    print_result("Entropy :     ", entropy  );     }
      }
      
   return EXIT_SUCCESS;
}

void do_math(void *caller_data, long num_voxels, 
             int input_num_buffers, int input_vector_length,
             double *input_data[], 
             int output_num_buffers, int output_vector_length,
             double *output_data[], Loop_Info *loop_info)
{
   long ivox;
   long index[MAX_VAR_DIMS];
   
   /* Loop through the voxels - a bit of optimization in case we 
      have a brain-dead compiler*/
   if (mask_file != NULL) {
      for (ivox=0; ivox < num_voxels*input_vector_length; ivox++) {
         if ((input_data[1][ivox] >= mask_range[0]) && 
             (input_data[1][ivox] <= mask_range[1])) {
            get_info_voxel_index(loop_info, ivox, file_ndims, index);
            do_stats(input_data[0][ivox], index);
         }
      }
   }
      
   else {
      for (ivox=0; ivox < num_voxels*input_vector_length; ivox++) {
         get_info_voxel_index(loop_info, ivox, file_ndims, index);
         do_stats(input_data[0][ivox], index);
      }
   }
      
   return;
}
   
void do_stats(double value, long index[]) 
{
   int idim;

   /* Check for NaNs */
   if (value == -DBL_MAX) {
      if (ignoreNaN)
         value = 0.0;
      else
         return;
   }

   /* Collect stats if we are within range */
   if ((value >= vol_range[0]) && (value <= vol_range[1])) {
      vvoxels++;
      sum  += value;
      sum2 += SQR(value);
      
      if (value < min) { min = value; }
      if (value > max) { max = value; }

      /* Get voxel index */
      for (idim=0; idim < WORLD_NDIMS; idim++) {
         if (space_to_dim[idim] >= 0) {
            com_sum[idim] += value * index[space_to_dim[idim]];
         }
      }
      
      if (Hist && (value >= hist_range[0]) && (value <= hist_range[1])) {
         /*lower limit <= value < upper limit */
         histogram[(int)floor((value - hist_range[0])/hist_sep)]++;
         hvoxels++;
      }
   }
}
   
void print_result(char* title, double result)
{
   if (!quiet){ fprintf(stdout, "%s", title); }
   fprintf(stdout, "%.10g\n", result);
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

/* Get an attribute from a minc file */
void get_minc_attribute(int mincid, char *varname, char *attname, 
                       int maxvals, double vals[])
{
   int varid;
   int old_ncopts;
   int status;
   int att_length;

   if (!mivar_exists(mincid, varname)) return;
   varid = ncvarid(mincid, varname);
   old_ncopts = ncopts; ncopts = 0;
   status = miattget(mincid, varid, attname, NC_DOUBLE, maxvals, vals, 
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

/* Get the mapping from spatial dimension - x, y, z - to file dimensions */
void find_minc_spatial_dims(int mincid, int space_to_dim[])
{
   int imgid, dim[MAX_VAR_DIMS];
   int idim, ndims;
   char dimname[MAX_NC_NAME];

   /* Set default values */
   for (idim=0; idim < 3; idim++) 
      space_to_dim[idim] = -1;

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
            space_to_dim[0] = idim;
            break;
         case 'y':
            space_to_dim[1] = idim;
            break;
         case 'z':
            space_to_dim[2] = idim;
            break;
      }
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
      get_minc_attribute(mincid, dimensions[idim], MIstart, 1, &start);
      get_minc_attribute(mincid, dimensions[idim], MIstep,  1, &step);
      get_minc_attribute(mincid, dimensions[idim], MIdirection_cosines, 
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
