#include  <volume_io.h>

int  main(
    int   argc,
    char  *argv[] )
{
    char    *tag_filename;
    FILE    *file;
    Status  status;
    int     n_volumes;
    int     n_tag_points;
    double  **tags1;
    double  **tags2;
    Real    *weights;
    int     *structure_ids;
    int     *patient_ids;
    char    **labels;


    initialize_argument_processing( argc, argv );

    if( !get_string_argument( "", &tag_filename ) )
        return( 0 );

    status = open_file_with_default_suffix( tag_filename,
                                            get_default_tag_file_suffix(),
                                            READ_FILE, ASCII_FORMAT, &file );

    if( status == OK )
        status = input_tag_points( file, &n_volumes, &n_tag_points,
                                   &tags1, &tags2, &weights, &structure_ids,
                                   &patient_ids, &labels );

    if( status == OK )
        status = close_file( file );

    if( status == OK )
    {
        print( "N volumes: %d\n", n_volumes );
        print( "N tags   : %d\n", n_tag_points );
    }

    if( status == OK )
    {
        free_tag_points( n_volumes, n_tag_points, tags1, tags2, weights,
                         structure_ids, patient_ids, labels );
    }

    return( status != OK );
}
