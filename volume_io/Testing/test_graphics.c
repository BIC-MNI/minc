
#include  <def_graphics.h>
#include  <def_alloc.h>

#define  LIGHT_INDEX       0

#define  UPDATE_INTERVAL                     0.1
#define  MIN_TIME_BETWEEN_EVENT_PROCESSING   0.01

main()
{
    Status            status;
    window_struct     *window, *event_window;
    text_struct       text;
    lines_struct      lines;
    polygons_struct   polygons;
    static Surfprop   spr = { 0.4, 0.5, 0.0, 20.0, 1.0 };
    Point             point;
    Vector            normal, light_direction;
    Event_types       event_type;
    Boolean           update_required, done;
    Status            initialize_text();
    Status            initialize_lines();
    Status            begin_adding_points_to_line();
    Status            add_point_to_line();
    Status            delete_lines();
    Status            initialize_polygons();
    Status            begin_adding_points_to_polygon();
    Status            add_point_to_polygon();
    Status            delete_polygons();
    int               key_pressed;
    int               mouse_x, mouse_y;
    int               current_mouse_x, current_mouse_y;
    int               x_position, y_position, x_size, y_size;
    Real              x, y;
    Real              time_since_last_events;
    Real              current_time, end_event_processing, previous_event_time;
    Real              current_realtime_seconds();
    void              sleep_program();
    Transform         modeling_transform;
    Transform         rotate_clockwise_transform;
    Transform         rotate_counter_clockwise_transform;
    void              make_identity_transform();
    void              make_rotation_transform();
    void              concat_transforms();
    static Point      origin = { 0.0, 0.0, 2.0 };
    static Vector     up_direction = { 0.0, 1.0, 0.0 };
    static Vector     line_of_sight = { 0.0, 0.0, -1.0 };

    status = G_create_window( "Test Window", -1, -1, -1, -1, &window );

    G_set_3D_view( window, &origin, &line_of_sight, &up_direction,
                   0.01, 4.0, ON, 2.0, 2.0, 2.0 );

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

    /* ------------ define polygons to be drawn  ------------- */

    status = initialize_polygons( &polygons, make_Colour(0,255,255), &spr );

    status = begin_adding_points_to_polygon( &polygons );

    fill_Point( point, -0.3, -0.3, 0.0 );
    fill_Vector( normal, -1.0, -1.0, 1.0 );
    status = add_point_to_polygon( &polygons, &point, &normal );
    fill_Point( point, 0.3, -0.3, 0.0 );
    fill_Vector( normal, 1.0, -1.0, 1.0 );
    status = add_point_to_polygon( &polygons, &point, &normal );
    fill_Point( point, 0.3, 0.3, 0.0 );
    fill_Vector( normal, 1.0, 1.0, 1.0 );
    status = add_point_to_polygon( &polygons, &point, &normal );
    fill_Point( point, -0.3, 0.3, 0.0 );
    fill_Vector( normal, -1.0, 1.0, 1.0 );
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

    make_identity_transform( &modeling_transform );
    make_rotation_transform( 5.0 * DEG_TO_RAD, Y, &rotate_clockwise_transform );
    make_rotation_transform( -5.0 * DEG_TO_RAD, Y,
                             &rotate_counter_clockwise_transform );

    update_required = TRUE;
    done = FALSE;

    (void) printf( "Hit middle mouse button to exit\n" );

    previous_event_time = -1000.0;

    do
    {
        /* to avoid using cpu time to do a busy wait, go to sleep before
           processing events */

        current_time = current_realtime_seconds();
        time_since_last_events = current_time - previous_event_time;

        if( time_since_last_events < MIN_TIME_BETWEEN_EVENT_PROCESSING )
        {
            sleep_program( MIN_TIME_BETWEEN_EVENT_PROCESSING -
                           time_since_last_events );
        }

        /* process events for up to UPDATE_INTERVAL seconds */

        end_event_processing = current_time + UPDATE_INTERVAL;

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
                    (void) printf( "Left mouse DOWN\n" );
                    concat_transforms( &modeling_transform, &modeling_transform,
                                       &rotate_clockwise_transform );
                    G_set_modeling_transform( window, &modeling_transform );
                    update_required = TRUE;
                    break;

                case LEFT_MOUSE_UP_EVENT:
                    (void) printf( "Left mouse UP\n" );
                    break;

                case RIGHT_MOUSE_DOWN_EVENT:
                    (void) printf( "Right mouse DOWN\n" );
                    concat_transforms( &modeling_transform, &modeling_transform,
                                       &rotate_counter_clockwise_transform );
                    G_set_modeling_transform( window, &modeling_transform );
                    update_required = TRUE;
                    break;

                case RIGHT_MOUSE_UP_EVENT:
                    (void) printf( "Right mouse UP\n" );
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
        }
        while( event_type != NO_EVENT &&
               current_realtime_seconds() < end_event_processing );

        previous_event_time = current_realtime_seconds();

        /* if one or more events caused an update, redraw the screen */

        if( update_required )
        {
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

    status = G_delete_window( window );

    return( status != OK );
}
