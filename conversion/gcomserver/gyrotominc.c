/* ----------------------------- MNI Header -----------------------------------
@NAME       : gyrotominc.c
@DESCRIPTION: Program to convert gyrocom files to minc format.
@GLOBALS    : 
@CREATED    : November 26, 1993 (Peter Neelin)
@MODIFIED   : 
 * $Log: gyrotominc.c,v $
 * Revision 6.4  2008-01-17 02:33:02  rotor
 *  * removed all rcsids
 *  * removed a bunch of ^L's that somehow crept in
 *  * removed old (and outdated) BUGS file
 *
 * Revision 6.3  2008/01/12 19:08:14  stever
 * Add __attribute__ ((unused)) to all rcsid variables.
 *
 * Revision 6.2  2001/04/09 23:02:50  neelin
 * Modified copyright notice, removing permission statement since copying,
 * etc. is probably not permitted by our non-disclosure agreement with
 * Philips.
 *
 * Revision 6.1  1999/10/29 17:52:04  neelin
 * Fixed Log keyword
 *
 * Revision 6.0  1997/09/12 13:23:50  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:24:50  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:01:07  neelin
 * Release of minc version 0.4
 *
 * Revision 3.1  1995/08/02  13:41:36  neelin
 * Added -prefix option.
 *
 * Revision 3.0  1995/05/15  19:31:44  neelin
 * Release of minc version 0.3
 *
 * Revision 2.1  1994/10/20  13:50:14  neelin
 * Write out direction cosines to support rotated volumes.
 * Store single slices as 1-slice volumes (3D instead of 2D).
 * Changed storing of minc history (get args for gyrotominc).
 *
 * Revision 2.0  94/09/28  10:35:30  neelin
 * Release of minc version 0.2
 * 
 * Revision 1.4  94/09/28  10:35:02  neelin
 * Pre-release
 * 
 * Revision 1.3  94/01/18  13:36:42  neelin
 * Added command line options to gyrotominc and fixed error message bug in
 * gyro_to_minc.
 * 
 * Revision 1.2  94/01/14  11:37:22  neelin
 * Fixed handling of multiple reconstructions and image types. Add spiinfo variable with extra info (including window min/max). Changed output
 * file name to include reconstruction number and image type number.
 * 
 * Revision 1.1  93/11/30  14:41:33  neelin
 * Initial revision
 * 
@COPYRIGHT  :
              Copyright 1993 Peter Neelin, McConnell Brain Imaging Centre, 
              Montreal Neurological Institute, McGill University.
---------------------------------------------------------------------------- */

#include <gcomserver.h>
#include <ParseArgv.h>

/* Global for minc history */
extern char *minc_history;

/* Argument variables */
int Do_logging = LOW_LOGGING;
int clobber = FALSE;
char *outputfile = NULL;
char *output_prefix = "";

/* Argument table */
ArgvInfo argTable[] = {
   {"-nologging", ARGV_CONSTANT, (char *) NO_LOGGING, (char *) &Do_logging,
       "Don't print log messages."},
   {"-lowlogging", ARGV_CONSTANT, (char *) LOW_LOGGING, (char *) &Do_logging,
       "Print some log messages (default)."},
   {"-highlogging", ARGV_CONSTANT, (char *) HIGH_LOGGING, (char *) &Do_logging,
       "Print all log messages."},
   {"-clobber", ARGV_CONSTANT, (char *) TRUE, (char *) &clobber,
       "Overwrite existing file."},
   {"-noclobber", ARGV_CONSTANT, (char *) FALSE, (char *) &clobber,
       "Don't overwrite existing file (default)."},
   {"-output", ARGV_STRING, (char *) 1, (char *) &outputfile,
       "Specify name of output file (default = make up name)."},
   {"-prefix", ARGV_STRING, (char *) 1, (char *) &output_prefix,
       "Specify a prefix for internally generated output filenames."},
   {NULL, ARGV_END, NULL, NULL, NULL}
};

int main(int argc, char *argv[])
{
   int exit_status;

   minc_history = time_stamp(argc, argv);

   if (ParseArgv(&argc, argv, argTable, 0) || (argc < 2)) {
      (void) fprintf(stderr, "\nUsage: %s [<options>] files...\n", argv[0]);
      (void) fprintf(stderr, "       %s -help\n\n", argv[0]);
      exit(EXIT_FAILURE);
   }

   exit_status = gyro_to_minc(argc-1, &argv[1], outputfile, clobber,
                              output_prefix, NULL);

   exit(exit_status);
}

