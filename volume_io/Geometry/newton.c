#include  <internal_volume_io.h>

public  BOOLEAN  newton_function_inversion(
    int    n_dimensions,
    void   (*function) ( void *function_data,
                         Real parameters[],  Real values[], Real **derivatives),
    void   *function_data,
    Real   initial_guess[],
    Real   desired_values[],
    Real   solution[],
    Real   tolerance,
    int    max_iterations )
{
    int    iter, dim;
    Real   *values, **derivatives, *delta, error;

    ALLOC( values, n_dimensions );
    ALLOC( delta, n_dimensions );
    ALLOC2D( derivatives, n_dimensions, n_dimensions );

    for_less( dim, 0, n_dimensions )
        solution[dim] = initial_guess[dim];

    iter = 0;
    error = tolerance;

    while( max_iterations < 0 || iter < max_iterations )
    {
        ++iter;

        (*function) ( function_data, solution, values, derivatives );

        error = 0.0;
        for_less( dim, 0, n_dimensions )
        {
            values[dim] = desired_values[dim] - values[dim];
            error += ABS( values[dim] );
        }

        if( error < tolerance )
            break;

        if( !solve_linear_system( n_dimensions, derivatives, values, delta ) )
            break;

        for_less( dim, 0, n_dimensions )
            solution[dim] += delta[dim];
    }

    FREE( values );
    FREE( delta );
    FREE2D( derivatives );

    return( error < tolerance );
}
