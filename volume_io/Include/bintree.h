#ifndef  DEF_BINTREE
#define  DEF_BINTREE

#include  <def_geometry.h>

#define  NO_CHILD_SIGNAL  -1
#define  LEAF_SIGNAL      -2

typedef  int   bintree_node_type;

typedef  struct
{
    union
    {
        Real   left_limit;
        int    *object_list;
    } left_info;

    union
    {
        Real   right_limit;
        int    n_objects;
    } right_info;

    bintree_node_type   left_child_index;
    bintree_node_type   right_child_index;
} bintree_node_struct;

typedef  struct
{
    Real   limits[N_DIMENSIONS][2];
} range_struct;

typedef  struct
{
    range_struct         range;

    int                  n_nodes;
    bintree_node_struct  *node_storage;

    int                  *object_list;
} bintree_struct;


#endif
