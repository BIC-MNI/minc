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
@MODIFIED   : December 2, 1992 (P.N.)
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
static char rcsid[]="$Header: /private-cvsroot/minc/progs/rawtominc/rawtominc.c,v 1.9 1993-07-21 12:53:10 neelin Exp $";
#endif

#include <stdlib.h>
#include <stdio.h>
#include <minc.h>
#include <float.h>
#include <ParseArgv.h>
#include <time_stamp.h>
#include <minc_def.h>

/* Some constants */

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

/* Macros */
#define STR_EQ(s1,s2) (strcmp(s1,s2)==0)

/* Function declarations */
void parse_args(int argc, char *argv[]);
void usage_error(char *pname);

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
int clobber=TRUE;
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
       "Options for writing output file. Default = -clobber."},
   {"-clobber", ARGV_CONSTANT, (char *) TRUE, (char *) &clobber,
       "Overwrite existing file"},
   {"-noclobber", ARGV_CONSTANT, (char *) FALSE, (char *) &clobber,
       "Don't overwrite existing file"},
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
   {NULL, ARGV_END, NULL, NULL, NULL}
};

/* Main program */

main(int argc, char *argv[])
{
   int do_norm, do_vrange;
   int cdfid, imgid, maxid, minid, varid;
   int icv;
   long start[MAX_VAR_DIMS];
   long count[MAX_VAR_DIMS];
   long end[MAX_VAR_DIMS];
   int dim[MAX_VAR_DIMS];
   void *image;
   double *dimage;
   float *fimage;
   double imgmax, imgmin, value;
   long image_size, image_pix, nread, fastdim;
   int pix_size;
   int image_dims;
   int i, j;
   int index;
   FILE *instream;
   char *tm_stamp;

   /* Save time stamp and args */
   tm_stamp = time_stamp(argc, argv);

   /* Parse arguments */
   parse_args(argc, argv);

   /* Do normalization if input type is float */
   do_norm = (( type==NC_FLOAT) || ( type==NC_DOUBLE));

   /* Find max and min for vrange if output type is float */
   do_vrange = do_norm && (( otype==NC_FLOAT) || ( otype==NC_DOUBLE));
   
   /* Create an icv */
   icv=miicv_create();
   (void) miicv_setint(icv, MI_ICV_TYPE, type);
   (void) miicv_setstr(icv, MI_ICV_SIGN, sign);
   if (vrange_set) {
      (void) miicv_setdbl(icv, MI_ICV_VALID_MIN, valid_range[0]);
      (void) miicv_setdbl(icv, MI_ICV_VALID_MAX, valid_range[1]);
   }
   if (do_norm) {
      (void) miicv_setint(icv, MI_ICV_DO_NORM, TRUE);
      (void) miicv_setint(icv, MI_ICV_USER_NORM, TRUE);
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
      if (!STR_EQ(dimname[i], MItime)) {
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
   if (do_norm) {
      maxid = micreate_std_variable(cdfid, MIimagemax, 
                                    NC_DOUBLE, ndims-image_dims, dim);
      minid = micreate_std_variable(cdfid, MIimagemin, 
                                    NC_DOUBLE, ndims-image_dims, dim);
   }

   /* End definition mode */
   (void) ncendef(cdfid);

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
   instream=stdin;
   fastdim=ndims-image_dims-1;
   if (fastdim<0) fastdim=0;
   if (do_vrange) {
      ovalid_range[0]=DBL_MAX;
      ovalid_range[1]=(-DBL_MAX);
   }
   fimage = image; dimage = image;

   while (start[0] < end[0]) {

      /* Read in image */
      nread=fread(image, pix_size, image_pix, instream);
      if (nread!=image_pix) {
         (void) fprintf(stderr, "%s: Premature end of file.\n", pname);
         exit(ERROR_STATUS);
      }
 
      /* Search for max and min for float and double */
      if (do_norm) {
         imgmax=(-DBL_MAX);
         imgmin=DBL_MAX;
         for (j=0; j<image_pix; j++) {
            switch (type) {
            case NC_DOUBLE:
               value=dimage[j]; break;
            case NC_FLOAT:
               value=fimage[j]; break;
            }
            if (value<imgmin) imgmin=value;
            if (value>imgmax) imgmax=value;
         }
         if (do_vrange) {
            if (imgmin<ovalid_range[0]) ovalid_range[0]=imgmin;
            if (imgmax>ovalid_range[1]) ovalid_range[1]=imgmax;
         }

         /* Write the image max and min */
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
   (void) fprintf(stderr, "\nUsage: %s [<options>] <filename> ", progname);
   (void) fprintf(stderr, "[[<size4>] <size3>] <size2> <size1>\n");
   (void) fprintf(stderr,   "       %s [-help]\n\n", progname);

   exit(ERROR_STATUS);
}
