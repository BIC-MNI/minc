#include  <volume_io.h>

int  main(
    int   argc,
    char  *argv[] )
{
    int                 i, a, n_dims, v, n_volumes, sizes[MAX_DIMENSIONS];
    char                *input_filename, *output_filename, *string;
    char                *in_dim_names[MAX_DIMENSIONS];
    char                *out_dim_names[MAX_DIMENSIONS];
    Real                amount_done;
    Real                real_min, real_max;
    STRING              dim_names[MAX_DIMENSIONS];
    Volume              volume;
    General_transform   transform;
    Minc_file           in_file, out_file;
    minc_output_options options;
    volume_input_struct volume_input;
    nc_type             data_type;
    BOOLEAN             signed_flag;

    if( argc < 3 )
    {
        print( "1: Arguments?\n" );
        return( 1 );
    }

    a = 1;
    input_filename = argv[a];
    ++a;

    n_dims = 0;
    while( a < argc )
    {
        string = argv[a];
        ALLOC( in_dim_names[n_dims], MAX_STRING_LENGTH );

        if( strcmp( string, "x" ) == 0 || strcmp( string, "X" ) == 0 )
            (void) strcpy( in_dim_names[n_dims], MIxspace );
        else if( strcmp( string, "y" ) == 0 || strcmp( string, "Y" ) == 0 )
            (void) strcpy( in_dim_names[n_dims], MIyspace );
        else if( strcmp( string, "z" ) == 0 || strcmp( string, "Z" ) == 0 )
            (void) strcpy( in_dim_names[n_dims], MIzspace );
        else
            break;
        ++n_dims;
        ++a;
    }

    if( a >= argc )
    {
        print( "2: Arguments?\n" );
        return( 1 );
    }

    output_filename = argv[a];
    ++a;

    if( a > argc-N_DIMENSIONS )
    {
        print( "3: Arguments?\n" );
        return( 1 );
    }
    else
    {
        /*--- determine the output order */

        for_less( i, 0, N_DIMENSIONS )
        {
            string = argv[a];
            ++a;

            if( strcmp( string, "x" ) == 0 || strcmp( string, "X" ) == 0 )
                (void) strcpy( dim_names[i], MIxspace );
            else if( strcmp( string, "y" ) == 0 || strcmp( string, "Y" ) == 0 )
                (void) strcpy( dim_names[i], MIyspace );
            else if( strcmp( string, "z" ) == 0 || strcmp( string, "Z" ) == 0 )
                (void) strcpy( dim_names[i], MIzspace );
            else
                (void) strcpy( dim_names[i], string );

            ALLOC( out_dim_names[i], strlen( dim_names[i] ) + 1 );
            (void) strcpy( out_dim_names[i], dim_names[i] );
        }
    }

    /*--- open and close volume as 3D to get correct transform */

    (void) start_volume_input( input_filename, N_DIMENSIONS,
                               out_dim_names, NC_UNSPECIFIED, FALSE, 0.0, 0.0,
                               TRUE, &volume, (minc_input_options *) NULL,
                               &volume_input );

    copy_general_transform( get_voxel_to_world_transform(volume),
                            &transform );
    get_volume_sizes( volume, sizes );

    cancel_volume_input( volume, &volume_input );

    /*--- input the volume */

    volume = create_volume( n_dims, in_dim_names, NC_UNSPECIFIED, FALSE,
                            0.0, 0.0 );

    in_file = initialize_minc_input( input_filename, volume,
                                     (minc_input_options *) NULL );

    n_volumes = get_n_input_volumes( in_file );
    set_default_minc_output_options( &options );
    set_minc_output_dimensions_order( &options, get_volume_n_dimensions(volume),
                                      dim_names );

    get_volume_real_range( volume, &real_min, &real_max );
    set_minc_output_real_range( &options, real_min, real_max );

    data_type = get_volume_nc_data_type( volume, &signed_flag );

    out_file = initialize_minc_output( output_filename, 3, dim_names,
                                       sizes, data_type, signed_flag,
                                       get_volume_voxel_min(volume),
                                       get_volume_voxel_max(volume),
                                       &transform, volume, &options );

    (void) copy_auxiliary_data_from_minc_file( out_file, input_filename,
                                               "Axes reordered." );

    for_less( v, 0, n_volumes )
    {
        while( input_more_minc_file( in_file, &amount_done ) )
        {}

        (void) advance_input_volume( in_file );

        if( output_minc_volume( out_file ) != OK )
            return( 1 );

        if( n_volumes > 0 && (v+1) % 10 == 0 )
            print( "Done %d out of %d\n", v+1, n_volumes );
    }

    (void) close_minc_input( in_file );
    (void) close_minc_output( out_file );

    return( 0 );
}
