#include <stdio.h>
#include <string.h>
#include "minc2.h"

#define TESTRPT(msg, val) (error_cnt++, fprintf(stderr, \
                                  "Error reported on line #%d, %s: %d\n", \
                                  __LINE__, msg, val))

#define TESTARRAYSIZE 11

static int error_cnt = 0;

int main(int argc, char **argv)
{
    mihandle_t hvol;
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

    r = micreate_volume("tst-grpa.mnc", 0, NULL, MI_TYPE_UINT, 
                        MI_CLASS_INT, NULL, &hvol);
    if (r < 0) {
	TESTRPT("Unable to create test file", r);
	return (-1);
    }

    r = micreate_group(hvol, "/minc-2.0", "test1");
    if (r < 0) {
        TESTRPT("micreate_group failed", r);
    }

    r = micreate_group(hvol, "/minc-2.0", "test2");
    if (r < 0) {
        TESTRPT("micreate_group failed", r);
    }

    r = micreate_group(hvol, "/minc-2.0/test1", "stuff");
    if (r < 0) {
	TESTRPT("micreate_group failed", r);
    }

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
	    fprintf(stderr, "intarr[%d] = %d, tstarr[%d] = %d\n",
		    r, intarr[r], r, (int) tstarr[r]);
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

