#include  <volume_io.h>

/* ------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and
              McGill University make no representations about the
              suitability of this software for any purpose.  It is
              provided "as is" without express or implied warranty.
------------------------------------------------------------------ */

int  main(
    int   argc,
    char  *argv[] )
{
    int        v1, v2, v3, sizes[MAX_DIMENSIONS];
    Real       value;
    Volume     volume;

    /*--- input the volume */

    if( input_volume( "volume.mnc", 3, NULL, MI_ORIGINAL_TYPE, FALSE,
            0.0, 0.0, TRUE, &volume,
            (minc_input_options *) NULL ) != OK )
        return( 1 );

    get_volume_sizes( volume, sizes );

    /*--- change all values over 100 to 100 */

    for( v1 = 0;  v1 < sizes[0];  ++v1 ) {
        for( v2 = 0;  v2 < sizes[1];  ++v2 ) {
            for( v3 = 0;  v3 < sizes[2];  ++v3 ) {
                value = get_volume_real_value( volume, v1, v2, v3,
                                               0, 0 );

                if( value > 100.0 ) {
                    set_volume_real_value( volume, v1, v2, v3,
                                           0, 0, 100.0 );
                }
            }
        }
    }

    /*--- output the modified volume */

    if( output_modified_volume( "output.mnc", MI_ORIGINAL_TYPE,
             FALSE, 0.0, 0.0, volume, "volume.mnc",
             "Modified by clamping to 100",
             (minc_output_options *) NULL ) != OK )
        return( 1 );

    return( 0 );
}
