/* ----------------------------- MNI Header -----------------------------------
@NAME       : gyrotominc.c
@DESCRIPTION: Program to convert gyrocom files to minc format.
@GLOBALS    : 
@CREATED    : November 26, 1993 (Peter Neelin)
@MODIFIED   : $Log: gyrotominc.c,v $
@MODIFIED   : Revision 2.0  1994-09-28 10:35:30  neelin
@MODIFIED   : Release of minc version 0.2
@MODIFIED   :
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
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

#include <gcomserver.h>
#include <ParseArgv.h>

/* Argument variables */
int Do_logging = LOW_LOGGING;
int clobber = FALSE;
char *outputfile = NULL;

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
   {NULL, ARGV_END, NULL, NULL, NULL}
};

int main(int argc, char *argv[])
{
   int exit_status;

   if (ParseArgv(&argc, argv, argTable, 0) || (argc < 2)) {
      (void) fprintf(stderr, "\nUsage: %s [<options>] files...\n", argv[0]);
      (void) fprintf(stderr, "       %s -help\n\n", argv[0]);
      exit(EXIT_FAILURE);
   }

   exit_status = gyro_to_minc(argc-1, &argv[1], outputfile, clobber,
                              "", NULL);

   exit(exit_status);
}

