/* ----------------------------- MNI Header -----------------------------------
@NAME       : voxel_loop.c
@DESCRIPTION: Routines to loop through a file doing an operation on a single
              voxel.
@METHOD     : 
@GLOBALS    : 
@CREATED    : January 10, 1994 (Peter Neelin)
@MODIFIED   : $Log: voxel_loop.c,v $
@MODIFIED   : Revision 1.1  1994-01-11 15:08:30  neelin
@MODIFIED   : Initial revision
@MODIFIED   :
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
static char rcsid[]="$Header: /private-cvsroot/minc/progs/mincwindow/Attic/voxel_loop.c,v 1.1 1994-01-11 15:08:30 neelin Exp $";
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <minc.h>
#include <minc_def.h>
#include <voxel_loop.h>

#ifndef public
#  define public
#endif
#ifndef private
#  define private static
#endif

#ifndef TRUE
#  define TRUE 1
#  define FALSE 0
#endif

/* Default ncopts values for error handling */
#define NC_OPTS_VAL NC_VERBOSE | NC_FATAL

/* Function prototypes */
private void setup_variables(int inmincid, int outmincid,
                             char *arg_string,
                             int *ndims_ret, int *nimgdims_ret, 
                             long dimlength[]);
private void update_history(int mincid, char *arg_string);
private void do_voxel_loop(int inicvid, int outicvid, 
                           int ndims, int nimgdims, long dimlength[],
                           VoxelFunction voxel_function, void *voxel_data);

/* ----------------------------- MNI Header -----------------------------------
@NAME       : voxel_loop
@INPUT      : inmincid - input minc file id
              outmincid - output minc file id
              arg_string - string for history
              voxel_function - function to call with a group of voxels.
                 The first argument is a pointer to client data. The second
                 argument gives the number of voxels to handle on this call,
                 and the third argument is an array of values. Each value
                 should be replaced by its new value, with -DBL_MAX being
                 used to represent illegal, out-of-range values.
              voxel_data - data to pass to voxel_function
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to loop through the voxels of a file and call a function
              to operate on each voxel.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 10, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void voxel_loop(int inmincid, int outmincid, char *arg_string,
                       VoxelFunction voxel_function, void *voxel_data)
{
   int inicvid, outicvid;
   int ndims, nimgdims;
   long dimlength[MAX_VAR_DIMS];

   /* Set up variables in output file */
   setup_variables(inmincid, outmincid, arg_string, 
                   &ndims, &nimgdims, dimlength);

   /* Set up icv for input */
   inicvid = miicv_create();
   (void) miicv_setint(inicvid, MI_ICV_TYPE, NC_DOUBLE);
   (void) miicv_setint(inicvid, MI_ICV_DO_NORM, TRUE);
   (void) miicv_setint(inicvid, MI_ICV_USER_NORM, TRUE);
   (void) miicv_setint(inicvid, MI_ICV_DO_FILLVALUE, TRUE);
   (void) miicv_attach(inicvid, inmincid, ncvarid(inmincid, MIimage));

   /* Set up icv for output */
   outicvid = miicv_create();
   (void) miicv_setint(outicvid, MI_ICV_TYPE, NC_DOUBLE);
   (void) miicv_setint(outicvid, MI_ICV_DO_NORM, TRUE);
   (void) miicv_setint(outicvid, MI_ICV_USER_NORM, TRUE);
   (void) miicv_attach(outicvid, outmincid, ncvarid(outmincid, MIimage));

   /* Loop through the voxels */
   do_voxel_loop(inicvid, outicvid, ndims, nimgdims, dimlength,
                 voxel_function, voxel_data);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : setup_variables
@INPUT      : inmincid - input minc file id
              outmincid - output minc file id
              arg_string - string for history
@OUTPUT     : ndims_ret - number of dimensions for image variable
              nimgdims_ret - number of image dimensions (2 or 3)
              dimlength - array of image dimension lengths
@RETURNS    : (nothing)
@DESCRIPTION: Routine to setup the variables in the output file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 10, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void setup_variables(int inmincid, int outmincid,
                             char *arg_string,
                             int *ndims_ret, int *nimgdims_ret, 
                             long dimlength[])
{
   int inimgid, outimgid, maxid, minid;
   int indim[MAX_VAR_DIMS], outdim[MAX_VAR_DIMS];
   nc_type datatype;
   int idim, ndims, nimgdims;
   char dimname[MAX_NC_NAME];
   int nexcluded, excluded_vars[MAX_VAR_DIMS];

   /* Get image variable id for input file */
   inimgid = ncvarid(inmincid, MIimage);

   /* Set up the output minc file */

   /* Get the list of dimensions subscripting the image variable */
   (void) ncvarinq(inmincid, inimgid, NULL, &datatype, &ndims, indim, NULL);

   /* Check the number of image dimensions (2 or 3) */
   nimgdims = 2;
   (void) ncdiminq(inmincid, indim[ndims-1], dimname, NULL);
   if (strcmp(dimname, MIvector_dimension) == 0)
      nimgdims++;
   if (nimgdims > ndims) nimgdims = ndims;

   /* Loop, creating output dimensions */
   for (idim=0; idim < ndims; idim++) {
      (void) ncdiminq(inmincid, indim[idim], dimname, &dimlength[idim]);
      outdim[idim] = ncdimdef(outmincid, dimname, dimlength[idim]);
   }

   /* Copy other variables in file */
   nexcluded = 0;
   excluded_vars[nexcluded] = inimgid;
   if (excluded_vars[nexcluded] != MI_ERROR) nexcluded++;
   excluded_vars[nexcluded] = ncvarid(inmincid, MIimagemax);
   if (excluded_vars[nexcluded] != MI_ERROR) nexcluded++;
   excluded_vars[nexcluded] = ncvarid(inmincid, MIimagemin);
   if (excluded_vars[nexcluded] != MI_ERROR) nexcluded++;
   (void) micopy_all_var_defs(inmincid, outmincid, nexcluded, excluded_vars);

   /* Add the time stamp to the history */
   update_history(outmincid, arg_string);
 
  /* Create the image and image-min/max variables */
   outimgid = micreate_std_variable(outmincid, MIimage, datatype, 
                                    ndims, outdim);
   (void) micopy_all_atts(inmincid, inimgid, outmincid, outimgid);
   ncopts = 0;
   (void) ncattdel(outmincid, outimgid, MIvalid_max);
   (void) ncattdel(outmincid, outimgid, MIvalid_min);
   ncopts = NC_OPTS_VAL;
   maxid = micreate_std_variable(outmincid, MIimagemax, NC_DOUBLE, 
                                 ndims-nimgdims, outdim);
   minid = micreate_std_variable(outmincid, MIimagemin, NC_DOUBLE, 
                                 ndims-nimgdims, outdim);
   ncopts = 0;
   (void) micopy_all_atts(inmincid, ncvarid(inmincid, MIimagemax),
                          outmincid, maxid);
   (void) micopy_all_atts(inmincid, ncvarid(inmincid, MIimagemin),
                          outmincid, minid);
   ncopts = NC_OPTS_VAL;

   /* Put the file in data mode */
   (void) ncendef(outmincid);

   /* Copy over variable values */
   (void) micopy_all_var_values(inmincid, outmincid,
                                nexcluded, excluded_vars);

   /* Set return values of ndims and nimgdims */
   *ndims_ret = ndims;
   *nimgdims_ret = nimgdims;

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
private void update_history(int mincid, char *arg_string)
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
@NAME       : do_voxel_loop
@INPUT      : inicvid - input icv id
              outicvid - output icv id
              ndims - number of dimensions
              nimgdims - number of image dimensions
              dimlength - array of lengths of dimensions
              voxel_function - function to call with a group of voxels.
                 The first argument is a pointer to client data. The second
                 argument gives the number of voxels to handle on this call,
                 and the third argument is an array of values. Each value
                 should be replaced by its new value, with -DBL_MAX being
                 used to represent illegal, out-of-range values.
              voxel_data - data to pass to voxel_function
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to loop through the voxels and do something to each one
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 10, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void do_voxel_loop(int inicvid, int outicvid, 
                           int ndims, int nimgdims, long dimlength[],
                           VoxelFunction voxel_function, void *voxel_data)
{
   int outmincid, imgid, maxid, minid;
   long start[MAX_VAR_DIMS], count[MAX_VAR_DIMS], end[MAX_VAR_DIMS];
   long mm_start[MAX_VAR_DIMS];
   int idim;
   double *data, minimum, maximum;
   long nvoxels, ivox;

   /* Get output file id and max/min variable ids */
   (void) miicv_inqint(outicvid, MI_ICV_CDFID, &outmincid);
   (void) miicv_inqint(outicvid, MI_ICV_VARID, &imgid);
   maxid = ncvarid(outmincid, MIimagemax);
   minid = ncvarid(outmincid, MIimagemin);

   /* Set up variables for looping through voxels */
   for (idim=0; idim < ndims; idim++) {
      start[idim] = 0;
      end[idim] = dimlength[idim];
      if (idim < ndims-nimgdims)
         count[idim] = 1;
      else
         count[idim] = dimlength[idim];
   }

   /* Allocate space for the data */
   nvoxels = 1;
   for (idim = ndims-nimgdims; idim < ndims; idim++)
      nvoxels *= dimlength[idim];
   data = MALLOC(sizeof(*data) * nvoxels);

   /* Loop through voxels */

   while (start[0] < end[0]) {

      /* Get the slice */
      (void) miicv_get(inicvid, start, count, data);

      /* Do something with it */
      voxel_function(voxel_data, nvoxels, data);

      /* Find the max and min */
      minimum = DBL_MAX;
      maximum = -DBL_MAX;
      for (ivox=0; ivox < nvoxels; ivox++) {
         if (data[ivox] != -DBL_MAX) {
            if (data[ivox] < minimum) minimum = data[ivox];
            if (data[ivox] > maximum) maximum = data[ivox];
         }
      }
      if ((minimum == DBL_MAX) && (maximum == -DBL_MAX)) {
         minimum = 0.0;
         maximum = 0.0;
      }

      /* Write out the max and min */
      (void) mitranslate_coords(outmincid, imgid, start, maxid, mm_start);
      (void) mivarput1(outmincid, maxid, mm_start, NC_DOUBLE, NULL, &maximum);
      (void) mivarput1(outmincid, minid, mm_start, NC_DOUBLE, NULL, &minimum);

      /* Write out the values */
      (void) miicv_put(outicvid, start, count, data);

      /* Update the counter */
      idim = ndims-1;
      start[idim] += count[idim];
      while ( (idim>0) && (start[idim] >= end[idim])) {
         start[idim] = 0;
         idim--;
         start[idim] += count[idim];
      }

   }     /* End of loop through chunks */

   /* Free buffer */
   FREE(data);

   return;

}

