#include <stdio.h>
#include <stdlib.h>
#include "minc2.h"

/* This test will attempt to create a few different test images
 * which can be tested with minc2.0
*/

#define TESTRPT(msg, val) (error_cnt++, fprintf(stderr, \
     "Error reported on line #%d, %s: %d\n", \
     __LINE__, msg, val))

#define CZ 142
#define CY 245
#define CX 120
#define NDIMS 3

int create_real_as_int_image(void)
{
    int r;
    double start_values[NDIMS]={-6.96, -12.453,  -9.48};
    double separations[NDIMS]={0.09,0.09,0.09};
    midimhandle_t hdim[NDIMS];
    mihandle_t hvol;
    int *buf = ( int *) malloc(CX * CY * CZ * sizeof(int));
    int i;
    long count[NDIMS];
    long start[NDIMS];
    miboolean_t flag=1;
    
    double min = -1.0;
    double max =  1.0;
    r = micreate_dimension("yspace", MI_DIMCLASS_SPATIAL, 
                           MI_DIMATTR_REGULARLY_SAMPLED, CY, &hdim[0]);

    r = micreate_dimension("xspace", MI_DIMCLASS_SPATIAL, 
                                MI_DIMATTR_REGULARLY_SAMPLED, CX, &hdim[1]);
    
    r = micreate_dimension("zspace", MI_DIMCLASS_SPATIAL, 
                           MI_DIMATTR_REGULARLY_SAMPLED, CZ, &hdim[2]);

    r = miset_dimension_starts(hdim, NDIMS, start_values);
    r = miset_dimension_separations(hdim, NDIMS, separations);

    r = micreate_volume("real_as_int_image.mnc", NDIMS, hdim, MI_TYPE_INT, 
			MI_CLASS_REAL, NULL, &hvol);

    /* set slice scaling flag to true */
    r = miset_slice_scaling_flag(hvol, flag);

    r = micreate_volume_image(hvol);

    for (i = 0; i < CY*CX*CZ; i++) {
        buf[i] = (int) i * 0.001;
    }

    start[0] = start[1] = start[2] = 0;
    count[0] = CY; count[1] = CX; count[2] = CZ;
    
    r = miset_voxel_value_hyperslab(hvol, MI_TYPE_INT, start, count, buf);
    /* Set random values to slice min and max for slice scaling*/
    start[0] =start[1]=start[2]=0;
    for (i=0; i < CY; i++) {
      start[0] = i;
      min += -0.1;
      max += 0.1;
      r = miset_slice_range(hvol,start,NDIMS , max, min);
    }
    
    r = miclose_volume(hvol);
}
int create_real_as_float_image(void)
{
    int r;
    double start_values[NDIMS]={-6.96, -12.453,  -9.48};
    double separations[NDIMS]={0.09,0.09,0.09};
    midimhandle_t hdim[NDIMS];
    mihandle_t hvol;
    float *buf = (float *) malloc(CX * CY * CZ * sizeof(float));
    int i;
    long count[NDIMS];
    long start[NDIMS];
    miboolean_t flag=1;
    
    double min = -1.0;
    double max =  1.0;
    r = micreate_dimension("yspace", MI_DIMCLASS_SPATIAL, 
                           MI_DIMATTR_REGULARLY_SAMPLED, CY, &hdim[0]);

    r = micreate_dimension("xspace", MI_DIMCLASS_SPATIAL, 
                                MI_DIMATTR_REGULARLY_SAMPLED, CX, &hdim[1]);
    
    r = micreate_dimension("zspace", MI_DIMCLASS_SPATIAL, 
                           MI_DIMATTR_REGULARLY_SAMPLED, CZ, &hdim[2]);

    r = miset_dimension_starts(hdim, NDIMS, start_values);
    r = miset_dimension_separations(hdim, NDIMS, separations);
    
    r = micreate_volume("real_as_float_image.mnc", NDIMS, hdim, MI_TYPE_FLOAT, 
			MI_CLASS_REAL, NULL, &hvol);
    
    /* set slice scaling flag to true */
    r = miset_slice_scaling_flag(hvol, flag);

    r = micreate_volume_image(hvol);
    
    for (i = 0; i < CY*CX*CZ; i++) {
        buf[i] =  i * 0.001;
    }

    start[0] = start[1] = start[2] = 0;
    count[0] = CY; count[1] = CX; count[2] = CZ;
    
    r = miset_voxel_value_hyperslab(hvol, MI_TYPE_FLOAT, start, count, buf);
    /* Set random values to slice min and max for slice scaling*/
    start[0] =start[1]=start[2]=0;
    for (i=0; i < CY; i++) {
      start[0] = i;
      min += -0.1;
      max += 0.1;
      r = miset_slice_range(hvol,start,NDIMS , max, min);
    }
    
    r = miclose_volume(hvol);
}
int main(int argc, char **argv)
{
  int r = 0;
  printf("Creating 3D image REAL stored as INT w/ slice scaling!! (real_as_int_image.mnc)\n");
  r +=create_real_as_int_image();
  printf("Creating 3D image REAL stored as FLOAT w/ slice scaling!! (real_as_float_image.mnc)\n");  r +=create_real_as_float_image();

  if (r != 0) {
    fprintf(stderr, "%d error%s reported\n", 
	    r, (r == 1) ? "" : "s");
  }
  else {
    fprintf(stderr, "\n No errors\n");
  }
    
  return (r);
}

