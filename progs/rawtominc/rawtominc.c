/* ----------------------------- MNI Header -----------------------------------
@NAME       : rawtominc
@INPUT      : argc, argv - command line arguments
@OUTPUT     : (none)
@RETURNS    : error status
@DESCRIPTION: Converts a raw volume of data values (read from standard input)
              to a minc format file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : September 25, 1992 (Peter Neelin)
@MODIFIED   : $Log: rawtominc.c,v $
@MODIFIED   : Revision 1.13  1994-06-10 15:24:41  neelin
@MODIFIED   : Added option -real_range.
@MODIFIED   :
 * Revision 1.12  94/05/05  10:31:53  neelin
 * Added -scan_range, -frame_time, -frame_width, -input, -attribute and 
 * modality options.
 * 
 * Revision 1.11  93/10/06  10:14:17  neelin
 * Added include of string.h.
 * 
 * Revision 1.10  93/08/11  15:25:05  neelin
 * Added RCS logging to source.
 * 
              December 2, 1992 (P.N.)
                 - changed to parse argv with ParseArgv
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

#ifndef lint
static char rcsid[]="$Header: /private-cvsroot/minc/progs/rawtominc/rawtominc.c,v 1.13 1994-06-10 15:24:41 neelin Exp $";
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <minc.h>
#include <float.h>
#include <ParseArgv.h>
#include <time_stamp.h>
#include <minc_def.h>

/* Some constants */

#define VECTOR_SEPARATOR ','
#define NORMAL_STATUS 0
#define ERROR_STATUS 1
#define MIN_DIMS 2
#define MAX_DIMS 4
#define TRANSVERSE 0
#define SAGITTAL 1
#define CORONAL 2
#define TIME_FAST 3
#define DEF_TYPE 0
#define DEF_SIGN 0
#define SIGNED 1
#define UNSIGNED 2
#define TRUE 1
#define FALSE 0
#define X 0
#define Y 1
#define Z 2
#define DEF_STEP DBL_MAX
#define DEF_START DBL_MAX
#define DEF_RANGE DBL_MAX

/* Macros */
#define STR_EQ(s1,s2) (strcmp(s1,s2)==0)

/* Function declarations */
void parse_args(int argc, char *argv[]);
void usage_error(char *pname);
int get_attribute(char *dst, char *key, char *nextarg);
int get_times(char *dst, char *key, char *nextarg);

/* Structure containing information about orientation */
char *orientation_names[][MAX_DIMS] = {
   {MItime, MIzspace, MIyspace, MIxspace},
   {MItime, MIxspace, MIzspace, MIyspace},
   {MItime, MIyspace, MIzspace, MIxspace},
   {MIzspace, MItime, MIyspace, MIxspace}
};

/* Structure containing information about signs */
char *sign_names[][6] = {
   {MI_SIGNED, MI_UNSIGNED, MI_SIGNED, MI_SIGNED, MI_SIGNED, MI_SIGNED},
   {MI_SIGNED, MI_SIGNED, MI_SIGNED, MI_SIGNED, MI_SIGNED, MI_SIGNED},
   {MI_SIGNED, MI_UNSIGNED, MI_UNSIGNED, MI_UNSIGNED, MI_UNSIGNED, MI_UNSIGNED}
};

/* Argument variables */
char *pname;
char *filename;
int clobber=FALSE;
char *dimname[MAX_VAR_DIMS];
long dimlength[MAX_VAR_DIMS];
int ndims;
int orientation = TRANSVERSE;
nc_type type = NC_BYTE;
int signtype = DEF_SIGN;
char *sign;
double valid_range[2] = {0.0, -1.0};
int vrange_set;
nc_type otype = DEF_TYPE;
int osigntype = DEF_SIGN;
char *osign;
double ovalid_range[2] = {0.0, -1.0};
int ovrange_set;
double dimstep[3] = {DEF_STEP, DEF_STEP, DEF_STEP};
double dimstart[3] = {DEF_START, DEF_START, DEF_START};
int vector_dimsize = -1;
char *modality = NULL;
int attribute_list_size = 0;
int attribute_list_alloc = 0;
struct {
   char *variable;
   char *attribute;
   char *value;
   double double_value;
} *attribute_list = NULL;
int num_frame_times = 0;
double *frame_times = NULL;
int num_frame_widths = 0;
double *frame_widths = NULL;
char *inputfile = NULL;
int do_minmax = FALSE;
double real_range[2] = {DEF_RANGE, DEF_RANGE};

/* Argument table */
ArgvInfo argTable[] = {
   {NULL, ARGV_HELP, (char *) NULL, (char *) NULL, 
       "Options to specify input dimension order (from slowest to fastest)."},
   {NULL, ARGV_HELP, (char *) NULL, (char *) NULL, 
       "   Default = -transverse."},
   {"-transverse", ARGV_CONSTANT, (char *) TRANSVERSE, (char *) &orientation,
       "Transverse images   : [[time] z] y x"},
   {"-sagittal", ARGV_CONSTANT, (char *) SAGITTAL, (char *) &orientation,
       "Sagittal images     : [[time] x] z y"},
   {"-coronal", ARGV_CONSTANT, (char *) CORONAL, (char *) &orientation,
       "Coronal images      : [[time] y] z x"},
   {"-time", ARGV_CONSTANT, (char *) TIME_FAST, (char *) &orientation,
       "Time ordered images : [[z] time] y x"},
   {"-vector", ARGV_INT, (char *) 1, (char *) &vector_dimsize,
       "Specifies the size of a vector dimension"},
   {NULL, ARGV_HELP, NULL, NULL,
       "Options to specify input data type. Default = -byte."},
   {"-byte", ARGV_CONSTANT, (char *) NC_BYTE, (char *) &type,
       "Byte values"},
   {"-short", ARGV_CONSTANT, (char *) NC_SHORT, (char *) &type,
       "Short integer values"},
   {"-long", ARGV_CONSTANT, (char *) NC_LONG, (char *) &type,
       "Long integer values"},
   {"-float", ARGV_CONSTANT, (char *) NC_FLOAT, (char *) &type,
       "Single-precision floating point values"},
   {"-double", ARGV_CONSTANT, (char *) NC_DOUBLE, (char *) &type,
       "Double-precision floating point values"},
   {NULL, ARGV_HELP, NULL, NULL,
       "Options for sign of input data. Default = -signed (-unsigned for byte)."},
   {"-signed", ARGV_CONSTANT, (char *) SIGNED, (char *) &signtype,
       "Signed values"},
   {"-unsigned", ARGV_CONSTANT, (char *) UNSIGNED, (char *) &signtype,
       "Unsigned values"},
   {NULL, ARGV_HELP, NULL, NULL,
       "Other input value options."},
   {"-range", ARGV_FLOAT, (char *) 2, (char *) valid_range, 
       "Valid range of input values (default = full range)."},
   {"-real_range", ARGV_FLOAT, (char *) 2, (char *) real_range, 
       "Real range of input values (ignored for floating-point types)."},
   {NULL, ARGV_HELP, NULL, NULL,
       "Options for type of output minc data. Default = input data type"},
   {"-obyte", ARGV_CONSTANT, (char *) NC_BYTE, (char *) &otype,
       "Byte values"},
   {"-oshort", ARGV_CONSTANT, (char *) NC_SHORT, (char *) &otype,
       "Short integer values"},
   {"-olong", ARGV_CONSTANT, (char *) NC_LONG, (char *) &otype,
       "Long integer values"},
   {"-ofloat", ARGV_CONSTANT, (char *) NC_FLOAT, (char *) &otype,
       "Single-precision floating point values"},
   {"-odouble", ARGV_CONSTANT, (char *) NC_DOUBLE, (char *) &otype,
       "Double precision floating point values"},
   {NULL, ARGV_HELP, NULL, NULL,
       "Options for sign of output minc data."},
   {NULL, ARGV_HELP, NULL, NULL,
       "   Default = -signed (-unsigned for byte data)"},
   {"-osigned", ARGV_CONSTANT, (char *) SIGNED, (char *) &osigntype,
       "Signed values"},
   {"-ounsigned", ARGV_CONSTANT, (char *) UNSIGNED, (char *) &osigntype,
       "Unsigned values"},
   {NULL, ARGV_HELP, NULL, NULL,
       "Other output value options."},
   {"-orange", ARGV_FLOAT, (char *) 2, (char *) ovalid_range, 
       "Valid range of output minc values."},
   {NULL, ARGV_HELP, NULL, NULL,
       "Options to specify whether integers are scanned for min and max."},
   {"-scan_range", ARGV_CONSTANT, (char *) TRUE, (char *) &do_minmax, 
       "Scan input for min and max."},
   {"-noscan_range", ARGV_CONSTANT, (char *) FALSE, (char *) &do_minmax, 
       "Do not scan input for min and max (default)."},
   {NULL, ARGV_HELP, NULL, NULL,
       "Options for writing output file. Default = -noclobber."},
   {"-clobber", ARGV_CONSTANT, (char *) TRUE, (char *) &clobber,
       "Overwrite existing file"},
   {"-noclobber", ARGV_CONSTANT, (char *) FALSE, (char *) &clobber,
       "Don't overwrite existing file"},
   {NULL, ARGV_HELP, NULL, NULL,
       "Option for specifying input raw data file."},
   {"-input", ARGV_STRING, (char *) 1, (char *) &inputfile,
       "Name of input file (default=stdin)."},
   {NULL, ARGV_HELP, NULL, NULL,
       "Options for specifying spatial dimension coordinates."},
   {"-xstep", ARGV_FLOAT, (char *) 1, (char *) &dimstep[X],
       "Step size for x dimension."},
   {"-ystep", ARGV_FLOAT, (char *) 1, (char *) &dimstep[Y],
       "Step size for y dimension."},
   {"-zstep", ARGV_FLOAT, (char *) 1, (char *) &dimstep[Z],
       "Step size for z dimension."},
   {"-xstart", ARGV_FLOAT, (char *) 1, (char *) &dimstart[X],
       "Starting coordinate for x dimension."},
   {"-ystart", ARGV_FLOAT, (char *) 1, (char *) &dimstart[Y],
       "Starting coordinate for y dimension."},
   {"-zstart", ARGV_FLOAT, (char *) 1, (char *) &dimstart[Z],
       "Starting coordinate for z dimension."},
   {NULL, ARGV_HELP, NULL, NULL,
       "Options for specifying imaging modality. Default = -nomodality."},
   {"-nomodality", ARGV_CONSTANT, NULL, (char *) &modality,
       "Do not store modality type in file."},
   {"-pet", ARGV_CONSTANT, MI_PET, (char *) &modality,
       "PET data."},
   {"-mri", ARGV_CONSTANT, MI_MRI, (char *) &modality,
       "MRI data."},
   {"-spect", ARGV_CONSTANT, MI_SPECT, (char *) &modality,
       "SPECT data."},
   {"-gamma", ARGV_CONSTANT, MI_GAMMA, (char *) &modality,
       "Data from a gamma camera."},
   {"-mrs", ARGV_CONSTANT, MI_MRS, (char *) &modality,
       "MR spectroscopy data."},
   {"-mra", ARGV_CONSTANT, MI_MRA, (char *) &modality,
       "MR angiography data."},
   {"-ct", ARGV_CONSTANT, MI_CT, (char *) &modality,
       "CT data."},
   {"-dsa", ARGV_CONSTANT, MI_DSA, (char *) &modality,
       "DSA data"},
   {"-dr", ARGV_CONSTANT, MI_DR, (char *) &modality,
       "Digital radiography data."},
   {NULL, ARGV_HELP, NULL, NULL,
       "Option for specifying attribute values by name."},
   {"-attribute", ARGV_FUNC, (char *) get_attribute, NULL,
       "Set an attribute (<var>:<attr>=<value>)."},
   {NULL, ARGV_HELP, NULL, NULL,
       "Options for specifying time dimension coordinates."},
   {"-frame_times", ARGV_FUNC, (char *) get_times, NULL,
       "Specify the frame starting times (\"<t1>,<t2>,<t3>,...\")."},
   {"-frame_widths", ARGV_FUNC, (char *) get_times, NULL,
       "Specify the frame lengths (\"<w1>,<w2>,<w3>,...\")."},
   {NULL, ARGV_END, NULL, NULL, NULL}
};

/* Main program */

main(int argc, char *argv[])
{
   int do_vrange;
   int cdfid, imgid, maxid, minid, varid;
   int icv;
   long start[MAX_VAR_DIMS];
   long count[MAX_VAR_DIMS];
   long end[MAX_VAR_DIMS];
   int dim[MAX_VAR_DIMS];
   void *image;
   double imgmax, imgmin, value;
   long image_size, image_pix, nread, fastdim;
   int pix_size;
   int image_dims;
   int i, j;
   int index;
   FILE *instream;
   char *tm_stamp;
   int iatt;
   long time_start, time_count;
   int is_signed;
   void *ptr;
   int floating_type;
   int do_real_range;
   double scale, offset, denom, pixel_min, pixel_max;

   /* Save time stamp and args */
   tm_stamp = time_stamp(argc, argv);

   /* Parse arguments */
   parse_args(argc, argv);

   /* Do normalization if input type is float */
   floating_type = (( type==NC_FLOAT) || ( type==NC_DOUBLE));
   do_minmax = do_minmax || floating_type;

   /* Find max and min for vrange if output type is float */
   do_vrange = do_minmax && (( otype==NC_FLOAT) || ( otype==NC_DOUBLE));

   /* Did the user provide a real range */
   do_real_range = (real_range[0] != DEF_RANGE) && !floating_type;
   
   /* Create an icv */
   icv=miicv_create();
   (void) miicv_setint(icv, MI_ICV_TYPE, type);
   (void) miicv_setstr(icv, MI_ICV_SIGN, sign);
   if (vrange_set) {
      (void) miicv_setdbl(icv, MI_ICV_VALID_MIN, valid_range[0]);
      (void) miicv_setdbl(icv, MI_ICV_VALID_MAX, valid_range[1]);
   }
   if (floating_type) {
      (void) miicv_setint(icv, MI_ICV_DO_NORM, TRUE);
      (void) miicv_setint(icv, MI_ICV_USER_NORM, TRUE);
   }

   /* Calculate scale and offset to convert pixel to real value for
      input data */
   if (do_real_range) {
      (void) miicv_inqdbl(icv, MI_ICV_VALID_MIN, &pixel_min);
      (void) miicv_inqdbl(icv, MI_ICV_VALID_MAX, &pixel_max);
      denom = pixel_max - pixel_min;
      if (denom != 0.0)
         scale = (real_range[1] - real_range[0]) / denom;
      else 
         scale = 0.0;
      offset = real_range[0] - pixel_min * scale;
   }
   else {
      scale = 1.0;
      offset = 0.0;
   }

   /* Create the file and save the time stamp */
   cdfid=nccreate(filename, (clobber ? NC_CLOBBER : NC_NOCLOBBER));
   (void) miattputstr(cdfid, NC_GLOBAL, MIhistory, tm_stamp);

   /* Set the number of image dimensions */
   image_dims = 2;

   /* Create the dimensions */
   for (i=0; i<ndims; i++) {

      /* Create dimension */
      dim[i] = ncdimdef(cdfid, dimname[i], dimlength[i]);

      /* Create the variable if needed */
      if (STR_EQ(dimname[i], MItime)) {
         if (num_frame_times > 0) {
            varid = micreate_std_variable(cdfid, MItime, 
                                          NC_DOUBLE, 1, &dim[i]);
         }
         if (num_frame_widths > 0) {
            varid = micreate_std_variable(cdfid, MItime_width,
                                          NC_DOUBLE, 1, &dim[i]);
         }
      }
      else {
         varid = micreate_std_variable(cdfid, dimname[i], 
                                       NC_LONG, 0, NULL);

         /* Write out step and start */
         if (STR_EQ(dimname[i], MIxspace)) index = X;
         if (STR_EQ(dimname[i], MIyspace)) index = Y;
         if (STR_EQ(dimname[i], MIzspace)) index = Z;
         if (dimstep[index] != DEF_STEP) {
            (void) miattputdbl(cdfid, varid, MIstep, dimstep[index]);
         }
         if (dimstart[index] != DEF_START) {
            (void) miattputdbl(cdfid, varid, MIstart, dimstart[index]);
         }
      }
      
      /* Set variables for looping through images */
      start[i]=0;
      count[i]= ((i<ndims-image_dims) ? 1 : dimlength[i]);
      end[i]=dimlength[i];
   }

   /* Check for vector dimension */
   if (vector_dimsize > 0) {
      ndims++;
      image_dims++;
      dim[ndims-1] = ncdimdef(cdfid, MIvector_dimension, 
                              (long) vector_dimsize);
      start[ndims-1] = 0;
      count[ndims-1] = vector_dimsize;
      end[ndims-1] = vector_dimsize;
   }

   /* Create the image */
   imgid=micreate_std_variable(cdfid, MIimage, otype, ndims, dim);
   (void) miattputstr(cdfid, imgid, MIcomplete, MI_FALSE);
   (void) miattputstr(cdfid, imgid, MIsigntype, osign);
   if (ovrange_set) 
      (void) ncattput(cdfid, imgid, MIvalid_range, NC_DOUBLE, 2, ovalid_range);
   if (do_minmax || do_real_range) {
      maxid = micreate_std_variable(cdfid, MIimagemax, 
                                    NC_DOUBLE, ndims-image_dims, dim);
      minid = micreate_std_variable(cdfid, MIimagemin, 
                                    NC_DOUBLE, ndims-image_dims, dim);
   }

   /* Create the modality attribute */
   if (modality != NULL) {
      varid = micreate_group_variable(cdfid, MIstudy);
      (void) miattputstr(cdfid, varid, MImodality, modality);
   }

   /* Create any special attributes */
   ncopts = 0;
   for (iatt=0; iatt < attribute_list_size; iatt++) {
      varid = ncvarid(cdfid, attribute_list[iatt].variable);
      if (varid == MI_ERROR) {
         varid = micreate_group_variable(cdfid, attribute_list[iatt].variable);
      }
      if (varid == MI_ERROR) {
         varid = ncvardef(cdfid, attribute_list[iatt].variable, NC_LONG,
                          0, NULL);
      }
      if (varid == MI_ERROR) {
         continue;
      }
      if (attribute_list[iatt].value != NULL) {
         (void) miattputstr(cdfid, varid, attribute_list[iatt].attribute,
                            attribute_list[iatt].value);
      }
      else {
         (void) miattputdbl(cdfid, varid, attribute_list[iatt].attribute,
                            attribute_list[iatt].double_value);
      }
   }
   ncopts = NC_VERBOSE | NC_FATAL;

   /* End definition mode */
   (void) ncendef(cdfid);

   /* Write out the frame times and widths */
   time_start = 0;
   time_count = num_frame_times;
   if (num_frame_times > 0) {
      (void) mivarput(cdfid, ncvarid(cdfid, MItime), 
                      &time_start, &time_count,
                      NC_DOUBLE, NULL, frame_times);
   }
   if (num_frame_widths > 0) {
      (void) mivarput(cdfid, ncvarid(cdfid, MItime_width), 
                      &time_start, &time_count,
                      NC_DOUBLE, NULL, frame_widths);
   }

   /* Attach the icv */
   (void) miicv_attach(icv, cdfid, imgid);

   /* Get a buffer for reading images */
   image_pix = 1;
   for (i=1; i<=image_dims; i++)
      image_pix *= end[ndims-i];
   pix_size=nctypelen(type);
   image_size=image_pix*pix_size;
   image=MALLOC(image_size);

   /* Loop through the images */
   if (inputfile == NULL) {
      instream=stdin;
   }
   else {
      instream = fopen(inputfile, "r");
   }
   fastdim=ndims-image_dims-1;
   if (fastdim<0) fastdim=0;
   if (do_vrange) {
      ovalid_range[0]=DBL_MAX;
      ovalid_range[1]=(-DBL_MAX);
   }

   while (start[0] < end[0]) {

      /* Read in image */
      nread=fread(image, pix_size, image_pix, instream);
      if (nread!=image_pix) {
         (void) fprintf(stderr, "%s: Premature end of file.\n", pname);
         exit(ERROR_STATUS);
      }

      /* Search for max and min for float and double */
      if (do_minmax) {
         imgmax=(-DBL_MAX);
         imgmin=DBL_MAX;
         is_signed = (signtype == SIGNED);
         for (j=0; j<image_pix; j++) {
            ptr = (char *) image + j * nctypelen(type);
            switch (type) { 
            case NC_BYTE : 
               if (is_signed)
                  value = (double) *((unsigned char *) ptr);
               else
                  value = (double) *((signed char *) ptr);
               break; 
            case NC_SHORT : 
               if (is_signed)
                  value = (double) *((unsigned short *) ptr);
               else
                  value = (double) *((signed short *) ptr);
               break; 
            case NC_LONG : 
               if (is_signed)
                  value = (double) *((unsigned long *) ptr); 
               else
                  value = (double) *((signed long *) ptr); 
               break; 
            case NC_FLOAT : 
               value = (double) *((float *) ptr); 
               break; 
            case NC_DOUBLE : 
               value = (double) *((double *) ptr); 
               break; 
            } 
            if (value<imgmin) imgmin=value;
            if (value>imgmax) imgmax=value;
         }
         if (do_vrange) {
            if (imgmin<ovalid_range[0]) ovalid_range[0]=imgmin;
            if (imgmax>ovalid_range[1]) ovalid_range[1]=imgmax;
         }

      }
      else {
         imgmin = pixel_min;
         imgmax = pixel_max;
      }

      /* Change the valid range for integer types if needed */
      if (do_minmax && !floating_type) {
         (void) miicv_detach(icv);
         (void) miicv_setdbl(icv, MI_ICV_VALID_MIN, imgmin);
         (void) miicv_setdbl(icv, MI_ICV_VALID_MAX, imgmax);
         (void) miicv_attach(icv, cdfid, imgid);
      }

      /* Write the image max and min after re-scaling */
      if (do_minmax || do_real_range) {
         imgmin = imgmin * scale + offset;
         imgmax = imgmax * scale + offset;
         (void) mivarput1(cdfid, minid, start, NC_DOUBLE, NULL, &imgmin);
         (void) mivarput1(cdfid, maxid, start, NC_DOUBLE, NULL, &imgmax);
      }
      
      /* Write the image */
      (void) miicv_put(icv, start, count, image);

     /* Increment the counters */
      start[fastdim] += count[fastdim];
      i=fastdim;
      while ((i>0) && (start[i]>=end[i])) {
         start[i] = 0;
         i--;
         start[i] += count[i];
      }
   }

   /* Free the memory */
   FREE(image);

   /* Write the valid max and min */
   if (do_vrange) {
      (void) ncattput(cdfid, imgid, MIvalid_range, NC_DOUBLE, 2, ovalid_range);
   }

   /* Close the file */
   (void) miattputstr(cdfid, imgid, MIcomplete, MI_TRUE);
   (void) ncclose(cdfid);
   
   exit(NORMAL_STATUS);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : parse_args
@INPUT      : argc        - number of command line arguments
              argv        - array of arguments
@OUTPUT     : 
@RETURNS    : (nothing).
@DESCRIPTION: Parses command line arguments.
@METHOD     : 
@GLOBALS    : pname        - program name
              filename     - file to write
              clobber      - overwrite?
              dimname      - names of dimensions
              dimlength    - lengths of dimensions
              ndims        - number of dimensions
              orientation  - orientation of dimensions
              type         - type of input data
              signtype     - sign of input data
              sign         - string sign of input
              valid_range  - valid range of input data
              vrange_set   - valid range used?
              otype        - type of output data
              osigntype    - sign of output data
              osign        - string sign of output
              ovalid_range - valid range of output data
              ovrange_set  - valid range used?
@CALLS      : 
@CREATED    : December 3, 1992
@MODIFIED   : 
---------------------------------------------------------------------------- */
void parse_args(int argc, char *argv[])
{
   char *ptr;
   int time_size;
   int i;

   /* Parse the command line */
   pname=argv[0];
   if (ParseArgv(&argc, argv, argTable, 0)) {
      usage_error(pname);
   }

   /* Check dimensions */
   ndims = argc - 2;
   if ((ndims<MIN_DIMS)||(ndims>MAX_DIMS)) {
      usage_error(pname);
   }

   /* Get filename */
   filename = argv[1];

   /* Get dimensions */
   for (i=0; i<ndims; i++) {
      dimlength[i] = strtol(argv[2+i], &ptr, 0);
      if ((ptr==argv[2+i]) || (*ptr!=0)) {
         usage_error(pname);
      }
      dimname[i]=orientation_names[orientation][i+MAX_DIMS-ndims];
   }

   /* Set types and signs */
   if ((otype==DEF_TYPE) && (osigntype==DEF_TYPE))
      osigntype=signtype;
   if (otype==DEF_TYPE) otype=type;
   sign  = sign_names[signtype][type];
   osign = sign_names[osigntype][otype];

   /* Check valid ranges */
   vrange_set=(valid_range[0]<valid_range[1]);
   ovrange_set=(ovalid_range[0]<ovalid_range[1]);
   if ((otype==type) && (STR_EQ(osign,sign))) {
      if (!ovrange_set && vrange_set) {
         ovalid_range[0]=valid_range[0];
         ovalid_range[1]=valid_range[1];
         ovrange_set = TRUE;
      }
      if (ovrange_set && !vrange_set) {
         valid_range[0]=ovalid_range[0];
         valid_range[1]=ovalid_range[1];
         vrange_set = TRUE;
      }
   }

   /* Always write valid range for floating point values */
   if ((otype==NC_FLOAT) || (otype==NC_DOUBLE)) {
      ovalid_range[0]=0.0;
      ovalid_range[1]=1.0;
      ovrange_set=TRUE;
   }

   /* Check that time variables correspond to given dimension size */
   if (strcmp(dimname[0], MItime) == 0) {
      time_size = dimlength[0];
   }
   else if (strcmp(dimname[1], MItime) == 0) {
      time_size = dimlength[1];
   }
   else {
      time_size = 0;
   }
   if (((num_frame_times != time_size) && (num_frame_times > 0)) || 
       ((num_frame_widths != time_size) && (num_frame_widths > 0))) {
      (void) fprintf(stderr, 
   "Number of frame times or widths does not match number of frames.\n");
      exit(EXIT_FAILURE);
   }

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : usage_error
@INPUT      : progname - program name
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Prints a usage error message and exits.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : September 25, 1992 (Peter Neelin)
@MODIFIED   : December 2, 1992 (P.N.)
---------------------------------------------------------------------------- */
void usage_error(char *progname)
{
   (void) fprintf(stderr, "\nUsage: %s [<options>] <output.mnc> ", 
                  progname);
   (void) fprintf(stderr, "[[<size4>] <size3>] <size2> <size1>\n");
   (void) fprintf(stderr,   "       %s [-help]\n\n", progname);

   exit(ERROR_STATUS);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_attribute
@INPUT      : dst - client data passed by ParseArgv
              key - matching key in argv
              nextarg - argument following key in argv
@OUTPUT     : (none)
@RETURNS    : TRUE since nextarg is used.
@DESCRIPTION: Gets attributes from command line. Syntax for argument is
              "<var>:<att>=<value>". Numeric values are converted to
              double precision.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : May 3, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
int get_attribute(char *dst, char *key, char *nextarg)
{           /* ARGSUSED */
   char *variable;
   char *attribute;
   char *value;
   char *end;
   double dvalue;

   /* Check for a following argument */
   if (nextarg == NULL) {
      (void) fprintf(stderr, 
                     "\"%s\" option requires an additional argument\n",
                     key);
      exit(EXIT_FAILURE);
   }

   /* Get the variable name */
   variable = nextarg;
   attribute = strchr(variable, ':');
   if (attribute == NULL) {
      (void) fprintf(stderr, 
                     "%s option requires argument <var>:<attr>=<val>\n", 
                     key);
      exit(EXIT_FAILURE);
   }
   *attribute = '\0';
   attribute++;

   /* Get the value */
   value = strchr(attribute, '=');
   if (value == NULL) {
      (void) fprintf(stderr, 
                     "%s option requires argument <var>:<attr>=<val>\n", 
                     key);
      exit(EXIT_FAILURE);
   }
   *value = '\0';
   value++;

   /* Save the information */
   attribute_list_size++;
   if (attribute_list_size > attribute_list_alloc) {
      attribute_list_alloc += 10;
      if (attribute_list == NULL) {
         attribute_list = 
            MALLOC(attribute_list_alloc * sizeof(*attribute_list));
      }
      else {
         attribute_list = 
            REALLOC(attribute_list, 
                    attribute_list_alloc * sizeof(*attribute_list));
      }
   }
   attribute_list[attribute_list_size-1].variable = variable;
   attribute_list[attribute_list_size-1].attribute = attribute;
   attribute_list[attribute_list_size-1].value = value;

   /* Try to get a double precision value */
   dvalue = strtod(value, &end);
   if ((end != value) && (*end == '\0')) {
      attribute_list[attribute_list_size-1].value = NULL;
      attribute_list[attribute_list_size-1].double_value = dvalue;
   }

   return TRUE;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_times
@INPUT      : dst - client data passed by ParseArgv
              key - matching key in argv
              nextarg - argument following key in argv
@OUTPUT     : (none)
@RETURNS    : TRUE since nextarg is used.
@DESCRIPTION: Gets frame start times or frame lengths from next argument.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : May 3, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
int get_times(char *dst, char *key, char *nextarg)
{           /* ARGSUSED */
   int *num_elements_ptr;
   double **time_list_ptr;
   int num_elements;
   int num_alloc;
   double *time_list;
   double dvalue;
   char *cur, *end, *prev;

   /* Check for a following argument */
   if (nextarg == NULL) {
      (void) fprintf(stderr, 
                     "\"%s\" option requires an additional argument\n",
                     key);
      exit(EXIT_FAILURE);
   }

   /* Get pointers to global variables */
   if (strcmp(key, "-frame_times") == 0) {
      num_elements_ptr = &num_frame_times;
      time_list_ptr = &frame_times;
   }
   else if (strcmp(key, "-frame_widths") == 0) {
      num_elements_ptr = &num_frame_widths;
      time_list_ptr = &frame_widths;
   }
   else {
      (void) fprintf(stderr, "Unknown option \"%s\".\n", key);
      exit(EXIT_FAILURE);
   }

   /* Set up pointers to end of string and first non-space character */
   end = nextarg + strlen(nextarg);
   cur = nextarg;
   while (isspace(*cur)) cur++;
   num_elements = 0;
   num_alloc = 0;
   time_list = NULL;

   /* Loop through string looking for doubles */
   while (cur!=end) {

      /* Get double */
      prev = cur;
      dvalue = strtod(prev, &cur);
      if (cur == prev) {
         (void) fprintf(stderr, 
            "expected vector of doubles for \"%s\", but got \"%s\"\n", 
                        key, nextarg);
         exit(EXIT_FAILURE);
      }

      /* Add the value to the list */
      num_elements++;
      if (num_elements > num_alloc) {
         num_alloc += 20;
         if (time_list == NULL) {
            time_list = 
               MALLOC(num_alloc * sizeof(*time_list));
         }
         else {
            time_list = 
               REALLOC(time_list, num_alloc * sizeof(*time_list));
         }
      }
      time_list[num_elements-1] = dvalue;

      /* Skip any spaces */
      while (isspace(*cur)) cur++;

      /* Skip an optional comma */
      if (*cur == VECTOR_SEPARATOR) cur++;

   }

   /* Update the global variables */
   *num_elements_ptr = num_elements;
   if (*time_list_ptr != NULL) {
      FREE(*time_list_ptr);
   }
   *time_list_ptr = time_list;


   

   return TRUE;
}
