#ifndef  DEF_ALLOC_CHECK
#define  DEF_ALLOC_CHECK

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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : alloc_check.h
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: These macros are only for use by Volume IO internal use and are
            : not meant for the user to call.
            :
            : A set of macros for use when allocating and freeing memory, in
            : order to test correctness of allocation,
            : i.e. freeing nil pointers, freeing pointers twice, or pointers
            : not allocated, checking if two alloced regions overlap, which
            : probably signals a previous write outside the bounds of an array.
@METHOD     : Requires the file alloc_check.c linked in
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#ifndef lint
static char alloc_check_rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Include/Attic/alloc_check.h,v 1.6 1995-07-31 13:44:34 david Exp $";
#endif

#ifndef NO_DEBUG_ALLOC

#include  <basic.h>

#define  RECORD_PTR( ptr, n_bytes )                                           \
               record_ptr( (void *) (ptr), n_bytes, __FILE__, __LINE__ )

#define  OLD_PTR( ptr )                                                       \
               void  *old_ptr = (void *) (ptr);

#define  CHANGE_PTR( new_ptr, n_bytes )                                       \
               change_ptr( old_ptr, (void *) (new_ptr), n_bytes,              \
                           __FILE__, __LINE__ )

#define  UNRECORD_PTR( ptr )                                                  \
               if( unrecord_ptr( (void *) (ptr), __FILE__, __LINE__ ) )

#else

#define  RECORD_PTR( ptr, n_bytes )
#define  OLD_PTR( ptr )
#define  CHANGE_PTR( new_ptr, n_bytes )
#define  UNRECORD_PTR( ptr )

#endif


#endif
