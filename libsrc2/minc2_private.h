/** The root of all MINC 2.0 objects in the HDF5 hierarchy.
 */
#define MI_ROOT_PATH "/minc-2.0"
//#define MI_ROOT_COMMENT "Root of the MINC 2.0 data hierarchy"
//#define MI_INFO_NAME "info"
//#define MI_INFO_COMMENT "Group holding directly accessible attributes"
#define MI_DIMENSIONS_PATH "dimensions"
//#define MI_DIMS_COMMENT "Group holding dimension variables"

#define MI2_3D 3
#define MI2_X 0
#define MI2_Y 1
#define MI2_Z 2

/** Size of a linear transform */
#define MI2_LIN_XFM_SIZE 4

/** The fixed path to the full-resolution image data.
 */
#define MI_FULLIMAGE_PATH MI_ROOT_PATH "/image/0"

/** The fixed path to the dimension 
 */
#define MI_FULLDIMENSIONS_PATH MI_ROOT_PATH "/dimensions"

/*! Volume properties  
 */
struct volprops_struct{
  BOOLEAN enable_flag; //enable multi-res 
  int depth;
  micompression_t compression_type;
  int zlib_level; 
  int edge_count;
  int *edge_lengths;
  int max_lengths;
  long record_length;
  char *record_name;
  int  template_flag;
}; 

/*! Dimension handle  
 */
struct dimension_struct{
  midimattr_t attr;
  midimclass_t class;
  double cosines[3];
  miflipping_t flipping_order;
  char *name;
  double *offsets;
  BOOLEAN sampling_flag;
  double separation;
  unsigned long size;
  double start;
  char *units;
  double width; 
  double *widths;
  mihandle_t volume_handle;
};

/*! Volume handle  
 */
struct volumehandle_struct {
  hid_t hdf_id;
  BOOLEAN has_slice_scaling;
  int number_of_dims;
  midimhandle_t *dim_handles;  // file order of dimensions
  int *dim_indices; // apparent order of dimensions
  mitype_t volume_type;
  miclass_t volume_class;
  mivolumeprops_t create_props;
};

/** Standard linear transform, a 4x4 matrix.
 */
typedef double mi_lin_xfm_t[MI2_LIN_XFM_SIZE][MI2_LIN_XFM_SIZE];

typedef long long mi_i64_t;

extern hid_t midescend_path(hid_t file_id, const char *path);
extern hid_t miget_volume_file_handle(mihandle_t volume);
extern hid_t mitype_to_hdftype(mitype_t);
extern int mitype_to_nctype(mitype_t, int *is_signed);

extern mi_i64_t miget_voxel_count(int mincid);
extern double miget_voxel_volume(int mincid);
extern void miget_attribute(int mincid, char *varname, char *attname,
                            int maxvals, double vals[]);
extern int miget_dim_count(int mincid);
extern void mifind_spatial_dims(int mincid, int space_to_dim[], int dim_to_space[]);
extern void miget_voxel_to_world(int mincid, mi_lin_xfm_t voxel_to_world);
extern void minormalize_vector(double vector[]);
extern void mitransform_coord(double out_coord[],
                              mi_lin_xfm_t transform,
                              double in_coord[]);

extern void miinit(void);
