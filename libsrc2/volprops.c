/** \file volprops.c
 * \brief MINC 2.0 Volume properties functions
 * \author Leila Baghdadi
 * 
 * These functions manipulate "volume properties" objects, which are
 * used to control several options related to MINC 2.0 volume structure.
 * These include compression, blocking, multi-resolution, and record s
 * structure.
 *
 * This approach was adopted with the intent that it would make the 
 * default volume creation as simple as possible, while allowing a
 * lot of control for more advanced applications.  This approach to 
 * managing properties is also believed to be more readily extensible than 
 * any obvious alternative.
 ************************************************************************/
#include <stdlib.h>
#include <hdf5.h>
#include "minc2.h"
#include "minc2_private.h"


/*! Create a volume property list.
 */
int
minew_volume_props(mivolumeprops_t  *props)
{
  volprops *handle;
  
  handle = (volprops *)malloc(sizeof(*handle));
  
  if (handle == NULL) {
    return (MI_ERROR);
  }
  /* Initialize all the fields.
   */
  handle->enable_flag = FALSE;
  handle->depth = 0;
  handle->compression_type = MI_COMPRESS_NONE;
  handle->zlib_level = 0;
  handle->edge_count = 0;
  handle->edge_lengths = NULL;
  handle->max_lengths = 0;
  handle->record_length = 0;
  handle->record_name = NULL;
  handle->template_flag = 0;

  *props = handle;

  return (MI_NOERROR);
}

/*! Destroy a volume property list.
 */
int
mifree_volume_props(mivolumeprops_t props)
{
  if (props == NULL) {
    return (MI_ERROR);
  }
  if (props->edge_lengths != NULL) {
    free(props->edge_lengths);
  }
  if (props->record_name != NULL) {
    free(props->record_name);
  }
  free(props);
  return (MI_NOERROR);
}

/*! Get a copy of the volume property list
 */
int
miget_volume_props(mihandle_t volume, mivolumeprops_t *props)
{
  volprops *handle;
  hid_t hdf_vol_dataset;
  hid_t hdf_plist;
  int nfilters;
  unsigned int flags;
  size_t cd_nelmts;
  unsigned int cd_values[MAX_CD_ELEMENTS];
  char fname[100];
  int fcode;
                    
  if (volume->hdf_id < 0) {
    return (MI_ERROR);
  }
  hdf_vol_dataset = midescend_path(volume->hdf_id, "/minc-2.0/image/0/image");
  if (hdf_vol_dataset < 0) {
    return (MI_ERROR);
  }
  hdf_plist = H5Dget_create_plist(hdf_vol_dataset);
  if (hdf_plist < 0) {
    return (MI_ERROR);
  }
  handle = (volprops *)malloc(sizeof(*handle));
  if (handle == NULL) {
    return (MI_ERROR);
  }
  /* Get the layout of the raw data for a dataset.
   */
  if (H5Pget_layout(hdf_plist) == H5D_CHUNKED) {
      hsize_t dims[MI2_MAX_VAR_DIMS];
      int i;
      /* Returns chunk dimensionality */
      handle->edge_count = H5Pget_chunk(hdf_plist, MI2_MAX_VAR_DIMS, dims);
      if (handle->edge_count < 0) {
	return (MI_ERROR);
      }
      handle->edge_lengths = (int *)malloc(handle->edge_count*sizeof(int));
      if (handle->edge_lengths == NULL) {
	  return (MI_ERROR);
      }
      for (i = 0; i < handle->edge_count; i++) {
	  handle->edge_lengths[i] = dims[i];
      }
      /* Get the number of filters in the pipeline */
      nfilters = H5Pget_nfilters(hdf_plist);
      if (nfilters == 0) {
	handle->zlib_level = 0;
	handle->compression_type = MI_COMPRESS_NONE;
      }
      else {
	for (i = 0; i < nfilters; i++) {
	  cd_nelmts = MAX_CD_ELEMENTS;
          fcode = H5Pget_filter(hdf_plist, i, &flags, &cd_nelmts,
                                cd_values, sizeof(fname), fname);
	  switch (fcode) {
	  case H5Z_FILTER_DEFLATE:
	    printf("Deflate");
	    break;
          case H5Z_FILTER_SHUFFLE:
	    /* NOT SURE WHAT TO DO */
	    printf("Shuffle");
	    break;
          case H5Z_FILTER_FLETCHER32:
	    /* NOT SURE WHAT TO DO */
	    printf("Fletcher32");
	    break;
          case H5Z_FILTER_SZIP:
	    printf("SZip");
	    break;
          default:
	    printf("Unknown(%d)", fcode);
	    break;
	  }
	}
      }
  }
  else {
      handle->edge_count = 0;
      handle->edge_lengths = NULL;
      handle->zlib_level = 0;
      handle->compression_type = MI_COMPRESS_NONE;
  }
  
  *props = handle;
   
  H5Pclose(hdf_plist);
  H5Dclose(hdf_vol_dataset);

  return (MI_NOERROR);

}


/*! Set Mutli-resolution properties
 */
int
miset_props_multi_resolution(mivolumeprops_t props, BOOLEAN enable_flag,
			    int depth)
{
  int i;
  if (props == NULL || depth > MAX_RESOLUTION_GROUP || depth <= 0) {
    return (MI_ERROR);
  }
  
  if (enable_flag){
    props->enable_flag = enable_flag;
    props->depth = depth;
  }
  else {
    return (MI_ERROR);
  }
  return (MI_NOERROR);
}
  
/*! Get Mutli-resolution properties
 */
int 
miget_props_multi_resolution(mivolumeprops_t props, BOOLEAN *enable_flag,
			     int *depth)
{
  if (props == NULL) {
    return (MI_ERROR);
  }
  
  *enable_flag = props->enable_flag;
  *depth = props->depth;

  return (MI_NOERROR);
}

/*! Compute a different resolution
 */
int
miselect_resolution(mihandle_t volume, int depth)
{
  hid_t grp_id;
  
  if ( volume->hdf_id < 0 || depth > MAX_RESOLUTION_GROUP || depth < 0) {
    return (MI_ERROR);
  }
  grp_id = H5Gopen(volume->hdf_id, "/minc-2.0/image");
  if (grp_id < 0) {
    return (MI_ERROR);
  }
  /* Check given depth with the available depth in file.
     Make sure the selected resolution does exist.
   */
  if (depth > volume->create_props->depth) {
    printf(" THIS RESOLUTION DOES NOT EXIST!!! \n");
    return (0);
  }
  else {
    if (minc_update_thumbnail(grp_id,0 ,depth) < 0) {
      return (MI_ERROR);
    }
  }

 return (MI_NOERROR);
}

/*! Compute all resolution
 */
int
miflush_from_resolution(mihandle_t volume, int depth)
{
  int i;
  
  if ( volume->hdf_id < 0 || depth > MAX_RESOLUTION_GROUP || depth <= 0) {
    return (MI_ERROR);
  }
  
  if (depth > volume->create_props->depth) {
    printf(" THIS RESOLUTION DOES NOT EXIST!!! \n");
    return (0);
  }
  else {
    if (minc_update_thumbnails(volume->hdf_id) < 0) {
      return (MI_ERROR);
    }
  }
  
 return (MI_NOERROR);
}

/*! Set compression type for a volume property list
    Note that enabling compression will automatically 
    enable blocking with default parameters. 
 */
int
miset_props_compression_type(mivolumeprops_t props, micompression_t compression_type)
{
    int i;
    int edge_lengths[MI2_MAX_VAR_DIMS];

 if (props == NULL) {
    return (MI_ERROR);
  }
 switch (compression_type) {
 case MI_COMPRESS_NONE:
   props->compression_type = MI_COMPRESS_NONE;
   break;
 case MI_COMPRESS_ZLIB:
   props->compression_type = MI_COMPRESS_ZLIB;
   props->zlib_level = MI2_DEFAULT_ZLIB_LEVEL;
   for (i = 0; i < MI2_MAX_VAR_DIMS; i++) {
       edge_lengths[i] = MI2_CHUNK_SIZE;
   }
   miset_props_blocking(props, MI2_MAX_VAR_DIMS, edge_lengths);
   break;
 default:
   return (MI_ERROR);
 }
 return (MI_NOERROR);
}

/*! Get compression type for a volume property list
 */
int 
miget_props_compression_type(mivolumeprops_t props, micompression_t *compression_type)
{

  if (props == NULL) {
    return (MI_ERROR);
  }
  
  *compression_type = props->compression_type;
  return (MI_NOERROR);
}

/*! Set zlib compression properties for a volume list
 */
int
miset_props_zlib_compression(mivolumeprops_t props, int zlib_level)
{
  if (props == NULL || zlib_level > MAX_ZLIB_LEVEL) {
    return (MI_ERROR);
  }
  
  props->zlib_level = zlib_level;
  return (MI_NOERROR);
}

/*! Get zlib compression properties for a volume list
 */
int
miget_props_zlib_compression(mivolumeprops_t props, int *zlib_level)
{
if (props == NULL) {
    return (MI_ERROR);
  }
  
  *zlib_level = props->zlib_level;
  return (MI_NOERROR);
}

/*! Set blocking structure properties for the volume
 */
int
miset_props_blocking(mivolumeprops_t props, int edge_count, const int *edge_lengths)
{
  int i;
  
  if (props == NULL || edge_count > MI2_MAX_VAR_DIMS) {
    return (MI_ERROR);
  }
  
  if (props->edge_lengths != NULL) {
      free(props->edge_lengths);
      props->edge_lengths = NULL;
  }
  
  props->edge_count = edge_count;
  if (edge_count != 0) {
      props->edge_lengths = (int *) malloc(edge_count*sizeof(int));
      if (props->edge_lengths == NULL) {
	  return (MI_ERROR);
      }
      for (i=0; i< edge_count; i++){
	  props->edge_lengths[i] = edge_lengths[i];
      }
  }

  return (MI_NOERROR);
}

/*! Get blocking structure properties for the volume
 * \param props The properties structure from which to get the information
 * \param edge_count Returns the number of edges (dimensions) in a block
 * \param edge_lengths The lengths of the edges
 * \param max_lengths The number of elements of the edge_lengths array
 */
int
miget_props_blocking(mivolumeprops_t props, int *edge_count, int *edge_lengths,
		     int max_lengths)
{
  int i; 
  
  if (props == NULL) {
    return (MI_ERROR);
  }
  *edge_count = props->edge_count;
  /* If max_lengths is greater than the actual edge count, reduce max_lengths
   * to the edge_count
   */
  if (max_lengths > props->edge_count) {
      max_lengths = props->edge_count;
  }
  edge_lengths = (int *) malloc(max_lengths *sizeof(int));
  for (i=0; i< max_lengths; i++){
    edge_lengths[i] = props->edge_lengths[i];
  }
  
  return (MI_NOERROR);
}

/*! Set properties for uniform/nonuniform record dimension
 */
int 
miset_props_record(mivolumeprops_t props, long record_length, char *record_name)
{
  if (props == NULL) {
    return (MI_ERROR);
  }
  if (record_length > 0) {
    props->record_length = record_length;
  }
  if (props->record_name != NULL) {
      free(props->record_name);
      props->record_name = NULL;
  }
  
  props->record_name = strdup(record_name);
  
  return (MI_NOERROR);
}
  
/*! Set the template volume flag
 */ 
int
miset_props_template(mivolumeprops_t props, int template_flag)
{
  if (props == NULL) {
    return (MI_ERROR);
  }
  
  props->template_flag = template_flag;
  return (MI_NOERROR);
}


#ifdef M2_TEST
#define TESTRPT(msg, val) (error_cnt++, fprintf(stderr, \
                                  "Error reported on line #%d, %s: %d\n", \
                                  __LINE__, msg, val))

static int error_cnt = 0;


int main(int argc, char **argv)
{
  mihandle_t vol;
  mivolumeprops_t  props;
  int r;
  micompression_t compression_type;
  BOOLEAN enable_flag;
  int zlib_level;
  int depth;
  int edge_lengths[MI2_MAX_VAR_DIMS];
  int edge_count;
  int i;
 /* Turn off automatic error reporting - we'll take care of this
   * ourselves, thanks!
   */
  H5Eset_auto(NULL, NULL);
  r = minew_volume_props(&props);

  if (r < 0) {
    TESTRPT("failed", r);
  }
  
  r = miset_props_multi_resolution(props, 1 , 2);

  if (r < 0) {
    TESTRPT("failed", r);
   }

  r = miget_props_multi_resolution(props, &enable_flag, &depth);
  if (r < 0) {
    TESTRPT("failed", r);
  }
  else {
    printf("%d %d \n", enable_flag, depth);
  }

  r = miset_props_compression_type(props, MI_COMPRESS_NONE);
  if (r < 0) {
    TESTRPT("failed", r);
  }

  r = miget_props_compression_type(props,&compression_type);
  if (r < 0) {
    TESTRPT("failed", r);
  }
  else {
    printf("%d \n", compression_type);
 }  

  r = miset_props_zlib_compression(props,4);
  if (r < 0) {
    TESTRPT("failed", r);
  }
  else {
    printf(" %d \n",props->zlib_level);
  }

  r = miget_props_zlib_compression(props,&zlib_level);
  if (r < 0) {
    TESTRPT("failed", r);
  }
  else {
    printf(" %d \n",zlib_level);
  }

  mifree_volume_props(props);

  while (--argc > 0) {
      r = miopen_volume(*++argv, MI2_OPEN_RDWR, &vol);
      if (r < 0) {
	  TESTRPT("failed", r);
      }
      r = miget_volume_props(vol, &props);
      if (r < 0) {
	  TESTRPT("failed", r);
      }
      r = miget_props_blocking(props, &edge_count, edge_lengths, 
			       MI2_MAX_VAR_DIMS);
      if (r < 0) {
	  TESTRPT("failed", r);
      }
      printf("edge_count %d\n", edge_count);
      for (i = 0; i < edge_count; i++) {
	  printf("  %d", edge_lengths[i]);
      }
      printf("\n");
      mifree_volume_props(props);
      miclose_volume(vol);
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
