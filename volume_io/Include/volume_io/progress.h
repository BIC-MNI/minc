#ifndef  DEF_PROGRESS
#define  DEF_PROGRESS

#ifndef lint
static char progress_rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Include/volume_io/progress.h,v 1.4 1994-11-25 14:19:41 david Exp $";
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : progress.h
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Defines type used for progress reporting.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#include  <basic.h>

typedef  struct
{
    BOOLEAN    force_one_line;
    BOOLEAN    first_msg_displayed;
    BOOLEAN    one_line_flag;
    int        n_steps;
    int        n_dots_so_far;
    int        total_n_dots;
    Real       start_time;
    Real       previous_time;
    Real       sum_xy;
    Real       sum_xx;
    STRING     title;

    Real       last_check_time;
    int        check_every;
    int        next_check_step;
    int        last_check_step;
} progress_struct;

#endif
