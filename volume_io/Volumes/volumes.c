#include  <def_mni.h>
#include  <def_splines.h>

private  char  *default_dimension_names[MAX_DIMENSIONS][MAX_DIMENSIONS] =
{
    { MIxspace },
    { MIyspace, MIxspace },
    { MIzspace, MIyspace, MIxspace },
    { MItime, MIzspace, MIyspace, MIxspace },
    { "", MItime, MIzspace, MIyspace, MIxspace }
};

private  void  free_volume_data(
    Volume   volume );

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_volume
@INPUT      : n_dimensions      - number of dimensions (1-5)
              dimension_names   - name of dimensions for use when reading file
              data_type         - type of the data, e.g. NC_BYTE
              signed_flag       - type is signed?
              min_value         - min and max value to be stored
              max_value
@OUTPUT     : 
@RETURNS    : Volume
@DESCRIPTION: Creates a Volume structure, and initializes it.  In order to 
              later use the volume, you must call either set_volume_size()
              and alloc_volume_data(), or one of the input volume routines,
              which in turn calls these two.
              The dimension_names are used when inputting
              MINC files, in order to match with the dimension names in the
              file.  Typically, use dimension names
              { MIzspace, MIyspace, MIxspace } to read the volume from the
              file in the order it is stored, or
              { MIxspace, MIyspace, MIzspace } to read it so you can subcript
              the volume in x, y, z order.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June, 1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public   Volume   create_volume(
    int         n_dimensions,
    String      dimension_names[],
    nc_type     nc_data_type,
    Boolean     signed_flag )
{
    int             i, c, sizes[MAX_DIMENSIONS];
    Status          status;
    volume_struct   *volume;
    Transform       identity;

    status = OK;

    if( n_dimensions < 1 || n_dimensions > MAX_DIMENSIONS )
    {
        print( "create_volume(): n_dimensions (%d) not in range 1 to %d.\n",
               n_dimensions, MAX_DIMENSIONS );
        status = ERROR;
    }

    if( status == ERROR )
    {
        return( (Volume) NULL );
    }

    ALLOC( volume, 1 );

    volume->data = (void *) NULL;

    volume->n_dimensions = n_dimensions;

    volume->min_voxel = 0.0;
    volume->max_voxel = 0.0;
    volume->real_range_set = FALSE;
    volume->real_value_scale = 1.0;
    volume->real_value_translation = 0.0;

    for_less( c, 0, N_DIMENSIONS )
    {
        volume->translation_voxel[c] = 0.0;
        volume->world_space_for_translation_voxel[c] = 0.0;
    }

    volume->labels = (unsigned char ***) NULL;

    for_less( i, 0, n_dimensions )
    {
        sizes[i] = 0;
        volume->separations[i] = 1.0;

        if( dimension_names != (String *) NULL )
            (void) strcpy( volume->dimension_names[i], dimension_names[i] );
        else
            (void) strcpy( volume->dimension_names[i],
                           default_dimension_names[n_dimensions-1][i] );
    }

    volume->data_type = NO_DATA_TYPE;
    set_volume_size( volume, nc_data_type, signed_flag, sizes );

    make_identity_transform( &identity );
    create_linear_transform( &volume->voxel_to_world_transform, &identity );

    return( volume );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_volume_size
@INPUT      : volume
              nc_data_type
              signed_flag
              sizes[]        - sizes per dimension
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the sizes of the dimensions of the volume, and if specified,
              the data type also.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June, 1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  set_volume_size(
    Volume       volume,
    nc_type      nc_data_type,
    Boolean      signed_flag,
    int          sizes[] )
{
    int             i;
    Data_types      data_type;

    if( nc_data_type != NC_UNSPECIFIED )
    {
        switch( nc_data_type )
        {
        case  NC_BYTE:
            if( signed_flag )
                data_type = SIGNED_BYTE;
            else
                data_type = UNSIGNED_BYTE;
            break;

        case  NC_SHORT:
            if( signed_flag )
                data_type = SIGNED_SHORT;
            else
                data_type = UNSIGNED_SHORT;
            break;

        case  NC_LONG:
            if( signed_flag )
                data_type = SIGNED_LONG;
            else
                data_type = UNSIGNED_LONG;
            break;

        case  NC_FLOAT:
            data_type = FLOAT;
            break;

        case  NC_DOUBLE:
            data_type = DOUBLE;
            break;
        }
        volume->data_type = data_type;
        volume->nc_data_type = nc_data_type;
        volume->signed_flag = signed_flag;
    }

    for_less( i, 0, volume->n_dimensions )
        volume->sizes[i] = sizes[i];
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_type_size
@INPUT      : type
@OUTPUT     : 
@RETURNS    : size of the type
@DESCRIPTION: Returns the size of the given type.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June, 1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  int  get_type_size(
    Data_types   type )
{
    int   size;

    switch( type )
    {
    case  UNSIGNED_BYTE:    size = sizeof( unsigned char );   break;
    case  SIGNED_BYTE:      size = sizeof( signed   char );   break;
    case  UNSIGNED_SHORT:   size = sizeof( unsigned short );  break;
    case  SIGNED_SHORT:     size = sizeof( signed   short );  break;
    case  UNSIGNED_LONG:    size = sizeof( unsigned long );   break;
    case  SIGNED_LONG:      size = sizeof( signed   long );   break;
    case  FLOAT:            size = sizeof( float );           break;
    case  DOUBLE:           size = sizeof( double );          break;
    }

    return( size );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ALLOCATE_DATA   - private macro for allocation
@INPUT      : n_dimensions
              type
              sizes
@OUTPUT     : ptr
@RETURNS    : Macro to allocate multidimensional data given the type.
@DESCRIPTION: 
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June, 1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  ALLOCATE_DATA( ptr, n_dimensions, type, sizes ) \
switch( n_dimensions ) \
{ \
    type  *_p1, **_p2, ***_p3, ****_p4, *****_p5; \
 \
    case  1:  ALLOC( _p1, (sizes)[0] ); \
              (ptr) = (void *) _p1; \
              break; \
    case  2:  ALLOC2D( _p2, (sizes)[0], (sizes)[1] ); \
              (ptr) = (void *) _p2; \
              break; \
    case  3:  ALLOC3D( _p3, (sizes)[0], (sizes)[1], (sizes)[2] ); \
              (ptr) = (void *) _p3; \
              break; \
    case  4:  ALLOC4D( _p4, (sizes)[0], (sizes)[1], (sizes)[2], (sizes)[3] ); \
              (ptr) = (void *) _p4; \
              break; \
    case  5:  ALLOC5D( _p5, (sizes)[0], (sizes)[1], (sizes)[2], (sizes)[3], (sizes)[4] ); \
              (ptr) = (void *) _p5; \
              break; \
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : alloc_volume_data
@INPUT      : volume
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Allocates the memory for the volume.  Assumes that the
              volume type and sizes have been set.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June, 1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  alloc_volume_data(
    Volume   volume )
{
    int    *sizes, n_dimensions;
    void   *ptr;

    if( volume->data != (void *) NULL )
    {
        free_volume_data( volume );
    }

    if( volume->data_type == NO_DATA_TYPE )
    {
        print( "Error: cannot allocate volume data until size specified.\n" );
        return;
    }

    n_dimensions = volume->n_dimensions;
    sizes = volume->sizes;

    switch( volume->data_type )
    {
    case  UNSIGNED_BYTE:
        ALLOCATE_DATA( ptr, n_dimensions, unsigned char, sizes );
        break;
    case  SIGNED_BYTE:
        ALLOCATE_DATA( ptr, n_dimensions, char, sizes );
        break;
    case  UNSIGNED_SHORT:
        ALLOCATE_DATA( ptr, n_dimensions, unsigned short, sizes );
        break;
    case  SIGNED_SHORT:
        ALLOCATE_DATA( ptr, n_dimensions, short, sizes );
        break;
    case  UNSIGNED_LONG:
        ALLOCATE_DATA( ptr, n_dimensions, unsigned long, sizes );
        break;
    case  SIGNED_LONG:
        ALLOCATE_DATA( ptr, n_dimensions, long, sizes );
        break;
    case  FLOAT:
        ALLOCATE_DATA( ptr, n_dimensions, float, sizes );
        break;
    case  DOUBLE:
        ALLOCATE_DATA( ptr, n_dimensions, double, sizes );
        break;
    }

    volume->data = ptr;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : free_volume_data
@INPUT      : volume
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Frees the memory associated with the volume multidimensional data.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June, 1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  void  free_volume_data(
    Volume   volume )
{
    void   *ptr, **ptr2, ***ptr3, ****ptr4, *****ptr5;

    if( volume->data == (void *) NULL )
    {
        print( "Error: cannot free NULL volume data.\n" );
        return;
    }

    ptr = volume->data;
    switch( volume->n_dimensions )
    {
    case  1:  FREE( ptr );
              break;
    case  2:  ptr2 = (void **) ptr;
              FREE2D( ptr2 );
              break;
    case  3:  ptr3 = (void ***) ptr;
              FREE3D( ptr3 );
              break;
    case  4:  ptr4 = (void ****) ptr;
              FREE4D( ptr4 );
              break;
    case  5:  ptr5 = (void *****) ptr;
              FREE5D( ptr5 );
              break;
    }

    volume->data = (void *) NULL;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : delete_volume
@INPUT      : volume
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Frees all memory from the volume and the volume struct itself.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June, 1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  delete_volume(
    Volume   volume )
{
    if( volume == (Volume) NULL )
    {
        print( "delete_volume():  cannot delete a null volume.\n" );
        return;
    }

    if( volume->data != (void *) NULL )
        free_volume_data( volume );

    free_auxiliary_data( volume );

    delete_general_transform( &volume->voxel_to_world_transform );

    FREE( volume );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_volume_n_dimensions
@INPUT      : volume
@OUTPUT     : 
@RETURNS    : number of dimensions
@DESCRIPTION: Returns the number of dimensions of the volume
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June, 1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  int  get_volume_n_dimensions(
    Volume   volume )
{
    return( volume->n_dimensions );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_volume_sizes
@INPUT      : volume
@OUTPUT     : sizes
@RETURNS    : 
@DESCRIPTION: Passes back the sizes of each of the dimensions.  Assumes sizes
              has enough room for n_dimensions integers.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June, 1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  get_volume_sizes(
    Volume   volume,
    int      sizes[] )
{
    int   i;

    for_less( i, 0, volume->n_dimensions )
        sizes[i] = volume->sizes[i];
}

public  void  set_voxel_to_world_transform(
    Volume             volume,
    General_transform  *transform )
{
    delete_general_transform( &volume->voxel_to_world_transform );

    volume->voxel_to_world_transform = *transform;
}

public  General_transform  *get_voxel_to_world_transform(
    Volume   volume )
{
    return( &volume->voxel_to_world_transform );
}

private  void  recompute_world_transform(
    Volume  volume )
{
    Transform                scale_xform, translation_xform, linear_xform;
    General_transform        general_transform;
    Real                     x, y, z;

    make_scale_transform( volume->separations[X],
                          volume->separations[Y],
                          volume->separations[Z], &scale_xform );

    transform_point( &scale_xform,
                     volume->translation_voxel[X],
                     volume->translation_voxel[Y],
                     volume->translation_voxel[Z], &x, &y, &z );

    make_translation_transform(volume->world_space_for_translation_voxel[X] - x,
                               volume->world_space_for_translation_voxel[Y] - y,
                               volume->world_space_for_translation_voxel[Z] - z,
                               &translation_xform );

    concat_transforms( &linear_xform, &scale_xform, &translation_xform );

    create_linear_transform( &general_transform, &linear_xform );

    set_voxel_to_world_transform( volume, &general_transform );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_volume_separations
@INPUT      : volume
@OUTPUT     : separations
@RETURNS    : 
@DESCRIPTION: Passes back the slice separations for each dimensions.  Assumes
              separations contains enough room for n_dimensions Reals.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June, 1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  get_volume_separations(
    Volume   volume,
    Real     separations[] )
{
    int   i;

    for_less( i, 0, volume->n_dimensions )
        separations[i] = volume->separations[i];
}

public  void  set_volume_separations(
    Volume   volume,
    Real     separations[] )
{
    int   i;

    for_less( i, 0, volume->n_dimensions )
        volume->separations[i] = separations[i];

    recompute_world_transform( volume );
}

public  void  set_volume_translation(
    Volume  volume,
    Real    voxel[],
    Real    world_space_voxel_maps_to[] )
{
    int  c;

    for_less( c, 0, N_DIMENSIONS )
    {
        volume->translation_voxel[c] = voxel[c];
        volume->world_space_for_translation_voxel[c] =
                               world_space_voxel_maps_to[c];
    }

    recompute_world_transform( volume );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_voxel_to_world
@INPUT      : volume
              x_voxel
              y_voxel
              z_voxel
@OUTPUT     : x_world
              y_world
              z_world
@RETURNS    : 
@DESCRIPTION: Converts the given voxel position to a world coordinate.
              Note that centre of first voxel corresponds to (0.0,0.0,0.0) in
              voxel coordinates.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  convert_voxel_to_world(
    Volume   volume,
    Real     x_voxel,
    Real     y_voxel,
    Real     z_voxel,
    Real     *x_world,
    Real     *y_world,
    Real     *z_world )
{
    /* apply linear transform */

    general_transform_point( &volume->voxel_to_world_transform,
                             x_voxel, y_voxel, z_voxel,
                             x_world, y_world, z_world );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_voxel_normal_vector_to_world
@INPUT      : volume
              x_voxel
              y_voxel
              z_voxel
@OUTPUT     : x_world
              y_world
              z_world
@RETURNS    : 
@DESCRIPTION: Converts a voxel vector to world coordinates.  Assumes the
              vector is a normal vector (ie. a derivative), so transforms by
              transpose of inverse transform.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  convert_voxel_normal_vector_to_world(
    Volume          volume,
    Real            x_voxel,
    Real            y_voxel,
    Real            z_voxel,
    Real            *x_world,
    Real            *y_world,
    Real            *z_world )
{
    Transform   *inverse;

    if( get_transform_type( &volume->voxel_to_world_transform ) != LINEAR )
    {
        HANDLE_INTERNAL_ERROR( "Cannot get normal vector of nonlinear xforms.");
    }

    inverse = get_inverse_linear_transform_ptr(
                                      &volume->voxel_to_world_transform );

    /* transform vector by transpose of inverse transformation */

    *x_world = Transform_elem(*inverse,0,0) * x_voxel+
               Transform_elem(*inverse,1,0) * y_voxel+
               Transform_elem(*inverse,2,0) * z_voxel;
    *y_world = Transform_elem(*inverse,0,1) * x_voxel+
               Transform_elem(*inverse,1,1) * y_voxel+
               Transform_elem(*inverse,2,1) * z_voxel;
    *z_world = Transform_elem(*inverse,0,2) * x_voxel+
               Transform_elem(*inverse,1,2) * y_voxel+
               Transform_elem(*inverse,2,2) * z_voxel;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_world_to_voxel
@INPUT      : volume
              x_world
              y_world
              z_world
@OUTPUT     : x_voxel
              y_voxel
              z_voxel
@RETURNS    : 
@DESCRIPTION: Converts from world coordinates to voxel coordinates.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  convert_world_to_voxel(
    Volume   volume,
    Real     x_world,
    Real     y_world,
    Real     z_world,
    Real     *x_voxel,
    Real     *y_voxel,
    Real     *z_voxel )
{
    /* apply linear transform */

    general_inverse_transform_point( &volume->voxel_to_world_transform,
                     x_world, y_world, z_world, x_voxel, y_voxel, z_voxel );
}

public  Real  get_volume_min_voxel(
    Volume   volume )
{
    return( volume->min_voxel );
}

public  Real  get_volume_max_voxel(
    Volume   volume )
{
    return( volume->max_voxel );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_volume_voxel_range
@INPUT      : volume
@OUTPUT     : min_voxel
              max_voxel
@RETURNS    : 
@DESCRIPTION: Passes back the min and max voxel values stored in the volume.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June, 1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  get_volume_voxel_range(
    Volume     volume,
    Real       *min_voxel,
    Real       *max_voxel )
{
    *min_voxel = get_volume_min_voxel( volume );
    *max_voxel = get_volume_max_voxel( volume );
}

public  void  set_volume_voxel_range(
    Volume   volume,
    Real     min_voxel,
    Real     max_voxel )
{
    Real  real_min, real_max;

    if( min_voxel >= max_voxel )
    {
        print( "set_volume_voxel_range( %g, %g ) : ", min_voxel, max_voxel );
        print( " min must be less than max\n" );
    }

    if( volume->real_range_set )
        get_volume_real_range( volume, &real_min, &real_max );

    volume->min_voxel = min_voxel;
    volume->max_voxel = max_voxel;

    if( volume->real_range_set )
        set_volume_real_range( volume, real_min, real_max );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_volume_real_range
@INPUT      : volume
@OUTPUT     : min_value
              max_value
@RETURNS    : 
@DESCRIPTION: Passes back the minimum and maximum scaled values.  These are
              the minimum and maximum stored voxel values scaled to the
              real value domain.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June, 1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  get_volume_real_range(
    Volume     volume,
    Real       *min_value,
    Real       *max_value )
{
    *min_value = get_volume_real_min( volume );
    *max_value = get_volume_real_max( volume );
}

public  Real  get_volume_real_min(
    Volume     volume )
{
    Real   real_min;

    real_min = get_volume_min_voxel( volume );

    if( volume->real_range_set )
        real_min = CONVERT_VOXEL_TO_VALUE( volume, real_min );

    return( real_min );
}

public  Real  get_volume_real_max(
    Volume     volume )
{
    Real   real_max;

    real_max = get_volume_max_voxel( volume );

    if( volume->real_range_set )
        real_max = CONVERT_VOXEL_TO_VALUE( volume, real_max );

    return( real_max );
}

public  void  set_volume_real_range(
    Volume   volume,
    Real     real_min,
    Real     real_max )
{
    Real    min_voxel, max_voxel;


    get_volume_voxel_range( volume, &min_voxel, &max_voxel );

    if( min_voxel < max_voxel )
    {
        volume->real_value_scale = (real_max - real_min) /
                                   (max_voxel - min_voxel);
        volume->real_value_translation = real_min -
                                         min_voxel * volume->real_value_scale;
    }
    else
    {
        volume->real_value_scale = 0.0;
        volume->real_value_translation = real_min;
    }

    volume->real_range_set = TRUE;
}
