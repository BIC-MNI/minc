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
static char volume_cache_rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Include/volume_io/volume_cache.h,v 1.7 1996-02-28 16:03:53 david Exp $";
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

typedef  enum  { SLICE_ACCESS, RANDOM_VOLUME_ACCESS }
               Cache_block_size_hints;

#define  CACHE_DEBUGGING
#undef   CACHE_DEBUGGING

typedef  struct  cache_block_struct
{
    int                         block_index;
    Smallest_int                modified_flag;
    multidim_array              array;
    struct  cache_block_struct  *prev_used;
    struct  cache_block_struct  *next_used;
    struct  cache_block_struct  **prev_hash;
    struct  cache_block_struct  *next_hash;
} cache_block_struct;

typedef  struct
{
    int       block_index_offset;
    int       block_offset;
} cache_lookup_struct;

typedef struct
{
    int                         n_dimensions;
    int                         file_offset[MAX_DIMENSIONS];
    STRING                      input_filename;

    STRING                      output_filename;
    nc_type                     file_nc_data_type;
    BOOLEAN                     file_signed_flag;
    Real                        file_voxel_min;
    Real                        file_voxel_max;
    STRING                      original_filename;
    STRING                      history;
    minc_output_options         options;

    BOOLEAN                     writing_to_temp_file;
    int                         total_block_size;
    int                         block_sizes[MAX_DIMENSIONS];
    int                         blocks_per_dim[MAX_DIMENSIONS];
    BOOLEAN                     output_file_is_open;
    BOOLEAN                     must_read_blocks_before_use;
    void                        *minc_file;
    int                         n_blocks;
    int                         max_cache_bytes;
    int                         max_blocks;
    int                         hash_table_size;
    cache_block_struct          *head;
    cache_block_struct          *tail;
    cache_block_struct          **hash_table;

    cache_lookup_struct         *lookup[MAX_DIMENSIONS];
    cache_block_struct          *previous_block;
    int                         previous_block_index;

    BOOLEAN                     debugging_on;
    int                         n_accesses;
    int                         output_every;
    int                         n_hits;
    int                         n_prev_hits;
} volume_cache_struct;

#endif
