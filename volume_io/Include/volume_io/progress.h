#ifndef  DEF_PROGRESS
#define  DEF_PROGRESS

#include  <def_basic.h>

typedef  struct
{
    Boolean    force_one_line;
    Boolean    first_msg_displayed;
    Boolean    one_line_flag;
    int        n_steps;
    int        n_dots_so_far;
    int        total_n_dots;
    Real       start_time;
    Real       previous_time;
    Real       sum_xy;
    Real       sum_xx;
    String     title;

    Real       last_check_time;
    int        check_every;
    int        next_check_step;
    int        last_check_step;
} progress_struct;

#endif
