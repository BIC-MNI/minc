#include  <def_mni.h>

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

private  void  alloc_linear_transform(
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

public  void  create_linear_transform(
    General_transform   *transform,
    Transform           *linear_transform )
{
    alloc_linear_transform( transform );

    if( linear_transform != (Transform *) NULL )
    {
        *(transform->linear_transform) = *linear_transform;
        compute_transform_inverse( linear_transform,
                                   transform->inverse_linear_transform );
    }
    else
    {
        make_identity_transform( transform->linear_transform );
        make_identity_transform( transform->inverse_linear_transform );
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
@DESCRIPTION: Creates a general transform of type thin plate spline.  Note that
              the points and displacements pointers are copied to the
              general transform and should therefore not be free'd by the
              caller.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  create_thin_plate_transform(
    General_transform    *transform,
    int                  n_dimensions,
    int                  n_points,
    float                **points,
    float                **displacements )
{
    transform->type = THIN_PLATE_SPLINE;
    transform->inverse_flag = FALSE;
    transform->n_dimensions = n_dimensions;
    transform->n_points = n_points;
    transform->points = points;
    transform->displacements = displacements;
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

public  void  create_user_transform(
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

public  Transform_types  get_transform_type(
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

public  int  get_n_concated_transforms(
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

public  General_transform  *get_nth_general_transform(
    General_transform   *transform,
    int                 n )
{
    if( n < 0 || n >= get_n_concated_transforms( transform ) )
    {
        HANDLE_INTERNAL_ERROR( "get_nth_general_transform" );
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

public  Transform  *get_linear_transform_ptr(
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
        HANDLE_INTERNAL_ERROR( "get_linear_transform_ptr" );
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

public  Transform  *get_inverse_linear_transform_ptr(
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
        HANDLE_INTERNAL_ERROR( "get_inverse_linear_transform_ptr" );
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
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  void  transform_or_invert_point(
    General_transform   *transform,
    Boolean             inverse_flag,
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

    case USER_TRANSFORM:
        if( inverse_flag )
        {
            transform->user_inverse_transform_function(
                           transform->user_data, x, y, z,
                           x_transformed, y_transformed, z_transformed );
        }
        else
        {
            transform->user_transform_function(
                           transform->user_data, x, y, z,
                           x_transformed, y_transformed, z_transformed );
        }
        break;

    case CONCATENATED_TRANSFORM:
        *x_transformed = x;
        *y_transformed = y;
        *z_transformed = z;

        if( transform->inverse_flag )
            inverse_flag = !inverse_flag;

        if( inverse_flag )
        {
            for( trans = transform->n_transforms-1;  trans >= 0;  --trans )
            {
                general_inverse_transform_point( &transform->transforms[trans],
                             *x_transformed, *y_transformed, *z_transformed,
                             x_transformed, y_transformed, z_transformed );
            }
        }
        else
        {
            for_less( trans, 0, transform->n_transforms )
            {
                general_transform_point( &transform->transforms[trans],
                             *x_transformed, *y_transformed, *z_transformed,
                             x_transformed, y_transformed, z_transformed );
            }
        }
        break;

    default:
        HANDLE_INTERNAL_ERROR( "general_transform_point" );
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

public  void  general_transform_point(
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

public  void  general_inverse_transform_point(
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
---------------------------------------------------------------------------- */

private  void  copy_and_invert_transform(
    General_transform   *transform,
    Boolean             invert_it,
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
        HANDLE_INTERNAL_ERROR( "copy_and_invert_transform" );
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

public  void  copy_general_transform(
    General_transform   *transform,
    General_transform   *copy )
{
    copy_and_invert_transform( transform, FALSE, copy );
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

public  void  create_inverse_general_transform(
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

public  void  concat_general_transforms(
    General_transform   *first,
    General_transform   *second,
    General_transform   *result )
{
    int                  first_start, first_end, first_step;
    int                  second_start, second_end, second_step;
    int                  i, trans;
    Boolean              crunching_linear;
    General_transform    result_tmp, *result_ptr;
    General_transform    *transform;

    if( result == first || result == second )
        result_ptr = &result_tmp;
    else
        result_ptr = result;

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
                                   first->inverse_flag,
                                   get_nth_general_transform(result_ptr,trans));
        ++trans;
    }

    if( crunching_linear )
    {
        transform = get_nth_general_transform( result_ptr, trans );
        alloc_linear_transform( transform );

        concat_transforms( get_linear_transform_ptr(transform),
          get_linear_transform_ptr(
             get_nth_general_transform(first,first_end+first_step)),
          get_linear_transform_ptr(
             get_nth_general_transform(second,second_start-second_step)) );

        concat_transforms( get_inverse_linear_transform_ptr(transform),
          get_inverse_linear_transform_ptr(
             get_nth_general_transform(second,second_start-second_step)),
          get_inverse_linear_transform_ptr(
             get_nth_general_transform(first,first_end+first_step)) );

        ++trans;
    }

    for( i = second_start;  i != second_end + second_step;  i += second_step )
    {
        copy_and_invert_transform( get_nth_general_transform( second, i ),
                                   second->inverse_flag,
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
---------------------------------------------------------------------------- */

public  void  delete_general_transform(
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
        HANDLE_INTERNAL_ERROR( "delete_general_transform" );
        break;
    }
}
