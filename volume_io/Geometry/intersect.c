
#include  <mni.h>

#define  MAX_POINTS    30

private  BOOLEAN  point_within_polygon(
    Point   *pt,
    int     n_points,
    Point   points[],
    Vector  *polygon_normal );
private  BOOLEAN  point_within_triangle_2d(
    Point   *pt,
    int     i1,
    int     i2,
    Point   points[] );
private  BOOLEAN  point_within_polygon_2d(
    Point   *pt,
    int     i1,
    int     i2,
    int     n_points,
    Point   points[] );

public  BOOLEAN   intersect_ray_polygon(
    Point            *ray_origin,
    Vector           *ray_direction,
    Real             *dist,
    polygons_struct  *polygons,
    int              poly_index )
{
    BOOLEAN  intersects;
    Point    points[MAX_POINTS];
    Vector   normal;
    Real     n_dot_d, t, plane_const;
    Point    centroid, pt;
    int      ind, p, start_index, end_index, size;

    intersects = FALSE;

    if( polygons->visibilities == (Smallest_int *) 0 ||
        polygons->visibilities[poly_index] )
    {
        start_index = START_INDEX( polygons->end_indices, poly_index );
        end_index = polygons->end_indices[poly_index];

        size = end_index - start_index;

        if( size > MAX_POINTS )
        {
            print( "Warning: awfully big polygon, size = %d\n", size );
            size = MAX_POINTS;
            end_index = start_index + size - 1;
        }

        for_less( p, start_index, end_index )
        {
            ind = polygons->indices[p];
            points[p-start_index] = polygons->points[ind];
        }

        find_polygon_normal( size, points, &normal );

        n_dot_d = DOT_VECTORS( normal, *ray_direction );

        if( n_dot_d != 0.0 )
        {
            get_points_centroid( size, points, &centroid );

            plane_const = DOT_POINT_VECTOR( centroid, normal );

            t = (plane_const - DOT_POINT_VECTOR(normal,*ray_origin) ) / n_dot_d;

            if( t >= 0.0 && t < *dist )
            {
                GET_POINT_ON_RAY( pt, *ray_origin, *ray_direction, t );

                if( point_within_polygon( &pt, size, points, &normal ) )
                {
                    *dist = t;
                    intersects = TRUE;
                }
            }
        }
    }

    return( intersects );
}

private  BOOLEAN  point_within_polygon(
    Point   *pt,
    int     n_points,
    Point   points[],
    Vector  *polygon_normal )
{
    BOOLEAN  intersects;
    Real     nx, ny, nz, max_val;
    int      i1, i2;

    nx = ABS( Vector_x(*polygon_normal) );
    ny = ABS( Vector_y(*polygon_normal) );
    nz = ABS( Vector_z(*polygon_normal) );

    max_val = MAX3( nx, ny, nz );

    if( nx == max_val )
    {
        i1 = Y;
        i2 = Z;
    }
    else if( ny == max_val )
    {
        i1 = Z;
        i2 = X;
    }
    else
    {
        i1 = X;
        i2 = Y;
    }

#ifdef DO_I_TRUST_THE_TRIANGLE_CODE
    if( n_points == 3 )
    {
        intersects = point_within_triangle_2d( pt, i1, i2, points );
    }
    else
#endif
    {
        intersects = point_within_polygon_2d( pt, i1, i2, n_points, points );
    }

    return( intersects );
}

private  BOOLEAN  point_within_triangle_2d(
    Point   *pt,
    int     i1,
    int     i2,
    Point   points[] )
{
    BOOLEAN  intersects;
    Real     alpha, beta, u0, u1, u2, v0, v1, v2, bottom;

    intersects = FALSE;

    u0 = Point_coord(*pt,i1) - Point_coord(points[0],i1);
    v0 = Point_coord(*pt,i2) - Point_coord(points[0],i2);

    u1 = Point_coord(points[1],i1) - Point_coord(points[0],i1);
    u2 = Point_coord(points[2],i1) - Point_coord(points[0],i1);

    v1 = Point_coord(points[1],i2) - Point_coord(points[0],i2);
    v2 = Point_coord(points[2],i2) - Point_coord(points[0],i2);

    if( u1 == 0.0 )
    {
        if( u2 != 0.0 )
        {
            beta = u0 / u2;

            if( 0.0 <= beta && beta <= 1.0 && v1 != 0.0 )
            {
                alpha = (v0 - beta * v2) / v1;
                intersects = ( (alpha >= 0.0) && ((alpha + beta) <= 1.0) );
            }
        }
    }
    else
    {
        bottom = v2 * u1 - u2 * v1;

        if( bottom != 0.0 )
        {
            beta = (v0 * u1 - u0 * v1) / bottom;

            if( 0.0 <= beta && beta <= 1.0 && u1 != 0.0 )
            {
                alpha = (u0 - beta * u2) / u1;
                intersects = ( (alpha >= 0.0) && ((alpha+beta) <= 1.0) );
            }
        }
    }

    return( intersects );
}

private  BOOLEAN  point_within_polygon_2d(
    Point   *pt,
    int     i1,
    int     i2,
    int     n_points,
    Point   points[] )
{
    BOOLEAN  intersects;
    Real     x, y, x1, y1, x2, y2, x_inter, dy;
    int      i;
    BOOLEAN  cross;

    x = Point_coord( *pt, i1 );
    y = Point_coord( *pt, i2 );

    cross = FALSE;

    intersects = FALSE;

    x2 = Point_coord(points[n_points-1],i1);
    y2 = Point_coord(points[n_points-1],i2);

    for_less( i, 0, n_points )
    {
        x1 = x2;
        y1 = y2;

        x2 = Point_coord(points[i],i1);
        y2 = Point_coord(points[i],i2);

        if( !( (y1 > y && y2 > y) || (y1 < y && y2 < y) || (x1 > x && x2 > x)) )
        {
            dy = y2 - y1;

            if( dy == 0.0 )
            {
                if( y1 == y && ( (x1 <= x && x2 >= x) ||
                                 (x1 >= x && x2 <= x) ) )
                {
                    intersects = TRUE;
                    break;
                }
            }
            else
            {
                if( y1 == y )
                {
                    if( y2 > y )
                    {
                        cross = !cross;
                    }
                }
                else if( y2 == y )
                {
                    if( y1 > y )
                    {
                        cross = !cross;
                    }
                }
                else if( x1 <= x && x2 <= x )
                {
                    cross = !cross;
                }
                else
                {
                    x_inter = x1 + (y - y1) / dy * (x2 - x1);

                    if( x_inter == x )
                    {
                        intersects = TRUE;
                        break;
                    }
                    else if( x_inter < x )
                    {
                        cross = !cross;
                    }
                }
            }
        }
    }

    if( !intersects )
        intersects = cross;

    return( intersects );
}
