#include  <volume_io.h>

int  main(
    int   argc,
    char  *argv[] )
{
    Volume               volume;
    int                  i, n_slices, sizes[MAX_DIMENSIONS];
    Real                 x, y, z, amount_done, value;
    Real                 voxel[MAX_DIMENSIONS];
    char                 *input_filename;
    Minc_file            file;
    static char          *dim_names[] = { MIxspace, MIyspace };

    if( argc < 2 )
    {
        print( "Need args.\n" );
        return( 1 );
    }

    input_filename = argv[1];

    volume = create_volume( 2, dim_names, NC_UNSPECIFIED, FALSE,
                            0.0, 0.0 );

    file = initialize_minc_input( input_filename, volume,
                                  (minc_input_options *) NULL );

    if( file == (Minc_file) NULL )
        return( 1 );

    n_slices = get_n_input_volumes( file );

    voxel[X] = 0.0;
    voxel[Y] = 0.0;

    for_less( i, 0, n_slices )
    {
        while( input_more_minc_file( file, &amount_done ) )
        {}

        (void) advance_input_volume( file );

        get_volume_sizes( volume, sizes );
        GET_VALUE_2D( value, volume, sizes[0] / 2, sizes[1] / 2 );

        convert_voxel_to_world( volume, voxel, &x, &y, &z );

        print( "Slice[%d]:  center value %g    ", i+1, value );
        print( "origin maps to %g %g %g\n", x, y, z );
    }

    (void) close_minc_input( file );

    return( 0 );
}
