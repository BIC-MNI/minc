#include  <def_mni.h>

public  Transform_types  get_transform_type(
    General_transform   *transform )
{
    return( transform->type );
}

public  void  create_linear_transform(
    General_transform   *transform,
    Transform           *linear_transform )
{
    transform->type = LINEAR;
    transform->inverse_flag = FALSE;

    if( linear_transform != (Transform *) NULL )
    {
        transform->linear_transform = *linear_transform;
        compute_transform_inverse( linear_transform,
                                   &transform->inverse_linear_transform );
    }
    else
    {
        make_identity_transform( &transform->linear_transform );
        make_identity_transform( &transform->inverse_linear_transform );
    }
}

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

public  Transform  *get_linear_transform_ptr(
    General_transform   *transform )
{
    if( transform->type == LINEAR )
    {
        return( &transform->linear_transform );
    }
    else
    {
        HANDLE_INTERNAL_ERROR( "get_linear_transform_ptr" );
        return( (Transform *) NULL );
    }
}

public  void  general_transform_point(
    General_transform   *transform,
    Real                x,
    Real                y,
    Real                z,
    Real                *x_transformed,
    Real                *y_transformed,
    Real                *z_transformed )
{
    Point    point;

    switch( transform->type )
    {
    case LINEAR:
        Point_x(point) = x;
        Point_y(point) = y;
        Point_z(point) = z;
        if( transform->inverse_flag )
            transform_point( &transform->inverse_linear_transform,
                             &point, &point );
        else
            transform_point( &transform->linear_transform, &point, &point );

        *x_transformed = Point_x(point);
        *y_transformed = Point_y(point);
        *z_transformed = Point_z(point);
        break;

    case THIN_PLATE_SPLINE:
        if( transform->inverse_flag )
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
    }
}

public  void  general_inverse_transform_point(
    General_transform   *transform,
    Real                x,
    Real                y,
    Real                z,
    Real                *x_transformed,
    Real                *y_transformed,
    Real                *z_transformed )
{
    Point    point;

    switch( transform->type )
    {
    case LINEAR:
        Point_x(point) = x;
        Point_y(point) = y;
        Point_z(point) = z;
        if( transform->inverse_flag )
            transform_point( &transform->linear_transform,
                             &point, &point );
        else
            transform_point( &transform->inverse_linear_transform,
                             &point, &point );

        *x_transformed = Point_x(point);
        *y_transformed = Point_y(point);
        *z_transformed = Point_z(point);
        break;

    case THIN_PLATE_SPLINE:
        if( transform->inverse_flag )
        {
            thin_plate_spline_transform( transform->n_dimensions,
                                         transform->n_points,
                                         transform->points,
                                         transform->displacements,
                                         x, y, z,
                                         x_transformed, y_transformed,
                                         z_transformed );
        }
        else
        {
            thin_plate_spline_inverse_transform( transform->n_dimensions,
                                                 transform->n_points,
                                                 transform->points,
                                                 transform->displacements,
                                                 x, y, z,
                                                 x_transformed, y_transformed,
                                                 z_transformed );
        }
        break;
    }
}

public  void  create_inverse_general_transform(
    General_transform   *transform,
    General_transform   *inverse )
{
    int   i, j;

    switch( transform->type )
    {
    case LINEAR:
        *inverse = *transform;

        if( inverse->inverse_flag )
            inverse->inverse_flag = FALSE;
        else
        {
            inverse->linear_transform = transform->inverse_linear_transform;
            inverse->inverse_linear_transform = transform->linear_transform;
        }
        break;

    case THIN_PLATE_SPLINE:
        *inverse = *transform;

        ALLOC2D( inverse->points, transform->n_points, transform->n_dimensions);
        ALLOC2D( inverse->displacements,
                 transform->n_points + transform->n_dimensions + 1,
                 transform->n_dimensions);

        for_less( i, 0, transform->n_points )
            for_less( j, 0, transform->n_dimensions )
                inverse->points[i][j] = transform->points[i][j];

        for_less( i, 0, transform->n_points + transform->n_dimensions + 1 )
            for_less( j, 0, transform->n_dimensions )
                inverse->displacements[i][j] = transform->displacements[i][j];

        inverse->inverse_flag = !transform->inverse_flag;
        break;
    }
}
