#include  <volume_io.h>

int  main(
    int   argc,
    char  *argv[] )
{
    int        v1, v2, v3, sizes[MAX_DIMENSIONS];
    Real       voxel_value, voxel_100;
    Volume     volume;

    /*--- input the volume */

    if( input_volume( "volume.mnc", 3, NULL, NC_UNSPECIFIED, FALSE,
            0.0, 0.0, TRUE, &volume,
            (minc_input_options *) NULL ) != OK )
        return( 1 );

    get_volume_sizes( volume, sizes );

    /*--- change all values over 100 to 100 */

    voxel_100 = CONVERT_VALUE_TO_VOXEL( volume, 100.0 );

    for( v1 = 0;  v1 < sizes[0];  ++v1 ) {
        for( v2 = 0;  v2 < sizes[1];  ++v2 ) {
            for( v3 = 0;  v3 < sizes[2];  ++v3 ) {
                GET_VALUE_3D( voxel_value, volume, v1, v2, v3 );

                if( voxel_value > 100.0 ) {
                    SET_VOXEL_3D( volume, v1, v2, v3, voxel_100 );
                }
            }
        }
    }

    /*--- output the modified volume */

    if( output_modified_volume( "output.mnc", NC_UNSPECIFIED,
             FALSE, 0.0, 0.0, volume, "volume.mnc",
             "Modified by clamping to 100",
             (minc_output_options *) NULL ) != OK )
        return( 1 );

    return( 0 );
}
