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
@MODIFIED   : $Log: xfmconcat.c,v $
@MODIFIED   : Revision 3.0  1995-05-15 19:31:03  neelin
@MODIFIED   : Release of minc version 0.3
@MODIFIED   :
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

#ifndef lint
static char rcsid[]="$Header: /private-cvsroot/minc/progs/xfm/xfmconcat.c,v 3.0 1995-05-15 19:31:03 neelin Rel $";
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <volume_io.h>
#include <minc_def.h>

/* Constants */
#ifndef TRUE
#  define TRUE 1
#  define FALSE 0
#endif
#ifndef public
#  define public
#  define private static
#endif

/* Main program */

int main(int argc, char *argv[])
{
   General_transform trans1, trans2, trans3;
   General_transform *new_result, *old_result, *input, *temp_result;
   int iarg, first_arg, last_arg, output_arg;

   /* Check arguments */
   if (argc < 3) {
      (void) fprintf(stderr, 
                "Usage: %s <input1.xfm> [<input2.xfm> ...] <result.xfm>\n",
                     argv[0]);
      exit(EXIT_FAILURE);
   }

   /* Set up pointers and indices of first and last files to concat */
   new_result = &trans1;
   old_result = &trans2;
   input = &trans3;
   first_arg = 1;
   last_arg = argc-2;
   output_arg = argc-1;

   /* Loop through arguments */

   for (iarg=first_arg; iarg <= last_arg; iarg++) {

      /* Read in file to concatenate */
      if (input_transform_file(argv[iarg], input) != OK) {
         (void) fprintf(stderr, "%s: Error reading transform file %s\n",
                        argv[0], argv[iarg]);
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
   if (output_transform_file(argv[output_arg], NULL, new_result) != OK) {
      (void) fprintf(stderr, "%s: Error writing transform file %s\n",
                     argv[0], argv[output_arg]);
      exit(EXIT_FAILURE);
   }

   exit(EXIT_SUCCESS);
}
