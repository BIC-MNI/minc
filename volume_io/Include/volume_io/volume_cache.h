#ifndef  DEF_VOLUME_CACHE
#define  DEF_VOLUME_CACHE

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

#ifndef lint
static char volume_cache_rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Include/volume_io/volume_cache.h,v 1.3 1995-09-13 13:24:43 david Exp $";
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : volume_cache.h
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Volume block caching mechanism for treating large volumes
              as if they are in memory.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Aug. 14, 1995   David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#include  <multidim.h>

typedef  struct  cache_block_struct
{
    multidim_array              array;
    struct  cache_block_struct  **prev;
    struct  cache_block_struct  **next;
} cache_block_struct;

typedef struct
{
    int                         n_dimensions;
    int                         file_offset[MAX_DIMENSIONS];
    STRING                      input_filename;
    STRING                      output_filename;
    int                         block_sizes[MAX_DIMENSIONS];
    int                         blocks_per_dim[MAX_DIMENSIONS];
    int                         previous_block_start[MAX_DIMENSIONS];
    int                         previous_block_index;
    BOOLEAN                     dim_names_set;
    STRING                      dimension_names[MAX_DIMENSIONS];
    BOOLEAN                     has_been_modified;
    void                        *minc_file;
    int                         n_blocks;
    int                         max_blocks;
    cache_block_struct          **head;
    cache_block_struct          **tail;
    cache_block_struct          **blocks;
} volume_cache_struct;

#endif
