#include  <internal_volume_io.h>

#define  MAX_ERRORS 1

private  int  compute_voxel(
    int    x,
    int    y,
    int    z,
    Real   factor );

#undef    TESTING_IO
#define   TESTING_IO

#define  CACHE_THRESHOLD  1

#ifdef  TESTING_IO
#define  CACHE_SIZE       1000000
#else
#define  CACHE_SIZE       1000000000
#endif

#define  X_SIZE  100
#define  Y_SIZE  100
#define  Z_SIZE  100

#define  BLOCK_SIZE  8

int  main(
    int   argc,
    char  *argv[] )
{
    Volume               volume;
    int                  sizes[N_DIMENSIONS];
    int                  x, y, z, n_errors;
    int                  true_voxel, test_voxel;
    STRING               output_filename, output_filename2;
    static STRING        dim_names[] = { MIxspace, MIzspace, MIyspace };
    static int           block_sizes[] = { BLOCK_SIZE,
                                           BLOCK_SIZE,
                                           BLOCK_SIZE,
                                           BLOCK_SIZE,
                                           BLOCK_SIZE };

    if( argc < 3 )
    {
        print( "Need args.\n" );
        return( 1 );
    }

    output_filename = argv[1];
    output_filename2 = argv[2];

    set_n_bytes_cache_threshold( CACHE_THRESHOLD );
    set_max_bytes_in_cache( CACHE_SIZE );
    set_volume_cache_block_sizes( block_sizes );

    volume = create_volume( N_DIMENSIONS, dim_names, NC_BYTE, FALSE,
                            0.0, 0.0 );

    sizes[X] = X_SIZE;
    sizes[Y] = Y_SIZE;
    sizes[Z] = Z_SIZE;

    set_volume_sizes( volume, sizes );
    alloc_volume_data( volume );

    print( "Setting volume.\n" );

    for_less( x, 0, sizes[X] )
    for_less( y, 0, sizes[Y] )
    for_less( z, 0, sizes[Z] )
    {
        true_voxel = compute_voxel( x, y, z, 1.0 );
        set_volume_voxel_value( volume, x, y, z, 0, 0, (Real) true_voxel );
    }

    print( "Checking volume.\n" );

    n_errors = 0;
    for_less( x, 0, sizes[X] )
    for_less( y, 0, sizes[Y] )
    for_less( z, 0, sizes[Z] )
    {
        true_voxel = compute_voxel( x, y, z, 1.0 );
        test_voxel = (int) get_volume_voxel_value( volume, x, y, z, 0, 0 );
        if( true_voxel != test_voxel )
        {
            ++n_errors;
            if( n_errors <= MAX_ERRORS )
                print( "Error: %d %d\n", true_voxel, test_voxel );
        }
    }

#ifdef  TESTING_IO
    print( "Outputting volume.\n" );

    if( output_volume( output_filename, NC_UNSPECIFIED, FALSE, 0.0, 0.0,
                       volume, "Testing Virtual Volumes", NULL ) != OK )
        return( 1 );

    delete_volume( volume );

    print( "Inputting volume.\n" );

    /*set_n_bytes_cache_threshold( 100000000 ); */

    if( input_volume( output_filename, N_DIMENSIONS, File_order_dimension_names,
                      NC_UNSPECIFIED, FALSE,
                      0.0, 0.0, TRUE, &volume,
                      (minc_input_options *) NULL ) != OK )
        return( 1 );

    print( "Checking values.\n" );

    n_errors = 0;

    for_less( x, 0, sizes[X] )
    for_less( y, 0, sizes[Y] )
    for_less( z, 0, sizes[Z] )
    {
        true_voxel = compute_voxel( x, y, z, 1.0 );
        test_voxel = (int) get_volume_voxel_value( volume, x, y, z, 0, 0 );
        if( true_voxel != test_voxel )
        {
            ++n_errors;
            if( n_errors <= MAX_ERRORS )
                print( "Error: %d %d %d: %d %d\n", x, y, z, true_voxel, test_voxel );
        }
    }
#endif

    print( "Setting voxels.\n" );

    for_less( x, 0, sizes[X] )
    for_less( y, 0, sizes[Y] )
    for_less( z, 0, sizes[Z] )
    {
        true_voxel = compute_voxel( x, y, z, 0.5 );
        set_volume_voxel_value( volume, x, y, z, 0, 0, (Real) true_voxel );
    }

    print( "Checking values.\n" );

    n_errors = 0;

    for_less( x, 0, sizes[X] )
    for_less( y, 0, sizes[Y] )
    for_less( z, 0, sizes[Z] )
    {
        true_voxel = compute_voxel( x, y, z, 0.5 );
        test_voxel = (int) get_volume_voxel_value( volume, x, y, z, 0, 0 );
        if( true_voxel != test_voxel )
        {
            ++n_errors;
            if( n_errors <= MAX_ERRORS )
                print( "Error: %d %d %d: %d %d\n", x, y, z, true_voxel, test_voxel);
        }
    }

#ifdef  TESTING_IO
    if( output_volume( output_filename2, NC_UNSPECIFIED, FALSE, 0.0, 0.0,
                       volume, "Testing Virtual Volumes", NULL ) != OK )
        return( 1 );
#endif

    delete_volume( volume );

    output_alloc_to_file( NULL );

    return( 0 );
}

private  int  compute_voxel(
    int    x,
    int    y,
    int    z,
    Real   factor )
{
    int   value;

    value = ((int) (factor * (Real) (x + y + z))) % 256;

    return( value );
}
