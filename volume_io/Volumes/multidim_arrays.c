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
static char rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Volumes/multidim_arrays.c,v 1.5 1995-08-21 04:36:30 david Exp $";
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
    int                 counts[],
    int                 to_dest_index[] )
{
    int       d, n_src_dims, n_dest_dims;
    int       type_size, src_sizes[MAX_DIMENSIONS];
    int       dest_offsets[MAX_DIMENSIONS], src_offsets[MAX_DIMENSIONS];
    int       dest_steps[MAX_DIMENSIONS], src_steps[MAX_DIMENSIONS];
    int       dest_sizes[MAX_DIMENSIONS], dest_index;
    int       n_transfer_dims;
    int       src_axis[MAX_DIMENSIONS], dest_axis[MAX_DIMENSIONS];
    int       transfer_counts[MAX_DIMENSIONS];
    int       v0, v1, v2, v3, v4;
    int       size0, size1, size2, size3, size4;
    char      *dest_ptr, *src_ptr;
    BOOLEAN   full_count_used;

    type_size = get_type_size( get_multidim_data_type(dest) );

    /*--- initialize dest */

    n_dest_dims = get_multidim_n_dimensions( dest );
    get_multidim_sizes( dest, dest_sizes );
    GET_MULTIDIM_PTR( dest_ptr, *dest, dest_ind[0], dest_ind[1], dest_ind[2],
                      dest_ind[3], dest_ind[4] );

    dest_steps[n_dest_dims-1] = type_size;
    for_down( d, n_dest_dims-2, 0 )
        dest_steps[d] = dest_steps[d+1] * dest_sizes[d+1];

    /*--- initialize src */

    n_src_dims = get_multidim_n_dimensions( src );
    get_multidim_sizes( src, src_sizes );
    GET_MULTIDIM_PTR( src_ptr, *src, src_ind[0], src_ind[1], src_ind[2],
                      src_ind[3], src_ind[4] );

    src_steps[n_src_dims-1] = type_size;
    for_down( d, n_src_dims-2, 0 )
        src_steps[d] = src_steps[d+1] * src_sizes[d+1];

    n_transfer_dims = 0;
    for_less( d, 0, n_src_dims )
    {
        dest_index = to_dest_index[d];
        if( dest_index >= 0 )
        {
            src_axis[n_transfer_dims] = d;
            dest_axis[n_transfer_dims] = dest_index;
            src_offsets[n_transfer_dims] = src_steps[d];
            dest_offsets[n_transfer_dims] = dest_steps[dest_index];
            transfer_counts[n_transfer_dims] = counts[d];
            ++n_transfer_dims;
        }
    }

    /*--- check if we can transfer more than one at once */

    full_count_used = TRUE;

    while( n_transfer_dims > 0 &&
           src_axis[n_transfer_dims-1] == n_src_dims-1 &&
           dest_axis[n_transfer_dims-1] == n_dest_dims-1 && full_count_used )
    {
        if( transfer_counts[n_transfer_dims-1] != src_sizes[n_src_dims-1] ||
            transfer_counts[n_transfer_dims-1] != dest_sizes[n_dest_dims-1] )
        {
            full_count_used = FALSE;
        }

        type_size *= transfer_counts[n_transfer_dims-1];
        --n_src_dims;
        --n_dest_dims;
        --n_transfer_dims;
    }

    for_down( d, n_transfer_dims-1, 1 )
    {
        src_offsets[d] -= src_offsets[d-1] * transfer_counts[d-1];
        dest_offsets[d] -= dest_offsets[d-1] * transfer_counts[d-1];
    }

    for_less( d, n_transfer_dims, MAX_DIMENSIONS )
    {
        transfer_counts[d] = 1;
        src_offsets[d] = 0;
        dest_offsets[d] = 0;
    }

    size0 = transfer_counts[0];
    size1 = transfer_counts[1];
    size2 = transfer_counts[2];
    size3 = transfer_counts[3];
    size4 = transfer_counts[4];

    for_less( v4, 0, size4 )
    {
        for_less( v3, 0, size3 )
        {
            for_less( v2, 0, size2 )
            {
                for_less( v1, 0, size1 )
                {
                    for_less( v0, 0, size0 )
                    {
                        (void) memcpy( dest_ptr, src_ptr, type_size );
                        src_ptr += src_offsets[0];
                        dest_ptr += dest_offsets[0];
                    }
                    src_ptr += src_offsets[1];
                    dest_ptr += dest_offsets[1];
                }
                src_ptr += src_offsets[2];
                dest_ptr += dest_offsets[2];
            }
            src_ptr += src_offsets[3];
            dest_ptr += dest_offsets[3];
        }
        src_ptr += src_offsets[4];
        dest_ptr += dest_offsets[4];
    }
}
