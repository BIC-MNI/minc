/* ----------------------------- MNI Header -----------------------------------
@NAME       : minctoraw
@INPUT      : argc, argv - command line arguments
@OUTPUT     : (none)
@RETURNS    : status
@DESCRIPTION: Program to dump minc file data
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 11, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */

#ifndef lint
static char rcsid[]="$Header: /private-cvsroot/minc/progs/minctoraw/minctoraw.c,v 1.1 1993-02-11 10:05:19 neelin Exp $";
#endif

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <minc.h>
#include <float.h>

/* Main program */

int main(int argc, char *argv[])
{
   char *filename;
   int mincid, imgid, ndims, dims[MAX_VAR_DIMS];
   nc_type datatype;
   long start[MAX_VAR_DIMS], count[MAX_VAR_DIMS], end[MAX_VAR_DIMS];
   long size;
   int idim;
   void *data;

   /* Check arguments */
   filename = argv[1];
   if ((argc != 2) || (strncmp(filename, "-help", strlen(filename))==0)) {
      (void) fprintf(stderr, "Usage: %s <mincfile>\n", argv[0]);
      exit(EXIT_FAILURE);
   }

   /* Open the file */
   mincid = ncopen(filename, NC_NOWRITE);

   /* Inquire about the image variable */
   imgid = ncvarid(mincid, MIimage);
   (void) ncvarinq(mincid, imgid, NULL, &datatype, &ndims, dims, NULL);

   /* Set input file start, count and end vectors for reading a slice
      at a time */
   for (idim=0; idim < ndims; idim++) {
      (void) ncdiminq(mincid, dims[idim], NULL, &end[idim]);
   }
   (void) miset_coords(ndims, (long) 0, start);
   (void) miset_coords(ndims, (long) 1, count);
   size = nctypelen(datatype);
   for (idim=ndims-2; idim < ndims; idim++) {
      count[idim] = end[idim];
      size *= count[idim];
   }

   /* Allocate space */
   data = malloc(size);

   /* Loop over input slices */

   while (start[0] < end[0]) {

      /* Read in the slice */
      (void) ncvarget(mincid, imgid, start, count, data);

      /* Write out the slice */
      if (fwrite(data, sizeof(char), (size_t) size, stdout) != size) {
         (void) fprintf(stderr, "Error writing data.\n");
         exit(EXIT_FAILURE);
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

   return EXIT_SUCCESS;
}

