#include  <volume_io.h>

int  main(
    int   argc,
    char  *argv[] )
{
    int                 i, a;
    char                *input_filename, *output_filename, *string;
    STRING              dim_names[MAX_DIMENSIONS];
    Volume              volume;
    minc_output_options options, *options_ptr;

    if( argc < 3 )
    {
        print( "Arguments?\n" );
        return( 1 );
    }

    input_filename = argv[1];
    output_filename = argv[2];
    a = 3;

    /*--- input the volume */

    if( input_volume( input_filename, 3, XYZ_dimension_names,
            NC_UNSPECIFIED, FALSE,
            0.0, 0.0, TRUE, &volume, (minc_input_options *) NULL ) != OK )
        return( 1 );

    /*--- determine the output order */

    options_ptr = &options;
    for_less( i, 0, get_volume_n_dimensions(volume) )
    {
        if( a >= argc )
        {
            print( "Using default.\n" );
            options_ptr = NULL;
            break;
        }
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

    set_default_minc_output_options( &options );
    set_minc_output_dimensions_order( &options, get_volume_n_dimensions(volume),
                                      dim_names );

    if( output_modified_volume( output_filename, NC_UNSPECIFIED,
             FALSE, 0.0, 0.0, volume, input_filename,
             "Axes reordered.", options_ptr ) != OK )
        return( 1 );

    return( 0 );
}
