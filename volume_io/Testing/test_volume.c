#include  <internal_volume_io.h>

private  void  check(
    Volume   volume,
    int      continuity,
    Real     x,
    Real     y,
    Real     z );

int  main(
    int   argc,
    char  *argv[] )
{
    Volume               volume;
    int                  continuity, arg;
    Real                 x, y, z;
    char                 *input_filename;
    static char          *dim_names[] = { MIxspace, MIyspace, MIzspace };

    if( argc < 2 )
    {
        print( "Need args.\n" );
        return( 1 );
    }

    arg = 1;
    input_filename = argv[arg++];

    if( arg >= argc || sscanf( argv[arg++], "%d", &continuity ) != 1 )
        continuity = 0;

    if( input_volume( input_filename, 3, dim_names, NC_UNSPECIFIED, FALSE,
                      0.0, 0.0, TRUE, &volume,
                      (minc_input_options *) NULL ) != OK )
        return( 1 );

    print( "Enter x, y, z: " );
    while( input_real( stdin, &x ) == OK &&
           input_real( stdin, &y ) == OK &&
           input_real( stdin, &z ) == OK )
    {
        check( volume, continuity, x, y, z );
        print( "\nEnter x, y, z: " );
    }

    return( 0 );
}

#define  TOLERANCE  0.01
#define  STEP       1.0e-4

private  Real  approx_deriv(
    Volume   volume,
    int      continuity,
    Real     pos[N_DIMENSIONS],
    int      axis )
{
    Real  val1, val2, orig;

    orig = pos[axis];

    pos[axis] = orig - STEP;

    evaluate_volume_in_world( volume,
                          pos[X], pos[Y], pos[Z], continuity, 0.0, &val1,
                          (Real *) NULL, (Real *) NULL, (Real *) NULL,
                          (Real *) NULL, (Real *) NULL, (Real *) NULL,
                          (Real *) NULL, (Real *) NULL, (Real *) NULL );

    pos[axis] = orig + STEP;

    evaluate_volume_in_world( volume,
                          pos[X], pos[Y], pos[Z], continuity, 0.0, &val2,
                          (Real *) NULL, (Real *) NULL, (Real *) NULL,
                          (Real *) NULL, (Real *) NULL, (Real *) NULL,
                          (Real *) NULL, (Real *) NULL, (Real *) NULL );

    pos[axis] = orig;

    return( (val2 - val1) / STEP / 2.0 );
}

private  Real  approx_deriv2(
    Volume   volume,
    int      continuity,
    Real     pos[N_DIMENSIONS],
    int      a1,
    int      a2 )
{
    Real  val1, val2, orig;

    orig = pos[a2];

    pos[a2] = orig - STEP;
    val1 = approx_deriv( volume, continuity, pos, a1 );
    pos[a2] = orig + STEP;
    val2 = approx_deriv( volume, continuity, pos, a1 );
    pos[a2] = orig;

    return( (val2 - val1) / STEP / 2.0 );
}

public  BOOLEAN  numerically_close(
    Real  n1,
    Real  n2,
    Real  threshold_ratio )
{
    double  avg, diff;

    diff = n1 - n2;
    if( diff < 0.0 )  diff = -diff;

    if( n1 <= threshold_ratio && n1 >= -threshold_ratio &&
        n2 <= threshold_ratio && n2 >= -threshold_ratio )
    {
        return( diff <= threshold_ratio );
    }

    avg = (n1 + n2) / 2.0;

    if( avg == 0.0 )
        return( diff <= (double) threshold_ratio );

    if( avg < 0.0 )  avg = -avg;

    return( (diff / avg) <= (double) threshold_ratio );
}

private  void  check(
    Volume   volume,
    int      continuity,
    Real     x,
    Real     y,
    Real     z )
{
    Real   value, pos[N_DIMENSIONS];
    Real   deriv[N_DIMENSIONS], deriv2[N_DIMENSIONS][N_DIMENSIONS];
    Real   true_deriv[N_DIMENSIONS], true_deriv2[N_DIMENSIONS][N_DIMENSIONS];
    int    c, c2;

    pos[X] = x;
    pos[Y] = y;
    pos[Z] = z;

    evaluate_volume_in_world( volume,
                          x, y, z, continuity, 0.0, &value,
                          &true_deriv[X], &true_deriv[Y], &true_deriv[Z],
                          &true_deriv2[X][X], &true_deriv2[X][Y],
                          &true_deriv2[X][Z], &true_deriv2[Y][Y],
                          &true_deriv2[Y][Z], &true_deriv2[Z][Z] );

    for_less( c, 0, N_DIMENSIONS )
    {
        deriv[c] = approx_deriv( volume, continuity, pos, c );
        if( continuity > 0 )
            for_less( c2, c, N_DIMENSIONS )
                deriv2[c][c2] = approx_deriv2( volume, continuity, pos, c, c2 );
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
