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
    if (volume == NULL || valid_max == NULL) {
        return (MI_ERROR);      /* Invalid arguments */
    }
    *valid_max = volume->valid_max;
    return (MI_NOERROR);
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
    if (volume == NULL) {
        return (MI_ERROR);      /* Invalid arguments */
    }
    /* TODO?: Should we require valid max to have some specific relationship
     * to valid_min?
     */
    volume->valid_max = valid_max;
    misave_valid_range(volume);
    return (MI_NOERROR);
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
    if (volume == NULL || valid_min == NULL) {
        return (MI_ERROR);      /* Invalid arguments. */
    }
    *valid_min = volume->valid_min;
    return (MI_NOERROR);
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
    if (volume == NULL) {
        return (MI_ERROR);       /* Invalid arguments */
    }
    volume->valid_min = valid_min;
    misave_valid_range(volume);
    return (MI_NOERROR);
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
    if (volume == NULL || valid_min == NULL || valid_max == NULL) {
        return (MI_ERROR);
    }
    *valid_min = volume->valid_min;
    *valid_max = volume->valid_max;
    return (MI_NOERROR);
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
    if (volume == NULL) {
        return (MI_ERROR);
    }
    /* TODO?: Again, should we require min<max, for example?  Or should we
     * just do the right thing and swap them?  What if valid_max is greater
     * than the maximum value that can be represented by the volume's type?
     */
    volume->valid_min = valid_min;
    volume->valid_max = valid_max;
    misave_valid_range(volume);
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
    int r;
    double min, max;
    double orig_min, orig_max;

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

        r = miset_volume_valid_range(hvol, orig_max, orig_min);
        if (r < 0) {
            TESTRPT("error restoring volume range", r);
        }

        miclose_volume(hvol);
    }
    return (error_cnt);
}
#endif
