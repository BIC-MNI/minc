#include  <internal_volume_io.h>

#define  MAX_ERRORS 5

private  int  compute_voxel(
    int    x,
    int    y,
    int    z,
    Real   factor );

#undef    TESTING_IO
#define   TESTING_IO

#define  CACHE_THRESHOLD  1

#ifdef  TESTING_IO
#define  CACHE_SIZE       100000
#else
#define  CACHE_SIZE       1000000000
#endif

#define  X_SIZE  81
#define  Y_SIZE  88
#define  Z_SIZE  82
#define  X_SIZE  31
#define  Y_SIZE  38
#define  Z_SIZE  32

#define  BLOCK_SIZE_0  8
#define  BLOCK_SIZE_1  9
#define  BLOCK_SIZE_2  16
#define  BLOCK_SIZE_3  8
#define  BLOCK_SIZE_4  8

#define  NEW_BLOCK_SIZE_0  16
#define  NEW_BLOCK_SIZE_1  8
#define  NEW_BLOCK_SIZE_2  4
#define  NEW_BLOCK_SIZE_3  8
#define  NEW_BLOCK_SIZE_4  8

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
    static int           block_sizes[] = { BLOCK_SIZE_0,
                                           BLOCK_SIZE_1,
                                           BLOCK_SIZE_2,
                                           BLOCK_SIZE_3,
                                           BLOCK_SIZE_4 };
    static int           new_block_sizes[] = { NEW_BLOCK_SIZE_0,
                                               NEW_BLOCK_SIZE_1,
                                               NEW_BLOCK_SIZE_2,
                                               NEW_BLOCK_SIZE_3,
                                               NEW_BLOCK_SIZE_4 };

    if( argc < 3 )
    {
        print( "Need args.\n" );
        return( 1 );
    }

    output_filename = argv[1];
    output_filename2 = argv[2];

    set_n_bytes_cache_threshold( CACHE_THRESHOLD );
    set_default_max_bytes_in_cache( CACHE_SIZE );
    set_cache_block_sizes_hint( RANDOM_VOLUME_ACCESS );

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

    set_volume_cache_block_sizes( volume, new_block_sizes );

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

    set_cache_output_volume_parameters( volume, output_filename2,
                                        NC_SHORT, FALSE, 0.0, 0.0,
                                        output_filename,
                                        "Testing Virtual Volumes\n", NULL );

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

    set_volume_cache_block_sizes( volume, block_sizes );

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
