/** The root of all MINC 2.0 objects in the HDF5 hierarchy.
 */
#define MI_ROOT_PATH "/minc-2.0"

#define MI2_3D 3
#define MI2_X 0
#define MI2_Y 1
#define MI2_Z 2

/** Size of a linear transform */
#define MI2_LIN_XFM_SIZE 4

/** The fixed path to the full-resolution image data.
 */
#define MI_FULLIMAGE_PATH MI_ROOT_PATH "/image/1"

struct mivolume {
    hid_t hdf_id;
    BOOLEAN has_slice_scaling;
};

typedef struct mivolume mivolume_struct;
typedef struct mivolume *mivolume_ptr;

struct volprops_struct{
  BOOLEAN enable_flag;
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

struct dimension {
  const char *name;
  midimclass_t class;
  midimattr_t attr;
  unsigned long size;
  double cosines[3];
  unsigned long start;
  BOOLEAN sampling_flag;
  double separation;
  const char *units;
  double width;  
  mihandle_t *volume_handle;
};

typedef struct dimension dimension_struct;

/** Standard linear transform, a 4x4 matrix.
 */
typedef double mi_lin_xfm_t[MI2_LIN_XFM_SIZE][MI2_LIN_XFM_SIZE];

typedef long long mi_i64_t;

extern hid_t midescend_path(hid_t file_id, const char *path);
extern hid_t  miget_volume_file_handle(mihandle_t volume);
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


