#include  <volume_io.h>

#ifdef sgi
typedef  size_t    alloc_int;
typedef  void      *alloc_ptr;
#else
typedef  unsigned  alloc_int;
typedef  char      *alloc_ptr;
#endif

private    void       record_alloc( int );
private    void       record_realloc( int );
private    void       record_free( void );

/* ----------------------------- MNI Header -----------------------------------
@NAME       : alloc_memory
@INPUT      : n_bytes
@OUTPUT     : ptr
@RETURNS    : 
@DESCRIPTION: Allocates the specified number of bytes.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  alloc_memory(
    void   **ptr,
    int    n_bytes )
{
    if( n_bytes > 0 )
    {
        *ptr = (void *) malloc( (alloc_int) n_bytes );

        if( *ptr == (void *) 0 )
        {
            set_print_function( NULL );
            print( "Error alloc_memory: out of memory, %d bytes.\n", n_bytes );
            abort_if_allowed();
        }
    }
    else
        *ptr = (void *) 0;

    record_alloc( n_bytes );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : realloc_memory
@INPUT      : ptr
            : n_bytes
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Reallocates the ptr.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  realloc_memory(
    void   **ptr,
    int    n_bytes )
{
    if( n_bytes > 0 )
    {
        *ptr = (void *) realloc( (alloc_ptr) *ptr, (alloc_int) n_bytes );

        if( *ptr == (void *) 0 )
        {
            set_print_function( NULL );
            print( "Error realloc_memory: out of memory, %d bytes.\n",
                    n_bytes );
            abort_if_allowed();
        }
    }

    record_realloc( n_bytes );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : free_memory
@INPUT      : ptr
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Frees the pointer, and sets it to NIL.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  free_memory( void   **ptr )
{
    record_free();

    if( *ptr != (void *) 0 )
    {
#ifdef sgi
        free( *ptr );
#else
        free( (char *) *ptr );
#endif
        *ptr = (void *) 0;
    }
}

#include  <stdio.h>

private  FILE  *file;

/* ----------------------------- MNI Header -----------------------------------
@NAME       : writing_alloc_debug
@INPUT      : 
@OUTPUT     : 
@RETURNS    : TRUE if saving debug info
@DESCRIPTION: 
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  BOOLEAN  writing_alloc_debug( void )
{
    static   BOOLEAN   first = TRUE;
    static   BOOLEAN   writing = FALSE;
    char               *filename;

    if( first )
    {
        first = FALSE;
        filename = getenv( "ALLOC_OUTPUT_FILE" );

        if( filename != (char *) 0 )
        {
            file = fopen( filename, "w" );

            if( file != (FILE *) 0 )
                writing = TRUE;
        }
    }

    return( writing );
}

private  void  record_alloc( int  n_bytes )
{
    if( writing_alloc_debug() )
    {
        (void) fprintf( file, "ALLOC   %20d\n", n_bytes );
        (void) fflush( file );
    }
}

private  void  record_realloc( int n_bytes )
{
    if( writing_alloc_debug() )
    {
        (void) fprintf( file, "REALLOC %20d\n", n_bytes );
        (void) fflush( file );
    }
}

private  void  record_free( void )
{
    if( writing_alloc_debug() )
    {
        (void) fprintf( file, "FREE\n" );
        (void) fflush( file );
    }
}
