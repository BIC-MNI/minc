
#include  <def_graphics.h>

main()
{
    Status         status;
    window_struct  *window;
    lines_struct   lines;
    Point          point;
    Status         initialize_lines();
    Status         begin_adding_points_to_line();
    Status         add_point_to_line();
    Status         delete_lines();

    status = G_create_window( "Test Window", -1, -1, -1, -1, &window );

    G_set_view_type( window, MODEL_VIEW );

    status = initialize_lines( &lines, make_Colour(255,255,128) );

    status = begin_adding_points_to_line( &lines );

    fill_Point( point, -0.2, -0.2, 0.0 );
    status = add_point_to_line( &lines, &point );

    fill_Point( point, 0.2, 0.2, 0.0 );
    status = add_point_to_line( &lines, &point );

    G_draw_lines( window, &lines );
    G_update_window( window );

    (void) printf( "Hit return: " );
    (void) getchar();

    status = delete_lines( &lines );

    status = G_delete_window( window );

    return( status != OK );
}
