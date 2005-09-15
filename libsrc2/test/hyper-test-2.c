#include <stdio.h>
#include <stdlib.h>
#include "minc2.h"

/* Courtesy of Dr Jason Lerch
   * A test of the dimensions ordering/hyperslab functions in minc2. The
   * basic flow is the following:
   * open an existing minc volume
   * set the apparent dimension order to be different from the file order
   * get the volume dimensions
   * load the entire image into a buffer using the hyperslab function
   * compare with single voxel value returned from single voxel functions.
*/

#define TESTRPT(msg, val) (error_cnt++, fprintf(stderr, \
     "Error reported on line #%d, %s: %d\n", \
     __LINE__, msg, val))

static int error_cnt = 0;

#define CZ 142
#define CY 245
#define CX 210
#define NDIMS 3

int create_test_file(void)
{
    int r;
    double start_values[3]={-6.96, -12.453,  -9.48};
    double separations[3]={0.09,0.09,0.09};
    midimhandle_t hdim[NDIMS];
    mihandle_t hvol;
    unsigned short *buf = malloc(CZ*CX*CY);
    int i;
    long count[NDIMS];
    long start[NDIMS];
    miboolean_t flag=1;
    
    double min = -1.0;
    double max =  1.0;
    r = micreate_dimension("zspace", MI_DIMCLASS_SPATIAL, 
                           MI_DIMATTR_REGULARLY_SAMPLED, CZ, &hdim[0]);

    r = micreate_dimension("yspace", MI_DIMCLASS_SPATIAL, 
                                MI_DIMATTR_REGULARLY_SAMPLED, CY, &hdim[1]);
    
    r = micreate_dimension("xspace", MI_DIMCLASS_SPATIAL, 
                           MI_DIMATTR_REGULARLY_SAMPLED, CX, &hdim[2]);

    r = miset_dimension_starts(hdim, NDIMS, start_values);
    r = miset_dimension_separations(hdim, NDIMS, separations);

    r = micreate_volume("hyperslab-test2.mnc", NDIMS, hdim, MI_TYPE_USHORT, 
                    MI_CLASS_REAL, NULL, &hvol);
    /* set slice scaling flag to true */
    r = miset_slice_scaling_flag(hvol, flag);

    r = micreate_volume_image(hvol);

    for (i = 0; i < CZ*CY*CX; i++) {
        buf[i] = (unsigned short) i * 0.01;
    }
    
    start[0] = start[1] = start[2] = 0;
    count[0] = CZ;
    count[1] = CY;
    count[2] = CX;
    
    r = miset_voxel_value_hyperslab(hvol, MI_TYPE_USHORT, start, count, buf);
    /* Set random values to slice min and max for slice scaling*/
    start[0] =start[1]=start[2]=0;
    for (i=0; i < CZ; i++) {
      start[0] = i;
      min += 0.1;
      max += 0.1;
      r = miset_slice_range(hvol,start, 3, max, min);
    }
    
    r = miclose_volume(hvol);
}

int main(int argc, char **argv)
{
    mihandle_t vol;
    
    midimhandle_t dim[NDIMS];

    unsigned long sizes[NDIMS];    
    unsigned long start[NDIMS];
    unsigned long count[NDIMS];
    unsigned long howfar[NDIMS];
    unsigned long location[NDIMS];

    unsigned short * Atmp;
    double *buffer,value;
    int r = 0;
    
    static char *dimorder[] = {"xspace", "yspace", "zspace"};
    Atmp = ( unsigned short *) malloc(CX * CY * CZ * sizeof(unsigned short));
   
    printf("Creating image with slice scaling!! \n");
    create_test_file();
    printf("Opening hyperslab-test2.mnc! \n");
   
    r = miopen_volume("hyperslab-test2.mnc", MI2_OPEN_READ, &vol);
    
    if (r < 0) {
	TESTRPT("failed to open image", r);
    }

#ifdef APPARENTORDER
    /* set the apparent dimension order to be xyz */
    r  = miset_apparent_dimension_order_by_name(vol, 3, dimorder);

    /* get the apparent dimensions and their sizes */
    r  = miget_volume_dimensions( vol, MI_DIMCLASS_SPATIAL,
			     MI_DIMATTR_ALL, MI_DIMORDER_APPARENT,
			     3, dim);
    r = miget_dimension_sizes( dim, 3, sizes );
#else
    /* get the apparent dimensions and their sizes */
    r = miget_volume_dimensions( vol, MI_DIMCLASS_SPATIAL,
				 MI_DIMATTR_ALL, MI_DIMORDER_FILE,
				 3, dim);
    r = miget_dimension_sizes( dim, 3, sizes );
#endif
    if (r == MI_NOERROR) {
    printf("Sizes: %d, %d, %d\n", sizes[0], sizes[1], sizes[2]);
    }
    else {
      fprintf(stderr, "Error getting dimension sizes\n");
    }
    /* try to play with hyperslab functions!! */
    start[0] = 4;
    start[1] = 3;
    start[2] = 5;
    
    howfar[0] = 120;
    howfar[1] = 180;
    howfar[2] = 110;

    count[0] = howfar[0] - start[0];
    count[1] = howfar[1] - start[1];
    count[2] = howfar[2] - start[2];
    
    /* Alocate memory for the hyperslab*/
    buffer = (double *)malloc(count[0] * count[1] * count[2] * sizeof(double));
    if (buffer == NULL) {
      fprintf(stderr, "Error allocation memory.\n");
      exit(-1);
    }

    /* Get real value hyperslab*/
    printf("\n");
    printf("Getting a real value hyperslab \n");
    printf("Starting at %d, %d, %d \n", start[0], start[1], start[2]);
    printf("Extending to %d, %d, %d \n", howfar[0], howfar[1], howfar[2]);
    printf("\n");
    if (miget_real_value_hyperslab(vol,MI_TYPE_DOUBLE, start, count, buffer)
	< 0) {
      fprintf(stderr, "Could not get hyperslab.\n");
      exit(-1);
    }
    /* set an arbitrary location to print values from */
    location[0] = 70; location[1] = 100; location[2] = 104;
    printf("Test arbitrary location %d, %d, %d \n", 
	   location[0], location[1], location[2]);
    miget_real_value(vol, location, 3, &value);
    printf("Test from hyperslab: %f \n", 
	 *( buffer + (location[0] - start[0])*count[1]*count[2] +
	    (location[1]- start[1]) * count[2] + (location[2]- start[2])));
    printf("Test from voxel scaled: %f\n", value);
    miget_voxel_value(vol, location, 3, &value);
    printf("Test voxel value itself: %f\n", value);
    printf("\n");
    printf("HMMMMMMMMMM! let's try something else \n");
    printf("\n");
    /* set another arbitrary location to print values from */
    location[0] = 104; location[1] = 100; location[2] = 70;
    printf("Test arbitrary location %d, %d, %d \n", 
	   location[0], location[1], location[2]);
    miget_real_value(vol, location, 3, &value);
    printf("Test from hyperslab: %f \n", 
	 *( buffer + (location[0] - start[0])*count[1]*count[2] +
	    (location[1]- start[1]) * count[2] + (location[2]- start[2])));
    printf("Test from voxel scaled: %f\n", value);
    miget_voxel_value(vol, location, 3, &value);
    printf("Test voxel value itself: %f\n", value);

    /* close volume*/
    miclose_volume(vol);

    if (error_cnt != 0) {
        fprintf(stderr, "%d error%s reported\n", 
                error_cnt, (error_cnt == 1) ? "" : "s");
    }
    else {
        fprintf(stderr, "\n No errors\n");
    }
    
    return (error_cnt);
}

