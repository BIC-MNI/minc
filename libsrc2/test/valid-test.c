#include <stdio.h>
#include "minc2.h"

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

