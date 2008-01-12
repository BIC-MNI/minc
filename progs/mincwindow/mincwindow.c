/* ----------------------------- MNI Header -----------------------------------
@NAME       : mincwindow
@INPUT      : argc, argv - command line arguments
@OUTPUT     : (none)
@RETURNS    : status
@DESCRIPTION: Program to limit voxel values to a given range
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 10, 1994 (Peter Neelin)
@MODIFIED   : 
 * $Log: mincwindow.c,v $
 * Revision 6.6  2008-01-12 19:08:15  stever
 * Add __attribute__ ((unused)) to all rcsid variables.
 *
 * Revision 6.5  2007/12/11 12:43:01  rotor
 *  * added static to all global variables in main programs to avoid linking
 *       problems with libraries (compress in mincconvert and libz for example)
 *
 * Revision 6.4  2005/08/26 21:07:18  bert
 * Use #if rather than #ifdef with MINC2 symbol, and be sure to include config.h whereever MINC2 is used
 *
 * Revision 6.3  2004/11/01 22:38:39  bert
 * Eliminate all references to minc_def.h
 *
 * Revision 6.2  2004/04/27 15:28:39  bert
 * Added -2 option
 *
 * Revision 6.1  1999/10/19 14:45:30  neelin
 * Fixed Log subsitutions for CVS
 *
 * Revision 6.0  1997/09/12 13:24:05  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:25:05  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:01:34  neelin
 * Release of minc version 0.4
 *
 * Revision 3.2  1995/11/16  13:15:11  neelin
 * Added include of math.h to get declaration of strtod under SunOs
 *
 * Revision 3.1  1995/10/25  17:36:49  neelin
 * Fixed check of arguments: comparing char to NULL.
 *
 * Revision 3.0  1995/05/15  19:32:20  neelin
 * Release of minc version 0.3
 *
 * Revision 2.4  1995/03/21  14:35:25  neelin
 * Changed usage message and handle vector volumes properly.
 *
 * Revision 2.3  1995/03/21  14:05:00  neelin
 * Modified calls to voxel_loop routines.
 *
 * Revision 2.2  1995/02/08  19:31:47  neelin
 * Moved ARGSUSED statements for irix 5 lint.
 *
 * Revision 2.1  1994/12/14  10:20:23  neelin
 * Changed to use standard (Proglib) voxel_loop routines.
 *
 * Revision 2.0  94/09/28  10:36:27  neelin
 * Release of minc version 0.2
 * 
 * Revision 1.4  94/09/28  10:36:20  neelin
 * Pre-release
 * 
 * Revision 1.3  94/01/12  10:18:33  neelin
 * Added logging. Turned off filling. Added miclose for files.
 * 
 * Revision 1.2  94/01/11  15:56:17  neelin
 * Added optional newvalue argument.
 * 
 * Revision 1.1  94/01/11  15:02:56  neelin
 * Initial revision
 * 
---------------------------------------------------------------------------- */

#ifndef lint
static char rcsid[] __attribute__ ((unused))="$Header: /private-cvsroot/minc/progs/mincwindow/mincwindow.c,v 6.6 2008-01-12 19:08:15 stever Exp $";
#endif

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <minc.h>
#include <ParseArgv.h>
#include <time_stamp.h>
#include <voxel_loop.h>

#ifndef TRUE
#  define TRUE 1
#  define FALSE 0
#endif

/* Structure for window information */
typedef struct {
   int use_newvalue;
   double newvalue;
   double minimum;
   double maximum;
} Window_Data;

/* Function prototypes */
static void do_window(void *caller_data, long num_voxels, 
                      int input_num_buffers, int input_vector_length,
                      double *input_data[],
                      int output_num_buffers, int output_vector_length,
                      double *output_data[],
                      Loop_Info *loop_info);

/* Argument variables */
static int clobber = FALSE;
static int verbose = TRUE;
#if MINC2
static int v2format = FALSE;
#endif /* MINC2 */

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
   {NULL, ARGV_END, NULL, NULL, NULL}
};
/* Main program */

int main(int argc, char *argv[])
{
   char *infile, *outfile;
   char *arg_string;
   Window_Data window_data;
   char *endptr;
   Loop_Options *loop_options;

   /* Save time stamp and args */
   arg_string = time_stamp(argc, argv);

   /* Get arguments */
   if (ParseArgv(&argc, argv, argTable, 0) || 
       (argc < 5) || (argc > 6)) {
      (void) fprintf(stderr, 
      "\nUsage: %s [options] <in.mnc> <out.mnc> <min> <max> [<newvalue>]\n",
                     argv[0]);
      (void) fprintf(stderr, 
        "Usage: %s -help\n\n", argv[0]);
      exit(EXIT_FAILURE);
   }
   infile = argv[1];
   outfile = argv[2];
   window_data.minimum = strtod(argv[3], &endptr);
   if ((endptr == argv[3]) || (*endptr != '\0')) {
      (void) fprintf(stderr, "Cannot get min value from %s\n", argv[3]);
      exit(EXIT_FAILURE);
   }
   window_data.maximum = strtod(argv[4], &endptr);
   if ((endptr == argv[4]) || (*endptr != '\0')) {
      (void) fprintf(stderr, "Cannot get max value from %s\n", argv[4]);
      exit(EXIT_FAILURE);
   }
   if (argc == 6) {
      window_data.use_newvalue = TRUE;
      window_data.newvalue = strtod(argv[5], &endptr);
      if ((endptr == argv[5]) || (*endptr != '\0')) {
         (void) fprintf(stderr, "Cannot get newvalue from %s\n", argv[5]);
         exit(EXIT_FAILURE);
      }
   }
   else {
      window_data.use_newvalue = FALSE;
      window_data.newvalue = 0.0;
   }

   /* Do loop */
   loop_options = create_loop_options();
   set_loop_verbose(loop_options, verbose);
   set_loop_clobber(loop_options, clobber);
#if MINC2
   set_loop_v2format(loop_options, v2format);
#endif /* MINC2 */
   voxel_loop(1, &infile, 1, &outfile, arg_string, loop_options,
              do_window, (void *) &window_data);

   exit(EXIT_SUCCESS);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : do_window
@INPUT      : voxel_data - pointer to structure containing windowing info
              nvoxels - number of voxels to operate on
              data - array of voxels
@OUTPUT     : data
@RETURNS    : (nothing)
@DESCRIPTION: Routine to loop through an array of voxels and limit the 
              range of values.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 11, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static void do_window(void *caller_data, long num_voxels, 
                      int input_num_buffers, int input_vector_length,
                      double *input_data[],
                      int output_num_buffers, int output_vector_length,
                      double *output_data[],
                      Loop_Info *loop_info)
     /* ARGSUSED */
{
   Window_Data *window_data;
   long ivox;

   /* Get pointer to window info */
   window_data = (Window_Data *) caller_data;

   /* Check arguments */
   if ((input_num_buffers != 1) || (output_num_buffers != 1) || 
       (output_vector_length != input_vector_length)) {
      (void) fprintf(stderr, "Bad arguments to do_window!\n");
      exit(EXIT_FAILURE);
   }

   /* Loop through the voxels */
   for (ivox=0; ivox < num_voxels*input_vector_length; ivox++) {
      if (input_data[0][ivox] < window_data->minimum) {
         if (window_data->use_newvalue)
            output_data[0][ivox] = window_data->newvalue;
         else
            output_data[0][ivox] = window_data->minimum;
      }
      else if (input_data[0][ivox] > window_data->maximum) {
         if (window_data->use_newvalue)
            output_data[0][ivox] = window_data->newvalue;
         else
            output_data[0][ivox] = window_data->maximum;
      }
      else {
         output_data[0][ivox] = input_data[0][ivox];
      }
   }

   return;
}
