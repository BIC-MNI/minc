/* ----------------------------- MNI Header -----------------------------------
@NAME       : use_the_files.c
@DESCRIPTION: Code to do something with the files copied through GYROCOM.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 23, 1993 (Peter Neelin)
@MODIFIED   : $Log: use_the_files.c,v $
@MODIFIED   : Revision 1.1  1993-11-23 14:12:58  neelin
@MODIFIED   : Initial revision
@MODIFIED   :
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

   /* Print out the file names */
   (void) fprintf(stderr, "\n\nFiles copied:\n");
   for (ifile=0; ifile < num_files; ifile++) {
      (void) fprintf(stderr, "     %s\n", file_list[ifile]);
   }

   return;
}
