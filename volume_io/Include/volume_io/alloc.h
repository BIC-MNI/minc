#ifndef  DEF_ALLOC
#define  DEF_ALLOC

/* ----------------------------- MNI Header -----------------------------------
@NAME       : def_alloc.h
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: A set of macros for allocating 1, 2, and 3 dimensional arrays.
@METHOD     : Requires the file alloc.c linked in.  This include file references
            : def_alloc_check.h, to allow for allocation error checking
            : (these are the macros RECORD_PTR, UNRECORD_PTR, etc.)
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#include  <def_mni.h>
#include  <def_alloc_check.h>

Status   alloc_memory( void **, int );
Status   realloc_memory( void **, int );
Status   free_memory( void ** );

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ALLOC
@INPUT      : n_items
@OUTPUT     : status
            : ptr
@RETURNS    : 
@DESCRIPTION: Macro to allocate n_items of the type ptr points to, assigning
            : ptr, and returning status of OK or OUT_OF_MEMORY.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  ALLOC( status, ptr, n_items )                                        \
         {                                                                    \
             (status) = alloc_memory( (void **) &(ptr),                       \
                                     (int) ((n_items) * sizeof((ptr)[0])) );  \
             RECORD_PTR( ptr, (int) ((n_items) * sizeof((ptr)[0])) );         \
         }

/* ----------------------------- MNI Header -----------------------------------
@NAME       : FREE
@INPUT      : ptr
@OUTPUT     : status
@RETURNS    : 
@DESCRIPTION: Macro to FREE the ptr, returning status of OK or ERROR.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  FREE( status, ptr )                                                  \
         {                                                                    \
             UNRECORD_PTR(ptr)                                                \
                 (status) = free_memory( (void **) &(ptr) );                  \
         }

/* ----------------------------- MNI Header -----------------------------------
@NAME       : REALLOC
@INPUT      : ptr
            : n_items
@OUTPUT     : status
@RETURNS    : 
@DESCRIPTION: Macro to change the number of items that ptr points to, assigning
            : ptr, and returning status of OK or OUT_OF_MEMORY.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  REALLOC( status, ptr, n_items )                                      \
         {                                                                    \
           OLD_PTR( ptr )                                                     \
           (status) = realloc_memory( (void **) &(ptr),                       \
                                      (int) ((n_items) * sizeof((ptr)[0])) ); \
           CHANGE_PTR( ptr, (int) (n_items) * (int) sizeof((ptr)[0]) );       \
         }

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ALLOC_VAR_SIZED_STRUCT
@INPUT      : element_type
            : n_elements
@OUTPUT     : status
            : ptr
@RETURNS    : 
@DESCRIPTION: Macro to allocate a structure with a variable size, assigning
            : ptr, and returning status of OK or OUT_OF_MEMORY.
            : To use this, the variable length array must be the last element
            : of the structure.
            :
            : Use:
            :    typedef struct
            :    {
            :       int      n_items;
            :       double   variable_length_list[1];
            :    } var_struct;
            :
            :    var_struct   *s_ptr;
            :
            :    ALLOC_VAR_SIZED_STRUCT( status, s_ptr, double, 15 );
            :
            :    s->n_items = 15;
            :    s->variable_length_list[0] = 1.0;
            :    s->variable_length_list[1] = -2.0;
            :            ...
            :            ...
            :    s->variable_length_list[13] = 1456.0;
            :    s->variable_length_list[14] = 1234.0;
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  ALLOC_VAR_SIZED_STRUCT( status, ptr, element_type, n_elements )      \
         {                                                                    \
             (status) = alloc_memory( (void **) &(ptr),                       \
               (int) (sizeof(*(ptr))+((n_elements)-1) * sizeof(element_type)));\
             RECORD_PTR( ptr,                                                 \
               (int) (sizeof(*(ptr))+((n_elements)-1) * sizeof(element_type)));\
         }

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ALLOC2D
@INPUT      : n1
            : n2
@OUTPUT     : status
            : ptr
@RETURNS    : 
@DESCRIPTION: Macro to allocate an n1 by n2 array, assigning : ptr,
              and returning status of OK or OUT_OF_MEMORY.
@METHOD     : Allocates a single chunk size n1 by n2, and a list of size n1
            : pointers, which are each assigned to point into the single
            : chunk.  Therefore, only 2 malloc's are required.
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  ALLOC2D( status, ptr, n1, n2 )                                       \
         {                                                                    \
             int  _i2_;                                                       \
                                                                              \
             ALLOC( status, ptr, n1 );                                        \
             if( status == OK )                                               \
                 ALLOC( status, (ptr)[0], (n1) * (n2) );                      \
                                                                              \
             if( status == OK )                                               \
             {                                                                \
                 for( _i2_ = 1;  _i2_ < (n1);  ++_i2_ )                       \
                     ((ptr)[_i2_]) = ((ptr)[_i2_-1] + (n2));                  \
             }                                                                \
         }

/* ----------------------------- MNI Header -----------------------------------
@NAME       : FREE2D
@INPUT      : ptr
@OUTPUT     : status
@RETURNS    : 
@DESCRIPTION: Macro to free the 2 dimensional array,
              returning status of OK or ERROR.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  FREE2D( status, ptr )                                                \
         {                                                                    \
             FREE( status, (ptr)[0] );                                        \
             if( status == OK )                                               \
             {                                                                \
                 FREE( status, ptr );                                         \
             }                                                                \
         }

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ALLOC3D
@INPUT      : n1
            : n2
            : n3
@OUTPUT     : status
            : ptr
@RETURNS    : 
@DESCRIPTION: Macro to allocate an n1 by n2 by n3 array, assigning : ptr,
              and returning status of OK or OUT_OF_MEMORY.
@METHOD     : Similar to ALLOC2D, this requires only 3 mallocs.
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  ALLOC3D( status, ptr, n1, n2, n3 )                                   \
         {                                                                    \
             int  _i3_, _j3_;                                                 \
                                                                              \
             ALLOC2D( status, ptr, n1, n2 );                                  \
                                                                              \
             if( status == OK )                                               \
                 ALLOC( status, (ptr)[0][0], (n1) * (n2) * (n3) );            \
                                                                              \
             if( status == OK )                                               \
             {                                                                \
                 for( _i3_ = 0;  _i3_ < (n1);  ++_i3_ )                       \
                 {                                                            \
                     if( _i3_ > 0 )                                           \
                          (ptr)[_i3_][0] = (ptr)[_i3_-1][0] + (n2) * (n3);    \
                     for( _j3_ = 1;  _j3_ < (n2);  ++_j3_ )                   \
                          (ptr)[_i3_][_j3_] = (ptr)[_i3_][_j3_-1] + (n3);     \
                 }                                                            \
             }                                                                \
         }

/* ----------------------------- MNI Header -----------------------------------
@NAME       : FREE3D
@INPUT      : ptr
@OUTPUT     : status
@RETURNS    : 
@DESCRIPTION: Frees a 3 dimensional array.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  FREE3D( status, ptr )                                                \
         {                                                                    \
             FREE( status, (ptr)[0][0] );                                     \
             if( status == OK )                                               \
                 FREE2D( status, (ptr) );                                     \
         }

#endif
