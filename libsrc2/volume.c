/************************************************************************
 * MINC 2.0 VOLUME FUNCTIONS
 ************************************************************************/

#include <stdlib.h>
#include <hdf5.h>
#include "minc2.h"
#include "minc2_private.h"

/* Create a volume with the specified properties.
 */ 

int
micreate_volume(const char *filename, int number_of_dimensions,
		midimhandle_t dimensions[], mitype_t volume_type,
		miclass_t volume_class, mivolumeprops_t create_props,
		mihandle_t *volume)
{
  int i;
  hid_t file_id; 
  hid_t g1_id; 
  volumehandle *handle;
  volprops *props_handle;

  if (filename == NULL || number_of_dimensions <=0 || 
      dimensions == NULL || create_props == NULL) {
    return (MI_ERROR);
  }      
  /* Create file in HDF5 with the given filename and
     H5F_AA_TRUNC: Truncate file, if it already exists, 
                   erasing all data previously stored in the file.
     and create ID and ID access as default.
  */
  file_id = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (file_id < 0) {
    return (MI_ERROR);
  }
  /* Create minc-2.0 under the ROOT group
   */
  g1_id = H5Gcreate(file_id, "/minc-2.0", 0);
  if (g1_id < 0) {
    return (MI_ERROR);
  }
  H5Gclose(g1_id);
  
  /* Allocate space for the volume handle
   */
  handle = (volumehandle *)malloc(sizeof(*handle));
  if (handle == NULL) {
    return (MI_ERROR);
  }
  handle->hdf_id = file_id;
  handle->has_slice_scaling = FALSE;
  handle->number_of_dims = number_of_dimensions;
  handle->dim_handles = (midimhandle_t *)malloc(number_of_dimensions*sizeof(int));
  if (handle->dim_handles == NULL) {
    return (MI_ERROR);
  }
  for (i=0; i<number_of_dimensions ; i++) {
    handle->dim_handles[i] = dimensions[i];  
  }
  /* Set the apparent order of dimensions to NULL
     until user defines them. Switch is used to 
     avoid errors.
   */
  handle->dim_indices = NULL;
  switch (volume_type) {
  case MI_TYPE_BYTE:
    handle->volume_type = MI_TYPE_BYTE;
    break;
  case MI_TYPE_CHAR:
    handle->volume_type = MI_TYPE_CHAR;
    break;
  case MI_TYPE_SHORT:
    handle->volume_type = MI_TYPE_SHORT;
    break;
  case MI_TYPE_INT:
    handle->volume_type = MI_TYPE_INT;
    break;
  case MI_TYPE_FLOAT:
    handle->volume_type = MI_TYPE_FLOAT;
    break;
  case MI_TYPE_DOUBLE:
    handle->volume_type = MI_TYPE_DOUBLE;
    break;
  case MI_TYPE_STRING:
    handle->volume_type = MI_TYPE_STRING;
    break;
  case MI_TYPE_UBYTE:
    handle->volume_type = MI_TYPE_UBYTE;
    break;
  case MI_TYPE_USHORT:
    handle->volume_type = MI_TYPE_USHORT;
    break;
  case MI_TYPE_UINT:
    handle->volume_type = MI_TYPE_UINT;
    break;
  case MI_TYPE_SCOMPLEX:
    handle->volume_type = MI_TYPE_SCOMPLEX;
    break;
  case MI_TYPE_ICOMPLEX:
    handle->volume_type = MI_TYPE_ICOMPLEX;
    break;
  case MI_TYPE_FCOMPLEX:
    handle->volume_type = MI_TYPE_FCOMPLEX;
    break;
  case MI_TYPE_DCOMPLEX:
    handle->volume_type = MI_TYPE_DCOMPLEX;
    break;
  case MI_TYPE_UNKNOWN:
    handle->volume_type = MI_TYPE_UNKNOWN;
    break;
  default:
    return (MI_ERROR);
  }
  switch (volume_class) {
  case MI_CLASS_REAL:
    handle->volume_class = MI_CLASS_REAL;
    break;
  case MI_CLASS_INT:
    handle->volume_class = MI_CLASS_INT;
    break;
  case MI_CLASS_LABEL:
    handle->volume_class = MI_CLASS_LABEL;
    break;
  case MI_CLASS_COMPLEX:
    handle->volume_class = MI_CLASS_COMPLEX;
    break;
  case MI_CLASS_UNIFORM_RECORD:
    handle->volume_class = MI_CLASS_UNIFORM_RECORD;
    break;
  case MI_CLASS_NON_UNIFORM_RECORD:
    handle->volume_class = MI_CLASS_NON_UNIFORM_RECORD;
    break;
  default:
    return (MI_ERROR);
  }
  
  props_handle = (volprops *)malloc(sizeof(*props_handle));
  
  props_handle->enable_flag = create_props->enable_flag;
  
  props_handle->depth = create_props->depth;
  switch (create_props->compression_type) {
  case MI_COMPRESS_NONE:
    props_handle->compression_type = MI_COMPRESS_NONE;
    break;
  case MI_COMPRESS_ZLIB:
    props_handle->compression_type = MI_COMPRESS_ZLIB;
    break;
  default:
    return (MI_ERROR);
  }
  
  props_handle->zlib_level = create_props->zlib_level;
  props_handle->edge_count = create_props->edge_count;

  props_handle->edge_lengths = (int *)malloc(sizeof(create_props->max_lengths));
  for (i=0;i<create_props->max_lengths; i++) {
    props_handle->edge_lengths[i] = create_props->edge_lengths[i];
  }
 
  props_handle->max_lengths = create_props->max_lengths;
  props_handle->record_length = create_props->record_length;
  /* Explicitly allocate storage for name
   */
  if (create_props->record_name != NULL) {
    props_handle->record_name =malloc(strlen(create_props->record_name) + 1 );
    strcpy(props_handle->record_name, create_props->record_name);
  }
  props_handle->template_flag = create_props->template_flag;
  
  handle->create_props = props_handle;
  
  *volume = handle;

  return (MI_NOERROR);
}

int
miget_volume_dimension_count(mihandle_t volume, midimclass_t class,
			     midimattr_t attr, int *number_of_dimensions)
{
  int i, count=0;
  
  if (volume == NULL) {
    return (MI_ERROR);
  }
  for (i=0; i< volume->number_of_dims; i++) {
   if (volume->dim_handles[i]->class == class && 
      (attr == MI_DIMATTR_ALL || volume->dim_handles[i]->attr == attr)) {
      count++;     
      }
  }

  *number_of_dimensions = count;
  return (MI_NOERROR);
}

int
miopen_volume(const char *filename, int mode, mihandle_t *volume)
{
    hid_t file_id;
    hid_t dset_id;
    hid_t space_id;
    volumehandle *handle;
    int hdf_mode;

    if (mode == MI2_OPEN_READ) {
        hdf_mode = H5F_ACC_RDONLY;
    }
    else if (mode == MI2_OPEN_RDWR) {
        hdf_mode = H5F_ACC_RDWR;
    }
    else {
        return (MI_ERROR);
    }
    file_id = H5Fopen(filename, hdf_mode, H5P_DEFAULT);
    if (file_id < 0) {
	return (MI_ERROR);
    }

    /* SEE IF SLICE SCALING IS ENABLED
     */
    handle = (volumehandle *)malloc(sizeof(*handle));
    if (handle == NULL) {
      return (MI_ERROR);
    }
    

    handle->hdf_id = file_id;

    handle->has_slice_scaling = FALSE;
    dset_id = midescend_path(file_id, "/minc-2.0/image/0/image-max");
    if (dset_id >= 0) {
	space_id = H5Dget_space(dset_id);
	if (space_id >= 0) {
	    /* If the dimensionality of the image-max variable is one or
	     * greater, we consider this volume to have slice-scaling enabled.
	     */
	    if (H5Sget_simple_extent_ndims(space_id) >= 1) {
		handle->has_slice_scaling = TRUE;
	    }
	    H5Sclose(space_id);	/* Close the dataspace handle */
	}
	H5Dclose(dset_id);	/* Close the dataset handle */
    }
    *volume = handle;
    return (MI_NOERROR);
}

int 
miclose_volume(mihandle_t volume)
{
  
    if (volume == NULL || H5Fclose(volume->hdf_id) < 0) {
      return (MI_ERROR);
    }
    
    free(volume->dim_handles);
    free(volume->dim_indices);
    if (volume->create_props != NULL) {
      free(volume->create_props->edge_lengths);
      free(volume->create_props->record_name);
      free(volume->create_props);
    }
    free(volume);

    return (MI_NOERROR);
}

//***************************** must move the following to the slice
//functions section

/*! Function to get the volume's slice-scaling flag.
 */
int
miget_slice_scaling_flag(mihandle_t volume, BOOLEAN *slice_scaling_flag)
{
    if (volume == NULL || slice_scaling_flag == NULL) {
	return (MI_ERROR);
    }
    *slice_scaling_flag = volume->has_slice_scaling;
    return (MI_NOERROR);
}

/*! Function to set the volume's slice-scaling flag.
 */
int
miset_slice_scaling_flag(mihandle_t volume, BOOLEAN slice_scaling_flag)
{
    if (volume == NULL) {
	return (MI_ERROR);
    }
    volume->has_slice_scaling = slice_scaling_flag;
    return (MI_NOERROR);
}





