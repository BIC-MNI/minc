#include  <internal_volume_io.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/minc/volume_io/MNI_formats/grid_transforms.c,v 1.1 1995-03-13 13:41:08 david Exp $";
#endif

#define   DEGREES_CONTINUITY         2    /* Cubic interpolation */

#define   INVERSE_FUNCTION_TOLERANCE     0.001
#define   INVERSE_DELTA_TOLERANCE        -1.0
#define   MAX_INVERSE_ITERATIONS         20

/* ----------------------------- MNI Header -----------------------------------
@NAME       : grid_transform_point
@INPUT      : transform
              x
              y
              z
@OUTPUT     : x_transformed
              y_transformed
              z_transformed
@RETURNS    : 
@DESCRIPTION: Applies a grid transform to the point
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Feb. 21, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  grid_transform_point(
    General_transform   *transform,
    Real                x,
    Real                y,
    Real                z,
    Real                *x_transformed,
    Real                *y_transformed,
    Real                *z_transformed )
{
    Real    values[N_DIMENSIONS];

    /* --- the volume that defines the transform is an offset vector,
           so evaluate the volume at the given position and add the
           resulting offset to the given position */

    evaluate_volume_in_world( transform->displacement_volume,
                              x, y, z, DEGREES_CONTINUITY, 0.0,
                              values, NULL, NULL, NULL,
                              NULL, NULL, NULL, NULL, NULL, NULL );
    
    *x_transformed = x + values[X];
    *y_transformed = y + values[Y];
    *z_transformed = z + values[Z];
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : forward_function
@INPUT      : function_data  - contains transform info
              parameters     - x,y,z position
@OUTPUT     : values         - where x,y,z, maps to
              derivatives    - the 3 by 3 derivatives of the mapping
@RETURNS    : 
@DESCRIPTION: This function does the same thing as grid_transform_point(),
              but also gets derivatives.  This function is passed to the
              newton function solution routine to perform the inverse mapping
              of the grid transformation.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Feb.   , 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  void  forward_function(
    void   *function_data,
    Real   parameters[],
    Real   values[],
    Real   **derivatives )
{
    int                c;
    General_transform  *transform;
    Real               deriv_x[N_DIMENSIONS], deriv_y[N_DIMENSIONS];
    Real               deriv_z[N_DIMENSIONS];

    transform = (General_transform *) function_data;

    /* --- store the offset vector in values[0-2] */

    evaluate_volume_in_world( transform->displacement_volume,
                              parameters[X], parameters[Y], parameters[Z],
                              DEGREES_CONTINUITY, 0.0,
                              values, deriv_x, deriv_y, deriv_z,
                              NULL, NULL, NULL, NULL, NULL, NULL );

    for_less( c, 0, N_DIMENSIONS )
    {
        values[c] += parameters[c];   /* to get x',y',z', add offset to x,y,z */

        /*--- given the derivatives of the offset, compute the
              derivatives of (x,y,z) + offset, with respect to x,y,z */

        derivatives[c][X] = deriv_x[c];
        derivatives[c][Y] = deriv_y[c];
        derivatives[c][Z] = deriv_z[c];
        derivatives[c][c] += 1.0;    /* deriv of (x,y,z) w.r.t. x or y or z  */
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : grid_inverse_transform_point
@INPUT      : transform
              x
              y
              z
@OUTPUT     : x_transformed
              y_transformed
              z_transformed
@RETURNS    : 
@DESCRIPTION: Applies the inverse grid transform to the point.  This is done
              by using newton-rhapson steps to find the point which maps to
              the parameters (x,y,z).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Feb. 21, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  grid_inverse_transform_point(
    General_transform   *transform,
    Real                x,
    Real                y,
    Real                z,
    Real                *x_transformed,
    Real                *y_transformed,
    Real                *z_transformed )
{
    Real   solution[N_DIMENSIONS];
    Real   initial_guess[N_DIMENSIONS];
    Real   desired_values[N_DIMENSIONS];

    /* --- fill in the initial guess */

    initial_guess[X] = x;
    initial_guess[Y] = y;
    initial_guess[Z] = z;

    /* --- define what the desired function values are */

    desired_values[X] = x;
    desired_values[Y] = y;
    desired_values[Z] = z;

    /* --- find the x,y,z that are mapped to the desired values */

    if( newton_root_find( N_DIMENSIONS, forward_function,
                          (void *) transform,
                          initial_guess, desired_values,
                          solution, INVERSE_FUNCTION_TOLERANCE, 
                          INVERSE_DELTA_TOLERANCE, MAX_INVERSE_ITERATIONS ))
    {
        *x_transformed = solution[X];
        *y_transformed = solution[Y];
        *z_transformed = solution[Z];
    }
    else  /* --- if no solution found, not sure what is reasonable to return */
    {
        *x_transformed = x;
        *y_transformed = y;
        *z_transformed = z;
    }
}
