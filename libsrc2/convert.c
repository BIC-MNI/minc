/** \file convert.c
 * \brief MINC 2.0 Coordinate and Voxel Conversion Functions
 * \author Bert Vincent
 *
 * Functions to convert "real" valued data to and from "voxel" valued
 * data, and to convert coordinates between "voxel" and "world" systems.
 */

#include <stdlib.h>
#include <hdf5.h>
#include <math.h>
#include <float.h>
#include "minc.h"
#include "minc2.h"
#include "minc2_private.h"

/** convert values between real (scaled) values and voxel (unscaled)
 * values.  The voxel value is the unscaled value, and corresponds to the
 * value actually stored in the file, whereas the "real" value is the
 * value at the given location after scaling has been applied.
 */
int
miconvert_real_to_voxel(mihandle_t volume,
                        const unsigned long coords[],
                        int ncoords,
                        double real_value,
                        double *voxel_value_ptr
                        )
{
    int result = MI_NOERROR;
    double valid_min, valid_max;
    double slice_min, slice_max;
    double voxel_range, voxel_offset;
    double real_range, real_offset;

    /* get valid min/max, image min/max 
     */
    miget_volume_valid_range(volume, &valid_max, &valid_min);
    
    /* get image min/max 
     */
    miget_slice_range(volume, coords, ncoords, &slice_max, &slice_min);
    
    /* Calculate the actual conversion.
     */
    voxel_offset = valid_min;
    real_offset = slice_min;
    voxel_range = valid_max - valid_min;
    real_range = slice_max - slice_min;
    
    real_value = (real_value - real_offset) / real_range;
    *voxel_value_ptr = (real_value * voxel_range) + voxel_offset;

    return (result);
}


/** convert values between real (scaled) values and voxel (unscaled)
 * values.  The voxel value is the unscaled value, and corresponds to the
 * value actually stored in the file, whereas the "real" value is the
 * value at the given location after scaling has been applied.
 */
int
miconvert_voxel_to_real(mihandle_t volume,
                        const unsigned long coords[],
                        int ncoords,
                        double voxel_value,
                        double *real_value_ptr)
{
    int result = MI_NOERROR;
    double valid_min, valid_max;
    double slice_min, slice_max;
    double voxel_range, voxel_offset;
    double real_range, real_offset;

    /* get valid min/max, image min/max 
     */
    miget_volume_valid_range(volume, &valid_max, &valid_min);
    
    /* get image min/max 
     */
    miget_slice_range(volume, coords, ncoords, &slice_max, &slice_min);

    /* Calculate the actual conversion.
     */
    voxel_offset = valid_min;
    real_offset = slice_min;
    voxel_range = valid_max - valid_min;
    real_range = slice_max - slice_min;
    
    voxel_value = (voxel_value - voxel_offset) / voxel_range;
    *real_value_ptr = (voxel_value * real_range) + real_offset;
    return (result);
}

/** Converts a 3-dimensional position in voxel coordinates into a 
 * 3-dimensional position in world coordinates.
 */
int
miconvert_3D_voxel_to_world(mihandle_t volume,
                            const double voxel[MI2_3D],
                            double world[MI2_3D])
{
    mitransform_coord(world, volume->v2w_transform, voxel);
    return (MI_NOERROR);
}

/** Converts a 3-dimensional position in world coordinates into a 
 * 3-dimensional position in voxel coordinates.
 */
int miconvert_3D_world_to_voxel(mihandle_t volume,
                                const double world[MI2_3D],
                                double voxel[MI2_3D])
{
    mitransform_coord(voxel, volume->w2v_transform, world);
    return (MI_NOERROR);
}

/** Convert a */
int 
miconvert_3D_voxel_to_spatial_frequency(mihandle_t volume,
                                        const double voxel[MI2_3D],
                                        double world[MI2_3D])

{
    return (MI_NOERROR);
}

int 
miconvert_3D_spatial_frequency_to_voxel(mihandle_t volume,
                                        const double world[MI2_3D],
                                        double voxel[MI2_3D])

{
    return (MI_NOERROR);
}

/**
 * This function converts coordinates in voxel coordinates to world 
 * coordinates.  Only the spatial coordinates are converted.
 */
int
miconvert_voxel_to_world(midimhandle_t dimensions[],
                         int ndims,
                         const double voxel[],
                         double world[])
{
    return (MI_NOERROR);
}

/**
 * This function converts coordinates in world coordinates to voxel
 * coordinates.  Only the spatial coordinates are converted.
 */
int
miconvert_world_to_voxel(midimhandle_t dimensions[],
                         int ndims,
                         const double voxel[],
                         double world[])
{
    return (MI_NOERROR);
}

/** This function retrieves the real values of a position in the
 *  MINC volume.  The "real" value is the value at the given location 
 *  after scaling has been applied.
 */
int
miget_real_value(mihandle_t volume,
                 const unsigned long coords[],
                 int ndims,
                 double *value_ptr)
{
    double voxel;
    int result;

    result = miget_voxel_value(volume, coords, ndims, &voxel);
    if (result != MI_NOERROR) {
        return (result);
    }
    miconvert_voxel_to_real(volume, coords, ndims, voxel, value_ptr);
    return (MI_NOERROR);
}

/** This function sets the  real value of a position in the MINC
 *  volume. The "real" value is the value at the given location 
 *  after scaling has been applied.
 */
int
miset_real_value(mihandle_t volume,
                 const unsigned long coords[],
                 int ndims,
                 double value)
{
    double voxel;

    miconvert_real_to_voxel(volume, coords, ndims, value, &voxel);
    return (miset_voxel_value(volume, coords, ndims, voxel));
}


int
miconvert_spatial_origin_to_start( mihandle_t volume,
                                   double world[3],
                                   double starts[3])
{
    return (MI_NOERROR);
}

int
miconvert_spatial_frequency_origin_to_start( mihandle_t volume,
                                             double world[3],
                                             double starts[3])
{
    return (MI_NOERROR);
}

int
miset_spatial_origin_to_start( mihandle_t volume,
                               double world[3])
{
    return (MI_NOERROR);
}

int
miset_spatial_frequency_origin_to_start(mihandle_t volume,
                                        double world[3])
{
    return (MI_NOERROR);
}

/** This function retrieves the voxel values of a position in the
 * MINC volume. The voxel value is the unscaled value, and corresponds
 * to the value actually stored in the file.
 */
int
miget_voxel_value(mihandle_t volume,
                  const unsigned long coords[],
                  int ndims,
                  double *voxel_ptr)
{
    int result;
    unsigned long count[MI2_MAX_VAR_DIMS];
    int i;

    for (i = 0; i < volume->number_of_dims; i++) {
        count[i] = 1;
    }
    result = miget_voxel_value_hyperslab(volume, MI_TYPE_DOUBLE, 
                                         coords, count, voxel_ptr);
    return (result);
}

/** This function sets the voxel value of a position in the MINC
 * volume.  The voxel value is the unscaled value, and corresponds to the
 * value actually stored in the file.
 */
int
miset_voxel_value(mihandle_t volume,
                  const unsigned long coords[],
                  int ndims,
                  double voxel)
{
    int result;
    unsigned long count[MI2_MAX_VAR_DIMS];
    int i;

    for (i = 0; i < ndims; i++) {
        count[i] = 1;
    }
    
    result = miset_voxel_value_hyperslab(volume, MI_TYPE_DOUBLE, 
                                         coords, count, &voxel);
    return (result);
}


/** Get the absolute minimum and maximum values of a volume.
 */
int
miget_volume_real_range(mihandle_t volume, double real_range[])
{
    hid_t spc_id;
    int n;
    double *buffer;
    int i;

    /* First find the real minimum.
     */
    spc_id = H5Dget_space(volume->imin_id);

    n = (int) H5Sget_simple_extent_npoints(spc_id);

    H5Sclose(spc_id);

    buffer = malloc(n * sizeof(double));
    if (buffer == NULL) {
        return (MI_ERROR);
    }

    H5Dread(volume->imin_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,
            buffer);

    real_range[0] = FLT_MAX;

    for (i = 0; i < n; i++) {
        if (buffer[i] < real_range[0]) {
            real_range[0] = buffer[i];
        }
    }
    
    free(buffer);

    /* Now find the maximum.
     */
    spc_id = H5Dget_space(volume->imax_id);

    n = (int) H5Sget_simple_extent_npoints(spc_id);

    H5Sclose(spc_id);

    buffer = malloc(n * sizeof(double));
    if (buffer == NULL) {
        return (MI_ERROR);
    }

    H5Dread(volume->imax_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,
            buffer);

    real_range[1] = FLT_MIN;

    for (i = 0; i < n; i++) {
        if (buffer[i] > real_range[1]) {
            real_range[1] = buffer[i];
        }
    }
    
    free(buffer);

    return (MI_NOERROR);
}

