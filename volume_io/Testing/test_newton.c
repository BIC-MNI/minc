#include  <internal_volume_io.h>

private      void   function(
    Real   parameters[],
    Real   values[],
    Real   **derivatives );

int main(
    int   argc,
    char  *argv[] )
{
    int    n_dims = 1, arg;
    Real   x, desired, solution, actual, **deriv;

    arg = 1;
    if( arg >= argc || sscanf( argv[arg++], "%lf", &x ) != 1 )
        x = 0.0;
    if( arg >= argc || sscanf( argv[arg++], "%lf", &desired ) != 1 )
        desired = 0.0;

    ALLOC2D( deriv, n_dims, n_dims );

    if( invert_function( 1, &function, &x, &desired, &solution, 1e-3, 1000 ) )
    {
        function( &solution, &actual, deriv );
        print( "Solution: %g = %g\n", solution, actual );
    }

    FREE2D( deriv );

    return( 0 );
}

private      void   function(
    Real   parameters[],
    Real   values[],
    Real   **derivatives )
{
    values[0] = 20.0 - parameters[0] * 9.0 + parameters[0] * parameters[0];
    derivatives[0][0] = 9.0 + 2.0 * parameters[0];
}
