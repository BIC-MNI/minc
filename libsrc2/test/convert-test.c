#include <stdio.h>
#include <stdlib.h>

#include "minc2.h"

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
