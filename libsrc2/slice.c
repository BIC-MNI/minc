/************************************************************************
 * MINC 2.0 "SLICE/VOLUME SCALE" FUNCTIONS
 ************************************************************************/
#include <stdlib.h>
#include <hdf5.h>
#include "minc2.h"
#include "minc2_private.h"

/*! Get the minimum or maximum value for the slice containing the given point.
 */
#define MIRW_SCALE_SET 0x0001
#define MIRW_SCALE_MIN 0x0002

static int
mirw_slice_minmax(int opcode, mihandle_t volume, 
                  const unsigned long start_positions[],
                  int array_length, double *value)
{
    hid_t file_id;
    hid_t dset_id;
    hid_t fspc_id;
    hid_t mspc_id;
    hssize_t coords[3][0];
    int ndims;
    int i;
    int result;
    BOOLEAN slice_scaling_enabled;

    result = miget_slice_scaling_flag(volume, &slice_scaling_enabled);
    if (result < 0 || !slice_scaling_enabled) {
	return (MI_ERROR);
    }

    file_id = volume->hdf_id;
    if (opcode & MIRW_SCALE_MIN) {
	dset_id = midescend_path(file_id, "/minc-2.0/image/0/image-min");
    }
    else {
	dset_id = midescend_path(file_id, "/minc-2.0/image/0/image-max");
    }
    if (dset_id < 0) {
	return (MI_ERROR);
    }

    fspc_id = H5Dget_space(dset_id);
    if (fspc_id < 0) {
	return (MI_ERROR);
    }

    ndims = H5Sget_simple_extent_ndims(fspc_id);
    if (ndims > array_length) {
	ndims = array_length;
    }

    for (i = 0; i < ndims; i++) {
	coords[i][0] = start_positions[i];
    }

    result = H5Sselect_elements(fspc_id, H5S_SELECT_SET, 1, 
				(const hssize_t **) coords);
    if (result < 0) {
	return (MI_ERROR);
    }

    /* Create a trivial scalar space to read the single value.
     */
    mspc_id = H5Screate(H5S_SCALAR);

    if (opcode & MIRW_SCALE_SET) {
	result = H5Dwrite(dset_id, H5T_NATIVE_DOUBLE, mspc_id, fspc_id, 
			  H5P_DEFAULT, value);
    }
    else {
	result = H5Dread(dset_id, H5T_NATIVE_DOUBLE, mspc_id, fspc_id, 
			 H5P_DEFAULT, value);
    }

    if (result < 0) {
	return (MI_ERROR);
    }

    H5Sclose(fspc_id);
    H5Sclose(mspc_id);
    H5Dclose(dset_id);
    return (MI_NOERROR);
}

int
miget_slice_min(mihandle_t volume, const unsigned long start_positions[],
		int array_length, double *slice_min)
{
    return (mirw_slice_minmax(MIRW_SCALE_MIN, 
			      volume, start_positions, 
			      array_length, slice_min));
}

int
miget_slice_max(mihandle_t volume, const unsigned long start_positions[],
		int array_length, double *slice_max)
{
    return (mirw_slice_minmax(0, 
			      volume, start_positions, 
			      array_length, slice_max));
}

int
miset_slice_min(mihandle_t volume, const unsigned long start_positions[],
		int array_length, double slice_min)
{
    return (mirw_slice_minmax(MIRW_SCALE_MIN + MIRW_SCALE_SET, 
			      volume, start_positions, 
			      array_length, &slice_min));
}

int
miset_slice_max(mihandle_t volume, const unsigned long start_positions[],
		int array_length, double slice_max)
{
    return (mirw_slice_minmax(MIRW_SCALE_SET, 
			      volume, start_positions, 
			      array_length, &slice_max));
}

int
miget_slice_range(mihandle_t volume, const unsigned long start_positions[],
		  int array_length, double *slice_max, double *slice_min)
{
    int r;

    r = mirw_slice_minmax(0, 
			  volume, start_positions, 
			  array_length, slice_max);
    if (r < 0) {
	return (MI_ERROR);
    }

    r = mirw_slice_minmax(MIRW_SCALE_MIN,
			  volume, start_positions,
			  array_length, slice_min);

    if (r < 0) {
	return (MI_ERROR);
    }

    return (MI_NOERROR);
}

int
miset_slice_range(mihandle_t volume, const unsigned long start_positions[],
		  int array_length, double slice_max, double slice_min)
{
    int r;

    r = mirw_slice_minmax(MIRW_SCALE_SET, 
			  volume, start_positions, 
			  array_length, &slice_max);
    if (r < 0) {
	return (MI_ERROR);
    }

    r = mirw_slice_minmax(MIRW_SCALE_SET + MIRW_SCALE_MIN,
			  volume, start_positions,
			  array_length, &slice_min);

    if (r < 0) {
	return (MI_ERROR);
    }

    return (MI_NOERROR);
}

/*!
 */
static int
mirw_volume_minmax(int opcode, mihandle_t volume, double *value)
{
    hid_t file_id;
    hid_t dset_id;
    hid_t fspc_id;
    hid_t mspc_id;
    int result;
    BOOLEAN slice_scaling_enabled;

    result = miget_slice_scaling_flag(volume, &slice_scaling_enabled);
    if (result < 0 || slice_scaling_enabled) {
	return (MI_ERROR);
    }

    file_id = volume->hdf_id;
    if (opcode & MIRW_SCALE_MIN) {
	dset_id = midescend_path(file_id, "/minc-2.0/image/0/image-min");
    }
    else {
	dset_id = midescend_path(file_id, "/minc-2.0/image/0/image-max");
    }
    if (dset_id < 0) {
	return (MI_ERROR);
    }

    fspc_id = H5Dget_space(dset_id);
    if (fspc_id < 0) {
	return (MI_ERROR);
    }

    /* Make certain the value is a scalar.
     */
    if (H5Sget_simple_extent_ndims(fspc_id) != 0) {
	return (MI_ERROR);
    }

    /* Create a trivial scalar space to read the single value.
     */
    mspc_id = H5Screate(H5S_SCALAR);

    if (opcode & MIRW_SCALE_SET) {
	result = H5Dwrite(dset_id, H5T_NATIVE_DOUBLE, mspc_id, fspc_id, 
			  H5P_DEFAULT, value);
    }
    else {
	result = H5Dread(dset_id, H5T_NATIVE_DOUBLE, mspc_id, fspc_id, 
			 H5P_DEFAULT, value);
    }

    if (result < 0) {
	return (MI_ERROR);
    }

    H5Sclose(fspc_id);
    H5Sclose(mspc_id);
    H5Dclose(dset_id);
    return (MI_NOERROR);
}

int
miget_volume_min(mihandle_t volume, double *vol_min)
{
    return (mirw_volume_minmax(MIRW_SCALE_MIN, volume, vol_min));
}

int
miget_volume_max(mihandle_t volume, double *vol_max)
{
    return (mirw_volume_minmax(0, volume, vol_max));
}

int
miset_volume_min(mihandle_t volume, double vol_min)
{
    return (mirw_volume_minmax(MIRW_SCALE_MIN + MIRW_SCALE_SET, 
			       volume, &vol_min));
}

int
miset_volume_max(mihandle_t volume, double vol_max)
{
    return (mirw_volume_minmax(MIRW_SCALE_SET, 
			       volume, &vol_max));
}

int
miget_volume_range(mihandle_t volume, double *vol_max, double *vol_min)
{
    int r;

    r = mirw_volume_minmax(0, volume, vol_max);
    if (r < 0) {
	return (MI_ERROR);
    }

    r = mirw_volume_minmax(MIRW_SCALE_MIN, volume, vol_min);
    if (r < 0) {
	return (MI_ERROR);
    }

    return (MI_NOERROR);
}

int
miset_volume_range(mihandle_t volume, double vol_max, double vol_min)
{
    int r;

    r = mirw_volume_minmax(MIRW_SCALE_SET, volume, &vol_max);
    if (r < 0) {
	return (MI_ERROR);
    }

    r = mirw_volume_minmax(MIRW_SCALE_SET + MIRW_SCALE_MIN, volume, 
			   &vol_min);
    if (r < 0) {
	return (MI_ERROR);
    }

    return (MI_NOERROR);
}
#ifdef M2_TEST

int
main(int argc, char **argv)
{
    mihandle_t hvol;
    int r;
    unsigned long coords[3];
    double min, max;
    int i;

    while (--argc > 0) {
	r = miopen_volume(*++argv, MI2_OPEN_READ, &hvol);
	if (r < 0) {
	    fprintf(stderr, "can't open %s, error %d\n", *argv, r);
	}
	else {
	    for (i = 0; i < 10; i++) {
		coords[0] = i;
		coords[1] = rand();
		coords[2] = rand();

		r = miget_slice_min(hvol, coords, 3, &min);
		if (r < 0) {
		    fprintf(stderr, "error %d getting slice minimum\n", r);
		}

		r = miget_slice_max(hvol, coords, 3, &max);
		if (r < 0) {
		    fprintf(stderr, "error %d getting slice maximum\n", r);
		}
		printf("%d. min %f max %f\n", i, min, max);
	    }
	    miclose_volume(hvol);
	}
    }
}
#endif
