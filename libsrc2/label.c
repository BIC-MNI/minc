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

int miswap2(unsigned short tmp)
{
    unsigned char *x = (unsigned char *) &tmp;
    unsigned char t = x[0];
    x[0] = x[1];
    x[1] = t;
    return (tmp);
}

int miswap4(unsigned int tmp)
{
    unsigned char *x = (unsigned char *) &tmp;
    unsigned char t = x[0];
    x[0] = x[3];
    x[3] = t;
    t = x[1];
    x[1] = x[2];
    x[2] = t;
    return (tmp);
}

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

    if (volume->ftype_id <= 0 || volume->mtype_id <= 0) {
	return (MI_ERROR);
    }

    result = H5Tenum_insert(volume->mtype_id, name, &value);
    if (result < 0) {
	return (MI_ERROR);
    }

    /* We might have to swap these values before adding them to
     * the file type.
     */
    if (H5Tget_order(volume->ftype_id) != H5Tget_order(volume->mtype_id)) {
        switch (H5Tget_size(volume->ftype_id)) {
        case 2:
            value = miswap2((unsigned short) value);
            break;
        case 4:
            value = miswap4((unsigned int) value);
            break;
        }
    }
    result = H5Tenum_insert(volume->ftype_id, name, &value);
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
    if (volume->mtype_id <= 0) {
        return (MI_ERROR);
    }
    *name = malloc(MI_LABEL_MAX);
    if (*name == NULL) {
        return (MI_ERROR);
    }

    H5E_BEGIN_TRY {
        result = H5Tenum_nameof(volume->mtype_id, &value, *name, MI_LABEL_MAX);
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

    if (volume->mtype_id <= 0) {
        return (MI_ERROR);
    }

    H5E_BEGIN_TRY {
        result = H5Tenum_valueof(volume->mtype_id, name, value_ptr);
    } H5E_END_TRY;

    if (result < 0) {
	return (MI_ERROR);
    }
    return (MI_NOERROR);
}

/**
This function returns the number of defined labels, if any, or zero.
*/
int
miget_number_of_defined_labels(mihandle_t volume, int *number_of_labels)
{
  int result;
 
  if (volume == NULL) {
    return (MI_ERROR);
  }
  if (volume->volume_class != MI_CLASS_LABEL) {
    return (MI_ERROR);
  }

  if (volume->mtype_id <= 0) {
    return (MI_ERROR);
  }

  H5E_BEGIN_TRY {
    result = H5Tget_nmembers(volume->mtype_id);
  } H5E_END_TRY;

  if (result < 0) {
    return (MI_ERROR);
  }
  else {
    *number_of_labels = result;
  }
    
  return (MI_NOERROR);
}
