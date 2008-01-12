/* ----------------------------- MNI Header -----------------------------------
@NAME       : minccopy
@INPUT      : argc, argv - command line arguments
@OUTPUT     : (none)
@RETURNS    : status
@DESCRIPTION: Program to copy minc image values from one minc file to 
              another (both files must exist).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : May 13, 1993 (Peter Neelin)
@MODIFIED   : 
 * $Log: minccopy.c,v $
 * Revision 6.5  2008-01-12 19:08:15  stever
 * Add __attribute__ ((unused)) to all rcsid variables.
 *
 * Revision 6.4  2007/12/11 12:43:01  rotor
 *  * added static to all global variables in main programs to avoid linking
 *       problems with libraries (compress in mincconvert and libz for example)
 *
 * Revision 6.3  2006/05/19 00:35:58  bert
 * Add config.h to several files that might need it
 *
 * Revision 6.2  2004/11/01 22:38:38  bert
 * Eliminate all references to minc_def.h
 *
 * Revision 6.1  1999/10/19 14:45:20  neelin
 * Fixed Log subsitutions for CVS
 *
 * Revision 6.0  1997/09/12 13:23:34  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:24:34  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:00:35  neelin
 * Release of minc version 0.4
 *
 * Revision 3.0  1995/05/15  19:31:08  neelin
 * Release of minc version 0.3
 *
 * Revision 2.1  1995/01/23  12:42:00  neelin
 * Change ncopen, ncclose to miopen, miclose.
 *
 * Revision 2.0  94/09/28  10:33:54  neelin
 * Release of minc version 0.2
 * 
 * Revision 1.7  94/09/28  10:33:51  neelin
 * Pre-release
 * 
 * Revision 1.6  93/08/11  15:18:00  neelin
 * Added RCS logging in source.
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
static char rcsid[] __attribute__ ((unused))="$Header: /private-cvsroot/minc/progs/minccopy/minccopy.c,v 6.5 2008-01-12 19:08:15 stever Exp $";
#endif

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <minc.h>
#include <ParseArgv.h>

/* Constants */
#ifndef TRUE
#  define TRUE 1
#  define FALSE 0
#endif

/* Variables used for argument parsing */
static int copy_pixel_values = FALSE;

/* Argument table */
ArgvInfo argTable[] = {
   {"-pixel_values", ARGV_CONSTANT, (char *) TRUE, (char *) &copy_pixel_values,
       "Copy pixel values as is."},
   {"-real_values", ARGV_CONSTANT, (char *) FALSE, (char *) &copy_pixel_values,
       "Copy real pixel intensities (default)."},
   {NULL, ARGV_END, NULL, NULL, NULL}
};

/* Main program */

int main(int argc, char *argv[])
{
   char *infilename, *outfilename;
   int inminc, outminc, inimg, outimg, inicv, outicv;
   int ndims, outndims, indims[MAX_VAR_DIMS], outdims[MAX_VAR_DIMS];
   nc_type indatatype, outdatatype;
   long start[MAX_VAR_DIMS], count[MAX_VAR_DIMS], end[MAX_VAR_DIMS];
   long length, size;
   int idim;
   void *data;

   /* Check arguments */
   if (ParseArgv(&argc, argv, argTable, 0) || (argc != 3)) {
      (void) fprintf(stderr, 
                     "\nUsage: %s [<options>] <infile> <outfile>\n", argv[0]);
      (void) fprintf(stderr,
                       "       %s -help\n\n", argv[0]);
      exit(EXIT_FAILURE);
   }
   infilename  = argv[1];
   outfilename = argv[2];

   /* Open the files */
   inminc = miopen(infilename, NC_NOWRITE);
   outminc = miopen(outfilename, NC_WRITE);

   /* Inquire about the image variables */
   inimg = ncvarid(inminc, MIimage);
   outimg = ncvarid(outminc, MIimage);
   (void) ncvarinq(inminc, inimg, NULL, &indatatype, &ndims, 
                   indims, NULL);
   (void) ncvarinq(outminc, outimg, NULL, &outdatatype, &outndims, 
                   outdims, NULL);

   /* If copying pixel values as is, then check that types are the same */
   if (copy_pixel_values && (indatatype != outdatatype)) {
      (void) fprintf(stderr, 
  "%s: Input and output datatypes must be the same for exact pixel copies.\n",
                     argv[0]);
      exit(EXIT_FAILURE);
   }

   /* Check number of dimensions */
   if (ndims != outndims) {
      (void) fprintf(stderr, 
  "%s: Input and output files have different number of image dimensions.\n",
                     argv[0]);
      exit(EXIT_FAILURE);
   }

   /* Set input file start, count and end vectors for reading a slice
      at a time. Check dimension sizes */
   for (idim=0; idim < ndims; idim++) {
      (void) ncdiminq(inminc, indims[idim], NULL, &end[idim]);
      (void) ncdiminq(outminc, outdims[idim], NULL, &length);
      if (end[idim] != length) {
         (void) fprintf(stderr, 
            "%s: Input and output files have different image dimensions\n",
                        argv[0]);
         exit(EXIT_FAILURE);
      }
   }
   (void) miset_coords(ndims, (long) 0, start);
   (void) miset_coords(ndims, (long) 1, count);
   if (copy_pixel_values)
      size = nctypelen(indatatype);
   else
      size = nctypelen(NC_DOUBLE);
   for (idim=ndims-2; idim < ndims; idim++) {
      count[idim] = end[idim];
      size *= count[idim];
   }

   /* Allocate space */
   data = malloc(size);

   /* Set up image conversion */
   if (!copy_pixel_values) {
      inicv = miicv_create();
      (void) miicv_setint(inicv, MI_ICV_TYPE, NC_DOUBLE);
      (void) miicv_setint(inicv, MI_ICV_DO_NORM, TRUE);
      (void) miicv_setint(inicv, MI_ICV_USER_NORM, TRUE);
      (void) miicv_attach(inicv, inminc, inimg);
      outicv = miicv_create();
      (void) miicv_setint(outicv, MI_ICV_TYPE, NC_DOUBLE);
      (void) miicv_setint(outicv, MI_ICV_DO_NORM, TRUE);
      (void) miicv_setint(outicv, MI_ICV_USER_NORM, TRUE);
      (void) miicv_attach(outicv, outminc, outimg);
   }

   /* Loop over input slices */

   while (start[0] < end[0]) {

      /* Read and write slice */
      if (copy_pixel_values) {
         (void) ncvarget(inminc, inimg, start, count, data);
         (void) ncvarput(outminc, outimg, start, count, data);
      }
      else {
         (void) miicv_get(inicv, start, count, data);
         (void) miicv_put(outicv, start, count, data);
      }

      /* Increment start counter */
      idim = ndims-1;
      start[idim] += count[idim];
      while ( (idim>0) && (start[idim] >= end[idim])) {
         start[idim] = 0;
         idim--;
         start[idim] += count[idim];
      }

   }       /* End loop over slices */

   /* Clean up */
   (void) miclose(outminc);
   (void) miclose(inminc);
   if (!copy_pixel_values) {
      (void) miicv_free(outicv);
      (void) miicv_free(inicv);
   }
   free(data);

   exit(EXIT_SUCCESS);
}

