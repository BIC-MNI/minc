/* ----------------------------- MNI Header -----------------------------------
@NAME       : gyro_to_minc.h
@DESCRIPTION: Header file for gyro_to_minc.h
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 25, 1993 (Peter Neelin)
@MODIFIED   : $Log: gyro_to_minc.h,v $
@MODIFIED   : Revision 4.0  1997-05-07 20:01:07  neelin
@MODIFIED   : Release of minc version 0.4
@MODIFIED   :
 * Revision 3.0  1995/05/15  19:31:44  neelin
 * Release of minc version 0.3
 *
 * Revision 2.2  1994/11/21  08:08:03  neelin
 * Modified code to properly calculate start from centre locations, then
 * changed calculation back to old way because it worked.
 * Added a ncsetfill(mincid, NC_NOFILL).
 *
 * Revision 2.1  94/10/20  13:50:16  neelin
 * Write out direction cosines to support rotated volumes.
 * Store single slices as 1-slice volumes (3D instead of 2D).
 * Changed storing of minc history (get args for gyrotominc).
 * 
 * Revision 2.0  94/09/28  10:35:29  neelin
 * Release of minc version 0.2
 * 
 * Revision 1.8  94/09/28  10:35:04  neelin
 * Pre-release
 * 
 * Revision 1.7  94/05/24  15:10:00  neelin
 * Break up multiple echoes or time frames into separate files for 2 echoes
 * or 2 frames (put in 1 file for more).
 * Changed units of repetition time, echo time, etc to seconds.
 * Save echo times in dimension variable when appropriate.
 * Changed to file names to end in _mri.mnc.
 * 
 * Revision 1.6  94/03/15  14:25:52  neelin
 * Changed image-max/min to use fp_scaled_max/min instead of ext_scale_max/min
 * Added acquisition:comments attribute
 * Changed reading of configuration file to allow execution of a command on
 * the minc file.
 * 
 * Revision 1.5  94/01/17  15:06:31  neelin
 * Added some acquisition parameters (flip angle) and fixed error in writing
 * of scanning sequence.
 * 
 * Revision 1.4  94/01/14  11:37:42  neelin
 * Fixed handling of multiple reconstructions and image types. Add spiinfo variable with extra info (including window min/max). Changed output
 * file name to include reconstruction number and image type number.
 * 
 * Revision 1.3  93/12/14  16:37:48  neelin
 * Modified general_info structure a little bit.
 * 
 * Revision 1.2  93/12/10  15:35:48  neelin
 * Improved file name generation from patient name. No buffering on stderr.
 * Added spi group list to minc header.
 * Optionally read a defaults file to get output minc directory and owner.
 * 
 * Revision 1.1  93/11/30  14:42:43  neelin
 * Initial revision
 * 
@COPYRIGHT  :
              Copyright 1993 Peter Neelin, McConnell Brain Imaging Centre, 
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

/* Default value for ncopts */
#define NCOPTS_DEFAULT NC_VERBOSE

/* MINC variable for spi elements */
#define SPI_ROOT_VAR "spi_groups"

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
   int acq_id;
   int rec_num;
   int image_type;
   Cstring image_type_string;
   int nrows;
   int ncolumns;
   int size[MRI_NDIMS];        /* Size of dimension across these files */
   int first[MRI_NDIMS];       /* First in dimenion across these files */
   int total_size[MRI_NDIMS];  /* Size of dimension across acquisition */
   int *position[MRI_NDIMS];    /* Array indicating position of each image in
                                   output file. Only allocated when size > 1 */
   int image_index[MRI_NDIMS];  /* Mapping from MRI dim to output image dim */
   World_Index slice_world;
   World_Index row_world;
   World_Index column_world;
   double step[WORLD_NDIMS];
   double start[WORLD_NDIMS];
   double dircos[WORLD_NDIMS][WORLD_NDIMS];
   double centre[WORLD_NDIMS];
   double slice_step;
   double slice_start;
   double slicepos_first;
   double slicepos_last;
   int sliceindex_first;
   int sliceindex_last;
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
   double pixel_max;
   double pixel_min;
   double slice_max;
   double slice_min;
   double window_max;
   double window_min;
   double slice_position;
   double dyn_begin_time;
   double echo_time;
   int index[MRI_NDIMS];
} File_Info;

/* Structure for image data */
typedef struct {
   int free;
   int nrows;
   int ncolumns;
   unsigned short *data;
} Image_Data;

