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
    nc_type    data_type,
    Boolean    signed_flag,
    Real       min_value,
    Real       max_value,
    Transform  *voxel_to_world_transform )
{
    minc_file_struct    *file;
    int                 img_var, dim_vars[MAX_VAR_DIMS], min_id, max_id;
    int                 dim_ids[MAX_VAR_DIMS];
    long                start_index, mindex[MAX_VAR_DIMS];
    double              separation[MAX_VAR_DIMS];
    double              start[MAX_VAR_DIMS];
    double              dir_cosines[MAX_VAR_DIMS][MI_NUM_SPACE_DIMS];
    double              image_range[2], dim_value;
    int                 i, j, d, axis;
    Point               origin;
    Vector              axes[N_DIMENSIONS];

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
        dim_vars[d] = ncdimdef( file->cdfid, dim_names[d],
                                sizes[d] );

        if( is_spatial_dimension( dim_names[d], &axis ) )
        {
            dim_ids[d] = micreate_std_variable( file->cdfid, dim_names[d],
                                                NC_DOUBLE, 0, NULL);
            (void) miattputdbl( file->cdfid, dim_ids[d], MIstep,
                                separation[axis]);
            (void) miattputdbl( file->cdfid, dim_ids[d], MIstart, start[axis]);
            if( !is_default_direction_cosine( axis, dir_cosines[axis] ) )
            {
                (void) ncattput( file->cdfid, dim_ids[d], MIdirection_cosines,
                                 NC_DOUBLE, N_DIMENSIONS, dir_cosines[axis]);
            }
            (void) miattputstr( file->cdfid, dim_ids[d], MIunits, UNITS );
        }
    }

    img_var = micreate_std_variable( file->cdfid, MIimage, data_type,
                                     n_dimensions, dim_vars );

    image_range[0] = min_value;
    image_range[1] = max_value;

    min_id = micreate_std_variable( file->cdfid, MIimagemin, NC_DOUBLE, 0,
                                   (int *) NULL );
    (void) miattput_pointer( file->cdfid, img_var, MIimagemin, min_id );

    max_id = micreate_std_variable( file->cdfid, MIimagemax, NC_DOUBLE, 0,
                                   (int *) NULL );
    (void) miattput_pointer( file->cdfid, img_var, MIimagemax, max_id );

    if( signed_flag )
        (void) miattputstr( file->cdfid, img_var, MIsigntype, MI_SIGNED);
    else
        (void) miattputstr( file->cdfid, img_var, MIsigntype, MI_UNSIGNED);

    /* Get into data mode */
    (void) ncendef( file->cdfid );

    for_less( d, 0, n_dimensions )
        mindex[d] = 0;

    dim_value = 0.0;
    for_less( d, 0, n_dimensions )
    {
        if( is_spatial_dimension( dim_names[d], &axis ) )
        {
            (void) mivarput1( file->cdfid, dim_ids[d], mindex,
                              NC_DOUBLE, MI_SIGNED, &dim_value );
        }
    }

    file->icv = miicv_create();

    (void) miicv_setint( file->icv, MI_ICV_TYPE, data_type );
    (void) miicv_setint( file->icv, MI_ICV_DO_NORM, TRUE );
    (void) miicv_setstr( file->icv, MI_ICV_SIGN,
                         signed_flag ? MI_SIGNED : MI_UNSIGNED );
    (void) miicv_attach( file->icv, file->cdfid, img_var );

    start_index = 0;
    (void) mivarput1( file->icv, min_id, &start_index,
                      NC_DOUBLE, MI_SIGNED, &image_range[0] );
    (void) mivarput1( file->icv, max_id, &start_index,
                      NC_DOUBLE, MI_SIGNED, &image_range[1] );

    ncopts = NC_VERBOSE | NC_FATAL;

    return( file );
}

public  Status  output_minc_volume(
    Minc_file   file,
    Volume      volume )
{
    int    d, n_volume_dims, sizes[MAX_DIMENSIONS];
    long   start[MAX_VAR_DIMS], count[MAX_VAR_DIMS];
    void   *data_ptr;

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
        print( "output_minc_volume: attempted to write too many subvolumes.\n" );
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

public  int  close_minc_output(
    Minc_file   file )
{
    if( file == (Minc_file) NULL )
    {
        print( "close_minc_output(): NULL file.\n" );
        return( MI_ERROR );
    }

    (void) ncclose( file->cdfid );
    (void) miicv_free( file->icv );

    FREE( file );

    return( MI_NOERROR );
}
