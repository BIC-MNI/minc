/* ----------------------------- MNI Header -----------------------------------
@NAME       : rawtominc
@INPUT      : argc, argv - command line arguments
@OUTPUT     : (none)
@RETURNS    : error status
@DESCRIPTION: Converts a raw volume of data values (read from standard input)
              to a minc format file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : September 25, 1992 (Peter Neelin)
@MODIFIED   : 
 * $Log: rawtominc.c,v $
 * Revision 6.16  2004-12-03 21:54:27  bert
 * Minor changes for Windows build
 *
 * Revision 6.15  2004/11/01 22:38:39  bert
 * Eliminate all references to minc_def.h
 *
 * Revision 6.14  2004/04/27 15:26:34  bert
 * Added -2 option
 *
 * Revision 6.13  2004/02/02 18:24:58  bert
 * Include a ARGV_VERINFO record in the argTable[]
 *
 * Revision 6.12  2003/11/14 16:52:24  stever
 * More last-minute fixes.
 *
 * Revision 6.11  2003/11/03 19:43:18  bert
 * Handle unspecified dimension order correctly
 *
 * Revision 6.10  2003/10/29 17:50:18  bert
 * Added -dimorder option
 *
 * Revision 6.9  2003/10/21 22:22:09  bert
 * Added -swap_bytes option for int or short input, per A. Janke.
 *
 * Revision 6.8  2002/08/05 00:53:50  neelin
 * Added slightly modified code from Colin Holmes to support -skip option
 *
 * Revision 6.7  2001/09/18 15:33:00  neelin
 * Create image variable last to allow big images and to fix compatibility
 * problems with 2.3 and 3.x.
 *
 * Revision 6.6  2001/08/16 16:41:37  neelin
 * Added library functions to handle reading of datatype, sign and valid range,
 * plus writing of valid range and setting of default ranges. These functions
 * properly handle differences between valid_range type and image type. Such
 * difference can cause valid data to appear as invalid when double to float
 * conversion causes rounding in the wrong direction (out of range).
 * Modified voxel_loop, volume_io and programs to use these functions.
 *
 * Revision 6.5  2001/04/24 13:38:46  neelin
 * Replaced NC_NAT with MI_ORIGINAL_TYPE.
 *
 * Revision 6.4  2001/04/17 18:40:25  neelin
 * Modifications to work with NetCDF 3.x
 * In particular, changed NC_LONG to NC_INT (and corresponding longs to ints).
 * Changed NC_UNSPECIFIED to NC_NAT.
 * A few fixes to the configure script.
 *
 * Revision 6.3  1999/10/19 14:45:31  neelin
 * Fixed Log subsitutions for CVS
 *
 * Revision 6.2  1998/06/22 14:06:01  neelin
 * Fixed bug in handling of default types and signs.
 *
 * Revision 6.1  1997/09/29  12:22:46  neelin
 * Clarified argument error messages.
 *
 * Revision 6.0  1997/09/12  13:23:25  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:24:26  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  19:59:55  neelin
 * Release of minc version 0.4
 *
 * Revision 3.3  1997/04/21  20:19:17  neelin
 * Added -origin option.
 *
 * Revision 3.2  1996/06/19  18:24:16  neelin
 * Check errors on fopen when -input is used.
 * Try opening input before creating minc file.
 *
 * Revision 3.1  1995/11/16  13:11:16  neelin
 * Added include of math.h to get declaration of strtod under SunOs
 *
 * Revision 3.0  1995/05/15  19:31:00  neelin
 * Release of minc version 0.3
 *
 * Revision 2.7  1995/03/30  13:00:05  neelin
 * Added -sattribute and -dattribute options.
 *
 * Revision 2.6  1995/02/15  18:10:35  neelin
 * Added check for global attribute specified with -attribute.
 *
 * Revision 2.5  1995/02/08  19:31:47  neelin
 * Moved ARGSUSED statements for irix 5 lint.
 *
 * Revision 2.4  1995/01/23  09:07:46  neelin
 * changed ncclose to miclose.
 *
 * Revision 2.3  95/01/23  08:59:31  neelin
 * Changed nccreate to micreate
 * 
 * Revision 2.2  95/01/03  13:09:24  neelin
 * Added direction cosine support.
 * 
 * Revision 2.1  94/10/11  16:18:50  neelin
 * Fixed scanning of integers for max and min (conversion to double had
 * signed and unsigned reversed).
 * 
 * Revision 2.0  94/09/28  10:33:01  neelin
 * Release of minc version 0.2
 * 
 * Revision 1.15  94/09/28  10:32:55  neelin
 * Pre-release
 * 
 * Revision 1.14  94/09/23  08:30:52  neelin
 * Added -xyz, etc options for image/volume orientation.
 * 
 * Revision 1.13  94/06/10  15:24:41  neelin
 * Added option -real_range.
 * 
 * Revision 1.12  94/05/05  10:31:53  neelin
 * Added -scan_range, -frame_time, -frame_width, -input, -attribute and 
 * modality options.
 * 
 * Revision 1.11  93/10/06  10:14:17  neelin
 * Added include of string.h.
 * 
 * Revision 1.10  93/08/11  15:25:05  neelin
 * Added RCS logging to source.
 * 
              December 2, 1992 (P.N.)
                 - changed to parse argv with ParseArgv
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
static char rcsid[]="$Header: /private-cvsroot/minc/progs/rawtominc/rawtominc.c,v 6.16 2004-12-03 21:54:27 bert Exp $";
#endif

#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <minc.h>
#if HAVE_FLOAT_H
#include <float.h>
#endif /* HAVE_FLOAT_H */
#include <math.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */
#include <ParseArgv.h>
#include <time_stamp.h>
#include <convert_origin_to_start.h>

/* Some constants */

#define VECTOR_SEPARATOR ','
#define NORMAL_STATUS 0
#define ERROR_STATUS 1
#define MIN_DIMS 2
#define MAX_DIMS 4
#define DEF_TYPE 0
#define BYTE_TYPE 1
#define SHORT_TYPE 2
#define INT_TYPE 3
#define FLOAT_TYPE 4
#define DOUBLE_TYPE 5
#define DEF_SIGN 0
#define SIGNED 1
#define UNSIGNED 2
#define TRUE 1
#define FALSE 0
#define X 0
#define Y 1
#define Z 2
#define WORLD_NDIMS 3
#define DEF_STEP DBL_MAX
#define DEF_START DBL_MAX
#define DEF_RANGE DBL_MAX
#define DEF_DIRCOS DBL_MAX
#define DEF_ORIGIN DBL_MAX
#define ARG_SEPARATOR ','

/* Macros */
#define STR_EQ(s1,s2) (strcmp(s1,s2)==0)

/* Function declarations */
static void parse_args(int argc, char *argv[]);
static void usage_error(char *pname);
static int get_attribute(char *dst, char *key, char *nextarg);
static int get_times(char *dst, char *key, char *nextarg);
static int get_axis_order(char *dst, char *key, char *nextArg);

/* Array containing information about signs. It is subscripted by
   [signtype][type]. Note that the first row should never be used, since
   default_signs should be used instead. */
char *sign_names[][6] = {
   {MI_SIGNED, MI_UNSIGNED, MI_SIGNED, MI_SIGNED, MI_SIGNED, MI_SIGNED},
   {MI_SIGNED, MI_SIGNED, MI_SIGNED, MI_SIGNED, MI_SIGNED, MI_SIGNED},
   {MI_SIGNED, MI_UNSIGNED, MI_UNSIGNED, MI_UNSIGNED, MI_UNSIGNED, MI_UNSIGNED}
};

/* Array containing default signs subscripted by [type] (default, byte, 
   short, int, float, double) */
int default_signs[] = {
   SIGNED, UNSIGNED, SIGNED, SIGNED, SIGNED, SIGNED
};

/* Information converting types for argument processing to NetCDF types */
nc_type convert_types[] = {
   MI_ORIGINAL_TYPE, NC_BYTE, NC_SHORT, NC_INT, NC_FLOAT, NC_DOUBLE
};

/* Argument variables */
char *pname;
char *filename;
int clobber=FALSE;
#ifdef MINC2
int v2format=FALSE;		/* Version 2.0 file format? */
#endif /* MINC2 defined */
char *dimname[MAX_VAR_DIMS];
long dimlength[MAX_VAR_DIMS];
int ndims;
int type = BYTE_TYPE;
int signtype = DEF_SIGN;
nc_type datatype;
char *sign;
double valid_range[2] = {0.0, -1.0};
int vrange_set;
int otype = DEF_TYPE;
int osigntype = DEF_SIGN;
nc_type odatatype;
char *osign;
double ovalid_range[2] = {0.0, -1.0};
int ovrange_set;
double dimstep[3] = {DEF_STEP, DEF_STEP, DEF_STEP};
double dimstart[3] = {DEF_START, DEF_START, DEF_START};
double dimdircos[3][3] = {
   DEF_DIRCOS, DEF_DIRCOS, DEF_DIRCOS,
   DEF_DIRCOS, DEF_DIRCOS, DEF_DIRCOS,
   DEF_DIRCOS, DEF_DIRCOS, DEF_DIRCOS
};
double origin[3] = {DEF_ORIGIN, DEF_ORIGIN, DEF_ORIGIN};
int vector_dimsize = -1;
char *modality = NULL;
int attribute_list_size = 0;
int attribute_list_alloc = 0;
struct {
   char *variable;
   char *attribute;
   char *value;
   double double_value;
} *attribute_list = NULL;
int num_frame_times = 0;
double *frame_times = NULL;
int num_frame_widths = 0;
double *frame_widths = NULL;
char *inputfile = NULL;
int do_minmax = FALSE;
double real_range[2] = {DEF_RANGE, DEF_RANGE};
long skip_length;
int swap_bytes = FALSE;
char *axis_order[MAX_DIMS+1] = { MItime, MIzspace, MIyspace, MIxspace };

/* Argument table */
ArgvInfo argTable[] = {
   {NULL, ARGV_VERINFO, (char *) VERSION, (char *) NULL, NULL},
   {NULL, ARGV_HELP, (char *) NULL, (char *) NULL, 
       "Options to specify input dimension order (from slowest to fastest)."},
   {NULL, ARGV_HELP, (char *) NULL, (char *) NULL, 
       "   Default = -transverse."},
   {"-transverse", ARGV_FUNC, (char *) get_axis_order, (char *) axis_order,
       "Transverse images   : [[time] z] y x"},
   {"-sagittal", ARGV_FUNC, (char *) get_axis_order, (char *) axis_order,
       "Sagittal images     : [[time] x] z y"},
   {"-coronal", ARGV_FUNC, (char *) get_axis_order, (char *) axis_order,
       "Coronal images      : [[time] y] z x"},
   {"-time", ARGV_FUNC, (char *) get_axis_order, (char *) axis_order,
       "Time ordered images : [[z] time] y x"},
   {"-xyz", ARGV_FUNC, (char *) get_axis_order, (char *) axis_order,
       "Dimension order     : [[time] x] y z"},
   {"-xzy", ARGV_FUNC, (char *) get_axis_order, (char *) axis_order,
       "Dimension order     : [[time] x] z y"},
   {"-yxz", ARGV_FUNC, (char *) get_axis_order, (char *) axis_order,
       "Dimension order     : [[time] y] x z"},
   {"-yzx", ARGV_FUNC, (char *) get_axis_order, (char *) axis_order,
       "Dimension order     : [[time] y] z x"},
   {"-zxy", ARGV_FUNC, (char *) get_axis_order, (char *) axis_order,
       "Dimension order     : [[time] z] x y"},
   {"-zyx", ARGV_FUNC, (char *) get_axis_order, (char *) axis_order,
       "Dimension order     : [[time] z] y x"},
   {"-dimorder", ARGV_FUNC, (char *) get_axis_order, (char *) axis_order,
       "Arbitrary dimensions: <dim1>,<dim2>[,<dim3>[,<dim4]]"},
   {"-vector", ARGV_INT, (char *) 1, (char *) &vector_dimsize,
       "Specifies the size of a vector dimension"},
   {NULL, ARGV_HELP, NULL, NULL,
       "Options to specify input data type. Default = -byte."},
   {"-byte", ARGV_CONSTANT, (char *) BYTE_TYPE, (char *) &type,
       "Byte values"},
   {"-short", ARGV_CONSTANT, (char *) SHORT_TYPE, (char *) &type,
       "Short integer values"},
   {"-int", ARGV_CONSTANT, (char *) INT_TYPE, (char *) &type,
       "32-bit integer values"},
   {"-long", ARGV_CONSTANT, (char *) INT_TYPE, (char *) &type,
       "Superseded by -int"},
   {"-float", ARGV_CONSTANT, (char *) FLOAT_TYPE, (char *) &type,
       "Single-precision floating point values"},
   {"-double", ARGV_CONSTANT, (char *) DOUBLE_TYPE, (char *) &type,
       "Double-precision floating point values"},
   {NULL, ARGV_HELP, NULL, NULL,
       "Options for sign of input data. Default = -signed (-unsigned for byte)."},
   {"-signed", ARGV_CONSTANT, (char *) SIGNED, (char *) &signtype,
       "Signed values"},
   {"-unsigned", ARGV_CONSTANT, (char *) UNSIGNED, (char *) &signtype,
       "Unsigned values"},
   {NULL, ARGV_HELP, NULL, NULL,
       "Other input value options."},
   {"-skip", ARGV_INT, (char *) 0, (char *) &skip_length,
       "Specifies the number of bytes to skip over in the input stream"},
   {"-range", ARGV_FLOAT, (char *) 2, (char *) valid_range, 
       "Valid range of input values (default = full range)."},
   {"-real_range", ARGV_FLOAT, (char *) 2, (char *) real_range, 
       "Real range of input values (ignored for floating-point types)."},
   {"-swap_bytes", ARGV_CONSTANT, (char *) TRUE, (char *)&swap_bytes,
       "Swap bytes on short or long integer input." },
   {NULL, ARGV_HELP, NULL, NULL,
       "Options for type of output minc data. Default = input data type"},
   {"-obyte", ARGV_CONSTANT, (char *) BYTE_TYPE, (char *) &otype,
       "Byte values"},
   {"-oshort", ARGV_CONSTANT, (char *) SHORT_TYPE, (char *) &otype,
       "Short integer values"},
   {"-oint", ARGV_CONSTANT, (char *) INT_TYPE, (char *) &otype,
       "32-bit integer values"},
   {"-olong", ARGV_CONSTANT, (char *) INT_TYPE, (char *) &otype,
       "Superseded by -oint"},
   {"-ofloat", ARGV_CONSTANT, (char *) FLOAT_TYPE, (char *) &otype,
       "Single-precision floating point values"},
   {"-odouble", ARGV_CONSTANT, (char *) DOUBLE_TYPE, (char *) &otype,
       "Double precision floating point values"},
   {NULL, ARGV_HELP, NULL, NULL,
       "Options for sign of output minc data."},
   {NULL, ARGV_HELP, NULL, NULL,
       "   Default = -signed (-unsigned for byte data)"},
   {"-osigned", ARGV_CONSTANT, (char *) SIGNED, (char *) &osigntype,
       "Signed values"},
   {"-ounsigned", ARGV_CONSTANT, (char *) UNSIGNED, (char *) &osigntype,
       "Unsigned values"},
   {NULL, ARGV_HELP, NULL, NULL,
       "Other output value options."},
   {"-orange", ARGV_FLOAT, (char *) 2, (char *) ovalid_range, 
       "Valid range of output minc values."},
   {NULL, ARGV_HELP, NULL, NULL,
       "Options to specify whether integers are scanned for min and max."},
   {"-scan_range", ARGV_CONSTANT, (char *) TRUE, (char *) &do_minmax, 
       "Scan input for min and max."},
   {"-noscan_range", ARGV_CONSTANT, (char *) FALSE, (char *) &do_minmax, 
       "Do not scan input for min and max (default)."},
   {NULL, ARGV_HELP, NULL, NULL,
       "Options for writing output file. Default = -noclobber."},
#ifdef MINC2
   {"-2", ARGV_CONSTANT, (char *) TRUE, (char *) &v2format,
       "Produce a MINC 2.0 format output file."},
#endif /* MINC2 defined */
   {"-clobber", ARGV_CONSTANT, (char *) TRUE, (char *) &clobber,
       "Overwrite existing file"},
   {"-noclobber", ARGV_CONSTANT, (char *) FALSE, (char *) &clobber,
       "Don't overwrite existing file"},
   {NULL, ARGV_HELP, NULL, NULL,
       "Option for specifying input raw data file."},
   {"-input", ARGV_STRING, (char *) 1, (char *) &inputfile,
       "Name of input file (default=stdin)."},
   {NULL, ARGV_HELP, NULL, NULL,
       "Options for specifying spatial dimension coordinates."},
   {"-xstep", ARGV_FLOAT, (char *) 1, (char *) &dimstep[X],
       "Step size for x dimension."},
   {"-ystep", ARGV_FLOAT, (char *) 1, (char *) &dimstep[Y],
       "Step size for y dimension."},
   {"-zstep", ARGV_FLOAT, (char *) 1, (char *) &dimstep[Z],
       "Step size for z dimension."},
   {"-xstart", ARGV_FLOAT, (char *) 1, (char *) &dimstart[X],
       "Starting coordinate for x dimension."},
   {"-ystart", ARGV_FLOAT, (char *) 1, (char *) &dimstart[Y],
       "Starting coordinate for y dimension."},
   {"-zstart", ARGV_FLOAT, (char *) 1, (char *) &dimstart[Z],
       "Starting coordinate for z dimension."},
   {"-xdircos", ARGV_FLOAT, (char *) 3, (char *) dimdircos[X],
       "Direction cosines for x dimension."},
   {"-ydircos", ARGV_FLOAT, (char *) 3, (char *) dimdircos[Y],
       "Direction cosines for y dimension."},
   {"-zdircos", ARGV_FLOAT, (char *) 3, (char *) dimdircos[Z],
       "Direction cosines for z dimension."},
   {"-origin", ARGV_FLOAT, (char *) 3, (char *) origin,
       "Coordinate of first pixel."},
   {NULL, ARGV_HELP, NULL, NULL,
       "Options for specifying imaging modality. Default = -nomodality."},
   {"-nomodality", ARGV_CONSTANT, NULL, (char *) &modality,
       "Do not store modality type in file."},
   {"-pet", ARGV_CONSTANT, MI_PET, (char *) &modality,
       "PET data."},
   {"-mri", ARGV_CONSTANT, MI_MRI, (char *) &modality,
       "MRI data."},
   {"-spect", ARGV_CONSTANT, MI_SPECT, (char *) &modality,
       "SPECT data."},
   {"-gamma", ARGV_CONSTANT, MI_GAMMA, (char *) &modality,
       "Data from a gamma camera."},
   {"-mrs", ARGV_CONSTANT, MI_MRS, (char *) &modality,
       "MR spectroscopy data."},
   {"-mra", ARGV_CONSTANT, MI_MRA, (char *) &modality,
       "MR angiography data."},
   {"-ct", ARGV_CONSTANT, MI_CT, (char *) &modality,
       "CT data."},
   {"-dsa", ARGV_CONSTANT, MI_DSA, (char *) &modality,
       "DSA data"},
   {"-dr", ARGV_CONSTANT, MI_DR, (char *) &modality,
       "Digital radiography data."},
   {NULL, ARGV_HELP, NULL, NULL,
       "Options for specifying attribute values by name."},
   {"-sattribute", ARGV_FUNC, (char *) get_attribute, NULL,
       "Set a string attribute (<var>:<attr>=<value>)."},
   {"-dattribute", ARGV_FUNC, (char *) get_attribute, NULL,
       "Set a double precision attribute (<var>:<attr>=<value>)."},
   {"-attribute", ARGV_FUNC, (char *) get_attribute, NULL,
       "Set an attribute, guessing the type (<var>:<attr>=<value>)."},
   {NULL, ARGV_HELP, NULL, NULL,
       "Options for specifying time dimension coordinates."},
   {"-frame_times", ARGV_FUNC, (char *) get_times, NULL,
       "Specify the frame starting times (\"<t1>,<t2>,<t3>,...\")."},
   {"-frame_widths", ARGV_FUNC, (char *) get_times, NULL,
       "Specify the frame lengths (\"<w1>,<w2>,<w3>,...\")."},
   {NULL, ARGV_END, NULL, NULL, NULL}
};

/* Main program */

int main(int argc, char *argv[])
{
   int do_vrange;
   int cdfid, imgid, maxid, minid, varid;
   int icv;
   long start[MAX_VAR_DIMS];
   long count[MAX_VAR_DIMS];
   long end[MAX_VAR_DIMS];
   int dim[MAX_VAR_DIMS];
   void *image;
   double imgmax, imgmin, value;
   long image_size, image_pix, nread, fastdim;
   int pix_size;
   int image_dims;
   int i, j;
   int index;
   FILE *instream;
   char *tm_stamp;
   int iatt;
   long time_start, time_count;
   int is_signed;
   void *ptr;
   int floating_type;
   int do_real_range;
   int status;
   double scale, offset, denom, pixel_min, pixel_max;
   double dircos[WORLD_NDIMS][WORLD_NDIMS];
   int cflags;

   /* Save time stamp and args */
   tm_stamp = time_stamp(argc, argv);

   /* Parse arguments */
   parse_args(argc, argv);

   /* Do normalization if input type is float */
   floating_type = (( datatype==NC_FLOAT) || ( datatype==NC_DOUBLE));
   do_minmax = do_minmax || floating_type;

   /* Find max and min for vrange if output type is float */
   do_vrange = 
      do_minmax && (( odatatype==NC_FLOAT) || ( odatatype==NC_DOUBLE));

   /* Did the user provide a real range */
   do_real_range = (real_range[0] != DEF_RANGE) && !floating_type;

   /* Check that only start or origin are specified, not both */
   for (i=0; i<WORLD_NDIMS; i++) {
      if ((dimstart[i] != DEF_START) && (origin[0] != DEF_START)) {
         (void) fprintf(stderr, "Do not specify both start and origin.\n");
         exit(ERROR_STATUS);
      }
   }

   /* If origin is given, convert it to a start using direction cosines.
      Default direction cosines must be provided. */
   if (origin[0] != DEF_ORIGIN) {

      /* Set direction cosines */
      for (i=0; i<WORLD_NDIMS; i++) {
         if (dimdircos[i][0] != DEF_DIRCOS) {
            for (j=0; j<WORLD_NDIMS; j++)
               dircos[i][j] = dimdircos[i][j];
         }
         else {    /* Use default */
            for (j=0; j<WORLD_NDIMS; j++)
               dircos[i][j] = 0.0;
            dircos[i][i] = 1.0;
         }
      }

      /* Convert the origin to a start value */
      status = convert_origin_to_start(origin, dircos[X], dircos[Y], dircos[Z],
                                       dimstart);
      switch (status) {
      case 1:
         (void) fprintf(stderr, 
                        "Cannot convert origin to start - some direction cosines have zero length.\n");
         break;
      case 2:
         (void) fprintf(stderr, 
                        "Cannot convert origin to start - some direction cosines are parallel.\n");
         break;
      }

   }

   /* Open the input file */
   if (inputfile == NULL) {
      instream=stdin;
   }
   else {
      instream = fopen(inputfile, "r");
      if (instream == NULL) {
         (void) fprintf(stderr, "Error opening input file \"%s\"\n", 
                        inputfile);
         exit(ERROR_STATUS);
      }
   }

   /* Create an icv */
   icv=miicv_create();
   (void) miicv_setint(icv, MI_ICV_TYPE, datatype);
   (void) miicv_setstr(icv, MI_ICV_SIGN, sign);
   if (vrange_set) {
      (void) miicv_setdbl(icv, MI_ICV_VALID_MIN, valid_range[0]);
      (void) miicv_setdbl(icv, MI_ICV_VALID_MAX, valid_range[1]);
   }
   if (floating_type) {
      (void) miicv_setint(icv, MI_ICV_DO_NORM, TRUE);
      (void) miicv_setint(icv, MI_ICV_USER_NORM, TRUE);
   }

   /* Calculate scale and offset to convert pixel to real value for
      input data */
   if (do_real_range) {
      (void) miicv_inqdbl(icv, MI_ICV_VALID_MIN, &pixel_min);
      (void) miicv_inqdbl(icv, MI_ICV_VALID_MAX, &pixel_max);
      denom = pixel_max - pixel_min;
      if (denom != 0.0)
         scale = (real_range[1] - real_range[0]) / denom;
      else 
         scale = 0.0;
      offset = real_range[0] - pixel_min * scale;
   }
   else {
      scale = 1.0;
      offset = 0.0;
   }

   /* Create the file and save the time stamp */
   if (clobber) {
       cflags = NC_CLOBBER;
   }
   else {
       cflags = NC_NOCLOBBER;
   }
#ifdef MINC2
   if (v2format) {
       cflags |= MI2_CREATE_V2;
   }
#endif /* MINC2 defined */
   cdfid=micreate(filename, cflags);
   (void) miattputstr(cdfid, NC_GLOBAL, MIhistory, tm_stamp);

   /* Set the number of image dimensions */
   image_dims = 2;

   /* Create the dimensions */
   for (i=0; i<ndims; i++) {

      /* Create dimension */
      dim[i] = ncdimdef(cdfid, dimname[i], dimlength[i]);

      /* Create the variable if needed */
      if (STR_EQ(dimname[i], MItime)) {
         if (num_frame_times > 0) {
            varid = micreate_std_variable(cdfid, MItime, 
                                          NC_DOUBLE, 1, &dim[i]);
         }
         if (num_frame_widths > 0) {
            varid = micreate_std_variable(cdfid, MItime_width,
                                          NC_DOUBLE, 1, &dim[i]);
         }
      }
      else {
         varid = micreate_std_variable(cdfid, dimname[i], 
                                       NC_INT, 0, NULL);

         /* Write out step and start and direction cosine */
         if (STR_EQ(dimname[i], MIxspace)) index = X;
         if (STR_EQ(dimname[i], MIyspace)) index = Y;
         if (STR_EQ(dimname[i], MIzspace)) index = Z;
         if (dimstep[index] != DEF_STEP) {
            (void) miattputdbl(cdfid, varid, MIstep, dimstep[index]);
         }
         if (dimstart[index] != DEF_START) {
            (void) miattputdbl(cdfid, varid, MIstart, dimstart[index]);
         }
         if (dimdircos[index][0] != DEF_DIRCOS) {
            (void) ncattput(cdfid, varid, MIdirection_cosines, NC_DOUBLE,
                            3, dimdircos[index]);
         }
      }
      
      /* Set variables for looping through images */
      start[i]=0;
      count[i]= ((i<ndims-image_dims) ? 1 : dimlength[i]);
      end[i]=dimlength[i];
   }

   /* Check for vector dimension */
   if (vector_dimsize > 0) {
      ndims++;
      image_dims++;
      dim[ndims-1] = ncdimdef(cdfid, MIvector_dimension, 
                              (long) vector_dimsize);
      start[ndims-1] = 0;
      count[ndims-1] = vector_dimsize;
      end[ndims-1] = vector_dimsize;
   }

   /* Create the modality attribute */
   if (modality != NULL) {
      varid = micreate_group_variable(cdfid, MIstudy);
      (void) miattputstr(cdfid, varid, MImodality, modality);
   }

   /* Create any special attributes */
   ncopts = 0;
   for (iatt=0; iatt < attribute_list_size; iatt++) {
      if (strlen(attribute_list[iatt].variable) == 0) {
         varid = NC_GLOBAL;
      }
      else {
         varid = ncvarid(cdfid, attribute_list[iatt].variable);
         if (varid == MI_ERROR) {
            varid = micreate_group_variable(cdfid, 
                                            attribute_list[iatt].variable);
         }
         if (varid == MI_ERROR) {
            varid = ncvardef(cdfid, attribute_list[iatt].variable, NC_INT,
                             0, NULL);
         }
         if (varid == MI_ERROR) {
            continue;
         }
      }
      if (attribute_list[iatt].value != NULL) {
         (void) miattputstr(cdfid, varid, attribute_list[iatt].attribute,
                            attribute_list[iatt].value);
      }
      else {
         (void) miattputdbl(cdfid, varid, attribute_list[iatt].attribute,
                            attribute_list[iatt].double_value);
      }
   }
   ncopts = NC_VERBOSE | NC_FATAL;

   /* Create the image */
   if (do_minmax || do_real_range) {
      maxid = micreate_std_variable(cdfid, MIimagemax, 
                                    NC_DOUBLE, ndims-image_dims, dim);
      minid = micreate_std_variable(cdfid, MIimagemin, 
                                    NC_DOUBLE, ndims-image_dims, dim);
   }
   imgid=micreate_std_variable(cdfid, MIimage, odatatype, ndims, dim);
   (void) miattputstr(cdfid, imgid, MIcomplete, MI_FALSE);
   (void) miattputstr(cdfid, imgid, MIsigntype, osign);
   if (ovrange_set) 
      (void) miset_valid_range(cdfid, imgid, ovalid_range);

   /* End definition mode */
   (void) ncendef(cdfid);

   /* Write out the frame times and widths */
   time_start = 0;
   time_count = num_frame_times;
   if (num_frame_times > 0) {
      (void) mivarput(cdfid, ncvarid(cdfid, MItime), 
                      &time_start, &time_count,
                      NC_DOUBLE, NULL, frame_times);
   }
   if (num_frame_widths > 0) {
      (void) mivarput(cdfid, ncvarid(cdfid, MItime_width), 
                      &time_start, &time_count,
                      NC_DOUBLE, NULL, frame_widths);
   }

   /* Attach the icv */
   (void) miicv_attach(icv, cdfid, imgid);

   /* Get a buffer for reading images */
   image_pix = 1;
   for (i=1; i<=image_dims; i++)
      image_pix *= end[ndims-i];
   pix_size=nctypelen(datatype);
   image_size=image_pix*pix_size;
   image=malloc(image_size);

   /* Loop through the images */
   fastdim=ndims-image_dims-1;
   if (fastdim<0) fastdim=0;
   if (do_vrange) {
      ovalid_range[0]=DBL_MAX;
      ovalid_range[1]=(-DBL_MAX);
   }
   
   /* CJH - July 02 - Skip over any header bytes  */
   if (skip_length > 0) {

      /* First try seeking over the header */
      if (fseek(instream, skip_length, SEEK_SET) == 0) {
         /* OK instream was a file and we skipped the requested bytes */
      }
      else {
         char buffer[8192];
         int bytes_copied = 0;
         while ( bytes_copied < skip_length ) {
            int bytes_to_read = sizeof(buffer);
            if ( bytes_to_read + bytes_copied > skip_length )
               bytes_to_read = skip_length - bytes_copied;
            nread = fread(buffer, sizeof(char), bytes_to_read, instream);
            bytes_copied += nread;
            if (nread != bytes_to_read) {
               (void) fprintf(stderr, "%s: Premature end of file.\n", pname);
               perror("While skipping over small header");
               exit(ERROR_STATUS);
            }
         }
      }
   }

   while (start[0] < end[0]) {

      /* Read in image */
      nread=fread(image, pix_size, image_pix, instream);
      if (nread!=image_pix) {
         (void) fprintf(stderr, "%s: Premature end of file.\n", pname);
         exit(ERROR_STATUS);
      }

      /* If the user wants to swap bytes, do it here before any further
       * processing of the image.
       */
      if (swap_bytes) {
          switch (datatype) {
          case NC_SHORT:
              /* Easy case - call swab() */
              if ((image_size & 1) != 0) {
                  fprintf(stderr,
                          "%s: image size must be even for -swap_bytes\n",
                          pname);
                  exit(ERROR_STATUS);
              }
              else {
                  swab(image, image, image_size);
              }
              break;

          case NC_INT:
              /* Harder case - have to do a more complex 4-byte swap. */
              if ((image_size & 3) != 0) {
                  fprintf(stderr, 
                          "%s: image size must be divisible by 4 for -swap_bytes!\n",
                          pname);
                  exit(ERROR_STATUS);
              }
              else {
                  unsigned char *img_ptr = image;
                  unsigned char *img_end = (unsigned char*)image + image_size;

                  for ( ; img_ptr < img_end; img_ptr += 4) {
                      unsigned char tmp;

                      /* Swap the inner 2 bytes 
                       */
                      tmp = img_ptr[1];
                      img_ptr[1] = img_ptr[2];
                      img_ptr[2] = tmp;

                      /* Swap the outer 2 bytes 
                       */
                      tmp = img_ptr[0];
                      img_ptr[0] = img_ptr[3];
                      img_ptr[3] = tmp;
                  }
              }
              break;

          default:
              /* I don't see any point in implementing swap for bytes, floats,
               * or doubles.
               */
              fprintf(stderr, 
                      "Warning: you specified -swap_bytes, but I can't swap this type if input\n");
              break;
          }
      }

      /* Search for max and min for float and double */
      if (do_minmax) {
         imgmax=(-DBL_MAX);
         imgmin=DBL_MAX;
         is_signed = (signtype == SIGNED);
         for (j=0; j<image_pix; j++) {
            ptr = (char *) image + j * nctypelen(datatype);
            switch (datatype) { 
            case NC_BYTE : 
               if (is_signed)
                  value = (double) *((signed char *) ptr);
               else
                  value = (double) *((unsigned char *) ptr);
               break; 
            case NC_SHORT : 
               if (is_signed)
                  value = (double) *((signed short *) ptr);
               else
                  value = (double) *((unsigned short *) ptr);
               break; 
            case NC_INT : 
               if (is_signed)
                  value = (double) *((signed int *) ptr); 
               else
                  value = (double) *((unsigned int *) ptr); 
               break; 
            case NC_FLOAT : 
               value = (double) *((float *) ptr); 
               break; 
            case NC_DOUBLE : 
               value = (double) *((double *) ptr); 
               break; 
            } 
            if (value<imgmin) imgmin=value;
            if (value>imgmax) imgmax=value;
         }
         if (do_vrange) {
            if (imgmin<ovalid_range[0]) ovalid_range[0]=imgmin;
            if (imgmax>ovalid_range[1]) ovalid_range[1]=imgmax;
         }

      }
      else {
         imgmin = pixel_min;
         imgmax = pixel_max;
      }

      /* Change the valid range for integer types if needed */
      if (do_minmax && !floating_type) {
         (void) miicv_detach(icv);
         (void) miicv_setdbl(icv, MI_ICV_VALID_MIN, imgmin);
         (void) miicv_setdbl(icv, MI_ICV_VALID_MAX, imgmax);
         (void) miicv_attach(icv, cdfid, imgid);
      }

      /* Write the image max and min after re-scaling */
      if (do_minmax || do_real_range) {
         imgmin = imgmin * scale + offset;
         imgmax = imgmax * scale + offset;
         (void) mivarput1(cdfid, minid, start, NC_DOUBLE, NULL, &imgmin);
         (void) mivarput1(cdfid, maxid, start, NC_DOUBLE, NULL, &imgmax);
      }
      
      /* Write the image */
      (void) miicv_put(icv, start, count, image);

     /* Increment the counters */
      start[fastdim] += count[fastdim];
      i=fastdim;
      while ((i>0) && (start[i]>=end[i])) {
         start[i] = 0;
         i--;
         start[i] += count[i];
      }
   }

   /* Free the memory */
   free(image);

   /* Write the valid max and min */
   if (do_vrange) {
      (void) miset_valid_range(cdfid, imgid, ovalid_range);
   }

   /* Close the file */
   (void) miattputstr(cdfid, imgid, MIcomplete, MI_TRUE);
   (void) miclose(cdfid);
   
   exit(NORMAL_STATUS);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : parse_args
@INPUT      : argc        - number of command line arguments
              argv        - array of arguments
@OUTPUT     : 
@RETURNS    : (nothing).
@DESCRIPTION: Parses command line arguments.
@METHOD     : 
@GLOBALS    : pname        - program name
              filename     - file to write
              clobber      - overwrite?
              dimname      - names of dimensions
              dimlength    - lengths of dimensions
              ndims        - number of dimensions
              type         - type of input data
              signtype     - sign of input data
              datatype     - NetCDF type of input data
              sign         - string sign of input
              valid_range  - valid range of input data
              vrange_set   - valid range used?
              otype        - type of output data
              osigntype    - sign of output data
              datatype     - NetCDF type of output data
              osign        - string sign of output
              ovalid_range - valid range of output data
              ovrange_set  - valid range used?
@CALLS      : 
@CREATED    : December 3, 1992
@MODIFIED   : 
---------------------------------------------------------------------------- */
static void parse_args(int argc, char *argv[])
{
   char *ptr;
   int time_size;
   int i;

   /* Parse the command line */
   pname=argv[0];
   if (ParseArgv(&argc, argv, argTable, 0)) {
      usage_error(pname);
   }

   /* Check dimensions */
   ndims = argc - 2;
   if ((ndims<MIN_DIMS)||(ndims>MAX_DIMS)) {
      (void) fprintf(stderr, 
         "\nWrong number of arguments.\n");
      usage_error(pname);
   }

   /* Get filename */
   filename = argv[1];

   /* Get dimensions */
   for (i=0; i<ndims; i++) {
      dimlength[i] = strtol(argv[2+i], &ptr, 0);
      if ((ptr==argv[2+i]) || (*ptr!=0)) {
         if (ptr==argv[2+i]) {
            (void) fprintf(stderr, "\nBad argument \"%s\".\n", ptr);
         }
         else {
            (void) fprintf(stderr, 
                           "\nDimension sizes must be integer values.\n");
         }
         usage_error(pname);
      }
      dimname[i]=axis_order[i+MAX_DIMS-ndims];
   }

   /* Set types and signs */
   if (signtype==DEF_TYPE) {
      signtype=default_signs[type];
   }
   if ((otype==DEF_TYPE) && (osigntype==DEF_TYPE))
      osigntype=signtype;
   if (otype==DEF_TYPE) 
      otype=type;
   if (osigntype==DEF_TYPE)
      osigntype=default_signs[otype];
   datatype = convert_types[type];
   odatatype = convert_types[otype];
   sign  = sign_names[signtype][type];
   osign = sign_names[osigntype][otype];

   /* Check valid ranges */
   vrange_set=(valid_range[0]<valid_range[1]);
   ovrange_set=(ovalid_range[0]<ovalid_range[1]);
   if ((odatatype==datatype) && (STR_EQ(osign,sign))) {
      if (!ovrange_set && vrange_set) {
         ovalid_range[0]=valid_range[0];
         ovalid_range[1]=valid_range[1];
         ovrange_set = TRUE;
      }
      if (ovrange_set && !vrange_set) {
         valid_range[0]=ovalid_range[0];
         valid_range[1]=ovalid_range[1];
         vrange_set = TRUE;
      }
   }

   /* Always write valid range for floating point values */
   if ((odatatype==NC_FLOAT) || (odatatype==NC_DOUBLE)) {
      ovalid_range[0]=0.0;
      ovalid_range[1]=1.0;
      ovrange_set=TRUE;
   }

   /* Check that time variables correspond to given dimension size */
   if (strcmp(dimname[0], MItime) == 0) {
      time_size = dimlength[0];
   }
   else if (strcmp(dimname[1], MItime) == 0) {
      time_size = dimlength[1];
   }
   else {
      time_size = 0;
   }
   if (((num_frame_times != time_size) && (num_frame_times > 0)) || 
       ((num_frame_widths != time_size) && (num_frame_widths > 0))) {
      (void) fprintf(stderr, 
   "Number of frame times or widths does not match number of frames.\n");
      exit(EXIT_FAILURE);
   }

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : usage_error
@INPUT      : progname - program name
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Prints a usage error message and exits.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : September 25, 1992 (Peter Neelin)
@MODIFIED   : December 2, 1992 (P.N.)
---------------------------------------------------------------------------- */
static void usage_error(char *progname)
{
   (void) fprintf(stderr, "\nUsage: %s [<options>] <output.mnc> ", 
                  progname);
   (void) fprintf(stderr, "[[<size4>] <size3>] <size2> <size1>\n");
   (void) fprintf(stderr,   "       %s [-help]\n\n", progname);

   exit(ERROR_STATUS);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_attribute
@INPUT      : dst - client data passed by ParseArgv
              key - matching key in argv
              nextarg - argument following key in argv
@OUTPUT     : (none)
@RETURNS    : TRUE since nextarg is used.
@DESCRIPTION: Gets attributes from command line. Syntax for argument is
              "<var>:<att>=<value>". Numeric values are converted to
              double precision.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : May 3, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static int get_attribute(char *dst, char *key, char *nextarg)
     /* ARGSUSED */
{
   int need_string, need_double;
   char *variable;
   char *attribute;
   char *value;
   char *end;
   double dvalue;

   /* Check for a following argument */
   if (nextarg == NULL) {
      (void) fprintf(stderr, 
                     "\"%s\" option requires an additional argument\n",
                     key);
      exit(EXIT_FAILURE);
   }

   /* Figure out whether we need a string or a double */
   need_string = (strcmp(key, "-sattribute") == 0);
   need_double = (strcmp(key, "-dattribute") == 0);

   /* Get the variable name */
   variable = nextarg;
   attribute = strchr(variable, ':');
   if (attribute == NULL) {
      (void) fprintf(stderr, 
                     "%s option requires argument <var>:<attr>=<val>\n", 
                     key);
      exit(EXIT_FAILURE);
   }
   *attribute = '\0';
   attribute++;

   /* Get the value */
   value = strchr(attribute, '=');
   if (value == NULL) {
      (void) fprintf(stderr, 
                     "%s option requires argument <var>:<attr>=<val>\n", 
                     key);
      exit(EXIT_FAILURE);
   }
   *value = '\0';
   value++;

   /* Save the information */
   attribute_list_size++;
   if (attribute_list_size > attribute_list_alloc) {
      attribute_list_alloc += 10;
      if (attribute_list == NULL) {
         attribute_list = 
            malloc(attribute_list_alloc * sizeof(*attribute_list));
      }
      else {
         attribute_list = 
            realloc(attribute_list, 
                    attribute_list_alloc * sizeof(*attribute_list));
      }
   }
   attribute_list[attribute_list_size-1].variable = variable;
   attribute_list[attribute_list_size-1].attribute = attribute;
   attribute_list[attribute_list_size-1].value = value;

   /* Try to get a double precision value */
   if (!need_string) {
      dvalue = strtod(value, &end);
      if ((end != value) && (*end == '\0')) {
         attribute_list[attribute_list_size-1].value = NULL;
         attribute_list[attribute_list_size-1].double_value = dvalue;
      }
      else if (need_double) {
         (void) fprintf(stderr, 
                        "\"%s\" option requires a numeric argument\n",
                        key);
         exit(EXIT_FAILURE);
      }
   }

   return TRUE;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_times
@INPUT      : dst - client data passed by ParseArgv
              key - matching key in argv
              nextarg - argument following key in argv
@OUTPUT     : (none)
@RETURNS    : TRUE since nextarg is used.
@DESCRIPTION: Gets frame start times or frame lengths from next argument.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : May 3, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static int get_times(char *dst, char *key, char *nextarg)
     /* ARGSUSED */
{
   int *num_elements_ptr;
   double **time_list_ptr;
   int num_elements;
   int num_alloc;
   double *time_list;
   double dvalue;
   char *cur, *end, *prev;

   /* Check for a following argument */
   if (nextarg == NULL) {
      (void) fprintf(stderr, 
                     "\"%s\" option requires an additional argument\n",
                     key);
      exit(EXIT_FAILURE);
   }

   /* Get pointers to global variables */
   if (strcmp(key, "-frame_times") == 0) {
      num_elements_ptr = &num_frame_times;
      time_list_ptr = &frame_times;
   }
   else if (strcmp(key, "-frame_widths") == 0) {
      num_elements_ptr = &num_frame_widths;
      time_list_ptr = &frame_widths;
   }
   else {
      (void) fprintf(stderr, "Unknown option \"%s\".\n", key);
      exit(EXIT_FAILURE);
   }

   /* Set up pointers to end of string and first non-space character */
   end = nextarg + strlen(nextarg);
   cur = nextarg;
   while (isspace(*cur)) cur++;
   num_elements = 0;
   num_alloc = 0;
   time_list = NULL;

   /* Loop through string looking for doubles */
   while (cur!=end) {

      /* Get double */
      prev = cur;
      dvalue = strtod(prev, &cur);
      if (cur == prev) {
         (void) fprintf(stderr, 
            "expected vector of doubles for \"%s\", but got \"%s\"\n", 
                        key, nextarg);
         exit(EXIT_FAILURE);
      }

      /* Add the value to the list */
      num_elements++;
      if (num_elements > num_alloc) {
         num_alloc += 20;
         if (time_list == NULL) {
            time_list = 
               malloc(num_alloc * sizeof(*time_list));
         }
         else {
            time_list = 
               realloc(time_list, num_alloc * sizeof(*time_list));
         }
      }
      time_list[num_elements-1] = dvalue;

      /* Skip any spaces */
      while (isspace(*cur)) cur++;

      /* Skip an optional comma */
      if (*cur == VECTOR_SEPARATOR) cur++;

   }

   /* Update the global variables */
   *num_elements_ptr = num_elements;
   if (*time_list_ptr != NULL) {
      free(*time_list_ptr);
   }
   *time_list_ptr = time_list;


   

   return TRUE;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_axis_order
@INPUT      : dst - Pointer to client data from argument table
              key - argument key
              nextArg - argument following key
@OUTPUT     : (nothing) 
@RETURNS    : TRUE or FALSE (so that ParseArgv will discard nextArg only
              when needed)
@DESCRIPTION: Routine called by ParseArgv to set the axis order
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : March 16, 1994 (Peter Neelin)
@MODIFIED   : Copied from mincreshape.c by bert, 29 Oct 2003
---------------------------------------------------------------------------- */
/* "Standard" orientations - index the orientation_names table defined
 * below.
 */
#define TRANSVERSE_ORIENTATION 0
#define SAGITTAL_ORIENTATION 1
#define CORONAL_ORIENTATION 2
#define TIME_FAST_ORIENTATION 3
#define XYZ_ORIENTATION 4
#define YXZ_ORIENTATION 5
#define ZXY_ORIENTATION 6
#define STD_ORIENTATION_COUNT 7
static int get_axis_order(char *dst, char *key, char *nextArg)
{
    char **dim_name_array;
    char *argp;
    int i, j;
    int orientation;

    /* Structure containing information about orientation */
    static char *orientation_names[STD_ORIENTATION_COUNT][MAX_DIMS] = {
        {MItime, MIzspace, MIyspace, MIxspace}, /* TRANSVERSE_ORIENTATION */
        {MItime, MIxspace, MIzspace, MIyspace}, /* SAGITTAL_ORIENTATION */
        {MItime, MIyspace, MIzspace, MIxspace}, /* CORONAL_ORIENTATION */
        {MIzspace, MItime, MIyspace, MIxspace}, /* TIME_FAST_ORIENTATION */
        {MItime, MIxspace, MIyspace, MIzspace}, /* XYZ_ORIENTATION */
        {MItime, MIyspace, MIxspace, MIzspace}, /* YXZ_ORIENTATION */
        {MItime, MIzspace, MIxspace, MIyspace}, /* ZXY_ORIENTATION */
    };

    /* Get pointer to client data */
    dim_name_array = (char **) dst;

    if (!strcmp(key, "-dimorder")) {
        /* Check for next argument */
        if (nextArg == NULL) {
            (void) fprintf(stderr, 
                           "\"%s\" option requires an additional argument\n",
                           key);
            exit(EXIT_FAILURE);
        }

        /* Set up pointers to end of string and first non-space character 
         */
        argp = nextArg;
        while (isspace(*argp)) 
            argp++;

        /* Loop through string looking for space or comma-separated names 
         */
        for (i = 0; i < MAX_DIMS && *argp != '\0'; i++) {
            dim_name_array[i] = argp; /* Get string */

            /* Search for end of dimension name 
             */
            while (!isspace(*argp) && *argp != ARG_SEPARATOR && *argp != '\0')
                argp++;

            if (*argp != '\0') {
                *argp = '\0';
                argp++;
            }

            /* Skip any spaces */
            while (isspace(*argp)) 
                argp++;
        }
        /* BEGIN WEIRDNESS */
        /* parse_args() assumes that the axis_order[] array contains
         * MAX_DIMS (4) entries, with optional values at the
         * _beginning_ of the array.  To make the array we just
         * constructed compatible with that assumption, I need to copy
         * all of the just-specified dimensions to the end of this
         * array, and fill the earlier entries up with some sort of
         * dummy value.
         */
        if (i < MAX_DIMS) {
            int deficit = MAX_DIMS - i; /* How many we're missing */
            for (j = i; j >= 0; j--) {
                dim_name_array[j + deficit] = dim_name_array[j];
            }
            for (j = 0; j < deficit; j++) {
                dim_name_array[j] = "unknown"; /* Just a dummy value. */
            }
        }
        /* END WEIRDNESS */

        /* Return TRUE to let the argv processor know that we used an
         * extra argument.
         */
        return (TRUE);
    }
    else if (!strcmp(key, "-transverse") || !strcmp(key, "-zyx")) {
        orientation = TRANSVERSE_ORIENTATION;
    }
    else if (!strcmp(key, "-sagittal") || !strcmp(key, "-xzy")) {
        orientation = SAGITTAL_ORIENTATION;
    }
    else if (!strcmp(key, "-coronal") || !strcmp(key, "-yzx")) {
        orientation = CORONAL_ORIENTATION;
    }
    else if (!strcmp(key, "-time")) {
        orientation = TIME_FAST_ORIENTATION;
    }
    else if (!strcmp(key, "-xyz")) {
        orientation = XYZ_ORIENTATION;
    }
    else if (!strcmp(key, "-yxz")) {
        orientation = YXZ_ORIENTATION;
    }
    else if (!strcmp(key, "-zxy")) {
        orientation = ZXY_ORIENTATION;
    }
    else {
        (void) fprintf(stderr, 
                       "Unrecognized option \"%s\": internal program error.\n",
                       key);
        exit(EXIT_FAILURE);
    }

    /* Copy from the appropriate row of the static array into the 
     * dimension name array.
     */
    for (i = 0; i < MAX_DIMS; i++) {
        dim_name_array[i] = orientation_names[orientation][i];
    }

    /* Return FALSE to let the argv processor know we didn't use anything
     * extra from the argument list.
     */
    return (FALSE);
}
