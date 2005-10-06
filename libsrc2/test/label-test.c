#include <stdio.h>
#include <string.h>
#include "minc2.h"

#define TESTRPT(msg, val) (error_cnt++, fprintf(stderr, \
                                  "Error reported on line #%d, %s: %d\n", \
                                  __LINE__, msg, val))

static int error_cnt = 0;
#define CX 3
#define CY 4
#define CZ 2
#define NDIMS 3


int create_label_image(void)
{

  mihandle_t hvol;
  char *name;
  int result;
  int value;
  midimhandle_t hdim[3];
  unsigned long coords[3];
  int i,j,k;

  result = micreate_dimension("xspace", MI_DIMCLASS_SPATIAL, 
			      MI_DIMATTR_REGULARLY_SAMPLED, 10, &hdim[0]);

  result = micreate_dimension("yspace", MI_DIMCLASS_SPATIAL, 
			      MI_DIMATTR_REGULARLY_SAMPLED, 10, &hdim[1]);

  result = micreate_dimension("zspace", MI_DIMCLASS_SPATIAL, 
			      MI_DIMATTR_REGULARLY_SAMPLED, 6, &hdim[2]);

  result = micreate_volume("tst-label.mnc", 3, hdim, MI_TYPE_UINT, 
			   MI_CLASS_LABEL, NULL, &hvol);
  if (result < 0) {
    fprintf(stderr, "Unable to create test file %x\n", result);
    return (-1);
  }

  result = miget_number_of_defined_labels(hvol,&value);
  if (result != MI_NOERROR) {
    TESTRPT("Invalid return from miget_label_name", result);
  }
  else {
    printf(" %d \n", value);
  }
  /* Now test some stuff... */

  midefine_label(hvol, 0, "Black");
  midefine_label(hvol, 0xffffff, "White");
  midefine_label(hvol, 0x808080, "Grey");
  midefine_label(hvol, 0xff0000, "Red");
  midefine_label(hvol, 0x00ff00, "Blue");
  midefine_label(hvol, 0x0000ff, "Green");
  result = miget_number_of_defined_labels(hvol,&value);
  if (result != MI_NOERROR) {
    TESTRPT("Invalid return from miget_label_name", result);
  }
  else {
    printf(" %d \n", value);
  }
  result = miget_label_name(hvol, 0, &name);
  if (result != MI_NOERROR) {
    TESTRPT("Invalid return from miget_label_name", result);
  }

  if (strcmp(name, "Black") != 0) {
    TESTRPT("Unexpected label for value 0", 0);
  }
  mifree_name(name);

  result = miget_label_name(hvol, 0x00ff00, &name);
  if (result != MI_NOERROR) {
    TESTRPT("Invalid return from miget_label_name", result);
  }

  if (strcmp(name, "Blue") != 0) {
    TESTRPT("Unexpected label for value 0", 0);
  }
  mifree_name(name);

  result = miget_label_name(hvol, 1, &name);
  if (result != MI_ERROR) {
    TESTRPT("Invalid return from miget_label_name", result);
  }

  result = miget_label_value(hvol, "White", &value);
  if (result != MI_NOERROR) {
    TESTRPT("Invalid return from miget_label_value", result);
  }
  
  if (value != 0xffffff) {
    TESTRPT("Unexpected value for label 'White'", 0);
  }

  result = miget_label_value(hvol, "Mauve", &value);
  if (result != MI_ERROR) {
    TESTRPT("Invalid return from miget_label_value", result);
  }

  micreate_volume_image(hvol);

  int counter=0;
  for (i=0; i < CX; i++) {
    for (j=0; j < CY ; j++) {
      for (k=0; k < CZ ; k++) {
	coords[0] = i;
	coords[1] = j;
	coords[2] = k;
	if (counter == 1) {
	  miset_voxel_value(hvol,coords, 3, 0xffffff);
	  
	}
	else if ( counter ==2) {
	  miset_voxel_value(hvol,coords, 3, 0x808080);
	  
	}
	else if ( counter ==3) {
	  miset_voxel_value(hvol,coords, 3, 0xff0000);
	    
	}
	else if ( counter ==4) {
	  miset_voxel_value(hvol,coords, 3, 0x00ff00);
	  
	}
	else if (counter ==5) {
	  miset_voxel_value(hvol,coords, 3,0x0000ff );
	}
	else {
	  miset_voxel_value(hvol,coords, 3, 0);
	}
	counter++;
	if (counter >=6)
	  {
	    counter =0;
	  }
      }
    }
  }
  miclose_volume(hvol);

  return 0;
}

int
main(int argc, char **argv)
{
  int i,j,k; 
  mihandle_t vol;
  midimhandle_t hdims[NDIMS], cp_hdims[NDIMS];
  unsigned long coords[NDIMS];
  unsigned long count[NDIMS];
  int value;
  int *buf = (int *)malloc(CX * CY * CZ * sizeof(int));
  printf("Creating label image !! \n");
  error_cnt += create_label_image();
  
  error_cnt += miopen_volume("tst-label.mnc", MI2_OPEN_READ, &vol);
  miget_number_of_defined_labels(vol,&value);
    
      printf("Number of defined labels %d \n", value);
  miget_label_value(vol, "White", &value);

  coords[0] = coords[1] = coords[2] = 0;
  count[0] = CX; count[1] = CY; count[2] = CZ;
  error_cnt += miget_voxel_value_hyperslab(vol,MI_TYPE_INT, coords, count,buf); 
  int counter = 0;
  int id;
  printf("Print label file with file order x,y,z \n");
  for (i=0; i < CX; i++) {
    for (j=0; j < CY; j++) {
      for (k=0; k < CZ; k++) {
	id = i * CY * CZ + j * CZ + k;
	printf("%8x ", buf[id]);
	counter++;
	if (counter == 6)
	  {
	    counter = 0;
	    printf(" \n");
	  }
      }
    }
  }
 
 
  miget_volume_dimensions(vol, MI_DIMCLASS_ANY, MI_DIMATTR_REGULARLY_SAMPLED,
			  MI_DIMORDER_FILE, NDIMS, hdims);
 
  cp_hdims[0] = hdims[2];
  cp_hdims[1] = hdims[1];
  cp_hdims[2] = hdims[0];
  
  miset_apparent_dimension_order(vol, NDIMS, cp_hdims);
  coords[0] = coords[1] = coords[2] = 0;
  count[0] = CZ; count[1] = CY; count[2] = CX;
  error_cnt += miget_voxel_value_hyperslab(vol,MI_TYPE_UINT, coords, count,buf); 
  printf("Print label file with apparent order z,y,x \n");
  counter = 0;
  for (i=0; i < CZ; i++) {
    for (j=0; j < CY; j++) {
      for (k=0; k < CX; k++) {
	id = i * CY * CX + j * CX + k;
	printf("%8x ", buf[id]);
	counter++;
	if (counter == 6)
	  {
	    counter = 0;
	    printf(" \n");
	  }
      }
    }
  }

  error_cnt += miclose_volume(vol);
    if (error_cnt != 0) {
	fprintf(stderr, "%d error%s reported\n", 
		error_cnt, (error_cnt == 1) ? "" : "s");
    }
    else {
	fprintf(stderr, "No errors\n");
    }
    return (error_cnt);
}

