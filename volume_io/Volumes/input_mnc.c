#include  <minc.h>
#include  <def_mni.h>

#define  INVALID_AXIS   -1

#define  MIN_SLAB_SIZE   10000      /* at least 10000 entries per read */
#define  MAX_SLAB_SIZE   200000     /* no more than 200 K at a time */

private  void  create_world_transform(
    Point       *origin,
    Vector      axes[N_DIMENSIONS],
    Real        axis_spacing[N_DIMENSIONS],
    Transform   *transform );
private  int  match_dimension_names(
    int               n_volume_dims,
    char              *volume_dimension_names[],
    int               n_file_dims,
    char              *file_dimension_names[],
    int               axis_index_in_file[] );

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
    minc_file_struct    *file;
    int                 img_var, dim_vars[MAX_VAR_DIMS];
    int                 slab_size, length;
    Boolean             min_voxel_found, max_voxel_found, range_specified;
    double              valid_range[2], temp;
    long                long_size, mindex[MAX_VAR_DIMS];
    Boolean             converted_sign;
    nc_type             converted_type;
    String              signed_flag;
    char                *dim_names[MAX_VAR_DIMS];
    nc_type             file_datatype;
    int                 sizes[MAX_VAR_DIMS];
    double              file_separations[MAX_VAR_DIMS];
    Real                axis_separations[MI_NUM_SPACE_DIMS];
    Real                volume_separations[MI_NUM_SPACE_DIMS];
    double              start_position[MAX_VAR_DIMS];
    double              dir_cosines[MAX_VAR_DIMS][MI_NUM_SPACE_DIMS];
    Vector              axes[MI_NUM_SPACE_DIMS];
    Boolean             spatial_dim_flags[MAX_VAR_DIMS];
    Vector              offset;
    Point               origin;
    double              real_min, real_max;
    int                 d, dimvar, which_valid_axis;
    int                 spatial_axis_indices[MAX_VAR_DIMS];
    Transform           voxel_to_world_transform;
    General_transform   general_transform;
    minc_input_options  default_options;

    if( options == (minc_input_options *) NULL )
    {
        get_default_minc_input_options( &default_options );
        options = &default_options;
    }

    ALLOC( file, 1 );

    file->file_is_being_read = TRUE;
    file->volume = volume;

    /* --- open the file */

    ncopts = 0;
    file->cdfid =  ncopen( filename, NC_NOWRITE );

    if( file->cdfid == MI_ERROR )
    {
        print( "Error: opening MINC file \"%s\".\n", filename );
        return( (Minc_file) 0 );
    }

    /* --- find the image variable */

    img_var = ncvarid( file->cdfid, MIimage );

    ncvarinq( file->cdfid, img_var, (char *) NULL, &file_datatype,
              &file->n_file_dimensions, dim_vars, (int *) NULL );

    if( file->n_file_dimensions < volume->n_dimensions )
    {
        print( "Error: MINC file has only %d dims, volume requires %d.\n",
               file->n_file_dimensions, volume->n_dimensions );
        (void) ncclose( file->cdfid );
        return( (Minc_file) 0 );
    }
    else if( file->n_file_dimensions > MAX_VAR_DIMS )
    {
        print( "Error: MINC file has %d dims, can only handle %d.\n",
               file->n_file_dimensions, MAX_VAR_DIMS );
        (void) ncclose( file->cdfid );
        return( (Minc_file) NULL );
    }

    for_less( d, 0, file->n_file_dimensions )
    {
        ALLOC( dim_names[d], MAX_STRING_LENGTH + 1 );

        (void) ncdiminq( file->cdfid, dim_vars[d], dim_names[d], &long_size );
        file->sizes_in_file[d] = long_size;
    }

    /* --- match the dimension names of the volume with those in the file */

    if( !match_dimension_names( volume->n_dimensions, volume->dimension_names,
                                file->n_file_dimensions, dim_names,
                                file->axis_index_in_file ) )
    {
        print( "Error:  dimension names did not match: \n" );
        
        print( "\n" );
        print( "Requested:\n" );
        for_less( d, 0, volume->n_dimensions )
            print( "%d: %s\n", d+1, volume->dimension_names[d] );

        print( "\n" );
        print( "In File:\n" );
        for_less( d, 0, file->n_file_dimensions )
            print( "%d: %s\n", d+1, dim_names[d] );

        (void) ncclose( file->cdfid );
        return( (Minc_file) NULL );
    }

    file->n_volumes_in_file = 1;

    /* --- find the spatial axes (x,y,z) */

    which_valid_axis = 0;

    for_less( d, 0, file->n_file_dimensions )
    {
        if( strcmp( dim_names[d], MIxspace ) == 0 )
            spatial_axis_indices[d] = X;
        else if( strcmp( dim_names[d], MIyspace ) == 0 )
            spatial_axis_indices[d] = Y;
        else if( strcmp( dim_names[d], MIzspace ) == 0 )
            spatial_axis_indices[d] = Z;
        else
            spatial_axis_indices[d] = INVALID_AXIS;

        spatial_dim_flags[d] = (spatial_axis_indices[d] != INVALID_AXIS);

        if( file->axis_index_in_file[d] != INVALID_AXIS )
        {
            file->valid_file_axes[which_valid_axis] = d;
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

        dimvar = ncvarid( file->cdfid, dim_names[d] );
        if( dimvar != MI_ERROR )
        {
            (void) miattget1( file->cdfid, dimvar, MIstep, NC_DOUBLE,
                              (void *) (&file_separations[d]) );

            if( spatial_dim_flags[d] )
            {
                 (void) miattget1( file->cdfid, dimvar, MIstart, NC_DOUBLE,
                                   (void *) (&start_position[d]) );
                 (void) miattget( file->cdfid, dimvar, MIdirection_cosines,
                                  NC_DOUBLE, MI_NUM_SPACE_DIMS,
                                  (void *) (dir_cosines[d]), (int *) NULL );
            }
        }

        if( file->axis_index_in_file[d] == INVALID_AXIS )
        {
            file->n_volumes_in_file *= file->sizes_in_file[d];
        }
        else
        {
            sizes[file->axis_index_in_file[d]] = file->sizes_in_file[d];
            volume_separations[file->axis_index_in_file[d]] =
                                          file_separations[d];
        }
    }

    set_volume_separations( volume, volume_separations );

    /* --- create the world transform from slice separation, cosines, etc. */

    fill_Vector( axes[X], 1.0, 0.0, 0.0 );
    fill_Vector( axes[Y], 0.0, 1.0, 0.0 );
    fill_Vector( axes[Z], 0.0, 0.0, 1.0 );

    axis_separations[X] = 1.0;
    axis_separations[Y] = 1.0;
    axis_separations[Z] = 1.0;

    fill_Point( origin, 0.0, 0.0, 0.0 );

    for_less( d, 0, file->n_file_dimensions )
    {
        if( spatial_axis_indices[d] != INVALID_AXIS )
        {
            fill_Vector( axes[spatial_axis_indices[d]],
                         dir_cosines[d][0],
                         dir_cosines[d][1],
                         dir_cosines[d][2] );
            NORMALIZE_VECTOR( axes[spatial_axis_indices[d]],
                              axes[spatial_axis_indices[d]] );
            
            SCALE_VECTOR( offset, axes[spatial_axis_indices[d]],
                          start_position[d] );
            ADD_POINT_VECTOR( origin, origin, offset );

            axis_separations[spatial_axis_indices[d]] = file_separations[d];
        }
    }

    create_world_transform( &origin, axes, axis_separations,
                            &voxel_to_world_transform );
    create_linear_transform( &general_transform, &voxel_to_world_transform );
    set_voxel_to_world_transform( volume, &general_transform );

    /* --- decide on type conversion */

    if( volume->data_type == NO_DATA_TYPE )     /* --- use type of file */
    {
        if( miattgetstr( file->cdfid, img_var, MIsigntype, MAX_STRING_LENGTH,
                         signed_flag ) != (char *) NULL )
        {
            converted_sign = (strcmp( signed_flag, MI_SIGNED ) == 0);
        }
        else
            converted_sign = file_datatype != NC_BYTE;

        converted_type = file_datatype;
    }
    else                                        /* --- use specified type */
    {
        converted_type = volume->nc_data_type;
        converted_sign = volume->signed_flag;
    }

    for_less( d, 0, file->n_file_dimensions )
        mindex[d] = 0;

    /* --- create the image conversion variable */

    file->icv = miicv_create();

    (void) miicv_setint( file->icv, MI_ICV_TYPE, converted_type );
    (void) miicv_setstr( file->icv, MI_ICV_SIGN,
                         converted_sign ? MI_SIGNED : MI_UNSIGNED );
    (void) miicv_setint( file->icv, MI_ICV_DO_NORM, TRUE );
    (void) miicv_setint( file->icv, MI_ICV_DO_FILLVALUE, TRUE );

    range_specified = (volume->min_voxel < volume->max_voxel);

    max_voxel_found = FALSE;
    min_voxel_found = FALSE;

    valid_range[0] = 0.0;
    valid_range[1] = 0.0;

    if( volume->data_type == NO_DATA_TYPE )
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

    if( volume->data_type == NO_DATA_TYPE || !range_specified )
    {
        if( !min_voxel_found )
        {
            if( miicv_inqdbl( file->icv, MI_ICV_VALID_MIN, &valid_range[0] )
                               != MI_ERROR )
            {
                min_voxel_found = TRUE;
            }
            else
            {
                HANDLE_INTERNAL_ERROR( "initialize minc input" );
            }
        }

        if( !max_voxel_found )
        {
            if( miicv_inqdbl( file->icv, MI_ICV_VALID_MAX, &valid_range[1] )
                               != MI_ERROR )
            {
                max_voxel_found = TRUE;
            }
            else
            {
                HANDLE_INTERNAL_ERROR( "initialize minc input" );
            }
        }
    }

    if( min_voxel_found && max_voxel_found )
        set_volume_voxel_range( volume, valid_range[0], valid_range[1] );
    else if( min_voxel_found && !max_voxel_found )
        set_volume_voxel_range( volume, valid_range[0], valid_range[0] + 1.0 );
    else if( !min_voxel_found && max_voxel_found )
        set_volume_voxel_range( volume, valid_range[1] - 1.0, valid_range[0] );

    get_volume_voxel_range( volume, &valid_range[0], &valid_range[1] );

    (void) miicv_setdbl( file->icv, MI_ICV_VALID_MIN, valid_range[0] );
    (void) miicv_setdbl( file->icv, MI_ICV_VALID_MAX, valid_range[1] );

    (void) miicv_attach( file->icv, file->cdfid, img_var );

    /* --- compute the mapping to real values */

    (void) miicv_inqdbl( file->icv, MI_ICV_NORM_MIN, &real_min );
    (void) miicv_inqdbl( file->icv, MI_ICV_NORM_MAX, &real_max );

    if( volume->data_type == FLOAT || volume->data_type == DOUBLE )
        set_volume_voxel_range( volume, real_min, real_max );
    else
        set_volume_real_range( volume, real_min, real_max );

    if( options->promote_invalid_to_min_flag )
    {
        (void) miicv_detach( file->icv );
        (void) miicv_setdbl( file->icv, MI_ICV_FILLVALUE, volume->min_voxel );
        (void) miicv_attach( file->icv, file->cdfid, img_var );
    }

    for_less( d, 0, file->n_file_dimensions )
        file->indices[d] = 0;

    file->end_volume_flag = FALSE;

    ncopts = NC_VERBOSE | NC_FATAL;

    /* --- decide how many dimensions to read in at a time */

    file->n_slab_dims = 0;
    slab_size = 1;
    
    do
    {
        ++file->n_slab_dims;
        d = file->valid_file_axes[volume->n_dimensions-file->n_slab_dims];
        slab_size *= file->sizes_in_file[d];
    }
    while( file->n_slab_dims < volume->n_dimensions &&
           slab_size < MIN_SLAB_SIZE );

    if( slab_size > MAX_SLAB_SIZE && file->n_slab_dims > 1 )
    {
        --file->n_slab_dims;
    }

    set_volume_size( volume, converted_type, converted_sign, sizes );

    for_less( d, 0, file->n_file_dimensions )
        FREE( dim_names[d] );

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
    if( file == (Minc_file) NULL )
    {
        print( "close_minc_input(): NULL file.\n" );
        return( ERROR );
    }

    (void) ncclose( file->cdfid );
    (void) miicv_free( file->icv );

    FREE( file );

    return( OK );
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
    long        start[],
    long        count[] )
{
    int      i, ind, valid_ind, file_ind, expected_ind;
    int      iv[MAX_DIMENSIONS], n_to_read;
    void     *void_ptr;
    Boolean  direct_to_volume;
    Boolean  non_full_size_found;
    char     *buffer;

    non_full_size_found = FALSE;
    direct_to_volume = TRUE;

    expected_ind = volume->n_dimensions-1;

    for( file_ind = file->n_file_dimensions-1;  file_ind >= 0;  --file_ind )
    {
        ind = file->axis_index_in_file[file_ind];
        if( ind != INVALID_AXIS && (count[file_ind] != 1 ||
                                    file->sizes_in_file[file_ind] == 1) )
        {
            if( non_full_size_found )
            {
                direct_to_volume = FALSE;
                break;
            }

            if( ind != expected_ind )
            {
                direct_to_volume = FALSE;
                break;
            }
            --expected_ind;

            if( count[file_ind] != file->sizes_in_file[file_ind] )
                non_full_size_found = TRUE;
        }
    }

    for_less( i, 0, volume->n_dimensions )
    {
        file_ind = file->valid_file_axes[i];
        iv[file->axis_index_in_file[file_ind]] = start[file_ind];
    }

    if( direct_to_volume )        /* file is same order as volume */
    {
        GET_VOXEL_PTR( void_ptr, volume, iv[0], iv[1], iv[2], iv[3], iv[4] );
        (void) miicv_get( file->icv, start, count, void_ptr );
    }
    else
    {
        n_to_read = 1;

        for_less( i, 0, file->n_file_dimensions )
            n_to_read *= count[i];

        ALLOC( buffer, sizeof(double) * n_to_read );

        void_ptr = (void *) buffer;

        (void) miicv_get( file->icv, start, count, void_ptr );

        for_less( i, 0, n_to_read )
        {
            switch( volume->data_type )
            {
            case UNSIGNED_BYTE:
                SET_VOXEL( volume, iv[0], iv[1], iv[2], iv[3], iv[4],
                           ( (unsigned char *) void_ptr )[i] );
                break;
            case SIGNED_BYTE:
                SET_VOXEL( volume, iv[0], iv[1], iv[2], iv[3], iv[4],
                           ( (signed char *) void_ptr )[i] );
                break;
            case UNSIGNED_SHORT:
                SET_VOXEL( volume, iv[0], iv[1], iv[2], iv[3], iv[4],
                           ( (unsigned short *) void_ptr )[i] );
                break;
            case SIGNED_SHORT:
                SET_VOXEL( volume, iv[0], iv[1], iv[2], iv[3], iv[4],
                           ( (signed short *) void_ptr )[i] );
                break;
            case UNSIGNED_LONG:
                SET_VOXEL( volume, iv[0], iv[1], iv[2], iv[3], iv[4],
                           ( (unsigned long *) void_ptr )[i] );
                break;
            case SIGNED_LONG:
                SET_VOXEL( volume, iv[0], iv[1], iv[2], iv[3], iv[4],
                           ( (signed long *) void_ptr )[i] );
                break;
            case FLOAT:
                SET_VOXEL( volume, iv[0], iv[1], iv[2], iv[3], iv[4],
                           ( (float *) void_ptr )[i] );
                break;
            case DOUBLE:
                SET_VOXEL( volume, iv[0], iv[1], iv[2], iv[3], iv[4],
                           ( (double *) void_ptr )[i] );
                break;
            }

            valid_ind = volume->n_dimensions-1;

            while( valid_ind >= 0 )
            {
                file_ind = file->valid_file_axes[valid_ind];
                if( count[file_ind] > 1 )
                {
                    ++iv[file->axis_index_in_file[file_ind]];
                    if( iv[file->axis_index_in_file[file_ind]] <
                        start[file_ind] + count[file_ind])
                        break;
                    iv[file->axis_index_in_file[file_ind]] = start[file_ind];
                }

                --valid_ind;
            }
        }

        FREE( buffer );
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

public  Boolean  input_more_minc_file(
    Minc_file   file,
    Real        *fraction_done )
{
    int      ind, file_ind, n_done, total;
    long     count[MAX_VAR_DIMS];
    Volume   volume;

    if( file->end_volume_flag )
        return( FALSE );

    volume = file->volume;

    if( volume->data == (void *) NULL )
        alloc_volume_data( volume );

    /* --- set the counts for reading, actually these will be the same
           every time */

    for_less( ind, 0, file->n_file_dimensions )
    {
        count[ind] = 1;
    }

    for_less( ind, volume->n_dimensions - file->n_slab_dims,
              volume->n_dimensions )
    {
        file_ind = file->valid_file_axes[ind];
        count[file_ind] = file->sizes_in_file[file_ind];
    }

    input_slab( file, volume, file->indices, count );

    /* --- advance to next slab */

    ind = volume->n_dimensions-file->n_slab_dims-1;

    while( ind >= 0 )
    {
        file_ind = file->valid_file_axes[ind];

        ++file->indices[file_ind];
        if( file->indices[file_ind] < file->sizes_in_file[file_ind] )
            break;

        file->indices[file_ind] = 0;
        --ind;
    }

    /* --- check if done */

    if( ind < 0 )
    {
        *fraction_done = 1.0;
        file->end_volume_flag = TRUE;
    }
    else
    {
        n_done = 0;             /* --- compute fraction done */
        total = 1;
        for_less( ind, 0, volume->n_dimensions - file->n_slab_dims )
        {
            n_done = n_done * file->sizes_in_file[file->valid_file_axes[ind]] +
                     file->indices[ind];
            total *= file->sizes_in_file[file->valid_file_axes[ind]];
        }

        *fraction_done = (Real) n_done / (Real) total;
    }

    return( !file->end_volume_flag );
}

public  Boolean  advance_volume(
    Minc_file   file )
{
    int   ind;

    ind = file->n_file_dimensions-1;

    while( ind >= 0 )
    {
        if( file->axis_index_in_file[ind] == INVALID_AXIS )
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
        {
            file->indices[file->valid_file_axes[ind]] = 0;
        }

    }
    else
        file->end_volume_flag = TRUE;

    return( file->end_volume_flag );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_world_transform
@INPUT      : origin        - point origin
              axes          - 3 vectors
              axis_spacing  - voxel spacing
@OUTPUT     : transform
@RETURNS    : 
@DESCRIPTION: Using the information from the mnc file, creates a 4 by 4
              transform which converts a voxel to world space.
              Voxel centres are at integer numbers in voxel space.  So the
              bottom left voxel is (0.0,0.0,0.0).
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  void  create_world_transform(
    Point       *origin,
    Vector      axes[N_DIMENSIONS],
    Real        axis_spacing[N_DIMENSIONS],
    Transform   *transform )
{
    Vector   x_axis, y_axis, z_axis;

    x_axis = axes[X];
    y_axis = axes[Y];
    z_axis = axes[Z];

    SCALE_VECTOR( x_axis, x_axis, axis_spacing[X] );
    SCALE_VECTOR( y_axis, y_axis, axis_spacing[Y] );
    SCALE_VECTOR( z_axis, z_axis, axis_spacing[Z] );

    make_change_to_bases_transform( origin, &x_axis, &y_axis, &z_axis,
                                    transform );
}

public  Boolean  is_spatial_dimension(
    char   dimension_name[],
    int    *axis )
{
    if( strcmp(dimension_name,MIxspace) == 0 )
    {
        *axis = X;
        return( TRUE );
    }
    else if( strcmp(dimension_name,MIyspace) == 0 )
    {
        *axis = Y;
        return( TRUE );
    }
    else if( strcmp(dimension_name,MIzspace) == 0 )
    {
        *axis = Z;
        return( TRUE );
    }
    else
        return( FALSE );
}

private  int  match_dimension_names(
    int               n_volume_dims,
    char              *volume_dimension_names[],
    int               n_file_dims,
    char              *file_dimension_names[],
    int               axis_index_in_file[] )
{
    int      i, j, iteration, n_matches, dummy;
    Boolean  match;
    Boolean  volume_dim_found[MAX_DIMENSIONS];

    n_matches = 0;

    for_less( i, 0, n_file_dims )
        axis_index_in_file[i] = INVALID_AXIS;

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
                    if( axis_index_in_file[j] == INVALID_AXIS )
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
                                is_spatial_dimension( file_dimension_names[j],
                                                      &dummy );
                            break;
                        case 2:
                            match = (strlen(volume_dimension_names[i]) == 0);
                            break;
                        }

                        if( match )
                        {
                            axis_index_in_file[j] = i;
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

public  int  get_minc_file_id(
    Minc_file  file )
{
    return( file->cdfid );
}

public  void  get_default_minc_input_options(
    minc_input_options  *options )
{
    set_minc_input_promote_invalid_to_min_flag( options, TRUE );
}

public  void  set_minc_input_promote_invalid_to_min_flag(
    minc_input_options  *options,
    Boolean             flag )
{
    options->promote_invalid_to_min_flag = flag;
}
