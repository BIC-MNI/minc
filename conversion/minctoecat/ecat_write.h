#include <stdio.h> 
#include <stdlib.h> 

typedef enum { ECAT6, ECAT7, Interfile } FileFormat;

typedef enum {
  MAT_OK,
  MAT_READ_ERROR,
  MAT_WRITE_ERROR,
  MAT_INVALID_DIRBLK,
  MAT_ACS_FILE_NOT_FOUND,
  MAT_INTERFILE_OPEN_ERR,
  MAT_FILE_TYPE_NOT_MATCH,
  MAT_READ_FROM_NILFPTR,
  MAT_NOMHD_FILE_OBJECT,
  MAT_NIL_SHPTR,
  MAT_NIL_DATA_PTR,
  MAT_MATRIX_NOT_FOUND,
  MAT_UNKNOWN_FILE_TYPE,
  MAT_ACS_CREATE_ERR,
  MAT_BAD_ATTRIBUTE,
  MAT_BAD_FILE_ACCESS_MODE,
  MAT_INVALID_DIMENSION,
  MAT_NO_SLICES_FOUND,
  MAT_INVALID_DATA_TYPE,
  MAT_INVALID_MBED_POSITION
} MatrixErrorCode;

typedef enum {
	NoData, Sinogram, PetImage, AttenCor, Normalization,
	PolarMap, ByteVolume, PetVolume, ByteProjection,
	PetProjection, ByteImage, Short3dSinogram, Byte3dSinogram, Norm3d,
	Float3dSinogram,InterfileImage, NumDataSetTypes
} DataSetType;

typedef enum {
  UnknownMatDataType, ByteData, VAX_Ix2, VAX_Ix4,
  VAX_Rx4, IeeeFloat, SunShort, SunLong, NumMatrixDataTypes, ColorData,
  BitData
} MatrixDataType;

MatrixErrorCode matrix_errno;

char matrix_errtxt[132];

typedef struct XMAIN_HEAD {
  char magic_number[14];
  char original_file_name[32];
  short sw_version;
  short system_type;
  short file_type;
  char serial_number[10];
  short align_0;						/* 4 byte alignment purpose */
  unsigned int scan_start_time;
  char isotope_code[8];
  float isotope_halflife;
  char radiopharmaceutical[32];
  float gantry_tilt;
  float gantry_rotation;
  float bed_elevation;
  float intrinsic_tilt;
  short wobble_speed;
  short transm_source_type;
  float distance_scanned;
  float transaxial_fov;
  short angular_compression;
  short coin_samp_mode;
  short axial_samp_mode;
  short align_1;
  float calibration_factor;
  short calibration_units;
  short calibration_units_label;
  short compression_code;
  char study_name[12];
  char patient_id[16];
  char patient_name[32];
  char patient_sex[1];
  char patient_dexterity[1];
  float patient_age;
  float patient_height;
  float patient_weight;
  int patient_birth_date;
  char physician_name[32];
  char operator_name[32];
  char study_description[32];
  short acquisition_type;
  short patient_orientation;
  char facility_name[20];
  short num_planes;
  short num_frames;
  short num_gates;
  short num_bed_pos;
  float init_bed_position;
  float bed_offset[15];
  float plane_separation;
  short lwr_sctr_thres;
  short lwr_true_thres;
  short upr_true_thres;
  char user_process_code[10];
  short acquisition_mode;
  short align_2;
  float bin_size;
  float branching_fraction;
  unsigned int dose_start_time;
  float dosage;
  float well_counter_factor;
  char data_units[32];
  short septa_state;
  short align_3;
} Main_header;

typedef struct XIMAGE_SUB {
  short data_type;
  short num_dimensions;
  short x_dimension;
  short y_dimension;
  short z_dimension;
  short align_0;
  float z_offset;
  float x_offset;
  float y_offset;
  float recon_zoom;
  float scale_factor;
  short image_min;
  short image_max;
  float x_pixel_size;
  float y_pixel_size;
  float z_pixel_size;
  unsigned int frame_duration;
  unsigned int frame_start_time;
  short filter_code;
  short align_1;
  float x_resolution;
  float y_resolution;
  float z_resolution;
  float num_r_elements;
  float num_angles;
  float z_rotation_angle;
  float decay_corr_fctr;
  int processing_code;
  unsigned int gate_duration;
  int r_wave_offset;
  int num_accepted_beats;
  float filter_cutoff_frequency;
  float filter_resolution;
  float filter_ramp_slope;
  short filter_order;
  short align_2;
  float filter_scatter_fraction;
  float filter_scatter_slope;
  char annotation[40];
  float mt_1_1;
  float mt_1_2;
  float mt_1_3;
  float mt_2_1;
  float mt_2_2;
  float mt_2_3;
  float mt_3_1;
  float mt_3_2;
  float mt_3_3;
  float rfilter_cutoff;
  float rfilter_resolution;
  short rfilter_code;
  short rfilter_order;
  float zfilter_cutoff;
  float zfilter_resolution;
  short zfilter_code;
  short zfilter_order;
  float mt_1_4;
  float mt_2_4;
  float mt_3_4;
  short scatter_type;
  short recon_type;
  short recon_views;
  short align_3;
} Image_subheader;

typedef struct matdirnode {
  int		matnum ;
  int		strtblk ;
  int		endblk ;
  int		matstat ;
  struct matdirnode *next ;
} MatDirNode ;

typedef struct matdirlist {
  int	nmats ;
  MatDirNode *first ;
  MatDirNode *last ;
} MatDirList ;

typedef struct matrix_file {
  char		*fname ;	
  Main_header	*mhptr ;	
  MatDirList	*dirlist ;	
  FILE		*fptr ;		
  int		acs ;		
  FileFormat file_format;
  char **interfile_header;
} MatrixFile;

typedef struct matrixdata {
  int		matnum ;	/* matrix number */
  MatrixFile	*matfile ;	/* pointer to parent */
  DataSetType	mat_type ;	/* type of matrix? */
  MatrixDataType	data_type ;	/* type of data */
  caddr_t		shptr ;		/* pointer to sub-header */
  caddr_t		data_ptr ;	/* pointer to data */
  int		data_size ;	/* size of data in bytes */
  int		xdim;		/* dimensions of data */
  int		ydim;		/* y dimension */
  int		zdim;		/* for volumes */
  float		scale_factor ;	/* valid if data is int? */
  float		pixel_size;	/* xdim data spacing (cm) */
  float		y_size;		/* ydim data spacing (cm) */
  float		z_size;		/* zdim data spacing (cm) */
  float		data_min;	/* min value of data */
  float		data_max;	/* max value of data */
  float       x_origin;       /* x origin of data */
  float       y_origin;       /* y origin of data */
  float       z_origin;       /* z origin of data */
} MatrixData ;

MatrixFile *matrix_create(const char *fname, Main_header * proto_mhptr);
int matrix_write(MatrixFile *mptr, int matnum, MatrixData *data);
int matrix_close(MatrixFile *mptr);
int mat_numcod(int frame, int plane, int gate, int data, int bed);
