#include  <sys/types.h>
#include  <sys/times.h>
#include  <sys/time.h>
#include  <sys/param.h>
#include  <sys/resource.h>
#include  <limits.h>
#include  <unistd.h>
#include  <internal_volume_io.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Prog_utils/time.c,v 1.13 1995-06-23 14:24:22 david Exp $";
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : current_cpu_seconds
@INPUT      : 
@OUTPUT     : 
@RETURNS    : # seconds
@DESCRIPTION: Returns the number of cpu seconds used by the program to date.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Real  current_cpu_seconds( void )
{
    Real            cpu_time;

#ifdef OLD
    struct  tms  buffer;

    (void) times( &buffer );

    cpu_time = (Real) buffer.tms_utime / (Real) CLK_TCK;
#else
    struct  rusage  buffer;

    (void) getrusage( RUSAGE_SELF, &buffer );

    cpu_time = (Real) buffer.ru_utime.tv_sec +
               (Real) buffer.ru_utime.tv_usec / 1.0e6;
#endif

    return( cpu_time );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : current_realtime_seconds
@INPUT      : 
@OUTPUT     : 
@RETURNS    : # seconds
@DESCRIPTION: Returns the number of seconds since the first invocation of this
            : function.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Real  current_realtime_seconds( void )
{
    static  BOOLEAN          first_call = TRUE;
    static  struct  timeval  first;
    struct  timeval          current;
    Real                     secs;

    if( first_call )
    {
        first_call = FALSE;
        (void) gettimeofday( &first, (struct timezone *) 0 );
        secs = 0.0;
    }
    else
    {
        (void) gettimeofday( &current, (struct timezone *) 0 );
        secs = (double) current.tv_sec - (double) first.tv_sec +
               1.0e-6 * (double) (current.tv_usec - first.tv_usec);
    }

    return( secs );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : format_time
@INPUT      : format
            : seconds
@OUTPUT     : str
@RETURNS    : 
@DESCRIPTION: Decides what time unit to use and displays the seconds value
            : in str, using format.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  format_time(
    char   str[],
    char   format[],
    Real   seconds )
{
    int     i;
    static  char   *units[] = { "us", "ms", "sec", "min", "hrs",
                                "days", "years"
                              };
    static  Real   scales[] = { 1000.0, 1000.0, 60.0, 60.0, 24.0, 365.0 };
    BOOLEAN  negative;

    negative = seconds < 0.0;
    if( negative )  seconds = -seconds;

    seconds *= 1.0e6;

    for_less( i, 0, SIZEOF_STATIC_ARRAY(units)-1 )
    {
        if( seconds > 2.0 * scales[i] )
        {
            seconds /= scales[i];
        }
        else
        {
            break;
        }
    }

    seconds = ROUND( 10.0 * seconds ) / 10.0;

    if( negative )  seconds = -seconds;

    (void) sprintf( str, format, seconds, units[i] );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : print_time
@INPUT      : format
            : seconds
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Prints out the time in suitable units.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  print_time(
    char   format[],
    Real   seconds )
{
    STRING  str;

    format_time( str, format, seconds );

    print( "%s", str );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_clock_time
@INPUT      : 
@OUTPUT     : time_str
@RETURNS    : 
@DESCRIPTION: Stores the current time of day in the "time_str".
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  get_clock_time(
    char  time_str[] )
{
    time_t           clock_time;
    struct  tm       *time_tm;
    char             *str;
#ifndef sgi
    time_t   time();
#endif

    (void) time( &clock_time );

    time_tm = localtime( &clock_time );

    str = asctime( time_tm );

    (void) strcpy( time_str, str );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : sleep_program
@INPUT      : seconds
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Make the program sleep for the specified number of seconds.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  sleep_program( Real seconds )
{
    struct  timeval  timeout;

    timeout.tv_sec = (long) seconds;
    timeout.tv_usec = (long) (1.0e6 * (seconds - (Real) timeout.tv_sec) + 0.5);

    (void) select( 0, NULL, NULL, NULL, &timeout );
#ifdef OLD
#ifdef sgi
    struct timespec  rqtp, rmtp;

    rqtp.tv_sec = FLOOR( seconds );
    rqtp.tv_nsec = (long) (1.0e9 * FRACTION(seconds));
    (void) nanosleep( &rqtp, &rmtp );
#else
    unsigned long  n_seconds, n_microseconds;

    n_seconds = FLOOR( seconds );
    if( n_seconds != 0 )
        (void) sleep( FLOOR( seconds ) );

    n_microseconds = ROUND( 1.0e6 * FRACTION(seconds) );
    usleep( n_microseconds );
#endif
#endif
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_date
@INPUT      : 
@OUTPUT     : date_str
@RETURNS    : 
@DESCRIPTION: Fills in the date into the string.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  get_date(
    char  date_str[] )
{
    time_t           clock_time;
    struct  tm       *time_tm;
    char             *str;
#ifndef sgi
    time_t time();
#endif

    (void) time( &clock_time );

    time_tm = localtime( &clock_time );

    str = asctime( time_tm );

    (void) strcpy( date_str, str );
}
