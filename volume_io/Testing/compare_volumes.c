#include  <internal_volume_io.h>

#define IN_ORDER

private  int  get_random_int( int );

int  main(
    int   argc,
    char  *argv[] )
{
    char                 *input_filename1, *input_filename2;
    Volume               volume1, volume2;
    int                  sizes1[N_DIMENSIONS], n_errors, iter;
    int                  sizes2[N_DIMENSIONS];
    int                  x, y, z;
    Real                 voxel1[N_DIMENSIONS], voxel2[N_DIMENSIONS];
    Real                 value1, value2, xw, yw, zw;
    minc_input_options   options;

    if( argc < 3 )
    {
        print( "Need args.\n" );
        return( 1 );
    }

    input_filename1 = argv[1];
    input_filename2 = argv[2];

    set_default_minc_input_options( &options );
    set_minc_input_vector_to_colour_flag( &options, TRUE );


    if( input_volume( input_filename1, N_DIMENSIONS,
                      XYZ_dimension_names,
                      NC_UNSPECIFIED, FALSE,
                      0.0, 0.0, TRUE, &volume1,
                      &options ) != OK )
        return( 1 );

    if( input_volume( input_filename2, N_DIMENSIONS,
                      XYZ_dimension_names,
                      NC_UNSPECIFIED, FALSE,
                      0.0, 0.0, TRUE, &volume2,
                      &options ) != OK )
        return( 1 );

    print( "Checking values.\n" );

    n_errors = 0;

    get_volume_sizes( volume1, sizes1 );
    get_volume_sizes( volume2, sizes2 );

#ifdef IN_ORDER
    for_less( x, 0, sizes1[X] )
    for_less( y, 0, sizes1[Y] )
    for_less( z, 0, sizes1[Z] )
    {
#else
    for_less( iter, 0, 100000 )
    {
        x = get_random_int( sizes1[X] );
        y = get_random_int( sizes1[Y] );
        z = get_random_int( sizes1[Z] );
#endif
        voxel1[X] = (Real) x;
        voxel1[Y] = (Real) y;
        voxel1[Z] = (Real) z;
        convert_voxel_to_world( volume1, voxel1, &xw, &yw, &zw );
        convert_world_to_voxel( volume2, xw, yw, zw, voxel2 );

        if( voxel2[X] >= -0.5 && voxel2[X] < (Real) sizes2[X] - 0.5 &&
            voxel2[Y] >= -0.5 && voxel2[Y] < (Real) sizes2[Y] - 0.5 &&
            voxel2[Z] >= -0.5 && voxel2[Z] < (Real) sizes2[Z] - 0.5 )
        {
            value1 = get_volume_voxel_value( volume1, x, y, z, 0, 0 );
            value2 = get_volume_voxel_value( volume2,
                                             ROUND( voxel2[X] ),
                                             ROUND( voxel2[Y] ),
                                             ROUND( voxel2[Z] ),
                                             0, 0 );

            if( value1 != value2 )
            {
                ++n_errors;
                if( n_errors <= 10 )
                    print( "Error: Vol1: %d %d %d  Vol2: %d %d %d:  %g %g\n",
                           x, y, z,
                           ROUND(voxel2[X]), ROUND(voxel2[Y]), ROUND(voxel2[Z]),
                           value1, value2 );
            }
        }
    }

    delete_volume( volume1 );
    delete_volume( volume2 );

    output_alloc_to_file( NULL );

    return( 0 );
}

#include  <sys/time.h>

static  BOOLEAN  initialized = FALSE;

private  void  set_random_seed( int seed )
{
    (void) srandom( seed );
    initialized = TRUE;
}

private  void  check_initialized( void )
{
    struct   timeval   t;
    int                seed;

    if( !initialized )
    {
        (void) gettimeofday( &t, (struct timezone *) 0 );

        seed = (int) t.tv_usec;

        set_random_seed( seed );
    }
}

private  int  get_random( void )
{
    check_initialized();

    return( random() );
}

private  int  get_random_int( int n )
{
    return( get_random() % n );
}
