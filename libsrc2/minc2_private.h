
/** The root of all MINC 2.0 objects in the HDF5 hierarchy.
 */
#define MI_ROOT_PATH "/minc-2.0"
#define MI_ROOT_COMMENT "Root of the MINC 2.0 data hierarchy"

#define MI_DIMAGE_PATH "image"


#define MI_INFO_NAME "info"
#define MI_INFO_COMMENT "Group holding directly accessible attributes"

#define MI_DIMENSIONS_PATH "dimensions"
#define MI_DIMS_COMMENT "Group holding dimension variables"

/** The fixed path to the full-resolution image data.
 */
#define MI_IMAGE_PATH MI_ROOT_PATH "/" MI_DIMAGE_PATH
#define MI_FULLIMAGE_PATH MI_IMAGE_PATH "/0"

/** The fixed path to the dimension 
 */
#define MI_FULLDIMENSIONS_PATH MI_ROOT_PATH "/dimensions"


/** World spatial coordinates should always have this structure.
 */
#define MI2_3D 3
#define MI2_X 0
#define MI2_Y 1
#define MI2_Z 2

/** Size of a linear transform */
#define MI2_LIN_XFM_SIZE 4

/** The fixed path to the full-resolution image data.
 */
//#define MI_FULLIMAGE_PATH MI_ROOT_PATH "/image/0"

/** Standard linear transform, a 4x4 matrix.
 */
typedef double mi_lin_xfm_t[MI2_LIN_XFM_SIZE][MI2_LIN_XFM_SIZE];

typedef long long mi_i64_t;

/** The fixed path to the dimension 
 */
#define MI_FULLDIMENSIONS_PATH MI_ROOT_PATH "/dimensions"

/*! Volume properties  
 */
struct volprops_struct{
  BOOLEAN enable_flag; //enable multi-res 
  int depth; //multi-res depth
  micompression_t compression_type;
  int zlib_level; 
  int edge_count; //how many chunks
  int *edge_lengths; //size of each chunk
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
  double direction_cosines[3];
  miflipping_t flipping_order;
  char *name;
  double *offsets;
  double step;
  unsigned long length;
  double start;
  char *units;
  double width; 
  double *widths;
  char *comments;
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
  double valid_min;             /* Volume-wide valid min */
  double valid_max;             /* Volume-wide valid max */
  mi_lin_xfm_t v2w_transform;   /* Voxel-to-world transform */
  mi_lin_xfm_t w2v_transform;   /* World-to-voxel transform (inverse) */
  int selected_resolution;      /* The current resolution (0-N) */
  int mode;                     /* Open mode */
  hid_t type_id;                /* Type ID if record or label type. */
  hid_t plist_id;               /* Image property list */
  double scale_min;             /* Global minimum */
  double scale_max;             /* Global maximum */
};

extern hid_t midescend_path(hid_t file_id, const char *path);
extern hid_t mitype_to_hdftype(mitype_t);
extern int mitype_to_nctype(mitype_t, int *is_signed);

extern int miget_attribute(mihandle_t volume, const char *varpath, 
                           const char *attname, mitype_t data_type, 
                           int maxvals, void *values);
extern int miset_attr_at_loc(hid_t hdf_loc, const char *attname, 
                             mitype_t data_type, 
                             int maxvals, const void *values);
extern int miset_attribute(mihandle_t volume, const char *varpath, 
                           const char *attname, mitype_t data_type, 
                           int maxvals, const void *values);
extern void mifind_spatial_dims(int mincid, int space_to_dim[], int dim_to_space[]);
extern void miget_voxel_to_world(mihandle_t volume, mi_lin_xfm_t voxel_to_world);
extern void minormalize_vector(double vector[]);
extern void mitransform_coord(double out_coord[],
                              mi_lin_xfm_t transform,
                              const double in_coord[]);
extern int miinvert_transform(mi_lin_xfm_t transform, mi_lin_xfm_t inverse);

extern void miinit(void);
extern void miinit_enum(hid_t);

extern int mitranslate_hyperslab_origin(mihandle_t volume, 
                                        const long start[], 
                                        const long count[],
                                        hsize_t hdf_start[],
                                        hsize_t hdf_count[],
                                        int dir[]);
extern int miget_scalar(hid_t loc_id, hid_t type_id, const char *path, 
                        void *data);
