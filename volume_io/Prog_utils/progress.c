
#include  <internal_volume_io.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Prog_utils/progress.c,v 1.4 1994-11-25 14:20:06 david Exp $";
#endif

#define  FIRST_MESSAGE_THRESHOLD   5.0

#define  ONE_LINE_THRESHOLD        160.0

#define  LINE_LENGTH               77

#define  UPDATE_RATE               20.0    /* seconds */

#define  RATIO_FOR_LINEAR          0.5

#define  DOUBLE_THRESHOLD          0.01
#define  HALF_THRESHOLD            0.5

private  void  show_one_line_progress(
    progress_struct    *progress,
    int                current_step );

private  void  show_multi_line_progress(
    progress_struct    *progress,
    int                current_step,
    Real               time_so_far,
    Real               est_total_time );

/* ----------------------------- MNI Header -----------------------------------
@NAME       : initialize_progress_report
@INPUT      : one_line_only        - whether line of dots is desired
            : n_steps
            : title
@OUTPUT     : progress             - structure is filled in
@RETURNS    : 
@DESCRIPTION: Initializes the progress report, which is either a line of dots
            : crossing the screen, or if the progress is too slow, a line
            : every 20 seconds or so indicating the amount of time left.
            : If one_line_only is true, then it is always a single line of dots.
            : n_steps is the total number of items or times through the loop.
            : If it is really fast, no messages at all are displayed.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  initialize_progress_report(
    progress_struct   *progress,
    BOOLEAN           one_line_only,
    int               n_steps,
    char              title[] )
{
    progress->force_one_line = one_line_only;
    progress->first_msg_displayed = FALSE;
    progress->one_line_flag = TRUE;
    progress->n_steps = n_steps;
    (void) strcpy( progress->title, title );
    progress->start_time = current_realtime_seconds();
    progress->previous_time = progress->start_time;
    progress->last_check_time = progress->start_time;
    progress->last_check_step = 0;
    progress->next_check_step = 1;
    progress->check_every = 1;
    progress->sum_xy = 0.0;
    progress->sum_xx = 0.0;
    progress->n_dots_so_far = 0;
    progress->total_n_dots = LINE_LENGTH - strlen( progress->title );

    if( progress->total_n_dots < 1 )
        progress->total_n_dots = 2;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : update_progress_report
@INPUT      : progress
            : current_step   (an integer between 1 and n_steps)
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Checks the current time and determines if it is time to output
            : a progress message.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  update_progress_report(
    progress_struct   *progress,
    int               current_step )
{
    Real    current_time, constant, n_seconds_per;
    Real    time_so_far, est_total_time;

    if( current_step < 1 || current_step < progress->next_check_step )
        return;

    if( current_step > progress->n_steps )
        current_step = progress->n_steps;

    current_time = current_realtime_seconds();

    n_seconds_per = progress->check_every *
                    (current_time - progress->last_check_time) /
                    (current_step - progress->last_check_step);

    if( n_seconds_per < DOUBLE_THRESHOLD )
        progress->check_every *= 2;
    else if( n_seconds_per > HALF_THRESHOLD && progress->check_every > 1 )
        progress->check_every /= 2;

    progress->last_check_time = current_time;
    progress->last_check_step = current_step;
    progress->next_check_step = current_step + progress->check_every;
    if( progress->next_check_step > progress->n_steps )
        progress->next_check_step = progress->n_steps;

    time_so_far = current_time - progress->start_time;

    progress->sum_xy = RATIO_FOR_LINEAR * progress->sum_xy +
                        (Real) current_step * time_so_far;
    progress->sum_xx = RATIO_FOR_LINEAR * progress->sum_xx +
                        (Real) current_step * (Real) current_step;

    if( time_so_far > FIRST_MESSAGE_THRESHOLD )
    {
        constant = progress->sum_xy / progress->sum_xx;
        est_total_time = (Real) progress->n_steps * constant;

        if( est_total_time <= time_so_far )
        {
            est_total_time = time_so_far * (Real) progress->n_steps /
                             (Real) current_step;
        }

        if( progress->force_one_line ||
            (progress->one_line_flag && est_total_time < ONE_LINE_THRESHOLD) )
        {
            show_one_line_progress( progress, current_step );
            progress->first_msg_displayed = TRUE;
        }
        else
        {
            if( progress->first_msg_displayed && progress->one_line_flag )
                print( "\n" );

            progress->one_line_flag = FALSE;

            if( current_time - progress->previous_time >= UPDATE_RATE )
            {
                show_multi_line_progress( progress, current_step, time_so_far,
                                          est_total_time );
                progress->first_msg_displayed = TRUE;
                progress->previous_time = current_time;
            }
        }
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : show_one_line_progress
@INPUT      : progress
            : current_step
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Given the current_step, and the total number, ensures that the
            : number of dots on the line is representative.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  void  show_one_line_progress(
    progress_struct    *progress,
    int                current_step )
{
    int     i, n_dots;

    n_dots = ROUND( (Real) current_step / (Real) progress->n_steps *
                    (Real) progress->total_n_dots );

    if( n_dots > progress->total_n_dots )
        handle_internal_error( "show_one_line_progress" );

    if( n_dots > progress->n_dots_so_far )
    {
        if( progress->n_dots_so_far == 0 )
        {
            print( "%s: ", progress->title );
        }

        for_less( i, progress->n_dots_so_far, n_dots )
        {
            print( "." );
        }

        (void) flush_file( stdout );

        progress->n_dots_so_far = n_dots;
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : show_multi_line_progress
@INPUT      : progress
            : current_step
            : time_so_far
            : est_total_time
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Displays report about time so far, estimated time left, etc.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  void  show_multi_line_progress(
    progress_struct    *progress,
    int                current_step,
    Real               time_so_far,
    Real               est_total_time )
{
    int     percent_done;
    Real    time_remaining;
    STRING  time_so_far_str, est_total_time_str, time_remaining_str;

    percent_done = ROUND( 100.0 * (Real) current_step /
                          (Real) progress->n_steps );

    time_remaining = est_total_time - time_so_far;

    format_time( time_so_far_str, "%g %s", time_so_far );
    format_time( est_total_time_str, "%g %s", est_total_time );
    format_time( time_remaining_str, "%g %s", time_remaining );

    print( "%s: %3d%% done. (%d/%d) (%s out of approx %s with %s remaining)\n",
           progress->title, percent_done, current_step, progress->n_steps,
           time_so_far_str, est_total_time_str, time_remaining_str );

    (void) flush_file( stdout );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : terminate_progress_report
@INPUT      : progress
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Terminates the progress report.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  terminate_progress_report(
    progress_struct   *progress )
{
    Real    total_time;
    STRING  time_str;

    if( progress->first_msg_displayed )
    {
        if( progress->one_line_flag )
        {
            show_one_line_progress( progress, progress->n_steps );
            print( "\n" );
        }
        else
        {
            total_time = current_realtime_seconds() - progress->start_time;

            format_time( time_str, "%g %s", total_time );

            print( "%s: DONE in %s\n", progress->title, time_str );
        }
    }
}
