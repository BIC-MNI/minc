#include  <internal_volume_io.h>

int  main(
    int   argc,
    char  *argv[] )
{
    Volume               volume, cached_volume;
    int                  sizes[N_DIMENSIONS];
    int                  x, y, z;
    Real                 true_value, test_value;
    char                 *input_filename, *output_filename;
    static char          *dim_names[] = { MIxspace, MIzspace, MIyspace };

    if( argc < 3 )
    {
        print( "Need args.\n" );
        return( 1 );
    }

    input_filename = argv[1];
    output_filename = argv[2];

    if( input_volume( input_filename, 3, File_order_dimension_names,
                      NC_UNSPECIFIED, FALSE,
                      0.0, 0.0, TRUE, &volume,
                      (minc_input_options *) NULL ) != OK )
        return( 1 );

    set_n_bytes_cache_threshold( 100 );
    set_max_bytes_in_cache( 100000 );

    if( input_volume( input_filename, 3, File_order_dimension_names,
                      NC_UNSPECIFIED, FALSE,
                      0.0, 0.0, TRUE, &cached_volume,
                      (minc_input_options *) NULL ) != OK )
        return( 1 );

    get_volume_sizes( volume, sizes );

    for_less( x, 0, sizes[X] )
    for_less( y, 0, sizes[Y] )
    for_less( z, 0, sizes[Z] )
    {
        true_value = get_volume_real_value( volume, x, y, z, 0, 0 );
        test_value = get_volume_real_value( cached_volume, x, y, z, 0, 0 );

        if( true_value != test_value )
            print( "%d %d %d:  %g != %g\n", x, y, z, true_value, test_value );
    }

    set_cache_volume_output_filename( cached_volume, output_filename );

    print( "Setting voxels.\n" );

    for_less( x, 0, sizes[X] )
    for_less( y, 0, sizes[Y] )
    for_less( z, 0, sizes[Z] )
    {
        test_value = get_volume_real_value( cached_volume, x, y, z, 0, 0 );
        test_value /= 2.0;
        if( test_value < get_volume_real_min(cached_volume) )
            test_value = get_volume_real_min(cached_volume);
        set_volume_real_value( cached_volume, x, y, z, 0, 0, test_value );
    }

    print( "Done setting voxels.\n" );

    delete_volume( cached_volume );

    if( input_volume( output_filename, 3, File_order_dimension_names,
                      NC_UNSPECIFIED, FALSE,
                      0.0, 0.0, TRUE, &cached_volume,
                      (minc_input_options *) NULL ) != OK )
        return( 1 );

    for_less( x, 0, sizes[X] )
    for_less( y, 0, sizes[Y] )
    for_less( z, 0, sizes[Z] )
    {
        true_value = get_volume_real_value( volume, x, y, z, 0, 0 );
        test_value = get_volume_real_value( cached_volume, x, y, z, 0, 0 );

        true_value = true_value / 2.0;
        if( true_value < get_volume_real_min( volume ) )
            true_value = get_volume_real_min( volume );
        true_value = CONVERT_VALUE_TO_VOXEL( volume, true_value );
        true_value = CONVERT_VOXEL_TO_VALUE( volume, ROUND(true_value) );
        if( true_value != test_value )
            print( "%d %d %d:  %g != %g\n", x, y, z, true_value, test_value );
    }

    delete_volume( volume );
    delete_volume( cached_volume );

    output_alloc_to_file( NULL );

    return( 0 );
}
