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
@MODIFIED   : $Log: mincaverage.c,v $
@MODIFIED   : Revision 3.0  1995-05-15 19:32:44  neelin
@MODIFIED   : Release of minc version 0.3
@MODIFIED   :
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

#ifndef lint
static char rcsid[]="$Header: /private-cvsroot/minc/progs/mincaverage/mincaverage.c,v 3.0 1995-05-15 19:32:44 neelin Rel $";
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <minc.h>
#include <ParseArgv.h>
#include <time_stamp.h>
#include <minc_def.h>
#include <voxel_loop.h>

/* Constants */

#ifndef public
#  define public
#endif

#ifndef TRUE
#  define TRUE 1
#  define FALSE 0
#endif

#define THRESH_FRACTION (1/50.0)

/* Structure for window information */
typedef struct {
   int binarize;
   int need_sd;
   double binrange[2];
   double *norm_factor;
} Average_Data;

typedef struct {
   int threshold_set;
   double threshold;
   double sum0, sum1;
} Norm_Data;

/* Function prototypes */
public int main(int argc, char *argv[]);
public void do_normalization(void *caller_data, long num_voxels, 
                             int input_num_buffers, int input_vector_length,
                             double *input_data[],
                             int output_num_buffers, int output_vector_length,
                             double *output_data[],
                             Loop_Info *loop_info);
public void find_mincfile_range(int mincid, double *minimum, double *maximum);
public void do_average(void *caller_data, long num_voxels, 
                       int input_num_buffers, int input_vector_length,
                       double *input_data[],
                       int output_num_buffers, int output_vector_length,
                       double *output_data[],
                       Loop_Info *loop_info);
public void start_average(void *caller_data, long num_voxels, 
                          int output_num_buffers, int output_vector_length,
                          double *output_data[],
                          Loop_Info *loop_info);
public void finish_average(void *caller_data, long num_voxels, 
                          int output_num_buffers, int output_vector_length,
                          double *output_data[],
                          Loop_Info *loop_info);

/* Argument variables */
int clobber = FALSE;
int verbose = TRUE;
int debug = FALSE;
int check_dimensions = TRUE;
#define NO_DEFAULT_NORM
#ifdef NO_DEFAULT_NORM
int normalize = -1;
#else
int normalize = FALSE;
#endif
char *sdfile = NULL;
nc_type datatype = NC_UNSPECIFIED;
int is_signed = FALSE;
double valid_range[2] = {0.0, 0.0};
int copy_all_header = FALSE;
char *averaging_dimension = NULL;
int max_buffer_size_in_kb = 4 * 1024;
int binarize = FALSE;
double binrange[2] = {DBL_MAX, -DBL_MAX};
double binvalue = -DBL_MAX;

/* Argument table */
ArgvInfo argTable[] = {
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
   {"-check_dimensions", ARGV_CONSTANT, (char *) TRUE, (char *) &check_dimensions,
       "Check that dimension info matches across files (default)."},
   {"-nocheck_dimensions", ARGV_CONSTANT, (char *) FALSE, (char *) &check_dimensions,
       "Do not check dimension info."},
   {"-max_buffer_size_in_kb", ARGV_INT, (char *) 1, 
       (char *) &max_buffer_size_in_kb,
       "Specify the maximum size of the internal buffers (in kbytes)."},
   {"-filetype", ARGV_CONSTANT, (char *) NC_UNSPECIFIED, (char *) &datatype,
       "Use data type of first file (default)."},
   {"-byte", ARGV_CONSTANT, (char *) NC_BYTE, (char *) &datatype,
       "Write out byte data."},
   {"-short", ARGV_CONSTANT, (char *) NC_SHORT, (char *) &datatype,
       "Write out short integer data."},
   {"-long", ARGV_CONSTANT, (char *) NC_LONG, (char *) &datatype,
       "Write out long integer data."},
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
       "Copy all of the header from the first file."},
   {"-nocopy_header", ARGV_CONSTANT, (char *) FALSE, (char *) &copy_all_header,
       "Do not copy all of the header from the first file (default)."},
   {"-avgdim", ARGV_STRING, (char *) 1, (char *) &averaging_dimension,
       "Specify a dimension along which we wish to average."},
   {"-binarize", ARGV_CONSTANT, (char *) TRUE, (char *) &binarize,
       "Binarize the volume by looking for values in a given range."},
   {"-binrange", ARGV_FLOAT, (char *) 2, (char *) binrange,
       "Specify a range for binarization."},
   {"-binvalue", ARGV_FLOAT, (char *) 1, (char *) &binvalue,
       "Specify a target value (+/- 0.5) for binarization."},
   {NULL, ARGV_END, NULL, NULL, NULL}
};

/* Main program */

public int main(int argc, char *argv[])
{
   char **infiles, *outfiles[2];
   int nfiles, nout;
   char *arg_string;
   Norm_Data norm_data;
   Average_Data average_data;
   Loop_Options *loop_options;
   double *vol_mean, vol_total, nvols, global_mean;
   int ifile;

   /* Save time stamp and args */
   arg_string = time_stamp(argc, argv);

   /* Get arguments */
   if (ParseArgv(&argc, argv, argTable, 0) || (argc < 3)) {
      (void) fprintf(stderr, 
      "\nUsage: %s [options] <in1.mnc> [...] <out.mnc>\n",
                     argv[0]);
      (void) fprintf(stderr, 
        "       %s -help\n\n", argv[0]);
      exit(EXIT_FAILURE);
   }
   nfiles = argc - 2;
   infiles = &argv[1];
   outfiles[0] = argv[argc-1];
   outfiles[1] = sdfile;
   nout = ((sdfile == NULL) ? 1 : 2);

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
      MALLOC(sizeof(*average_data.norm_factor) * nfiles);
   if (normalize) {
      vol_mean = MALLOC(sizeof(*vol_mean) * nfiles);
      loop_options = create_loop_options();
      set_loop_verbose(loop_options, FALSE);
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
      FREE(vol_mean);
   }
   else {
      for (ifile=0; ifile < nfiles; ifile++) {
         average_data.norm_factor[ifile] = 1.0;
      }
   }

   /* Do averaging */
   average_data.need_sd = (sdfile != NULL);
   loop_options = create_loop_options();
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
   FREE(average_data.norm_factor);

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
public void do_normalization(void *caller_data, long num_voxels, 
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
public void find_mincfile_range(int mincid, double *minimum, double *maximum)
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
public void do_average(void *caller_data, long num_voxels, 
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
   int curfile;
   int num_out;
   double norm_factor, binmin, binmax;
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
   norm_factor = average_data->norm_factor[curfile];
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
         output_data[0][ivox] += 1.0;
         output_data[1][ivox] += value;
         if (average_data->need_sd)
            output_data[2][ivox] += value * value;
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
public void start_average(void *caller_data, long num_voxels, 
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
public void finish_average(void *caller_data, long num_voxels, 
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

