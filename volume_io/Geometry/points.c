
#include  <def_mni.h>

public  Boolean  null_Point(
    Point   *point )
{
    return( Point_x(*point) == 0.0 &&
            Point_y(*point) == 0.0 &&
            Point_z(*point) == 0.0 );
}

public  Boolean  null_Vector(
    Vector   *vector )
{
    return( Vector_x(*vector) == 0.0 &&
            Vector_y(*vector) == 0.0 &&
            Vector_z(*vector) == 0.0 );
}

public  Real  distance_between_points(
    Point  *p1,
    Point  *p2 )
{
    Vector  diff;

    SUB_POINTS( diff, *p1, *p2 );

    return( MAGNITUDE( diff ) );
}

public  Boolean  points_within_distance(
    Point  *p1,
    Point  *p2,
    Real   distance )
{
    Real  dx;

    dx = Point_x(*p1) - Point_x(*p2);
    if( dx < 0.0 )  dx = -dx;

    if( dx > distance )
        return( FALSE );

    return( distance_between_points( p1, p2 ) <= distance );
}

public  void  create_noncolinear_vector(
    Vector  *v,
    Vector  *not_v )
{
    Real   x, y, z;

    x = Vector_x(*v);
    y = Vector_y(*v);
    z = Vector_z(*v);

    if( x != 0.0 || y != 0.0 )
    {
        fill_Vector( *not_v, x, y, z + 1.0 );
    }
    else
    {
        fill_Vector( *not_v, 1.0, y, z );
    }
}

public  void  apply_point_to_min_and_max(
    Point   *point,
    Point   *min_point,
    Point   *max_point )
{
    int    c;

    for_less( c, 0, N_DIMENSIONS )
    {
        if( Point_coord(*point,c) < Point_coord(*min_point,c) )
        {
            Point_coord(*min_point,c) = Point_coord(*point,c);
        }
        if( Point_coord(*point,c) > Point_coord(*max_point,c) )
        {
            Point_coord(*max_point,c) = Point_coord(*point,c);
        }
    }
}

public  void  expand_min_and_max_points(
    Point   *min_point,
    Point   *max_point,
    Point   *min_to_check,
    Point   *max_to_check )
{
    int    c;

    for_less( c, 0, N_DIMENSIONS )
    {
        if( Point_coord(*min_to_check,c) < Point_coord(*min_point,c) )
        {
            Point_coord(*min_point,c) = Point_coord(*min_to_check,c);
        }
        if( Point_coord(*max_to_check,c) > Point_coord(*max_point,c) )
        {
            Point_coord(*max_point,c) = Point_coord(*max_to_check,c);
        }
    }
}

public  void  get_range_points(
    int                n_points,
    Point              points[],
    Point              *min_corner,
    Point              *max_corner )
{
    int     i;

    if( n_points > 0 )
    {
        *min_corner = points[0];
        *max_corner = points[0];

        for_less( i, 1, n_points )
        {
            apply_point_to_min_and_max( &points[i], min_corner, max_corner );
        }
    }
}

public  void  get_points_centroid(
    int     n_points,
    Point   points[],
    Point   *centroid )
{
    int   i;

    Point_x(*centroid) = 0.0;
    Point_y(*centroid) = 0.0;
    Point_z(*centroid) = 0.0;

    for_less( i, 0, n_points )
    {
        Point_x(*centroid) += Point_x(points[i]);
        Point_y(*centroid) += Point_y(points[i]);
        Point_z(*centroid) += Point_z(points[i]);
    }

    Point_x(*centroid) /= (Real) n_points;
    Point_y(*centroid) /= (Real) n_points;
    Point_z(*centroid) /= (Real) n_points;
}

public   void     reverse_vectors(
    int       n_vectors,
    Vector    vectors[] )
{
    int     i;

    for_less( i, 0, n_vectors )
    {
        Vector_x(vectors[i]) = -Vector_x(vectors[i]);
        Vector_y(vectors[i]) = -Vector_y(vectors[i]);
        Vector_z(vectors[i]) = -Vector_z(vectors[i]);
    }
}
