/**
 * \file label.c
 * \brief MINC 2.0 Label functions
 * \author Bert Vincent
 *
 * This small set of three functions are intended to allow for the
 * definition of labeled, or enumerated, volumes.
 * 
 * Labeled volumes must have been created with the class MI_CLASS_LABEL,
 * and with any integer subtype.
 *
 ************************************************************************/

#include <stdlib.h>
#include <hdf5.h>
#include "minc2.h"
#include "minc2_private.h"

#define MI_LABEL_MAX 128

/**
This function associates a label name with an integer value for the given
volume. Functions which read and write voxel values will read/write 
in integer values, and must call miget_label_name() to discover the 
descriptive text string which corresponds to the integer value.
*/
int 
midefine_label(mihandle_t volume, int value, const char *name)
{
    int result;

    if (volume == NULL || name == NULL) {
        return (MI_ERROR);
    }

    if (strlen(name) > MI_LABEL_MAX) {
        return (MI_ERROR);
    }

    if (volume->volume_class != MI_CLASS_LABEL) {
	return (MI_ERROR);
    }

    if (volume->type_id <= 0) {
	return (MI_ERROR);
    }

    result = H5Tenum_insert(volume->type_id, name, &value);
    if (result < 0) {
	return (MI_ERROR);
    }

    return (MI_NOERROR);
}

/**
For a labelled volume, this function retrieves the text name
associated with a given integer value.

The name pointer returned must be freed by calling mifree_name().
*/
int
miget_label_name(mihandle_t volume, int value, char **name)
{
    int result;

    if (volume == NULL || name == NULL) {
        return (MI_ERROR);
    }

    if (volume->volume_class != MI_CLASS_LABEL) {
        return (MI_ERROR);
    }
    if (volume->type_id <= 0) {
        return (MI_ERROR);
    }
    *name = malloc(MI_LABEL_MAX);
    if (*name == NULL) {
        return (MI_ERROR);
    }

    H5E_BEGIN_TRY {
        result = H5Tenum_nameof(volume->type_id, &value, *name, MI_LABEL_MAX);
    } H5E_END_TRY;

    if (result < 0) {
	return (MI_ERROR);
    }
    return (MI_NOERROR);
}

/**
This function is the inverse of miget_label_name(). It is called to determine
what integer value, if any, corresponds to the given text string.
*/
int
miget_label_value(mihandle_t volume, const char *name, int *value_ptr)
{
    int result;

    if (volume == NULL || name == NULL || value_ptr == NULL) {
        return (MI_ERROR);
    }

    if (volume->volume_class != MI_CLASS_LABEL) {
        return (MI_ERROR);
    }

    if (volume->type_id <= 0) {
        return (MI_ERROR);
    }

    H5E_BEGIN_TRY {
        result = H5Tenum_valueof(volume->type_id, name, value_ptr);
    } H5E_END_TRY;

    if (result < 0) {
	return (MI_ERROR);
    }
    return (MI_NOERROR);
}

