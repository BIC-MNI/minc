/************************************************************************
 * MINC 2.0 "GROUP/ATTRIBUTE" FUNCTIONS
 ************************************************************************/
#include <stdlib.h>
#include <hdf5.h>
#include "minc2.h"
#include "minc2_private.h"

/*! Create a group at "path" using "name".
 */
int
micreate_group(mihandle_t vol, const char *path, const char *name)
{
    hid_t hdf_file;
    hid_t hdf_grp;
    hid_t hdf_new_grp;

    /* Get a handle to the actual HDF file 
     */
    hdf_file = miget_volume_file_handle(vol);
    if (hdf_file < 0) {
	return (MI_ERROR);
    }

    /* Search through the path, descending into each group encountered.
     */
    hdf_grp = midescend_path(hdf_file, path);
    if (hdf_grp < 0) {
	return (MI_ERROR);
    }

    /* Actually create the requested group.
     */
    hdf_new_grp = H5Gcreate(hdf_grp, name, 0);
    if (hdf_new_grp < 0) {
	return (MI_ERROR);
    }

    /* Close the handles we created.
     */
    H5Gclose(hdf_new_grp);
    H5Gclose(hdf_grp);
  
    return (MI_NOERROR);
}

/*! Delete the named attribute.
 */
int
midelete_attr(mihandle_t vol, const char *path, const char *name)
{
    hid_t hdf_file;
    hid_t hdf_grp;
    herr_t hdf_result;

    /* Get a handle to the actual HDF file 
     */
    hdf_file = miget_volume_file_handle(vol);
    if (hdf_file < 0) {
	return (MI_ERROR);
    }

    /* Search through the path, descending into each group encountered.
     */
    hdf_grp = midescend_path(hdf_file, path);
    if (hdf_grp < 0) {
	return (MI_ERROR);
    }

    /* Delete the attribute from the path.
     */
    hdf_result = H5Adelete(hdf_grp, name);
    if (hdf_result < 0) {
	return (MI_ERROR);
    }

    /* Close the handles we created.
     */
    H5Gclose(hdf_grp);

    return (MI_NOERROR);
}

int
midelete_group(mihandle_t vol, const char *path, const char *name)
{
    hid_t hdf_file;
    hid_t hdf_grp;
    herr_t hdf_result;

    /* Get a handle to the actual HDF file 
     */
    hdf_file = miget_volume_file_handle(vol);
    if (hdf_file < 0) {
	return (MI_ERROR);
    }

    /* Search through the path, descending into each group encountered.
     */
    hdf_grp = midescend_path(hdf_file, path);
    if (hdf_grp < 0) {
	return (MI_ERROR);
    }
 
    /* Delete the group (or any object, really) from the path.
     */
    hdf_result = H5Gunlink(hdf_grp, name);
    if (hdf_result < 0) {
	hdf_result = MI_ERROR;
    }
    else {
	hdf_result = MI_NOERROR;
    }

    /* Close the handles we created.
     */
    H5Gclose(hdf_grp);
  
    return (hdf_result);
}

int
miget_attr_length(mihandle_t vol, const char *path, const char *name,
		  int *length)
{
    hid_t hdf_file;
    hid_t hdf_grp;
    hid_t hdf_attr;
    hsize_t hdf_dims[1];   /* TODO: symbolic constant for "1" here? */
    hid_t hdf_space;
    hid_t hdf_type;

    /* Get a handle to the actual HDF file 
     */
    hdf_file = miget_volume_file_handle(vol);
    if (hdf_file < 0) {
	return (MI_ERROR);
    }

    /* Search through the path, descending into each group encountered.
     */
    hdf_grp = midescend_path(hdf_file, path);
    if (hdf_grp < 0) {
	return (MI_ERROR);
    }

    hdf_attr = H5Aopen_name(hdf_grp, name);
    if (hdf_attr < 0) {
	return (MI_ERROR);
    }

    hdf_space = H5Aget_space(hdf_attr);
    if (hdf_space < 0) {
	return (MI_ERROR);
    }

    hdf_type = H5Aget_type(hdf_attr);
    if (hdf_type < 0) {
	return (MI_ERROR);
    }

    switch (H5Sget_simple_extent_ndims(hdf_space)) {
    case 0:			/* Scalar */
	/* String types need to return the length of the string.
	 */
	if (H5Tget_class(hdf_type) == H5T_STRING) {
	    *length = H5Tget_size(hdf_type);
	}
	else {
	    *length = 1;
	}
	break;

    case 1:
	H5Sget_simple_extent_dims(hdf_space, hdf_dims, NULL);
	*length = hdf_dims[0];
	break;

    default:
	/* For now, we allow only scalars and vectors.  No multidimensional
	 * arrays for MINC 2.0 attributes.
	 */
	return (MI_ERROR);
    }

    H5Tclose(hdf_type);
    H5Sclose(hdf_space);
    H5Aclose(hdf_attr);
    H5Gclose(hdf_grp);

    return (MI_NOERROR);
}

int
miget_attr_type(mihandle_t vol, const char *path, const char *name,
		mitype_t *data_type)
{
    hid_t hdf_file;
    hid_t hdf_grp;
    hid_t hdf_attr;
    hid_t hdf_type;

    /* Get a handle to the actual HDF file 
     */
    hdf_file = miget_volume_file_handle(vol);
    if (hdf_file < 0) {
	return (MI_ERROR);
    }

    /* Search through the path, descending into each group encountered.
     */
    hdf_grp = midescend_path(hdf_file, path);
    if (hdf_grp < 0) {
	return (MI_ERROR);
    }

    hdf_attr = H5Aopen_name(hdf_grp, name);
    if (hdf_attr < 0) {
	return (MI_ERROR);
    }

    hdf_type = H5Aget_type(hdf_attr);
    if (hdf_type == H5T_NATIVE_DOUBLE) {
	*data_type = MI_TYPE_DOUBLE;
    }
    else if (H5Tget_class(hdf_type) == H5T_STRING) {
	*data_type = MI_TYPE_STRING;
    }
    else {
	return (MI_ERROR);
    }

    H5Tclose(hdf_type);
    H5Aclose(hdf_attr);
    H5Gclose(hdf_grp);

    return (MI_NOERROR);
}

int
miget_attr_values(mihandle_t vol, mitype_t data_type, const char *path, 
		  const char *name, int length, void *values)
{
    hid_t hdf_file;
    hid_t hdf_grp;
    hid_t hdf_type;
    hid_t hdf_space;
    hid_t hdf_attr;

    /* Get a handle to the actual HDF file 
     */
    hdf_file = miget_volume_file_handle(vol);
    if (hdf_file < 0) {
	return (MI_ERROR);
    }

    /* Search through the path, descending into each group encountered.
     */
    hdf_grp = midescend_path(hdf_file, path);
    if (hdf_grp < 0) {
	return (MI_ERROR);
    }

    hdf_attr = H5Aopen_name(hdf_grp, name);
    if (hdf_attr < 0) {
	return (MI_ERROR);
    }

    switch (data_type) {
    case MI_TYPE_INT:
	hdf_type = H5Tcopy(H5T_NATIVE_INT);
	break;
    case MI_TYPE_FLOAT:
	hdf_type = H5Tcopy(H5T_NATIVE_FLOAT);
	break;
    case MI_TYPE_DOUBLE:
	hdf_type = H5Tcopy(H5T_NATIVE_DOUBLE);
	break;
    case MI_TYPE_STRING:
	hdf_type = H5Tcopy(H5T_C_S1);
	H5Tset_size(hdf_type, length);
	break;
    default:
	return (MI_ERROR);
    }

    hdf_space = H5Aget_space(hdf_attr);
    if (hdf_space < 0) {
	return (MI_ERROR);
    }

    /* If we're retrieving a vector, make certain the length passed into this
     * function is sufficient.
     */
    if (H5Sget_simple_extent_ndims(hdf_space) == 1) {
	hsize_t hdf_dims[1];

	H5Sget_simple_extent_dims(hdf_space, hdf_dims, NULL);
	if (length < hdf_dims[0]) {
	    return (MI_ERROR);
	}
    }
    

    H5Aread(hdf_attr, hdf_type, values);

    H5Aclose(hdf_attr);
    H5Tclose(hdf_type);
    H5Sclose(hdf_space);
    H5Gclose(hdf_grp);

    return (MI_NOERROR);
}

int
miset_attr_values(mihandle_t vol, mitype_t data_type, const char *path,
		  const char *name, int length, const void *values)
{
    hid_t hdf_file;
    hid_t hdf_grp;
    hid_t hdf_type;
    hid_t hdf_space;
    hid_t hdf_attr;

    /* Get a handle to the actual HDF file 
     */
    hdf_file = miget_volume_file_handle(vol);
    if (hdf_file < 0) {
	return (MI_ERROR);
    }

    /* Search through the path, descending into each group encountered.
     */
    hdf_grp = midescend_path(hdf_file, path);
    if (hdf_grp < 0) {
	return (MI_ERROR);
    }

    switch (data_type) {
    case MI_TYPE_DOUBLE:
	hdf_type = H5Tcopy(H5T_NATIVE_DOUBLE);
	break;

    case MI_TYPE_STRING:
	hdf_type = H5Tcopy(H5T_C_S1);
	H5Tset_size(hdf_type, length);
	break;

    default:
	return (MI_ERROR);
    }

    if (length == 1 || data_type == MI_TYPE_STRING) {
	hdf_space = H5Screate(H5S_SCALAR);
    }
    else {
	hsize_t dims[1];
	hsize_t maxdims[1];

	dims[0] = length;
	maxdims[0] = length;

	hdf_space = H5Screate_simple(1, dims, maxdims);
    }

    /* Delete attribute if it already exists. */
    H5Adelete(hdf_grp, name);

    hdf_attr = H5Acreate(hdf_grp, name, hdf_type, hdf_space, H5P_DEFAULT);
    if (hdf_attr < 0) {
	return (MI_ERROR);
    }

    H5Awrite(hdf_attr, hdf_type, values);

    H5Aclose(hdf_attr);
    H5Sclose(hdf_space);
    H5Tclose(hdf_type);
    H5Gclose(hdf_grp);

    return (MI_NOERROR);
}

		  
#ifdef M2_TEST
#define TESTRPT(msg, val) (error_cnt++, fprintf(stderr, \
                                  "Error reported on line #%d, %s: %d\n", \
                                  __LINE__, msg, val))

#define TESTARRAYSIZE 11

static int error_cnt = 0;

int main(int argc, char **argv)
{
    mihandle_t hvol;
    hid_t file_id;
    hid_t g1_id, g2_id, g3_id, g4_id;
    int r;
    mitype_t data_type;
    int length;
    static double tstarr[TESTARRAYSIZE] = { 
	1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9, 10.10, 11.11
    };
    double dblarr[TESTARRAYSIZE];
    float fltarr[TESTARRAYSIZE];
    int intarr[TESTARRAYSIZE];
    char valstr[128];

    /* Turn off automatic error reporting - we'll take care of this
     * ourselves, thanks!
     */
    // H5Eset_auto(NULL, NULL);

    r = micreate_volume("test.h5", 0, NULL, 0, 0, NULL, &hvol);
    if (r < 0) {
	TESTRPT("Unable to create test file", r);
	return (-1);
    }

    file_id = miget_volume_file_handle(hvol);

    g1_id = H5Gopen(file_id, "minc-2.0");

    g2_id = H5Gcreate(g1_id, "test1", 0);
    g3_id = H5Gcreate(g1_id, "test2", 0);

    g4_id = H5Gcreate(g2_id, "stuff", 0);

    /* Close the things we created. */
    H5Gclose(g4_id);
    H5Gclose(g3_id);
    H5Gclose(g2_id);
    H5Gclose(g1_id);

    r = micreate_group(hvol, "/minc-2.0/test1/stuff", "hello");
    if (r < 0) {
	TESTRPT("micreate_group failed", r);
    }

    r = miset_attr_values(hvol, MI_TYPE_STRING, "/minc-2.0/test1/stuff/hello", 
			  "animal", 8, "fruitbat");
    if (r < 0) {
	TESTRPT("miset_attr_values failed", r);
    }

    r = miset_attr_values(hvol, MI_TYPE_STRING, "/minc-2.0/test1/stuff", 
			  "objtype", 10, "automobile");
    if (r < 0) {
	TESTRPT("miset_attr_values failed", r);
    }

    r = miset_attr_values(hvol, MI_TYPE_DOUBLE, "/minc-2.0/test2", 
			  "maxvals", TESTARRAYSIZE, tstarr);
    if (r < 0) {
	TESTRPT("miset_attr_values failed", r);
    }

    r = miget_attr_type(hvol, "/minc-2.0/test1/stuff/hello", "animal", 
			&data_type);
    if (r < 0) {
	TESTRPT("miget_attr_type failed", r);
    }

    r = miget_attr_length(hvol, "/minc-2.0/test1/stuff/hello", "animal", 
			  &length);
    if (r < 0) {
	TESTRPT("miget_attr_length failed", r);
    }

    if (data_type != MI_TYPE_STRING) {
	TESTRPT("miget_attr_type failed", data_type);
    }
    if (length != 8) {
	TESTRPT("miget_attr_length failed", length);
    }

    r = midelete_group(hvol, "/minc-2.0/test1/stuff", "goodbye");
    if (r >= 0) {
	TESTRPT("midelete_group failed", r);
    }

    r = midelete_group(hvol, "/minc-2.0/test1/stuff", "hello");
    /* This should succeed.
     */
    if (r < 0) {
	TESTRPT("midelete_group failed", r);
    }

    r = miget_attr_length(hvol, "/minc-2.0/test1/stuff/hello", "animal", 
			  &length);
    /* This should fail since we deleted the group.
     */
    if (r >= 0) {
	TESTRPT("miget_attr_length failed", r);
    }

    r = miget_attr_values(hvol, MI_TYPE_DOUBLE, "/minc-2.0/test2", "maxvals", 
			  TESTARRAYSIZE, dblarr);
    if (r < 0) {
	TESTRPT("miget_attr_values failed", r);
    }

    for (r = 0; r < TESTARRAYSIZE; r++) {
	if (dblarr[r] != tstarr[r]) {
	    TESTRPT("miget_attr_values mismatch", r);
	}
    }

    /* Get the values again in float rather than double format.
     */
    r = miget_attr_values(hvol, MI_TYPE_FLOAT, "/minc-2.0/test2", "maxvals", 
			  TESTARRAYSIZE, fltarr);
    if (r < 0) {
	TESTRPT("miget_attr_values failed", r);
    }

    for (r = 0; r < TESTARRAYSIZE; r++) {
	if (fltarr[r] != (float) tstarr[r]) {
	    TESTRPT("miget_attr_values mismatch", r);
	    fprintf(stderr, "fltarr[%d] = %f, tstarr[%d] = %f\n",
		    r, fltarr[r], r, tstarr[r]);
	}
    }

    /* Get the values again in int rather than double format.
     */
    r = miget_attr_values(hvol, MI_TYPE_INT, "/minc-2.0/test2", "maxvals", 
			  TESTARRAYSIZE, intarr);
    if (r < 0) {
	TESTRPT("miget_attr_values failed", r);
    }

    for (r = 0; r < TESTARRAYSIZE; r++) {
	if (intarr[r] != (int) tstarr[r]) {
	    TESTRPT("miget_attr_values mismatch", r);
	    fprintf(stderr, "intarr[%d] = %d, tstarr[%d] = %f\n",
		    r, intarr[r], r, tstarr[r]);
	}
    }

    r = miget_attr_values(hvol, MI_TYPE_STRING, "/minc-2.0/test1/stuff", 
			  "objtype", 128, valstr);
    if (r < 0) {
	TESTRPT("miget_attr_values failed", r);
    }

    if (strcmp(valstr, "automobile") != 0) {
	TESTRPT("miget_attr_values failed", 0);
    }

    r = miset_attr_values(hvol, MI_TYPE_STRING, "/minc-2.0/test1/stuff",
			  "objtype", 8, "bicycle");

    if (r < 0) {
	TESTRPT("miset_attr_values failed on rewrite", r);
    }

    r = miget_attr_values(hvol, MI_TYPE_STRING, "/minc-2.0/test1/stuff", 
			  "objtype", 128, valstr);
    if (r < 0) {
	TESTRPT("miget_attr_values failed", r);
    }

    if (strcmp(valstr, "bicycle") != 0) {
	TESTRPT("miget_attr_values failed", 0);
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
#endif /* M2_TEST */
