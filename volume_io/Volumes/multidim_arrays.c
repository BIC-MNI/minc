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

#include  <internal_volume_io.h>
#include  <limits.h>
#include  <float.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Volumes/multidim_arrays.c,v 1.1 1995-08-15 17:56:32 david Exp $";
#endif

public   void   create_empty_multidim_array(
    multidim_array  *array,
    int             n_dimensions,
    Data_types      data_type )
{
    if( n_dimensions < 1 || n_dimensions > MAX_DIMENSIONS )
    {
        print_error(
"create_empty_multidim_array(): n_dimensions (%d) not in range 1 to %d.\n",
               n_dimensions, MAX_DIMENSIONS );
    }

    array->n_dimensions = n_dimensions;
    array->data_type = data_type;
    array->data = (void *) NULL;
}

public  Data_types  get_multidim_data_type(
    multidim_array       *array )
{
    return( array->data_type );
}

public  void  set_multidim_data_type(
    multidim_array       *array,
    Data_types           data_type )
{
    array->data_type = data_type;
}

/* ----------------------------------------------------------------------------
@NAME       : get_type_size
@INPUT      : type
@OUTPUT     : 
@RETURNS    : size of the type
@DESCRIPTION: Returns the size of the given type.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June, 1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  int  get_type_size(
    Data_types   type )
{
    int   size;

    switch( type )
    {
    case  UNSIGNED_BYTE:    size = sizeof( unsigned char );   break;
    case  SIGNED_BYTE:      size = sizeof( signed   char );   break;
    case  UNSIGNED_SHORT:   size = sizeof( unsigned short );  break;
    case  SIGNED_SHORT:     size = sizeof( signed   short );  break;
    case  UNSIGNED_LONG:    size = sizeof( unsigned long );   break;
    case  SIGNED_LONG:      size = sizeof( signed   long );   break;
    case  FLOAT:            size = sizeof( float );           break;
    case  DOUBLE:           size = sizeof( double );          break;
    }

    return( size );
}

public  void  set_multidim_sizes(
    multidim_array   *array,
    int              sizes[] )
{
    int    dim;

    for_less( dim, 0, array->n_dimensions )
        array->sizes[dim] = sizes[dim];
}

public  BOOLEAN  multidim_array_is_alloced(
    multidim_array   *array )
{
    return( array->data != NULL );
}

public  void  alloc_multidim_array(
    multidim_array   *array )
{
    int    type_size, *sizes;
    void   *p1, **p2, ***p3, ****p4, *****p5;

    if( multidim_array_is_alloced( array ) )
        delete_multidim_array( array );

    if( array->data_type == NO_DATA_TYPE )
    {
        print_error(
           "Error: cannot allocate array data until size specified.\n" );
        return;
    }

    sizes = array->sizes;

    type_size = get_type_size( array->data_type );

    switch( array->n_dimensions )
    {
    case  1:
        alloc_memory_1d( &p1, sizes[0], type_size _ALLOC_SOURCE_LINE );
        array->data = (void *) p1;
        break;
    case  2:
        alloc_memory_2d( &p2, sizes[0], sizes[1], type_size _ALLOC_SOURCE_LINE);
        array->data = (void *) p2;
        break;
    case  3:
        alloc_memory_3d( &p3, sizes[0], sizes[1], sizes[2], type_size
                         _ALLOC_SOURCE_LINE );
        array->data = (void *) p3;
        break;
    case  4:
        alloc_memory_4d( &p4, sizes[0], sizes[1],
                         sizes[2], sizes[3], type_size _ALLOC_SOURCE_LINE );
        array->data = (void *) p4;
        break;
    case  5:
        alloc_memory_5d( &p5, sizes[0], sizes[1],
                         sizes[2], sizes[3], sizes[4], type_size
                         _ALLOC_SOURCE_LINE );
        array->data = (void *) p5;
        break;
    }
}

public   void   create_multidim_array(
    multidim_array  *array,
    int             n_dimensions,
    int             sizes[],
    Data_types      data_type )
{
    create_empty_multidim_array( array, n_dimensions, data_type );
    set_multidim_sizes( array, sizes );
    alloc_multidim_array( array );
}

public  void  delete_multidim_array(
    multidim_array   *array )
{
    if( array->data == NULL )
    {
        print_error( "Warning: cannot free NULL multidim data.\n" );
        return;
    }

    switch( array->n_dimensions )
    {
    case  1:  free_memory_1d( (void **) &array->data _ALLOC_SOURCE_LINE );
              break;
    case  2:  free_memory_2d( (void ***) &array->data _ALLOC_SOURCE_LINE );
              break;
    case  3:  free_memory_3d( (void ****) &array->data _ALLOC_SOURCE_LINE );
              break;
    case  4:  free_memory_4d( (void *****) &array->data _ALLOC_SOURCE_LINE );
              break;
    case  5:  free_memory_5d( (void ******) &array->data _ALLOC_SOURCE_LINE );
              break;
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_multidim_n_dimensions
@INPUT      : array
@OUTPUT     : 
@RETURNS    : number of dimensions
@DESCRIPTION: Returns the number of dimensions of the array
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June, 1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  int  get_multidim_n_dimensions(
    multidim_array   *array )
{
    return( array->n_dimensions );
}

public  void  get_multidim_sizes(
    multidim_array   *array,
    int              sizes[] )
{
    int   i;

    for_less( i, 0, array->n_dimensions )
        sizes[i] = array->sizes[i];
}

public  void  copy_multidim_reordered(
    multidim_array      *dest,
    int                 dest_ind[],
    multidim_array      *src,
    int                 src_ind[],
    int                 to_dest_index[] )
{
    int     i, last_src_dim, inner_size, src_inner_step, dest_inner_step;
    int     d, n_src_dims, n_dest_dims, ind[MAX_DIMENSIONS];
    int     type_size, src_sizes[MAX_DIMENSIONS];
    int     dest_offset[MAX_DIMENSIONS], src_offset[MAX_DIMENSIONS];
    int     dest_sizes[MAX_DIMENSIONS], dest_index;
    char    *dest_ptr, *src_ptr;
    BOOLEAN done;

    type_size = get_type_size( get_multidim_data_type(dest) );

    /*--- initialize dest */

    n_dest_dims = get_multidim_n_dimensions( dest );
    get_multidim_sizes( dest, dest_sizes );
    GET_MULTIDIM_PTR( dest_ptr, *dest, dest_ind[0], dest_ind[1], dest_ind[2],
                   dest_ind[3], dest_ind[4] );

    dest_offset[n_dest_dims-1] = type_size;
    for( d = n_dest_dims-2;  d >= 0;  --d )
        dest_offset[d] = dest_offset[d+1] * dest_sizes[d+1];

    /*--- initialize src */

    n_src_dims = get_multidim_n_dimensions( src );
    get_multidim_sizes( src, src_sizes );
    GET_MULTIDIM_PTR( src_ptr, *src, src_ind[0], src_ind[1], src_ind[2],
                      src_ind[3], src_ind[4] );

    src_offset[n_src_dims-1] = type_size;
    for( d = n_src_dims-2;  d >= 0;  --d )
        src_offset[d] = src_offset[d+1] * src_sizes[d+1];

    for_less( d, 0, n_src_dims )
        ind[d] = src_ind[d];

    /*--- check if we can transfer more than one at once */

    while( n_src_dims > 0 && to_dest_index[n_src_dims-1] == n_dest_dims-1 )
    {
        type_size *= src_sizes[n_src_dims-1];
        --n_src_dims;
        --n_dest_dims;
    }

    if( n_src_dims > 0 )
    {
        last_src_dim = n_src_dims-1;
        while( to_dest_index[last_src_dim] < 0 )
            --last_src_dim;
        inner_size = src_sizes[last_src_dim];
        src_inner_step = src_offset[last_src_dim];
        dest_inner_step = dest_offset[to_dest_index[last_src_dim]];
    }
    else
    {
        last_src_dim = 0;
        inner_size = 1;
        src_inner_step = 0;
        dest_inner_step = 0;
    }

    done = FALSE;
    while( !done )
    {
        if( src_inner_step == 1 )
        {
            for_less( i, 0, inner_size )
            {
                (void) memcpy( dest_ptr, src_ptr, type_size );
                ++src_ptr;
                dest_ptr += dest_inner_step;
            }
        }
        else if( dest_inner_step == 1 )
        {
            for_less( i, 0, inner_size )
            {
                (void) memcpy( dest_ptr, src_ptr, type_size );
                src_ptr += src_inner_step;
                ++dest_ptr;
            }
        }
        else
        {
            for_less( i, 0, inner_size )
            {
                (void) memcpy( dest_ptr, src_ptr, type_size );
                src_ptr += src_inner_step;
                dest_ptr += dest_inner_step;
            }
        }

        src_ptr -= src_inner_step * inner_size;
        dest_ptr -= dest_inner_step * inner_size;

        done = TRUE;
        d = last_src_dim-1;
        while( d >= 0 && done )
        {
            dest_index = to_dest_index[d];
            if( dest_index >= 0 )
            {
                src_ptr += src_offset[d];
                dest_ptr += dest_offset[dest_index];

                ++ind[d];
                if( ind[d] < src_sizes[d] )
                    done = FALSE;
                else
                {
                    ind[d] = 0;
                    src_ptr -= src_offset[d] * src_sizes[d];
                    dest_ptr -= dest_offset[dest_index] * src_sizes[d];
                }
            }

            --d;
        }
    }
}

