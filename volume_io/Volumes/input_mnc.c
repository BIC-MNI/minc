#include  <def_mni.h>
#include  <minc.h>

#define  N_VALUES  256

private  void  create_world_transform(
    Point       *origin,
    Vector      axes[N_DIMENSIONS],
    Real        thickness[N_DIMENSIONS],
    Transform   *transform );

public  Status  initialize_mnc_input(
    volume_struct        *volume,
    volume_input_struct  *input )
{
    Status            status;
    char              name[MAX_NC_NAME];
    double            slice_separation[N_DIMENSIONS];
    double            start_position[N_DIMENSIONS];
    double            direction_cosines[N_DIMENSIONS][N_DIMENSIONS];
    Real              axis_spacing[N_DIMENSIONS];
    Point             origin;
    Vector            axes[N_DIMENSIONS];
    Vector            x_offset, y_offset, z_offset, start_offset;
    int               axis;
    long              length;
    int               img, dimvar;
    int               ndims, dim[MAX_VAR_DIMS];

    status = OK;

    input->icv = miicv_create();

    (void) miicv_setint( input->icv, MI_ICV_TYPE, NC_BYTE );
    (void) miicv_setstr( input->icv, MI_ICV_SIGN, MI_UNSIGNED );
    (void) miicv_setdbl( input->icv, MI_ICV_VALID_MAX, 255.0 );
    (void) miicv_setdbl( input->icv, MI_ICV_VALID_MIN, 0.0 );

    ncopts = 0;
    input->cdfid = ncopen( volume->filename, NC_NOWRITE );
    ncopts = NC_VERBOSE | NC_FATAL;

    if( input->cdfid == MI_ERROR )
    {
        print( "Error opening volume file \"%s\".\n", volume->filename );
        return( ERROR );
    }

    img = ncvarid( input->cdfid, MIimage );
    (void) miicv_attach( input->icv, input->cdfid, img );

    (void) ncvarinq( input->cdfid, img, (char *) 0, (nc_type *) 0,
                     &ndims, dim, (int *) 0 );

    if( ndims != N_DIMENSIONS )
    {
        print(
          "Error:  input volume file \"%s\" does not have exactly 3 dimensions.\n",
          volume->filename );
        status = ERROR;
        return( status );
    }

    input->axis_index[X] = -1;
    input->axis_index[Y] = -1;
    input->axis_index[Z] = -1;

    for_less( axis, 0, ndims )
    {
        (void) ncdiminq( input->cdfid, dim[axis], name, &length );
        input->sizes_in_file[axis] = length;

        if( EQUAL_STRINGS( name, MIxspace ) )
            input->axis_index[axis] = X;
        else if( EQUAL_STRINGS( name, MIyspace ) )
            input->axis_index[axis] = Y;
        else if( EQUAL_STRINGS( name, MIzspace ) )
            input->axis_index[axis] = Z;
        else
        {
            print("Error:  cannot handle dimensions other than x, y, and z.\n");
            print("        offending dimension: %s\n", name );
            status = ERROR;
            return( status );
        }

        slice_separation[axis] = 1.0;
        start_position[axis] = 0.0;
        direction_cosines[axis][0] = 0.0;
        direction_cosines[axis][1] = 0.0;
        direction_cosines[axis][2] = 0.0;
        direction_cosines[axis][input->axis_index[axis]] = 1.0;

        ncopts = 0;
        dimvar = ncvarid( input->cdfid, name );
        if( dimvar != MI_ERROR )
        {
            (void) miattget1( input->cdfid, dimvar, MIstep, NC_DOUBLE,
                              (void *) &slice_separation[axis] );

            (void) miattget1( input->cdfid, dimvar, MIstart, NC_DOUBLE,
                              (void *) &start_position[axis] );

            (void) miattget( input->cdfid, dimvar,
                             MIdirection_cosines, NC_DOUBLE,
                             N_DIMENSIONS, (void *) direction_cosines[axis],
                             (int *) 0 );
        }
        ncopts = NC_VERBOSE | NC_FATAL;
    }

    if( input->axis_index[X] == -1 || input->axis_index[Y] == -1 ||
        input->axis_index[Z] == -1 )
    {
        print( "Error:  missing some of the 3 dimensions in \"%s\".\n",
                volume->filename );
        status = ERROR;
        return( status );
    }

    for_less( axis, 0, N_DIMENSIONS )
    {
        axis_spacing[input->axis_index[axis]] = (Real) slice_separation[axis];

        volume->thickness[input->axis_index[axis]] =
                                   ABS( (Real) slice_separation[axis] );

        volume->flip_axis[input->axis_index[axis]] =
                                                (slice_separation[axis] < 0.0);

        volume->sizes[input->axis_index[axis]] = input->sizes_in_file[axis];
        fill_Vector( axes[input->axis_index[axis]],
                     direction_cosines[axis][0],
                     direction_cosines[axis][1],
                     direction_cosines[axis][2] );
        NORMALIZE_VECTOR( axes[input->axis_index[axis]],
                          axes[input->axis_index[axis]] );
        if( null_Vector( &axes[input->axis_index[axis]] ) )
        {
            fill_Vector( axes[input->axis_index[axis]],
                         0.0, 0.0, 0.0 );
            Vector_coord(axes[input->axis_index[axis]],input->axis_index[axis])
                                                                       = 1.0;
        }
    }

    SCALE_VECTOR( x_offset, axes[X], start_position[input->axis_index[X]] );
    SCALE_VECTOR( y_offset, axes[Y], start_position[input->axis_index[Y]] );
    SCALE_VECTOR( z_offset, axes[Z], start_position[input->axis_index[Z]] );
    ADD_VECTORS( start_offset, x_offset, y_offset );
    ADD_VECTORS( start_offset, start_offset, z_offset );

    CONVERT_VECTOR_TO_POINT( origin, start_offset );

    create_world_transform( &origin, axes, axis_spacing,
                            &volume->voxel_to_world_transform );

    ALLOC( input->slice_buffer, input->sizes_in_file[1] *
                                input->sizes_in_file[2] );
    input->slice_index = 0;

    return( status );
}

public  void  delete_mnc_input(
    volume_input_struct   *input )
{
    FREE( input->slice_buffer );

    (void) ncclose( input->cdfid );
    (void) miicv_free( input->icv );
}

public  Boolean  input_more_mnc_file(
    volume_struct         *volume,
    volume_input_struct   *input,
    Real                  *fraction_done )
{
    Boolean           more_to_do;
    int               index1, index2, indices[N_DIMENSIONS];
    long              start[N_DIMENSIONS], count[N_DIMENSIONS];
    Volume_type       *slice_pointer;

    if( input->slice_index < input->sizes_in_file[0] )
    {
        indices[input->axis_index[0]] = input->slice_index;

        start[0] = input->slice_index;
        start[1] = 0;
        start[2] = 0;
        count[0] = 1;
        count[1] = input->sizes_in_file[1];
        count[2] = input->sizes_in_file[2];

        (void) miicv_get( input->icv, start, count,
                          (void *) input->slice_buffer );

        slice_pointer = input->slice_buffer;

        for_less( index1, 0, input->sizes_in_file[1] )
        {
            indices[input->axis_index[1]] = index1;
            for_less( index2, 0, input->sizes_in_file[2] )
            {
                indices[input->axis_index[2]] = index2;
                volume->data[indices[X]][indices[Y]][indices[Z]] =
                     *slice_pointer;
                ++slice_pointer;
            }
        }

        ++input->slice_index;
    }

    *fraction_done = (Real) input->slice_index /
                     (Real) volume->sizes[input->axis_index[0]];

    more_to_do = TRUE;

    if( input->slice_index == (Real) volume->sizes[input->axis_index[0]] )
    {
        more_to_do = FALSE;
        delete_volume_input( input );
    }

    return( more_to_do );
}

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

#ifdef FORCE_ORTHONORMAL
    CROSS_VECTORS( z_axis, x_axis, y_axis );

    if( DOT_VECTORS(z_axis,axes[Z]) < 0.0 )
    {
        SCALE_VECTOR( z_axis, z_axis, -1.0 );
        CROSS_VECTORS( y_axis, x_axis, z_axis );
    }
    else
    {
        CROSS_VECTORS( y_axis, z_axis, x_axis );
    }

    NORMALIZE_VECTOR( x_axis, x_axis );
    NORMALIZE_VECTOR( y_axis, y_axis );
    NORMALIZE_VECTOR( z_axis, z_axis );
#endif

    SCALE_VECTOR( x_axis, x_axis, axis_spacing[X] );
    SCALE_VECTOR( y_axis, y_axis, axis_spacing[Y] );
    SCALE_VECTOR( z_axis, z_axis, axis_spacing[Z] );

    make_change_to_bases_transform( origin, &x_axis, &y_axis, &z_axis,
                                    transform );
}
