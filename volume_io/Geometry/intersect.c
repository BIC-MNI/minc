
#include  <mni.h>

#define  MAX_POINTS    30

private  BOOLEAN  point_within_triangle_2d(
    Point   *pt,
    Point   points[] );
private  BOOLEAN  point_within_polygon_2d(
    Point   *pt,
    int     n_points,
    Point   points[],
    Vector  *polygon_normal );

public  BOOLEAN   intersect_ray_polygon(
    Point            *ray_origin,
    Vector           *ray_direction,
    Real             *dist,
    polygons_struct  *polygons,
    int              poly_index )
{
    BOOLEAN  intersects;
    Point    points[MAX_POINTS];
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

        intersects = intersect_ray_polygon_points( ray_origin, ray_direction,
                                                   size, points, dist );
    }

    return( intersects );
}

public  BOOLEAN   intersect_ray_polygon_points(
    Point            *ray_origin,
    Vector           *ray_direction,
    int              n_points,
    Point            points[],
    Real             *dist )
{
    BOOLEAN  intersects;
    Vector   normal;
    Real     n_dot_d, t, plane_const;
    Point    centroid, pt;

    intersects = FALSE;

    find_polygon_normal( n_points, points, &normal );

    n_dot_d = DOT_VECTORS( normal, *ray_direction );

    if( n_dot_d != 0.0 )
    {
        get_points_centroid( n_points, points, &centroid );

        plane_const = DOT_POINT_VECTOR( centroid, normal );

        t = (plane_const - DOT_POINT_VECTOR(normal,*ray_origin) ) / n_dot_d;

        if( t >= 0.0 && t <= *dist )
        {
            GET_POINT_ON_RAY( pt, *ray_origin, *ray_direction, t );

            if( point_within_polygon( &pt, n_points, points, &normal ) )
            {
                *dist = t;
                intersects = TRUE;
            }
        }
    }

    return( intersects );
}

public  BOOLEAN  point_within_polygon(
    Point   *pt,
    int     n_points,
    Point   points[],
    Vector  *polygon_normal )
{
    BOOLEAN  intersects;

    if( n_points == 3 )
        intersects = point_within_triangle_2d( pt, points );
    else
        intersects = point_within_polygon_2d( pt, n_points, points,
                                              polygon_normal );

    return( intersects );
}

private  BOOLEAN  point_within_triangle_2d(
    Point   *pt,
    Point   points[] )
{
    BOOLEAN  inside;
    int      i;
    Vector   edges[3];
    Vector   normal, offset, edge_normal;

    SUB_VECTORS( edges[0], points[1], points[0] );
    SUB_VECTORS( edges[1], points[2], points[1] );
    SUB_VECTORS( edges[2], points[0], points[2] );
    CROSS_VECTORS( normal, edges[2], edges[0] );

    inside = TRUE;

    for_less( i, 0, 3 )
    {
        SUB_POINTS( offset, *pt, points[i] );
        CROSS_VECTORS( edge_normal, edges[i], normal );
        if( DOT_VECTORS( offset, edge_normal ) > 0.0 )
        {
            inside = FALSE;
            break;
        }
    }

    return( inside );
}

private  BOOLEAN  point_within_polygon_2d(
    Point   *pt,
    int     n_points,
    Point   points[],
    Vector  *polygon_normal )
{
    BOOLEAN  intersects;
    Real     x, y, x1, y1, x2, y2, x_inter, dy;
    Real     nx, ny, nz, max_val;
    int      i1, i2;
    int      i;
    BOOLEAN  cross;

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

public  BOOLEAN   intersect_ray_quadmesh(
    Point            *ray_origin,
    Vector           *ray_direction,
    Real             *dist,
    quadmesh_struct  *quadmesh,
    int              obj_index )
{
    BOOLEAN  intersects;
    Point    points[4];
    int      i, j, m, n;

    get_quadmesh_n_objects( quadmesh, &m, &n );

    i = obj_index / n;
    j = obj_index % n;

    get_quadmesh_patch( quadmesh, i, j, points );

    intersects = intersect_ray_polygon_points( ray_origin, ray_direction,
                                               4, points, dist );

    return( intersects );
}

public  BOOLEAN  ray_intersects_sphere(
    Point       *origin,
    Vector      *direction,
    Point       *centre,
    Real        radius,
    Real        *dist )
{
    int     n_sols;
    Real    a, b, c, sols[2];
    Vector  o_minus_c;

    SUB_VECTORS( o_minus_c, *origin, *centre );

    a = DOT_VECTORS( *direction, *direction );
    b = 2.0 * DOT_VECTORS( o_minus_c, *direction );
    c = DOT_VECTORS( o_minus_c, o_minus_c ) - radius * radius;

    n_sols = solve_quadratic( a, b, c, &sols[0], &sols[1] );

    if( n_sols == 0 )
        *dist = -1.0;
    else if( n_sols == 1 )
        *dist = sols[0];
    else
    {
        if( sols[0] < 0.0 )
            *dist = sols[1];
        else if( sols[1] < 0.0 )
            *dist = sols[0];
        else
            *dist = MIN( sols[0], sols[1] );
    }

    return( *dist >= 0.0 );
}

public  BOOLEAN  ray_intersects_tube(
    Point       *origin,
    Vector      *direction,
    Point       *p1,
    Point       *p2,
    Real        radius,
    Real        *dist )
{
    Real     o_dot_o, o_dot_v, v_dot_v, d_dot_d, d_dot_v, d_dot_o, d;
    Vector   v, o, offset;
    Point    point;
    int      n_sols;
    Real     a, b, c, sols[2];

    SUB_POINTS( v, *p2, *p1 );
    NORMALIZE_VECTOR( v, v );

    SUB_POINTS( o, *origin, *p1 );

    o_dot_o = DOT_VECTORS( o, o );
    o_dot_v = DOT_VECTORS( o, v );
    v_dot_v = DOT_VECTORS( v, v );
    d_dot_d = DOT_VECTORS( *direction, *direction );
    d_dot_o = DOT_VECTORS( *direction, o );
    d_dot_v = DOT_VECTORS( *direction, v );
    a = d_dot_d - 2.0 * d_dot_v * d_dot_v + v_dot_v * d_dot_v * d_dot_v;
    b = 2.0 * d_dot_o - 4.0 * o_dot_v * d_dot_v +
        2.0 * v_dot_v * o_dot_v * d_dot_v;
    c = o_dot_o - 2.0 * o_dot_v * o_dot_v + v_dot_v * o_dot_v * o_dot_v -
         radius * radius;

    n_sols = solve_quadratic( a, b, c, &sols[0], &sols[1] );

    if( n_sols == 0 )
        *dist = -1.0;
    else
    {
        if( sols[0] < 0.0 )
            *dist = sols[1];
        else if( sols[1] < 0.0 )
            *dist = sols[0];
        else
            *dist = MIN( sols[0], sols[1] );

        if( *dist >= 0.0 )
        {
            GET_POINT_ON_RAY( point, *origin, *direction, *dist );
            SUB_POINTS( offset, point, *p1 );
            d = DOT_VECTORS( v, offset );
            if( d < 0.0 || d > distance_between_points( p1, p2 ) )
                *dist = -1.0;
        }
    }

    return( *dist >= 0.0 );
}

public  BOOLEAN   intersect_ray_tube_segment(
    Point            *origin,
    Vector           *direction,
    Real             *dist,
    lines_struct     *lines,
    int              obj_index )
{
    Real     a_dist;
    int      line, seg, p1, p2;
    BOOLEAN  found;

    get_line_segment_index( lines, obj_index, &line, &seg );

    p1 = lines->indices[POINT_INDEX(lines->end_indices,line,seg)];
    p2 = lines->indices[POINT_INDEX(lines->end_indices,line,seg+1)];
    
    found = ray_intersects_sphere( origin, direction, &lines->points[p1],
                                   (Real) lines->line_thickness, dist );

    if( ray_intersects_sphere( origin, direction, &lines->points[p2],
                               (Real) lines->line_thickness, &a_dist ) &&
        (!found || a_dist < *dist ) )
    {
        found = TRUE;
        *dist = a_dist;
    }

    if( ray_intersects_tube( origin, direction,
                             &lines->points[p1],
                             &lines->points[p2],
                             (Real) lines->line_thickness, &a_dist ) &&
        (!found || a_dist < *dist ) )
    {
        found = TRUE;
        *dist = a_dist;
    }

    return( found );
}
