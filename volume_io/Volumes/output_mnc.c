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
#include  <minc.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Volumes/output_mnc.c,v 1.37 1995-10-19 15:47:08 david Exp $";
#endif

#define  INVALID_AXIS   -1

#define  MIN_SLAB_SIZE   10000      /* at least 10000 entries per read */
#define  MAX_SLAB_SIZE   200000     /* no more than 200 K at a time */

#define  UNITS           "mm"

private  Status  get_dimension_ordering(
    int          n_vol_dims,
    STRING       vol_dim_names[],
    int          n_file_dims,
    STRING       file_dim_names[],
    int          to_volume[],
    int          to_file[] );

/* ----------------------------- MNI Header -----------------------------------
@NAME       : is_default_direction_cosine
@INPUT      : axis
              dir_cosines
@OUTPUT     : 
@RETURNS    : TRUE if is default
@DESCRIPTION: Checks to see if the cosine is the default for the axis,
              i.e., for x axis, is it ( 1, 0, 0 ).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : initialize_minc_output
@INPUT      : filename
              n_dimensions
              dim_names
              sizes
              file_nc_data_type
              file_signed_flag
              file_voxel_min
              file_voxel_max
              voxel_to_world_transform
              volume_to_attach
              options
@OUTPUT     : 
@RETURNS    : minc file
@DESCRIPTION: Creates a minc file for outputting volumes.  The n_dimensions,
              dim_names, sizes, file_nc_data_type, file_signed_flag,
              file_voxel_min, file_voxel_max, and voxel_to_world_transform
              define the type and shape of the file.  The volume_to_attach
              is the volume that will be output once or many times to 
              fill up the file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Minc_file  initialize_minc_output(
    STRING                 filename,
    int                    n_dimensions,
    STRING                 dim_names[],
    int                    sizes[],
    nc_type                file_nc_data_type,
    BOOLEAN                file_signed_flag,
    Real                   file_voxel_min,
    Real                   file_voxel_max,
    General_transform      *voxel_to_world_transform,
    Volume                 volume_to_attach,
    minc_output_options    *options )
{
    minc_file_struct    *file;
    int                 dim_vars[MAX_VAR_DIMS], volume_sizes[MAX_DIMENSIONS];
    int                 n_volume_dims;
    double              separation[MAX_VAR_DIMS], valid_range[2];
    Real                start[MAX_VAR_DIMS];
    double              dir_cosines[MAX_VAR_DIMS][MI_NUM_SPACE_DIMS];
    int                 i, j, d, axis, vol_index, n_range_dims;
    Point               origin;
    Vector              axes[MAX_DIMENSIONS];
    static  STRING      default_dim_names[] = { { MIzspace }, { MIyspace },
                                                { MIxspace } };
    Transform           transform, t, inverse;
    STRING              *vol_dimension_names;
    minc_output_options default_options;

    if( options == (minc_output_options *) NULL )
    {
        set_default_minc_output_options( &default_options );
        options = &default_options;
    }

    if( dim_names == NULL )
    {
        if( n_dimensions != 3 )
        {
            print_error( "initialize_minc_output: " );
            print_error(
                "can't use NULL dim_names except with 3 dimensions.\n" );
            return( (Minc_file) NULL );
        }

        dim_names = default_dim_names;
    }

    if( file_nc_data_type == NC_UNSPECIFIED )
    {
        file_nc_data_type = get_volume_nc_data_type( volume_to_attach,
                                                     &file_signed_flag );
        get_volume_voxel_range( volume_to_attach,
                                &file_voxel_min, &file_voxel_max );
    }
    else if( (file_nc_data_type == NC_FLOAT ||
              file_nc_data_type == NC_DOUBLE) &&
              file_voxel_min >= file_voxel_max )
    {
        get_volume_real_range( volume_to_attach,
                               &file_voxel_min, &file_voxel_max );
    }

    /* --- check if dimension name correspondence between volume and file */

    n_volume_dims = get_volume_n_dimensions( volume_to_attach );

    if( n_volume_dims > n_dimensions )
    {
        print_error( "initialize_minc_output:" );
        print_error( " volume (%d) has more dimensions than file (%d).\n",
                     n_volume_dims, n_dimensions );
        return( (Minc_file) NULL );
    }

    ALLOC( file, 1 );

    file->file_is_being_read = FALSE;
    file->n_file_dimensions = n_dimensions;
    file->volume = volume_to_attach;
    file->entire_file_written = FALSE;
    file->ignoring_because_cached = FALSE;

    file->filename = expand_filename( filename );

    if( volume_to_attach->is_cached_volume &&
        volume_to_attach->cache.output_file_is_open &&
        equal_strings( volume_to_attach->cache.output_filename, file->filename))
    {
        file->ignoring_because_cached = TRUE;
        return( file );
    }

    /*--- find correspondence between volume dimensions and file dimensions */

    vol_dimension_names = get_volume_dimension_names( volume_to_attach );

    for_less( d, 0, n_dimensions )
        dim_names[d] = dim_names[d];

    if( get_dimension_ordering( n_volume_dims, vol_dimension_names,
                                n_dimensions, dim_names,
                            file->to_volume_index, file->to_file_index ) != OK )
    {
        FREE( file );
        return( (Minc_file) NULL );
    }

    delete_dimension_names( volume_to_attach, vol_dimension_names );

    /*--- check if image range specified */

    if( options->global_image_range[0] >= options->global_image_range[1] )
    {
        n_range_dims = n_dimensions - 2;
        if( equal_strings( dim_names[n_dimensions-1], MIvector_dimension ) )
            --n_range_dims;

        for_less( d, n_range_dims, n_dimensions )
        {
            if( file->to_volume_index[d] == INVALID_AXIS )
            {
                print_error( "initialize_minc_output: " );
                print_error( "if outputting volumes which don't contain all image\n");
                print_error( "dimensions, then must specify global image range.\n" );
                FREE( file );
                return( (Minc_file) NULL );
            }
        }
    }

    /*--- check sizes match between volume and file */

    get_volume_sizes( volume_to_attach, volume_sizes );

    for_less( d, 0, n_dimensions )
    {
        vol_index = file->to_volume_index[d];

        if( vol_index >= 0 && volume_sizes[vol_index] != sizes[d] )
        {
            print_error( "initialize_minc_output: " );
            print_error( "volume size[%d]=%d does not match file[%d]=%d.\n",
                   vol_index, volume_sizes[vol_index], d, sizes[d] );
            return( (Minc_file) NULL );
        }
    }

    /*--- create the file */

    ncopts = NC_VERBOSE;

    file->cdfid =  micreate( file->filename, NC_CLOBBER );

    if( file->cdfid == MI_ERROR )
    {
        print_error( "Error: opening MINC file \"%s\".\n", file->filename );
        return( (Minc_file) 0 );
    }

    if( voxel_to_world_transform == (General_transform *) NULL )
    {
        make_identity_transform( &transform );
    }
    else if( get_transform_type( voxel_to_world_transform ) == LINEAR )
    {
        transform = *(get_linear_transform_ptr( voxel_to_world_transform ));
    }
    else
    {
        print_error( "Cannot output non-linear transforms.  Using identity.\n" );
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

    make_identity_transform( &t );
    set_transform_x_axis( &t, &axes[X] );
    set_transform_y_axis( &t, &axes[Y] );
    set_transform_z_axis( &t, &axes[Z] );

    (void) compute_transform_inverse( &t, &inverse );

    transform_point( &inverse,
                     (Real) Point_x(origin), (Real) Point_y(origin),
                     (Real) Point_z(origin),
                     &start[X], &start[Y], &start[Z] );

    for_less( d, 0, n_dimensions )
    {
        file->sizes_in_file[d] = sizes[d];
        file->indices[d] = 0;
        file->dim_names[d] = create_string( dim_names[d] );
        dim_vars[d] = ncdimdef( file->cdfid, dim_names[d], sizes[d] );

        if( convert_dim_name_to_spatial_axis( dim_names[d], &axis ) )
        {
            file->dim_ids[d] = micreate_std_variable( file->cdfid,
                                      dim_names[d], NC_DOUBLE, 0, NULL);

            if( file->dim_ids[d] < 0 )
            {
                FREE( file );
                return( (Minc_file) NULL );
            }

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
        else
            file->dim_ids[d] = -1;
    }

    file->img_var_id = micreate_std_variable( file->cdfid, MIimage,
                                              file_nc_data_type,
                                              n_dimensions, dim_vars );
    (void) miattputstr( file->cdfid, file->img_var_id, MIcomplete, MI_FALSE );

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

    file->image_range[0] = options->global_image_range[0];
    file->image_range[1] = options->global_image_range[1];

    if( file->image_range[0] < file->image_range[1] )
    {
        file->min_id = micreate_std_variable( file->cdfid, MIimagemin,
                                              NC_DOUBLE, 0, (int *) NULL );
        file->max_id = micreate_std_variable( file->cdfid, MIimagemax,
                                              NC_DOUBLE, 0, (int *) NULL );
    }
    else
    {
        n_range_dims = n_dimensions - 2;
        if( strcmp( dim_names[n_dimensions-1], MIvector_dimension ) == 0 )
            --n_range_dims;

        file->min_id = micreate_std_variable( file->cdfid, MIimagemin,
                                      NC_DOUBLE, n_range_dims, file->dim_ids );
        file->max_id = micreate_std_variable( file->cdfid, MIimagemax,
                                      NC_DOUBLE, n_range_dims, file->dim_ids );
    }

    ncopts = NC_VERBOSE | NC_FATAL;

    file->end_def_done = FALSE;
    file->variables_written = FALSE;

    return( file );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : copy_auxiliary_data_from_minc_file
@INPUT      : file
              filename
              history_string
@OUTPUT     : 
@RETURNS    : OK or ERROR
@DESCRIPTION: Copies the auxiliary data from the filename to the opened
              Minc file, 'file'.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  copy_auxiliary_data_from_minc_file(
    Minc_file   file,
    STRING      filename,
    STRING      history_string )
{
    Status  status;
    int     src_cdfid;
    STRING  expanded;

    if( file->ignoring_because_cached )
        return( OK );

    ncopts = NC_VERBOSE;

    expanded = expand_filename( filename );

    src_cdfid =  miopen( expanded, NC_NOWRITE );

    if( src_cdfid == MI_ERROR )
    {
        print_error( "Error opening %s\n", expanded );
        return( ERROR );
    }

    delete_string( expanded );

    status = copy_auxiliary_data_from_open_minc_file( file, src_cdfid,
                                                      history_string );

    (void) miclose( src_cdfid );

    ncopts = NC_VERBOSE | NC_FATAL;

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : copy_auxiliary_data_from_open_minc_file
@INPUT      : file
              src_cdfid
              history_string
@OUTPUT     : 
@RETURNS    : OK or ERROR
@DESCRIPTION: Copies the auxiliary data from the opened minc file specified
              by src_cdfid to the opened minc file specified by 'file'.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  copy_auxiliary_data_from_open_minc_file(
    Minc_file   file,
    int         src_cdfid,
    STRING      history_string )
{
    int     src_img_var, varid, n_excluded, excluded_vars[10];
    int     src_min_id, src_max_id, src_root_id;
    Status  status;

    if( file->ignoring_because_cached )
        return( OK );

    if( file->end_def_done )
    {
        print_error( "Cannot call copy_auxiliary_data_from_open_minc_file when not in define mode\n" );
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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : add_minc_history
@INPUT      : file
              history_string
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Adds the history_string to the history in the file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  add_minc_history(
    Minc_file   file,
    STRING      history_string )
{
    int      new_att_length, old_att_length;
    nc_type  datatype;
    STRING   new_history;

    if( file->ignoring_because_cached )
        return( OK );

    if( file->end_def_done )
    {
        print_error( "Cannot call add_minc_history when not in define mode\n" );
        return( ERROR );
    }

    ncopts = 0;

    if( ncattinq(file->cdfid, NC_GLOBAL, MIhistory, &datatype, &old_att_length)
                                                          == MI_ERROR ||
        datatype != NC_CHAR )
    {
        old_att_length = 0;
    }

    new_att_length = old_att_length + string_length(history_string);

    /* Allocate a string and get the old history */

    new_history = alloc_string( new_att_length );

    (void) miattgetstr( file->cdfid, NC_GLOBAL, MIhistory, old_att_length+1,
                        new_history );

    /* Add the new command and put the new history. */

    concat_to_string( &new_history, history_string );

    ncopts = NC_VERBOSE | NC_FATAL;
    (void) miattputstr( file->cdfid, NC_GLOBAL, MIhistory, new_history );

    delete_string( new_history );

    return( OK );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_dimension_ordering
@INPUT      : n_vol_dims
              vol_dim_names
              n_file_dims
              file_dim_names
@OUTPUT     : to_volume
              to_file
@RETURNS    : OK or ERROR
@DESCRIPTION: Matches dimension names between the volume and file, setting
              the axis conversion from file to_volume and from volume to_file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  Status  get_dimension_ordering(
    int          n_vol_dims,
    STRING       vol_dim_names[],
    int          n_file_dims,
    STRING       file_dim_names[],
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
        print_error( "Unsuccessful matching of volume and output dimension names.\n");
        status = ERROR;
    }
    else
        status = OK;

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : check_minc_output_variables
@INPUT      : file
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Checks if the file variables has been put into data mode,
              and does so if necessary.  Then it checks if the variables have
              been written, and does so if necessary.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Sep. 1, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  check_minc_output_variables(
    Minc_file   file )
{
    int               d, axis;
    long              start_index, mindex[MAX_VAR_DIMS];
    Real              voxel_min, voxel_max, real_min, real_max;
    double            dim_value;
    Volume            volume;

    if( !file->end_def_done )
    {
        /* --- Get into data mode */

        (void) ncendef( file->cdfid );
        file->end_def_done = TRUE;
    }

    if( !file->variables_written )
    {
        volume = file->volume;

        file->variables_written = TRUE;

        ncopts = NC_VERBOSE;
        for_less( d, 0, file->n_file_dimensions )
            mindex[d] = 0;

        dim_value = 0.0;
        for_less( d, 0, file->n_file_dimensions )
        {
            if( convert_dim_name_to_spatial_axis( file->dim_names[d], &axis ) )
            {
                (void) mivarput1( file->cdfid, file->dim_ids[d], mindex,
                                  NC_DOUBLE, MI_SIGNED, &dim_value );
            }
        }

        file->minc_icv = miicv_create();

        (void) miicv_setint( file->minc_icv, MI_ICV_TYPE, volume->nc_data_type);
        (void) miicv_setstr( file->minc_icv, MI_ICV_SIGN,
                             volume->signed_flag ? MI_SIGNED : MI_UNSIGNED );
        (void) miicv_setint( file->minc_icv, MI_ICV_DO_NORM, TRUE );
        (void) miicv_setint( file->minc_icv, MI_ICV_USER_NORM, TRUE );

        if( file->image_range[0] < file->image_range[1] )
        {
            (void) miicv_setdbl( file->minc_icv, MI_ICV_IMAGE_MIN,
                                 file->image_range[0] );
            (void) miicv_setdbl( file->minc_icv, MI_ICV_IMAGE_MAX,
                                 file->image_range[1] );
        }
        else
        {
            get_volume_real_range( volume, &real_min, &real_max );
            (void) miicv_setdbl( file->minc_icv, MI_ICV_IMAGE_MIN, real_min );
            (void) miicv_setdbl( file->minc_icv, MI_ICV_IMAGE_MAX, real_max );
        }

        get_volume_voxel_range( volume, &voxel_min, &voxel_max );
        if( voxel_min < voxel_max )
        {
            (void) miicv_setdbl( file->minc_icv, MI_ICV_VALID_MIN, voxel_min );
            (void) miicv_setdbl( file->minc_icv, MI_ICV_VALID_MAX, voxel_max );
        }
        else
            print_error( "Volume has invalid min and max voxel value\n" );

        (void) miicv_attach( file->minc_icv, file->cdfid, file->img_var_id );

        start_index = 0;

        if( file->image_range[0] < file->image_range[1] )
        {
            (void) mivarput1( file->minc_icv, file->min_id, &start_index,
                              NC_DOUBLE, MI_SIGNED, &file->image_range[0] );
            (void) mivarput1( file->minc_icv, file->max_id, &start_index,
                              NC_DOUBLE, MI_SIGNED, &file->image_range[1] );
        }
        ncopts = NC_VERBOSE | NC_FATAL;
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : output_minc_hyperslab
@INPUT      : file
              data_type
              n_array_dims
              array_sizes
              array_data_ptr
              to_array
              file_start
              file_count
@OUTPUT     : 
@RETURNS    : OK or ERROR
@DESCRIPTION: Outputs a hyperslab from an array to the file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Sep. 1, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  output_minc_hyperslab(
    Minc_file           file,
    Data_types          data_type,
    int                 n_array_dims,
    int                 array_sizes[],
    void                *array_data_ptr,
    int                 to_array[],
    int                 file_start[],
    int                 file_count[] )
{
    int              ind, expected_ind, file_ind, dim;
    int              n_file_dims, n_tmp_dims;
    long             long_file_start[MAX_DIMENSIONS];
    long             long_file_count[MAX_DIMENSIONS];
    void             *void_ptr;
    BOOLEAN          direct_from_array, non_full_size_found;
    int              tmp_ind, tmp_sizes[MAX_DIMENSIONS];
    int              array_indices[MAX_DIMENSIONS];
    int              array_counts[MAX_VAR_DIMS];
    int              zero[MAX_VAR_DIMS];
    Status           status;
    multidim_array   buffer_array;

    check_minc_output_variables( file );

    n_file_dims = file->n_file_dimensions;
    expected_ind = n_array_dims-1;
    tmp_ind = n_file_dims-1;
    non_full_size_found = FALSE;

    for_less( ind, 0, n_array_dims )
    {
        array_indices[ind] = -1;
        array_counts[ind] = 1;
    }

    direct_from_array = TRUE;

    /*--- determine if the hyperslab represents a consecutive chunk of
          memory in the array */

    for( file_ind = n_file_dims-1;  file_ind >= 0;  --file_ind )
    {
        long_file_start[file_ind] = (long) file_start[file_ind];
        long_file_count[file_ind] = (long) file_count[file_ind];
        ind = to_array[file_ind];
        if( ind != INVALID_AXIS )
        {
            array_counts[ind] = file_count[file_ind];

            if( !non_full_size_found &&
                file_count[file_ind] < file->sizes_in_file[file_ind] )
                non_full_size_found = TRUE;
            else if( non_full_size_found && file_count[file_ind] > 1 )
                direct_from_array = FALSE;

            if( file_count[file_ind] > 1 && ind != expected_ind )
                direct_from_array = FALSE;

            if( file_count[file_ind] != 1 ||
                file->sizes_in_file[file_ind] == 1 )
            {
                tmp_sizes[tmp_ind] = file_count[file_ind];
                array_indices[ind] = tmp_ind;
                --tmp_ind;
            }

            --expected_ind;
        }
    }

    if( direct_from_array )     /* hyperslab is consecutive chunk of memory */
    {
        void_ptr = array_data_ptr;
    }
    else
    {
        /*--- create a temporary array to copy hyperslab to, so that
              we have a consecutive chunk of memory to write from */

        n_tmp_dims = n_file_dims - tmp_ind - 1;

        for_less( dim, 0, n_tmp_dims )
        {
            tmp_sizes[dim] = tmp_sizes[dim+tmp_ind+1];
            zero[dim] = 0;
        }

        for_less( dim, 0, n_array_dims )
            array_indices[dim] -= tmp_ind + 1;

        create_multidim_array( &buffer_array, n_tmp_dims, tmp_sizes, data_type);

        GET_MULTIDIM_PTR( void_ptr, buffer_array, 0, 0, 0, 0, 0 );

        /*--- copy from the array argument to the temporary array */

        copy_multidim_data_reordered( get_type_size(data_type),
                                      void_ptr, n_tmp_dims, tmp_sizes,
                                      array_data_ptr, n_array_dims, array_sizes,
                                      array_counts, array_indices );

        GET_MULTIDIM_PTR( void_ptr, buffer_array, 0, 0, 0, 0, 0 );
    }

    /*--- output the data to the file */

    if( miicv_put( file->minc_icv, long_file_start, long_file_count,
                   void_ptr ) == MI_ERROR )
        status = ERROR;
    else
        status = OK;

    if( !direct_from_array )
        delete_multidim_array( &buffer_array );

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : output_slab
@INPUT      : file
              volume
              to_volume
              file_start
              file_count
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Outputs the slab specified by the file start and count arrays,
              from the volume.  The to_volume array translates axes in the file
              to axes in the volume.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : Sep  1, 1995    D. MacDonald - added cached volumes.
---------------------------------------------------------------------------- */

private  void  output_slab(
    Minc_file   file,
    Volume      volume,
    int         to_volume[],
    long        file_start[],
    long        file_count[] )
{
    int               dim, file_ind, ind, n_slab_dims;
    int               to_array[MAX_DIMENSIONS];
    int               volume_start[MAX_DIMENSIONS];
    int               volume_sizes[MAX_DIMENSIONS];
    int               array_start[MAX_DIMENSIONS];
    int               int_file_count[MAX_DIMENSIONS];
    int               int_file_start[MAX_DIMENSIONS];
    int               volume_to_array[MAX_DIMENSIONS];
    int               slab_sizes[MAX_DIMENSIONS];
    int               v[MAX_DIMENSIONS];
    int               size0, size1, size2, size3, size4;
    Real              value;
    void              *array_data_ptr;
    multidim_array    array;

    for_less( file_ind, 0, file->n_file_dimensions )
    {
        int_file_start[file_ind] = (int) file_start[file_ind];
        int_file_count[file_ind] = (int) file_count[file_ind];

        ind = to_volume[file_ind];
        if( ind != INVALID_AXIS )
            volume_start[ind] = int_file_start[file_ind];
        else
            volume_start[ind] = 0;
    }

    if( volume->is_cached_volume )
    {
        /*--- must make a temporary hyperslab array to contain the volume */

        for_less( dim, 0, get_volume_n_dimensions(volume) )
            volume_sizes[dim] = 1;

        for_less( dim, get_volume_n_dimensions(volume), MAX_DIMENSIONS )
        {
            volume_start[dim] = 0;
            volume_sizes[dim] = 1;
            volume_to_array[dim] = 0;
        }

        n_slab_dims = 0;
        for_less( file_ind, 0, file->n_file_dimensions )
        {
            ind = to_volume[file_ind];
            if( ind != INVALID_AXIS )
            {
                to_array[file_ind] = n_slab_dims;
                volume_to_array[ind] = n_slab_dims;
                array_start[n_slab_dims] = 0;
                slab_sizes[n_slab_dims] = int_file_count[file_ind];
                volume_sizes[ind] = int_file_count[file_ind];
                ++n_slab_dims;
            }
            else
            {
                to_array[file_ind] = INVALID_AXIS;
            }
        }

        create_multidim_array( &array, n_slab_dims, slab_sizes,
                               get_volume_data_type(volume) );

        /*--- copy from the cached volume to the temporary array */

        size0 = volume_sizes[0];
        size1 = volume_sizes[1];
        size2 = volume_sizes[2];
        size3 = volume_sizes[3];
        size4 = volume_sizes[4];

        for_less( v[4], 0, size4 )
        for_less( v[3], 0, size3 )
        for_less( v[2], 0, size2 )
        for_less( v[1], 0, size1 )
        for_less( v[0], 0, size0 )
        {
            value = get_volume_voxel_value( volume,
                                            volume_start[0] + v[0],
                                            volume_start[1] + v[1],
                                            volume_start[2] + v[2],
                                            volume_start[3] + v[3],
                                            volume_start[4] + v[4] );

            SET_MULTIDIM( array, v[volume_to_array[0]],
                                 v[volume_to_array[1]],
                                 v[volume_to_array[2]],
                                 v[volume_to_array[3]],
                                 v[volume_to_array[4]], value );
        }

        /*--- output the temporary array */

        GET_MULTIDIM_PTR( array_data_ptr, array, 0, 0, 0, 0, 0 );
        (void) output_minc_hyperslab( file, get_volume_data_type(volume),
                                      n_slab_dims, slab_sizes, array_data_ptr,
                                      to_array, int_file_start, int_file_count);
        delete_multidim_array( &array );
    }
    else
    {
        GET_MULTIDIM_PTR( array_data_ptr, volume->array,
                          volume_start[0], volume_start[1], volume_start[2],
                          volume_start[3], volume_start[4] );
        get_volume_sizes( volume, volume_sizes );

        (void) output_minc_hyperslab( file, get_volume_data_type(volume),
                                      get_volume_n_dimensions(volume),
                                      volume_sizes, array_data_ptr,
                                      to_volume,
                                      int_file_start, int_file_count );
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : output_the_volume
@INPUT      : file
              volume
              volume_count
              file_start
@OUTPUT     : 
@RETURNS    : OK or ERROR
@DESCRIPTION: Outputs the volume to the file in the given position.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  Status  output_the_volume(
    Minc_file   file,
    Volume      volume,
    int         volume_count[],
    long        file_start[] )
{
    Status            status;
    int               i, d, n_volume_dims, sizes[MAX_DIMENSIONS];
    int               slab_size, n_slab, this_count;
    int               vol_index, step, n_steps, n_range_dims;
    int               to_volume_index[MAX_VAR_DIMS];
    int               to_file_index[MAX_DIMENSIONS];
    long              file_indices[MAX_VAR_DIMS];
    long              count[MAX_VAR_DIMS];
    Real              real_min, real_max;
    STRING            *vol_dimension_names;
    BOOLEAN           increment;
    progress_struct   progress;

    check_minc_output_variables( file );

    /* --- check if dimension name correspondence between volume and file */

    n_volume_dims = get_volume_n_dimensions( volume );

    if( n_volume_dims > file->n_file_dimensions )
    {
        print_error( "output_volume_to_minc_file_position:" );
        print_error( " volume (%d) has more dimensions than file (%d).\n",
                     n_volume_dims, file->n_file_dimensions );
        return( ERROR );
    }

    /*--- find correspondence between volume dimensions and file dimensions */

    vol_dimension_names = get_volume_dimension_names( volume );

    status = get_dimension_ordering( n_volume_dims, vol_dimension_names,
                                     file->n_file_dimensions, file->dim_names,
                                     to_volume_index, to_file_index );

    delete_dimension_names( volume, vol_dimension_names );

    if( status != OK )
        return( ERROR );

    /*--- check sizes match between volume and file */

    get_volume_sizes( volume, sizes );

    for_less( d, 0, file->n_file_dimensions )
    {
        vol_index = to_volume_index[d];

        if( vol_index >= 0 )
        {
            if( volume_count[vol_index] < 0 ||
                volume_count[vol_index] > sizes[vol_index] )
            {
                print_error( "output_the_volume: invalid volume count.\n" );
                print_error( "    count[%d] = %d\n",
                       vol_index, volume_count[vol_index] );
                return( ERROR );
            }

            this_count = volume_count[vol_index];
        }
        else
        {
            this_count = 1;
        }

        if( file_start[d] < 0 || file_start[d] + this_count >
            file->sizes_in_file[d] )
        {
            print_error( "output_the_volume:  invalid minc file position.\n" );
            print_error( "    start[%d] = %d     count[%d] = %d\n", d, file_start[d],
                      d, this_count );
            return( ERROR );
        }
    }

    /*--- if per slice image ranges, output the ranges corresponding to this
          volume */

    if( file->image_range[0] >= file->image_range[1] )
    {
        long     n_ranges, range_start[MAX_VAR_DIMS], range_count[MAX_VAR_DIMS];
        double   *image_range;

        n_range_dims = file->n_file_dimensions - 2;
        if( strcmp( file->dim_names[file->n_file_dimensions-1],
                    MIvector_dimension ) == 0 )
            --n_range_dims;

        n_ranges = 1;
        for_less( d, 0, n_range_dims )
        {
            vol_index = to_volume_index[d];
            if( vol_index == INVALID_AXIS )
            {
                range_count[d] = 1;
                range_start[d] = file_start[d];
            }
            else
            {
                n_ranges *= volume_count[vol_index];
                range_count[d] = volume_count[vol_index];
                range_start[d] = 0;
            }
        }

        get_volume_real_range( volume, &real_min, &real_max );

        ALLOC( image_range, n_ranges );

        for_less( i, 0, n_ranges )
            image_range[i] = real_min;

        (void) mivarput( file->minc_icv, file->min_id,
                         range_start, range_count,
                         NC_DOUBLE, MI_UNSIGNED, (void *) image_range );

        for_less( i, 0, n_ranges )
            image_range[i] = real_max;

        (void) mivarput( file->minc_icv, file->max_id,
                         range_start, range_count,
                         NC_DOUBLE, MI_UNSIGNED, (void *) image_range );

        FREE( image_range );
    }

    /*--- determine which contiguous blocks of volume to output */

    file->n_slab_dims = 0;
    slab_size = 1;
    d = file->n_file_dimensions-1;

    do
    {
        if( to_volume_index[d] != INVALID_AXIS )
        {
            ++file->n_slab_dims;
            slab_size *= volume_count[to_volume_index[d]];
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
        vol_index = to_volume_index[d];
        if( vol_index != INVALID_AXIS && n_slab >= file->n_slab_dims )
            n_steps *= volume_count[vol_index];
        if( vol_index != INVALID_AXIS )
            ++n_slab;
        file_indices[d] = file_start[d];
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
            vol_index = to_volume_index[d];

            if( vol_index == INVALID_AXIS || n_slab >= file->n_slab_dims )
                count[d] = 1;
            else
                count[d] = volume_count[vol_index];

            if( vol_index != INVALID_AXIS )
                ++n_slab;
        }

        output_slab( file, volume, to_volume_index, file_indices, count );

        increment = TRUE;

        /*--- increment the file index dimensions which correspond
              to volume dimensions not output */

        d = file->n_file_dimensions-1;
        n_slab = 0;
        while( increment && d >= 0 )
        {
            vol_index = to_volume_index[d];

            if( vol_index != INVALID_AXIS && n_slab >= file->n_slab_dims )
            {
                ++file_indices[d];
                if( file_indices[d] < file_start[d] + volume_count[vol_index] )
                    increment = FALSE;
                else
                    file_indices[d] = file_start[d];
            }

            if( vol_index != INVALID_AXIS )
                ++n_slab;

            --d;
        }

        ++step;

        if( n_steps > 1 )
            update_progress_report( &progress, step );
    }

    terminate_progress_report( &progress );

    return( OK );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : output_volume_to_minc_file_position
@INPUT      : file
              volume
              volume_count
              file_start
@OUTPUT     : Outputs the volume to the specified file position.
@RETURNS    : 
@DESCRIPTION: 
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  output_volume_to_minc_file_position(
    Minc_file   file,
    Volume      volume,
    int         volume_count[],
    long        file_start[] )
{
    if( file->ignoring_because_cached )
        return( OK );

    file->outputting_in_order = FALSE;

    return( output_the_volume( file, volume, volume_count, file_start ) );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : output_minc_volume
@INPUT      : file
@OUTPUT     : 
@RETURNS    : OK or ERROR
@DESCRIPTION: Outputs the attached volume to the MINC file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  output_minc_volume(
    Minc_file   file )
{
    int        d, volume_count[MAX_DIMENSIONS];
    BOOLEAN    increment;

    if( file->ignoring_because_cached )
        return( OK );

    /*--- check number of volumes written */

    d = 0;
    while( d < file->n_file_dimensions &&
           file->to_volume_index[d] != INVALID_AXIS )
        ++d;

    if( d < file->n_file_dimensions &&
        file->indices[d] >= file->sizes_in_file[d] )
    {
        print_error(
             "output_minc_volume: attempted to write too many subvolumes.\n");
        return( ERROR );
    }

    get_volume_sizes( file->volume, volume_count );

    if( output_the_volume( file, file->volume, volume_count,
                           file->indices ) != OK )
        return( ERROR );

    /*--- increment the file index dimensions which do not
          correspond to volume dimensions */

    increment = TRUE;

    d = file->n_file_dimensions-1;
    while( increment && d >= 0 )
    {
        if( file->to_volume_index[d] == INVALID_AXIS )
        {
            ++file->indices[d];

            if( file->indices[d] < file->sizes_in_file[d] )
                increment = FALSE;
            else
                file->indices[d] = 0;
        }

        --d;
    }

    if( increment )
        file->entire_file_written = TRUE;

    return( OK );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : close_minc_output
@INPUT      : file
@OUTPUT     : 
@RETURNS    : OK or ERROR
@DESCRIPTION: Closes the MINC file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  close_minc_output(
    Minc_file   file )
{
    int    d;

    if( file == (Minc_file) NULL )
    {
        print_error( "close_minc_output(): NULL file.\n" );
        return( ERROR );
    }

    if( !file->ignoring_because_cached )
    {
        if( file->outputting_in_order && !file->entire_file_written )
        {
            print_error( "Warning:  the MINC file has been " );
            print_error( "closed without writing part of it.\n");
        }

        (void) miattputstr( file->cdfid, file->img_var_id, MIcomplete, MI_TRUE);

        (void) miclose( file->cdfid );
        (void) miicv_free( file->minc_icv );

        for_less( d, 0, file->n_file_dimensions )
            delete_string( file->dim_names[d] );
    }

    delete_string( file->filename );

    FREE( file );

    return( OK );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_default_minc_output_options
@INPUT      : 
@OUTPUT     : options
@RETURNS    : 
@DESCRIPTION: Sets the minc output options to the default.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  set_default_minc_output_options(
    minc_output_options  *options           )
{
    int   i;

    for_less( i, 0, MAX_DIMENSIONS )
        options->dimension_names[i] = NULL;
    options->global_image_range[0] = 0.0;
    options->global_image_range[1] = -1.0;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : delete_minc_output_options
@INPUT      : options
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Deletes the minc output options.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  delete_default_minc_output_options(
    minc_output_options  *options           )
{
    int   i;

    for_less( i, 0, MAX_DIMENSIONS )
        delete_string( options->dimension_names[i] );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_minc_output_dimensions_order
@INPUT      : n_dimensions
              dimension_names
@OUTPUT     : options
@RETURNS    : 
@DESCRIPTION: Sets the dimension ordering of the minc output options.
              This option is used by output_volume, but not by
              initialize_minc_output.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  set_minc_output_dimensions_order(
    minc_output_options  *options,
    int                  n_dimensions,
    STRING               dimension_names[] )
{
    int   i;

    for_less( i, 0, n_dimensions )
    {
        replace_string( &options->dimension_names[i],
                        create_string(dimension_names[i]) );
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_minc_output_real_range
@INPUT      : real_min
              real_max
@OUTPUT     : options
@RETURNS    : 
@DESCRIPTION: Sets the global real range of the entire file, unless real_min
              >= real_max.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  set_minc_output_real_range(
    minc_output_options  *options,
    Real                 real_min,
    Real                 real_max )
{
    options->global_image_range[0] = real_min;
    options->global_image_range[1] = real_max;
}
