#define _GNU_SOURCE 1
#include <stdio.h>
#include <stdlib.h>

#include <volume_io.h>


Real tolerance = 1e-8;


int is_equal_real( Real e, Real a )
{
    return fabs(e-a) < tolerance;
}



/* Args: expected, actual.
 */
void assert_equal_point( Real ex, Real ey, Real ez,
			 Real ax, Real ay, Real az,
			 const char* msg )
{
    if ( is_equal_real(ex,ax) && 
	 is_equal_real(ey,ay) &&
	 is_equal_real(ez,az) )
	return;

    printf( "%s failure.\n"
	    "Expected: %f %f %f\n"
	    "  Actual: %f %f %f\n", 
	    msg, ex,ey,ez,  ax,ay,az );

    exit(3);
}



int main( int ac, char* av[] )
{
    int N;
    General_transform xfm;


    if ( ac != 3 && ac != 4 ) {
	fprintf( stderr, "usage: %s N transform.xfm [tolerance]\n", av[0] );
	return 1;
    }

    N = atoi( av[1] );
    if ( input_transform_file( av[2], &xfm ) != OK ) {
	fprintf( stderr, "Failed to load transform '%s'\n", av[2] );
	return 2;
    }

    if ( ac == 4 ) {
	tolerance = atof( av[3] );
	printf( "Setting tolerance to %f.\n", tolerance );
    }

    while (N-- > 0) {
	Real x = 500.0 * ( drand48() - 0.5 );
	Real y = 500.0 * ( drand48() - 0.5 );
	Real z = 500.0 * ( drand48() - 0.5 );

	Real tx,ty,tz;
	Real a,b,c;

	general_transform_point( &xfm,  x,y,z,  &tx,&ty,&tz );

	/* Check that general_inverse_transform_point() and
	   invert_general_transform() behave sensibly.
	*/
	general_inverse_transform_point( &xfm,  tx,ty,tz,  &a,&b,&c );
	assert_equal_point( x,y,z, a,b,c,
			    "general_inverse_transform_point()" );

	invert_general_transform( &xfm );

	general_transform_point( &xfm, tx,ty,tz,  &a,&b,&c );
	assert_equal_point( x,y,z, a,b,c,
			    "general_transform_point() / inverted xfm" );

	general_inverse_transform_point( &xfm,  x,y,z,  &a,&b,&c );
	assert_equal_point( tx,ty,tz, a,b,c,
			    "general_inverse_transform_point() / inverted xfm" );
    }

    return 0;
}

