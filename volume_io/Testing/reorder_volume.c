#include  <volume_io.h>

int  main(
    int   argc,
    char  *argv[] )
{
    int                 i, a, n_dims, v, n_volumes;
    char                *input_filename, *output_filename, *string;
    char                *in_dim_names[MAX_DIMENSIONS];
    Real                amount_done;
    STRING              dim_names[MAX_DIMENSIONS];
    Volume              volume;
    Minc_file           file;
    minc_output_options options, *options_ptr;

    if( argc < 3 )
    {
        print( "Arguments?\n" );
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
        print( "Arguments?\n" );
        return( 1 );
    }

    output_filename = argv[a];
    ++a;

    if( a >= argc-N_DIMENSIONS )
        options_ptr = NULL;
    else
    {
        /*--- determine the output order */

        options_ptr = &options;

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
        }
    }

    /*--- input the volume */

    volume = create_volume( n_dims, in_dim_names, NC_UNSPECIFIED, FALSE,
                            0.0, 0.0 );

    file = initialize_minc_input( input_filename, volume,
                                  (minc_input_options *) NULL );

    n_volumes = get_n_input_volumes( file );
    set_default_minc_output_options( &options );
    set_minc_output_dimensions_order( &options, get_volume_n_dimensions(volume),
                                      dim_names );

    for_less( v, 0, n_volumes )
    {
        while( input_more_minc_file( file, &amount_done ) )
        {}

        (void) advance_input_volume( file );

        if( output_modified_volume( output_filename, NC_UNSPECIFIED,
                 FALSE, 0.0, 0.0, volume, input_filename,
                 "Axes reordered.", options_ptr ) != OK )
            return( 1 );
    }

    (void) close_minc_input( file );

    return( 0 );
}
