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
    Real        min_value,
    Real        max_value )
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

    volume->min_value = min_value;
    volume->max_value = max_value;
    volume->value_scale = 1.0;
    volume->value_translation = 0.0;
    volume->fill_value = 0.0;
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
@NAME       : voxel_is_within_volume
@INPUT      : volume
              voxel_position
@OUTPUT     : 
@RETURNS    : TRUE if voxel is within volume.
@DESCRIPTION: Determines if a voxel position is within the volume.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Boolean  voxel_is_within_volume(
    Volume   volume,
    Real     voxel_position[] )
{
    int      i;
    Boolean  inside;

    inside = TRUE;

    for_less( i, 0, volume->n_dimensions )
    {
        if( voxel_position[i] < -0.5 ||
            voxel_position[i] >= (Real) volume->sizes[i] - 0.5 )
        {
            inside = FALSE;
            break;
        }
    }

    return( inside );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : cube_is_within_volume
@INPUT      : volume
              indices
@OUTPUT     : 
@RETURNS    : TRUE if cube within volume
@DESCRIPTION: Determines if the voxel with integer coordinates is within the
              volume.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Boolean  cube_is_within_volume(
    Volume   volume,
    int      indices[] )
{
    int      i;
    Boolean  inside;

    inside = TRUE;

    for_less( i, 0, volume->n_dimensions )
    {
        if( indices[i] < 0 || indices[i] >= volume->sizes[i] )
        {
            inside = FALSE;
            break;
        }
    }

    return( inside );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_volume_voxel_range
@INPUT      : volume
@OUTPUT     : min_value
              max_value
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
    Real       *min_value,
    Real       *max_value )
{
    *min_value = volume->min_value;
    *max_value = volume->max_value;
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
    *min_value = CONVERT_VOXEL_TO_VALUE( volume, volume->min_value );
    *max_value = CONVERT_VOXEL_TO_VALUE( volume, volume->max_value );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : voxel_contains_value
@INPUT      : volume
              indices
              target_value
@OUTPUT     : 
@RETURNS    : TRUE if voxel contains this value
@DESCRIPTION: Determines if the voxel contains this value, by assuming
              trilinear interpolation between the 8 corner values of this
              voxel.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Boolean  voxel_contains_value(
    Volume   volume,
    int      x,
    int      y,
    int      z,
    Real     target_value )
{               
    Boolean  less, greater;
    int      x_offset, y_offset, z_offset;
    Real     value;

    if( volume->n_dimensions != 3 )
    {
        HANDLE_INTERNAL_ERROR( "voxel_contains_value.\n" );
    }

    less = FALSE;
    greater = FALSE;

    for_less( x_offset, 0, 2 )
    {
        for_less( y_offset, 0, 2 )
        {
            for_less( z_offset, 0, 2 )
            {
                GET_VALUE_3D( value, volume,
                              x + x_offset, y + y_offset, z + z_offset );

                if( value < target_value )
                {
                    if( greater )
                        return( TRUE );
                    less = TRUE;
                }

                if( value > target_value )
                {
                    if( less )
                        return( TRUE );
                    greater = TRUE;
                }
            }
        }
    }

    return( FALSE );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : evaluate_volume_in_world
@INPUT      : volume
              x
              y
              z
              activity_if_mixed
@OUTPUT     : value
              deriv_x
              deriv_y
              deriv_z
@RETURNS    : TRUE if point is within active voxel.
@DESCRIPTION: Takes a world space position and evaluates the value within
              the volume by trilinear interpolation.  If the activities of the
              8 voxels containing this point agree, then that activity is
              returned, if not, then activity_if_mixed is returned.  If
              deriv_x is not a null pointer, then the 3 derivatives are passed
              back.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Boolean   evaluate_volume_in_world(
    Volume         volume,
    Real           x,
    Real           y,
    Real           z,
    int            degrees_continuity,
    Boolean        activity_if_mixed,
    Real           *value,
    Real           *deriv_x,
    Real           *deriv_y,
    Real           *deriv_z,
    Real           *deriv_xx,
    Real           *deriv_xy,
    Real           *deriv_xz,
    Real           *deriv_yy,
    Real           *deriv_yz,
    Real           *deriv_zz )
{
    Boolean   voxel_is_active;
    Real      ignore, dxx, dxy, dxz, dyy, dyz, dzz;
    Real      txx, txy, txz;
    Real      tyx, tyy, tyz;
    Real      tzx, tzy, tzz;

    convert_world_to_voxel( volume, x, y, z, &x, &y, &z );

    voxel_is_active = evaluate_volume( volume, x, y, z, degrees_continuity,
                                       activity_if_mixed,
                                       value, deriv_x, deriv_y, deriv_z,
                                       deriv_xx, deriv_xy, deriv_xz,
                                       deriv_yy, deriv_yz,
                                       deriv_zz );

    if( deriv_x != (Real *) 0 )
    {
        convert_voxel_normal_vector_to_world( volume,
                                              *deriv_x, *deriv_y, *deriv_z,
                                              deriv_x, deriv_y, deriv_z );
    }

    if( deriv_xx != (Real *) 0 )
    {
        dxx = *deriv_xx;
        dxy = *deriv_xy;
        dxz = *deriv_xz;
        dyy = *deriv_yy;
        dyz = *deriv_yz;
        dzz = *deriv_zz;
        convert_voxel_normal_vector_to_world( volume,
                                              dxx, dxy, dxz,
                                              &txx, &txy, &txz );
        convert_voxel_normal_vector_to_world( volume,
                                              dxy, dyy, dyz,
                                              &tyx, &tyy, &tyz );
        convert_voxel_normal_vector_to_world( volume,
                                              dxz, dyz, dzz,
                                              &tzx, &tzy, &tzz );

        convert_voxel_normal_vector_to_world( volume,
                                              txx, tyx, tzx,
                                              deriv_xx, &ignore, &ignore );
        convert_voxel_normal_vector_to_world( volume,
                                              txy, tyy, tzy,
                                              deriv_xy, deriv_yy, &ignore );
        convert_voxel_normal_vector_to_world( volume,
                                              txz, tyz, tzz,
                                              deriv_xz, deriv_yz, deriv_zz );
    }

    return( voxel_is_active );
}

typedef enum { NONE_ACTIVE, SOME_ACTIVE, ALL_ACTIVE } Group_activity;

private  Group_activity   triconstant_interpolate_volume(
    Volume         volume,
    Real           x,
    Real           y,
    Real           z,
    Real           *value,
    Real           *deriv_x,
    Real           *deriv_y,
    Real           *deriv_z )
{
    Group_activity     activity;
    int                i, j, k;
    Real               prev, next, dx, dy, dz, voxel_value;
    int                nx, ny, nz;

    nx = volume->sizes[X];
    ny = volume->sizes[Y];
    nz = volume->sizes[Z];

    i = ROUND( x );
    if( i == nx )
        i = nx-1;
    j = ROUND( y );
    if( j == ny )
        j = ny-1;
    k = ROUND( z );
    if( k == nz )
        k = nz-1;

    GET_VALUE_3D( voxel_value, volume, i, j, k );

    if( value != (Real *) 0 )
        *value = voxel_value;

    if( get_voxel_activity_flag( volume, i, j, k ) )
        activity = ALL_ACTIVE;
    else
        activity = NONE_ACTIVE;

    if( deriv_x != (Real *) NULL )
    {
        /* --- get derivative wrt x */

        dx = 0;
        if( i == 0 )
            prev = voxel_value;
        else
        {
            GET_VALUE_3D( prev, volume, i-1, j, k );
            ++dx;
        }

        if( i == nx-1 )
            next = voxel_value;
        else
        {
            GET_VALUE_3D( next, volume, i+1, j, k );
            ++dx;
        }

        if( dx == 0 )
            *deriv_x = 0.0;
        else
            *deriv_x = (next - prev) / (Real) dx;

        /* --- get derivative wrt y */

        dy = 0;
        if( j == 0 )
            prev = voxel_value;
        else
        {
            GET_VALUE_3D( prev, volume, i, j-1, k );
            ++dy;
        }

        if( j == ny-1 )
            next = voxel_value;
        else
        {
            GET_VALUE_3D( next, volume, i, j+1, k );
            ++dy;
        }

        if( dy == 0 )
            *deriv_y = 0.0;
        else
            *deriv_y = (next - prev) / (Real) dy;

        /* --- get derivative wrt z */

        dz = 0;
        if( k == 0 )
            prev = voxel_value;
        else
        {
            GET_VALUE_3D( prev, volume, i, j, k-1 );
            ++dz;
        }

        if( k == nz-1 )
            next = voxel_value;
        else
        {
            GET_VALUE_3D( next, volume, i, j, k+1 );
            ++dz;
        }

        if( dz == 0 )
            *deriv_z = 0.0;
        else
            *deriv_z = (next - prev) / (Real) dz;
    }

    return( activity );
}

private  Group_activity   trilinear_interpolate_volume(
    Volume         volume,
    Real           x,
    Real           y,
    Real           z,
    Real           *value,
    Real           *deriv_x,
    Real           *deriv_y,
    Real           *deriv_z )
{
    Group_activity     activity;
    int                n_inactive;
    int                i, j, k;
    Real               u, v, w;
    Real               c000, c001, c010, c011, c100, c101, c110, c111;
    Real               c00, c01, c10, c11;
    Real               c0, c1;
    Real               du00, du01, du10, du11, du0, du1;
    Real               dv0, dv1;
    int                nx, ny, nz;

    nx = volume->sizes[X];
    ny = volume->sizes[Y];
    nz = volume->sizes[Z];

    if( x == (Real) (nx-1) )
    {
        i = nx-2;
        u = 1.0;
    }
    else
    {
        i = (int) x;
        u = FRACTION( x );
    }

    if( y == (Real) (ny-1) )
    {
        j = ny-2;
        v = 1.0;
    }
    else
    {
        j = (int) y;
        v = FRACTION( y );
    }

    if( z == (Real) (nz-1) )
    {
        k = nz-2;
        w = 1.0;
    }
    else
    {
        k = (int) z;
        w = FRACTION( z );
    }

    n_inactive = 0;

    GET_VALUE_3D( c000, volume, i,   j,   k );
    GET_VALUE_3D( c001, volume, i,   j,   k+1 );
    GET_VALUE_3D( c010, volume, i,   j+1, k );
    GET_VALUE_3D( c011, volume, i,   j+1, k+1 );
    GET_VALUE_3D( c100, volume, i+1, j,   k );
    GET_VALUE_3D( c101, volume, i+1, j,   k+1 );
    GET_VALUE_3D( c110, volume, i+1, j+1, k );
    GET_VALUE_3D( c111, volume, i+1, j+1, k+1 );

    if( !get_voxel_activity_flag( volume, i  , j  , k ) )
        ++n_inactive;
    if( !get_voxel_activity_flag( volume, i  , j  , k+1 ) )
        ++n_inactive;
    if( !get_voxel_activity_flag( volume, i  , j+1, k ) )
            ++n_inactive;
    if( !get_voxel_activity_flag( volume, i  , j+1, k+1 ) )
        ++n_inactive;
    if( !get_voxel_activity_flag( volume, i+1, j  , k ) )
        ++n_inactive;
    if( !get_voxel_activity_flag( volume, i+1, j  , k+1 ) )
        ++n_inactive;
    if( !get_voxel_activity_flag( volume, i+1, j+1, k ) )
        ++n_inactive;
    if( !get_voxel_activity_flag( volume, i+1, j+1, k+1 ) )
        ++n_inactive;
    
    du00 = c100 - c000;
    du01 = c101 - c001;
    du10 = c110 - c010;
    du11 = c111 - c011;

    c00 = c000 + u * du00;
    c01 = c001 + u * du01;
    c10 = c010 + u * du10;
    c11 = c011 + u * du11;

    dv0 = c10 - c00;
    dv1 = c11 - c01;

    c0 = c00 + v * dv0;
    c1 = c01 + v * dv1;

    if( value != (Real *) 0 )
        *value = INTERPOLATE( w, c0, c1 );

    if( deriv_x != (Real *) 0 )
    {
        du0 = INTERPOLATE( v, du00, du10 );
        du1 = INTERPOLATE( v, du01, du11 );

        *deriv_x = INTERPOLATE( w, du0, du1 );
        *deriv_y = INTERPOLATE( w, dv0, dv1 );
        *deriv_z = (c1 - c0);
    }

    if( n_inactive == 0 )
        activity = ALL_ACTIVE;
    else if( n_inactive == 8 )
        activity = NONE_ACTIVE;
    else
        activity = SOME_ACTIVE;

    return( activity );
}

private  Group_activity   triquadratic_interpolate_volume(
    Volume         volume,
    Real           x,
    Real           y,
    Real           z,
    Real           *value,
    Real           *deriv_x,
    Real           *deriv_y,
    Real           *deriv_z,
    Real           *deriv_xx,
    Real           *deriv_xy,
    Real           *deriv_xz,
    Real           *deriv_yy,
    Real           *deriv_yz,
    Real           *deriv_zz )
{
    Group_activity     activity;
    int                n_inactive;
    int                i, j, k, di, dj, dk;
    int                nx, ny, nz;
    Real               tx, ty, tz, u, v, w;
    Real               c000, c001, c002, c010, c011, c012, c020, c021, c022;
    Real               c100, c101, c102, c110, c111, c112, c120, c121, c122;
    Real               c200, c201, c202, c210, c211, c212, c220, c221, c222;


    nx = volume->sizes[X];
    ny = volume->sizes[Y];
    nz = volume->sizes[Z];

    tx = x + 0.5;
    ty = y + 0.5;
    tz = z + 0.5;

    if( x == (Real) nx - 1.5 )
    {
        i = nx-2;
        u = 1.0;
    }
    else
    {
        i = (int) tx;
        u = FRACTION( tx );
    }

    if( y == (Real) ny - 1.5 )
    {
        j = ny-2;
        v = 1.0;
    }
    else
    {
        j = (int) ty;
        v = FRACTION( ty );
    }

    if( z == (Real) nz - 1.5 )
    {
        k = nz-2;
        w = 1.0;
    }
    else
    {
        k = (int) tz;
        w = FRACTION( tz );
    }

    GET_VALUE_3D( c000, volume, i-1, j-1, k-1 );
    GET_VALUE_3D( c001, volume, i-1, j-1, k+0 );
    GET_VALUE_3D( c002, volume, i-1, j-1, k+1 );
    GET_VALUE_3D( c010, volume, i-1, j+0, k-1 );
    GET_VALUE_3D( c011, volume, i-1, j+0, k+0 );
    GET_VALUE_3D( c012, volume, i-1, j+0, k+1 );
    GET_VALUE_3D( c020, volume, i-1, j+1, k-1 );
    GET_VALUE_3D( c021, volume, i-1, j+1, k+0 );
    GET_VALUE_3D( c022, volume, i-1, j+1, k+1 );

    GET_VALUE_3D( c100, volume, i+0, j-1, k-1 );
    GET_VALUE_3D( c101, volume, i+0, j-1, k+0 );
    GET_VALUE_3D( c102, volume, i+0, j-1, k+1 );
    GET_VALUE_3D( c110, volume, i+0, j+0, k-1 );
    GET_VALUE_3D( c111, volume, i+0, j+0, k+0 );
    GET_VALUE_3D( c112, volume, i+0, j+0, k+1 );
    GET_VALUE_3D( c120, volume, i+0, j+1, k-1 );
    GET_VALUE_3D( c121, volume, i+0, j+1, k+0 );
    GET_VALUE_3D( c122, volume, i+0, j+1, k+1 );

    GET_VALUE_3D( c200, volume, i+1, j-1, k-1 );
    GET_VALUE_3D( c201, volume, i+1, j-1, k+0 );
    GET_VALUE_3D( c202, volume, i+1, j-1, k+1 );
    GET_VALUE_3D( c210, volume, i+1, j+0, k-1 );
    GET_VALUE_3D( c211, volume, i+1, j+0, k+0 );
    GET_VALUE_3D( c212, volume, i+1, j+0, k+1 );
    GET_VALUE_3D( c220, volume, i+1, j+1, k-1 );
    GET_VALUE_3D( c221, volume, i+1, j+1, k+0 );
    GET_VALUE_3D( c222, volume, i+1, j+1, k+1 );

    n_inactive = 0;

    for_inclusive( di, -1, 1 )
    {
        for_inclusive( dj, -1, 1 )
        {
            for_inclusive( dk, -1, 1 )
            {
                if( !get_voxel_activity_flag( volume, i+di, j+dj, k+dk ) )
                    ++n_inactive;
            }
        }
    }

    if( value != (Real *) 0 )
    {
        QUADRATIC_TRIVAR( c, u, v, w, *value );
    }

    if( deriv_x != (Real *) 0 )
    {
        QUADRATIC_TRIVAR_DERIV( c, u, v, w, *deriv_x, *deriv_y, *deriv_z );
    }

    if( deriv_xx != (Real *) 0 )
    {
        QUADRATIC_TRIVAR_DERIV2( c, u, v, w, *deriv_xx, *deriv_xy, *deriv_xz,
                                 *deriv_yy, *deriv_yz, *deriv_zz );
    }

    if( n_inactive == 0 )
        activity = ALL_ACTIVE;
    else if( n_inactive == 27 )
        activity = NONE_ACTIVE;
    else
        activity = SOME_ACTIVE;

    return( activity );
}

private  Group_activity   tricubic_interpolate_volume(
    Volume         volume,
    Real           x,
    Real           y,
    Real           z,
    Real           *value,
    Real           *deriv_x,
    Real           *deriv_y,
    Real           *deriv_z,
    Real           *deriv_xx,
    Real           *deriv_xy,
    Real           *deriv_xz,
    Real           *deriv_yy,
    Real           *deriv_yz,
    Real           *deriv_zz )
{
    Group_activity     activity;
    int                n_inactive;
    int                i, j, k, di, dj, dk;
    int                nx, ny, nz;
    Real               u, v, w;
    Real               c000, c001, c002, c003, c010, c011, c012, c013;
    Real               c020, c021, c022, c023, c030, c031, c032, c033;
    Real               c100, c101, c102, c103, c110, c111, c112, c113;
    Real               c120, c121, c122, c123, c130, c131, c132, c133;
    Real               c200, c201, c202, c203, c210, c211, c212, c213;
    Real               c220, c221, c222, c223, c230, c231, c232, c233;
    Real               c300, c301, c302, c303, c310, c311, c312, c313;
    Real               c320, c321, c322, c323, c330, c331, c332, c333;


    nx = volume->sizes[X];
    ny = volume->sizes[Y];
    nz = volume->sizes[Z];

    if( x == (Real) nx - 1.5 )
    {
        i = nx-2;
        u = 1.0;
    }
    else
    {
        i = (int) x;
        u = FRACTION( x );
    }

    if( y == (Real) ny - 1.5 )
    {
        j = ny-2;
        v = 1.0;
    }
    else
    {
        j = (int) y;
        v = FRACTION( y );
    }

    if( z == (Real) nz - 1.5 )
    {
        k = nz-2;
        w = 1.0;
    }
    else
    {
        k = (int) z;
        w = FRACTION( z );
    }

    GET_VALUE_3D( c000, volume, i-1, j-1, k-1 );
    GET_VALUE_3D( c001, volume, i-1, j-1, k+0 );
    GET_VALUE_3D( c002, volume, i-1, j-1, k+1 );
    GET_VALUE_3D( c003, volume, i-1, j-1, k+2 );
    GET_VALUE_3D( c010, volume, i-1, j+0, k-1 );
    GET_VALUE_3D( c011, volume, i-1, j+0, k+0 );
    GET_VALUE_3D( c012, volume, i-1, j+0, k+1 );
    GET_VALUE_3D( c013, volume, i-1, j+0, k+2 );
    GET_VALUE_3D( c020, volume, i-1, j+1, k-1 );
    GET_VALUE_3D( c021, volume, i-1, j+1, k+0 );
    GET_VALUE_3D( c022, volume, i-1, j+1, k+1 );
    GET_VALUE_3D( c023, volume, i-1, j+1, k+2 );
    GET_VALUE_3D( c030, volume, i-1, j+2, k-1 );
    GET_VALUE_3D( c031, volume, i-1, j+2, k+0 );
    GET_VALUE_3D( c032, volume, i-1, j+2, k+1 );
    GET_VALUE_3D( c033, volume, i-1, j+2, k+2 );

    GET_VALUE_3D( c100, volume, i+0, j-1, k-1 );
    GET_VALUE_3D( c101, volume, i+0, j-1, k+0 );
    GET_VALUE_3D( c102, volume, i+0, j-1, k+1 );
    GET_VALUE_3D( c103, volume, i+0, j-1, k+2 );
    GET_VALUE_3D( c110, volume, i+0, j+0, k-1 );
    GET_VALUE_3D( c111, volume, i+0, j+0, k+0 );
    GET_VALUE_3D( c112, volume, i+0, j+0, k+1 );
    GET_VALUE_3D( c113, volume, i+0, j+0, k+2 );
    GET_VALUE_3D( c120, volume, i+0, j+1, k-1 );
    GET_VALUE_3D( c121, volume, i+0, j+1, k+0 );
    GET_VALUE_3D( c122, volume, i+0, j+1, k+1 );
    GET_VALUE_3D( c123, volume, i+0, j+1, k+2 );
    GET_VALUE_3D( c130, volume, i+0, j+2, k-1 );
    GET_VALUE_3D( c131, volume, i+0, j+2, k+0 );
    GET_VALUE_3D( c132, volume, i+0, j+2, k+1 );
    GET_VALUE_3D( c133, volume, i+0, j+2, k+2 );

    GET_VALUE_3D( c200, volume, i+1, j-1, k-1 );
    GET_VALUE_3D( c201, volume, i+1, j-1, k+0 );
    GET_VALUE_3D( c202, volume, i+1, j-1, k+1 );
    GET_VALUE_3D( c203, volume, i+1, j-1, k+2 );
    GET_VALUE_3D( c210, volume, i+1, j+0, k-1 );
    GET_VALUE_3D( c211, volume, i+1, j+0, k+0 );
    GET_VALUE_3D( c212, volume, i+1, j+0, k+1 );
    GET_VALUE_3D( c213, volume, i+1, j+0, k+2 );
    GET_VALUE_3D( c220, volume, i+1, j+1, k-1 );
    GET_VALUE_3D( c221, volume, i+1, j+1, k+0 );
    GET_VALUE_3D( c222, volume, i+1, j+1, k+1 );
    GET_VALUE_3D( c223, volume, i+1, j+1, k+2 );
    GET_VALUE_3D( c230, volume, i+1, j+2, k-1 );
    GET_VALUE_3D( c231, volume, i+1, j+2, k+0 );
    GET_VALUE_3D( c232, volume, i+1, j+2, k+1 );
    GET_VALUE_3D( c233, volume, i+1, j+2, k+2 );

    GET_VALUE_3D( c300, volume, i+2, j-1, k-1 );
    GET_VALUE_3D( c301, volume, i+2, j-1, k+0 );
    GET_VALUE_3D( c302, volume, i+2, j-1, k+1 );
    GET_VALUE_3D( c303, volume, i+2, j-1, k+2 );
    GET_VALUE_3D( c310, volume, i+2, j+0, k-1 );
    GET_VALUE_3D( c311, volume, i+2, j+0, k+0 );
    GET_VALUE_3D( c312, volume, i+2, j+0, k+1 );
    GET_VALUE_3D( c313, volume, i+2, j+0, k+2 );
    GET_VALUE_3D( c320, volume, i+2, j+1, k-1 );
    GET_VALUE_3D( c321, volume, i+2, j+1, k+0 );
    GET_VALUE_3D( c322, volume, i+2, j+1, k+1 );
    GET_VALUE_3D( c323, volume, i+2, j+1, k+2 );
    GET_VALUE_3D( c330, volume, i+2, j+2, k-1 );
    GET_VALUE_3D( c331, volume, i+2, j+2, k+0 );
    GET_VALUE_3D( c332, volume, i+2, j+2, k+1 );
    GET_VALUE_3D( c333, volume, i+2, j+2, k+2 );

    n_inactive = 0;

    for_inclusive( di, -1, 2 )
    {
        for_inclusive( dj, -1, 2 )
        {
            for_inclusive( dk, -1, 2 )
            {
                if( !get_voxel_activity_flag( volume, i+di, j+dj, k+dk ) )
                    ++n_inactive;
            }
        }
    }

    if( value != (Real *) 0 )
    {
        CUBIC_TRIVAR( c, u, v, w, *value );
    }

    if( deriv_x != (Real *) 0 )
    {
        CUBIC_TRIVAR_DERIV( c, u, v, w, *deriv_x, *deriv_y, *deriv_z );
    }

    if( deriv_xx != (Real *) 0 )
    {
        CUBIC_TRIVAR_DERIV2( c, u, v, w, *deriv_xx, *deriv_xy, *deriv_xz,
                             *deriv_yy, *deriv_yz, *deriv_zz );
    }

    if( n_inactive == 0 )
        activity = ALL_ACTIVE;
    else if( n_inactive == 64 )
        activity = NONE_ACTIVE;
    else
        activity = SOME_ACTIVE;

    return( activity );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : evaluate_volume
@INPUT      : volume
              x
              y
              z
              activity_if_mixed
@OUTPUT     : value
              deriv_x
              deriv_y
              deriv_z
@RETURNS    : TRUE if point is within active voxel.
@DESCRIPTION: Takes a voxel space position and evaluates the value within
              the volume by trilinear interpolation.  If the activities of the
              8 voxels containing this point agree, then that activity is
              returned, if not, then activity_if_mixed is returned.  If
              deriv_x is not a null pointer, then the 3 derivatives are passed
              back.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Boolean   evaluate_volume(
    Volume         volume,
    Real           x,
    Real           y,
    Real           z,
    int            degrees_continuity,
    Boolean        activity_if_mixed,
    Real           *value,
    Real           *deriv_x,
    Real           *deriv_y,
    Real           *deriv_z,
    Real           *deriv_xx,
    Real           *deriv_xy,
    Real           *deriv_xz,
    Real           *deriv_yy,
    Real           *deriv_yz,
    Real           *deriv_zz )
{
    Group_activity   activity;
    Boolean          voxel_is_active;
    int              nx, ny, nz;

    nx = volume->sizes[X];
    ny = volume->sizes[Y];
    nz = volume->sizes[Z];

    if( x < -0.5 || x > (Real) nx - 0.5 ||
        y < -0.5 || y > (Real) ny - 0.5 ||
        z < -0.5 || z > (Real) nz - 0.5 )
    {
        *value = volume->fill_value;
        if( deriv_x != (Real *) NULL )
        {
            *deriv_x = 0.0;
            *deriv_y = 0.0;
            *deriv_z = 0.0;
        }
        if( deriv_xx != (Real *) NULL )
        {
            *deriv_xx = 0.0;
            *deriv_xy = 0.0;
            *deriv_xz = 0.0;
            *deriv_yy = 0.0;
            *deriv_yz = 0.0;
            *deriv_zz = 0.0;
        }
        return( FALSE );
    }

    if( x < (Real) degrees_continuity * 0.5 ||
        x > (Real) (nx-1) - (Real) degrees_continuity * 0.5 ||
        y < (Real) degrees_continuity * 0.5 ||
        y > (Real) (ny-1) - (Real) degrees_continuity * 0.5 ||
        z < (Real) degrees_continuity * 0.5 ||
        z > (Real) (nz-1) - (Real) degrees_continuity * 0.5 )
    {
        degrees_continuity = -1;
    }

    if( degrees_continuity < 1 && deriv_xx != (Real *) NULL )
    {
        *deriv_xx = 0.0;
        *deriv_xy = 0.0;
        *deriv_xz = 0.0;
        *deriv_yy = 0.0;
        *deriv_yz = 0.0;
        *deriv_zz = 0.0;
    }

    switch( degrees_continuity )
    {
    case -1:
        activity = triconstant_interpolate_volume( volume, x, y, z, value,
                                                   deriv_x, deriv_y, deriv_z );
        break;

    case 0:
        activity = trilinear_interpolate_volume( volume, x, y, z, value,
                                                 deriv_x, deriv_y, deriv_z );
        break;

    case 1:
        activity = triquadratic_interpolate_volume( volume, x, y, z, value,
                                               deriv_x, deriv_y, deriv_z,
                                               deriv_xx, deriv_xy, deriv_xz,
                                               deriv_yy, deriv_yz, deriv_zz );
        break;

    case 2:
        activity = tricubic_interpolate_volume( volume, x, y, z, value,
                                                deriv_x, deriv_y, deriv_z,
                                                deriv_xx, deriv_xy, deriv_xz,
                                                deriv_yy, deriv_yz, deriv_zz );
        break;

    default:
        HANDLE_INTERNAL_ERROR( "evaluate_volume: invalid continuity" );
    }

    if( activity == ALL_ACTIVE )
        voxel_is_active = TRUE;
    else if( activity == NONE_ACTIVE )
        voxel_is_active = FALSE;
    else
        voxel_is_active = activity_if_mixed;

    return( voxel_is_active );
}
