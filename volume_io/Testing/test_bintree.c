#include  <mni.h>

int  main(
    int    argc,
    char   *argv[] )
{
    Status         status;
    char           *input_filename;
    int            i, n_objects, max_nodes;
    File_formats   format;
    object_struct  **object_list;

    status = OK;

    if( argc == 1 )
    {
        (void) fprintf( stderr, "Must have a filename argument.\n" );
        status = ERROR;
    }

    if( status == OK )
    {
        input_filename = argv[1];

        status = input_graphics_file( input_filename, &format, &n_objects,
                                      &object_list );

        if( status == OK )
            print( "Objects input.\n" );
    }

    if( argc <= 2 || sscanf( argv[2], "%d", &max_nodes ) != 1 )
        max_nodes = 10000;

    if( status == OK && n_objects > 0 &&
        object_list[0]->object_type == POLYGONS )
    {
        create_polygons_bintree( get_polygons_ptr(object_list[0]), max_nodes );
    }

    if( status == OK )
        delete_object_list( n_objects, object_list );

    return( status != OK );
}
