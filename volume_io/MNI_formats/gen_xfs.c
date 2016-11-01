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

#include  <internal_volume_io.h>

/* ----------------------------- MNI Header -----------------------------------
@NAME       : alloc_linear_transform
@INPUT      : transform
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Allocates memory for the linear transform and its inverse.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

static  void  alloc_linear_transform(
    General_transform   *transform )
{
    transform->type = LINEAR;
    transform->inverse_flag = FALSE;

    ALLOC( transform->linear_transform, 1 );
    ALLOC( transform->inverse_linear_transform, 1 );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_linear_transform
@INPUT      : linear_transform
@OUTPUT     : transform
@RETURNS    : 
@DESCRIPTION: Creates a general transform of type linear, copying the
              linear_transform and computing its inverse.  If the linear
              transform is NULL, the identity transform is created.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

VIOAPI  void  create_linear_transform(
    General_transform   *transform,
    Transform           *linear_transform )
{
    alloc_linear_transform( transform );

    if( linear_transform != (Transform *) NULL &&
        compute_transform_inverse( linear_transform,
                                   transform->inverse_linear_transform ) )
    {
        *(transform->linear_transform) = *linear_transform;
    }
    else
    {
        make_identity_transform( transform->linear_transform );
        make_identity_transform( transform->inverse_linear_transform );
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : initialize_thin_plate_transform
@INPUT      : transform
              n_dimensions
              n_points
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Initializes a General_transform structure for thin plate
              transforms.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Feb. 21, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

static  void  initialize_thin_plate_transform(
    General_transform    *transform,
    int                  n_dimensions,
    int                  n_points )
{
    transform->type = THIN_PLATE_SPLINE;
    transform->inverse_flag = FALSE;
    transform->n_dimensions = n_dimensions;
    transform->n_points = n_points;

    ALLOC2D( transform->points, n_points, n_dimensions );
    ALLOC2D( transform->displacements, n_points + n_dimensions + 1,
             n_dimensions );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_thin_plate_transform_real
@INPUT      : n_dimensions
              n_points
              points
              displacements
@OUTPUT     : transform
@RETURNS    : 
@DESCRIPTION: Creates a general transform of type thin plate spline, given
              Real-type points and displacements.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : Feb. 21, 1995   David MacDonald - make a real and float version
---------------------------------------------------------------------------- */

VIOAPI  void  create_thin_plate_transform_real(
    General_transform    *transform,
    int                  n_dimensions,
    int                  n_points,
    Real                 **points,
    Real                 **displacements )
{
    int    p, d;

    initialize_thin_plate_transform( transform, n_dimensions, n_points );

    for_less( p, 0, n_points )
    {
        for_less( d, 0, n_dimensions )
            transform->points[p][d] = points[p][d];
    }

    for_less( p, 0, n_points + n_dimensions + 1 )
    {
        for_less( d, 0, n_dimensions )
            transform->displacements[p][d] = displacements[p][d];
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_thin_plate_transform
@INPUT      : n_dimensions
              n_points
              points
              displacements
@OUTPUT     : transform
@RETURNS    : 
@DESCRIPTION: Creates a general transform of type thin plate spline.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : Feb. 21, 1995   David MacDonald - make a real and float version
---------------------------------------------------------------------------- */

VIOAPI  void  create_thin_plate_transform(
    General_transform    *transform,
    int                  n_dimensions,
    int                  n_points,
    float                **points,
    float                **displacements )
{
    int    p, d;

    initialize_thin_plate_transform( transform, n_dimensions, n_points );

    for_less( p, 0, n_points )
    {
        for_less( d, 0, n_dimensions )
            transform->points[p][d] = (Real) points[p][d];
    }

    for_less( p, 0, n_points + n_dimensions + 1 )
    {
        for_less( d, 0, n_dimensions )
            transform->displacements[p][d] = (Real) displacements[p][d];
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : internal_create_grid_transform
@INPUT      : displacement_volume
@OUTPUT     : transform
@RETURNS    : 
@DESCRIPTION: Creates a general transform of type grid.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Feb. 21, 1995            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

static  void  internal_create_grid_transform(
    General_transform    *transform,
    Volume               displacement_volume,
    BOOLEAN              copy_flag )
{
    int       dim, sizes[MAX_DIMENSIONS], vector_dim;
    STRING    *dim_names;
    Volume    copy;
    BOOLEAN   volume_ok, dim_found[N_DIMENSIONS];

    volume_ok = TRUE;
    if( get_volume_n_dimensions(displacement_volume) != 4 )
    {
        volume_ok = FALSE;
        print_error( "Grid transform must be 4 dimensional.\n" );
    }
    else
    {
        dim_names = get_volume_dimension_names( displacement_volume );
        get_volume_sizes( displacement_volume, sizes );

        dim_found[X] = FALSE;
        dim_found[Y] = FALSE;
        dim_found[Z] = FALSE;
        vector_dim = -1;

        for_less( dim, 0, 4 )
        {
            if( equal_strings( dim_names[dim], MIxspace ) )
                dim_found[X] = TRUE;
            else if( equal_strings( dim_names[dim], MIyspace ) )
                dim_found[Y] = TRUE;
            else if( equal_strings( dim_names[dim], MIzspace ) )
                dim_found[Z] = TRUE;
            else
            {
                if( sizes[dim] != 3 )
                {
                    print_error(
                            "displacement_volume must have 3 components on " );
                    print_error( "the non-spatial axis.\n" );
                    volume_ok = FALSE;
                }

                vector_dim = dim;
            }
        }

        if( !dim_found[X] || !dim_found[Y] || !dim_found[Z] )
        {
            print_error(
              "Must have an x, y, and z dimension in displacement volume.\n" );
            volume_ok = FALSE;
        }

        delete_dimension_names( displacement_volume, dim_names );
    }

    if( !volume_ok )
    {
        create_linear_transform( transform, NULL );  /*--- make identity */
        return;
    }

    transform->type = GRID_TRANSFORM;
    transform->inverse_flag = FALSE;

    if( copy_flag )
        copy = copy_volume( displacement_volume );
    else
        copy = displacement_volume;

    /* --- force 4th dimension to be vector dimension */

    replace_string( &copy->dimension_names[vector_dim],
                    create_string(MIvector_dimension) );

    transform->displacement_volume = (void *) copy;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_grid_transform
@INPUT      : displacement_volume
@OUTPUT     : transform
@RETURNS    : 
@DESCRIPTION: Creates a grid transform General_transform.  Makes a copy of
              the displacement volume and puts it in the General_transform.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Feb. 21, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

VIOAPI  void  create_grid_transform(
    General_transform    *transform,
    Volume               displacement_volume )
{
    internal_create_grid_transform( transform, displacement_volume, TRUE );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_grid_transform_no_copy
@INPUT      : displacement_volume
@OUTPUT     : transform
@RETURNS    : 
@DESCRIPTION: Creates a grid transform General_transform.  Places the
              displacement volume into the General_transform; therefore the
              calling program should not delete the volume.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Feb. 21, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

VIOAPI  void  create_grid_transform_no_copy(
    General_transform    *transform,
    Volume               displacement_volume )
{
    internal_create_grid_transform( transform, displacement_volume, FALSE );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_user_transform
@INPUT      : user_data
              size_user_data
              transform_function
              inverse_transform_function
@OUTPUT     : transform
@RETURNS    : 
@DESCRIPTION: Creates a general transform of type user transform.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

VIOAPI  void  create_user_transform(
    General_transform         *transform,
    void                      *user_data,
    size_t                    size_user_data,
    User_transform_function   transform_function,
    User_transform_function   inverse_transform_function )
{
    unsigned char  *byte_ptr;

    transform->type = USER_TRANSFORM;
    transform->inverse_flag = FALSE;

    transform->size_user_data = size_user_data;
    ALLOC( byte_ptr, size_user_data );
    transform->user_data = byte_ptr;
    (void) memcpy( transform->user_data, user_data, size_user_data );
    transform->user_transform_function = transform_function;
    transform->user_inverse_transform_function = inverse_transform_function;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_transform_type
@INPUT      : transform
@OUTPUT     : 
@RETURNS    : type
@DESCRIPTION: Returns the type of the general transform.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

VIOAPI  Transform_types  get_transform_type(
    General_transform   *transform )
{
    return( transform->type );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_n_concated_transforms
@INPUT      : transform
@OUTPUT     : 
@RETURNS    : # transforms
@DESCRIPTION: Returns the number of concatenated transforms if the transform
              type is concatenated, otherwise, 1.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

VIOAPI  int  get_n_concated_transforms(
    General_transform   *transform )
{
    if( transform->type == CONCATENATED_TRANSFORM )
        return( transform->n_transforms );
    else
        return( 1 );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_nth_general_transform
@INPUT      : transform
              n
@OUTPUT     : 
@RETURNS    : pointer to nth transform
@DESCRIPTION: Returns a pointer to the nth transform of the general transform.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

VIOAPI  General_transform  *get_nth_general_transform(
    General_transform   *transform,
    int                 n )
{
    if( n < 0 || n >= get_n_concated_transforms( transform ) )
    {
        handle_internal_error( "get_nth_general_transform" );
        return( (General_transform *) NULL );
    }
    else if( transform->type == CONCATENATED_TRANSFORM )
        return( &transform->transforms[n] );
    else
        return( transform );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_linear_transform_ptr
@INPUT      : transform
@OUTPUT     : 
@RETURNS    : pointer to linear transform
@DESCRIPTION: Returns a pointer to the linear transform of the general
              transform.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

VIOAPI  Transform  *get_linear_transform_ptr(
    General_transform   *transform )
{
    if( transform->type == LINEAR )
    {
        if( transform->inverse_flag )
            return( transform->inverse_linear_transform );
        else
            return( transform->linear_transform );
    }
    else
    {
        handle_internal_error( "get_linear_transform_ptr" );
        return( (Transform *) NULL );
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_inverse_linear_transform_ptr
@INPUT      : transform
@OUTPUT     : 
@RETURNS    : pointer to inverse linear transform
@DESCRIPTION: Returns a pointer to the inverse linear transform of the general
              transform.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

VIOAPI  Transform  *get_inverse_linear_transform_ptr(
    General_transform   *transform )
{
    if( transform->type == LINEAR )
    {
        if( transform->inverse_flag )
            return( transform->linear_transform );
        else
            return( transform->inverse_linear_transform );
    }
    else
    {
        handle_internal_error( "get_inverse_linear_transform_ptr" );
        return( (Transform *) NULL );
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : transform_or_invert_point
@INPUT      : transform
              inverse_flag
              x
              y
              z
@OUTPUT     : x_transformed
              y_transformed
              z_transformed
@RETURNS    : 
@DESCRIPTION: Transforms a point by the general transform or its inverse,
              depending on inverse_flag.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : Feb. 27, 1995   D. MacDonald  - added grid transforms
---------------------------------------------------------------------------- */

static  void  transform_or_invert_point(
    General_transform   *transform,
    BOOLEAN             inverse_flag,
    Real                x,
    Real                y,
    Real                z,
    Real                *x_transformed,
    Real                *y_transformed,
    Real                *z_transformed )
{
    int      trans;

    switch( transform->type )
    {
    case LINEAR:
        if( inverse_flag )
            transform_point( transform->inverse_linear_transform,
                             x, y, z,
                             x_transformed, y_transformed, z_transformed );
        else
            transform_point( transform->linear_transform,
                             x, y, z,
                             x_transformed, y_transformed, z_transformed );
        break;

    case THIN_PLATE_SPLINE:
        if( inverse_flag )
        {
            thin_plate_spline_inverse_transform( transform->n_dimensions,
                                                 transform->n_points,
                                                 transform->points,
                                                 transform->displacements,
                                                 x, y, z,
                                                 x_transformed, y_transformed,
                                                 z_transformed );
        }
        else
        {
            thin_plate_spline_transform( transform->n_dimensions,
                                         transform->n_points,
                                         transform->points,
                                         transform->displacements,
                                         x, y, z,
                                         x_transformed, y_transformed,
                                         z_transformed );
        }
        break;

    case GRID_TRANSFORM:
        if( inverse_flag ) {
            grid_inverse_transform_point( transform,
                                          x, y, z,
                                          x_transformed, y_transformed,
                                          z_transformed );
        } else {
            grid_transform_point( transform,
                                  x, y, z,
                                  x_transformed, y_transformed,
                                  z_transformed );
        }
        break;

    case USER_TRANSFORM:
        if( inverse_flag ) {
            transform->user_inverse_transform_function(
                           transform->user_data, x, y, z,
                           x_transformed, y_transformed, z_transformed );
        } else {
            transform->user_transform_function(
                           transform->user_data, x, y, z,
                           x_transformed, y_transformed, z_transformed );
        }
        break;

    case CONCATENATED_TRANSFORM:
        *x_transformed = x;
        *y_transformed = y;
        *z_transformed = z;

        if( inverse_flag ) {
            for( trans = transform->n_transforms-1;  trans >= 0;  --trans ) {
                general_inverse_transform_point( &transform->transforms[trans],
                             *x_transformed, *y_transformed, *z_transformed,
                             x_transformed, y_transformed, z_transformed );
            }
        } else {
            for_less( trans, 0, transform->n_transforms ) {
                general_transform_point( &transform->transforms[trans],
                             *x_transformed, *y_transformed, *z_transformed,
                             x_transformed, y_transformed, z_transformed );
            }
        }
        break;

    default:
        handle_internal_error( "transform_or_invert_point" );
        break;
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : general_transform_point
@INPUT      : transform
              x
              y
              z
@OUTPUT     : x_transformed
              y_transformed
              z_transformed
@RETURNS    : 
@DESCRIPTION: Transforms a point by the general transform.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

VIOAPI  void  general_transform_point(
    General_transform   *transform,
    Real                x,
    Real                y,
    Real                z,
    Real                *x_transformed,
    Real                *y_transformed,
    Real                *z_transformed )
{

    transform_or_invert_point( transform, transform->inverse_flag, x, y, z,
                               x_transformed, y_transformed, z_transformed );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : general_inverse_transform_point
@INPUT      : transform
              x
              y
              z
@OUTPUT     : x_transformed
              y_transformed
              z_transformed
@RETURNS    : 
@DESCRIPTION: Transforms a point by the inverse of the general transform.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

VIOAPI  void  general_inverse_transform_point(
    General_transform   *transform,
    Real                x,
    Real                y,
    Real                z,
    Real                *x_transformed,
    Real                *y_transformed,
    Real                *z_transformed )
{

    transform_or_invert_point( transform, !transform->inverse_flag, x, y, z,
                               x_transformed, y_transformed, z_transformed );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : copy_and_invert_transform
@INPUT      : transform
              invert_it
@OUTPUT     : copy
@RETURNS    : 
@DESCRIPTION: Copies the transform or its inverse to copy.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
@MODIFIED   : Feb. 27, 1995   D. MacDonald  - added grid transforms
---------------------------------------------------------------------------- */

static  void  copy_and_invert_transform(
    General_transform   *transform,
    BOOLEAN             invert_it,
    General_transform   *copy )
{
    unsigned char  *byte_ptr;
    Transform      *swap;
    int            i, j, trans;

    *copy = *transform;

    switch( transform->type )
    {
    case LINEAR:
        alloc_linear_transform( copy );
        *(copy->linear_transform) = *(transform->linear_transform);
        *(copy->inverse_linear_transform) =
                                       *(transform->inverse_linear_transform);

        if( transform->inverse_flag )
            invert_it = !invert_it;

        if( invert_it )
        {
            swap = copy->linear_transform;
            copy->linear_transform = copy->inverse_linear_transform;
            copy->inverse_linear_transform = swap;
        }
        copy->inverse_flag = FALSE;
        break;

    case THIN_PLATE_SPLINE:
        ALLOC2D( copy->points, copy->n_points, copy->n_dimensions);
        ALLOC2D( copy->displacements, copy->n_points + copy->n_dimensions + 1,
                 copy->n_dimensions);

        for_less( i, 0, copy->n_points )
            for_less( j, 0, copy->n_dimensions )
                copy->points[i][j] = transform->points[i][j];

        for_less( i, 0, copy->n_points + copy->n_dimensions + 1 )
            for_less( j, 0, copy->n_dimensions )
                copy->displacements[i][j] = transform->displacements[i][j];

        if( invert_it )
            copy->inverse_flag = !copy->inverse_flag;
        break;

    case GRID_TRANSFORM:
        copy->displacement_volume = (void *) copy_volume(
                                    (Volume) transform->displacement_volume );

        if( invert_it )
            copy->inverse_flag = !copy->inverse_flag;

        break;

    case USER_TRANSFORM:
        ALLOC( byte_ptr, copy->size_user_data );
        copy->user_data = byte_ptr;
        (void) memcpy( copy->user_data, transform->user_data,
                       copy->size_user_data );
        if( invert_it )
            copy->inverse_flag = !copy->inverse_flag;
        break;

    case CONCATENATED_TRANSFORM:
        ALLOC( copy->transforms, copy->n_transforms );
        for_less( trans, 0, copy->n_transforms )
        {
            copy_general_transform( &transform->transforms[trans],
                                    &copy->transforms[trans] );
        }
        if( invert_it )
            copy->inverse_flag = !copy->inverse_flag;
        break;

    default:
        handle_internal_error( "copy_and_invert_transform" );
        break;
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : copy_general_transform
@INPUT      : transform
@OUTPUT     : copy
@RETURNS    : 
@DESCRIPTION: Copies the general transform to copy.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

VIOAPI  void  copy_general_transform(
    General_transform   *transform,
    General_transform   *copy )
{
    copy_and_invert_transform( transform, FALSE, copy );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : invert_general_transform
@INPUT      : transform
@OUTPUT     : transform
@RETURNS    : 
@DESCRIPTION: Inverts a general transform in place.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Nov. 20, 1995   David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

VIOAPI  void  invert_general_transform(
    General_transform   *transform )
{
    transform->inverse_flag = !transform->inverse_flag;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_inverse_general_transform
@INPUT      : transform
@OUTPUT     : inverse
@RETURNS    : 
@DESCRIPTION: Creates a general transform that is the inverse of the given one.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

VIOAPI  void  create_inverse_general_transform(
    General_transform   *transform,
    General_transform   *inverse )
{
    copy_and_invert_transform( transform, TRUE, inverse );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : concat_general_transforms
@INPUT      : first
              second
@OUTPUT     : result
@RETURNS    : 
@DESCRIPTION: Concatenates two general transforms into result.  Transforming
              a point by result is the same as transforming it by 'first',
              then transforming by 'second'.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

VIOAPI  void  concat_general_transforms(
    General_transform   *first,
    General_transform   *second,
    General_transform   *result )
{
    int                  first_start, first_end, first_step;
    int                  second_start, second_end, second_step;
    int                  i, trans;
    BOOLEAN              crunching_linear;
    BOOLEAN              first_inverted_concat, second_inverted_concat;
    Transform            *first_transform, *first_inverse;
    Transform            *second_transform, *second_inverse;
    General_transform    result_tmp, *result_ptr;
    General_transform    *transform;

    if( result == first || result == second )
        result_ptr = &result_tmp;
    else
        result_ptr = result;
    

    first_inverted_concat = first->type == CONCATENATED_TRANSFORM &&
                            first->inverse_flag;
    second_inverted_concat = second->type == CONCATENATED_TRANSFORM &&
                             second->inverse_flag;

    if( first->inverse_flag )
    {
        first_start = get_n_concated_transforms( first ) - 1;
        first_end = 0;
        first_step = -1;
    }
    else
    {
        first_start = 0;
        first_end = get_n_concated_transforms( first ) - 1;
        first_step = 1;
    }

    if( second->inverse_flag )
    {
        second_start = get_n_concated_transforms( second ) - 1;
        second_end = 0;
        second_step = -1;
    }
    else
    {
        second_start = 0;
        second_end = get_n_concated_transforms( second ) - 1;
        second_step = 1;
    }

    result_ptr->n_transforms = ABS( first_end - first_start ) + 1 +
                               ABS( second_end - second_start ) + 1;

    crunching_linear = FALSE;
    if( get_nth_general_transform( first, first_end )->type == LINEAR &&
        get_nth_general_transform( second, second_start )->type == LINEAR )
    {
        --result_ptr->n_transforms;
        crunching_linear = TRUE;
        first_end -= first_step;
        second_start += second_step;
    }

    if( result_ptr->n_transforms == 1 )
        result_ptr->type = LINEAR;
    else
    {
        result_ptr->type = CONCATENATED_TRANSFORM;
        ALLOC( result_ptr->transforms, result_ptr->n_transforms );
    }

    result_ptr->inverse_flag = FALSE;

    trans = 0;
    for( i = first_start;  i != first_end + first_step;  i += first_step )
    {
        copy_and_invert_transform( get_nth_general_transform( first, i ),
                                   first_inverted_concat,
                                   get_nth_general_transform(result_ptr,trans));
        ++trans;
    }

    if( crunching_linear )
    {
        transform = get_nth_general_transform( result_ptr, trans );
        alloc_linear_transform( transform );

        if( first_inverted_concat )
        {
            first_inverse = get_linear_transform_ptr(
                      get_nth_general_transform(first,first_end+first_step));
            first_transform = get_inverse_linear_transform_ptr(
                      get_nth_general_transform(first,first_end+first_step));
        }
        else
        {
            first_transform = get_linear_transform_ptr(
                      get_nth_general_transform(first,first_end+first_step));
            first_inverse = get_inverse_linear_transform_ptr(
                      get_nth_general_transform(first,first_end+first_step));
        }

        if( second_inverted_concat )
        {
            second_inverse = get_linear_transform_ptr(
                   get_nth_general_transform(second,second_start-second_step));
            second_transform = get_inverse_linear_transform_ptr(
                   get_nth_general_transform(second,second_start-second_step));
        }
        else
        {
            second_transform = get_linear_transform_ptr(
                   get_nth_general_transform(second,second_start-second_step));
            second_inverse = get_inverse_linear_transform_ptr(
                   get_nth_general_transform(second,second_start-second_step));
        }

        concat_transforms( get_linear_transform_ptr(transform),
                           first_transform, second_transform );

        concat_transforms( get_inverse_linear_transform_ptr(transform),
                           second_inverse, first_inverse );

        ++trans;
    }

    for( i = second_start;  i != second_end + second_step;  i += second_step )
    {
        copy_and_invert_transform( get_nth_general_transform( second, i ),
                                   second_inverted_concat,
                                   get_nth_general_transform(result_ptr,trans));
        ++trans;
    }

    if( result == first || result == second )
        *result = *result_ptr;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : delete_general_transform
@INPUT      : transform
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Deletes the transform, freeing up memory.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
@MODIFIED   : Feb. 27, 1995   D. MacDonald  - added grid transforms
---------------------------------------------------------------------------- */

VIOAPI  void  delete_general_transform(
    General_transform   *transform )
{
    int   trans;

    switch( transform->type )
    {
    case LINEAR:
        FREE( transform->linear_transform );
        FREE( transform->inverse_linear_transform );
        break;

    case THIN_PLATE_SPLINE:
        if( transform->n_points > 0 && transform->n_dimensions > 0 )
        {
            FREE2D( transform->points );
            FREE2D( transform->displacements );
        }
        break;

    case GRID_TRANSFORM:
        delete_volume( (Volume) transform->displacement_volume );
        break;

    case USER_TRANSFORM:
        if( transform->size_user_data )
            FREE( transform->user_data );
        break;

    case CONCATENATED_TRANSFORM:
        for_less( trans, 0, transform->n_transforms )
            delete_general_transform( &transform->transforms[trans] );

        if( transform->n_transforms > 0 )
            FREE( transform->transforms );
        break;

    default:
        handle_internal_error( "delete_general_transform" );
        break;
    }
}
