#ifndef  DEF_ARRAYS
#define  DEF_ARRAYS

/* ----------------------------- MNI Header -----------------------------------
@NAME       : def_arrays.h
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Macros for adding to and deleting from arrays.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#include  <def_alloc.h>

#define  DEFAULT_CHUNK_SIZE    100

/* ----------------------------- MNI Header -----------------------------------
@NAME       : SET_ARRAY_SIZE
@INPUT      : array
              previous_n_elems
              new_n_elems       - desired new array size
              chunk_size
@OUTPUT     : status
@RETURNS    : 
@DESCRIPTION: Sets the number of items allocated in the array to a multiple of
            : chunk_size larger than new_n_elems
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  SET_ARRAY_SIZE( status, array, previous_n_elems, new_n_elems, chunk_size )   \
     {                                                                        \
         (status) = OK;                                                       \
         if( (new_n_elems) > 0 )                                              \
         {                                                                    \
             int  _size_required;                                             \
                                                                              \
             _size_required = (((new_n_elems)+(chunk_size)-1)/(chunk_size))   \
                              * (chunk_size);                                 \
             if( (previous_n_elems) <= 0 )                                    \
             {                                                                \
                 ALLOC( status, array, _size_required );                      \
             }                                                                \
             else                                                             \
             {                                                                \
                 int  _previous_n_elems;                                      \
                                                                              \
                 _previous_n_elems = (((previous_n_elems)+(chunk_size)-1)/    \
                                  (chunk_size)) * (chunk_size);               \
                                                                              \
                 if( _size_required != _previous_n_elems )                    \
                     REALLOC( status, array, _size_required );                \
             }                                                                \
         }                                                                    \
         else if( (previous_n_elems) > 0 )                                    \
             FREE( status, array );                                           \
     }

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ADD_ELEMENT_TO_ARRAY
@INPUT      : n_elems       : current number of items in the array
            : array         : the array to add to
            : elem_to_add   : the item to add
            : chunk_size    : the chunk_size for allocation
@OUTPUT     : status
@RETURNS    : 
@DESCRIPTION: Adds an element to the end of an array, and increments the n_elems
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  ADD_ELEMENT_TO_ARRAY( status, n_elems, array, elem_to_add, chunk_size)\
         {                                                                     \
             SET_ARRAY_SIZE( status, array, n_elems, (n_elems)+1, chunk_size );\
             if( status == OK )                                                \
             {                                                                 \
                 (array) [(n_elems)] = (elem_to_add);                          \
                 ++(n_elems);                                                  \
             }                                                                 \
         }

/* ----------------------------- MNI Header -----------------------------------
@NAME       : DELETE_ELEMENT_FROM_ARRAY
@INPUT      : n_elems           : current number of items in the array
            : array             : the array to add to
            : index_to_remove   : the index of the element to delete
            : chunk_size        : the chunk_size for allocation
@OUTPUT     : status
@RETURNS    : 
@DESCRIPTION: Deletes an element from an array, sliding down subsequent
            : elements, and decrements the n_elems
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  DELETE_ELEMENT_FROM_ARRAY( status, n_elems, array, index_to_remove, chunk_size ) \
         {                                                                     \
             int    _i_;                                                       \
             for_less( _i_, index_to_remove, (n_elems) - 1 )                   \
                 (array) [_i_] = (array)[_i_+1];                               \
                                                                               \
             --(n_elems);                                                      \
                                                                               \
             SET_ARRAY_SIZE( status, array, (n_elems)+1, (n_elems),chunk_size);\
         }

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ADD_ELEMENT_TO_ARRAY_WITH_SIZE
@INPUT      : n_alloced
            : n_elems
            : array
            : elem_to_add
            : chunk_size
@OUTPUT     : status
@RETURNS    : 
@DESCRIPTION: Adds an element to an array where a separate n_allocated and 
            : n_elems is maintained.  n_allocated will always be greater than
            : or equal to n_elems.  This routine is useful so that you don't
            : have to call SET_ARRAY_SIZE everytime you remove an element,
            : as in done in DELETE_ELEMENT_FROM_ARRAY
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  ADD_ELEMENT_TO_ARRAY_WITH_SIZE( status, n_alloced, n_elems, array, elem_to_add, chunk_size )                                                         \
         {                                                                    \
             SET_ARRAY_SIZE( status, array, n_alloced, (n_elems) + 1,         \
                             chunk_size );                                    \
             if( status == OK )                                               \
             {                                                                \
                 (array) [(n_elems)] = (elem_to_add);                         \
                 ++(n_elems);                                                 \
                 (n_alloced) = (n_elems);                                     \
             }                                                                \
         }

#endif
