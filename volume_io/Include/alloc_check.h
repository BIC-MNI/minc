#ifndef  DEF_ALLOC_CHECK
#define  DEF_ALLOC_CHECK

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
static char alloc_check_rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Include/Attic/alloc_check.h,v 1.4 1994-11-25 14:19:42 david Exp $";
#endif

#ifndef NO_DEBUG_ALLOC

#include  <basic.h>

void      record_ptr( void *, int, char [], int );
void      change_ptr( void *, void *, int, char [], int );
BOOLEAN   unrecord_ptr( void *, char [], int );
int       get_total_memory_alloced( void );
void      output_alloc_to_file( char [] );

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
