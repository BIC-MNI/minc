/* A test program to evaluate some of the MINC2 API's and features.
 */
#include <stdio.h>
#include "minc2.h"

#define ND 3
#define CX 100
#define CY 100
#define CZ 100

int test1(int do_real)
{
    int i, j, k;
    int r;
    mivolumeprops_t hprops;
    midimhandle_t hdim[ND];
    mihandle_t hvol;
    double offsets[CX];
    unsigned long coords[ND];

    for (i = 0; i < CX; i++) {
        offsets[i] = (double) i * (double) i;
    }

    r = minew_volume_props(&hprops);
    if (r != 0) {
        fprintf(stderr, "unexpected error\n");
        return (1);
    }

    r = miset_props_compression_type(hprops, MI_COMPRESS_ZLIB);
    if (r != 0) {
        fprintf(stderr, "unexpected error\n");
        return (1);
    }

    r = micreate_dimension("xspace", MI_DIMCLASS_SPATIAL, 
                           MI_DIMATTR_NOT_REGULARLY_SAMPLED, CX, &hdim[0]);
    if (r != 0) {
        fprintf(stderr, "unexpected error\n");
        return (1);
    }

    r = miset_dimension_offsets(hdim[0], 100, 0, offsets);
    if (r != 0) {
        fprintf(stderr, "unexpected error\n");
        return (1);
    }

    r = micreate_dimension("yspace", MI_DIMCLASS_SPATIAL, 
                           MI_DIMATTR_REGULARLY_SAMPLED, CY, &hdim[1]);
    if (r < 0) {
        return (1);
    }

    r = miset_dimension_start(hdim[1], -10.0);
    if (r < 0) {
        return (1);
    }

    r = micreate_dimension("zspace", MI_DIMCLASS_SPATIAL, 
                           MI_DIMATTR_REGULARLY_SAMPLED, CZ, &hdim[2]);
    if (r < 0) {
        return (1);
    }

    r = miset_dimension_start(hdim[2], -20.0);
    if (r != 0) {
        fprintf(stderr, "error setting dimension start\n");
        return (1);
    }

    r = miset_dimension_separation(hdim[2], 2.0);
    if (r != 0) {
        fprintf(stderr, "error setting dimension separation\n");
        return (1);
    }

    r = micreate_volume("fulltest.mnc", 3, hdim, MI_TYPE_SHORT, MI_CLASS_REAL,
                        hprops, &hvol);
    if (r != 0) {
        fprintf(stderr, "error creating volume\n");
        return (1);
    }

    r = micreate_volume_image(hvol);
    if (r != 0) {
        fprintf(stderr, "error creating volume image\n");
        return (1);
    }

    r = miset_volume_valid_range(hvol, 1000, -1000);
    if (r != 0) {
        fprintf(stderr, "error setting valid range\n");
        return (1);
    }

    r = miset_volume_range(hvol, 5, -5);
    if (r < 0) {
        fprintf(stderr, "error setting volume range\n");
        return (1);
    }

    for (i = 0; i < CX; i++) {
        for (j = 0; j < CY; j++) {
           for (k = 0; k < CZ; k++) {
                coords[0] = i;
                coords[1] = j;
                coords[2] = k;

                if (do_real) {
                    r = miset_real_value(hvol, coords, ND, 1.0);
                    if (r < 0) {
                        return (1);
                    }
                }
                else {
                    r = miset_voxel_value(hvol, coords, ND, 200.0);
                    if (r < 0) {
                        return (1);
                    }
                }
            }
        }
    }

    r = mifree_volume_props(hprops);
    if (r < 0) {
        return (1);
    }

    r = miclose_volume(hvol);
    if (r < 0) {
        return (1);
    }

    return (0);
}

#define CCX 20
#define CCY 20
#define CCZ 20

int test2()
{
    midimhandle_t hdim[ND];
    mihandle_t hvol;
    int r;
    int i,j,k;
    unsigned long coords[ND];
    unsigned long lengths[ND];
    mifcomplex_t fcmpl;

    r = micreate_dimension("xspace", MI_DIMCLASS_SPATIAL, 
                           MI_DIMATTR_REGULARLY_SAMPLED, CCX, &hdim[2]);
    if (r != 0) {
        return (1);
    }

    r = micreate_dimension("yspace", MI_DIMCLASS_SPATIAL, 
                           MI_DIMATTR_REGULARLY_SAMPLED, CCY, &hdim[1]);

    if (r < 0) {
        return (1);
    }

    r = miset_dimension_start(hdim[1], -10.0);
    if (r < 0) {
        return (1);
    }

    r = micreate_dimension("zspace", MI_DIMCLASS_SPATIAL, 
                           MI_DIMATTR_REGULARLY_SAMPLED, CCZ, &hdim[0]);
    if (r < 0) {
        return (1);
    }

    r = miset_dimension_start(hdim[2], -20.0);
    if (r < 0) {
        fprintf(stderr, "error setting dimension start\n");
        return (1);
    }

    r = miset_dimension_separation(hdim[2], 2.0);
    if (r < 0) {
        fprintf(stderr, "error setting dimension separation\n");
        return (1);
    }

    r = micreate_volume("cmpltest.mnc", 3, hdim, MI_TYPE_FCOMPLEX, 
                        MI_CLASS_COMPLEX, NULL, &hvol);
    if (r != 0) {
        fprintf(stderr, "error creating volume\n");
        return (1);
    }

    r = miset_volume_valid_range(hvol, 1919.0, 0.0);

    r = micreate_volume_image(hvol);
    if (r != 0) {
        fprintf(stderr, "error creating volume image\n");
        return (1);
    }

    lengths[0] = lengths[1] = lengths[2] = 1;

    for (i = 0; i < CCX; i++) {
        for (j = 0; j < CCY; j++) {
            for (k = 0; k < CCZ; k++) {
                coords[0] = i;
                coords[1] = j;
                coords[2] = k;

                fcmpl.real = (i * 100) + j;
                fcmpl.imag = k;
                r = miset_voxel_value_hyperslab(hvol, 
                                                MI_TYPE_FCOMPLEX,
                                                coords,
                                                lengths,
                                                &fcmpl);
                if (r < 0) {
                    fprintf(stderr, "error writing complex voxel\n");
                    return (1);
                }
            }
        }
    }

    for (i = 0; i < CCX; i++) {
        for (j = 0; j < CCY; j++) {
            for (k = 0; k < CCZ; k++) {
                coords[0] = i;
                coords[1] = j;
                coords[2] = k;

                r = miget_voxel_value_hyperslab(hvol, 
                                                MI_TYPE_FCOMPLEX,
                                                coords,
                                                lengths,
                                                &fcmpl);
                if (r < 0) {
                    fprintf(stderr, "error writing complex voxel\n");
                    return (1);
                }

                if (fcmpl.real != (i * 100) + j || fcmpl.imag != k) {
                    fprintf(stderr, "value mismatch for complex voxel\n");
                    return (1);
                }
            }
        }
    }

    r = miclose_volume(hvol);
    if (r < 0) {
        return (1);
    }

    return (0);
}

int
main(int argc, char **argv)
{
    int errors;
    int do_real = 0;

    while (--argc > 0) {
        char *argp = *++argv;
        if (*argp == '-') {
            argp++;
            switch (*argp) {
            case 'r':
                do_real = 1;
                break;
            }
        }
    }

    errors = 0;
    errors += test1(do_real);
    errors += test2();

    if (errors == 0) {
        printf("No errors\n");
    }
    else {
        printf("%d error%s found\n", errors, (errors == 1) ? "" : "s");
    }
    return (errors);
}

