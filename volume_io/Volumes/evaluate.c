#include  <internal_volume_io.h>

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
    Real           values[],
    Real           **first_deriv,
    Real           ***second_deriv,
    BOOLEAN        *inside )
{
    int   n, v, d, dim, n_values, sizes[MAX_DIMENSIONS], n_dims, ind;
    int   start[MAX_DIMENSIONS], n_interp_dims;
    int   end[MAX_DIMENSIONS];
    int   interp_dims[MAX_DIMENSIONS];
    int   n_coefs;
    int   vi[MAX_DIMENSIONS];
    Real  value, fraction[MAX_DIMENSIONS], bound, *coefs;

    if( degrees_continuity > 2 )
    {
        print( "Warning: evaluate_volume(), degrees too large: %d\n",
               degrees_continuity );
        degrees_continuity = 2;
    }

    n_dims = get_volume_n_dimensions(volume);
    get_volume_sizes( volume, sizes );
    n_values = 1;
    n_interp_dims = 0;

    for_less( d, 0, n_dims )
    {
        if( interpolating_dimensions == NULL || interpolating_dimensions[d] )
        {
            if( degrees_continuity > -2 &&
                (voxel[d] < (Real) degrees_continuity * 0.5 ||
                 voxel[d] > (Real)(sizes[d]-1) - (Real)degrees_continuity*0.5) )
            {
                --degrees_continuity;
                for( ; degrees_continuity > -2;  --degrees_continuity )
                {
                    if( voxel[d] >= (Real) degrees_continuity * 0.5 &&
                        voxel[d] <= (Real)(sizes[d]-1) -
                                    (Real)degrees_continuity * 0.5 )
                    {
                        break;
                    }
                }
            }
            ++n_interp_dims;
        }
        else
        {
            n_values *= sizes[d];
        }
    }

    if( inside != NULL )
        *inside = degrees_continuity >= -1;

    if( degrees_continuity < -1 )
    {
        value = get_volume_real_min( volume );

        for_less( v, 0, n_values )
            values[v] = value;
    }

    if( degrees_continuity < 0 && first_deriv != NULL )
    {
        for_less( v, 0, n_values )
            for_less( d, 0, n_interp_dims )
                first_deriv[v][d] = 0.0;
    }

    if( degrees_continuity < 1 && second_deriv != NULL )
    {
        for_less( v, 0, n_values )
            for_less( d, 0, n_interp_dims )
                for_less( dim, 0, n_interp_dims )
                   second_deriv[v][d][dim] = 0.0;
    }

    if( degrees_continuity > -2 )
    {
        bound = degrees_continuity / 2.0;
        n_interp_dims = 0;
        n_coefs = 1;

        for_less( d, 0, n_dims )
        {
            if( interpolating_dimensions == NULL || interpolating_dimensions[d])
            {
                interp_dims[n_interp_dims] = d;
                n_coefs *= 2 + degrees_continuity;
                if( voxel[d] >= (Real) sizes[d] - 1.0 - bound )
                {
                    start[n_interp_dims] = sizes[d] - degrees_continuity - 2;
                    fraction[n_interp_dims] = 1.0;
                }
                else
                {
                    if( voxel[d] < bound )
                        voxel[d] = bound;

                    start[n_interp_dims] = (int) ( voxel[d] - bound );
                    fraction[n_interp_dims] = FRACTION( voxel[d] - bound );
                }

                end[n_interp_dims] = start[d] + degrees_continuity + 2;
                ++n_interp_dims;
            }
        }

        n = 0;
        for_less( d, 0, n_dims )
        {
            if( interpolating_dimensions != NULL &&
                !interpolating_dimensions[d] )
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

        switch( degrees_continuity )
        {
        case -1:
            for_less( v, 0, n_values )
                values[v] = coefs[v];
            break;

        case 0:
        case 1:
        case 2:
            interpolate_volume( n_interp_dims, fraction, n_values,
                                degrees_continuity + 2, coefs,
                                values, first_deriv, second_deriv );
            break;
        }

        FREE( coefs );
    }

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

public  BOOLEAN   evaluate_volume_in_world(
    Volume         volume,
    Real           x,
    Real           y,
    Real           z,
    int            degrees_continuity,
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
    BOOLEAN   interpolating_dimensions[MAX_DIMENSIONS], inside;

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
            return( FALSE );
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
                      degrees_continuity, values, first_deriv, second_deriv,
                      &inside );

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
    }

    return( inside );
}
