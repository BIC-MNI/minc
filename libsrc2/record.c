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

