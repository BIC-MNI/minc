/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_origin_to_start.h
@DESCRIPTION: Header file for covnert_origin_to_start.c
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 7, 1995 (Peter Neelin)
@MODIFIED   : 
 * $Log: convert_origin_to_start.h,v $
 * Revision 6.1  1999-10-19 14:45:13  neelin
 * Fixed Log subsitutions for CVS
 *
 * Revision 6.0  1997/09/12 13:23:41  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:24:41  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:00:50  neelin
 * Release of minc version 0.4
 *
 * Revision 1.1  1995/11/10  20:01:27  neelin
 * Initial revision
 *
@COPYRIGHT  :
              Copyright 1995 Peter Neelin, McConnell Brain Imaging Centre, 
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

int convert_origin_to_start(double origin[], 
                            double xdircos[],
                            double ydircos[], 
                            double zdircos[],
                            double start[]);

