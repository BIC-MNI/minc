#include <stdio.h>
#include "minc2.h"

#define TESTRPT(msg, val) (error_cnt++, fprintf(stderr, \
     "Error reported on line #%d, %s: %d\n", \
     __LINE__, msg, val))

static int error_cnt = 0;

#define CZ 41
#define CY 410
#define CX 530
#define CT 8
#define NDIMS 4

int main(int argc, char **argv)
{
    mihandle_t vol;
    int r;
    midimhandle_t dim[NDIMS];
    midimhandle_t copy_dim[NDIMS];
    int n;
    unsigned long coords[NDIMS];
    unsigned long count[NDIMS];
    int i,j,k;
    double offset;
    unsigned int voxel;
    unsigned char * Atmp;
    
    midimclass_t dimension_class;
    r =0;
    Atmp = ( unsigned char *) malloc(CX * CY * CZ * sizeof(unsigned char));
    r = miopen_volume(*++argv, MI2_OPEN_READ, &vol);
    if (r < 0) {
	TESTRPT("failed to open volume", r);
    }
    
    r = miget_volume_dimensions(vol, MI_DIMCLASS_ANY, MI_DIMATTR_ALL,MI_DIMORDER_FILE , 4, dim);
    if (r < 0) {
      TESTRPT("failed to get dimensions", r);
    }
    r = miget_dimension_class(dim[0],&dimension_class);
    if (r < 0) {
      TESTRPT("failed to get dimensions", r);
    }
    printf(" class is %d \n", dimension_class);
    r = miget_dimension_class(dim[1],&dimension_class);
    if (r < 0) {
      TESTRPT("failed to get dimensions", r);
    }
    printf(" class is %d \n", dimension_class);
    r = miget_dimension_class(dim[2],&dimension_class);
    if (r < 0) {
      TESTRPT("failed to get dimensions", r);
    }
    printf(" class is %d \n", dimension_class);
    r = miget_dimension_class(dim[3],&dimension_class);
    if (r < 0) {
      TESTRPT("failed to get dimensions", r);
    }
    printf(" class is %d \n", dimension_class);

    /*
    //coords[0]=coords[1]=coords[2]=coords[3]=0;
    coords[0]=coords[1]=coords[2]=0;
    coords[3]=2;
    count[0]=41;
    count[1]=410;
    count[2]=530;
    count[3]=1;
    r = miget_voxel_value_hyperslab(vol, MI_TYPE_UBYTE, coords, count,Atmp);
    for (j=0; j < 20; j++)
      {
	printf( " This is my data %u \n", Atmp[j]);
      }
    */
    
    miflipping_t flip;
    miflipping_t sign;

    r = miget_dimension_apparent_voxel_order(dim[3],&flip, &sign);
     if (r < 0) {
      TESTRPT("failed to get voxel order", r);
    }
    
     printf(" voxel order is %d %d \n", flip, sign);
    // Set the apparent dimension order 
    
    copy_dim[0] = dim[3];
    copy_dim[1] = dim[0];
    copy_dim[2] = dim[1];
    copy_dim[3] = dim[2];

    r = miset_apparent_dimension_order(vol, 4, copy_dim);
    if (r < 0) {
      TESTRPT("failed to set apparent order", r);
    }
    
    coords[0]=2;  //vector-dimension
    coords[1]=coords[2]=coords[3]=0;
    
    
    count[0]=1; //must always be one
    count[1]=41;
    count[2]=410;
    count[3]=530;

    r = miget_voxel_value_hyperslab(vol, MI_TYPE_UBYTE, coords, count,Atmp);
    for (j=0; j < 20; j++)
      {
	printf( " This is my data %u \n", Atmp[j]);
      }
   
    if (r < 0) {
      TESTRPT("failed to do hyperslb", r);
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

