#include  <minc.h>
#include  <volume_io.h>

#define  INVALID_AXIS   -1

#define  MIN_SLAB_SIZE   10000      /* at least 10000 entries per read */
#define  MAX_SLAB_SIZE   200000     /* no more than 200 K at a time */

#define  UNITS           "mm"

private  void  create_world_transform(
    Point       *origin,
    Vector      axes[N_DIMENSIONS],
    Real        axis_spacing[N_DIMENSIONS],
    Transform   *transform );

private  BOOLEAN  is_default_direction_cosine(
    int        axis,
    double     dir_cosines[] )
{
    BOOLEAN   is_default;
    int       i;

    is_default = TRUE;
    for_less( i, 0, N_DIMENSIONS )
    {
        if( i == axis && dir_cosines[i] != 1.0 ||
            i != axis && dir_cosines[i] != 0.0 )
        {
            is_default = FALSE;
            break;
        }
    }

    return( is_default );
}

public  Minc_file  initialize_minc_output(
    char                   filename[],
    int                    n_dimensions,
    STRING                 dim_names[],
    int                    sizes[],
    nc_type                file_nc_data_type,
    BOOLEAN                file_signed_flag,
    Real                   file_voxel_min,
    Real                   file_voxel_max,
    Real                   real_min,
    Real                   real_max,
    General_transform      *voxel_to_world_transform,
    minc_output_options    *options )
{
    minc_file_struct    *file;
    int                 dim_vars[MAX_VAR_DIMS];
    double              separation[MAX_VAR_DIMS], valid_range[2];
    double              start[MAX_VAR_DIMS];
    double              dir_cosines[MAX_VAR_DIMS][MI_NUM_SPACE_DIMS];
    int                 i, j, d, axis;
    Point               origin;
    Vector              axes[MAX_DIMENSIONS];
    static  char        *default_dim_names[] = { MIzspace, MIyspace, MIxspace };
    Transform           transform;
    minc_output_options default_options;

    if( options == (minc_output_options *) NULL )
    {
        set_default_minc_output_options( &default_options );
        options = &default_options;
    }

    if( dim_names == (char **) NULL )
    {
        if( n_dimensions != 3 )
        {
            print( "initialize_minc_output: can't use NULL dim_names except with 3 dimensions.\n" );
            return( (Minc_file) NULL );
        }

        dim_names = default_dim_names;
    }

    ALLOC( file, 1 );

    file->file_is_being_read = FALSE;
    file->n_file_dimensions = n_dimensions;

    ncopts = NC_VERBOSE;
    file->cdfid =  micreate( filename, NC_CLOBBER );

    if( file->cdfid == MI_ERROR )
    {
        print( "Error: opening MINC file \"%s\".\n", filename );
        return( (Minc_file) 0 );
    }

    if( get_transform_type( voxel_to_world_transform ) == LINEAR )
    {
        transform = *(get_linear_transform_ptr( voxel_to_world_transform ));
    }
    else
    {
        print( "Cannot output non-linear transforms.  Using identity.\n" );
        make_identity_transform( &transform );
    }

    get_transform_origin( &transform, &origin );
    get_transform_x_axis( &transform, &axes[X] );
    get_transform_y_axis( &transform, &axes[Y] );
    get_transform_z_axis( &transform, &axes[Z] );

    separation[X] = MAGNITUDE( axes[X] );
    separation[Y] = MAGNITUDE( axes[Y] );
    separation[Z] = MAGNITUDE( axes[Z] );
    SCALE_VECTOR( axes[X], axes[X], 1.0 / separation[X] );
    SCALE_VECTOR( axes[Y], axes[Y], 1.0 / separation[Y] );
    SCALE_VECTOR( axes[Z], axes[Z], 1.0 / separation[Z] );

    for_less( i, 0, 3 )
    {
        if( Vector_coord(axes[i],i) < 0.0 )
        {
            SCALE_VECTOR( axes[i], axes[i], -1.0 );
            separation[i] *= -1.0;
        }
    }

    for_less( i, 0, 3 )
    {
        for_less( j, 0, 3 )
        {
            dir_cosines[i][j] = Vector_coord(axes[i],j);
        }
    }

    start[X] = DOT_POINT_VECTOR( origin, axes[X] );
    start[Y] = DOT_POINT_VECTOR( origin, axes[Y] );
    start[Z] = DOT_POINT_VECTOR( origin, axes[Z] );

    for_less( d, 0, n_dimensions )
    {
        file->sizes_in_file[d] = sizes[d];
        file->indices[d] = 0;
        ALLOC( file->dim_names[d], strlen( dim_names[d] + 1 ) );
        (void) strcpy( file->dim_names[d], dim_names[d] );
        dim_vars[d] = ncdimdef( file->cdfid, dim_names[d], sizes[d] );

        if( is_spatial_dimension( dim_names[d], &axis ) )
        {
            file->dim_ids[d] = micreate_std_variable( file->cdfid,
                                dim_names[d], NC_DOUBLE, 0, NULL);
            (void) miattputdbl( file->cdfid, file->dim_ids[d], MIstep,
                                separation[axis]);
            (void) miattputdbl( file->cdfid, file->dim_ids[d], MIstart,
                                start[axis]);
            if( !is_default_direction_cosine( axis, dir_cosines[axis] ) )
            {
                (void) ncattput( file->cdfid, file->dim_ids[d],
                                 MIdirection_cosines,
                                 NC_DOUBLE, N_DIMENSIONS, dir_cosines[axis]);
            }
            (void) miattputstr( file->cdfid, file->dim_ids[d], MIunits, UNITS );
        }
    }

    file->img_var_id = micreate_std_variable( file->cdfid, MIimage,
                                              file_nc_data_type,
                                              n_dimensions, dim_vars );

    if( file_signed_flag )
        (void) miattputstr( file->cdfid, file->img_var_id, MIsigntype,
                            MI_SIGNED );
    else
        (void) miattputstr( file->cdfid, file->img_var_id, MIsigntype,
                            MI_UNSIGNED );

    /* --- put the valid voxel range */

    if( file_voxel_min < file_voxel_max )
    {
        valid_range[0] = file_voxel_min;
        valid_range[1] = file_voxel_max;
        (void) ncattput( file->cdfid, file->img_var_id, MIvalid_range,
                         NC_DOUBLE, 2, (void *) valid_range );
    }

    file->image_range[0] = real_min;
    file->image_range[1] = real_max;

    if( file->image_range[0] < file->image_range[1] )
    {
       file->min_id = micreate_std_variable( file->cdfid, MIimagemin,
                                             NC_DOUBLE, 0, (int *) NULL );
       file->max_id = micreate_std_variable( file->cdfid, MIimagemax,
                                             NC_DOUBLE, 0, (int *) NULL );
    }

    ncopts = NC_VERBOSE | NC_FATAL;

    file->end_def_done = FALSE;
    file->variables_written = FALSE;

    return( file );
}

public  Status  copy_auxiliary_data_from_minc_file(
    Minc_file   file,
    char        filename[],
    char        history_string[] )
{
    Status  status;
    int     src_cdfid;

    ncopts = NC_VERBOSE;
    src_cdfid =  miopen( filename, NC_NOWRITE );

    if( src_cdfid == MI_ERROR )
    {
        print( "Error opening %s\n", filename );
        return( ERROR );
    }

    status = copy_auxiliary_data_from_open_minc_file( file, src_cdfid,
                                                      history_string );

    (void) miclose( src_cdfid );

    ncopts = NC_VERBOSE | NC_FATAL;

    return( status );
}

public  Status  copy_auxiliary_data_from_open_minc_file(
    Minc_file   file,
    int         src_cdfid,
    char        history_string[] )
{
    int     src_img_var, varid, n_excluded, excluded_vars[10];
    int     src_min_id, src_max_id, src_root_id;
    Status  status;

    if( file->end_def_done )
    {
        print( "Cannot call copy_auxiliary_data_from_open_minc_file when not in define mode\n" );
        return( ERROR );
    }

    ncopts = NC_VERBOSE;

    n_excluded = 0;
    if( (varid = ncvarid(src_cdfid, MIxspace )) != MI_ERROR )
        excluded_vars[n_excluded++] = varid;
    if( (varid = ncvarid(src_cdfid, MIyspace )) != MI_ERROR )
        excluded_vars[n_excluded++] = varid;
    if( (varid = ncvarid(src_cdfid, MIzspace )) != MI_ERROR )
        excluded_vars[n_excluded++] = varid;
    if( (src_img_var = ncvarid(src_cdfid, MIimage )) != MI_ERROR )
        excluded_vars[n_excluded++] = src_img_var;
    if( (src_max_id = ncvarid(src_cdfid, MIimagemax )) != MI_ERROR )
        excluded_vars[n_excluded++] = src_max_id;
    if( (src_min_id = ncvarid(src_cdfid, MIimagemin )) != MI_ERROR )
        excluded_vars[n_excluded++] = src_min_id;
    if( (src_root_id = ncvarid(src_cdfid, MIrootvariable )) != MI_ERROR )
        excluded_vars[n_excluded++] = src_root_id;

    (void) micopy_all_var_defs( src_cdfid, file->cdfid, n_excluded,
                                excluded_vars );

    if( src_img_var != MI_ERROR )
        (void) micopy_all_atts( src_cdfid, src_img_var,
                                file->cdfid, file->img_var_id );

    ncopts = 0;
    (void) ncattdel( file->cdfid, file->img_var_id, MIvalid_max );
    (void) ncattdel( file->cdfid, file->img_var_id, MIvalid_min );
    ncopts = NC_VERBOSE;

    if( src_min_id != MI_ERROR )
    {
        (void) micopy_all_atts( src_cdfid, src_min_id,
                                file->cdfid, file->min_id );
    }

    if( src_max_id != MI_ERROR )
    {
        (void) micopy_all_atts( src_cdfid, src_max_id,
                                file->cdfid, file->max_id );
    }

    if( src_root_id != MI_ERROR )
    {
        (void) micopy_all_atts( src_cdfid, src_root_id,
                                file->cdfid,
                                ncvarid( file->cdfid, MIrootvariable) );
    }

    status = OK;

    if( history_string != (char *) NULL )
        status = add_minc_history( file, history_string );

    (void) ncendef( file->cdfid );
    file->end_def_done = TRUE;

    (void) micopy_all_var_values( src_cdfid, file->cdfid,
                                  n_excluded, excluded_vars );

    ncopts = NC_VERBOSE | NC_FATAL;

    return( status );
}

public  Status  add_minc_history(
    Minc_file   file,
    char        history_string[] )
{
    int      att_length;
    nc_type  datatype;
    char     *new_history;

    if( file->end_def_done )
    {
        print( "Cannot call add_minc_history when not in define mode\n" );
        return( ERROR );
    }

    ncopts = 0;

    if( ncattinq(file->cdfid, NC_GLOBAL, MIhistory, &datatype, &att_length)
                                                          == MI_ERROR ||
        datatype != NC_CHAR )
    {
        att_length = 0;
    }

    att_length += strlen(history_string) + 1;

    /* Allocate a string and get the old history */

    ALLOC( new_history, att_length );

    new_history[0] = '\0';

    (void) miattgetstr( file->cdfid, NC_GLOBAL, MIhistory, att_length,
                        new_history );

    /* Add the new command and put the new history. */
    (void) strcat( new_history, history_string );

    ncopts = NC_VERBOSE | NC_FATAL;
    (void) miattputstr( file->cdfid, NC_GLOBAL, MIhistory, new_history );

    FREE( new_history );

    return( OK );
}

private  Status  get_dimension_ordering(
    int          n_vol_dims,
    char         *vol_dim_names[],
    int          n_file_dims,
    char         *file_dim_names[],
    int          to_volume[],
    int          to_file[] )
{
    Status   status;
    int      v, f, n_found;

    for_less( f, 0, n_file_dims )
        to_volume[f] = -1;

    for_less( v, 0, n_vol_dims )
        to_file[v] = -1;

    n_found = 0;

    for_less( v, 0, n_vol_dims )
    {
        for_less( f, 0, n_file_dims )
        {
            if( to_volume[f] < 0 &&
                strcmp( vol_dim_names[v], file_dim_names[f] ) == 0 )
            {
                to_volume[f] = v;
                to_file[v] = f;
                ++n_found;
            }
        }
    }

    if( n_found != n_vol_dims )
    {
        print( "Unsuccessful matching of volume and output dimension names.\n");
        status = ERROR;
    }
    else
        status = OK;

    return( status );
}

private  void  output_slab(
    Minc_file   file,
    Volume      volume,
    int         to_volume[],
    long        start[],
    long        count[] )
{
    int      ind, expected_ind, n_vol_dims, file_ind;
    int      iv[MAX_DIMENSIONS];
    void     *void_ptr;
    BOOLEAN  direct_to_volume, signed_flag, non_full_size_found;
    Volume   tmp_volume;
    int      tmp_ind, tmp_sizes[MAX_DIMENSIONS];
    int      tmp_vol_indices[MAX_DIMENSIONS];
    int      zero[MAX_DIMENSIONS];
    nc_type  data_type;

    direct_to_volume = TRUE;
    n_vol_dims = get_volume_n_dimensions( volume );
    expected_ind = n_vol_dims-1;
    tmp_ind = file->n_slab_dims-1;
    non_full_size_found = FALSE;

    for_less( ind, 0, n_vol_dims )
        tmp_vol_indices[ind] = -1;

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
                tmp_vol_indices[ind] = tmp_ind;
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
        (void) miicv_put( file->icv, start, count, void_ptr );
    }
    else
    {
        data_type = get_volume_nc_data_type( volume, &signed_flag );

        tmp_volume = create_volume( file->n_slab_dims, NULL,
                                    data_type, signed_flag,
                                    get_volume_voxel_min(volume),
                                    get_volume_voxel_max(volume) );

        set_volume_sizes( tmp_volume, tmp_sizes );
        alloc_volume_data( tmp_volume );

        copy_volumes_reordered( tmp_volume, zero, volume, iv, tmp_vol_indices );

        GET_VOXEL_PTR( void_ptr, tmp_volume, 0, 0, 0, 0, 0 );
        (void) miicv_put( file->icv, start, count, void_ptr );

        delete_volume( tmp_volume );
    }
}

public  Status  output_minc_volume(
    Minc_file   file,
    Volume      volume )
{
    int               d, axis, n_volume_dims, sizes[MAX_DIMENSIONS];
    int               slab_size, n_slab;
    int               to_volume[MAX_DIMENSIONS], to_file[MAX_DIMENSIONS];
    int               vol_index, step, n_steps;
    long              count[MAX_VAR_DIMS];
    long              start_index, mindex[MAX_VAR_DIMS];
    Real              voxel_min, voxel_max;
    char              **vol_dimension_names;
    double            dim_value;
    BOOLEAN           increment;
    progress_struct   progress;

    if( !file->end_def_done )
    {
        /* --- Get into data mode */

        (void) ncendef( file->cdfid );
        file->end_def_done = TRUE;
    }

    if( !file->variables_written )
    {
        file->variables_written = TRUE;

        ncopts = NC_VERBOSE;
        for_less( d, 0, file->n_file_dimensions )
            mindex[d] = 0;

        dim_value = 0.0;
        for_less( d, 0, file->n_file_dimensions )
        {
            if( is_spatial_dimension( file->dim_names[d], &axis ) )
            {
                (void) mivarput1( file->cdfid, file->dim_ids[d], mindex,
                                  NC_DOUBLE, MI_SIGNED, &dim_value );
            }
        }

        file->icv = miicv_create();

        (void) miicv_setint( file->icv, MI_ICV_TYPE, volume->nc_data_type);
        (void) miicv_setstr( file->icv, MI_ICV_SIGN,
                             volume->signed_flag ? MI_SIGNED : MI_UNSIGNED );
        (void) miicv_setint( file->icv, MI_ICV_DO_NORM, TRUE );
        (void) miicv_setint( file->icv, MI_ICV_USER_NORM, TRUE );
        (void) miicv_setdbl( file->icv, MI_ICV_IMAGE_MIN, file->image_range[0]);
        (void) miicv_setdbl( file->icv, MI_ICV_IMAGE_MAX, file->image_range[1]);

        get_volume_voxel_range( volume, &voxel_min, &voxel_max );
        if( voxel_min < voxel_max )
        {
            (void) miicv_setdbl( file->icv, MI_ICV_VALID_MIN, voxel_min );
            (void) miicv_setdbl( file->icv, MI_ICV_VALID_MAX, voxel_max );
        }
        else
            print( "Volume has invalid min and max voxel value\n" );

        (void) miicv_attach( file->icv, file->cdfid, file->img_var_id );

        start_index = 0;

        if( file->image_range[0] < file->image_range[1] )
        {
            (void) mivarput1( file->icv, file->min_id, &start_index,
                              NC_DOUBLE, MI_SIGNED, &file->image_range[0] );
            (void) mivarput1( file->icv, file->max_id, &start_index,
                              NC_DOUBLE, MI_SIGNED, &file->image_range[1] );
        }
        ncopts = NC_VERBOSE | NC_FATAL;
    }

    n_volume_dims = get_volume_n_dimensions( volume );

    if( n_volume_dims > file->n_file_dimensions )
    {
        print(
        "output_minc_volume: volume (%d) has more dimensions than file (%d).\n",
            n_volume_dims, file->n_file_dimensions );
        return( ERROR );
    }

    /*--- find correspondence between volume dimensions and file dimensions */

    vol_dimension_names = get_volume_dimension_names( volume );

    if( get_dimension_ordering( n_volume_dims, vol_dimension_names,
                                file->n_file_dimensions, file->dim_names,
                                to_volume, to_file ) != OK )
        return( ERROR );

    delete_dimension_names( vol_dimension_names );

    get_volume_sizes( volume, sizes );

    /*--- check sizes match between volume and file */

    for_less( d, 0, file->n_file_dimensions )
    {
        vol_index = to_volume[d];
 
        if( vol_index >= 0 && sizes[vol_index] != file->sizes_in_file[d] )
        {
            print(
            "output_minc_volume: volume size[%d]=%d does not match file[%d]=%d.\n",
                vol_index, sizes[vol_index], d, file->sizes_in_file[d] );
            return( ERROR );
        }
    }

    /*--- check number of volumes written */

    d = 0;
    while( d < file->n_file_dimensions && to_volume[d] != INVALID_AXIS )
        ++d;

    if( d < file->n_file_dimensions &&
        file->indices[d] >= file->sizes_in_file[d] )
    {
        print( "output_minc_volume: attempted to write too many subvolumes.\n");
        return( ERROR );
    }

    /*--- determine which contiguous blocks of volume to output */

    file->n_slab_dims = 0;
    slab_size = 1;
    d = file->n_file_dimensions-1;

    do
    {
        if( to_volume[d] != INVALID_AXIS )
        {
            ++file->n_slab_dims;
            slab_size *= file->sizes_in_file[d];
        }
        --d;
    }
    while( d >= 0 && slab_size < MIN_SLAB_SIZE );

    if( slab_size > MAX_SLAB_SIZE && file->n_slab_dims > 1 )
        --file->n_slab_dims;

    /*--- now write entire volume in contiguous chunks (possibly only 1 req'd)*/

    n_slab = 0;
    n_steps = 1;

    for( d = file->n_file_dimensions-1;  d >= 0;  --d )
    {
        if( to_volume[d] == INVALID_AXIS || n_slab >= file->n_slab_dims )
            n_steps *= file->sizes_in_file[d];
        if( to_volume[d] != INVALID_AXIS )
            ++n_slab;
    }

    step = 0;

    initialize_progress_report( &progress, FALSE, n_steps,"Outputting Volume" );

    increment = FALSE;
    while( !increment )
    {
        /*--- set the indices of the file array to write */

        n_slab = 0;
        for( d = file->n_file_dimensions-1;  d >= 0;  --d )
        {
            if( to_volume[d] == INVALID_AXIS || n_slab >= file->n_slab_dims )
                count[d] = 1;
            else
                count[d] = file->sizes_in_file[d];

            if( to_volume[d] != INVALID_AXIS )
                ++n_slab;
        }

        output_slab( file, volume, to_volume, file->indices, count );

        increment = TRUE;

        /*--- increment the file index dimensions which correspond
              to volume dimensions not output */

        d = file->n_file_dimensions-1;
        n_slab = 0;
        while( increment && d >= 0 )
        {
            if( to_volume[d] != INVALID_AXIS && n_slab >= file->n_slab_dims )
            {
                ++file->indices[d];
                if( file->indices[d] < file->sizes_in_file[d] )
                    increment = FALSE;
                else
                    file->indices[d] = 0;
            }

            if( to_volume[d] != INVALID_AXIS )
                ++n_slab;

            --d;
        }

        ++step;

        if( n_steps > 1 )
            update_progress_report( &progress, step );
    }

    terminate_progress_report( &progress );

    /*--- increment the file index dimensions which do not
          correspond to volume dimensions */

    d = file->n_file_dimensions-1;
    while( increment && d <= 0 )
    {
        if( to_volume[d] == INVALID_AXIS )
        {
            ++file->indices[d];

            if( file->indices[d] < file->sizes_in_file[d] )
                increment = FALSE;
            else
                file->indices[d] = 0;
        }

        --d;
    }

    return( OK );
}

public  Status  close_minc_output(
    Minc_file   file )
{
    int    d;

    if( file == (Minc_file) NULL )
    {
        print( "close_minc_output(): NULL file.\n" );
        return( ERROR );
    }

    (void) miclose( file->cdfid );
    (void) miicv_free( file->icv );

    for_less( d, 0, file->n_file_dimensions )
        FREE( file->dim_names[d] );

    FREE( file );

    return( OK );
}

public  void  set_default_minc_output_options(
    minc_output_options  *options           )
{
    int   i;

    for_less( i, 0, MAX_DIMENSIONS )
        (void) strcpy( options->dimension_names[i], "" );
}

public  void  set_minc_output_dimensions_order(
    minc_output_options  *options,
    int                  n_dimensions,
    STRING               dimension_names[] )
{
    int   i;

    for_less( i, 0, n_dimensions )
        (void) strcpy( options->dimension_names[i], dimension_names[i] );
}
