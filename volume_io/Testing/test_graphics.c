
#include  <def_mni.h>

#define  LIGHT_INDEX       0

main()
{
    Status            status;
    window_struct     *window, *event_window;
    text_struct       text;
    lines_struct      lines;
    polygons_struct   polygons;
    pixels_struct     pixels;
    static Surfprop   spr = { 0.2, 0.5, 0.5, 20.0, 1.0 };
    Point             point, centre_of_rotation;
    Vector            normal, light_direction;
    Event_types       event_type;
    Boolean           update_required, done;
    int               key_pressed;
    int               mouse_x, mouse_y;
    int               current_mouse_x, current_mouse_y;
    int               x_position, y_position, x_size, y_size;
    int               x_pixel, y_pixel;
    Boolean           in_rotation_mode;
    int               prev_rotation_mouse_x;
    Real              angle_in_degrees;
    int               i, j, pixels_x_size, pixels_y_size;
    int               pixels_x_position, pixels_y_position;
    Real              x, y;
    Transform         modeling_transform, rotation_transform;
    static Point      origin = { 0.0, 0.0, 2.0 };
    static Vector     up_direction = { 0.0, 1.0, 0.0 };
    static Vector     line_of_sight = { 0.0, 0.0, -1.0 };

    status = G_create_window( "Test Window", -1, -1, -1, -1, &window );

    G_set_3D_view( window, &origin, &line_of_sight, &up_direction,
                   0.01, 4.0, ON, 2.0, 2.0, 2.0 );

    fill_Point( point, -0.3, 0.3, 0.0 );
    G_transform_point( window, &point, MODEL_VIEW, &x_pixel, &y_pixel );

    (void) printf( "(%g,%g,%g) maps to %d %d in pixels\n",
                   Point_x(point), Point_y(point), Point_z(point),
                   x_pixel, y_pixel );

    /* ------- define text to be drawn (text.string filled in later ----- */

    fill_Point( point, 10.0, 10.0, 0.0 );
    status = initialize_text( &text, &point, make_Colour(255,0,255),
                              SIZED_FONT, 14.0 );
    text.string[0] = (char) 0;

    /* ------------ define line to be drawn  ------------- */

    status = initialize_lines( &lines, make_Colour(255,255,0) );

    lines.n_points = 4;
    ALLOC( status, lines.points, 4 );
    fill_Point( lines.points[0], 0.0, 0.0, 0.0 );
    fill_Point( lines.points[1], 1.0, 1.0, 0.0 );
    fill_Point( lines.points[2], -0.3, 1.0, -1.0 );
    fill_Point( lines.points[3], 0.3, 1.0, 1.0 );

    lines.n_items = 3;
    ALLOC( status, lines.end_indices, lines.n_items );
    lines.end_indices[0] = 2;
    lines.end_indices[1] = 4;
    lines.end_indices[2] = 6;

    ALLOC( status, lines.indices, lines.end_indices[lines.n_items-1] );
    lines.indices[0] = 0;
    lines.indices[1] = 1;

    lines.indices[2] = 0;
    lines.indices[3] = 2;

    lines.indices[4] = 0;
    lines.indices[5] = 3;

    /* ------------ define pixels to be drawn  ------------- */

    pixels_x_size = 256;
    pixels_y_size = 256;

    status = initialize_pixels( &pixels, pixels_x_size, pixels_y_size,
                                RGB_PIXEL );

    for_less( i, 0, pixels_x_size )
    {
        for_less( j, 0, pixels_y_size )
        {
            PIXEL_RGB_COLOUR(pixels,i,j) = make_Colour( i % 256, j % 256, 0 );
        }
    }

    pixels_x_position = 10;
    pixels_y_position = 10;

    /* ------------ define polygons to be drawn  ------------- */

    status = initialize_polygons( &polygons, make_Colour(0,255,255), &spr );

    status = start_new_polygon( &polygons );

    fill_Point( point, -0.3, -0.3, 0.0 );
    fill_Vector( normal, 0.0, 0.0, 1.0 );
    status = add_point_to_polygon( &polygons, &point, &normal );
    fill_Point( point, 0.3, -0.3, 0.0 );
    status = add_point_to_polygon( &polygons, &point, &normal );
    fill_Point( point, 0.3, 0.3, 0.0 );
    status = add_point_to_polygon( &polygons, &point, &normal );
    fill_Point( point, -0.3, 0.3, 0.0 );
    status = add_point_to_polygon( &polygons, &point, &normal );

    /* ------------ define lights ----------------- */

    fill_Vector( light_direction, 1.0, 1.0, -1.0 );/* from over left shoulder */

    G_define_light( window, LIGHT_INDEX, DIRECTIONAL_LIGHT,
                    make_Colour(255,255,255),
                    &light_direction, (Point *) 0, 0.0, 0.0 );
    G_set_light_state( window, LIGHT_INDEX, ON );

    G_set_lighting_state( window, ON );

    /* --------------------------------------- */
    /* ------------ do main loop ------------- */
    /* --------------------------------------- */

    current_mouse_x = 0;
    current_mouse_y = 0;

    make_identity_transform( &modeling_transform );
    update_required = TRUE;

    in_rotation_mode = FALSE;

    (void) printf( "Hold down left button and move mouse to rotate\n" );
    (void) printf( "Hit middle mouse button to exit\n" );

    done = FALSE;

    do
    {
        do
        {
            event_type = G_get_event( &event_window, &key_pressed,
                                      &mouse_x, &mouse_y );

            if( event_window == window )
            {
                switch( event_type )
                {
                case KEYBOARD_EVENT:
                    (void) printf( "Key pressed: \"%c\"\n", key_pressed );
                    break;

                case LEFT_MOUSE_DOWN_EVENT:
                    prev_rotation_mouse_x = current_mouse_x;
                    in_rotation_mode = TRUE;
                    break;

                case LEFT_MOUSE_UP_EVENT:
                    in_rotation_mode = FALSE;
                    update_required = TRUE;
                    break;

                case RIGHT_MOUSE_DOWN_EVENT:
                    break;

                case RIGHT_MOUSE_UP_EVENT:
                    break;

                case MIDDLE_MOUSE_DOWN_EVENT:
                    (void) printf( "Middle mouse DOWN\n" );
                    done = TRUE;
                    break;

                case MIDDLE_MOUSE_UP_EVENT:
                    (void) printf( "Middle mouse UP\n" );
                    break;

                case MOUSE_MOVEMENT_EVENT:
                    current_mouse_x = mouse_x;
                    current_mouse_y = mouse_y;
                    G_convert_mouse_pixels_to_0_1( window,
                                               current_mouse_x, current_mouse_y,
                                               &x, &y );
                    (void) sprintf( text.string,
                                "Mouse: %4d,%4d pixels   %4.2f,%4.2f window",
                                current_mouse_x, current_mouse_y, x, y );
                    update_required = TRUE;
                    break;

                case WINDOW_RESIZE_EVENT:
                    G_get_window_position( window, &x_position, &y_position );
                    G_get_window_size( window, &x_size, &y_size );
                    (void) printf( "Window resized, moved, or popped." );
                    (void) printf( "  New position: %d %d   New size: %d %d\n",
                                   x_position, y_position, x_size, y_size );
                    update_required = TRUE;
                    break;
                }
            }
        }                  /* break to do update when no events */
        while( event_type != NO_EVENT );

        /* check if in rotation mode and moved mouse horizontally */

        if( in_rotation_mode && current_mouse_x != prev_rotation_mouse_x )
        {
            angle_in_degrees = (prev_rotation_mouse_x - current_mouse_x);

            make_rotation_transform( angle_in_degrees * DEG_TO_RAD, Y,
                                     &rotation_transform );

            fill_Point( centre_of_rotation, 0.3, 0.0, 0.0 );
            make_transform_relative_to_point( &centre_of_rotation,
                                              &rotation_transform,
                                              &rotation_transform );
            concat_transforms( &modeling_transform, &modeling_transform,
                               &rotation_transform );
            G_set_modeling_transform( window, &modeling_transform );

            prev_rotation_mouse_x = current_mouse_x;

            update_required = TRUE;
        }

        /* if one or more events caused an update, redraw the screen */

        if( update_required )
        {
            G_draw_pixels( window, pixels_x_position, pixels_y_position,
                           &pixels );

            G_set_view_type( window, MODEL_VIEW );
            G_draw_polygons( window, &polygons );
            G_draw_lines( window, &lines );

            G_set_view_type( window, PIXEL_VIEW );
            G_draw_text( window, &text );

            G_update_window( window );
            update_required = FALSE;
        }
    }
    while( !done );

    /* delete drawing objects and window (text does not need to be deleted */

    status = delete_lines( &lines );

    status = delete_polygons( &polygons );

    status = delete_pixels( &pixels );

    status = G_delete_window( window );

    return( status != OK );
}
