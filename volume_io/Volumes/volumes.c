#include  <volume_io.h>
#include  <limits.h>
#undef FLT_DIG
#undef DBL_DIG
#undef DBL_MIN
#undef DBL_MAX
#include  <float.h>

char   *XYZ_dimension_names[] = { MIxspace, MIyspace, MIzspace };

private  char  *default_dimension_names[MAX_DIMENSIONS][MAX_DIMENSIONS] =
{
    { MIxspace },
    { MIyspace, MIxspace },
    { MIzspace, MIyspace, MIxspace },
    { MItime, MIzspace, MIyspace, MIxspace },
    { "", MItime, MIzspace, MIyspace, MIxspace }
};

public  char  **get_default_dim_names(
    int    n_dimensions )
{
    return( default_dimension_names[n_dimensions-1] );
}

private  char  *get_dim_name(
    int   axis )
{
    switch( axis )
    {
    case X:  return( MIxspace );
    case Y:  return( MIyspace );
    case Z:  return( MIzspace );
    default:  HANDLE_INTERNAL_ERROR( "get_dim_name" ); break;
    }

    return( (char *) 0 );
}

public  BOOLEAN  convert_dim_name_to_axis(
    char    name[],
    int     *axis )
{
    *axis = -1;

    if( strcmp( name, MIxspace ) == 0 )
        *axis = X;
    else if( strcmp( name, MIyspace ) == 0 )
        *axis = Y;
    else if( strcmp( name, MIzspace ) == 0 )
        *axis = Z;

    return( *axis >= 0 );
}

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
    char        *dimension_names[],
    nc_type     nc_data_type,
    BOOLEAN     signed_flag,
    Real        voxel_min,
    Real        voxel_max )
{
    int             i, axis, sizes[MAX_DIMENSIONS];
    Status          status;
    char            *name;
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

    volume->real_range_set = FALSE;
    volume->real_value_scale = 1.0;
    volume->real_value_translation = 0.0;

    for_less( i, 0, N_DIMENSIONS )
    {
        volume->world_space_for_translation_voxel[i] = 0.0;
        volume->spatial_axes[i] = -1;
    }

    for_less( i, 0, n_dimensions )
    {
        volume->translation_voxel[i] = 0.0;
        volume->separations[i] = 1.0;
        volume->direction_cosines[i][X] = 0.0;
        volume->direction_cosines[i][Y] = 0.0;
        volume->direction_cosines[i][Z] = 0.0;

        sizes[i] = 0;

        if( dimension_names != (char **) NULL )
            name = dimension_names[i];
        else
            name = default_dimension_names[n_dimensions-1][i];

        if( convert_dim_name_to_axis( name, &axis ) )
        {
            volume->spatial_axes[axis] = i;
            volume->direction_cosines[i][axis] = 1.0;
        }

        ALLOC( volume->dimension_names[i], strlen( name ) + 1 );
        (void) strcpy( volume->dimension_names[i], name );
    }

    volume->data_type = NO_DATA_TYPE;

    set_volume_type( volume, nc_data_type, signed_flag, voxel_min, voxel_max );
    set_volume_sizes( volume, sizes );

    make_identity_transform( &identity );
    create_linear_transform( &volume->voxel_to_world_transform, &identity );

    return( volume );
}

public  void  set_volume_type(
    Volume       volume,
    nc_type      nc_data_type,
    BOOLEAN      signed_flag,
    Real         voxel_min,
    Real         voxel_max )
{
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

        set_volume_voxel_range( volume, voxel_min, voxel_max );
    }
}

public  nc_type  get_volume_nc_data_type(
    Volume       volume,
    BOOLEAN      *signed_flag )
{
    if( signed_flag != (BOOLEAN *) NULL )
        *signed_flag = volume->signed_flag;
    return( volume->nc_data_type );
}

public  Data_types  get_volume_data_type(
    Volume       volume )
{
    return( volume->data_type );
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
        free_volume_data( volume );

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

public  void  free_volume_data(
    Volume   volume )
{
    void   *ptr, **ptr2, ***ptr3, ****ptr4, *****ptr5;

    if( volume->data == (void *) NULL )
    {
        print( "Warning: cannot free NULL volume data.\n" );
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
    int   d;

    if( volume == (Volume) NULL )
    {
        print( "delete_volume():  cannot delete a null volume.\n" );
        return;
    }

    if( volume->data != (void *) NULL )
        free_volume_data( volume );

    delete_general_transform( &volume->voxel_to_world_transform );

    for_less( d, 0, volume->n_dimensions )
        FREE( volume->dimension_names[d] );

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

public  void  set_volume_sizes(
    Volume       volume,
    int          sizes[] )
{
    int             i;

    for_less( i, 0, volume->n_dimensions )
        volume->sizes[i] = sizes[i];
}

public  int  get_volume_total_n_voxels(
    Volume    volume )
{
    int   n, i, sizes[MAX_DIMENSIONS];

    n = 1;

    get_volume_sizes( volume, sizes );

    for_less( i, 0, volume->n_dimensions )
        n *= sizes[i];

    return( n );
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

public  void  compute_world_transform(
    int                 spatial_axes[N_DIMENSIONS],
    Real                separations[],
    Real                translation_voxel[],
    Real                world_space_for_translation_voxel[N_DIMENSIONS],
    Real                direction_cosines[][N_DIMENSIONS],
    General_transform   *world_transform )
{
    Transform                transform;
    Real                     separations_3D[N_DIMENSIONS];
    Vector                   directions[N_DIMENSIONS];
    Point                    point;
    Real                     x_trans, y_trans, z_trans;
    Real                     voxel[N_DIMENSIONS];
    int                      c, axis, n_axes, axis_list[N_DIMENSIONS];

    n_axes = 0;

    for_less( c, 0, N_DIMENSIONS )
    {
        axis = spatial_axes[c];
        if( axis >= 0 )
        {
            separations_3D[c] = separations[axis];
            voxel[c] = translation_voxel[axis];
            fill_Vector( directions[c],
                         direction_cosines[axis][X],
                         direction_cosines[axis][Y],
                         direction_cosines[axis][Z]);

            axis_list[n_axes] = c;
            ++n_axes;
        }
        else
        {
            separations_3D[c] = 1.0;
            voxel[c] = 0.0;
        }
    }

    if( n_axes == 0 )
    {
        print( "error compute_world_transform:  no axes.\n" );
        return;
    }

    if( n_axes == 1 )
    {
        create_two_orthogonal_vectors( &directions[axis_list[0]],
                            &directions[(axis_list[0]+1) % N_DIMENSIONS],
                            &directions[(axis_list[0]+2) % N_DIMENSIONS] );
    }
    else if( n_axes == 2 )
    {
        axis = N_DIMENSIONS - axis_list[0] - axis_list[1];

        create_orthogonal_vector( &directions[(axis+1) % N_DIMENSIONS],
                                  &directions[(axis+2) % N_DIMENSIONS],
                                  &directions[axis] );
    }

    if( EQUAL_VECTORS( directions[0], directions[1] ) ||
        EQUAL_VECTORS( directions[1], directions[2] ) ||
        EQUAL_VECTORS( directions[2], directions[0] ) )
    {
        fill_Vector( directions[0], 1.0, 0.0, 0.0 );
        fill_Vector( directions[1], 0.0, 1.0, 0.0 );
        fill_Vector( directions[2], 0.0, 0.0, 1.0 );
    }

    for_less( c, 0, N_DIMENSIONS )
    {
        NORMALIZE_VECTOR( directions[c], directions[c] );
        SCALE_VECTOR( directions[c], directions[c], separations_3D[c] );
    }

    fill_Point( point, 0.0, 0.0, 0.0 );

    make_change_to_bases_transform( &point,
              &directions[X], &directions[Y], &directions[Z], &transform );

    transform_point( &transform, voxel[X], voxel[Y], voxel[Z],
                     &x_trans, &y_trans, &z_trans );

    fill_Point( point, world_space_for_translation_voxel[X] - x_trans,
                       world_space_for_translation_voxel[Y] - y_trans,
                       world_space_for_translation_voxel[Z] - z_trans );

    set_transform_origin( &transform, &point );

    create_linear_transform( world_transform, &transform );
}

private  void  recompute_world_transform(
    Volume  volume )
{
    General_transform        world_transform;
    Real                     separations[MAX_DIMENSIONS];

    get_volume_separations( volume, separations );

    compute_world_transform( volume->spatial_axes, separations,
                             volume->translation_voxel,
                             volume->world_space_for_translation_voxel,
                             volume->direction_cosines,
                             &world_transform );

    set_voxel_to_world_transform( volume, &world_transform );
}

public  char  **get_volume_dimension_names(
    Volume   volume )
{
    int    i;
    char   **names;

    ALLOC2D( names, get_volume_n_dimensions(volume), MAX_STRING_LENGTH+1 );

    for_less( i, 0, get_volume_n_dimensions(volume) )
        (void) strcpy( names[i], volume->dimension_names[i] );

    for_less( i, 0, N_DIMENSIONS )
    {
        if( volume->spatial_axes[i] >= 0 )
            (void) strcpy( names[volume->spatial_axes[i]], get_dim_name( i ) );
    }

    return( names );
}

public  void  delete_dimension_names(
    char   **dimension_names )
{
    FREE2D( dimension_names );
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

    for_less( c, 0, volume->n_dimensions )
        volume->translation_voxel[c] = voxel[c];

    for_less( c, 0, N_DIMENSIONS )
        volume->world_space_for_translation_voxel[c] =
                               world_space_voxel_maps_to[c];

    recompute_world_transform( volume );
}

public  void  get_volume_translation(
    Volume  volume,
    Real    voxel[],
    Real    world_space_voxel_maps_to[] )
{
    int  c;

    for_less( c, 0, volume->n_dimensions )
        voxel[c] = volume->translation_voxel[c];

    for_less( c, 0, N_DIMENSIONS )
        world_space_voxel_maps_to[c] =
                 volume->world_space_for_translation_voxel[c];
}

public  void  set_volume_direction_cosine(
    Volume   volume,
    int      axis,
    Real     dir[] )
{
    Real   len;

    if( axis < 0 || axis >= get_volume_n_dimensions(volume) )
    {
        print(
         "set_volume_direction_cosine:  cannot set dir cosine for axis %d\n",
          axis );
        return;
    }

    volume->direction_cosines[axis][X] = dir[X];
    volume->direction_cosines[axis][Y] = dir[Y];
    volume->direction_cosines[axis][Z] = dir[Z];

    len = dir[X] * dir[X] + dir[Y] * dir[Y] + dir[Z] * dir[Z];

    if( len == 0.0 )
    {
        print( "Warning: zero length direction cosine in set_volume_direction_cosine()\n" );
        return;
    }

    if( len != 1.0 )
    {
        len = sqrt( len );
        volume->direction_cosines[axis][X] /= len;
        volume->direction_cosines[axis][Y] /= len;
        volume->direction_cosines[axis][Z] /= len;
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
    Real     voxel[],
    Real     *x_world,
    Real     *y_world,
    Real     *z_world )
{
    Real   x_voxel, y_voxel, z_voxel;

    if( volume->spatial_axes[0] >= 0 )
        x_voxel = voxel[volume->spatial_axes[0]];
    else
        x_voxel = 0.0;

    if( volume->spatial_axes[1] >= 0 )
        y_voxel = voxel[volume->spatial_axes[1]];
    else
        y_voxel = 0.0;

    if( volume->spatial_axes[2] >= 0 )
        z_voxel = voxel[volume->spatial_axes[2]];
    else
        z_voxel = 0.0;

    /* apply linear transform */

    general_transform_point( &volume->voxel_to_world_transform,
                             x_voxel, y_voxel, z_voxel,
                             x_world, y_world, z_world );
}

public  void  convert_3D_voxel_to_world(
    Volume   volume,
    Real     x_voxel,
    Real     y_voxel,
    Real     z_voxel,
    Real     *x_world,
    Real     *y_world,
    Real     *z_world )
{
    Real   voxel[MAX_DIMENSIONS];

    if( get_volume_n_dimensions(volume) != 3 )
    {
        print( "convert_3D_voxel_to_world:  Volume must be 3D.\n" );
        return;
    }

    voxel[X] = x_voxel;
    voxel[Y] = y_voxel;
    voxel[Z] = z_voxel;

    convert_voxel_to_world( volume, voxel, x_world, y_world, z_world );
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

    *x_world = Transform_elem(*inverse,0,0) * x_voxel +
               Transform_elem(*inverse,1,0) * y_voxel +
               Transform_elem(*inverse,2,0) * z_voxel;
    *y_world = Transform_elem(*inverse,0,1) * x_voxel +
               Transform_elem(*inverse,1,1) * y_voxel +
               Transform_elem(*inverse,2,1) * z_voxel;
    *z_world = Transform_elem(*inverse,0,2) * x_voxel +
               Transform_elem(*inverse,1,2) * y_voxel +
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
    Real     voxel[] )
{
    int    c;
    Real   x_voxel, y_voxel, z_voxel;

    /* apply linear transform */

    general_inverse_transform_point( &volume->voxel_to_world_transform,
                     x_world, y_world, z_world,
                     &x_voxel, &y_voxel, &z_voxel );

    for_less( c, 0, get_volume_n_dimensions(volume) )
        voxel[c] = 0.0;

    if( volume->spatial_axes[0] >= 0 )
        voxel[volume->spatial_axes[0]] = x_voxel;

    if( volume->spatial_axes[1] >= 0 )
        voxel[volume->spatial_axes[1]] = y_voxel;

    if( volume->spatial_axes[2] >= 0 )
        voxel[volume->spatial_axes[2]] = z_voxel;
}

public  void  convert_3D_world_to_voxel(
    Volume   volume,
    Real     x_world,
    Real     y_world,
    Real     z_world,
    Real     *x_voxel,
    Real     *y_voxel,
    Real     *z_voxel )
{
    Real   voxel[MAX_DIMENSIONS];

    if( get_volume_n_dimensions(volume) != 3 )
    {
        print( "convert_3D_world_to_voxel:  Volume must be 3D.\n" );
        return;
    }

    convert_world_to_voxel( volume, x_world, y_world, z_world, voxel );

    *x_voxel = voxel[X];
    *y_voxel = voxel[Y];
    *z_voxel = voxel[Z];
}

public  Real  get_volume_voxel_min(
    Volume   volume )
{
    return( volume->voxel_min );
}

public  Real  get_volume_voxel_max(
    Volume   volume )
{
    return( volume->voxel_max );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_volume_voxel_range
@INPUT      : volume
@OUTPUT     : voxel_min
              voxel_max
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
    Real       *voxel_min,
    Real       *voxel_max )
{
    *voxel_min = get_volume_voxel_min( volume );
    *voxel_max = get_volume_voxel_max( volume );
}

public  void  set_volume_voxel_range(
    Volume   volume,
    Real     voxel_min,
    Real     voxel_max )
{
    Real  real_min, real_max;

    if( voxel_min >= voxel_max )
    {
        switch( volume->data_type )
        {
        case UNSIGNED_BYTE:
            voxel_min = 0.0;          voxel_max = UCHAR_MAX;     break;
        case SIGNED_BYTE:
            voxel_min = SCHAR_MIN;    voxel_max = SCHAR_MAX;     break;
        case UNSIGNED_SHORT:
            voxel_min = 0.0;          voxel_max = USHRT_MAX;     break;
        case SIGNED_SHORT:
            voxel_min = SHRT_MIN;     voxel_max = SHRT_MAX;      break;
        case UNSIGNED_LONG:
            voxel_min = 0.0;          voxel_max = ULONG_MAX;     break;
        case SIGNED_LONG:
            voxel_min = LONG_MIN;     voxel_max = LONG_MAX;      break;
        case FLOAT:
            voxel_min = -FLT_MAX;     voxel_max = FLT_MAX;       break;
        case DOUBLE:
            voxel_min = -DBL_MAX;     voxel_max = DBL_MAX;       break;
        }
    }

    if( volume->real_range_set )
        get_volume_real_range( volume, &real_min, &real_max );

    volume->voxel_min = voxel_min;
    volume->voxel_max = voxel_max;

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

    real_min = get_volume_voxel_min( volume );

    if( volume->real_range_set )
        real_min = CONVERT_VOXEL_TO_VALUE( volume, real_min );

    return( real_min );
}

public  Real  get_volume_real_max(
    Volume     volume )
{
    Real   real_max;

    real_max = get_volume_voxel_max( volume );

    if( volume->real_range_set )
        real_max = CONVERT_VOXEL_TO_VALUE( volume, real_max );

    return( real_max );
}

public  void  set_volume_real_range(
    Volume   volume,
    Real     real_min,
    Real     real_max )
{
    Real    voxel_min, voxel_max;

    if( volume->data_type == FLOAT || volume->data_type == DOUBLE )
    {
        set_volume_voxel_range( volume, real_min, real_max );
        volume->real_value_scale = 1.0;
        volume->real_value_translation = 0.0;
    }
    else
    {
        get_volume_voxel_range( volume, &voxel_min, &voxel_max );

        if( voxel_min < voxel_max )
        {
            volume->real_value_scale = (real_max - real_min) /
                                       (voxel_max - voxel_min);
            volume->real_value_translation = real_min -
                                       voxel_min * volume->real_value_scale;
        }
        else
        {
            volume->real_value_scale = 0.0;
            volume->real_value_translation = real_min;
        }

        volume->real_range_set = TRUE;
    }
}

public  Volume   copy_volume_definition(
    Volume   volume,
    nc_type  nc_data_type,
    BOOLEAN  signed_flag,
    Real     voxel_min,
    Real     voxel_max )
{
    int                c, sizes[MAX_DIMENSIONS];
    Real               separations[MAX_DIMENSIONS];
    Volume             copy;
    General_transform  transform;

    get_volume_sizes( volume, sizes );
    get_volume_separations( volume, separations );

    if( nc_data_type == NC_UNSPECIFIED )
    {
        nc_data_type = volume->nc_data_type;
        signed_flag = volume->signed_flag;
        get_volume_voxel_range( volume, &voxel_min, &voxel_max );
    }

    copy = create_volume( get_volume_n_dimensions(volume),
                          volume->dimension_names, nc_data_type, signed_flag,
                          voxel_min, voxel_max );
    set_volume_sizes( copy, sizes );
    alloc_volume_data( copy );

    for_less( c, 0, N_DIMENSIONS )
        copy->spatial_axes[c] = volume->spatial_axes[c];

    set_volume_real_range( copy,
                           get_volume_real_min(volume),
                           get_volume_real_max(volume) );

    set_volume_separations( copy, separations );

    for_less( c, 0, get_volume_n_dimensions(volume) )
        set_volume_direction_cosine( copy, c, volume->direction_cosines[c] );

    copy_general_transform( get_voxel_to_world_transform(volume),
                            &transform );

    set_voxel_to_world_transform( copy, &transform );

    return( copy );
}
