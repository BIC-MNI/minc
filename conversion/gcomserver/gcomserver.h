/* ----------------------------- MNI Header -----------------------------------
@NAME       : gcomserver.h
@DESCRIPTION: Header file that includes things needed for gcomserver.
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 23, 1993 (Peter Neelin)
@MODIFIED   : $Log: gcomserver.h,v $
@MODIFIED   : Revision 1.5  1994-01-11 12:38:42  neelin
@MODIFIED   : Modified handling of output directory and user id.
@MODIFIED   : Defaults are current dir and no chown.
@MODIFIED   : Read from file /usr/local/lib/gcomserver.<hostname>
@MODIFIED   :
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

/* Type for carrying around object information */
typedef struct {
   int study_id;
   int acq_id;
} Data_Object_Info;

#include <gyro_to_minc.h>
#include <gcom_prototypes.h>
