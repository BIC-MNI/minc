#include <stdio.h>
#include "minc2.h"

#define NDIMS 3
#define CX 11
#define CY 10
#define CZ 9

#define TESTRPT(msg, val) (error_cnt++, fprintf(stderr, \
                                  "Error reported on line #%d, %s: %d\n", \
                                  __LINE__, msg, val))
static int error_cnt = 0;

int 
main(int argc, char **argv)
{
    mihandle_t hvol;
    int result;
    unsigned long start[NDIMS];
    unsigned long count[NDIMS] = {CX,CY,CZ};
    double dtemp[CX][CY][CZ];
    unsigned short stemp[CX][CY][CZ];
    unsigned char btemp[CX][CY][CZ];
    unsigned short stmp2[CX][CY][CZ];
    int i,j,k;
    long imap[NDIMS];
    long offset;
    long sizes[NDIMS] = {CX,CY,CZ};
    midimhandle_t hdims[NDIMS];
    char *dimnames[] = {"xspace", "yspace", "zspace"};
    
    result = micreate_dimension("xspace", MI_DIMCLASS_SPATIAL, 
                                MI_DIMATTR_REGULARLY_SAMPLED, CX, &hdims[0]);

    result = micreate_dimension("yspace", MI_DIMCLASS_SPATIAL, 
                                MI_DIMATTR_REGULARLY_SAMPLED, CY, &hdims[1]);

    result = micreate_dimension("zspace", MI_DIMCLASS_SPATIAL, 
                                MI_DIMATTR_REGULARLY_SAMPLED, CZ, &hdims[2]);

    result = micreate_volume("tst-hyper.mnc", NDIMS, hdims, MI_TYPE_UINT, 
                             MI_CLASS_REAL, NULL, &hvol);
    if (result < 0) {
        TESTRPT("Unable to create test volume", result);
    }

    result = miget_volume_dimensions(hvol, MI_DIMCLASS_ANY, MI_DIMATTR_ALL, 
                                     MI_DIMORDER_FILE, NDIMS, hdims);
    if (result < 0) {
        TESTRPT("Unable to get volume dimensions", result);
    }

    micreate_volume_image(hvol);

    start[0] = start[1] = start[2] = 0;
    for (i = 0; i < CX; i++) {
        for (j = 0; j < CY; j++) {
            for (k = 0; k < CZ; k++) {
                stemp[i][j][k] = (i*101)+(j*17)+(k);
            }
        }
    }

    result = miset_volume_valid_range(hvol, (CX*101)+(CY*17)+CZ, 0);
    if (result < 0) {
        TESTRPT("error setting valid range\n", result);
    }

    result = miset_volume_range(hvol, 1, -1);
    if (result < 0) {
        TESTRPT("error setting real range\n", result);
    }

    result = miset_voxel_value_hyperslab(hvol, MI_TYPE_SHORT, start, count, 
                                         stemp);
    if (result < 0) {
        TESTRPT("unable to set hyperslab\n", result);
    }
    
    miset_apparent_dimension_order_by_name(hvol, NDIMS, dimnames);

    miset_dimension_apparent_voxel_order(hdims[2], MI_COUNTER_FILE_ORDER);


    start[0] = start[1] = start[2] = 0;
    result = miget_real_value_hyperslab(hvol, MI_TYPE_DOUBLE, start, count,
                                        dtemp);
        
    printf("miget_real_value_hyperslab()\n");

    if (result < 0) {
        TESTRPT("oops\n", result);
    }
    else {
        for (i = 0; i < CX; i++) {
            for (j = 0; j < CY; j++) {
                printf("[%d][%d]: ", i, j);
                for (k = 0; k < CZ; k++) {
                    printf("%f ", dtemp[i][j][k]);
                }
                printf("\n");
            }
        }
    }

    printf("miget_voxel_value_hyperslab()\n");

    start[0] = start[1] = start[2] = 0;
    i = miget_voxel_value_hyperslab(hvol, MI_TYPE_USHORT, start, count,
                                    stemp);
    if (i < 0) {
        printf("oops\n");
    }
    else {
        for (i = 0; i < CX; i++) {
            for (j = 0; j < CY; j++) {
                printf("[%d][%d]: ", i, j);
                for (k = 0; k < CZ; k++) {
                    printf("%u ", stemp[i][j][k]);
                }
                printf("\n");
            }
        }
    }

    printf("miget_hyperslab_normalized()\n");

    i = miget_hyperslab_normalized(hvol, MI_TYPE_UBYTE, start, count,
                                   0.0, 0.001, btemp);
    if (i < 0) {
        printf("oops\n");
    }
    else {
        for (i = 0; i < CX; i++) {
            for (j = 0; j < CY; j++) {
                printf("[%d][%d]: ", i, j);
                for (k = 0; k < CZ; k++) {
                    printf("%u ", btemp[i][j][k]);
                }
                printf("\n");
            }
        }
    }

    i = miget_voxel_value_hyperslab(hvol, MI_TYPE_USHORT, start, count, stemp);
    if (i < 0) {
        printf("oops\n");
    }
    else {
        for (i = 0; i < CX; i++) {
            for (j = 0; j < CY; j++) {
                for (k = 0; k < CZ; k++) {
                    stemp[i][j][k] = stemp[i][j][k] ^ 0x55;
                }
            }
        }
    }
    i = miset_voxel_value_hyperslab(hvol, MI_TYPE_USHORT, start, count, stemp);
    if (i < 0) {
        printf("oops\n");
    }
    else {
        for (i = 0; i < CX; i++) {
            for (j = 0; j < CY; j++) {
                for (k = 0; k < CZ; k++) {
                    stemp[i][j][k] = stemp[i][j][k] ^ 0x55;
                }
            }
        }
    }
    i = miset_voxel_value_hyperslab(hvol, MI_TYPE_USHORT, start, count, stemp);
    miclose_volume(hvol);
    return (error_cnt);
}

