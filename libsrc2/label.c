/**
 * \file label.c
 * \brief MINC 2.0 Label functions
 * \author Bert Vincent
 *
 * This small set of three functions are intended to allow for the
 * definition of labeled, or enumerated, volumes.
 * 
 * Labeled volumes must have been created with the class MI_CLASS_LABEL,
 * and with any integer subtype.
 *
 ************************************************************************/

#include <stdlib.h>
#include <hdf5.h>
#include "minc2.h"
#include "minc2_private.h"

#define MI_LABEL_MAX 128

/**
This function associates a label name with an integer value for the given
volume. Functions which read and write voxel values will read/write 
in integer values, and must call miget_label_name() to discover the 
descriptive text string which corresponds to the integer value.
*/
int 
midefine_label(mihandle_t volume, int value, const char *name)
{
    int result;

    if (volume == NULL || name == NULL) {
        return (MI_ERROR);
    }

    if (strlen(name) > MI_LABEL_MAX) {
        return (MI_ERROR);
    }

    if (volume->volume_class != MI_CLASS_LABEL) {
	return (MI_ERROR);
    }

    if (volume->type_id <= 0) {
	return (MI_ERROR);
    }

    result = H5Tenum_insert(volume->type_id, name, &value);
    if (result < 0) {
	return (MI_ERROR);
    }

    return (MI_NOERROR);
}

/**
For a labelled volume, this function retrieves the text name
associated with a given integer value.

The name pointer returned must be freed by calling mifree_name().
*/
int
miget_label_name(mihandle_t volume, int value, char **name)
{
    int result;

    if (volume == NULL || name == NULL) {
        return (MI_ERROR);
    }

    if (volume->volume_class != MI_CLASS_LABEL) {
        return (MI_ERROR);
    }
    if (volume->type_id <= 0) {
        return (MI_ERROR);
    }
    *name = malloc(MI_LABEL_MAX);
    if (*name == NULL) {
        return (MI_ERROR);
    }

    H5E_BEGIN_TRY {
        result = H5Tenum_nameof(volume->type_id, &value, *name, MI_LABEL_MAX);
    } H5E_END_TRY;

    if (result < 0) {
	return (MI_ERROR);
    }
    return (MI_NOERROR);
}

/**
This function is the inverse of miget_label_name(). It is called to determine
what integer value, if any, corresponds to the given text string.
*/
int
miget_label_value(mihandle_t volume, const char *name, int *value_ptr)
{
    int result;

    if (volume == NULL || name == NULL || value_ptr == NULL) {
        return (MI_ERROR);
    }

    if (volume->volume_class != MI_CLASS_LABEL) {
        return (MI_ERROR);
    }

    if (volume->type_id <= 0) {
        return (MI_ERROR);
    }

    H5E_BEGIN_TRY {
        result = H5Tenum_valueof(volume->type_id, name, value_ptr);
    } H5E_END_TRY;

    if (result < 0) {
	return (MI_ERROR);
    }
    return (MI_NOERROR);
}

#ifdef M2_TEST
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

    /* Now test some stuff... */

    midefine_label(hvol, 0, "Black");
    midefine_label(hvol, 0xffffff, "White");
    midefine_label(hvol, 0x808080, "Grey");
    midefine_label(hvol, 0xff0000, "Red");
    midefine_label(hvol, 0x00ff00, "Blue");
    midefine_label(hvol, 0x0000ff, "Green");

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

    coords[0] = 0;
    coords[1] = 0;
    coords[2] = 0;
    miset_voxel_value(hvol, coords, 3, 0xffffff);
    coords[2] = 2;
    miset_voxel_value(hvol, coords, 3, 0x00ff00);

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

#endif
