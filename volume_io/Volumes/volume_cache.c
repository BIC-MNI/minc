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
static char rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Volumes/volume_cache.c,v 1.2 1995-08-19 18:57:07 david Exp $";
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
    cache->dim_names_set = FALSE;

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
    cache->minc_file = NULL;
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

private  void  get_block_start(
    volume_cache_struct  *cache,
    int                  block_index,
    int                  block_start[] )
{
    int    dim, block_i;

    for_down( dim, cache->n_dimensions-1, 0 )
    {
        block_i = block_index % cache->blocks_per_dim[dim];
        block_start[dim] = block_i * cache->block_sizes[dim];
        block_index /= cache->blocks_per_dim[dim];
    }
}

private  void  write_cache_block(
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

    minc_file = (Minc_file) cache->minc_file;

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

    (void) output_minc_hyperslab( (Minc_file) cache->minc_file,
                                  &block->array,
                                  array_start,
                                  minc_file->to_volume_index,
                                  file_start,
                                  file_count );
}

private  void  free_cache_blocks(
    volume_cache_struct   *cache,
    Volume                volume )
{
    int                 block, total_blocks, block_index;
    int                 block_start[MAX_DIMENSIONS];
    cache_block_struct  **this, **next;

    this = cache->head;
    while( this != NULL )
    {
        if( cache->has_been_modified )
        {
            block_index = (int) (this - cache->blocks);
            get_block_start( cache, block_index, block_start );
            write_cache_block( cache, volume, *this, block_start );
        }

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
    volume_cache_struct   *cache,
    Volume                volume )
{
    free_cache_blocks( cache, volume );

    FREE( cache->blocks );

    if( cache->minc_file != NULL )
    {
        if( cache->has_been_modified )
        {
            (void) miicv_free( ((Minc_file) cache->minc_file)->input_icv );
            (void) close_minc_output( (Minc_file) cache->minc_file );
        }
        else
            (void) close_minc_input( (Minc_file) cache->minc_file );
    }
}

public  void  set_cache_volume_output_dimension_names(
    Volume      volume,
    char        **dimension_names )
{
    int   dim;

    for_less( dim, 0, get_volume_n_dimensions(volume) )
    {
        (void) strcpy( volume->cache.dimension_names[dim],
                       dimension_names[dim] );
    }
    volume->cache.dim_names_set = TRUE;
}
    
public  void  set_cache_volume_output_filename(
    Volume      volume,
    char        filename[] )
{
    (void) strcpy( volume->cache.output_filename, filename );
}
    
public  void  open_cache_volume_input_file(
    volume_cache_struct   *cache,
    Volume                volume,
    char                  filename[],
    minc_input_options    *options )
{
    (void) strcpy( cache->input_filename, filename );

    cache->minc_file = initialize_minc_input( filename,
                                              volume, options );
}

private  void  open_cache_volume_output_file(
    volume_cache_struct   *cache,
    Volume                volume )
{
    int        dim, n_dims;
    int        out_sizes[MAX_DIMENSIONS], vol_sizes[MAX_DIMENSIONS];
    int        i, j, n_found;
    Real       voxel_min, voxel_max;
    nc_type    nc_data_type;
    Minc_file  out_minc_file;
    BOOLEAN    done[MAX_DIMENSIONS], signed_flag;
    char       **vol_dim_names;
    STRING     out_dim_names[MAX_DIMENSIONS], output_filename;

    if( strlen( cache->output_filename ) == 0 )
    {
        (void) tmpnam( output_filename );
        (void) strcat( output_filename, "." );
        (void) strcat( output_filename, MNC_ENDING );
    }
    else
    {
        (void) strcpy( output_filename, cache->output_filename );
    }

    n_dims = get_volume_n_dimensions( volume );
    vol_dim_names = get_volume_dimension_names( volume );
    get_volume_sizes( volume, vol_sizes );

    if( cache->dim_names_set )
    {
        for_less( dim, 0, n_dims )
            (void) strcpy( out_dim_names[dim], cache->dimension_names[dim] );
    }
    else
    {
        for_less( dim, 0, n_dims )
            (void) strcpy( out_dim_names[dim], vol_dim_names[dim] );
    }

    n_found = 0;
    for_less( i, 0, n_dims )
        done[i] = FALSE;

    for_less( i, 0, n_dims )
    {
        for_less( j, 0, n_dims )
        {
            if( !done[j] &&
                strcmp( vol_dim_names[i], out_dim_names[j] ) == 0 )
            {
                out_sizes[j] = vol_sizes[i];
                ++n_found;
                done[j] = TRUE;
            }
        }
    }

    delete_dimension_names( vol_dim_names );

    if( n_found != n_dims )
    {
        handle_internal_error(
                    "Open_cache: Volume dimension name do not match.\n" );
    }

    nc_data_type = get_volume_nc_data_type( volume, &signed_flag );
    get_volume_voxel_range( volume, &voxel_min, &voxel_max );

    out_minc_file = initialize_minc_output( output_filename,
                                        n_dims, out_dim_names, out_sizes,
                                        nc_data_type, signed_flag,
                                        voxel_min, voxel_max,
                                        get_voxel_to_world_transform(volume),
                                        volume, NULL );

    out_minc_file->input_icv = miicv_create();

    (void) miicv_setint( out_minc_file->input_icv, MI_ICV_TYPE, nc_data_type );
    (void) miicv_setstr( out_minc_file->input_icv, MI_ICV_SIGN,
                         signed_flag ? MI_SIGNED : MI_UNSIGNED );
    (void) miicv_setint( out_minc_file->input_icv, MI_ICV_DO_NORM, TRUE );
    (void) miicv_setint( out_minc_file->input_icv, MI_ICV_DO_FILLVALUE, TRUE );
    (void) miicv_setdbl( out_minc_file->input_icv, MI_ICV_VALID_MIN, voxel_min);
    (void) miicv_setdbl( out_minc_file->input_icv, MI_ICV_VALID_MAX, voxel_max);

    (void) miicv_attach( out_minc_file->input_icv, out_minc_file->cdfid,
                         out_minc_file->img_var );

    out_minc_file->converting_to_colour = FALSE;

    /*--- make temp file disappear when the volume is deleted */

    if( strlen( cache->output_filename ) == 0 )
        remove_file( output_filename );

    if( cache->minc_file != NULL )
    {
        (void) output_minc_volume( out_minc_file );

        (void) close_minc_input( (Minc_file) cache->minc_file );
    }

    cache->minc_file = out_minc_file;
}

public  void  set_cache_volume_file_offset(
    volume_cache_struct   *cache,
    Volume                volume,
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
        free_cache_blocks( cache, volume );
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

    minc_file = (Minc_file) cache->minc_file;

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

    (void) input_minc_hyperslab( (Minc_file) cache->minc_file,
                                 &block->array,
                                 array_start,
                                 minc_file->to_volume_index,
                                 file_start,
                                 file_count );
}

private  void  get_cache_block(
    volume_cache_struct  *cache,
    Volume               volume,
    cache_block_struct   **block )
{
    int    block_index, block_start[MAX_DIMENSIONS];

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
        if( cache->has_been_modified )
        {
            block_index = (int) (cache->tail - cache->blocks);
            get_block_start( cache, block_index, block_start );
            write_cache_block( cache, volume, *cache->tail, block_start );
        }

        if( cache->head == cache->tail )
            cache->head = block;
        else
            (*(*cache->tail)->prev)->next = block;

        *block = *cache->tail;
        *cache->tail = NULL;
    }
    
    cache->tail = block;
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

private  cache_block_struct  *get_cache_block_for_voxel(
    Volume   volume,
    int      x,
    int      y,
    int      z,
    int      t,
    int      v,
    int      block_indices[] )
{
    cache_block_struct   **save_next, **save_prev, **block;
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

    block_indices[0] = x - block_start[0];
    block_indices[1] = y - block_start[1];
    block_indices[2] = z - block_start[2];
    block_indices[3] = t - block_start[3];
    block_indices[4] = v - block_start[4];

    return( *block );
}

public  Real  get_cached_volume_voxel(
    Volume   volume,
    int      x,
    int      y,
    int      z,
    int      t,
    int      v )
{
    Real                 value;
    cache_block_struct   *block;
    int                  block_index[MAX_DIMENSIONS];

    if( volume->cache.minc_file == NULL )
        return( get_volume_voxel_min( volume ) );

    block = get_cache_block_for_voxel( volume, x, y, z, t, v, block_index );

    GET_MULTIDIM( value, block->array,
                  block_index[0],
                  block_index[1],
                  block_index[2],
                  block_index[3],
                  block_index[4] );

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
    cache_block_struct   *block;
    int                  block_index[MAX_DIMENSIONS];

    if( !volume->cache.has_been_modified )
    {
        open_cache_volume_output_file( &volume->cache, volume );
        volume->cache.has_been_modified = TRUE;
    }

    block = get_cache_block_for_voxel( volume, x, y, z, t, v, block_index );

    SET_MULTIDIM( block->array,
                  block_index[0],
                  block_index[1],
                  block_index[2],
                  block_index[3],
                  block_index[4], value );
}

public  BOOLEAN  cached_volume_has_been_modified(
    volume_cache_struct  *cache )
{
    return( cache->minc_file != NULL );
}
