#include  <def_mni.h>

private  void  free_auxiliary_data(
    volume_struct  *volume );

/* ----------------------------- MNI Header -----------------------------------
@NAME       : alloc_volume
@INPUT      : volume
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Assumes that the volume sizes and the data type have been
              assigned, and allocates the volume data.
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  alloc_volume(
    volume_struct  *volume )
{
    if( volume->sizes[X] > 0 && volume->sizes[Y] > 0 && volume->sizes[Z] > 0 )
    {
        switch( volume->data_type )
        {
        case UNSIGNED_BYTE:
            ALLOC3D( volume->byte_data, volume->sizes[X], volume->sizes[Y],
                     volume->sizes[Z] );
            break;

        case UNSIGNED_SHORT:
            ALLOC3D( volume->short_data, volume->sizes[X], volume->sizes[Y],
                     volume->sizes[Z] );
            break;

        default:
            HANDLE_INTERNAL_ERROR( "alloc_volume" );
        }
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : delete_volume
@INPUT      : volume
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Frees the memory associated with the volume.
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  delete_volume(
    volume_struct  *volume )
{
    if( volume->sizes[X] > 0 && volume->sizes[Y] > 0 && volume->sizes[Z] > 0 )
    {
        switch( volume->data_type )
        {
        case UNSIGNED_BYTE:
            FREE3D( volume->byte_data );
            break;

        case UNSIGNED_SHORT:
            FREE3D( volume->short_data );
            break;

        default:
            HANDLE_INTERNAL_ERROR( "alloc_volume" );
        }
    }

    free_auxiliary_data( volume );
}

public  void  initialize_volume(
    volume_struct  *volume )
{
    volume->value_scale = 1.0;
    volume->value_translation = 0.0;
    volume->sizes[X] = 0;
    volume->sizes[Y] = 0;
    volume->sizes[Z] = 0;
    volume->labels = (unsigned char ***) NULL;
}

public  void  get_volume_size(
    volume_struct   *volume,
    int             *x_size,
    int             *y_size,
    int             *z_size )
{
    *x_size = volume->sizes[X];
    *y_size = volume->sizes[Y];
    *z_size = volume->sizes[Z];
}

public  void  get_volume_slice_thickness(
    volume_struct   *volume,
    Real            *x_thickness,
    Real            *y_thickness,
    Real            *z_thickness )
{
    *x_thickness = volume->thickness[X];
    *y_thickness = volume->thickness[Y];
    *z_thickness = volume->thickness[Z];
}

public  void  convert_voxel_to_world(
    volume_struct   *volume,
    Real            x_voxel,
    Real            y_voxel,
    Real            z_voxel,
    Real            *x_world,
    Real            *y_world,
    Real            *z_world )
{
    Point   voxel, world;

    fill_Point( voxel, x_voxel, y_voxel, z_voxel );

    transform_point( &volume->voxel_to_world_transform,
                     &voxel, &world );

    *x_world = Point_x(world);
    *y_world = Point_y(world);
    *z_world = Point_z(world);
}

public  void  convert_voxel_vector_to_world(
    volume_struct   *volume,
    Real            xv_voxel,
    Real            yv_voxel,
    Real            zv_voxel,
    Real            *xv_world,
    Real            *yv_world,
    Real            *zv_world )
{
    /* transform vector by transpose of inverse transformation */
    *xv_world = Transform_elem(volume->world_to_voxel_transform,0,0) * xv_voxel+
                Transform_elem(volume->world_to_voxel_transform,1,0) * yv_voxel+
                Transform_elem(volume->world_to_voxel_transform,2,0) * zv_voxel;
    *yv_world = Transform_elem(volume->world_to_voxel_transform,0,1) * xv_voxel+
                Transform_elem(volume->world_to_voxel_transform,1,1) * yv_voxel+
                Transform_elem(volume->world_to_voxel_transform,2,1) * zv_voxel;
    *zv_world = Transform_elem(volume->world_to_voxel_transform,0,2) * xv_voxel+
                Transform_elem(volume->world_to_voxel_transform,1,2) * yv_voxel+
                Transform_elem(volume->world_to_voxel_transform,2,2) * zv_voxel;
}

public  void  convert_world_to_voxel(
    volume_struct   *volume,
    Real            x_world,
    Real            y_world,
    Real            z_world,
    Real            *x_voxel,
    Real            *y_voxel,
    Real            *z_voxel )
{
    Point   voxel, world;

    fill_Point( world, x_world, y_world, z_world );

    transform_point( &volume->world_to_voxel_transform,
                     &world, &voxel );

    *x_voxel = Point_x(voxel);
    *y_voxel = Point_y(voxel);
    *z_voxel = Point_z(voxel);
}

public  Boolean  voxel_is_within_volume(
    volume_struct   *volume,
    Real            x, 
    Real            y, 
    Real            z )
{
    return( x >= -0.5 &&
            x < (Real) volume->sizes[X] - 0.5 &&
            y >= -0.5 &&
            y < (Real) volume->sizes[Y] - 0.5 &&
            z >= -0.5 &&
            z < (Real) volume->sizes[Z] - 0.5 );
}

public  Boolean  cube_is_within_volume(
    volume_struct   *volume,
    int             x,
    int             y,
    int             z )
{
    int     nx, ny, nz;

    get_volume_size( volume, &nx, &ny, &nz );

    return( x >= 0 && x < nx-1 && y >= 0 && y < ny-1 && z >= 0 && z < nz-1 );
}

public  Boolean  voxel_contains_value(
    volume_struct   *volume,
    int             x,
    int             y,
    int             z,
    Real            target_value )
{
    Boolean  less, greater;
    int      x_offset, y_offset, z_offset;
    Real     value;

    less = FALSE;
    greater = FALSE;

    for_less( x_offset, 0, 2 )
    {
        for_less( y_offset, 0, 2 )
        {
            for_less( z_offset, 0, 2 )
            {
                value = (Real) GET_VOLUME_DATA( *volume,
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

public  void  alloc_auxiliary_data(
    volume_struct  *volume )
{
    ALLOC3D( volume->labels, volume->sizes[X], volume->sizes[Y],
             volume->sizes[Z] );
    set_all_volume_auxiliary_data( volume, ACTIVE_BIT );
}

private  void  free_auxiliary_data(
    volume_struct  *volume )
{
    if( volume->labels != (unsigned char ***) NULL )
        FREE3D( volume->labels );
}

public  void  set_all_volume_auxiliary_data(
    volume_struct  *volume,
    int            value )
{
    int             n_voxels, i, nx, ny, nz;
    unsigned char   *label_ptr;

    get_volume_size( volume, &nx, &ny, &nz );

    n_voxels = nx * ny * nz;

    label_ptr = volume->labels[0][0];

    for_less( i, 0, n_voxels )
    {
        *label_ptr = (unsigned char) value;
        ++label_ptr;
    }
}

public  void  set_all_volume_auxiliary_data_bit(
    volume_struct  *volume,
    int            bit,
    Boolean        value )
{
    int             n_voxels, i, nx, ny, nz;
    unsigned char   *label_ptr;

    get_volume_size( volume, &nx, &ny, &nz );

    n_voxels = nx * ny * nz;

    label_ptr = volume->labels[0][0];

    for_less( i, 0, n_voxels )
    {
        if( value )
            *label_ptr |= bit;
        else if( (*label_ptr & bit) != 0 )
            *label_ptr ^= bit;

        ++label_ptr;
    }
}

public  void  set_all_voxel_activity_flags(
    volume_struct  *volume,
    Boolean        value )
{
    set_all_volume_auxiliary_data_bit( volume, ACTIVE_BIT, value );
}

public  void  set_all_voxel_label_flags(
    volume_struct  *volume,
    Boolean        value )
{
    set_all_volume_auxiliary_data_bit( volume, LABEL_BIT, value );
}

public  void  set_volume_auxiliary_data(
    volume_struct   *volume,
    int             x,
    int             y,
    int             z,
    int             value )
{
    volume->labels[x][y][z] = (unsigned char) value;
}

public  Boolean  get_voxel_activity_flag(
    volume_struct   *volume,
    int             x,
    int             y,
    int             z )
{
    if( volume->labels == (unsigned char ***) NULL )
        return( TRUE );
    else
        return( (volume->labels[x][y][z] & ACTIVE_BIT) != 0 );
}

public  void  set_voxel_activity_flag(
    volume_struct   *volume,
    int             x,
    int             y,
    int             z,
    Boolean         value )
{
    unsigned  char  *ptr;

    ptr = &volume->labels[x][y][z];
    if( value )
        *ptr |= ACTIVE_BIT;
    else if( (*ptr & ACTIVE_BIT) != 0 )
        *ptr ^= ACTIVE_BIT;
}

public  Boolean  get_voxel_label_flag(
    volume_struct   *volume,
    int             x,
    int             y,
    int             z )
{
    return( (volume->labels[x][y][z] & LABEL_BIT) != 0 );
}

public  void  set_voxel_label_flag(
    volume_struct   *volume,
    int             x,
    int             y,
    int             z,
    Boolean         value )
{
    unsigned  char  *ptr;

    ptr = &volume->labels[x][y][z];
    if( value )
        *ptr |= LABEL_BIT;
    else if( (*ptr & LABEL_BIT) != 0 )
        *ptr ^= LABEL_BIT;
}

public  Status  io_volume_auxiliary_bit(
    FILE           *file,
    IO_types       io_type,
    volume_struct  *volume,
    int            bit )
{
    Status             status;
    bitlist_3d_struct  bitlist;
    int                x, y, z, nx, ny, nz;
    unsigned char      *label_ptr;

    status = OK;

    get_volume_size( volume, &nx, &ny, &nz );

    create_bitlist_3d( nx, ny, nz, &bitlist );

    if( io_type == WRITE_FILE )
    {
        label_ptr = volume->labels[0][0];

        for_less( x, 0, nx )
        {
            for_less( y, 0, ny )
            {
                for_less( z, 0, nz )
                {
                    if( (*label_ptr & bit) != 0 )
                        set_bitlist_bit_3d( &bitlist, x, y, z, TRUE );

                    ++label_ptr;
                }
            }
        }
    }

    status = io_bitlist_3d( file, io_type, &bitlist );

    if( status == OK && io_type == READ_FILE )
    {
        label_ptr = volume->labels[0][0];

        for_less( x, 0, nx )
        {
            for_less( y, 0, ny )
            {
                for_less( z, 0, nz )
                {
                    if( get_bitlist_bit_3d( &bitlist, x, y, z ) )
                        *label_ptr |= bit;
                    else if( (*label_ptr & bit) != 0 )
                        *label_ptr ^= bit;

                    ++label_ptr;
                }
            }
        }
    }

    if( status == OK )
        delete_bitlist_3d( &bitlist );

    return( status );
}

public  Boolean   evaluate_volume_in_world(
    volume_struct  *volume,
    Real           x,
    Real           y,
    Real           z,
    Boolean        activity_if_mixed,
    Real           *value,
    Real           *deriv_x,
    Real           *deriv_y,
    Real           *deriv_z )
{
    Boolean voxel_is_active;

    convert_world_to_voxel( volume, x, y, z, &x, &y, &z );

    voxel_is_active = evaluate_volume( volume, x, y, z, activity_if_mixed,
                                       value, deriv_x, deriv_y, deriv_z );

    if( deriv_x != (Real *) 0 )
    {
        convert_voxel_vector_to_world( volume, *deriv_x, *deriv_y, *deriv_z,
                                       deriv_x, deriv_y, deriv_z );
    }

    return( voxel_is_active );
}

public  Boolean   evaluate_volume(
    volume_struct  *volume,
    Real           x,
    Real           y,
    Real           z,
    Boolean        activity_if_mixed,
    Real           *value,
    Real           *deriv_x,
    Real           *deriv_y,
    Real           *deriv_z )
{
    Boolean voxel_is_active;
    int     n_inactive;
    int     i, j, k;
    int     xi, yi, zi, x_voxel, y_voxel, z_voxel;
    Real    u, v, w;
    Real    val;
    Real    c000, c001, c010, c011, c100, c101, c110, c111;
    Real    c00, c01, c10, c11;
    Real    c0, c1;
    Real    du00, du01, du10, du11, du0, du1;
    Real    dv0, dv1;
    int     nx, ny, nz;

    get_volume_size( volume, &nx, &ny, &nz );

    if( x < 0.0 || x > (Real) (nx-1) ||
        y < 0.0 || y > (Real) (ny-1) ||
        z < 0.0 || z > (Real) (nz-1) )
    {
        *value = 0.0;
        if( deriv_x != (Real *) 0 )
        {
            *deriv_x = 0.0;
            *deriv_y = 0.0;
            *deriv_z = 0.0;
        }

        return( FALSE );
    }

    if( x < 0.0 )
        i = -1;
    else if( x == (Real) (nx-1) )
    {
        i = nx-2;
        u = 1.0;
    }
    else
    {
        i = (int) x;
        u = FRACTION( x );
    }

    if( y < 0.0 )
        j = -1;
    else if( y == (Real) (ny-1) )
    {
        j = ny-2;
        v = 1.0;
    }
    else
    {
        j = (int) y;
        v = FRACTION( y );
    }

    if( z < 0.0 )
        k = -1;
    else if( z == (Real) (nz-1) )
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

    if( i >= 0 && i < nx-1 && j >= 0 && j < ny-1 && k >= 0 && k < nz-1 )
    {
        c000 = (Real) GET_VOLUME_DATA( *volume, i,   j,   k );
        c001 = (Real) GET_VOLUME_DATA( *volume, i,   j,   k+1 );
        c010 = (Real) GET_VOLUME_DATA( *volume, i,   j+1, k );
        c011 = (Real) GET_VOLUME_DATA( *volume, i,   j+1, k+1 );
        c100 = (Real) GET_VOLUME_DATA( *volume, i+1, j,   k );
        c101 = (Real) GET_VOLUME_DATA( *volume, i+1, j,   k+1 );
        c110 = (Real) GET_VOLUME_DATA( *volume, i+1, j+1, k );
        c111 = (Real) GET_VOLUME_DATA( *volume, i+1, j+1, k+1 );

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
    }
    else         /* for now, won't get to this case */
    {
        HANDLE_INTERNAL_ERROR( "evaluate_volume" );
        for_less( xi, 0, 2 )
        {
            x_voxel = i + xi;
            for_less( yi, 0, 2 )
            {
                y_voxel = j + yi;
                for_less( zi, 0, 2 )
                {
                    z_voxel = k + zi;
                    if( x_voxel >= 0 && x_voxel < nx &&
                        y_voxel >= 0 && y_voxel < ny &&
                        z_voxel >= 0 && z_voxel < nz )
                    {
                        val = (Real) GET_VOLUME_DATA( *volume, x_voxel,
                                                      y_voxel, z_voxel );

                        if( !get_voxel_activity_flag( volume, x_voxel,
                                                      y_voxel, z_voxel ) )
                            ++n_inactive;
                    }
                    else
                    {
                        val = 0.0;
                        ++n_inactive;
                    }

                    if( xi == 0 )
                    {
                        if( yi == 0 )
                        {
                            if( zi == 0 )
                                c000 = val;
                            else
                                c001 = val;
                        }
                        else
                        {
                            if( zi == 0 )
                                c010 = val;
                            else
                                c011 = val;
                        }
                    }
                    else
                    {
                        if( yi == 0 )
                        {
                            if( zi == 0 )
                                c100 = val;
                            else
                                c101 = val;
                        }
                        else
                        {
                            if( zi == 0 )
                                c110 = val;
                            else
                                c111 = val;
                        }
                    }
                }
            }
        }
    }
    
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
        voxel_is_active = TRUE;
    else if( n_inactive == 8 )
        voxel_is_active = FALSE;
    else
        voxel_is_active = activity_if_mixed;

    return( voxel_is_active );
}
