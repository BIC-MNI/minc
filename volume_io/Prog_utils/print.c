#include  <def_mni.h>
#include  <stdarg.h>

private  void  (*print_function) ( char [] );

public  void  set_print_function( void (*function) ( char [] ) )
{
    print_function = function;
}

/* VARARGS */
public  void  print( char format[], ... )
{
    va_list  ap;
    char     print_buffer[1000];

    va_start( ap, format );
    (void) vsprintf( print_buffer, format, ap );
    va_end( ap );

    if( print_function == NULL )
        (void) printf( "%s", print_buffer );
    else
        (*print_function) ( print_buffer );
}
