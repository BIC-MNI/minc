#ifndef  DEF_TAG_IO
#define  DEF_TAG_IO

typedef enum { LINEAR6, LINEAR7, LINEAR9, LINEAR12, BOOKSTEIN } Transform_types;

typedef  struct
{
    Transform_types    type;
    Boolean            inverse_flag;

    /* linear transform */

    Transform          linear_transform;
    Transform          inverse_linear_transform;

    /* non-linear transform */

    int                n_points;
    int                n_dimensions;
    float              **points;
    float              **displacements; /* n_points + n_dim + 1 by n_dim */
} General_transform;

#endif
