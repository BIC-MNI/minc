#include  <internal_volume_io.h>

#define  STEP_RATIO  1.0

public  BOOLEAN  newton_root_find(
    int    n_dimensions,
    void   (*function) ( void *function_data,
                         Real parameters[],  Real values[], Real **derivatives),
    void   *function_data,
    Real   initial_guess[],
    Real   desired_values[],
    Real   solution[],
    Real   function_tolerance,
    Real   delta_tolerance,
    int    max_iterations )
{
    int       iter, dim;
    Real      *values, **derivatives, *delta, error, best_error, *position;
    BOOLEAN   success;

    ALLOC( position, n_dimensions );
    ALLOC( values, n_dimensions );
    ALLOC( delta, n_dimensions );
    ALLOC2D( derivatives, n_dimensions, n_dimensions );

    for_less( dim, 0, n_dimensions )
        position[dim] = initial_guess[dim];

    iter = 0;
    success = FALSE;
    best_error = 0.0;

    while( max_iterations < 0 || iter < max_iterations )
    {
        ++iter;

        (*function) ( function_data, position, values, derivatives );

        error = 0.0;
        for_less( dim, 0, n_dimensions )
        {
            values[dim] = desired_values[dim] - values[dim];
            error += ABS( values[dim] );
        }

        if( iter == 1 || error < best_error )
        {
            best_error = error;
            for_less( dim, 0, n_dimensions )
                solution[dim] = position[dim];

            if( error < function_tolerance )
            {
                success = TRUE;
                break;
            }
        }

        if( !solve_linear_system( n_dimensions, derivatives, values, delta ) )
            break;

        error = 0.0;
        for_less( dim, 0, n_dimensions )
        {
            position[dim] += STEP_RATIO * delta[dim];
            error += ABS( delta[dim] );
        }

        if( error < delta_tolerance )
        {
            success = TRUE;
            break;
        }
    }

    FREE( values );
    FREE( delta );
    FREE2D( derivatives );
    FREE( position );

    return( success );
}
