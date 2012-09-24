#include <stdio.h>
#include "minc2.h"

#define TESTRPT(msg, val) (error_cnt++, fprintf(stderr, \
                                  "Error reported on line #%d, %s: %d\n", \
                                  __LINE__, msg, val))

static int error_cnt = 0;

#define CX 10
#define CY 10
#define CZ 6
#define CT 8
#define NDIMS 4

#define XSTART (-24)
#define XSTEP (1)

#define YSTART (-26)
#define YSTEP (2)

#define ZSTART (22)
#define ZSTEP (-1.5)

int 
check_dims(mihandle_t vol, midimhandle_t dim[])
{
    int i;
    int r;
    int n;
    mihandle_t vol_tmp;
    midimhandle_t dim_tmp[NDIMS];
    double offsets[100];

    for (i = 0; i < CT; i++) {
        double tmp = -1;

        r = miget_dimension_offsets(dim[0], 1, i, &tmp);
        if (r < 0) {
            TESTRPT("failed", r);
        }
        if ((i * i) + 100.0 != tmp) {
            TESTRPT("bad value", i);
        }
    }

    r = miget_dimension_offsets(dim[1], CX, 0, offsets);
    if (r < 0) {
        TESTRPT("failed", r);
    }
    for (i = 0; i < CX; i++) {
        if (offsets[i] != XSTART + (i * XSTEP)) {
            TESTRPT("bad value", i);
        }
    }

    r = miget_dimension_offsets(dim[2], CY, 0, offsets);
    if (r < 0) {
        TESTRPT("failed", r);
    }
    for (i = 0; i < CY; i++) {
        if (offsets[i] != YSTART + (i * YSTEP)) {
            TESTRPT("bad value", i);
        }
    }

    r = miget_dimension_offsets(dim[3], CZ, 0, offsets);
    if (r < 0) {
        TESTRPT("failed", r);
    }
    for (i = 0; i < CZ; i++) {
        if (offsets[i] != ZSTART + (i * ZSTEP)) {
            TESTRPT("bad value", i);
        }
    }


    r = miget_volume_dimension_count(vol, MI_DIMCLASS_SPATIAL, MI_DIMATTR_ALL,
                                     &n);
    if (r < 0) {
        TESTRPT("failed", r);
    }
    if (n != NDIMS-1) {
        TESTRPT("wrong number of spatial dimensions", n);
    }

    r = miget_volume_dimension_count(vol, MI_DIMCLASS_ANY, MI_DIMATTR_ALL,
                                     &n);
    if (r < 0) {
        TESTRPT("failed", r);
    }
    if (n != NDIMS) {
        TESTRPT("wrong number of dimensions", n);
    }

    r = miget_volume_dimension_count(vol, MI_DIMCLASS_TIME, MI_DIMATTR_ALL, 
                                     &n);
    if (r < 0) {
        TESTRPT("failed", r);
    }
    if (n != 1) {
        TESTRPT("wrong number of time dimensions", n);
    }

    for (i = 0; i < NDIMS; i++) {
        miboolean_t flag_value;

        r = miget_dimension_sampling_flag(dim[i], &flag_value);
        if (r < 0) {
            TESTRPT("error getting sampling flag", r);
        }
        else if (flag_value != (i == 0)) {
            TESTRPT("wrong value for sampling flag", i);
        }
    }

    r = miget_volume_dimensions(vol, MI_DIMCLASS_ANY, MI_DIMATTR_ALL,
                                MI_DIMORDER_FILE, NDIMS, dim_tmp);
    if (r < 0) {
        TESTRPT("failed to get dimensions", r);
    }

    for (i = 0; i < NDIMS; i++) {
        vol_tmp = NULL;
        r = miget_volume_from_dimension(dim_tmp[i], &vol_tmp);
        if (r < 0) {
            TESTRPT("failed to get volume from dimension", r);
        }
        else if (vol_tmp != vol) {
            TESTRPT("wrong volume returned", i);
        }
    }
    return (error_cnt);
}

int main(int argc, char **argv)
{
    mihandle_t vol;
    int r;
    midimhandle_t dim[NDIMS];
    int n;
    unsigned long coords[NDIMS];
    unsigned long count[NDIMS];
    int i,j,k;
    double offset;
    unsigned int voxel;

    /* Write data one voxel at a time. */
    for (i = 0; i < NDIMS; i++) {
        count[i] = 1;
    }

    r = micreate_dimension("time", MI_DIMCLASS_TIME,
                           MI_DIMATTR_NOT_REGULARLY_SAMPLED, CT, &dim[0]);
    if (r < 0) {
        TESTRPT("failed", r);
    }

    for (i = 0; i < CT; i++) {
        offset = (i * i) + 100.0;
        r = miset_dimension_offsets(dim[0], 1, i, &offset);
        if (r < 0) {
            TESTRPT("failed", r);
        }
    }

    r = micreate_dimension("xspace",MI_DIMCLASS_SPATIAL,
                           MI_DIMATTR_REGULARLY_SAMPLED, CX, &dim[1]);
    if (r < 0) {
        TESTRPT("failed", r);
    }

    r = miset_dimension_start(dim[1], XSTART);
    if (r < 0) {
        TESTRPT("failed", r);
    }

    r = miset_dimension_separation(dim[1], XSTEP);
    if (r < 0) {
        TESTRPT("failed", r);
    }

    r = micreate_dimension("yspace",MI_DIMCLASS_SPATIAL,
                           MI_DIMATTR_REGULARLY_SAMPLED, CY, &dim[2]);
    if (r < 0) {
        TESTRPT("failed", r);
    }

    r = miset_dimension_start(dim[2], YSTART);
    if (r < 0) {
        TESTRPT("failed", r);
    }

    r = miset_dimension_separation(dim[2], YSTEP);
    if (r < 0) {
        TESTRPT("failed", r);
    }

    r = micreate_dimension("zspace",MI_DIMCLASS_SPATIAL,
                           MI_DIMATTR_REGULARLY_SAMPLED, CZ, &dim[3]);
    if (r < 0) {
        TESTRPT("failed", r);
    }

    r = miset_dimension_start(dim[3], ZSTART);
    if (r < 0) {
        TESTRPT("failed", r);
    }

    r = miset_dimension_separation(dim[3], ZSTEP);
    if (r < 0) {
        TESTRPT("failed", r);
    }

    r = micreate_volume("tst-dim.mnc", NDIMS, dim, MI_TYPE_UINT,
                        MI_CLASS_REAL, NULL, &vol);
    if (r < 0) {
        TESTRPT("failed", r);
    }

    r = micreate_volume_image(vol);
    if (r < 0) {
        TESTRPT("failed", r);
    }

    check_dims(vol, dim);

    for (i = 0; i < CX; i++) {
        for (j = 0; j < CY; j++) {
            for (k = 0; k < CZ; k++) {
                coords[0] = 0;
                coords[1] = i;
                coords[2] = j;
                coords[3] = k;

                voxel = (i*10000)+(j*100)+k;
                r = miset_voxel_value_hyperslab(vol, 
                                                MI_TYPE_UINT,
                                                coords,
                                                count, 
                                                &voxel);
                if (r < 0) {
                    TESTRPT("Error writing voxel", r);
                }
            }
        }
    }


    r = miclose_volume(vol);
    if (r < 0) {
        TESTRPT("failed", r);
    }

    /***** 03-Aug-2004: Added two tests for bugs reported by Leila */

    r = miopen_volume("tst-dim.mnc", MI2_OPEN_RDWR, &vol);
    if (r < 0) {
        TESTRPT("failed", r);
    }

    r = miget_volume_dimension_count(vol, MI_DIMCLASS_ANY,
                                     MI_DIMATTR_REGULARLY_SAMPLED, &n);
    if (r < 0) {
        TESTRPT("failed", r);
    }
    if (n != NDIMS - 1) {
        TESTRPT("wrong result", n);
    }

    r = miget_volume_dimension_count(vol, MI_DIMCLASS_ANY,
                                     MI_DIMATTR_NOT_REGULARLY_SAMPLED, &n);
    if (r < 0) {
        TESTRPT("failed", r);
    }
    if (n != 1) {
        TESTRPT("wrong result", n);
    }

    r = miclose_volume(vol);
    if (r < 0) {
        TESTRPT("failed", r);
    }

    /* Test #2 - verify that we don't print anything scary if a user
     * closes a volume prematurely.
     */
    r = micreate_dimension("xspace",MI_DIMCLASS_SPATIAL,
                           MI_DIMATTR_REGULARLY_SAMPLED, CX, &dim[0]);
    if (r < 0) {
        TESTRPT("failed", r);
    }
    r = micreate_dimension("yspace",MI_DIMCLASS_SPATIAL,
                           MI_DIMATTR_REGULARLY_SAMPLED, CY, &dim[1]);
    if (r < 0) {
        TESTRPT("failed", r);
    }
    r = micreate_dimension("zspace",MI_DIMCLASS_SPATIAL,
                           MI_DIMATTR_REGULARLY_SAMPLED, CZ, &dim[2]);
    if (r < 0) {
        TESTRPT("failed", r);
    }
    r = micreate_volume("tst-vol.mnc", 3, dim, MI_TYPE_SHORT,
                        MI_CLASS_LABEL, NULL, &vol);
    if (r < 0) {
        TESTRPT("failed", r);
    }

    r = miclose_volume(vol);
    if (r < 0) {
        TESTRPT("failed", r);
    }
    /** End of tests added 03-Aug-2004 **/

    if (error_cnt != 0) {
        fprintf(stderr, "%d error%s reported\n", 
                error_cnt, (error_cnt == 1) ? "" : "s");
    }
    else {
        fprintf(stderr, "No errors\n");
    }
    return (error_cnt);
}

