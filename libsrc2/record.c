/**\file record.c
 * \brief MINC 2.0 Record Functions
 * \author Bert Vincent
 */

#include <hdf5.h>
#include "minc2.h"
#include "minc2_private.h"

/** This method gets the name of the record dimension
 * TODO: set record name??
 */
int 
miget_record_name(mihandle_t volume,
                  char **name)
{
    return (MI_NOERROR);
}


/** This method gets the length (i.e., number of fields in the case of
 * uniform records and number of bytes for non_uniform ones) of the
 * record.
 */
int 
miget_record_length(mihandle_t volume,
                    int *length)
{
    if (volume == NULL || length == NULL) {
        return (MI_ERROR);
    }
    if (volume->volume_class == MI_CLASS_UNIFORM_RECORD ||
        volume->volume_class == MI_CLASS_NON_UNIFORM_RECORD) {
        *length = H5Tget_nmembers(volume->type_id);
        return (MI_NOERROR);
    }
    return (MI_ERROR);
}

/** This method returns the field name for the given field.
 */
int
miget_record_field_name(mihandle_t volume,
                        int index,
                        char **name)
{
    if (volume == NULL || name == NULL) {
        return (MI_ERROR);
    }
    *name = H5Tget_member_name(volume->type_id, index);
    if (*name == NULL) {
        return (MI_ERROR);
    }
    return (MI_NOERROR);
}

/** This method sets a field name for the given record. e.g. field is "red"
 */
int
miset_record_field_name(mihandle_t volume,
                        int index,
                        const char *name)
{
    hid_t type_id;
    int offset;

    if (volume == NULL || name == NULL) {
        return (MI_ERROR);
    }
    if (volume->volume_class != MI_CLASS_UNIFORM_RECORD &&
        volume->volume_class != MI_CLASS_NON_UNIFORM_RECORD) {
        return (MI_ERROR);
    }
    type_id = mitype_to_hdftype(volume->volume_type);
    offset = index * H5Tget_size(type_id);
    if (offset + H5Tget_size(type_id) > H5Tget_size(volume->type_id)) {
        H5Tset_size(volume->type_id, offset + H5Tget_size(type_id));
    }
    H5Tinsert(volume->type_id, name, offset, type_id);
    H5Tclose(type_id);
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
    char *name;
    int result;
    int value;
    midimhandle_t hdim[3];
    unsigned long coords[3];

    result = micreate_dimension("xspace", MI_DIMCLASS_SPATIAL, 
                           MI_DIMATTR_REGULARLY_SAMPLED, 10, &hdim[0]);

    result = micreate_dimension("yspace", MI_DIMCLASS_SPATIAL, 
                           MI_DIMATTR_REGULARLY_SAMPLED, 10, &hdim[1]);

    result = micreate_dimension("zspace", MI_DIMCLASS_SPATIAL, 
                           MI_DIMATTR_REGULARLY_SAMPLED, 6, &hdim[2]);

    result = micreate_volume("tst-rec.mnc", 3, hdim, MI_TYPE_UINT, 
                             MI_CLASS_UNIFORM_RECORD, NULL, &hvol);
    if (result < 0) {
	fprintf(stderr, "Unable to create test file %x\n", result);
	return (-1);
    }

    miset_record_field_name(hvol, 0, "Red");
    miset_record_field_name(hvol, 1, "Green");
    miset_record_field_name(hvol, 2, "Blue");

    miget_record_field_name(hvol, 1, &name);
    if (strcmp(name, "Green") != 0) {
	TESTRPT("Unexpected label for value 1", 0);
    }
    mifree_name(name);

    miget_record_field_name(hvol, 0, &name);
    if (strcmp(name, "Red") != 0) {
	TESTRPT("Unexpected label for value 0", 0);
    }
    mifree_name(name);

    miget_record_field_name(hvol, 2, &name);
    if (strcmp(name, "Blue") != 0) {
	TESTRPT("Unexpected label for value 2", 0);
    }
    mifree_name(name);

    micreate_volume_image(hvol);

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

#endif /* M2_TEST */
