/* ----------------------------- MNI Header -----------------------------------
@NAME       : siemens_dicom_to_minc.h
@DESCRIPTION: Header file for siemens_dicom_to_minc.h
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 28, 1997 (Peter Neelin)
@MODIFIED   : 
 * $Log: siemens_dicom_to_minc.h,v $
 * Revision 6.1  1999-10-29 17:51:59  neelin
 * Fixed Log keyword
 *
 * Revision 6.0  1997/09/12 13:24:27  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:25:26  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:06:20  neelin
 * Release of minc version 0.4
 *
 * Revision 1.1  1997/03/04  20:56:47  neelin
 * Initial revision
 *
@COPYRIGHT  :
              Copyright 1997 Peter Neelin, McConnell Brain Imaging Centre, 
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

#include <minc.h>

/* General constants */
#define SECONDS_PER_MINUTE 60
#define MINUTES_PER_HOUR 60
#define SECONDS_PER_HOUR (MINUTES_PER_HOUR*SECONDS_PER_MINUTE)
#define HOURS_PER_DAY 24
#define SECONDS_PER_DAY (HOURS_PER_DAY*SECONDS_PER_HOUR)
#define MS_PER_SECOND 1000
#define COORDINATE_EPSILON (100.0*FLT_EPSILON)

/* Default value for ncopts */
#define NCOPTS_DEFAULT NC_VERBOSE

/* MINC variable for dicom elements */
#define DICOM_ROOT_VAR "dicom_groups"

/* Possible MRI dimensions */
typedef enum { SLICE = 0, ECHO, TIME, PHASE, CHEM_SHIFT, MRI_NDIMS } Mri_Index;

/* World dimensions */
typedef enum { XCOORD = 0, YCOORD, ZCOORD, WORLD_NDIMS } World_Index;

/* Volume dimensions */
typedef enum { VSLICE = 0, VROW, VCOLUMN, VOL_NDIMS } Volume_Index;

/* Orientations */
typedef enum {TRANSVERSE = 0, SAGITTAL, CORONAL, NUM_ORIENTATIONS} Orientation;

/* String type */
typedef char Cstring[256];

/* Structure for general info about files */
typedef struct {
   int initialized;
   int study_id;
   int acq_id;                   /* Time of scan */
   int rec_num;
   int image_type;
   Cstring image_type_string;
   int nrows;
   int ncolumns;
   int default_index[MRI_NDIMS]; /* Index for dimensions with size == 1 */
   int size[MRI_NDIMS];        /* Size of dimension across these files */
   int total_size[MRI_NDIMS];  /* Size of dimension across acquisition */
   int *indices[MRI_NDIMS];    /* List of indices found for each dimension.
                                  Only allocated when size > 1 */
   int search_start[MRI_NDIMS]; /* Indices into lists for starting searches */
   double *coordinates[MRI_NDIMS]; /* Array indicating coordinate of each
                                      index in indices array */
   int image_index[MRI_NDIMS];  /* Mapping from MRI dim to output image dim */
   World_Index slice_world;
   World_Index row_world;
   World_Index column_world;
   double step[WORLD_NDIMS];
   double start[WORLD_NDIMS];
   double dircos[WORLD_NDIMS][WORLD_NDIMS];
   nc_type datatype;
   int is_signed;
   double pixel_min;
   double pixel_max;
   Cstring units;
   double window_min;
   double window_max;
   struct {
      Cstring name;
      Cstring identification;
      Cstring birth_date;
      Cstring sex;
      double weight;
   } patient;
   struct {
      Cstring start_time;
      Cstring modality;
      Cstring manufacturer;
      Cstring model;
      Cstring institution;
      Cstring station_id;
      Cstring ref_physician;
      Cstring procedure;
      Cstring study_id;
      Cstring acquisition_id;
   } study;
   struct {
      Cstring scan_seq;
      double rep_time;
      double echo_time;
      double inv_time;
      double flip_angle;
      double num_avg;
      double imaging_freq;
      Cstring imaged_nucl;
      Cstring comments;
   } acq;
   Acr_Group group_list;
} General_Info;

/* Structure for file-specific info */
typedef struct {
   int valid;
   int bits_alloc;
   int bits_stored;
   int index[MRI_NDIMS];
   double pixel_max;
   double pixel_min;
   double slice_max;
   double slice_min;
   double window_max;
   double window_min;
   double coordinate[MRI_NDIMS];
} File_Info;

/* Structure for image data */
typedef struct {
   int free;
   int nrows;
   int ncolumns;
   unsigned short *data;
} Image_Data;

/* Structure for sorting dimensions */
typedef struct {
   int identifier;
   int original_index;
   double value;
} Sort_Element;
