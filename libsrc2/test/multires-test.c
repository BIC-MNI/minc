#include <stdio.h>
#include "minc2.h"

#define TESTRPT(msg, val) (error_cnt++, fprintf(stderr, \
                                  "Error reported on line #%d, %s: %d\n", \
                                  __LINE__, msg, val))

static int error_cnt = 0;

#define CX 60
#define CY 55
#define CZ 50

#define NDIMS 3

int main(int argc, char **argv)
{
    mihandle_t vol;
    int r;
    midimhandle_t dim[NDIMS];
    mivolumeprops_t props;
    int n;
    unsigned long coords[NDIMS];
    unsigned long count[NDIMS];
    int i,j,k;
    unsigned int voxel;

    printf("Creating volume...\n");

    /* Write data one voxel at a time. */
    for (i = 0; i < NDIMS; i++) {
        count[i] = 1;
    }

    r = minew_volume_props(&props);
    r = miset_props_compression_type(props, MI_COMPRESS_ZLIB);
    r = miset_props_zlib_compression(props, 3);
    r = miset_props_multi_resolution(props, 1, 3);
    if (r < 0) {
        TESTRPT("failed", r);
    }

    r = micreate_dimension("xspace",MI_DIMCLASS_SPATIAL,MI_DIMATTR_REGULARLY_SAMPLED, CX,&dim[0]);
    if (r < 0) {
        TESTRPT("failed", r);
    }
  
    r = micreate_dimension("yspace",MI_DIMCLASS_SPATIAL,MI_DIMATTR_REGULARLY_SAMPLED, CY, &dim[1]);
    if (r < 0) {
        TESTRPT("failed", r);
    }
    r = micreate_dimension("zspace",MI_DIMCLASS_SPATIAL,MI_DIMATTR_REGULARLY_SAMPLED, CZ,&dim[2]);
    if (r < 0) {
        TESTRPT("failed", r);
    }
 
    r = micreate_volume("tst-multi.mnc", NDIMS, dim, MI_TYPE_UINT, MI_CLASS_REAL,props,&vol);
    if (r < 0) {
        TESTRPT("failed", r);
    }

    r = miset_volume_valid_range(vol, CX*10000.0 + CY*100 + CZ, 0.0);

    r = micreate_volume_image(vol);
    if (r < 0) {
        TESTRPT("failed", r);
    }
  
    r = miget_volume_dimension_count(vol, MI_DIMCLASS_SPATIAL, MI_DIMATTR_ALL, &n);
    if (r < 0) {
        TESTRPT("failed", r);
    }

    printf("Writing data...\n");

    for (i = 0; i < CX; i++) {
        for (j = 0; j < CY; j++) {
            for (k = 0; k < CZ; k++) {
                coords[0] = i;
                coords[1] = j;
                coords[2] = k;

                voxel = i*10000 + j*100 + k;
                
                r = miset_voxel_value_hyperslab(vol, MI_TYPE_UINT,
                                                coords, count, &voxel);
                if (r < 0) {
                    TESTRPT("Error writing voxel", r);
                }
            }
        }
    }

    printf("Selecting half-size image\n");

    r = miselect_resolution(vol, 1);
    if (r < 0) {
        TESTRPT("miselect_resolution failed", r);
    }

    /* OK, now try to read the lower-resolution hyperslab */
    coords[0] = 0;
    coords[1] = 0;
    coords[2] = 0;
    count[0] = CX/2;
    count[1] = CY/2;
    count[2] = CZ/2;

    {
        unsigned int buffer[CX/2][CY/2][CZ/2];
    
        r = miget_voxel_value_hyperslab(vol, MI_TYPE_UINT,
                                        coords, count, buffer);
        if (r < 0) {
            TESTRPT("failed", r);
        }
    }

    printf("Selecting quarter-size image\n");

    r = miselect_resolution(vol, 2);
    if (r < 0) {
        TESTRPT("miselect_resolution failed", r);
    }

    /* OK, now try to read the lower-resolution hyperslab */
    coords[0] = 0;
    coords[1] = 0;
    coords[2] = 0;
    count[0] = CX/4;
    count[1] = CY/4;
    count[2] = CZ/4;

    {
        unsigned int buffer[CX/4][CY/4][CZ/4];
    
        r = miget_voxel_value_hyperslab(vol, MI_TYPE_UINT,
                                        coords, count, buffer);
        if (r < 0) {
            TESTRPT("failed", r);
        }
    }

    printf("Return to full resolution.\n");

    r = miselect_resolution(vol, 0); /* Back to full resolution */
    if (r < 0) {
        TESTRPT("miselect_resolution failed", r);
    }

    printf("Flush any remaining thumbnails.\n");

    r = miflush_from_resolution(vol, 3);
    if (r < 0) {
        TESTRPT("failed", r);
    }

    r = miclose_volume(vol);
    if (r < 0) {
        TESTRPT("failed", r);
    }
      
    if (error_cnt != 0) {
        fprintf(stderr, "%d error%s reported\n", 
                error_cnt, (error_cnt == 1) ? "" : "s");
    }
    else {
        fprintf(stderr, "No errors\n");
    }
    return (error_cnt);
}

