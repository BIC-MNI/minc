#include  <minc.h>
#include  <def_mni.h>

#define  INVALID_AXIS   -1

#define  MIN_SLAB_SIZE   10000      /* at least 10000 entries per read */
#define  MAX_SLAB_SIZE   200000     /* no more than 200 K at a time */

#define  UNITS           "mm"

private  void  create_world_transform(
    Point       *origin,
    Vector      axes[N_DIMENSIONS],
    Real        axis_spacing[N_DIMENSIONS],
    Transform   *transform );
private  int  match_dimension_names(
    int               n_volume_dims,
    String            *volume_dimension_names,
    int               n_file_dims,
    String            *file_dimension_names,
    int               axis_index_in_file[] );

private  Boolean  is_default_direction_cosine(
    int        axis,
    double     dir_cosines[] )
{
    Boolean   is_default;
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
    char       filename[],
    int        n_dimensions,
    String     dim_names[],
    int        sizes[],
    nc_type    nc_data_type,
    Boolean    signed_flag,
    Real       min_voxel,
    Real       max_voxel,
    Real       real_min,
    Real       real_max,
    Transform  *voxel_to_world_transform )
{
    minc_file_struct    *file;
    int                 dim_vars[MAX_VAR_DIMS];
    double              separation[MAX_VAR_DIMS], valid_range[2];
    double              start[MAX_VAR_DIMS];
    double              dir_cosines[MAX_VAR_DIMS][MI_NUM_SPACE_DIMS];
    int                 i, j, d, axis;
    Point               origin;
    Vector              axes[N_DIMENSIONS];
    static  String      default_dim_names[] = { MIzspace, MIyspace, MIxspace };

    if( dim_names == (String *) NULL )
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
    file->cdfid =  nccreate( filename, NC_CLOBBER );

    if( file->cdfid == MI_ERROR )
    {
        print( "Error: opening MINC file \"%s\".\n", filename );
        return( (Minc_file) 0 );
    }

    get_transform_origin( voxel_to_world_transform, &origin );
    get_transform_x_axis( voxel_to_world_transform, &axes[X] );
    get_transform_y_axis( voxel_to_world_transform, &axes[Y] );
    get_transform_z_axis( voxel_to_world_transform, &axes[Z] );

    separation[X] = MAGNITUDE( axes[X] );
    separation[Y] = MAGNITUDE( axes[Y] );
    separation[Z] = MAGNITUDE( axes[Z] );
    SCALE_VECTOR( axes[X], axes[X], 1.0 / separation[X] );
    SCALE_VECTOR( axes[Y], axes[Y], 1.0 / separation[Y] );
    SCALE_VECTOR( axes[Z], axes[Z], 1.0 / separation[Z] );

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
        (void) strcpy( file->dim_names[d], dim_names[d] );
        dim_vars[d] = ncdimdef( file->cdfid, dim_names[d], sizes[d] );

        if( is_spatial_dimension( dim_names[d], &axis ) )
        {
            file->dim_ids[d] = micreate_std_variable( file->cdfid, dim_names[d],
                                                      NC_DOUBLE, 0, NULL);
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
                                              nc_data_type,
                                              n_dimensions, dim_vars );

    if( signed_flag )
        (void) miattputstr( file->cdfid, file->img_var_id, MIsigntype,
                            MI_SIGNED );
    else
        (void) miattputstr( file->cdfid, file->img_var_id, MIsigntype,
                            MI_UNSIGNED );

    /* --- put the valid voxel range */

    if( min_voxel < max_voxel )
    {
        valid_range[0] = min_voxel;
        valid_range[1] = max_voxel;
        (void) ncattput( file->cdfid, file->img_var_id, MIvalid_range,
                         NC_DOUBLE, 2, (void *) valid_range );
    }
    else
    {
        print(
          "initialize_minc_output:  min voxel must be less than max voxel.\n" );
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
    src_cdfid =  ncopen( filename, NC_NOWRITE );

    if( src_cdfid == MI_ERROR )
    {
        print( "Error opening %s\n", filename );
        return( ERROR );
    }

    status = copy_auxiliary_data_from_open_minc_file( file, src_cdfid,
                                                      history_string );

    (void) ncclose( src_cdfid );

    ncopts = NC_VERBOSE | NC_FATAL;

    return( status );
}

public  Status  copy_auxiliary_data_from_open_minc_file(
    Minc_file   file,
    int         src_cdfid,
    char        history_string[] )
{
    int     src_img_var, varid, n_excluded, excluded_vars[10];
    int     src_min_id, src_max_id;
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
        (void) micopy_all_atts( src_cdfid, src_min_id,
                                file->cdfid, file->min_id );

    if( src_max_id != MI_ERROR )
        (void) micopy_all_atts( src_cdfid, src_max_id,
                                file->cdfid, file->max_id );

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

public  Status  output_minc_volume(
    Minc_file   file,
    Volume      volume )
{
    int    d, axis, n_volume_dims, sizes[MAX_DIMENSIONS];
    long   start[MAX_VAR_DIMS], count[MAX_VAR_DIMS];
    long   start_index, mindex[MAX_VAR_DIMS];
    Real   min_value, max_value;
    double dim_value;
    void   *data_ptr;

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

        get_volume_voxel_range( volume, &min_value, &max_value );
        if( min_value < max_value )
        {
            (void) miicv_setdbl( file->icv, MI_ICV_VALID_MIN, min_value );
            (void) miicv_setdbl( file->icv, MI_ICV_VALID_MAX, max_value );
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

    get_volume_sizes( volume, sizes );

    for_less( d, 0, n_volume_dims )
    {
        if( sizes[d] !=
            file->sizes_in_file[d + file->n_file_dimensions - n_volume_dims] )
        {
            print(
            "output_minc_volume: volume size[%d]=%d does not match file[%d]=%d.\n",
                d, sizes[d], d + file->n_file_dimensions - n_volume_dims,
                file->sizes_in_file[d + file->n_file_dimensions-n_volume_dims]);
            return( ERROR );
        }
    }

    if( file->indices[0] >= file->sizes_in_file[0] )
    {
        print( "output_minc_volume: attempted to write too many subvolumes.\n");
        return( ERROR );
    }

    for_less( d, 0, file->n_file_dimensions )
    {
        if( d < file->n_file_dimensions - n_volume_dims )
        {
            start[d] = file->indices[d];
            count[d] = 1;
        }
        else
        {
            start[d] = 0;
            count[d] = file->sizes_in_file[d];
        }
    }

    GET_VOXEL_PTR( data_ptr, volume, 0, 0, 0, 0, 0 );
    (void) miicv_put( file->icv, start, count, (void *) data_ptr );

    d = file->n_file_dimensions - n_volume_dims - 1;
    if( d < 0 )
        d = 0;

    while( TRUE )
    {
        ++file->indices[d];

        if( file->indices[d] < file->sizes_in_file[d] || d == 0 )
            break;

        file->indices[d] = 0;

        --d;
    }

    return( OK );
}

public  Status  close_minc_output(
    Minc_file   file )
{
    if( file == (Minc_file) NULL )
    {
        print( "close_minc_output(): NULL file.\n" );
        return( ERROR );
    }

    (void) ncclose( file->cdfid );
    (void) miicv_free( file->icv );

    FREE( file );

    return( OK );
}
