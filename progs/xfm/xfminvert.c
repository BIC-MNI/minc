/* ----------------------------- MNI Header -----------------------------------
@NAME       : xfminvert
@INPUT      : argc, argv - command line arguments
@OUTPUT     : (none)
@RETURNS    : status
@DESCRIPTION: Program to invert an MNI transform file
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 13, 1993 (Peter Neelin)
@MODIFIED   : $Log: xfminvert.c,v $
@MODIFIED   : Revision 1.1  1993-08-13 15:26:53  neelin
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
static char rcsid[]="$Header: /private-cvsroot/minc/progs/xfm/xfminvert.c,v 1.1 1993-08-13 15:26:53 neelin Exp $";
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
   General_transform transform, inverse;

   /* Check arguments */
   if (argc != 3) {
      (void) fprintf(stderr, "Usage: %s <input.xfm> <result.xfm>\n",
                     argv[0]);
      exit(EXIT_FAILURE);
   }

   /* Read in file to invert */
   if (((fp=fopen(argv[1],"r")) == NULL) ||
       (input_transform(fp, &transform) != OK)) {
      (void) fprintf(stderr, "%s: Error reading transform file %s\n",
                     argv[0], argv[1]);
      exit(EXIT_FAILURE);
   }
   fclose(fp);

   /* Invert the transform */
   create_inverse_general_transform(&transform, &inverse);

   /* Write out the transform */
   if (((fp=fopen(argv[2],"w")) == NULL) ||
       (output_transform(fp, NULL, &inverse) != OK)) {
      (void) fprintf(stderr, "%s: Error writing transform file %s\n",
                     argv[0], argv[2]);
      exit(EXIT_FAILURE);
   }
   fclose(fp);

   exit(EXIT_SUCCESS);
}
