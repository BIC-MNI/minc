/* ----------------------------- MNI Header -----------------------------------
@NAME       : mnitominc.h
@DESCRIPTION: Header file for mnitominc
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 9, 1992 (Peter Neelin)
@MODIFIED   : 
 * $Log: mnitominc.h,v $
 * Revision 6.1  1999-10-29 17:52:06  neelin
 * Fixed Log keyword
 *
 * Revision 6.0  1997/09/12 13:23:27  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:24:28  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:00:05  neelin
 * Release of minc version 0.4
 *
 * Revision 3.0  1995/05/15  19:31:02  neelin
 * Release of minc version 0.3
 *
 * Revision 2.0  1994/09/28  10:33:18  neelin
 * Release of minc version 0.2
 *
 * Revision 1.11  94/09/28  10:33:13  neelin
 * Pre-release
 * 
 * Revision 1.10  94/09/26  08:58:12  neelin
 * Changed default to -noclobber.
 * 
 * Revision 1.9  93/08/30  16:44:30  neelin
 * Added -slcstep option.
 * Fixed -field_of_view option.
 * Changed -xstep/-ystep to -colstep -rowstep.
 * 
 * Revision 1.8  93/08/11  15:24:29  neelin
 * Added RCS logging to source.
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
/* Some constants */

#define NORMAL_STATUS 0
#define ERROR_STATUS 1
#define TRUE 1
#define FALSE 0
#define MIN_VALID_CHAR ' '
#define MAX_VALID_CHAR '\177'
#define PADDING_CHAR ' '
#define SEC_PER_MIN 60.0
#define EPSILON 100.0*FLT_EPSILON

/* Constants for argument parsing */
#define MAX_VOL_IMGS 128
#define DEFAULT_NSLICES 15
#define IMAGE_DIMS 2
#define MAX_DIMS 4
#define DEF_ORIENT -1
#define TRANSVERSE 0
#define SAGITTAL 1
#define CORONAL 2

/* Constants for mni files */
#define BLOCK_SIZE 512
#define MNI_HEADER_SIZE BLOCK_SIZE
#define MNI_FILE_TYPE 1
#define NIL_FILE_TYPE 2
#define MNI_STX_XBRAIN 67.0
#define MNI_STX_YBRAIN 86.0
#define MNI_STX_ZBRAIN 75.0
#define MNI_STX_PBRAIN 50.0
#define MNI_STX_XACLOC 0.0
#define MNI_STX_YACLOC 16.0
#define MNI_STX_NPIXEL 128
#define MNI_STX_FACTOR 100.0
#define MNI_STX_XFOV MNI_STX_NPIXEL*MNI_STX_XBRAIN/MNI_STX_PBRAIN
#define MNI_STX_YFOV MNI_STX_NPIXEL*MNI_STX_YBRAIN/MNI_STX_PBRAIN
#define MNI_STX_ZFOV MNI_STX_NPIXEL*MNI_STX_ZBRAIN/MNI_STX_PBRAIN
#define MNI_STX_XSCALE MNI_STX_XBRAIN/MNI_STX_FACTOR
#define MNI_STX_YSCALE MNI_STX_YBRAIN/MNI_STX_FACTOR
#define MNI_STX_ZSCALE MNI_STX_ZBRAIN/MNI_STX_FACTOR
#define MNI_FOV 256.0
#define NIL_FOV 250.0
#define MNI_DIR (1.0)
#define NIL_DIR (-1.0)
#define MNI_VMIN 0.0
#define MNI_VMAX 255.0
#define NIL_VMIN 0.0
#define NIL_VMAX 4095.0
#define MNI_MRI_SCAN 1
#define MNI_SCX_SCAN 2
#define MNI_STX_SCAN 3
#define MNI_POS_SCAN 4
#define MNI_TRANSVERSE 100
#define MNI_CORONAL 200
#define MNI_SAGITTAL 300
#define MNI_POSITOME_MASK 0
#define MNI_CORNER_XSIZE 12
#define MNI_CORNER_YSIZE 7
#define MNI_LOC_OFFSET 212
#define MNI_LOC_NIMAGES 230
#define MNI_LOC_MATYPE 238
#define MNI_LOC_SCANNER_ID 200
#define MNI_LOC_ORIENTATION 204
#define MNI_LOC_PATNAM 0
#define MNI_PATNAM_LENGTH 23
#define MNI_LOC_PATNUM 23
#define MNI_PATNUM_LENGTH 14
#define MNI_LOC_ACQDAT 42
#define MNI_ACQDAT_LENGTH 10
#define MNI_LOC_ACQTIM 52
#define MNI_ACQTIM_LENGTH 10
#define MNI_LOC_ISOTOPE 129
#define MNI_ISOTOPE_LENGTH 14
#define MNI_LOC_DOSE 143
#define MNI_DOSE_LENGTH 14
#define MNI_LOC_INJTIM 157
#define MNI_INJTIM_LENGTH 10
#define MNI_LOC_CORR_MASK 196
#define MNI_XLOC_QSC 0
#define MNI_YLOC_QSC 0
#define MNI_XLOC_ISEA 4
#define MNI_YLOC_ISEA 2
#define MNI_XLOC_ZPOS 8
#define MNI_YLOC_ZPOS 0
#define MNI_XLOC_TIME 0
#define MNI_YLOC_TIME 2
#define MNI_XLOC_TLEN 4
#define MNI_YLOC_TLEN 3

/* Structure containing information about orientation */
char *orientation_names[][IMAGE_DIMS+1] = {
   {MIzspace, MIyspace, MIxspace},
   {MIxspace, MIzspace, MIyspace},
   {MIyspace, MIzspace, MIxspace},
};

/* Structure containing information about signs */
char *sign_names[][6] = {
   {MI_SIGNED, MI_UNSIGNED, MI_SIGNED, MI_SIGNED, MI_SIGNED, MI_SIGNED},
   {MI_SIGNED, MI_SIGNED, MI_SIGNED, MI_SIGNED, MI_SIGNED, MI_SIGNED},
   {MI_SIGNED, MI_UNSIGNED, MI_UNSIGNED, MI_UNSIGNED, MI_UNSIGNED, MI_UNSIGNED}
};

/* Argument variables */
char *pname;
char *infilename;
char *outfilename;
int clobber = FALSE;
int verbose = TRUE;
char *dimname[MAX_VAR_DIMS] = {MItime, NULL, NULL, NULL};
long dimlength[MAX_VAR_DIMS] = {0};
int dimfixed[MAX_VAR_DIMS] = {FALSE, FALSE, TRUE, TRUE};
long nframes;
long nslices;
int ndims;
int orientation = DEF_ORIENT;
double field_of_view = 0.0;
double xstep = 0.0;
double ystep = 0.0;
double zstep = 0.0;

/* Argument table */
ArgvInfo argTable[] = {
   {NULL, ARGV_HELP, (char *) NULL, (char *) NULL, 
       "Options to specify image orientation. Default = -transverse."},
   {"-transverse", ARGV_CONSTANT, (char *) TRANSVERSE, (char *) &orientation,
       "Transverse images"},
   {"-sagittal", ARGV_CONSTANT, (char *) SAGITTAL, (char *) &orientation,
       "Sagittal images"},
   {"-coronal", ARGV_CONSTANT, (char *) CORONAL, (char *) &orientation,
       "Coronal images"},
   {NULL, ARGV_HELP, NULL, NULL,
       "Options to help specify number of slices and time frames."},
   {"-nslices", ARGV_INT, (char *) 1, (char *) &dimlength[1],
       "Number of slices in volume."},
   {"-nframes", ARGV_INT, (char *) 1, (char *) &dimlength[0],
       "Number of time frames."},
   {NULL, ARGV_HELP, NULL, NULL,
       "Options for writing output file. Default = -noclobber."},
   {"-clobber", ARGV_CONSTANT, (char *) TRUE, (char *) &clobber,
       "Overwrite existing file"},
   {"-noclobber", ARGV_CONSTANT, (char *) FALSE, (char *) &clobber,
       "Don't overwrite existing file"},
   {NULL, ARGV_HELP, NULL, NULL,
       "Options for logging progress. Default = -verbose."},
   {"-verbose", ARGV_CONSTANT, (char *) TRUE, (char *) &verbose,
       "Write messages indicating progress"},
   {"-quiet", ARGV_CONSTANT, (char *) FALSE, (char *) &verbose,
       "Do not write log messages"},
   {NULL, ARGV_HELP, NULL, NULL,
       "Other options."},
   {"-field_of_view", ARGV_FLOAT, (char *) 1, (char *) &field_of_view,
       "Field of view of images (in mm)."},
   {"-colstep", ARGV_FLOAT, (char *) 1, (char *) &xstep,
       "Distance between pixels along columns (in mm)."},
   {"-rowstep", ARGV_FLOAT, (char *) 1, (char *) &ystep,
       "Distance between pixels along rows (in mm)."},
   {"-slcstep", ARGV_FLOAT, (char *) 1, (char *) &zstep,
       "Distance between slices (in mm)."},
   {NULL, ARGV_END, NULL, NULL, NULL}
};

/* Mni format file information */
typedef struct {
   FILE *fp;
   int file_type;
   short offset;
   short nimages;
   int npixels;
   int pix_size;
   double valid_range[2];
   short scanner_id;
   double xstep;
   double ystep;
   double zstep_scale;
   double xstart;
   double ystart;
   short orientation;
   char patient_name[MNI_PATNAM_LENGTH + 1];
   char patient_num[MNI_PATNUM_LENGTH + 1];
   char start_time[MNI_ACQDAT_LENGTH + MNI_ACQTIM_LENGTH + 2];
   char isotope[MNI_ISOTOPE_LENGTH + 1];
   char dose_string[MNI_DOSE_LENGTH + 1];
   double dose;
   char dose_units[10];
   char injection_time[MNI_INJTIM_LENGTH + 1];
} mni_header_type;

typedef struct {
   unsigned char *image;
   long npixels;
   long image_pix;
   int pix_size;
   long image_size;
   double minimum;
   double maximum;
   double zposition;
   double time;
   double time_length;
} mni_image_type;

/* Function declarations */
void parse_args(int argc, char *argv[], mni_header_type *mni_header);
void usage_error(char *progname);
int get_mni_header(char *file, mni_header_type *mni_header);
int get_mni_image(mni_header_type *mni_header, mni_image_type *mni_image,
                  int image_num);
int dblcmp(double dbl1, double dbl2, double epsilon);

