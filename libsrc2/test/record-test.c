#include <stdio.h>
#include "minc2.h"

#define TESTRPT(msg, val) (error_cnt++, fprintf(stderr, \
                                  "Error reported on line #%d, %s: %d\n", \
                                  __LINE__, msg, val))

static int error_cnt = 0;

#define CX 10
#define CY 10
#define CZ 6
#define NDIMS 3

int
main(int argc, char **argv)
{
    mihandle_t hvol;
    char *name;
    int result;
    midimhandle_t hdim[NDIMS];
    unsigned long coords[NDIMS];
    unsigned long count[NDIMS];
    int i,j,k;
    struct test {
        int r;
        int g;
        int b;
    } voxel;

    /* Write data one voxel at a time. */
    for (i = 0; i < NDIMS; i++) {
        count[i] = 1;
    }

    result = micreate_dimension("xspace", MI_DIMCLASS_SPATIAL, 
                           MI_DIMATTR_REGULARLY_SAMPLED, CX, &hdim[0]);

    result = micreate_dimension("yspace", MI_DIMCLASS_SPATIAL, 
                           MI_DIMATTR_REGULARLY_SAMPLED, CY, &hdim[1]);

    result = micreate_dimension("zspace", MI_DIMCLASS_SPATIAL, 
                           MI_DIMATTR_REGULARLY_SAMPLED, CZ, &hdim[2]);

    result = micreate_volume("tst-rec.mnc", NDIMS, hdim, MI_TYPE_UINT, 
                             MI_CLASS_UNIFORM_RECORD, NULL, &hvol);
    if (result < 0) {
	TESTRPT("Unable to create test file", result);
    }

    result = miset_record_field_name(hvol, 0, "Red");
    if (result < 0) {
        TESTRPT("miset_record_field_name", result);
    }
    miset_record_field_name(hvol, 1, "Green");
    miset_record_field_name(hvol, 2, "Blue");

    miget_record_field_name(hvol, 1, &name);
    if (strcmp(name, "Green") != 0) {
	TESTRPT("Unexpected label for value 1", 0);
    }
    mifree_name(name);

    miget_record_field_name(hvol, 0, &name);
    if (strcmp(name, "Red") != 0) {
	TESTRPT("Unexpected label for value 0", 0);
    }
    mifree_name(name);

    miget_record_field_name(hvol, 2, &name);
    if (strcmp(name, "Blue") != 0) {
	TESTRPT("Unexpected label for value 2", 0);
    }
    mifree_name(name);

    result = micreate_volume_image(hvol);
    if (result < 0) {
        TESTRPT("micreate_volume_image failed", result);
    }

    for (i = 0; i < CX; i++) {
        for (j = 0; j < CY; j++) {
            for (k = 0; k < CZ; k++) {
                coords[0] = i;
                coords[1] = j;
                coords[2] = k;

                voxel.r = i;
                voxel.g = j;
                voxel.b = k;
                
                result = miset_voxel_value_hyperslab(hvol, MI_TYPE_UNKNOWN,
                                                     coords, count, &voxel);
                if (result < 0) {
                    TESTRPT("Error writing voxel", result);
                }
            }
        }
    }

    for (i = 0; i < CX; i++) {
        for (j = 0; j < CY; j++) {
            for (k = 0; k < CZ; k++) {
                coords[0] = i;
                coords[1] = j;
                coords[2] = k;

                result = miget_voxel_value_hyperslab(hvol, MI_TYPE_UNKNOWN,
                                                     coords, count, &voxel);
                if (result < 0) {
                    TESTRPT("Error reading voxel", result);
                }
                if (voxel.r != i || voxel.g != j || voxel.b != k) {
                    TESTRPT("Data mismatch", 0);
                }
            }
        }
    }

    miclose_volume(hvol);

    if (error_cnt != 0) {
	fprintf(stderr, "%d error%s reported\n", 
		error_cnt, (error_cnt == 1) ? "" : "s");
    }
    else {
	fprintf(stderr, "No errors\n");
    }
    return (error_cnt);
}

