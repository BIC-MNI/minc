/** \file grpattr.c
 * \brief MINC 2.0 group/attribute functions
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
    hdf_file = vol->hdf_id;
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
    hdf_file = vol->hdf_id;
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

/** Delete the subgroup \a name from the group \a path
 */
int
midelete_group(mihandle_t vol, const char *path, const char *name)
{
    hid_t hdf_file;
    hid_t hdf_grp;
    herr_t hdf_result;

    /* Get a handle to the actual HDF file 
     */
    hdf_file = vol->hdf_id;
    if (hdf_file < 0) {
	return (MI_ERROR);
    }

    /* Search through the path, descending into each group encountered.
     */
    hdf_grp = midescend_path(hdf_file, path);
    if (hdf_grp < 0) {
	return (MI_ERROR);
    }

    H5E_BEGIN_TRY {
        /* Delete the group (or any object, really) from the path.
         */
        hdf_result = H5Gunlink(hdf_grp, name);
        if (hdf_result < 0) {
            hdf_result = MI_ERROR;
        }
        else {
            hdf_result = MI_NOERROR;
        }
    } H5E_END_TRY;

    /* Close the handles we created.
     */
    H5Gclose(hdf_grp);
  
    return (hdf_result);
}

/** Get the length of a attribute
 */
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
    hdf_file = vol->hdf_id;
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

/** Get the type of an attribute.
 */
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
    hdf_file = vol->hdf_id;
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

/** Get the values of an attribute.
 */
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
    hdf_file = vol->hdf_id;
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

/** Set the values of an attribute.
 */
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
    hdf_file = vol->hdf_id;
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

    H5E_BEGIN_TRY {
        /* Delete attribute if it already exists. */
        H5Adelete(hdf_grp, name);
    } H5E_END_TRY;

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
