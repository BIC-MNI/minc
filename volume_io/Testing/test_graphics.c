
#include  <def_graphics.h>

#define  LIGHT_INDEX  0

main()
{
    Status            status;
    window_struct     *window;
    lines_struct      lines;
    polygons_struct   polygons;
    static Surfprop   spr = { 0.4, 0.5, 0.5, 40.0, 1.0 };
    Point             point;
    Vector            normal, light_direction;
    Status            initialize_lines();
    Status            begin_adding_points_to_line();
    Status            add_point_to_line();
    Status            delete_lines();
    Status            initialize_polygons();
    Status            begin_adding_points_to_polygon();
    Status            add_point_to_polygon();
    Status            delete_polygons();

    status = G_create_window( "Test Window", -1, -1, -1, -1, &window );

    G_set_view_type( window, MODEL_VIEW );

    /* ------------ define line to be drawn  ------------- */

    status = initialize_lines( &lines, make_Colour(255,255,0) );

    status = begin_adding_points_to_line( &lines );

    fill_Point( point, -0.2, -0.32, 0.0 );
    status = add_point_to_line( &lines, &point );

    fill_Point( point, 0.2, -0.32, 0.0 );
    status = add_point_to_line( &lines, &point );

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

    /* ------------ draw objects ------------- */

    G_draw_lines( window, &lines );
    G_draw_polygons( window, &polygons );
    G_update_window( window );

    status = delete_lines( &lines );

    status = delete_polygons( &polygons );

    status = G_delete_window( window );

    return( status != OK );
}
