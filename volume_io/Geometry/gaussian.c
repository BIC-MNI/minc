#ifndef lint
static char rcsid[] = "$Header";
#endif

#include  <internal_volume_io.h>

public  BOOLEAN  scaled_maximal_pivoting_gaussian_elimination(
    int   n,
    int   row[],
    Real  **a,
    int   n_values,
    Real  **solution )
{
    int       i, j, k, p, v, tmp;
    Real      *s, val, best_val, m, scale_factor;
    BOOLEAN   success;

    ALLOC( s, n );

    for_less( i, 0, n )
        row[i] = i;

    for_less( i, 0, n )
    {
        s[i] = ABS( a[i][0] );
        for_less( j, 1, n )
        {
            if( ABS(a[i][j]) > s[i] )
               s[i] = ABS(a[i][j]);
        }
    }

    success = TRUE;

    for_less( i, 0, n-1 )
    {
        p = i;
        best_val = a[row[i]][i] / s[row[i]];
        best_val = ABS( best_val );
        for_less( j, i+1, n )
        {
            val = a[row[j]][i] / s[row[j]];
            val = ABS( val );
            if( val > best_val )
            {
                best_val = val;
                p = j;
            }
        }

        if( a[row[p]][i] == 0.0 )
        {
            success = FALSE;
            break;
        }

        if( i != p )
        {
            tmp = row[i];
            row[i] = row[p];
            row[p] = tmp;
        }

        for_less( j, i+1, n )
        {
            m = a[row[j]][i] / a[row[i]][i];
            for_less( k, i+1, n )
                a[row[j]][k] -= m * a[row[i]][k];
            for_less( v, 0, n_values )
                solution[row[j]][v] -= m * solution[row[i]][v];
        }
    }

    if( success && a[row[n-1]][n-1] == 0.0 )
        success = FALSE;

    if( success )
    {
        for( i = n-1;  i >= 0;  --i )
        {
            for_less( j, i+1, n )
            {
                scale_factor = a[row[i]][j];
                for_less( v, 0, n_values )
                    solution[row[i]][v] -= scale_factor * solution[row[j]][v];
            }

            for_less( v, 0, n_values )
                solution[row[i]][v] /= a[row[i]][i];
        }
    }

    FREE( s );

    return( success );
}

private  BOOLEAN  scaled_maximal_pivoting_gaussian_elimination_real(
    int   n,
    Real  **coefs,
    int   n_values,
    Real  **values )
{
    int       i, j, v, *row;
    Real      **a, **solution;
    BOOLEAN   success;

    ALLOC( row, n );
    ALLOC2D( a, n, n );
    ALLOC2D( solution, n, n_values );

    for_less( i, 0, n )
    {
        for_less( j, 0, n )
            a[i][j] = coefs[i][j];
        for_less( v, 0, n_values )
            solution[i][v] = values[v][i];
    }

    success = scaled_maximal_pivoting_gaussian_elimination( n, row, a, n_values,
                                                            solution );

    if( success )
    {
        for_less( i, 0, n )
        {
            for_less( v, 0, n_values )
            {
                values[v][i] = solution[row[i]][v];
            }
        }
    }

    FREE2D( a );
    FREE2D( solution );
    FREE( row );

    return( success );
}

public  BOOLEAN  solve_linear_system(
    int   n,
    Real  **coefs,
    Real  values[],
    Real  solution[] )
{
    int       i;

    for_less( i, 0, n )
        solution[i] = values[i];

    return( scaled_maximal_pivoting_gaussian_elimination_real( n, coefs, 1,
                                                               &solution ) );
}

public  BOOLEAN  invert_square_matrix(
    int   n,
    Real  **matrix,
    Real  **inverse )
{
    Real      tmp;
    BOOLEAN   success;
    int       i, j;

    for_less( i, 0, n )
    {
        for_less( j, 0, n )
            inverse[i][j] = 0.0;
        inverse[i][i] = 1.0;
    }

    success = scaled_maximal_pivoting_gaussian_elimination_real( n, matrix,
                                                                 n, inverse );

    if( success )
    {
        for_less( i, 0, n-1 )
        {
            for_less( j, i+1, n )
            {
                tmp = inverse[i][j];
                inverse[i][j] = inverse[j][i];
                inverse[j][i] = tmp;
            }
        }
    }

    return( success );
}
