/**\file volume.c
 * \brief MINC 2.0 Volume Functions
 * \author Bert Vincent and Leila Baghdadi
 *
 * Functions to create, open, and close MINC volumes.
 ************************************************************************/

#include <stdlib.h>
#include <hdf5.h>
#include "minc2.h"
#include "minc2_private.h"

/* TODO: Right now these functions are just minimal placeholders, not the
 * "real thing".
 */
/* OOPS! Forgot the return value in the documentation... */
int
micreate_volume(const char *filename, int number_of_dimensions,
		midimhandle_t dimensions[],
		mitype_t volume_type,
		miclass_t volume_class,
		mivolumeprops_t create_props,
		mihandle_t *volume)
{
    /* TODO: this is just a skeleton, needs lots of extensions. */
    hid_t file_id;
    hid_t g1_id;
    mivolume_ptr vol_ptr;

    file_id = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    if (file_id < 0) {
        return (MI_ERROR);
    }
    g1_id = H5Gcreate(file_id, "/minc-2.0", 0);
    if (g1_id < 0) {
	return (MI_ERROR);
    }
    H5Gclose(g1_id);

    vol_ptr = (mivolume_ptr) malloc(sizeof (mivolume_struct));
    if (vol_ptr == NULL) {
	return (MI_ERROR);
    }

    vol_ptr->hdf_id = file_id;
    vol_ptr->has_slice_scaling = FALSE;
    
    *volume = (mihandle_t) vol_ptr;
    return (MI_NOERROR);
}

int
miget_volume_dimension_count(mihandle_t volume,
			     midimclass_t class,
			     midimattr_t attr )
{
    /* TODO: something!!! */
    return (0);
}

int
miopen_volume(const char *filename, int mode, mihandle_t *volume)
{
    hid_t file_id;
    hid_t dset_id;
    hid_t space_id;
    mivolume_ptr vol_ptr;
    int hdf_mode;
    if (mode == MI2_OPEN_READ) {
        hdf_mode = H5F_ACC_RDONLY;
    }
    else if (mode == MI2_OPEN_RDWR) {
        hdf_mode = H5F_ACC_RDWR;
    }
    else {
        return (MI_ERROR);
    }
    file_id = hdf_open(filename, hdf_mode);
    if (file_id < 0) {
	return (MI_ERROR);
    }

    /* SEE IF SLICE SCALING IS ENABLED
     */

    vol_ptr = malloc(sizeof (mivolume_struct));
    if (vol_ptr == NULL) {
	return (MI_ERROR);
    }

    vol_ptr->hdf_id = file_id;

    vol_ptr->has_slice_scaling = FALSE;
    dset_id = midescend_path(file_id, "/minc-2.0/image/0/image-max");
    if (dset_id >= 0) {
	space_id = H5Dget_space(dset_id);
	if (space_id >= 0) {
	    /* If the dimensionality of the image-max variable is one or
	     * greater, we consider this volume to have slice-scaling enabled.
	     */
	    if (H5Sget_simple_extent_ndims(space_id) >= 1) {
		vol_ptr->has_slice_scaling = TRUE;
	    }
	    H5Sclose(space_id);	/* Close the dataspace handle */
	}
	H5Dclose(dset_id);	/* Close the dataset handle */
    }
    *volume = (mihandle_t) vol_ptr;
    return (MI_NOERROR);
}

int 
miclose_volume(mihandle_t volume)
{
    hid_t file_id = miget_volume_file_handle(volume);
    if (H5Fclose(file_id) < 0) {
	return (MI_ERROR);
    }
    free((mivolume_ptr) volume);
    return (MI_NOERROR);
}

/*! Function to get the volume's slice-scaling flag.
 */
int
miget_slice_scaling_flag(mihandle_t volume, BOOLEAN *slice_scaling_flag)
{
    if (volume == NULL || slice_scaling_flag == NULL) {
	return (MI_ERROR);
    }
    *slice_scaling_flag = ((mivolume_ptr) volume)->has_slice_scaling;
    return (MI_NOERROR);
}

/*! Function to set the volume's slice-scaling flag.
 */
int
miset_slice_scaling_flag(mihandle_t volume, BOOLEAN slice_scaling_flag)
{
    if (volume == NULL) {
	return (MI_ERROR);
    }
    ((mivolume_ptr) volume)->has_slice_scaling = slice_scaling_flag;
    return (MI_NOERROR);
}

