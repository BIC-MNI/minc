#include  <internal_volume_io.h>

public  Real  get_volume_voxel_value(
    Volume   volume,
    int      v0,
    int      v1,
    int      v2,
    int      v3,
    int      v4 )
{
    Real   voxel;

    GET_VOXEL( voxel, volume, v0, v1, v2, v3, v4 );

    return( voxel );
}

public  Real  get_volume_real_value(
    Volume   volume,
    int      v0,
    int      v1,
    int      v2,
    int      v3,
    int      v4 )
{
    Real   voxel, value;

    voxel = get_volume_voxel_value( volume, v0, v1, v2, v3, v4 );

    value = CONVERT_VOXEL_TO_VALUE( volume, voxel );

    return( value );
}

public  void  set_volume_voxel_value(
    Volume   volume,
    int      v0,
    int      v1,
    int      v2,
    int      v3,
    int      v4,
    Real     voxel )
{
    SET_VOXEL( volume, v0, v1, v2, v3, v4, voxel );
}

public  void  set_volume_real_value(
    Volume   volume,
    int      v0,
    int      v1,
    int      v2,
    int      v3,
    int      v4,
    Real     value )
{
    Real         voxel;
    Data_types   data_type;

    voxel = CONVERT_VALUE_TO_VOXEL( volume, value );

    data_type = get_volume_data_type( volume );

    if( data_type != FLOAT &&
        data_type != DOUBLE )
    {
        voxel = ROUND( voxel );
    }

    set_volume_voxel_value( volume, v0, v1, v2, v3, v4, voxel );
}

private  void    trilinear_interpolate_volume(
    Real     u,
    Real     v,
    Real     w,
    Real     coefs[],
    Real     *value,
    Real     derivs[] )
{
    Real   du00, du01, du10, du11, c00, c01, c10, c11, c0, c1, du0, du1;
    Real   dv0, dv1, dw;

    du00 = coefs[4] - coefs[0];
    du01 = coefs[5] - coefs[1];
    du10 = coefs[6] - coefs[2];
    du11 = coefs[7] - coefs[3];

    c00 = coefs[0] + u * du00;
    c01 = coefs[1] + u * du01;
    c10 = coefs[2] + u * du10;
    c11 = coefs[3] + u * du11;

    dv0 = c10 - c00;
    dv1 = c11 - c01;

    c0 = c00 + v * dv0;
    c1 = c01 + v * dv1;

    dw = c1 - c0;

    if( value != NULL )
        *value = c0 + w * dw;

    if( derivs != NULL )
    {
        du0 = INTERPOLATE( v, du00, du10 );
        du1 = INTERPOLATE( v, du01, du11 );

        derivs[X] = INTERPOLATE( w, du0, du1 );
        derivs[Y] = INTERPOLATE( w, dv0, dv1 );
        derivs[Z] = dw;
    }
}

private  void   interpolate_volume(
    int      n_dims,
    Real     parameters[],
    int      n_values,
    int      degree,
    Real     coefs[],
    Real     values[],
    Real     **first_deriv,
    Real     ***second_deriv )
{
    int       v, d, d2, n_derivs, derivs_per_value, mult, mult2;
    Real      *derivs;

    if( second_deriv != NULL )
        n_derivs = 2;
    else if( first_deriv != NULL )
        n_derivs = 1;
    else
        n_derivs = 0;

    derivs_per_value = 1;
    for_less( d, 0, n_dims )
        derivs_per_value *= 1 + n_derivs;

    ALLOC( derivs, n_values * derivs_per_value );

    evaluate_interpolating_spline( n_dims, parameters, degree, n_values, coefs,
                                   n_derivs, derivs );

    /* --- derivs is now a one dimensional array representing
           derivs[n_values][1+n_derivs][1+n_derivs]... */

    if( values != NULL )
    {
        for_less( v, 0, n_values )
            values[v] = derivs[v*derivs_per_value];
    }

    if( first_deriv != NULL )
    {
        mult = 1;
        for_down( d, n_dims-1, 0 )
        {
            for_less( v, 0, n_values )
                first_deriv[v][d] = derivs[mult + v*derivs_per_value];

            mult *= 1 + n_derivs;
        }
    }

    if( second_deriv != NULL )
    {
        mult = 1;
        for_down( d, n_dims-1, 0 )
        {
            for_less( v, 0, n_values )
                second_deriv[v][d][d] = derivs[2*mult + v*derivs_per_value];

            mult *= 1 + n_derivs;
        }

        mult = 1;
        for_down( d, n_dims-1, 0 )
        {
            mult2 = 1;

            for_down( d2, n_dims-1, d+1 )
            {
                for_less( v, 0, n_values )
                {
                    second_deriv[v][d][d2] =
                           derivs[mult+mult2+v*derivs_per_value];
                    second_deriv[v][d2][d] =
                           derivs[mult+mult2+v*derivs_per_value];
                }

                mult2 *= 1 + n_derivs;
            }

            mult *= 1 + n_derivs;
        }
    }

    FREE( derivs );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : evaluate_volume
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
              the volume by nearest_neighbour, linear, quadratic, or
              cubic interpolation.
              If first_deriv is not a null pointer, then the first derivatives
              are passed back.  Similarly for the second_deriv.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  int   evaluate_volume(
    Volume         volume,
    Real           voxel[],
    BOOLEAN        interpolating_dimensions[],
    int            degrees_continuity,
    Real           outside_value,
    Real           values[],
    Real           **first_deriv,
    Real           ***second_deriv )
{
    int      n, v, d, dim, n_values, sizes[MAX_DIMENSIONS], n_dims, ind;
    int      start[MAX_DIMENSIONS], n_interp_dims;
    int      end[MAX_DIMENSIONS];
    int      interp_dims[MAX_DIMENSIONS];
    int      n_coefs;
    int      vi[MAX_DIMENSIONS];
    Real     fraction[MAX_DIMENSIONS], bound, *coefs;
    BOOLEAN  fully_inside, fully_outside;

    if( degrees_continuity < -1 || degrees_continuity > 2 )
    {
        print( "Warning: evaluate_volume(), degrees invalid: %d\n",
               degrees_continuity );
        degrees_continuity = 0;
    }

    n_dims = get_volume_n_dimensions(volume);
    get_volume_sizes( volume, sizes );

    bound = (Real) degrees_continuity / 2.0;
    n_interp_dims = 0;
    n_values = 1;
    n_coefs = 1;

    fully_inside = TRUE;
    fully_outside = TRUE;

    for_less( d, 0, n_dims )
    {
        if( interpolating_dimensions == NULL || interpolating_dimensions[d])
        {
            interp_dims[n_interp_dims] = d;
            start[n_interp_dims] =       FLOOR( voxel[d] - bound );
            fraction[n_interp_dims] = FRACTION( voxel[d] - bound );
            end[n_interp_dims] = start[n_interp_dims] + degrees_continuity + 2;
            n_coefs *= 2 + degrees_continuity;

            if( start[n_interp_dims] < 0 || end[n_interp_dims] > sizes[d] )
                fully_inside = FALSE;

            if( start[n_interp_dims] < sizes[d] && end[n_interp_dims] > 0 )
                fully_outside = FALSE;

            ++n_interp_dims;
        }
        else
            n_values *= sizes[d];
    }

    if( (fully_outside || degrees_continuity < 0) && first_deriv != NULL )
    {
        for_less( v, 0, n_values )
            for_less( d, 0, n_interp_dims )
                first_deriv[v][d] = 0.0;
    }

    if( (fully_outside || degrees_continuity < 1) && second_deriv != NULL )
    {
        for_less( v, 0, n_values )
            for_less( d, 0, n_interp_dims )
                for_less( dim, 0, n_interp_dims )
                   second_deriv[v][d][dim] = 0.0;
    }

    if( fully_outside )
    {
        for_less( v, 0, n_values )
            values[v] = outside_value;

        return( n_values );
    }

    n = 0;
    for_less( d, 0, n_dims )
    {
        if( interpolating_dimensions != NULL && !interpolating_dimensions[d] )
        {
            interp_dims[n_interp_dims+n] = d;
            start[n_interp_dims+n] = 0;
            end[n_interp_dims+n] = sizes[d];
            ++n;
        }
    }

    for_less( d, n_dims, MAX_DIMENSIONS )
    {
        start[d] = 0;
        end[d] = 1;
        interp_dims[d] = d;
    }

    ALLOC( coefs, n_values * n_coefs );

    if( fully_inside )
    {
        ind = 0;
        for_less( vi[interp_dims[0]], start[0], end[0] )
        for_less( vi[interp_dims[1]], start[1], end[1] )
        for_less( vi[interp_dims[2]], start[2], end[2] )
        for_less( vi[interp_dims[3]], start[3], end[3] )
        for_less( vi[interp_dims[4]], start[4], end[4] )
        {
            GET_VALUE( coefs[ind], volume, vi[0], vi[1], vi[2], vi[3], vi[4] );
            ++ind;
        }
    }
    else
    {
        ind = 0;
        for_less( vi[interp_dims[0]], start[0], end[0] )
        for_less( vi[interp_dims[1]], start[1], end[1] )
        for_less( vi[interp_dims[2]], start[2], end[2] )
        for_less( vi[interp_dims[3]], start[3], end[3] )
        for_less( vi[interp_dims[4]], start[4], end[4] )
        {
            for_less( d, 0, n_dims )
            {
                if( vi[d] < 0 || vi[d] >= sizes[d] )
                    break;
            }

            if( d == n_dims )
            {
                GET_VALUE( coefs[ind], volume,
                           vi[0], vi[1], vi[2], vi[3], vi[4] );
            }
            else
                coefs[ind] = outside_value;
            ++ind;
        }

        if( ind > n_values * n_coefs )
            handle_internal_error( "overflow" );
    }

    switch( degrees_continuity )
    {
    case -1:
        for_less( v, 0, n_values )
            values[v] = coefs[v];
        break;

    case 0:
    case 1:
    case 2:
        /*--- check for the common case which must be done fast */

        if( degrees_continuity == 0 && n_interp_dims == 3 && n_values == 1 )
        {
            Real   *deriv;

            if( first_deriv == NULL )
                deriv = NULL;
            else
                deriv = first_deriv[0];

            trilinear_interpolate_volume( fraction[0], fraction[1], fraction[2],
                                          coefs, values, deriv );
        }
        else
        {
            interpolate_volume( n_interp_dims, fraction, n_values,
                                degrees_continuity + 2, coefs,
                                values, first_deriv, second_deriv );
        }

        break;
    }

    FREE( coefs );

    return( n_values );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : evaluate_volume_in_world
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

public  void   evaluate_volume_in_world(
    Volume         volume,
    Real           x,
    Real           y,
    Real           z,
    int            degrees_continuity,
    Real           outside_value,
    Real           values[],
    Real           deriv_x[],
    Real           deriv_y[],
    Real           deriv_z[],
    Real           deriv_xx[],
    Real           deriv_xy[],
    Real           deriv_xz[],
    Real           deriv_yy[],
    Real           deriv_yz[],
    Real           deriv_zz[] )
{
    Real      ignore;
    Real      voxel[MAX_DIMENSIONS];
    Real      **first_deriv, ***second_deriv;
    Real      t[MAX_DIMENSIONS][MAX_DIMENSIONS];
    int       c, d, dim, v, n_values, n_interp_dims, n_dims, axis;
    int       sizes[MAX_DIMENSIONS];
    BOOLEAN   interpolating_dimensions[MAX_DIMENSIONS];

    convert_world_to_voxel( volume, x, y, z, voxel );
    get_volume_sizes( volume, sizes );

    n_dims = get_volume_n_dimensions( volume );
    for_less( d, 0, n_dims )
        interpolating_dimensions[d] = FALSE;

    for_less( d, 0, N_DIMENSIONS )
    {
        axis = volume->spatial_axes[d];
        if( axis < 0 )
        {
            print("evaluate_volume_in_world(): must have 3 spatial axes.\n");
            return;
        }

        interpolating_dimensions[axis] = TRUE;
    }

    
    n_values = 1;
    for_less( d, 0, n_dims )
    {
        if( !interpolating_dimensions[d] )
            n_values *= sizes[d];
    }

    if( deriv_x != NULL )
    {
        ALLOC2D( first_deriv, n_values, N_DIMENSIONS );
    }
    else
        first_deriv = NULL;

    if( deriv_xx != NULL )
    {
        ALLOC3D( second_deriv, n_values, N_DIMENSIONS, N_DIMENSIONS );
    }
    else
        second_deriv = NULL;

    n_values = evaluate_volume( volume, voxel, interpolating_dimensions,
                      degrees_continuity, outside_value,
                      values, first_deriv, second_deriv );

    if( deriv_x != NULL )
    {
        for_less( v, 0, n_values )
        {
            n_interp_dims = 0;
            for_less( c, 0, n_dims )
            {
                if( interpolating_dimensions[c] )
                {
                    voxel[c] = first_deriv[v][n_interp_dims];
                    ++n_interp_dims;
                }
                else
                    voxel[c] = 0.0;
            }

            convert_voxel_normal_vector_to_world( volume, voxel,
                                   &deriv_x[v], &deriv_y[v], &deriv_z[v] );
        }

        FREE2D( first_deriv );
    }

    if( deriv_xx != (Real *) 0 )
    {
        for_less( v, 0, n_values )
        {
            for_less( dim, 0, N_DIMENSIONS )
            {
                n_interp_dims = 0;
                for_less( c, 0, n_dims )
                {
                    if( interpolating_dimensions[c] )
                    {
                        voxel[c] = second_deriv[v][dim][n_interp_dims];
                        ++n_interp_dims;
                    }
                    else
                        voxel[c] = 0.0;
                }

                convert_voxel_normal_vector_to_world( volume, voxel,
                      &t[dim][volume->spatial_axes[X]],
                      &t[dim][volume->spatial_axes[Y]],
                      &t[dim][volume->spatial_axes[Z]] );
            }
    
            for_less( c, 0, n_dims )
                voxel[c] = t[c][volume->spatial_axes[X]];

            convert_voxel_normal_vector_to_world( volume, voxel,
                                              &deriv_xx[v], &ignore, &ignore );
    
            for_less( c, 0, n_dims )
                voxel[c] = t[c][volume->spatial_axes[Y]];

            convert_voxel_normal_vector_to_world( volume, voxel,
                                          &deriv_xy[v], &deriv_yy[v], &ignore );
    
            for_less( c, 0, n_dims )
                voxel[c] = t[c][volume->spatial_axes[Z]];

            convert_voxel_normal_vector_to_world( volume, voxel,
                                  &deriv_xz[v], &deriv_yz[v], &deriv_zz[v] );
        }

        FREE3D( second_deriv );
    }
}
