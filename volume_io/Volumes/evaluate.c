#include  <internal_volume_io.h>

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_volume_voxel_value
@INPUT      : volume
              v0          voxel indices
              v1
              v2
              v3
              v4
@OUTPUT     : 
@RETURNS    : Voxel value
@DESCRIPTION: Returns the voxel at the specified voxel index.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Mar. 20, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_volume_real_value
@INPUT      : volume
              v0          voxel indices
              v1
              v2
              v3
              v4
@OUTPUT     : 
@RETURNS    : Real value
@DESCRIPTION: Returns the volume real value at the specified voxel index.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Mar. 20, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_volume_voxel_value
@INPUT      : volume
              v0          voxel indices
              v1
              v2
              v3
              v4
              voxel
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the voxel at the specified voxel index.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Mar. 20, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_volume_real_value
@INPUT      : volume
              v0          voxel indices
              v1
              v2
              v3
              v4
              value
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the volume real value at the specified voxel index.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Mar. 20, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : trilinear_interpolate_volume
@INPUT      : u              0 to 1 pos
              v              0 to 1 pos
              w              0 to 1 pos
              coefs          8 coeficients
@OUTPUT     : value    
              derivs
@RETURNS    : 
@DESCRIPTION: Computes the trilinear interpolation of the 8 coeficients, passing
              the value back in the 'value' parameter.  If the derivs parameter
              is not null, then the 3 derivatives are also computed and
              passed back.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Mar. 20, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

    /*--- get the 4 differences in the u direction */

    du00 = coefs[4] - coefs[0];
    du01 = coefs[5] - coefs[1];
    du10 = coefs[6] - coefs[2];
    du11 = coefs[7] - coefs[3];

    /*--- reduce to a 2D problem, by interpolating in the u direction */

    c00 = coefs[0] + u * du00;
    c01 = coefs[1] + u * du01;
    c10 = coefs[2] + u * du10;
    c11 = coefs[3] + u * du11;

    /*--- get the 2 differences in the v direction for the 2D problem */

    dv0 = c10 - c00;
    dv1 = c11 - c01;

    /*--- reduce 2D to a 1D problem, by interpolating in the v direction */

    c0 = c00 + v * dv0;
    c1 = c01 + v * dv1;

    /*--- get the 1 difference in the w direction for the 1D problem */

    dw = c1 - c0;

    /*--- if the value is desired, interpolate in 1D to get the value */

    if( value != NULL )
        *value = c0 + w * dw;

    /*--- if the derivatives are desired, compute them */

    if( derivs != NULL )
    {
        /*--- reduce the 2D u derivs to 1D */

        du0 = INTERPOLATE( v, du00, du10 );
        du1 = INTERPOLATE( v, du01, du11 );

        /*--- interpolate the 1D problems in w, or for Z deriv, just use dw */

        derivs[X] = INTERPOLATE( w, du0, du1 );
        derivs[Y] = INTERPOLATE( w, dv0, dv1 );
        derivs[Z] = dw;
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : interpolate_volume
@INPUT      : n_dims        - number of dimensions to interpolate
              parameters[]  - 0 to 1 parameters for each dim
              n_values      - number of values to interpolate
              degree        - degree of interpolation, 4 == cubic
              coefs         - [degree*degree*degree... *n_values] coeficients
@OUTPUT     : values        - pass back values
              first_deriv   - pass first derivs [n_values][n_dims]
              second_deriv  - pass back values  [n_values][n_dims][n_dims]
@RETURNS    : 
@DESCRIPTION: Computes the interpolation of the box specified by coefs and
              its derivatives, if necessary.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Mar. 20, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

    /*--- determine how many derivatives should be computed */

    if( second_deriv != NULL )
        n_derivs = 2;
    else if( first_deriv != NULL )
        n_derivs = 1;
    else
        n_derivs = 0;

    /*--- compute the total number of values, 1st and 2nd derivatives per val*/

    derivs_per_value = 1;
    for_less( d, 0, n_dims )
        derivs_per_value *= 1 + n_derivs;

    /*--- make storage for the spline routines to place the answers */

    ALLOC( derivs, n_values * derivs_per_value );

    /*--- evaluate the interpolating spline */

    evaluate_interpolating_spline( n_dims, parameters, degree, n_values, coefs,
                                   n_derivs, derivs );

    /*--- derivs is now a one dimensional array representing
          derivs[n_values][1+n_derivs][1+n_derivs]...,
          where derivs[0][0][0][0]... = the interpolated value for 1st comp,
                derivs[1][0][0][0]... = the interpolated value for 2nd comp,
                derivs[n_values-1][0][0][0]... = interpolated value for last,
                derivs[0][1][0][0]... = derivative of 1st comp wrt x
                derivs[0][0][1][0]... = derivative of 1st comp wrt y
                derivs[1][1][0][0]... = derivative of 2nd comp wrt x, etc. */

    if( values != NULL )
    {
        for_less( v, 0, n_values )
            values[v] = derivs[v*derivs_per_value];
    }

    /*--- fill in the first derivatives, if required */

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

    /*--- fill in the second derivatives, if required */

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
              voxel
              interpolating_dimensions - whether each dimension is interpolated
              degrees_continuity
              use_linear_at_edge
              outside_value
@OUTPUT     : values
              first_deriv
              second_deriv
@RETURNS    : 
@DESCRIPTION: Takes a voxel space position and evaluates the value within
              the volume by nearest_neighbour, linear, quadratic, or
              cubic interpolation. degrees_continuity == 2 corresponds to
              cubic, 1 for quadratic, etc.
              If first_deriv is not a null pointer, then the first derivatives
              are passed back.  Similarly for the second_deriv.
              If use_linear_at_edge is TRUE, then near the boundaries, either
              linear or nearest neighbour interpolation is used, even if cubic
              is specified by the degrees_continuity.
              If use_linear_at_edge is FALSE, then the 'outside_value' is used
              to provide coefficients for outside the volume, and the degree
              specified by degrees_continuity is used.

              Each dimension may or may not be interpolated, specified by the
              interpolating_dimensions parameter.  For instance, a 4D volume
              of x,y,z,RGB may be interpolated in 3D (x,y,z) for each of the
              3 RGB components, with one call to evaluate_volume.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  int   evaluate_volume(
    Volume         volume,
    Real           voxel[],
    BOOLEAN        interpolating_dimensions[],
    int            degrees_continuity,
    BOOLEAN        use_linear_at_edge,
    Real           outside_value,
    Real           values[],
    Real           **first_deriv,
    Real           ***second_deriv )
{
    int      inc0, inc1, inc2, inc3, inc4, inc[MAX_DIMENSIONS];
    int      ind0, spline_degree;
    int      start0, start1, start2, start3, start4;
    int      end0, end1, end2, end3, end4;
    int      v0, v1, v2, v3, v4, next_d;
    int      n, v, d, dim, n_values, sizes[MAX_DIMENSIONS], n_dims;
    int      start[MAX_DIMENSIONS], n_interp_dims;
    int      end[MAX_DIMENSIONS];
    int      interp_dims[MAX_DIMENSIONS];
    int      n_coefs;
    Real     fraction[MAX_DIMENSIONS], bound, *coefs, pos;
    BOOLEAN  fully_inside, fully_outside;

    /*--- check if the degrees continuity is between nearest neighbour
          and cubic */

    if( degrees_continuity < -1 || degrees_continuity > 2 )
    {
        print( "Warning: evaluate_volume(), degrees invalid: %d\n",
               degrees_continuity );
        degrees_continuity = 0;
    }

    n_dims = get_volume_n_dimensions(volume);
    get_volume_sizes( volume, sizes );

    bound = (Real) degrees_continuity / 2.0;

    /*--- if we must use linear interpolation near the boundaries, then
          check if we are near the boundaries, and adjust the degrees_continuity
          accordingly */

    if( use_linear_at_edge )
    {
        for_less( d, 0, n_dims )
        {
            if( interpolating_dimensions == NULL || interpolating_dimensions[d])
            {
                while( degrees_continuity >= 0 &&
                       (voxel[d] < bound  ||
                        voxel[d] > (Real) sizes[d] - 1.0 - bound) )
                {
                    --degrees_continuity;
                    if( degrees_continuity == 1 )
                        degrees_continuity = 0;
                    bound = (Real) degrees_continuity / 2.0;
                }
            }
        }
    }

    /*--- now check which dimensions are being interpolated.  Also, compute
          how many values must be interpolated, which are all the values not
          in the interpolated dimensions */

    n_interp_dims = 0;
    n_values = 1;
    n_coefs = 1;
    spline_degree = degrees_continuity + 2;

    fully_inside = TRUE;
    fully_outside = FALSE;

    for_less( d, 0, n_dims )
    {
        if( interpolating_dimensions == NULL || interpolating_dimensions[d])
        {
            interp_dims[n_interp_dims] = d;
            pos = voxel[d] - bound;
            start[d] =       FLOOR( pos );
            fraction[n_interp_dims] = pos - start[d];
            end[d] = start[d] + spline_degree;
            n_coefs *= spline_degree;

            if( start[d] < 0 || end[d] > sizes[d] )
            {
                fully_inside = FALSE;

                if( end[d] <= 0 || start[d] >= sizes[d] )
                {
                    fully_outside = TRUE;
                    break;
                }
            }

            ++n_interp_dims;
        }
        else
            n_values *= sizes[d];
    }

    /*--- check if the first derivatives are uncomputable */

    if( first_deriv != NULL && (fully_outside || degrees_continuity < 0) )
    {
        for_less( v, 0, n_values )
            for_less( d, 0, n_interp_dims )
                first_deriv[v][d] = 0.0;
    }

    /*--- check if the second derivatives are uncomputable */

    if( second_deriv != NULL && (fully_outside || degrees_continuity < 1) )
    {
        for_less( v, 0, n_values )
            for_less( d, 0, n_interp_dims )
                for_less( dim, 0, n_interp_dims )
                   second_deriv[v][d][dim] = 0.0;
    }

    /*--- check if the values are uncomputable, i.e., outside volume */

    if( fully_outside )
    {
        if( values != NULL )
        {
            for_less( v, 0, n_values )
                values[v] = outside_value;
        }

        return( n_values );
    }

    /*--- add the non-interpolated dimensions to the list of dimensions, in
          order, after the interpolated dimensions */

    n = 0;
    for_less( d, 0, n_dims )
    {
        if( interpolating_dimensions != NULL && !interpolating_dimensions[d] )
        {
            interp_dims[n_interp_dims+n] = d;
            start[d] = 0;
            end[d] = sizes[d];
            ++n;
        }
    }

    /*--- make room for the coeficients */

    ALLOC( coefs, n_values * n_coefs );

    /*--- compute the increments in the coefs[] array for each dimension,
          in order to simulate a multidimensional array with a single dim
          array, coefs */

    inc[interp_dims[n_dims-1]] = 1;
    for_down( d, n_dims-2, 0 )
    {
        next_d = interp_dims[d+1];
        inc[interp_dims[d]] = inc[next_d] * (end[next_d] - start[next_d]);
    }

    /*--- figure out the offset within coefs.  If we are inside, the offset
          is zero, since all coefs must be filled in.  If we are partially
          inside, set the offset to the first coef within the volume. */

    ind0 = 0;

    if( !fully_inside )
    {
        for_less( d, 0, n_dims )
        {
            if( start[d] < 0 )
            {
                ind0 += -start[d] * inc[d];
                start[d] = 0;
            }

            if( end[d] > sizes[d] )
                end[d] = sizes[d];
        }

        for_less( v, 0, n_values * n_coefs )
            coefs[v] = outside_value;

        /*--- adjust the inc stride for stepping through coefs to account
              for the additions of the inner loops */

        for_less( d, 0, n_dims-1 )
            inc[d] -= inc[d+1] * (end[d+1] - start[d+1]);
    }
    else
    {
        /*--- adjust the inc stride for stepping through coefs to account
              for the additions of the inner loops */

        for_less( d, 0, n_dims-1 )
            inc[d] -= inc[d+1] * spline_degree;
    }

    /*--- for speed, use non-array variables for the loops */

    start0 = start[0];
    start1 = start[1];
    start2 = start[2];
    start3 = start[3];
    start4 = start[4];

    end0 = end[0];
    end1 = end[1];
    end2 = end[2];
    end3 = end[3];
    end4 = end[4];

    inc0 = inc[0];
    inc1 = inc[1];
    inc2 = inc[2];
    inc3 = inc[3];
    inc4 = inc[4];

    /*--- get the coefs[] from the volume.  For speed, do each dimension
          separately */

    switch( n_dims )
    {
    case 1:
        for_less( v0, start0, end0 )
        {
            GET_VALUE_1D( coefs[ind0], volume, v0 );
            ind0 += inc0;
        }
        break;

    case 2:
        for_less( v0, start0, end0 )
        {
            for_less( v1, start1, end1 )
            {
                GET_VALUE_2D( coefs[ind0], volume, v0, v1 );
                ind0 += inc1;
            }
            ind0 += inc0;
        }
        break;

    case 3:
        for_less( v0, start0, end0 )
        {
            for_less( v1, start1, end1 )
            {
                for_less( v2, start2, end2 )
                {
                    GET_VALUE_3D( coefs[ind0], volume, v0, v1, v2 );
                    ind0 += inc2;
                }
                ind0 += inc1;
            }
            ind0 += inc0;
        }
        break;

    case 4:
        for_less( v0, start0, end0 )
        {
            for_less( v1, start1, end1 )
            {
                for_less( v2, start2, end2 )
                {
                    for_less( v3, start3, end3 )
                    {
                        GET_VALUE_4D( coefs[ind0], volume, v0, v1, v2, v3 );
                        ind0 += inc3;
                    }
                    ind0 += inc2;
                }
                ind0 += inc1;
            }
            ind0 += inc0;
        }
        break;

    case 5:
        for_less( v0, start0, end0 )
        {
            for_less( v1, start1, end1 )
            {
                for_less( v2, start2, end2 )
                {
                    for_less( v3, start3, end3 )
                    {
                        for_less( v4, start4, end4 )
                        {
                            GET_VALUE_5D( coefs[ind0], volume,
                                          v0, v1, v2, v3, v4 );
                            ind0 += inc4;
                        }
                        ind0 += inc3;
                    }
                    ind0 += inc2;
                }
                ind0 += inc1;
            }
            ind0 += inc0;
        }
        break;
    }

    /*--- now that we have the coeficients, do the interpolation */

    switch( degrees_continuity )
    {
    case -1:                        /*--- nearest neighbour interpolation */
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
                                spline_degree, coefs,
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
              use_linear_at_edge
              outside_value
@OUTPUT     : values
              deriv_x
              deriv_y
              deriv_z
              deriv_xx
              deriv_xy
              deriv_xz
              deriv_yy
              deriv_yz
              deriv_zz
@RETURNS    : 
@DESCRIPTION: Takes a world space position and evaluates the value within
              the volume.
              If deriv_x is not a null pointer, then the 3 derivatives are
              passed back.  If deriv_xx is not null, then the 6 second
              derivatives are passed back.  If the volume is 3D, then only
              one value, and one derivative per deriv_x,etc. is passed back.
              If the volume has more than 3 dimensions, say 5 dimensions, with
              dimensions 3 and 4 being the non-spatial dimensions, then there
              will be sizes[3] * sizes[4] values passed back.  The derivatives
              are converted to world space.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void   evaluate_volume_in_world(
    Volume         volume,
    Real           x,
    Real           y,
    Real           z,
    int            degrees_continuity,
    BOOLEAN        use_linear_at_edge,
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
    Real      t[N_DIMENSIONS][MAX_DIMENSIONS];
    int       c, d, dim, v, n_values, n_dims, axis;
    int       sizes[MAX_DIMENSIONS], dims_interpolated[N_DIMENSIONS];
    BOOLEAN   interpolating_dimensions[MAX_DIMENSIONS];

    /*--- convert the world space to a voxel coordinate */

    convert_world_to_voxel( volume, x, y, z, voxel );
    get_volume_sizes( volume, sizes );

    /*--- initialize all dimensions to not being interpolated */

    n_dims = get_volume_n_dimensions( volume );
    for_less( d, 0, n_dims )
        interpolating_dimensions[d] = FALSE;

    /*--- set each spatial dimension to being interpolated */

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

    /*--- compute the number of values, the product of the sizes of the
          non-interpolating dimensions */
    
    n_values = 1;
    for_less( d, 0, n_dims )
    {
        if( !interpolating_dimensions[d] )
            n_values *= sizes[d];
    }

    /*--- make room for the first derivative, if necessary */

    if( deriv_x != NULL )
    {
        ALLOC2D( first_deriv, n_values, N_DIMENSIONS );
    }
    else
        first_deriv = NULL;

    /*--- make room for the second derivative, if necessary */

    if( deriv_xx != NULL )
    {
        ALLOC3D( second_deriv, n_values, N_DIMENSIONS, N_DIMENSIONS );
    }
    else
        second_deriv = NULL;

    /*--- evaluate the volume and derivatives in voxel space */

    n_values = evaluate_volume( volume, voxel, interpolating_dimensions,
                      degrees_continuity, use_linear_at_edge, outside_value,
                      values, first_deriv, second_deriv );

    /*--- if the derivative is desired, convert the voxel derivative
          to world space */

    if( deriv_x != NULL || deriv_xx != NULL )
    {
        /*--- figure out the dimensions interpolated, in order */

        dim = 0;
        for_less( d, 0, n_dims )
        {
            if( interpolating_dimensions[d] )
            {
                dims_interpolated[dim] = d;
                ++dim;
            }
        }
    }

    if( deriv_x != NULL )
    {
        for_less( v, 0, n_values )    /*--- convert the deriv of each value */
        {
            /*--- get the voxel coordinates of the first derivative */

            for_less( c, 0, N_DIMENSIONS )
                voxel[dims_interpolated[c]] = first_deriv[v][c];

            /*--- convert the voxel-space derivative to a world derivative */

            convert_voxel_normal_vector_to_world( volume, voxel,
                                   &deriv_x[v], &deriv_y[v], &deriv_z[v] );
        }

        FREE2D( first_deriv );
    }

    /*--- if the derivative is desired, convert the voxel derivative
          to world space */

    if( deriv_xx != (Real *) 0 )
    {
        for_less( v, 0, n_values )    /*--- convert the deriv of each value */
        {
            /*--- get the voxel coordinates of the first derivative */

            for_less( dim, 0, N_DIMENSIONS )
            {
                for_less( c, 0, N_DIMENSIONS )
                    voxel[dims_interpolated[c]] = second_deriv[v][dim][c];

                /*--- convert the voxel-space derivative to a world derivative*/

                convert_voxel_normal_vector_to_world( volume, voxel,
                      &t[X][dims_interpolated[dim]],
                      &t[Y][dims_interpolated[dim]],
                      &t[Z][dims_interpolated[dim]] );
            }

            /*--- now convert the results to world */
    
            convert_voxel_normal_vector_to_world( volume, t[X],
                                              &deriv_xx[v], &ignore, &ignore );
    
            convert_voxel_normal_vector_to_world( volume, t[Y],
                                          &deriv_xy[v], &deriv_yy[v], &ignore );
    
            convert_voxel_normal_vector_to_world( volume, t[Z],
                                  &deriv_xz[v], &deriv_yz[v], &deriv_zz[v] );
        }

        FREE3D( second_deriv );
    }
}
