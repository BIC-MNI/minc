

/************************************************************************
 * MINC 2.0 "DIMENSION" FUNCTIONS
 ************************************************************************/
#include <stdlib.h>
#include <hdf5.h>
#include "minc2.h"
#include "minc2_private.h"
#include "minc.h"


/*! Figure out whether a dimension is associated with a volume.
 */

int 
miget_volume_from_dimension(midimhandle_t dimension, mihandle_t *volume)
{
 
  if (dimension == NULL) {
    return (MI_ERROR);
  }  

  if (dimension->volume_handle != NULL) {
    
    *volume = dimension->volume_handle;
  }
  else {
    return (MI_ERROR);
  }

  return (MI_NOERROR);
}

/*! Create a copy of a given dimension.
 */

int 
micopy_dimension(midimhandle_t dim_ptr, midimhandle_t *new_dim_ptr)
{
  int i;
  dimension *handle;
  
  if (dim_ptr == NULL) {
    return (MI_ERROR);
  }

  /* Allocate storage for the structure
   */
  handle = (dimension *)malloc(sizeof(*handle));
  if (handle == NULL) {
    return (MI_ERROR);
  }
  handle->attr = dim_ptr->attr;
  handle->class = dim_ptr->class;
  /* Copy direction cosines */
  handle->direction_cosines[MI2_X] = dim_ptr->direction_cosines[0];
  handle->direction_cosines[MI2_Y] = dim_ptr->direction_cosines[1];
  handle->direction_cosines[MI2_Z] = dim_ptr->direction_cosines[2];
 
  switch (dim_ptr->flipping_order) {
  case MI_FILE_ORDER:
    handle->flipping_order = MI_FILE_ORDER;
    break;
  case MI_COUNTER_FILE_ORDER:
    handle->flipping_order = MI_COUNTER_FILE_ORDER;
    break;
  default:
    return (MI_ERROR);
  }
  
  handle->name =strdup(dim_ptr->name);
  handle->length = dim_ptr->length;
  if (dim_ptr->offsets != NULL) {
    handle->offsets = (double *) malloc(dim_ptr->length*sizeof(double));
    if (handle->offsets == NULL) {
      return (MI_ERROR);
    }
    for (i=0; i < dim_ptr->length; i++) {
      handle->offsets[i] = dim_ptr->offsets[i];
    }
  }
  else {
    handle->offsets = NULL;
  }
  handle->start = dim_ptr->start;
  handle->step = dim_ptr->step;
  if (dim_ptr->units != NULL) {
    handle->units = strdup(dim_ptr->units);
  }
  else {
    handle->units = NULL;
  }
  handle->width = dim_ptr->width;
  if (dim_ptr->widths != NULL) {
    handle->widths = (double *) malloc(dim_ptr->length*sizeof(double));
    if (handle->widths == NULL) {
      return (MI_ERROR);
    }
    for (i=0; i < dim_ptr->length; i++) {
      handle->widths[i] = dim_ptr->widths[i];
    }
  }
  else {
    handle->widths = NULL;
  }
  handle->volume_handle = dim_ptr->volume_handle;

  *new_dim_ptr = handle;
  
  return (MI_NOERROR);
}

/*! Define a new dimension in a MINC volume.
 */

int 
micreate_dimension(const char *name, midimclass_t class, midimattr_t attr, 
		   unsigned long length, midimhandle_t *new_dim_ptr)
{  
  
  dimension *handle;
  int i;
  /* Allocate space for the new dimension
   */
  handle = (dimension *)malloc(sizeof(*handle));
  if (handle == NULL) {
    return (MI_ERROR);
  }


  /* Duplicate the dimension name
   */
  handle->name = strdup(name);
  /* Set the dimension comment to NULL unless otherwise
   */
  handle->comments = NULL;  
  switch (class) {
  case MI_DIMCLASS_ANY:
    handle->class  = MI_DIMCLASS_ANY;
    break;
  case MI_DIMCLASS_SPATIAL:
    handle->class  = MI_DIMCLASS_SPATIAL;
    if (strcmp(name, "xspace") == 0) {
      handle->direction_cosines[MI2_X] = 1.0;
      handle->direction_cosines[MI2_Y] = 0.0;
      handle->direction_cosines[MI2_Z] = 0.0;
      handle->comments = strdup("X increases from patient left to right");
    }
    else if (strcmp(name, "yspace") == 0) {
      handle->direction_cosines[MI2_X] = 0.0;
      handle->direction_cosines[MI2_Y] = 1.0;
      handle->direction_cosines[MI2_Z] = 0.0;
      handle->comments = strdup("Y increases from patient posterior to anterior");
    }
    else if (strcmp(name, "zspace") == 0) {
      handle->direction_cosines[MI2_X] = 0.0;
      handle->direction_cosines[MI2_Y] = 0.0;
      handle->direction_cosines[MI2_Z] = 1.0;
      handle->comments = strdup("Z increases from patient inferior to superior");
    }
    else {
      handle->direction_cosines[MI2_X] = 1.0;
      handle->direction_cosines[MI2_Y] = 0.0;
      handle->direction_cosines[MI2_Z] = 0.0;
      handle->comments = NULL;
    }
    break;
  case MI_DIMCLASS_TIME:
    handle->class  = MI_DIMCLASS_TIME;
    break;
  case MI_DIMCLASS_SFREQUENCY:
    handle->class  = MI_DIMCLASS_SFREQUENCY;
    if (strcmp(name, "xfrequency") == 0) {
      handle->direction_cosines[MI2_X] = 1.0;
      handle->direction_cosines[MI2_Y] = 0.0;
      handle->direction_cosines[MI2_Z] = 0.0;
    }
    else if (strcmp(name, "yfrequency") == 0) {
      handle->direction_cosines[MI2_X] = 0.0;
      handle->direction_cosines[MI2_Y] = 1.0;
      handle->direction_cosines[MI2_Z] = 0.0;
    }
    else if (strcmp(name, "zfrequency") == 0) {
      handle->direction_cosines[MI2_X] = 0.0;
      handle->direction_cosines[MI2_Y] = 0.0;
      handle->direction_cosines[MI2_Z] = 1.0;
    }
    else {
      handle->direction_cosines[MI2_X] = 1.0;
      handle->direction_cosines[MI2_Y] = 0.0;
      handle->direction_cosines[MI2_Z] = 0.0;
    }
    break;
  case MI_DIMCLASS_TFREQUENCY:
    handle->class  = MI_DIMCLASS_TFREQUENCY;
    break;
  case MI_DIMCLASS_USER:
    handle->class  = MI_DIMCLASS_USER;
    break;
  case MI_DIMCLASS_RECORD:
    handle->class  = MI_DIMCLASS_RECORD;
    break;
  default:
    return (MI_ERROR);
  }

  handle->offsets = NULL;
  handle->attr = attr;
  if (attr & MI_DIMATTR_NOT_REGULARLY_SAMPLED) {
    handle->widths = (double *) malloc(length *sizeof(double));
    for (i=0; i< length; i++) {
      
      handle->widths[i] = 1.0;
    }
  }
  else {
    handle->widths = NULL;
  }
  handle->start = 0.0;
  handle->step = 1.0;
  handle->width = 1.0;

  handle->flipping_order = MI_FILE_ORDER;
  if (class != MI_DIMCLASS_SPATIAL && class != MI_DIMCLASS_SFREQUENCY ) {
    handle->direction_cosines[MI2_X] = 1.0;
    handle->direction_cosines[MI2_Y] = 0.0;
    handle->direction_cosines[MI2_Z] = 0.0;
  }
  handle->length = length;
  handle->units = strdup("mm");
  /* volume_handle is the only NULL value once the dimension is created.
   */
  handle->volume_handle = NULL;

  *new_dim_ptr = handle;
  
  return (MI_NOERROR);

}

/*! Delete the dimension definition.
    Note: The original document stated that a dimension has to be
    associated with a given volume before it can be deleted. This
    feature was erased from the document and not considered here.
 */
int 
mifree_dimension_handle(midimhandle_t dim_ptr)
{
  
  if (dim_ptr == NULL) {
    return (MI_ERROR);
  }
  free(dim_ptr->name);
  //if (dim_ptr->offsets != NULL) {
    free(dim_ptr->offsets);
    //}
  free(dim_ptr->units);
  //if (dim_ptr->widths !=NULL) {
    free(dim_ptr->widths);
    // }
  free(dim_ptr);
  
  return (MI_NOERROR);
}

/*! Retrieve the list of dimensions defined in a MINC volume, 
    with the same class and attribute as given.
 */

int 
miget_volume_dimensions(mihandle_t volume, midimclass_t class, midimattr_t attr,
			miorder_t order, int array_length, 
			midimhandle_t dimensions[])
{
  // THE PARAMETER "miorder_t order" WAS NOT CONSIDERED WHEW WRITING
  // THIS FUNCTION. MUST FIGURE OUT WHAT TO DO WITH IT
  
  hsize_t number_of_dims; 
  int i=0, j=0, max_dims;
  
  if (volume == NULL) {
    return (MI_ERROR);
  }
 
  number_of_dims = volume->number_of_dims;

  if (array_length > number_of_dims) {
    max_dims = number_of_dims;
  }
  else {
    max_dims = array_length;
  }
  
  /* Go through each dimension separately and figure out
     which one has a matching class and attirbute.
   */
  for (i=0; i < max_dims; i++) {
      midimhandle_t hdim = volume->dim_handles[i];
      if (class == MI_DIMCLASS_ANY || class == hdim->class) {
          if (hdim->attr == attr || attr ==  MI_DIMATTR_ALL) {
              dimensions[j] = hdim;
              j++;
          }
      }
  }
  return (MI_NOERROR);
}

/*! Set apparent dimension order.
 */

int 
miset_apparent_dimension_order(mihandle_t volume, int array_length, 
			       midimhandle_t dimensions[])
{
  int diff;
  int i=0, j=0, k=0;

  if (volume == NULL || array_length <= 0 || 
      array_length != volume->number_of_dims) {
    return (MI_ERROR);
  }
   /* If array_length was more than the number of dimensions
     the rest of the given dimensions will be ignored.
   */ 
  diff = volume->number_of_dims - array_length;
  if (diff < 0) {
    diff = 0;
  }
  /* Allocated space for dimensions indices, if not already done.
   */
  if (volume->dim_indices == NULL){
    volume->dim_indices = (int *)malloc(volume->number_of_dims*sizeof(int));
    memset(volume->dim_indices, -1, sizeof(volume->number_of_dims));
  }
  for (i=0; i < volume->number_of_dims; i++) {
    for (j=0; j < array_length; j++) {
      if (volume->dim_handles[i] == dimensions[j]) {
	volume->dim_indices[j+diff] = i;
	break;
      }
      if (j == (array_length-1)) {
	volume->dim_indices[k++] = i;
      }
    }
  }
  return (MI_NOERROR);
}

/*! Set apparent dimension order by name.
 */

int 
miset_apparent_dimension_order_by_name(mihandle_t volume, int array_length, 
				       char **names)
{
  int diff;
  int i=0, j=0, k=0;

  if (volume == NULL || array_length <= 0 ) {
    return (MI_ERROR);
  }
  /* Note that all dimension names must be different or an error occurs.
   */
  for (i=0;i<array_length;i++) {
    for (j=i+1;j<array_length;j++) {
      if (strcmp(names[i],names[j]) == 0) {
	return (MI_ERROR);
      }
    }
  }
  /* If array_length was more than the number of dimensions
     the rest of the given dimensions will be ignored.
   */ 
  diff = volume->number_of_dims - array_length;
  if (diff < 0) {
    diff = 0;
  }
  /* Allocated space for dimensions indices, if not already done.
   */
  if (volume->dim_indices == NULL) {
    volume->dim_indices = (int *)malloc(volume->number_of_dims*sizeof(int));
    for (i = 0; i < volume->number_of_dims; i++) {
        volume->dim_indices[i] = -1;
    }
  }
  for (i=0; i < volume->number_of_dims; i++) {
    for (j=0; j < array_length; j++) {
      if (!strcmp(volume->dim_handles[i]->name,names[j])) {
          volume->dim_indices[i+diff] = j;
	break;
      }
      if (j == (array_length-1)) {
	volume->dim_indices[k++] = i;
      }
    }
  }
  return (MI_NOERROR);
}

/*! Set the record flag and add a record dimension to the volume
    dimensions so the volume wuld appear to have n+1 dimensions
 */

int 
miset_apparent_record_dimension_flag(mihandle_t volume, int record_flag)
{
  
  dimension *handle;  
 
  if (volume == NULL) {
    return (MI_ERROR);
  }
  /* Allocate space for the dimension
   */
  handle = (dimension *)malloc(sizeof(*handle));
  if (handle == NULL) {
    return (MI_ERROR);
  }
  handle->class = MI_DIMCLASS_RECORD;
  handle->volume_handle = volume;

  volume->dim_handles[volume->number_of_dims] = handle;
  /* Add one to the number of dimensions so the volume
     will appear to have (n+1) dimensions
   */
  volume->number_of_dims++;
  
  record_flag = 1;

  return (MI_NOERROR);
}

/*! Get the apparent order of voxels.
 */

int 
miget_dimension_apparent_voxel_order(midimhandle_t dimension, miflipping_t *file_order,
				     miflipping_t *sign)
{
  if (dimension == NULL) {
    return (MI_ERROR);
  }
  switch (dimension->flipping_order) {
  case MI_FILE_ORDER:
    *file_order = MI_FILE_ORDER;
    if (dimension->step > 0) {
      *sign = MI_POSITIVE;
    }
    else {
      *sign = MI_NEGATIVE;
    }
    break;
  case MI_COUNTER_FILE_ORDER:
    *file_order = MI_COUNTER_FILE_ORDER;
    if (dimension->step > 0) {
      *sign = MI_NEGATIVE;
    }
    else {
      *sign = MI_POSITIVE;
    }
    break;
  case MI_POSITIVE:
    *sign = MI_POSITIVE;
    if (dimension->step > 0) {
      *file_order = MI_FILE_ORDER;
    }
    else {
      *file_order = MI_COUNTER_FILE_ORDER; 
    }
    break;  
  case MI_NEGATIVE:
    *sign = MI_NEGATIVE;
    if (dimension->step > 0) {
      *file_order = MI_COUNTER_FILE_ORDER;
    }
    else {
      *file_order = MI_FILE_ORDER; 
    }
    break;  
  default:
    return (MI_ERROR);
  }
  return (MI_NOERROR);
}

/*! Set the apparent order of voxels.
 */

int 
miset_dimension_apparent_voxel_order(midimhandle_t dimension, miflipping_t flipping_order)
{
  if (dimension == NULL) {
    return (MI_ERROR);
  }
  switch (flipping_order) {
  case MI_FILE_ORDER:
    dimension->flipping_order  = MI_FILE_ORDER;
    break;
  case MI_COUNTER_FILE_ORDER:
    dimension->flipping_order  = MI_COUNTER_FILE_ORDER;
    break;
  case MI_POSITIVE:
    dimension->flipping_order  = MI_POSITIVE;
    break;
    /*
    if (dimension->separation > 0) {
      dimension->flipping_order  = MI_FILE_ORDER;
    }
    else {
      dimension->flipping_order  = MI_COUNTER_FILE_ORDER;
    }
    break;
    */
  case MI_NEGATIVE:
    dimension->flipping_order  = MI_NEGATIVE;
    break;
    /*
    if (dimension->separation > 0) {
      dimension->flipping_order  = MI_COUNTER_FILE_ORDER;
    }
    else {
      dimension->flipping_order  = MI_FILE_ORDER;
    }
    break;
    */
  default:
    return (MI_ERROR);
  }
  
  return (MI_NOERROR);
}

/*! Get the class of a MINC dimension.
 */

int 
miget_dimension_class(midimhandle_t dimension, midimclass_t *class)
{
  if (dimension == NULL) {
    return (MI_ERROR);
  }

  switch (dimension->class) {
  case MI_DIMCLASS_ANY:
    *class = MI_DIMCLASS_ANY;
    break;
  case MI_DIMCLASS_SPATIAL:
    *class = MI_DIMCLASS_SPATIAL;
    break;
  case MI_DIMCLASS_TIME:
    *class = MI_DIMCLASS_TIME;
    break;
  case MI_DIMCLASS_SFREQUENCY:
    *class = MI_DIMCLASS_SFREQUENCY;
    break;
  case MI_DIMCLASS_TFREQUENCY:
    *class = MI_DIMCLASS_TFREQUENCY;
    break;
  case MI_DIMCLASS_USER:
    *class = MI_DIMCLASS_USER;
    break;
  case MI_DIMCLASS_RECORD:
    *class = MI_DIMCLASS_RECORD;
    break;
  default:
    return (MI_ERROR);
  }
  
  return (MI_NOERROR);
}

/*! Set the class of a MINC dimension.
 */

int 
miset_dimension_class(midimhandle_t dimension, midimclass_t class)
{
   if (dimension == NULL) {
    return (MI_ERROR);
  }
  switch (class) {
  case MI_DIMCLASS_ANY:
    dimension->class = MI_DIMCLASS_ANY;
    break;
  case MI_DIMCLASS_SPATIAL:
    dimension->class = MI_DIMCLASS_SPATIAL;
    break;
  case MI_DIMCLASS_TIME:
    dimension->class = MI_DIMCLASS_TIME;
    break;
  case MI_DIMCLASS_SFREQUENCY:
    dimension->class = MI_DIMCLASS_SFREQUENCY;
    break;
  case MI_DIMCLASS_TFREQUENCY:
    dimension->class = MI_DIMCLASS_TFREQUENCY;
    break;
  case MI_DIMCLASS_USER:
    dimension->class = MI_DIMCLASS_USER;
    break;
  case MI_DIMCLASS_RECORD:
    dimension->class = MI_DIMCLASS_RECORD;
    break;
  default:
    return (MI_ERROR);
  } 
  
  return (MI_NOERROR);
}

/*! Get the direction cosine vector of a given SPATIAL dimension.
 */

int 
miget_dimension_cosines(midimhandle_t dimension, double direction_cosines[3])
{
  if (dimension == NULL || (dimension->class != MI_DIMCLASS_SPATIAL &&
                            dimension->class != MI_DIMCLASS_SFREQUENCY)){
    return (MI_ERROR);
  }
  
  direction_cosines[0] = dimension->direction_cosines[0];
  direction_cosines[1] = dimension->direction_cosines[1];
  direction_cosines[2] = dimension->direction_cosines[2];

  return (MI_NOERROR);
}

/*! Set the direction cosine vector for a given SPATIAL dimension.
 */

int 
miset_dimension_cosines(midimhandle_t dimension, const double direction_cosines[3])
{
  
  if (dimension == NULL || dimension->class != MI_DIMCLASS_SPATIAL) {
    return (MI_ERROR);
  }
  
  dimension->direction_cosines[0] = direction_cosines[0];
  dimension->direction_cosines[1] = direction_cosines[1];
  dimension->direction_cosines[2] = direction_cosines[2];

  return (MI_NOERROR);
}

/*! Get the comments attribute for a given dimension.
 */

int
miget_dimension_description(midimhandle_t dimension, char **comments_ptr)
{

  if (dimension == NULL) {
    return (MI_ERROR);
  }
 
  *comments_ptr = strdup(dimension->comments);
  
  return (MI_NOERROR); 
}

/*! Set the comments attribute for a given dimension.
 */

int
miset_dimension_description(midimhandle_t dimension, const char *comments)
{

 if (dimension == NULL || comments == NULL) {
    return (MI_ERROR);
  }
 
  if ((strlen(comments) + 1) <= MI2_CHAR_LENGTH) {
    dimension->comments = strdup(comments);
  }
  else {
    return (MI_ERROR);
  }
    
  return (MI_NOERROR); 
}


/*! Get the identifier (name) of a MINC dimension.
 */

int 
miget_dimension_name(midimhandle_t dimension, char **name_ptr)
{
  if (dimension == NULL) {
    return (MI_ERROR);
  }

  *name_ptr = strdup(dimension->name);

  return (MI_NOERROR);
}

/*! Set the identifier (name) of a given MINC dimension.
 */

int 
miset_dimension_name(midimhandle_t dimension, const char *name)
{
  if (dimension == NULL || name == NULL) {
    return (MI_ERROR);
  }
 
  if ((strlen(name) + 1) <= MI2_CHAR_LENGTH) {
    dimension->name = strdup(name);
  }
  else {
    return (MI_ERROR);
  }
    
  return (MI_NOERROR);
}

/*! Get the absolute world coordinates of points along a MINC dimension.
 */

int 
miget_dimension_offsets(midimhandle_t dimension, unsigned long array_length, 
			unsigned long start_position, double offsets[])
{
  unsigned long  diff;
  int i, j=0;

  if (dimension == NULL || start_position > dimension->length ) {
    return (MI_ERROR);
  }
  if ((start_position + array_length) > dimension->length) {
    diff = dimension->length;
  }
  else {
    diff = array_length;
  }
  /* Allocate enough space for offsets.
   */
  offsets = (double *) malloc(diff*sizeof(double));
  
  if (dimension->offsets == NULL) {
    for (i=start_position; i <= diff ; i++) {
      /* For regularly sampled dimensions, the step value
	 is added to each value to get the next value.
       */
      offsets[j] = dimension->start + (i * dimension->step);
      j++;
    }
  }
  else {
    for (i=start_position; i <= diff ; i++) {
      offsets[j] = dimension->offsets[i];
      j++;
    }
  }
  return (MI_NOERROR);
}

/*! Set the absolute world coordinates of points along a MINC dimension.
 */

int 
miset_dimension_offsets(midimhandle_t dimension, unsigned long array_length, 
			unsigned long start_position, const double offsets[])
{
  unsigned long  diff;
  int i, j=0;
  /* Check to see whether the dimension is regularly sampled.
   */
  if (dimension == NULL || 
      (dimension->attr & MI_DIMATTR_NOT_REGULARLY_SAMPLED) == 0 || 
      start_position > dimension->length ) {
    return (MI_ERROR);
  }
  if ((start_position + array_length) > dimension->length) {
    diff = dimension->length;
  }
  else {
    diff = array_length;
  }
  
  /* Allocate space for the offsets if not already done.
   */
  if (dimension->offsets == NULL) {
    dimension->offsets = (double *) malloc(dimension->length*sizeof(double));
  }
  if (start_position == 0) {
    diff--;
  }
  for (i=start_position; i <= diff ; i++) {
      dimension->offsets[i] = offsets[j] ;
      j++;
    }
  return (MI_NOERROR);
}

/*! Get the sampling flag for a MINC dimension. 
 */

int 
miget_dimension_sampling_flag(midimhandle_t dimension, BOOLEAN *sampling_flag)
{
  if (dimension == NULL) {
    return (MI_ERROR);
  }

  *sampling_flag = (dimension->attr & MI_DIMATTR_NOT_REGULARLY_SAMPLED) != 0;

  return (MI_NOERROR);
}

/*! Set the sampling flag for a MINC dimension.
 */

int 
miset_dimension_sampling_flag(midimhandle_t dimension, BOOLEAN sampling_flag)
{
    if (dimension == NULL) {
        return (MI_ERROR);
    }
    if (sampling_flag) {
        dimension->attr |= MI_DIMATTR_NOT_REGULARLY_SAMPLED;
    }
    else {
        dimension->attr &= ~MI_DIMATTR_NOT_REGULARLY_SAMPLED;
    }
    return (MI_NOERROR);
}

/*! Get the sampling interval (STEP) for a single dimension.
 */

int 
miget_dimension_separation(midimhandle_t dimension, mivoxel_order_t voxel_order, 
			   double *separation_ptr)
{
  if (dimension == NULL) {
    return (MI_ERROR);
  }
  if (voxel_order == 0) {
    *separation_ptr = dimension->step;
  }
  else {
    *separation_ptr = -1 * dimension->step;
  }
  return (MI_NOERROR);
}

/*! Set the sampling interval (STEP) for a single dimension.
 */

int 
miset_dimension_separation(midimhandle_t dimension, double separation)
{
  /* file-order of voxels is assumed.
   */
  if (dimension == NULL) {
    return (MI_ERROR);
  }
  
  dimension->step = separation;
  /* If not explicitly set, the width will be assumed to be equal to the
     dimension's step size.
  */
  //dimension->width = separation;

  return (MI_NOERROR);
}

/*! Get the sampling interval (STEP) for a list of dimensions.
 */

int 
miget_dimension_separations(const midimhandle_t dimensions[], mivoxel_order_t voxel_order,
			    int array_length, double separations[])
{
  int i;
  /* Allocated space for the separations array.
   */
  separations  = (double *) malloc(array_length*sizeof(double));
  for (i=0; i< array_length; i++) {
    miget_dimension_separation(dimensions[i], voxel_order, &separations[i]);
  }
  
  return (MI_NOERROR);
}

/*! Set the sampling interval (STEP) for a list of dimensions.
 */

int 
miset_dimension_separations(const midimhandle_t dimensions[], int array_length, 
			    const double separations[])
{
  int i;
  for (i=0; i< array_length; i++) {
    miset_dimension_separation(dimensions[i],separations[i]);
  }
  return (MI_NOERROR);
}

/*! Get the length of a MINC dimension.
 */

int 
miget_dimension_size(midimhandle_t dimension, unsigned long *size_ptr)
{
  if (dimension == NULL) {
    return (MI_ERROR);
  }
  *size_ptr = dimension->length;
  return (MI_NOERROR);
}

/*! Set the length of a MINC dimension if not associated with a 
    volume.
 */

int 
miset_dimension_size(midimhandle_t dimension, unsigned long size)
{
  /* Check whether the dimension is associated with a volume.
   */
  if (dimension == NULL || dimension->volume_handle != NULL) {
    return (MI_ERROR);
  }
  dimension->length = size;
  return (MI_NOERROR);
}

/*! Retrieve the length of all dimensions in dimensions array.
 */

int 
miget_dimension_sizes(const midimhandle_t dimensions[], int array_length,
		      unsigned long sizes[])
{
  int i;
  /* Allocated space for the length array.
   */
  sizes  = (unsigned long *) malloc(array_length*sizeof(unsigned long));
  for(i=0; i<array_length; i++) {
    miget_dimension_size(dimensions[i], &sizes[i]);
  }
    
  return (MI_NOERROR);
}

/*! Get the start value of a MINC dimension.
 */

int 
miget_dimension_start(midimhandle_t dimension, mivoxel_order_t voxel_order,
		      double *start_ptr)
{
  /* If voxel_order is set to apparent file order (i.e., 1)
     start = start + step * (n-1)
   */
  if (dimension == NULL) {
    return (MI_ERROR);
  }
  if (voxel_order == 0) {
    *start_ptr = dimension->start;
  }
  else {
    
    *start_ptr = dimension->start + (dimension->step * (dimension->length-1));
  }
  return (MI_NOERROR);
}

/*! Set the origin of a MINC dimension.
 */

int 
miset_dimension_start(midimhandle_t dimension, double start_ptr)
{
  if (dimension == NULL) {
    return (MI_ERROR);
  }
  dimension->start = start_ptr;

  return (MI_NOERROR);
}

/*! Get the start values for MINC dimensions in dimensions array.
 */

int 
miget_dimension_starts(const midimhandle_t dimensions[], mivoxel_order_t voxel_order,
		       int array_length, double starts[])
{ 
  int i;
  /* Allocated space for the starts array.
   */
  starts  = (double *) malloc(array_length*sizeof(double));
  for (i=0; i < array_length; i++) {
    miget_dimension_start(dimensions[i], voxel_order, &starts[i]);
  }

  return (MI_NOERROR);
}

/*! Set the start values for MINC dimensions in dimensions array.
 */

int 
miset_dimension_starts(const midimhandle_t dimensions[], int array_length, 
		       const double starts[])
{
  int i;
 
  for (i=0; i<array_length; i++) {
    miset_dimension_start(dimensions[i], starts[i]);
  }
  return (MI_NOERROR);
}

/*! Get the unit string for a MINC dimension.
 */

int 
miget_dimension_units(midimhandle_t dimension, char **units_ptr)
{
  if (dimension == NULL) {
    return (MI_ERROR);
  }
  
  *units_ptr = strdup(dimension->units);
  
  return (MI_NOERROR);
}

/*! Set the unit string for a MINC dimension.
 */

int 
miset_dimension_units(midimhandle_t dimension, const char *units)
{
  if (dimension == NULL) {
    return (MI_ERROR);
  }
  
  if (strlen(units) + 1 <= MI2_CHAR_LENGTH) {
    dimension->units = strdup(units);
  }
  else {
    return (MI_ERROR);
  }

  return (MI_NOERROR);
}

/*! Get A single full-width half-maximum value from a 
    regularly sampled dimension.
 */

int 
miget_dimension_width(midimhandle_t dimension, double *width_ptr)
{
  
  if (dimension == NULL || 
      (dimension->attr & MI_DIMATTR_NOT_REGULARLY_SAMPLED) != 0) {
    return (MI_ERROR);
  }
  *width_ptr = dimension->width;
  return (MI_NOERROR);
}

/*! Set the A single full-width half-maximum value for a 
    regulalry sampled dimension.
 */

int 
miset_dimension_width(midimhandle_t dimension, double width_ptr)
{
  if (dimension == NULL || 
      (dimension->attr & MI_DIMATTR_NOT_REGULARLY_SAMPLED) != 0) {
    return (MI_ERROR);
  }
  /* Check to make sure width value is positive.
   */
  if (width_ptr < 0) {
    dimension->width = -1 * width_ptr;
  }
  else {
    dimension->width = width_ptr;
  }
  return (MI_NOERROR);
}

/*! Get the full-width half-maximum value for points along an
    irregularly sampled dimension.
 */

int 
miget_dimension_widths(midimhandle_t dimension, mivoxel_order_t voxel_order,
		       unsigned long array_length, unsigned long start_position,
		       double widths[])
{
  unsigned long  diff;
  int i, j = 0;

  if (dimension == NULL || start_position > dimension->length) {
    return (MI_ERROR);
  }
  
  if ((start_position + array_length) > dimension->length) {
    diff = dimension->length;
  }
  else {
    diff = array_length;
  }
  /* Allocate space for the widths array
   */
  widths = (double *) malloc(diff*sizeof(double));
  /* Check to see whether the dimension is regularly sampled.
   */
  if (start_position == 0) {
    diff--;
  }
  if (dimension->widths == NULL) {
    for (i=start_position; i <= diff; i++) {
      widths[j] = dimension->width;
      j++;
    }
  }
  else { 
    /* If the apparent order is requested, the widths are returned
       REVERSE (flip) order.
     */
    if (voxel_order == 0) {
      for (i=start_position; i <= diff; i++) {
	widths[j] = dimension->widths[i];
	j++;
      }
    }
    else {
      for (i=diff; i >= start_position; i--) {
	widths[j] = dimension->widths[i];
	j++;
      }
    }
      
  }

  return (MI_NOERROR);
}

/*! Set the full-width half-maximum value for points along an
    irregularly sampled dimension.
 */

int 
miset_dimension_widths(midimhandle_t dimension, unsigned long array_length,
		       unsigned long start_position, const double widths[])
{
  unsigned long  diff;
  int i, j=0;
  /* Check to see whether the dimension is regularly sampled.
   */
  if (dimension == NULL || 
      (dimension->attr & MI_DIMATTR_NOT_REGULARLY_SAMPLED) == 0 || 
      start_position > dimension->length) {
    return (MI_ERROR);
  }

  if ((start_position + array_length) > dimension->length) {
    diff = dimension->length;
  }
  else {
    diff = array_length;
  }
  /* Allocate space for widths array if not already done
   */
  if (dimension->widths == NULL) {
    dimension->widths = (double *) malloc(dimension->length*sizeof(double));
  }
  if (start_position == 0) {
    diff--;
  }
  for (i=start_position; i <= diff; i++) {
    if (widths[i] < 0) {
      dimension->widths[i] = -1 * widths[j];
    }
    else {
      dimension->widths[i] = widths[j];
    }
    j++;
  }
  return (MI_NOERROR);
}



#ifdef M2_TEST
#define TESTRPT(msg, val) (error_cnt++, fprintf(stderr, \
                                  "Error reported on line #%d, %s: %d\n", \
                                  __LINE__, msg, val))

static int error_cnt = 0;


int main(int argc, char **argv)
{
  mihandle_t vol, vol1;
  int r;
  midimhandle_t dimh, dimh1,dimh2, dimh3; 
  midimhandle_t dim[4];
  mivolumeprops_t props;
  double cosines[3];
  double offsets[3];
  double widths[3];
  int n;
  midimhandle_t dimens[3];
   /* Turn off automatic error reporting - we'll take care of this
   * ourselves, thanks!
   */
  H5Eset_auto(NULL, NULL);
  
  r = minew_volume_props(&props);
  r = miset_props_compression_type(props, MI_COMPRESS_ZLIB);
  r = miset_props_zlib_compression(props, 3);
  r = micreate_dimension("xspace",MI_DIMCLASS_SPATIAL,1, 10,&dimh);
  if (r < 0) {
    TESTRPT("failed", r);
  }
  dim[0]=dimh;
  
  r = micreate_dimension("yspace",MI_DIMCLASS_SPATIAL,1, 12,&dimh1);
  if (r < 0) {
    TESTRPT("failed", r);
  }
  dim[1]=dimh1;
  r = micreate_dimension("zspace",MI_DIMCLASS_SPATIAL,2, 3,&dimh2);
  if (r < 0) {
    TESTRPT("failed", r);
  }
  
  offsets[0]=1.5;
  offsets[1]=2.5;
  offsets[2]=3.5;
  r = miset_dimension_offsets(dimh2, 3, 0, offsets);
  r = miget_dimension_cosines(dimh1, cosines);
  if (r < 0) {
    TESTRPT("failed", r);
  }
  
  dim[2]=dimh2;
   r = micreate_dimension("zfrequency",MI_DIMCLASS_SFREQUENCY,1, 12,&dimh3);
  if (r < 0) {
    TESTRPT("failed", r);
  }
  r = miget_dimension_cosines(dimh3, cosines);
  if (r < 0) {
    TESTRPT("failed", r);
  } 
  dim[3]=dimh3;
  r = micreate_volume("test.h5", 4, dim, MI_TYPE_UBYTE, MI_CLASS_INT,props,&vol);
  if (r < 0) {
    TESTRPT("failed", r);
  }
  r = micreate_volume_image(vol);
  if (r < 0) {
    TESTRPT("failed", r);
  }
  
  r =miset_dimension_description(dimh3, "this is funny");
  if ( r < 0) {
    TESTRPT("failed", r);
  }
  widths[0]=2;
  widths[1]=3.5;
  widths[2]=4.5;
  r = miset_dimension_widths(dimh2, 3, 0, widths);
  r = miget_volume_dimensions(vol, MI_DIMCLASS_ANY, 0, 0, 4, dimens);
  if (r < 0) {
    TESTRPT("failed", r);
  }
  printf(" %s %s %s \n", dimens[0]->name, dimens[1]->name, dimens[2]->name);
  printf(" %s \n", dimens[3]->comments);
  printf(" %s \n", dimens[0]->comments);
  printf( " %f %f %f \n", dimens[2]->offsets[0], dimens[2]->offsets[1], dimens[2]->offsets[2] );
  printf( " %f %f %f \n", dimens[2]->widths[0], dimens[2]->widths[1], dimens[2]->widths[2] );
  r = miget_volume_dimension_count(vol, MI_DIMCLASS_SPATIAL, MI_DIMATTR_ALL, &n);
  if (r < 0) {
    TESTRPT("failed", r);
  }
  printf( " N is %d \n", n);
 
  r = miclose_volume(vol);
  if (r < 0) {
    TESTRPT("failed", r);
  }
  r= miopen_volume("test.h5",MI2_OPEN_READ ,&vol);
  if (r < 0) {
    TESTRPT("failed", r);
  }
  if (error_cnt != 0) {
    fprintf(stderr, "%d error%s reported\n", 
	    error_cnt, (error_cnt == 1) ? "" : "s");
  }
  else {
    fprintf(stderr, "No errors\n");
   
  }
  return (error_cnt);
}
#endif
