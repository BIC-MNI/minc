/* ----------------------------- MNI Header -----------------------------------
@NAME       : mincaverage
@INPUT      : argc, argv - command line arguments
@OUTPUT     : (none)
@RETURNS    : status
@DESCRIPTION: Program to average minc files
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 28, 1995 (Peter Neelin)
@MODIFIED   : 
 * $Log: mincaverage.c,v $
 * Revision 6.11  2008/01/17 02:33:02  rotor
 *  * removed all rcsids
 *  * removed a bunch of ^L's that somehow crept in
 *  * removed old (and outdated) BUGS file
 *
 * Revision 6.10  2008/01/12 19:08:15  stever
 * Add __attribute__ ((unused)) to all rcsid variables.
 *
 * Revision 6.9  2007/12/11 12:43:01  rotor
 *  * added static to all global variables in main programs to avoid linking
 *       problems with libraries (compress in mincconvert and libz for example)
 *
 * Revision 6.8  2005/08/26 21:07:16  bert
 * Use #if rather than #ifdef with MINC2 symbol, and be sure to include config.h whereever MINC2 is used
 *
 * Revision 6.7  2004/12/14 23:52:50  bert
 * Get rid of compilation warnings w/c99
 *
 * Revision 6.6  2004/11/01 22:38:38  bert
 * Eliminate all references to minc_def.h
 *
 * Revision 6.5  2004/04/27 15:38:15  bert
 * Added -2 option
 *
 * Revision 6.4  2001/04/24 13:38:42  neelin
 * Replaced NC_NAT with MI_ORIGINAL_TYPE.
 *
 * Revision 6.3  2001/04/17 18:40:17  neelin
 * Modifications to work with NetCDF 3.x
 * In particular, changed NC_LONG to NC_INT (and corresponding longs to ints).
 * Changed NC_UNSPECIFIED to NC_NAT.
 * A few fixes to the configure script.
 *
 * Revision 6.2  2000/07/07 13:19:12  neelin
 * Added option -filelist to read file names from a file. This gets around
 * command-line length limits.
 *
 * Revision 6.1  1999/10/19 14:45:18  neelin
 * Fixed Log subsitutions for CVS
 *
 * Revision 6.0  1997/09/12 13:24:19  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:25:18  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:05:59  neelin
 * Release of minc version 0.4
 *
 * Revision 3.2  1996/04/02  20:16:09  neelin
 * Added -width_weighted option. Allow -weights with -avgdim option.
 *
 * Revision 3.1  1995/11/20  14:24:47  neelin
 * Added -weights option.
 *
 * Revision 3.0  1995/05/15  19:32:44  neelin
 * Release of minc version 0.3
 *
 * Revision 1.6  1995/05/05  18:08:17  neelin
 * Removed debugging line for sd calculation.
 *
 * Revision 1.5  1995/05/02  16:08:17  neelin
 * Added -check, -nocheck options.
 *
 * Revision 1.4  1995/04/27  14:05:38  neelin
 * Added binarization options.
 *
 * Revision 1.3  1995/04/27  12:48:42  neelin
 * Changed order of options.
 *
 * Revision 1.2  1995/04/27  11:50:35  neelin
 * Require either -norm or -nonorm on command line.
 *
 * Revision 1.1  1995/04/26  14:16:38  neelin
 * Initial revision
 *
---------------------------------------------------------------------------- */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#define _GNU_SOURCE
#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <minc.h>
#include <ParseArgv.h>
#include <time_stamp.h>
#include <voxel_loop.h>

/* Constants */

#ifndef TRUE
#  define TRUE 1
#  define FALSE 0
#endif

#define THRESH_FRACTION (1/50.0)

#define WIDTH_SUFFIX "-width"

#define DEFAULT_BOOLEAN -1

/* Double_Array structure */
typedef struct {
   int numvalues;
   double *values;
} Double_Array;

/* Structures for averaging and normalizing information */
typedef struct {
   int binarize;
   int need_sd;
   double binrange[2];
   double *norm_factor;
   int averaging_over_dimension;
   int num_weights;
   double *weights;
} Average_Data;

typedef struct {
   int threshold_set;
   double threshold;
   double sum0, sum1;
} Norm_Data;

/* Function prototypes */
static void do_normalization(void *caller_data, long num_voxels, 
                             int input_num_buffers, int input_vector_length,
                             double *input_data[],
                             int output_num_buffers, int output_vector_length,
                             double *output_data[],
                             Loop_Info *loop_info);
static void find_mincfile_range(int mincid, double *minimum, double *maximum);
static void do_average(void *caller_data, long num_voxels, 
                       int input_num_buffers, int input_vector_length,
                       double *input_data[],
                       int output_num_buffers, int output_vector_length,
                       double *output_data[],
                       Loop_Info *loop_info);
static void start_average(void *caller_data, long num_voxels, 
                          int output_num_buffers, int output_vector_length,
                          double *output_data[],
                          Loop_Info *loop_info);
static void finish_average(void *caller_data, long num_voxels, 
                          int output_num_buffers, int output_vector_length,
                          double *output_data[],
                          Loop_Info *loop_info);
static int get_double_list(char *dst, char *key, char *nextarg);
static char **read_file_names(char *filelist, int *num_files);

/* Argument variables */
static int clobber = FALSE;
static int verbose = TRUE;
static int debug = FALSE;
static int check_dimensions = TRUE;
#ifdef NO_DEFAULT_NORM
static int normalize = -1;
#else
static int normalize = FALSE;
#endif
static char *sdfile = NULL;
static nc_type datatype = MI_ORIGINAL_TYPE;
static int is_signed = FALSE;
static double valid_range[2] = {0.0, 0.0};
static int copy_all_header = DEFAULT_BOOLEAN;
static char *averaging_dimension = NULL;
static int max_buffer_size_in_kb = 4 * 1024;
static int binarize = FALSE;
static double binrange[2] = {DBL_MAX, -DBL_MAX};
static double binvalue = -DBL_MAX;
static Double_Array weights = {0, NULL};
static int width_weighted = FALSE;
static char *filelist = NULL;
#if MINC2
static int minc2_format = FALSE;
#endif /* MINC2 */

/* Argument table */
ArgvInfo argTable[] = {
#if MINC2
    {"-2", ARGV_CONSTANT, (char *) TRUE, (char *) &minc2_format,
     "Produce a MINC 2.0 format output file"},
#endif /* MINC2 */
   {"-clobber", ARGV_CONSTANT, (char *) TRUE, (char *) &clobber,
       "Overwrite existing file."},
   {"-noclobber", ARGV_CONSTANT, (char *) FALSE, (char *) &clobber,
       "Don't overwrite existing file (default)."},
   {"-no_clobber", ARGV_CONSTANT, (char *) FALSE, (char *) &clobber,
       "Synonym for -noclobber."},
   {"-verbose", ARGV_CONSTANT, (char *) TRUE, (char *) &verbose,
       "Print out log messages (default)."},
   {"-quiet", ARGV_CONSTANT, (char *) FALSE, (char *) &verbose,
       "Do not print out log messages."},
   {"-debug", ARGV_CONSTANT, (char *) TRUE, (char *) &debug,
       "Print out debugging messages."},
   {"-filelist", ARGV_STRING, (char *) 1, (char *) &filelist,
       "Specify the name of a file containing input file names (- for stdin)."},
   {"-check_dimensions", ARGV_CONSTANT, (char *) TRUE, (char *) &check_dimensions,
       "Check that dimension info matches across files (default)."},
   {"-nocheck_dimensions", ARGV_CONSTANT, (char *) FALSE, (char *) &check_dimensions,
       "Do not check dimension info."},
   {"-max_buffer_size_in_kb", ARGV_INT, (char *) 1, 
       (char *) &max_buffer_size_in_kb,
       "Specify the maximum size of the internal buffers (in kbytes)."},
   {"-filetype", ARGV_CONSTANT, (char *) MI_ORIGINAL_TYPE, (char *) &datatype,
       "Use data type of first file (default)."},
   {"-byte", ARGV_CONSTANT, (char *) NC_BYTE, (char *) &datatype,
       "Write out byte data."},
   {"-short", ARGV_CONSTANT, (char *) NC_SHORT, (char *) &datatype,
       "Write out short integer data."},
   {"-int", ARGV_CONSTANT, (char *) NC_INT, (char *) &datatype,
       "Write out 32-bit integer data."},
   {"-long", ARGV_CONSTANT, (char *) NC_INT, (char *) &datatype,
       "Superseded by -int."},
   {"-float", ARGV_CONSTANT, (char *) NC_FLOAT, (char *) &datatype,
       "Write out single-precision floating-point data."},
   {"-double", ARGV_CONSTANT, (char *) NC_DOUBLE, (char *) &datatype,
       "Write out double-precision floating-point data."},
   {"-signed", ARGV_CONSTANT, (char *) TRUE, (char *) &is_signed,
       "Write signed integer data."},
   {"-unsigned", ARGV_CONSTANT, (char *) FALSE, (char *) &is_signed,
       "Write unsigned integer data (default if type specified)."},
   {"-range", ARGV_FLOAT, (char *) 2, (char *) valid_range,
       "Valid range for output data."},
   {"-normalize", ARGV_CONSTANT, (char *) TRUE, (char *) &normalize,
       "Normalize data sets for mean intensity."},
   {"-nonormalize", ARGV_CONSTANT, (char *) FALSE, (char *) &normalize,
       "Do not normalize data sets (default)."},
   {"-sdfile", ARGV_STRING, (char *) 1, (char *) &sdfile,
       "Specify an output sd file (default=none)."},
   {"-copy_header", ARGV_CONSTANT, (char *) TRUE, (char *) &copy_all_header,
       "Copy all of the header from the first file (default for one file)."},
   {"-nocopy_header", ARGV_CONSTANT, (char *) FALSE, (char *) &copy_all_header,
       "Do not copy all of the header from the first file (default for many files))."},
   {"-avgdim", ARGV_STRING, (char *) 1, (char *) &averaging_dimension,
       "Specify a dimension along which we wish to average."},
   {"-binarize", ARGV_CONSTANT, (char *) TRUE, (char *) &binarize,
       "Binarize the volume by looking for values in a given range."},
   {"-binrange", ARGV_FLOAT, (char *) 2, (char *) binrange,
       "Specify a range for binarization."},
   {"-binvalue", ARGV_FLOAT, (char *) 1, (char *) &binvalue,
       "Specify a target value (+/- 0.5) for binarization."},
   {"-weights", ARGV_FUNC, (char *) get_double_list, 
       (char *) &weights,
       "Specify weights for averaging (\"<w1>,<w2>,...\")."},
   {"-width_weighted", ARGV_CONSTANT, (char *) TRUE, (char *) &width_weighted,
       "Weight by dimension widths when -avgdim is used."},
   {NULL, ARGV_END, NULL, NULL, NULL}
};

/* Main program */

int main(int argc, char *argv[])
{
   char **infiles, *outfiles[2];
   int nfiles, nout;
   char *arg_string;
   Norm_Data norm_data;
   Average_Data average_data;
   Loop_Options *loop_options;
   double *vol_mean, vol_total, nvols, global_mean, total_weight;
   int ifile, iweight;
   int weights_specified;
   int first_mincid, dimid, varid, dim[MAX_VAR_DIMS];
   int ndims;
   long start, count;
   int old_ncopts;
   int strlength;
   char dimname[MAX_NC_NAME];

   /* Save time stamp and args */
   arg_string = time_stamp(argc, argv);

   /* Get arguments */
   if (ParseArgv(&argc, argv, argTable, 0) || (argc < 2)) {
      (void) fprintf(stderr, 
      "\nUsage: %s [options] [<in1.mnc> ...] <out.mnc>\n",
                     argv[0]);
      (void) fprintf(stderr, 
        "       %s -help\n\n", argv[0]);
      exit(EXIT_FAILURE);
   }
   outfiles[0] = argv[argc-1];
   outfiles[1] = sdfile;
   nout = ((sdfile == NULL) ? 1 : 2);
   first_mincid = MI_ERROR;

   /* Get the list of input files either from the command line or
      from a file, or report an error if both are specified */
   nfiles = argc - 2;
   if (filelist == NULL) {
      infiles = &argv[1];
   }
   else if (nfiles <= 0) {
      infiles = read_file_names(filelist, &nfiles);
      if (infiles == NULL) {
         (void) fprintf(stderr, 
                        "Error reading in file names from file \"%s\"\n",
                        filelist);
         exit(EXIT_FAILURE);
      }
   }
   else {
      (void) fprintf(stderr, 
                     "Do not specify both -filelist and input file names\n");
      exit(EXIT_FAILURE);
   }

   /* Make sure that we have something to process */
   if (nfiles == 0) {
      (void) fprintf(stderr, "No input files specified\n");
      exit(EXIT_FAILURE);
   }

   /* Set default value of copy_all_header */
   if (copy_all_header == DEFAULT_BOOLEAN) {
      copy_all_header = (nfiles <= 1);
   }

   /* Are we averaging over a dimension? */
   average_data.averaging_over_dimension = (averaging_dimension != NULL);

   /* Check for weights and width-weighting */
   weights_specified = weights.numvalues > 0;
   if (weights_specified && width_weighted) {
      (void) fprintf(stderr, 
         "%s: Please do not specify weights and width-weighting.\n",
                     argv[0]);
      exit(EXIT_FAILURE);
   }

   /* Default is no weighting */
   average_data.num_weights = 0;
   average_data.weights = NULL;

   /* Check for weights */
   if (weights_specified) {
      if (averaging_dimension == NULL) {
         if (weights.numvalues != nfiles) {
            (void) fprintf(stderr, 
               "%s: Number of weights does not match number of files.\n",
                           argv[0]);
            exit(EXIT_FAILURE);
         }
      }
      else {
         if (nfiles > 1) {
            (void) fprintf(stderr,
               "%s: Only one input file allowed with -weights and -avgdim.\n",
                           argv[0]);
            exit(EXIT_FAILURE);
         }

         /* Check that the dimension size matches the number of weights */
         first_mincid = miopen(infiles[0], NC_NOWRITE);
         dimid = ncdimid(first_mincid, averaging_dimension);
         (void) ncdiminq(first_mincid, dimid, NULL, &count);
         if (weights.numvalues != count) {
            (void) fprintf(stderr,
               "%s: Number of weights does not match size of dimension.\n",
                           argv[0]);
         }
      }

      /* Save the weights */
      average_data.num_weights = weights.numvalues;
      average_data.weights = 
         malloc(sizeof(*average_data.weights) * average_data.num_weights);
      for (iweight=0; iweight < average_data.num_weights; iweight++) {
         average_data.weights[iweight] = weights.values[iweight];
      }

      free(weights.values);
   }

   /* Check for width weighting */
   if (width_weighted) {

      /* Check for errors */
      if (averaging_dimension == NULL) {
         (void) fprintf(stderr, 
                        "%s: Please specify -avgdim with -width_weighted.\n",
                        argv[0]);
         exit(EXIT_FAILURE);
      }
      if (nfiles > 1) {
         (void) fprintf(stderr,
                        "%s: Use -width_weighted with only one input file.\n",
                        argv[0]);
         exit(EXIT_FAILURE);
      }

      /* Open the file */
      first_mincid = miopen(infiles[0], NC_NOWRITE);

      /* Get the dimension id */
      dimid = ncdimid(first_mincid, averaging_dimension);

      /* Look for the width variable */
      strlength = MAX_NC_NAME - strlen(WIDTH_SUFFIX) - 1;
      (void) strncpy(dimname, averaging_dimension, strlength);
      dimname[strlength] = '\0';
      (void) strcat(dimname, WIDTH_SUFFIX);
      old_ncopts = ncopts; ncopts = 0;
      varid = ncvarid(first_mincid, dimname);
      (void) ncvarinq(first_mincid, varid, NULL, NULL, &ndims, dim, NULL);
      ncopts = old_ncopts;
      if (varid != MI_ERROR) {

         /* Check that things match up */
         if ((ndims != 1) || (dim[0] != dimid)) {
            (void) fprintf(stderr,
                "%s: Dimension width variable does not match avgdim.\n",
                           argv[0]);
         }

         /* Get the size of the dimension */
         (void) ncdiminq(first_mincid, dim[0], NULL, &count);
         average_data.num_weights = count;
         average_data.weights = 
            malloc(sizeof(*average_data.weights) * average_data.num_weights);

         /* Read in the widths */
         start = 0;
         (void) mivarget(first_mincid, varid, &start, &count, NC_DOUBLE, NULL,
                         average_data.weights);

      }
   }    /* If width_weighted */

   /* Check that weights sum to non-zero. We don't need to normalize them,
      since a running sum is done in the averaging. */
   if (average_data.num_weights > 0) {
      total_weight = 0.0;
      for (iweight=0; iweight < average_data.num_weights; iweight++) {
         total_weight += average_data.weights[iweight];
      }
      if (total_weight == 0.0) {
         (void) fprintf(stderr, "%s: Weights sum to zero.\n", argv[0]);
         exit(EXIT_FAILURE);
      }
   }

   /* Check for binarization */
   if (binarize) {
      if (normalize == TRUE) {
         (void) fprintf(stderr, 
            "%s: Normalization and binarization cannot both be specified\n",
                        argv[0]);
         exit(EXIT_FAILURE);
      }
      normalize = FALSE;
      if (binvalue != -DBL_MAX) {
         binrange[0] = binvalue - 0.5;
         binrange[1] = binvalue + 0.5;
      }
      if (binrange[0] > binrange[1]) {
         (void) fprintf(stderr, 
         "%s: Please specify a binarization range with min less than max\n",
                        argv[0]);
         exit(EXIT_FAILURE);
      }
      average_data.binrange[0] = binrange[0];
      average_data.binrange[1] = binrange[1];
   }
   average_data.binarize = binarize;

   /* Check for no specification of normalization */
#ifdef NO_DEFAULT_NORM
   if (normalize == -1) {
      (void) fprintf(stderr, "\n%s: %s\n\n%s\n%s\n%s\n%s\n%s\n\n", argv[0],
"Please specify either -norm or -nonorm.",
"The default setting for normalization is being changed from \"-norm\" to",
"\"-nonorm\". To prevent undetected problems with data, this program will ",
"not work unless one of these flags is explicitly given on the command-line",
"(ie. no default is permitted). The new default will come into effect some",
"time in the future."
                     );
      exit(EXIT_FAILURE);
   }
#endif

   /* Do normalization if needed */
   average_data.norm_factor = 
      malloc(sizeof(*average_data.norm_factor) * nfiles);
   if (normalize) {
      vol_mean = malloc(sizeof(*vol_mean) * nfiles);
      loop_options = create_loop_options();
      set_loop_verbose(loop_options, FALSE);
#if MINC2
      set_loop_v2format(loop_options, minc2_format);
#endif /* MINC2 */
      set_loop_accumulate(loop_options, TRUE, 0, NULL, NULL);
      set_loop_buffer_size(loop_options, (long) 1024 * max_buffer_size_in_kb);
      set_loop_check_dim_info(loop_options, check_dimensions);
      vol_total = 0.0;
      nvols = 0;
      if (verbose) {
         (void) fprintf(stderr, "Normalizing:");
         (void) fflush(stderr);
      }
      for (ifile=0; ifile < nfiles; ifile++) {
         norm_data.threshold_set = FALSE;
         norm_data.sum0 = 0.0;
         norm_data.sum1 = 0.0;
         if (verbose) {
            (void) fprintf(stderr, ".");
            (void) fflush(stderr);
         }
         if (first_mincid != MI_ERROR) {
            set_loop_first_input_mincid(loop_options, first_mincid);
            first_mincid = MI_ERROR;
         }
         voxel_loop(1, &infiles[ifile], 0, NULL, NULL, loop_options,
                    do_normalization, (void *) &norm_data);
         if (norm_data.sum0 > 0.0) {
            vol_mean[ifile] = norm_data.sum1 / norm_data.sum0;
            vol_total += vol_mean[ifile];
            nvols++;
         }
         else {
            vol_mean[ifile] = 0.0;
         }
         if (debug) {
            (void) fprintf(stderr, "Volume %d mean = %.15g\n",
                           ifile, vol_mean[ifile]);
         }
      }
      free_loop_options(loop_options);
      if (verbose) {
         (void) fprintf(stderr, "Done\n");
         (void) fflush(stderr);
      }
      if (nvols > 0)
         global_mean = vol_total / nvols;
      else
         global_mean = 0.0;
      for (ifile=0; ifile < nfiles; ifile++) {
         if (vol_mean[ifile] > 0.0)
            average_data.norm_factor[ifile] = global_mean / vol_mean[ifile];
         else
            average_data.norm_factor[ifile] = 0.0;
         if (debug) {
            (void) fprintf(stderr, "Volume %d norm factor = %.15g\n", 
                           ifile, average_data.norm_factor[ifile]);
         }
      }
      free(vol_mean);
   }
   else {
      for (ifile=0; ifile < nfiles; ifile++) {
         average_data.norm_factor[ifile] = 1.0;
      }
   }

   /* Do averaging */
   average_data.need_sd = (sdfile != NULL);
   loop_options = create_loop_options();
   if (first_mincid != MI_ERROR) {
      set_loop_first_input_mincid(loop_options, first_mincid);
      first_mincid = MI_ERROR;
   }
   set_loop_verbose(loop_options, verbose);
   set_loop_clobber(loop_options, clobber);
   set_loop_datatype(loop_options, datatype, is_signed, 
                     valid_range[0], valid_range[1]);
   set_loop_accumulate(loop_options, TRUE, 1, start_average, finish_average);
   set_loop_copy_all_header(loop_options, copy_all_header);
   set_loop_dimension(loop_options, averaging_dimension);
   set_loop_buffer_size(loop_options, (long) 1024 * max_buffer_size_in_kb);
   set_loop_check_dim_info(loop_options, check_dimensions);
   voxel_loop(nfiles, infiles, nout, outfiles, arg_string, loop_options,
              do_average, (void *) &average_data);
   free_loop_options(loop_options);

   /* Free stuff */
   free(average_data.weights);
   free(average_data.norm_factor);

   exit(EXIT_SUCCESS);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : do_normalization
@INPUT      : Standard for voxel_loop
@OUTPUT     : Standard for voxel_loop
@RETURNS    : (nothing)
@DESCRIPTION: Routine to loop through an array of voxels and calculate 
              normalization values.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 25, 1995 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static void do_normalization(void *caller_data, long num_voxels, 
                             int input_num_buffers, int input_vector_length,
                             double *input_data[],
                             int output_num_buffers, int output_vector_length,
                             double *output_data[],
                             Loop_Info *loop_info)
     /* ARGSUSED */
{
   Norm_Data *norm_data;
   long ivox;
   double value, minimum, maximum;

   /* Get pointer to window info */
   norm_data = (Norm_Data *) caller_data;

   /* Check arguments */
   if ((input_num_buffers != 1) || (output_num_buffers != 0)) {
      (void) fprintf(stderr, "Bad arguments to do_normalization!\n");
      exit(EXIT_FAILURE);
   }

   /* Check to see if the threshold has been set */
   if (!norm_data->threshold_set) {
      find_mincfile_range(get_info_current_mincid(loop_info),
                          &minimum, &maximum);
      norm_data->threshold = minimum + (maximum - minimum) * THRESH_FRACTION;
      norm_data->threshold_set = TRUE;
   }

   /* Loop through the voxels */
   for (ivox=0; ivox < num_voxels*input_vector_length; ivox++) {
      value = input_data[0][ivox];
      if ((value != -DBL_MAX) && (value > norm_data->threshold)) {
         norm_data->sum0 += 1.0;
         norm_data->sum1 += value;
      }
   }

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : find_mincfile_range
@INPUT      : mincid - id of minc file
@OUTPUT     : minimum - minimum for file
              maximum - maximum for file
@RETURNS    : (nothing)
@DESCRIPTION: Routine to find the min and max in a minc file
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 25, 1995 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static void find_mincfile_range(int mincid, double *minimum, double *maximum)
{
   int varid;
   char *varname;
   double sign, value;
   double *extreme;
   long index[MAX_VAR_DIMS], count[MAX_VAR_DIMS];
   int ndims, dim[MAX_VAR_DIMS];
   int idim, imm;
   int old_ncopts;

   *minimum = 0.0;
   *maximum = 1.0;
   for (imm=0; imm < 2; imm++) {

      /* Set up for max or min */
      if (imm == 0) {
         varname = MIimagemin;
         sign = -1.0;
         extreme = minimum;
      }
      else {
         varname = MIimagemax;
         sign = 1.0;
         extreme = maximum;
      }

      /* Get the variable id */
      old_ncopts = ncopts; ncopts = 0;
      varid = ncvarid(mincid, varname);
      ncopts = old_ncopts;
      if (varid == MI_ERROR) continue;

      /* Get the dimension info */
      (void) ncvarinq(mincid, varid, NULL, NULL, &ndims, dim, NULL);
      for (idim=0; idim < ndims; idim++) {
         (void) ncdiminq(mincid, dim[idim], NULL, &count[idim]);
      }
      if (ndims <= 0) {
         ndims = 1;
         count[0] = 1;
      }

      /* Loop through values, getting extrema */
      (void) miset_coords(ndims, (long) 0, index);
      *extreme = sign * (-DBL_MAX);
      while (index[0] < count[0]) {
         (void) mivarget1(mincid, varid, index, NC_DOUBLE, NULL, &value);
         if ((value * sign) > (*extreme * sign)) {
            *extreme = value;
         }
         idim = ndims-1;
         index[idim]++;
         while ((index[idim] > count[idim]) && (idim > 0)) {
            idim--;
            index[idim]++;
         }
      }
   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : do_average
@INPUT      : Standard for voxel loop
@OUTPUT     : Standard for voxel loop
@RETURNS    : (nothing)
@DESCRIPTION: Routine to loop through an array of voxels and perform averaging
              of across volumes.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 25, 1995 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static void do_average(void *caller_data, long num_voxels, 
                       int input_num_buffers, int input_vector_length,
                       double *input_data[],
                       int output_num_buffers, int output_vector_length,
                       double *output_data[],
                       Loop_Info *loop_info)
     /* ARGSUSED */
{
   Average_Data *average_data;
   long ivox;
   double value;
   int curfile, curindex;
   int num_out;
   double norm_factor, binmin, binmax, weight;
   int binarize;

   /* Get pointer to window info */
   average_data = (Average_Data *) caller_data;

   /* Check arguments */
   num_out = (average_data->need_sd ? 3 : 2);
   if ((input_num_buffers != 1) || (output_num_buffers != num_out) || 
       (output_vector_length != input_vector_length)) {
      (void) fprintf(stderr, "Bad arguments to do_average!\n");
      exit(EXIT_FAILURE);
   }

   /* Get the normalization factor and binarization range */
   curfile = get_info_current_file(loop_info);
   curindex = get_info_current_index(loop_info);
   norm_factor = average_data->norm_factor[curfile];
   if ((average_data->num_weights <= 0) || (average_data->weights == NULL)) {
      weight = 1.0;
   }
   else {
      if (average_data->averaging_over_dimension) {
         if (curindex >= average_data->num_weights) {
            (void) fprintf(stderr, "Internal error in index!\n");
            exit(EXIT_FAILURE);
         }
         weight = average_data->weights[curindex];
      }
      else {
         if (curfile >= average_data->num_weights) {
            (void) fprintf(stderr, "Internal error in file number!\n");
            exit(EXIT_FAILURE);
         }
         weight = average_data->weights[curfile];
      }
   }
   binarize = average_data->binarize;
   binmin = average_data->binrange[0];
   binmax = average_data->binrange[1];

   /* Loop through the voxels */
   for (ivox=0; ivox < num_voxels*input_vector_length; ivox++) {
      value = input_data[0][ivox];
      if (binarize) {
         value = ( ((value >= binmin) && (value <= binmax)) ? 1.0 : 0.0 );
      }
      if (value != -DBL_MAX) {
         value *= norm_factor;
         output_data[0][ivox] += weight;
         output_data[1][ivox] += value * weight;
         if (average_data->need_sd)
            output_data[2][ivox] += value * value * weight;
      }
   }

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : start_average
@INPUT      : Standard for voxel loop
@OUTPUT     : Standard for voxel loop
@RETURNS    : (nothing)
@DESCRIPTION: Start routine for averaging.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 25, 1995 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static void start_average(void *caller_data, long num_voxels, 
                          int output_num_buffers, int output_vector_length,
                          double *output_data[],
                          Loop_Info *loop_info)
     /* ARGSUSED */
{
   Average_Data *average_data;
   long ivox;
   int num_out;

   /* Get pointer to window info */
   average_data = (Average_Data *) caller_data;

   /* Check arguments */
   num_out = (average_data->need_sd ? 3 : 2);
   if (output_num_buffers != num_out) {
      (void) fprintf(stderr, "Bad arguments to start_average!\n");
      exit(EXIT_FAILURE);
   }

   /* Loop through the voxels */
   for (ivox=0; ivox < num_voxels*output_vector_length; ivox++) {
      output_data[0][ivox] = 0.0;
      output_data[1][ivox] = 0.0;
      if (average_data->need_sd)
         output_data[2][ivox] = 0.0;
   }

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : finish_average
@INPUT      : Standard for voxel loop
@OUTPUT     : Standard for voxel loop
@RETURNS    : (nothing)
@DESCRIPTION: Finish routine for averaging.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 25, 1995 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static void finish_average(void *caller_data, long num_voxels, 
                          int output_num_buffers, int output_vector_length,
                          double *output_data[],
                          Loop_Info *loop_info)
     /* ARGSUSED */
{
   Average_Data *average_data;
   long ivox;
   int num_out;
   double sum0, sum1, sum2, value;

   /* Get pointer to window info */
   average_data = (Average_Data *) caller_data;

   /* Check arguments */
   num_out = (average_data->need_sd ? 3 : 2);
   if (output_num_buffers != num_out) {
      (void) fprintf(stderr, "Bad arguments to finish_average!\n");
      exit(EXIT_FAILURE);
   }

   /* Loop through the voxels */
   for (ivox=0; ivox < num_voxels*output_vector_length; ivox++) {
      sum0 = output_data[0][ivox];
      sum1 = output_data[1][ivox];
      if (sum0 > 0.0) {
         output_data[0][ivox] = sum1 / sum0;
         if (average_data->need_sd) {
            sum2 = output_data[2][ivox];
            if (sum0 > 1.0) {
               value = (sum2 - sum1*sum1 / sum0) / (sum0 - 1.0);
               if (value > 0.0)
                  value = sqrt(value);
               else
                  value = 0.0;
               output_data[1][ivox] = value;
               
            }
            else
               output_data[1][ivox] = 0.0;
         }
      }
      else {
         output_data[0][ivox] = 0.0;
         if (average_data->need_sd)
            output_data[1][ivox] = 0.0;
      }
            
   }

   return;
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
static int get_double_list(char *dst, char *key, char *nextarg)
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
               malloc(num_alloc * sizeof(*double_list));
         }
         else {
            double_list = 
               realloc(double_list, num_alloc * sizeof(*double_list));
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
      free(double_array->values);
   }
   double_array->values = double_list;

   return TRUE;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_file_names
@INPUT      : filelist - name of file from which to read names
@OUTPUT     : num_files - number of files read in
@RETURNS    : Pointer to a NULL-terminated array of file names
@DESCRIPTION: Reads in a list of file names from file filelist or stdin if 
              "-" is specified. Returns NULL if an error occurs. If
              no error occurs, then a pointer to an empty array is 
              returned and num_files is zero.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : March 8, 1995 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static char **read_file_names(char *filelist, int *num_files)
{
#define FILE_NAME_ALLOC_SIZE 10
   char **files;
   int array_size;
   int nfiles;
   FILE *fp;
   char line[PATH_MAX+1];
   int length;

   /* Open the file */
   if (strcmp(filelist, "-") == 0) {
      fp = stdin;
   }
   else {
      fp = fopen(filelist, "r");
      if (fp == NULL) {
         (void) fprintf(stderr, "Error opening file \"%s\"\n", filelist);
         return NULL;
      }
   }

   /* Allocate an initial array and NULL-terminate it */
   array_size = FILE_NAME_ALLOC_SIZE;
   files = malloc(sizeof(*files) * array_size);
   if (files == NULL) {
      (void) fprintf(stderr, "Error allocating memory\n");
      return NULL;
   }
   nfiles = 0;
   files[nfiles] = NULL;

   /* Read in file names */
   while (fgets(line, sizeof(line)/sizeof(line[0]), fp) != NULL) {

      /* Remove a trailing newline and check that there is a name */
      length = strlen(line);
      if ((length > 0) && (line[length-1] == '\n')) {
         line[length-1] = '\0';
         length--;
      }
      if (length == 0) continue;

      /* Make room for names if needed */
      while (nfiles >= array_size-1) {
         array_size += FILE_NAME_ALLOC_SIZE;
         files = realloc(files, sizeof(*files) * array_size);
         if (files == NULL) {
            (void) fprintf(stderr, "Error allocating memory\n");
            return NULL;
         }
      }

      /* Save the name, making sure that the list is NULL-terminated */
      files[nfiles] = strdup(line);
      if (files[nfiles] == NULL) {
         (void) fprintf(stderr, "Error allocating memory\n");
         return NULL;
      }
      nfiles++;
      files[nfiles] = NULL;
   }

   /* Close the file */
   (void) fclose(fp);

   /* Return the number of files */
   *num_files = nfiles;

   return files;
}

