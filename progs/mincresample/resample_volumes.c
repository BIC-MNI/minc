/* ----------------------------- MNI Header -----------------------------------
@NAME       : resample_volumes.c
@DESCRIPTION: File containing routines to resample minc volumes.
@METHOD     : 
@GLOBALS    : 
@CREATED    : February 8, 1993 (Peter Neelin)
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
static char rcsid[]="$Header: /private-cvsroot/minc/progs/mincresample/resample_volumes.c,v 1.6 1993-07-21 12:49:23 neelin Exp $";
#endif

#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <string.h>
#include <math.h>
#include <minc.h>
#include <recipes.h>
#include <minc_def.h>
#include "mincresample.h"

/* Some external functions used in this file */
void lubksb(float **a, int n, int *indx, float *b);
void ludcmp(float **a, int n, int *indx, float *d);


#define public
#define private static

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
                             Volume *in_vol, Volume *out_vol, 
                             Transformation *transformation)
{
   long in_start[MAX_VAR_DIMS], in_count[MAX_VAR_DIMS], in_end[MAX_VAR_DIMS];
   long out_start[MAX_VAR_DIMS], out_count[MAX_VAR_DIMS];
   long mm_start[MAX_VAR_DIMS];   /* Vector for min/max variables */
   long nslice, islice;
   int idim, index, slice_index;
   double maximum, minimum, valid_range[2];
   File_Info *ifp,*ofp;

   /* Set pointers to file information */
   ifp = in_vol->file;
   ofp = out_vol->file;

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
         if (minimum > valid_range[0]) valid_range[0] = minimum;

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
   long islice, mm_start[MAX_VAR_DIMS], mm_count[MAX_VAR_DIMS];
   int varid, ivar;
   double *values, maximum, minimum, denom;

   /* Load the file */
   (void) ncvarget(file->mincid, file->imgid, 
                   start, count, volume->data);

   /* Read the max and min from the file into the scale and offset variables 
      (maxima into scale and minima into offset */

   if ((file->maxid != MI_ERROR) && (file->minid != MI_ERROR)) {

      for (ivar=0; ivar<2; ivar++) {
         varid  = (ivar == 0 ? file->maxid   : file->minid);
         values = (ivar == 0 ? volume->scale : volume->offset);
         (void) mivarget(file->mincid, varid, 
                         mitranslate_coords(file->mincid, file->imgid, start,
                                            varid,
                               miset_coords(MAX_VAR_DIMS, 0L, mm_start)),
                         mitranslate_coords(file->mincid, file->imgid, count,
                                            varid,
                               miset_coords(MAX_VAR_DIMS, 1L, mm_count)),
                      NC_DOUBLE, NULL, values);
      }        /* Loop over max/min variables */

   }        /* If max/min variables both exist */

   /* If max/min variables don't exist, set max to 1 and min to 0 */
   else {
      for (islice=0; islice < volume->size[0]; islice++) {
         volume->scale[islice] = DEFAULT_MAX;
         volume->offset[islice] = DEFAULT_MIN;
      }
   }
                                                      

   /* Calculate the scale and offset */
   for (islice=0; islice < volume->size[0]; islice++) {

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
public void get_slice(long slice_num, Volume *in_vol, Volume *out_vol, 
                      Transformation *transformation, 
                      double *minimum, double *maximum)
{
   Slice_Data *slice;
   Volume_Data *volume;
   double *dptr;
   long irow, icol;
   int voxel_to_world_done, world_to_world_done, world_to_voxel_done;
   int all_linear;

   /* Coordinate vectors for stepping through slice */
   Coord_Vector zero = {0, 0, 0};
   Coord_Vector column = {0, 0, 1};
   Coord_Vector row = {0, 1, 0};
   Coord_Vector start = {0, 0, 0};    /* start.z set later to slice_num */
   Coord_Vector coord;

   /* Identity transformation */
   static Linear_Transformation identity_transformation = {
      {{1, 0, 0, 0},
       {0, 1, 0, 0},
       {0, 0, 1, 0}}
   };
   Transformation first_transf_struct;
   Transformation *first_transf = &first_transf_struct;
   Linear_Transformation *matrx;

   /* Set up first transformation */
   matrx = MALLOC(sizeof(*matrx));
   *matrx = identity_transformation;
   first_transf->linear = TRUE;
   first_transf->transform = do_linear_transformation;
   first_transf->trans_data = matrx;

   /* Get slice and volume pointers */
   volume = in_vol->volume;
   slice = out_vol->slice;

   /* Check for linear transformations */
   voxel_to_world_done = FALSE;
   world_to_world_done = FALSE; 
   world_to_voxel_done = FALSE;
   if (IS_LINEAR(out_vol->voxel_to_world)) {
      voxel_to_world_done = TRUE;
      mult_linear_transform(first_transf, 
                            out_vol->voxel_to_world, first_transf);
      if (IS_LINEAR(transformation)) {
         world_to_world_done = TRUE; 
         mult_linear_transform(first_transf, transformation, first_transf);
         if (IS_LINEAR(in_vol->world_to_voxel)) {
            world_to_voxel_done = TRUE;
            mult_linear_transform(first_transf, 
                                  in_vol->world_to_voxel, first_transf);
         }
      }
   }
   all_linear = world_to_voxel_done;

   /* Transform vectors with first transformation */
   start.x = slice_num;
   DO_TRANSFORM(&zero, first_transf, &zero);
   DO_TRANSFORM(&column, first_transf, &column);
   DO_TRANSFORM(&row, first_transf, &row);
   DO_TRANSFORM(&start, first_transf, &start);

   /* Make sure that row and column are vectors and not points */
   VECTOR_DIFF(row, row, zero);
   VECTOR_DIFF(column, column, zero);

   /* Initialize maximum and minimum */
   *maximum = -DBL_MAX;
   *minimum =  DBL_MAX;

   /* Loop over rows of slice */

   for (irow=0; irow < slice->size[0]; irow++) {

      /* Set starting coordinate of row */
      VECTOR_SCALAR_MULT(coord, row, irow);
      VECTOR_ADD(coord, coord, start);

      /* Loop over columns */

      dptr = slice->data + irow*slice->size[1];
      for (icol=0; icol < slice->size[1]; icol++) {

         /* If transformation is not completely linear, then transform 
            voxel to world, world to world and world to voxel, as needed */
         if (!all_linear) {
            if (!voxel_to_world_done) {
               DO_TRANSFORM(&coord, out_vol->voxel_to_world, &coord);
            }
            if (!world_to_world_done) {
               DO_TRANSFORM(&coord, transformation, &coord);
            }
            if (!world_to_voxel_done) {
               DO_TRANSFORM(&coord, in_vol->world_to_voxel, &coord);
            }
         }

         /* Do interpolation */
         if (INTERPOLATE(volume, &coord, dptr) || volume->use_fill) {
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

   /* Free matrix */
   FREE(matrx);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : invert_transformation
@INPUT      : transformation - transformation to invert
@OUTPUT     : result - resultant transformation
@RETURNS    : (nothing)
@DESCRIPTION: Routine to invert a transformation. Currently only works on
              linear transformations.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 9, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void invert_transformation(Transformation *result, 
                                  Transformation *transformation)
{
   Linear_Transformation *matrx, *result_matrix;
   float **nrmatrix, **nrresult, nrd, nrcol[MAT_NDIMS+1];
   int nrindex[MAT_NDIMS+1], idim, jdim;

   /* Check that transformation is linear */
   if (!IS_LINEAR(transformation)) {
      (void) fprintf(stderr, "Unable to invert non-linear transformations!\n");
      exit(EXIT_FAILURE);
   }
   matrx = transformation->trans_data;

   /* Set up numerical recipes matrices */
   nrmatrix = matrix(1, MAT_NDIMS, 1, MAT_NDIMS);
   nrresult = matrix(1, MAT_NDIMS, 1, MAT_NDIMS);

   for (idim=1; idim<=MAT_NDIMS; idim++) {
      for (jdim=1; jdim<=MAT_NDIMS; jdim++) {
         if (idim<=WORLD_NDIMS)
            nrmatrix[idim][jdim] = matrx->mat[idim-1][jdim-1];
         else if (jdim<=WORLD_NDIMS)
            nrmatrix[idim][jdim] = 0.0;
         else 
            nrmatrix[idim][jdim] = 1.0;
      }
   }

   /* Invert matrix */
   ludcmp( nrmatrix, MAT_NDIMS, nrindex, &nrd );

   for (jdim=1; jdim<=MAT_NDIMS; jdim++) {
      for (idim=1; idim<=MAT_NDIMS; idim++)
         nrcol[idim] = 0.0;
      nrcol[jdim] = 1.0;
      lubksb( nrmatrix, MAT_NDIMS, nrindex, nrcol );
      for (idim=1; idim<=MAT_NDIMS; idim++)
         nrresult[idim][jdim] = nrcol[idim];
   }

   /* Save the result */
   if (result->trans_data != NULL) FREE(result->trans_data);
   *result = *transformation;
   matrx = MALLOC(sizeof(*result_matrix));
   result->trans_data = matrx;
   for (idim=1; idim<=WORLD_NDIMS; idim++)
      for (jdim=1; jdim<=MAT_NDIMS; jdim++)
         matrx->mat[idim-1][jdim-1] = nrresult[idim][jdim];

   /* free the nr matrices */
   free_matrix(nrmatrix, 1, MAT_NDIMS, 1, MAT_NDIMS);
   free_matrix(nrresult, 1, MAT_NDIMS, 1, MAT_NDIMS);


   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : do_linear_transformation
@INPUT      : trans_data - pointer to transformation data
              coordinate - point to be transformed
@OUTPUT     : result - resulting coordinate
@RETURNS    : (nothing)
@DESCRIPTION: Routine to apply a linear transformation. 
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void do_linear_transformation(Coord_Vector *result, void *trans_data, 
                                     Coord_Vector *coordinate)
{
   Linear_Transformation *matrx;
   int idim, jdim;
   double lcoord[WORLD_NDIMS], lresult[WORLD_NDIMS];

   /* Get linear transformation info */
   matrx = trans_data;

   /* Make our own coord vector */
   lcoord[X] = coordinate->x;
   lcoord[Y] = coordinate->y;
   lcoord[Z] = coordinate->z;

   /* Calculate transformation */
   for (idim=0; idim<WORLD_NDIMS; idim++) {
      lresult[idim] = matrx->mat[idim][MAT_NDIMS-1];
      for (jdim=0; jdim<WORLD_NDIMS; jdim++) {
         lresult[idim] += matrx->mat[idim][jdim] * lcoord[jdim];
      }
   }

   /* Save the result */
   result->x = lresult[X];
   result->y = lresult[Y];
   result->z = lresult[Z];

   return;
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
                                 Coord_Vector *coord, double *result)
{
   long ind0, ind1, ind2, max0, max1, max2;
   static double f0, f1, f2, r0, r1, r2, r1r2, r1f2, f1r2, f1f2;
   static double v000, v001, v010, v011, v100, v101, v110, v111;

   /* Check that the coordinate is inside the volume */
   max0 = volume->size[0] - 1;
   max1 = volume->size[1] - 1;
   max2 = volume->size[2] - 1;
   if ((coord->x < 0) || (coord->x > max0) ||
       (coord->y < 0) || (coord->y > max1) ||
       (coord->z < 0) || (coord->z > max2)) {
      *result = volume->fillvalue;
      return FALSE;
   }

   /* Get the whole part of the coordinate */ 
   ind0 = (long) coord->x;
   ind1 = (long) coord->y;
   ind2 = (long) coord->z;
   if (ind0 >= max0-1) ind0 = max0-1;
   if (ind1 >= max1-1) ind1 = max1-1;
   if (ind2 >= max2-1) ind2 = max2-1;

   /* Get the relevant voxels */
   VOLUME_VALUE(volume, ind0  , ind1  , ind2  , v000);
   VOLUME_VALUE(volume, ind0  , ind1  , ind2+1, v001);
   VOLUME_VALUE(volume, ind0  , ind1+1, ind2  , v010);
   VOLUME_VALUE(volume, ind0  , ind1+1, ind2+1, v011);
   VOLUME_VALUE(volume, ind0+1, ind1  , ind2  , v100);
   VOLUME_VALUE(volume, ind0+1, ind1  , ind2+1, v101);
   VOLUME_VALUE(volume, ind0+1, ind1+1, ind2  , v110);
   VOLUME_VALUE(volume, ind0+1, ind1+1, ind2+1, v111);

   /* Get the fraction parts */
   f0 = coord->x - ind0;
   f1 = coord->y - ind1;
   f2 = coord->z - ind2;
   r0 = 1.0 - f0;
   r1 = 1.0 - f1;
   r2 = 1.0 - f2;

   /* Do the interpolation */
   r1r2 = r1 * r2;
   r1f2 = r1 * f2;
   f1r2 = f1 * r2;
   f1f2 = f1 * f2;
   *result =
      r0 * (volume->scale[ind0] *
            (r1r2 * v000 +
             r1f2 * v001 +
             f1r2 * v010 +
             f1f2 * v011) + volume->offset[ind0]);
   *result +=
      f0 * (volume->scale[ind0+1] *
            (r1r2 * v100 +
             r1f2 * v101 +
             f1r2 * v110 +
             f1f2 * v111) + volume->offset[ind0+1]);
   
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
                                Coord_Vector *coord, double *result)
{
   long ind0, ind1, ind2, max0, max1, max2, index[VOL_NDIMS];
   double frac[VOL_NDIMS];

   /* Check that the coordinate is inside the volume */
   max0 = volume->size[0] - 1;
   max1 = volume->size[1] - 1;
   max2 = volume->size[2] - 1;
   if ((coord->x < 0) || (coord->x > max0) ||
       (coord->y < 0) || (coord->y > max1) ||
       (coord->z < 0) || (coord->z > max2)) {
      *result = volume->fillvalue;
      return FALSE;
   }

   /* Get the whole and fractional part of the coordinate */
   ind0 = (long) coord->x;
   ind1 = (long) coord->y;
   ind2 = (long) coord->z;
   frac[0] = coord->x - ind0;
   frac[1] = coord->y - ind1;
   frac[2] = coord->z - ind2;
   ind0--;
   ind1--;
   ind2--;

   /* Check for edges - do linear interpolation at edges */
   if ((ind0 > max0-3) || (ind0 < 0) ||
       (ind1 > max1-3) || (ind1 < 0) ||
       (ind2 > max2-3) || (ind2 < 0)) {
      return trilinear_interpolant(volume, coord, result);
   }
   index[0]=ind0; index[1]=ind1; index[2]=ind2;

   /* Do the interpolation */
   do_Ncubic_interpolation(volume, index, 0, frac, result);

   return TRUE;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : do_Ncubic_interpolation
@INPUT      : volume - pointer to volume data
              index - indices to start point in volume
                 (bottom of 4x4x4 cube for interpolation)
              cur_dim - dimension to be interpolated (0 = volume, 1 = slice,
                 2 = line)
@OUTPUT     : result - interpolated value.
@RETURNS    : (nothing)
@DESCRIPTION: Routine to interpolate a volume, slice or line (specified by
              cur_dim).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 12, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void do_Ncubic_interpolation(Volume_Data *volume, 
                                    long index[], int cur_dim, 
                                    double frac[], double *result)
{
   long base_index;
   double v0, v1, v2, v3, u;

   /* Save index that we will change */
   base_index = index[cur_dim];

   /* If last dimension, then just get the values */
   if (cur_dim == VOL_NDIMS-1) {
      VOLUME_VALUE(volume, index[0] ,index[1], index[2], v0);
      index[cur_dim]++;
      VOLUME_VALUE(volume, index[0] ,index[1], index[2], v1);
      index[cur_dim]++;
      VOLUME_VALUE(volume, index[0] ,index[1], index[2], v2);
      index[cur_dim]++;
      VOLUME_VALUE(volume, index[0] ,index[1], index[2], v3);
   }

   /* Otherwise, recurse */
   else {
      do_Ncubic_interpolation(volume, index, cur_dim+1, frac, &v0);
      index[cur_dim]++;
      do_Ncubic_interpolation(volume, index, cur_dim+1, frac, &v1);
      index[cur_dim]++;
      do_Ncubic_interpolation(volume, index, cur_dim+1, frac, &v2);
      index[cur_dim]++;
      do_Ncubic_interpolation(volume, index, cur_dim+1, frac, &v3);
   }

   /* Restore index */
   index[cur_dim] = base_index;

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

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : nearest_neighbour_interpolant
@INPUT      : volume - pointer to volume data
              coord - point at which volume should be interpolated in voxel 
                 units (with 0 being first point of the volume).
@OUTPUT     : result - interpolated value.
@RETURNS    : TRUE if coord is within the volume, FALSE otherwise.
@DESCRIPTION: Routine to interpolate a volume at a point with nearest
              neighbour interpolation.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 12, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int nearest_neighbour_interpolant(Volume_Data *volume, 
                                         Coord_Vector *coord, double *result)
{
   long ind0, ind1, ind2, max0, max1, max2;

   /* Check that the coordinate is inside the volume */
   max0 = volume->size[0] - 1;
   max1 = volume->size[1] - 1;
   max2 = volume->size[2] - 1;
   if ((coord->x < 0) || (coord->x > max0) ||
       (coord->y < 0) || (coord->y > max1) ||
       (coord->z < 0) || (coord->z > max2)) {
      *result = volume->fillvalue;
      return FALSE;
   }

   /* Get the whole part of the coordinate */
   ind0 = (long) (coord->x + 0.5);
   ind1 = (long) (coord->y + 0.5);
   ind2 = (long) (coord->z + 0.5);

   /* Get the value */
   VOLUME_VALUE(volume, ind0  , ind1  , ind2  , *result);

   *result = volume->scale[ind0] * (*result) + volume->offset[ind0];

   return TRUE;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : mult_linear_transform
@INPUT      : transform1 - first transformation
              transform2 - second transformation
@OUTPUT     : result - resulting transformation
@RETURNS    : (nothing)
@DESCRIPTION: Routine to multiply two linear matrices.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void mult_linear_transform(Transformation *result, 
                                  Transformation *transform1, 
                                  Transformation *transform2)
{
   int idim, jdim, kdim;
   Linear_Transformation *matrix1, *matrix2, *result_matrix;

   /* Check for linear transformations */
   if (!IS_LINEAR(transform1) || !IS_LINEAR(transform2)) {
      (void) fprintf(stderr, 
                     "Unable to multiply two non-linear transformations.\n");
      exit(EXIT_FAILURE);
   }
   matrix1 = transform1->trans_data;
   matrix2 = transform2->trans_data;

   /* Get space for the result */
   result_matrix = MALLOC(sizeof(*result_matrix));

   /* Multiply the matrices */
   for (idim=0; idim < WORLD_NDIMS; idim++) {
      for (jdim=0; jdim < MAT_NDIMS; jdim++) {
         if (jdim < WORLD_NDIMS) 
            result_matrix->mat[idim][jdim] = 0.0;
         else
            result_matrix->mat[idim][jdim] = 
               matrix1->mat[idim][MAT_NDIMS-1];
         for (kdim=0; kdim < WORLD_NDIMS; kdim++) {
            result_matrix->mat[idim][jdim] += 
               matrix1->mat[idim][kdim] * matrix2->mat[kdim][jdim];
         }
      }
   }

   /* Save the result */
   if (result->trans_data != NULL) FREE(result->trans_data);
   *result = *transform1;
   result->trans_data = result_matrix;

   return;
}
