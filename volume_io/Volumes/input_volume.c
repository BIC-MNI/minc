#include  <def_mni.h>
#include  <minc.h>

#define  N_VALUES  256

public  Status  input_volume(
    char           filename[],
    volume_struct  *volume )
{
    Status       status;
    int          x, y, z;
    Volume_type  *x_strip;
    int          axis;
    long         length;
    int          icv, cdfid, img;
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
        (void) ncdiminq( cdfid, dim[axis], (char *) 0, &length );
        volume->sizes[2-axis] = length;
    }

    ALLOC3D( status, volume->data, volume->sizes[X], volume->sizes[Y],
             volume->sizes[Z] );

    ALLOC( status, x_strip, volume->sizes[X] );

    for_less( z, 0, volume->sizes[Z] )
    {
        for_less( y, 0, volume->sizes[Y] )
        {
            start[0] = z;
            start[1] = y;
            start[2] = 0;
            count[0] = 1;
            count[1] = 1;
            count[2] = volume->sizes[X];
            (void) miicv_get( icv, start, count, (void *) x_strip );

            for_less( x, 0, volume->sizes[X] )
            {
                volume->data[x][y][z] = x_strip[x];
            }
        }
    }

    FREE( status, x_strip );

    (void) ncclose( cdfid );
    (void) miicv_free( icv );

    volume->thickness[X] = 1.0;
    volume->thickness[Y] = 1.0;
    volume->thickness[Z] = 1.0;
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
