/** \file datatype.c
 * \brief MINC 2.0 data type/space functions
 ************************************************************************/

#include <stdlib.h>
#include <hdf5.h>
#include "minc2.h"
#include "minc2_private.h"

/** Return the data class of a volume (See miclass_t).
 */
int
miget_data_class(mihandle_t volume, miclass_t *volume_class)
{
    /* TODO: where do we derive the class?  Is it an attribute? */
    *volume_class = 0;
    return (MI_NOERROR);
}

/** Return the data type of a volume (See mitype_t).
 */
int
miget_data_type(mihandle_t volume, mitype_t *data_type)
{
    hid_t grp_id;
    hid_t dset_id;
    hid_t type_id;
    H5T_class_t class;
    size_t nbytes;
    int is_signed;
    int result = MI_NOERROR;
    hid_t file_id = volume->hdf_id;

    grp_id = midescend_path(file_id, MI_FULLIMAGE_PATH);
    if (grp_id < 0) {
	return (MI_ERROR);
    }

    dset_id = H5Dopen(grp_id, "image");
    if (dset_id < 0) {
	return (MI_ERROR);
    }
    type_id = H5Dget_type(dset_id);
    if (type_id < 0) {
	return (MI_ERROR);
    }
    /* Convert the type to a MINC type.
     */
    class = H5Tget_class(type_id);
    nbytes = H5Tget_size(type_id);

    switch (class) {
    case H5T_INTEGER:
	is_signed = (H5Tget_size(type_id) == H5T_SGN_2);

	switch (nbytes) {
	case 1:
	    *data_type = (is_signed ? MI_TYPE_BYTE : MI_TYPE_UBYTE);
	    break;
	case 2:
	    *data_type = (is_signed ? MI_TYPE_SHORT : MI_TYPE_USHORT);
	    break;
	case 4:
	    *data_type = (is_signed ? MI_TYPE_INT : MI_TYPE_UINT);
	    break;
	default:
	    result = MI_ERROR;
	    break;
	}
	break;
    case H5T_FLOAT:
	*data_type = (nbytes == 4) ? MI_TYPE_FLOAT : MI_TYPE_DOUBLE;
	break;
    case H5T_STRING:
	*data_type = MI_TYPE_STRING;
	break;
    case H5T_ARRAY:
	/* TODO: handle this case for uniform records (arrays)? */
	break;
    case H5T_COMPOUND:
	/* TODO: handle this case for non-uniform records? */
	break;
    default:
	result = MI_ERROR;
	break;
    }
    H5Tclose(type_id);
    H5Dclose(dset_id);
    H5Gclose(grp_id);
    return (result);
}

int
miget_data_type_size(mihandle_t volume, misize_t *voxel_size)
{
    hid_t grp_id;
    hid_t dset_id;
    hid_t type_id;
    hid_t file_id = volume->hdf_id;

    grp_id = midescend_path(file_id, MI_FULLIMAGE_PATH);
    if (grp_id < 0) {
	return (MI_ERROR);
    }
    dset_id = H5Dopen(grp_id, "image");
    if (dset_id < 0) {
	return (MI_ERROR);
    }

    type_id = H5Dget_type(dset_id);
    if (type_id < 0) {
	return (MI_ERROR);
    }

    *voxel_size = H5Tget_size(type_id);

    H5Tclose(type_id);
    H5Dclose(dset_id);
    H5Gclose(grp_id);

    return (MI_NOERROR);
}

int
miget_space_name(mihandle_t volume, char **name)
{
    int length;
    int result;
    int i;
    /* This is the order of the search for candidates for the space type.
       The reason for this is complication is to permit support for older-style
       MINC files which associate the spacetype with individual dimensions.
    */
    static const char *path_list[] = {
	"/minc-2.0/info",
	"/minc-2.0/dimensions/xspace",
	"/minc-2.0/dimensions/yspace",
	"/minc-2.0/dimensions/zspace",
	NULL
    };

    /* Search for the spacetype attribute in all available paths.
     */
    for (i = 0; path_list[i] != 0; i++) {
	result = miget_attr_length(volume, path_list[i], "spacetype", 
				   &length);
	if (result == MI_NOERROR) {
	    break;
	}
    }

    if (result != MI_NOERROR) {
	/* Nothing found, so use the default.
	 */
	length = strlen(MI_NATIVE);
	*name = malloc(length + 1);
	strcpy(*name, MI_NATIVE);
    }
    else {
	*name = malloc(length + 1);
	result = miget_attr_values(volume, MI_TYPE_STRING, path_list[i],
				   "spacetype", length, *name);
    }
    return (result);
}

int
miset_space_name(mihandle_t volume, const char *name)
{
  return miset_attr_values(volume, MI_TYPE_STRING, "/minc-2.0/info", 
			   "spacetype", strlen(name), name);
		    
}

#ifdef M2_TEST
int
main(int argc, char **argv)
{
    miclass_t myclass;
    mitype_t mytype;
    misize_t mysize;
    char *myname;
    mihandle_t volume;

    /* Turn off automatic error reporting.
     */
    H5Eset_auto(NULL, NULL);

    /* Check each file.
     */
    while (--argc > 0) {
	
	++argv;

	if (micreate_volume(*argv, 0, NULL, 0, 0, NULL, &volume) < 0) {
	    fprintf(stderr, "Error opening %s\n", *argv);
	}
	else {
	    int i;
	    /* Repeat many times to expose resource leakage problems, etc.
	     */
	    for (i = 0; i < 25000; i++) {
		miget_data_type(volume, &mytype);
		miget_data_type_size(volume, &mysize);
		miget_data_class(volume, &myclass);
		miget_space_name(volume, &myname);
	    
		mifree_name(myname);
	    }

	    miclose_volume(volume);

	    printf("file: %s type %d size %ld class %d name %s\n", *argv,
		   mytype, mysize, myclass, myname);
	}

    }
    return (0);
}
#endif
