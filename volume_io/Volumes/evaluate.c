#include  <internal_volume_io.h>
#include  <vols.h>
#include  <numerical.h>

private  void   trilinear_interpolate_volume(
    Volume         volume,
    Real           x,
    Real           y,
    Real           z,
    BOOLEAN        x_use_higher,
    BOOLEAN        y_use_higher,
    BOOLEAN        z_use_higher,
    Real           *value,
    Real           *deriv_x,
    Real           *deriv_y,
    Real           *deriv_z );

/* ----------------------------- MNI Header -----------------------------------
@NAME       : evaluate_3D_volume_in_world
@INPUT      : volume
              x
              y
              z
              degrees_continuity - 0 = linear, 2 = cubic
@OUTPUT     : value
              deriv_x
              deriv_y
              deriv_z
@RETURNS    : 
@DESCRIPTION: Takes a world space position and evaluates the value within
              the volume.
              If deriv_x is not a null pointer, then the 3 derivatives are
              passed back.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void   evaluate_3D_volume_in_world(
    Volume         volume,
    Real           x,
    Real           y,
    Real           z,
    int            degrees_continuity,
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
    Real      ignore, dxx, dxy, dxz, dyy, dyz, dzz;
    Real      voxel[MAX_DIMENSIONS];
    Real      txx, txy, txz;
    Real      tyx, tyy, tyz;
    Real      tzx, tzy, tzz;

    if( get_volume_n_dimensions(volume) != 3 )
    {
        handle_internal_error(
                 "evaluate_3D_volume_in_world: volume must be 3D.\n" );
    }

    convert_world_to_voxel( volume, x, y, z, voxel );

    evaluate_3D_volume( volume, voxel[X], voxel[Y], voxel[Z],
                        degrees_continuity, value, deriv_x, deriv_y, deriv_z,
                        deriv_xx, deriv_xy, deriv_xz,
                        deriv_yy, deriv_yz, deriv_zz );

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
}

public  void   special_evaluate_3D_volume_in_world(
    Volume         volume,
    Real           x,
    Real           y,
    Real           z,
    BOOLEAN        x_use_higher,
    BOOLEAN        y_use_higher,
    BOOLEAN        z_use_higher,
    Real           *value,
    Real           *deriv_x,
    Real           *deriv_y,
    Real           *deriv_z )
{
    Real      voxel[MAX_DIMENSIONS];
    int       sizes[MAX_DIMENSIONS];

    convert_world_to_voxel( volume, x, y, z, voxel );

    get_volume_sizes( volume, sizes );
    if( x < -0.5 || x > (Real) sizes[X] - 0.5 ||
        y < -0.5 || y > (Real) sizes[Y] - 0.5 ||
        z < -0.5 || z > (Real) sizes[Z] - 0.5 )
    {
        *value = get_volume_voxel_min( volume );
        if( deriv_x != (Real *) NULL )
        {
            *deriv_x = 0.0;
            *deriv_y = 0.0;
            *deriv_z = 0.0;
        }
        return;
    }

    trilinear_interpolate_volume( volume, voxel[X], voxel[Y], voxel[Z],
                                  x_use_higher, y_use_higher, z_use_higher,
                                  value, deriv_x, deriv_y, deriv_z );

    if( deriv_x != (Real *) 0 )
    {
        convert_voxel_normal_vector_to_world( volume,
                                              *deriv_x, *deriv_y, *deriv_z,
                                              deriv_x, deriv_y, deriv_z );
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : evaluate_3D_slice_in_world
@INPUT      : volume
              x
              y
              z
@OUTPUT     : value
              deriv_x
              deriv_y
              deriv_xx
              deriv_xy
              deriv_yy
@RETURNS    : 
@DESCRIPTION: Takes a world space position and evaluates the value within
              the volume by bilinear interpolation within the slice.
              If deriv_x is not a null pointer, then the derivatives are passed
              back.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void   evaluate_3D_slice_in_world(
    Volume         volume,
    Real           x,
    Real           y,
    Real           z,
    Real           *value,
    Real           *deriv_x,
    Real           *deriv_y,
    Real           *deriv_xx,
    Real           *deriv_xy,
    Real           *deriv_yy )
{
    Real      ignore, dxx, dxy, dyy;
    Real      voxel[MAX_DIMENSIONS];
    Real      txx, txy, txz;
    Real      tyx, tyy, tyz;
    Real      tzx, tzy, tzz;

    if( get_volume_n_dimensions(volume) != 3 )
    {
        handle_internal_error(
                 "evaluate_3D_slice_in_world: volume must be 3D.\n" );
    }

    convert_world_to_voxel( volume, x, y, z, voxel );

    evaluate_3D_slice( volume, voxel[X], voxel[Y], voxel[Z],
                       value, deriv_x, deriv_y,
                       deriv_xx, deriv_xy, deriv_yy );

    if( deriv_x != (Real *) 0 )
    {
        convert_voxel_normal_vector_to_world( volume,
                                              *deriv_x, *deriv_y, 0.0,
                                              deriv_x, deriv_y, &ignore );
    }

    if( deriv_xx != (Real *) 0 )
    {
        dxx = *deriv_xx;
        dxy = *deriv_xy;
        dyy = *deriv_yy;
        convert_voxel_normal_vector_to_world( volume,
                                              dxx, dxy, 0.0,
                                              &txx, &txy, &txz );
        convert_voxel_normal_vector_to_world( volume,
                                              dxy, dyy, 0.0,
                                              &tyx, &tyy, &tyz );
        convert_voxel_normal_vector_to_world( volume,
                                              0.0, 0.0, 0.0,
                                              &tzx, &tzy, &tzz );

        convert_voxel_normal_vector_to_world( volume,
                                              txx, tyx, tzx,
                                              deriv_xx, &ignore, &ignore );
        convert_voxel_normal_vector_to_world( volume,
                                              txy, tyy, tzy,
                                              deriv_xy, deriv_yy, &ignore );
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : triconstant_interpolate_volume
@INPUT      : volume
              x
              y
              z
@OUTPUT     : value
              deriv_x
              deriv_y
              deriv_z
@RETURNS    : 
@DESCRIPTION: Returns the value within the volume, assuming constant voxels,
              (step function).  Derivative is approximated by neighbours.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  void   triconstant_interpolate_volume(
    Volume         volume,
    Real           x,
    Real           y,
    Real           z,
    Real           *value,
    Real           *deriv_x,
    Real           *deriv_y,
    Real           *deriv_z )
{
    int                i, j, k, sizes[MAX_DIMENSIONS];
    Real               prev, next, dx, dy, dz, voxel_value;

    get_volume_sizes( volume, sizes );

    i = ROUND( x );
    if( i == sizes[X] )
        i = sizes[X]-1;
    j = ROUND( y );
    if( j == sizes[Y] )
        j = sizes[Y]-1;
    k = ROUND( z );
    if( k == sizes[Z] )
        k = sizes[Z]-1;

    GET_VALUE_3D( voxel_value, volume, i, j, k );

    if( value != (Real *) 0 )
        *value = voxel_value;

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

        if( i == sizes[X]-1 )
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

        if( j == sizes[Y]-1 )
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

        if( k == sizes[Z]-1 )
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
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : trilinear_interpolate_volume
@INPUT      : volume
              x
              y
              z
@OUTPUT     : value
              deriv_x
              deriv_y
              deriv_z
@RETURNS    : 
@DESCRIPTION: Returns the value within the volume, assuming trilinear
              interpolation.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  void   trilinear_interpolate_volume(
    Volume         volume,
    Real           x,
    Real           y,
    Real           z,
    BOOLEAN        x_use_higher,
    BOOLEAN        y_use_higher,
    BOOLEAN        z_use_higher,
    Real           *value,
    Real           *deriv_x,
    Real           *deriv_y,
    Real           *deriv_z )
{
    int                i, j, k, sizes[MAX_DIMENSIONS];
    Real               u, v, w;
    Real               c000, c001, c010, c011, c100, c101, c110, c111;
    Real               c00, c01, c10, c11;
    Real               c0, c1;
    Real               du00, du01, du10, du11, du0, du1;
    Real               dv0, dv1;

    get_volume_sizes( volume, sizes );

    if( x == (Real) (sizes[X]-1) )
    {
        i = sizes[X]-2;
        u = 1.0;
    }
    else
    {
        i = (int) x;
        u = FRACTION( x );
        if( !x_use_higher && u == 0.0 && i > 0 )
        {
            --i;
            u = 1.0;
        }
    }

    if( y == (Real) (sizes[Y]-1) )
    {
        j = sizes[Y]-2;
        v = 1.0;
    }
    else
    {
        j = (int) y;
        v = FRACTION( y );
        if( !y_use_higher && v == 0.0 && j > 0 )
        {
            --j;
            v = 1.0;
        }
    }

    if( z == (Real) (sizes[Z]-1) )
    {
        k = sizes[Z]-2;
        w = 1.0;
    }
    else
    {
        k = (int) z;
        w = FRACTION( z );
        if( !z_use_higher && w == 0.0 && k > 0 )
        {
            --k;
            w = 1.0;
        }
    }

    GET_VALUE_3D( c000, volume, i,   j,   k );
    GET_VALUE_3D( c001, volume, i,   j,   k+1 );
    GET_VALUE_3D( c010, volume, i,   j+1, k );
    GET_VALUE_3D( c011, volume, i,   j+1, k+1 );
    GET_VALUE_3D( c100, volume, i+1, j,   k );
    GET_VALUE_3D( c101, volume, i+1, j,   k+1 );
    GET_VALUE_3D( c110, volume, i+1, j+1, k );
    GET_VALUE_3D( c111, volume, i+1, j+1, k+1 );

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
}

private  void   trivar_interpolate_volume(
    Volume         volume,
    Real           x,
    Real           y,
    Real           z,
    int            degree,
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
    int                i, j, k, tu, tv, tw, ind, n_derivs;
    Real               u, v, w, bound;
    Real               coefs[4*4*4];
    Real               derivs[3*3*3];
    int                sizes[MAX_DIMENSIONS];

    get_volume_sizes( volume, sizes );

    bound = (degree - 2) / 2.0;

    if( x >= (Real) sizes[X] - 1.0 - bound )
    {
        i = sizes[X] - degree;
        u = 1.0;
    }
    else
    {
        if( x < bound )
            x = bound;

        i = (int) ( x - bound );
        u = FRACTION( x - bound );
    }

    if( y >= (Real) sizes[Y] - 1.0 - bound )
    {
        j = sizes[Y] - degree;
        v = 1.0;
    }
    else
    {
        if( y < bound )
            y = bound;

        j = (int) ( y - bound );
        v = FRACTION( y - bound );
    }

    if( z >= (Real) sizes[Z] - 1.0 - bound )
    {
        k = sizes[Z] - degree;
        w = 1.0;
    }
    else
    {
        if( z < bound )
            z = bound;

        k = (int) ( z - bound );
        w = FRACTION( z - bound );
    }

    ind = 0;
    for_less( tu, 0, degree )
    for_less( tv, 0, degree )
    for_less( tw, 0, degree )
    {
        GET_VALUE_3D( coefs[ind], volume, i+tu, j+tv, k+tw );
        ++ind;
    }

    if( deriv_xx != NULL )
        n_derivs = 2;
    else if( deriv_x != NULL )
        n_derivs = 1;
    else
        n_derivs = 0;

    evaluate_trivariate_catmull_spline( u, v, w, degree, coefs, n_derivs,
                                        derivs );

    if( deriv_xx != NULL )
    {
        *deriv_xx = derivs[IJK(2,0,0,n_derivs+1,n_derivs+1)];
        *deriv_xy = derivs[IJK(1,1,0,n_derivs+1,n_derivs+1)];
        *deriv_xz = derivs[IJK(1,0,1,n_derivs+1,n_derivs+1)];
        *deriv_yy = derivs[IJK(0,2,0,n_derivs+1,n_derivs+1)];
        *deriv_yz = derivs[IJK(0,1,1,n_derivs+1,n_derivs+1)];
        *deriv_zz = derivs[IJK(0,0,2,n_derivs+1,n_derivs+1)];
    }

    if( deriv_x != NULL )
    {
        *deriv_x = derivs[IJK(1,0,0,n_derivs+1,n_derivs+1)];
        *deriv_y = derivs[IJK(0,1,0,n_derivs+1,n_derivs+1)];
        *deriv_z = derivs[IJK(0,0,1,n_derivs+1,n_derivs+1)];
    }

    if( value != NULL )
        *value = derivs[IJK(0,0,0,n_derivs+1,n_derivs+1)];
}

private  void   bicubic_interpolate_volume(
    Volume         volume,
    Real           x,
    Real           y,
    Real           z,
    Real           *value,
    Real           *deriv_x,
    Real           *deriv_y,
    Real           *deriv_xx,
    Real           *deriv_xy,
    Real           *deriv_yy )
{
    int                i, j, k, tu, tv, ind, n_derivs;
    Real               u, v;
    Real               coefs[4*4];
    Real               derivs[3*3];
    int                sizes[MAX_DIMENSIONS];

    get_volume_sizes( volume, sizes );

    if( x >= (Real) sizes[X] - 1.5 )
    {
        i = sizes[X]-2;
        u = 1.0;
    }
    else
    {
        i = (int) x;
        u = FRACTION( x );
    }

    if( y >= (Real) sizes[Y] - 1.5 )
    {
        j = sizes[Y]-2;
        v = 1.0;
    }
    else
    {
        j = (int) y;
        v = FRACTION( y );
    }

    if( z == (Real) sizes[Z] - 1.5 )
    {
        k = sizes[Z]-2;
    }
    else
    {
        k = (int) z;
    }

    ind = 0;
    for_less( tu, 0, 4 )
    for_less( tv, 0, 4 )
    {
        GET_VALUE_3D( coefs[ind], volume, i-1+tu, j-1+tv, k );
        ++ind;
    }

    if( deriv_xx != NULL )
        n_derivs = 2;
    else if( deriv_x != NULL )
        n_derivs = 1;
    else
        n_derivs = 0;

    evaluate_bivariate_catmull_spline( u, v, 4, coefs, n_derivs, derivs );

    if( deriv_xx != NULL )
    {
        *deriv_xx = derivs[IJ(2,0,n_derivs+1)];
        *deriv_xy = derivs[IJ(1,1,n_derivs+1)];
        *deriv_yy = derivs[IJ(0,2,n_derivs+1)];
    }

    if( deriv_x != NULL )
    {
        *deriv_x = derivs[IJ(1,0,n_derivs+1)];
        *deriv_y = derivs[IJ(0,1,n_derivs+1)];
    }

    if( value != NULL )
        *value = derivs[IJ(0,0,n_derivs+1)];
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : evaluate_3D_volume
@INPUT      : volume
              x
              y
              z
@OUTPUT     : value
              deriv_x
              deriv_y
              deriv_z
@RETURNS    : 
@DESCRIPTION: Takes a voxel space position and evaluates the value within
              the volume by trilinear interpolation.
              If deriv_x is not a null pointer, then the 3 derivatives are passed
              back.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void   evaluate_3D_volume(
    Volume         volume,
    Real           x,
    Real           y,
    Real           z,
    int            degrees_continuity,
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
    int         sizes[MAX_DIMENSIONS];

    if( get_volume_n_dimensions(volume) != 3 )
    {
        handle_internal_error( "evaluate_3D_volume: volume must be 3D.\n" );
    }

    get_volume_sizes( volume, sizes );

    if( x < -0.5 || x > (Real) sizes[X] - 0.5 ||
        y < -0.5 || y > (Real) sizes[Y] - 0.5 ||
        z < -0.5 || z > (Real) sizes[Z] - 0.5 )
    {
        *value = get_volume_voxel_min( volume );
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
        return;
    }

    if( x < (Real) degrees_continuity * 0.5 ||
        x > (Real) (sizes[X]-1) - (Real) degrees_continuity * 0.5 ||
        y < (Real) degrees_continuity * 0.5 ||
        y > (Real) (sizes[Y]-1) - (Real) degrees_continuity * 0.5 ||
        z < (Real) degrees_continuity * 0.5 ||
        z > (Real) (sizes[Z]-1) - (Real) degrees_continuity * 0.5 )
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
        triconstant_interpolate_volume( volume, x, y, z, value,
                                        deriv_x, deriv_y, deriv_z );
        break;

    case 0:
        trilinear_interpolate_volume( volume, x, y, z,
                                      TRUE, TRUE, TRUE, value,
                                      deriv_x, deriv_y, deriv_z );
        break;

    case 1:
    case 2:
        trivar_interpolate_volume( volume, x, y, z, degrees_continuity+2,
                                   value,
                                   deriv_x, deriv_y, deriv_z,
                                   deriv_xx, deriv_xy, deriv_xz,
                                   deriv_yy, deriv_yz, deriv_zz );
        break;

    default:
        handle_internal_error( "evaluate_3D_volume: invalid continuity" );
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : evaluate_3D_slice
@INPUT      : volume
              x
              y
              z
@OUTPUT     : value
              deriv_x
              deriv_y
              deriv_xx
              deriv_xy
              deriv_yy
@RETURNS    : 
@DESCRIPTION: Takes a voxel position and evaluates the value within
              the volume by bilinear interpolation within the slice.
              If deriv_x is not a null pointer, then the
              derivatives are passed back.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void   evaluate_3D_slice(
    Volume         volume,
    Real           x,
    Real           y,
    Real           z,
    Real           *value,
    Real           *deriv_x,
    Real           *deriv_y,
    Real           *deriv_xx,
    Real           *deriv_xy,
    Real           *deriv_yy )
{
    int           sizes[MAX_DIMENSIONS];

    if( get_volume_n_dimensions(volume) != 3 )
    {
        handle_internal_error( "evaluate_3D_slice: volume must be 3D.\n" );
    }

    get_volume_sizes( volume, sizes );

    if( x < 1.0 || x > (Real) sizes[X] - 2.0 ||
        y < 1.0 || y > (Real) sizes[Y] - 2.0 ||
        z < 1.0 || z > (Real) sizes[Z] - 2.0 )
    {
        *value = get_volume_voxel_min( volume );
        if( deriv_x != (Real *) NULL )
        {
            *deriv_x = 0.0;
            *deriv_y = 0.0;
        }
        if( deriv_xx != (Real *) NULL )
        {
            *deriv_xx = 0.0;
            *deriv_xy = 0.0;
            *deriv_yy = 0.0;
        }
        return;
    }

    bicubic_interpolate_volume( volume, x, y, z, value,
                                deriv_x, deriv_y,
                                deriv_xx, deriv_xy, deriv_yy );
}
