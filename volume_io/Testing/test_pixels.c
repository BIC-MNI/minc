
#include  <def_mni.h>

#define  N_ITER   200

#undef  COLOUR_MAP_MODE
#define  COLOUR_MAP_MODE

#define  COLOUR_8BIT
#undef   COLOUR_8BIT

#define  COLOUR_MAP_INDEX_OFFSET  0

main()
{
    int               iter;
    Status            status;
    window_struct     *window;
    pixels_struct     pixels;
#ifdef  COLOUR_8BIT
    Pixel_types       pixel_type = COLOUR_INDEX_8BIT_PIXEL;
#else
    Pixel_types       pixel_type = COLOUR_INDEX_16BIT_PIXEL;
#endif
    int               i, j, pixels_x_size, pixels_y_size;
    Real              start, end;
#ifndef  COLOUR_MAP_MODE
    Colour            colour_table[256];
#endif

    status = G_create_window( "Test Pixels", -1, -1, 500, 500, &window );

    /* ------------ define pixels to be drawn  ------------- */

    pixels_x_size = 256;
    pixels_y_size = 256;

#ifdef COLOUR_MAP_MODE
    G_set_colour_map_state( window, ON );

    G_set_background_colour( window, 64 );

    G_set_colour_map_entry( window, NORMAL_PLANES, 64, make_Colour(60,60,60) );

    for_less( i, 0, 256 )
        G_set_colour_map_entry( window, NORMAL_PLANES,
                                i + COLOUR_MAP_INDEX_OFFSET,
                                make_Colour(i,i,i) );

    status = initialize_pixels( &pixels, pixels_x_size, pixels_y_size,
                                pixel_type );

    start = current_realtime_seconds();

    for_less( iter, 0, N_ITER )
    for_less( i, 0, pixels_x_size )
    {
        for_less( j, 0, pixels_y_size )
        {
#ifdef COLOUR_8BIT
            PIXEL_COLOUR_INDEX_8(pixels,i,j) = i % 256 + COLOUR_MAP_INDEX_OFFSET;
#else
            PIXEL_COLOUR_INDEX_16(pixels,i,j) = i % 256 +
                                                COLOUR_MAP_INDEX_OFFSET;
#endif
        }
    }

    end = current_realtime_seconds();

    (void) printf( "%g seconds\n", (end - start) / (Real) N_ITER );

#else
    status = initialize_pixels( &pixels, pixels_x_size, pixels_y_size,
                                RGB_PIXEL );

    for_less( i, 0, 256 )
    {
        colour_table[i] = make_Colour( i, i, i );
    }

    start = current_realtime_seconds();

    for_less( iter, 0, N_ITER )
    for_less( i, 0, pixels_x_size )
    {
        for_less( j, 0, pixels_y_size )
        {
            PIXEL_RGB_COLOUR(pixels,i,j) = colour_table[j];
        }
    }
    end = current_realtime_seconds();

    (void) printf( "%g seconds\n", (end - start) / (Real) N_ITER );
#endif

    start = current_realtime_seconds();

    for_less( iter, 0, N_ITER )
        G_draw_pixels( window, 10, 10, COLOUR_MAP_INDEX_OFFSET, &pixels );

    end = current_realtime_seconds();

    G_update_window( window );

    (void) printf( "%g seconds\n", (end - start) / (Real) N_ITER );

    (void) getchar();

    status = delete_pixels( &pixels );

    status = G_delete_window( window );

    return( status != OK );
}
