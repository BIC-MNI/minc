#ifndef  DEF_TAG_IO
#define  DEF_TAG_IO

#ifndef lint
static char transforms_rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Include/volume_io/transforms.h,v 1.9 1995-03-21 19:01:42 david Exp $";
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
    float                       **points;
    float                       **displacements;   /* n_points + n_dim + 1 by */
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
