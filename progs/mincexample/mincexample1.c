/* ----------------------------- MNI Header -----------------------------------
@NAME       : mincexample
@INPUT      : argc, argv - command line arguments
@OUTPUT     : (none)
@RETURNS    : status
@DESCRIPTION: Example program to copy a transverse minc volume into a byte
              volume and ensure positive orientation for image axes.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 24, 1993 (Peter Neelin)
@MODIFIED   : $Log: mincexample1.c,v $
@MODIFIED   : Revision 1.3  1993-08-26 12:31:17  neelin
@MODIFIED   : Added define so that minc_def.h is optionally included.
@MODIFIED   :
 * Revision 1.2  93/08/26  11:57:00  neelin
 * Removed include of ParseArgv stuff and added MALLOC and FREE definitions
 * from minc_def.h so that it is not always needed.
 * 
 * Revision 1.1  93/08/26  11:45:49  neelin
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
static char rcsid[]="$Header: /private-cvsroot/minc/progs/mincexample/mincexample1.c,v 1.3 1993-08-26 12:31:17 neelin Exp $";
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <minc.h>
#include <time_stamp.h>

/* Include the standard minc definitions for cross-platform compilation
   if we are compiling the package, otherwise, just define MALLOC and
   FREE */
#ifdef COMPILING_MINC_PACKAGE
#  include <minc_def.h>
#else
#  define MALLOC(size) ((void *) malloc(size))
#  define FREE(ptr) free(ptr)
#endif

/* Constants */
#define public
#ifndef TRUE
#  define TRUE 1
#  define FALSE 0
#endif

#define NUMBER_OF_DIMENSIONS 3
#define NC_OPTS_VAL NC_VERBOSE | NC_FATAL

/* Types */
typedef struct {
   long nslices;             /* Number of slices */
   long nrows;               /* Number of rows in image */
   long ncolumns;            /* Number of columns in image */
   char dimension_names[NUMBER_OF_DIMENSIONS][MAX_NC_NAME];
                             /* Dimension names */
   unsigned char *data;      /* Pointer to volume data */
   double maximum;           /* Volume maximum */
   double minimum;           /* Volume minimum */
} Volume;

/* Function prototypes */
public int main(int argc, char *argv[]);
public void load_volume(char *infile, Volume *volume);
public void get_dimension_info(char *infile, int mincid, int imgid, 
                               Volume *volume);
public void setup_icv(int icvid);
public void read_volume_data(int icvid, Volume *volume);
public void save_volume(char *infile, char *outfile, char *arg_string, 
                        Volume *volume);
public void setup_variables(int inmincid, int mincid, Volume *volume, 
                            char *arg_string);
public void write_volume_data(int icvid, Volume *volume);

/* Main program */

public int main(int argc, char *argv[])
{
   char *infile, *outfile;
   Volume volume;
   char *arg_string;

   /* Get arguments */
   if (argc != 3) {
      (void) fprintf(stderr, "Usage: %s <infile.mnc> <outfile.mnc>\n",
                     argv[0]);
      exit(EXIT_FAILURE);
   }
   infile = argv[1];
   outfile = argv[2];

   /* Save list of arguments as string */
   arg_string = time_stamp(argc, argv);

   /* Read in input volume */
   load_volume(infile, &volume);

   /* Save the volume, copying information from input file */
   save_volume(infile, outfile, arg_string, &volume);

   exit(EXIT_SUCCESS);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : load_volume
@INPUT      : infile - input file name
@OUTPUT     : volume - input volume data
@RETURNS    : (nothing)
@DESCRIPTION: Routine to read in a transverse volume and store the data in
              a volume structure.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void load_volume(char *infile, Volume *volume)
{
   int icvid, mincid, imgid;

   /* Open the image file */
   mincid = ncopen(infile, NC_NOWRITE);

   /* Get id of image variable */
   imgid = ncvarid(mincid, MIimage);

   /* Get dimension information */
   get_dimension_info(infile, mincid, imgid, volume);

   /* Create and set up icv for input */
   icvid = miicv_create();
   setup_icv(icvid);

   /* Attach the icv to the file */
   (void) miicv_attach(icvid, mincid, imgid);

   /* Read in volume data */
   read_volume_data(icvid, volume);

   /* Close the file and free the icv */
   (void) ncclose(mincid);
   (void) miicv_free(icvid);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_dimension_info
@INPUT      : infile - name of input file
              mincid - id of minc file
              imgid - id of image variable
@OUTPUT     : volume - input volume data
@RETURNS    : (nothing)
@DESCRIPTION: Routine to get dimension information from an input file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 26, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void get_dimension_info(char *infile, int mincid, int imgid, 
                               Volume *volume)
{
   int idim, ndims;
   int dim[MAX_VAR_DIMS];
   long *dimlength;

   /* Get the list of dimensions subscripting the image variable */
   (void) ncvarinq(mincid, imgid, NULL, NULL, &ndims, dim, NULL);

   /* Check that we only have three dimensions */
   if (ndims != NUMBER_OF_DIMENSIONS) {
      (void) fprintf(stderr, 
                     "File %s does not have exactly %d dimensions\n",
                     infile, NUMBER_OF_DIMENSIONS);
      exit(EXIT_FAILURE);
   }

   /* Loop through dimensions, checking them and getting their sizes */
   for (idim=0; idim<ndims; idim++) {

      /* Get a pointer to the appropriate dimension size */
      switch (idim) {
      case 0: dimlength = &(volume->nslices) ; break;
      case 1: dimlength = &(volume->nrows) ; break;
      case 2: dimlength = &(volume->ncolumns) ; break;
      }

      /* Get dimension name and size */
      (void) ncdiminq(mincid, dim[idim], volume->dimension_names[idim], 
                      dimlength);

   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : setup_icv
@INPUT      : icvid - id of icv to set up
@OUTPUT     : (nothing)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to set up an icv
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 26, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void setup_icv(int icvid)
{
   /* Set desired type */
   (void) miicv_setint(icvid, MI_ICV_TYPE, NC_BYTE);
   (void) miicv_setstr(icvid, MI_ICV_SIGN, MI_UNSIGNED);

   /* Set range of values */
   (void) miicv_setint(icvid, MI_ICV_VALID_MIN, 0);
   (void) miicv_setint(icvid, MI_ICV_VALID_MAX, 255);

   /* Do normalization so that all pixels are on same scale */
   (void) miicv_setint(icvid, MI_ICV_DO_NORM, TRUE);

   /* Make sure that any out of range values are mapped to lowest value
      of type (for input only) */
   (void) miicv_setint(icvid, MI_ICV_DO_FILLVALUE, TRUE);

   /* We want to ensure that images have X, Y and Z dimensions in the
      positive direction, giving patient left on left and for drawing from
      bottom up. If we wanted patient right on left and drawing from
      top down, we would set to MI_ICV_NEGATIVE. */
   (void) miicv_setint(icvid, MI_ICV_DO_DIM_CONV, TRUE);
   (void) miicv_setint(icvid, MI_ICV_XDIM_DIR, MI_ICV_POSITIVE);
   (void) miicv_setint(icvid, MI_ICV_YDIM_DIR, MI_ICV_POSITIVE);
   (void) miicv_setint(icvid, MI_ICV_ZDIM_DIR, MI_ICV_POSITIVE);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_volume_data
@INPUT      : icvid - id of icv to set up
@OUTPUT     : volume - volume data
@RETURNS    : (nothing)
@DESCRIPTION: Routine to read in the volume data.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 26, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void read_volume_data(int icvid, Volume *volume)
{
   long start[MAX_VAR_DIMS], count[MAX_VAR_DIMS];

   /* Set up the start and count variables for reading the volume */
   (void) miset_coords(3, 0, start);
   count[0] = volume->nslices;
   count[1] = volume->nrows;
   count[2] = volume->ncolumns;

   /* Allocate space for the data */
   volume->data = MALLOC(sizeof(*volume->data) *
                         volume->ncolumns * volume->nrows *
                         volume->nslices);

   /* Read in the volume */
   (void) miicv_get(icvid, start, count, volume->data);

   /* Get the volume min and max */
   (void) miicv_inqdbl(icvid, MI_ICV_NORM_MIN, &volume->minimum);
   (void) miicv_inqdbl(icvid, MI_ICV_NORM_MAX, &volume->maximum);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : save_volume
@INPUT      : infile - input file name (NULL means don't copy info)
              outfile - output file name
              arg_string - string giving argument list
              volume - volume data
@OUTPUT     : (nothing)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to save a transverse volume, copying information
              from an optional input file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void save_volume(char *infile, char *outfile, char *arg_string, 
                        Volume *volume)
{
   int mincid, icvid, inmincid;

   /* Create output file */
   mincid = nccreate(outfile, NC_CLOBBER);

   /* Open the input file if it is provided */
   if (infile != NULL) {
      inmincid = ncopen(infile, NC_NOWRITE);
   }
   else {
      inmincid = MI_ERROR;
   }

   /* Set up variables and put output file in data mode */
   setup_variables(inmincid, mincid, volume, arg_string);

   /* Close the input file */
   if (inmincid != MI_ERROR) {
      (void) ncclose(inmincid);
   }

   /* Create an icv and set it up */
   icvid = miicv_create();
   setup_icv(icvid);

   /* Attach the icv to the file */
   (void) miicv_attach(icvid, mincid, ncvarid(mincid, MIimage));

   /* Write out the volume data */
   write_volume_data(icvid, volume);

   /* Close the file and free the icv */
   (void) ncclose(mincid);
   (void) miicv_free(icvid);

}
/* ----------------------------- MNI Header -----------------------------------
@NAME       : setup_variables
@INPUT      : inmincid - id of input minc file (MI_ERROR if no file)
              mincid - id of output minc file
              volume - volume information
              arg_string - string giving argument list
@OUTPUT     : (nothing)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to set up variables in the output minc file
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 26, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void setup_variables(int inmincid, int mincid, Volume *volume, 
                            char *arg_string)
{
   int dim[MAX_VAR_DIMS], ndims;
   int imgid, maxid, minid;
   int excluded_vars[10], nexcluded;
   nc_type datatype;
   int att_length;
   char *string;
   static double valid_range[2] = {0.0, 255.0};

   /* Create the dimensions */
   ndims = NUMBER_OF_DIMENSIONS;
   dim[0] = ncdimdef(mincid, volume->dimension_names[0], volume->nslices);
   dim[1] = ncdimdef(mincid, volume->dimension_names[1], volume->nrows);
   dim[2] = ncdimdef(mincid, volume->dimension_names[2], volume->ncolumns);

   /* If an input file is provided, copy all header info from that file except
      image, image-max, image-min */
   if (inmincid != MI_ERROR) {

      /* Look for the image variable and the image-max/min variables so that
         we can exclude them from the copy */
      nexcluded = 0;
      excluded_vars[nexcluded] = ncvarid(inmincid, MIimage);
      if (excluded_vars[nexcluded] != MI_ERROR) nexcluded++;
      excluded_vars[nexcluded] = ncvarid(inmincid, MIimagemax);
      if (excluded_vars[nexcluded] != MI_ERROR) nexcluded++;
      excluded_vars[nexcluded] = ncvarid(inmincid, MIimagemin);
      if (excluded_vars[nexcluded] != MI_ERROR) nexcluded++;

      /* Copy the variable definitions */
      (void) micopy_all_var_defs(inmincid, mincid, nexcluded, excluded_vars);

   }

   /* Create the image variable, copy attributes, set the signtype attribute,
      set the valid range attribute and delete valid max/min attributes */
   imgid = micreate_std_variable(mincid, MIimage, NC_BYTE, ndims, dim);
   if (inmincid != MI_ERROR) {
      (void) micopy_all_atts(inmincid, ncvarid(inmincid, MIimage),
                             mincid, imgid);
      ncopts = 0;
      (void) ncattdel(mincid, imgid, MIvalid_max);
      (void) ncattdel(mincid, imgid, MIvalid_min);
      ncopts = NC_OPTS_VAL;
   }
   (void) miattputstr(mincid, imgid, MIsigntype, MI_UNSIGNED);
   (void) ncattput(mincid, imgid, MIvalid_range, NC_DOUBLE, 2, valid_range);

   /* Create the image max and min variables */
   maxid = micreate_std_variable(mincid, MIimagemax, NC_DOUBLE, 0, NULL);
   minid = micreate_std_variable(mincid, MIimagemin, NC_DOUBLE, 0, NULL);
   if (inmincid != MI_ERROR) {
      (void) micopy_all_atts(inmincid, ncvarid(inmincid, MIimagemax),
                             mincid, maxid);
      (void) micopy_all_atts(inmincid, ncvarid(inmincid, MIimagemin),
                             mincid, minid);
   }

   /* Add the time stamp (get the history attribute length, allocate a string,
      get the old history, add the new command and put the new history). */
   ncopts=0;
   if ((ncattinq(mincid, NC_GLOBAL, MIhistory, &datatype,
                 &att_length) == MI_ERROR) ||
       (datatype != NC_CHAR))
      att_length = 0;
   att_length += strlen(arg_string) + 1;
   string = MALLOC(att_length);
   string[0] = '\0';
   (void) miattgetstr(mincid, NC_GLOBAL, MIhistory, att_length, 
                      string);
   ncopts = NC_OPTS_VAL;
   (void) strcat(string, arg_string);
   (void) miattputstr(mincid, NC_GLOBAL, MIhistory, string);
   FREE(string);

   /* Put the file in data mode */
   (void) ncendef(mincid);

   /* Copy over variable values */
   (void) micopy_all_var_values(inmincid, mincid,
                                nexcluded, excluded_vars);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : write_volume_data
@INPUT      : icvid - id of icv to set up
              volume - volume data
@OUTPUT     : (nothing)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to write out the volume data.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 26, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void write_volume_data(int icvid, Volume *volume)
{
   int mincid;
   long start[MAX_VAR_DIMS], count[MAX_VAR_DIMS];

   /* Get the minc file id */
   (void) miicv_inqint(icvid, MI_ICV_CDFID, &mincid);

   /* Set up the start and count variables for writinging the volume */
   (void) miset_coords(3, 0, start);
   count[0] = volume->nslices;
   count[1] = volume->nrows;
   count[2] = volume->ncolumns;

   /* Write out the volume min and max */
   (void) mivarput1(mincid, ncvarid(mincid, MIimagemin), NULL, NC_DOUBLE,
                    NULL, &volume->minimum);
   (void) mivarput1(mincid, ncvarid(mincid, MIimagemax), NULL, NC_DOUBLE,
                    NULL, &volume->maximum);

   /* Write out the volume */
   (void) miicv_put(icvid, start, count, volume->data);

   /* Free space for the data */
   FREE(volume->data);

}

