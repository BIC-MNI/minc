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

/** This method returns the field name for the given field index.  Memory
 * for returned string is allocated on the heap and should be released using
 * mifree_name().
 */
int
miget_record_field_name(mihandle_t volume,
                        int index,
                        char **name)
{
    if (volume == NULL || name == NULL) {
        return (MI_ERROR);
    }
    /* Get the field name.  The H5Tget_member_name() function allocates
     * the memory for the string using malloc(), so we can return the 
     * pointer directly without any further manipulations.
     */
    *name = H5Tget_member_name(volume->type_id, index);
    if (*name == NULL) {
        return (MI_ERROR);
    }
    return (MI_NOERROR);
}

/** This method sets a field name for the volume record. The volume
 * must be of class "MI_CLASS_UNIFORM_RECORD".  The size of record
 * type will be increased if necessary to accomodate the new field.
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
    /* Get the type of the record's fields.  This is recorded as the
     * type of the volume.
     */
    type_id = mitype_to_hdftype(volume->volume_type);

    /* Calculate the offset of the new member.
     */
    offset = index * H5Tget_size(type_id);

    /* If the offset plus the size of the member is larger than the
     * current size of the structure, increase the size of the structure.
     */
    if (offset + H5Tget_size(type_id) > H5Tget_size(volume->type_id)) {
        H5Tset_size(volume->type_id, offset + H5Tget_size(type_id));
    }

    /* Actually define the field within the structure.
     */
    H5Tinsert(volume->type_id, name, offset, type_id);

    /* Delete the HDF5 type object returned by mitype_to_hdftype().
     */
    H5Tclose(type_id);

    return (MI_NOERROR);
}

#ifdef M2_TEST
#define TESTRPT(msg, val) (error_cnt++, fprintf(stderr, \
                                  "Error reported on line #%d, %s: %d\n", \
                                  __LINE__, msg, val))

static int error_cnt = 0;

#define CX 10
#define CY 10
#define CZ 6
#define NDIMS 3

int
main(int argc, char **argv)
{
    mihandle_t hvol;
    char *name;
    int result;
    midimhandle_t hdim[NDIMS];
    unsigned long coords[NDIMS];
    unsigned long count[NDIMS];
    int i,j,k;
    struct test {
        int r;
        int g;
        int b;
    } voxel;

    /* Write data one voxel at a time. */
    for (i = 0; i < NDIMS; i++) {
        count[i] = 1;
    }

    result = micreate_dimension("xspace", MI_DIMCLASS_SPATIAL, 
                           MI_DIMATTR_REGULARLY_SAMPLED, CX, &hdim[0]);

    result = micreate_dimension("yspace", MI_DIMCLASS_SPATIAL, 
                           MI_DIMATTR_REGULARLY_SAMPLED, CY, &hdim[1]);

    result = micreate_dimension("zspace", MI_DIMCLASS_SPATIAL, 
                           MI_DIMATTR_REGULARLY_SAMPLED, CZ, &hdim[2]);

    result = micreate_volume("tst-rec.mnc", NDIMS, hdim, MI_TYPE_UINT, 
                             MI_CLASS_UNIFORM_RECORD, NULL, &hvol);
    if (result < 0) {
	TESTRPT("Unable to create test file", result);
    }

    result = miset_record_field_name(hvol, 0, "Red");
    if (result < 0) {
        TESTRPT("miset_record_field_name", result);
    }
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

    result = micreate_volume_image(hvol);
    if (result < 0) {
        TESTRPT("micreate_volume_image failed", result);
    }

    for (i = 0; i < CX; i++) {
        for (j = 0; j < CY; j++) {
            for (k = 0; k < CZ; k++) {
                coords[0] = i;
                coords[1] = j;
                coords[2] = k;

                voxel.r = i;
                voxel.g = j;
                voxel.b = k;
                
                result = miset_voxel_value_hyperslab(hvol, MI_TYPE_UNKNOWN,
                                                     coords, count, &voxel);
                if (result < 0) {
                    TESTRPT("Error writing voxel", result);
                }
            }
        }
    }

    for (i = 0; i < CX; i++) {
        for (j = 0; j < CY; j++) {
            for (k = 0; k < CZ; k++) {
                coords[0] = i;
                coords[1] = j;
                coords[2] = k;

                result = miget_voxel_value_hyperslab(hvol, MI_TYPE_UNKNOWN,
                                                     coords, count, &voxel);
                if (result < 0) {
                    TESTRPT("Error reading voxel", result);
                }
                if (voxel.r != i || voxel.g != j || voxel.b != k) {
                    TESTRPT("Data mismatch", 0);
                }
            }
        }
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

#endif /* M2_TEST */
