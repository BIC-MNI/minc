/* ----------------------------- MNI Header -----------------------------------
@NAME       : resample_volumes.c
@DESCRIPTION: File containing routines to resample minc volumes.
@METHOD     : 
@GLOBALS    : 
@CREATED    : February 8, 1993 (Peter Neelin)
@MODIFIED   : 
 * $Log: resample_volumes.c,v $
 * Revision 6.1  1999-10-19 14:45:28  neelin
 * Fixed Log subsitutions for CVS
 *
 * Revision 6.0  1997/09/12 13:23:21  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:24:22  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  19:59:42  neelin
 * Release of minc version 0.4
 *
 * Revision 3.0  1995/05/15  19:30:57  neelin
 * Release of minc version 0.3
 *
 * Revision 2.0  1994/09/28  10:32:50  neelin
 * Release of minc version 0.2
 *
 * Revision 1.15  94/09/28  10:32:38  neelin
 * Pre-release
 * 
 * Revision 1.14  94/03/15  10:58:47  neelin
 * Fixed tricubic interpolation (wasn't initializing variable for fillvalue
 * detection).
 * 
 * Revision 1.13  94/03/01  14:33:48  neelin
 * Fixed error in calculating valid minimum for float volumes.
 * 
 * Revision 1.12  93/11/02  11:23:52  neelin
 * Handle imagemax/min potentially varying over slices (for vector data, etc.)
 * 
 * Revision 1.11  93/10/20  14:06:36  neelin
 * Modified tri-linear interpolation to allow volumes to extend epsilon
 * beyond the first voxel.
 * Added code to handle volume dimensions of size one (in tri-linear
 * interpolation).
 * 
 * Revision 1.10  93/10/15  13:48:22  neelin
 * Removed include of recipes.h
 * 
 * Revision 1.9  93/10/12  12:48:08  neelin
 * Use volume_io.h instead of def_mni.h
 * 
 * Revision 1.8  93/08/11  14:29:53  neelin
 * Use volume->datatype in load_volume instead of file->datatype.
 * Loop for slice max/min is from 0, not 1 in load_volume.
 * 
 * Revision 1.7  93/08/11  13:34:15  neelin
 * Converted to use Dave MacDonald's General_transform code.
 * Fixed bug in get_slice - for non-linear transformations coord was
 * transformed, then used again as a starting coordinate.
 * Handle files that have image-max/min that doesn't vary over slices.
 * Handle files that have image-max/min varying over row/cols.
 * Allow volume to extend to voxel edge for -nearest_neighbour interpolation.
 * Handle out-of-range values (-fill values from a previous mincresample, for
 * example).
 * Save transformation file as a string attribute to processing variable.
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
static char rcsid[]="$Header: /private-cvsroot/minc/progs/mincresample/resample_volumes.c,v 6.1 1999-10-19 14:45:28 neelin Exp $";
#endif

#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <string.h>
#include <math.h>
#include <minc.h>
#include <volume_io.h>
#include <minc_def.h>
#include "mincresample.h"

/* ----------------------------- MNI Header -----------------------------------
@NAME       : resample_volumes
@INPUT      : program_flags - data for program execution
              in_vol - description of input volume
              out_vol - description of output volume
              transformation - description of world transformation
@OUTPUT     : (none)
@RETURNS    : (none)
@DESCRIPTION: Resamples in_vol into file specified by out_vol using given 
              world transformation.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 8, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void resample_volumes(Program_Flags *program_flags,
                             VVolume *in_vol, VVolume *out_vol, 
                             General_transform *transformation)
{
   long in_start[MAX_VAR_DIMS], in_count[MAX_VAR_DIMS], in_end[MAX_VAR_DIMS];
   long out_start[MAX_VAR_DIMS], out_count[MAX_VAR_DIMS];
   long mm_start[MAX_VAR_DIMS];   /* Vector for min/max variables */
   long nslice, islice, slice_count;
   int idim, index, slice_index;
   double maximum, minimum, valid_range[2];
   double *slice_max, *slice_min;
   File_Info *ifp,*ofp;

   /* Set pointers to file information */
   ifp = in_vol->file;
   ofp = out_vol->file;

   /* Allocate slice min/max arrays if needed */
   if (ofp->do_slice_renormalization) {
      slice_min = MALLOC(ofp->images_per_file * ofp->slices_per_image *
                         sizeof(double));
      slice_max = MALLOC(ofp->images_per_file * ofp->slices_per_image *
                         sizeof(double));
   }

   /* Set input file start, count and end vectors for reading a volume
      at a time */
   (void) miset_coords(ifp->ndims, (long) 0, in_start);
   (void) miset_coords(ifp->ndims, (long) 1, in_count);
   for (idim=0; idim < ifp->ndims; idim++) {
      in_end[idim] = ifp->nelements[idim];
   }
   for (idim=0; idim < VOL_NDIMS; idim++) {
      index = ifp->indices[idim];
      in_count[index] = ifp->nelements[index];
   }

   /* Set output file count for writing a slice and get the number of 
      output slices */
   (void) miset_coords(ifp->ndims, (long) 1, out_count);
   for (idim=0; idim < VOL_NDIMS; idim++) {
      index = ofp->indices[idim];
      if (idim==0) {
         slice_index = index;
         nslice = ofp->nelements[index];
      }
      else {
         out_count[index] = ofp->nelements[index];
      }
   }

   /* Initialize global max and min */
   valid_range[0] =  DBL_MAX;
   valid_range[1] = -DBL_MAX;

   /* Initialize file max/min slice count */
   slice_count = 0;

   /* Print log message */
   if (program_flags->verbose) {
      (void) fprintf(stderr, "Transforming slices:");
      (void) fflush(stderr);
   }

   /* Loop over input volumes */

   while (in_start[0] < in_end[0]) {

      /* Copy the start vector */
      for (idim=0; idim < ifp->ndims; idim++)
         out_start[idim] = in_start[idim];

      /* Read in the volume */
      load_volume(ifp, in_start, in_count, in_vol->volume);

      /* Loop over slices */
      for (islice=0; islice < nslice; islice++) {

         /* Print log message */
         if (program_flags->verbose) {
            (void) fprintf(stderr, ".");
            (void) fflush(stderr);
         }

         /* Set slice number in out_start */
         out_start[slice_index] = islice;

         /* Get the slice */
         get_slice(islice, in_vol, out_vol, transformation, 
                   &minimum, &maximum);

         /* Update global max and min */
         if (maximum > valid_range[1]) valid_range[1] = maximum;
         if (minimum < valid_range[0]) valid_range[0] = minimum;

         /* Write the max, min and slice */
         (void) mivarput1(ofp->mincid, ofp->maxid, 
                          mitranslate_coords(ofp->mincid, 
                                             ofp->imgid, out_start,
                                             ofp->maxid, mm_start),
                          NC_DOUBLE, NULL, &maximum);
         (void) mivarput1(ofp->mincid, ofp->minid, 
                          mitranslate_coords(ofp->mincid, 
                                             ofp->imgid, out_start,
                                             ofp->minid, mm_start),
                          NC_DOUBLE, NULL, &minimum);
         (void) miicv_put(ofp->icvid, out_start, out_count,
                          out_vol->slice->data);

         /* Save the max, min if needed */
         if (ofp->do_slice_renormalization) {
            slice_max[slice_count] = maximum;
            slice_min[slice_count] = minimum;
         }

         /* Increment slice count */
         slice_count++;

      }    /* End loop over slices */

      /* Increment in_start counter */
      idim = ofp->ndims-1;
      in_start[idim] += in_count[idim];
      while ( (idim>0) && (in_start[idim] >= in_end[idim])) {
         in_start[idim] = 0;
         idim--;
         in_start[idim] += in_count[idim];
      }

   }       /* End loop over volumes */

   /* Print end of log message */
   if (program_flags->verbose) {
      (void) fprintf(stderr, "Done\n");
      (void) fflush(stderr);
   }

   /* If output volume is floating point, write out global max and min */
   if ((ofp->datatype == NC_FLOAT) || (ofp->datatype == NC_DOUBLE)) {
      (void) ncattput(ofp->mincid, ofp->imgid, MIvalid_range, 
                      NC_DOUBLE, 2, valid_range);
   }

   /* Recompute slices and free vectors, if needed */
   if (ofp->do_slice_renormalization) {
      renormalize_slices(program_flags, out_vol, slice_min, slice_max);
      FREE(slice_min);
      FREE(slice_max);
   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : load_volume
@INPUT      : file - description of input file
              start - index of start of volume in minc file
              count - vector size of volume in minc file
              volume - description of volume data
@OUTPUT     : volume - contains new volume data and scales and offsets
@RETURNS    : (none)
@DESCRIPTION: Loads a volume from a minc file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void load_volume(File_Info *file, long start[], long count[], 
                        Volume_Data *volume)
{
   long nread, islice, mm_start[MAX_VAR_DIMS], mm_count[MAX_VAR_DIMS];
   int varid, ivar, idim, ndims;
   double *values, maximum, minimum, denom;

   /* Load the file */
   if (file->using_icv) {
      (void) miicv_get(file->icvid, start, count, volume->data);
   }
   else {
      (void) ncvarget(file->mincid, file->imgid, 
                      start, count, volume->data);
   }

   /* Read the max and min from the file into the scale and offset variables 
      (maxima into scale and minima into offset) if datatype is not
      floating point */

   /* Don't do it ourselves if the icv is doing it for us */
   if (file->using_icv) {
      for (islice=0; islice < volume->size[SLC_AXIS]; islice++) {
         (void) miicv_inqdbl(file->icvid, MI_ICV_NORM_MAX, 
                             &volume->scale[islice]);
         (void) miicv_inqdbl(file->icvid, MI_ICV_NORM_MIN, 
                             &volume->offset[islice]);
      }
   }

   /* If either max/min variable doesn't exist, or if type is floating
      point, then set max to 1 and min to 0 */
   else if ((file->maxid == MI_ERROR) || (file->minid == MI_ERROR) ||
            (volume->datatype == NC_FLOAT) || 
            (volume->datatype == NC_DOUBLE)) {
      for (islice=0; islice < volume->size[SLC_AXIS]; islice++) {
         volume->scale[islice] = DEFAULT_MAX;
         volume->offset[islice] = DEFAULT_MIN;
      }
   }

   /* Otherwise the imagemax/min variables exist - use them for integer
      types */
   else {

      for (ivar=0; ivar<2; ivar++) {

         /* Set up variables */
         varid  = (ivar == 0 ? file->maxid   : file->minid);
         values = (ivar == 0 ? volume->scale : volume->offset);

         /* Read max or min */
         (void) mivarget(file->mincid, varid, 
                         mitranslate_coords(file->mincid, file->imgid, start,
                                            varid,
                               miset_coords(MAX_VAR_DIMS, 0L, mm_start)),
                         mitranslate_coords(file->mincid, file->imgid, count,
                                            varid,
                               miset_coords(MAX_VAR_DIMS, 1L, mm_count)),
                      NC_DOUBLE, NULL, values);

         /* Check for number of values read */
         (void) ncvarinq(file->mincid, varid, NULL, NULL, &ndims, NULL, NULL);
         for (nread=1, idim=0; idim<ndims; idim++) nread *= mm_count[idim];

         /* If only one value read, then copy it for each slice */
         if (nread==1) {
            for (islice=1; islice < volume->size[SLC_AXIS]; islice++) {
               values[islice] = values[0];
            }
         }
         else if (nread != volume->size[SLC_AXIS]) {
            (void) fprintf(stderr, 
                           "Program bug while reading image max/min\n");
            exit(EXIT_FAILURE);
         }

      }        /* Loop over max/min variables */

   }        /* If max/min variables both exist */


   /* Calculate the scale and offset */
   for (islice=0; islice < volume->size[SLC_AXIS]; islice++) {

      /* If the variables type is floating point, then don't scale */
      if ((volume->datatype==NC_FLOAT) || (volume->datatype==NC_DOUBLE)) {
         volume->scale[islice] = 1.0;
         volume->offset[islice] = 0.0;
      }

      /* Otherwise, calculate scale and offset */
      else {
         maximum = volume->scale[islice];
         minimum = volume->offset[islice];
         denom = file->vrange[1] - file->vrange[0];
         if (denom != 0.0) {
            volume->scale[islice] = (maximum - minimum) / denom;
         }
         else {
            volume->scale[islice] = 0.0;
         }
         volume->offset[islice] = minimum - 
            file->vrange[0] * volume->scale[islice];
      }

   }        /* End of loop through slices */
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_slice
@INPUT      : in_vol - description of input volume
              out_vol - description of output volume
              transformation - description of world transformation
@OUTPUT     : out_vol - slice field contains new slice
              minimum - slice minimum (excluding data from outside volume)
              maximum - slice maximum (excluding data from outside volume)
@RETURNS    : (none)
@DESCRIPTION: Resamples current volume of in_vol into slice in out_vol 
              using given world transformation.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 8, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void get_slice(long slice_num, VVolume *in_vol, VVolume *out_vol, 
                      General_transform *transformation, 
                      double *minimum, double *maximum)
{
   Slice_Data *slice;
   Volume_Data *volume;
   double *dptr;
   long irow, icol;
   int all_linear;
   int idim;

   /* Coordinate vectors for stepping through slice */
   Coord_Vector zero = {0, 0, 0};
   Coord_Vector column = {0, 0, 1};
   Coord_Vector row = {0, 1, 0};
   Coord_Vector start = {0, 0, 0};    /* start[SLICE] set later to slice_num */
   Coord_Vector coord, transf_coord;

   /* Transformation stuff */
   General_transform total_transf, temp_transf;

   /* Get slice and volume pointers */
   volume = in_vol->volume;
   slice = out_vol->slice;

   /* Concatenate transforms */
   concat_general_transforms(out_vol->voxel_to_world, 
                             transformation, &temp_transf);
   concat_general_transforms(&temp_transf, in_vol->world_to_voxel,
                             &total_transf);
   delete_general_transform(&temp_transf);

   /* Check for complete linear transformation */
   all_linear = (get_transform_type(&total_transf) == LINEAR);

   /* Transform vectors for linear transformation */
   start[SLICE] = slice_num;
   if (all_linear) {
      DO_TRANSFORM(zero, &total_transf, zero);
      DO_TRANSFORM(column, &total_transf, column);
      DO_TRANSFORM(row, &total_transf, row);
      DO_TRANSFORM(start, &total_transf, start);
   }

   /* Make sure that row and column are vectors and not points */
   VECTOR_DIFF(row, row, zero);
   VECTOR_DIFF(column, column, zero);

   /* Initialize maximum and minimum */
   *maximum = -DBL_MAX;
   *minimum =  DBL_MAX;

   /* Loop over rows of slice */

   for (irow=0; irow < slice->size[SLICE_ROW]; irow++) {

      /* Set starting coordinate of row */
      VECTOR_SCALAR_MULT(coord, row, irow);
      VECTOR_ADD(coord, coord, start);

      /* Loop over columns */

      dptr = slice->data + irow*slice->size[SLICE_COL];
      for (icol=0; icol < slice->size[SLICE_COL]; icol++) {

         /* If transformation is not completely linear, then transform 
            voxel to world, world to world and world to voxel, as needed */
         for (idim=0; idim<WORLD_NDIMS; idim++) 
            transf_coord[idim]=coord[idim];
         if (!all_linear) {
            DO_TRANSFORM(transf_coord, &total_transf, transf_coord);
         }

         /* Do interpolation */
         if (INTERPOLATE(volume, transf_coord, dptr) || volume->use_fill) {
            if (*dptr > *maximum) *maximum = *dptr;
            if (*dptr < *minimum) *minimum = *dptr;
         }

         /* Increment coordinate */
         VECTOR_ADD(coord, coord, column);

         /* Increment slice pointer */
         dptr++;

      }     /* Loop over columns */
   }        /* Loop over rows */

   if ((*maximum == -DBL_MAX) && (*minimum ==  DBL_MAX)) {
      *minimum = 0.0;
      *maximum = SMALL_VALUE;
   }
   else if (*maximum <= *minimum) {
      if (*minimum == 0.0) 
         *maximum = SMALL_VALUE;
      else if (*minimum < 0.0)
         *maximum = 0.0;
      else
         *maximum = 2.0 * (*minimum);
   }

   /* Delete the transformation */
   delete_general_transform(&total_transf);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : trilinear_interpolant
@INPUT      : volume - pointer to volume data
              coord - point at which volume should be interpolated in voxel 
                 units (with 0 being first point of the volume).
@OUTPUT     : result - interpolated value.
@RETURNS    : TRUE if coord is within the volume, FALSE otherwise.
@DESCRIPTION: Routine to interpolate a volume at a point with tri-linear
              interpolation.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int trilinear_interpolant(Volume_Data *volume, 
                                 Coord_Vector coord, double *result)
{
   long slcind, rowind, colind, slcmax, rowmax, colmax;
   long slcnext, rownext, colnext;
   static double f0, f1, f2, r0, r1, r2, r1r2, r1f2, f1r2, f1f2;
   static double v000, v001, v010, v011, v100, v101, v110, v111;

   /* Check that the coordinate is inside the volume */
   slcmax = volume->size[SLC_AXIS] - 1;
   rowmax = volume->size[ROW_AXIS] - 1;
   colmax = volume->size[COL_AXIS] - 1;
   if ((coord[SLICE]  < -VOXEL_COORD_EPS) || 
       (coord[SLICE]  > slcmax+VOXEL_COORD_EPS) ||
       (coord[ROW]    < -VOXEL_COORD_EPS) || 
       (coord[ROW]    > rowmax+VOXEL_COORD_EPS) ||
       (coord[COLUMN] < -VOXEL_COORD_EPS) || 
       (coord[COLUMN] > colmax+VOXEL_COORD_EPS)) {
      *result = volume->fillvalue;
      return FALSE;
   }

   /* Get the whole part of the coordinate */ 
   slcind = (long) coord[SLICE];
   rowind = (long) coord[ROW];
   colind = (long) coord[COLUMN];
   if (slcind >= slcmax-1) slcind = slcmax-1;
   if (rowind >= rowmax-1) rowind = rowmax-1;
   if (colind >= colmax-1) colind = colmax-1;

   /* Get the next voxel up */
   slcnext = slcind+1;
   rownext = rowind+1;
   colnext = colind+1;

   /* Check for case of dimension of length one */
   if (slcmax == 0) {
      slcind = 0;
      slcnext = 0;
   }
   if (rowmax == 0) {
      rowind = 0;
      rownext = 0;
   }
   if (colmax == 0) {
      colind = 0;
      colnext = 0;
   }

   /* Get the relevant voxels */
   VOLUME_VALUE(volume, slcind , rowind , colind , v000);
   VOLUME_VALUE(volume, slcind , rowind , colnext, v001);
   VOLUME_VALUE(volume, slcind , rownext, colind , v010);
   VOLUME_VALUE(volume, slcind , rownext, colnext, v011);
   VOLUME_VALUE(volume, slcnext, rowind , colind , v100);
   VOLUME_VALUE(volume, slcnext, rowind , colnext, v101);
   VOLUME_VALUE(volume, slcnext, rownext, colind , v110);
   VOLUME_VALUE(volume, slcnext, rownext, colnext, v111);

   /* Check that the values are not fill values */
   if ((v000 < volume->vrange[0]) || (v000 > volume->vrange[1]) ||
       (v001 < volume->vrange[0]) || (v001 > volume->vrange[1]) ||
       (v010 < volume->vrange[0]) || (v010 > volume->vrange[1]) ||
       (v011 < volume->vrange[0]) || (v011 > volume->vrange[1]) ||
       (v100 < volume->vrange[0]) || (v100 > volume->vrange[1]) ||
       (v101 < volume->vrange[0]) || (v101 > volume->vrange[1]) ||
       (v110 < volume->vrange[0]) || (v110 > volume->vrange[1]) ||
       (v111 < volume->vrange[0]) || (v111 > volume->vrange[1])) {
      *result = volume->fillvalue;
      return FALSE;
   }

   /* Get the fraction parts */
   f0 = coord[SLICE]  - slcind;
   f1 = coord[ROW]    - rowind;
   f2 = coord[COLUMN] - colind;
   r0 = 1.0 - f0;
   r1 = 1.0 - f1;
   r2 = 1.0 - f2;

   /* Do the interpolation */
   r1r2 = r1 * r2;
   r1f2 = r1 * f2;
   f1r2 = f1 * r2;
   f1f2 = f1 * f2;
   *result =
      r0 * (volume->scale[slcind] *
            (r1r2 * v000 +
             r1f2 * v001 +
             f1r2 * v010 +
             f1f2 * v011) + volume->offset[slcind]);
   *result +=
      f0 * (volume->scale[slcind+1] *
            (r1r2 * v100 +
             r1f2 * v101 +
             f1r2 * v110 +
             f1f2 * v111) + volume->offset[slcind+1]);
   
   return TRUE;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : tricubic_interpolant
@INPUT      : volume - pointer to volume data
              coord - point at which volume should be interpolated in voxel 
                 units (with 0 being first point of the volume).
@OUTPUT     : result - interpolated value.
@RETURNS    : TRUE if coord is within the volume, FALSE otherwise.
@DESCRIPTION: Routine to interpolate a volume at a point with tri-cubic
              interpolation.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 12, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int tricubic_interpolant(Volume_Data *volume, 
                                Coord_Vector coord, double *result)
{
   long slcind, rowind, colind, slcmax, rowmax, colmax, index[VOL_NDIMS];
   double frac[VOL_NDIMS];

   /* Check that the coordinate is inside the volume */
   slcmax = volume->size[SLC_AXIS] - 1;
   rowmax = volume->size[ROW_AXIS] - 1;
   colmax = volume->size[COL_AXIS] - 1;
   if ((coord[SLICE]  < 0) || (coord[SLICE]  > slcmax) ||
       (coord[ROW]    < 0) || (coord[ROW]    > rowmax) ||
       (coord[COLUMN] < 0) || (coord[COLUMN] > colmax)) {
      *result = volume->fillvalue;
      return FALSE;
   }

   /* Get the whole and fractional part of the coordinate */
   slcind = (long) coord[SLICE];
   rowind = (long) coord[ROW];
   colind = (long) coord[COLUMN];
   frac[0] = coord[SLICE]  - slcind;
   frac[1] = coord[ROW]    - rowind;
   frac[2] = coord[COLUMN] - colind;
   slcind--;
   rowind--;
   colind--;

   /* Check for edges - do linear interpolation at edges */
   if ((slcind > slcmax-3) || (slcind < 0) ||
       (rowind > rowmax-3) || (rowind < 0) ||
       (colind > colmax-3) || (colind < 0)) {
      return trilinear_interpolant(volume, coord, result);
   }
   index[0]=slcind; index[1]=rowind; index[2]=colind;

   /* Do the interpolation and return its value */
   return do_Ncubic_interpolation(volume, index, 0, frac, result);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : do_Ncubic_interpolation
@INPUT      : volume - pointer to volume data
              index - indices to start point in volume
                 (bottom of 4x4x4 cube for interpolation)
              cur_dim - dimension to be interpolated (0 = volume, 1 = slice,
                 2 = line)
@OUTPUT     : result - interpolated value.
@RETURNS    : TRUE if coord is within the volume, FALSE otherwise.
@DESCRIPTION: Routine to interpolate a volume, slice or line (specified by
              cur_dim).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 12, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int do_Ncubic_interpolation(Volume_Data *volume, 
                                   long index[], int cur_dim, 
                                   double frac[], double *result)
{
   long base_index;
   double v0, v1, v2, v3, u;
   int found_fillvalue;

   /* Save index that we will change */
   base_index = index[cur_dim];

   /* If last dimension, then just get the values */
   found_fillvalue = FALSE;
   if (cur_dim == VOL_NDIMS-1) {
      VOLUME_VALUE(volume, index[0] ,index[1], index[2], v0);
      index[cur_dim]++;
      VOLUME_VALUE(volume, index[0] ,index[1], index[2], v1);
      index[cur_dim]++;
      VOLUME_VALUE(volume, index[0] ,index[1], index[2], v2);
      index[cur_dim]++;
      VOLUME_VALUE(volume, index[0] ,index[1], index[2], v3);

      /* Check for fillvalues */
      if ((v0 < volume->vrange[0]) || (v0 > volume->vrange[1]) ||
          (v1 < volume->vrange[0]) || (v1 > volume->vrange[1]) ||
          (v2 < volume->vrange[0]) || (v2 > volume->vrange[1]) ||
          (v3 < volume->vrange[0]) || (v3 > volume->vrange[1])) {
         found_fillvalue = TRUE;
      }
   }

   /* Otherwise, recurse */
   else {
      if (!do_Ncubic_interpolation(volume, index, cur_dim+1, frac, &v0)) {
         found_fillvalue = TRUE;
      }
      index[cur_dim]++;
      if (!do_Ncubic_interpolation(volume, index, cur_dim+1, frac, &v1)) {
         found_fillvalue = TRUE;
      }
      index[cur_dim]++;
      if (!do_Ncubic_interpolation(volume, index, cur_dim+1, frac, &v2)) {
         found_fillvalue = TRUE;
      }
      index[cur_dim]++;
      if (!do_Ncubic_interpolation(volume, index, cur_dim+1, frac, &v3)) {
         found_fillvalue = TRUE;
      }
   }

   /* Restore index */
   index[cur_dim] = base_index;

   /* Check for fill value found */
   if (found_fillvalue) {
      *result = volume->fillvalue;
      return FALSE;
   }

   /* Scale values for slices */
   if (cur_dim==0) {
      v0 = v0 * volume->scale[base_index  ] + volume->offset[base_index  ];
      v1 = v1 * volume->scale[base_index+1] + volume->offset[base_index+1];
      v2 = v2 * volume->scale[base_index+2] + volume->offset[base_index+2];
      v3 = v3 * volume->scale[base_index+3] + volume->offset[base_index+3];
   }

   /* Get fraction */
   u = frac[cur_dim];

   /* Do tricubic interpolation (code from Dave MacDonald).
      Gives v1 and v2 at u = 0 and 1 and gives continuity of intensity
      and first derivative. */
   *result =
     ( (v1) + (u) * (
       0.5 * ((v2)-(v0)) + (u) * (
       (v0) - 2.5 * (v1) + 2.0 * (v2) - 0.5 * (v3) + (u) * (
       -0.5 * (v0) + 1.5 * (v1) - 1.5 * (v2) + 0.5 * (v3)  )
                                 )
                    )
     );

   return TRUE;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : nearest_neighbour_interpolant
@INPUT      : volume - pointer to volume data
              coord - point at which volume should be interpolated in voxel 
                 units (with 0 being first point of the volume).
@OUTPUT     : result - interpolated value.
@RETURNS    : TRUE if coord is within the volume, FALSE otherwise.
@DESCRIPTION: Routine to interpolate a volume at a point with nearest
              neighbour interpolation. Allows the coord to be outside
              the volume by up to 1/2 a pixel.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 12, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int nearest_neighbour_interpolant(Volume_Data *volume, 
                                         Coord_Vector coord, double *result)
{
   long slcind, rowind, colind, slcmax, rowmax, colmax;

   /* Check that the coordinate is inside the volume */
   slcmax = volume->size[SLC_AXIS] - 1;
   rowmax = volume->size[ROW_AXIS] - 1;
   colmax = volume->size[COL_AXIS] - 1;
   slcind = ROUND(coord[SLICE]);
   rowind = ROUND(coord[ROW]);
   colind = ROUND(coord[COLUMN]);
   if ((slcind < 0) || (slcind > slcmax) ||
       (rowind < 0) || (rowind > rowmax) ||
       (colind < 0) || (colind > colmax)) {
      *result = volume->fillvalue;
      return FALSE;
   }

   /* Get the value */
   VOLUME_VALUE(volume, slcind  , rowind  , colind  , *result);

   /* Check for fillvalue on input */
   if ((*result < volume->vrange[0]) || (*result > volume->vrange[1])) {
      *result = volume->fillvalue;
      return FALSE;
   }

   *result = volume->scale[slcind] * (*result) + volume->offset[slcind];

   return TRUE;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : renormalize_slices
@INPUT      : ofp - output file pointer
              slice_min - array of slice minima
              slice_max - array of slice maxima
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to loop through the output file and renormalize the
              slices.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : October 29, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void renormalize_slices(Program_Flags *program_flags, VVolume *out_vol,
                               double slice_min[], double slice_max[])
{
   File_Info *ofp;
   long start[MAX_VAR_DIMS], count[MAX_VAR_DIMS], end[MAX_VAR_DIMS];
   long mm_start[MAX_VAR_DIMS];
   long nslice, image, islice, ivolume, image_slice, slice_count;
   int idim, slice_index, index;
   double *image_maximum, *image_minimum;

   /* Set pointer to file information */
   ofp = out_vol->file;

   /* Set output file start, count and end vectors for stepping 
      a slice at a time */
   (void) miset_coords(ofp->ndims, (long) 0, start);
   (void) miset_coords(ofp->ndims, (long) 1, count);
   for (idim=0; idim < ofp->ndims; idim++) {
      end[idim] = ofp->nelements[idim];
   }
   for (idim=0; idim < VOL_NDIMS; idim++) {
      index = ofp->indices[idim];
      if (idim==0) {
         slice_index = index;
         nslice = ofp->nelements[index];
         count[slice_index] = 1;
      }
      else {
         count[index] = ofp->nelements[index];
      }
   }

   /* Find the max/min for each image */
   image_maximum = MALLOC( sizeof(double) * ofp->images_per_file);
   image_minimum = MALLOC( sizeof(double) * ofp->images_per_file);
   for (image=0; image < ofp->images_per_file; image++) {
      image_maximum[image] = -DBL_MAX;
      image_minimum[image] =  DBL_MAX;
   }
   slice_count = 0;
   for (ivolume=0; ivolume < ofp->images_per_file / nslice; ivolume++) {
      for (image_slice=0; image_slice<ofp->slices_per_image; image_slice++) {
         for (islice=0; islice < nslice; islice++) {
            image = ivolume * nslice + islice;
            image_maximum[image] = 
               MAX(image_maximum[image], slice_max[slice_count]);
            image_minimum[image] = 
               MIN(image_minimum[image], slice_min[slice_count]);
            slice_count++;
         }
      }
   }

   /* Initialize file max/min slice count */
   slice_count = 0;

   /* Print log message */
   if (program_flags->verbose) {
      (void) fprintf(stderr, "Renormalizing slices:");
      (void) fflush(stderr);
   }

   /* Loop over output volumes */

   while (start[0] < end[0]) {

      /* Loop over slices */
      for (islice=0; islice < nslice; islice++) {

         /* Print log message */
         if (program_flags->verbose) {
            (void) fprintf(stderr, ".");
            (void) fflush(stderr);
         }

         /* Set slice number in out_start */
         start[slice_index] = islice;

         /* Figure out which image, slice, volume, etc we are dealing with */
         islice = slice_count % nslice;
         ivolume = slice_count / nslice / ofp->slices_per_image;
         image = ivolume * nslice + islice;

         /* Get the slice min/max start coordinate */
         (void) mitranslate_coords(ofp->mincid, 
                                   ofp->imgid, start,
                                   ofp->maxid, mm_start),

         /* Write out the slice max and min */
         (void) mivarput1(ofp->mincid, ofp->maxid, mm_start,
                          NC_DOUBLE, NULL, &slice_max[slice_count]);
         (void) mivarput1(ofp->mincid, ofp->minid, mm_start,
                          NC_DOUBLE, NULL, &slice_min[slice_count]);

         /* Read in the slice */
         (void) miicv_get(ofp->icvid, start, count, out_vol->slice->data);

         /* Write the image max, min and slice */
         (void) mivarput1(ofp->mincid, ofp->maxid, mm_start,
                          NC_DOUBLE, NULL, &image_maximum[image]);
         (void) mivarput1(ofp->mincid, ofp->minid, mm_start,
                          NC_DOUBLE, NULL, &image_minimum[image]);
         (void) miicv_put(ofp->icvid, start, count, out_vol->slice->data);

         /* Increment slice count */
         slice_count++;

      }    /* End loop over slices */

      /* Set slice counter to ensure that we move to the next volume */
      start[slice_index] = end[slice_index] - 1;

      /* Increment in_start counter */
      idim = ofp->ndims-1;
      start[idim] += count[idim];
      while ( (idim>0) && (start[idim] >= end[idim])) {
         start[idim] = 0;
         idim--;
         start[idim] += count[idim];
      }

   }       /* End loop over volumes */

   /* Print end of log message */
   if (program_flags->verbose) {
      (void) fprintf(stderr, "Done\n");
      (void) fflush(stderr);
   }

   /* Free the image max/min arrays */
   FREE(image_maximum);
   FREE(image_minimum);

   return;
}

