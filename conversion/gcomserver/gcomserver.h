/* ----------------------------- MNI Header -----------------------------------
@NAME       : gcomserver.h
@DESCRIPTION: Header file that includes things needed for gcomserver.
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 23, 1993 (Peter Neelin)
@MODIFIED   : $Log: gcomserver.h,v $
@MODIFIED   : Revision 1.8  1994-05-24 15:09:57  neelin
@MODIFIED   : Break up multiple echoes or time frames into separate files for 2 echoes
@MODIFIED   : or 2 frames (put in 1 file for more).
@MODIFIED   : Changed units of repetition time, echo time, etc to seconds.
@MODIFIED   : Save echo times in dimension variable when appropriate.
@MODIFIED   : Changed to file names to end in _mri.mnc.
@MODIFIED   :
 * Revision 1.7  94/04/07  11:03:37  neelin
 * Changed error handling to be more explicit about errors.
 * When the server terminates due to an error, a message is printed to /dev/log.
 * Changed handling of file cleanup.
 * 
 * Revision 1.6  94/01/14  11:37:40  neelin
 * Fixed handling of multiple reconstructions and image types. Add spiinfo variable with extra info (including window min/max). Changed output
 * file name to include reconstruction number and image type number.
 * 
 * Revision 1.5  94/01/11  12:38:42  neelin
 * Modified handling of output directory and user id.
 * Defaults are current dir and no chown.
 * Read from file /usr/local/lib/gcomserver.<hostname>
 * 
 * Revision 1.4  93/12/10  15:35:46  neelin
 * Improved file name generation from patient name. No buffering on stderr.
 * Added spi group list to minc header.
 * Optionally read a defaults file to get output minc directory and owner.
 * 
 * Revision 1.3  93/11/30  14:42:41  neelin
 * Copies to minc format.
 * 
 * Revision 1.2  93/11/25  13:27:25  neelin
 * Working version.
 * 
 * Revision 1.1  93/11/23  14:13:43  neelin
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
#include <spi_element_defs.h>

#ifndef TRUE
#  define TRUE 1
#endif
#ifndef FALSE
#  define FALSE 0
#endif

/* Define logging constants */
#define NO_LOGGING   0
#define LOW_LOGGING  1
#define HIGH_LOGGING 2

/* File containing defaults for gcomserver */
#define OUTPUT_DEFAULT_FILE "/usr/local/lib/gcomserver."

/* System log file (set to NULL for no logging of error) */
#define SYSTEM_LOG "/dev/log"

/* Type for carrying around object information */
typedef struct {
   int study_id;
   int acq_id;
   int rec_num;
   int image_type;
   int num_echoes;
   int echo_number;
   int num_dyn_scans;
   int dyn_scan_number;
} Data_Object_Info;

/* Define macro for array size */
#define ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))

#include <gyro_to_minc.h>
#include <gcom_prototypes.h>
