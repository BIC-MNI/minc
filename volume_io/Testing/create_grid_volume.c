#include  <internal_volume_io.h>

#define  X_SIZE   10
#define  Y_SIZE   10
#define  Z_SIZE   10

int  main(
    int   argc,
    char  *argv[] )
{
    Status               status;
    char                 *output_filename;
    int                  ind[MAX_DIMENSIONS], sizes[MAX_DIMENSIONS];
    static  char         *dim_names[] = { MIxspace, MIyspace, MIzspace,
                                          MIvector_dimension };
    Real                 value;
    Volume               volume;

    if( argc < 2 )
    {
        print( "Args.\n" );
        return( 1 );
    }

    output_filename = argv[1];

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
                    value = (Real) ind[ind[Z+1]] / (Real) sizes[ind[Z+1]];
                    value = 4.0 * value - 2.0;
                    value = CONVERT_VALUE_TO_VOXEL( volume, value );
                    SET_VOXEL( volume, ind[X], ind[Y], ind[Z], ind[Z+1], 0,
                               value );
                }
            }
        }
    }

    status = output_volume( output_filename, NC_UNSPECIFIED, FALSE, 0.0, 0.0,
                            volume, "", NULL );

    return( status != OK );
}
