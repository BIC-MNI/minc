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
@MODIFIED   : $Log: mincwindow.c,v $
@MODIFIED   : Revision 1.4  1994-09-28 10:36:20  neelin
@MODIFIED   : Pre-release
@MODIFIED   :
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
static char rcsid[]="$Header: /private-cvsroot/minc/progs/mincwindow/mincwindow.c,v 1.4 1994-09-28 10:36:20 neelin Exp $";
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <minc.h>
#include <ParseArgv.h>
#include <time_stamp.h>
#include <minc_def.h>
#include <voxel_loop.h>

#ifndef public
#  define public
#endif

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
public void do_window(void *voxel_data, long nvoxels, double *data);

/* Argument variables */
int clobber = NC_NOCLOBBER;
int verbose = TRUE;

/* Argument table */
ArgvInfo argTable[] = {
   {"-clobber", ARGV_CONSTANT, (char *) NC_CLOBBER, (char *) &clobber,
       "Overwrite existing file."},
   {"-noclobber", ARGV_CONSTANT, (char *) NC_NOCLOBBER, (char *) &clobber,
       "Don't overwrite existing file (default)."},
   {"-verbose", ARGV_CONSTANT, (char *) TRUE, (char *) &verbose,
       "Print out log messages (default)."},
   {"-quiet", ARGV_CONSTANT, (char *) FALSE, (char *) &verbose,
       "Do not print out log messages."},
   {NULL, ARGV_END, NULL, NULL, NULL}
};
/* Main program */

public int main(int argc, char *argv[])
{
   char *infile, *outfile;
   char *arg_string;
   int inmincid, outmincid;
   Window_Data window_data;
   char *endptr;

   /* Save time stamp and args */
   arg_string = time_stamp(argc, argv);

   /* Get arguments */
   if (ParseArgv(&argc, argv, argTable, 0) || 
       (argc < 5) || (argc > 6)) {
      (void) fprintf(stderr, 
      "Usage: %s [options] <in.mnc> <out.mnc> <min> <max> [<newvalue>]\n",
                     argv[0]);
      exit(EXIT_FAILURE);
   }
   infile = argv[1];
   outfile = argv[2];
   window_data.minimum = strtod(argv[3], &endptr);
   if ((endptr == argv[3]) || (*endptr != NULL)) {
      (void) fprintf(stderr, "Cannot get min value from %s\n", argv[3]);
      exit(EXIT_FAILURE);
   }
   window_data.maximum = strtod(argv[4], &endptr);
   if ((endptr == argv[4]) || (*endptr != NULL)) {
      (void) fprintf(stderr, "Cannot get max value from %s\n", argv[4]);
      exit(EXIT_FAILURE);
   }
   if (argc == 6) {
      window_data.use_newvalue = TRUE;
      window_data.newvalue = strtod(argv[5], &endptr);
      if ((endptr == argv[5]) || (*endptr != NULL)) {
         (void) fprintf(stderr, "Cannot get newvalue from %s\n", argv[5]);
         exit(EXIT_FAILURE);
      }
   }
   else {
      window_data.use_newvalue = FALSE;
      window_data.newvalue = 0.0;
   }

   /* Open input file */
   inmincid = miopen(infile, NC_NOWRITE);

   /* Create output file */
   outmincid = micreate(outfile, clobber);

   /* Do loop */
   voxel_loop(inmincid, outmincid, arg_string, verbose,
              do_window, (void *) &window_data);

   /* Close the files */
   (void) miclose(inmincid);
   (void) miclose(outmincid);

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
public void do_window(void *voxel_data, long nvoxels, double *data)
{
   Window_Data *window_data;
   long ivox;

   /* Get pointer to window info */
   window_data = (Window_Data *) voxel_data;

   /* Loop through the voxels */
   for (ivox=0; ivox < nvoxels; ivox++) {
      if (data[ivox] < window_data->minimum) {
         if (window_data->use_newvalue)
            data[ivox] = window_data->newvalue;
         else
            data[ivox] = window_data->minimum;
      }
      else if (data[ivox] > window_data->maximum) {
         if (window_data->use_newvalue)
            data[ivox] = window_data->newvalue;
         else
            data[ivox] = window_data->maximum;
      }
   }

   return;
}
