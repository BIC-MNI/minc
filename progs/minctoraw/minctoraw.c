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
static char rcsid[]="$Header: /private-cvsroot/minc/progs/minctoraw/minctoraw.c,v 1.4 1993-05-05 12:55:50 neelin Exp $";
#endif

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <minc.h>
#include <limits.h>
#include <float.h>
#include <ParseArgv.h>

/* Constants */
#ifndef TRUE
#  define TRUE 1
#  define FALSE 0
#endif
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
int normalize_output = FALSE;

/* Argument table */
ArgvInfo argTable[] = {
   {"-byte", ARGV_CONSTANT, (char *) NC_BYTE, (char *) &output_datatype,
       "Write out data as bytes"},
   {"-short", ARGV_CONSTANT, (char *) NC_SHORT, (char *) &output_datatype,
       "Write out data as short integers"},
   {"-long", ARGV_CONSTANT, (char *) NC_LONG, (char *) &output_datatype,
       "Write out data as long integers"},
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
       "Turn off pixel normalization (Default)"},
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

   /* Open the file */
   mincid = ncopen(filename, NC_NOWRITE);

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
   data = malloc(size);

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
   (void) ncclose(mincid);
   (void) miicv_free(icvid);

   return EXIT_SUCCESS;
}

