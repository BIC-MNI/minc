/* ----------------------------- MNI Header -----------------------------------
@NAME       : xfmconcat
@INPUT      : argc, argv - command line arguments
@OUTPUT     : (none)
@RETURNS    : status
@DESCRIPTION: Program to concatenate MNI transform files
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 13, 1993 (Peter Neelin)
@MODIFIED   : 
 * $Log: xfmconcat.c,v $
 * Revision 6.7  2008-09-04 03:20:16  rotor
 *  * added -clobber option to xfmconcat and readied for 2.0.16 release
 *
 * Revision 6.6  2008/01/23 22:54:35  rotor
 *  * added Claude to AUTHORS
 *  * added a patch for history to xfmconcat from Mishkin Derakhshan
 *
 * Revision 6.5  2008/01/17 02:33:06  rotor
 *  * removed all rcsids
 *  * removed a bunch of ^L's that somehow crept in
 *  * removed old (and outdated) BUGS file
 *
 * Revision 6.4  2008/01/12 19:08:15  stever
 * Add __attribute__ ((unused)) to all rcsid variables.
 *
 * Revision 6.3  2004/11/01 22:38:39  bert
 * Eliminate all references to minc_def.h
 *
 * Revision 6.2  2004/02/02 18:24:11  bert
 * Call ParseArgv() so that version information will be available
 *
 * Revision 6.1  1999/10/19 14:45:32  neelin
 * Fixed Log subsitutions for CVS
 *
 * Revision 6.0  1997/09/12 13:23:28  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:24:29  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:00:08  neelin
 * Release of minc version 0.4
 *
 * Revision 3.0  1995/05/15  19:31:03  neelin
 * Release of minc version 0.3
 *
 * Revision 2.0  1994/09/28  10:33:25  neelin
 * Release of minc version 0.2
 *
 * Revision 1.5  94/09/28  10:33:20  neelin
 * Pre-release
 * 
 * Revision 1.4  93/10/12  12:52:35  neelin
 * Replaced def_mni.h with volume_io.h
 * 
 * Revision 1.3  93/09/16  09:40:21  neelin
 * Use dave's open_file_with_default_suffix and input_transform_file and
 * output_transform_file to add suffixes to file names.
 * 
 * Revision 1.2  93/09/01  15:58:49  neelin
 * Cast return of fclose to (void).
 * 
 * Revision 1.1  93/08/13  15:27:18  neelin
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

#include <config.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <volume_io.h>
#include <ParseArgv.h>
 #include <time_stamp.h>

/* Constants */
#ifndef TRUE
#  define TRUE 1
#  define FALSE 0
#endif

/* Argument variables */
int clobber = FALSE;
int verbose = FALSE;

/* Argument table */
ArgvInfo argTable[] = {
   {"-clobber", ARGV_CONSTANT, (char *) TRUE, (char *) &clobber,
       "Overwrite existing file."},
   {"-noclobber", ARGV_CONSTANT, (char *) FALSE, (char *) &clobber,
       "Don't overwrite existing file (default)."},
   {"-verbose", ARGV_CONSTANT, (char *) TRUE, (char *) &verbose,
       "Print out extra information."},
   
   {NULL, ARGV_HELP, NULL, NULL, ""},
   {NULL, ARGV_END, NULL, NULL, NULL}
};

/* Main program */
int main(int argc, char *argv[]){
   General_transform trans1, trans2, trans3;
   General_transform *new_result, *old_result, *input, *temp_result;
   int iarg, first_arg, last_arg;
   char *outfile;
   char *arg_string;
   char *pname;
   
   /* collect the command line for history */
   arg_string = time_stamp(argc, argv);
   
   /* Check arguments */
   pname = argv[0];
   if (ParseArgv(&argc, argv, argTable, 0) || argc < 3) {
      (void) fprintf(stderr, 
                "Usage: %s <input1.xfm> [<input2.xfm> ...] <result.xfm>\n",
                     pname);
      (void) fprintf(stderr, 
        "       %s -help\n\n", pname);
      exit(EXIT_FAILURE);
   }

   /* Set up pointers and indices of first and last files to concat */
   new_result = &trans1;
   old_result = &trans2;
   input = &trans3;
   first_arg = 1;
   last_arg = argc-2;
   outfile = argv[argc-1];

   /* check for the outfile */
   if(access(outfile, F_OK) == 0 && !clobber){
      fprintf(stderr, "%s: %s exists! (use -clobber to overwrite)\n\n", pname, outfile);
      exit(EXIT_FAILURE);
   }
   
   /* Loop through arguments */
   for (iarg=first_arg; iarg <= last_arg; iarg++) {

      /* Read in file to concatenate */
      if (input_transform_file(argv[iarg], input) != OK) {
         (void) fprintf(stderr, "%s: Error reading transform file %s\n",
                        pname, argv[iarg]);
         exit(EXIT_FAILURE);
      }

      /* Concatenate the transform */
      temp_result = new_result;
      new_result = old_result;
      old_result = temp_result;
      if (iarg == first_arg) {
         copy_general_transform(input, new_result);
      }
      else {
         concat_general_transforms(old_result, input, new_result);
         delete_general_transform(old_result);
      }
      delete_general_transform(input);

   }     /* End of loop through arguments */

   /* Write out the transform */
   if (output_transform_file(outfile, arg_string, new_result) != OK) {
      (void) fprintf(stderr, "%s: Error writing transform file %s\n",
                     pname, outfile);
      exit(EXIT_FAILURE);
   }

   exit(EXIT_SUCCESS);
}
