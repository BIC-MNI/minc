#include  <mni.h>

int  main(
    int   argc,
    char  *argv[] )
{
    Volume     volume, new_volume;
    Status     status;
    Real       x_width, y_width, z_width, separations[MAX_DIMENSIONS];
    BOOLEAN    world_space;
    char       *input_filename, *output_filename, *history, *dummy;
    static     int  axis_ordering[] = { X, Y, Z };

    initialize_argument_processing( argc, argv );

    if( !get_string_argument( "", &input_filename ) ||
        !get_string_argument( "", &output_filename ) )
    {
        print( "Need args.\n" );
        return( 1 );
    }

    (void) get_real_argument( 3.0, &x_width );
    (void) get_real_argument( 3.0, &y_width );
    (void) get_real_argument( 3.0, &z_width );

    world_space = get_string_argument( "", &dummy );

    status = input_volume( input_filename, 3, XYZ_dimension_names,
                      NC_UNSPECIFIED, FALSE, 0.0, 0.0,
                      TRUE, &volume, (minc_input_options *) NULL ) ;

    if( status != OK )
        return( 1 );

    if( world_space )
    {
        get_volume_separations( volume, separations );
        x_width /= ABS( separations[X] );
        y_width /= ABS( separations[Y] );
        z_width /= ABS( separations[Z] );
    }

    new_volume = create_box_filtered_volume( volume, NC_UNSPECIFIED, FALSE,
                                             0.0, 0.0,
                                             x_width, y_width, z_width );

    history = "box filtered";

    if( filename_extension_matches( output_filename, "mnc" ) )
        status = output_volume( output_filename, NC_UNSPECIFIED, FALSE,
                           0.0, 0.0,
                           new_volume, history, (minc_output_options *) NULL );
    else
        status =  output_volume_free_format( output_filename,
                                        new_volume, axis_ordering );


    return( 0 );
}
