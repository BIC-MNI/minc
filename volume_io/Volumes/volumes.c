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
              later use the volume, you must call either set_volume_sizes()
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
    Boolean     signed_flag,
    Real        min_voxel,
    Real        max_voxel )
{
    int             i, sizes[MAX_DIMENSIONS];
    Status          status;
    volume_struct   *volume;

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

    volume->min_voxel = min_voxel;
    volume->max_voxel = max_voxel;
    volume->value_scale = 1.0;
    volume->value_translation = 0.0;
    volume->labels = (unsigned char ***) NULL;

    for_less( i, 0, n_dimensions )
    {
        sizes[i] = 0;
        volume->separation[i] = 1.0;

        if( dimension_names != (String *) NULL )
            (void) strcpy( volume->dimension_names[i], dimension_names[i] );
        else
            (void) strcpy( volume->dimension_names[i],
                           default_dimension_names[n_dimensions-1][i] );
    }

    volume->data_type = NO_DATA_TYPE;
    set_volume_size( volume, nc_data_type, signed_flag, sizes );

    make_identity_transform( &volume->world_to_voxel_transform );
    make_identity_transform( &volume->voxel_to_world_transform );

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
        separations[i] = volume->separation[i];
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

    transform_point( &volume->voxel_to_world_transform,
                     x_voxel, y_voxel, z_voxel, x_world, y_world, z_world );
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
    /* transform vector by transpose of inverse transformation */

    *x_world = Transform_elem(volume->world_to_voxel_transform,0,0) * x_voxel+
               Transform_elem(volume->world_to_voxel_transform,1,0) * y_voxel+
               Transform_elem(volume->world_to_voxel_transform,2,0) * z_voxel;
    *y_world = Transform_elem(volume->world_to_voxel_transform,0,1) * x_voxel+
               Transform_elem(volume->world_to_voxel_transform,1,1) * y_voxel+
               Transform_elem(volume->world_to_voxel_transform,2,1) * z_voxel;
    *z_world = Transform_elem(volume->world_to_voxel_transform,0,2) * x_voxel+
               Transform_elem(volume->world_to_voxel_transform,1,2) * y_voxel+
               Transform_elem(volume->world_to_voxel_transform,2,2) * z_voxel;
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

    transform_point( &volume->world_to_voxel_transform,
                     x_world, y_world, z_world, x_voxel, y_voxel, z_voxel );
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
    *min_voxel = volume->min_voxel;
    *max_voxel = volume->max_voxel;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_volume_range
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

public  void  get_volume_range(
    Volume     volume,
    Real       *min_value,
    Real       *max_value )
{
    *min_value = CONVERT_VOXEL_TO_VALUE( volume, volume->min_voxel );
    *max_value = CONVERT_VOXEL_TO_VALUE( volume, volume->max_voxel );
}
