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
#define CX 10
#define CU 5
#define NDIMS 3

int create_2D_image(void)
{
  int r,i;
  midimhandle_t hdim[NDIMS-1];
  mihandle_t hvol;
  short *buf = (short *)malloc(CX * CY * sizeof(short));
  double *offsets = (double *)malloc(CX * sizeof(double)); 
  double start_values[NDIMS-1]={-1.01, -2.02};
  miboolean_t flag=0;
  long count[NDIMS-1];
  long start[NDIMS-1];

  r = micreate_dimension("xspace", MI_DIMCLASS_SPATIAL, MI_DIMATTR_NOT_REGULARLY_SAMPLED, CX, &hdim[0]);

 
  
  r = micreate_dimension("yspace", MI_DIMCLASS_USER, MI_DIMATTR_REGULARLY_SAMPLED, CY, &hdim[1]);
  
   for(i=0; i < CX; i++)
    {
      offsets[i] = (i * i) + 0.1;
    } 
    r = miset_dimension_offsets(hdim[0], CX, 0, offsets);
  r = miset_dimension_separation(hdim[1], 0.06);
  r = miset_dimension_starts(hdim,  NDIMS-1, start_values);
  
  r = micreate_volume("2D_image.mnc", NDIMS-1 , hdim, MI_TYPE_SHORT, 
		      MI_CLASS_REAL, NULL, &hvol);

  /* set slice scaling flag to true */
  r = miset_slice_scaling_flag(hvol, flag);

  r = micreate_volume_image(hvol);

  for (i = 0; i < CX*CY; i++) {
    buf[i] = (short) i * 0.1;
  }
  start[0] = start[1] = 0;
  count[0] = CX; count[1] = CY;
    
  r = miset_voxel_value_hyperslab(hvol, MI_TYPE_SHORT, start, count, buf);

  r = miclose_volume(hvol);
}

int create_3D_image(void)
{
    int r;
    double start_values[NDIMS]={-6.96, -12.453,  -9.48};
    double separations[NDIMS]={0.09,0.09,0.09};
    midimhandle_t hdim[NDIMS];
    mihandle_t hvol;
    unsigned short *buf = ( unsigned short *) malloc(CX * CY * CZ * sizeof(unsigned short));
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

    r = micreate_volume("3D_image.mnc", NDIMS, hdim, MI_TYPE_USHORT, 
                    MI_CLASS_REAL, NULL, &hvol);
    /* set slice scaling flag to true */
    r = miset_slice_scaling_flag(hvol, flag);

    r = micreate_volume_image(hvol);

    for (i = 0; i < CY*CX*CZ; i++) {
        buf[i] = (unsigned short) i * 0.001;
    }
    
    start[0] = start[1] = start[2] = 0;
    count[0] = CY; count[1] = CX; count[2] = CZ;
    
    r = miset_voxel_value_hyperslab(hvol, MI_TYPE_USHORT, start, count, buf);
    /* Set random values to slice min and max for slice scaling*/
    start[0] =start[1]=start[2]=0;
    for (i=0; i < CY; i++) {
      start[0] = i;
      min += 0.1;
      max += 0.1;
      r = miset_slice_range(hvol,start,NDIMS , max, min);
    }
    
    r = miclose_volume(hvol);
}

int create_4D_image(void)
{
    int r;
    double start_values[NDIMS+1]={-6.96, -12.453,  -9.48, 20.002};
    double separations[NDIMS+1]={0.09,0.09,0.09, 1};
    midimhandle_t hdim[NDIMS+1];
    mihandle_t hvol;
    unsigned char *buf = (unsigned char *) malloc(CX * CU * CZ * CY * sizeof(unsigned char));
    int i,j;
    long count[NDIMS+1];
    long start[NDIMS+1];
    miboolean_t flag=1;
    
    double min = -1.0;
    double max =  1.0;
    r = micreate_dimension("xspace", MI_DIMCLASS_SPATIAL, 
                           MI_DIMATTR_REGULARLY_SAMPLED, CX, &hdim[0]);

    r = micreate_dimension("user", MI_DIMCLASS_USER, 
                                MI_DIMATTR_REGULARLY_SAMPLED, CU, &hdim[1]);
    
    r = micreate_dimension("zspace", MI_DIMCLASS_SPATIAL, 
                           MI_DIMATTR_REGULARLY_SAMPLED, CZ, &hdim[2]);

    r = micreate_dimension("yspace", MI_DIMCLASS_SPATIAL, 
                           MI_DIMATTR_REGULARLY_SAMPLED, CY, &hdim[3]);

    r = miset_dimension_starts(hdim, NDIMS+1, start_values);
    r = miset_dimension_separations(hdim, NDIMS+1, separations);

    r = micreate_volume("4D_image.mnc", NDIMS+1, hdim, MI_TYPE_UBYTE, 
                    MI_CLASS_REAL, NULL, &hvol);

    /* set slice scaling flag to true */
    r = miset_slice_scaling_flag(hvol, flag);

    r = micreate_volume_image(hvol);

    for (i = 0; i < CX*CU*CZ*CY; i++) {
        buf[i] = (unsigned char) i;
    }
    
    start[0] = start[1] = start[2] = start[3] = 0;
    count[0] = CX; count[1] = CU; count[2] = CZ; count[3] = CY;
    
    r = miset_voxel_value_hyperslab(hvol, MI_TYPE_UBYTE, start, count, buf);
    /* Set random values to slice min and max for slice scaling*/
    start[0] =start[1]=start[2]=start[3]=0;
    for (i=0; i < CX; i++) {
      
      start[0] = i;
      for ( j=0; j < CU; j++)
	{
	start[1] = j;
	min += -0.1;
	max += 0.1;
	r = miset_slice_range(hvol,start,NDIMS+1 , max, min);
	}
    }
    
    r = miclose_volume(hvol);
}

int main(int argc, char **argv)
{
  int r=0;
  printf("Creating 2D image with IRREGULAR sample dimension!! (2D_image.mnc)\n");
  r +=create_2D_image();
  printf("Creating 3D image with slice scaling!! (3D_image.mnc)\n");
  r +=create_3D_image();
  printf("Creating 4D image with slice scaling!! (4D_image.mnc)\n");
  r +=create_4D_image();

  if (r != 0) {
    fprintf(stderr, "%d error%s reported\n", 
	    r, (r == 1) ? "" : "s");
  }
  else {
    fprintf(stderr, "\n No errors\n");
  }
    
  return (r);
}

