/* ----------------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

#include  <internal_volume_io.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Volumes/get_hyperslab.c,v 1.1 1996-05-07 14:01:43 david Exp $";
#endif

public  void  convert_voxels_to_values(
    Volume   volume,
    int      n_voxels,
    Real     voxels[],
    Real     values[] )
{
    int    v;
    Real   scale, trans;

    if( !volume->real_range_set )
    {
        if( voxels != values )
        {
            for_less( v, 0, n_voxels )
                values[v] = voxels[v];
        }
        return;
    }

    scale = volume->real_value_scale;
    trans = volume->real_value_translation;

    for_less( v, 0, n_voxels )
        values[v] = scale * voxels[v] + trans;
}

public  void  get_volume_value_hyperslab(
    Volume   volume,
    int      v0,
    int      v1,
    int      v2,
    int      v3,
    int      v4,
    int      n0,
    int      n1,
    int      n2,
    int      n3,
    int      n4,
    Real     values[] )
{
    switch( get_volume_n_dimensions(volume) )
    {
    case 1:
        get_volume_value_hyperslab_1d( volume, v0, n0, values );
        break;
    case 2:
        get_volume_value_hyperslab_2d( volume, v0, v1, n0, n1, values );
        break;
    case 3:
        get_volume_value_hyperslab_3d( volume, v0, v1, v2, n0, n1, n2, values );
        break;
    case 4:
        get_volume_value_hyperslab_4d( volume, v0, v1, v2, v3,
                                       n0, n1, n2, n3, values );
        break;
    case 5:
        get_volume_value_hyperslab_5d( volume, v0, v1, v2, v3, v4,
                                       n0, n1, n2, n3, n4, values );
        break;
    }
}

public  void  get_volume_value_hyperslab_5d(
    Volume   volume,
    int      v0,
    int      v1,
    int      v2,
    int      v3,
    int      v4,
    int      n0,
    int      n1,
    int      n2,
    int      n3,
    int      n4,
    Real     values[] )
{
    get_volume_voxel_hyperslab_5d( volume, v0, v1, v2, v3, v4,
                                   n0, n1, n2, n3, n4, values );

    convert_voxels_to_values( volume, n0 * n1 * n2 * n3 * n4, values, values );
}

public  void  get_volume_value_hyperslab_4d(
    Volume   volume,
    int      v0,
    int      v1,
    int      v2,
    int      v3,
    int      n0,
    int      n1,
    int      n2,
    int      n3,
    Real     values[] )
{
    get_volume_voxel_hyperslab_4d( volume, v0, v1, v2, v3,
                                   n0, n1, n2, n3, values );

    convert_voxels_to_values( volume, n0 * n1 * n2 * n3, values, values );
}

public  void  get_volume_value_hyperslab_3d(
    Volume   volume,
    int      v0,
    int      v1,
    int      v2,
    int      n0,
    int      n1,
    int      n2,
    Real     values[] )
{
    get_volume_voxel_hyperslab_3d( volume, v0, v1, v2, n0, n1, n2, values );

    convert_voxels_to_values( volume, n0 * n1 * n2, values, values );
}

public  void  get_volume_value_hyperslab_2d(
    Volume   volume,
    int      v0,
    int      v1,
    int      n0,
    int      n1,
    Real     values[] )
{
    get_volume_voxel_hyperslab_2d( volume, v0, v1, n0, n1, values );

    convert_voxels_to_values( volume, n0 * n1, values, values );
}

public  void  get_volume_value_hyperslab_1d(
    Volume   volume,
    int      v0,
    int      n0,
    Real     values[] )
{
    get_volume_voxel_hyperslab_1d( volume, v0, n0, values );

    convert_voxels_to_values( volume, n0, values, values );
}

private  void  slow_get_volume_voxel_hyperslab(
    Volume   volume,
    int      v0,
    int      v1,
    int      v2,
    int      v3,
    int      v4,
    int      n0,
    int      n1,
    int      n2,
    int      n3,
    int      n4,
    Real     values[] )
{
    int    ind, i0, i1, i2, i3, i4, n_dims;

    n_dims = get_volume_n_dimensions( volume );

    if( n_dims < 5 )
        n4 = 1;
    if( n_dims < 4 )
        n3 = 1;
    if( n_dims < 3 )
        n2 = 1;
    if( n_dims < 2 )
        n1 = 1;
    if( n_dims < 1 )
        n0 = 1;

    ind = 0;
    for_less( i0, 0, n0 )
    for_less( i1, 0, n1 )
    for_less( i2, 0, n2 )
    for_less( i3, 0, n3 )
    for_less( i4, 0, n4 )
    {
        values[ind] = get_volume_voxel_value( volume,
                                              v0 + i0,
                                              v1 + i1,
                                              v2 + i2,
                                              v3 + i3,
                                              v4 + i4 );
        ++ind;
    }
}

public  void  get_volume_voxel_hyperslab_5d(
    Volume   volume,
    int      v0,
    int      v1,
    int      v2,
    int      v3,
    int      v4,
    int      n0,
    int      n1,
    int      n2,
    int      n3,
    int      n4,
    Real     values[] )
{
    int              sizes[MAX_DIMENSIONS];
    int              ind, step0, step1, step2, step3;
    int              i0, i1, i2, i3, i4;
    unsigned  char   *unsigned_byte_ptr;
    signed  char     *signed_byte_ptr;
    unsigned  short  *unsigned_short_ptr;
    signed  short    *signed_short_ptr;
    unsigned  long   *unsigned_long_ptr;
    signed  long     *signed_long_ptr;
    float            *float_ptr;
    double           *double_ptr;
    void             *void_ptr;

    if( volume->is_cached_volume )
    {
        slow_get_volume_voxel_hyperslab( volume, v0, v1, v2, v3, v4,
                                         n0, n1, n2, n3, n4, values );
        return;
    }

    get_volume_sizes( volume, sizes );

    GET_MULTIDIM_PTR_5D( void_ptr, volume->array, v0, v1, v2, v3, v4 )

    step3 = sizes[4];
    step2 = sizes[3] * step3;
    step1 = sizes[2] * step2;
    step0 = sizes[1] * step1;
    step0 -= n1 * step1;
    step1 -= n2 * step2;
    step2 -= n3 * step3;
    step3 -= n4 * 1;

    ind = 0;

    switch( get_volume_data_type(volume) )
    {
    case UNSIGNED_BYTE:
        unsigned_byte_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            for_less( i1, 0, n1 )
            {
                for_less( i2, 0, n2 )
                {
                    for_less( i3, 0, n3 )
                    {
                        for_less( i4, 0, n4 )
                        {
                            values[ind] = (Real) *unsigned_byte_ptr;
                            ++ind;
                            ++unsigned_byte_ptr;
                        }
                        unsigned_byte_ptr += step3;
                    }
                    unsigned_byte_ptr += step2;
                }
                unsigned_byte_ptr += step1;
            }
            unsigned_byte_ptr += step0;
        }
        break;

    case SIGNED_BYTE:
        signed_byte_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            for_less( i1, 0, n1 )
            {
                for_less( i2, 0, n2 )
                {
                    for_less( i3, 0, n3 )
                    {
                        for_less( i4, 0, n4 )
                        {
                            values[ind] = (Real) *signed_byte_ptr;
                            ++ind;
                            ++signed_byte_ptr;
                        }
                        signed_byte_ptr += step3;
                    }
                    signed_byte_ptr += step2;
                }
                signed_byte_ptr += step1;
            }
            signed_byte_ptr += step0;
        }
        break;

    case UNSIGNED_SHORT:
        unsigned_short_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            for_less( i1, 0, n1 )
            {
                for_less( i2, 0, n2 )
                {
                    for_less( i3, 0, n3 )
                    {
                        for_less( i4, 0, n4 )
                        {
                            values[ind] = (Real) *unsigned_short_ptr;
                            ++ind;
                            ++unsigned_short_ptr;
                        }
                        unsigned_short_ptr += step3;
                    }
                    unsigned_short_ptr += step2;
                }
                unsigned_short_ptr += step1;
            }
            unsigned_short_ptr += step0;
        }
        break;

    case SIGNED_SHORT:
        signed_short_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            for_less( i1, 0, n1 )
            {
                for_less( i2, 0, n2 )
                {
                    for_less( i3, 0, n3 )
                    {
                        for_less( i4, 0, n4 )
                        {
                            values[ind] = (Real) *signed_short_ptr;
                            ++ind;
                            ++signed_short_ptr;
                        }
                        signed_short_ptr += step3;
                    }
                    signed_short_ptr += step2;
                }
                signed_short_ptr += step1;
            }
            signed_short_ptr += step0;
        }
        break;

    case UNSIGNED_LONG:
        unsigned_long_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            for_less( i1, 0, n1 )
            {
                for_less( i2, 0, n2 )
                {
                    for_less( i3, 0, n3 )
                    {
                        for_less( i4, 0, n4 )
                        {
                            values[ind] = (Real) *unsigned_long_ptr;
                            ++ind;
                            ++unsigned_long_ptr;
                        }
                        unsigned_long_ptr += step3;
                    }
                    unsigned_long_ptr += step2;
                }
                unsigned_long_ptr += step1;
            }
            unsigned_long_ptr += step0;
        }
        break;

    case SIGNED_LONG:
        signed_long_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            for_less( i1, 0, n1 )
            {
                for_less( i2, 0, n2 )
                {
                    for_less( i3, 0, n3 )
                    {
                        for_less( i4, 0, n4 )
                        {
                            values[ind] = (Real) *signed_long_ptr;
                            ++ind;
                            ++signed_long_ptr;
                        }
                        signed_long_ptr += step3;
                    }
                    signed_long_ptr += step2;
                }
                signed_long_ptr += step1;
            }
            signed_long_ptr += step0;
        }
        break;

    case FLOAT:
        float_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            for_less( i1, 0, n1 )
            {
                for_less( i2, 0, n2 )
                {
                    for_less( i3, 0, n3 )
                    {
                        for_less( i4, 0, n4 )
                        {
                            values[ind] = (Real) *float_ptr;
                            ++ind;
                            ++float_ptr;
                        }
                        float_ptr += step3;
                    }
                    float_ptr += step2;
                }
                float_ptr += step1;
            }
            float_ptr += step0;
        }
        break;

    case DOUBLE:
        double_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            for_less( i1, 0, n1 )
            {
                for_less( i2, 0, n2 )
                {
                    for_less( i3, 0, n3 )
                    {
                        for_less( i4, 0, n4 )
                        {
                            values[ind] = (Real) *double_ptr;
                            ++ind;
                            ++double_ptr;
                        }
                        double_ptr += step3;
                    }
                    double_ptr += step2;
                }
                double_ptr += step1;
            }
            double_ptr += step0;
        }
        break;
    }
}

public  void  get_volume_voxel_hyperslab_4d(
    Volume   volume,
    int      v0,
    int      v1,
    int      v2,
    int      v3,
    int      n0,
    int      n1,
    int      n2,
    int      n3,
    Real     values[] )
{
    int              sizes[MAX_DIMENSIONS];
    int              ind, step0, step1, step2;
    int              i0, i1, i2, i3;
    unsigned  char   *unsigned_byte_ptr;
    signed  char     *signed_byte_ptr;
    unsigned  short  *unsigned_short_ptr;
    signed  short    *signed_short_ptr;
    unsigned  long   *unsigned_long_ptr;
    signed  long     *signed_long_ptr;
    float            *float_ptr;
    double           *double_ptr;
    void             *void_ptr;

    if( volume->is_cached_volume )
    {
        slow_get_volume_voxel_hyperslab( volume, v0, v1, v2, v3, 0,
                                         n0, n1, n2, n3, 0, values );
        return;
    }

    get_volume_sizes( volume, sizes );

    GET_MULTIDIM_PTR_4D( void_ptr, volume->array, v0, v1, v2, v3 );

    step2 = sizes[3];
    step1 = sizes[2] * step2;
    step0 = sizes[1] * step1;
    step0 -= n1 * step1;
    step1 -= n2 * step2;
    step2 -= n3 * 1;

    ind = 0;

    switch( get_volume_data_type(volume) )
    {
    case UNSIGNED_BYTE:
        unsigned_byte_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            for_less( i1, 0, n1 )
            {
                for_less( i2, 0, n2 )
                {
                    for_less( i3, 0, n3 )
                    {
                        values[ind] = (Real) *unsigned_byte_ptr;
                        ++ind;
                        ++unsigned_byte_ptr;
                    }
                    unsigned_byte_ptr += step2;
                }
                unsigned_byte_ptr += step1;
            }
            unsigned_byte_ptr += step0;
        }
        break;

    case SIGNED_BYTE:
        signed_byte_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            for_less( i1, 0, n1 )
            {
                for_less( i2, 0, n2 )
                {
                    for_less( i3, 0, n3 )
                    {
                        values[ind] = (Real) *signed_byte_ptr;
                        ++ind;
                        ++signed_byte_ptr;
                    }
                    signed_byte_ptr += step2;
                }
                signed_byte_ptr += step1;
            }
            signed_byte_ptr += step0;
        }
        break;

    case UNSIGNED_SHORT:
        unsigned_short_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            for_less( i1, 0, n1 )
            {
                for_less( i2, 0, n2 )
                {
                    for_less( i3, 0, n3 )
                    {
                        values[ind] = (Real) *unsigned_short_ptr;
                        ++ind;
                        ++unsigned_short_ptr;
                    }
                    unsigned_short_ptr += step2;
                }
                unsigned_short_ptr += step1;
            }
            unsigned_short_ptr += step0;
        }
        break;

    case SIGNED_SHORT:
        signed_short_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            for_less( i1, 0, n1 )
            {
                for_less( i2, 0, n2 )
                {
                    for_less( i3, 0, n3 )
                    {
                        values[ind] = (Real) *signed_short_ptr;
                        ++ind;
                        ++signed_short_ptr;
                    }
                    signed_short_ptr += step2;
                }
                signed_short_ptr += step1;
            }
            signed_short_ptr += step0;
        }
        break;

    case UNSIGNED_LONG:
        unsigned_long_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            for_less( i1, 0, n1 )
            {
                for_less( i2, 0, n2 )
                {
                    for_less( i3, 0, n3 )
                    {
                        values[ind] = (Real) *unsigned_long_ptr;
                        ++ind;
                        ++unsigned_long_ptr;
                    }
                    unsigned_long_ptr += step2;
                }
                unsigned_long_ptr += step1;
            }
            unsigned_long_ptr += step0;
        }
        break;

    case SIGNED_LONG:
        signed_long_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            for_less( i1, 0, n1 )
            {
                for_less( i2, 0, n2 )
                {
                    for_less( i3, 0, n3 )
                    {
                        values[ind] = (Real) *signed_long_ptr;
                        ++ind;
                        ++signed_long_ptr;
                    }
                    signed_long_ptr += step2;
                }
                signed_long_ptr += step1;
            }
            signed_long_ptr += step0;
        }
        break;

    case FLOAT:
        float_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            for_less( i1, 0, n1 )
            {
                for_less( i2, 0, n2 )
                {
                    for_less( i3, 0, n3 )
                    {
                        values[ind] = (Real) *float_ptr;
                        ++ind;
                        ++float_ptr;
                    }
                    float_ptr += step2;
                }
                float_ptr += step1;
            }
            float_ptr += step0;
        }
        break;

    case DOUBLE:
        double_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            for_less( i1, 0, n1 )
            {
                for_less( i2, 0, n2 )
                {
                    for_less( i3, 0, n3 )
                    {
                        values[ind] = (Real) *double_ptr;
                        ++ind;
                        ++double_ptr;
                    }
                    double_ptr += step2;
                }
                double_ptr += step1;
            }
            double_ptr += step0;
        }
        break;
    }
}

public  void  get_volume_voxel_hyperslab_3d(
    Volume   volume,
    int      v0,
    int      v1,
    int      v2,
    int      n0,
    int      n1,
    int      n2,
    Real     values[] )
{
    int              sizes[MAX_DIMENSIONS];
    int              ind, step0, step1;
    int              i0, i1, i2;
    unsigned  char   *unsigned_byte_ptr;
    signed  char     *signed_byte_ptr;
    unsigned  short  *unsigned_short_ptr;
    signed  short    *signed_short_ptr;
    unsigned  long   *unsigned_long_ptr;
    signed  long     *signed_long_ptr;
    float            *float_ptr;
    double           *double_ptr;
    void             *void_ptr;

    if( volume->is_cached_volume )
    {
        slow_get_volume_voxel_hyperslab( volume, v0, v1, v2, 0, 0,
                                         n0, n1, n2, 0, 0, values );
        return;
    }

    get_volume_sizes( volume, sizes );

    GET_MULTIDIM_PTR_3D( void_ptr, volume->array, v0, v1, v2 );

    step1 = sizes[2];
    step0 = sizes[1] * step1;
    step0 -= n1 * step1;
    step1 -= n2 * 1;

    ind = 0;

    switch( get_volume_data_type(volume) )
    {
    case UNSIGNED_BYTE:
        unsigned_byte_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            for_less( i1, 0, n1 )
            {
                for_less( i2, 0, n2 )
                {
                    values[ind] = (Real) *unsigned_byte_ptr;
                    ++ind;
                    ++unsigned_byte_ptr;
                }
                unsigned_byte_ptr += step1;
            }
            unsigned_byte_ptr += step0;
        }
        break;

    case SIGNED_BYTE:
        signed_byte_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            for_less( i1, 0, n1 )
            {
                for_less( i2, 0, n2 )
                {
                    values[ind] = (Real) *signed_byte_ptr;
                    ++ind;
                    ++signed_byte_ptr;
                }
                signed_byte_ptr += step1;
            }
            signed_byte_ptr += step0;
        }
        break;

    case UNSIGNED_SHORT:
        unsigned_short_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            for_less( i1, 0, n1 )
            {
                for_less( i2, 0, n2 )
                {
                    values[ind] = (Real) *unsigned_short_ptr;
                    ++ind;
                    ++unsigned_short_ptr;
                }
                unsigned_short_ptr += step1;
            }
            unsigned_short_ptr += step0;
        }
        break;

    case SIGNED_SHORT:
        signed_short_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            for_less( i1, 0, n1 )
            {
                for_less( i2, 0, n2 )
                {
                    values[ind] = (Real) *signed_short_ptr;
                    ++ind;
                    ++signed_short_ptr;
                }
                signed_short_ptr += step1;
            }
            signed_short_ptr += step0;
        }
        break;

    case UNSIGNED_LONG:
        unsigned_long_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            for_less( i1, 0, n1 )
            {
                for_less( i2, 0, n2 )
                {
                    values[ind] = (Real) *unsigned_long_ptr;
                    ++ind;
                    ++unsigned_long_ptr;
                }
                unsigned_long_ptr += step1;
            }
            unsigned_long_ptr += step0;
        }
        break;

    case SIGNED_LONG:
        signed_long_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            for_less( i1, 0, n1 )
            {
                for_less( i2, 0, n2 )
                {
                    values[ind] = (Real) *signed_long_ptr;
                    ++ind;
                    ++signed_long_ptr;
                }
                signed_long_ptr += step1;
            }
            signed_long_ptr += step0;
        }
        break;

    case FLOAT:
        float_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            for_less( i1, 0, n1 )
            {
                for_less( i2, 0, n2 )
                {
                    values[ind] = (Real) *float_ptr;
                    ++ind;
                    ++float_ptr;
                }
                float_ptr += step1;
            }
            float_ptr += step0;
        }
        break;

    case DOUBLE:
        double_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            for_less( i1, 0, n1 )
            {
                for_less( i2, 0, n2 )
                {
                    values[ind] = (Real) *double_ptr;
                    ++ind;
                    ++double_ptr;
                }
                double_ptr += step1;
            }
            double_ptr += step0;
        }
        break;
    }
}

public  void  get_volume_voxel_hyperslab_2d(
    Volume   volume,
    int      v0,
    int      v1,
    int      n0,
    int      n1,
    Real     values[] )
{
    int              sizes[MAX_DIMENSIONS];
    int              ind, step0;
    int              i0, i1;
    unsigned  char   *unsigned_byte_ptr;
    signed  char     *signed_byte_ptr;
    unsigned  short  *unsigned_short_ptr;
    signed  short    *signed_short_ptr;
    unsigned  long   *unsigned_long_ptr;
    signed  long     *signed_long_ptr;
    float            *float_ptr;
    double           *double_ptr;
    void             *void_ptr;

    if( volume->is_cached_volume )
    {
        slow_get_volume_voxel_hyperslab( volume, v0, v1, 0, 0, 0,
                                         n0, n1, 0, 0, 0, values );
        return;
    }

    get_volume_sizes( volume, sizes );

    GET_MULTIDIM_PTR_2D( void_ptr, volume->array, v0, v1 );

    step0 = sizes[1];
    step0 -= n1 * 1;

    ind = 0;

    switch( get_volume_data_type(volume) )
    {
    case UNSIGNED_BYTE:
        unsigned_byte_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            for_less( i1, 0, n1 )
            {
                values[ind] = (Real) *unsigned_byte_ptr;
                ++ind;
                ++unsigned_byte_ptr;
            }
            unsigned_byte_ptr += step0;
        }
        break;

    case SIGNED_BYTE:
        signed_byte_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            for_less( i1, 0, n1 )
            {
                values[ind] = (Real) *signed_byte_ptr;
                ++ind;
                ++signed_byte_ptr;
            }
            signed_byte_ptr += step0;
        }
        break;

    case UNSIGNED_SHORT:
        unsigned_short_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            for_less( i1, 0, n1 )
            {
                values[ind] = (Real) *unsigned_short_ptr;
                ++ind;
                ++unsigned_short_ptr;
            }
            unsigned_short_ptr += step0;
        }
        break;

    case SIGNED_SHORT:
        signed_short_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            for_less( i1, 0, n1 )
            {
                values[ind] = (Real) *signed_short_ptr;
                ++ind;
                ++signed_short_ptr;
            }
            signed_short_ptr += step0;
        }
        break;

    case UNSIGNED_LONG:
        unsigned_long_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            for_less( i1, 0, n1 )
            {
                values[ind] = (Real) *unsigned_long_ptr;
                ++ind;
                ++unsigned_long_ptr;
            }
            unsigned_long_ptr += step0;
        }
        break;

    case SIGNED_LONG:
        signed_long_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            for_less( i1, 0, n1 )
            {
                values[ind] = (Real) *signed_long_ptr;
                ++ind;
                ++signed_long_ptr;
            }
            signed_long_ptr += step0;
        }
        break;

    case FLOAT:
        float_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            for_less( i1, 0, n1 )
            {
                values[ind] = (Real) *float_ptr;
                ++ind;
                ++float_ptr;
            }
            float_ptr += step0;
        }
        break;

    case DOUBLE:
        double_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            for_less( i1, 0, n1 )
            {
                values[ind] = (Real) *double_ptr;
                ++ind;
                ++double_ptr;
            }
            double_ptr += step0;
        }
        break;
    }
}

public  void  get_volume_voxel_hyperslab_1d(
    Volume   volume,
    int      v0,
    int      n0,
    Real     values[] )
{
    int              sizes[MAX_DIMENSIONS];
    int              ind;
    int              i0;
    unsigned  char   *unsigned_byte_ptr;
    signed  char     *signed_byte_ptr;
    unsigned  short  *unsigned_short_ptr;
    signed  short    *signed_short_ptr;
    unsigned  long   *unsigned_long_ptr;
    signed  long     *signed_long_ptr;
    float            *float_ptr;
    double           *double_ptr;
    void             *void_ptr;

    if( volume->is_cached_volume )
    {
        slow_get_volume_voxel_hyperslab( volume, v0, 0, 0, 0, 0,
                                         n0, 0, 0, 0, 0, values );
        return;
    }

    get_volume_sizes( volume, sizes );

    GET_MULTIDIM_PTR_1D( void_ptr, volume->array, v0 );

    ind = 0;

    switch( get_volume_data_type(volume) )
    {
    case UNSIGNED_BYTE:
        unsigned_byte_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            values[ind] = (Real) *unsigned_byte_ptr;
            ++ind;
            ++unsigned_byte_ptr;
        }
        break;

    case SIGNED_BYTE:
        signed_byte_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            values[ind] = (Real) *signed_byte_ptr;
            ++ind;
            ++signed_byte_ptr;
        }
        break;

    case UNSIGNED_SHORT:
        unsigned_short_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            values[ind] = (Real) *unsigned_short_ptr;
            ++ind;
            ++unsigned_short_ptr;
        }
        break;

    case SIGNED_SHORT:
        signed_short_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            values[ind] = (Real) *signed_short_ptr;
            ++ind;
            ++signed_short_ptr;
        }
        break;

    case UNSIGNED_LONG:
        unsigned_long_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            values[ind] = (Real) *unsigned_long_ptr;
            ++ind;
            ++unsigned_long_ptr;
        }
        break;

    case SIGNED_LONG:
        signed_long_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            values[ind] = (Real) *signed_long_ptr;
            ++ind;
            ++signed_long_ptr;
        }
        break;

    case FLOAT:
        float_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            values[ind] = (Real) *float_ptr;
            ++ind;
            ++float_ptr;
        }
        break;

    case DOUBLE:
        double_ptr = void_ptr;
        for_less( i0, 0, n0 )
        {
            values[ind] = (Real) *double_ptr;
            ++ind;
            ++double_ptr;
        }
        break;
    }
}
