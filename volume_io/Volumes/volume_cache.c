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
static char rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Volumes/volume_cache.c,v 1.1 1995-08-18 13:33:49 david Exp $";
#endif

#include  <internal_volume_io.h>

#define   DEFAULT_BLOCK_SIZE  8

private  int  volume_block_sizes[MAX_DIMENSIONS] = { DEFAULT_BLOCK_SIZE,
                                                     DEFAULT_BLOCK_SIZE,
                                                     DEFAULT_BLOCK_SIZE,
                                                     DEFAULT_BLOCK_SIZE,
                                                     DEFAULT_BLOCK_SIZE };

private  BOOLEAN  volume_block_sizes_set = FALSE;

private  int  n_bytes_cache_threshold = 80000000;
private  BOOLEAN  n_bytes_cache_threshold_set = FALSE;

private  int  max_bytes_in_cache = 80000000;
private  BOOLEAN  max_bytes_in_cache_set = FALSE;

public  void  set_n_bytes_cache_threshold(
    int  threshold )
{
    n_bytes_cache_threshold = threshold;
    n_bytes_cache_threshold_set = TRUE;
}

public  int  get_n_bytes_cache_threshold()
{
    int   n_bytes;

    if( !n_bytes_cache_threshold_set )
    {
        if( getenv( "VOLUME_CACHE_THRESHOLD" ) != NULL &&
            sscanf( getenv( "VOLUME_CACHE_THRESHOLD" ), "%d", &n_bytes ) == 1 )
        {
            n_bytes_cache_threshold = n_bytes;
        }
        n_bytes_cache_threshold_set = TRUE;
    }

    return( n_bytes_cache_threshold );
}

public  void  set_max_bytes_in_cache(
    int  n_bytes )
{
    max_bytes_in_cache = n_bytes;
    max_bytes_in_cache_set = TRUE;
}

private  int  get_max_bytes_in_cache()
{
    int   n_bytes;

    if( !max_bytes_in_cache_set )
    {
        if( getenv( "VOLUME_CACHE_SIZE" ) != NULL &&
            sscanf( getenv( "VOLUME_CACHE_SIZE" ), "%d", &n_bytes ) == 1 )
        {
            max_bytes_in_cache = n_bytes;
        }
        max_bytes_in_cache_set = TRUE;
    }

    return( max_bytes_in_cache );
}

public  void  set_volume_cache_block_sizes(
    int   block_sizes[] )
{
    int   dim;

    for_less( dim, 0, MAX_DIMENSIONS )
    {
        if( block_sizes[dim] > 1 )
            volume_block_sizes[dim] = block_sizes[dim];
    }

    volume_block_sizes_set = TRUE;
}

private  void  get_volume_cache_block_sizes(
    int    block_sizes[] )
{
    int   dim, block_size;

    if( !volume_block_sizes_set )
    {
        if( getenv( "VOLUME_CACHE_BLOCK_SIZE" ) != NULL &&
            sscanf( getenv( "VOLUME_CACHE_BLOCK_SIZE" ), "%d", &block_size )
                    == 1 && block_size >= 1 )
        {
            for_less( dim, 0, MAX_DIMENSIONS )
                volume_block_sizes[dim] = block_size;
        }
        volume_block_sizes_set = TRUE;
    }

    for_less( dim, 0, MAX_DIMENSIONS )
        block_sizes[dim] = volume_block_sizes[dim];
}


private  int  get_cache_total_blocks(
    volume_cache_struct   *cache )
{
    int   dim, n_dims, total_blocks;

    n_dims = cache->n_dimensions;

    total_blocks = 1;
    for_less( dim, 0, n_dims )
        total_blocks *= cache->blocks_per_dim[dim];

    return( total_blocks );
}

public  void  initialize_volume_cache(
    volume_cache_struct   *cache,
    Volume                volume )
{
    int    dim, n_dims, sizes[MAX_DIMENSIONS], block, total_blocks;

    get_volume_sizes( volume, sizes );
    n_dims = get_volume_n_dimensions( volume );
    cache->n_dimensions = n_dims;

    for_less( dim, 0, MAX_DIMENSIONS )
        cache->file_offset[dim] = 0;

    get_volume_cache_block_sizes( cache->block_sizes );

    for_less( dim, 0, n_dims )
    {
        cache->blocks_per_dim[dim] = (sizes[dim] - 1) / cache->block_sizes[dim]
                                     + 1;
    }

    total_blocks = get_cache_total_blocks( cache );

    ALLOC( cache->blocks, total_blocks );

    for_less( block, 0, total_blocks )
        cache->blocks[block] = NULL;

    cache->head = NULL;
    cache->tail = NULL;
    cache->input_file = NULL;
    cache->output_file = NULL;
    cache->input_filename[0] = (char) 0;
    cache->output_filename[0] = (char) 0;
    cache->empty_flag = TRUE;
    cache->has_been_modified = FALSE;
    cache->n_blocks = 0;

    cache->max_blocks = get_max_bytes_in_cache() / total_blocks /
                        get_type_size( get_volume_data_type(volume) );

    if( cache->max_blocks < 1 )
        cache->max_blocks = 1;
}

private  void  free_cache_blocks(
    volume_cache_struct   *cache )
{
    int                 block, total_blocks;
    cache_block_struct  **this, **next;

    this = cache->head;
    while( this != NULL )
    {
        next = (*this)->next;
        delete_multidim_array( &(*this)->array );
        FREE( *this );
        this = next;
    }

    cache->n_blocks = 0;

    total_blocks = get_cache_total_blocks( cache );
    for_less( block, 0, total_blocks )
        cache->blocks[block] = NULL;
}

public  void  delete_volume_cache(
    volume_cache_struct   *cache )
{
    free_cache_blocks( cache );

    FREE( cache->blocks );

    if( cache->input_file != NULL )
        (void) close_minc_input( (Minc_file) cache->input_file );
}

public  void  open_cache_volume_input_file(
    volume_cache_struct   *cache,
    Volume                volume,
    char                  filename[],
    minc_input_options    *options )
{
    (void) strcpy( cache->input_filename, filename );

    cache->input_file = initialize_minc_input( filename,
                                               volume, options );
}

public  void  set_cache_volume_file_offset(
    volume_cache_struct   *cache,
    long                  file_offset[] )
{
    BOOLEAN  changed;
    int      dim;

    changed = FALSE;

    for_less( dim, 0, MAX_DIMENSIONS )
    {
        if( cache->file_offset[dim] != (int) file_offset[dim] )
            changed = TRUE;

        cache->file_offset[dim] = (int) file_offset[dim];
    }

    if( changed )
        free_cache_blocks( cache );
}

private  void  get_cache_block(
    volume_cache_struct  *cache,
    Volume               volume,
    cache_block_struct   **block )
{
    if( cache->n_blocks < cache->max_blocks )
    {
        ALLOC( *block, 1 );

        create_multidim_array( &(*block)->array,
                               cache->n_dimensions,
                               cache->block_sizes,
                               get_volume_data_type(volume) );

        ++cache->n_blocks;

        if( cache->head == NULL )
        {
            (*block)->prev = NULL;
            (*block)->next = NULL;
            cache->head = block;
        }
        else
        {
            (*block)->prev = cache->tail;
            (*block)->next = NULL;
            (*cache->tail)->next = block;
        }
    }
    else
    {
        if( cache->head == cache->tail )
            cache->head = block;
        else
            (*(*cache->tail)->prev)->next = block;

        *block = *cache->tail;
        *cache->tail = NULL;
    }
    
    cache->tail = block;
}

private  void  read_cache_block(
    volume_cache_struct  *cache,
    Volume               volume,
    cache_block_struct   *block,
    int                  block_start[] )
{
    Minc_file   minc_file;
    int         dim, ind;
    int         sizes[MAX_DIMENSIONS];
    int         array_start[MAX_DIMENSIONS];
    int         file_start[MAX_DIMENSIONS];
    int         file_count[MAX_DIMENSIONS];

    minc_file = (Minc_file) cache->input_file;

    get_volume_sizes( volume, sizes );

    for_less( dim, 0, minc_file->n_file_dimensions )
    {
        ind = minc_file->to_volume_index[dim];
        if( ind >= 0 )
        {
            array_start[ind] = 0;
            file_start[dim] = cache->file_offset[dim] + block_start[ind];
            file_count[dim] = MIN( cache->block_sizes[ind],
                                   sizes[ind] - file_start[dim] );
        }
        else
        {
            file_start[dim] = cache->file_offset[dim];
            file_count[dim] = 0;
        }
    }

    (void) input_minc_hyperslab( (Minc_file) cache->input_file,
                                 &block->array,
                                 array_start,
                                 minc_file->to_volume_index,
                                 file_start,
                                 file_count );
}

private  int   get_block_index(
    volume_cache_struct   *cache,
    int                   x,
    int                   y,
    int                   z,
    int                   t,
    int                   v,
    int                   block_start[] )
{
    int      block_index, block_i;
    int      n_dims, dim, ind[MAX_DIMENSIONS];

    n_dims = cache->n_dimensions;

    ind[0] = x;
    ind[1] = y;
    ind[2] = z;
    ind[3] = t;
    ind[4] = v;

    block_index = 0;

    for_less( dim, 0, n_dims )
    {
        block_i = ind[dim] / cache->block_sizes[dim];
        block_start[dim] = block_i * cache->block_sizes[dim];
        block_index = block_index * cache->blocks_per_dim[dim] + block_i;
    }

    return( block_index );
}

public  Real  get_cached_volume_voxel(
    Volume   volume,
    int      x,
    int      y,
    int      z,
    int      t,
    int      v )
{
    cache_block_struct   **save_next, **save_prev, **block;
    Real                 value;
    int                  block_index, block_start[MAX_DIMENSIONS];

    block_index = get_block_index( &volume->cache,
                                   x, y, z, t, v, block_start );
    block = &(volume->cache.blocks[block_index]);

    if( *block == NULL )
    {
        get_cache_block( &volume->cache, volume, block );
        read_cache_block( &volume->cache, volume, *block, block_start );
    }

    /*--- move block to the head */

    if( block != volume->cache.head )
    {
        save_next = (*block)->next;
        save_prev = (*block)->prev;

        if( save_next != NULL )
            (*save_next)->prev = save_prev;
        else
            volume->cache.tail = save_prev;

        if( save_prev != NULL )
            (*save_prev)->next = save_next;

        (*block)->prev = NULL;
        (*block)->next = volume->cache.head;
        (*volume->cache.head)->prev = block;

        volume->cache.head = block;
    }

    GET_MULTIDIM( value, (*block)->array,
                  x - block_start[0],
                  y - block_start[1],
                  z - block_start[2],
                  t - block_start[3],
                  v - block_start[4] );

    return( value );
}

public  void  set_cached_volume_voxel(
    Volume   volume,
    int      x,
    int      y,
    int      z,
    int      t,
    int      v,
    Real     value )
{
}
