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

#include  <internal_volume_io.h>

#include  <sys/types.h>

#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif
#if HAVE_UNISTD_H
#include  <unistd.h>
#endif

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Prog_utils/time.c,v 1.21.2.1 2004-10-04 20:19:22 bert Exp $";
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

static Real  get_clock_ticks_per_second( void )
{
    static  BOOLEAN  initialized = FALSE;
    static  Real     clock_ticks_per_second;

    if( !initialized )
    {
        initialized = TRUE;
        clock_ticks_per_second = (Real) CLK_TCK;
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

VIOAPI Real  current_cpu_seconds( void )
{
    static BOOLEAN first_call = TRUE;
    static clock_t first;
    clock_t current;
    Real secs;

    if (first_call)
    {
        first_call = FALSE;
        first = clock();
        secs = (Real) first / get_clock_ticks_per_second();
    }
    else
    {
        current = clock();
        secs = (Real) (current - first) / get_clock_ticks_per_second();
    }
    return (secs);
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

VIOAPI  Real  current_realtime_seconds( void )
{
    static BOOLEAN first_call = TRUE;
    static time_t first;
    time_t current;
    Real secs;

    if( first_call )
    {
        first_call = FALSE;
        first = time(NULL);
        secs = 0.0;
    }
    else
    {
        current = time(NULL);
        secs = (double) (current - first);
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

VIOAPI  STRING  format_time(
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

VIOAPI  void  print_time(
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

VIOAPI  STRING  get_clock_time( void )
{
    time_t           clock_time;
    struct  tm       *time_tm;
    char             *str;

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

VIOAPI  void  sleep_program( Real seconds )
{
#if HAVE_SELECT
    struct  timeval  timeout;

    timeout.tv_sec = (long) seconds;
    timeout.tv_usec = (long) (1.0e6 * (seconds - (Real) timeout.tv_sec) + 0.5);

    (void) select( 0, NULL, NULL, NULL, &timeout );
#else
    sleep((unsigned int) seconds);
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

VIOAPI  STRING  get_date( void )
{
    time_t           clock_time;
    struct  tm       *time_tm;
    char             *str;

    (void) time( &clock_time );

    time_tm = localtime( &clock_time );

    str = asctime( time_tm );

    return( create_string( str ) );
}
