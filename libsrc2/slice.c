/**
 * \file slice.c
 * \brief MINC 2.0 Slice/volume scale functions
 * \author Bert Vincent
 *
 * These functions get and set the real minimum and maximum values for
 * either a particular slice or an entire volume.
 *
 * Each of the slice scale functions take an array of long integer
 * coordinates to specify the slice to consider.  The order of these
 * coordinates is always raw file (voxel) order, rather than the
 * apparent order.
 *
 * If slice scaling is not enabled, the slice scaling functions will
 * use the appropriate global volume scale value.
 ************************************************************************/
#include <stdlib.h>
#include <hdf5.h>
#include "minc2.h"
#include "minc2_private.h"

#define MIRW_SCALE_SET 0x0001
#define MIRW_SCALE_GET 0x0000

#define MIRW_SCALE_MIN 0x0002
#define MIRW_SCALE_MAX 0x0000

/* Forward declaration
 */
static int mirw_volume_minmax(int opcode, mihandle_t volume, double *value);

/** Get the minimum or maximum value for the slice containing the given point.
 */
static int
mirw_slice_minmax(int opcode, mihandle_t volume, 
                  const unsigned long start_positions[],
                  int array_length, double *value)
{
    hid_t file_id;
    hid_t dset_id;
    hid_t fspc_id;
    hid_t mspc_id;
    hssize_t coords[3][1];
    int ndims;
    int i;
    int result;
    char path[128];             /* TODO: string length? */

    if (volume == NULL || value == NULL) {
        return (MI_ERROR);      /* Bad parameters */
    }

    if (!volume->has_slice_scaling) {
        mirw_volume_minmax(opcode, volume, value);
    }

    file_id = volume->hdf_id;

    if (opcode & MIRW_SCALE_MIN) {
        sprintf(path, "/minc-2.0/image/%d/image-min", 
                volume->selected_resolution);
    }
    else {
        sprintf(path, "/minc-2.0/image/%d/image-max", 
                volume->selected_resolution);
    }
    dset_id = midescend_path(file_id, path);
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

/**
This function sets \a slice_min to the minimum real value of
voxels in the slice containing the coordinates \a start_positions.
The \a array_length may be less than or equal to the number of dimensions
in the volume, extra coordinates will be ignored.  Specifying too few
coordinates will trigger an error.
Coordinates must always be specified in raw file order.
 */
int
miget_slice_min(mihandle_t volume, const unsigned long start_positions[],
		int array_length, double *slice_min)
{
    return (mirw_slice_minmax(MIRW_SCALE_MIN + MIRW_SCALE_GET, 
			      volume, start_positions, 
			      array_length, slice_min));
}

/**
This function sets \a slice_max to the maximum real value of
voxels in the slice containing the coordinates \a start_positions.
The \a array_length may be less than or equal to the number of dimensions
in the volume, extra coordinates will be ignored.  Specifying too few
coordinates will trigger an error.
Coordinates must always be specified in raw file order.
 */
int
miget_slice_max(mihandle_t volume, const unsigned long start_positions[],
		int array_length, double *slice_max)
{
    return (mirw_slice_minmax(MIRW_SCALE_MAX + MIRW_SCALE_GET, 
			      volume, start_positions, 
			      array_length, slice_max));
}

/**
This function sets minimum real value of
values in the slice containing the coordinates \a start_positions.
The \a array_length may be less than or equal to the number of dimensions
in the volume, extra coordinates will be ignored.  Specifying too few
coordinates will trigger an error.
Coordinates must always be specified in raw file order.
 */
int
miset_slice_min(mihandle_t volume, const unsigned long start_positions[],
		int array_length, double slice_min)
{
    return (mirw_slice_minmax(MIRW_SCALE_MIN + MIRW_SCALE_SET, 
			      volume, start_positions, 
			      array_length, &slice_min));
}

/**
This function sets maximum real value of
values in the slice containing the coordinates \a start_positions.
The \a array_length may be less than or equal to the number of dimensions
in the volume, extra coordinates will be ignored.  Specifying too few
coordinates will trigger an error.
Coordinates must always be specified in raw file order.
 */
int
miset_slice_max(mihandle_t volume, const unsigned long start_positions[],
		int array_length, double slice_max)
{
    return (mirw_slice_minmax(MIRW_SCALE_MAX + MIRW_SCALE_SET, 
			      volume, start_positions, 
			      array_length, &slice_max));
}

/**
This function gets both the minimum and
maximum real value of voxels in the slice containing the coordinates
\a start_positions.  The \a array_length may be less than or equal to
the number of dimensions in the volume, extra coordinates will be
ignored.  Specifying too few coordinates will trigger an error.
Coordinates must always be specified in raw file order.
 */
int
miget_slice_range(mihandle_t volume, const unsigned long start_positions[],
		  int array_length, double *slice_max, double *slice_min)
{
    int r;

    r = mirw_slice_minmax(MIRW_SCALE_MAX + MIRW_SCALE_GET, 
			  volume, start_positions, 
			  array_length, slice_max);
    if (r < 0) {
        *slice_max = 1.0;
    }

    r = mirw_slice_minmax(MIRW_SCALE_MIN + MIRW_SCALE_GET,
			  volume, start_positions,
			  array_length, slice_min);

    if (r < 0) {
        *slice_min = 0.0;
    }

    return (MI_NOERROR);
}

/**
This function the minimum and maximum real value of voxels in the
slice containing the coordinates \a start_positions.  The \a
array_length may be less than or equal to the number of dimensions in
the volume, extra coordinates will be ignored.  Specifying too few
coordinates will trigger an error.  Coordinates must always be
specified in raw file order.
 */
int
miset_slice_range(mihandle_t volume, const unsigned long start_positions[],
		  int array_length, double slice_max, double slice_min)
{
    int r;

    r = mirw_slice_minmax(MIRW_SCALE_MAX + MIRW_SCALE_SET, 
			  volume, start_positions, 
			  array_length, &slice_max);
    if (r < 0) {
	return (MI_ERROR);
    }

    r = mirw_slice_minmax(MIRW_SCALE_MIN + MIRW_SCALE_SET,
			  volume, start_positions,
			  array_length, &slice_min);

    if (r < 0) {
	return (MI_ERROR);
    }

    return (MI_NOERROR);
}

/*! Internal function to read/write the volume global minimum or
 * maximum real range.
 */
static int
mirw_volume_minmax(int opcode, mihandle_t volume, double *value)
{
    hid_t file_id;
    hid_t dset_id;
    hid_t fspc_id;
    hid_t mspc_id;
    int result;
    char path[128];

    if (volume == NULL || value == NULL) {
        return (MI_ERROR);
    }
    if (volume->has_slice_scaling) {
	return (MI_ERROR);
    }

    file_id = volume->hdf_id;
    if (opcode & MIRW_SCALE_MIN) {
	sprintf(path, "/minc-2.0/image/%d/image-min", 
                volume->selected_resolution);
    }
    else {
	sprintf(path, "/minc-2.0/image/%d/image-max", 
                volume->selected_resolution);
    }
    dset_id = midescend_path(file_id, path);
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

/**
This function returns the minimum real value of
voxels in the entire \a volume.  If per-slice scaling is enabled, this
function will return an error.
 */
int
miget_volume_min(mihandle_t volume, double *vol_min)
{
    return (mirw_volume_minmax(MIRW_SCALE_MIN + MIRW_SCALE_GET, 
                               volume, vol_min));
}

/**
This function returns the maximum real value of
voxels in the entire \a volume.  If per-slice scaling is enabled, this
function will return an error.
 */
int
miget_volume_max(mihandle_t volume, double *vol_max)
{
    return (mirw_volume_minmax(MIRW_SCALE_MAX + MIRW_SCALE_GET, 
                               volume, vol_max));
}

/**
This function sets the minimum real value of
voxels in the entire \a volume.  If per-slice scaling is enabled, this
function will return an error.
 */
int
miset_volume_min(mihandle_t volume, double vol_min)
{
    return (mirw_volume_minmax(MIRW_SCALE_MIN + MIRW_SCALE_SET, 
			       volume, &vol_min));
}

/**
This function sets the maximum real value of
voxels in the entire \a volume.  If per-slice scaling is enabled, this
function will return an error.
 */
int
miset_volume_max(mihandle_t volume, double vol_max)
{
    return (mirw_volume_minmax(MIRW_SCALE_MAX + MIRW_SCALE_SET, 
			       volume, &vol_max));
}

/**
This function retrieves the maximum and minimum real values of
voxels in the entire \a volume.  If per-slice scaling is enabled, this
function will return an error.
 */
int
miget_volume_range(mihandle_t volume, double *vol_max, double *vol_min)
{
    int r;

    r = mirw_volume_minmax(MIRW_SCALE_MAX + MIRW_SCALE_GET, volume, vol_max);
    if (r < 0) {
	return (MI_ERROR);
    }

    r = mirw_volume_minmax(MIRW_SCALE_MIN + MIRW_SCALE_GET, volume, vol_min);
    if (r < 0) {
	return (MI_ERROR);
    }

    return (MI_NOERROR);
}

/**
This function sets the maximum and minimum real values of
voxels in the entire \a volume.  If per-slice scaling is enabled, this
function will return an error.
 */
int
miset_volume_range(mihandle_t volume, double vol_max, double vol_min)
{
    int r;

    r = mirw_volume_minmax(MIRW_SCALE_MAX + MIRW_SCALE_SET, volume, &vol_max);
    if (r < 0) {
	return (MI_ERROR);
    }

    r = mirw_volume_minmax(MIRW_SCALE_MIN + MIRW_SCALE_MIN, volume, &vol_min);
    if (r < 0) {
	return (MI_ERROR);
    }

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
    *slice_scaling_flag = volume->has_slice_scaling;
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
    volume->has_slice_scaling = slice_scaling_flag;
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
    return (0);
}
#endif
