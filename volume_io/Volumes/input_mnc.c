#ifndef  NO_MNC_FILES

#include  <def_mni.h>
#include  <minc.h>

#define  N_VALUES  256

private  void  create_world_transform(
    Point       *origin,
    Vector      axes[N_DIMENSIONS],
    Real        thickness[N_DIMENSIONS],
    Transform   *transform );

/* ----------------------------- MNI Header -----------------------------------
@NAME       : initialize_mnc_input
@INPUT      : 
@OUTPUT     : volume
              input_info
@RETURNS    : OK if successful
@DESCRIPTION: Assumes volume->filename has been assigned.  Reads the MINC
              (Medical image Net CDF file format) header.
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  initialize_mnc_input(
    volume_struct        *volume,
    volume_input_struct  *input_info )
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

    volume->data_type = UNSIGNED_BYTE;

    input_info->icv = miicv_create();

    (void) miicv_setint( input_info->icv, MI_ICV_TYPE, NC_BYTE );
    (void) miicv_setstr( input_info->icv, MI_ICV_SIGN, MI_UNSIGNED );
    (void) miicv_setdbl( input_info->icv, MI_ICV_VALID_MAX, 255.0 );
    (void) miicv_setdbl( input_info->icv, MI_ICV_VALID_MIN, 0.0 );

    ncopts = 0;
    input_info->cdfid = ncopen( volume->filename, NC_NOWRITE );
    ncopts = NC_VERBOSE | NC_FATAL;

    if( input_info->cdfid == MI_ERROR )
    {
        print( "Error opening volume file \"%s\".\n", volume->filename );
        return( ERROR );
    }

    img = ncvarid( input_info->cdfid, MIimage );
    (void) miicv_attach( input_info->icv, input_info->cdfid, img );

    (void) ncvarinq( input_info->cdfid, img, (char *) 0, (nc_type *) 0,
                     &ndims, dim, (int *) 0 );

    if( ndims != N_DIMENSIONS )
    {
        print(
          "Error:  input volume file \"%s\" does not have exactly 3 dimensions.\n",
          volume->filename );
        status = ERROR;
        return( status );
    }

    volume->axis_index_from_file[X] = -1;
    volume->axis_index_from_file[Y] = -1;
    volume->axis_index_from_file[Z] = -1;

    for_less( axis, 0, ndims )
    {
        (void) ncdiminq( input_info->cdfid, dim[axis], name, &length );
        input_info->sizes_in_file[axis] = length;

        if( EQUAL_STRINGS( name, MIxspace ) )
            volume->axis_index_from_file[axis] = X;
        else if( EQUAL_STRINGS( name, MIyspace ) )
            volume->axis_index_from_file[axis] = Y;
        else if( EQUAL_STRINGS( name, MIzspace ) )
            volume->axis_index_from_file[axis] = Z;
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
        direction_cosines[axis][volume->axis_index_from_file[axis]] = 1.0;

        ncopts = 0;
        dimvar = ncvarid( input_info->cdfid, name );
        if( dimvar != MI_ERROR )
        {
            (void) miattget1( input_info->cdfid, dimvar, MIstep, NC_DOUBLE,
                              (void *) &slice_separation[axis] );

            (void) miattget1( input_info->cdfid, dimvar, MIstart, NC_DOUBLE,
                              (void *) &start_position[axis] );

            (void) miattget( input_info->cdfid, dimvar,
                             MIdirection_cosines, NC_DOUBLE,
                             N_DIMENSIONS, (void *) direction_cosines[axis],
                             (int *) 0 );
        }
        ncopts = NC_VERBOSE | NC_FATAL;
    }

    if( volume->axis_index_from_file[X] == -1 ||
        volume->axis_index_from_file[Y] == -1 ||
        volume->axis_index_from_file[Z] == -1 )
    {
        print( "Error:  missing some of the 3 dimensions in \"%s\".\n",
                volume->filename );
        status = ERROR;
        return( status );
    }

    for_less( axis, 0, N_DIMENSIONS )
    {
        axis_spacing[volume->axis_index_from_file[axis]] =
                                   (Real) slice_separation[axis];

        volume->thickness[volume->axis_index_from_file[axis]] =
                                   ABS( (Real) slice_separation[axis] );

        volume->flip_axis[volume->axis_index_from_file[axis]] =
                                   (slice_separation[axis] < 0.0);

        volume->sizes[volume->axis_index_from_file[axis]] =
                                   input_info->sizes_in_file[axis];
        fill_Vector( axes[volume->axis_index_from_file[axis]],
                     direction_cosines[axis][0],
                     direction_cosines[axis][1],
                     direction_cosines[axis][2] );
        NORMALIZE_VECTOR( axes[volume->axis_index_from_file[axis]],
                          axes[volume->axis_index_from_file[axis]] );
        if( null_Vector( &axes[volume->axis_index_from_file[axis]] ) )
        {
            fill_Vector( axes[volume->axis_index_from_file[axis]],
                         0.0, 0.0, 0.0 );
            Vector_coord(axes[volume->axis_index_from_file[axis]],
                         volume->axis_index_from_file[axis]) = 1.0;
        }
    }

    SCALE_VECTOR( x_offset, axes[X],
                  start_position[volume->axis_index_from_file[X]] );
    SCALE_VECTOR( y_offset, axes[Y],
                  start_position[volume->axis_index_from_file[Y]] );
    SCALE_VECTOR( z_offset, axes[Z],
                  start_position[volume->axis_index_from_file[Z]] );
    ADD_VECTORS( start_offset, x_offset, y_offset );
    ADD_VECTORS( start_offset, start_offset, z_offset );

    CONVERT_VECTOR_TO_POINT( origin, start_offset );

    create_world_transform( &origin, axes, axis_spacing,
                            &volume->voxel_to_world_transform );

    ALLOC( input_info->byte_slice_buffer, input_info->sizes_in_file[1] *
                                          input_info->sizes_in_file[2] );
    input_info->slice_index = 0;

    volume->value_scale = 1.0;
    volume->value_translation = 0.0;

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : delete_mnc_input
@INPUT      : input_info
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Frees the slice buffer, and closes the file.
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  delete_mnc_input(
    volume_input_struct   *input_info )
{
    FREE( input_info->byte_slice_buffer );

    (void) ncclose( input_info->cdfid );
    (void) miicv_free( input_info->icv );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : input_more_mnc_file
@INPUT      : volume
              input_info
@OUTPUT     : fraction_done
@RETURNS    : TRUE if there is more data to input
@DESCRIPTION: Inputs the next slice from the data file.
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Boolean  input_more_mnc_file(
    volume_struct         *volume,
    volume_input_struct   *input_info,
    Real                  *fraction_done )
{
    Boolean           more_to_do;
    int               index1, index2, indices[N_DIMENSIONS];
    long              start[N_DIMENSIONS], count[N_DIMENSIONS];
    unsigned char     *slice_pointer;

    if( input_info->slice_index < input_info->sizes_in_file[0] )
    {
        indices[volume->axis_index_from_file[0]] = input_info->slice_index;

        start[0] = input_info->slice_index;
        start[1] = 0;
        start[2] = 0;
        count[0] = 1;
        count[1] = input_info->sizes_in_file[1];
        count[2] = input_info->sizes_in_file[2];

        (void) miicv_get( input_info->icv, start, count,
                          (void *) input_info->byte_slice_buffer );

        slice_pointer = input_info->byte_slice_buffer;

        for_less( index1, 0, input_info->sizes_in_file[1] )
        {
            indices[volume->axis_index_from_file[1]] = index1;
            for_less( index2, 0, input_info->sizes_in_file[2] )
            {
                indices[volume->axis_index_from_file[2]] = index2;
                volume->byte_data[indices[X]][indices[Y]][indices[Z]] =
                     (unsigned char) (*slice_pointer);
                ++slice_pointer;
            }
        }

        ++input_info->slice_index;
    }

    *fraction_done = (Real) input_info->slice_index /
                     (Real) volume->sizes[volume->axis_index_from_file[0]];

    more_to_do = TRUE;

    if( input_info->slice_index ==
        volume->sizes[volume->axis_index_from_file[0]] )
    {
        more_to_do = FALSE;
        delete_volume_input( input_info );
    }

    return( more_to_do );
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

#endif
