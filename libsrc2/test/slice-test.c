#include <stdio.h>
#include <stdlib.h>
#include "minc2.h"

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

