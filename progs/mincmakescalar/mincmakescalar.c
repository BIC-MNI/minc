/* ----------------------------- MNI Header -----------------------------------
@NAME       : mincmakescalar
@INPUT      : argc, argv - command line arguments
@OUTPUT     : (none)
@RETURNS    : status
@DESCRIPTION: Program to make a scalar minc file from a vector minc file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 7, 1997 (Peter Neelin)
@MODIFIED   : 
 * $Log: mincmakescalar.c,v $
 * Revision 6.3.2.2  2005-03-16 19:02:51  bert
 * Port changes from 2.0 branch
 *
 * Revision 6.3.2.1  2005/01/25 21:01:52  bert
 * Check for proper placement of vector_dimension
 *
 * Revision 6.3  2001/04/24 13:38:43  neelin
 * Replaced NC_NAT with MI_ORIGINAL_TYPE.
 *
 * Revision 6.2  2001/04/17 18:40:20  neelin
 * Modifications to work with NetCDF 3.x
 * In particular, changed NC_LONG to NC_INT (and corresponding longs to ints).
 * Changed NC_UNSPECIFIED to NC_NAT.
 * A few fixes to the configure script.
 *
 * Revision 6.1  1999/10/19 14:45:25  neelin
 * Fixed Log subsitutions for CVS
 *
 * Revision 6.0  1997/09/12 13:24:20  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:25:19  neelin
 * Release of minc version 0.5
 *
 * Revision 1.2  1997/08/07  16:23:19  neelin
 * Change -length to -magnitude.
 *
 * Revision 1.1  1997/08/07  16:09:07  neelin
 * Initial revision
 *
---------------------------------------------------------------------------- */

#ifndef lint
static char rcsid[]="$Header: /private-cvsroot/minc/progs/mincmakescalar/mincmakescalar.c,v 6.3.2.2 2005-03-16 19:02:51 bert Exp $";
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <float.h>
#include <minc.h>
#include <ParseArgv.h>
#include <time_stamp.h>
#include <voxel_loop.h>

#ifndef TRUE
#  define TRUE 1
#  define FALSE 0
#endif

#define DEFAULT_RANGE DBL_MAX
#define NCOPTS_DEFAULT NC_VERBOSE | NC_FATAL
#define INVALID_DATA (-DBL_MAX)

/* Types */
typedef enum 
{CONV_DEFAULT, CONV_AVERAGE, CONV_MAGNITUDE, CONV_GREY, CONV_LINEAR} 
Conversion_Type;

/* Double_Array structure */
typedef struct {
   int numvalues;
   double *values;
} Double_Array;

/* Structure for program info */
typedef struct {
   Conversion_Type conversion_type;
   int num_coefficients;
   double *linear_coefficients;
} Program_Data;

/* Function prototypes */
static void do_makescalar(void *caller_data, long num_voxels,
                          int input_num_buffers, int input_vector_length,
                          double *input_data[],
                          int output_num_buffers, int output_vector_length,
                          double *output_data[], Loop_Info *loop_info);
static int get_double_list(char *dst, char *key, char *nextarg);
static long get_vector_length(int mincid);

/* Argument variables */
int clobber = FALSE;
int verbose = TRUE;
nc_type datatype = MI_ORIGINAL_TYPE;
int is_signed = FALSE;
double valid_range[2] = {0.0, 0.0};
int buffer_size = 10 * 1024;
Conversion_Type conversion_type = CONV_DEFAULT;
Double_Array linear_coefficients = {0, NULL};

/* Argument table */
ArgvInfo argTable[] = {
   {"-clobber", ARGV_CONSTANT, (char *) TRUE, (char *) &clobber,
       "Overwrite existing file."},
   {"-noclobber", ARGV_CONSTANT, (char *) FALSE, (char *) &clobber,
       "Don't overwrite existing file (default)."},
   {"-verbose", ARGV_CONSTANT, (char *) TRUE, (char *) &verbose,
       "Print out log messages (default)."},
   {"-quiet", ARGV_CONSTANT, (char *) FALSE, (char *) &verbose,
       "Do not print out log messages."},
   {"-buffer_size", ARGV_INT, (char *) 1, (char *) &buffer_size,
       "Set the internal buffer size (in kb)."},
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
   {"-valid_range", ARGV_FLOAT, (char *) 2, (char *) valid_range,
       "Valid range for output data."},
   {"-magnitude", ARGV_CONSTANT, (char *) CONV_MAGNITUDE, 
       (char *) &conversion_type,
       "Compute magnitude of vectors (default)."},
   {"-average", ARGV_CONSTANT, (char *) CONV_AVERAGE, 
       (char *) &conversion_type,
       "Average components of vectors."},
   {"-rgbtogrey", ARGV_CONSTANT, (char *) CONV_GREY, (char *) &conversion_type,
       "Convert RGB to greyscale."},
   {"-rgbtogray", ARGV_CONSTANT, (char *) CONV_GREY, (char *) &conversion_type,
       "Synonym for rgbtogrey."},
   {"-linear", ARGV_FUNC, (char *) get_double_list, 
       (char *) &linear_coefficients,
       "Specify comma-separated list of coefficients for linear combination."},
   {NULL, ARGV_END, NULL, NULL, NULL}
};

const char str_wrong_dimension_order[] = {
    "Your input file contains an image with a vector dimension, but the\n"
    "vector dimension isn't the last (i.e. fastest-varying) dimension in\n"
    "the image. Please restructure the file using mincreshape before\n"
    "attempting to use mincmakescalar.\n"
};

/* Main program */

int main(int argc, char *argv[])
{
   char *infile, *outfile;
   char *arg_string;
   int inmincid;
   int input_vector_length;
   int ivalue;
   Loop_Options *loop_options;
   Program_Data program_data;

   /* Save time stamp and args */
   arg_string = time_stamp(argc, argv);

   /* Get arguments */
   if (ParseArgv(&argc, argv, argTable, 0) || (argc != 3)) {
      (void) fprintf(stderr, "\nUsage: %s [options] <in.mnc> <out.mnc>\n",
                     argv[0]);
      (void) fprintf(stderr,   "       %s -help\n\n",
                     argv[0]);
      exit(EXIT_FAILURE);
   }
   infile = argv[1];
   outfile = argv[2];

   /* Check for conflicting options */
   if ((conversion_type != CONV_DEFAULT) && 
       (linear_coefficients.numvalues > 0)) {
      (void) fprintf(stderr, 
                     "Do not specify -linear with other conversion options\n");
      exit(EXIT_FAILURE);
   }

   /* Set up conversion information */
   if (conversion_type == CONV_DEFAULT) conversion_type = CONV_MAGNITUDE;
   program_data.conversion_type = conversion_type;
   program_data.num_coefficients = 0;
   program_data.linear_coefficients = NULL;

   /* Check for coefficients for linear combination */
   if (linear_coefficients.numvalues > 0) {
      conversion_type = CONV_LINEAR;
      program_data.conversion_type = conversion_type;
      program_data.num_coefficients = linear_coefficients.numvalues;
      program_data.linear_coefficients = 
         malloc(linear_coefficients.numvalues * 
                sizeof(*program_data.linear_coefficients));
      for (ivalue=0; ivalue < linear_coefficients.numvalues; ivalue++) {
         program_data.linear_coefficients[ivalue] =
            linear_coefficients.values[ivalue];
      }
   }

   /* Open the input file and get the vector length */
   inmincid = miopen(infile, NC_NOWRITE);
   input_vector_length = get_vector_length(inmincid);
   if (input_vector_length < 0) {
       fprintf(stderr, str_wrong_dimension_order);
       exit(EXIT_FAILURE);
   }
   if (input_vector_length < 1) input_vector_length = 1;

   /* Check that this length is okay */
   if ((conversion_type == CONV_GREY) && 
       (input_vector_length != 3) && (input_vector_length > 1)) {
      (void) fprintf(stderr, "Input file does not contain RGB data\n");
      exit(EXIT_FAILURE);
   }
   if ((conversion_type == CONV_LINEAR) && 
       (input_vector_length != program_data.num_coefficients) && 
       (input_vector_length > 1)) {
      (void) fprintf(stderr, 
         "Input vector length does not match number of linear coefficients\n");
      exit(EXIT_FAILURE);
   }

   /* Set up looping options */
   loop_options = create_loop_options();
   set_loop_clobber(loop_options, clobber);
   set_loop_verbose(loop_options, verbose);
   set_loop_datatype(loop_options, datatype, is_signed, 
                     valid_range[0], valid_range[1]);
   set_loop_output_vector_size(loop_options, 1);
   set_loop_buffer_size(loop_options, (long) buffer_size * 1024);
   set_loop_first_input_mincid(loop_options, inmincid);

   /* Do loop */
   voxel_loop(1, &infile, 1, &outfile, arg_string, loop_options,
              do_makescalar, (void *) &program_data);

   /* Free stuff */
   if (program_data.linear_coefficients != NULL) {
      free(program_data.linear_coefficients);
   }
   if (linear_coefficients.values != NULL) {
      free(linear_coefficients.values);
   }

   exit(EXIT_SUCCESS);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : do_makescalar
@INPUT      : caller_data - pointer to structure containing lookup info
              num_voxels - number of voxels to work on
              input_num_buffers - number of input buffers
              input_vector_length - length of input vector dimension
              input_data - vector of pointers to input buffer data
              output_num_buffers - number of output buffers
              output_vector_length - length of output vector dimension
              start - vector specifying start of hyperslab (not used)
              count - vector specifying count of hyperslab (not used)
@OUTPUT     : output_data - vector of pointers to output buffer data
@RETURNS    : (nothing)
@DESCRIPTION: Routine to loop through an array of vector values and convert
              them to scalar.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 7, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static void do_makescalar(void *caller_data, long num_voxels,
                          int input_num_buffers, int input_vector_length,
                          double *input_data[],
                          int output_num_buffers, int output_vector_length,
                          double *output_data[], Loop_Info *loop_info)
     /* ARGSUSED */
{
   Program_Data *program_data;
   long ivoxel;
   double *input_vector;
   double value, result;
   int ivalue;
   static double grey_coefficients[3] = {0.299, 0.587, 0.114};

   /* Get pointer to lookup info */
   program_data = (Program_Data *) caller_data;

   /* Check that values correspond */
   if ((input_num_buffers != 1) || (output_num_buffers != 1) ||
       (output_vector_length != 1)) {
      (void) fprintf(stderr, "Bad internal values.\n");
      exit(EXIT_FAILURE);
   }
   if ((program_data->conversion_type == CONV_GREY) &&
       (input_vector_length != 3)) {
      (void) fprintf(stderr, 
         "Input must have three components for -grey conversion.\n");
      exit(EXIT_FAILURE);
   }

   /* Loop through the voxels */
   for (ivoxel=0; ivoxel < num_voxels; ivoxel++) {

      /* Handle the special case of scalar input */
      if (input_vector_length <= 1) {
         output_data[0][ivoxel] = input_data[0][ivoxel];
         continue;
      }

      /* Get location of input value */
      input_vector = &input_data[0][ivoxel*input_vector_length];

      /* Loop over components of vector */
      result = 0.0;
      for (ivalue=0; ivalue < input_vector_length; ivalue++) {

         /* Get value and check for invalid values */
         value = input_vector[ivalue];
         if (value == INVALID_DATA) {
            result = INVALID_DATA;
            break;
         }

         /* Sum things up according to scheme */
         switch (program_data->conversion_type) {
         case CONV_AVERAGE:
            result += value;
            break;
         case CONV_MAGNITUDE:
            result += value * value;
            break;
         case CONV_GREY:
            result += grey_coefficients[ivalue] * value;
            break;
         case CONV_LINEAR:
            result += program_data->linear_coefficients[ivalue] * value;
            break;
         }

      }     /* End of loop over components */

      /* Finish up the calculation */
      if (result != INVALID_DATA) {
         switch (program_data->conversion_type) {
         case CONV_AVERAGE:
            result /= (double) input_vector_length;
            break;
         case CONV_MAGNITUDE:
            result = sqrt(result);
            break;
         }
      }

      /* Save the result */
      output_data[0][ivoxel] = result;
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
@NAME       : get_vector_length
@INPUT      : mincid - minc file id
@OUTPUT     : (none)
@RETURNS    : Length of vector dimension or zero if no such dimension.
@DESCRIPTION: Routine to get the length of the vector dimension in a minc file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 30, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static long get_vector_length(int mincid)
{
   int imgid;
   int ndims;
   int dim[MAX_VAR_DIMS];
   char dimname[MAX_NC_NAME];
   long vector_length;
   int i;

   /* Get image variable id */
   imgid = ncvarid(mincid, MIimage);

   /* Get the image dimension info */
   (void) ncvarinq(mincid, imgid, NULL, NULL, &ndims, dim, NULL);

   /* Check for vector dimension */
   (void) ncdiminq(mincid, dim[ndims-1], dimname, &vector_length);
   if ((strcmp(dimname, MIvector_dimension) != 0) || (ndims <= 2)) {
      vector_length = 0;

      /* New deal - check for an actual vector_dimension anywhere in the
       * file.
       */
      for (i = 0; i < ndims; i++) {
          ncdiminq(mincid, dim[i], dimname, NULL);
          if (!strcmp(dimname, MIvector_dimension)) {
              vector_length = -1;
              break;
          }
      }
   }

   return vector_length;
}

