#include  <internal_volume_io.h>

#define  X_SIZE   50
#define  Y_SIZE   50
#define  Z_SIZE   50

#define  N_ITERATIONS  10000

int  main(
    int   argc,
    char  *argv[] )
{
    int                  iter, n_iters = N_ITERATIONS;
    Status               status;
    char                 *output_prefix;
    STRING               transform_filename;
    int                  ind[MAX_DIMENSIONS], sizes[MAX_DIMENSIONS];
    static  char         *dim_names[] = { MIxspace, MIyspace, MIzspace,
                                          MIvector_dimension };
    Real                 value, x, y, z, xt, yt, zt;
    Volume               volume;
    General_transform    transform, linear;
    Real                 start_time, end_time;

    if( argc < 2 )
    {
        print( "Args.\n" );
        return( 1 );
    }

    output_prefix = argv[1];
    (void) sprintf( transform_filename, "%s", output_prefix );

    volume = create_volume( 4, dim_names, NC_SHORT, TRUE, 0.0, 0.0 );

    sizes[X] = X_SIZE;
    sizes[Y] = Y_SIZE;
    sizes[Z] = Z_SIZE;
    sizes[Z+1] = 3;
    set_volume_sizes( volume, sizes );

    set_volume_real_range( volume, -100.0, 100.0 );

    alloc_volume_data( volume );

    for_less( ind[X], 0, sizes[X] )
    {
        for_less( ind[Y], 0, sizes[Y] )
        {
            for_less( ind[Z], 0, sizes[Z] )
            {
                for_less( ind[Z+1], 0, sizes[Z+1] )
                {
                    value = (Real) ind[ind[Z+1]] / (Real) sizes[ind[Z+1]] - 0.5;
                    value = -10.0 * value;
                    value = CONVERT_VALUE_TO_VOXEL( volume, value );
                    SET_VOXEL( volume, ind[X], ind[Y], ind[Z], ind[Z+1], 0,
                               value );
                }
            }
        }
    }

    create_linear_transform( &linear, NULL );
    create_grid_transform( &transform, volume );

    status = output_transform_file( transform_filename, NULL, &transform );

    print( "Enter x, y, z: " );
    while( input_real( stdin, &x ) == OK &&
           input_real( stdin, &y ) == OK &&
           input_real( stdin, &z ) == OK )
    {
        start_time = current_cpu_seconds();
        for_less( iter, 0, n_iters )
            general_transform_point( &linear, x, y, z, &xt, &yt, &zt );
        end_time = current_cpu_seconds();

        print( "Linear: %g microseconds\n", 1.0e6 * (end_time - start_time) /
                                       (Real) n_iters );

        start_time = current_cpu_seconds();
        for_less( iter, 0, n_iters )
            general_transform_point( &transform, x, y, z, &xt, &yt, &zt );
        end_time = current_cpu_seconds();

        print( "Forward: %g %g %g -> %g %g %g\n", x, y, z, xt, yt, zt );
        print( "%g microseconds\n", 1.0e6 * (end_time - start_time) /
                                       (Real) n_iters );

        x = xt;
        y = yt;
        z = zt;

        start_time = current_cpu_seconds();
        for_less( iter, 0, n_iters )
            general_inverse_transform_point( &transform, x, y, z, &xt, &yt,&zt);
        end_time = current_cpu_seconds();

        print( "Inverse:          -> %g %g %g\n", xt, yt, zt );
        print( "%g microseconds\n", 1.0e6 * (end_time - start_time) /
                                       (Real) n_iters );

        print( "Enter x, y, z: " );
    }

    delete_volume( volume );
    delete_general_transform( &transform );
    delete_general_transform( &linear );

    output_alloc_to_file( "alloc.out" );

    return( status != OK );
}
