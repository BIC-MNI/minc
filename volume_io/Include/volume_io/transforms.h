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
@VERSION    : $Header: /private-cvsroot/minc/volume_io/Include/volume_io/transforms.h,v 1.12.2.2 2005-03-31 17:39:49 bert Exp $
---------------------------------------------------------------------------- */

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
              VIO_Transform_types;

/* --- the user transformation function */

typedef  void   (*VIO_User_transform_function)( void  *user_data,
                                            VIO_Real  x,
                                            VIO_Real  y,
                                            VIO_Real  z,
                                            VIO_Real  *x_trans,
                                            VIO_Real  *y_trans,
                                            VIO_Real  *z_trans );

/* --- the general transformation type */

typedef struct VIO_General_transform
{
    VIO_Transform_types         type;
    VIO_BOOL                    inverse_flag;

    /* --- linear transform */

    VIO_Transform               *linear_transform;
    VIO_Transform               *inverse_linear_transform;

    /* --- non-linear transform */

    int                         n_points;
    int                         n_dimensions;
    VIO_Real                    **points;
    VIO_Real                    **displacements;   /* n_points + n_dim + 1 by */
                                                   /* n_dim */

    /* --- grid transform */

    void                        *displacement_volume;

    /* --- user_defined */

    void                        *user_data;
    size_t                      size_user_data;
    VIO_User_transform_function     user_transform_function;
    VIO_User_transform_function     user_inverse_transform_function;

    /* --- concatenated transform */

    int                         n_transforms;
    struct VIO_General_transform    *transforms;

} VIO_General_transform;

#if !VIO_PREFIX_NAMES
typedef VIO_Transform_types Transform_types;
typedef VIO_General_transform General_transform;
typedef VIO_User_transform_function User_transform_function;
#endif /* !VIO_PREFIX_NAMES */

#endif
