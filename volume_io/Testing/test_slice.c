#include  <def_mni.h>

private  void  check(
    Volume   volume,
    BOOLEAN  world_space,
    Real     x,
    Real     y,
    Real     z );

int  main(
    int   argc,
    char  *argv[] )
{
    Volume               volume;
    BOOLEAN              world_space;
    Real                 x, y, z;
    char                 *input_filename;
    static STRING        dim_names[] = { MIxspace, MIyspace, MIzspace };

    initialize_argument_processing( argc, argv );

    if( !get_string_argument( "", &input_filename ) )
    {
        print( "Need args.\n" );
        return( 1 );
    }

    world_space = (argc > 2);

    if( input_volume( input_filename, dim_names, FALSE, &volume ) != OK )
        return( 1 );

    print( "Enter x, y, z: " );
    while( input_real( stdin, &x ) == OK &&
           input_real( stdin, &y ) == OK &&
           input_real( stdin, &z ) == OK )
    {
        check( volume, world_space, x, y, z );
        print( "\nEnter x, y, z: " );
    }

    return( 0 );
}

#define  TOLERANCE  0.01
#define  STEP       1.0e-4

private  Real  approx_deriv(
    Volume   volume,
    BOOLEAN  world_space,
    Real     pos[N_DIMENSIONS],
    int      axis )
{
    Real  val1, val2, orig;

    orig = pos[axis];

    pos[axis] = orig - STEP;

    if( world_space )
    (void) evaluate_slice_in_world( volume,
                          pos[X], pos[Y], pos[Z], FALSE, &val1,
                          (Real *) NULL, (Real *) NULL,
                          (Real *) NULL, (Real *) NULL, (Real *) NULL );
    else
    (void) evaluate_slice( volume,
                          pos[X], pos[Y], pos[Z], FALSE, &val1,
                          (Real *) NULL, (Real *) NULL,
                          (Real *) NULL, (Real *) NULL, (Real *) NULL );

    pos[axis] = orig + STEP;

    if( world_space )
    (void) evaluate_slice_in_world( volume,
                          pos[X], pos[Y], pos[Z], FALSE, &val2,
                          (Real *) NULL, (Real *) NULL,
                          (Real *) NULL, (Real *) NULL, (Real *) NULL );
    else
    (void) evaluate_slice( volume,
                          pos[X], pos[Y], pos[Z], FALSE, &val2,
                          (Real *) NULL, (Real *) NULL,
                          (Real *) NULL, (Real *) NULL, (Real *) NULL );

    pos[axis] = orig;

    return( (val2 - val1) / STEP / 2.0 );
}

private  Real  approx_deriv2(
    Volume   volume,
    BOOLEAN  world_space,
    Real     pos[N_DIMENSIONS],
    int      a1,
    int      a2 )
{
    Real  val1, val2, orig;

    orig = pos[a2];

    pos[a2] = orig - STEP;
    val1 = approx_deriv( volume, world_space, pos, a1 );
    pos[a2] = orig + STEP;
    val2 = approx_deriv( volume, world_space, pos, a1 );
    pos[a2] = orig;

    return( (val2 - val1) / STEP / 2.0 );
}

private  void  check(
    Volume   volume,
    BOOLEAN  world_space,
    Real     x,
    Real     y,
    Real     z )
{
    Real   value, pos[N_DIMENSIONS];
    Real   deriv[2], deriv2[2][2];
    Real   true_deriv[2], true_deriv2[2][2];
    int    c, c2;

    if( world_space )
    (void) evaluate_slice_in_world( volume,
                          x, y, z, FALSE, &value,
                          &true_deriv[X], &true_deriv[Y],
                          &true_deriv2[X][X], &true_deriv2[X][Y],
                          &true_deriv2[Y][Y] );
    else
    (void) evaluate_slice( volume,
                          x, y, z, FALSE, &value,
                          &true_deriv[X], &true_deriv[Y],
                          &true_deriv2[X][X], &true_deriv2[X][Y],
                          &true_deriv2[Y][Y] );

    print( "Value %g\n", value );

    pos[X] = x;
    pos[Y] = y;
    pos[Z] = z;

    for_less( c, 0, 2 )
    {
        deriv[c] = approx_deriv( volume, world_space, pos, c );
        for_less( c2, c, 2 )
            deriv2[c][c2] = approx_deriv2( volume, world_space, pos, c, c2 );
    }

    for_less( c, 0, 2 )
    {
        if( !numerically_close( deriv[c], true_deriv[c], TOLERANCE ) )
            print( "Deriv %d %g %g\n", c, deriv[c], true_deriv[c] );

        for_less( c2, c, 2 )
            if( !numerically_close(deriv2[c][c2],
                                   true_deriv2[c][c2],TOLERANCE) )
                print( "Deriv2 %d %d %g %g\n", c, c2, deriv2[c][c2],
                       true_deriv2[c][c2] );
    }

}
