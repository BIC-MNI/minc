#ifndef  DEF_PROGRESS
#define  DEF_PROGRESS

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

#ifndef lint
static char progress_rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Include/volume_io/progress.h,v 1.5 1995-07-31 13:44:34 david Exp $";
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
