/* ----------------------------- MNI Header -----------------------------------
@NAME       : gyrotominc.c
@DESCRIPTION: Program to convert gyrocom files to minc format.
@GLOBALS    : 
@CREATED    : November 26, 1993 (Peter Neelin)
@MODIFIED   : $Log: gyrotominc.c,v $
@MODIFIED   : Revision 1.2  1994-01-14 11:37:22  neelin
@MODIFIED   : Fixed handling of multiple reconstructions and image types. Add spiinfo variable with extra info (including window min/max). Changed output
@MODIFIED   : file name to include reconstruction number and image type number.
@MODIFIED   :
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

int Do_logging = LOW_LOGGING;

int main(int argc, char *argv[])
{
   int exit_status;

   if (argc < 2) {
      (void) fprintf(stderr, "Usage: %s files...\n", argv[0]);
      exit(EXIT_FAILURE);
   }

   exit_status = gyro_to_minc(argc-1, &argv[1], NULL, FALSE,
                              "", NULL);

   exit(exit_status);
}

