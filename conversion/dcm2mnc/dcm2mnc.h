/* ----------------------------- MNI Header -----------------------------------
@NAME       : dicomserver.h
@DESCRIPTION: Header file that includes things needed for dicomserver.
@METHOD     : 
@GLOBALS    : 
@CREATED    : January 28, 1997 (Peter Neelin)
@MODIFIED   : 

 * $Log: dcm2mnc.h,v $
 * Revision 1.1  2005-02-17 16:38:09  bert
 * Initial checkin, revised DICOM to MINC converter
 *
 * Revision 1.1.1.1  2003/08/15 19:52:55  leili
 * Leili's dicom server for sonata
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
#include <time_stamp.h>
#include <acr_nema.h>

#include "acr_element_defs.h"
#include "spi_element_defs.h"
#include "ext_element_defs.h"

#ifndef TRUE
#  define TRUE 1
#endif
#ifndef FALSE
#  define FALSE 0
#endif

typedef char string_t[511+1];
#define STRING_T_LEN (sizeof(string_t) - 1)

#define FILE_ALLOC_INCREMENT 10

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

/* Type for carrying around object information */
typedef struct {
    int file_index;             /* input file index */
    char *file_name;            /* input file name */
    double study_id;            /* yyyymmdd.hhmmss */
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
    string_t sequence_name;
    string_t protocol_name;
} Data_Object_Info;

#include "dicom_to_minc.h"
#include "dicom_read.h"
#include "minc_file.h"
#include "progress.h"
#include "siemens_to_dicom.h"
#include "string_to_filename.h"

/* Globals */
struct globals {
    char *minc_history;         /* Global for minc history */
    char *pname;                /* program name */
    File_Type file_type;        /* type of input files */
    short Debug;                /* Debug on/off */
    short Anon;                 /* "Anonymize" the output */
    short List;
    short useMinMax;
    short splitEcho;            /* TRUE if echos in separate files */
    short splitDynScan;  /* TRUE if dynamic scans in separate files */
    short clobber;
    string_t Name;
    string_t command_line;
};

extern struct globals G;

#define CHKMEM(x) \
    if ((x) == NULL) \
        (fprintf(stderr, "Out of memory at %s:%d\n", __FILE__, __LINE__), \
         exit(-1))
