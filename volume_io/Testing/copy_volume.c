#include  <internal_volume_io.h>

int  main(
    int   argc,
    char  *argv[] )
{
    Status         status;
    int            sizes[MAX_DIMENSIONS];
    Real           separations[MAX_DIMENSIONS];
    Real           min_value, max_value;
    int            v1, v2, v3, v4, v5;
    Real           value;
    Volume         volume;
    char           *input_filename, *output_filename;
    Minc_file      file;
    static STRING  dim_names[3] = { MIzspace, MIyspace, MIxspace };
    minc_input_options   options;

    input_filename = argv[1];
    output_filename = argv[2];

    if( argc >= 4 )
    {
        (void) strcpy( dim_names[0], MIxspace );
        (void) strcpy( dim_names[1], MIyspace );
        (void) strcpy( dim_names[2], MIzspace );
    }

    set_default_minc_input_options( &options );

    if( input_volume( input_filename, 3, dim_names,
                      NC_UNSPECIFIED, FALSE, 0.0, 0.0,
                      TRUE, &volume, &options ) != OK )
        return( 1 );

    get_volume_sizes( volume, sizes );
    get_volume_separations( volume, separations );
    get_volume_real_range( volume, &min_value, &max_value );

    print( "Volume %s: %d by %d by %d\n",
            input_filename, sizes[X], sizes[Y], sizes[Z] );
    print( "Thickness: %g %g %g\n",
            separations[X], separations[Y], separations[Z] );
    print( "%g %g\n", min_value, max_value );

    file = initialize_minc_output( output_filename, 3, dim_names, sizes,
                                   NC_UNSPECIFIED, FALSE, 0.0, 0.0,
                                   &volume->voxel_to_world_transform,
                                   volume, NULL );

    status = copy_auxiliary_data_from_minc_file( file, input_filename,
                                                 NULL );

    if( status == OK )
        status = output_minc_volume( file );

    if( status == OK )
        status = close_minc_output( file );

    return( status != OK );
}
