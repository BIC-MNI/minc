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
 * $Log: minctoraw.c,v $
 * Revision 6.3  2001-08-16 16:10:50  neelin
 * Force user to specify either -normalize or -nonormalize.
 *
 * Revision 6.2  2001/04/17 18:40:25  neelin
 * Modifications to work with NetCDF 3.x
 * In particular, changed NC_LONG to NC_INT (and corresponding longs to ints).
 * Changed NC_UNSPECIFIED to NC_NAT.
 * A few fixes to the configure script.
 *
 * Revision 6.1  1999/10/19 14:45:30  neelin
 * Fixed Log subsitutions for CVS
 *
 * Revision 6.0  1997/09/12 13:23:26  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:24:27  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:00:01  neelin
 * Release of minc version 0.4
 *
 * Revision 3.0  1995/05/15  19:31:01  neelin
 * Release of minc version 0.3
 *
 * Revision 2.2  1995/01/23  09:05:31  neelin
 * changed ncclose to miclose
 *
 * Revision 2.1  95/01/23  09:03:19  neelin
 * Changed ncopen to miopen.
 * 
 * Revision 2.0  94/09/28  10:33:06  neelin
 * Release of minc version 0.2
 * 
 * Revision 1.8  94/09/28  10:33:03  neelin
 * Pre-release
 * 
 * Revision 1.7  93/08/11  15:23:15  neelin
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
static char rcsid[]="$Header: /private-cvsroot/minc/progs/minctoraw/minctoraw.c,v 6.3 2001-08-16 16:10:50 neelin Exp $";
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <minc.h>
#include <limits.h>
#include <float.h>
#include <ParseArgv.h>
#include <minc_def.h>

/* Constants */
#ifndef TRUE
#  define TRUE 1
#  define FALSE 0
#endif
#define BOOLEAN_DEFAULT -1
static double default_max[][2] = {
   0.0, 0.0,
   UCHAR_MAX, SCHAR_MAX,
   0.0, 0.0,
   USHRT_MAX, SHRT_MAX,
   ULONG_MAX, LONG_MAX,
   1.0, 1.0,
   1.0, 1.0
};
static double default_min[][2] = {
   0.0, 0.0,
   0.0, SCHAR_MIN,
   0.0, 0.0,
   0.0, SHRT_MIN,
   0.0, LONG_MIN,
   0.0, 0.0,
   0.0, 0.0
};

/* Variables used for argument parsing */
nc_type output_datatype = INT_MAX;
int output_signed = INT_MAX;
double valid_range[2] = {DBL_MAX, DBL_MAX};
int normalize_output = BOOLEAN_DEFAULT;

/* Argument table */
ArgvInfo argTable[] = {
   {"-byte", ARGV_CONSTANT, (char *) NC_BYTE, (char *) &output_datatype,
       "Write out data as bytes"},
   {"-short", ARGV_CONSTANT, (char *) NC_SHORT, (char *) &output_datatype,
       "Write out data as short integers"},
   {"-int", ARGV_CONSTANT, (char *) NC_INT, (char *) &output_datatype,
       "Write out data as 32-bit integers"},
   {"-long", ARGV_CONSTANT, (char *) NC_INT, (char *) &output_datatype,
       "Superseded by -int"},
   {"-float", ARGV_CONSTANT, (char *) NC_FLOAT, (char *) &output_datatype,
       "Write out data as single precision floating-point values"},
   {"-double", ARGV_CONSTANT, (char *) NC_DOUBLE, (char *) &output_datatype,
       "Write out data as double precision floating-point values"},
   {"-signed", ARGV_CONSTANT, (char *) TRUE, (char *) &output_signed,
       "Write out signed data"},
   {"-unsigned", ARGV_CONSTANT, (char *) FALSE, (char *) &output_signed,
       "Write out unsigned data"},
   {"-range", ARGV_FLOAT, (char *) 2, (char *) valid_range,
       "Specify the range of output values"},
   {"-normalize", ARGV_CONSTANT, (char *) TRUE, (char *) &normalize_output,
       "Normalize integer pixel values to file max and min"},
   {"-nonormalize", ARGV_CONSTANT, (char *) FALSE, (char *) &normalize_output,
       "Turn off pixel normalization"},
   {NULL, ARGV_END, NULL, NULL, NULL}
};

/* Main program */

int main(int argc, char *argv[])
{
   char *filename;
   int mincid, imgid, icvid, ndims, dims[MAX_VAR_DIMS];
   nc_type datatype;
   int is_signed;
   long start[MAX_VAR_DIMS], count[MAX_VAR_DIMS], end[MAX_VAR_DIMS];
   long size;
   int idim;
   void *data;
   char the_sign[MI_MAX_ATTSTR_LEN];
   int length;
   double temp;

   /* Check arguments */
   if (ParseArgv(&argc, argv, argTable, 0) || (argc != 2)) {
      (void) fprintf(stderr, "\nUsage: %s [<options>] <mincfile>\n", argv[0]);
      (void) fprintf(stderr,   "       %s -help\n\n", argv[0]);
      exit(EXIT_FAILURE);
   }
   filename = argv[1];

   /* Check that a normalization option was specified */
   if (normalize_output == BOOLEAN_DEFAULT) {
      (void) fprintf(stderr, "Please specify either -normalize or -nonormalize
n");
      exit(EXIT_FAILURE);
   }

   /* Open the file */
   mincid = miopen(filename, NC_NOWRITE);

   /* Inquire about the image variable */
   imgid = ncvarid(mincid, MIimage);
   (void) ncvarinq(mincid, imgid, NULL, &datatype, &ndims, dims, NULL);

   /* Check if arguments set */

   /* Get output data type */
   if (output_datatype == INT_MAX) output_datatype = datatype;

   /* Get output sign */ 
   ncopts = 0;
   if (miattgetstr(mincid, imgid, MIsigntype, MI_MAX_ATTSTR_LEN, 
                   the_sign) != NULL) {
      if (strcmp(the_sign, MI_SIGNED) == 0)
         is_signed = TRUE;
      else if (strcmp(the_sign, MI_UNSIGNED) == 0)
         is_signed = FALSE;
      else
         is_signed = (datatype != NC_BYTE);
   }
   else
      is_signed = (datatype != NC_BYTE);
   ncopts = NC_VERBOSE | NC_FATAL;
   if (output_signed == INT_MAX) {
      if (output_datatype == datatype)
         output_signed = is_signed;
      else 
         output_signed = (output_datatype != NC_BYTE);
   }

   /* Get output range */
   if (valid_range[0] == DBL_MAX) {
      if ((output_datatype == datatype) && (output_signed == is_signed)) {
         ncopts = 0;
         if ((miattget(mincid, imgid, MIvalid_range, NC_DOUBLE, 2, 
                       valid_range, &length) == MI_ERROR) || (length!=2)) {
            if (miattget1(mincid, imgid, MIvalid_max, NC_DOUBLE, 
                          &valid_range[1]) == MI_ERROR)
               valid_range[1] = default_max[datatype][is_signed]; 
            if (miattget1(mincid, imgid, MIvalid_min, NC_DOUBLE, 
                          &valid_range[0]) == MI_ERROR)
               valid_range[0] = default_min[datatype][is_signed]; 
         }
         ncopts = NC_VERBOSE | NC_FATAL;
      }
      else {
         valid_range[0] = default_min[output_datatype][output_signed];
         valid_range[1] = default_max[output_datatype][output_signed];
      }
   }
   if (valid_range[0] > valid_range[1]) {
      temp = valid_range[0];
      valid_range[0] = valid_range[1];
      valid_range[1] = temp;
   }

   /* Set up image conversion */
   icvid = miicv_create();
   (void) miicv_setint(icvid, MI_ICV_TYPE, output_datatype);
   (void) miicv_setstr(icvid, MI_ICV_SIGN, (output_signed ? 
                                            MI_SIGNED : MI_UNSIGNED));
   (void) miicv_setdbl(icvid, MI_ICV_VALID_MIN, valid_range[0]);
   (void) miicv_setdbl(icvid, MI_ICV_VALID_MAX, valid_range[1]);
   if ((output_datatype == NC_FLOAT) || (output_datatype == NC_DOUBLE)) {
      (void) miicv_setint(icvid, MI_ICV_DO_NORM, TRUE);
      (void) miicv_setint(icvid, MI_ICV_USER_NORM, TRUE);
   }
   else if (normalize_output) {
      (void) miicv_setint(icvid, MI_ICV_DO_NORM, TRUE);
   }
   (void) miicv_attach(icvid, mincid, imgid);

   /* Set input file start, count and end vectors for reading a slice
      at a time */
   for (idim=0; idim < ndims; idim++) {
      (void) ncdiminq(mincid, dims[idim], NULL, &end[idim]);
   }
   (void) miset_coords(ndims, (long) 0, start);
   (void) miset_coords(ndims, (long) 1, count);
   size = nctypelen(output_datatype);
   for (idim=ndims-2; idim < ndims; idim++) {
      count[idim] = end[idim];
      size *= count[idim];
   }

   /* Allocate space */
   data = MALLOC(size);

   /* Loop over input slices */

   while (start[0] < end[0]) {

      /* Read in the slice */
      (void) miicv_get(icvid, start, count, data);

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

   /* Clean up */
   (void) miclose(mincid);
   (void) miicv_free(icvid);
   FREE(data);

   exit(EXIT_SUCCESS);
}

