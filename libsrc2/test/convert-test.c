#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "minc2.h"

#define TESTRPT(msg, val) (error_cnt++, fprintf(stderr, \
                                  "Error reported on line #%d, %s: %d\n", \
                                  __LINE__, msg, val))
static int error_cnt = 0;

#define NDIMS 3

#define CX 40
#define CY 40
#define CZ 40

/* "Close enough for government work" */
#define EPSILON 1e-13
#define NEARLY_EQUAL(x, y) (fabs(x - y) < EPSILON)


#define VALID_MAX 100
#define VALID_MIN 0
#define REAL_MIN -2.0
#define REAL_MAX 2.0

int
main(int argc, char **argv)
{
    int result;
    double voxel[NDIMS];
    double world[NDIMS];
    double new_voxel[NDIMS];
    unsigned long coords[NDIMS];
    unsigned long count[NDIMS] = {1,1,1};
    midimhandle_t hdims[NDIMS];
    int i, j, k;
    int n;
    mihandle_t hvol;
    double v1, v2;
    double r1, r2, r3;
    double cosines[3];

    result = micreate_dimension("xspace", MI_DIMCLASS_SPATIAL, 
                                MI_DIMATTR_REGULARLY_SAMPLED, CX, &hdims[0]);

    result = micreate_dimension("yspace", MI_DIMCLASS_SPATIAL, 
                                MI_DIMATTR_REGULARLY_SAMPLED, CY, &hdims[1]);

    result = micreate_dimension("zspace", MI_DIMCLASS_SPATIAL, 
                                MI_DIMATTR_REGULARLY_SAMPLED, CZ, &hdims[2]);


    /* Set cosines, steps, and starts to reference values.
     */
    cosines[0] = 0.999815712665963;
    cosines[1] = 0.019197414051189;
    cosines[2] = 0;
    miset_dimension_cosines(hdims[0], cosines);
    miset_dimension_start(hdims[0], -75.4957607345912);
    miset_dimension_separation(hdims[0], 1.0);

    cosines[0] = -0.019197414051189;
    cosines[1] = 0.999815712665963;
    cosines[2] = 0;
    miset_dimension_cosines(hdims[1], cosines);
    miset_dimension_start(hdims[1], 160.392861750822);
    miset_dimension_separation(hdims[1], -1.0);

    cosines[0] = 0;
    cosines[1] = 0;
    cosines[2] = 1;
    miset_dimension_cosines(hdims[2], cosines);
    miset_dimension_start(hdims[2], 121.91304);
    miset_dimension_separation(hdims[2], -1.0);
    result = micreate_volume("tst-convert.mnc", NDIMS, hdims, MI_TYPE_UINT,
                             MI_CLASS_REAL, NULL, &hvol);
    if (result < 0) {
        TESTRPT("micreate_volume error", result);
    }

    result = micreate_volume_image(hvol);
    if (result < 0) {
        TESTRPT("micreate_volume_image error", result);
    }

    /* Valid values are from 0 to 100 */
    miset_volume_valid_range(hvol, VALID_MAX, VALID_MIN);

    /* "real" values are -2.0 to 2.0 */
    miset_volume_range(hvol, REAL_MAX, REAL_MIN);

    for (i = 0; i < CX; i++) {
        for (j = 0; j < CY; j++) {
            for (k = 0; k < CZ; k++) {
                coords[0] = i;
                coords[1] = j;
                coords[2] = k;

                voxel[0] = coords[0];
                voxel[1] = coords[1];
                voxel[2] = coords[2];

                miconvert_3D_voxel_to_world(hvol, voxel, world);
        
                miconvert_3D_world_to_voxel(hvol, world, new_voxel);

                for (n = 0; n < 3; n++) {
                    if (!NEARLY_EQUAL(voxel[n], new_voxel[n])) {
                        fprintf(stderr, "%d %f %f\n", 
                                n, voxel[n], new_voxel[n]);
                        TESTRPT("conversion error", 0);
                    }
                }

                v1 = VALID_MAX * (voxel[0] + voxel[1] + voxel[2]) / (CX+CY+CZ);
                result = miset_voxel_value_hyperslab(hvol, MI_TYPE_DOUBLE,
                                                     coords, count, &v1);

                if (result < 0) {
                    TESTRPT("miset_voxel_value_hyperslab error", result);
                }

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

                /* Double check the conversion.
                 */
                r3 = (v1 - VALID_MIN) / (VALID_MAX - VALID_MIN);
                r3 = (r3 * (REAL_MAX - REAL_MIN)) + REAL_MIN;

                if (!NEARLY_EQUAL(r3, r1)) {
                    TESTRPT("real value mismatch", 0);
                }

                /* Convert it back to voxel.
                 */
                result = miconvert_real_to_voxel(hvol, coords, 3, r1, &v2);
                if (result < 0) {
                    TESTRPT("miconvert_real_to_voxel error", result);
                }

                /* Compare to the value read via the ICV
                 */
                result = miget_real_value(hvol, coords, 3, &r2);
                if (result < 0) {
                    TESTRPT("miget_real_value error", result);
                }

                if (!NEARLY_EQUAL(v1, v2)) {
                    fprintf(stderr, "v1 %f v2 %f\n", v1, v2);
                    TESTRPT("Voxel value mismatch", 0);
                }
                if (!NEARLY_EQUAL(r1, r2)) {
                    fprintf(stderr, "r1 %f r2 %f\n", r1, r2);
                    TESTRPT("Real value mismatch", 0);
                }

                result = miget_voxel_value(hvol, coords, 3, &v1);
                if (result < 0) {
                    TESTRPT("miget_voxel_value error", result);
                }

                if (!NEARLY_EQUAL(v1, v2)) {
                    fprintf(stderr, "v1 %f v2 %f\n", v1, v2);
                    TESTRPT("Voxel value mismatch", 0);
                }
            }
        }
    }

    if (error_cnt == 0) {
        printf("No errors\n");
    }
    else {
        printf("%d error%s\n", error_cnt, error_cnt == 1 ? "" : "s");
    }
    return (error_cnt);
}
