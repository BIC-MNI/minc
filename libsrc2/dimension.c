
/************************************************************************
 * MINC 2.0 "DIMENSION" FUNCTIONS
 ************************************************************************/
#include <stdlib.h>
#include <hdf5.h>
#include "minc2.h"
#include "minc2_private.h"


/*! Figure out whether a dimension is associated with a volume.
 */

int 
miget_volume_from_dimension(midimhandle_t hdim, mihandle_t *volume)
{
  dimension_struct *dimension = (dimension_struct *) hdim;
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
micopy_dimension(midimhandle_t hdim, midimhandle_t *new_dim_ptr)
{
  dimension_struct *handle;
  dimension_struct *dim_ptr;

  if ((dim_ptr = (dimension_struct *) hdim) == NULL) {
    return (MI_ERROR);
  }

  handle = (dimension_struct *)malloc(sizeof(dimension_struct));
  
  handle->name = dim_ptr->name;
  handle->class = dim_ptr->class;
  handle->attr = dim_ptr->attr;
  handle->size = dim_ptr->size;
  handle->cosines[0] = dim_ptr->cosines[0];
  handle->cosines[1] = dim_ptr->cosines[1];
  handle->cosines[2] = dim_ptr->cosines[2];
  handle->start = dim_ptr->start;
  handle->sampling_flag = dim_ptr->sampling_flag;
  handle->separation = dim_ptr->separation;
  handle->units = dim_ptr->units;
  handle->width = dim_ptr->width;

  *new_dim_ptr = handle;
  
  return (MI_NOERROR);
}

/*! Define a new dimension in a MINC volume.
 */

int 
micreate_dimension(mihandle_t volume, const char *name, midimclass_t class, 
		   midimattr_t attr, unsigned long size, midimhandle_t *new_dim_ptr)
{

  hid_t hdf_file;
  hid_t hdf_mnc_grp;		/* /minc-2.0 */
  hid_t hdf_dims_grp;
  
  int stat;
  
  hsize_t dim[1] = {size};      /* Dataspace dimensions */

  hid_t dataset;
  hid_t space;                 /* Handles */
  hid_t dim_tid;               /* Dimension datatype identifier */
  hid_t h5_enum;
  herr_t status;
  dimension_struct *handle;
  dimension_struct *buff_dim;
  
  if (volume == NULL) {
    return (MI_ERROR);
  }
  
  /* Get a handle to the actual HDF file 
     */
  hdf_file = miget_volume_file_handle(volume);
  if (hdf_file < 0) {
    return (MI_ERROR);
  }
  /* Try opening the ROOT group or create one if 
     it does not exist.
     */
  if ((hdf_mnc_grp = H5Gopen(hdf_file, MI2_ROOT_NAME)) < 0) {
    hdf_mnc_grp = H5Gcreate(hdf_file, MI2_ROOT_NAME, 0);
    stat = H5Gset_comment(hdf_file, MI2_ROOT_NAME, MI2_ROOT_COMMENT);
    if (stat < 0) {
      return (MI_ERROR);
    }
  }

  if (hdf_mnc_grp < 0) {
    return (MI_ERROR);
  }
  
  /* Try opening the DIMENSION group or create one if 
     it does not exist.
     */
  if ((hdf_dims_grp = H5Gopen(hdf_mnc_grp, MI2_DIMS_NAME)) < 0) {
    hdf_dims_grp = H5Gcreate(hdf_mnc_grp, MI2_DIMS_NAME, 0);
    stat = H5Gset_comment(hdf_mnc_grp, MI2_DIMS_NAME, MI2_DIMS_COMMENT);
    if (stat < 0) {
      return (MI_ERROR);
    }
  }
  if (hdf_dims_grp < 0) {
    return (MI_ERROR);
  }
  
  /* Create the data space
   */ 
  space = H5Screate_simple(1,dim, NULL);

  /* Create the memory datatype
   */
  dim_tid = H5Tcreate(H5T_COMPOUND, sizeof(dimension_struct));

  /* Insert individual fields
   */
  H5Tinsert(dim_tid, "name", HOFFSET(dimension_struct, name), H5T_NATIVE_UCHAR);
  
  h5_enum = H5Tenum_create(H5T_NATIVE_UCHAR);
  if (h5_enum >= 0) {
    unsigned char v;
    v = 0;
    H5Tenum_insert(h5_enum, "MI_DIMCLASS_ANY", &v);
    v = 1;
    H5Tenum_insert(h5_enum, "MI_DIMCLASS_SPATIAL", &v);
    v = 2;
    H5Tenum_insert(h5_enum, "MI_DIMCLASS_TIME", &v);
    v = 3;
    H5Tenum_insert(h5_enum, "MI_DIMCLASS_SFREQUENCY", &v);
    v = 4;
    H5Tenum_insert(h5_enum, "MI_DIMCLASS_TFREQUENCY", &v);
    v = 5;
    H5Tenum_insert(h5_enum, "MI_DIMCLASS_USER", &v);
    v = 6;
    H5Tenum_insert(h5_enum, "MI_DIMCLASS_FLAT_RECORD", &v);

    stat = H5Tcommit(dim_tid, "class", h5_enum);
    H5Tclose(h5_enum);
  }
    
  H5Tinsert(dim_tid, "attr", HOFFSET(dimension_struct, attr), H5T_NATIVE_UINT);
  H5Tinsert(dim_tid, "size", HOFFSET(dimension_struct, size), H5T_NATIVE_ULONG);
  H5Tinsert(dim_tid, "cosines", HOFFSET(dimension_struct, cosines), H5T_NATIVE_DOUBLE);
  H5Tinsert(dim_tid, "start", HOFFSET(dimension_struct, start), H5T_NATIVE_ULONG);
  H5Tinsert(dim_tid, "sampling_flag", HOFFSET(dimension_struct, sampling_flag), H5T_NATIVE_HBOOL);
  H5Tinsert(dim_tid, "separation", HOFFSET(dimension_struct, separation), H5T_NATIVE_DOUBLE);
  H5Tinsert(dim_tid, "units", HOFFSET(dimension_struct, units), H5T_NATIVE_UCHAR);
  H5Tinsert(dim_tid, "width", HOFFSET(dimension_struct, width), H5T_NATIVE_DOUBLE);
  H5Tinsert(dim_tid, "volume_handle", HOFFSET(dimension_struct, volume_handle), H5T_NATIVE_INT);
  
  /* Create the dataset
   */
  
  dataset = H5Dcreate(hdf_dims_grp, name, dim_tid, space, H5P_DEFAULT);
  if (dataset < 0) {
    return (MI_ERROR);
  }
 
  /* Write data to the dataset // NOT SURE IF WE NEED TO WRITE ON DISK
   */
  status = H5Dwrite(dataset, dim_tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, buff_dim);
  if (status < 0) {
    return (MI_ERROR);
  }
 
  /* Close/free the handles we created.
     */
  H5Tclose(dim_tid);
  H5Sclose(space);
  H5Dclose(dataset);
  H5Gclose(hdf_dims_grp);
  H5Gclose(hdf_mnc_grp);

  /* Allocate space for the dimension
   */
  handle = (dimension_struct *)malloc(sizeof(*handle));

  if (handle == NULL) {
    return (MI_ERROR);
  }

  /* Initialize everything to zero.
   */
  memset(handle, 0, sizeof(*handle));

  // now set the fields which value is given through parameters
  handle->name = name;
  
  switch (class) {
  case MI_DIMCLASS_ANY:
    handle->class  = 0;
    break;
  case MI_DIMCLASS_SPATIAL:
    handle->class  = 1;
    break;
  case MI_DIMCLASS_TIME:
    handle->class  = 2;
    break;
  case MI_DIMCLASS_SFREQUENCY:
    handle->class  = 3;
    break;
  case MI_DIMCLASS_TFREQUENCY:
    handle->class  = 4;
    break;
  case MI_DIMCLASS_USER:
    handle->class  = 5;
    break;
  case MI_DIMCLASS_FLAT_RECORD:
    handle->class  = 6;
    break;
  default:
    return (MI_ERROR);
  }
  
  handle->attr = attr;
  handle->size = size;
  handle->volume_handle = volume;

  *new_dim_ptr = handle;
  
  return (MI_NOERROR);

}

/*! Delete the dimension definition associated with a given volume.
 */
int 
mifree_dimension_handle(midimhandle_t hdim)
{
  dimension_struct *dim_ptr;
  // WHAT HAPPENS WITH HDF5 STUFF THAT WAS 
  // CREATED BY CALLING micreate_dimension(..)
  if ((dim_ptr = (dimension_struct *) hdim) == NULL) {
    return (MI_ERROR);
  }
  
  if (dim_ptr->volume_handle != NULL) {
    free(dim_ptr);
  }
  else {
    return (MI_ERROR);
  }
  return (MI_NOERROR);
}

/*! Retrieve the list of dimensions defined in a MINC volume, 
    according to their class and attribute.
 */

int 
miget_volume_dimensions(mihandle_t volume, midimclass_t class, midimattr_t attr,
			mivoxel_order_t voxel_order, int array_length, 
			midimhandle_t dimensions[])
{
  hid_t hdf_file;
  hid_t hdf_mnc_grp;	
  hid_t hdf_dims_grp;
  hsize_t num;
  herr_t status;
  int number;
  if (volume == NULL) {
    return (MI_ERROR);
  }
  /* Get a handle to the actual HDF file 
     */
  hdf_file = miget_volume_file_handle(volume);
  if (hdf_file < 0) {
    return (MI_ERROR);
  }
  /* Try opening the ROOT group or create one if 
     it does not exist.
     */
  
  if ((hdf_mnc_grp = H5Gopen(hdf_file, MI2_ROOT_NAME)) < 0) {
    return (MI_ERROR);
  }
  
  /* Try opening the DIMENSION group or create one if 
     it does not exist.
     */

  if ((hdf_dims_grp = H5Gopen(hdf_mnc_grp, MI2_DIMS_NAME)) < 0) {
    return (MI_ERROR);
  }
  if (hdf_dims_grp < 0) {
    return (MI_ERROR);
    }
  /* Figure out how many objects are part of this dimension 
     group.
   */

  status = H5Gget_num_objs(hdf_dims_grp, &num);
  if (status < 0) {
    return (MI_ERROR);
  }
  number = (int) num;
  printf(" %d \n", number);
  return (MI_NOERROR);
  
  }

/*! Set apparent dimension order.
 */
/*
int 
miset_apparent_dimension_order(mihandle_t volume, int array_length, 
			       midimhandle_t dimensions[])
{
}
*/
/*! Set apparent dimension order by name.
 */
/*
int 
miset_apparent_dimension_order_by_name(mihandle_t volume, int array_length, 
				       char **names)
{
}
*/
/*! Set the record flag
 */
/*
int 
miset_apparent_record_dimension_flag(mihandle_t volume, int flatten_flag)
{
}
*/
/*! Get the apparent order of voxels.
 */
/*
int miget_dimension_apparent_voxel_order(midimhandle_t dimension, miflipping_t *file_order,
					 miflipping_t *sign)
{
}
*/
/*! Set the apparent order of voxels.
 */
/*
int 
miset_dimension_apparent_voxel_order(midimhandle_t dimension, miflipping_t flipping_order)
{
}
*/
/*! Get the class of a MINC dimension.
 */
/*
int 
miget_dimension_class(midimhandle_t dimension, midimclass_t *class)
{
}
*/
/*! Set the class of a MINC dimension.
 */
/*
int 
miset_dimension_class(midimhandle_t dimension, midimclass_t class)
{
}
*/
/*! Get the dimension's cosine vector.
 */
/*
int 
miget_dimension_cosines(midimhandle_t dimension, double cosines[3])
{
}
*/
/*! Set the dimension's cosine vector.
 */
/*
int 
miset_dimension_cosines(midimhandle_t dimension, const double cosines[3])
{
}
*/
/*! Get the identifier of a MINC dimension.
 */
/*
int 
miget_dimension_name(midimhandle_t dimension, char **name_ptr)
{
}
*/
/*! Set the identifier of a MINC dimension.
 */
/*
int 
miset_dimension_name(midimhandle_t dimension, const char *name)
{
}
*/
/*! Get the absolute world coordinates of points along a 
    MINC dimension.
 */
/*
int 
miget_dimension_offsets(midimhandle_t dimension, double offsets[],
			unsinged long array_length, unsigned long start_postion)
{
}
*/
/*! Set the absolute world coordinates of points along a 
    MINC dimension.
 */
/*
int 
miset_dimension_offsets(midimhandle_t dimension, const double offsets[],
			unsinged long array_length, unsigned long start_postion)
{
}
*/
/*! Get the sampling flag for a MINC dimension. 
 */
/*
int 
miget_dimension_sampling_flag(midimhandle_t dimension, BOOLEAN *sampling_flag)
{
}
*/
/*! Set the sampling flag for a MINC dimension.
 */
/*
int miset_dimension_sampling_flag(midimhandle_t dimension, BOOLEAN sampling_flag)
{
}
*/
/*! Get the sampling interval for a single dimension.
 */
/*
int 
miget_dimension_separation(midimhandle_t dimension, mivoxel_order_t voxel_order, 
			   double *separation_ptr)
{
}
*/
/*! Set the sampling interval for a single dimension.
 */
/*
int miset_dimension_separation(midimhandle_t dimension, mivoxel_order_t voxel_order,
			       double separation_ptr)
{
}
*/
/*! Get the sampling interval for a list of dimensions.
 */
/*
int 
miget_dimension_separations(const midimhandle_t dimensions[], mivoxel_order_t voxel_order,
			    int array_length, double separations[])
{
}
*/
/*! Set the sampling interval for a list of dimensions.
 */
/*
int 
miset_dimension_separations(const midimhandle_t dimensions[], mivoxel_order_t voxel_order,
			    int array_length, const double separations[])
{
}
*/
/*! Get the length of a MINC dimension.
 */
/*
int 
miget_dimension_size(midimhandle_t dimension, unsigned long *size_ptr)
{
}
*/
/*! Set the length of a MINC dimension.
 */
/*
int 
miset_dimension_size(midimhandle_t dimension, unsigned long size)
{
}
*/
/*! Retrieve the sizes of an array of dimension handles.
 */
/*
int 
miget_dimension_sizes(const midimhandle_t dimensions[], int array_length,
			  unsigned long sizes[])
{
}
*/
/*! Get the origin of a MINC dimension.
 */
/*
int 
miget_dimension_start(midimhandle_t dimension, mivoxel_order_t voxel_order,
		      double *start_ptr)
{
}
*/
/*! Set the origin of a MINC dimension.
 */
/*
int 
miset_dimension_start(midimhandle_t dimension, mivoxel_order_t voxel_order,
		      double start_prt)
{
}
*/
/*! Get the start values for a MINC dimension.
 */
/*
int 
miget_dimension_starts(const midimhandle_t dimensions[], mivoxel_order_t voxel_order,
		       int array_length, double starts[])
{
}
*/
/*! Set the start values for a MINC dimension.
 */
/*
int 
miset_dimension_starts(const midimhandle_t dimensions[], mivoxel_order_t voxel_order,
		       int array_length, const double starts[])
{
}
*/
/*! Get the unit string for a MINC dimension.
 */
/*
int 
miget_dimension_units(midimhandle_t dimension, char **units_ptr)
{
}
*/
/*! Set the unit string for a MINC dimension.
 */
/*
int 
miset_dimension_units(midimhandle_t dimension, const char *units)
{
}
*/
/*! Get the full-width half-maximum value for points along a 
    MINC dimension.
 */
/*
int 
miget_dimension_width(midimhandle_t dimension, mivoxel_order_t voxel_order,
		      double *width_ptr)
{
}
*/
/*! Get the full-width half-maximum value for points along a 
    MINC dimension.
 */
/*
int 
miset_dimension_width(midimhandle_t dimension, mivoxel_order_t voxel_order,
		      double width_ptr)
{
}
*/
/*! Get the full-width half-maximum value for points along all 
    MINC dimensions.
 */
/*
int 
miget_dimension_widths(midimhandle_t dimension, mivoxel_order_t voxel_order,
		       unsigned long array_length, unsigned long start_position,
		       double widths[])
{
}
*/
/*! Set the full-width half-maximum value for points along all 
    MINC dimensions.
 */
/*
int 
miset_dimension_widths(midimhandle_t dimension, mivoxel_order_t voxel_order,
		       unsigned long array_length, unsigned long start_position,
		       const double widths[])
{
}
*/

#ifdef M2_TEST
#define TESTRPT(msg, val) (error_cnt++, fprintf(stderr, \
                                  "Error reported on line #%d, %s: %d\n", \
                                  __LINE__, msg, val))

static int error_cnt = 0;


int main(int argc, char **argv)
{
  mihandle_t vol, vol1;
  int r;
  hid_t file_id, file_id1;
  midimhandle_t dimh, dimh1, dim_copy, dimh2;
  hid_t g1_id;
  dimension_struct *handle; 
  midimhandle_t dimensions[3];
   /* Turn off automatic error reporting - we'll take care of this
   * ourselves, thanks!
   */
  H5Eset_auto(NULL, NULL);
 
  
  file_id = H5Fcreate("test.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  file_id1 = H5Fcreate("test.h6", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

  if (file_id < 0) {
    fprintf(stderr, "Unable to create test file %x", file_id);
    return (-1);
  }

  vol = (mihandle_t) file_id;
  vol1= (mihandle_t) file_id1;
  g1_id = H5Gcreate(file_id, "minc-2.0", 0);
  /* Close the things we created. */
  H5Gclose(g1_id);

  r = micreate_dimension(vol,"MIxspace",MI_DIMCLASS_SPATIAL,2, 10,&dimh1);
  if (r < 0) {
    TESTRPT("failed", r);
  }

  r = micreate_dimension(vol,"MIyspace",MI_DIMCLASS_SPATIAL,2, 12,&dimh);
  if (r < 0) {
    TESTRPT("failed", r);
  }
  r = micreate_dimension(vol,"MIzspace",MI_DIMCLASS_SPATIAL,2, 12,&dimh2);
  if (r < 0) {
    TESTRPT("failed", r);
  }
  r = micreate_dimension(vol1,"MIzspace",MI_DIMCLASS_SPATIAL,2, 12,&dimh2);
  r = micopy_dimension(dimh,&dim_copy);
  if (r < 0) {
    TESTRPT("failed", r);
  }
  
  r = mifree_dimension_handle(dim_copy);
  if (r < 0) {
    TESTRPT("failed", r);
  }

  r = miget_volume_from_dimension(dimh, &vol);
  if (r < 0) {
    TESTRPT("failed", r);
  }
  r = miget_volume_dimensions(vol,MI_DIMCLASS_SPATIAL, MI_DIMATTR_ALL, MI_ORDER_FILE,3,dimensions);
  
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
