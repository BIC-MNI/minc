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
    int result = 1;
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

    r = miset_volume_valid_range(vol, 100000.0, 0.0);

    r = micreate_volume_image(vol);
    if (r < 0) {
        TESTRPT("failed", r);
    }
  
    r = miget_volume_dimension_count(vol, MI_DIMCLASS_SPATIAL, MI_DIMATTR_ALL, &n);
    if (r < 0) {
        TESTRPT("failed", r);
    }
    printf( " N is %d \n", n);

    for (i = 0; i < CX; i++) {
        for (j = 0; j < CY; j++) {
            for (k = 0; k < CZ; k++) {
                coords[0] = i;
                coords[1] = j;
                coords[2] = k;

                voxel = i*10000 + j*100 + k;
                
                result = miset_voxel_value_hyperslab(vol, MI_TYPE_UINT,
						     coords, count, &voxel);
                if (result < 0) {
                    TESTRPT("Error writing voxel", result);
                }
            }
        }
    }

  /* call miselect_resolution() 
   */
#if 0 
  r = miflush_from_resolution(vol, 3);
  if (r < 0) {
    TESTRPT("failed", r);
  }
#endif
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

