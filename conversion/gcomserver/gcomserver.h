/* ----------------------------- MNI Header -----------------------------------
@NAME       : gcomserver.h
@DESCRIPTION: Header file that includes things needed for gcomserver.
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 23, 1993 (Peter Neelin)
@MODIFIED   : 
 * $Log: gcomserver.h,v $
 * Revision 6.4  2000-02-21 23:48:14  neelin
 * More changes to improve dicom conformance for MNH PACS system.
 * Allow UID prefix to be defined in project file. Define SOP instance UID in
 * addition to study and series instance UIDs and frame-of-reference UID and
 * make sure that these are all the constant for the same image data.
 * Set series number from acquisition number.
 * Set study description from part of comment field.
 *
 * Revision 6.3  2000/01/31 13:57:38  neelin
 * Added keyword to project file to allow definition of the local AEtitle.
 * A simple syntax allows insertion of the host name into the AEtitle.
 *
 * Revision 6.2  1999/10/29 17:52:03  neelin
 * Fixed Log keyword
 *
 * Revision 6.1  1997/09/12 23:13:28  neelin
 * Added ability to convert gyrocom images to dicom images.
 *
 * Revision 6.0  1997/09/12  13:23:50  neelin
 * Release of minc version 0.6
 *
 * Revision 5.1  1997/09/11  13:09:40  neelin
 * Added more complicated syntax for project files so that different things
 * can be done to the data. The old syntax is still supported.
 *
 * Revision 5.0  1997/08/21  13:24:50  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:01:07  neelin
 * Release of minc version 0.4
 *
 * Revision 3.0  1995/05/15  19:31:44  neelin
 * Release of minc version 0.3
 *
 * Revision 2.1  1995/02/14  18:12:26  neelin
 * Added project names and defaults files (using volume name).
 * Added process id to log file name.
 * Moved temporary files to subdirectory.
 *
 * Revision 2.0  1994/09/28  10:35:25  neelin
 * Release of minc version 0.2
 *
 * Revision 1.9  94/09/28  10:34:56  neelin
 * Pre-release
 * 
 * Revision 1.8  94/05/24  15:09:57  neelin
 * Break up multiple echoes or time frames into separate files for 2 echoes
 * or 2 frames (put in 1 file for more).
 * Changed units of repetition time, echo time, etc to seconds.
 * Save echo times in dimension variable when appropriate.
 * Changed to file names to end in _mri.mnc.
 * 
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
#include <unistd.h>
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
#define OUTPUT_DEFAULT_FILE_DIR "/usr/local/lib"
#define OUTPUT_DEFAULT_FILE_PREFIX "gcomserver."

/* String lengths */
#define SHORT_LINE 64
#define LONG_LINE 512
#define MAX_AE_LEN 16

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

/* Type for project file information */
typedef struct {

   /* Type of information stored in project file */
   enum {PROJECT_DIRECTORY, PROJECT_DICOM, PROJECT_UNKNOWN} type;

   /* Information associated with project */
   union {

      /* Information for storing data in directories */
      struct {
         char file_prefix[LONG_LINE];
         int output_uid;
         int output_gid;
         char command_line[LONG_LINE];
      } directory;

      /* Information for converting data to dicom */
      struct {
         char hostname[SHORT_LINE];
         char port[SHORT_LINE];
         char AEtitle[SHORT_LINE];
         char LocalAEtitle[SHORT_LINE];
         char UIDprefix[SHORT_LINE];
         Acr_File *afpin;
         Acr_File *afpout;
      } dicom;

   } info;

} Project_File_Info;

/* Define macro for array size */
#define ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))

#include <gyro_to_minc.h>
#include <gcom_prototypes.h>
