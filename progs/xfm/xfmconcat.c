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
@MODIFIED   : Revision 1.1  1993-08-13 15:27:18  neelin
@MODIFIED   : Initial revision
@MODIFIED   :
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
static char rcsid[]="$Header: /private-cvsroot/minc/progs/xfm/xfmconcat.c,v 1.1 1993-08-13 15:27:18 neelin Exp $";
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <def_mni.h>
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
   FILE *fp;
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
      if (((fp=fopen(argv[iarg],"r")) == NULL) ||
          (input_transform(fp, input) != OK)) {
         (void) fprintf(stderr, "%s: Error reading transform file %s\n",
                        argv[0], argv[iarg]);
         exit(EXIT_FAILURE);
      }
      fclose(fp);

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
   if (((fp=fopen(argv[output_arg],"w")) == NULL) ||
       (output_transform(fp, NULL, new_result) != OK)) {
      (void) fprintf(stderr, "%s: Error writing transform file %s\n",
                     argv[0], argv[output_arg]);
      exit(EXIT_FAILURE);
   }
   fclose(fp);

   exit(EXIT_SUCCESS);
}
