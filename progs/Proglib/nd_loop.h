/* ----------------------------- MNI Header -----------------------------------
@NAME       : nd_loop.h
@DESCRIPTION: Header file for nd_loop.c
@METHOD     : Use these routines in the following way:

   Set start, end and increment vectors to define looping;

   nd_begin_looping(start, current, ndims);
   while (!nd_end_of_loop(start, end, ndims)) {
      nd_update_current_count(current, increment, end,
                              current_count, ndims);

      Use current and current_count to work on hyperslab;

      nd_increment_loop(current, start, increment, end, ndims);
   }

@GLOBALS    : 
@CREATED    : December 2, 1994 (Peter Neelin)
@MODIFIED   : $Log: nd_loop.h,v $
@MODIFIED   : Revision 1.2  1994-12-02 09:20:17  neelin
@MODIFIED   : Added comments to clarify use of routines.
@MODIFIED   :
 * Revision 1.1  94/12/02  08:40:31  neelin
 * Initial revision
 * 
@COPYRIGHT  :
              Copyright 1994 Peter Neelin, McConnell Brain Imaging Centre, 
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

public void nd_begin_looping(long start[], long current[], int ndims);
public int nd_end_of_loop(long start[], long end[], int ndims);
public void nd_update_current_count(long current[], 
                                    long increment[], long end[],
                                    long current_count[],
                                    int ndims);
public void nd_increment_loop(long current[], 
                              long start[], long increment[], long end[],
                              int ndims);
