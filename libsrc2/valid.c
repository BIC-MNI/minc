/** \file valid.c
 * \brief MINC 2.0 Valid Min/Max and Range Functions.
 * \author Bert Vincent
 *
 * These functions set and get the valid range of the datatype of a volume.
 * This range refers to the maximum values of the datatype as stored in the
 * file, not to the range of the "real" values.  For example, many scanners
 * store voxel data as unsigned, 12-bit integers.  Since there is no explicit
 * 12-bit data type in MINC, we may use these functions to set the valid range
 * in the image to 0..4095.
 ************************************************************************/
#include <stdlib.h>
#include <hdf5.h>
#include "minc.h"               /* For MINC 1 routines we use */
#include "minc2.h"
#include "minc2_private.h"

/** This function gets the maximum valid value specific to the data
 *  type of the \a volume parameter.
 * \retval MI_ERROR on failure
 * \retval MI_NOERROR on success
 */
int
miget_volume_valid_max(mihandle_t volume, /**< MINC 2.0 volume handle */
                       double *valid_max) /**< the output value */
{
    double range[2];
    int result;
    hid_t file_id;

    file_id = volume->hdf_id;

    //result = miget_valid_range(file_id, MI2varid(file_id, MIimage), range);
    if (result == MI_NOERROR) {
        *valid_max = range[1];
    }
    return (result);
}

/** This function sets the maximum valid value specific to the data
 *  type of the \a volume parameter.
 * \retval MI_ERROR on failure
 * \retval MI_NOERROR on success
 */
int
miset_volume_valid_max(mihandle_t volume, /**< MINC 2.0 volume handle */
                       double valid_max) /**< the new maximum value  */
{
    double range[2];
    int result;
    hid_t file_id;

    file_id = volume->hdf_id;

    //result = miget_valid_range(file_id, MI2varid(file_id, MIimage), range);
    if (result == MI_NOERROR) {
        range[1] = valid_max;
        //result = miset_valid_range(file_id, MI2varid(file_id, MIimage), range);
    }
    return (result);
}

/** This function gets the minimum valid value specific to the data
 *  type of the \a volume parameter.
 * \retval MI_ERROR on failure
 * \retval MI_NOERROR on success
 */
int
miget_volume_valid_min(mihandle_t volume, /**< MINC 2.0 volume handle */
                       double *valid_min) /**< the output value  */
{
    double range[2];
    int result;
    hid_t file_id;

    file_id = volume->hdf_id;

    //result = miget_valid_range(file_id, MI2varid(file_id, MIimage), range);
    if (result == MI_NOERROR) {
        *valid_min = range[0];
    }
    return (result);
}

/** This function sets the minimum valid value specific to the data
 *  type of the \a volume parameter.
 * \retval MI_ERROR on failure
 * \retval MI_NOERROR on success
 */
int
miset_volume_valid_min(mihandle_t volume,  /**< MINC 2.0 volume handle */
                       double valid_min) /**< the new minimum value  */
{
    double range[2];
    int result;
    hid_t file_id;

    file_id = volume->hdf_id;

    //result = miget_valid_range(file_id, MI2varid(file_id, MIimage), range);
    if (result == MI_NOERROR) {
        range[0] = valid_min;
	// result = miset_valid_range(file_id, MI2varid(file_id, MIimage), range);
    }
    return (result);
}

/** This function gets the minimum and maximum valid value specific to the 
 * data type of the \a volume parameter.
 * \retval MI_ERROR on failure
 * \retval MI_NOERROR on success
 */
int
miget_volume_valid_range(mihandle_t volume,  /**< MINC 2.0 volume handle */
                         double *valid_max, /**< the output maximum value */
                         double *valid_min) /**< the output minimum value */
{
    double range[2];
    int result;
    hid_t file_id;

    file_id = volume->hdf_id;

    //result = miget_valid_range(file_id, MI2varid(file_id, MIimage), range);
    if (result == MI_NOERROR) {
        *valid_min = range[0];
        *valid_max = range[1];
    }
    return (result);
}

/** This function sets the minimum and maximum valid value specific to the 
 * data type of the \a volume parameter.
 * \retval MI_ERROR on failure
 * \retval MI_NOERROR on success
 */
int
miset_volume_valid_range(mihandle_t volume, /**< MINC 2.0 volume handle */
                         double valid_max, /**< the new maximum value */
                         double valid_min) /**< the output minimum value */
{
    double range[2];
    hid_t file_id;

    file_id = volume->hdf_id;

    range[0] = valid_min;
    range[1] = valid_max;
    //return miset_valid_range(file_id, MI2varid(file_id, MIimage), range);
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
    int r;
    double min, max;
    double orig_min, orig_max;
    double range[2];
    int length;
    hid_t file_id;

    while (--argc > 0) {
	r = miopen_volume(*++argv, MI2_OPEN_RDWR, &hvol);
	if (r < 0) {
	    TESTRPT("can't open input", r);
            continue;
	}

        r = miget_volume_valid_min(hvol, &min);
        if (r < 0) {
            TESTRPT("error getting valid minimum", r);
        }

        r = miget_volume_valid_max(hvol, &max);
        if (r < 0) {
            TESTRPT("error getting valid maximum", r);
        }
            
        r = miget_volume_valid_range(hvol, &max, &min);
        if (r < 0) {
            TESTRPT("error getting valid range", r);
        }

        printf("min %f max %f\n", min, max);
        if (min > max) {
            TESTRPT("error - min exceeds max!", 0);
        }

        orig_min = min;
        orig_max = max;

        /* Try some arbitrary manipulations */
        max = orig_max + 100;
        min = orig_min - 100;

        r = miset_volume_valid_range(hvol, max, min);
        if (r < 0) {
            TESTRPT("error setting new volume range", 0);
        }

        r = miget_volume_valid_min(hvol, &min);
        if (r != 0 || min != orig_min - 100) {
            TESTRPT("error changing volume minimum", 0);
        }

        r = miget_volume_valid_max(hvol, &max);
        if (max != orig_max + 100) {
            TESTRPT("error changing volume maximum", 0);
        }

        file_id = hvol->hdf_id;
        //r = miattget(file_id, MI2varid(file_id, MIimage),
	//MIvalid_range, NC_DOUBLE, 2, range, &length);
        if (r < 0 || length != 2) {
            TESTRPT("error reading attribute", r);
        }

        if (range[0] != min || range[1] != max) {
            TESTRPT("error reading attribute", 0);
        }

        r = miset_volume_valid_range(hvol, orig_max, orig_min);
        if (r < 0) {
            TESTRPT("error restoring volume range", r);
        }

        miclose_volume(hvol);
    }
    return (error_cnt);
}
#endif
