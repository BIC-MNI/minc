/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_origin_to_start.c
@DESCRIPTION: File containing routine to convert an origin specified in
              world coordinates to 3 minc start values by parallel projection.
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 7, 1995 (Peter Neelin)
@MODIFIED   : $Log: convert_origin_to_start.c,v $
@MODIFIED   : Revision 6.0  1997-09-12 13:23:41  neelin
@MODIFIED   : Release of minc version 0.6
@MODIFIED   :
 * Revision 5.0  1997/08/21  13:24:41  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:00:50  neelin
 * Release of minc version 0.4
 *
 * Revision 1.1  1995/11/10  20:01:27  neelin
 * Initial revision
 *
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

#ifndef lint
static char rcsid[]="$Header: /private-cvsroot/minc/progs/Proglib/convert_origin_to_start.c,v 6.0 1997-09-12 13:23:41 neelin Rel $";
#endif

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <convert_origin_to_start.h>

/* Constant definition */
#define public
#define private static
#define WORLD_NDIMS 3

/* Function declarations */
private void calculate_orthogonal_vector(double vector1[], 
                                         double vector2[],
                                         double result[]);

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_origin_to_start
@INPUT      : origin - point to project
              xdircos - vector specifying X direction cosine
              ydircos - vector specifying Y direction cosine
              zdircos - vector specifying Z direction cosine
@OUTPUT     : start - array giving X, Y and Z start values
@RETURNS    : 0 on success, 1 if some direction cosines have zero length,
              2 if some direction cosines are parallel.
@DESCRIPTION: Routine to convert an origin specified in world coordinates
              to an array of minc start positions by projecting the point 
              along the edges of the parallelopiped formed by the 
              3 direction cosines. Direction cosines are normalized.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 7, 1995 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int convert_origin_to_start(double origin[], 
                                    double xdircos[],
                                    double ydircos[], 
                                    double zdircos[],
                                    double start[])
{
   int idim, jdim;
   int nparallel;
   double *axes[WORLD_NDIMS];
   double normal[WORLD_NDIMS], lengths[WORLD_NDIMS];
   double numerator, denominator, normal_length;

   /* Set up dircos matrix */
   axes[0] = xdircos;
   axes[1] = ydircos;
   axes[2] = zdircos;

   /* Calculate lengths of direction cosines, checking for zero lengths and
      parallel vectors. */
   nparallel = 0;
   for (idim=0; idim < WORLD_NDIMS; idim++) {
      calculate_orthogonal_vector(axes[idim], axes[(idim+1)%WORLD_NDIMS],
                                  normal);
      lengths[idim] = 0.0;
      normal_length = 0.0;
      for (jdim=0; jdim < WORLD_NDIMS; jdim++) {
         lengths[idim] += axes[idim][jdim] * axes[idim][jdim];
         normal_length += normal[jdim] * normal[jdim];
      }
      lengths[idim] = sqrt(lengths[idim]);
      if (lengths[idim] == 0.0) {
         return 1;
      }
      if (normal_length == 0.0) {
         nparallel++;
      }
   }
   if (nparallel != 0)
      return 2;

   /* Loop over axes, calculating projections */
   for (idim=0; idim < WORLD_NDIMS; idim++) {

      /* Calculate vector normal to other two axes by doing cross product */
      calculate_orthogonal_vector(axes[(idim+1)%WORLD_NDIMS], 
                                  axes[(idim+2)%WORLD_NDIMS], normal);

      /* Calculate dot product of origin with normal (numerator) and
         dot product of current axis with normal (denominator) */
      denominator = numerator = 0.0;
      for (jdim=0; jdim < WORLD_NDIMS; jdim++) {
         numerator += origin[jdim] * normal[jdim];
         denominator += axes[idim][jdim] * normal[jdim];
      }

      /* Calculate parallel projection */
      if (denominator != 0.0)
         start[idim] = lengths[idim] * numerator / denominator;
      else
         start[idim] = 0.0;
   }

   return 0;

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
---------------------------------------------------------------------------- */
private void calculate_orthogonal_vector(double vector1[], 
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

