#include  <def_mni.h>
#include  <minc.h>

#define  N_VALUES  256

private  void  create_world_transform(
    Point       *origin,
    Vector      axes[N_DIMENSIONS],
    Real        thickness[N_DIMENSIONS],
    Transform   *transform );

public  Status  input_volume(
    char           filename[],
    volume_struct  *volume )
{
    Status       status;
    char         name[MAX_NC_NAME];
    int          axis_index[N_DIMENSIONS];
    double       slice_separation[N_DIMENSIONS];
    double       start_position[N_DIMENSIONS];
    double       direction_cosines[N_DIMENSIONS][N_DIMENSIONS];
    Point        origin;
    Vector       axes[N_DIMENSIONS];
    Vector       x_offset, y_offset, z_offset, start_offset;
    Volume_type  *image;
    int          indices[N_DIMENSIONS], sizes[N_DIMENSIONS];
    int          index0, index1, index2;
    int          axis;
    long         length;
    int          icv, cdfid, img, dimvar;
    int          ndims, dim[MAX_VAR_DIMS];
    long         start[3], count[3];

    icv = miicv_create();

    (void) miicv_setint( icv, MI_ICV_TYPE, NC_BYTE );
    (void) miicv_set( icv, MI_ICV_SIGN, MI_UNSIGNED );
    (void) miicv_setdbl( icv, MI_ICV_VALID_MAX, 255.0 );
    (void) miicv_setdbl( icv, MI_ICV_VALID_MIN, 0.0 );

    cdfid = ncopen( filename, NC_NOWRITE );

    img = ncvarid( cdfid, MIimage );
    (void) miicv_attach( icv, cdfid, img );

    (void) ncvarinq( cdfid, img, (char *) 0, (nc_type *) 0,
                     &ndims, dim, (int *) 0 );

    if( ndims != N_DIMENSIONS )
    {
        print(
           "Error:  input volume file does not have exactly 3 dimensions.\n" );
        status = ERROR;
        return( status );
    }

    axis_index[X] = -1;
    axis_index[Y] = -1;
    axis_index[Z] = -1;

    for_less( axis, 0, ndims )
    {
        (void) ncdiminq( cdfid, dim[axis], name, &length );
        sizes[axis] = length;

        if( EQUAL_STRINGS( name, MIxspace ) )
            axis_index[axis] = X;
        else if( EQUAL_STRINGS( name, MIyspace ) )
            axis_index[axis] = Y;
        else if( EQUAL_STRINGS( name, MIzspace ) )
            axis_index[axis] = Z;
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
        direction_cosines[axis][axis_index[axis]] = 1.0;

        ncopts = 0;
        dimvar = ncvarid( cdfid, name );
        if( dimvar != MI_ERROR )
        {
            (void) miattget1( cdfid, dimvar, MIstep, NC_DOUBLE,
                              (void *) &slice_separation[axis] );

            (void) miattget1( cdfid, dimvar, MIstart, NC_DOUBLE,
                              (void *) &start_position[axis] );

            (void) miattget( cdfid, dimvar, MIdirection_cosines, NC_DOUBLE,
                             N_DIMENSIONS, (void *) direction_cosines[axis],
                             (int *) 0 );
        }
        ncopts = NC_VERBOSE | NC_FATAL;
    }

    if( axis_index[X] == -1 || axis_index[Y] == -1 || axis_index[Z] == -1 )
    {
        print( "Error:  missing some of the 3 dimensions.\n" );
        status = ERROR;
        return( status );
    }

    for_less( axis, 0, N_DIMENSIONS )
    {
        volume->thickness[axis_index[axis]] = (Real) slice_separation[axis];
        volume->sizes[axis_index[axis]] = sizes[axis];
        fill_Vector( axes[axis_index[axis]],
                     direction_cosines[axis][0],
                     direction_cosines[axis][1],
                     direction_cosines[axis][2] );
        NORMALIZE_VECTOR( axes[axis_index[axis]], axes[axis_index[axis]] );
        if( null_Vector( &axes[axis_index[axis]] ) )
        {
            fill_Vector( axes[axis_index[axis]],
                         0.0, 0.0, 0.0 );
            Vector_coord(axes[axis_index[axis]],axis_index[axis]) = 1.0;
        }
    }

    SCALE_VECTOR( x_offset, axes[X], start_position[axis_index[X]] );
    SCALE_VECTOR( y_offset, axes[Y], start_position[axis_index[Y]] );
    SCALE_VECTOR( z_offset, axes[Z], start_position[axis_index[Z]] );
    ADD_VECTORS( start_offset, x_offset, y_offset );
    ADD_VECTORS( start_offset, start_offset, z_offset );

    CONVERT_VECTOR_TO_POINT( origin, start_offset );

    create_world_transform( &origin, axes, volume->thickness,
                            &volume->world_transform );

    ALLOC3D( status, volume->data, volume->sizes[X], volume->sizes[Y],
             volume->sizes[Z] );

    ALLOC( status, image, sizes[1] * sizes[2] );

    for_less( index0, 0, sizes[0] )
    {
        indices[axis_index[0]] = index0;
        start[0] = index0;
        start[1] = 0;
        start[2] = 0;
        count[0] = 1;
        count[1] = sizes[1];
        count[2] = sizes[2];

        (void) miicv_get( icv, start, count, (void *) image );

        for_less( index1, 0, sizes[1] )
        {
            indices[axis_index[1]] = index1;
            for_less( index2, 0, sizes[2] )
            {
                indices[axis_index[2]] = index2;
                volume->data[indices[X]][indices[Y]][indices[Z]] =
                                 image[IJ(index1,index2,sizes[2])];
            }
        }
    }

    FREE( status, image );

    (void) ncclose( cdfid );
    (void) miicv_free( icv );

    volume->value_scale = 1.0;
    volume->value_translation = 0.0;

    return( status );
}

private  void  create_world_transform(
    Point       *origin,
    Vector      axes[N_DIMENSIONS],
    Real        thickness[N_DIMENSIONS],
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

    SCALE_VECTOR( x_axis, x_axis, thickness[X] );
    SCALE_VECTOR( y_axis, y_axis, thickness[Y] );
    SCALE_VECTOR( z_axis, z_axis, thickness[Z] );

    make_change_to_bases_transform( origin, &x_axis, &y_axis, &z_axis,
                                    transform );
}

public  Status  input_fake_volume(
    char           filename[],
    volume_struct  *volume )    /* ARGSUSED */
{
    Status    status;
    int       x, y, z, val;
    Real      dx, dy, dz;

    volume->sizes[X] = 100;
    volume->sizes[Y] = 100;
    volume->sizes[Z] = 100;
    volume->thickness[X] = 1.0;
    volume->thickness[Y] = 1.0;
    volume->thickness[Z] = 1.0;
    volume->value_scale = 1.0;
    volume->value_translation = 0.0;

    ALLOC3D( status, volume->data, volume->sizes[X], volume->sizes[Y],
             volume->sizes[Z] );

    for_less( x, 0, volume->sizes[X] )
    {
        dx = 2.0 * (Real) x / (Real) volume->sizes[X] - 1.0;
        if( dx < 0.0 ) dx = -dx;
        for_less( y, 0, volume->sizes[Y] )
        {
            dy = 2.0 * (Real) y / (Real) volume->sizes[Y] - 1.0;
            if( dy < 0.0 ) dy = -dy;
            for_less( z, 0, volume->sizes[Z] )
            {
                dz = 2.0 * (Real) z / (Real) volume->sizes[Z] - 1.0;
                if( dz < 0.0 ) dz = -dz;
                val = (int) ((dx + dy + dz) * (Real) N_VALUES / 2.0 );
                if( val > N_VALUES-1 )  val = 0;
                volume->data[x][y][z] = val;
            }
        }
        if( x % 10 == 0 )
            print( "%d/%d\n", x, volume->sizes[X] );
    }

    return( status );
}
