/* ----------------------------- MNI Header -----------------------------------
@NAME       : dcm2mnc.h
@DESCRIPTION: Header file that includes things needed for dicomserver.
@METHOD     : 
@GLOBALS    : 
@CREATED    : January 28, 1997 (Peter Neelin)
@MODIFIED   : 

 * $Log: dcm2mnc.h,v $
 * Revision 1.10.2.4  2005-08-18 18:17:36  bert
 * Add -usecoordinates option
 *
 * Revision 1.10.2.3  2005/06/09 20:46:11  bert
 * Fairly major fixes to use integers for all arguments, add filename_format and dirname_format to global options, always include math.h, and defined OPTS_NO_RESCALE for testing converter without ICV
 *
 * Revision 1.10.2.2  2005/05/16 19:45:23  bert
 * Add config.h, sys/types.h, other config.h conditionals for float.h, int32_t and int16_t.  Also fix definitions of Name and command_line in the globals structure.
 *
 * Revision 1.10.2.1  2005/05/12 21:16:47  bert
 * Initial checkin
 *
 * Revision 1.10  2005/04/29 23:09:36  bert
 * Add support for -stdin option to read file list from standard input
 *
 * Revision 1.9  2005/04/18 16:38:09  bert
 * Add comments and defines for both DICOM and IMA file detection
 *
 * Revision 1.8  2005/04/05 21:50:11  bert
 * Add comment
 *
 * Revision 1.7  2005/03/18 19:10:39  bert
 * Scan coordinate and location information for validity before relying on it
 *
 * Revision 1.6  2005/03/13 19:34:41  bert
 * Add pms_element_defs.h to the header
 *
 * Revision 1.5  2005/03/03 20:10:14  bert
 * Consider patient_id and patient_name when sorting into series
 *
 * Revision 1.4  2005/03/03 18:59:15  bert
 * Fix handling of image position so that we work with the older field (0020, 0030) as well as the new (0020, 0032)
 *
 * Revision 1.3  2005/03/02 18:23:33  bert
 * Added mosaic sequence and bitwise options
 *
 * Revision 1.2  2005/02/23 18:28:11  bert
 * Minor updates
 *
 * Revision 1.1  2005/02/17 16:38:09  bert
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <memory.h>
#include <limits.h>
#include <math.h>
#if HAVE_FLOAT_H
#include <float.h>
#endif

#include <time_stamp.h>
#include <acr_nema.h>

#include "acr_element_defs.h"
#include "spi_element_defs.h"
#include "ext_element_defs.h"
#include "pms_element_defs.h"   /* Philips Medical Systems */
#include "gems_element_defs.h"  /* GE Medical Systems */

#ifndef TRUE
#  define TRUE 1
#endif
#ifndef FALSE
#  define FALSE 0
#endif

/* Constants for "standard" DICOM files (herein referred to as
 * CD/Export files).  For more details of this, see section 7.1 "DICOM
 * FILE META INFORMATION" of the DICOM specification section 3.10.
 */
#define DICM_MAGIC_SIZE 4
#define DICM_MAGIC_OFFS 0x0080
#define DICM_MAGIC_STR "DICM"

/* Constants for Siemens IMA file format detection.  These are somewhat
 * arbitrary - I test for IMA format by looking for the manufacturer string
 * SIEMENS at offset 0x0060
 */
#define IMA_MAGIC_SIZE (8+1)
#define IMA_MAGIC_OFFS 0x0060
#define IMA_MAGIC_STR "SIEMENS"

/* Test to see if two floating-point numbers are very close in value.
 */

extern int fcmp(double x, double y, double delta);

#define NEARLY_EQUAL(x, y) (fcmp(x, y, 1e-6))

#if !HAVE_INT16_T
typedef short int16_t;
#endif

#if !HAVE_INT32_T
typedef int int32_t;
#endif

typedef char string_t[511+1];
#define STRING_T_LEN (sizeof(string_t) - 1)

/* Define logging constants */
#define NO_LOGGING 0
#define LO_LOGGING 1
#define HI_LOGGING 2

/* added by rhoge for ACQ and MEAS loop handling */
typedef enum { NONE = 0 , ACQ , MEAS } Loop_Type;

/* supported file types */
typedef enum { UNDEF, IMA, N3DCM, N4DCM } File_Type;

/* Type for carrying around object information 
 */
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
    string_t patient_name;
    string_t patient_id;
    double slice_location;
    int coord_found;
} Data_Object_Info;

#include "dicom_to_minc.h"
#include "dicom_read.h"
#include "minc_file.h"
#include "progress.h"
#include "siemens_to_dicom.h"
#include "string_to_filename.h"

typedef enum {
    MOSAIC_SEQ_UNKNOWN,
    MOSAIC_SEQ_INTERLEAVED,
    MOSAIC_SEQ_ASCENDING,
    MOSAIC_SEQ_DESCENDING
} mosaic_seq_t;

/* Globals */
struct globals {
    char *minc_history;         /* Global for minc history */
    char *pname;                /* program name */
    File_Type file_type;        /* type of input files */
    int Debug;                  /* Debug on/off */
    int Anon;                   /* "Anonymize" the output */
    int List;
    int useMinMax;      /* TRUE if need to use pixel min/max values */
    int splitEcho;              /* TRUE if echos in separate files */
    int splitDynScan;    /* TRUE if dynamic scans in separate files */
    int clobber;
    char * command_line;
    int opts;
    mosaic_seq_t mosaic_seq;
    int use_stdin;
    char * filename_format;
    char * dirname_format;
    int prefer_coords;           /* In event of slice thickness conflict, 
                                    use the coordinate information rather 
                                    than the slice thickness or spacing. */
};

/* Values for options flags */
#define OPTS_NO_MOSAIC   0x00000001 /* Don't parse mosaic information. */
#define OPTS_KEEP_COORD  0x00000002 /* Don't flip DICOM coordinates */
#define OPTS_NO_LOCATION 0x00000004 /* Never rely on slice location */
#define OPTS_NO_RESCALE  0x00000008 /* Do NOT rescale pixel data in any way! */

extern struct globals G;

#define CHKMEM(x) \
    if ((x) == NULL) \
        (fprintf(stderr, "Out of memory at %s:%d\n", __FILE__, __LINE__), \
         exit(-1))
