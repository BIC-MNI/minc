#include  <internal_volume_io.h>

private  Real   linear_coefs[2][2] = {
                                           {  1.0,  0.0 },
                                           { -1.0,  1.0 }
                                      };
private  Real   quadratic_coefs[3][3] = {
                                           {  0.5,  0.5,  0.0 },
                                           { -1.0,  1.0,  0.0 },
                                           {  0.5, -1.0,  0.5 }
                                        };

private  Real   cubic_coefs[4][4] = {
                                        {  0.0,  1.0,  0.0,  0.0 },
                                        { -0.5,  0.0,  0.5,  0.0 },
                                        {  1.0, -2.5,  2.0, -0.5 },
                                        { -0.5,  1.5, -1.5,  0.5 }
                                    };

public  void  get_linear_spline_coefs(
    Real  **coefs )
{
    int    i, j;

    for_less( i, 0, 2 )
    for_less( j, 0, 2 )
        coefs[i][j] = linear_coefs[i][j];
}

public  void  get_quadratic_spline_coefs(
    Real  **coefs )
{
    int    i, j;

    for_less( i, 0, 3 )
    for_less( j, 0, 3 )
        coefs[i][j] = quadratic_coefs[i][j];
}

public  void  get_cubic_spline_coefs(
    Real  **coefs )
{
    int    i, j;

    for_less( i, 0, 4 )
    for_less( j, 0, 4 )
        coefs[i][j] = cubic_coefs[i][j];
}

public  Real  cubic_interpolate(
    Real   u,
    Real   v0,
    Real   v1,
    Real   v2,
    Real   v3 )
{
    Real   coefs[4], value;

    coefs[0] = v0;
    coefs[1] = v1;
    coefs[2] = v2;
    coefs[3] = v3;

    evaluate_univariate_interpolating_spline( u, 4, coefs, 0, &value );

    return( value );
}

public  void  evaluate_univariate_interpolating_spline(
    Real    u,
    int     degree,
    Real    coefs[],
    int     n_derivs,
    Real    derivs[] )
{
    evaluate_interpolating_spline( 1, &u, degree, 1, coefs, n_derivs, derivs );
}

public  void  evaluate_bivariate_interpolating_spline(
    Real    u,
    Real    v,
    int     degree,
    Real    coefs[],
    int     n_derivs,
    Real    derivs[] )
{
    Real   positions[2];

    positions[0] = u;
    positions[1] = v;

    evaluate_interpolating_spline( 2, positions, degree, 1, coefs,
                                   n_derivs, derivs );
}

public  void  evaluate_trivariate_interpolating_spline(
    Real    u,
    Real    v,
    Real    w,
    int     degree,
    Real    coefs[],
    int     n_derivs,
    Real    derivs[] )
{
    Real   positions[3];

    positions[0] = u;
    positions[1] = v;
    positions[2] = w;

    evaluate_interpolating_spline( 3, positions, degree, 1, coefs,
                                   n_derivs, derivs );
}

#define  MAX_DIMS  100

public  void  evaluate_interpolating_spline(
    int     n_dims,
    Real    parameters[],
    int     degree,
    int     n_values,
    Real    coefs[],
    int     n_derivs,
    Real    derivs[] )
{
    int    d, degrees[MAX_DIMS], n_derivs_list[MAX_DIMS];
    Real   *bases[MAX_DIMS];

    if( degree < 2 || degree > 4 )
    {
        print( "evaluate_interpolating_spline: invalid degree: %d\n", degree );
        return;
    }

    if( n_dims < 1 || n_dims > MAX_DIMS )
    {
        print( "evaluate_interpolating_spline: invalid n dims: %d\n", n_dims );
        return;
    }

    switch( degree )
    {
    case 2:   bases[0] = &linear_coefs[0][0];      break;
    case 3:   bases[0] = &quadratic_coefs[0][0];   break;
    case 4:   bases[0] = &cubic_coefs[0][0];       break;
    }

    for_less( d, 1, n_dims )
        bases[d] = bases[0];

    for_less( d, 0, n_dims )
    {
        degrees[d] = degree;
        n_derivs_list[d] = n_derivs;
    }

    spline_tensor_product( n_dims, parameters, degrees, bases, n_values, coefs,
                           n_derivs_list, derivs );
}
