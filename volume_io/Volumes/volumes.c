#include  <internal_volume_io.h>
#include  <limits.h>
#include  <float.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Volumes/volumes.c,v 1.39 1995-02-20 13:12:21 david Exp $";
#endif

char   *XYZ_dimension_names[] = { MIxspace, MIyspace, MIzspace };

private  char  *default_dimension_names[MAX_DIMENSIONS][MAX_DIMENSIONS] =
{
    { MIxspace },
    { MIyspace, MIxspace },
    { MIzspace, MIyspace, MIxspace },
    { MItime, MIzspace, MIyspace, MIxspace },
    { "", MItime, MIzspace, MIyspace, MIxspace }
};

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_default_dim_names
@INPUT      : n_dimensions
@OUTPUT     : 
@RETURNS    : list of dimension names
@DESCRIPTION: Returns the list of default dimension names for the given
              number of dimensions.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  char  **get_default_dim_names(
    int    n_dimensions )
{
    return( default_dimension_names[n_dimensions-1] );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_spatial_axis_to_dim_name
@INPUT      : axis
@OUTPUT     : 
@RETURNS    : dimension name
@DESCRIPTION: Returns the name of the dimension.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  char  *convert_spatial_axis_to_dim_name(
    int   axis )
{
    switch( axis )
    {
    case X:  return( MIxspace );
    case Y:  return( MIyspace );
    case Z:  return( MIzspace );
    default:  handle_internal_error(
        "convert_spatial_axis_to_dim_name" ); break;
    }

    return( (char *) 0 );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_dim_name_to_spatial_axis
@INPUT      : dimension_name
@OUTPUT     : axis
@RETURNS    : TRUE if axis name is a spatial dimension
@DESCRIPTION: Checks if the dimension name corresponds to a spatial dimension
              and if so, passes back the corresponding axis index.
@METHOD     :
@GLOBALS    :
@CALLS      :
@CREATED    : 1993            David MacDonald
@MODIFIED   :
---------------------------------------------------------------------------- */

public  BOOLEAN  convert_dim_name_to_spatial_axis(
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

    volume->is_rgba_data = FALSE;

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

        if( convert_dim_name_to_spatial_axis( name, &axis ) )
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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_volume_type
@INPUT      : volume
              nc_data_type
              signed_flag
              voxel_min
              voxel_max
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the data type and valid range of the volume.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_volume_nc_data_type
@INPUT      : volume
@OUTPUT     : signed_flag
@RETURNS    : data type
@DESCRIPTION: Returns the NETCDF data type of the volume and passes back
              the signed flag.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  nc_type  get_volume_nc_data_type(
    Volume       volume,
    BOOLEAN      *signed_flag )
{
    if( signed_flag != (BOOLEAN *) NULL )
        *signed_flag = volume->signed_flag;
    return( volume->nc_data_type );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_volume_data_type
@INPUT      : volume
@OUTPUT     : 
@RETURNS    : data type
@DESCRIPTION: Returns the data type of the volume (not the NETCDF type).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

public  BOOLEAN  is_an_rgb_volume(
    Volume   volume )
{
    return( volume->is_rgba_data );
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
    int    type_size, *sizes;
    void   *p1, **p2, ***p3, ****p4, *****p5;

    if( volume->data != (void *) NULL )
        free_volume_data( volume );

    if( volume->data_type == NO_DATA_TYPE )
    {
        print( "Error: cannot allocate volume data until size specified.\n" );
        return;
    }

    sizes = volume->sizes;
    type_size = get_type_size( volume->data_type );

    switch( volume->n_dimensions )
    {
    case  1:
        ALLOC_private( p1, type_size, sizes[0] );
        volume->data = (void *) p1;
        break;
    case  2:
        ALLOC2D_private( p2, type_size, sizes[0], sizes[1] );
        volume->data = (void *) p2;
        break;
    case  3:
        ALLOC3D_private( p3, type_size, sizes[0], sizes[1], sizes[2] );
        volume->data = (void *) p3;
        break;
    case  4:
        ALLOC4D_private( p4, type_size, sizes[0], sizes[1],
                         sizes[2], sizes[3] );
        volume->data = (void *) p4;
        break;
    case  5:
        ALLOC5D_private( p5, type_size, sizes[0], sizes[1],
                         sizes[2], sizes[3], sizes[4] );
        volume->data = (void *) p5;
        break;
    }
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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_volume_sizes
@INPUT      : volume
              sizes
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the sizes (number of voxels in each dimension) of the volume.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  set_volume_sizes(
    Volume       volume,
    int          sizes[] )
{
    int             i;

    for_less( i, 0, volume->n_dimensions )
        volume->sizes[i] = sizes[i];
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_volume_total_n_voxels
@INPUT      : volume
@OUTPUT     : 
@RETURNS    : n voxels
@DESCRIPTION: Returns the total number of voxels in the volume.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_voxel_to_world_transform
@INPUT      : volume
              transform
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the volume's transformation from voxel to world coords.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  set_voxel_to_world_transform(
    Volume             volume,
    General_transform  *transform )
{
    delete_general_transform( &volume->voxel_to_world_transform );

    volume->voxel_to_world_transform = *transform;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_voxel_to_world_transform
@INPUT      : 
@OUTPUT     : 
@RETURNS    : transform
@DESCRIPTION: Returns a pointer to the voxel to world transform of the volume.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  General_transform  *get_voxel_to_world_transform(
    Volume   volume )
{
    return( &volume->voxel_to_world_transform );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : compute_world_transform
@INPUT      : spatial_axes
              separations
              translation_voxel
              world_space_for_translation_voxel
              direction_cosines
@OUTPUT     : world_transform
@RETURNS    : 
@DESCRIPTION: Computes the linear transform from the indices of the spatial
              dimensions (spatial_axes), the separations, the translation
              (translation_voxel,world_space_for_translation_voxel) and
              the direction cosines.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : recompute_world_transform
@INPUT      : volume
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Recompute the voxel to world transform.  Called when one of
              the attributes affecting this is changed.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_volume_dimension_names
@INPUT      : volume
@OUTPUT     : 
@RETURNS    : list of dimension names
@DESCRIPTION: Creates a copy of the dimension names of the volume.  Therefore,
              after use, the calling function must free the list, by calling
              delete_dimension_names().
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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
            (void) strcpy( names[volume->spatial_axes[i]],
                           convert_spatial_axis_to_dim_name( i ) );
    }

    return( names );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : delete_dimension_names
@INPUT      : dimension_names
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Frees the memory allocated to the dimension names, which came
              from the above function, get_volume_dimension_names().
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_volume_separations
@INPUT      : volume
              separations
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the separations between slices for the given volume.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  set_volume_separations(
    Volume   volume,
    Real     separations[] )
{
    int   i;

    for_less( i, 0, volume->n_dimensions )
        volume->separations[i] = separations[i];

    recompute_world_transform( volume );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_volume_translation
@INPUT      : volume
              voxel
              world_space_voxel_maps_to
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the translation portion of the voxel to world transform,
              by specifying a point in voxel space (voxel), and a point
              in world space (world_space_voxel_maps_to).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_volume_translation
@INPUT      : volume
@OUTPUT     : voxel
              world_space_voxel_maps_to
@RETURNS    : 
@DESCRIPTION: Passes back the translation portion of the voxel to world
              transform of the volume.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_volume_direction_cosine
@INPUT      : volume
              axis
              dir
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the direction cosine for one axis.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

private  void  reorder_voxel_to_xyz(
    Volume   volume,
    Real     voxel[],
    Real     xyz[] )
{
    int   c, axis;

    for_less( c, 0, N_DIMENSIONS )
    {
        axis = volume->spatial_axes[c];
        if( axis >= 0 )
            xyz[c] = voxel[axis];
        else
            xyz[c] = 0.0;
    }
}

private  void  reorder_xyz_to_voxel(
    Volume   volume,
    Real     xyz[],
    Real     voxel[] )
{
    int   c, axis, n_dims;

    n_dims = get_volume_n_dimensions( volume );
    for_less( c, 0, n_dims )
        voxel[c] = 0.0;

    for_less( c, 0, N_DIMENSIONS )
    {
        axis = volume->spatial_axes[c];
        if( axis >= 0 )
            voxel[axis] = xyz[c];
    }
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
    Real   xyz[N_DIMENSIONS];

    reorder_voxel_to_xyz( volume, voxel, xyz );

    /* apply linear transform */

    general_transform_point( &volume->voxel_to_world_transform,
                             xyz[X], xyz[Y], xyz[Z],
                             x_world, y_world, z_world );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_3D_voxel_to_world
@INPUT      : volume
              voxel1
              voxel2
              voxel3
@OUTPUT     : x_world
              y_world
              z_world
@RETURNS    : 
@DESCRIPTION: Convenience function which performs same task as
              convert_voxel_to_world(), but for 3D volumes only.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  convert_3D_voxel_to_world(
    Volume   volume,
    Real     voxel1,
    Real     voxel2,
    Real     voxel3,
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

    voxel[0] = voxel1;
    voxel[1] = voxel2;
    voxel[2] = voxel3;

    convert_voxel_to_world( volume, voxel, x_world, y_world, z_world );
}

/* ----------------------------- MNI Header -----------------------------------@NAME       : convert_voxel_normal_vector_to_world
@INPUT      : volume
              voxel_vector0
              voxel_vector1
              voxel_vector2
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
    Real            voxel_vector0,
    Real            voxel_vector1,
    Real            voxel_vector2,
    Real            *x_world,
    Real            *y_world,
    Real            *z_world )
{
    Real        voxel[N_DIMENSIONS], xyz[N_DIMENSIONS];
    Transform   *inverse;

    voxel[0] = voxel_vector0;
    voxel[1] = voxel_vector1;
    voxel[2] = voxel_vector2;

    reorder_voxel_to_xyz( volume, voxel, xyz );

    if( get_transform_type( &volume->voxel_to_world_transform ) != LINEAR )
        handle_internal_error( "Cannot get normal vector of nonlinear xforms.");

    inverse = get_inverse_linear_transform_ptr(
                                      &volume->voxel_to_world_transform );

    /* transform vector by transpose of inverse transformation */

    *x_world = Transform_elem(*inverse,0,0) * xyz[X] +
               Transform_elem(*inverse,1,0) * xyz[Y] +
               Transform_elem(*inverse,2,0) * xyz[Z];
    *y_world = Transform_elem(*inverse,0,1) * xyz[X] +
               Transform_elem(*inverse,1,1) * xyz[Y] +
               Transform_elem(*inverse,2,1) * xyz[Z];
    *z_world = Transform_elem(*inverse,0,2) * xyz[X] +
               Transform_elem(*inverse,1,2) * xyz[Y] +
               Transform_elem(*inverse,2,2) * xyz[Z];
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_voxel_vector_to_world
@INPUT      : volume
              voxel_vector
@OUTPUT     : x_world
              y_world
              z_world
@RETURNS    : 
@DESCRIPTION: Converts a voxel vector to world coordinates.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  convert_voxel_vector_to_world(
    Volume          volume,
    Real            voxel_vector[],
    Real            *x_world,
    Real            *y_world,
    Real            *z_world )
{
    int         i;
    Real        origin[MAX_DIMENSIONS], x0, y0, z0, x1, y1, z1;

    for_less( i, 0, MAX_DIMENSIONS )
        origin[i] = 0.0;

    convert_voxel_to_world( volume, origin, &x0, &y0, &z0 );

    convert_voxel_to_world( volume, voxel_vector, &x1, &y1, &z1 );

    *x_world = x1 - x0;
    *y_world = y1 - y0;
    *z_world = z1 - z0;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_world_vector_to_voxel
@INPUT      : volume
              x_world
              y_world
              z_world
@OUTPUT     : voxel_vector
@RETURNS    : 
@DESCRIPTION: Converts a world vector to voxel coordinates.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  convert_world_vector_to_voxel(
    Volume          volume,
    Real            x_world,
    Real            y_world,
    Real            z_world,
    Real            voxel_vector[] )
{
    int         c;
    Real        voxel[MAX_DIMENSIONS], origin[MAX_DIMENSIONS];

    convert_world_to_voxel( volume, 0.0, 0.0, 0.0, origin );
    convert_world_to_voxel( volume, x_world, y_world, z_world, voxel );

    for_less( c, 0, get_volume_n_dimensions(volume) )
        voxel_vector[c] = voxel[c] - origin[c];
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
    Real   xyz[N_DIMENSIONS];

    /* apply linear transform */

    general_inverse_transform_point( &volume->voxel_to_world_transform,
                                     x_world, y_world, z_world,
                                     &xyz[X], &xyz[Y], &xyz[Z] );

    reorder_xyz_to_voxel( volume, xyz, voxel );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_3D_world_to_voxel
@INPUT      : volume
              x_world
              y_world
              z_world
@OUTPUT     : voxel1
              voxel2
              voxel3
@RETURNS    : 
@DESCRIPTION: Convenience function that does same task as
              convert_world_to_voxel(), but only for 3D volumes.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  convert_3D_world_to_voxel(
    Volume   volume,
    Real     x_world,
    Real     y_world,
    Real     z_world,
    Real     *voxel1,
    Real     *voxel2,
    Real     *voxel3 )
{
    Real   voxel[MAX_DIMENSIONS];

    if( get_volume_n_dimensions(volume) != 3 )
    {
        print( "convert_3D_world_to_voxel:  Volume must be 3D.\n" );
        return;
    }

    convert_world_to_voxel( volume, x_world, y_world, z_world, voxel );

    *voxel1 = voxel[X];
    *voxel2 = voxel[Y];
    *voxel3 = voxel[Z];
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_volume_voxel_min
@INPUT      : volume
@OUTPUT     : 
@RETURNS    : min valid voxel 
@DESCRIPTION: Returns the minimum valid voxel value.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Real  get_volume_voxel_min(
    Volume   volume )
{
    return( volume->voxel_min );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_volume_voxel_max
@INPUT      : volume
@OUTPUT     : 
@RETURNS    : max valid voxel 
@DESCRIPTION: Returns the maximum valid voxel value.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_volume_voxel_range
@INPUT      : volume
              voxel_min
              voxel_max
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the valid range of voxels.  If an invalid range is
              specified (voxel_min >= voxel_max), the full range of the
              volume's type is used.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_volume_real_min
@INPUT      : volume
@OUTPUT     : 
@RETURNS    : real range minimum
@DESCRIPTION: Returns the minimum of the real range of the volume.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Real  get_volume_real_min(
    Volume     volume )
{
    Real   real_min;

    real_min = get_volume_voxel_min( volume );

    if( volume->real_range_set )
        real_min = CONVERT_VOXEL_TO_VALUE( volume, real_min );

    return( real_min );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_volume_real_max
@INPUT      : volume
@OUTPUT     : 
@RETURNS    : real range max
@DESCRIPTION: Returns the maximum of the real range of the volume.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Real  get_volume_real_max(
    Volume     volume )
{
    Real   real_max;

    real_max = get_volume_voxel_max( volume );

    if( volume->real_range_set )
        real_max = CONVERT_VOXEL_TO_VALUE( volume, real_max );

    return( real_max );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_volume_real_range
@INPUT      : volume
              real_min
              real_max
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the range of real values to which the valid voxel
              range maps
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : copy_volume_definition_no_alloc
@INPUT      : volume
              nc_data_type
              signed_flag
              voxel_min
              voxel_max
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Copies the volume to a new volume, optionally changing type
              (if nc_data_type is not NC_UNSPECIFIED), but not allocating
              the volume voxel data (alloc_volume_data() must subsequently
              be called).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Volume   copy_volume_definition_no_alloc(
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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : copy_volume_definition
@INPUT      : volume
              nc_data_type
              signed_flag
              voxel_min
              voxel_max
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Copies the volume to a new volume, optionally changing type
              (if nc_data_type is not NC_UNSPECIFIED), allocating
              the volume voxel data, but not initializing the data.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Volume   copy_volume_definition(
    Volume   volume,
    nc_type  nc_data_type,
    BOOLEAN  signed_flag,
    Real     voxel_min,
    Real     voxel_max )
{
    Volume   copy;

    copy = copy_volume_definition_no_alloc( volume,
                                            nc_data_type, signed_flag,
                                            voxel_min, voxel_max );
    alloc_volume_data( copy );

    return( copy );
}
