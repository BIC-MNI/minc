#include  <internal_volume_io.h>

private      void   function(
    Real   parameters[],
    Real   values[],
    Real   **derivatives );

#define  MAX_DIMS   3

private  int  n_dims;

int main(
    int   argc,
    char  *argv[] )
{
    int    d, arg;
    Real   x[MAX_DIMS], desired[MAX_DIMS], solution[MAX_DIMS];
    Real   actual[MAX_DIMS], **deriv;

    arg = 1;
    if( arg >= argc || sscanf( argv[arg++], "%d", &n_dims ) != 1 )
        n_dims = 1;

    for_less( d, 0, n_dims )
    {
        if( arg >= argc || sscanf( argv[arg++], "%lf", &x[d] ) != 1 )
            x[d] = 0.0;
    }

    for_less( d, 0, n_dims )
    {
        if( arg >= argc || sscanf( argv[arg++], "%lf", &desired[d] ) != 1 )
            desired[d] = 0.0;
    }

    ALLOC2D( deriv, n_dims, n_dims );

    function( x, actual, deriv );
    print( "Start: " );
    for_less( d, 0, n_dims )
        print( " %g", x[d] );
    print( "\n" );
    print( "Value: " );
    for_less( d, 0, n_dims )
        print( " %g", actual[d] );
    print( "\n" );

    if( invert_function( n_dims, &function, x, desired, solution, 1e-3, 1000 ) )
    {
        function( solution, actual, deriv );
        print( "Solution: " );
        for_less( d, 0, n_dims )
            print( " %g", solution[d] );
        print( "\n" );
        print( "Value: " );
        for_less( d, 0, n_dims )
            print( " %g", actual[d] );
        print( "\n" );
    }

    FREE2D( deriv );

    return( 0 );
}

private      void   function(
    Real   parameters[],
    Real   values[],
    Real   **derivatives )
{
    Real   x, y;
    Real   xc11 = 3.0, yc11 = 4.0, xc12 = 5.0, yc12 = 6.0;
    Real   xc21 = -3.0, yc21 = 1.0, xc22 = 7.0, yc22 = 2.0;

    if( n_dims == 1 )
    {
        values[0] = 20.0 - parameters[0] * 9.0 + parameters[0] * parameters[0];
        derivatives[0][0] = 9.0 + 2.0 * parameters[0];
    }
    else if( n_dims == 2 )
    {
        x = parameters[0];
        y = parameters[1];

        values[0] = (x - xc11) * (x - xc12) * (y - yc11) * (y - yc12);
        values[1] = (x - xc21) * (x - xc22) * (y - yc21) * (y - yc22);
        derivatives[0][0] = (x - xc11) * (y - yc11) * (y - yc12) +
                            (x - xc12) * (y - yc11) * (y - yc12);
        derivatives[0][1] = (x - xc11) * (x - xc12) * (y - yc11) +
                            (x - xc11) * (x - xc12) * (y - yc12);
        derivatives[1][0] = (x - xc21) * (y - yc21) * (y - yc22) +
                            (x - xc22) * (y - yc21) * (y - yc22);
        derivatives[1][1] = (x - xc21) * (x - xc22) * (y - yc21) +
                            (x - xc21) * (x - xc22) * (y - yc22);
    }
}
