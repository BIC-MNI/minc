#include  <def_mni.h>

typedef double  REAL;

int  main(
    int   argc,
    char  *argv[] )
{
    static REAL   transform[4][4] = { { 1.0, 0.0, 0.0, 0.0 },
                                      { 0.0, 1.0, 0.0, 0.0 },
                                      { 0.0, 0.0, 1.0, 0.0 },
                                      { 0.0, 0.0, 0.0, 1.0 } };
    REAL x, y, z, nx, ny, nz;
    long   i, n_iters;
    Real  start, stop;

    if( argc == 1 || sscanf( argv[1], "%d", &n_iters ) != 1 )
        n_iters = 10000;

    if( argc > 10 )
    {
        (void) scanf( "%d %d $d", &x, &y, &z );
        (void) scanf( "%d %d %d %d", &transform[0][0], &transform[0][1],
                                         &transform[0][2], &transform[0][3] );
        (void) scanf( "%d %d %d %d", &transform[1][0], &transform[1][1],
                                         &transform[1][2], &transform[1][3] );
        (void) scanf( "%d %d %d %d", &transform[2][0], &transform[2][1],
                                         &transform[2][2], &transform[2][3] );
        (void) scanf( "%d %d %d %d", &transform[3][0], &transform[3][1],
                                         &transform[3][2], &transform[3][3] );
    }

    start = current_cpu_seconds();

    x = 1.0;
    y = 1.0;
    z = 1.0;
    if( y == z )
        x = y / z;

    for( i = 0;  i < n_iters;  ++i )
    {
        nx = x * transform[0][0] +
             y * transform[0][1] +
             z * transform[0][2] +
             transform[0][3];
        ny = x * transform[1][0] +
             y * transform[1][1] +
             z * transform[1][2] +
             transform[1][3];
        nz = x * transform[2][0] +
             y * transform[2][1] +
             z * transform[2][2] +
             transform[2][3];

        x = nx;
        y = ny;
        z = nz;
    }

    stop = current_cpu_seconds();

    print( "%g/sec \n", n_iters / (stop - start) );

    return( 0 );
}
