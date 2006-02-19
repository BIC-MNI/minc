/* Create a grid transform from an existing MINC volume.
 */

#include <stdio.h>
#include <volume_io.h>
#include "time_stamp.h"


int main( int ac, char* av[] )
{
    Volume v;
    minc_input_options mio;
    General_transform t;
    Status st;

    if ( ac != 3 ) {
	fprintf( stderr, "usage: %s in_grid.mnc out.xfm\n", av[0] );
	return 1;
    }

    /* The displacement volume must retain the vector dimension,
     * so we turn off "vector -> scalar" conversion.
     */
    set_default_minc_input_options( &mio );
    set_minc_input_vector_to_scalar_flag( &mio, 0 );

    st = input_volume( av[1],
		       0, NULL, 
		       MI_ORIGINAL_TYPE, FALSE, 0.0, 0.0, 
		       TRUE, &v,
		       &mio );
    if ( st != OK ) { 
	fprintf( stderr, "failed to read grid volume \"%s\"\n", av[1] );
	return 1;
    }

    create_grid_transform( &t, v );
    st = output_transform_file( av[2], time_stamp(ac,av), &t );

    if ( st != OK ) {
	fprintf( stderr, "error writing to xfm file \"%s\"\n", av[2] );
	return 1;
    }

    return 0;
}
