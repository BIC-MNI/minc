/************************************************************************
 * MINC 2 DATA TYPE/SPACE FUNCTIONS
 ************************************************************************/

#include <stdlib.h>
#include <hdf5.h>
#include "minc2.h"
#include "minc2_private.h"

#define MI_LABEL_MAX 128

int 
midefine_label(mihandle_t volume, int value, const char *name)
{
    hid_t file_id;
    hid_t grp_id;
    hid_t type_id;
    int result;

    file_id = volume->hdf_id;
    if (file_id < 0) {
	return (MI_ERROR);
    }

    grp_id = midescend_path(file_id, "/minc-2.0");
    if (grp_id < 0) {
	return (MI_ERROR);
    }

    type_id = H5Topen(grp_id, "Labels");
    if (type_id < 0) {
	type_id = H5Tenum_create(H5T_NATIVE_INT);
    }
    else {
	hid_t new_type_id = H5Tcopy(type_id);
	H5Tclose(type_id);
	H5Gunlink(grp_id, "Labels");
	type_id = new_type_id;
    }

    if (type_id < 0) {
	return (MI_ERROR);
    }

    result = H5Tenum_insert(type_id, name, &value);
    if (result < 0) {
	return (MI_ERROR);
    }

    result = H5Tcommit(grp_id, "Labels", type_id);
    if (result < 0) {
	return (MI_ERROR);
    }

    H5Tclose(type_id);
    H5Gclose(grp_id);

    return (MI_NOERROR);
}

int
miget_label_name(mihandle_t volume, int value, char **name)
{
    hid_t file_id;
    hid_t grp_id;
    hid_t type_id;
    int result;

    file_id = volume->hdf_id;
    if (file_id < 0) {
	return (MI_ERROR);
    }

    grp_id = midescend_path(file_id, "/minc-2.0");
    if (grp_id < 0) {
	return (MI_ERROR);
    }

    type_id = H5Topen(grp_id, "Labels");
    if (type_id < 0) {
	return (MI_ERROR);
    }

    *name = malloc(MI_LABEL_MAX);
    result = H5Tenum_nameof(type_id, &value, *name, MI_LABEL_MAX);
    if (result < 0) {
	return (MI_ERROR);
    }

    H5Tclose(type_id);
    H5Gclose(grp_id);
    return (MI_NOERROR);
}

int
miget_label_value(mihandle_t volume, const char *name, int *value_ptr)
{
    hid_t file_id;
    hid_t grp_id;
    hid_t type_id;
    int result;

    file_id = volume->hdf_id;
    if (file_id < 0) {
	return (MI_ERROR);
    }

    grp_id = midescend_path(file_id, "/minc-2.0");
    if (grp_id < 0) {
	return (MI_ERROR);
    }

    type_id = H5Topen(grp_id, "Labels");
    if (type_id < 0) {
	return (MI_ERROR);
    }

    result = H5Tenum_valueof(type_id, name, value_ptr);
    if (result < 0) {
	return (MI_ERROR);
    }

    H5Tclose(type_id);
    H5Gclose(grp_id);
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
    
    /* Turn off automatic error reporting - we'll take care of this
     * ourselves, thanks!
     */
    H5Eset_auto(NULL, NULL);
    
    result = micreate_volume("test.h5", 0, NULL, 0, 0, NULL, &hvol);
    if (result < 0) {
	fprintf(stderr, "Unable to create test file %x", result);
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
