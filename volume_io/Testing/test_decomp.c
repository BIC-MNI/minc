#include  <volume_io.h>

int  main(
    int   argc,
    char  *argv[] )
{
    FILE  *file;
    int   ch;

    file = fopen( argv[1], "rb" );

    while( (ch = fgetc(file)) != EOF )
        (void) putchar( ch );

    (void) fclose( file );

    return( 0 );
}
