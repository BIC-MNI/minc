#ifndef  DEF_ALLOC
#define  DEF_ALLOC

/* ----------------------------- MNI Header -----------------------------------
@NAME       : alloc.h
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: A set of macros for allocating 1, 2, and 3 dimensional arrays.
@METHOD     : Requires the file alloc.c linked in.  This includes file
            : references alloc_check.h, to allow for allocation error
            : checking (these are the macros RECORD_PTR, UNRECORD_PTR, etc.)
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#ifndef lint
static char alloc_rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Include/volume_io/alloc.h,v 1.8 1995-04-28 18:32:49 david Exp $";
#endif

#include  <basic.h>
#include  <alloc_check.h>

/* --- prototypes and macros used by the public allocation macros --- */

#define  ALLOC_private( ptr, size, n_items )                                  \
         {                                                                    \
             alloc_memory( (void **) &(ptr),                                  \
                           (long) ((n_items) * (size)) );                     \
             RECORD_PTR( ptr, (long) ((n_items) * (size)) );                  \
         }

#define  ALLOC2D_private( ptr, size, n1, n2 )                                 \
         {                                                                    \
             long  _i2_;                                                      \
                                                                              \
             ALLOC_private( ptr, sizeof(*(ptr)), n1 );                        \
             ALLOC_private( (ptr)[0], size, (n1) * (n2) );                    \
                                                                              \
             for_less( _i2_, 1, n1 )                                          \
                 ((ptr)[_i2_]) = (void *) ((long) ((ptr)[_i2_-1]) +           \
                                                (n2) * (size));               \
         }

#define  ALLOC3D_private( ptr, size, n1, n2, n3 )                             \
         {                                                                    \
             long  _i3_, _j3_;                                                \
                                                                              \
             ALLOC2D_private( ptr, sizeof(**(ptr)), n1, n2 );                 \
                                                                              \
             ALLOC_private( (ptr)[0][0], size, (n1) * (n2) * (n3) );          \
                                                                              \
             for_less( _i3_, 0, n1 )                                          \
             {                                                                \
                 if( _i3_ > 0 )                                               \
                      (ptr)[_i3_][0] = (void *) ((long) ((ptr)[_i3_-1][0]) +  \
                                            (n2) * (n3) * (size));            \
                 for_less( _j3_, 1, n2 )                                      \
                      (ptr)[_i3_][_j3_] = (void *) ((long)                    \
                               ((ptr)[_i3_][_j3_-1]) + (n3) * (size));        \
             }                                                                \
         }

#define  ALLOC4D_private( ptr, size, n1, n2, n3, n4 )                         \
         {                                                                    \
             long  _i4_, _j4_, _k4_;                                          \
                                                                              \
             ALLOC3D_private( ptr, sizeof(***(ptr)), n1, n2, n3 );            \
                                                                              \
             ALLOC_private( (ptr)[0][0][0], size, (n1) * (n2) * (n3) * (n4) );\
                                                                              \
             for_less( _i4_, 0, n1 )                                          \
             {                                                                \
                 if( _i4_ > 0 )                                               \
                     (ptr)[_i4_][0][0] = (void *) ((long) ((ptr)[_i4_-1][0][0])\
                                            + (n2)*(n3)*(n4)*(size));         \
                 for_less( _j4_, 0, n2 )                                      \
                 {                                                            \
                     if( _j4_ > 0 )                                           \
                         (ptr)[_i4_][_j4_][0] =                               \
                                   (void *) ((long)((ptr)[_i4_][_j4_-1][0]) + \
                                                (n3)*(n4) * (size));          \
                     for_less( _k4_, 1, n3 )                                  \
                     {                                                        \
                         (ptr)[_i4_][_j4_][_k4_] = (void *) (                 \
                                   (long) ((ptr)[_i4_][_j4_][_k4_-1]) +       \
                                                   (n4) * (size));            \
                     }                                                        \
                 }                                                            \
             }                                                                \
         }

#define  ALLOC5D_private( ptr, size, n1, n2, n3, n4, n5 )                     \
         {                                                                    \
             long  _i5_, _j5_, _k5_, _l5_;                                    \
                                                                              \
             ALLOC4D_private( ptr, sizeof(****(ptr)), n1, n2, n3, n4 );       \
                                                                              \
             ALLOC_private( (ptr)[0][0][0][0], size, (n1) * (n2) * (n3) * (n4) * (n5) );    \
                                                                              \
             for_less( _i5_, 0, n1 )                                          \
             {                                                                \
                 if( _i5_ > 0 )                                               \
                     (ptr)[_i5_][0][0][0] = (void *)                          \
                            ((long) ((ptr)[_i5_-1][0][0][0]) +                \
                                            (n2)*(n3)*(n4)*(n5)*(size));      \
                 for_less( _j5_, 0, n2 )                                      \
                 {                                                            \
                     if( _j5_ > 0 )                                           \
                         (ptr)[_i5_][_j5_][0][0] = (void *)                   \
                                       ((long) ((ptr)[_i5_][_j5_-1][0][0]) +  \
                                                    (n3)*(n4)*(n5)*(size));   \
                     for_less( _k5_, 0, n3 )                                  \
                     {                                                        \
                         if( _k5_ > 0 )                                       \
                             (ptr)[_i5_][_j5_][_k5_][0] = (void *) ((long)    \
                                 ((ptr)[_i5_][_j5_][_k5_-1][0]) +             \
                                                    (n4)*(n5)*(size));        \
                                                                              \
                         for_less( _l5_, 1, (n4) )                            \
                             (ptr)[_i5_][_j5_][_k5_][_l5_] = (void *)(        \
                             (long) ((ptr)[_i5_][_j5_][_k5_][_l5_-1]) +       \
                                                     (n5) * (size));          \
                     }                                                        \
                 }                                                            \
             }                                                                \
         }

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ALLOC
@INPUT      : n_items
@OUTPUT     : ptr
@RETURNS    : 
@DESCRIPTION: Macro to allocate n_items of the type ptr points to, assigning
            : ptr.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  ALLOC( ptr, n_items )                                                \
             ALLOC_private( ptr, sizeof(*(ptr)), n_items )

/* ----------------------------- MNI Header -----------------------------------
@NAME       : FREE
@INPUT      : ptr
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Macro to FREE the ptr.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  FREE( ptr )                                                          \
         {                                                                    \
             UNRECORD_PTR(ptr)                                                \
                 free_memory( (void **) &(ptr) );                             \
         }

/* ----------------------------- MNI Header -----------------------------------
@NAME       : REALLOC
@INPUT      : ptr
            : n_items
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Macro to change the number of items that ptr points to, assigning
            : ptr.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  REALLOC( ptr, n_items )                                              \
         {                                                                    \
           OLD_PTR( ptr )                                                     \
           realloc_memory( (void **) &(ptr),                                  \
                           (size_t) ((n_items) * sizeof((ptr)[0])) );         \
           CHANGE_PTR( ptr, (size_t) (n_items) * (int) sizeof((ptr)[0]) );    \
         }

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ALLOC_VAR_SIZED_STRUCT
@INPUT      : element_type
            : n_elements
@OUTPUT     : 
            : ptr
@RETURNS    : 
@DESCRIPTION: Macro to allocate a structure with a variable size, assigning
            : ptr.
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
            :    ALLOC_VAR_SIZED_STRUCT( s_ptr, double, 15 );
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

#define  ALLOC_VAR_SIZED_STRUCT( ptr, element_type, n_elements )              \
         {                                                                    \
             alloc_memory( (void **) &(ptr),                                  \
               (size_t) (sizeof(*(ptr))+((n_elements)-1) * sizeof(element_type)));\
             RECORD_PTR( ptr,                                                 \
               (size_t) (sizeof(*(ptr))+((n_elements)-1) * sizeof(element_type)));\
         }

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ALLOC2D
@INPUT      : n1
            : n2
@OUTPUT     : 
            : ptr
@RETURNS    : 
@DESCRIPTION: Macro to allocate an n1 by n2 array, assigning : ptr.
@METHOD     : Allocates a single chunk size n1 by n2, and a list of size n1
            : pointers, which are each assigned to point into the single
            : chunk.  Therefore, only 2 malloc's are required.
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  ALLOC2D( ptr, n1, n2 )                                               \
         ALLOC2D_private( ptr, sizeof(**(ptr)), n1, n2 )

/* ----------------------------- MNI Header -----------------------------------
@NAME       : FREE2D
@INPUT      : ptr
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Macro to free the 2 dimensional array.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  FREE2D( ptr )                                                        \
         {                                                                    \
             FREE( (ptr)[0] );                                                \
             FREE( ptr );                                                     \
         }

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ALLOC3D
@INPUT      : n1
            : n2
            : n3
@OUTPUT     : 
            : ptr
@RETURNS    : 
@DESCRIPTION: Macro to allocate an n1 by n2 by n3 array, assigning : ptr.
@METHOD     : Similar to ALLOC2D, this requires only 3 mallocs.
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  ALLOC3D( ptr, n1, n2, n3 )                                           \
         ALLOC3D_private( ptr, sizeof(***(ptr)), n1, n2, n3 )

/* ----------------------------- MNI Header -----------------------------------
@NAME       : FREE3D
@INPUT      : ptr
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Frees a 3 dimensional array.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  FREE3D( ptr )                                                        \
         {                                                                    \
             FREE( (ptr)[0][0] );                                             \
             FREE2D( (ptr) );                                                 \
         }

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ALLOC4D
@INPUT      : n1
            : n2
            : n3
            : n4
@OUTPUT     : 
            : ptr
@RETURNS    : 
@DESCRIPTION: Macro to allocate an n1 by n2 by n3 by n4 array, assigning : ptr.
@METHOD     : Similar to ALLOC2D, this requires only 4 mallocs.
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  ALLOC4D( ptr, n1, n2, n3, n4 )                                       \
         ALLOC4D_private( ptr, sizeof(****(ptr)), n1, n2, n3, n4 )

/* ----------------------------- MNI Header -----------------------------------
@NAME       : FREE4D
@INPUT      : ptr
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Frees a 4 dimensional array.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  FREE4D( ptr )                                                        \
         {                                                                    \
             FREE( (ptr)[0][0][0] );                                          \
             FREE3D( (ptr) );                                                 \
         }

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ALLOC5D
@INPUT      : n1
            : n2
            : n3
            : n4
            : n5
@OUTPUT     : 
            : ptr
@RETURNS    : 
@DESCRIPTION: Macro to allocate an n1 by n2 by n3 by n4 by n5 array, assigning
            : ptr.
@METHOD     : Similar to ALLOC2D, this requires only 5 mallocs.
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  ALLOC5D( ptr, n1, n2, n3, n4, n5 )                                   \
         ALLOC5D_private( ptr, sizeof(*****(ptr)), n1, n2, n3, n4, n5 )

/* ----------------------------- MNI Header -----------------------------------
@NAME       : FREE5D
@INPUT      : ptr
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Frees a 5 dimensional array.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  FREE5D( ptr )                                                        \
         {                                                                    \
             FREE( (ptr)[0][0][0][0] );                                       \
             FREE4D( (ptr) );                                                 \
         }

#endif
