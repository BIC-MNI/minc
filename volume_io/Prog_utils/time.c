/* ----------------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

#include  <sys/types.h>
#include  <sys/times.h>
#include  <sys/time.h>
#include  <unistd.h>
#include  <internal_volume_io.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Prog_utils/time.c,v 1.19 1997-03-23 21:11:31 david Exp $";
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_clock_ticks_per_second
@INPUT      : 
@OUTPUT     : 
@RETURNS    : number clock ticks per second
@DESCRIPTION: Returns the number of clock ticks per second in a system
              independent fashion
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Jul 3, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  Real  get_clock_ticks_per_second( void )
{
    static  BOOLEAN  initialized = FALSE;
    static  Real     clock_ticks_per_second;

    if( !initialized )
    {
        initialized = TRUE;
        clock_ticks_per_second = (Real) sysconf( _SC_CLK_TCK );
    }

    return( clock_ticks_per_second );
}

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

#ifdef USE_REALTIME_FOR_CPU_TIME

    /*--- use this code if neither of the two methods below work for getting
          the cpu time usage of a program, and you cannot manufacture one
          yourself */

    static  BOOLEAN initialized = FALSE;
    static  Real    first_real_time;

    if( !initialized )     /*--- first call will return about 1 microsecond */
    {
        initialized = TRUE;
        first_real_time = current_realtime_seconds();
    }

    cpu_time = current_realtime_seconds() - first_real_time;
#else
#ifdef USE_GETRUSAGE
    struct  rusage  buffer;

    (void) getrusage( RUSAGE_SELF, &buffer );

    cpu_time = (Real) buffer.ru_utime.tv_sec +
               (Real) buffer.ru_utime.tv_usec / 1.0e6;
#else
    struct  tms  buffer;

    (void) times( &buffer );

    cpu_time = (Real) buffer.tms_utime / get_clock_ticks_per_second();
#endif
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

public  STRING  format_time(
    STRING   format,
    Real     seconds )
{
    int      i;
    static   char   *units[] = { "us", "ms", "sec", "min", "hrs",
                                 "days", "years"
                               };
    static   Real   scales[] = { 1000.0, 1000.0, 60.0, 60.0, 24.0, 365.0 };
    char     buffer[EXTREMELY_LARGE_STRING_SIZE];
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

    seconds = (Real) ROUND( 10.0 * seconds ) / 10.0;

    if( negative )  seconds = -seconds;

    (void) sprintf( buffer, format, seconds, units[i] );

    return( create_string( buffer ) );
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
    STRING   format,
    Real     seconds )
{
    STRING  str;

    str = format_time( format, seconds );

    print( "%s", str );

    delete_string( str );
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

public  STRING  get_clock_time( void )
{
    time_t           clock_time;
    struct  tm       *time_tm;
    char             *str;
#ifndef __sgi
    time_t   time();
#endif

    (void) time( &clock_time );

    time_tm = localtime( &clock_time );

    str = asctime( time_tm );

    return( create_string( str ) );
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
#ifdef __sgi
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

public  STRING  get_date( void )
{
    time_t           clock_time;
    struct  tm       *time_tm;
    char             *str;
#ifndef __sgi
    time_t time();
#endif

    (void) time( &clock_time );

    time_tm = localtime( &clock_time );

    str = asctime( time_tm );

    return( create_string( str ) );
}
