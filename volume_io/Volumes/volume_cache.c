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
static char rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Volumes/volume_cache.c,v 1.8 1995-09-13 13:24:47 david Exp $";
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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_n_bytes_cache_threshold
@INPUT      : threshold
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the threshold number of bytes which decides if a volume
              is small enough to be held entirely in memory, or whether it
              should be cached.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Sep. 1, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  set_n_bytes_cache_threshold(
    int  threshold )
{
    n_bytes_cache_threshold = threshold;
    n_bytes_cache_threshold_set = TRUE;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_n_bytes_cache_threshold
@INPUT      : 
@OUTPUT     : 
@RETURNS    : number of bytes
@DESCRIPTION: Returns the number of bytes defining the cache threshold.  If it
              hasn't been set, returns the program initialized value, or the
              value set by the environment variable.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Sep. 1, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_max_bytes_in_cache
@INPUT      : n_bytes
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the maximum number of bytes to be used for a single
              volume's cache.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Sep. 1, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  set_max_bytes_in_cache(
    int  n_bytes )
{
    max_bytes_in_cache = n_bytes;
    max_bytes_in_cache_set = TRUE;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_max_bytes_in_cache
@INPUT      : 
@OUTPUT     : 
@RETURNS    : number of bytes
@DESCRIPTION: Returns the maximum number of bytes allowed for a single
              volume's cache.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Sep. 1, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_volume_cache_block_sizes
@INPUT      : block_sizes[]
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the size (in voxels) of each dimension of a cache block.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Sep. 1, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_volume_cache_block_sizes
@INPUT      : 
@OUTPUT     : block_sizes[]
@RETURNS    : 
@DESCRIPTION: Passes back the size (in voxels) of each dimension of a cache
              block.  If it hasn't been set, returns the program initialized
              value, or the value set by the environment variable.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Sep. 1, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_cache_total_blocks
@INPUT      : cache
@OUTPUT     : 
@RETURNS    : n_blocks
@DESCRIPTION: Computes the total number of cache blocks covering the volume.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Sep. 1, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : initialize_volume_cache
@INPUT      : cache
              volume
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Initializes the cache for a volume.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Sep. 1, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  initialize_volume_cache(
    volume_cache_struct   *cache,
    Volume                volume )
{
    int    dim, n_dims, sizes[MAX_DIMENSIONS], block, total_blocks, block_size;

    get_volume_sizes( volume, sizes );
    n_dims = get_volume_n_dimensions( volume );
    cache->n_dimensions = n_dims;
    cache->dim_names_set = FALSE;

    for_less( dim, 0, MAX_DIMENSIONS )
        cache->file_offset[dim] = 0;

    cache->blocks = NULL;

    cache->head = NULL;
    cache->tail = NULL;
    cache->minc_file = NULL;
    cache->input_filename[0] = (char) 0;
    cache->output_filename[0] = (char) 0;
    cache->has_been_modified = FALSE;
    cache->n_blocks = 0;
}

private  BOOLEAN  cache_is_alloced(
    volume_cache_struct  *cache )
{
    return( cache->blocks != NULL );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : check_alloc_volume_cache
@INPUT      : cache
              volume
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Ensures that the volume cache has been allocated.  This is
              delayed, i.e., not performed on volume cache initialization,
              so that block sizes and cache parameters may be set before it
              is allocated but after a volume cache has been initialized,
              i.e., a large volume file opened for reading.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Sep. 1, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  void  check_alloc_volume_cache(
    volume_cache_struct   *cache,
    Volume                volume )
{
    int    dim, n_dims, sizes[MAX_DIMENSIONS], block, total_blocks, block_size;

    if( cache_is_alloced( cache ) )
        return;

    get_volume_sizes( volume, sizes );
    n_dims = get_volume_n_dimensions( volume );

    get_volume_cache_block_sizes( cache->block_sizes );

    for_less( dim, 0, MAX_DIMENSIONS )
        cache->previous_block_start[dim] = -cache->block_sizes[dim];

    block_size = get_volume_data_type( volume );

    /*--- count number of blocks needed per dimension */

    for_less( dim, 0, n_dims )
    {
        cache->blocks_per_dim[dim] = (sizes[dim] - 1) / cache->block_sizes[dim]
                                     + 1;
        block_size *= cache->block_sizes[dim];
    }

    total_blocks = get_cache_total_blocks( cache );

    /*--- allocate a block pointer for every block in the volume */

    ALLOC( cache->blocks, total_blocks );

    for_less( block, 0, total_blocks )
        cache->blocks[block] = NULL;

    cache->max_blocks = get_max_bytes_in_cache() / block_size;

    if( cache->max_blocks < 1 )
        cache->max_blocks = 1;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_block_start
@INPUT      : cache
              block_index
@OUTPUT     : block_start[]
@RETURNS    : 
@DESCRIPTION: Computes the starting voxel indices for a block.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Sep. 1, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : write_cache_block
@INPUT      : cache
              volume
              block
              block_start
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Writes out a cache block to the appropriate position in the
              corresponding file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Sep. 1, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : free_cache_blocks
@INPUT      : cache
              volume
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Deletes all cache blocks, writing out all blocks, if the volume
              has been modified.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Sep. 1, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  void  free_cache_blocks(
    volume_cache_struct   *cache,
    Volume                volume )
{
    int                 block, total_blocks, block_index;
    int                 block_start[MAX_DIMENSIONS];
    cache_block_struct  **this, **next;

    if( !cache_is_alloced( cache ) )
        return;

    /*--- step through linked list, freeing blocks */

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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : delete_volume_cache
@INPUT      : cache
              volume
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Deletes the volume cache.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Sep. 1, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  delete_volume_cache(
    volume_cache_struct   *cache,
    Volume                volume )
{
    free_cache_blocks( cache, volume );

    FREE( cache->blocks );

    /*--- close the file that cache was reading from or writing to */

    if( cache->minc_file != NULL )
    {
        if( cache->has_been_modified )
        {
            (void) close_minc_output( (Minc_file) cache->minc_file );
        }
        else
            (void) close_minc_input( (Minc_file) cache->minc_file );
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_cache_volume_output_dimension_names
@INPUT      : volume
              dimension_names
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the output dimension names for any file created as a
              result of using volume caching.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Sep. 1, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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
    
/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_cache_volume_output_filename
@INPUT      : volume
              filename
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the output filename for any file created as a result of
              using volume caching.  If a cached volume is modified, a
              temporary file is created to store the volume.  If this function
              is called prior to this, a permanent file with the given name
              is created instead.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Sep. 1, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  set_cache_volume_output_filename(
    Volume      volume,
    char        filename[] )
{
    (void) strcpy( volume->cache.output_filename, filename );
}
    
/* ----------------------------- MNI Header -----------------------------------
@NAME       : open_cache_volume_input_file
@INPUT      : cache
              volume
              filename
              options
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Opens the volume file for reading into the cache as needed.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Sep. 1, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : open_cache_volume_output_file
@INPUT      : cache
              volume
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Opens a volume file for reading and writing cache blocks.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Sep. 1, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  void  open_cache_volume_output_file(
    volume_cache_struct   *cache,
    Volume                volume )
{
    int        dim, n_dims;
    int        out_sizes[MAX_DIMENSIONS], vol_sizes[MAX_DIMENSIONS];
    int        i, j, n_found;
    Real       voxel_min, voxel_max;
    Real       min_value, max_value;
    nc_type    nc_data_type;
    Minc_file  out_minc_file;
    BOOLEAN    done[MAX_DIMENSIONS], signed_flag;
    char       **vol_dim_names;
    STRING     out_dim_names[MAX_DIMENSIONS], output_filename;
    minc_output_options  options;

    /*--- check if the output filename has been set */

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

    /*--- using the dimension names, create output sizes */

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
    get_volume_real_range( volume, &min_value, &max_value );

    set_default_minc_output_options( &options );
    set_minc_output_real_range( &options, min_value, max_value );

    /*--- open the file for writing */

    out_minc_file = initialize_minc_output( output_filename,
                                        n_dims, out_dim_names, out_sizes,
                                        nc_data_type, signed_flag,
                                        voxel_min, voxel_max,
                                        get_voxel_to_world_transform(volume),
                                        volume, &options );

    out_minc_file->converting_to_colour = FALSE;

    /*--- make temp file disappear when the volume is deleted */

    if( strlen( cache->output_filename ) == 0 )
        remove_file( output_filename );

    /*--- if the volume was previously reading a file, copy the volume to
          the output and close the input file */

    if( cache->minc_file != NULL )
    {
        (void) output_minc_volume( out_minc_file );

        (void) close_minc_input( (Minc_file) cache->minc_file );
    }
    else
        check_minc_output_variables( out_minc_file );

    cache->minc_file = out_minc_file;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_cache_volume_file_offset
@INPUT      : cache
              volume
              file_offset
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the offset in the file for writing volumes.  Used when
              writing several cached volumes to a file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Sep. 1, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_cache_block
@INPUT      : cache
              volume
              block
              block_start
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Reads one cache block.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Sep. 1, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_cache_block
@INPUT      : cache
              volume
@OUTPUT     : block
@RETURNS    : 
@DESCRIPTION: Finds an available cache block, either by allocating one, or
              stealing the least recently used one.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Sep. 1, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  void  get_cache_block(
    volume_cache_struct  *cache,
    Volume               volume,
    cache_block_struct   **block )
{
    int    block_index, block_start[MAX_DIMENSIONS];

    /*--- if can allocate more blocks, do so */

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
    else  /*--- otherwise, steal the least-recently used block */
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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_block_index
@INPUT      : cache
              x
              y
              z
              t
              v
@OUTPUT     : block_start
@RETURNS    : block index
@DESCRIPTION: Converts a set of indices into a block index and sets the indices
              to their values within the block.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Sep. 1, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  int   get_block_index(
    volume_cache_struct   *cache,
    int                   *x,
    int                   *y,
    int                   *z,
    int                   *t,
    int                   *v,
    int                   block_start[] )
{
    int      block_index, block0, block1, block2, block3, block4;
    int      n_dims;

    n_dims = cache->n_dimensions;

    if( cache->previous_block_start[0] <= *x &&
        *x < cache->previous_block_start[0] + cache->block_sizes[0] &&
        (n_dims <= 1 || cache->previous_block_start[1] <= *y &&
         *y < cache->previous_block_start[1] + cache->block_sizes[1]) &&
        (n_dims <= 2 || cache->previous_block_start[2] <= *z &&
         *z < cache->previous_block_start[2] + cache->block_sizes[2]) &&
        (n_dims <= 3 || cache->previous_block_start[3] <= *t &&
         *t < cache->previous_block_start[3] + cache->block_sizes[3]) &&
        (n_dims <= 4 || cache->previous_block_start[4] <= *v &&
         *v < cache->previous_block_start[4] + cache->block_sizes[4]) )
    {
        block_start[0] = cache->previous_block_start[0];
        *x -= block_start[0];
        if( n_dims == 1 )
            return( cache->previous_block_index );

        block_start[1] = cache->previous_block_start[1];
        *y -= block_start[1];
        if( n_dims == 2 )
            return( cache->previous_block_index );

        block_start[2] = cache->previous_block_start[2];
        *z -= block_start[2];
        if( n_dims == 3 )
            return( cache->previous_block_index );

        *t -= block_start[3];
        block_start[3] = cache->previous_block_start[3];
        if( n_dims == 4 )
            return( cache->previous_block_index );

        *v -= block_start[4];
        block_start[4] = cache->previous_block_start[4];
        if( n_dims == 5 )
            return( cache->previous_block_index );
    }

    block0 = *x / cache->block_sizes[0];
    block_index = block0;
    block_start[0] = block0 * cache->block_sizes[0];
    cache->previous_block_start[0] = block_start[0];
    *x -= block_start[0];

    if( n_dims == 1 )
    {
        cache->previous_block_index = block_index;
        return( block_index );
    }

    block1 = *y / cache->block_sizes[1];
    block_index = block_index * cache->blocks_per_dim[1] + block1;
    block_start[1] = block1 * cache->block_sizes[1];
    cache->previous_block_start[1] = block_start[1];
    *y -= block_start[1];

    if( n_dims == 2 )
    {
        cache->previous_block_index = block_index;
        return( block_index );
    }

    block2 = *z / cache->block_sizes[2];
    block_index = block_index * cache->blocks_per_dim[2] + block2;
    block_start[2] = block2 * cache->block_sizes[2];
    cache->previous_block_start[2] = block_start[2];
    *z -= block_start[2];

    if( n_dims == 3 )
    {
        cache->previous_block_index = block_index;
        return( block_index );
    }

    block3 = *t / cache->block_sizes[3];
    block_index = block_index * cache->blocks_per_dim[3] + block3;
    block_start[3] = block3 * cache->block_sizes[3];
    cache->previous_block_start[3] = block_start[3];
    *t -= block_start[3];

    if( n_dims == 4 )
    {
        cache->previous_block_index = block_index;
        return( block_index );
    }

    block4 = *v / cache->block_sizes[4];
    block_index = block_index * cache->blocks_per_dim[4] + block4;
    block_start[4] = block4 * cache->block_sizes[4];
    cache->previous_block_start[4] = block_start[4];
    *v -= block_start[4];

    cache->previous_block_index = block_index;

    return( block_index );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_cache_block_for_voxel
@INPUT      : volume
              x
              y
              z
              t
              v
@OUTPUT     : 
@RETURNS    : pointer to cache block
@DESCRIPTION: Finds the cache block corresponding to a given voxel, and
              modifies the voxel indices to be block indices.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Sep. 1, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  cache_block_struct  *get_cache_block_for_voxel(
    Volume   volume,
    int      *x,
    int      *y,
    int      *z,
    int      *t,
    int      *v )
{
    cache_block_struct   **save_next, **save_prev, **block_ptr, *block;
    int                  block_index, block_start[MAX_DIMENSIONS];

    block_index = get_block_index( &volume->cache,
                                   x, y, z, t, v, block_start );
    block_ptr = &(volume->cache.blocks[block_index]);
    block = *block_ptr;

    if( block == NULL )
    {
        get_cache_block( &volume->cache, volume, block_ptr );
        block = *block_ptr;
        read_cache_block( &volume->cache, volume, block, block_start );
    }

    /*--- move block to the head */

    if( block_ptr != volume->cache.head )
    {
        save_next = block->next;
        save_prev = block->prev;

        if( save_next != NULL )
            (*save_next)->prev = save_prev;
        else
            volume->cache.tail = save_prev;

        if( save_prev != NULL )
            (*save_prev)->next = save_next;

        block->prev = NULL;
        block->next = volume->cache.head;
        (*volume->cache.head)->prev = block_ptr;

        volume->cache.head = block_ptr;
    }

    return( block );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_cached_volume_voxel
@INPUT      : volume
              x
              y
              z
              t
              v
@OUTPUT     : 
@RETURNS    : voxel value
@DESCRIPTION: Finds the voxel value for the given voxel in a cached volume.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Sep. 1, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

    if( volume->cache.minc_file == NULL )
        return( get_volume_voxel_min( volume ) );

    check_alloc_volume_cache( &volume->cache, volume );

    block = get_cache_block_for_voxel( volume, &x, &y, &z, &t, &v );

    GET_MULTIDIM( value, block->array, x, y, z, t, v );

    return( value );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_cached_volume_voxel
@INPUT      : volume
              x
              y
              z
              t
              v
              value
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the voxel value for the given voxel in a cached volume.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Sep. 1, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

    if( !volume->cache.has_been_modified )
    {
        check_alloc_volume_cache( &volume->cache, volume );

        open_cache_volume_output_file( &volume->cache, volume );
        volume->cache.has_been_modified = TRUE;
    }

    block = get_cache_block_for_voxel( volume, &x, &y, &z, &t, &v );

    SET_MULTIDIM( block->array, x, y, z, t, v, value );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : cached_volume_has_been_modified
@INPUT      : cache
@OUTPUT     : 
@RETURNS    : TRUE if the volume has been modified since creation
@DESCRIPTION: Determines if the volume has been modified.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Sep. 1, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  BOOLEAN  cached_volume_has_been_modified(
    volume_cache_struct  *cache )
{
    return( cache->minc_file != NULL );
}
