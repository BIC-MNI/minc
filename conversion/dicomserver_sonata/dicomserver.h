/* ----------------------------- MNI Header -----------------------------------
@NAME       : dicomserver.h
@DESCRIPTION: Header file that includes things needed for dicomserver.
@METHOD     : 
@GLOBALS    : 
@CREATED    : January 28, 1997 (Peter Neelin)
@MODIFIED   : 

 * $Log: dicomserver.h,v $
 * Revision 1.1  2003-08-15 19:52:55  leili
 * Initial revision
 *
 * Revision 1.5  2002/03/19 13:13:56  rhoge
 * initial working mosaic support - I think time is scrambled though.
 *
 * Revision 1.4  2001/12/31 18:27:21  rhoge
 * modifications for dicomreader processing of Numaris 4 dicom files - at
 * this point code compiles without warning, but does not deal with
 * mosaiced files.  Also will probably not work at this time for Numaris
 * 3 .ima files.  dicomserver may also not be functional...
 *
 * Revision 1.3  2000/12/14 21:17:34  rhoge
 * cleanup of log messages
 *
 * Revision 1.2  2000/12/14 21:15:58  rhoge
 * added ACQ and MEAS constants as flags for type of (non-standard)
 * dynamic scan looping
 *
 * Revision 1.1.1.1  2000/11/30 02:13:15  rhoge
 * imported sources to CVS repository on amoeba
 * added num_slices_nominal to Data_Object_Info
 * (for support of acquisition loop scans)
 *
 * Revision 6.1  1999/10/29 17:51:55  neelin
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
 * Revision 1.2  1997/03/11  13:10:48  neelin
 * Working version of dicomserver.
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <memory.h>
#include <limits.h>
#ifdef FLT_DIG
#  undef FLT_DIG
#endif
#ifdef DBL_DIG
#  undef DBL_DIG
#endif
#ifdef DBL_MIN
#  undef DBL_MIN
#endif
#ifdef DBL_MAX
#  undef DBL_MAX
#endif
#include <float.h>
#include <minc_def.h>
#include <time_stamp.h>
#include <acr_nema.h>
#include <dicom_element_defs.h>

#ifndef TRUE
#  define TRUE 1
#endif
#ifndef FALSE
#  define FALSE 0
#endif

#define FILE_ALLOC_INCREMENT 10

/* Connection timeout length in seconds */
/* Changed to 10 min instead of 5 by Leili */
#define CONNECTION_TIMEOUT (60*10)

/* Time to sleep between image reads when a child process is running.
   This prevents the server from outrunning its children. */

#define SERVER_SLEEP_TIME 3

/* Define logging constants */
#define NO_LOGGING   0
#define LOW_LOGGING  1
#define HIGH_LOGGING 2

/* added by rhoge for ACQ and MEAS loop handling */
typedef enum { NONE = 0 , ACQ , MEAS } Loop_Type;

/* supported file types */
typedef enum { UNDEF, IMA, N3DCM, N4DCM } File_Type;

/* File containing defaults for dicomserver */
#define OUTPUT_DEFAULT_FILE_DIR "/usr/local/lib"
#define OUTPUT_DEFAULT_FILE_PREFIX "dicomserver."

/* System log file (set to NULL for no logging of error) */
#define SYSTEM_LOG "/dev/log"

/* Type for carrying around object information */
typedef struct {
  int file_index;
  char *file_name;
  double study_id; // yyyymmdd.hhmmss
  int study_date;
  int study_time;
  int scanner_serialno;
  int acq_id;
  int rec_num;
  int image_type;
  int num_echoes;
  int echo_number;
  int num_dyn_scans;
  int dyn_scan_number;
  int global_image_number;
  int num_slices_nominal;
  int slice_number;
  int acq_rows;
  int acq_cols;
  int rec_rows;
  int rec_cols;
  int num_mosaic_rows;
  int num_mosaic_cols;
  int num_slices_in_file;
  char sequence_name[256];
  char protocol_name[256];

} Data_Object_Info;

/* Define macro for array size */
#define ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))

#include <siemens_dicom_to_minc.h>
#include <dicom_prototypes.h>
