
#include  <volume_io.h>

public  void  make_identity_transform( Transform   *transform )
{
    Transform_elem( *transform, 0, 0 ) = 1.0;
    Transform_elem( *transform, 0, 1 ) = 0.0;
    Transform_elem( *transform, 0, 2 ) = 0.0;
    Transform_elem( *transform, 0, 3 ) = 0.0;
    Transform_elem( *transform, 1, 0 ) = 0.0;
    Transform_elem( *transform, 1, 1 ) = 1.0;
    Transform_elem( *transform, 1, 2 ) = 0.0;
    Transform_elem( *transform, 1, 3 ) = 0.0;
    Transform_elem( *transform, 2, 0 ) = 0.0;
    Transform_elem( *transform, 2, 1 ) = 0.0;
    Transform_elem( *transform, 2, 2 ) = 1.0;
    Transform_elem( *transform, 2, 3 ) = 0.0;
    Transform_elem( *transform, 3, 0 ) = 0.0;
    Transform_elem( *transform, 3, 1 ) = 0.0;
    Transform_elem( *transform, 3, 2 ) = 0.0;
    Transform_elem( *transform, 3, 3 ) = 1.0;
}

#define   TOLERANCE   0.001

public  BOOLEAN  close_to_identity(
    Transform   *transform )
{
    BOOLEAN    close;
    Real       expected_val;
    int        i, j;

    close = TRUE;

    for_less( i, 0, 4 )
    {
        for_less( j, 0, 4 )
        {
            if( i == j )
                expected_val = 1.0;
            else
                expected_val = 0.0;

            if( Transform_elem(*transform,i,j) < expected_val - TOLERANCE ||
                Transform_elem(*transform,i,j) > expected_val + TOLERANCE )
            {
                close = FALSE;
            }
        }
    }

    return( close );
}

public  void  make_scale_transform( 
    Real        sx,
    Real        sy,
    Real        sz,
    Transform   *transform )
{
    make_identity_transform( transform );

    Transform_elem( *transform, 0, 0 ) = sx;
    Transform_elem( *transform, 1, 1 ) = sy;
    Transform_elem( *transform, 2, 2 ) = sz;
}

public  void  get_transform_origin(
    Transform   *transform,
    Point       *origin )
{
    fill_Point( *origin,
                Transform_elem(*transform,0,3),
                Transform_elem(*transform,1,3),
                Transform_elem(*transform,2,3) );
}

public  void  set_transform_origin(
    Transform   *transform,
    Point       *origin )
{
    Transform_elem(*transform,0,3) = Point_x(*origin);
    Transform_elem(*transform,1,3) = Point_y(*origin);
    Transform_elem(*transform,2,3) = Point_z(*origin);
}

public  void  get_transform_x_axis(
    Transform   *transform,
    Vector      *x_axis )
{
    fill_Vector( *x_axis,
                 Transform_elem(*transform,0,0),
                 Transform_elem(*transform,1,0),
                 Transform_elem(*transform,2,0) );
}

public  void  set_transform_x_axis(
    Transform   *transform,
    Vector      *x_axis )
{
    Transform_elem(*transform,0,0) = Vector_x(*x_axis);
    Transform_elem(*transform,1,0) = Vector_y(*x_axis);
    Transform_elem(*transform,2,0) = Vector_z(*x_axis);
}

public  void  get_transform_y_axis(
    Transform   *transform,
    Vector      *y_axis )
{
    fill_Vector( *y_axis,
                 Transform_elem(*transform,0,1),
                 Transform_elem(*transform,1,1),
                 Transform_elem(*transform,2,1) );
}

public  void  set_transform_y_axis(
    Transform   *transform,
    Vector      *y_axis )
{
    Transform_elem(*transform,0,1) = Vector_x(*y_axis);
    Transform_elem(*transform,1,1) = Vector_y(*y_axis);
    Transform_elem(*transform,2,1) = Vector_z(*y_axis);
}

public  void  get_transform_z_axis(
    Transform   *transform,
    Vector      *z_axis )
{
    fill_Vector( *z_axis,
                 Transform_elem(*transform,0,2),
                 Transform_elem(*transform,1,2),
                 Transform_elem(*transform,2,2) );
}

public  void  set_transform_z_axis(
    Transform   *transform,
    Vector      *z_axis )
{
    Transform_elem(*transform,0,2) = Vector_x(*z_axis);
    Transform_elem(*transform,1,2) = Vector_y(*z_axis);
    Transform_elem(*transform,2,2) = Vector_z(*z_axis);
}

public  void  set_transform_x_and_z_axes(
    Transform   *transform,
    Vector      *x_axis,
    Vector      *z_axis )
{
    Vector  n_z, n_y, n_x;

    NORMALIZE_VECTOR( n_z, *z_axis );
    CROSS_VECTORS( n_y, n_z, *x_axis );
    NORMALIZE_VECTOR( n_y, n_y );
    CROSS_VECTORS( n_x, n_z, n_y );
    NORMALIZE_VECTOR( n_x, n_x );

    set_transform_x_axis( transform, &n_x );
    set_transform_y_axis( transform, &n_y );
    set_transform_z_axis( transform, &n_z );
}

public  void  make_translation_transform(
    Real        x_trans,
    Real        y_trans,
    Real        z_trans,
    Transform   *transform )
{
    Transform_elem( *transform, 0, 0 ) = 1.0;
    Transform_elem( *transform, 0, 1 ) = 0.0;
    Transform_elem( *transform, 0, 2 ) = 0.0;
    Transform_elem( *transform, 0, 3 ) = x_trans;
    Transform_elem( *transform, 1, 0 ) = 0.0;
    Transform_elem( *transform, 1, 1 ) = 1.0;
    Transform_elem( *transform, 1, 2 ) = 0.0;
    Transform_elem( *transform, 1, 3 ) = y_trans;
    Transform_elem( *transform, 2, 0 ) = 0.0;
    Transform_elem( *transform, 2, 1 ) = 0.0;
    Transform_elem( *transform, 2, 2 ) = 1.0;
    Transform_elem( *transform, 2, 3 ) = z_trans;
    Transform_elem( *transform, 3, 0 ) = 0.0;
    Transform_elem( *transform, 3, 1 ) = 0.0;
    Transform_elem( *transform, 3, 2 ) = 0.0;
    Transform_elem( *transform, 3, 3 ) = 1.0;
}

public  void  make_origin_transform(
    Point      *origin,
    Transform   *transform )
{
    Transform_elem( *transform, 0, 0 ) = 1.0;
    Transform_elem( *transform, 0, 1 ) = 0.0;
    Transform_elem( *transform, 0, 2 ) = 0.0;
    Transform_elem( *transform, 0, 3 ) = Point_x(*origin);
    Transform_elem( *transform, 1, 0 ) = 0.0;
    Transform_elem( *transform, 1, 1 ) = 1.0;
    Transform_elem( *transform, 1, 2 ) = 0.0;
    Transform_elem( *transform, 1, 3 ) = Point_y(*origin);
    Transform_elem( *transform, 2, 0 ) = 0.0;
    Transform_elem( *transform, 2, 1 ) = 0.0;
    Transform_elem( *transform, 2, 2 ) = 1.0;
    Transform_elem( *transform, 2, 3 ) = Point_z(*origin);
    Transform_elem( *transform, 3, 0 ) = 0.0;
    Transform_elem( *transform, 3, 1 ) = 0.0;
    Transform_elem( *transform, 3, 2 ) = 0.0;
    Transform_elem( *transform, 3, 3 ) = 1.0;
}

public  void   make_change_to_bases_transform(
    Point      *origin,
    Vector     *x_axis,
    Vector     *y_axis,
    Vector     *z_axis,
    Transform  *transform )
{
    Transform_elem( *transform, 0, 0 ) = Vector_x( *x_axis );
    Transform_elem( *transform, 0, 1 ) = Vector_x( *y_axis );
    Transform_elem( *transform, 0, 2 ) = Vector_x( *z_axis );
    Transform_elem( *transform, 0, 3 ) = Point_x( *origin );
    Transform_elem( *transform, 1, 0 ) = Vector_y( *x_axis );
    Transform_elem( *transform, 1, 1 ) = Vector_y( *y_axis );
    Transform_elem( *transform, 1, 2 ) = Vector_y( *z_axis );
    Transform_elem( *transform, 1, 3 ) = Point_y( *origin );
    Transform_elem( *transform, 2, 0 ) = Vector_z( *x_axis );
    Transform_elem( *transform, 2, 1 ) = Vector_z( *y_axis );
    Transform_elem( *transform, 2, 2 ) = Vector_z( *z_axis );
    Transform_elem( *transform, 2, 3 ) = Point_z( *origin );
    Transform_elem( *transform, 3, 0 ) = 0.0;
    Transform_elem( *transform, 3, 1 ) = 0.0;
    Transform_elem( *transform, 3, 2 ) = 0.0;
    Transform_elem( *transform, 3, 3 ) = 1.0;
}

public  void   make_change_from_bases_transform(
    Point      *origin,
    Vector     *x_axis,
    Vector     *y_axis,
    Vector     *z_axis,
    Transform  *transform )
{
    Transform_elem( *transform, 0, 0 ) = Vector_x( *x_axis );
    Transform_elem( *transform, 0, 1 ) = Vector_y( *x_axis );
    Transform_elem( *transform, 0, 2 ) = Vector_z( *x_axis );
    Transform_elem( *transform, 0, 3 ) = - DOT_POINT_VECTOR( *origin, *x_axis );
    Transform_elem( *transform, 1, 0 ) = Vector_x( *y_axis );
    Transform_elem( *transform, 1, 1 ) = Vector_y( *y_axis );
    Transform_elem( *transform, 1, 2 ) = Vector_z( *y_axis );
    Transform_elem( *transform, 1, 3 ) = - DOT_POINT_VECTOR( *origin, *y_axis );
    Transform_elem( *transform, 2, 0 ) = Vector_x( *z_axis );
    Transform_elem( *transform, 2, 1 ) = Vector_y( *z_axis );
    Transform_elem( *transform, 2, 2 ) = Vector_z( *z_axis );
    Transform_elem( *transform, 2, 3 ) = - DOT_POINT_VECTOR( *origin, *z_axis );
    Transform_elem( *transform, 3, 0 ) = 0.0;
    Transform_elem( *transform, 3, 1 ) = 0.0;
    Transform_elem( *transform, 3, 2 ) = 0.0;
    Transform_elem( *transform, 3, 3 ) = 1.0;
}

public  void   compute_inverse_of_orthogonal_transform(
    Transform  *transform,
    Transform  *inverse )
{
    Real   x_trans, y_trans, z_trans;

    x_trans = -Transform_elem( *transform, 0, 0 ) *
               Transform_elem( *transform, 0, 3 ) -
               Transform_elem( *transform, 0, 1 ) *
               Transform_elem( *transform, 1, 3 ) -
               Transform_elem( *transform, 0, 2 ) *
               Transform_elem( *transform, 2, 3 );

    y_trans = -Transform_elem( *transform, 1, 0 ) *
               Transform_elem( *transform, 0, 3 ) -
               Transform_elem( *transform, 1, 1 ) *
               Transform_elem( *transform, 1, 3 ) -
               Transform_elem( *transform, 1, 2 ) *
               Transform_elem( *transform, 2, 3 );

    z_trans = -Transform_elem( *transform, 2, 0 ) *
               Transform_elem( *transform, 0, 3 ) -
               Transform_elem( *transform, 2, 1 ) *
               Transform_elem( *transform, 1, 3 ) -
               Transform_elem( *transform, 2, 2 ) *
               Transform_elem( *transform, 2, 3 );

    Transform_elem( *inverse, 0, 0 ) = Transform_elem( *transform, 0, 0 );
    Transform_elem( *inverse, 0, 1 ) = Transform_elem( *transform, 1, 0 );
    Transform_elem( *inverse, 0, 2 ) = Transform_elem( *transform, 2, 0 );
    Transform_elem( *inverse, 0, 3 ) = x_trans;
    Transform_elem( *inverse, 1, 0 ) = Transform_elem( *transform, 0, 1 );
    Transform_elem( *inverse, 1, 1 ) = Transform_elem( *transform, 1, 1 );
    Transform_elem( *inverse, 1, 2 ) = Transform_elem( *transform, 2, 1 );
    Transform_elem( *inverse, 1, 3 ) = y_trans;
    Transform_elem( *inverse, 2, 0 ) = Transform_elem( *transform, 0, 2 );
    Transform_elem( *inverse, 2, 1 ) = Transform_elem( *transform, 1, 2 );
    Transform_elem( *inverse, 2, 2 ) = Transform_elem( *transform, 2, 2 );
    Transform_elem( *inverse, 2, 3 ) = z_trans;
    Transform_elem( *inverse, 3, 0 ) = 0.0;
    Transform_elem( *inverse, 3, 1 ) = 0.0;
    Transform_elem( *inverse, 3, 2 ) = 0.0;
    Transform_elem( *inverse, 3, 3 ) = 1.0;
}

public  void   concat_transforms(
    Transform   *result,
    Transform   *t1,
    Transform   *t2 )
{
    int         i, j, k;
    Real        sum;
    BOOLEAN     result_is_also_an_arg;
    Transform   tmp, *t;

    if( result == t1 || result == t2 )
    {
        result_is_also_an_arg = TRUE;
        t = &tmp;
    }
    else
    {
        result_is_also_an_arg = FALSE;
        t = result;
    }

    for_less( i, 0, 4 )
    {
        for_less( j, 0, 4 )
        {
            sum = 0.0;
            for_less( k, 0, 4 )
            {
                sum += Transform_elem( *t2, i, k ) *
                       Transform_elem( *t1, k, j );
            }
            Transform_elem( *t, i, j ) = sum;
        }
    }

    if( result_is_also_an_arg )
    {
        *result = tmp;
    }
}

public  void  make_rotation_transform(
    Real       radians,
    int        axis,
    Transform  *transform )
{
    int   a1, a2;
    Real  c, s;

    a1 = (axis + 1) % N_DIMENSIONS;
    a2 = (axis + 2) % N_DIMENSIONS;

    make_identity_transform( transform );

    c = cos( (double) radians );
    s = sin( (double) radians );

    Transform_elem( *transform, a1, a1 ) = c;
    Transform_elem( *transform, a1, a2 ) = s;
    Transform_elem( *transform, a2, a1 ) = -s;
    Transform_elem( *transform, a2, a2 ) = c;
}

public  void  make_transform_relative_to_point(
    Point      *point,
    Transform  *transform,
    Transform  *rel_transform )
{
    Transform  to_origin, to_point;

    make_translation_transform( Point_x(*point), Point_y(*point),
                                Point_z(*point), &to_point );

    make_translation_transform( -Point_x(*point), -Point_y(*point),
                                -Point_z(*point), &to_origin );

    concat_transforms( rel_transform, &to_origin, transform );
    concat_transforms( rel_transform, rel_transform, &to_point );
}

public  void  make_transform_in_coordinate_system(
    Point      *origin,
    Vector     *x_axis,
    Vector     *y_axis,
    Vector     *z_axis,
    Transform  *transform,
    Transform  *rel_transform )
{
    Transform  to_bases, from_bases;

    make_change_to_bases_transform( origin, x_axis, y_axis, z_axis, &to_bases );
    make_change_from_bases_transform( origin, x_axis, y_axis, z_axis,
                                      &from_bases );

    concat_transforms( rel_transform, &from_bases, transform );
    concat_transforms( rel_transform, rel_transform, &to_bases );
}

public  void  make_rotation_about_axis(
    Vector     *axis,
    Real       angle,
    Transform  *transform )
{
    Real  c, s, t;
    Real  txy, txz, tyz, sx, sy, sz;
    Real  x, y, z;

    c = cos( (double) -angle );
    s = sin( (double) -angle );
    t = 1.0 - c;

    x = Point_x( *axis );
    y = Point_y( *axis );
    z = Point_z( *axis );

    txy = t * x * y;
    txz = t * x * z;
    tyz = t * y * z;

    sx = s * x;
    sy = s * y;
    sz = s * z;

    Transform_elem( *transform, 0, 0 ) = t * x * x + c;
    Transform_elem( *transform, 0, 1 ) = txy + sz;
    Transform_elem( *transform, 0, 2 ) = txz - sy;
    Transform_elem( *transform, 0, 3 ) = 0.0;

    Transform_elem( *transform, 1, 0 ) = txy - sz;
    Transform_elem( *transform, 1, 1 ) = t * y * y + c;
    Transform_elem( *transform, 1, 2 ) = tyz + sx;
    Transform_elem( *transform, 1, 3 ) = 0.0;

    Transform_elem( *transform, 2, 0 ) = txz + sy;
    Transform_elem( *transform, 2, 1 ) = tyz - sx;
    Transform_elem( *transform, 2, 2 ) = t * z * z + c;
    Transform_elem( *transform, 2, 3 ) = 0.0;

    Transform_elem( *transform, 3, 0 ) = 0.0;
    Transform_elem( *transform, 3, 1 ) = 0.0;
    Transform_elem( *transform, 3, 2 ) = 0.0;
    Transform_elem( *transform, 3, 3 ) = 1.0;
}

public  void  transform_point(
    Transform  *transform,
    Real       x,
    Real       y,
    Real       z,
    Real       *x_trans,
    Real       *y_trans,
    Real       *z_trans )
{
    Real       w;

    *x_trans = Transform_elem(*transform,0,0) * x +
               Transform_elem(*transform,0,1) * y +
               Transform_elem(*transform,0,2) * z +
               Transform_elem(*transform,0,3);

    *y_trans = Transform_elem(*transform,1,0) * x +
               Transform_elem(*transform,1,1) * y +
               Transform_elem(*transform,1,2) * z +
               Transform_elem(*transform,1,3);

    *z_trans = Transform_elem(*transform,2,0) * x +
               Transform_elem(*transform,2,1) * y +
               Transform_elem(*transform,2,2) * z +
               Transform_elem(*transform,2,3);

    w =        Transform_elem(*transform,3,0) * x +
               Transform_elem(*transform,3,1) * y +
               Transform_elem(*transform,3,2) * z +
               Transform_elem(*transform,3,3);

    if( w != 0.0 && w != 1.0 )
    {
        *x_trans /= w;
        *y_trans /= w;
        *z_trans /= w;
    }
}

public  void  transform_vector(
    Transform  *transform,
    Real       x,
    Real       y,
    Real       z,
    Real       *x_trans,
    Real       *y_trans,
    Real       *z_trans )
{
    *x_trans = Transform_elem(*transform,0,0) * x +
               Transform_elem(*transform,0,1) * y +
               Transform_elem(*transform,0,2) * z;

    *y_trans = Transform_elem(*transform,1,0) * x +
               Transform_elem(*transform,1,1) * y +
               Transform_elem(*transform,1,2) * z;

    *z_trans = Transform_elem(*transform,2,0) * x +
               Transform_elem(*transform,2,1) * y +
               Transform_elem(*transform,2,2) * z;
}

public  void  inverse_transform_point(
    Transform  *transform,
    Real       x,
    Real       y,
    Real       z,
    Real       *x_trans,
    Real       *y_trans,
    Real       *z_trans )
{
    x -= Transform_elem(*transform,0,3);
    y -= Transform_elem(*transform,1,3);
    z -= Transform_elem(*transform,2,3);
    
    *x_trans = Transform_elem(*transform,0,0) * x +
               Transform_elem(*transform,1,0) * y +
               Transform_elem(*transform,2,0) * z;
    
    *y_trans = Transform_elem(*transform,0,1) * x +
               Transform_elem(*transform,1,1) * y +
               Transform_elem(*transform,2,1) * z;
    
    *z_trans = Transform_elem(*transform,0,2) * x +
               Transform_elem(*transform,1,2) * y +
               Transform_elem(*transform,2,2) * z;
}

public  void  inverse_transform_vector(
    Transform  *transform,
    Real       x,
    Real       y,
    Real       z,
    Real       *x_trans,
    Real       *y_trans,
    Real       *z_trans )
{
    *x_trans = Transform_elem(*transform,0,0) * x +
               Transform_elem(*transform,1,0) * y +
               Transform_elem(*transform,2,0) * z;
    
    *y_trans = Transform_elem(*transform,0,1) * x +
               Transform_elem(*transform,1,1) * y +
               Transform_elem(*transform,2,1) * z;
    
    *z_trans = Transform_elem(*transform,0,2) * x +
               Transform_elem(*transform,1,2) * y +
               Transform_elem(*transform,2,2) * z;
}

public  void  convert_2d_transform_to_rotation_translation(
    Transform  *transform,
    Real       *degrees_clockwise,
    Real       *x_trans,
    Real       *y_trans )
{
    Real   x, y;

    x = Transform_elem(*transform, X, X );
    y = Transform_elem(*transform, Y, X );

    *degrees_clockwise = RAD_TO_DEG * compute_clockwise_rotation( x, y );
    *x_trans = Transform_elem( *transform, X, 3 );
    *y_trans = Transform_elem( *transform, Y, 3 );
}

public  Real  compute_clockwise_rotation( Real x, Real y )
{
    if( x == 0.0 )
    {
        if( y < 0.0 )
            return( PI / 2.0 );
        else if( y > 0.0 )
            return( 3.0 * PI / 2.0 );
        else
            return( 0.0 );
    }
    else if( y == 0.0 )
    {
        if( x > 0.0 )
            return( 0.0 );
        else
            return( PI );
    }
    else
        return( - (Real) atan2( (double) y, (double) x ) );
}

public  void  make_identity_transform_2d( Transform_2d *transform )
{
    Transform_2d_elem( *transform, 0, 0 ) = 1.0;
    Transform_2d_elem( *transform, 0, 1 ) = 0.0;
    Transform_2d_elem( *transform, 0, 2 ) = 0.0;
    Transform_2d_elem( *transform, 1, 0 ) = 0.0;
    Transform_2d_elem( *transform, 1, 1 ) = 1.0;
    Transform_2d_elem( *transform, 1, 2 ) = 0.0;
}

public  void  get_inverse_transform_2d(
    Transform_2d   *transform,
    Transform_2d   *inverse )
{
    Real  determinant;

    determinant = Transform_2d_elem(*transform,0,0) *
                  Transform_2d_elem(*transform,1,1) -
                  Transform_2d_elem(*transform,1,0) *
                  Transform_2d_elem(*transform,0,1);

    Transform_2d_elem(*inverse,0,0) = Transform_2d_elem(*transform,1,1) /
                                      determinant;
    Transform_2d_elem(*inverse,0,1) = -Transform_2d_elem(*transform,0,1) /
                                      determinant;
    Transform_2d_elem(*inverse,0,2) = (Transform_2d_elem(*transform,1,2) *
                                       Transform_2d_elem(*transform,0,1) -
                                       Transform_2d_elem(*transform,1,1) *
                                       Transform_2d_elem(*transform,0,2)) /
                                      determinant;

    Transform_2d_elem(*inverse,1,0) = -Transform_2d_elem(*transform,1,0) /
                                      determinant;
    Transform_2d_elem(*inverse,1,1) = Transform_2d_elem(*transform,0,0) /
                                      determinant;
    Transform_2d_elem(*inverse,1,2) = (Transform_2d_elem(*transform,1,0) *
                                       Transform_2d_elem(*transform,0,2) -
                                       Transform_2d_elem(*transform,0,0) *
                                       Transform_2d_elem(*transform,1,2)) /
                                      determinant;
}

public  void  transform_point_2d(
    Transform_2d   *transform,
    Real           x,
    Real           y,
    Real           *x_trans,
    Real           *y_trans )
{
    *x_trans = Transform_2d_elem(*transform,0,0) * x +
               Transform_2d_elem(*transform,0,1) * y +
               Transform_2d_elem(*transform,0,2);
    *y_trans = Transform_2d_elem(*transform,1,0) * x +
               Transform_2d_elem(*transform,1,1) * y +
               Transform_2d_elem(*transform,1,2);
}

public  Status  write_transform_file(
    char       filename[],
    char       comments[],
    Transform  *transform )
{
    Status              status;
    FILE                *file;
    General_transform   gen_transform;

    create_linear_transform( &gen_transform, transform );

    status = open_file_with_default_suffix( filename, "xfm",
                                 WRITE_FILE, ASCII_FORMAT, &file );

    if( status == OK )
    {
        status = output_transform( file, comments, &gen_transform );

        (void) close_file( file );
    }

    delete_general_transform( &gen_transform );

    return( status );
}

public  Status  read_transform_file(
    char       filename[],
    Transform  *transform )
{
    Status             status;
    FILE               *file;
    General_transform  gen_transform;

    status = open_file_with_default_suffix( filename, "xfm",
                         READ_FILE, ASCII_FORMAT, &file );

    if( status == OK )
    {
        if( input_transform( file, &gen_transform ) != OK )
            status = ERROR;

        (void) close_file( file );
    }

    if( status == OK )
        *transform = *get_linear_transform_ptr(&gen_transform);

    delete_general_transform( &gen_transform );

    return( status );
}

private  void  least_squares(
    int     n,
    Real    x_list[],
    Real    y_list[],
    Real    trans_list[],
    Real    *a,
    Real    *b,
    Real    *c )
{
    int   i;
    Real  x, y, xp;
    Real  sumx, sumy, sumxx, sumyy, sumxy, sumxp, sumyxp, sumxxp;
    Real  denom;

    sumx = 0.0;
    sumy = 0.0;
    sumxx = 0.0;
    sumxy = 0.0;
    sumyy = 0.0;
    sumxp = 0.0;
    sumxxp = 0.0;
    sumyxp = 0.0;

    for_less( i, 0, n )
    {
        x = x_list[i];
        y = y_list[i];
        xp = trans_list[i];

        sumx += x;
        sumy += y;
        sumxx += x * x;
        sumxy += x * y;
        sumyy += y * y;
        sumxp += xp;
        sumxxp += x * xp;
        sumyxp += y * xp;
    }

    denom = n*sumxx*sumyy - n*sumxy*sumxy - sumx*sumx*sumyy +
            2.0*sumxy*sumx*sumy - sumxx*sumy*sumy;

    if( denom == 0.0 )
    {
        HANDLE_INTERNAL_ERROR( "least_squares_transform_2d: division by zero.");
        *a = 0.0;
        *b = 0.0;
        *c = 0.0;
        return;
    }

    *a = (n*sumxxp*sumyy - n*sumxy*sumyxp - sumxxp*sumy*sumy +
          sumxy*sumy*sumxp + sumy*sumx*sumyxp - sumx*sumxp*sumyy) / denom;

    *b = -(-sumxy*sumx*sumxp + sumx*sumx*sumyxp + n*sumxy*sumxxp -
           n*sumxx*sumyxp - sumy*sumx*sumxxp + sumxx*sumy*sumxp) / denom;

    *c = (-sumx*sumxxp*sumyy + sumx*sumxy*sumyxp + sumy*sumxy*sumxxp -
           sumy*sumxx*sumyxp + sumxp*sumxx*sumyy - sumxp*sumxy*sumxy) / denom;
}

public  void  get_least_squares_transform_2d(
    int           n_points,
    Real          x[],
    Real          y[],
    Real          x_trans[],
    Real          y_trans[],
    Transform_2d  *transform_2d )
{
    Real  a, b, c;
    
    least_squares( n_points, x, y, x_trans, &a, &b, &c );
    Transform_2d_elem( *transform_2d, 0, 0 ) = a;
    Transform_2d_elem( *transform_2d, 0, 1 ) = b;
    Transform_2d_elem( *transform_2d, 0, 2 ) = c;
    
    least_squares( n_points, x, y, y_trans, &a, &b, &c );
    Transform_2d_elem( *transform_2d, 1, 0 ) = a;
    Transform_2d_elem( *transform_2d, 1, 1 ) = b;
    Transform_2d_elem( *transform_2d, 1, 2 ) = c;

#ifdef DEBUG
{
    int  i;
    Real xp, yp;
    print( "%g %g %g\n", Transform_2d_elem(*transform_2d,0,0),
                         Transform_2d_elem(*transform_2d,0,1),
                         Transform_2d_elem(*transform_2d,0,2) );
    print( "%g %g %g\n", Transform_2d_elem(*transform_2d,1,0),
                         Transform_2d_elem(*transform_2d,1,1),
                         Transform_2d_elem(*transform_2d,1,2) );
    for_less( i, 0, n_points )
    {
        transform_point_2d( transform_2d, x[i], y[i], &xp, &yp );
        print( "%g %g   %g %g\n", x_trans[i], y_trans[i], xp, yp );
    }
    print( "\n" );
}
#endif

}
