#ifndef  DEF_TAG_IO
#define  DEF_TAG_IO

/* ----------------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
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
static char transforms_rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Include/volume_io/transforms.h,v 1.11 1995-07-31 13:44:33 david Exp $";
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : transforms.h
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Types for defining general transforms.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

/* --- the list of supported transform types */

typedef enum { LINEAR, THIN_PLATE_SPLINE, USER_TRANSFORM,
               CONCATENATED_TRANSFORM, GRID_TRANSFORM }
               Transform_types;

/* --- the user transformation function */

typedef  void   (*User_transform_function)( void  *user_data,
                                            Real  x,
                                            Real  y,
                                            Real  z,
                                            Real  *x_trans,
                                            Real  *y_trans,
                                            Real  *z_trans );

/* --- the general transformation type */

typedef struct General_transform
{
    Transform_types             type;
    BOOLEAN                     inverse_flag;

    /* --- linear transform */

    Transform                   *linear_transform;
    Transform                   *inverse_linear_transform;

    /* --- non-linear transform */

    int                         n_points;
    int                         n_dimensions;
    Real                        **points;
    Real                        **displacements;   /* n_points + n_dim + 1 by */
                                                   /* n_dim */

    /* --- grid transform */

    void                        *displacement_volume;

    /* --- user_defined */

    void                        *user_data;
    size_t                      size_user_data;
    User_transform_function     user_transform_function;
    User_transform_function     user_inverse_transform_function;

    /* --- concatenated transform */

    int                         n_transforms;
    struct General_transform    *transforms;

} General_transform;

#endif
