/* ----------------------------- MNI Header -----------------------------------
@NAME       : nd_loop.c
@DESCRIPTION: File containing routines for doing n-dimensional looping
@METHOD     : 
@GLOBALS    : 
@CREATED    : March 10, 1994 (Peter Neelin)
@MODIFIED   : $Log: nd_loop.c,v $
@MODIFIED   : Revision 4.0  1997-05-07 20:00:50  neelin
@MODIFIED   : Release of minc version 0.4
@MODIFIED   :
 * Revision 3.0  1995/05/15  19:31:35  neelin
 * Release of minc version 0.3
 *
 * Revision 1.3  1995/02/08  19:31:47  neelin
 * Moved ARGSUSED statements for irix 5 lint.
 *
 * Revision 1.2  1994/12/02  09:19:59  neelin
 * Added comments to clarify use of routines.
 *
 * Revision 1.1  94/12/02  08:40:12  neelin
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

#ifndef lint
static char rcsid[]="$Header: /private-cvsroot/minc/progs/Proglib/Attic/nd_loop.c,v 4.0 1997-05-07 20:00:50 neelin Rel $";
#endif

#include <stdlib.h>
#include <stdio.h>

/* Constant definition */
#define public

/* ----------------------------- MNI Header -----------------------------------
@NAME       : nd_begin_looping
@INPUT      : start - vector of indices specifying starting subscript
                 for each dimension
              ndims - number of dimensions in vector
@OUTPUT     : current - vector of indices giving current subscript
@RETURNS    : (none)
@DESCRIPTION: Sets up current variable for looping
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : October 28, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void nd_begin_looping(long start[], long current[], int ndims)
{
   int idim;

   for (idim=0; idim < ndims; idim++) {
      current[idim] = start[idim];
   }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : nd_end_of_loop
@INPUT      : current - vector of indices giving current subscript
              end - vector of indices specifying last subscripts plus one
              ndims - number of dimensions in vector
@OUTPUT     : (none)
@RETURNS    : TRUE if end of loop.
@DESCRIPTION: Tests for end of a multi-dimensional loop.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : March 10, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int nd_end_of_loop(long current[], long end[], int ndims)
     /* ARGSUSED */
{
   return (current[0] >= end[0]);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : nd_update_current_count
@INPUT      : current - vector of indices giving current subscript
              increment - vector of indices specifying increment
                 for each dimension
              end - vector of indices specifying last subscripts plus one
              ndims - number of dimensions in vector
@OUTPUT     : current_count - vector of indices giving count for current 
                 position
@RETURNS    : (none)
@DESCRIPTION: Sets the count so that we don't go past end
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : October 28, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void nd_update_current_count(long current[], 
                                    long increment[], long end[],
                                    long current_count[],
                                    int ndims)
{
   int idim;

   for (idim=0; idim < ndims; idim++) {
      current_count[idim] = increment[idim];
      if ((current[idim] + current_count[idim]) > end[idim]) {
         current_count[idim] = end[idim] - current[idim];
      }
   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : nd_increment_loop
@INPUT      : current - vector of indices giving current subscript
              start - vector of indices specifying starting subscript
                 for each dimension
              increment - vector of indices specifying increment
                 for each dimension
              end - vector of indices specifying last subscripts plus one
              ndims - number of dimensions in vector
@OUTPUT     : current - vector of indices giving new subscript
@RETURNS    : (none)
@DESCRIPTION: Does incrementing for multi-dimensional loop
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : March 10, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void nd_increment_loop(long current[], 
                              long start[], long increment[], long end[],
                              int ndims)
{
   int idim;

   idim = ndims-1;
   current[idim] += increment[idim];
   while ( (idim>0) && (current[idim] >= end[idim])) {
      current[idim] = start[idim];
      idim--;
      current[idim] += increment[idim];
   }

}

