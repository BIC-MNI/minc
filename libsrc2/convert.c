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
                        const unsigned long location[],
                        int ndims,
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
    miget_slice_range(volume, location, ndims, &slice_max, &slice_min);
    
    /* Calculate the actual conversion.
     */
    voxel_offset = valid_min;
    real_offset = slice_min;
    voxel_range = valid_max - valid_min;
    real_range = slice_max - slice_min;
    
    real_value = (real_value - real_offset) / real_range;
    *voxel_value_ptr = (real_value + voxel_offset) * voxel_range;

    return (result);
}


/** convert values between real (scaled) values and voxel (unscaled)
 * values.  The voxel value is the unscaled value, and corresponds to the
 * value actually stored in the file, whereas the "real" value is the
 * value at the given location after scaling has been applied.
 */
int
miconvert_voxel_to_real(mihandle_t volume,
                        const unsigned long voxel_coords[],
                        int ndims,
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
    miget_slice_range(volume, voxel_coords, ndims, &slice_max, &slice_min);

    /* Calculate the actual conversion.
     */
    voxel_offset = valid_min;
    real_offset = slice_min;
    voxel_range = valid_max - valid_min;
    real_range = slice_max - slice_min;
    
    voxel_value = (voxel_value - voxel_offset) / voxel_range;
    *real_value_ptr = (voxel_value + real_offset) * real_range;
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

int
miconvert_voxel_to_world(midimhandle_t dimensions[],
                         int ndims,
                         const double voxel[],
                         double world[])
{
    return (MI_NOERROR);
}

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
    int result;
    unsigned long count[ndims]; /*  */
    int i;

    for (i = 0; i < ndims; i++) {
        count[i] = 1;
    }
    result = miget_real_value_hyperslab(volume, MI_TYPE_DOUBLE,
                                        coords, count, value_ptr);
    return (result);
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
    int result;
    unsigned long count[ndims]; /*  */
    int i;

    for (i = 0; i < ndims; i++) {
        count[i] = 1;
    }
    result = miset_real_value_hyperslab(volume, MI_TYPE_DOUBLE,
                                        coords, count, &value);
    return (result);
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
    unsigned long count[MAX_VAR_DIMS];
    int i;

    for (i = 0; i < ndims; i++) {
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
    unsigned long count[MAX_VAR_DIMS];
    int i;

    for (i = 0; i < ndims; i++) {
        count[i] = 1;
    }
    
    result = miset_voxel_value_hyperslab(volume, MI_TYPE_DOUBLE, 
                                         coords, count, &voxel);
    return (result);
}


#ifdef M2_TEST
#define TESTRPT(msg, val) (error_cnt++, fprintf(stderr, \
                                  "Error reported on line #%d, %s: %d\n", \
                                  __LINE__, msg, val))
static int error_cnt = 0;

int
main(int argc, char **argv)
{
    int result;
    double voxel[3];
    double world[3];
    double new_voxel[3];
    unsigned long coords[3];
    unsigned long count[3] = {1,1,1};
    int i;
    mihandle_t hvol;
    double v1, v2;
    double r1, r2;
    double f;

    if (argc != 5) {
        TESTRPT("must specify a file name and position!", 0);
        exit(-1);
    }

    result = miopen_volume(argv[1], MI2_OPEN_READ, &hvol);
    if (result < 0) {
        TESTRPT("miopen_volume error", result);
    }

    coords[0] = atof(argv[2]);
    coords[1] = atof(argv[3]);
    coords[2] = atof(argv[4]);

    voxel[0] = coords[0];
    voxel[1] = coords[1];
    voxel[2] = coords[2];

    miconvert_3D_voxel_to_world(hvol, voxel, world);

    for (i = 0; i < 3; i++) {
        printf("%.20g ", world[i]);
    }
    printf("\n");

    miconvert_3D_world_to_voxel(hvol, world, new_voxel);
    for (i = 0; i < 3; i++) {
        printf("%.20g ", new_voxel[i]);
    }
    printf("\n");


    /* Get a voxel value.
     */
    result = miget_voxel_value_hyperslab(hvol, MI_TYPE_DOUBLE,
                                         coords, count, &v1);
    if (result < 0) {
        TESTRPT("miget_voxel_value_hyperslab error", result);
    }

    /* Convert from voxel to real.
     */
    result = miconvert_voxel_to_real(hvol, coords, 3, v1, &r1);
    if (result < 0) {
        TESTRPT("miconvert_voxel_to_real error", result);
    }
    printf("voxel %f => real %f\n", v1, r1);

    /* Convert it back to voxel.
     */
    result = miconvert_real_to_voxel(hvol, coords, 3, r1, &v2);
    if (result < 0) {
        TESTRPT("miconvert_real_to_voxel error", result);
    }
    printf("real %f => voxel %f\n", r1, v2);

    /* Compare to the value read via the ICV
     */
    result = miget_real_value(hvol, coords, 3, &r2);
    if (result < 0) {
        TESTRPT("miget_real_value error", result);
    }
    printf("real from ICV: %f\n", r2);
    printf("\n");


    if (v1 != v2) {
        TESTRPT("Voxel value mismatch", 0);
    }
    if (r1 != r2) {
        TESTRPT("Real value mismatch", 0);
    }

    result = miget_voxel_value(hvol, coords, 3, &v1);
    if (result < 0) {
        TESTRPT("miget_voxel_value error", result);
    }

    printf("voxel from mivarget1: %f\n", v1);

    return (error_cnt);
}

#endif
