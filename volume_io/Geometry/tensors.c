#include  <internal_volume_io.h>

private  void  multiply_matrices(
    int    x1,
    int    y1,
    Real   m1[],
    int    sa1,
    int    sb1,
    int    y2,
    Real   m2[],
    int    sa2,
    int    sb2,
    Real   prod[],
    int    sap,
    int    sbp )
{
    int   i, j, k;
    Real  sum, *m1_ptr, *m2_ptr, *prod_ptr;

    for_less( i, 0, x1 )
    {
        prod_ptr = &prod[i*sap];
        for_less( j, 0, y2 )
        {
            sum = 0.0;
            m1_ptr = &m1[i*sa1];
            m2_ptr = &m2[j*sb2];
            for_less( k, 0, y1 )
            {
                sum += (*m1_ptr) * (*m2_ptr);
                m1_ptr += sb1;
                m2_ptr += sa2;
            }
            *prod_ptr = sum;
            prod_ptr += sbp;
        }
    }
}

#define  MAX_DEGREE        4
#define  MAX_TOTAL_VALUES  4000

public  void  spline_tensor_product(
    int     n_dims,
    Real    positions[],
    int     degrees[],     /* [n_dims] */
    Real    *bases[],      /* [n_dims][degress[dim]*degrees[dim]] */
    int     n_values,
    Real    coefs[],       /* [n_values*degrees[0]*degrees[1]*...] */
    int     n_derivs[],    /* [n_dims] */
    Real    results[] )    /* [n_values*n_derivs[0]*n_derivs[1]*...] */
{
    int     i, deriv, d, k, total_values, src;
    int     ind, prev_ind;
    Real    us[MAX_DEGREE*MAX_DEGREE], weights[MAX_DEGREE*MAX_DEGREE];
    Real    *tmp_results[2], *r;
    Real    static_tmp_results[2*MAX_TOTAL_VALUES];

    /*--- check arguments */

    total_values = n_values;
    for_less( d, 0, n_dims )
    {
        if( degrees[d] < 2 || degrees[d] > MAX_DEGREE )
        {
            print(
               "spline_tensor_product: Degree %d is not be between 2 and %d\n",
               degrees[d], MAX_DEGREE );
            return;
        }
        total_values *= degrees[d];
    }

    if( total_values > MAX_TOTAL_VALUES )
    {
        print( "Spline size too large for static memory.\n" );
        return;
    }

    tmp_results[0] = &static_tmp_results[0];
    tmp_results[1] = &static_tmp_results[total_values];

    for_less( i, 0, total_values )
        tmp_results[0][i] = coefs[i];

    src = 0;

    /*--- do each dimension */

    for_less( d, 0, n_dims )
    {
        us[0] = 1.0;
        for_less( k, 1, degrees[d] )
            us[k] = us[k-1] * positions[d];

        ind = degrees[d];
        for_inclusive( deriv, 1, n_derivs[d] )
        {
            for_less( k, 0, deriv )
            {
                us[ind] = 0.0;
                ++ind;
            }
   
            prev_ind = IJ( deriv-1, deriv-1, degrees[d] );
            for_less( k, deriv, degrees[d] )
            {
                us[ind] = us[prev_ind] * (Real) k;
                ++ind;
                ++prev_ind;
            }
        }

        multiply_matrices( 1 + n_derivs[d], degrees[d], us, degrees[d], 1,
                           degrees[d], bases[d], degrees[d], 1,
                           weights, degrees[d], 1 );

        total_values = n_values;
        for_less( i, 0, d )
            total_values *= 1 + n_derivs[i];
        for_less( i, d+1, n_dims )
            total_values *= degrees[i];

        if( d == n_dims-1 )
            r = results;
        else
            r = tmp_results[1-src];

        multiply_matrices( 1 + n_derivs[d], degrees[d], weights, degrees[d], 1,
                           total_values, tmp_results[src], total_values, 1,
                           r, 1, 1 + n_derivs[d] );

        src = 1 - src;
    }
}
