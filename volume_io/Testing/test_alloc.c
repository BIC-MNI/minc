#include  "alloc.h"
#include  <volume_io.h>

int  main(
    int   argc,
    char   *argv[] )
{
    double    *p;
    int    i, n;

    ALLOC_NEW( p, 20 );
    REALLOC( p, 30 );
    FREE( p );

#ifndef NO_DEBUG_ALLOC
    if( alloc_checking_enabled() )
    {
        p = (int *) 3245534;
        FREE( p );
    }
#endif

    p = NULL;
    FREE( p );

/*
    n = 0;

    ADD_ELEMENT_TO_ARRAY( p, n, 34, DEFAULT_CHUNK_SIZE );
    ADD_ELEMENT_TO_ARRAY( p, n, 35, DEFAULT_CHUNK_SIZE );
    ADD_ELEMENT_TO_ARRAY( p, n, 36, DEFAULT_CHUNK_SIZE );
    ADD_ELEMENT_TO_ARRAY( p, n, 37, DEFAULT_CHUNK_SIZE );

    DELETE_ELEMENT_FROM_ARRAY( p, n, 2, DEFAULT_CHUNK_SIZE );
    
    for_less( i, 0, n )
        print( "%d\n", p[i] );
*/

    return( 0 );
}
