#include  <mni.h>

private  void  get_random_point_on_sphere(
    Point      *centre,
    Real       radius,
    Point      *point );
private  void  get_random_ray(
    range_struct  *limits,
    Point         *centre,
    Real          radius,
    Point         *origin,
    Vector        *direction );

int  main(
    int    argc,
    char   *argv[] )
{
    Status           status;
    char             *input_filename;
    int              i, c, n_objects, max_nodes, n_rays, n_intersections;
    int              poly_index;
    Real             dist, radius, delta;
    Point            origin, centre;
    Vector           direction;
    File_formats     format;
    object_struct    **object_list;
    polygons_struct  *polygons;

    status = OK;

    if( argc == 1 )
    {
        (void) fprintf( stderr, "Must have a filename argument.\n" );
        status = ERROR;
    }

    if( status == OK )
    {
        input_filename = argv[1];

        status = input_graphics_file( input_filename, &format, &n_objects,
                                      &object_list );

        if( status == OK )
            print( "Objects input.\n" );
    }

    if( argc <= 2 || sscanf( argv[2], "%d", &max_nodes ) != 1 )
        max_nodes = 10000;

    if( argc <= 3 || sscanf( argv[3], "%d", &n_rays ) != 1 )
        n_rays = 10000;

    if( status == OK && n_objects > 0 &&
        object_list[0]->object_type == POLYGONS )
    {
        polygons = get_polygons_ptr( object_list[0] );
        create_polygons_bintree( polygons, max_nodes );
    }

    n_intersections = 0;

    radius = 0.0;
    for_less( c, 0, N_DIMENSIONS )
    {
        Point_coord( centre, c ) = (polygons->bintree->range.limits[c][0] +
                                    polygons->bintree->range.limits[c][1]) /2.0;
        delta = polygons->bintree->range.limits[c][1] -
                polygons->bintree->range.limits[c][0];
        radius += delta * delta;
    }

    radius = 10.0 * sqrt( radius );

    for_less( i, 0, n_rays )
    {
        get_random_ray( &polygons->bintree->range, &centre, radius,
                        &origin, &direction );
        if( intersect_ray_with_bintree( &origin, &direction,
                                        polygons->bintree, polygons,
                                        &poly_index, &dist ) )
            ++n_intersections;
    }

    print( "%d/%d: ", n_intersections, n_rays );
    print_bintree_stats( n_rays );

    if( status == OK )
        delete_object_list( n_objects, object_list );

    return( status != OK );
}

private  void  get_random_ray(
    range_struct  *limits,
    Point         *centre,
    Real          radius,
    Point         *origin,
    Vector        *direction )
{
    int    c;
    Real   alpha;
    Point  dest;

    get_random_point_on_sphere( centre, radius, origin );

    for_less( c, 0, N_DIMENSIONS )
    {
        alpha = get_random_0_to_1();
        Point_coord( dest, c ) = alpha * limits->limits[c][0] +
                                 (1.0 - alpha) * limits->limits[c][1];
    }

    SUB_POINTS( *direction, dest, *origin );
}

private  void  get_random_point_on_sphere(
    Point      *centre,
    Real       radius,
    Point      *point )
{
    int    c;
    Real   offset, sum, factor;
    Point  pos;

    do
    {
        sum = 0.0;
        for_less( c, 0, N_DIMENSIONS )
        {
            offset = 2.0 * radius * get_random_0_to_1() - radius;
            Point_coord( pos, c ) = Point_coord( *centre, c ) + offset;
            sum += offset * offset;
        }
    } while( sum > radius * radius && sum != 0.0 );

    sum = sqrt( sum );

    factor = radius / sum;

    for_less( c, 0, N_DIMENSIONS )
    {
        offset = Point_coord( pos, c ) - Point_coord( *centre, c );
        offset *= factor;
        Point_coord( *point, c ) = Point_coord( *centre, c ) + offset;
    }
}
