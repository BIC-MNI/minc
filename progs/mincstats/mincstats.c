/* mincstats.c
 *
 * Andrew Janke - rotor@cmr.uq.edu.au
 * Center for Magnetic Resonance
 * University of Queensland, Australia
 *
 * Original version - 1999 sometime..
 * Fri Jan 19 15:25:44 EST 2001 - created first version from minccount as a mirror
 *                                   of Alex Zijdenbos + John Sleds volume_stats proggy 
 *                                   with less memory overhead
 * Wed Jan 31 14:33:30 EST 2001 - finished -entropy, -median and -histogram
 * Thu Feb  1 17:16:21 EST 2001 - completed filename checking and other mundane stuff
 *                                   first release 1.0
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
#include <float.h>
#include <math.h>
#include <ParseArgv.h>
#include <mincprog.h>
#include <volume_io.h>

#ifndef TRUE
#  define TRUE  1
#  define FALSE 0
#endif

#define SQR(x)    ((x)*(x))

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
void do_stats(double value);
void print_result(char* title, double result);

/* Argument variables */
int max_buffer_size_in_kb = 4 * 1024;

static int    verbose        = FALSE;
static int    quiet          = FALSE;
static int    clobber        = FALSE;
static int    ignoreNaN      = TRUE;

static int    All            = NULL;
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

static int    Hist           = FALSE;
static int    Hist_Count     = FALSE;
static int    Hist_Per       = FALSE;
static int    Median         = FALSE;
static int    Majority       = FALSE;
static int    BiModalT       = FALSE;
static int    PctT           = FALSE;
static double pctT           = 0.0;
static int    Entropy        = FALSE;
  
static double  vol_floor     = -DBL_MAX;
static double  vol_ceil      =  DBL_MAX;
static double  vol_range[2]  = {-DBL_MAX, DBL_MAX};

char          *mask_file;
static double  mask_floor    =  0.0;
static double  mask_ceil     =  DBL_MAX;
static double  mask_range[2] = {-DBL_MAX, DBL_MAX};

float         *histogram;
char          *hist_file;
static int     hist_bins     = 2000;
static double  hist_sep;
static double  hist_floor    = -DBL_MAX;
static double  hist_ceil     =  DBL_MAX;
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
  {"-floor",      ARGV_FLOAT,   (char*)1,    (char*)&vol_floor, "Ignore voxels below this value. (incl)"    },
  {"-ceil",       ARGV_FLOAT,   (char*)1,    (char*)&vol_ceil,  "Ignore voxels above this value. (incl)"    },
  {"-range",      ARGV_FLOAT,   (char*)2,    (char*)&vol_range, "Ignore voxels outside the range. (incl)"   },
  {"-mask",       ARGV_STRING,  (char*)1,    (char*)&mask_file, "<mask.mnc> Use mask file for calculations."},
  {"-mask_floor", ARGV_FLOAT,   (char*)1,    (char*)&mask_floor,"Mask floor value. (incl)"                  },
  {"-mask_ceil",  ARGV_FLOAT,   (char*)1,    (char*)&mask_ceil, "Mask ceiling value. (incl)"                },
  {"-mask_range", ARGV_FLOAT,   (char*)2,    (char*)&mask_range,"Mask floor and ceiling. (incl)"            },
  {"-include_nan",ARGV_CONSTANT,(char*)FALSE,(char*)&ignoreNaN, "Include NaN values in calculations."       },
  
  {NULL,          ARGV_HELP,    (char*)NULL, (char*)NULL,       "\nHistogram Options:"                      },
  {"-histogram",  ARGV_STRING,  (char*)1,    (char*)&hist_file, "<hist_file> Compute histogram."            },
  {"-hist_bins",  ARGV_INT,     (char*)1,    (char*)&hist_bins, "<number> of bins in each histogram."       },
  {"-bins",       ARGV_INT,     (char*)1,    (char*)&hist_bins, "synonym for -hist_bins."                   },
  {"-hist_floor", ARGV_FLOAT,   (char*)1,    (char*)&hist_floor,"Histogram floor value. (incl)"             },
  {"-hist_ceil",  ARGV_FLOAT,   (char*)1,    (char*)&hist_ceil, "Histogram ceiling value. (incl)"           },
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


void main(int argc, char *argv[]){
   char          **infiles;
   int             nfiles;
   Math_Data       math_data;
   Loop_Options   *loop_options;
   
   volume_input_struct input_info;
   Volume          in_volume;
   char           *axis_order[3] = {MIzspace, MIyspace, MIxspace};
   Real            real_min, real_max;
   Real            steps[MAX_VAR_DIMS];
   int             sizes[MAX_VAR_DIMS];

   /* Get arguments */
   if (ParseArgv(&argc, argv, argTable, 0) || (argc != 2)){
      (void) fprintf(stderr, "\nUsage: %s [options] <infile.mnc>\n", argv[0]);
      (void) fprintf(stderr,   "       %s -help\n\n", argv[0]);
      exit(EXIT_FAILURE);
      }
   nfiles = argc - 1;
   infiles = &argv[1];
   infiles[1] = &mask_file[0];
   
   if (infiles[1] != NULL){
      nfiles++;
      }

   /* Set up ranges from C/L Arguments*/
   if (vol_floor  != -DBL_MAX){ vol_range[0]  = vol_floor;  }
   if (vol_ceil   !=  DBL_MAX){ vol_range[1]  = vol_ceil;   }
   if (mask_floor != -DBL_MAX){ mask_range[0] = mask_floor; }
   if (mask_ceil  !=  DBL_MAX){ mask_range[1] = mask_ceil;  }
   if (hist_floor != -DBL_MAX){ hist_range[0] = hist_floor; }
   if (hist_ceil  !=  DBL_MAX){ hist_range[1] = hist_ceil;  }
   
   /* if nothing selected, do everything */
   if (!Vol_Count && !Vol_Per && !Vol && !Min && !Max && !Sum && !Sum2 && 
         !Mean && !Variance && !Stddev && !Hist_Count && !Hist_Per && 
         !Median && !Majority && !BiModalT && !PctT && !Entropy){
      All = TRUE;
      Hist = TRUE;
      }
      
   if ((hist_file != NULL) || Hist_Count || Hist_Per || 
         Median || Majority || BiModalT || PctT || Entropy){
      Hist = TRUE;
      }
      
   if (pctT > 0.0){ PctT = TRUE; pctT /= 100; }
   
   /* do checking on arguments */
   if (hist_bins < 1){
      fprintf(stderr, "%s: Must have one or more bins for a histogram\n", argv[0]);
      exit(EXIT_FAILURE);
      }
   
   if (access(infiles[0], F_OK) != 0 ){
      fprintf(stderr, "%s: Couldn't find %s\n", argv[0], infiles[0]);
      exit(EXIT_FAILURE);
      }
   
   if (infiles[1] != NULL && access(infiles[1], F_OK) != 0 ){
      fprintf(stderr, "%s: Couldn't find mask file: %s\n", argv[0], infiles[1]);
      exit(EXIT_FAILURE);
      }
   
   if (hist_file != NULL && !clobber && access(hist_file, F_OK) != -1 ){
      fprintf(stderr, "%s: Histogram %s exists! (use -clobber to overwrite)\n", argv[0], hist_file);
      exit(EXIT_FAILURE);
      }
   
   /* get volume range and dimension sizes */
   start_volume_input(infiles[0], MAX_VAR_DIMS, axis_order, NC_UNSPECIFIED, 
			 TRUE, 0.0, 0.0, TRUE, &in_volume, (minc_input_options*)NULL, 
			 &input_info);
         
   get_volume_real_range(in_volume, &real_min, &real_max);
   get_volume_sizes(in_volume, sizes);
   get_volume_separations(in_volume, steps);
   
   cancel_volume_input(in_volume, &input_info);
   
   if (verbose){
      fprintf(stdout, "Volume Range:     %g\t%g\n", vol_range[0], vol_range[1]);
      fprintf(stdout, "Mask Range:       %g\t%g\n", mask_range[0], mask_range[1]);
      }
      
   /* set up the histogram if we are doing one */
   if (Hist){
      histogram = (float*)calloc(hist_bins, sizeof(float));
      
      if (hist_range[0] == -DBL_MAX){ 
         if (vol_range[0] != -DBL_MAX){ hist_range[0] = vol_range[0]; }
         else                         { hist_range[0] = real_min;     }
         }
         
      if (hist_range[1] == DBL_MAX){ 
         if (vol_range[1] !=  DBL_MAX){ hist_range[1] = vol_range[1]; }
         else                         { hist_range[1] = real_max;     }
         }
         
      hist_sep = (hist_range[1] - hist_range[0])/hist_bins;
      
      if (verbose){
         fprintf(stdout, "Histogram Range:  %g\t%g\n", hist_range[0], hist_range[1]);
         fprintf(stdout, "Histogram bins:   %i  of Width (separation): %f\n", hist_bins, hist_sep);
         }
      }
   
   /* Do math */
   loop_options = create_loop_options();
   set_loop_verbose(loop_options, verbose);
   set_loop_buffer_size(loop_options, (long)1024 * max_buffer_size_in_kb);
   voxel_loop(nfiles, infiles, 0, NULL, NULL, loop_options, do_math, (void *)&math_data);
	free_loop_options(loop_options);
   
   nvoxels  = sizes[0] * sizes[1] * sizes[2];
   volume   = vvoxels * fabs(steps[0] * steps[1] * steps[2]);
   vol_per  = vvoxels / nvoxels * 100;
   hist_per = hvoxels / nvoxels * 100;
   mean     = sum/vvoxels;
   variance = sum2/vvoxels - SQR(mean);
   stddev   = sqrt(variance);
   
   /* Do the histogram calculations */
   if (Hist){
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
      
      for (c=0; c < hist_bins; c++){
         hist_center[c] = (c*hist_sep) + (hist_sep/2);
         pdf[c] = histogram[c]/hvoxels;
         if (c == 0){ cdf[c] = pdf[c];            }
         else {       cdf[c] = cdf[c-1] + pdf[c]; }
         
         /* Majority */
         if (histogram[c] > histogram[majority_bin]){ 
            majority_bin = c; 
            }
         
         /* Entropy */
         if (histogram[c] > 0.0){
            entropy -= pdf[c] * (log(pdf[c])/log(2.0));
            }
         
         /* Histogram Median */
         if (cdf[c] < 0.5){
            median_bin = c;
            }
         
         /* BiModal Threshold */
         if (c > 0){
            zero_moment  += pdf[c];
            first_moment += hist_center[c]*pdf[c];
            
            var = SQR((mean * zero_moment) - first_moment) / (zero_moment * (1 - zero_moment));

            if (var > max_var){
               bimodalt_bin = c;
               max_var = var;
               }
            }
         
         /* pct Threshold */
         if (cdf[c] < pctT){
            pctt_bin = c;
            }
         }
         
      /* median */
      if (median_bin == 0)
         median = 0.5 * pdf[median_bin] *  hist_sep;
      else
         median = ((double)median_bin + (0.5 - cdf[median_bin]) * pdf[median_bin + 1]) *  hist_sep;
      
      majority = hist_center[majority_bin];
      biModalT = hist_center[bimodalt_bin];
      
      /* pct Threshold */
      if (pctt_bin == 0)
         pct_T = pctT * pdf[pctt_bin] *  hist_sep;
      else
         pct_T = ((double)pctt_bin + (pctT - cdf[pctt_bin]) * pdf[pctt_bin + 1]) *  hist_sep;
      
      /* output the histogram */
      if (hist_file != NULL){
         
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
   
   if (!quiet && real_min != min && vol_range[0] == -DBL_MAX){ 
      fprintf(stderr, "*** %s - reported min (%g) doesn't equal header (%g)\n", argv[0], min, real_min); } 
   if (!quiet && real_max != max && vol_range[0] == DBL_MAX){ 
      fprintf(stderr, "*** %s - reported max (%g) doesn't equal header (%g)\n", argv[0], max, real_max); } 
      
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
      
   exit(EXIT_SUCCESS);
   }

void do_math(void *caller_data, long num_voxels, int input_num_buffers, int input_vector_length,
                    double *input_data[], int output_num_buffers, int output_vector_length,
                    double *output_data[], Loop_Info *loop_info){
   long ivox;
   
   /* Loop through the voxels - a bit of optimization in case we have a brain-dead compiler*/
   if (mask_file != NULL){
      for (ivox=0; ivox < num_voxels*input_vector_length; ivox++)
         if ((input_data[1][ivox] >= mask_range[0]) && (input_data[1][ivox] <= mask_range[1]))
            do_stats(input_data[0][ivox]);
      }
      
   else{ 
      for (ivox=0; ivox < num_voxels*input_vector_length; ivox++)
         do_stats(input_data[0][ivox]);
	   }
      
   return;
   }
   
void do_stats(double value){
   if ((value >= vol_range[0]) && (value <= vol_range[1])){
      vvoxels++;
      sum  += value;
      sum2 += SQR(value);
      
      if (value < min){ min = value; }
      if (value > max){ max = value; }
      
      if (Hist && (value >= hist_range[0]) && (value <= hist_range[1])){
         /*lower limit <= value < upper limit */
         histogram[(int)floor((value - hist_range[0])/hist_sep)]++;
         hvoxels++;
         }
      }
   }
   
void print_result(char* title, double result){
   if (!quiet){ fprintf(stdout, "%s", title); }
   fprintf(stdout, "%.10g\n", result);
   }
