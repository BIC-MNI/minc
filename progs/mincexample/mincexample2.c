/* ----------------------------- MNI Header -----------------------------------
@NAME       : mincexample2
@INPUT      : argc, argv - command line arguments
@OUTPUT     : (none)
@RETURNS    : status
@DESCRIPTION: Example program copy a minc file one slice at a time
              (where mincexample loads in the whole volume and then
              writes it out).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : March 16, 1994 (Peter Neelin)
@MODIFIED   : $Log: mincexample2.c,v $
@MODIFIED   : Revision 1.2  1994-03-17 08:36:29  neelin
@MODIFIED   : Added support for 2-d files.
@MODIFIED   :
 * Revision 1.1  94/03/16  12:00:59  neelin
 * Initial revision
 * 
---------------------------------------------------------------------------- */

#ifndef lint
static char rcsid[]="$Header: /private-cvsroot/minc/progs/mincexample/mincexample2.c,v 1.2 1994-03-17 08:36:29 neelin Exp $";
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <minc.h>
#include <time_stamp.h>

/* Include the standard minc definitions for cross-platform compilation
   if we are compiling the package, otherwise, just define MALLOC,
   FREE, EXIT_SUCCESS and EXIT_FAILURE */
#ifdef COMPILING_MINC_PACKAGE
#  include <minc_def.h>
#else
#  define MALLOC(size) ((void *) malloc(size))
#  define FREE(ptr) free(ptr)
#  ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#    define EXIT_FAILURE 1
#  endif
#endif

/* Constants */
#define public
#ifndef TRUE
#  define TRUE 1
#  define FALSE 0
#endif

/* Definitions for accessing information on each dimension */
#define NUMBER_OF_DIMENSIONS 3      /* Number of dimensions of volume */
#define SLICE 0                     /* Index for slice information */
#define ROW 1                       /* Index for row information */
#define COLUMN 2                    /* Index for column information */

/* Default ncopts values for error handling */
#define NC_OPTS_VAL NC_VERBOSE | NC_FATAL

/* Types */
typedef struct {
   long nslices;             /* Number of slices */
   long nrows;               /* Number of rows in image */
   long ncolumns;            /* Number of columns in image */
   double maximum;           /* Volume maximum */
   double minimum;           /* Volume minimum */
   double step[NUMBER_OF_DIMENSIONS];     /* Step sizes for dimensions */
   double start[NUMBER_OF_DIMENSIONS];    /* Start positions for dimensions */
   char dimension_names[NUMBER_OF_DIMENSIONS][MAX_NC_NAME];
                             /* Dimension names */
} Volume_Info;

/* Function prototypes */
public int get_volume_info(char *infile, Volume_Info *volume_info);
public void setup_input_icv(int icvid);
public void get_dimension_info(char *infile, int icvid, 
                               Volume_Info *volume_info);
public void close_volume(int icvid);
public void get_volume_slice(int icvid, Volume_Info *volume_info, 
                             int slice_num, unsigned char *image);
public int save_volume_info(int input_icvid, char *outfile, char *arg_string, 
                            Volume_Info *volume_info);
public void setup_output_icv(int icvid);
public void setup_variables(int inmincid, int mincid, 
                            Volume_Info *volume_info, 
                            char *arg_string);
public void setup_image_variables(int inmincid, int mincid, 
                                  int ndims, int dim[]);
public void update_history(int mincid, char *arg_string);
public void save_volume_slice(int icvid, Volume_Info *volume_info, 
                              int slice_num, unsigned char *image,
                              double slice_min, double slice_max);

/* Main program */

public int main(int argc, char *argv[])
{
   char *infile, *outfile;
   Volume_Info volume_info;
   char *arg_string;
   int input_icvid, output_icvid;
   unsigned char *image;
   int islice;

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

   /* Read in input volume information */
   input_icvid = get_volume_info(infile, &volume_info);

   /* Print out volume information */
   (void) printf("File %s:\n", infile);
   (void) printf("  maximum = %10g, minimum = %10g\n",
                 volume_info.maximum, volume_info.minimum);
   (void) printf("  slices  = %10s: n=%3d, step=%10g, start=%10g\n",
                 volume_info.dimension_names[SLICE], 
                 (int) volume_info.nslices,
                 volume_info.step[SLICE], volume_info.start[SLICE]);
   (void) printf("  rows    = %10s: n=%3d, step=%10g, start=%10g\n",
                 volume_info.dimension_names[ROW], 
                 (int) volume_info.nrows,
                 volume_info.step[ROW], volume_info.start[ROW]);
   (void) printf("  columns = %10s: n=%3d, step=%10g, start=%10g\n",
                 volume_info.dimension_names[COLUMN], 
                 (int) volume_info.ncolumns,
                 volume_info.step[COLUMN], volume_info.start[COLUMN]);

   /* Save the volume, copying information from input file */
   output_icvid = save_volume_info(input_icvid, outfile, arg_string, 
                                   &volume_info);

   /* Loop through slices, copying them */
   image = MALLOC(volume_info.nrows * volume_info.ncolumns * sizeof(*image));
   for (islice=0; islice < volume_info.nslices; islice++) {
      get_volume_slice(input_icvid, &volume_info, islice, image);
      save_volume_slice(output_icvid, &volume_info, islice, image,
                        volume_info.minimum, volume_info.maximum);
   }

   /* Free up image and close files */
   close_volume(input_icvid);
   close_volume(output_icvid);
   FREE(image);

   exit(EXIT_SUCCESS);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_volume_info
@INPUT      : infile - input file name
@OUTPUT     : volume_info - input volume information
@RETURNS    : Id of icv created
@DESCRIPTION: Routine to read volume information for a file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int get_volume_info(char *infile, Volume_Info *volume_info)
{
   int icvid, mincid;

   /* Create and set up icv for input */
   icvid = miicv_create();
   setup_input_icv(icvid);

   /* Open the image file */
   mincid = miopen(infile, NC_NOWRITE);

   /* Attach the icv to the file */
   (void) miicv_attach(icvid, mincid, ncvarid(mincid, MIimage));

   /* Get dimension information */
   get_dimension_info(infile, icvid, volume_info);

   /* Get the volume min and max */
   (void) miicv_inqdbl(icvid, MI_ICV_NORM_MIN, &volume_info->minimum);
   (void) miicv_inqdbl(icvid, MI_ICV_NORM_MAX, &volume_info->maximum);

   return icvid;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : setup_input_icv
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
public void setup_input_icv(int icvid)
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
   (void) miicv_setint(icvid, MI_ICV_DO_SCALAR, TRUE);
   (void) miicv_setint(icvid, MI_ICV_XDIM_DIR, MI_ICV_POSITIVE);
   (void) miicv_setint(icvid, MI_ICV_YDIM_DIR, MI_ICV_POSITIVE);
   (void) miicv_setint(icvid, MI_ICV_ZDIM_DIR, MI_ICV_POSITIVE);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_dimension_info
@INPUT      : infile - name of input file
              icvid - id of the image conversion variable
@OUTPUT     : volume - input volume data
@RETURNS    : (nothing)
@DESCRIPTION: Routine to get dimension information from an input file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 26, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void get_dimension_info(char *infile, int icvid, 
                               Volume_Info *volume_info)
{
   int mincid, imgid, varid;
   int idim, ndims;
   int dim[MAX_VAR_DIMS];
   long *dimlength;
   char *dimname;
   int offset;
   int missing_one_dimension;

   /* Get the minc file id and the image variable id */
   (void) miicv_inqint(icvid, MI_ICV_CDFID, &mincid);
   (void) miicv_inqint(icvid, MI_ICV_VARID, &imgid);
   if ((mincid == MI_ERROR) || (imgid == MI_ERROR)) {
      (void) fprintf(stderr, "File %s is not attached to an icv!\n",
                     infile);
      exit(EXIT_FAILURE);
   }

   /* Get the list of dimensions subscripting the image variable */
   (void) ncvarinq(mincid, imgid, NULL, NULL, &ndims, dim, NULL);
   (void) miicv_inqint(icvid, MI_ICV_NUM_DIMS, &ndims);

   /* Check that we have two or three dimensions */
   if ((ndims != NUMBER_OF_DIMENSIONS) &&
       (ndims != NUMBER_OF_DIMENSIONS-1)) {
      (void) fprintf(stderr, 
                     "File %s does not have %d or %d dimensions\n",
                     infile, NUMBER_OF_DIMENSIONS,
                     NUMBER_OF_DIMENSIONS-1);
      exit(EXIT_FAILURE);
   }

   /* Pretend that we have three dimensions */
   offset = ndims - NUMBER_OF_DIMENSIONS;
   missing_one_dimension = (offset < 0);
   ndims = NUMBER_OF_DIMENSIONS;

   /* Loop through dimensions, checking them and getting their sizes */
   for (idim=0; idim<ndims; idim++) {

      /* Get pointers to the appropriate dimension size and name */
      switch (idim) {
      case 0: dimlength = &(volume_info->nslices) ; break;
      case 1: dimlength = &(volume_info->nrows) ; break;
      case 2: dimlength = &(volume_info->ncolumns) ; break;
      }
      dimname = volume_info->dimension_names[idim];

      /* Get dimension name and size */
      if (missing_one_dimension && (idim==0)) {
         (void) strcpy(dimname, "unknown");
         *dimlength = 1;
      }
      else {
         (void) ncdiminq(mincid, dim[idim+offset], dimname, dimlength);
      }

   }

   /* Get dimension step and start (defaults = 1 and 0). For slices,
      we read straight from the variable, but for image dimensions, we
      get the step and start from the icv. If we didn't have 
      MI_ICV_DO_DIM_CONV set to TRUE, then we would have to get the image 
      dimension step and start straight from the variables. */
   for (idim=0; idim<ndims; idim++) {
      volume_info->step[idim] = 1.0;
      volume_info->start[idim] = 0.0;
   }
   ncopts = 0;
   (void) miicv_inqdbl(icvid, MI_ICV_ADIM_STEP, &volume_info->step[COLUMN]);
   (void) miicv_inqdbl(icvid, MI_ICV_ADIM_START, &volume_info->start[COLUMN]);
   (void) miicv_inqdbl(icvid, MI_ICV_BDIM_STEP, &volume_info->step[ROW]);
   (void) miicv_inqdbl(icvid, MI_ICV_BDIM_START, &volume_info->start[ROW]);
   if ((varid=ncvarid(mincid, volume_info->dimension_names[SLICE])) 
                                != MI_ERROR) {
      (void) miattget1(mincid, varid, MIstep, NC_DOUBLE, 
                       &volume_info->step[SLICE]);
      (void) miattget1(mincid, varid, MIstart, NC_DOUBLE,
                       &volume_info->start[SLICE]);
   }
   ncopts = NC_OPTS_VAL;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : close_volume
@INPUT      : icvid - id of open icv
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to close a minc file and free the associated icv
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : March 16, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void close_volume(int icvid)
{
   int mincid;

   /* Get the minc file id and close the file */
   ncopts = 0;
   if (miicv_inqint(icvid, MI_ICV_CDFID, &mincid) != MI_ERROR) {
      (void) miclose(mincid);
   }
   ncopts = NC_OPTS_VAL;

   /* Free the icv */
   (void) miicv_free(icvid);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_volume_slice
@INPUT      : icvid - id of icv to read
              volume_info - info for volume
              slice_num - number of slice to read in (counting from zero)
@OUTPUT     : image - image that is read in
@RETURNS    : (nothing)
@DESCRIPTION: Routine to read in an image.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 26, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void get_volume_slice(int icvid, Volume_Info *volume_info, 
                             int slice_num, unsigned char *image)
{
   long start[MAX_VAR_DIMS], count[MAX_VAR_DIMS];
   int offset, ndims;

   /* Get number of dimensions */
   (void) miicv_inqint(icvid, MI_ICV_NUM_DIMS, &ndims);
   offset = ndims - NUMBER_OF_DIMENSIONS;

   /* Check slice_num */
   if (slice_num >= volume_info->nslices) {
      (void) fprintf(stderr, "Slice %d is not in the file.\n",
                     slice_num);
      exit(EXIT_FAILURE);
   }

   /* Set up the start and count variables for reading the volume */
   (void) miset_coords(3, 0, start);
   if (offset >= 0) {
      start[offset] = slice_num;
      count[offset] = 1;
   }
   count[1+offset] = volume_info->nrows;
   count[2+offset] = volume_info->ncolumns;

   /* Read in the volume */
   (void) miicv_get(icvid, start, count, image);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : save_volume_info
@INPUT      : input_icvid - input file icvid (MI_ERROR means no input volume)
              outfile - output file name
              arg_string - string giving argument list
              volume_info - volume information
@OUTPUT     : (nothing)
@RETURNS    : icv of output file
@DESCRIPTION: Routine to save a 3-D volume, copying information
              from an optional input file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int save_volume_info(int input_icvid, char *outfile, char *arg_string, 
                            Volume_Info *volume_info)
{
   int mincid, icvid, inmincid;

   /* Create output file */
   mincid = micreate(outfile, NC_NOCLOBBER);

   /* Open the input file if it is provided */
   inmincid = MI_ERROR;
   if (input_icvid != MI_ERROR) {
      (void) miicv_inqint(input_icvid, MI_ICV_CDFID, &inmincid);
   }

   /* Set up variables and put output file in data mode */
   setup_variables(inmincid, mincid, volume_info, arg_string);

   /* Create an icv and set it up */
   icvid = miicv_create();
   setup_output_icv(icvid);

   /* Attach the icv to the file */
   (void) miicv_attach(icvid, mincid, ncvarid(mincid, MIimage));

   return icvid;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : setup_output_icv
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
public void setup_output_icv(int icvid)
{
   /* Set desired type */
   (void) miicv_setint(icvid, MI_ICV_TYPE, NC_BYTE);
   (void) miicv_setstr(icvid, MI_ICV_SIGN, MI_UNSIGNED);

   /* Set range of values */
   (void) miicv_setint(icvid, MI_ICV_VALID_MIN, 0);
   (void) miicv_setint(icvid, MI_ICV_VALID_MAX, 255);

   /* No normalization so that pixels are scaled to the slice */
   (void) miicv_setint(icvid, MI_ICV_DO_NORM, FALSE);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : setup_variables
@INPUT      : inmincid - id of input minc file (MI_ERROR if no file)
              mincid - id of output minc file
              volume_info - volume information
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
public void setup_variables(int inmincid, int mincid, 
                            Volume_Info *volume_info, 
                            char *arg_string)
{
   int dim[MAX_VAR_DIMS], ndims, idim, varid;
   int excluded_vars[10], nexcluded;

   /* Create the dimensions */
   ndims = NUMBER_OF_DIMENSIONS;
   dim[0] = ncdimdef(mincid, volume_info->dimension_names[0], 
                     volume_info->nslices);
   dim[1] = ncdimdef(mincid, volume_info->dimension_names[1], 
                     volume_info->nrows);
   dim[2] = ncdimdef(mincid, volume_info->dimension_names[2], 
                     volume_info->ncolumns);

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

   /* Set up the dimension variables. If the variable doesn't exist, create
      it (either no input file or variable did not exist in it). If the
      dimensions are not standard, then no variable is created. */

   for (idim=0; idim < ndims; idim++) {
      ncopts = 0;
      varid = ncvarid(mincid, volume_info->dimension_names[idim]);
      if (varid == MI_ERROR) {
         varid = micreate_std_variable(mincid, 
                                       volume_info->dimension_names[idim],
                                       NC_LONG, 0, NULL);
      }
      ncopts = NC_OPTS_VAL;
      if (varid != MI_ERROR) {
         (void) miattputdbl(mincid, varid, MIstep, 
                            volume_info->step[idim]);
         (void) miattputdbl(mincid, varid, MIstart, 
                            volume_info->start[idim]);
      }
   }
   
   /* Create the image, image-max and image-min variables */
   setup_image_variables(inmincid, mincid, ndims, dim);

   /* Add the time stamp to the history */
   update_history(mincid, arg_string);

   /* Put the file in data mode */
   (void) ncendef(mincid);

   /* Copy over variable values */
   if (inmincid != MI_ERROR) {
      (void) micopy_all_var_values(inmincid, mincid,
                                   nexcluded, excluded_vars);
   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : setup_image_variables
@INPUT      : inmincid - id of input minc file (MI_ERROR if no file)
              mincid - id of output minc file
              ndims - number of dimensions
              dim - list of dimension ids
@OUTPUT     : (nothing)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to set up image, image-max and image-min variables 
              in the output minc file
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 26, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void setup_image_variables(int inmincid, int mincid, 
                                  int ndims, int dim[])
{
   int imgid, maxid, minid;
   static double valid_range[2] = {0.0, 255.0};

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

   /* Create the image max and min variables (varying over slices) */
   maxid = micreate_std_variable(mincid, MIimagemax, NC_DOUBLE, 1, dim);
   minid = micreate_std_variable(mincid, MIimagemin, NC_DOUBLE, 1, dim);
   if (inmincid != MI_ERROR) {
      (void) micopy_all_atts(inmincid, ncvarid(inmincid, MIimagemax),
                             mincid, maxid);
      (void) micopy_all_atts(inmincid, ncvarid(inmincid, MIimagemin),
                             mincid, minid);
   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : update_history
@INPUT      : mincid - id of output minc file
              arg_string - string giving list of arguments
@OUTPUT     : (nothing)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to update the history global variable in the output 
              minc file
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 26, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void update_history(int mincid, char *arg_string)
{
   nc_type datatype;
   int att_length;
   char *string;

   /* Get the history attribute length */
   ncopts=0;
   if ((ncattinq(mincid, NC_GLOBAL, MIhistory, &datatype,
                 &att_length) == MI_ERROR) ||
       (datatype != NC_CHAR))
      att_length = 0;
   att_length += strlen(arg_string) + 1;

   /* Allocate a string and get the old history */
   string = MALLOC(att_length);
   string[0] = '\0';
   (void) miattgetstr(mincid, NC_GLOBAL, MIhistory, att_length, 
                      string);
   ncopts = NC_OPTS_VAL;

   /* Add the new command and put the new history. */
   (void) strcat(string, arg_string);
   (void) miattputstr(mincid, NC_GLOBAL, MIhistory, string);
   FREE(string);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : save_volume_slice
@INPUT      : icvid - id of icv to write
              volume_info - volume information (minimum and maximum are
                 ignored)
              slice_num - number of slice to write
              image - image to write
              slice_min - minimum real value for slice
              slice_max - maximum real value for slice
@OUTPUT     : (nothing)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to write out a slice.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 26, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void save_volume_slice(int icvid, Volume_Info *volume_info, 
                              int slice_num, unsigned char *image,
                              double slice_min, double slice_max)
{
   int mincid;
   long start[MAX_VAR_DIMS], count[MAX_VAR_DIMS];

   /* Get the minc file id */
   (void) miicv_inqint(icvid, MI_ICV_CDFID, &mincid);

   /* Set up the start and count variables for writinging the volume */
   (void) miset_coords(3, 0, start);
   start[0] = slice_num;
   count[0] = 1;
   count[1] = volume_info->nrows;
   count[2] = volume_info->ncolumns;

   /* Write out the slice min and max */
   (void) mivarput1(mincid, ncvarid(mincid, MIimagemin), start, NC_DOUBLE,
                    NULL, &slice_min);
   (void) mivarput1(mincid, ncvarid(mincid, MIimagemax), start, NC_DOUBLE,
                    NULL, &slice_max);

   /* Write out the volume */
   (void) miicv_put(icvid, start, count, image);

}

