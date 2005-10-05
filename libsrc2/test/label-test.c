#include <stdio.h>
#include <string.h>
#include "minc2.h"

#define TESTRPT(msg, val) (error_cnt++, fprintf(stderr, \
                                  "Error reported on line #%d, %s: %d\n", \
                                  __LINE__, msg, val))

static int error_cnt = 0;

int
main(int argc, char **argv)
{
    mihandle_t hvol;
    char *name;
    int result;
    int value;
    midimhandle_t hdim[3];
    unsigned long coords[3];

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

    for (i=0; i < 10; i++) {
      for (j=0; j < 10 ; j++) {
	for (k=0; k < 6 ; k++) {
	  coords[0] = i;
	  coords[1] = j;
	  coords[2] = k;
	  if ((i*j*k) < 150) {
	    miset_voxel_value(hvol,coords, 3, 0);
	  }
	  else if ((i*j*k) < 250) {
	    miset_voxel_value(hvol,coords, 3, 0xffffff);
	  }
	  else if ((i*j*k) < 350) {
	    miset_voxel_value(hvol,coords, 3, 0x808080);
	  }
	  else if ((i*j*k) < 450) {
	    miset_voxel_value(hvol,coords, 3, 0xff0000);
	  }
	  else if ((i*j*k) < 550) {
	    miset_voxel_value(hvol,coords, 3, 0x00ff00);
	  }
	  else {
	    miset_voxel_value(hvol,coords, 3,0x0000ff );
	  }
	}
      }
    }
    miclose_volume(hvol);

    if (error_cnt != 0) {
	fprintf(stderr, "%d error%s reported\n", 
		error_cnt, (error_cnt == 1) ? "" : "s");
    }
    else {
	fprintf(stderr, "No errors\n");
    }
    return (error_cnt);
}

