/* ----------------------------- MNI Header -----------------------------------
@NAME       : mincmath
@INPUT      : argc, argv - command line arguments
@OUTPUT     : (none)
@RETURNS    : status
@DESCRIPTION: Program to do simple math on minc files
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 28, 1995 (Peter Neelin)
@MODIFIED   : $Log: mincmath.c,v $
@MODIFIED   : Revision 3.0  1995-05-15 19:32:42  neelin
@MODIFIED   : Release of minc version 0.3
@MODIFIED   :
 * Revision 1.2  1995/05/03  16:13:46  neelin
 * Changed default for -copy/-nocopy to depend on number of input files.
 *
 * Revision 1.1  1995/05/03  13:19:56  neelin
 * Initial revision
 *
---------------------------------------------------------------------------- */

#ifndef lint
static char rcsid[]="$Header: /private-cvsroot/minc/progs/mincmath/mincmath.c,v 3.0 1995-05-15 19:32:42 neelin Rel $";
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

#define DEFAULT_DBL DBL_MAX

#define DEFAULT_BOOL -1

/* Typedefs */
typedef enum {
UNSPECIFIED_OP = 0, ADD_OP, SUB_OP, MULT_OP, DIV_OP, SQRT_OP, SQUARE_OP,
SCALE_OP, CLAMP_OP, SEGMENT_OP, NSEGMENT_OP, PERCENTDIFF_OP, 
EQ_OP, NE_OP, GT_OP, GE_OP, LT_OP, LE_OP, AND_OP, OR_OP, NOT_OP, 
ISNAN_OP, NISNAN_OP
} Operation;

typedef enum {
   ILLEGAL_NUMOP, UNARY_NUMOP, BINARY_NUMOP, NARY_NUMOP
} Num_Operands;

/* Table that matches [Operation][Number of constants (0,1,2)] to a number
   of volume operands */
Num_Operands OperandTable[][3] = {
   ILLEGAL_NUMOP, ILLEGAL_NUMOP, ILLEGAL_NUMOP,       /* UNSPECIFIED_OP */
   NARY_NUMOP,    UNARY_NUMOP,   ILLEGAL_NUMOP,       /* ADD_OP */
   BINARY_NUMOP,  UNARY_NUMOP,   ILLEGAL_NUMOP,       /* SUB_OP */
   NARY_NUMOP,    UNARY_NUMOP,   ILLEGAL_NUMOP,       /* MULT_OP */
   BINARY_NUMOP,  UNARY_NUMOP,   ILLEGAL_NUMOP,       /* DIV_OP */
   UNARY_NUMOP,   ILLEGAL_NUMOP, ILLEGAL_NUMOP,       /* SQRT_OP */
   UNARY_NUMOP,   ILLEGAL_NUMOP, ILLEGAL_NUMOP,       /* SQUARE_OP */
   ILLEGAL_NUMOP, UNARY_NUMOP,   UNARY_NUMOP,         /* SCALE_OP */
   ILLEGAL_NUMOP, ILLEGAL_NUMOP, UNARY_NUMOP,         /* CLAMP_OP */
   ILLEGAL_NUMOP, ILLEGAL_NUMOP, UNARY_NUMOP,         /* SEGMENT_OP */
   ILLEGAL_NUMOP, ILLEGAL_NUMOP, UNARY_NUMOP,         /* NSEGMENT_OP */
   BINARY_NUMOP,  BINARY_NUMOP,  ILLEGAL_NUMOP,       /* PERCENTDIFF_OP */
   BINARY_NUMOP,  UNARY_NUMOP,   ILLEGAL_NUMOP,       /* EQ_OP */
   BINARY_NUMOP,  UNARY_NUMOP,   ILLEGAL_NUMOP,       /* NE_OP */
   BINARY_NUMOP,  UNARY_NUMOP,   ILLEGAL_NUMOP,       /* GT_OP */
   BINARY_NUMOP,  UNARY_NUMOP,   ILLEGAL_NUMOP,       /* GE_OP */
   BINARY_NUMOP,  UNARY_NUMOP,   ILLEGAL_NUMOP,       /* LT_OP */
   BINARY_NUMOP,  UNARY_NUMOP,   ILLEGAL_NUMOP,       /* LE_OP */
   NARY_NUMOP,    ILLEGAL_NUMOP, ILLEGAL_NUMOP,       /* AND_OP */
   NARY_NUMOP,    ILLEGAL_NUMOP, ILLEGAL_NUMOP,       /* OR_OP */
   UNARY_NUMOP,   ILLEGAL_NUMOP, ILLEGAL_NUMOP,       /* NOT_OP */
   UNARY_NUMOP,   ILLEGAL_NUMOP, ILLEGAL_NUMOP,       /* ISNAN_OP */
   UNARY_NUMOP,   ILLEGAL_NUMOP, ILLEGAL_NUMOP,       /* NISNAN_OP */
   ILLEGAL_NUMOP, ILLEGAL_NUMOP, ILLEGAL_NUMOP        /* nothing */
};

/* Structure for window information */
typedef struct {
   Operation operation;
   Num_Operands num_operands;
   int num_constants;
   double constants[2];
   int propagate_nan;
   double illegal_value;
} Math_Data;

/* Function prototypes */
public int main(int argc, char *argv[]);
public void do_math(void *caller_data, long num_voxels, 
                    int input_num_buffers, int input_vector_length,
                    double *input_data[],
                    int output_num_buffers, int output_vector_length,
                    double *output_data[],
                    Loop_Info *loop_info);
public void accum_math(void *caller_data, long num_voxels, 
                       int input_num_buffers, int input_vector_length,
                       double *input_data[],
                       int output_num_buffers, int output_vector_length,
                       double *output_data[],
                       Loop_Info *loop_info);
public void start_math(void *caller_data, long num_voxels, 
                       int output_num_buffers, int output_vector_length,
                       double *output_data[],
                       Loop_Info *loop_info);

/* Argument variables */
int clobber = FALSE;
int verbose = TRUE;
int debug = FALSE;
nc_type datatype = NC_UNSPECIFIED;
int is_signed = FALSE;
double valid_range[2] = {0.0, 0.0};
int copy_all_header = DEFAULT_BOOL;
char *loop_dimension = NULL;
int max_buffer_size_in_kb = 4 * 1024;
double constant = DEFAULT_DBL;
double constant2[2] = {DEFAULT_DBL, DEFAULT_DBL};
Operation operation = UNSPECIFIED_OP;
int propagate_nan = TRUE;
int use_nan_for_illegal_values = TRUE;

/* Argument table */
ArgvInfo argTable[] = {
   {NULL, ARGV_HELP, (char *) NULL, (char *) NULL, 
       "General options:"},
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
   {"-copy_header", ARGV_CONSTANT, (char *) TRUE, (char *) &copy_all_header,
       "Copy all of the header from the first file."},
   {"-nocopy_header", ARGV_CONSTANT, (char *) FALSE, (char *) &copy_all_header,
       "Do not copy all of the header from the first file."},
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
   {"-max_buffer_size_in_kb", ARGV_INT, (char *) 1, 
       (char *) &max_buffer_size_in_kb,
       "Specify the maximum size of the internal buffers (in kbytes)."},
   {"-dimension", ARGV_STRING, (char *) 1, (char *) &loop_dimension,
       "Specify a dimension along which we wish to perform a calculation."},
   {"-ignore_nan", ARGV_CONSTANT, (char *) FALSE, (char *) &propagate_nan,
       "Ignore invalid data (NaN) for accumulations."},
   {"-propagate_nan", ARGV_CONSTANT, (char *) TRUE, (char *) &propagate_nan,
       "Invalid data in any file at a voxel produces a NaN (default)."},
   {"-nan", ARGV_CONSTANT, (char *) TRUE, 
       (char *) &use_nan_for_illegal_values,
       "Output NaN when an illegal operation is done (default)."},
   {"-zero", ARGV_CONSTANT, (char *) FALSE, 
       (char *) &use_nan_for_illegal_values,
       "Output zero when an illegal operation is done."},
   {NULL, ARGV_HELP, (char *) NULL, (char *) NULL, 
       "Options for specifying constants:"},
   {"-constant", ARGV_FLOAT, (char *) 1, (char *) &constant,
       "Specify a constant argument."},
   {"-const", ARGV_FLOAT, (char *) 1, (char *) &constant,
       "Synonym for -constant."},
   {"-const2", ARGV_FLOAT, (char *) 2, (char *) constant2,
       "Specify two constant arguments."},
   {NULL, ARGV_HELP, (char *) NULL, (char *) NULL, 
       "Operations:"},
   {"-add", ARGV_CONSTANT, (char *) ADD_OP, (char *) &operation,
       "Add N volumes or volume + constant."},
   {"-sub", ARGV_CONSTANT, (char *) SUB_OP, (char *) &operation,
       "Subtract 2 volumes or volume - constant."},
   {"-mult", ARGV_CONSTANT, (char *) MULT_OP, (char *) &operation,
       "Multiply N volumes or volume * constant."},
   {"-div", ARGV_CONSTANT, (char *) DIV_OP, (char *) &operation,
       "Divide 2 volumes or volume / constant."},
   {"-sqrt", ARGV_CONSTANT, (char *) SQRT_OP, (char *) &operation,
       "Take square root of a volume."},
   {"-square", ARGV_CONSTANT, (char *) SQUARE_OP, (char *) &operation,
       "Take square of a volume."},
   {"-scale", ARGV_CONSTANT, (char *) SCALE_OP, (char *) &operation,
       "Scale a volume: volume * c1 + c2."},
   {"-clamp", ARGV_CONSTANT, (char *) CLAMP_OP, (char *) &operation,
       "Clamp a volume to lie between two values."},
   {"-segment", ARGV_CONSTANT, (char *) SEGMENT_OP, (char *) &operation,
       "Segment a volume: within range = 1, outside range = 0."},
   {"-nsegment", ARGV_CONSTANT, (char *) NSEGMENT_OP, (char *) &operation,
       "Opposite of -segment: within range = 0, outside range = 1."},
   {"-percentdiff", ARGV_CONSTANT, (char *) PERCENTDIFF_OP, 
       (char *) &operation,
       "Percent difference between 2 volumes, thresholded (const def=0.0)."},
   {"-pd", ARGV_CONSTANT, (char *) PERCENTDIFF_OP, (char *) &operation,
       "Synonym for -percentdiff."},
   {"-eq", ARGV_CONSTANT, (char *) EQ_OP, (char *) &operation,
       "Test for integer vol1 == vol2 or vol1 == const."},
   {"-ne", ARGV_CONSTANT, (char *) NE_OP, (char *) &operation,
       "Test for integer vol1 != vol2 or vol1 != const."},
   {"-gt", ARGV_CONSTANT, (char *) GT_OP, (char *) &operation,
       "Test for vol1 > vol2 or vol1 > const."},
   {"-ge", ARGV_CONSTANT, (char *) GE_OP, (char *) &operation,
       "Test for vol1 >= vol2 or vol1 >= const."},
   {"-lt", ARGV_CONSTANT, (char *) LT_OP, (char *) &operation,
       "Test for vol1 < vol2 or vol1 < const."},
   {"-le", ARGV_CONSTANT, (char *) LE_OP, (char *) &operation,
       "Test for vol1 <= vol2 or vol1 <= const."},
   {"-and", ARGV_CONSTANT, (char *) AND_OP, (char *) &operation,
       "Calculate vol1 && vol2 (&& ...)."},
   {"-or", ARGV_CONSTANT, (char *) OR_OP, (char *) &operation,
       "Calculate vol1 || vol2 (|| ...)."},
   {"-not", ARGV_CONSTANT, (char *) NOT_OP, (char *) &operation,
       "Calculate !vol1."},
   {"-isnan", ARGV_CONSTANT, (char *) ISNAN_OP, (char *) &operation,
       "Test for NaN values in vol1."},
   {"-nisnan", ARGV_CONSTANT, (char *) NISNAN_OP, (char *) &operation,
       "Negation of -isnan."},
   {NULL, ARGV_END, NULL, NULL, NULL}
};

/* Main program */

public int main(int argc, char *argv[])
{
   char **infiles, **outfiles;
   int nfiles, nout;
   char *arg_string;
   Math_Data math_data;
   Loop_Options *loop_options;
   char *pname;
   int num_constants;
   Num_Operands num_operands;
   VoxelFunction math_function;

   /* Save time stamp and args */
   arg_string = time_stamp(argc, argv);

   /* Get arguments */
   pname = argv[0];
   if (ParseArgv(&argc, argv, argTable, 0) || (argc < 3)) {
      (void) fprintf(stderr, 
      "\nUsage: %s [options] <in1.mnc> [...] <out.mnc>\n",
                     pname);
      (void) fprintf(stderr, 
        "       %s -help\n\n", pname);
      exit(EXIT_FAILURE);
   }
   nfiles = argc - 2;
   infiles = &argv[1];
   nout = 1;
   outfiles = &argv[argc-1];

   /* Check that the arguments make sense */
   if ((constant != DEFAULT_DBL) && (constant2[0] != DEFAULT_DBL)) {
      (void) fprintf(stderr, "%s: Specify only one of -constant or -const2\n",
                     pname);
      exit(EXIT_FAILURE);
   }
   if (constant != DEFAULT_DBL)
      num_constants = 1;
   else if (constant2[0] != DEFAULT_DBL)
      num_constants = 2;
   else
      num_constants = 0;
   num_operands = OperandTable[operation][num_constants];
   if (num_operands == ILLEGAL_NUMOP) {
      (void) fprintf(stderr, "%s: Operation and constants do not match.\n",
                     pname);
      exit(EXIT_FAILURE);
   }
   if ((num_operands != NARY_NUMOP) && (loop_dimension != NULL)) {
      (void) fprintf(stderr, "%s: Use -dimension only for cumulative ops.\n",
                     pname);
      exit(EXIT_FAILURE);
   }
   if ((num_operands == UNARY_NUMOP) && (nfiles != 1)) {
      (void) fprintf(stderr, "%s: Expected only one input file.\n", pname);
      exit(EXIT_FAILURE);
   }
   if ((num_operands == BINARY_NUMOP) && (nfiles != 2)) {
      (void) fprintf(stderr, "%s: Expected two input files.\n", pname);
      exit(EXIT_FAILURE);
   }
   if ((num_operands == NARY_NUMOP) && (nfiles < 2) && 
       (loop_dimension == NULL)) {
      (void) fprintf(stderr, "%s: Expected at least two input files.\n", 
                     pname);
      exit(EXIT_FAILURE);
   }

   /* Set default copy_all_header according to number of input files */
   if (copy_all_header == DEFAULT_BOOL)
      copy_all_header = (nfiles == 1);

   /* Set up math data structure */
   math_data.operation = operation;
   math_data.num_operands = num_operands;
   math_data.propagate_nan = propagate_nan;
   math_data.num_constants = num_constants;
   switch (num_constants) {
   case 1: 
      math_data.constants[0] = constant;
      break;
   case 2: 
      math_data.constants[0] = constant2[0];
      math_data.constants[1] = constant2[1];
      break;
   }
   if (use_nan_for_illegal_values)
      math_data.illegal_value = -DBL_MAX;
   else
      math_data.illegal_value = 0.0;

   /* Do math */
   loop_options = create_loop_options();
   set_loop_verbose(loop_options, verbose);
   set_loop_clobber(loop_options, clobber);
   set_loop_datatype(loop_options, datatype, is_signed, 
                     valid_range[0], valid_range[1]);
   if (num_operands == NARY_NUMOP) {
      math_function = accum_math;
      set_loop_accumulate(loop_options, TRUE, 0, start_math, NULL);
   }
   else {
      math_function = do_math;
   }
   set_loop_copy_all_header(loop_options, copy_all_header);
   set_loop_dimension(loop_options, loop_dimension);
   set_loop_buffer_size(loop_options, (long) 1024 * max_buffer_size_in_kb);
   voxel_loop(nfiles, infiles, nout, outfiles, arg_string, loop_options,
              math_function, (void *) &math_data);
   free_loop_options(loop_options);

   exit(EXIT_SUCCESS);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : do_math
@INPUT      : Standard for voxel loop
@OUTPUT     : Standard for voxel loop
@RETURNS    : (nothing)
@DESCRIPTION: Routine doing math operations.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 25, 1995 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void do_math(void *caller_data, long num_voxels, 
                    int input_num_buffers, int input_vector_length,
                    double *input_data[],
                    int output_num_buffers, int output_vector_length,
                    double *output_data[],
                    Loop_Info *loop_info)
     /* ARGSUSED */
{
   Math_Data *math_data;
   long ivox;
   double value1, value2;
   double illegal_value;
   Operation operation;
   int num_constants, iconst;
   double constants[2];

   /* Get pointer to window info */
   math_data = (Math_Data *) caller_data;

   /* Check arguments */
   if ((input_num_buffers > 2) || (output_num_buffers != 1) || 
       (output_vector_length != input_vector_length)) {
      (void) fprintf(stderr, "Bad arguments to accum_math!\n");
      exit(EXIT_FAILURE);
   }

   /* Get info */
   operation = math_data->operation;
   num_constants = math_data->num_constants;
   for (iconst=0; iconst < sizeof(constants)/sizeof(constants[0]); iconst++) {
      if (iconst < num_constants)
         constants[iconst] = math_data->constants[iconst];
      else
         constants[iconst] = 0.0;
   }
   illegal_value = math_data->illegal_value;

   /* Set default second value */
   value2 = constants[0];

   /* Loop through the voxels */
   for (ivox=0; ivox < num_voxels*input_vector_length; ivox++) {
      value1 = input_data[0][ivox];
      if (input_num_buffers == 2) 
         value2 = input_data[1][ivox];
      if ((value1 == -DBL_MAX) || (value2 == -DBL_MAX)) {
         switch(operation) {
         case ISNAN_OP:
            output_data[0][ivox] = 1.0;
            break;
         case NISNAN_OP:
            output_data[0][ivox] = 0.0;
            break;
         default:
            output_data[0][ivox] = -DBL_MAX;
            break;
         }
      }
      else {
         switch (operation) {
         case ADD_OP:
            output_data[0][ivox] = value1 + value2; break;
         case SUB_OP:
            output_data[0][ivox] = value1 - value2; break;
         case MULT_OP:
            output_data[0][ivox] = value1 * value2; break;
         case DIV_OP:
            if (value2 != 0.0)
               output_data[0][ivox] = value1 / value2;
            else
               output_data[0][ivox] = illegal_value;
            break;
         case SQRT_OP:
            if (value1 < 0.0)
               output_data[0][ivox] = 0.0;
            else
               output_data[0][ivox] = sqrt(value1);
            break;
         case SQUARE_OP:
            output_data[0][ivox] = value1 * value1; break;
         case SCALE_OP:
            output_data[0][ivox] = value1 * constants[0] + constants[1]; break;
         case CLAMP_OP:
            if (value1 < constants[0])
               value1 = constants[0];
            else if (value1 > constants[1])
               value1 = constants[1];
            output_data[0][ivox] = value1;
            break;
         case SEGMENT_OP:
            if ((value1 < constants[0]) || (value1 > constants[1]))
               output_data[0][ivox] = 0.0;
            else
               output_data[0][ivox] = 1.0;
            break;
         case NSEGMENT_OP:
            if ((value1 < constants[0]) || (value1 > constants[1]))
               output_data[0][ivox] = 1.0;
            else
               output_data[0][ivox] = 0.0;
            break;
         case PERCENTDIFF_OP:
            if ((value1 < constants[0]) || (value1 == 0.0))
               output_data[0][ivox] = illegal_value;
            else {
               output_data[0][ivox] = 100.0 * (value1 - value2) / value1;
            }
            break;
         case EQ_OP:
            output_data[0][ivox] = (((rint(value1)-rint(value2)) == 0.0)
                                    ? 1.0 : 0.0);
            break;
         case NE_OP:
            output_data[0][ivox] = (((rint(value1)-rint(value2)) != 0.0)
                                    ? 1.0 : 0.0);
            break;
         case GT_OP:
            output_data[0][ivox] = value1 > value2; break;
         case GE_OP:
            output_data[0][ivox] = value1 >= value2; break;
         case LT_OP:
            output_data[0][ivox] = value1 < value2; break;
         case LE_OP:
            output_data[0][ivox] = value1 <= value2; break;
         case AND_OP:
            output_data[0][ivox] = 
               (((rint(value1) != 0.0) && (rint(value2) != 0.0)) ? 1.0 : 0.0);
            break;
         case OR_OP:
            output_data[0][ivox] = 
               (((rint(value1) != 0.0) || (rint(value2) != 0.0)) ? 1.0 : 0.0);
            break;
         case NOT_OP:
            output_data[0][ivox] = ((rint(value1) == 0.0) ? 1.0 : 0.0);
            break;
         case ISNAN_OP:
            output_data[0][ivox] = 0.0;     /* To get here, value is not nan */
            break;
         case NISNAN_OP:
            output_data[0][ivox] = 1.0;
            break;
         default:
            (void) fprintf(stderr, "Bad op in do_math!\n");
            exit(EXIT_FAILURE);
         }
      }
   }

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : accum_math
@INPUT      : Standard for voxel loop
@OUTPUT     : Standard for voxel loop
@RETURNS    : (nothing)
@DESCRIPTION: Routine for doing accumulation math operations.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 25, 1995 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void accum_math(void *caller_data, long num_voxels, 
                       int input_num_buffers, int input_vector_length,
                       double *input_data[],
                       int output_num_buffers, int output_vector_length,
                       double *output_data[],
                       Loop_Info *loop_info)
     /* ARGSUSED */
{
   Math_Data *math_data;
   long ivox;
   double value, oldvalue;
   Operation operation;
   int propagate_nan;

   /* Get pointer to window info */
   math_data = (Math_Data *) caller_data;

   /* Check arguments */
   if ((input_num_buffers != 1) || (output_num_buffers != 1) || 
       (output_vector_length != input_vector_length)) {
      (void) fprintf(stderr, "Bad arguments to accum_math!\n");
      exit(EXIT_FAILURE);
   }

   /* Get info */
   operation = math_data->operation;
   propagate_nan = math_data->propagate_nan;

   /* Loop through the voxels */
   for (ivox=0; ivox < num_voxels*input_vector_length; ivox++) {
      oldvalue = output_data[0][ivox];
      if (oldvalue != -DBL_MAX) {
         value = input_data[0][ivox];
         if (value != -DBL_MAX) {
            switch (operation) {
            case ADD_OP:
               output_data[0][ivox] = oldvalue + value;
               break;
            case MULT_OP:
               output_data[0][ivox] = oldvalue * value;
               break;
            case AND_OP:
               output_data[0][ivox] = 
                  (((oldvalue != 0.0) && (rint(value) != 0.0)) ? 1.0 : 0.0);
               break;
            case OR_OP:
               output_data[0][ivox] = 
                  (((oldvalue != 0.0) || (rint(value) != 0.0)) ? 1.0 : 0.0);
               break;
            default:
               (void) fprintf(stderr, "Bad op in accum_math!\n");
               exit(EXIT_FAILURE);
            }
         }
         else if (propagate_nan) {
            output_data[0][ivox] = -DBL_MAX;
         }
      }
   }

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : start_math
@INPUT      : Standard for voxel loop
@OUTPUT     : Standard for voxel loop
@RETURNS    : (nothing)
@DESCRIPTION: Start routine for math accumulation.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 25, 1995 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void start_math(void *caller_data, long num_voxels, 
                       int output_num_buffers, int output_vector_length,
                       double *output_data[],
                       Loop_Info *loop_info)
     /* ARGSUSED */
{
   Math_Data *math_data;
   long ivox;

   /* Get pointer to window info */
   math_data = (Math_Data *) caller_data;

   /* Check arguments */
   if (output_num_buffers != 1) {
      (void) fprintf(stderr, "Bad arguments to start_math!\n");
      exit(EXIT_FAILURE);
   }

   /* Get info */
   operation = math_data->operation;

   /* Loop through the voxels */
   for (ivox=0; ivox < num_voxels*output_vector_length; ivox++) {
      switch (operation) {
      case ADD_OP:
         output_data[0][ivox] = 0.0;
         break;
      case MULT_OP:
         output_data[0][ivox] = 1.0;
         break;
      case AND_OP:
         output_data[0][ivox] = 1.0;
         break;
      case OR_OP:
         output_data[0][ivox] = 0.0;
         break;
      default:
         (void) fprintf(stderr, "Bad op in start_math!\n");
         exit(EXIT_FAILURE);
      }
   }

   return;
}

