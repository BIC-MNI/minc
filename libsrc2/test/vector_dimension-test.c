#include <stdio.h>
#include "minc2.h"

#define TESTRPT(msg, val) (error_cnt++, fprintf(stderr, \
     "Error reported on line #%d, %s: %d\n", \
     __LINE__, msg, val))

static int error_cnt = 0;

#define CZ 41
#define CY 410
#define CX 530
#define NDIMS 4

int main(int argc, char **argv)
{
    mihandle_t vol;
    int r = 0;
    midimhandle_t dim[NDIMS];
    unsigned long lengths[NDIMS];
    midimhandle_t copy_dim[NDIMS];
    int n;
    unsigned long coords[NDIMS];
    unsigned long count[NDIMS];
    int i,j,k;
    double offset;
    unsigned int voxel;
    unsigned char * Atmp;
    midimclass_t dimension_class;
    int ndims;
    Atmp = ( unsigned char *) malloc(CX * CY * CZ * sizeof(unsigned char));

    printf(" \n");
    printf("This test uses the vector_dimension file in test directory\n");
    printf(" \n");

    r = miopen_volume("example_vector2.mnc", MI2_OPEN_READ, &vol);
    
    if (r < 0) {
	TESTRPT("failed to open vector_dimension volume", r);
    }

    r = miget_volume_dimension_count(vol, MI_DIMCLASS_ANY, MI_DIMATTR_REGULARLY_SAMPLED, &ndims);
    if (r < 0) {
	TESTRPT("failed to get number of dimensions", r);
    
    }
   
    printf("Total number of dimensions : %d \n", ndims);

    r = miget_volume_dimensions(vol, MI_DIMCLASS_ANY, MI_DIMATTR_REGULARLY_SAMPLED, MI_DIMORDER_FILE, NDIMS, dim);

    if (r < 0) {
      TESTRPT("Could not get dimension handles from volume", r);
	
    }
    
    r = miget_dimension_sizes(dim, NDIMS, lengths);
    if (r < 0) {
      TESTRPT(" more trouble", r);
	  
    }
    printf( "Dimension Size in file order : ");
    for(i=0; i < NDIMS; i++) {
      printf( " %d ", lengths[i]);
    }
    printf(" \n");
    
    for( i=0; i < NDIMS; i++) {
	r = miget_dimension_class(dim[i],&dimension_class);
	if (r < 0) {
	  TESTRPT("failed to get dimension class", r);
	}
	if (dimension_class == MI_DIMCLASS_RECORD) {
	  printf("Dim class RECORD present check dim name for *vector_dimension*\n");
	}
    }
    
    printf("Let's get the first 10 data values of each vector component (file order) \n");
    
    coords[0]=coords[1]=coords[2]=0;
    
    count[0]=CZ;
    count[1]=CY;
    count[2]=CX;
    count[3]=1;
    printf(" FILE ORDER --> zspace, yspace, xspace, vector_dimension \n");
    for (i=0; i < 3; i++) {
      printf("Vector Componenet %d \n", i+1);
      coords[3]=i;
      r = miget_voxel_value_hyperslab(vol, MI_TYPE_UBYTE, coords, count,Atmp);
      if (r < 0) {
      TESTRPT("Failed to operate hyperslab function", r);
      }
     
      for (j=0; j < 10; j++) {
	printf( " %u ", Atmp[j]);
      }
      printf(" \n");
    }
    printf("APPARENT ORDER --> vector_dimension, zspace, yspace, xspace\n");
    
    // Set the apparent dimension order 
    
    copy_dim[0] = dim[3];
    copy_dim[1] = dim[0];
    copy_dim[2] = dim[1];
    copy_dim[3] = dim[2];

    r = miset_apparent_dimension_order(vol, NDIMS, copy_dim);
    if (r < 0) {
      TESTRPT("failed to set apparent order", r);
    }
    
    coords[1]=coords[2]=coords[3]=0;
    
    count[0]=1; //must always be one
    count[1]=CZ;
    count[2]=CY;
    count[3]=CZ;
    
    printf("APPARENT ORDER SET \n");
    for (i=0; i < 3; i++) {
      printf("Vector Componenet %d \n", i+1);
      coords[0]=i;
      r = miget_voxel_value_hyperslab(vol, MI_TYPE_UBYTE, coords, count,Atmp);
      if (r < 0) {
      TESTRPT("Failed to operate hyperslab function", r);
      }
     
      for (j=0; j < 10; j++) {
	printf( " %u ", Atmp[j]);
      }
      printf(" \n");
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

