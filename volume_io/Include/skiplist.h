#ifndef  DEF_SKIPLIST
#define  DEF_SKIPLIST


/* ----------------------------- MNI Header -----------------------------------
@NAME       : skiplist.h
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Maintains a skiplist structure to handle ordered lists, searches.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#include  <basic.h>

typedef  struct  skip_struct
{
    void                    *data_ptr;
    struct  skip_struct     *forward[1];
} skip_struct;

#define  LESS_THAN     -1
#define  EQUAL_TO       0
#define  GREATER_THAN   1

typedef  struct
{
    int            (*compare_function) ( void *, void * );
    skip_struct    *header;
    int            level;
} skiplist_struct;

#endif
