#include  <volume_io.h>

int  main(
    int  argc,
    char *argv[] )
{
    char               *input_filename, *output_filename;
    char               *second_filename, *third_filename;
    Status             status;
    FILE               *file;
    static char        comments[] = "If this works, we'll be surprised.";
    General_transform  transform, second_transform;
    Real               x, y, z, xt, yt, zt;

    initialize_argument_processing( argc, argv );

    if( !get_string_argument( "", &input_filename ) ||
        !get_string_argument( "", &second_filename ) )
    {
        print( "Need input and output arguments.\n" );
        return( 1 );
    }

    if( get_string_argument( (char *) NULL, &third_filename ) )
        output_filename = third_filename;
    else
        output_filename = second_filename;

    status = open_file( input_filename, READ_FILE, ASCII_FORMAT, &file );

    if( status == OK )
        status = input_transform( file, &transform );

    if( status == OK )
        status = close_file( file );

    if( third_filename != (char *) NULL )
    {
        status = open_file( second_filename, READ_FILE, ASCII_FORMAT, &file );

        if( status == OK )
            status = input_transform( file, &second_transform );

        if( status == OK )
            status = close_file( file );

        concat_general_transforms( &transform, &second_transform, &transform );
    }

    status = open_file( output_filename, WRITE_FILE, ASCII_FORMAT, &file );

    if( status == OK )
        status = output_transform( file, comments, &transform );

    if( status == OK )
        status = close_file( file );

    print( "Enter x, y, z: " );
    while( input_real( stdin, &x ) == OK &&
           input_real( stdin, &y ) == OK &&
           input_real( stdin, &z ) == OK )
    {
        general_transform_point( &transform, x, y, z, &xt, &yt, &zt );
        print( "%g %g %g -> %g %g %g\n", x, y, z, xt, yt, zt );
        print( "Enter x, y, z: " );
    }

    return( status != OK );
}
