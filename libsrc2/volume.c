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
  int stat;
  hid_t file_id;
  hid_t hdf_attr, hdf_type;
  hid_t hdf_plist;
  hsize_t dim[1];
  hid_t grp_root_id, grp_image_id;
  hid_t grp_fullimage_id, grp_dimensions_id; 

  hid_t dimage_id = -1;
  hid_t dataset_id = -1;
  hid_t dataspace_id = -1;
  
  hsize_t hdf_chunk_size[MI2_MAX_BLOCK_EDGES];
  volumehandle *handle;
  volprops *props_handle;

  if (filename == NULL || number_of_dimensions <=0 || 
      dimensions == NULL || create_props == NULL) {
    return (MI_ERROR);
  }    
  /* convert minc type to hdf type
   */
  hdf_type = mitype_to_hdftype(volume_type);

  /* Create file in HDF5 with the given filename and
     H5F_AA_TRUNC: Truncate file, if it already exists, 
                   erasing all data previously stored in the file.
     and create ID and ID access as default.
  */
  file_id = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (file_id < 0) {
    return (MI_ERROR);
  }
  
  /* Try opening ROOT GROUP i.e. /minc-2.0 or CREATE ONE!
   */
  if ((grp_root_id = H5Gopen(file_id, MI_ROOT_PATH)) < 0) {
    grp_root_id = H5Gcreate(file_id, MI_ROOT_PATH , 0);
    if (grp_root_id < 0) {
      return (MI_ERROR);
    }
  }

  /* Try opening IMAGE GROUP i.e. /minc-2.0/image/ or CREATE ONE!
   */
  if((grp_image_id = H5Gopen(grp_root_id, MI_IMAGE_PATH)) < 0) {
    grp_image_id = H5Gcreate(grp_root_id, MI_IMAGE_PATH , 0);
    if (grp_image_id < 0) {
      return (MI_ERROR);
    }
  }

  /* Try opening FULLIMAGE GROUP i.e. /minc-2.0/image/0 or CREATE ONE!
   */
  if((grp_fullimage_id = H5Gopen(grp_image_id, MI_FULLIMAGE_PATH)) < 0) {
    grp_fullimage_id = H5Gcreate(grp_image_id, MI_FULLIMAGE_PATH , 0);
    if (grp_fullimage_id < 0) {
      return (MI_ERROR);
    }
  }
 
  /* Create dataset property list */
  hdf_plist = H5Pcreate(H5P_DATASET_CREATE);
  if (hdf_plist < 0) {
    return (MI_ERROR);
  }
  
  /* See if chunking and/or compression should be enabled */
  if (create_props->compression_type == MI_COMPRESS_ZLIB ||
      create_props->edge_count > 1) {

    stat = H5Pset_layout(hdf_plist, H5D_CHUNKED); /* Chunked data */
    if (stat < 0) {
      return (MI_ERROR);
    }
    
    for (i=0; i < create_props->edge_count; i++) {
      hdf_chunk_size[i] = create_props->edge_lengths[i];
    }
    
    /* Sets the size of the chunks used to store a chunked layout dataset */
    stat = H5Pset_chunk(hdf_plist, create_props->edge_count, hdf_chunk_size);
    if (stat < 0) {
      return (MI_ERROR);
    }
    /* Sets compression method and compression level */
    stat = H5Pset_deflate(hdf_plist, create_props->zlib_level);
    if (stat < 0) {
      return (MI_ERROR);
    }

     /* Create a SIMPLE dataspace  */
    dataspace_id = H5Screate_simple(create_props->edge_count, hdf_chunk_size, NULL);			  
    if (dataspace_id < 0) {
    return (MI_ERROR);
    }
  }
  else { /* No COMPRESSION or CHUNKING is enabled */
    stat = H5Pset_layout(hdf_plist, H5D_CONTIGUOUS); /*  CONTIGUOUS data */
    if (stat < 0) {
      return (MI_ERROR);
    }
    /* Create a SCALAR dataspace  */
    dataspace_id = H5Screate(H5S_SCALAR);
    if (dataspace_id < 0) {
    return (MI_ERROR);
    }
  }
    
  /* Try opening IMAGE dataset i.e. /minc-2.0/image/0/image or CREATE ONE!
   */
  if((dimage_id = H5Dopen(grp_fullimage_id, MI_DIMAGE_PATH)) < 0) {
    dimage_id = H5Dcreate(grp_fullimage_id, MI_DIMAGE_PATH, hdf_type, dataspace_id, hdf_plist);
    if (dimage_id < 0) {  
      return (MI_ERROR);
    }
  }
  
   /* Try opening DIMENSIONS GROUP i.e. /minc-2.0/dimensions or CREATE ONE!
   */
  if((grp_dimensions_id = H5Gopen(grp_root_id, MI_FULLDIMENSIONS_PATH)) < 0) {
    grp_dimensions_id = H5Gcreate(grp_root_id, MI_FULLDIMENSIONS_PATH , 0);
    if (grp_dimensions_id < 0) {
      return (MI_ERROR);
    }
  }
   
  for (i=0; i < number_of_dimensions ; i++) {
    
    if (dimensions[i]->attr == MI_DIMATTR_REGULARLY_SAMPLED) {
      /* Dimension variable for a regular dimension contains
	 no meaningful data.
      */
      dataspace_id = H5Screate(H5S_SCALAR);
    }
    else {
      /* Dimension variable for an irregular dimension contains
	 a vector with the lengths equal to the sampled points
	 along the dimension.
      */
      dim[0] = dimensions[i]->length;
      dataspace_id = H5Screate_simple(1, dim, NULL);
    }
    if (dataspace_id < 0) {
      return (MI_ERROR);
    }
  
    /* Create a dataset(dimension's name) in DIMENSIONS GROUP */
    dataset_id = H5Dcreate(grp_dimensions_id, dimensions[i]->name, hdf_type, dataspace_id, H5P_DEFAULT);
    /* Create Dimension attribute  "attr" */
   dataspace_id = H5Screate(H5S_SCALAR);
   /* Create attribute. */
   hdf_attr = H5Acreate(dataset_id, "attr", H5T_NATIVE_INT, dataspace_id, H5P_DEFAULT);
   if (hdf_attr < 0) {
     return (MI_ERROR);
   }
  
   /* Write data to the attribute. */
   H5Awrite(hdf_attr, H5T_NATIVE_INT, &dimensions[i]->attr);
   /* Close attribute dataspace. */
   H5Sclose(dataspace_id); 
   /* Close attribute. */
   H5Aclose(hdf_attr);

   /* Create Dimension attribute  "class" */
   dataspace_id = H5Screate(H5S_SCALAR);
   /* Create attribute. */
   hdf_attr = H5Acreate(dataset_id, "class", H5T_NATIVE_INT, dataspace_id, H5P_DEFAULT);
   if (hdf_attr < 0) {
     return (MI_ERROR);
   }
   /* Write data to the attribute. */
   H5Awrite(hdf_attr, H5T_NATIVE_INT, &dimensions[i]->class);
   /* Close attribute dataspace. */
   H5Sclose(dataspace_id); 
   /* Close attribute. */
   H5Aclose(hdf_attr);
   
   /* Create Dimension attribute "direction_cosines"  */
   dim[0] = 3;
   dataspace_id = H5Screate_simple(1, dim, NULL);
   hdf_attr = H5Acreate(dataset_id, "direction_cosines", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT);
   if (hdf_attr < 0) {
     return (MI_ERROR);
   }
   H5Awrite(hdf_attr, H5T_NATIVE_DOUBLE, dimensions[i]->direction_cosines);
   /* Close attribute dataspace. */
   H5Sclose(dataspace_id); 
   /* Close attribute. */
   H5Aclose(hdf_attr);
   
   /* Create Dimension flipping_order attribute */
   dataspace_id = H5Screate(H5S_SCALAR);
   hdf_attr = H5Acreate(dataset_id, "flipping_order", H5T_NATIVE_INT, dataspace_id, H5P_DEFAULT);
   if (hdf_attr < 0) {
     return (MI_ERROR);
   }
   H5Awrite(hdf_attr, H5T_NATIVE_INT, &dimensions[i]->flipping_order);
   /* Close attribute dataspace. */
   H5Sclose(dataspace_id); 
   /* Close attribute. */
   H5Aclose(hdf_attr);

   if (dimensions[i]->offsets != NULL) {
     /* Create Dimension attribute "offsets"  */
     dim[0] = dimensions[i]->length;
     dataspace_id = H5Screate_simple(1, dim, NULL);
     hdf_attr = H5Acreate(dataset_id, "offsets", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT);
     if (hdf_attr < 0) {
       return (MI_ERROR);
     }
     H5Awrite(hdf_attr, H5T_NATIVE_DOUBLE, dimensions[i]->offsets);
     /* Close attribute dataspace. */
     H5Sclose(dataspace_id); 
     /* Close attribute. */
     H5Aclose(hdf_attr);
   }
   
   /* Create Dimension attribute "sampling_flag" */
   dataspace_id = H5Screate(H5S_SCALAR);
   hdf_attr = H5Acreate(dataset_id, "sampling_flag", H5T_NATIVE_INT, dataspace_id, H5P_DEFAULT);
   if (hdf_attr < 0) {
     return (MI_ERROR);
   }
   H5Awrite(hdf_attr, H5T_NATIVE_INT, &dimensions[i]->sampling_flag);
   /* Close attribute dataspace. */
   H5Sclose(dataspace_id); 
   /* Close attribute. */
   H5Aclose(hdf_attr);

   /* Create Dimension attribute  "length" */
   dataspace_id = H5Screate(H5S_SCALAR);
   hdf_attr = H5Acreate(dataset_id, "length", H5T_NATIVE_ULONG, dataspace_id, H5P_DEFAULT);
   if (hdf_attr < 0) {
     return (MI_ERROR);
   }
   H5Awrite(hdf_attr, H5T_NATIVE_ULONG, &dimensions[i]->length);
   /* Close attribute dataspace. */
   H5Sclose(dataspace_id); 
   /* Close attribute. */
   H5Aclose(hdf_attr);
   
   /* Create Dimension attribute "step" */
   dataspace_id = H5Screate(H5S_SCALAR);
   hdf_attr = H5Acreate(dataset_id, "step", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT);
   if (hdf_attr < 0) {
	return (MI_ERROR);
   }
   H5Awrite(hdf_attr, H5T_NATIVE_DOUBLE, &dimensions[i]->step);
   /* Close attribute dataspace. */
   H5Sclose(dataspace_id); 
   /* Close attribute. */
   H5Aclose(hdf_attr);
   
   /* Create Dimension start attribute */
   dataspace_id = H5Screate(H5S_SCALAR);
   hdf_attr = H5Acreate(dataset_id, "start", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT);
   if (hdf_attr < 0) {
     return (MI_ERROR);
   }
   H5Awrite(hdf_attr, H5T_NATIVE_DOUBLE, &dimensions[i]->start);
   /* Close attribute dataspace. */
   H5Sclose(dataspace_id); 
   /* Close attribute. */
   H5Aclose(hdf_attr);
   
   /* Create Dimension attribute "units" */
   dataspace_id = H5Screate(H5S_SCALAR);
   hdf_type = H5Tcopy(H5T_C_S1);
              H5Tset_size(hdf_type, (strlen(dimensions[i]->units) + 1));
   hdf_attr = H5Acreate(dataset_id, "units", hdf_type, dataspace_id, H5P_DEFAULT);
   if (hdf_attr < 0) {
     return (MI_ERROR);
   }
   
   H5Awrite(hdf_attr, hdf_type, dimensions[i]->units);
   /* Close attribute dataspace. */
   H5Sclose(dataspace_id); 
   /* Close attribute. */
   H5Aclose(hdf_attr);
   
   /* Create Dimension attribute "width" */
   dataspace_id = H5Screate(H5S_SCALAR);
   hdf_attr = H5Acreate(dataset_id, "width", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT);
   if (hdf_attr < 0) {
     return (MI_ERROR);
   }
   H5Awrite(hdf_attr, H5T_NATIVE_DOUBLE, &dimensions[i]->width);
   /* Close attribute dataspace. */
   H5Sclose(dataspace_id); 
   /* Close attribute. */
   H5Aclose(hdf_attr);
   if (dimensions[i]->widths != NULL) {
     /* Create Dimension attribute "widths" */
     dim[0] = dimensions[i]->length;
     dataspace_id = H5Screate_simple(1, dim, NULL);
     hdf_attr = H5Acreate(dataset_id, "widths", H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT);
     if (hdf_attr < 0) {
       return (MI_ERROR);
     }
     H5Awrite(hdf_attr, H5T_NATIVE_DOUBLE, dimensions[i]->widths);
     /* Close attribute dataspace. */
     H5Sclose(dataspace_id); 
     /* Close attribute. */
     H5Aclose(hdf_attr);
   }
  }
  /* "mitype_to_hdftype" returns a copy of the datatype, so the returned value 
     must be explicitly freed with a call to H5Tclose().
     Close all Groups and Datset.
   */
  H5Tclose(hdf_type);
  H5Sclose(dataspace_id);
  H5Dclose(dimage_id);
  H5Gclose(grp_dimensions_id);
  H5Pclose(hdf_plist);
  H5Gclose(grp_fullimage_id);
  H5Gclose(grp_image_id);
  H5Gclose(grp_root_id);
  

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

  props_handle->edge_lengths = (int *)malloc(create_props->max_lengths*sizeof(int));
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






