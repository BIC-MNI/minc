#include  <mni.h>

private  void  get_random_point_on_sphere(
    Point      *centre,
    Real       radius,
    Point      *point );
private  void  get_random_ray(
    Point         *centre,
    Real          radius,
    Point         *origin,
    Vector        *direction );
private  void  get_random_direction(
    Vector      *dir );

int  main(
    int    argc,
    char   *argv[] )
{
    Status           status;
    char             *input_filename;
    int              i, c, n_objects, max_nodes, n_rays, n_intersections;
    int              poly_index, n_good_rays;
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

    radius = 0.0;
    for_less( c, 0, N_DIMENSIONS )
    {
        Point_coord( centre, c ) = (polygons->bintree->range.limits[c][0] +
                                    polygons->bintree->range.limits[c][1]) /2.0;
        delta = polygons->bintree->range.limits[c][1] -
                polygons->bintree->range.limits[c][0];
        radius += delta * delta;
    }

    radius = sqrt( radius );

    n_intersections = 0;
    n_good_rays = 0;

    for_less( i, 0, n_rays )
    {
        Real   t_min, t_max;

        get_random_ray( &centre, radius, &origin, &direction );

        if( ray_intersects_range( &polygons->bintree->range,
                                  &origin, &direction, &t_min, &t_max ) )
        {
            ++n_good_rays;
            if( intersect_ray_with_bintree( &origin, &direction,
                                            polygons->bintree, polygons,
                                            &poly_index, &dist,
                                            (Real **) NULL ) > 0 )
                ++n_intersections;
        }
    }

    print( "%d/%d/%d: ", n_intersections, n_good_rays, n_rays );
    print_bintree_stats( n_good_rays );

    if( status == OK )
        delete_object_list( n_objects, object_list );

    return( status != OK );
}

private  void  get_random_ray(
    Point         *centre,
    Real          radius,
    Point         *origin,
    Vector        *direction )
{
    get_random_point_on_sphere( centre, radius, origin );

    get_random_direction( direction );
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

private  void  get_random_direction(
    Vector      *dir )
{
    Point   centre, point;

    fill_Point( centre, 0.0, 0.0, 0.0 );

    get_random_point_on_sphere( &centre, 1.0, &point );

    fill_Vector( *dir, Point_x(point), Point_y(point), Point_z(point) );
}
