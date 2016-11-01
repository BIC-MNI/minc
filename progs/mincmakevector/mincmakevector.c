/* ----------------------------- MNI Header -----------------------------------
@NAME       : mincmakevector
@INPUT      : argc, argv - command line arguments
@OUTPUT     : (none)
@RETURNS    : status
@DESCRIPTION: Program to make a vector minc file from a series of scalar 
              minc files.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 11, 1997 (Peter Neelin)
@MODIFIED   : 
 * $Log: mincmakevector.c,v $
 * Revision 6.10  2008/01/17 02:33:02  rotor
 *  * removed all rcsids
 *  * removed a bunch of ^L's that somehow crept in
 *  * removed old (and outdated) BUGS file
 *
 * Revision 6.9  2008/01/12 19:08:15  stever
 * Add __attribute__ ((unused)) to all rcsid variables.
 *
 * Revision 6.8  2007/12/11 12:43:01  rotor
 *  * added static to all global variables in main programs to avoid linking
 *       problems with libraries (compress in mincconvert and libz for example)
 *
 * Revision 6.7  2005/08/26 21:07:17  bert
 * Use #if rather than #ifdef with MINC2 symbol, and be sure to include config.h whereever MINC2 is used
 *
 * Revision 6.6  2004/11/01 22:38:38  bert
 * Eliminate all references to minc_def.h
 *
 * Revision 6.5  2004/04/30 19:53:30  bert
 * Remove unused variable
 *
 * Revision 6.4  2004/04/27 15:32:15  bert
 * Added -2 option
 *
 * Revision 6.3  2001/04/24 13:38:44  neelin
 * Replaced NC_NAT with MI_ORIGINAL_TYPE.
 *
 * Revision 6.2  2001/04/17 18:40:21  neelin
 * Modifications to work with NetCDF 3.x
 * In particular, changed NC_LONG to NC_INT (and corresponding longs to ints).
 * Changed NC_UNSPECIFIED to NC_NAT.
 * A few fixes to the configure script.
 *
 * Revision 6.1  1999/10/19 14:45:25  neelin
 * Fixed Log subsitutions for CVS
 *
 * Revision 6.0  1997/09/12 13:24:31  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:25:32  neelin
 * Release of minc version 0.5
 *
 * Revision 1.1  1997/08/12  19:03:46  neelin
 * Initial revision
 *
---------------------------------------------------------------------------- */

#if HAVE_CONFIG_H
#include "config.h"
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

/* Structure for program info */
typedef struct {
   int dummy;
} Program_Data;

/* Function prototypes */
static void do_makevector(void *caller_data, long num_voxels,
                          int input_num_buffers, int input_vector_length,
                          double *input_data[],
                          int output_num_buffers, int output_vector_length,
                          double *output_data[], Loop_Info *loop_info);
static long get_vector_length(int mincid);

/* Argument variables */
static int clobber = FALSE;
static int verbose = TRUE;
#if MINC2
static int v2format = FALSE;
#endif /* MINC2 */
static nc_type datatype = MI_ORIGINAL_TYPE;
static int is_signed = FALSE;
static double valid_range[2] = {0.0, 0.0};
static int buffer_size = 10 * 1024;

/* Argument table */
static ArgvInfo argTable[] = {
#if MINC2
    {"-2", ARGV_CONSTANT, (char *) TRUE, (char *) &v2format,
       "Produce a MINC 2.0 format output file."},
#endif /* MINC2 */
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
   {NULL, ARGV_END, NULL, NULL, NULL}
};

/* Main program */

int main(int argc, char *argv[])
{
   char **input_files;
   char *output_file;
   char *arg_string;
   int num_input_files;
   int inmincid;
   Loop_Options *loop_options;
   Program_Data program_data;

   /* Save time stamp and args */
   arg_string = time_stamp(argc, argv);

   /* Get arguments */
   if (ParseArgv(&argc, argv, argTable, 0) || (argc < 3)) {
      (void) fprintf(stderr, 
                     "\nUsage: %s [options] <in1.mnc> [...] <out.mnc>\n",
                     argv[0]);
      (void) fprintf(stderr,
                     "       %s -help\n\n",
                     argv[0]);
      exit(EXIT_FAILURE);
   }
   input_files = &argv[1];
   num_input_files = argc - 2;
   output_file = argv[argc-1];

   /* Open the first input file and get the vector length */
   inmincid = miopen(input_files[0], NC_NOWRITE);
   if (get_vector_length(inmincid) > 1) {
      (void) fprintf(stderr, "Input file %s is not a scalar file\n",
                     input_files[0]);
      exit(EXIT_FAILURE);
   }

   /* Set up looping options */
   loop_options = create_loop_options();
   set_loop_clobber(loop_options, clobber);
   set_loop_verbose(loop_options, verbose);
#if MINC2
   set_loop_v2format(loop_options, v2format);
#endif /* MINC2 */
   set_loop_datatype(loop_options, datatype, is_signed, 
                     valid_range[0], valid_range[1]);
   set_loop_output_vector_size(loop_options, num_input_files);
   set_loop_buffer_size(loop_options, (long) buffer_size * 1024);
   set_loop_first_input_mincid(loop_options, inmincid);
   set_loop_accumulate(loop_options, TRUE, 0, NULL, NULL);

   /* Do loop */
   voxel_loop(num_input_files, input_files, 1, &output_file, 
              arg_string, loop_options,
              do_makevector, (void *) &program_data);

   exit(EXIT_SUCCESS);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : do_makevector
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
@DESCRIPTION: Routine to copy values from scalar files into a vector.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 11, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static void do_makevector(void *caller_data, long num_voxels,
                          int input_num_buffers, int input_vector_length,
                          double *input_data[],
                          int output_num_buffers, int output_vector_length,
                          double *output_data[], Loop_Info *loop_info)
     /* ARGSUSED */
{
   long ivoxel, ovoxel;
   int current_input_file;

   /* Check that values correspond */
   if ((input_num_buffers != 1) || (output_num_buffers != 1) ||
       (input_vector_length != 1)) {
      (void) fprintf(stderr, "Bad internal values.\n");
      exit(EXIT_FAILURE);
   }

   /* Figure out which file we are looking at */
   current_input_file = get_info_current_file(loop_info);

   /* Loop through the voxels */
   for (ivoxel=0, ovoxel = current_input_file; 
        ivoxel < num_voxels; 
        ivoxel++, ovoxel += output_vector_length) {

      output_data[0][ovoxel] = input_data[0][ivoxel];

   }

   return;
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

   /* Get image variable id */
   imgid = ncvarid(mincid, MIimage);

   /* Get the image dimension info */
   (void) ncvarinq(mincid, imgid, NULL, NULL, &ndims, dim, NULL);

   /* Check for vector dimension */
   (void) ncdiminq(mincid, dim[ndims-1], dimname, &vector_length);
   if ((strcmp(dimname, MIvector_dimension) != 0) || (ndims <= 2)) {
      vector_length = 0;
   }

   return vector_length;
}

