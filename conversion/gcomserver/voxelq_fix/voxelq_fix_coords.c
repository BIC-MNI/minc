/* ----------------------------- MNI Header -----------------------------------
@NAME       : voxelq_fix_coords.c
@DESCRIPTION: Routines to fix up the dicom image coordinate information
              so that they work properly with the Picker Voxelq 
              viewing station. This station seems to have trouble with 
              arbitrarily rotated images. The fix consists of recalculating
              the image position along axes parallel to the image acquisition,
              and replacing the direction cosines with those along major
              axes.
@GLOBALS    : (none)
@CREATED    : March 19, 2001 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */

#include <math.h>

/* Coordinate indices relate either to image axis (normal, column or row) or
   to world coordinates (x, y or z) */
typedef enum {NORMAL=0, ROW, COLUMN, AXIS_NDIMS} Axis_Index;
typedef enum {XAXIS=0, YAXIS, ZAXIS, WORLD_NDIMS} World_Index;

/* Function declarations */
static void calculate_orthogonal_vector(double vector1[], 
                                        double vector2[],
                                        double result[]);



/* ----------------------------- MNI Header -----------------------------------
@NAME       : voxelq_fix_coords
@INPUT      : position - coordinate of upper left hand pixel of image
              row_dircos - direction cosines of row axis of image
              col_dircos - direction cosines of column axis of image
@OUTPUT     : position - position of first pixel in new coordinate system
              row_dircos - standard direction cosines ([1,0,0], [0,1,0] or
                 [0,0,1])
              col_dircos - standard direction cosines of column axis
@RETURNS    : (nothing)
@DESCRIPTION: Transforms the position of the first pixel into a new coordinate
              system that has two of its axes parallel to the image axes. The
              third axis is orthogonal to these two. Also transforms the
              image axis direction cosines into this new system.
@METHOD     : 
@GLOBALS    : (none)
@CALLS      : calculate_orthogonal_vector
@CREATED    : March 19, 2001 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
void voxelq_fix_coords(double position[], 
                       double row_dircos[], double col_dircos[])
{
   double *dircos[AXIS_NDIMS];
   double normal[WORLD_NDIMS], new_position[AXIS_NDIMS];
   double lengths[AXIS_NDIMS];
   double largest_value, value;
   Axis_Index iaxis;
   World_Index iworld, largest_index;
   int world_to_axis[WORLD_NDIMS];
   double world_axis_flips[WORLD_NDIMS];

   /* This variable gives the proper sign for each world axis as
      normal, column or row axis. This is used to ensure that axes
      are pointing in the right directino and the signs of the coordinates
      are correct */
   static double world_axis_signs[WORLD_NDIMS][AXIS_NDIMS] = {
      {-1.0,  1.0, 1.0},
      { 1.0,  1.0, 1.0},
      { 1.0, -1.0,-1.0}
   };

   /* 
    * Get orthogonal vector
    */
   calculate_orthogonal_vector(row_dircos, col_dircos, normal);

   /* 
    * Put the direction cosines of the axes into one matrix.
    */
   dircos[NORMAL] = normal;
   dircos[ROW] = row_dircos;
   dircos[COLUMN] = col_dircos;

   /* 
    * Get vector lengths
    */
   for (iaxis=0; iaxis < AXIS_NDIMS; iaxis++) {
      lengths[iaxis] = 0.0;
      for (iworld=0; iworld < WORLD_NDIMS; iworld++) {
         lengths[iaxis] += dircos[iaxis][iworld];
      }
      lengths[iaxis] = sqrt(lengths[iaxis]);
   }

   /* 
    * If anyone has length zero (normal could have zero length from parallel
    * image vectors), then just get out of here without touching anything.
    */
   for (iaxis=0; iaxis < AXIS_NDIMS; iaxis++) {
      if (lengths[iaxis] == 0.0) return;
   }

   /* 
    * Find world component (x,y,z) to axis (normal, column, row) mapping 
    * by looking for the largest world coordinate (absolute value) of
    * each axis. Also keep track of whether axis needs to be flipped.
    */
   for (iworld=0; iworld < WORLD_NDIMS; iworld++) {
      world_to_axis[iworld] = -1;
   }
   for (iaxis=0; iaxis < AXIS_NDIMS; iaxis++) {
      largest_index = 0;
      largest_value = dircos[iaxis][largest_index];
      for (iworld=1; iworld < WORLD_NDIMS; iworld++) {
         value = dircos[iaxis][iworld];
         if (fabs(value) > fabs(largest_value)) {
            largest_value = value;
            largest_index = iworld;
         }
      }
      world_to_axis[largest_index] = iaxis;

      /* Figure out whether to flip the coordinate or not */
      world_axis_flips[largest_index] = world_axis_signs[largest_index][iaxis];
      if (largest_value * world_axis_signs[largest_index][iaxis] < 0.0) {
         world_axis_flips[largest_index] *= -1.0;
      }
   }

   /* 
    * Make sure that there is no conflict. If there is, then bail.
    */
   for (iworld=0; iworld < WORLD_NDIMS; iworld++) {
      if (world_to_axis[iworld] < 0) return;
   }
   

   /* 
    * Project position onto axes
    */
   for (iaxis=0; iaxis < AXIS_NDIMS; iaxis++) {
      new_position[iaxis] = 0.0;
      for (iworld=0; iworld < WORLD_NDIMS; iworld++) {
         new_position[iaxis] += dircos[iaxis][iworld] * position[iworld];
      }
   }

   /* 
    * Re-order the position coordinates into world coordinates
    * and flip any coordinates that need it
    */
   for (iworld=0; iworld < WORLD_NDIMS; iworld++) {
      iaxis = world_to_axis[iworld];
      position[iworld] = new_position[iaxis] * world_axis_flips[iworld];
   }

   /* Set the standard direction cosines. Since dircos just points back
    * to the original data, this writes the output arguments. We are also
    *  setting the normal vector, but that does not hurt.
    */
   for (iaxis=0; iaxis < AXIS_NDIMS; iaxis++) {
      for (iworld=0; iworld < WORLD_NDIMS; iworld++) {
         if (world_to_axis[iworld] == iaxis) {
            dircos[iaxis][iworld] = world_axis_signs[iworld][iaxis];
         }
         else {
            dircos[iaxis][iworld] = 0.0;
         }
      }
   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : calculate_orthogonal_vector
@INPUT      : vector1 - first vector
              vector2 - second vector
@OUTPUT     : result - vector orthogonal to vector1 and vector2
@RETURNS    : (nothing)
@DESCRIPTION: Routine to calculate a vector that is orthogonal to 2 other
              vectors by doing a cross product. The vectors are copied
              before the calculation so the result vector can be one of
              the input vectors.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 7, 1995 (Peter Neelin)
@MODIFIED   : 
@COPYRIGHT  :
              Copyright 1995 Peter Neelin, McConnell Brain Imaging Centre, 
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */
static void calculate_orthogonal_vector(double vector1[], 
                                         double vector2[],
                                         double result[])
{
   int idim;
   double v1[WORLD_NDIMS], v2[WORLD_NDIMS];

   /* Make a copy of the vectors */
   for (idim=0; idim < WORLD_NDIMS; idim++) {
      v1[idim] = vector1[idim];
      v2[idim] = vector2[idim];
   }

   for (idim=0; idim < WORLD_NDIMS; idim++) {
      result[idim] = 
         v1[(idim+1)%WORLD_NDIMS] * v2[(idim+2)%WORLD_NDIMS] - 
         v2[(idim+1)%WORLD_NDIMS] * v1[(idim+2)%WORLD_NDIMS];
   }

}
