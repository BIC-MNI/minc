/* ----------------------------- MNI Header -----------------------------------
@NAME       : scxmnem
@INPUT      : argc, argv - command line arguments
@OUTPUT     : (none)
@RETURNS    : error status
@DESCRIPTION: Prints out scanditronix format file mnemonics
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 3, 1993 (Peter Neelin)
@MODIFIED   : $Log: scxmnem.c,v $
@MODIFIED   : Revision 5.0  1997-08-21 13:24:32  neelin
@MODIFIED   : Release of minc version 0.5
@MODIFIED   :
 * Revision 4.0  1997/05/07  20:00:13  neelin
 * Release of minc version 0.4
 *
 * Revision 3.0  1995/05/15  19:31:05  neelin
 * Release of minc version 0.3
 *
 * Revision 2.0  1994/09/28  10:33:49  neelin
 * Release of minc version 0.2
 *
 * Revision 1.2  94/09/28  10:33:36  neelin
 * Pre-release
 * 
 * Revision 1.1  93/11/03  16:51:41  neelin
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
static char rcsid[]="$Header: /private-cvsroot/minc/conversion/scxtominc/scxmnem.c,v 5.0 1997-08-21 13:24:32 neelin Rel $";
#endif

#include <stdlib.h>
#include <stdio.h>
#include <scx_file.h>
#include <minc_def.h>


int main(int argc, char *argv[])
{
   char *pname, *filename, *mnemonic;
   int multiplicity;
   scx_file *file;
   long lvalue;
   float fvalue;
   char svalue[256];

   /* Check arguments */
   pname = argv[0];
   if (argc == 3) {
      filename = argv[1];
      mnemonic = argv[2];
      multiplicity = 0;
   }
   else if (argc == 4) {
      filename = argv[1];
      mnemonic = argv[2];
      multiplicity = atoi(argv[3]);
   }
   else {
      (void) fprintf(stderr, "Usage: %s <scxfile> <mnemonic> [<mult>]\n",
                     pname);
      exit(EXIT_FAILURE);
   }

   /* Open the file */
   if ((file=scx_open(filename)) == NULL) {
      (void) fprintf(stderr, "%s: Error opening file %s\n",
                     pname, filename);
      exit(EXIT_FAILURE);
   }

   if (scx_get_mnem(file, mnemonic, multiplicity, 
                    &lvalue, &fvalue, svalue)) {
      (void) fprintf(stderr, "%s: Error reading mnemnonic %s(%d)\n",
                     pname, mnemonic, multiplicity);
      exit(EXIT_FAILURE);
   }

   /* Print out the result */
   (void) printf("%s(%d): lvalue = %d, fvalue = %g, svalue = %s\n",
                 mnemonic, multiplicity, (int) lvalue, fvalue, svalue);

   /* Close the file */
   scx_close(file);

   exit(EXIT_SUCCESS);

}
