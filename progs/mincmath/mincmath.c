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
@MODIFIED   : 
 * $Log: mincmath.c,v $
 * Revision 6.15  2009/01/20 11:58:13  rotor
 *  * CMakeLists.txt: updated version
 *  * Updated Changelog to include releases
 *  * Warning cleanups below
 *  * conversion/dcm2mnc/minc_file.c: fixed printf type
 *  * conversion/dcm2mnc/siemens_to_dicom.c: fixed printf type
 *  * conversion/ecattominc/machine_indep.c: added string.h and fixed
 *      2 fprintf missing format args
 *  * conversion/micropet/upet2mnc.c: fixed two fprintf format args
 *  * conversion/minctoecat/ecat_write.c: added string.h
 *  * conversion/minctoecat/minctoecat.c: added missing argument to fprintf
 *  * conversion/nifti1/mnc2nii.c: fixed incorrect printf type
 *  * progs/mincview/invert_raw_image.c: added fwrite checking
 *
 * Revision 6.14  2008/01/17 02:33:02  rotor
 *  * removed all rcsids
 *  * removed a bunch of ^L's that somehow crept in
 *  * removed old (and outdated) BUGS file
 *
 * Revision 6.13  2008/01/13 04:30:28  stever
 * Add braces around static initializers.
 *
 * Revision 6.12  2008/01/12 19:08:15  stever
 * Add __attribute__ ((unused)) to all rcsid variables.
 *
 * Revision 6.11  2007/12/11 12:43:01  rotor
 *  * added static to all global variables in main programs to avoid linking
 *       problems with libraries (compress in mincconvert and libz for example)
 *
 * Revision 6.10  2005/08/26 21:07:17  bert
 * Use #if rather than #ifdef with MINC2 symbol, and be sure to include config.h whereever MINC2 is used
 *
 * Revision 6.9  2004/12/14 23:40:07  bert
 * Get rid of c99 compilation problems
 *
 * Revision 6.8  2004/12/03 21:56:51  bert
 * Include config.h
 *
 * Revision 6.7  2004/11/01 22:38:39  bert
 * Eliminate all references to minc_def.h
 *
 * Revision 6.6  2004/04/27 15:31:45  bert
 * Added -2 option
 *
 * Revision 6.5  2001/04/24 13:38:44  neelin
 * Replaced NC_NAT with MI_ORIGINAL_TYPE.
 *
 * Revision 6.4  2001/04/17 18:40:22  neelin
 * Modifications to work with NetCDF 3.x
 * In particular, changed NC_LONG to NC_INT (and corresponding longs to ints).
 * Changed NC_UNSPECIFIED to NC_NAT.
 * A few fixes to the configure script.
 *
 * Revision 6.3  2001/01/15 14:58:43  neelin
 * Modified description of -segment option.
 *
 * Revision 6.2  2000/07/07 13:19:54  neelin
 * Added option -filelist to read file names from a file. This gets around
 * command-line length limits.
 *
 * Revision 6.1  1999/10/19 14:45:26  neelin
 * Fixed Log subsitutions for CVS
 *
 * Revision 6.0  1997/09/12 13:24:17  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:25:16  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:02:04  neelin
 * Release of minc version 0.4
 *
 * Revision 3.5  1997/04/24  13:48:51  neelin
 * Fixed handling of invalid or uninitialized data for cumulative operations.
 * Added options -illegal_value and -count_valid.
 *
 * Revision 3.4  1997/04/23  19:34:56  neelin
 * Added options -maximum, -minimum, -abs.
 *
 * Revision 3.3  1996/01/17  21:24:06  neelin
 * Added -exp and -log options.
 *
 * Revision 3.2  1996/01/16  13:29:31  neelin
 * Added -invert option.
 *
 * Revision 3.1  1995/12/13  16:22:24  neelin
 * Added -check_dimensions and -nocheck_dimensions options.
 *
 * Revision 3.0  1995/05/15  19:32:42  neelin
 * Release of minc version 0.3
 *
 * Revision 1.2  1995/05/03  16:13:46  neelin
 * Changed default for -copy/-nocopy to depend on number of input files.
 *
 * Revision 1.1  1995/05/03  13:19:56  neelin
 * Initial revision
 *
---------------------------------------------------------------------------- */

#define _GNU_SOURCE 1
#if HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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

/* Data values for invalid data and for uninitialized data */
#define INVALID_DATA -DBL_MAX
#define UNINITIALIZED_DATA DBL_MAX

/* Values for representing default case for command-line options */
#define DEFAULT_DBL DBL_MAX
#define DEFAULT_BOOL -1

/* Typedefs */
typedef enum {
UNSPECIFIED_OP = 0, ADD_OP, SUB_OP, MULT_OP, DIV_OP, SQRT_OP, SQUARE_OP,
SCALE_OP, CLAMP_OP, SEGMENT_OP, NSEGMENT_OP, PERCENTDIFF_OP, 
EQ_OP, NE_OP, GT_OP, GE_OP, LT_OP, LE_OP, AND_OP, OR_OP, NOT_OP, 
ISNAN_OP, NISNAN_OP, INVERT_OP, EXP_OP, LOG_OP, MAX_OP, MIN_OP, ABS_OP,
COUNT_OP
} Operation;

typedef enum {
   ILLEGAL_NUMOP, UNARY_NUMOP, BINARY_NUMOP, NARY_NUMOP
} Num_Operands;

/* Table that matches [Operation][Number of constants (0,1,2)] to a number
   of volume operands */
Num_Operands OperandTable[][3] = {
   { ILLEGAL_NUMOP, ILLEGAL_NUMOP, ILLEGAL_NUMOP },     /* UNSPECIFIED_OP */
   { NARY_NUMOP,    UNARY_NUMOP,   ILLEGAL_NUMOP },     /* ADD_OP */
   { BINARY_NUMOP,  UNARY_NUMOP,   ILLEGAL_NUMOP },     /* SUB_OP */
   { NARY_NUMOP,    UNARY_NUMOP,   ILLEGAL_NUMOP },     /* MULT_OP */
   { BINARY_NUMOP,  UNARY_NUMOP,   ILLEGAL_NUMOP },     /* DIV_OP */
   { UNARY_NUMOP,   ILLEGAL_NUMOP, ILLEGAL_NUMOP },     /* SQRT_OP */
   { UNARY_NUMOP,   ILLEGAL_NUMOP, ILLEGAL_NUMOP },     /* SQUARE_OP */
   { ILLEGAL_NUMOP, UNARY_NUMOP,   UNARY_NUMOP },       /* SCALE_OP */
   { ILLEGAL_NUMOP, ILLEGAL_NUMOP, UNARY_NUMOP },       /* CLAMP_OP */
   { ILLEGAL_NUMOP, ILLEGAL_NUMOP, UNARY_NUMOP },       /* SEGMENT_OP */
   { ILLEGAL_NUMOP, ILLEGAL_NUMOP, UNARY_NUMOP },       /* NSEGMENT_OP */
   { BINARY_NUMOP,  BINARY_NUMOP,  ILLEGAL_NUMOP },     /* PERCENTDIFF_OP */
   { BINARY_NUMOP,  UNARY_NUMOP,   ILLEGAL_NUMOP },     /* EQ_OP */
   { BINARY_NUMOP,  UNARY_NUMOP,   ILLEGAL_NUMOP },     /* NE_OP */
   { BINARY_NUMOP,  UNARY_NUMOP,   ILLEGAL_NUMOP },     /* GT_OP */
   { BINARY_NUMOP,  UNARY_NUMOP,   ILLEGAL_NUMOP },     /* GE_OP */
   { BINARY_NUMOP,  UNARY_NUMOP,   ILLEGAL_NUMOP },     /* LT_OP */
   { BINARY_NUMOP,  UNARY_NUMOP,   ILLEGAL_NUMOP },     /* LE_OP */
   { NARY_NUMOP,    ILLEGAL_NUMOP, ILLEGAL_NUMOP },     /* AND_OP */
   { NARY_NUMOP,    ILLEGAL_NUMOP, ILLEGAL_NUMOP },     /* OR_OP */
   { UNARY_NUMOP,   ILLEGAL_NUMOP, ILLEGAL_NUMOP },     /* NOT_OP */
   { UNARY_NUMOP,   ILLEGAL_NUMOP, ILLEGAL_NUMOP },     /* ISNAN_OP */
   { UNARY_NUMOP,   ILLEGAL_NUMOP, ILLEGAL_NUMOP },     /* NISNAN_OP */
   { UNARY_NUMOP,   UNARY_NUMOP,   ILLEGAL_NUMOP },     /* INVERT_OP */
   { UNARY_NUMOP,   UNARY_NUMOP,   UNARY_NUMOP },       /* EXP_OP */
   { UNARY_NUMOP,   UNARY_NUMOP,   UNARY_NUMOP },       /* LOG_OP */
   { NARY_NUMOP,    ILLEGAL_NUMOP, ILLEGAL_NUMOP },     /* MAX_OP */
   { NARY_NUMOP,    ILLEGAL_NUMOP, ILLEGAL_NUMOP },     /* MIN_OP */
   { UNARY_NUMOP,   ILLEGAL_NUMOP, ILLEGAL_NUMOP },     /* ABS_OP */
   { NARY_NUMOP,    ILLEGAL_NUMOP, ILLEGAL_NUMOP },     /* COUNT_OP */
   { ILLEGAL_NUMOP, ILLEGAL_NUMOP, ILLEGAL_NUMOP }      /* nothing */
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
static void do_math(void *caller_data, long num_voxels, 
                    int input_num_buffers, int input_vector_length,
                    double *input_data[],
                    int output_num_buffers, int output_vector_length,
                    double *output_data[],
                    Loop_Info *loop_info);
static void accum_math(void *caller_data, long num_voxels, 
                       int input_num_buffers, int input_vector_length,
                       double *input_data[],
                       int output_num_buffers, int output_vector_length,
                       double *output_data[],
                       Loop_Info *loop_info);
static void start_math(void *caller_data, long num_voxels, 
                       int output_num_buffers, int output_vector_length,
                       double *output_data[],
                       Loop_Info *loop_info);
static void end_math(void *caller_data, long num_voxels, 
                     int output_num_buffers, int output_vector_length,
                     double *output_data[],
                     Loop_Info *loop_info);
static char **read_file_names(char *filelist, int *num_files);

/* Argument variables */
static int clobber = FALSE;
static int verbose = TRUE;
static int debug = FALSE;
static nc_type datatype = MI_ORIGINAL_TYPE;
static int is_signed = FALSE;
static double valid_range[2] = {0.0, 0.0};
static int copy_all_header = DEFAULT_BOOL;
static char *loop_dimension = NULL;
static int max_buffer_size_in_kb = 4 * 1024;
static double constant = DEFAULT_DBL;
static double constant2[2] = {DEFAULT_DBL, DEFAULT_DBL};
static Operation operation = UNSPECIFIED_OP;
static int propagate_nan = TRUE;
static int use_nan_for_illegal_values = TRUE;
static double value_for_illegal_operations = DEFAULT_DBL;
static int check_dim_info = TRUE;
static char *filelist = NULL;
#if MINC2
static int minc2_format = FALSE;
#endif /* MINC2 */

/* Argument table */
static ArgvInfo argTable[] = {
   {NULL, ARGV_HELP, (char *) NULL, (char *) NULL, 
       "General options:"},
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
   {"-copy_header", ARGV_CONSTANT, (char *) TRUE, (char *) &copy_all_header,
       "Copy all of the header from the first file."},
   {"-nocopy_header", ARGV_CONSTANT, (char *) FALSE, (char *) &copy_all_header,
       "Do not copy all of the header from the first file."},
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
   {"-max_buffer_size_in_kb", ARGV_INT, (char *) 1, 
       (char *) &max_buffer_size_in_kb,
       "Specify the maximum size of the internal buffers (in kbytes)."},
   {"-dimension", ARGV_STRING, (char *) 1, (char *) &loop_dimension,
       "Specify a dimension along which we wish to perform a calculation."},
   {"-check_dimensions", ARGV_CONSTANT, (char *) TRUE, 
       (char *) &check_dim_info,
       "Check that files have matching dimensions (default)."},
   {"-nocheck_dimensions", ARGV_CONSTANT, (char *) FALSE, 
       (char *) &check_dim_info,
       "Do not check that files have matching dimensions."},
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
   {"-illegal_value", ARGV_FLOAT, (char *) 1, 
       (char *) &value_for_illegal_operations,
       "Value to write out when an illegal operation is done."},
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
   {"-invert", ARGV_CONSTANT, (char *) INVERT_OP, (char *) &operation,
       "Calculate 1/x at each voxel (use -constant for c/x)."},
   {"-sqrt", ARGV_CONSTANT, (char *) SQRT_OP, (char *) &operation,
       "Take square root of a volume."},
   {"-square", ARGV_CONSTANT, (char *) SQUARE_OP, (char *) &operation,
       "Take square of a volume."},
   {"-abs", ARGV_CONSTANT, (char *) ABS_OP, (char *) &operation,
       "Take absolute value of a volume."},
   {"-max", ARGV_CONSTANT, (char *) MAX_OP, (char *) &operation,
       "Synonym for -maximum."},
   {"-maximum", ARGV_CONSTANT, (char *) MAX_OP, (char *) &operation,
       "Find maximum of N volumes."},
   {"-minimum", ARGV_CONSTANT, (char *) MIN_OP, (char *) &operation,
       "Find minimum of N volumes."},
   {"-exp", ARGV_CONSTANT, (char *) EXP_OP, (char *) &operation,
       "Calculate c2*exp(c1*x). The constants c1 and c2 default to 1."},
   {"-log", ARGV_CONSTANT, (char *) LOG_OP, (char *) &operation,
       "Calculate log(x/c2)/c1. The constants c1 and c2 default to 1."},
   {"-scale", ARGV_CONSTANT, (char *) SCALE_OP, (char *) &operation,
       "Scale a volume: volume * c1 + c2."},
   {"-clamp", ARGV_CONSTANT, (char *) CLAMP_OP, (char *) &operation,
       "Clamp a volume to lie between two values."},
   {"-segment", ARGV_CONSTANT, (char *) SEGMENT_OP, (char *) &operation,
       "Segment a volume using range of -const2: within range = 1, outside range = 0."},
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
   {"-count_valid", ARGV_CONSTANT, (char *) COUNT_OP, (char *) &operation,
       "Count the number of valid values in N volumes."},
   {NULL, ARGV_END, NULL, NULL, NULL}
};

/* Main program */

int main(int argc, char *argv[])
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
   if (ParseArgv(&argc, argv, argTable, 0) || (argc < 2)) {
      (void) fprintf(stderr, 
      "\nUsage: %s [options] [<in1.mnc> ...] <out.mnc>\n",
                     pname);
      (void) fprintf(stderr, 
        "       %s -help\n\n", pname);
      exit(EXIT_FAILURE);
   }
   nout = 1;
   outfiles = &argv[argc-1];
   
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

   /* Handle special case of COUNT_OP - it always assume -ignore_nan and 
      -zero */
   if (operation == COUNT_OP) {
      propagate_nan = FALSE;
      value_for_illegal_operations = 0.0;
   }

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
   if ((num_operands == NARY_NUMOP) && (nfiles < 1) && 
       (loop_dimension == NULL)) {
      (void) fprintf(stderr, "%s: Expected at least one input files.\n", 
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
   if (value_for_illegal_operations != DEFAULT_DBL)
      math_data.illegal_value = value_for_illegal_operations;
   else if (use_nan_for_illegal_values)
      math_data.illegal_value = INVALID_DATA;
   else
      math_data.illegal_value = 0.0;

   /* Do math */
   loop_options = create_loop_options();
   set_loop_verbose(loop_options, verbose);
   set_loop_clobber(loop_options, clobber);
#if MINC2
   set_loop_v2format(loop_options, minc2_format);
#endif /* MINC2 */
   set_loop_datatype(loop_options, datatype, is_signed, 
                     valid_range[0], valid_range[1]);
   if (num_operands == NARY_NUMOP) {
      math_function = accum_math;
      set_loop_accumulate(loop_options, TRUE, 0, start_math, end_math);
   }
   else {
      math_function = do_math;
   }
   set_loop_copy_all_header(loop_options, copy_all_header);
   set_loop_dimension(loop_options, loop_dimension);
   set_loop_buffer_size(loop_options, (long) 1024 * max_buffer_size_in_kb);
   set_loop_check_dim_info(loop_options, check_dim_info);
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
static void do_math(void *caller_data, long num_voxels, 
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
      (void) fprintf(stderr, "Bad arguments to do_math!\n");
      exit(EXIT_FAILURE);
   }

   /* Get info */
   operation = math_data->operation;
   num_constants = math_data->num_constants;
   for (iconst=0; iconst < sizeof(constants)/sizeof(constants[0]); iconst++) {
      if (iconst < num_constants)
         constants[iconst] = math_data->constants[iconst];
      else if ((operation == INVERT_OP) ||
               (operation == EXP_OP) ||
               (operation == LOG_OP))
         constants[iconst] = 1.0;
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
      if ((value1 == INVALID_DATA) || (value2 == INVALID_DATA)) {
         switch(operation) {
         case ISNAN_OP:
            output_data[0][ivox] = 1.0;
            break;
         case NISNAN_OP:
            output_data[0][ivox] = 0.0;
            break;
         default:
            output_data[0][ivox] = INVALID_DATA;
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
         case INVERT_OP:
            if (value1 == 0.0)
               output_data[0][ivox] = illegal_value;
            else
               output_data[0][ivox] = value2 / value1;
            break;
         case SQRT_OP:
            if (value1 < 0.0)
               output_data[0][ivox] = illegal_value;
            else
               output_data[0][ivox] = sqrt(value1);
            break;
         case SQUARE_OP:
            output_data[0][ivox] = value1 * value1; break;
         case ABS_OP:
            if (value1 < 0.0)
               output_data[0][ivox] = -value1;
            else
               output_data[0][ivox] = value1;
            break;
         case EXP_OP:
            output_data[0][ivox] = constants[1] * exp(value1 * constants[0]);
            break;
         case LOG_OP:
            if ((value1 <= 0.0) || (constants[1] <= 0.0) || 
                (constants[0] == 0.0))
               output_data[0][ivox] = illegal_value;
            else
               output_data[0][ivox] = log(value1/constants[1])/constants[0];
            break;
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
static void accum_math(void *caller_data, long num_voxels, 
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

      /* Get previous value and the next value */
      oldvalue = output_data[0][ivox];
      value = input_data[0][ivox];

      /* If the new data is invalid, then either mark the output as invalid
         or ignore it */
      if (value == INVALID_DATA) {
         if (propagate_nan) {
            output_data[0][ivox] = INVALID_DATA;
         }
      }

      /* If we haven't set anything yet, then just copy the new value */
      else if (oldvalue == UNINITIALIZED_DATA) {
         output_data[0][ivox] = input_data[0][ivox];
      }

      /* Do the operation if the old data and the new data are valid */
      else if (oldvalue != INVALID_DATA) {
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
         case MAX_OP:
            if (value > oldvalue)
               output_data[0][ivox] = value;
            break;
         case MIN_OP:
            if (value < oldvalue)
               output_data[0][ivox] = value;
            break;
         case COUNT_OP:
            output_data[0][ivox]++;
            break;
         default:
            (void) fprintf(stderr, "Bad op in accum_math!\n");
            exit(EXIT_FAILURE);
         }
      }

   }              /* Loop over voxels */

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
static void start_math(void *caller_data, long num_voxels, 
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

   /* Loop through the voxels, marking them all as uninitialized. We treat
      COUNT_OP as a special case since it always has a value. This is 
      especially important to prevent it from going through
      the code in accum_math for handling the first valid voxel which
      just assigns the first value. */
   for (ivox=0; ivox < num_voxels*output_vector_length; ivox++) {
      switch (operation) {
      case COUNT_OP:
         output_data[0][ivox] = 0.0;
         break;
      default:
         output_data[0][ivox] = UNINITIALIZED_DATA;
         break;
      }
   }

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : end_math
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
static void end_math(void *caller_data, long num_voxels, 
                     int output_num_buffers, int output_vector_length,
                     double *output_data[],
                     Loop_Info *loop_info)
     /* ARGSUSED */
{
   Math_Data *math_data;
   long ivox;
   double value;
   double illegal_value;

   /* Get pointer to window info */
   math_data = (Math_Data *) caller_data;

   /* Check arguments */
   if (output_num_buffers != 1) {
      (void) fprintf(stderr, "Bad arguments to end_math!\n");
      exit(EXIT_FAILURE);
   }

   /* Get info */
   operation = math_data->operation;
   illegal_value = math_data->illegal_value;

   /* Loop through the voxels, checking for uninitialized values */
   for (ivox=0; ivox < num_voxels*output_vector_length; ivox++) {
      value = output_data[0][ivox];
      if ((value == UNINITIALIZED_DATA) || (value == INVALID_DATA)) {
         output_data[0][ivox] = illegal_value;
      }
   }

   return;
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

