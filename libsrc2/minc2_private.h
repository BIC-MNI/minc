/** \internal
 * \file minc2_private.h
 * \brief MINC 2.0 private constants, types, and functions.
 */
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

/** The fixed path to the full-resolution image data.
 */
#define MI_FULLIMAGE_PATH MI_IMAGE_PATH "/0"

/** The fixed path to the dimension 
 */
#define MI_FULLDIMENSIONS_PATH MI_ROOT_PATH "/dimensions"


/** Size of a linear transform */
#define MI2_LIN_XFM_SIZE 4

/** Standard linear transform, a 4x4 matrix.
 */
typedef double mi_lin_xfm_t[MI2_LIN_XFM_SIZE][MI2_LIN_XFM_SIZE];

#ifdef _WIN32
typedef __int64 mi_i64_t;
#else //_WIN32
typedef long long mi_i64_t;
#endif //_WIN32

/** The fixed path to the dimension 
 */
#define MI_FULLDIMENSIONS_PATH MI_ROOT_PATH "/dimensions"

/** \internal
 * Volume properties  
 */
struct mivolprops {
    miboolean_t enable_flag;        /* enable multi-res */
    int depth;                  /* multi-res depth */
    micompression_t compression_type;
    int zlib_level; 
    int edge_count;             /* how many chunks */
    int *edge_lengths;          /* size of each chunk */
    int max_lengths;
    long record_length;
    char *record_name;
    int  template_flag;
}; 

/** \internal
 * Dimension handle  
 */
struct midimension {
  midimattr_t attr;             /* Dimension attributes */
  midimclass_t class;           /* Dimension class */
  double direction_cosines[3];  /* Direction cosines */
  miflipping_t flipping_order;
  char *name;                   /* Dimension name */
  double *offsets;              /* Offsets (if irregular) */
  double step;                  /* Step size */
  unsigned long length;         /* Length */
  double start;                 /* Start value */
  char *units;                  /* Units string */
  double width;                 /* Sample width (if regular) */
  double *widths;               /* Widths (if irregular) */
  char *comments;               /* Comment string */
  mihandle_t volume_handle;     /* Handle of associated volume */
  short world_index;            /* -1, MI2_X, MI2_Y, or MI2_Z */
};

/** \internal
 * Volume handle  
 */
struct mivolume {
  hid_t hdf_id;
  miboolean_t has_slice_scaling;
  int number_of_dims;
  midimhandle_t *dim_handles;   /* file order of dimensions */
  int *dim_indices;             /* apparent order of dimensions */
  mitype_t volume_type;
  miclass_t volume_class;
  mivolumeprops_t create_props;
  double valid_min;             /* Volume-wide valid min */
  double valid_max;             /* Volume-wide valid max */
  mi_lin_xfm_t v2w_transform;   /* Voxel-to-world transform */
  mi_lin_xfm_t w2v_transform;   /* World-to-voxel transform (inverse) */
  int selected_resolution;      /* The current resolution (0-N) */
  int mode;                     /* Open mode */
  hid_t ftype_id;               /* File type ID of image. */
  hid_t mtype_id;               /* Memory type ID of image. */
  hid_t plist_id;               /* Image property list */
  hid_t image_id;               /* Dataset for image */
  hid_t imax_id;                /* Dataset for image-max */
  hid_t imin_id;                /* Dataset for image-min */
  double scale_min;             /* Global minimum */
  double scale_max;             /* Global maximum */
  miboolean_t is_dirty;             /* TRUE if data has been modified. */
};

/**
 * \internal
 * "semi-private" functions.
 ****************************************************************************/
/* From m2util.c */
extern hid_t midescend_path(hid_t file_id, const char *path);
extern hid_t mitype_to_hdftype(mitype_t, int);
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

extern int miget_scalar(hid_t loc_id, hid_t type_id, const char *path, 
                        void *data);

extern int minc_create_thumbnail(mihandle_t volume, int grp);
extern int minc_update_thumbnail(mihandle_t volume, hid_t loc_id, int igrp, int ogrp);
extern int minc_update_thumbnails(mihandle_t volume);

extern int scaled_maximal_pivoting_gaussian_elimination(int   n,
                                                        int   row[],
                                                        double **a,
                                                        int   n_values,
                                                        double **solution );

extern int scaled_maximal_pivoting_gaussian_elimination_real(int n,
                                                             double **coefs,
                                                             int n_values,
                                                             double **values );

extern double *alloc1d(int);
extern double **alloc2d(int, int);
extern void free2d(int, double **);

/* From hyper.c */
extern int mitranslate_hyperslab_origin(mihandle_t volume, 
                                        const unsigned long start[], 
                                        const unsigned long count[],
                                        hssize_t hdf_start[],
                                        hsize_t hdf_count[],
                                        int dir[]);
/* From volume.c */
extern void misave_valid_range(mihandle_t volume);

/* External */
#include "../libsrc/minc_private.h"


