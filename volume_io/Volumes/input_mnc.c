#include  <internal_volume_io.h>
#include  <minc.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Volumes/input_mnc.c,v 1.31 1994-12-08 09:49:18 david Exp $";
#endif

#define  INVALID_AXIS   -1

#define  MIN_SLAB_SIZE   10000      /* at least 10000 entries per read */
#define  MAX_SLAB_SIZE   200000     /* no more than 200 K at a time */

private  void  create_world_transform(
    Point       *origin,
    Vector      axes[N_DIMENSIONS],
    Real        axis_spacing[N_DIMENSIONS],
    Transform   *transform );
private  BOOLEAN  match_dimension_names(
    int               n_volume_dims,
    char              *volume_dimension_names[],
    int               n_file_dims,
    char              *file_dimension_names[],
    int               to_volume_index[] );

/* ----------------------------- MNI Header -----------------------------------
@NAME       : initialize_minc_input_from_minc_id
@INPUT      : minc_id
              volume
              options
@OUTPUT     : 
@RETURNS    : Minc_file
@DESCRIPTION: Initializes input of volumes from an already opened MINC file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Minc_file  initialize_minc_input_from_minc_id(
    int                  minc_id,
    Volume               volume,
    minc_input_options   *options )
{
    minc_file_struct    *file;
    int                 img_var, dim_vars[MAX_VAR_DIMS], n_vol_dims;
    int                 slab_size, length, prev_sizes[MAX_VAR_DIMS];
    nc_type             prev_nc_type;
    BOOLEAN             different;
    BOOLEAN             min_voxel_found, max_voxel_found, range_specified;
    double              valid_range[2], temp;
    long                long_size, mindex[MAX_VAR_DIMS];
    BOOLEAN             converted_sign;
    nc_type             converted_type;
    STRING              signed_flag, last_dim_name;
    nc_type             file_datatype;
    int                 sizes[MAX_VAR_DIMS];
    double              file_separations[MAX_VAR_DIMS];
    Real                volume_separations[MI_NUM_SPACE_DIMS];
    Real                default_voxel_min, default_voxel_max;
    Real                world_space[N_DIMENSIONS];
    double              start_position[MAX_VAR_DIMS];
    double              dir_cosines[MAX_VAR_DIMS][MI_NUM_SPACE_DIMS];
    double              tmp_cosines[MI_NUM_SPACE_DIMS];
    BOOLEAN             spatial_dim_flags[MAX_VAR_DIMS];
    Vector              offset;
    Point               origin;
    Real                zero_voxel[MAX_DIMENSIONS];
    Vector              spatial_axis;
    double              real_min, real_max;
    int                 d, dimvar, which_valid_axis, axis;
    int                 spatial_axis_indices[MAX_VAR_DIMS];
    minc_input_options  default_options;
    BOOLEAN             no_volume_data_type;

    if( options == (minc_input_options *) NULL )
    {
        set_default_minc_input_options( &default_options );
        options = &default_options;
    }

    ALLOC( file, 1 );

    file->cdfid = minc_id;
    file->file_is_being_read = TRUE;
    file->volume = volume;

    get_volume_sizes( volume, prev_sizes );
    prev_nc_type = volume->nc_data_type;

    /* --- find the image variable */

    img_var = ncvarid( file->cdfid, MIimage );

    ncvarinq( file->cdfid, img_var, (char *) NULL, &file_datatype,
              &file->n_file_dimensions, dim_vars, (int *) NULL );

    (void) ncdiminq( file->cdfid, dim_vars[file->n_file_dimensions-1],
                     last_dim_name, &long_size );

    file->converting_to_colour = FALSE;

    if( strcmp( last_dim_name, MIvector_dimension ) == 0 )
    {
        if( options->convert_vector_to_colour_flag && long_size == 3 )
        {
            set_volume_type( volume, NC_LONG, FALSE, 0.0, 0.0 );
            file->converting_to_colour = TRUE;
            --file->n_file_dimensions;
        }
        else if( options->convert_vector_to_scalar_flag )
        {
            --file->n_file_dimensions;
        }
    }

    n_vol_dims = get_volume_n_dimensions( volume );

    if( file->n_file_dimensions < n_vol_dims )
    {
        print( "Error: MINC file has only %d dims, volume requires %d.\n",
               file->n_file_dimensions, n_vol_dims );
        FREE( file );
        return( (Minc_file) 0 );
    }
    else if( file->n_file_dimensions > MAX_VAR_DIMS )
    {
        print( "Error: MINC file has %d dims, can only handle %d.\n",
               file->n_file_dimensions, MAX_VAR_DIMS );
        FREE( file );
        return( (Minc_file) NULL );
    }

    for_less( d, 0, file->n_file_dimensions )
    {
        ALLOC( file->dim_names[d], MAX_STRING_LENGTH + 1 );

        (void) ncdiminq( file->cdfid, dim_vars[d], file->dim_names[d],
                         &long_size );
        file->sizes_in_file[d] = long_size;
    }

    /* --- match the dimension names of the volume with those in the file */

    if( !match_dimension_names( get_volume_n_dimensions(volume),
                                volume->dimension_names,
                                file->n_file_dimensions, file->dim_names,
                                file->to_volume_index ) )
    {
        print( "Error:  dimension names did not match: \n" );
        
        print( "\n" );
        print( "Requested:\n" );
        for_less( d, 0, n_vol_dims )
            print( "%d: %s\n", d+1, volume->dimension_names[d] );

        print( "\n" );
        print( "In File:\n" );
        for_less( d, 0, file->n_file_dimensions )
            print( "%d: %s\n", d+1, file->dim_names[d] );

        FREE( file );
        return( (Minc_file) NULL );
    }

    for_less( d, 0, n_vol_dims )
        file->to_file_index[d] = INVALID_AXIS;

    for_less( d, 0, file->n_file_dimensions )
    {
        if( file->to_volume_index[d] != INVALID_AXIS )
            file->to_file_index[file->to_volume_index[d]] = d;
    }

    file->n_volumes_in_file = 1;

    /* --- find the spatial axes (x,y,z) */

    which_valid_axis = 0;

    for_less( d, 0, N_DIMENSIONS )
    {
        volume->spatial_axes[d] = INVALID_AXIS;
        file->spatial_axes[d] = INVALID_AXIS;
    }

    for_less( d, 0, file->n_file_dimensions )
    {
        if( convert_dim_name_to_spatial_axis( file->dim_names[d], &axis ) )
        {
            spatial_axis_indices[d] = axis;
            file->spatial_axes[axis] = d;
        }
        else
            spatial_axis_indices[d] = INVALID_AXIS;

        spatial_dim_flags[d] = (spatial_axis_indices[d] != INVALID_AXIS);

        if( file->to_volume_index[d] != INVALID_AXIS )
        {
            file->valid_file_axes[which_valid_axis] = d;

            if( spatial_dim_flags[d] )
            {
                volume->spatial_axes[spatial_axis_indices[d]] =
                                        file->to_volume_index[d];
            }

            ++which_valid_axis;
        }
    }

    /* --- get the spatial axis info, slice separation, start pos, etc. */

    for_less( d, 0, file->n_file_dimensions )
    {
        file_separations[d] = 1.0;
        start_position[d] = 0.0;

        if( spatial_dim_flags[d] )
        {
            dir_cosines[d][0] = 0.0;
            dir_cosines[d][1] = 0.0;
            dir_cosines[d][2] = 0.0;
            dir_cosines[d][spatial_axis_indices[d]] = 1.0;
        }

        dimvar = ncvarid( file->cdfid, file->dim_names[d] );
        if( dimvar != MI_ERROR )
        {
            (void) miattget1( file->cdfid, dimvar, MIstep, NC_DOUBLE,
                              (void *) (&file_separations[d]) );

            if( spatial_dim_flags[d] )
            {
                if( miattget1( file->cdfid, dimvar, MIstart, NC_DOUBLE,
                               (void *) (&start_position[d]) ) == MI_ERROR )
                    start_position[d] = 0.0;

                if( miattget( file->cdfid, dimvar, MIdirection_cosines,
                                 NC_DOUBLE, MI_NUM_SPACE_DIMS,
                                 (void *) tmp_cosines, (int *) NULL )
                     != MI_ERROR )
                {
                    dir_cosines[d][0] = tmp_cosines[0];
                    dir_cosines[d][1] = tmp_cosines[1];
                    dir_cosines[d][2] = tmp_cosines[2];
                }
            }
        }

        if( file->to_volume_index[d] == INVALID_AXIS )
        {
            file->n_volumes_in_file *= file->sizes_in_file[d];
        }
        else
        {
            sizes[file->to_volume_index[d]] = file->sizes_in_file[d];
            volume_separations[file->to_volume_index[d]] =
                                          file_separations[d];
        }
    }

    /* --- create the file world transform */

    fill_Point( origin, 0.0, 0.0, 0.0 );

    for_less( d, 0, MAX_DIMENSIONS )
        zero_voxel[d] = 0.0;

    for_less( d, 0, N_DIMENSIONS )
    {
        axis = file->spatial_axes[d];
        if( axis != INVALID_AXIS )
        {
            fill_Vector( spatial_axis,
                         dir_cosines[axis][0],
                         dir_cosines[axis][1],
                         dir_cosines[axis][2] );
            NORMALIZE_VECTOR( spatial_axis, spatial_axis );
            
            SCALE_VECTOR( offset, spatial_axis, start_position[axis] );
            ADD_POINT_VECTOR( origin, origin, offset );
        }
    }

    world_space[X] = Point_x(origin);
    world_space[Y] = Point_y(origin);
    world_space[Z] = Point_z(origin);

    compute_world_transform( file->spatial_axes, file_separations,
                             zero_voxel, world_space, dir_cosines,
                             &file->voxel_to_world_transform );

    /* --- create the world transform stored in the volume */

    fill_Point( origin, 0.0, 0.0, 0.0 );

    for_less( d, 0, file->n_file_dimensions )
    {
        if( file->to_volume_index[d] != INVALID_AXIS )
        {
            set_volume_direction_cosine( volume,
                                         file->to_volume_index[d],
                                         dir_cosines[d] );
        }
    }

    general_transform_point( &file->voxel_to_world_transform,
                             0.0, 0.0, 0.0,
                             &world_space[X], &world_space[Y], &world_space[Z]);

    for_less( d, 0, N_DIMENSIONS )
        zero_voxel[d] = 0.0;

    set_volume_translation( volume, zero_voxel, world_space );
    set_volume_separations( volume, volume_separations );

    /* --- decide on type conversion */

    if( file->converting_to_colour )
    {
        converted_type = NC_BYTE;
        converted_sign = FALSE;
    }
    else
    {
        no_volume_data_type = (get_volume_data_type(volume) == NO_DATA_TYPE);
        if( no_volume_data_type )     /* --- use type of file */
        {
            if( miattgetstr( file->cdfid, img_var, MIsigntype,
                             MAX_STRING_LENGTH, signed_flag ) != (char *) NULL )
            {
                converted_sign = (strcmp( signed_flag, MI_SIGNED ) == 0);
            }
            else
                converted_sign = file_datatype != NC_BYTE;
    
            converted_type = file_datatype;
            set_volume_type( volume, converted_type, converted_sign, 0.0, 0.0 );
        }
        else                                        /* --- use specified type */
        {
            converted_type = get_volume_nc_data_type( volume, &converted_sign );
        }
    }

    set_volume_sizes( volume, sizes );

    for_less( d, 0, file->n_file_dimensions )
        mindex[d] = 0;

    /* --- create the image conversion variable */

    file->icv = miicv_create();

    (void) miicv_setint( file->icv, MI_ICV_TYPE, converted_type );
    (void) miicv_setstr( file->icv, MI_ICV_SIGN,
                         converted_sign ? MI_SIGNED : MI_UNSIGNED );
    (void) miicv_setint( file->icv, MI_ICV_DO_NORM, TRUE );
    (void) miicv_setint( file->icv, MI_ICV_DO_FILLVALUE, TRUE );

    get_volume_voxel_range( volume, &valid_range[0], &valid_range[1] );
    range_specified = (valid_range[0] < valid_range[1]);

    max_voxel_found = FALSE;
    min_voxel_found = FALSE;

    valid_range[0] = 0.0;
    valid_range[1] = 0.0;

    if( file->converting_to_colour )
    {
        min_voxel_found = TRUE;
        max_voxel_found = TRUE;
        valid_range[0] = 0.0;
        valid_range[1] = 2.0 * (double) (1ul << 31ul) - 1.0;
        set_volume_voxel_range( volume, valid_range[0], valid_range[1] );
    }
    else if( no_volume_data_type )
    {
        if( miattget( file->cdfid, img_var, MIvalid_range, NC_DOUBLE,
                         2, (void *) valid_range, &length ) == MI_ERROR ||
            length != 2 )
        {
            if( miattget1( file->cdfid, img_var, MIvalid_min, NC_DOUBLE,
                           (void *) &valid_range[0] ) != MI_ERROR )
            {
                min_voxel_found = TRUE;
            }
            if( miattget1( file->cdfid, img_var, MIvalid_max, NC_DOUBLE,
                           (void *) &valid_range[1] ) != MI_ERROR )
            {
                max_voxel_found = TRUE;
            }
        }
        else
        {
            if( valid_range[0] > valid_range[1] )
            {
                temp = valid_range[0];
                valid_range[0] = valid_range[1];
                valid_range[1] = temp;
            }
            min_voxel_found = TRUE;
            max_voxel_found = TRUE;
        }
    }

    if( !file->converting_to_colour &&
        (no_volume_data_type || !range_specified) )
    {
        set_volume_voxel_range( volume, 0.0, 0.0 );
        get_volume_voxel_range( volume, &default_voxel_min, &default_voxel_max);

        if( min_voxel_found && max_voxel_found )
            set_volume_voxel_range( volume, valid_range[0], valid_range[1] );
        else if( min_voxel_found && !max_voxel_found )
            set_volume_voxel_range( volume, valid_range[0], default_voxel_max );
        else if( !min_voxel_found && max_voxel_found )
            set_volume_voxel_range( volume, default_voxel_min, valid_range[0] );
    }

    if( !file->converting_to_colour )
    {
        get_volume_voxel_range( volume, &valid_range[0], &valid_range[1] );

        (void) miicv_setdbl( file->icv, MI_ICV_VALID_MIN, valid_range[0] );
        (void) miicv_setdbl( file->icv, MI_ICV_VALID_MAX, valid_range[1] );
    }
    else
    {
        (void) miicv_setdbl( file->icv, MI_ICV_VALID_MIN, 0.0 );
        (void) miicv_setdbl( file->icv, MI_ICV_VALID_MAX, 1.0 );
    }

    if( options->convert_vector_to_scalar_flag && !file->converting_to_colour )
    {
        (void) miicv_setint( file->icv, MI_ICV_DO_DIM_CONV, TRUE );
        (void) miicv_setint( file->icv, MI_ICV_DO_SCALAR, TRUE );
        (void) miicv_setint( file->icv, MI_ICV_XDIM_DIR, FALSE );
        (void) miicv_setint( file->icv, MI_ICV_YDIM_DIR, FALSE );
        (void) miicv_setint( file->icv, MI_ICV_ZDIM_DIR, FALSE );
        (void) miicv_setint( file->icv, MI_ICV_KEEP_ASPECT, FALSE );
    }

    (void) miicv_attach( file->icv, file->cdfid, img_var );

    /* --- compute the mapping to real values */

    if( !file->converting_to_colour )
    {
         (void) miicv_inqdbl( file->icv, MI_ICV_NORM_MIN, &real_min );
         (void) miicv_inqdbl( file->icv, MI_ICV_NORM_MAX, &real_max );

         set_volume_real_range( volume, real_min, real_max );
    }

    if( options->promote_invalid_to_min_flag )
    {
        if( !file->converting_to_colour )
            (void) miicv_setdbl( file->icv, MI_ICV_FILLVALUE, valid_range[0] );
        else
            (void) miicv_setdbl( file->icv, MI_ICV_FILLVALUE, 0.0 );
    }

    for_less( d, 0, file->n_file_dimensions )
        file->indices[d] = 0;

    file->end_volume_flag = FALSE;

    ncopts = NC_VERBOSE | NC_FATAL;

    /* --- decide how many dimensions to read in at a time */

    file->n_slab_dims = 0;
    slab_size = 1;
    d = file->n_file_dimensions-1;
    
    do
    {
        if( file->to_volume_index[d] != INVALID_AXIS )
        {
            ++file->n_slab_dims;
            slab_size *= file->sizes_in_file[d];
        }
        --d;
    }
    while( d >= 0 && slab_size < MIN_SLAB_SIZE );

    if( slab_size > MAX_SLAB_SIZE && file->n_slab_dims > 1 )
    {
        --file->n_slab_dims;
    }

    /* --- decide whether the volume data must be freed (if it changed size) */

    different = FALSE;
    for_less( d, 0, n_vol_dims )
    {
        if( sizes[d] != prev_sizes[d] )
            different = TRUE;
    }

    if( prev_nc_type != converted_type )
        different = TRUE;

    if( different && volume->data != (char *) NULL )
        free_volume_data( volume );

    return( file );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : initialize_minc_input
@INPUT      : filename
              volume
@OUTPUT     : 
@RETURNS    : Minc_file
@DESCRIPTION: Initializes the input of a MINC file, passing back a MINC
              file pointer.  It assumes that the volume has been created,
              with the desired type, or NC_UNSPECIFIED type if it is desired
              to use whatever type is in the file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June, 1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Minc_file  initialize_minc_input(
    char                 filename[],
    Volume               volume,
    minc_input_options   *options )
{
    Minc_file    file;
    int          minc_id;

    ncopts = 0;
    minc_id = miopen( filename, NC_NOWRITE );

    if( minc_id == MI_ERROR )
    {
        print( "Error: opening MINC file \"%s\".\n", filename );
        return( (Minc_file) 0 );
    }

    file = initialize_minc_input_from_minc_id( minc_id, volume, options );

    if( file == (Minc_file) NULL )
    {
        (void) miclose( minc_id );
    }

    return( file );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_n_input_volumes
@INPUT      : file
@OUTPUT     : 
@RETURNS    : number of input volumes
@DESCRIPTION: After initializing the file input with a specified volume,
              the user calls this function to decide how many volumes are
              stored in the file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June, 1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  int  get_n_input_volumes(
    Minc_file  file )
{
    return( file->n_volumes_in_file );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : close_minc_input
@INPUT      : file
@OUTPUT     : 
@RETURNS    : OK or ERROR
@DESCRIPTION: Closes the minc input file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June, 1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  close_minc_input(
    Minc_file   file )
{
    int  d;

    if( file == (Minc_file) NULL )
    {
        print( "close_minc_input(): NULL file.\n" );
        return( ERROR );
    }

    (void) miclose( file->cdfid );
    (void) miicv_free( file->icv );

    for_less( d, 0, file->n_file_dimensions )
        FREE( file->dim_names[d] );

    delete_general_transform( &file->voxel_to_world_transform );
    FREE( file );

    return( OK );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : copy_volumes_reordered
@INPUT      : dest
              dest_ind
              src
              src_ind
              to_dest_index
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Copies a hyperslab from one volume to another.  The src_ind and
              dest_ind are the starting points in the src and dest.  The
              array to_dest_index gives the destination axis that each
              src axis corresponds to, or -1 if no correspondence.  For
              any src_ind[] and dest_ind[] which have a correspondence, the
              starting position used is 0.  Therefore only hyperslabs which
              have a count of either 1 or sizes[d] are performed.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  copy_volumes_reordered(
    Volume      dest,
    int         dest_ind[],
    Volume      src,
    int         src_ind[],
    int         to_dest_index[] )
{
    int     i, last_src_dim, inner_size, src_inner_step, dest_inner_step;
    int     d, n_src_dims, n_dest_dims, ind[MAX_DIMENSIONS];
    int     type_size, src_sizes[MAX_DIMENSIONS];
    int     dest_offset[MAX_DIMENSIONS], src_offset[MAX_DIMENSIONS];
    int     dest_sizes[MAX_DIMENSIONS], dest_index;
    char    *dest_ptr, *src_ptr;
    BOOLEAN done;

    type_size = get_type_size( get_volume_data_type(dest) );

    /*--- initialize dest */

    n_dest_dims = get_volume_n_dimensions( dest );
    get_volume_sizes( dest, dest_sizes );
    GET_VOXEL_PTR( dest_ptr, dest, dest_ind[0], dest_ind[1], dest_ind[2],
                   dest_ind[3], dest_ind[4] );

    dest_offset[n_dest_dims-1] = type_size;
    for( d = n_dest_dims-2;  d >= 0;  --d )
        dest_offset[d] = dest_offset[d+1] * dest_sizes[d+1];

    /*--- initialize src */

    n_src_dims = get_volume_n_dimensions( src );
    get_volume_sizes( src, src_sizes );
    GET_VOXEL_PTR( src_ptr, src, src_ind[0], src_ind[1], src_ind[2],
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
        while( to_dest_index[last_src_dim] == INVALID_AXIS )
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
            if( dest_index != INVALID_AXIS )
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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : input_slab
@INPUT      : file
              volume
              start
              count
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Inputs a multidimensional slab from the file and copies it
              into the appropriate part of the volume.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June, 1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  void  input_slab(
    Minc_file   file,
    Volume      volume,
    int         to_volume[],
    long        start[],
    long        count[] )
{
    int      ind, expected_ind, n_vol_dims, file_ind, d, i;
    int      iv[MAX_VAR_DIMS];
    void     *void_ptr;
    BOOLEAN  direct_to_volume, signed_flag, non_full_size_found;
    Volume   slab_volume, rgb_volume, volume_to_read;
    int      tmp_ind, tmp_sizes[MAX_VAR_DIMS], vol1_indices[MAX_DIMENSIONS];
    int      zero[MAX_VAR_DIMS];
    int      v[MAX_DIMENSIONS], voxel[MAX_DIMENSIONS];
    Real     rgb[3];
    Colour   colour;
    nc_type  data_type;

    direct_to_volume = !file->converting_to_colour;
    n_vol_dims = get_volume_n_dimensions( volume );
    expected_ind = n_vol_dims-1;
    tmp_ind = file->n_slab_dims-1;
    non_full_size_found = FALSE;

    for_less( ind, 0, file->n_slab_dims )
        vol1_indices[ind] = -1;

    for( file_ind = file->n_file_dimensions-1;  file_ind >= 0;  --file_ind )
    {
        ind = to_volume[file_ind];
        if( ind != INVALID_AXIS )
        {
            if( !non_full_size_found &&
                count[file_ind] < file->sizes_in_file[file_ind] )
                non_full_size_found = TRUE;
            else if( non_full_size_found && count[file_ind] > 1 )
                direct_to_volume = FALSE;

            if( count[file_ind] > 1 && ind != expected_ind )
                direct_to_volume = FALSE;

            if( count[file_ind] != 1 || file->sizes_in_file[file_ind] == 1 )
            {
                tmp_sizes[tmp_ind] = file->sizes_in_file[file_ind];
                vol1_indices[tmp_ind] = ind;
                zero[tmp_ind] = 0;
                --tmp_ind;
            }

            --expected_ind;

            iv[ind] = start[file_ind];
        }
    }

    if( direct_to_volume )        /* file is same order as volume */
    {
        GET_VOXEL_PTR( void_ptr, volume, iv[0], iv[1], iv[2], iv[3], iv[4] );
        (void) miicv_get( file->icv, start, count, void_ptr );
    }
    else
    {
        data_type = get_volume_nc_data_type( volume, &signed_flag );

        slab_volume = create_volume( file->n_slab_dims, NULL,
                                    data_type, signed_flag,
                                    get_volume_voxel_min(volume),
                                    get_volume_voxel_max(volume) );

        set_volume_sizes( slab_volume, tmp_sizes );
        alloc_volume_data( slab_volume );

        if( file->converting_to_colour )
        {
            rgb_volume = create_volume( file->n_slab_dims+1, NULL,
                                        NC_FLOAT, FALSE, 0.0, 1.0 );
            tmp_sizes[file->n_slab_dims] = 3;
            start[file->n_slab_dims] = 0;
            count[file->n_slab_dims] = 3;
            set_volume_sizes( rgb_volume, tmp_sizes );
            alloc_volume_data( rgb_volume );
            volume_to_read = rgb_volume;
        }
        else
            volume_to_read = slab_volume;

        GET_VOXEL_PTR( void_ptr, volume_to_read, 0, 0, 0, 0, 0 );

        (void) miicv_get( file->icv, start, count, void_ptr );

        if( file->converting_to_colour )
        {
            BEGIN_ALL_VOXELS( slab_volume, v[0], v[1], v[2], v[3], v[4] )
                for_less( d, 0, file->n_slab_dims )
                    voxel[d] = v[d];
                for_less( i, 0, 3 )
                {
                    voxel[file->n_slab_dims] = i;
                    GET_VOXEL( rgb[i], rgb_volume, voxel[0], voxel[1],
                                        voxel[2], voxel[3], voxel[4] );
                }

                colour = make_Colour_0_1( rgb[0], rgb[1], rgb[2] );
                SET_VOXEL( slab_volume, v[0], v[1], v[2], v[3], v[4], colour );
            END_ALL_VOXELS

            delete_volume( rgb_volume );
        }

        copy_volumes_reordered( volume, iv, slab_volume, zero, vol1_indices );

        delete_volume( slab_volume );
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : input_more_minc_file
@INPUT      : file
@OUTPUT     : fraction_done        - amount of file read
@RETURNS    : TRUE if volume has more left to read
@DESCRIPTION: Reads another chunk from the input file, passes back the
              total fraction read so far, and returns FALSE when the whole
              volume has been read.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June, 1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  BOOLEAN  input_more_minc_file(
    Minc_file   file,
    Real        *fraction_done )
{
    int      d, ind, n_done, total, n_slab;
    long     count[MAX_VAR_DIMS];
    Volume   volume;
    BOOLEAN  increment;

    if( file->end_volume_flag )
    {
        print( "End of file in input_more_minc_file()\n" );
        return( FALSE );
    }

    volume = file->volume;

    if( volume->data == (void *) NULL )
        alloc_volume_data( volume );

    /* --- set the counts for reading, actually these will be the same
           every time */

    for_less( ind, 0, file->n_file_dimensions )
        count[ind] = 1;

    n_slab = 0;

    for( d = file->n_file_dimensions-1;  d >= 0 && n_slab < file->n_slab_dims;
         --d )
    {
        if( file->to_volume_index[d] != INVALID_AXIS )
        {
            count[d] = file->sizes_in_file[d];
            ++n_slab;
        }
    }

    input_slab( file, volume, file->to_volume_index, file->indices, count );

    /* --- advance to next slab */

    increment = TRUE;
    n_slab = 0;
    total = 1;
    n_done = 0;

    for( d = file->n_file_dimensions-1;  d >= 0;  --d )
    {
        if( n_slab >= file->n_slab_dims &&
            file->to_volume_index[d] != INVALID_AXIS )
        {
            if( increment )
            {
                ++file->indices[d];
                if( file->indices[d] < file->sizes_in_file[d] )
                    increment = FALSE;
                else
                    file->indices[d] = 0;
            }
            n_done += total * file->indices[d];
            total *= file->sizes_in_file[d];
        }

        if( file->to_volume_index[d] != INVALID_AXIS )
            ++n_slab;
    }

    if( increment )
    {
        *fraction_done = 1.0;
        file->end_volume_flag = TRUE;
    }
    else
    {
        *fraction_done = (Real) n_done / (Real) total;
    }

    return( !file->end_volume_flag );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : advance_input_volume
@INPUT      : file
@OUTPUT     : 
@RETURNS    : TRUE if more volumes to read
@DESCRIPTION: Advances the file indices to prepare for reading the next
              volume from the file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  BOOLEAN  advance_input_volume(
    Minc_file   file )
{
    int   ind, c, axis;
    Real  voxel[MAX_DIMENSIONS], world_space[N_DIMENSIONS];

    ind = file->n_file_dimensions-1;

    while( ind >= 0 )
    {
        if( file->to_volume_index[ind] == INVALID_AXIS )
        {
            ++file->indices[ind];
            if( file->indices[ind] < file->sizes_in_file[ind] )
                break;

            file->indices[ind] = 0;
        }
        --ind;
    }

    if( ind >= 0 )
    {
        file->end_volume_flag = FALSE;

        for_less( ind, 0, file->volume->n_dimensions )
            file->indices[file->valid_file_axes[ind]] = 0;

        for_less( c, 0, N_DIMENSIONS )
        {
            axis = file->spatial_axes[c];
            if( axis != INVALID_AXIS )
                voxel[c] = file->indices[axis];
            else
                voxel[c] = 0.0;
        }

        general_transform_point( &file->voxel_to_world_transform,
                                 voxel[0], voxel[1], voxel[2],
                                 &world_space[X], &world_space[Y],
                                 &world_space[Z]);

        for_less( c, 0, get_volume_n_dimensions(file->volume) )
            voxel[c] = 0.0;

        set_volume_translation( file->volume, voxel, world_space );
    }
    else
        file->end_volume_flag = TRUE;

    return( file->end_volume_flag );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : reset_input_volume
@INPUT      : file
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Rewinds the file indices to start inputting volumes from the
              file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  reset_input_volume(
    Minc_file   file )
{
    int   d;

    for_less( d, 0, file->n_file_dimensions )
        file->indices[d] = 0;
    file->end_volume_flag = FALSE;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : match_dimension_names
@INPUT      : n_volume_dims
              volume_dimension_names
              n_file_dims
              file_dimension_names
@OUTPUT     : to_volume_index
@RETURNS    : TRUE if match found
@DESCRIPTION: Attempts to match all the volume dimensions with the file
              dimensions.  This is done in 3 passes.  In the first pass,
              exact matches are found.  In the second pass, volume dimensions
              of "any_spatial_dimension" are matched.  On the final pass,
              volume dimension names which are empty strings are matched
              to any remaining file dimensions.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  BOOLEAN  match_dimension_names(
    int               n_volume_dims,
    char              *volume_dimension_names[],
    int               n_file_dims,
    char              *file_dimension_names[],
    int               to_volume_index[] )
{
    int      i, j, iteration, n_matches, dummy;
    BOOLEAN  match;
    BOOLEAN  volume_dim_found[MAX_DIMENSIONS];

    n_matches = 0;

    for_less( i, 0, n_file_dims )
        to_volume_index[i] = INVALID_AXIS;

    for_less( i, 0, n_volume_dims )
        volume_dim_found[i] = FALSE;

    for_less( iteration, 0, 3 )
    {
        for( i = n_volume_dims-1;  i >= 0;  --i )
        {
            if( !volume_dim_found[i] )
            {
                for( j = n_file_dims-1;  j >= 0;  --j )
                {
                    if( to_volume_index[j] == INVALID_AXIS )
                    {
                        switch( iteration )
                        {
                        case 0:
                            match = strcmp( volume_dimension_names[i],
                                            file_dimension_names[j] ) == 0;
                            break;
                        case 1:
                            match = (strcmp( volume_dimension_names[i],
                                             ANY_SPATIAL_DIMENSION ) == 0) &&
                                convert_dim_name_to_spatial_axis(
                                     file_dimension_names[j], &dummy );
                            break;
                        case 2:
                            match = (strlen(volume_dimension_names[i]) == 0);
                            break;
                        }

                        if( match )
                        {
                            to_volume_index[j] = i;
                            volume_dim_found[i] = TRUE;
                            ++n_matches;
                            break;
                        }
                    }
                }
            }
        }
    }

    return( n_matches == n_volume_dims );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_minc_file_id
@INPUT      : file
@OUTPUT     : 
@RETURNS    : minc file id
@DESCRIPTION: Returns the minc file id to allow user to perform MINC calls on
              this file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  int  get_minc_file_id(
    Minc_file  file )
{
    return( file->cdfid );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_default_minc_input_options
@INPUT      : 
@OUTPUT     : options
@RETURNS    : 
@DESCRIPTION: Sets the default minc input options.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  set_default_minc_input_options(
    minc_input_options  *options )
{
    set_minc_input_promote_invalid_to_min_flag( options, TRUE );
    set_minc_input_vector_to_scalar_flag( options, TRUE );
    set_minc_input_vector_to_colour_flag( options, FALSE );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_minc_input_promote_invalid_to_min_flag
@INPUT      : flag
@OUTPUT     : options
@RETURNS    : 
@DESCRIPTION: Sets the invalid promotion flag of the input options.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  set_minc_input_promote_invalid_to_min_flag(
    minc_input_options  *options,
    BOOLEAN             flag )
{
    options->promote_invalid_to_min_flag = flag;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_minc_input_vector_to_scalar_flag
@INPUT      : flag
@OUTPUT     : options
@RETURNS    : 
@DESCRIPTION: Sets the vector conversion flag of the input options.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  set_minc_input_vector_to_scalar_flag(
    minc_input_options  *options,
    BOOLEAN             flag )
{
    options->convert_vector_to_scalar_flag = flag;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_minc_input_vector_to_colour_flag
@INPUT      : flag
@OUTPUT     : options
@RETURNS    : 
@DESCRIPTION: Sets the colour conversion flag of the input options.  Any
              volume with a vector dimension of length 3 will be converted
              to a 32 bit colour.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  set_minc_input_vector_to_colour_flag(
    minc_input_options  *options,
    BOOLEAN             flag )
{
    options->convert_vector_to_colour_flag = flag;
}
