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
@MODIFIED   : 
 * $Log: xfminvert.c,v $
 * Revision 6.2  2004-02-02 18:24:11  bert
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
 * Revision 2.0  1994/09/28  10:33:26  neelin
 * Release of minc version 0.2
 *
 * Revision 1.5  94/09/28  10:33:20  neelin
 * Pre-release
 * 
 * Revision 1.4  93/10/12  12:52:37  neelin
 * Replaced def_mni.h with volume_io.h
 * 
 * Revision 1.3  93/09/16  09:40:24  neelin
 * Use dave's open_file_with_default_suffix and input_transform_file and
 * output_transform_file to add suffixes to file names.
 * 
 * Revision 1.2  93/09/01  15:59:05  neelin
 * Cast return of fclose to (void).
 * 
 * Revision 1.1  93/08/13  15:26:53  neelin
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
static char rcsid[]="$Header: /private-cvsroot/minc/progs/xfm/xfminvert.c,v 6.2 2004-02-02 18:24:11 bert Exp $";
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <volume_io.h>
#include <ParseArgv.h>
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

/* Argument table */
ArgvInfo argTable[] = {
   {NULL, ARGV_END, NULL, NULL, NULL}
};

/* Main program */

int main(int argc, char *argv[])
{
   General_transform transform, inverse;

   /* Check arguments */
   if (ParseArgv(&argc, argv, argTable, 0) || argc != 3) {
      (void) fprintf(stderr, "Usage: %s <input.xfm> <result.xfm>\n",
                     argv[0]);
      exit(EXIT_FAILURE);
   }

   /* Read in file to invert */
   if (input_transform_file(argv[1], &transform) != OK) {
      (void) fprintf(stderr, "%s: Error reading transform file %s\n",
                     argv[0], argv[1]);
      exit(EXIT_FAILURE);
   }

   /* Invert the transform */
   create_inverse_general_transform(&transform, &inverse);

   /* Write out the transform */
   if (output_transform_file(argv[2], NULL, &inverse) != OK) {
      (void) fprintf(stderr, "%s: Error writing transform file %s\n",
                     argv[0], argv[2]);
      exit(EXIT_FAILURE);
   }

   exit(EXIT_SUCCESS);
}
