#include  <def_mni.h>
#include  <minc.h>

#define  N_VALUES  256

public  Status  input_volume(
    char           filename[],
    volume_struct  *volume )
{
    Status       status;
    char         name[MAX_NC_NAME];
    int          x, y, z, volume_axis;
    double       slice_separation[N_DIMENSIONS];
    double       start_position[N_DIMENSIONS];
    double       direction_cosines[N_DIMENSIONS][N_DIMENSIONS];
    Volume_type  *image;
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

    for_less( axis, 0, ndims )
    {
        volume_axis = 2 - axis;

        (void) ncdiminq( cdfid, dim[axis], name, &length );
        volume->sizes[volume_axis] = length;

        ncopts = 0;
        dimvar = ncvarid( cdfid, name );
        if( dimvar != MI_ERROR )
        {
            if( miattget1( cdfid, dimvar, MIstep, NC_DOUBLE,
                        (void *) &slice_separation[volume_axis] ) == MI_ERROR )
            {
                slice_separation[volume_axis] = 1.0;
            }

            if( miattget1( cdfid, dimvar, MIstart, NC_DOUBLE,
                           (void *) &start_position[volume_axis] ) == MI_ERROR )
            {
                start_position[volume_axis] = 0.0;
            }

            if( miattget( cdfid, dimvar, MIstart, NC_DOUBLE, N_DIMENSIONS,
                          (void *) direction_cosines[volume_axis], (int *) 0 )
                           == MI_ERROR )
            {
                direction_cosines[volume_axis][0] = 0.0;
                direction_cosines[volume_axis][1] = 0.0;
                direction_cosines[volume_axis][2] = 0.0;
                direction_cosines[volume_axis][volume_axis] = 1.0;
            }
        }
        ncopts = NC_VERBOSE | NC_FATAL;
    }

    ALLOC3D( status, volume->data, volume->sizes[X], volume->sizes[Y],
             volume->sizes[Z] );

    ALLOC( status, image, volume->sizes[X] * volume->sizes[Y] );

    for_less( z, 0, volume->sizes[Z] )
    {
        start[0] = z;
        start[1] = 0;
        start[2] = 0;
        count[0] = 1;
        count[1] = volume->sizes[Y];
        count[2] = volume->sizes[X];

        (void) miicv_get( icv, start, count, (void *) image );

        for_less( y, 0, volume->sizes[Y] )
        {
            for_less( x, 0, volume->sizes[X] )
            {
                volume->data[x][y][z] = image[IJ(y,x,volume->sizes[X])];
            }
        }
    }

    FREE( status, image );

    (void) ncclose( cdfid );
    (void) miicv_free( icv );

    volume->thickness[X] = (Real) slice_separation[X];
    volume->thickness[Y] = (Real) slice_separation[Y];
    volume->thickness[Z] = (Real) slice_separation[Z];
    volume->value_scale = 1.0;
    volume->value_translation = 0.0;

    return( status );
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
