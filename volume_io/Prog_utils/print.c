#include  <volume_io.h>
#include  <stdarg.h>

private  void  (*saved_print_function) ( char [] );
private  void  (*print_function) ( char [] );

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_print_function
@INPUT      : function
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the output function.  If you use the function print()
              everywhere, in place of printf, then by default it uses
              printf to send output to stdout.  However, you can call
              the set_print_function() to tell it to use a different output
              function, e.g. output to a GL or X window.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  set_print_function( void (*function) ( char [] ) )
{
    print_function = function;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : push_print_function
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Save the current print function, so, for instance, you can
              print to stdout temporarily.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  push_print_function()
{
    saved_print_function = print_function;
    print_function = 0;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : pop_print_function
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Restore the print function.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  pop_print_function()
{
    print_function = saved_print_function;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : print
@INPUT      : exactly same arguments as printf
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: prints the arguments to a temporary string buffer, then either
              printf's the or calls the user installed function to output
              the string.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

/* VARARGS */
public  void  print( char format[], ... )
{
    va_list  ap;
    char     print_buffer[10000];

    va_start( ap, format );
    (void) vsprintf( print_buffer, format, ap );
    va_end( ap );

    if( print_function == NULL )
        (void) printf( "%s", print_buffer );
    else
        (*print_function) ( print_buffer );
}
