/** \file volume.c
 * \brief MINC 2.0 Volume Functions
 * \author Leila Baghdadi, Bert Vincent
 *
 * Functions to create, open, and close MINC volume objects.
 ************************************************************************/

#include <stdlib.h>
#include <hdf5.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include "minc2.h"
#include "minc2_private.h"

/* Forward declarations */

static void miinit_default_range(mitype_t mitype, double *valid_max, 
                                 double *valid_min);
static void miread_valid_range(mihandle_t volume, double *valid_max, 
                               double *valid_min);

static int _miset_volume_class(mihandle_t volume, miclass_t volclass);
static int _miget_volume_class(mihandle_t volume, miclass_t *volclass);

/**
 */
int
micreate_volume_image(mihandle_t volume)
{
    char dimorder[MI2_CHAR_LENGTH];
    int i;
    hid_t dataspace_id;
    hid_t dset_id;
    hsize_t hdf_size[MI2_MAX_VAR_DIMS];

    /* Try creating IMAGE dataset i.e. /minc-2.0/image/0/image
     */

    dimorder[0] = '\0'; /* Set string to empty */
    
    for (i = 0; i < volume->number_of_dims; i++) {
        hdf_size[i] = volume->dim_handles[i]->length;

        /* Create the dimorder string, ordered comma-separated
           list of dimension names.
        */
        strcat(dimorder, volume->dim_handles[i]->name);
        if (i != volume->number_of_dims - 1) {
            strcat(dimorder, ",");
        }
    }
    
   
    /* Create a SIMPLE dataspace  */
    dataspace_id = H5Screate_simple(volume->number_of_dims, hdf_size, NULL);
    if (dataspace_id < 0) {
        return (MI_ERROR);
    }
    
    dset_id = H5Dcreate(volume->hdf_id, "/minc-2.0/image/0/image", 
                        volume->type_id, 
                        dataspace_id, volume->plist_id);
    
    if (dset_id < 0) {  
        return (MI_ERROR);
    }

    volume->image_id = dset_id;
    
    hdf_var_declare(volume->hdf_id, "image", "/minc-2.0/image/0/image",
                    volume->number_of_dims, hdf_size);
    /* Create the dimorder attribute, ordered comma-separated
       list of dimension names.
    */
    miset_attr_at_loc(dset_id, "dimorder", MI_TYPE_STRING,
                      strlen(dimorder), dimorder);

    H5Sclose(dataspace_id);
    
    if (volume->volume_class == MI_CLASS_REAL) {
        int ndims;
        if (volume->has_slice_scaling) {
            /* TODO: Find the slowest-varying spatial dimension; that forms
             * the basis for the image-min and image-max variables.  Right
             * now this is an oversimplification!
             */
            ndims = volume->number_of_dims - 2;
            dataspace_id = H5Screate_simple(ndims, hdf_size, NULL);
        }
        else {
            ndims = 0;
            dataspace_id = H5Screate(H5S_SCALAR);
        }

        if (ndims != 0) {
            dimorder[0] = '\0'; /* Set string to empty */
            for (i = 0; i < ndims; i++) {
                /* Create the dimorder string, ordered comma-separated
                   list of dimension names.
                */
                strcat(dimorder, volume->dim_handles[i]->name);
                if (i != volume->number_of_dims - 1) {
                    strcat(dimorder, ",");
                }
            }
        }

        dset_id = H5Dcreate(volume->hdf_id, "/minc-2.0/image/0/image-min",
                            H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT);
        if (ndims != 0) {
            miset_attr_at_loc(dset_id, "dimorder", MI_TYPE_STRING,
                              strlen(dimorder), dimorder);
        }
        volume->imin_id = dset_id;
        hdf_var_declare(volume->hdf_id, "image-min", "/minc-2.0/image/0/image-min", ndims, hdf_size);
        dset_id = H5Dcreate(volume->hdf_id, "/minc-2.0/image/0/image-max",
                            H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT);
        if (ndims != 0) {
            miset_attr_at_loc(dset_id, "dimorder", MI_TYPE_STRING,
                              strlen(dimorder), dimorder);
        }
        volume->imax_id = dset_id;
        hdf_var_declare(volume->hdf_id, "image-max", "/minc-2.0/image/0/image-max", ndims, hdf_size);
        H5Sclose(dataspace_id);
	
    }

    return (MI_NOERROR);
}

/** Create a volume with the specified properties.
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
  hid_t hdf_type;
  hid_t hdf_plist;
  hid_t fspc_id;
  hsize_t dim[1];
  hid_t grp_id; 
  herr_t status;
  hid_t dataset_id = -1;
  hid_t dataset_width = -1;
  hid_t dataspace_id = -1;
  char *name;
  int size;
  hsize_t hdf_size[MI2_MAX_VAR_DIMS];
  volumehandle *handle;
  volprops *props_handle;

  miinit();

  if (filename == NULL) {
    return (MI_ERROR);
  }

  if (dimensions == NULL && number_of_dimensions != 0) {
      return (MI_ERROR);
  }

  /* Allocate space for the volume handle
   */
  handle = (volumehandle *)malloc(sizeof(*handle));
  if (handle == NULL) {
    return (MI_ERROR);
  }

  /* convert minc type to hdf type
   */
  hdf_type = mitype_to_hdftype(volume_type);

  /* Setting up volume type_id 
   */
  switch (volume_class) {
  case MI_CLASS_REAL:
      handle->type_id = hdf_type;
      break;
  case MI_CLASS_INT:
      handle->type_id = hdf_type;
      break;
  case MI_CLASS_LABEL:
      /* A volume of class LABEL must have an integer type.
       */
      switch (volume_type) {
      case MI_TYPE_BYTE:
      case MI_TYPE_SHORT:
      case MI_TYPE_INT:
      case MI_TYPE_UBYTE:
      case MI_TYPE_USHORT:
      case MI_TYPE_UINT:
          handle->type_id = H5Tenum_create(hdf_type);
          if (handle->type_id < 0) {
              return (MI_ERROR);
          }
          H5Tclose(hdf_type);
          hdf_type = handle->type_id;
          miinit_enum(hdf_type);
          break;
      default:
          return (MI_ERROR);
      }
      break;

  case MI_CLASS_COMPLEX:
      switch (volume_type) {
      case MI_TYPE_SCOMPLEX:
      case MI_TYPE_ICOMPLEX:
      case MI_TYPE_FCOMPLEX:
      case MI_TYPE_DCOMPLEX:
          handle->type_id = hdf_type;
          break;
      default:
          return (MI_ERROR);
      }
      break;

  case MI_CLASS_UNIFORM_RECORD:
      handle->type_id = H5Tcreate(H5T_COMPOUND, H5Tget_size(hdf_type));
      H5Tclose(hdf_type);
      break;

  default:
      return (MI_ERROR);
  }

  handle->volume_class = volume_class;

  /* Create file in HDF5 with the given filename and
     H5F_ACC_TRUNC: Truncate file, if it already exists, 
                   erasing all data previously stored in the file.
     and create ID and ID access as default.
  */
  file_id = hdf_create(filename, H5F_ACC_TRUNC);
  if (file_id < 0) {
    return (MI_ERROR);
  }

  handle->hdf_id = file_id;

  _miset_volume_class(handle, handle->volume_class);

  hdf_plist = H5Pcreate(H5P_DATASET_CREATE);
  if (hdf_plist < 0) {
    return (MI_ERROR);
  }

  handle->plist_id = hdf_plist;

  /* Set fill value to guarantee valid data on incomplete datasets.
   */
  {
    char *tmp = calloc(1, H5Tget_size(handle->type_id));
    H5Pset_fill_value(hdf_plist, handle->type_id, tmp);
    free(tmp);
  }
  
  /* See if chunking and/or compression should be enabled */
  if (create_props != NULL &&
      (create_props->compression_type == MI_COMPRESS_ZLIB ||
       create_props->edge_count != 0)) {
      stat = H5Pset_layout(hdf_plist, H5D_CHUNKED); /* Chunked data */
      if (stat < 0) {
          return (MI_ERROR);
      }
      
      for (i=0; i < number_of_dimensions; i++) {
          hdf_size[i] = create_props->edge_lengths[i];
	  
          if (hdf_size[i] > dimensions[i]->length) {
              hdf_size[i] = dimensions[i]->length;
          }
      }
    
      /* Sets the size of the chunks used to store a chunked layout dataset */
      stat = H5Pset_chunk(hdf_plist, number_of_dimensions, hdf_size);
      if (stat < 0) {
          return (MI_ERROR);
      }
      /* Sets compression method and compression level */
      stat = H5Pset_deflate(hdf_plist, create_props->zlib_level);
      if (stat < 0) {
          return (MI_ERROR);
      }
  }
  else { /* No COMPRESSION or CHUNKING is enabled */
    stat = H5Pset_layout(hdf_plist, H5D_CONTIGUOUS); /*  CONTIGUOUS data */
    if (stat < 0) {
      return (MI_ERROR);
    }
  }
  
  /* See if Multi-res is set to a level above 0 and if yes create subgroups*/
  // must add some code to make sure that the res level is possible
  if (create_props != NULL && create_props->depth > 0) {
    for (i=0; i < create_props->depth ; i++) {
      if (minc_create_thumbnail(file_id, i+1) < 0) {
	return (MI_ERROR);
      }
    }
  }

  /* Try creating DIMENSIONS GROUP i.e. /minc-2.0/dimensions
   */
  grp_id = H5Gopen(file_id, MI_FULLDIMENSIONS_PATH);
  if (grp_id < 0) {
      return (MI_ERROR);
  }
  
  for (i=0; i < number_of_dimensions ; i++) {
    
    /* First create the dataspace required to create a 
       dimension variable (dataset)
     */
    if (dimensions[i]->attr & MI_DIMATTR_NOT_REGULARLY_SAMPLED) {
      dim[0] = dimensions[i]->length;
      dataspace_id = H5Screate_simple(1, dim, NULL);
    }
    else {
      dataspace_id = H5Screate(H5S_SCALAR);
    }

    if (dataspace_id < 0) {
      return (MI_ERROR);
    }
     
    /* Create a dataset(dimension variable name) in DIMENSIONS GROUP */
    dataset_id = H5Dcreate(grp_id, dimensions[i]->name, 
                           H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT);
    
    /* Dimension variable for a regular dimension contains
       no meaningful data. Whereas, Dimension variable for 
       an irregular dimension contains a vector with the lengths 
       equal to the sampled points along the dimension.
       Also, create a variable named "<dimension>-width" and 
       write the dimension->widths.
     */
    
    if (dimensions[i]->attr & MI_DIMATTR_NOT_REGULARLY_SAMPLED) {
      if (dimensions[i]->offsets == NULL) {
	return (MI_ERROR);
      }
      else {
	fspc_id = H5Dget_space(dataset_id);
	if (fspc_id < 0) {
	  return (MI_ERROR);
	}
	status = H5Dwrite(dataset_id, H5T_NATIVE_DOUBLE, dataspace_id, 
                          fspc_id, H5P_DEFAULT, dimensions[i]->offsets);
	if (status < 0) {
	  return (MI_ERROR);
	}
	size = strlen(dimensions[i]->name) + 6;
        name = malloc(size);
	strcpy(name, dimensions[i]->name);
	strcat(name, "-width");
	dataset_width = H5Dcreate(grp_id, name, H5T_NATIVE_DOUBLE, 
                                  dataspace_id, H5P_DEFAULT);
	fspc_id = H5Dget_space(dataset_width);
	if (fspc_id < 0) {
	  return (MI_ERROR);
	}
	status = H5Dwrite(dataset_width, H5T_NATIVE_DOUBLE, dataspace_id, fspc_id, H5P_DEFAULT, dimensions[i]->widths);
	if (status < 0) {
	  return (MI_ERROR);
	}
        miset_attr_at_loc(dataset_width, "length", MI_TYPE_INT,
                          1, &dimensions[i]->length);
	H5Dclose(dataset_width);
        free(name);
      }
    }
    
    if (dimensions[i]->attr & MI_DIMATTR_NOT_REGULARLY_SAMPLED) {
        name = "irregular";
    }
    else {
        name = "regular__";
    }
    
    miset_attr_at_loc(dataset_id, "spacing", MI_TYPE_STRING,
                      strlen(name), name);

    switch (dimensions[i]->class) {
    case MI_DIMCLASS_SPATIAL:
        name = "spatial";
        break;
    case MI_DIMCLASS_TIME:
        name = "time___";
        break;
    case MI_DIMCLASS_SFREQUENCY:
        name = "sfreq__";
        break;
    case MI_DIMCLASS_TFREQUENCY:
        name = "tfreq__";
        break;
    case MI_DIMCLASS_USER:
        name = "user___";
        break;
    case MI_DIMCLASS_RECORD:
        name = "record_";
        break;
    case MI_DIMCLASS_ANY:
    default:
        /* These should not be seen in this context!!!
         */
        return (MI_ERROR);
    }
    miset_attr_at_loc(dataset_id, "class", MI_TYPE_STRING, strlen(name),
                      name);

   /* Create Dimension attribute "direction_cosines"  */
    miset_attr_at_loc(dataset_id, "direction_cosines", MI_TYPE_DOUBLE,
                      3, dimensions[i]->direction_cosines);
    
    /* Save dimension length */
    miset_attr_at_loc(dataset_id, "length", MI_TYPE_INT,
                      1, &dimensions[i]->length);

    /* Save step value. */
    miset_attr_at_loc(dataset_id, "step", MI_TYPE_DOUBLE,
                      1, &dimensions[i]->step);

    /* Save start value. */
    miset_attr_at_loc(dataset_id, "start", MI_TYPE_DOUBLE,
                      1, &dimensions[i]->start);
   
    /* Save units. */
    miset_attr_at_loc(dataset_id, "units", MI_TYPE_STRING,
                      strlen(dimensions[i]->units), dimensions[i]->units);
    
    /* Save sample width. */
    miset_attr_at_loc(dataset_id, "width", MI_TYPE_DOUBLE,
                      1,  &dimensions[i]->width);

    /* Save comments. If no comments do not add this attr*/
    if (dimensions[i]->comments != NULL) {
      miset_attr_at_loc(dataset_id, "comments", MI_TYPE_STRING,
			strlen(dimensions[i]->comments),
                        dimensions[i]->comments);
    }
    H5Dclose(dataset_id);
    
    
  } //for (i=0; i < number_of_dimensions ; i++) 

  
  H5Gclose(grp_id);
  
  handle->mode = MI2_OPEN_RDWR;
  handle->has_slice_scaling = FALSE;
  handle->scale_min = -1.0;
  handle->scale_max = 1.0;
  handle->number_of_dims = number_of_dimensions;
  handle->image_id = -1;
  handle->imax_id = -1;
  handle->imin_id = -1;
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

  /* Verify the volume type.
   */
  switch (volume_type) {
  case MI_TYPE_BYTE:
  case MI_TYPE_SHORT:
  case MI_TYPE_INT:
  case MI_TYPE_FLOAT:
  case MI_TYPE_DOUBLE:
  case MI_TYPE_STRING:
  case MI_TYPE_UBYTE:
  case MI_TYPE_USHORT:
  case MI_TYPE_UINT:
  case MI_TYPE_SCOMPLEX:
  case MI_TYPE_ICOMPLEX:
  case MI_TYPE_FCOMPLEX:
  case MI_TYPE_DCOMPLEX:
  case MI_TYPE_UNKNOWN:
    break;
  default:
    return (MI_ERROR);
  }

  handle->volume_type = volume_type;

  /* Set the initial value of the valid-range 
   */
  miinit_default_range(handle->volume_type,
                       &handle->valid_max, 
                       &handle->valid_min);

  miget_voxel_to_world(handle, handle->v2w_transform);

  props_handle = (volprops *)malloc(sizeof(*props_handle));
  memset(props_handle, 0, sizeof (volprops));
  if (create_props != NULL) {
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
  }
  handle->create_props = props_handle;
  
  *volume = handle;
 
  return (MI_NOERROR);
}

/** Return the number of dimensions associated with this volume.
 */
int
miget_volume_dimension_count(mihandle_t volume, midimclass_t cls,
			     midimattr_t attr, int *number_of_dimensions)
{
    int i, count=0;
  
    if (volume == NULL || number_of_dimensions == NULL) {
        return (MI_ERROR);
    }
    for (i=0; i< volume->number_of_dims; i++) {
        if ((cls == MI_DIMCLASS_ANY || volume->dim_handles[i]->class == cls) &&
            (attr == MI_DIMATTR_ALL || volume->dim_handles[i]->attr == attr)) {
            count++;
        }
    }

    *number_of_dimensions = count;
    return (MI_NOERROR);
}

/** Returns the number of voxels in the volume.
 */
int
miget_volume_voxel_count(mihandle_t volume, int *number_of_voxels)
{
    char path[MI2_MAX_PATH];
    hid_t dset_id;
    hid_t fspc_id;

    if (volume == NULL || number_of_voxels == NULL) {
        return (MI_ERROR);
    }

    /* Quickest way to do this is with the dataspace identifier of the
     * volume.
     */
    sprintf(path, "/minc-2.0/image/%d/image", volume->selected_resolution);

    dset_id = H5Dopen(volume->hdf_id, path);
    if (dset_id < 0) {
        return (MI_ERROR);
    }

    fspc_id = H5Dget_space(dset_id);
    if (fspc_id < 0) {
        return (MI_ERROR);
    }

    *number_of_voxels = (int) H5Sget_simple_extent_npoints(fspc_id);

    H5Sclose(fspc_id);
    H5Dclose(dset_id);
    return (MI_NOERROR);
}

static int 
_miget_file_dimension_count(hid_t file_id)
{
    hid_t dset_id;
    hid_t space_id;
    int result = -1;

    H5E_BEGIN_TRY {
        dset_id = midescend_path(file_id, "/minc-2.0/image/0/image");
    } H5E_END_TRY;

    if (dset_id >= 0) {
        space_id = H5Dget_space(dset_id);
        if (space_id > 0) {
            result = H5Sget_simple_extent_ndims(space_id);
            H5Sclose(space_id);
        }
        H5Dclose(dset_id);
    }
    return (result);
}

static int
_miset_volume_class(mihandle_t volume, miclass_t volume_class)
{
    const char *class_ptr;

    switch (volume_class) {
    case MI_CLASS_REAL:
        class_ptr = "real___";
        break;
    case MI_CLASS_INT:
        class_ptr = "integer";
        break;
    case MI_CLASS_LABEL:
        class_ptr = "label__";
        break;
    case MI_CLASS_COMPLEX:
        class_ptr = "complex";
        break;
    case MI_CLASS_UNIFORM_RECORD:
        class_ptr = "array__";
        break;
    default:
        return (MI_ERROR);
    }
    miset_attribute(volume, MI_ROOT_PATH, "class", MI_TYPE_STRING,
                    strlen(class_ptr), class_ptr);
    return (MI_NOERROR);
}

static int
_miget_volume_class(mihandle_t volume, miclass_t *volume_class)
{
    char class_buf[MI2_CHAR_LENGTH];

    miget_attribute(volume, MI_ROOT_PATH, "class", MI_TYPE_STRING,
                    MI2_CHAR_LENGTH, class_buf);

    if (!strcmp(class_buf, "label__")) {
        *volume_class = MI_CLASS_LABEL;
    }
    else if (!strcmp(class_buf, "integer")) {
        *volume_class = MI_CLASS_INT;
    }
    else if (!strcmp(class_buf, "complex")) {
        *volume_class = MI_CLASS_COMPLEX;
    }
    else if (!strcmp(class_buf, "array__")) {
        *volume_class = MI_CLASS_UNIFORM_RECORD;
    }
    else {
        *volume_class = MI_CLASS_REAL;
    }
    return (MI_NOERROR);
}

static int
_miget_file_dimension(mihandle_t volume, const char *dimname, 
                      midimhandle_t *hdim_ptr)
{
    char path[MI2_CHAR_LENGTH];
    char temp[MI2_CHAR_LENGTH];
    midimhandle_t hdim;

    sprintf(path, "/minc-2.0/dimensions/%s", dimname);

    hdim = (midimhandle_t) malloc(sizeof (*hdim));
    memset(hdim, 0, sizeof (*hdim));

    hdim->name = strdup(dimname);
    H5E_BEGIN_TRY {
        int r;
        r = miget_attribute(volume, path, "spacing", MI_TYPE_STRING, 
                            MI2_CHAR_LENGTH, temp);
        if (!strcmp(temp, "irregular")) {
            hdim->attr |= MI_DIMATTR_NOT_REGULARLY_SAMPLED;
        }

        r = miget_attribute(volume, path, "class", MI_TYPE_STRING, 
                            MI2_CHAR_LENGTH, temp);
        if (r < 0) {
            /* Get the default class. */
            if (!strcmp(dimname, "time")) {
                hdim->class = MI_DIMCLASS_TIME;
            }
            else {
                hdim->class =  MI_DIMCLASS_SPATIAL;
            }
        }
        else {
            if (!strcmp(temp, "spatial")) {
                hdim->class = MI_DIMCLASS_SPATIAL;
            }
            else if (!strcmp(temp, "time___")) {
                hdim->class = MI_DIMCLASS_TIME;
            }
            else if (!strcmp(temp, "sfreq__")) {
                hdim->class = MI_DIMCLASS_SFREQUENCY;
            }
            else if (!strcmp(temp, "tfreq__")) {
                hdim->class = MI_DIMCLASS_TFREQUENCY;
            }
            else if (!strcmp(temp, "user___")) {
                hdim->class = MI_DIMCLASS_USER;
            }
            else if (!strcmp(temp, "record_")) {
                hdim->class = MI_DIMCLASS_RECORD;
            }
            else {
                /* TODO: error message?? */
            }
        }
        r = miget_attribute(volume, path, "length", MI_TYPE_UINT, 1, &hdim->length);
        if (r < 0) {
            fprintf(stderr, "Can't get length\n");
        }
        r = miget_attribute(volume, path, "start", MI_TYPE_DOUBLE, 1, &hdim->start);
        if (r < 0) {
            hdim->start = 0.0;
        }
        
        r = miget_attribute(volume, path, "step", MI_TYPE_DOUBLE, 1, &hdim->step);
        if (r < 0) {
            hdim->step = 1.0;
        }
        r = miget_attribute(volume, path, "direction_cosines", MI_TYPE_DOUBLE, 3, 
                            hdim->direction_cosines);
        if (r < 0) {
            hdim->direction_cosines[MI2_X] = 0.0;
            hdim->direction_cosines[MI2_Y] = 0.0;
            hdim->direction_cosines[MI2_Z] = 0.0;
            if (!strcmp(dimname, "xspace")) {
                hdim->direction_cosines[MI2_X] = 1.0;
            }
            else if (!strcmp(dimname, "yspace")) {
                hdim->direction_cosines[MI2_Y] = 1.0;
            }
            else if (!strcmp(dimname, "zspace")) {
                hdim->direction_cosines[MI2_Z] = 1.0;
            }
        }
    } H5E_END_TRY;

    *hdim_ptr = hdim;
    return (MI_NOERROR);
}


/** Opens an existing MINC volume for read-only access if mode argument is
    MI2_OPEN_READ, or read-write access if mode argument is MI2_OPEN_RDWR.
 */
int
miopen_volume(const char *filename, int mode, mihandle_t *volume)
{
    hid_t file_id;
    hid_t dset_id;
    hid_t space_id;
    volumehandle *handle;
    int hdf_mode;
    char dimorder[MI2_CHAR_LENGTH];
    int i,r;
    char *p1, *p2;
    H5T_class_t class;
    size_t nbytes;
    int is_signed;

    miinit();
    
    if (mode == MI2_OPEN_READ) {
        hdf_mode = H5F_ACC_RDONLY;
    }
    else if (mode == MI2_OPEN_RDWR) {
        hdf_mode = H5F_ACC_RDWR;
    }
    else {
        return (MI_ERROR);
    }
    
    file_id = hdf_open(filename, hdf_mode);
    if (file_id < 0) {
	return (MI_ERROR);
    }
    
    handle = (volumehandle *)malloc(sizeof(*handle));
    if (handle == NULL) {
      return (MI_ERROR);
    }
    
    handle->hdf_id = file_id;
    handle->mode = mode;
    handle->selected_resolution = 0;

    /* Get the volume class.
     */
    _miget_volume_class(handle, &handle->volume_class);

    /* GET THE DIMENSION COUNT
     */
    handle->number_of_dims = _miget_file_dimension_count(file_id);
    
    /* READ EACH OF THE DIMENSIONS 
     */
    handle->dim_handles = (midimhandle_t *)malloc(handle->number_of_dims *
                                                  sizeof(midimhandle_t));
    r =  miget_attribute(handle, "/minc-2.0/image/0/image", "dimorder", 
                    MI_TYPE_STRING, sizeof(dimorder), dimorder);
    
    if ( r < 0) {
      return (MI_ERROR);
    }
    p1 = dimorder;

    for (i = 0; i < handle->number_of_dims; i++) {
        p2 = strchr(p1, ',');
        if (p2 != NULL) {
            *p2 = '\0';
        }
        _miget_file_dimension(handle, p1, &handle->dim_handles[i]);
        p1 = p2 + 1;
    }
    
    /* SEE IF SLICE SCALING IS ENABLED
     */
    handle->has_slice_scaling = FALSE;
    H5E_BEGIN_TRY {
        dset_id = H5Dopen(file_id, "/minc-2.0/image/0/image-max");
    } H5E_END_TRY;
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

    if (!handle->has_slice_scaling) {
        miget_scalar(handle->hdf_id, H5T_NATIVE_DOUBLE,
                     "/minc-2.0/image/0/image-min", &handle->scale_min);

        miget_scalar(handle->hdf_id, H5T_NATIVE_DOUBLE,
                     "/minc-2.0/image/0/image-max", &handle->scale_max);
    }
    
    /* Read the current settings for valid-range */
    miread_valid_range(handle, &handle->valid_max, &handle->valid_min);

    /* Read the current voxel-to-world transform */
    miget_voxel_to_world(handle, handle->v2w_transform);

    /* Calculate the inverse transform */
    miinvert_transform(handle->v2w_transform, handle->w2v_transform);

    /* Initialize the selected resolution. */
    handle->selected_resolution = 0;
    
    handle->image_id = H5Dopen(file_id, "/minc-2.0/image/0/image");
    if (handle->image_id < 0) {
	return (MI_ERROR);
    }
    handle->type_id = H5Dget_type(handle->image_id);
    if (handle->type_id < 0) {
	return (MI_ERROR);
    }

    H5E_BEGIN_TRY {
        handle->imax_id = H5Dopen(file_id, "/minc-2.0/image/0/image-max");
        handle->imin_id = H5Dopen(file_id, "/minc-2.0/image/0/image-min");
    } H5E_END_TRY;

    /* Convert the type to a MINC type.
     */
    class = H5Tget_class(handle->type_id);
    nbytes = H5Tget_size(handle->type_id);

    switch (class) {
    case H5T_INTEGER:
	is_signed = (H5Tget_size(handle->type_id) == H5T_SGN_2);

	switch (nbytes) {
	case 1:
	    handle->volume_type = (is_signed ? MI_TYPE_BYTE : MI_TYPE_UBYTE);
	    break;
	case 2:
	    handle->volume_type = (is_signed ? MI_TYPE_SHORT : MI_TYPE_USHORT);
	    break;
	case 4:
	    handle->volume_type = (is_signed ? MI_TYPE_INT : MI_TYPE_UINT);
	    break;
	default:
            return (MI_ERROR);
	}
	break;
    case H5T_FLOAT:
	handle->volume_type = (nbytes == 4) ? MI_TYPE_FLOAT : MI_TYPE_DOUBLE;
	break;
    case H5T_STRING:
	handle->volume_type = MI_TYPE_STRING;
	break;
    case H5T_ARRAY:
	/* TODO: handle this case for uniform records (arrays)? */
	break;
    case H5T_COMPOUND:
	/* TODO: handle this case for non-uniform records? */
	break;
    default:
        return (MI_ERROR);
    }
    *volume = handle;
    return (MI_NOERROR);
}

/** Writes any changes associated with the volume to disk.
 */
int
miflush_volume(mihandle_t volume)
{
    H5Fflush(volume->hdf_id, H5F_SCOPE_GLOBAL);
    misave_valid_range(volume);
    return (MI_NOERROR);
}

/** Close an existing MINC volume. If the volume was newly created, all
changes will be written to disk. In all cases this function closes
the open volume and frees memory associated with the volume handle.
 */
int 
miclose_volume(mihandle_t volume)
{
    if (volume == NULL) {
        return (MI_ERROR);
    }

    miflush_volume(volume);

    if (volume->image_id > 0) {
        H5Dclose(volume->image_id);
    }
    if (volume->imax_id > 0) {
        H5Dclose(volume->imax_id);
    }
    if (volume->imin_id > 0) {
        H5Dclose(volume->imin_id);
    }
    if (volume->type_id > 0) {
        H5Tclose(volume->type_id);
    }
    if (volume->plist_id > 0) {
        H5Pclose(volume->plist_id);
    }
    if (H5Fclose(volume->hdf_id) < 0) {
      return (MI_ERROR);
    }
    if (volume->dim_handles != NULL) {
        free(volume->dim_handles);
    }
    if (volume->dim_indices != NULL) {
        free(volume->dim_indices);
    }
    if (volume->create_props != NULL) {
      mifree_volume_props(volume->create_props);
    }
    free(volume);

    return (MI_NOERROR);
}



/* Internal functions
 */
static void
miinit_default_range(mitype_t mitype, double *valid_max, double *valid_min)
{
    switch (mitype) {
    case MI_TYPE_BYTE:
        *valid_min = CHAR_MIN;
        *valid_max = CHAR_MAX;
        break;
    case MI_TYPE_SHORT:
        *valid_min = SHRT_MIN;
        *valid_max = SHRT_MAX;
        break;
    case MI_TYPE_INT:
        *valid_min = INT_MIN;
        *valid_max = INT_MAX;
        break;
    case MI_TYPE_UBYTE:
        *valid_min = 0;
        *valid_max = UCHAR_MAX;
        break;
    case MI_TYPE_USHORT:
        *valid_min = 0;
        *valid_max = USHRT_MAX;
        break;
    case MI_TYPE_UINT:
        *valid_min = 0;
        *valid_max = UINT_MAX;
        break;
    case MI_TYPE_FLOAT:
        *valid_min = -FLT_MAX;
        *valid_max = FLT_MAX;
        break;
    case MI_TYPE_DOUBLE:
        *valid_min = -DBL_MAX;
        *valid_max = DBL_MAX;
        break;
    default:
        *valid_min = 0;
        *valid_max = 1;
        break;
    }
}

static void
miread_valid_range(mihandle_t volume, double *valid_max, double *valid_min)
{
    int r;
    double range[2];

    H5E_BEGIN_TRY {
        r = miget_attribute(volume, "/minc-2.0/image/0/image", "valid_range",
                            MI_TYPE_DOUBLE, 2, range);
    } H5E_END_TRY;
    if (r == MI_NOERROR) {
        if (range[0] < range[1]) {
            *valid_min = range[0];
            *valid_max = range[1];
        }
        else {
            *valid_min = range[1];
            *valid_max = range[0];
        }
    }
    else {
        /* Didn't find the attribute, so assign default values. */
        miinit_default_range(volume->volume_type, valid_max, valid_min);
    }
}

void
misave_valid_range(mihandle_t volume)
{
    double range[2];
    range[0] = volume->valid_min;
    range[1] = volume->valid_max;
    miset_attribute(volume, "/minc-2.0/image/0/image", "valid_range",
                    MI_TYPE_DOUBLE, 2, range);
}

