#include  <internal_volume_io.h>

#define  X_SIZE  3
#define  Y_SIZE  3
#define  Z_SIZE  3

int  main(
    int   argc,
    char  *argv[] )
{
    Volume               volume;
    int                  x, y, z, sizes[N_DIMENSIONS];
    static char          *dim_names[] = { MIxspace, MIyspace, MIzspace };

    volume = create_volume( 3, dim_names, NC_FLOAT, FALSE, 0.0, 10.0 );

    sizes[X] = X_SIZE;
    sizes[Y] = Y_SIZE;
    sizes[Z] = Z_SIZE;

    set_volume_sizes( volume, sizes );

/*
    set_volume_real_range( volume, 0.0, 10.0 );
*/

    alloc_volume_data( volume );

    for_less( x, 0, sizes[X] )
    for_less( y, 0, sizes[Y] )
    for_less( z, 0, sizes[z] )
        set_volume_real_value( volume, x, y, z, 0, 0, (Real) (x + y + z) );

    if( output_volume( "float_volume.mnc", NC_FLOAT, FALSE, 0.0, 0.0,
                       volume, "float", (minc_output_options *) NULL ) != OK )
        return( 1 );


    return( 0 );
}
