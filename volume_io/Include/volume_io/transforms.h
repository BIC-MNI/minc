#ifndef  DEF_TAG_IO
#define  DEF_TAG_IO

typedef enum { LINEAR, THIN_PLATE_SPLINE, USER_TRANSFORM,
               CONCATENATED_TRANSFORM }
               Transform_types;

typedef  void   (*User_transform_function)( void  *user_data,
                                            Real  x,
                                            Real  y,
                                            Real  z,
                                            Real  *x_trans,
                                            Real  *y_trans,
                                            Real  *z_trans );

typedef struct General_transform
{
    Transform_types             type;
    Boolean                     inverse_flag;

    /* --- linear transform */

    Transform                   *linear_transform;
    Transform                   *inverse_linear_transform;

    /* --- non-linear transform */

    int                         n_points;
    int                         n_dimensions;
    float                       **points;
    float                       **displacements;   /* n_points + n_dim + 1 by */
                                                   /* n_dim */

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
