/* ----------------------------- MNI Header -----------------------------------
@NAME       : mincextract
@INPUT      : argc, argv - command line arguments
@OUTPUT     : (none)
@RETURNS    : status
@DESCRIPTION: Program to dump a hyperslab of minc file data
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June 10, 1993 (Peter Neelin)
@MODIFIED   : $Log: mincextract.c,v $
@MODIFIED   : Revision 2.2  1995-02-08 19:31:47  neelin
@MODIFIED   : Moved ARGSUSED statements for irix 5 lint.
@MODIFIED   :
 * Revision 2.1  1995/01/23  12:32:52  neelin
 * Changed ncopen, ncclose to miopen, miclose.
 *
 * Revision 2.0  94/09/28  10:34:16  neelin
 * Release of minc version 0.2
 * 
 * Revision 1.12  94/09/28  10:34:12  neelin
 * Pre-release
 * 
 * Revision 1.11  94/04/14  08:45:51  neelin
 * Added options for flipping images.
 * 
 * Revision 1.10  94/04/11  16:12:42  neelin
 * Added -image_range, -image_minimum, -image_maximum.
 * Changed default to -normalize.
 * Changed behaviour so that -byte gives default range and sign even if
 * file is of type byte (must use -filetype to preserve range and sign).
 * 
 * Revision 1.9  93/08/11  15:49:49  neelin
 * get_arg_vector must return a value (TRUE if all goes well).
 * 
 * Revision 1.8  93/08/11  15:44:54  neelin
 * Functions called by ParseArgv must check that nextArg is not NULL.
 * 
 * Revision 1.7  93/08/11  15:20:02  neelin
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
static char rcsid[]="$Header: /private-cvsroot/minc/progs/mincextract/mincextract.c,v 2.2 1995-02-08 19:31:47 neelin Exp $";
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <minc.h>
#include <limits.h>
#include <float.h>
#include <ctype.h>
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
#define VECTOR_SEPARATOR ','
#define TYPE_ASCII  0
#define TYPE_BYTE   1
#define TYPE_SHORT  2
#define TYPE_LONG   3
#define TYPE_FLOAT  4
#define TYPE_DOUBLE 5
#define TYPE_FILE   6
static nc_type nc_type_list[8] = {
   NC_DOUBLE, NC_BYTE, NC_SHORT, NC_LONG, NC_FLOAT, NC_DOUBLE, NC_DOUBLE
};
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

/* Function declarations */
public int get_arg_vector(char *dst, char *key, char *nextArg);

/* Variables used for argument parsing */
int arg_odatatype = TYPE_ASCII;
nc_type output_datatype = NC_DOUBLE;
int output_signed = INT_MAX;
double valid_range[2] = {DBL_MAX, DBL_MAX};
int normalize_output = TRUE;
double image_range[2] = {DBL_MAX, DBL_MAX};
long hs_start[MAX_VAR_DIMS] = {LONG_MIN};
long hs_count[MAX_VAR_DIMS] = {LONG_MIN};
int xdirection = INT_MAX;
int ydirection = INT_MAX;
int zdirection = INT_MAX;
int default_direction = INT_MAX;

/* Argument table */
ArgvInfo argTable[] = {
   {"-ascii", ARGV_CONSTANT, (char *) TYPE_ASCII, (char *) &arg_odatatype,
       "Write out data as ascii strings (default)"},
   {"-byte", ARGV_CONSTANT, (char *) TYPE_BYTE, (char *) &arg_odatatype,
       "Write out data as bytes"},
   {"-short", ARGV_CONSTANT, (char *) TYPE_SHORT, (char *) &arg_odatatype,
       "Write out data as short integers"},
   {"-long", ARGV_CONSTANT, (char *) TYPE_LONG, (char *) &arg_odatatype,
       "Write out data as long integers"},
   {"-float", ARGV_CONSTANT, (char *) TYPE_FLOAT, (char *) &arg_odatatype,
       "Write out data as single precision floating-point values"},
   {"-double", ARGV_CONSTANT, (char *) TYPE_DOUBLE, (char *) &arg_odatatype,
       "Write out data as double precision floating-point values"},
   {"-filetype", ARGV_CONSTANT, (char *) TYPE_FILE, (char *) &arg_odatatype,
       "Write out data in the type of the file"},
   {"-signed", ARGV_CONSTANT, (char *) TRUE, (char *) &output_signed,
       "Write out signed data"},
   {"-unsigned", ARGV_CONSTANT, (char *) FALSE, (char *) &output_signed,
       "Write out unsigned data"},
   {"-range", ARGV_FLOAT, (char *) 2, (char *) valid_range,
       "Specify the range of output values"},
   {"-normalize", ARGV_CONSTANT, (char *) TRUE, (char *) &normalize_output,
       "Normalize integer pixel values to file max and min (Default)"},
   {"-nonormalize", ARGV_CONSTANT, (char *) FALSE, (char *) &normalize_output,
       "Turn off pixel normalization"},
   {"-image_range", ARGV_FLOAT, (char *) 2, (char *) image_range,
       "Specify the range of real image values for normalization"},
   {"-image_minimum", ARGV_FLOAT, (char *) 1, (char *) &image_range[0],
       "Specify the minimum real image value for normalization"},
   {"-image_maximum", ARGV_FLOAT, (char *) 1, (char *) &image_range[1],
       "Specify the maximum real image value for normalization"},
   {"-start", ARGV_FUNC, (char *) get_arg_vector, (char *) hs_start,
       "Specifies corner of hyperslab (C conventions for indices)"},
   {"-count", ARGV_FUNC, (char *) get_arg_vector, (char *) hs_count,
       "Specifies edge lengths of hyperslab to read"},
   {"-positive_direction", ARGV_CONSTANT, (char *) MI_ICV_POSITIVE, 
       (char *) &default_direction,
       "Flip images to always have positive direction."},
   {"-negative_direction", ARGV_CONSTANT, (char *) MI_ICV_NEGATIVE, 
       (char *) &default_direction,
       "Flip images to always have negative direction."},
   {"-any_direction", ARGV_CONSTANT, (char *) MI_ICV_ANYDIR, 
       (char *) &default_direction,
       "Do not flip images (Default)."},
   {"+xdirection", ARGV_CONSTANT, (char *) MI_ICV_POSITIVE, 
       (char *) &xdirection,
       "Flip images to give positive xspace:step value (left-to-right)."},
   {"-xdirection", ARGV_CONSTANT, (char *) MI_ICV_NEGATIVE, 
       (char *) &xdirection,
       "Flip images to give negative xspace:step value (right-to-left)."},
   {"-xanydirection", ARGV_CONSTANT, (char *) MI_ICV_ANYDIR, 
       (char *) &xdirection,
       "Don't flip images along x-axis (default)."},
   {"+ydirection", ARGV_CONSTANT, (char *) MI_ICV_POSITIVE, 
          (char *) &ydirection,
       "Flip images to give positive yspace:step value (post-to-ant)."},
   {"-ydirection", ARGV_CONSTANT, (char *) MI_ICV_NEGATIVE, 
       (char *) &ydirection,
       "Flip images to give negative yspace:step value (ant-to-post)."},
   {"-yanydirection", ARGV_CONSTANT, (char *) MI_ICV_ANYDIR, 
       (char *) &ydirection,
       "Don't flip images along y-axis (default)."},
   {"+zdirection", ARGV_CONSTANT, (char *) MI_ICV_POSITIVE, 
       (char *) &zdirection,
       "Flip images to give positive zspace:step value (inf-to-sup)."},
   {"-zdirection", ARGV_CONSTANT, (char *) MI_ICV_NEGATIVE, 
       (char *) &zdirection,
       "Flip images to give negative zspace:step value (sup-to-inf)."},
   {"-zanydirection", ARGV_CONSTANT, (char *) MI_ICV_ANYDIR, 
       (char *) &zdirection,
       "Don't flip images along z-axis (default)."},
   {NULL, ARGV_END, NULL, NULL, NULL}
};

/* Main program */

int main(int argc, char *argv[])
{
   char *filename;
   int mincid, imgid, icvid, ndims, dims[MAX_VAR_DIMS];
   nc_type datatype;
   int is_signed;
   long start[MAX_VAR_DIMS], end[MAX_VAR_DIMS];
   long count[MAX_VAR_DIMS], cur[MAX_VAR_DIMS];
   int element_size;
   int idim;
   int nstart, ncount;
   void *data;
   char the_sign[MI_MAX_ATTSTR_LEN];
   int length;
   double temp;
   long nelements, ielement;
   double *dbl_data;
   int user_normalization;

   /* Check arguments */
   if (ParseArgv(&argc, argv, argTable, 0) || (argc != 2)) {
      (void) fprintf(stderr, "\nUsage: %s [<options>] <mincfile>\n", argv[0]);
      (void) fprintf(stderr,   "       %s -help\n\n", argv[0]);
      exit(EXIT_FAILURE);
   }
   filename = argv[1];

   /* Set normalization if image_range specified */
   user_normalization = FALSE;
   if ((image_range[0] != DBL_MAX) || (image_range[1] != DBL_MAX)) {
      user_normalization = TRUE;
      normalize_output = TRUE;
   }

   /* Check direction values */
   if (default_direction == INT_MAX)
      default_direction = MI_ICV_ANYDIR;
   if (xdirection == INT_MAX)
      xdirection = default_direction;
   if (ydirection == INT_MAX)
      ydirection = default_direction;
   if (zdirection == INT_MAX)
      zdirection = default_direction;

   /* Open the file */
   mincid = miopen(filename, NC_NOWRITE);

   /* Inquire about the image variable */
   imgid = ncvarid(mincid, MIimage);
   (void) ncvarinq(mincid, imgid, NULL, &datatype, &ndims, dims, NULL);

   /* Check if arguments set */

   /* Check the start and count arguments */
   for (nstart=0; (nstart<MAX_VAR_DIMS) && (hs_start[nstart]!=LONG_MIN); 
        nstart++) {}
   for (ncount=0; (ncount<MAX_VAR_DIMS) && (hs_count[ncount]!=LONG_MIN); 
        ncount++) {}
   if (((nstart != 0) && (nstart != ndims)) || 
       ((ncount != 0) && (ncount != ndims))) {
      (void) fprintf(stderr, 
  "Dimensions of start or count vectors not equal to dimensions in file.\n");
      exit(EXIT_FAILURE);
   }

   /* Get output data type */
   output_datatype = nc_type_list[arg_odatatype];
   if (arg_odatatype == TYPE_FILE) output_datatype = datatype;

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
      if (arg_odatatype == TYPE_FILE)
         output_signed = is_signed;
      else 
         output_signed = (output_datatype != NC_BYTE);
   }

   /* Get output range */
   if (valid_range[0] == DBL_MAX) {
      if (arg_odatatype == TYPE_FILE) {
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
   (void) miicv_setint(icvid, MI_ICV_DO_DIM_CONV, TRUE);
   (void) miicv_setint(icvid, MI_ICV_DO_SCALAR, FALSE);
   (void) miicv_setint(icvid, MI_ICV_XDIM_DIR, xdirection);
   (void) miicv_setint(icvid, MI_ICV_YDIM_DIR, ydirection);
   (void) miicv_setint(icvid, MI_ICV_ZDIM_DIR, zdirection);
   if ((output_datatype == NC_FLOAT) || (output_datatype == NC_DOUBLE)) {
      (void) miicv_setint(icvid, MI_ICV_DO_NORM, TRUE);
      (void) miicv_setint(icvid, MI_ICV_USER_NORM, TRUE);
   }
   else if (normalize_output) {
      (void) miicv_setint(icvid, MI_ICV_DO_NORM, TRUE);
      if (user_normalization) {
         (void) miicv_attach(icvid, mincid, imgid);
         if (image_range[0] == DBL_MAX) {
            (void) miicv_inqdbl(icvid, MI_ICV_NORM_MIN, &image_range[0]);
         }
         if (image_range[1] == DBL_MAX) {
            (void) miicv_inqdbl(icvid, MI_ICV_NORM_MAX, &image_range[1]);
         }
         (void) miicv_detach(icvid);
         (void) miicv_setint(icvid, MI_ICV_USER_NORM, TRUE);
         (void) miicv_setdbl(icvid, MI_ICV_IMAGE_MIN, image_range[0]);
         (void) miicv_setdbl(icvid, MI_ICV_IMAGE_MAX, image_range[1]);
      }
   }
   (void) miicv_attach(icvid, mincid, imgid);

   /* Set input file start, count and end vectors for reading a slice
      at a time */
   nelements = 1;
   for (idim=0; idim < ndims; idim++) {

      /* Get start */
      start[idim] = (nstart == 0) ? 0 : hs_start[idim];
      cur[idim] = start[idim];

      /* Get end */
      if (ncount!=0)
         end[idim] = start[idim]+hs_count[idim];
      else if (nstart!=0)
         end[idim] = start[idim]+1;
      else
         (void) ncdiminq(mincid, dims[idim], NULL, &end[idim]);

      /* Compare start and end */
      if (start[idim] >= end[idim]) {
         (void) fprintf(stderr, "start or count out of range\n");
         exit(EXIT_FAILURE);
      }

      /* Get count and nelements */
      if (idim < ndims-2)
         count[idim] = 1;
      else
         count[idim] = end[idim] - start[idim];
      nelements *= count[idim];
   }
   element_size = nctypelen(output_datatype);

   /* Allocate space */
   data = MALLOC(element_size*nelements);

   /* Loop over input slices */

   while (cur[0] < end[0]) {

      /* Read in the slice */
      (void) miicv_get(icvid, cur, count, data);

      /* Write out the slice */
      if (arg_odatatype == TYPE_ASCII) {
         dbl_data = data;
         for (ielement=0; ielement<nelements; ielement++) {
            (void) fprintf(stdout, "%.20g\n", dbl_data[ielement]);
         }
      }
      else {
         if (fwrite(data, (size_t) element_size, (size_t) nelements, stdout)
                       != nelements) {
            (void) fprintf(stderr, "Error writing data.\n");
            exit(EXIT_FAILURE);
         }
      }

      /* Increment cur counter */
      idim = ndims-1;
      cur[idim] += count[idim];
      while ( (idim>0) && (cur[idim] >= end[idim])) {
         cur[idim] = start[idim];
         idim--;
         cur[idim] += count[idim];
      }

   }       /* End loop over slices */

   /* Clean up */
   (void) miclose(mincid);
   (void) miicv_free(icvid);
   FREE(data);

   exit(EXIT_SUCCESS);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_arg_vector
@INPUT      : key - argv key string (-start, -count)
              nextArg - string from which vector should be read
@OUTPUT     : dst - pointer to vector of longs into which values should
                 be written (padded with LONG_MIN)
@RETURNS    : TRUE, since nextArg is used (unless it is NULL)
@DESCRIPTION: Parses a command-line argument into a vector of longs. The
              string should contain at most MAX_VAR_DIMS comma separated 
              integer values (spaces are skipped).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int get_arg_vector(char *dst, char *key, char *nextArg)
     /* ARGSUSED */
{

   long *vector;
   int nvals, i;
   char *cur, *end, *prev;

   /* Check for following argument */
   if (nextArg == NULL) {
      (void) fprintf(stderr, 
                     "\"%s\" option requires an additional argument\n",
                     key);
      return FALSE;
   }

   /* Get pointer to vector of longs */
   vector = (long *) dst;

   /* Set up pointers to end of string and first non-space character */
   end = nextArg + strlen(nextArg);
   cur = nextArg;
   while (isspace(*cur)) cur++;
   nvals = 0;

   /* Loop through string looking for integers */
   while ((nvals < MAX_VAR_DIMS) && (cur!=end)) {

      /* Get integer */
      prev = cur;
      vector[nvals] = strtol(prev, &cur, 0);
      if (cur == prev) {
         (void) fprintf(stderr, 
            "expected vector of integers for \"%s\", but got \"%s\"\n", 
                        key, nextArg);
         exit(EXIT_FAILURE);
      }
      nvals++;

      /* Skip any spaces */
      while (isspace(*cur)) cur++;

      /* Skip an optional comma */
      if (*cur == VECTOR_SEPARATOR) cur++;

   }

   /* Pad with LONG_MIN */
   for (i=nvals; i < MAX_VAR_DIMS; i++) {
      vector[i] = LONG_MIN;
   }

   return TRUE;
}
