/* ----------------------------- MNI Header -----------------------------------
@NAME       : mincreshape
@INPUT      : argc, argv - command line arguments
@OUTPUT     : (none)
@RETURNS    : error status
@DESCRIPTION: Program to allow reshaping of minc lattices: selecting a
              a subrange (or superrange) of dimension indices, eliminating
              dimensions, or re-ordering axes. As well, all icv conversions
              are made accessible on the command line.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : March 10, 1994 (Peter Neelin)
@MODIFIED   : 
 * $Log: mincreshape.c,v $
 * Revision 6.4  2001-04-17 18:40:24  neelin
 * Modifications to work with NetCDF 3.x
 * In particular, changed NC_LONG to NC_INT (and corresponding longs to ints).
 * Changed NC_UNSPECIFIED to NC_NAT.
 * A few fixes to the configure script.
 *
 * Revision 6.3  1999/10/19 14:45:29  neelin
 * Fixed Log subsitutions for CVS
 *
 * Revision 6.2  1998/08/19 13:57:23  neelin
 * Fixed argument parsing to detect errors such as octal numbers
 * containing illegal digits (08) and extraneous characters on the end of
 * arguments.
 *
 * Revision 6.0  1997/09/12  13:24:12  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:25:10  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:01:44  neelin
 * Release of minc version 0.4
 *
 * Revision 3.0  1995/05/15  19:32:36  neelin
 * Release of minc version 0.3
 *
 * Revision 1.6  1995/02/09  14:08:24  neelin
 * Mods to make irix 5 lint happy.
 *
 * Revision 1.5  1995/02/08  19:31:47  neelin
 * Moved ARGSUSED statements for irix 5 lint.
 *
 * Revision 1.4  1994/11/23  11:47:05  neelin
 * Handle image-min/max properly when using icv for normalization.
 *
 * Revision 1.3  94/11/22  08:46:09  neelin
 * Fixed handling of normalization for number of image dimensions > 2.
 * Added appropriate default values of image-max and image-min.
 * 
 * Revision 1.2  94/11/03  08:48:20  neelin
 * Allow chunk_count to have sizes less than full block size.
 * 
 * Revision 1.1  94/11/02  16:21:24  neelin
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
static char rcsid[]="$Header: /private-cvsroot/minc/progs/mincreshape/mincreshape.c,v 6.4 2001-04-17 18:40:24 neelin Exp $";
#endif

#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <minc.h>
#include <ParseArgv.h>
#include <time_stamp.h>
#include <minc_def.h>
#include "mincreshape.h"

/* Main program */

public int main(int argc, char *argv[])
{
   Reshape_info reshape_info;

   /* Get argument information and create the output file */
   get_arginfo(argc, argv, &reshape_info);

   /* Copy the data */
   copy_data(&reshape_info);

   /* Close the output file */
   (void) miattputstr(reshape_info.outmincid, reshape_info.outimgid,
                      MIcomplete, MI_TRUE);
   (void) miclose(reshape_info.outmincid);

   exit(EXIT_SUCCESS);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_arginfo
@INPUT      : argc - number of command-line arguments
              argv - command-line arguments
@OUTPUT     : reshape_info - information for reshaping file
@RETURNS    : (nothing)
@DESCRIPTION: Routine to get information from arguments about input and 
              output files and reshaping.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : March 11, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void get_arginfo(int argc, char *argv[],
                        Reshape_info *reshape_info)
{

   /* Argument variables */
   static int clobber = FALSE;
   static int verbose = TRUE;
   static nc_type datatype = NC_NAT;
   static int is_signed = INT_MIN;
   static double valid_range[2] = {DBL_MAX,DBL_MAX};
   static double image_range[2] = {DBL_MAX,DBL_MAX};
   static int do_norm = FALSE;
   static double pixfillvalue = FILL;
   static int do_scalar= FALSE;
   static int direction = MI_ICV_ANYDIR;
   static int xdirection = INT_MIN;
   static int ydirection = INT_MIN;
   static int zdirection = INT_MIN;
   static int keep_aspect = FALSE;
   static int image_size = MI_ICV_ANYSIZE;
   static int row_size = MI_ICV_ANYSIZE;
   static int col_size = MI_ICV_ANYSIZE;
   static Dimsize_list dimsize_list = {0};
   static char *axis_order[MAX_VAR_DIMS+1];
   static Axis_ranges axis_ranges = {0};
   static long hs_start[MAX_VAR_DIMS] = {LONG_MIN};
   static long hs_count[MAX_VAR_DIMS] = {LONG_MIN};
   static double fillvalue = NOFILL;
   static int max_chunk_size_in_kb = DEFAULT_MAX_CHUNK_SIZE_IN_KB;

   /* Argument table */
   static ArgvInfo argTable[] = {
      {NULL, ARGV_HELP, (char *) NULL, (char *) NULL, 
          "General options:"},
      {"-clobber", ARGV_CONSTANT, (char *) TRUE, (char *) &clobber, 
          "Overwrite existing file."},
      {"-noclobber", ARGV_CONSTANT, (char *) FALSE, (char *) &clobber, 
          "Do not overwrite existing file (default)."},
      {"-verbose", ARGV_CONSTANT, (char *) TRUE, (char *) &verbose, 
          "Print out log messages as processing is being done (default).\n"},
      {"-quiet", ARGV_CONSTANT, (char *) FALSE, (char *) &verbose, 
          "Do not print out any log messages."},
      {"-max_chunk_size_in_kb", ARGV_INT, (char *) 0, 
          (char *) &max_chunk_size_in_kb,
          "Specify the maximum size of the copy buffer (in kbytes)."},

      {NULL, ARGV_HELP, (char *) NULL, (char *) NULL, 
          "Image conversion options (pixel type and range):"},
      {"-filetype", ARGV_CONSTANT, (char *) NC_NAT, (char *) &datatype,
          "Don't do any type conversion (default)."},
      {"-byte", ARGV_CONSTANT, (char *) NC_BYTE, (char *) &datatype,
          "Convert to  byte data"},
      {"-short", ARGV_CONSTANT, (char *) NC_SHORT, (char *) &datatype,
          "Convert to short integer data"},
      {"-int", ARGV_CONSTANT, (char *) NC_INT, (char *) &datatype,
          "Convert to 32-bit integer data"},
      {"-long", ARGV_CONSTANT, (char *) NC_INT, (char *) &datatype,
          "Superseded by -int"},
      {"-float", ARGV_CONSTANT, (char *) NC_FLOAT, (char *) &datatype,
          "Convert to single-precision floating-point data"},
      {"-double", ARGV_CONSTANT, (char *) NC_DOUBLE, (char *) &datatype,
          "Convert to double-precision floating-point data"},
      {"-signed", ARGV_CONSTANT, (char *) TRUE, (char *) &is_signed,
          "Convert to signed integer data"},
      {"-unsigned", ARGV_CONSTANT, (char *) FALSE, (char *) &is_signed,
          "Convert to unsigned integer data"},
      {"-valid_range", ARGV_FLOAT, (char *) 2, (char *) valid_range,
          "Valid range for output data (pixel values)"},

      {"-image_range", ARGV_FLOAT, (char *) 2, (char *) image_range,
          "Normalize images to a given minimum and maximum"},
      {"-normalize", ARGV_CONSTANT, (char *) TRUE, (char *) &do_norm,
          "Normalize images to file minimum and maximum."},
      {"-nonormalize", ARGV_CONSTANT, (char *) FALSE, (char *) &do_norm,
          "Do not normalize images (default)."},
      {"-nopixfill", ARGV_FUNC, (char *) get_fillvalue, 
          (char *) &pixfillvalue,
          "Do not convert out-of-range values in input file."},
      {"-pixfill", ARGV_FUNC, (char *) get_fillvalue, 
          (char *) &pixfillvalue,
          "Replace out-of-range values in input file by smallest value (default)."},
      {"-pixfillvalue", ARGV_FLOAT, (char *) 0, 
          (char *) &pixfillvalue,
          "Specify new value to replace out-of-range values in input file."},

      {NULL, ARGV_HELP, (char *) NULL, (char *) NULL, 
          "Image conversion options (dimension direction and size):"},
      {"-scalar", ARGV_CONSTANT, (char *) TRUE, (char *) &do_scalar,
          "Convert vector images to scalar images."},
      {"-noscalar", ARGV_CONSTANT, (char *) FALSE, (char *) &do_scalar,
          "Do not convert vector images to scalar images (default)."},

      {"+direction", ARGV_CONSTANT, (char *) MI_ICV_POSITIVE, 
          (char *) &direction,
          "Flip images to give positive step value for spatial axes."},
      {"-direction", ARGV_CONSTANT, (char *) MI_ICV_NEGATIVE, 
          (char *) &direction,
          "Flip images to give negative step value for spatial axes."},
      {"-anydirection", ARGV_CONSTANT, (char *) MI_ICV_ANYDIR, 
          (char *) &direction,
          "Don't flip images along spatial axes (default)."},
      {"+xdirection", ARGV_CONSTANT, (char *) MI_ICV_POSITIVE, 
          (char *) &xdirection,
          "Flip images to give positive xspace:step value (left-to-right)."},
      {"-xdirection", ARGV_CONSTANT, (char *) MI_ICV_NEGATIVE, 
          (char *) &xdirection,
          "Flip images to give negative xspace:step value (right-to-left)."},
      {"-xanydirection", ARGV_CONSTANT, (char *) MI_ICV_ANYDIR, 
          (char *) &xdirection,
          "Don't flip images along x-axis."},
      {"+ydirection", ARGV_CONSTANT, (char *) MI_ICV_POSITIVE, 
          (char *) &ydirection,
          "Flip images to give positive yspace:step value (post-to-ant)."},
      {"-ydirection", ARGV_CONSTANT, (char *) MI_ICV_NEGATIVE, 
          (char *) &ydirection,
          "Flip images to give negative yspace:step value (ant-to-post)."},
      {"-yanydirection", ARGV_CONSTANT, (char *) MI_ICV_ANYDIR, 
          (char *) &ydirection,
          "Don't flip images along y-axis."},
      {"+zdirection", ARGV_CONSTANT, (char *) MI_ICV_POSITIVE, 
          (char *) &zdirection,
          "Flip images to give positive zspace:step value (inf-to-sup)."},
      {"-zdirection", ARGV_CONSTANT, (char *) MI_ICV_NEGATIVE, 
          (char *) &zdirection,
          "Flip images to give negative zspace:step value (sup-to-inf)."},
      {"-zanydirection", ARGV_CONSTANT, (char *) MI_ICV_ANYDIR, 
          (char *) &zdirection,
          "Don't flip images along z-axis."},

      {"-keepaspect", ARGV_CONSTANT, (char *) TRUE, (char *) &keep_aspect,
          "Preserve aspect ratio when resizing images."},
      {"-nokeepaspect", ARGV_CONSTANT, (char *) FALSE, (char *) &keep_aspect,
          "Do not preserve aspect ratio when resizing images (default)."},

      {"-imgsize", ARGV_INT, (char *) 0, (char *) &image_size,
          "Specify the desired image size."},
      {"-rowsize", ARGV_INT, (char *) 0, (char *) &row_size,
          "Specify the desired number of rows in the image."},
      {"-colsize", ARGV_INT, (char *) 0, (char *) &col_size,
          "Specify the desired number of columns in the image."},
      {"-dimsize", ARGV_FUNC, (char *) get_dimsize, 
          (char *) &dimsize_list,
          "Specify the size of a named dimension (<dimension>=<size>)."},

      {NULL, ARGV_HELP, (char *) NULL, (char *) NULL, 
          "Reshaping options:"},
      {"-transverse", ARGV_FUNC, (char *) get_axis_order, 
          (char *) axis_order,
          "Write out transverse slices"},
      {"-sagittal", ARGV_FUNC, (char *) get_axis_order, 
          (char *) axis_order,
          "Write out sagittal slices"},
      {"-coronal", ARGV_FUNC, (char *) get_axis_order, 
          (char *) axis_order,
          "Write out coronal slices"},
      {"-dimorder", ARGV_FUNC, (char *) get_axis_order, 
          (char *) axis_order,
          "Specify dimension order (<dim1>,<dim2>,<dim3>,...)."},
      {"-dimrange", ARGV_FUNC, (char *) get_axis_range, 
          (char *) &axis_ranges,
          "Specify range of dimension subscripts (<dim>=<start>[,<count>])."},
      {"-start", ARGV_FUNC, (char *) get_arg_vector, (char *) hs_start,
          "Specifies corner of hyperslab (C conventions for indices)"},
      {"-count", ARGV_FUNC, (char *) get_arg_vector, (char *) hs_count,
          "Specifies edge lengths of hyperslab to read"},

      {NULL, ARGV_HELP, (char *) NULL, (char *) NULL, 
          "Missing data options:"},
      {"-nofill", ARGV_FUNC, (char *) get_fillvalue, 
          (char *) &fillvalue,
          "Use value zero for points outside of input volume (default)"},
      {"-fill", ARGV_FUNC, (char *) get_fillvalue, 
          (char *) &fillvalue,
          "Use a fill value for points outside of input volume"},
      {"-fillvalue", ARGV_FLOAT, (char *) 0, 
          (char *) &fillvalue,
          "Specify a fill value for points outside of input volume"},

      {NULL, ARGV_END, NULL, NULL, NULL}
   };

   /* Other variables */
   char *infile, *outfile;
   char *history, *pname;
   int icvid;

   /* Get the history information and program name */
   history = time_stamp(argc, argv);
   pname = argv[0];

   /* Call ParseArgv */
   if (ParseArgv(&argc, argv, argTable, 0) || (argc!=3)) {
      (void) fprintf(stderr, 
                     "\nUsage: %s [<options>] <infile> <outfile>\n", pname);
      (void) fprintf(stderr,   
                     "       %s [-help]\n\n", pname);
      exit(EXIT_FAILURE);
   }
   infile = argv[1];
   outfile = argv[2];

   /* Save verbose setting */
   reshape_info->verbose = verbose;

   /* Check max chunk size value */
   if (max_chunk_size_in_kb <= 0) {
      (void) fprintf(stderr, "Illegal value for max_chunk_size (%d)\n",
                     max_chunk_size_in_kb);
      exit(EXIT_FAILURE);
   }

   /* Check the x, y and z directions */
   if (xdirection == INT_MIN) xdirection = direction;
   if (ydirection == INT_MIN) ydirection = direction;
   if (zdirection == INT_MIN) zdirection = direction;

   /* Check the row and column size */
   if (row_size == MI_ICV_ANYSIZE) row_size = image_size;
   if (col_size == MI_ICV_ANYSIZE) col_size = image_size;

   /* Check for normalization to specified range */
   if (image_range[0] != DBL_MAX) do_norm = TRUE;

   /* Open the input file */
   reshape_info->inmincid = miopen(infile, NC_NOWRITE);

   /* Get the default datatype */
   get_default_datatype(reshape_info->inmincid, &datatype, &is_signed,
                        valid_range);
   reshape_info->output_datatype = datatype;
   reshape_info->output_is_signed = is_signed;

   /* Create the icv */
   reshape_info->icvid = miicv_create();
   icvid = reshape_info->icvid;

   /* Set the icv properties */

   /* Set datatype properties (get min and max for type from icv) */
   (void) miicv_setint(icvid, MI_ICV_TYPE, datatype);
   if (is_signed != INT_MIN) {
      if (is_signed)
         (void) miicv_setstr(icvid, MI_ICV_SIGN, MI_SIGNED);
      else
         (void) miicv_setstr(icvid, MI_ICV_SIGN, MI_UNSIGNED);
   }
   if (valid_range[0] != DBL_MAX) {
      (void) miicv_setdbl(icvid, MI_ICV_VALID_MIN, valid_range[0]);
      (void) miicv_setdbl(icvid, MI_ICV_VALID_MAX, valid_range[1]);
   }

   /* Check for normalization */
   (void) miicv_setint(icvid, MI_ICV_DO_NORM, do_norm);
   (void) miicv_setint(icvid, MI_ICV_USER_NORM, (image_range[0] != DBL_MAX));
   (void) miicv_setdbl(icvid, MI_ICV_IMAGE_MIN, image_range[0]);
   (void) miicv_setdbl(icvid, MI_ICV_IMAGE_MAX, image_range[1]);

   /* Check for pixel fill value conversion */
   (void) miicv_setint(icvid, MI_ICV_DO_FILLVALUE,
                       (pixfillvalue != NOFILL));
   (void) miicv_setdbl(icvid, MI_ICV_FILLVALUE, pixfillvalue);

   /* Set up for dimension conversion */
   (void) miicv_setint(icvid, MI_ICV_DO_DIM_CONV, TRUE);
   (void) miicv_setint(icvid, MI_ICV_DO_SCALAR, do_scalar);
   (void) miicv_setint(icvid, MI_ICV_XDIM_DIR, xdirection);
   (void) miicv_setint(icvid, MI_ICV_YDIM_DIR, ydirection);
   (void) miicv_setint(icvid, MI_ICV_ZDIM_DIR, zdirection);

   /* Set up for image resizing */
   (void) miicv_setint(icvid, MI_ICV_KEEP_ASPECT, keep_aspect);
   (void) miicv_setint(icvid, MI_ICV_ADIM_SIZE, col_size);
   (void) miicv_setint(icvid, MI_ICV_BDIM_SIZE, row_size);
   setup_dim_sizes(icvid, reshape_info->inmincid, &dimsize_list);

   /* Save reshaping information */
   setup_reshaping_info(icvid, reshape_info->inmincid, 
                        do_norm, fillvalue, do_scalar, 
                        axis_order, &axis_ranges, hs_start, hs_count,
                        max_chunk_size_in_kb,
                        reshape_info);

   /* Attach the icv */
   (void) miicv_attach(icvid, reshape_info->inmincid, 
                       ncvarid(reshape_info->inmincid, MIimage));

   /* Create the output file */
   reshape_info->outmincid = 
      micreate(outfile, (clobber ? NC_CLOBBER : NC_NOCLOBBER));
   setup_output_file(reshape_info->outmincid, history, reshape_info);

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_fillvalue
@INPUT      : dst - Pointer to client data from argument table
              key - argument key
              nextArg - argument following key
@OUTPUT     : (nothing) 
@RETURNS    : FALSE so that ParseArgv will not discard nextArg
@DESCRIPTION: Routine called by ParseArgv to set the fill value
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : March 16, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int get_fillvalue(char *dst, char *key, char *nextArg)
     /* ARGSUSED */
{
   double *dptr;

   /* Get pointer to client data */
   dptr = (double *) dst;

   /* Check key for fill value to set */
   if ((strcmp(key, "-fill") == 0) ||
       (strcmp(key, "-pixfill") == 0)) {
      *dptr = FILL;
   }
   else if ((strcmp(key, "-nofill") == 0) || 
            (strcmp(key, "-nopixfill") == 0)) {
      *dptr = NOFILL;
   }

   return FALSE;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_dimsize
@INPUT      : dst - Pointer to client data from argument table
              key - argument key
              nextArg - argument following key
@OUTPUT     : (nothing) 
@RETURNS    : TRUE so that ParseArgv will discard nextArg
@DESCRIPTION: Routine called by ParseArgv to get a dimension size
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : March 16, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int get_dimsize(char *dst, char *key, char *nextArg)
     /* ARGSUSED */
{
   Dimsize_list *dimsize_list;
   char *size_string;
   int ientry;
   char *cur;

   /* Get pointer to client data */
   dimsize_list = (Dimsize_list *) dst;

   /* Check for next argument */
   if (nextArg == NULL) {
      (void) fprintf(stderr, 
                     "\"%s\" option requires an additional argument\n",
                     key);
      exit(EXIT_FAILURE);
   }

   /* Check that we have enough space in the list */
   if (dimsize_list->nentries >= MAX_VAR_DIMS) {
      (void) fprintf(stderr, "Too many \"%s\" options.\n", key);
      exit(EXIT_FAILURE);
   }
   ientry = dimsize_list->nentries;

   /* Parse the argument (<dim name>=<size>) */

   /* Remove leading space */
   while (ISSPACE(*nextArg)) nextArg++;
   dimsize_list->name[ientry] = nextArg;

   /* Find the '=' */
   size_string = strchr(nextArg, '=');
   if ((size_string == NULL) || (size_string == nextArg)) {
      (void) fprintf(stderr,
                     "\"%s\" option requires the argument <dim>=<size>\n",
                     key);
      exit(EXIT_FAILURE);
   }

   /* Remove trailing blanks on name */
   cur = size_string - 1;
   while ((cur>=nextArg) && ISSPACE(*cur)) cur--;
   cur++;
   *cur = '\0';

   /* Get the size */
   size_string++;
   dimsize_list->size[ientry] = strtol(size_string, &cur, 0);
   if (cur == size_string) {
      (void) fprintf(stderr,
                     "\"%s\" option requires the argument <dim>=<size>\n",
                     key);
      exit(EXIT_FAILURE);
   }

   /* Check for extra stuff (spaces are allowed) */
   while (ISSPACE(*cur)) cur++;
   if (*cur != '\0') {
      (void) fprintf(stderr,
         "\"%s\" option requires the argument <dim>=<size>\n",
                     key);
      exit(EXIT_FAILURE);
   }
   
   dimsize_list->nentries++;

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
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int get_axis_order(char *dst, char *key, char *nextArg)
     /* ARGSUSED */
{
   char **axis_order;
   char *cur;
   int ndims;

   /* Get pointer to client data */
   axis_order = (char **) dst;

   /* Check key */
   if (strcmp(key, "-transverse") == 0) {
      axis_order[0] = MIzspace;
      axis_order[1] = MIyspace;
      axis_order[2] = MIxspace;
      return FALSE;
   }
   if (strcmp(key, "-sagittal") == 0) {
      axis_order[0] = MIxspace;
      axis_order[1] = MIzspace;
      axis_order[2] = MIyspace;
      return FALSE;
   }
   if (strcmp(key, "-coronal") == 0) {
      axis_order[0] = MIyspace;
      axis_order[1] = MIzspace;
      axis_order[2] = MIxspace;
      return FALSE;
   }

   /* Make sure that we have a "-dimorder" argument */
   if (strcmp(key, "-dimorder") != 0) {
      (void) fprintf(stderr, 
                     "Unrecognized option \"%s\": internal program error.\n",
                     key);
      exit(EXIT_FAILURE);
   }

   /* Check for next argument */
   if (nextArg == NULL) {
      (void) fprintf(stderr, 
                     "\"%s\" option requires an additional argument\n",
                     key);
      exit(EXIT_FAILURE);
   }

   /* Set up pointers to end of string and first non-space character */
   cur = nextArg;
   while (ISSPACE(*cur)) cur++;
   ndims = 0;

   /* Loop through string looking for space or comma-separated names */
   while ((ndims < MAX_VAR_DIMS) && (*cur!='\0')) {

      /* Get string */
      axis_order[ndims] = cur;

      /* Search for end of dimension name */
      while (!ISSPACE(*cur) && (*cur != ARG_SEPARATOR) && 
             (*cur != '\0')) cur++;
      if (*cur != '\0') {
         *cur = '\0';
         cur++;
      }
      ndims++;

      /* Skip any spaces */
      while (ISSPACE(*cur)) cur++;

   }

   /* Terminate list with NULL */
   axis_order[ndims] = NULL;

   return TRUE;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_axis_range
@INPUT      : dst - Pointer to client data from argument table
              key - argument key
              nextArg - argument following key
@OUTPUT     : (nothing) 
@RETURNS    : TRUE so that ParseArgv will discard nextArg
@DESCRIPTION: Routine called by ParseArgv to set the axis range
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : March 16, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int get_axis_range(char *dst, char *key, char *nextArg)
     /* ARGSUSED */
{
   Axis_ranges *axis_ranges;
   int ientry;
   char *num_string, *cur;

   /* Get pointer to client data */
   axis_ranges = (Axis_ranges *) dst;

   /* Check for next argument */
   if (nextArg == NULL) {
      (void) fprintf(stderr, 
                     "\"%s\" option requires an additional argument\n",
                     key);
      exit(EXIT_FAILURE);
   }

   /* Check that we have enough space in the list */
   if (axis_ranges->nentries >= MAX_VAR_DIMS) {
      (void) fprintf(stderr, "Too many \"%s\" options.\n", key);
      exit(EXIT_FAILURE);
   }
   ientry = axis_ranges->nentries;

   /* Parse the argument (<dim name>=<start>,[<count>]) */

   /* Remove leading space */
   while (ISSPACE(*nextArg)) nextArg++;
   axis_ranges->name[ientry] = nextArg;

   /* Find the '=' */
   num_string = strchr(nextArg, '=');
   if ((num_string == NULL) || (num_string == nextArg)) {
      (void) fprintf(stderr,
         "\"%s\" option requires the argument <dim>=<start>[,<count>]\n",
                     key);
      exit(EXIT_FAILURE);
   }

   /* Remove trailing blanks on name */
   cur = num_string - 1;
   while ((cur>=nextArg) && ISSPACE(*cur)) cur--;
   cur++;
   *cur = '\0';

   /* Get the start */
   num_string++;
   axis_ranges->start[ientry] = strtol(num_string, &cur, 0);
   if ((cur == num_string) || 
       !(ISSPACE(*cur) || (*cur == ARG_SEPARATOR) || (*cur == '\0'))) {
      (void) fprintf(stderr,
         "\"%s\" option requires the argument <dim>=<start>[,<count>]\n",
                     key);
      exit(EXIT_FAILURE);
   }

   /* Skip any spaces */
   while (ISSPACE(*cur)) cur++;
   
   /* Skip an optional comma */
   if (*cur == ARG_SEPARATOR) cur++;

   /* Look for a count string */
   num_string = cur;
   axis_ranges->count[ientry] = strtol(num_string, &cur, 0);
   if ((cur == num_string) || (axis_ranges->count[ientry] == 0)) {
      axis_ranges->count[ientry] = 0;
   }

   axis_ranges->nentries++;

   /* Check for extra stuff (spaces are allowed) */
   while (ISSPACE(*cur)) cur++;
   if (*cur != '\0') {
      (void) fprintf(stderr,
         "\"%s\" option requires the argument <dim>=<start>[,<count>]\n",
                     key);
      exit(EXIT_FAILURE);
   }
   
   return TRUE;
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
   while (ISSPACE(*cur)) cur++;
   nvals = 0;

   /* Loop through string looking for integers */
   while ((nvals < MAX_VAR_DIMS) && (cur!=end)) {

      /* Get integer */
      prev = cur;
      vector[nvals] = strtol(prev, &cur, 0);
      if ((cur == prev) ||
          !(ISSPACE(*cur) || (*cur == VECTOR_SEPARATOR) || (*cur == '\0'))) {
         (void) fprintf(stderr, 
            "expected vector of integers for \"%s\", but got \"%s\"\n", 
                        key, nextArg);
         exit(EXIT_FAILURE);
      }
      nvals++;

      /* Skip any spaces */
      while (ISSPACE(*cur)) cur++;

      /* Skip an optional comma */
      if (*cur == VECTOR_SEPARATOR) cur++;

   }

   /* Pad with LONG_MIN */
   for (i=nvals; i < MAX_VAR_DIMS; i++) {
      vector[i] = LONG_MIN;
   }

   return TRUE;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_default_datatype
@INPUT      : mincid - id of input minc file
@OUTPUT     : datatype - datatype of file
              is_signed - TRUE if data is signed, FALSE if not. Defaults
                 to FALSE for byte, TRUE otherwise.
              valid_range - DBL_MAX if not known
@RETURNS    : (nothing)
@DESCRIPTION: Routine to get the datatype info from a file. If datatype
              is not NC_NAT, then is_signed only is set to its 
              default. Otherwise, is_signed is only modified if it is set 
              to INT_MIN and valid_range is only modified if it is set to 
              DBL_MAX.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : March 16, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void get_default_datatype(int mincid, nc_type *datatype, int *is_signed,
                                 double valid_range[2])
{
   int imgid;
   int status;
   double vrange[2];
   char string[MI_MAX_ATTSTR_LEN];
   int length;

   /* Get the image variable id */
   imgid = ncvarid(mincid, MIimage);

   /* Check that datatype is not specified */
   if (*datatype != NC_NAT) {
      if (*is_signed == INT_MIN) {
         *is_signed = ((*datatype == NC_BYTE) ? FALSE : TRUE);
      }
      return;
   }

   /* Get data type */
   (void) ncvarinq(mincid, imgid, NULL, datatype, NULL, NULL, NULL);

   /* Look for sign if needed */
   if (*is_signed == INT_MIN) {
      ncopts = 0;
      if (miattgetstr(mincid, imgid, MIsigntype, sizeof(string), string) 
          != NULL) {
         if (strcmp(string, MI_SIGNED) == 0)
            *is_signed = TRUE;
         else if (strcmp(string, MI_UNSIGNED) == 0)
            *is_signed = FALSE;
      }
      ncopts = NCOPTS_DEFAULT;
      if (*is_signed == INT_MIN) {
         *is_signed = ((*datatype == NC_BYTE) ? FALSE : TRUE);
      }
   }

   /* Look for valid range if needed */
   if (valid_range[0] == DBL_MAX) {
      ncopts = 0;
      status=miattget(mincid, imgid, MIvalid_range, 
                      NC_DOUBLE, 2, vrange, &length);
      if ((status!=MI_ERROR) && (length==2)) {
         if (vrange[1] > vrange[0]) {
            valid_range[0] = vrange[0];
            valid_range[1] = vrange[1];
         }
         else {
            valid_range[0] = vrange[1];
            valid_range[1] = vrange[0];
         }
      }
      else {
         status=miattget1(mincid, imgid, MIvalid_max, 
                          NC_DOUBLE, &vrange[1]);
         if (status!=MI_ERROR) valid_range[1] = vrange[1];
  
         status=miattget1(mincid, imgid, MIvalid_min, 
                          NC_DOUBLE, &vrange[0]);
         if (status!=MI_ERROR)
         if (status!=MI_ERROR) valid_range[1] = vrange[1];

      }
      ncopts = NCOPTS_DEFAULT;
   }

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : setup_dim_sizes
@INPUT      : mincid - id of input minc file
              dimsize_list - list of dimension names and sizes
@OUTPUT     : icvid - icvid to modify
@RETURNS    : (nothing)
@DESCRIPTION: Routine to modify an icv so that the appropriate dimensions
              have given sizes
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : May 18, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void setup_dim_sizes(int icvid, int mincid, Dimsize_list *dimsize_list)
{
   int ientry, idim;
   int imgid, dimid;
   int ndims;
   int dim[MAX_VAR_DIMS];
   char dimname[MAX_NC_NAME];
   int image_dim, n_image_dims;

   /* Get image dimension info */
   imgid = ncvarid(mincid, MIimage);
   (void) ncvarinq(mincid, imgid, NULL, NULL, &ndims, dim, NULL);
   if (ndims > 0) {
      (void) ncdiminq(mincid, dim[ndims-1], dimname, NULL);
      if (strcmp(dimname, MIvector_dimension) == 0) ndims--;
   }

   /* Get default number of image dimensions */
   (void) miicv_inqint(icvid, MI_ICV_NUM_IMGDIMS, &n_image_dims);

   /* Loop through list of names, looking for dimensions */
   for (ientry=0; ientry < dimsize_list->nentries; ientry++) {
      ncopts = 0;
      dimid = ncdimid(mincid, dimsize_list->name[ientry]);
      ncopts = NCOPTS_DEFAULT;
      for (idim=0; idim < ndims; idim++) {
         if (dim[idim] == dimid) break;
      }
      if (idim < ndims) {
         image_dim = ndims - idim - 1;
         (void) miicv_setint(icvid, MI_ICV_DIM_SIZE+image_dim,
                             dimsize_list->size[ientry]);
         if (n_image_dims < image_dim+1)
            n_image_dims = image_dim+1;
      }
      else {
         (void) fprintf(stderr, "Unable to set size of dimension \"%s\"\n",
                        dimsize_list->name[ientry]);
         exit(EXIT_FAILURE);
      }
   }

   /* Update number of image dimensions, if needed */
   if (n_image_dims > 2) {
      (void) miicv_setint(icvid, MI_ICV_NUM_IMGDIMS, n_image_dims);
   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : setup_reshaping_info
@INPUT      : mincid - id of input minc file
              do_norm - indicates if normalization is being done already
                 through the icv
              fillvalue - value to use where there is no input value
              do_scalar - TRUE if vector image should be converted to scalar
              axis_order - order of dimensions by name
              axis_ranges - range of subscripts for each axis
              hs_start - starting coordinate of hyperslab to read
              hs_count - edge lengths of hyperslab to read
              max_chunk_size_in_kb - maximum size of copy buffer in kbytes.
@OUTPUT     : reshape_info - information describing the reshaping
@RETURNS    : (nothing)
@DESCRIPTION: Routine to set up reshaping information.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : May 26, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void setup_reshaping_info(int icvid, int mincid, 
                                 int do_norm, double fillvalue, int do_scalar,
                                 char *axis_order[], Axis_ranges *axis_ranges,
                                 long hs_start[], long hs_count[],
                                 int max_chunk_size_in_kb,
                                 Reshape_info *reshape_info)
{
   int input_ndims, input_dim[MAX_VAR_DIMS], order_dim[MAX_VAR_DIMS];
   int output_ndims, norder;
   int idim, jdim, ientry, iloop, order_idim;
   int input_dim_used[MAX_VAR_DIMS];
   char name[MAX_NC_NAME];
   int *o2i, *i2o;
   nc_type datatype;
   int min_ndims, max_ndims, min_dim[MAX_VAR_DIMS], max_dim[MAX_VAR_DIMS];
   int minid, maxid, dimid;
   long total_size, size;
   int nstart, ncount;
   int has_vector_dimension;
   int num_imgdims;
   int fastest_input_img_dim, fastest_output_img_dim;
   long length;
   long first, last;

   /* Get input file dimension info */
   (void) ncvarinq(mincid, ncvarid(mincid, MIimage), NULL, NULL, 
                   &input_ndims, input_dim, NULL);
   (void) ncdiminq(mincid, input_dim[input_ndims-1], name, NULL);
   has_vector_dimension = (strcmp(name, MIvector_dimension) == 0);
   fastest_input_img_dim = (has_vector_dimension ? 
                            input_ndims-2 : input_ndims-1);
   if (do_scalar && has_vector_dimension)
      input_ndims--;
   reshape_info->input_ndims = input_ndims;

   /* Check length of hs_start and hs_count vectors */
   for (nstart=0; (nstart<MAX_VAR_DIMS) && (hs_start[nstart]!=LONG_MIN); 
        nstart++) {}
   for (ncount=0; (ncount<MAX_VAR_DIMS) && (hs_count[ncount]!=LONG_MIN); 
        ncount++) {}
#if 0
   if ((nstart != 0) && (ncount != 0) && (nstart != ncount)) {
      (void) fprintf(stderr, 
                     "Dimensions of start or count vectors not equal.\n");
      exit(EXIT_FAILURE);
   }
#endif
   if ((ncount > input_ndims) || (nstart > input_ndims)) {
      (void) fprintf(stderr, 
                     "Start and/or count vectors are too long.\n");
      exit(EXIT_FAILURE);
   }

   /* Get start and count from file info and from hs_start and hs_count */
   (void) miicv_inqint(icvid, MI_ICV_NUM_IMGDIMS, &num_imgdims);
   for (idim=0; idim < input_ndims; idim++) {
      (void) ncdiminq(mincid, input_dim[idim], NULL, 
                      &reshape_info->input_size[idim]);
      if ((idim > fastest_input_img_dim-num_imgdims) && 
          (idim <= fastest_input_img_dim)) {
         (void) miicv_inqlong(icvid, 
                              MI_ICV_DIM_SIZE+fastest_input_img_dim-idim,
                              &length);
         if (length > 0) {
            reshape_info->input_size[idim] = length;
         }
      }
      if (idim < nstart)
         reshape_info->input_start[idim] = hs_start[idim];
      else
         reshape_info->input_start[idim] = 0;
      if (idim < ncount)
         reshape_info->input_count[idim] = hs_count[idim];
      else
         reshape_info->input_count[idim] = reshape_info->input_size[idim];
   }
   
   /* Get input dimension start and count from axis_ranges variable */
   for (ientry=0; ientry < axis_ranges->nentries; ientry++) {
      dimid = ncdimid(mincid, axis_ranges->name[ientry]);
      for (idim=0; idim < input_ndims; idim++) {
         if (dimid == input_dim[idim]) break;
      }
      if (idim >= input_ndims) {
         (void) fprintf(stderr, "Unknown image dimension \"%s\"\n",
                        axis_ranges->name[ientry]);
         exit(EXIT_FAILURE);
      }
      reshape_info->input_start[idim] = axis_ranges->start[ientry];
      reshape_info->input_count[idim] = axis_ranges->count[ientry];
   }

   /* Check to see if we will need a fill value */
   reshape_info->need_fillvalue = FALSE;
   for (idim=0; idim < input_ndims; idim++) {
      first = reshape_info->input_start[idim];
      last = first;
      if (reshape_info->input_count[idim] > 0)
         last += reshape_info->input_count[idim] - 1;
      else if (reshape_info->input_count[idim] < 0)
         last += reshape_info->input_count[idim] + 1;
      if ((first < 0) || (first >= reshape_info->input_size[idim]) ||
          (last < 0) || (last >= reshape_info->input_size[idim]))
         reshape_info->need_fillvalue = TRUE;
   }

   /* Get output dimensions in terms of input */

   /* Add up number of output dimensions */
   output_ndims = 0;
   for (idim=0; idim < input_ndims; idim++) {
      if (reshape_info->input_count[idim] != 0) output_ndims++;
   }
   reshape_info->output_ndims = output_ndims;

   /* Get dim ids for specified order */
   for (norder=0; norder < MAX_VAR_DIMS+1; norder++) {
      if (axis_order[norder] == NULL) break;
      order_dim[norder] = ncdimid(mincid, axis_order[norder]);
   }
   if (norder > output_ndims) {
      for (idim=0; idim < output_ndims; idim++) {
         order_dim[idim] = order_dim[idim + norder - output_ndims];
      }
      norder = output_ndims;
   }

   /* Keep track of input dims already used in output (dimensions that
      are disappearing are considered used) */
   for (idim=0; idim < input_ndims; idim++) {
      input_dim_used[idim] = (reshape_info->input_count[idim] == 0);
   }

   /* Re-order dimensions */
   for (idim=output_ndims-1; idim >= 0; idim--) {    /* Output dim loop */
      order_idim = idim - output_ndims + norder;
      for (jdim=input_ndims-1; jdim >= 0; jdim--) {    /* Input dim loop */
         /* For specified dimensions, look for corresponding input dim */
         if (order_idim >= 0) {
            if (input_dim[jdim] == order_dim[order_idim]) break;
         }
         /* For remaining dims, take next available */
         else {
            if (!input_dim_used[jdim]) break;
         }
      }
      /* Check for error */
      if ((jdim < 0) || input_dim_used[jdim]) {
         if (order_idim >= 0) {
            (void) fprintf(stderr, 
      "Cannot re-order dimension \"%s\" (not found, repeated or removed).\n",
                           axis_order[order_idim]);
         }
         else {
            (void) fprintf(stderr, "Program error in re-ordering axes.\n");
         }
         exit(EXIT_FAILURE);
      }
      /* Save dimension mapping */
      input_dim_used[jdim] = TRUE;
      reshape_info->map_out_to_in[idim] = jdim;
   }

   /* Get mapping from input to output (-1 means no mapping) */
   for (idim=0; idim < input_ndims; idim++) {
      reshape_info->map_in_to_out[idim] = -1;
   }
   for (idim=0; idim < output_ndims; idim++) {
      reshape_info->map_in_to_out[reshape_info->map_out_to_in[idim]] = idim;
   }

   /* Get fastest varying output image dimension (excluding vector dim) */
   idim = input_dim[reshape_info->map_out_to_in[output_ndims-1]];
   (void) ncdiminq(mincid, idim, name, NULL);
   fastest_output_img_dim = ((strcmp(name, MIvector_dimension) == 0) ?
                             output_ndims-2 : output_ndims-1);

   /* Save dimensions used in blocks and chunks */
   o2i = reshape_info->map_out_to_in;
   i2o = reshape_info->map_in_to_out;
   (void) miicv_inqint(reshape_info->icvid, MI_ICV_TYPE, (int *) &datatype);
   total_size = nctypelen(datatype);
   for (idim=0; idim < output_ndims; idim++) {
      reshape_info->dim_used_in_block[idim] = FALSE;
      reshape_info->chunk_count[idim] = 1;
   }
   for (iloop=0; iloop < 6; iloop++) {
      /* Go through possible dimensions in descending order of priority.
         We start with the fastest varying dimension, but allow for the
         possibility of vector dimensions in either volume (looping twice
         on the same dimension is not a problem). Note that idim refers to
         an output dimension. */
      switch (iloop) {
      case 0: idim = output_ndims-1; break;
      case 1: idim = i2o[input_ndims-1]; break;
      case 2: idim = fastest_output_img_dim; break;
      case 3: idim = i2o[fastest_input_img_dim]; break;
      case 4: idim = fastest_output_img_dim-1; break;
      case 5: idim = i2o[fastest_input_img_dim-1]; break;
      default: idim = -1;
      }
      if (idim != -1) size = ABS(reshape_info->input_count[o2i[idim]]);
      else size = 0;
      if (size == 0) idim = -1;
      if ((idim != -1) && !reshape_info->dim_used_in_block[idim]) {
         reshape_info->dim_used_in_block[idim] = TRUE;
         if ((total_size * size) > (max_chunk_size_in_kb * 1024))
            size = max_chunk_size_in_kb * 1024 / total_size;
         if (size < 1) size = 1;
         reshape_info->chunk_count[idim] = size;
         total_size *= size;
      }
   }

   /* Make sure that all input image dimensions are considered used in the
      block */
   for (iloop=2; iloop < num_imgdims; iloop++) {
      idim = fastest_input_img_dim - iloop;
      if (idim >= 0)
         reshape_info->dim_used_in_block[i2o[idim]] = TRUE;
   }

   /* If we are doing icv normalization, then all dimensions are used in the
      block */
   if (do_norm) {
      for (idim=0; idim < output_ndims; idim++) {
         reshape_info->dim_used_in_block[idim] = TRUE;
      }
   }

   /* Save fillvalue */
   reshape_info->fillvalue = fillvalue;

   /* Are we doing normalization through the icv? */
   reshape_info->do_icv_normalization = do_norm;

   /* Do we need to normalize to slices to a block min and max? */
   if (do_norm) {
      reshape_info->do_block_normalization = FALSE;
   }
   else {
      reshape_info->do_block_normalization = FALSE;
      
      /* Loop through block dimensions and check if image-min/max varies
         on the dimension */
      ncopts = 0;
      minid = ncvarid(mincid, MIimagemin);
      maxid = ncvarid(mincid, MIimagemax);
      ncopts = NCOPTS_DEFAULT;
      if ((minid != MI_ERROR) && (maxid != MI_ERROR)) {
         (void) ncvarinq(mincid, minid, NULL, NULL, &min_ndims, min_dim, NULL);
         (void) ncvarinq(mincid, maxid, NULL, NULL, &max_ndims, max_dim, NULL);
         for (idim=0; idim < input_ndims; idim++) {
            jdim = reshape_info->map_in_to_out[idim];
            if ((jdim>=0) && reshape_info->dim_used_in_block[jdim]) {
               dimid = input_dim[idim];
               for (jdim=0; jdim < min_ndims; jdim++) {
                  if (min_dim[jdim] == dimid) {
                     reshape_info->do_block_normalization = TRUE;
                  }
               }
               for (jdim=0; jdim < max_ndims; jdim++) {
                  if (max_dim[jdim] == dimid) {
                     reshape_info->do_block_normalization = TRUE;
                  }
               }
            }
         }
      }

   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : setup_output_file
@INPUT      : mincid - id of output minc file
              history - string to be added to history list
              reshape_info - information describing the reshaping
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to set up the output file
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June 16, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void setup_output_file(int mincid, char *history, 
                              Reshape_info *reshape_info)
{
   int output_ndims, output_dim[MAX_VAR_DIMS];
   int input_ndims, input_dim[MAX_VAR_DIMS];
   int minmax_ndims, minmax_dim[MAX_VAR_DIMS];
   int idim, odim, iloop;
   int varid, icvid, imgid, varid2;
   char dimname[MAX_NC_NAME];
   long length;
   int excluded_vars[2*MAX_VAR_DIMS + 10];
   int nexcluded;
   nc_type datatype;
   char signtype[MI_MAX_ATTSTR_LEN];
   double valid_range[2];
   char *string;
   int att_length;
   int has_vector_dimension;
   int fastest_img_dim;

   /* Get useful info */
   output_ndims = reshape_info->output_ndims;
   (void) ncvarinq(reshape_info->inmincid, 
                   ncvarid(reshape_info->inmincid, MIimage), 
                   NULL, NULL, &input_ndims, input_dim, NULL);
   input_ndims = reshape_info->input_ndims;

   /* Check for vector dimension */
   (void) ncdiminq(reshape_info->inmincid,
                   input_dim[input_ndims-1], dimname, NULL);
   has_vector_dimension = (strcmp(dimname, MIvector_dimension) == 0);
   fastest_img_dim = (has_vector_dimension ? input_ndims-2 : input_ndims-1);

   /* Create image dimensions */
   for (odim=0; odim < output_ndims; odim++) {
      idim = reshape_info->map_out_to_in[odim];
      length = ABS(reshape_info->input_count[idim]);
      (void) ncdiminq(reshape_info->inmincid, input_dim[idim], dimname, NULL);
      output_dim[odim] = ncdimdef(mincid, dimname, length);
   }

   /* Copy all variables except dimensions and dimension widths */
   ncopts = 0;
   nexcluded = 0;
   for (idim = 0; idim < input_ndims; idim++) {
      (void) ncdiminq(reshape_info->inmincid, input_dim[idim], dimname, NULL);
      if ((varid=ncvarid(reshape_info->inmincid, dimname)) != MI_ERROR)
         excluded_vars[nexcluded++] = varid;
      (void) strncat(dimname, DIM_WIDTH_SUFFIX, 
                     sizeof(dimname)-strlen(dimname)-1);
      if ((varid=ncvarid(reshape_info->inmincid, dimname)) != MI_ERROR)
         excluded_vars[nexcluded++] = varid;
   }
   if ((varid=ncvarid(reshape_info->inmincid, MIimage)) != MI_ERROR)
      excluded_vars[nexcluded++] = varid;
   if ((varid=ncvarid(reshape_info->inmincid, MIimagemax)) != MI_ERROR)
      excluded_vars[nexcluded++] = varid;
   if ((varid=ncvarid(reshape_info->inmincid, MIimagemin)) != MI_ERROR)
      excluded_vars[nexcluded++] = varid;
   (void) micopy_all_var_defs(reshape_info->inmincid, mincid, 
                              nexcluded, excluded_vars);
   ncopts = NCOPTS_DEFAULT;

   /* Create image dimension variables */
   for (odim=0; odim < output_ndims; odim++) {
      idim = reshape_info->map_out_to_in[odim];
      create_dim_var(mincid, output_dim[odim], 
                     reshape_info->icvid,
                     fastest_img_dim - idim,
                     reshape_info->inmincid,
                     reshape_info->input_start[idim],
                     reshape_info->input_count[idim]);
   }

   /* Get basic image variable info */
   icvid = reshape_info->icvid;
   (void) miicv_inqint(icvid, MI_ICV_TYPE, (int *) &datatype);
   (void) miicv_inqstr(icvid, MI_ICV_SIGN, signtype);
   (void) miicv_inqdbl(icvid, MI_ICV_VALID_MIN, &valid_range[0]);
   (void) miicv_inqdbl(icvid, MI_ICV_VALID_MAX, &valid_range[1]);

   /* Set the valid range to include 0.0 for floating point if needed */
   if (reshape_info->need_fillvalue &&
       ((datatype == NC_FLOAT) || (datatype == NC_DOUBLE)) &&
       (reshape_info->fillvalue == NOFILL)) {
      if (0.0 < valid_range[0]) valid_range[0] = 0.0;
      if (0.0 > valid_range[1]) valid_range[1] = 0.0;
   }

   /* Create the image variable */
   imgid = micreate_std_variable(mincid, MIimage, datatype,
                                 output_ndims, output_dim);
   reshape_info->outimgid = imgid;
   (void) micopy_all_atts(reshape_info->inmincid, 
                          ncvarid(reshape_info->inmincid, MIimage),
                          mincid, imgid);
   (void) miattputstr(mincid, imgid, MIsigntype, signtype);
   (void) ncattput(mincid, imgid, MIvalid_range, NC_DOUBLE, 2, valid_range);
   (void) miattputstr(mincid, imgid, MIcomplete, MI_FALSE);

   /* Create the imagemax/min variables */
   minmax_ndims = 0;
   for (odim=0; odim < output_ndims; odim++) {
      if (!reshape_info->dim_used_in_block[odim]) {
         minmax_dim[minmax_ndims++] = output_dim[odim];
      }
   }
   for (iloop=0; iloop < 2; iloop++) {
      if (iloop == 0)
         string = MIimagemin;
      else
         string = MIimagemax;
      varid = micreate_std_variable(mincid, string, NC_DOUBLE,
                                    minmax_ndims, minmax_dim);
      ncopts = 0;
      varid2 = ncvarid(reshape_info->inmincid, string);
      ncopts = NCOPTS_DEFAULT;
      if (varid2 != MI_ERROR)
         (void) micopy_all_atts(reshape_info->inmincid, 
                                varid2, mincid, varid);
   }

   /* Add history */
   ncopts=0;
   if ((ncattinq(mincid, NC_GLOBAL, MIhistory, &datatype, &att_length) 
        == MI_ERROR) || (datatype != NC_CHAR))
      att_length = 0;
   att_length += strlen(history) + 1;
   string = MALLOC(att_length);
   string[0] = '\0';
   (void) miattgetstr(mincid, NC_GLOBAL, MIhistory, att_length, string);
   ncopts = NCOPTS_DEFAULT;
   (void) strcat(string, history);
   (void) miattputstr(mincid, NC_GLOBAL, MIhistory, string);
   FREE(string);

   /* Get into data mode */
   (void) ncsetfill(mincid, NC_NOFILL);
   (void) ncendef(mincid);

   /* Copy all the other data */
   (void) micopy_all_var_values(reshape_info->inmincid, mincid,
                                nexcluded, excluded_vars);

   /* Copy the dimension variable values, if needed */
   for (odim=0; odim < output_ndims; odim++) {
      idim = reshape_info->map_out_to_in[odim];
      copy_dimension_values(mincid, output_dim[odim], 
                            reshape_info->inmincid,
                            reshape_info->input_start[idim],
                            reshape_info->input_count[idim]);
   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_dim_var
@INPUT      : outmincid - id of output minc file
              outdimid - id of output dimension
              inicvid - id of input icv
              cur_image_dim - image dim number of current dimension in
                 input icv (0 is fastest varying dimension)
              inmincid - id of input minc file
              input_start - start index of input dimension
              input_count - count for input dimension (may be negative)
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Creates a dimension variable and sets attributes properly.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June 16, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void create_dim_var(int outmincid, int outdimid,
                           int inicvid, int cur_image_dim, int inmincid, 
                           long input_start, long input_count)
{
   int invarid, outvarid;
   int num_image_dims, var_ndims;
   int is_regular, step_found, start_found, changed_spacing;
   double dim_start, dim_step, icv_start, icv_step;
   char dimname[MAX_NC_NAME];
   char spacing[MAX_NC_NAME];

   /* Get the dimension name */
   (void) ncdiminq(outmincid, outdimid, dimname, NULL);

   /* Get number of image dimensions */
   (void) miicv_inqint(inicvid, MI_ICV_NUM_IMGDIMS, &num_image_dims);

   /* Get step and start and spacing for dimension */
   dim_step = 1.0;
   dim_start = 0.0;
   is_regular = TRUE;
   changed_spacing = ((input_start != 0) || (input_count <= 0));
   ncopts = 0;
   invarid = ncvarid(inmincid, dimname);
   if (invarid != MI_ERROR) {
      step_found = (miattget1(inmincid, invarid, MIstep, NC_DOUBLE, 
                              &dim_step) != MI_ERROR);
      start_found = (miattget1(inmincid, invarid, MIstart, NC_DOUBLE, 
                               &dim_start) != MI_ERROR);
      (void) ncvarinq(inmincid, invarid, NULL, NULL, &var_ndims, NULL, NULL);
      if (var_ndims > 0) {
         (void) strcpy(spacing, MI_IRREGULAR);
         (void) miattgetstr(inmincid, invarid, MIspacing, sizeof(spacing),
                            spacing);
         is_regular = (strcmp(spacing, MI_REGULAR) == 0);
      }
   }
   ncopts = NCOPTS_DEFAULT;

   /* Is the sampling changed because of the icv? */
   if ((cur_image_dim < num_image_dims) && (cur_image_dim >= 0)) {
      (void) miicv_inqdbl(inicvid, MI_ICV_DIM_STEP+cur_image_dim, &icv_step);
      (void) miicv_inqdbl(inicvid, MI_ICV_DIM_START+cur_image_dim, &icv_start);
      if ((icv_step != dim_step) || (icv_start != dim_start)) {
         dim_step = icv_step;
         dim_start = icv_start;
         is_regular = TRUE;
         changed_spacing = TRUE;
      }
   }

   /* If spacing is not changed and the input variable does not exist don't
      create the variable */
   if (!changed_spacing && (invarid == MI_ERROR)) 
      return;

   /* Calculate the new dim_start and dim_step (if needed) */
   dim_start += input_start * dim_step;
   if (input_count < 0) dim_step = -dim_step;

   /* Create the variable */
   var_ndims = (is_regular ? 0 : 1);
   ncopts = 0;
   outvarid = micreate_std_variable(outmincid, dimname, NC_DOUBLE, 
                                        var_ndims, &outdimid);
   ncopts = NCOPTS_DEFAULT;
   if (outvarid == MI_ERROR) {
      outvarid = ncvardef(outmincid, dimname, NC_DOUBLE, var_ndims, &outdimid);
   }
   if (invarid != MI_ERROR)
      (void) micopy_all_atts(inmincid, invarid, outmincid, outvarid);
   if (is_regular || step_found)
      (void) miattputdbl(outmincid, outvarid, MIstep, dim_step);
   if (is_regular || start_found)
      (void) miattputdbl(outmincid, outvarid, MIstart, dim_start);

   /* Create width variable if needed */
   ncopts = 0;
   (void) strncat(dimname, DIM_WIDTH_SUFFIX, 
                  sizeof(dimname)-strlen(dimname)-1);
   invarid = ncvarid(inmincid, dimname);
   if (invarid != MI_ERROR) {
      (void) ncvarinq(inmincid, invarid, NULL, NULL, &var_ndims, NULL, NULL);
      if (var_ndims > 0) var_ndims = 1;
      outvarid = micreate_std_variable(outmincid, dimname, NC_DOUBLE, 
                                           var_ndims, &outdimid);
      if (outvarid == MI_ERROR) {
         outvarid = ncvardef(outmincid, dimname, NC_DOUBLE, 
                             var_ndims, &outdimid);
      }
      if (invarid != MI_ERROR)
         (void) micopy_all_atts(inmincid, invarid, outmincid, outvarid);
   }
   ncopts = NCOPTS_DEFAULT;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : copy_dimension_values
@INPUT      : outmincid - id of output minc file
              outdimid - id of output dimension
              inmincid - id of input minc file
              input_start - start index of input dimension
              input_count - length of input dimension
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Copies the data for a dimension variable.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June 16, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void copy_dimension_values(int outmincid, int outdimid, int inmincid,
                                  long input_start, long input_count)
{
   char dimname[MAX_NC_NAME];
   char varname[MAX_NC_NAME];

   /* Get the dimension name */
   (void) ncdiminq(outmincid, outdimid, dimname, NULL);
   (void) strcpy(varname, dimname);

   /* Copy the dimension coorindates */
   copy_dim_var_values(outmincid, dimname, varname, inmincid,
                       input_start, input_count);

   /* Copy the dimension widths */
   (void) strncat(varname, DIM_WIDTH_SUFFIX, 
                  sizeof(varname)-strlen(varname)-1);
   copy_dim_var_values(outmincid, dimname, varname, inmincid,
                       input_start, input_count);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : copy_dim_var_values
@INPUT      : outmincid - id of output minc file
              dimname - name of dimension
              varname - name of variable to copy
              inmincid - id of input minc file
              input_start - start index of input dimension
              input_count - length of input dimension
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Copies the data for a dimension variable.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June 16, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void copy_dim_var_values(int outmincid, char *dimname, char *varname,
                                int inmincid,
                                long input_start, long input_count)
{
   int invarid, outvarid;
   int in_ndims, in_dim[MAX_VAR_DIMS], out_ndims;
   char string[MAX_NC_NAME];
   int good_data, is_width, flip_dimension;
   long output_index, input_index, index, input_length;
   double value, dim_width, dim_step, dim_start;

   /* Do we need to copy data? */
   ncopts = 0;
   outvarid = ncvarid(outmincid, varname);
   ncopts = NCOPTS_DEFAULT;
   if (outvarid == MI_ERROR) return;
   (void) ncvarinq(outmincid, outvarid, NULL, NULL, &out_ndims, NULL, NULL);
   if (out_ndims != 1) return;

   /* Is this a width variable? */
   index = strlen(varname) - strlen(DIM_WIDTH_SUFFIX);
   if (index < 0)
      is_width = FALSE;
   else
      is_width = (strcmp(&varname[index], DIM_WIDTH_SUFFIX));

   /* Check if there is a valid dimension variable from which to copy */
   ncopts = 0;
   invarid = ncvarid(inmincid, varname);
   ncopts = NCOPTS_DEFAULT;
   good_data = (invarid != MI_ERROR);
   if (good_data) {
      (void) ncvarinq(inmincid, invarid, NULL, NULL, &in_ndims, in_dim, NULL);
      good_data = (in_ndims == 1);
   }
   if (good_data) {
      (void) ncdiminq(inmincid, in_dim[0], string, &input_length);
      good_data = (strcmp(string, dimname) == 0);
   }

   /* Get data from input file for estimating unknown values */
   if (is_width) {       /* Get width for width variables */
      dim_width = 0.0;
      ncopts = 0;
      (void) miattget1(inmincid, invarid, MIwidth, NC_DOUBLE, &dim_width);
      ncopts = NCOPTS_DEFAULT;
   }
   else {                /* Get step and start for coordinate variables */
      dim_step = 1.0;
      dim_start = 0.0;
      if (good_data) {
         input_index = 0;
         (void) mivarget1(inmincid, invarid, &input_index, NC_DOUBLE, NULL,
                          &dim_start);
         input_index = input_length - 1;
         if (input_length <= 1) {
            ncopts = 0;
            (void) miattget1(inmincid, invarid, MIstep, NC_DOUBLE, &dim_step);
            ncopts = NCOPTS_DEFAULT;
         }
         else {
            (void) mivarget1(inmincid, invarid, &input_index, NC_DOUBLE, NULL,
                             &value);
            dim_step = (value - dim_start) / ((double) input_length - 1);
         }
      }
      else {
         ncopts = 0;
         (void) miattget1(inmincid, invarid, MIstep, NC_DOUBLE, &dim_step);
         (void) miattget1(inmincid, invarid, MIstart, NC_DOUBLE, &dim_start);
         ncopts = NCOPTS_DEFAULT;
      }
      if (dim_step == 0.0) dim_step = 1.0;
   }

   /* Loop through output values */
   flip_dimension = (input_count < 0);
   input_count = ABS(input_count);
   for (output_index=0; output_index < input_count; output_index++) {

      /* Get input value */
      if (!flip_dimension) {
         input_index = input_start + output_index;
      }
      else {
         input_index = input_start - output_index;
      }
      if (good_data && (input_index >= 0) && (input_index < input_length)) {
         (void) mivarget1(inmincid, invarid, &input_index, NC_DOUBLE, NULL,
                          &value);
      }
      else {
         if (is_width) {
            value = dim_width;
         }
         else {
            value = input_index * dim_step + dim_start;
         }
      }
      (void) mivarput1(outmincid, outvarid, &output_index, NC_DOUBLE, NULL,
                       &value);

   }

}

