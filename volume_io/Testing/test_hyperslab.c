#include  <internal_volume_io.h>
#include  <alloc.h>

static  int  get_random_int(
    int   n )
{
    static  unsigned long  c = 0;

    c = c * 31541 + 342544321;

    return( (int) (c % (unsigned long) n) );
}

static  void  usage(
    STRING   executable )
{
    STRING  usage_str = "\n\
Usage: %s  input.mnc [n_iterations_to_test [block_size]]\n\
\n\
\n\
    Tests the get_hyperslab aspect of volume_io.\n\n";

    print_error( usage_str, executable );
}

int  main(
    int   argc,
    char  *argv[] )
{
    STRING               volume_filename;
    int                  n_dims, sizes[MAX_DIMENSIONS], origin[MAX_DIMENSIONS];
    int                  block_size, block_sizes[MAX_DIMENSIONS];
    int                  to_array[MAX_DIMENSIONS];
    int                  iter, n_iterations, array_size, dim, i;
    float                avg;
    float                *data;
    Volume               volume;
    volume_input_struct  input_info;


    if( argc < 2 )
    {
        usage( argv[0] );
        return( 1 );
    }

    volume_filename = argv[1];

    if( argc < 3 || sscanf( argv[2], "%d", &n_iterations ) != 1 )
        n_iterations = 100;
    if( argc < 4 || sscanf( argv[3], "%d", &block_size ) != 1 )
        block_size = 10;


    if( start_volume_input( volume_filename, -1, File_order_dimension_names,
                            NC_FLOAT, FALSE,
                            0.0, 0.0,
                            TRUE, &volume, NULL, &input_info ) != OK )
        return( 1 );

    n_dims = get_volume_n_dimensions( volume );
    get_volume_sizes( volume, sizes );

    for_less( dim, 0, n_dims )
        block_sizes[dim] = MIN( block_size, sizes[dim] );

    array_size = 1;
    for_less( dim, 0, n_dims )
    {
        array_size *= block_sizes[dim];
        to_array[dim] = dim;
    }

    ALLOC( data, array_size );

    for_less( iter, 0, n_iterations )
    {
        for_less( dim, 0, n_dims )
            origin[dim] = get_random_int( sizes[dim] - block_sizes[dim] + 1 );

        print( "Reading %d: ", iter+1 );
        for_less( dim, 0, n_dims )
            print( " %d", origin[dim] );
        print( "  :  " );
        for_less( dim, 0, n_dims )
            print( " %d", block_sizes[dim] );

        if( input_minc_hyperslab( get_volume_input_minc_file(&input_info),
                                  FLOAT, n_dims, block_sizes,
                                  (void *) data, to_array,
                                  origin, block_sizes ) != OK )
        {
            print_error( "Error reading block[%d]:%d %d %d %d %d\n",
                         iter+1,
                         origin[0],
                         origin[1],
                         origin[2],
                         origin[3],
                         origin[4] );
        }

        avg = 0.0f;
        for_less( i, 0, array_size )
            avg += data[i];
        avg /= (float) array_size;
        print( "\tAvg value: %g\n", avg );
    }

    
    delete_volume_input( &input_info );

    return( 0 );
}
