#include  <volume_io.h>
#include  <splines.h>

static   Real   coefs[4][4][4];

private  void  check(
    int      continuity,
    Real     x,
    Real     y,
    Real     z );

int  main(
    int   argc,
    char  *argv[] )
{
    int                  i, j, k, continuity;
    Real                 x, y, z;

    set_random_seed( 13242239 );

    for_less( i, 0, 4)
    for_less( j, 0, 4)
    for_less( k, 0, 4)
        coefs[i][j][k] = get_random_0_to_1();

    initialize_argument_processing( argc, argv );
    (void) get_int_argument( 0, &continuity );

    print( "Enter x, y, z: " );
    while( input_real( stdin, &x ) == OK &&
           input_real( stdin, &y ) == OK &&
           input_real( stdin, &z ) == OK )
    {
        check( continuity, x, y, z );
        print( "\nEnter x, y, z: " );
    }

    return( 0 );
}

#define  TOLERANCE  0.01
#define  STEP       1.0e-4

private  Real  evaluate( 
    Real   u,
    Real   v,
    Real   w,
    int    continuity,
    Real   *dx,
    Real   *dy,
    Real   *dz,
    Real   *dxx,
    Real   *dxy,
    Real   *dxz,
    Real   *dyy,
    Real   *dyz,
    Real   *dzz )
{
    Real  value;
    Real  du00, du01, du10, du11, c00, c01, c10, c11, dv0, dv1, c0, c1, du0,du1;
    Real  c000, c001, c002, c003, c010, c011, c012, c013;
    Real  c020, c021, c022, c023, c030, c031, c032, c033;
    Real  c100, c101, c102, c103, c110, c111, c112, c113;
    Real  c120, c121, c122, c123, c130, c131, c132, c133;
    Real  c200, c201, c202, c203, c210, c211, c212, c213;
    Real  c220, c221, c222, c223, c230, c231, c232, c233;
    Real  c300, c301, c302, c303, c310, c311, c312, c313;
    Real  c320, c321, c322, c323, c330, c331, c332, c333;

    if( continuity == 0 )
    {
        c000 = coefs[0][0][0];
        c001 = coefs[0][0][1];
        c010 = coefs[0][1][0];
        c011 = coefs[0][1][1];
        c100 = coefs[1][0][0];
        c101 = coefs[1][0][1];
        c110 = coefs[1][1][0];
        c111 = coefs[1][1][1];

        du00 = c100 - c000;
        du01 = c101 - c001;
        du10 = c110 - c010;
        du11 = c111 - c011;

        c00 = c000 + u * du00;
        c01 = c001 + u * du01;
        c10 = c010 + u * du10;
        c11 = c011 + u * du11;

        dv0 = c10 - c00;
        dv1 = c11 - c01;

        c0 = c00 + v * dv0;
        c1 = c01 + v * dv1;

        value = INTERPOLATE( w, c0, c1 );

        if( dx != (Real *) 0 )
        {
            du0 = INTERPOLATE( v, du00, du10 );
            du1 = INTERPOLATE( v, du01, du11 );

            *dx = INTERPOLATE( w, du0, du1 );
            *dy = INTERPOLATE( w, dv0, dv1 );
            *dz = (c1 - c0);
        }
    }
    else if( continuity == 1 )
    {
        c000 = coefs[0][0][0];
        c001 = coefs[0][0][1];
        c002 = coefs[0][0][2];
        c010 = coefs[0][1][0];
        c011 = coefs[0][1][1];
        c012 = coefs[0][1][2];
        c020 = coefs[0][2][0];
        c021 = coefs[0][2][1];
        c022 = coefs[0][2][2];
        c100 = coefs[1][0][0];
        c101 = coefs[1][0][1];
        c102 = coefs[1][0][2];
        c110 = coefs[1][1][0];
        c111 = coefs[1][1][1];
        c112 = coefs[1][1][2];
        c120 = coefs[1][2][0];
        c121 = coefs[1][2][1];
        c122 = coefs[1][2][2];
        c200 = coefs[2][0][0];
        c201 = coefs[2][0][1];
        c202 = coefs[2][0][2];
        c210 = coefs[2][1][0];
        c211 = coefs[2][1][1];
        c212 = coefs[2][1][2];
        c220 = coefs[2][2][0];
        c221 = coefs[2][2][1];
        c222 = coefs[2][2][2];
        QUADRATIC_TRIVAR( c, u, v, w, value );

        if( dx != (Real *) 0 )
        {
            QUADRATIC_TRIVAR_DERIV( c, u, v, w, *dx, *dy, *dz );
        }

        if( dxx != (Real *) 0 )
        {
            QUADRATIC_TRIVAR_DERIV2( c, u, v, w, *dxx, *dxy, *dxz,
                                     *dyy, *dyz, *dzz );
        }
    }
    else if( continuity == 2 )
    {

        c000 = coefs[0][0][0];
        c001 = coefs[0][0][1];
        c002 = coefs[0][0][2];
        c003 = coefs[0][0][3];
        c010 = coefs[0][1][0];
        c011 = coefs[0][1][1];
        c012 = coefs[0][1][2];
        c013 = coefs[0][1][3];
        c020 = coefs[0][2][0];
        c021 = coefs[0][2][1];
        c022 = coefs[0][2][2];
        c023 = coefs[0][2][3];
        c030 = coefs[0][3][0];
        c031 = coefs[0][3][1];
        c032 = coefs[0][3][2];
        c033 = coefs[0][3][3];

        c100 = coefs[1][0][0];
        c101 = coefs[1][0][1];
        c102 = coefs[1][0][2];
        c103 = coefs[1][0][3];
        c110 = coefs[1][1][0];
        c111 = coefs[1][1][1];
        c112 = coefs[1][1][2];
        c113 = coefs[1][1][3];
        c120 = coefs[1][2][0];
        c121 = coefs[1][2][1];
        c122 = coefs[1][2][2];
        c123 = coefs[1][2][3];
        c130 = coefs[1][3][0];
        c131 = coefs[1][3][1];
        c132 = coefs[1][3][2];
        c133 = coefs[1][3][3];

        c200 = coefs[2][0][0];
        c201 = coefs[2][0][1];
        c202 = coefs[2][0][2];
        c203 = coefs[2][0][3];
        c210 = coefs[2][1][0];
        c211 = coefs[2][1][1];
        c212 = coefs[2][1][2];
        c213 = coefs[2][1][3];
        c220 = coefs[2][2][0];
        c221 = coefs[2][2][1];
        c222 = coefs[2][2][2];
        c223 = coefs[2][2][3];
        c230 = coefs[2][3][0];
        c231 = coefs[2][3][1];
        c232 = coefs[2][3][2];
        c233 = coefs[2][3][3];

        c300 = coefs[3][0][0];
        c301 = coefs[3][0][1];
        c302 = coefs[3][0][2];
        c303 = coefs[3][0][3];
        c310 = coefs[3][1][0];
        c311 = coefs[3][1][1];
        c312 = coefs[3][1][2];
        c313 = coefs[3][1][3];
        c320 = coefs[3][2][0];
        c321 = coefs[3][2][1];
        c322 = coefs[3][2][2];
        c323 = coefs[3][2][3];
        c330 = coefs[3][3][0];
        c331 = coefs[3][3][1];
        c332 = coefs[3][3][2];
        c333 = coefs[3][3][3];

        CUBIC_TRIVAR( c, u, v, w, value );

        if( dx != (Real *) 0 )
        {
            CUBIC_TRIVAR_DERIV( c, u, v, w, *dx, *dy, *dz );
        }

        if( dxx != (Real *) 0 )
        {
            CUBIC_TRIVAR_DERIV2( c, u, v, w, *dxx, *dxy, *dxz,
                                 *dyy, *dyz, *dzz );
        }
    }

    return( value );
}

private  Real  approx_deriv(
    int      continuity,
    Real     pos[N_DIMENSIONS],
    int      axis )
{
    Real  val1, val2;

    pos[axis] -= STEP;

    val1 = evaluate( pos[X], pos[Y], pos[Z], continuity,
                     (Real *) NULL, (Real *) NULL, (Real *) NULL,
                     (Real *) NULL, (Real *) NULL, (Real *) NULL,
                     (Real *) NULL, (Real *) NULL, (Real *) NULL );

    pos[axis] += 2.0 * STEP;

    val2 = evaluate( pos[X], pos[Y], pos[Z], continuity,
                     (Real *) NULL, (Real *) NULL, (Real *) NULL,
                     (Real *) NULL, (Real *) NULL, (Real *) NULL,
                     (Real *) NULL, (Real *) NULL, (Real *) NULL );

    pos[axis] -= STEP;

    return( (val2 - val1) / STEP / 2.0 );
}

private  Real  approx_deriv2(
    int      continuity,
    Real     pos[N_DIMENSIONS],
    int      a1,
    int      a2 )
{
    Real  val1, val2;

    pos[a2] -= STEP;
    val1 = approx_deriv( continuity, pos, a1 );
    pos[a2] += 2.0 * STEP;
    val2 = approx_deriv( continuity, pos, a1 );
    pos[a2] -= STEP;

    return( (val2 - val1) / STEP / 2.0 );
}

private  void  check(
    int      continuity,
    Real     x,
    Real     y,
    Real     z )
{
    Real   value, pos[N_DIMENSIONS];
    Real   deriv[N_DIMENSIONS], deriv2[N_DIMENSIONS][N_DIMENSIONS];
    Real   true_deriv[N_DIMENSIONS], true_deriv2[N_DIMENSIONS][N_DIMENSIONS];
    int    c, c2;

    value = evaluate( x, y, z, continuity,
                      &true_deriv[X], &true_deriv[Y], &true_deriv[Z],
                      &true_deriv2[X][X], &true_deriv2[X][Y],
                      &true_deriv2[X][Z], &true_deriv2[Y][Y],
                      &true_deriv2[Y][Z], &true_deriv2[Z][Z] );

    pos[X] = x;
    pos[Y] = y;
    pos[Z] = z;

    for_less( c, 0, N_DIMENSIONS )
    {
        deriv[c] = approx_deriv( continuity, pos, c );
        if( continuity > 0 )
            for_less( c2, c, N_DIMENSIONS )
                deriv2[c][c2] = approx_deriv2( continuity, pos, c, c2 );
    }

    for_less( c, 0, N_DIMENSIONS )
    {
        if( !numerically_close( deriv[c], true_deriv[c], TOLERANCE ) )
            print( "Deriv %d %g %g\n", c, deriv[c], true_deriv[c] );

        if( continuity > 0 )
            for_less( c2, c, N_DIMENSIONS )
                if( !numerically_close(deriv2[c][c2],
                                       true_deriv2[c][c2],TOLERANCE) )
                    print( "Deriv2 %d %d %g %g\n", c, c2, deriv2[c][c2],
                           true_deriv2[c][c2] );
    }

}
