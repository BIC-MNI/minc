/* ----------------------------- MNI Header -----------------------------------
@NAME       : use_the_files.c
@DESCRIPTION: Code to do something with the files copied through GYROCOM.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 23, 1993 (Peter Neelin)
@MODIFIED   : $Log: use_the_files.c,v $
@MODIFIED   : Revision 1.2  1993-11-25 13:27:23  neelin
@MODIFIED   : Working version.
@MODIFIED   :
 * Revision 1.1  93/11/23  14:12:58  neelin
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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : use_the_files
@INPUT      : num_files - number of image files
              file_list - list of file names
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to do something with the files.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 23, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void use_the_files(int num_files, char *file_list[])
{
   int ifile;
   extern int Do_logging;

   /* Print out the file names */
   if (Do_logging >= LOW_LOGGING) {
      (void) fprintf(stderr, "\nFiles copied:\n");
      for (ifile=0; ifile < num_files; ifile++) {
         (void) fprintf(stderr, "     %s\n", file_list[ifile]);
      }
   }

   return;
}
