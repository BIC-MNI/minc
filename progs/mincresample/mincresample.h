/* ----------------------------- MNI Header -----------------------------------
@NAME       : mincresample.h
@DESCRIPTION: Header file for mincresample.c
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 8, 1993 (Peter Neelin)
@MODIFIED   : 
 * $Log: mincresample.h,v $
 * Revision 6.8  2008/01/13 09:38:54  stever
 * Avoid compiler warnings about functions and variables that are defined
 * but not used.  Remove some such functions and variables,
 * conditionalize some, and move static declarations out of header files
 * into C files.
 *
 * Revision 6.7  2005/07/13 21:34:25  bert
 * Add sinc interpolant (ported from 1.X branch)
 *
 * Revision 6.6  2004/11/01 22:38:39  bert
 * Eliminate all references to minc_def.h
 *
 * Revision 6.5  2004/04/27 15:31:20  bert
 * Added -2 option
 *
 * Revision 6.4  2002/11/06 13:32:23  jason
 * Fixes to mincresample: setting the interpolation type is now done
 * through an enum rather than function pointers.
 *
 * Revision 6.3  2001/04/17 18:40:23  neelin
 * Modifications to work with NetCDF 3.x
 * In particular, changed NC_LONG to NC_INT (and corresponding longs to ints).
 * Changed NC_UNSPECIFIED to NC_NAT.
 * A few fixes to the configure script.
 *
 * Revision 6.2  2001/04/02 14:58:09  neelin
 * Added -keep_real_range option to prevent rescaling of slices on output
 *
 * Revision 6.1  1999/10/19 14:45:27  neelin
 * Fixed Log subsitutions for CVS
 *
 * Revision 6.0  1997/09/12 13:23:21  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:24:22  neelin
 * Release of minc version 0.5
 *
 * Revision 4.1  1997/08/13  15:41:12  neelin
 * Fixed initialization problem that caused crashing under Linux.
 *
 * Revision 4.0  1997/05/07  19:59:42  neelin
 * Release of minc version 0.4
 *
 * Revision 3.4  1996/01/31  15:22:02  neelin
 * Fixed bug in transformation of input sampling.
 *
 * Revision 3.3  1995/12/12  19:15:35  neelin
 * Added -spacetype, -talairach and -units options.
 *
 * Revision 3.2  1995/11/21  14:13:20  neelin
 * Transform input sampling with transformation and use this as default.
 * Added -tfm_input_sampling to specify above option.
 * Added -use_input_sampling to get old behaviour (no longer the default).
 * Added -origin option (to specify coordinate instead of start values).
 * Added -standard_sampling option (to set standard values of start, step
 * and direction cosines).
 * Added -invert_transformation option.
 *
 * Revision 3.1  1995/11/07  15:04:02  neelin
 * Modified argument parsing so that only one pass is done.
 *
 * Revision 3.0  1995/05/15  19:30:57  neelin
 * Release of minc version 0.3
 *
 * Revision 2.0  1994/09/28  10:32:48  neelin
 * Release of minc version 0.2
 *
 * Revision 1.11  94/09/28  10:32:40  neelin
 * Pre-release
 * 
 * Revision 1.10  93/11/04  15:13:40  neelin
 * Added support for irregularly spaced dimensions.
 * 
 * Revision 1.9  93/11/02  11:23:56  neelin
 * Handle imagemax/min potentially varying over slices (for vector data, etc.)
 * 
 * Revision 1.8  93/10/20  14:05:42  neelin
 * Added VOXEL_COORD_EPS - an epsilon for doing voxel coordinate comparisons.
 * 
 * Revision 1.7  93/08/11  14:31:50  neelin
 * Changed prototype for check_imageminmax.
 * 
 * Revision 1.6  93/08/11  13:34:20  neelin
 * Converted to use Dave MacDonald's General_transform code.
 * Fixed bug in get_slice - for non-linear transformations coord was
 * transformed, then used again as a starting coordinate.
 * Handle files that have image-max/min that doesn't vary over slices.
 * Handle files that have image-max/min varying over row/cols.
 * Allow volume to extend to voxel edge for -nearest_neighbour interpolation.
 * Handle out-of-range values (-fill values from a previous mincresample, for
 * example).
 * Save transformation file as a string attribute to processing variable.
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

/* Constants used in program */

/* Number of dimensions for various things */
#define VOL_NDIMS    3   /* Number of volume dimensions */
#define WORLD_NDIMS  3   /* Number of world spatial dimensions */
#define SLICE_NDIMS  2   /* Number of slice dimensions */
#define MAT_NDIMS    WORLD_NDIMS+1   /* Number of dims for homogenous matrix */
/* For referring to world axes in arrays subscripted by WORLD_NDIMS */
#define NO_AXIS -1
#define XAXIS 0
#define YAXIS 1
#define ZAXIS 2
/* For referring to volume axes in arrays subscripted by VOL_NDIMS */
#define SLC_AXIS  0
#define ROW_AXIS    1
#define COL_AXIS    2
/* For referring to slice axes in arrays subscripted by SLICE_NDIMS */
#define SLICE_ROW 0
#define SLICE_COL 1
/* For referring to world coordinates in Coord_Vector */
#define XCOORD 0
#define YCOORD 1
#define ZCOORD 2
/* For referring to volume coordinates in Coord_Vector */
#define SLICE  0
#define ROW    1
#define COLUMN 2
/* Various constants */
#define NO_VALUE DBL_MAX   /* Constant to flag fact that value not set */
#define DEFAULT_MAX 1.0
#define DEFAULT_MIN 0.0
#define FILL_DEFAULT DBL_MAX   /* Fillvalue indicating -nofill */
#define SMALL_VALUE (100.0*FLT_MIN)   /* A small floating-point value */
#define VOXEL_COORD_EPS (100.0*FLT_EPSILON)  /* Epsilon for voxel coords */
#define TRANSFORM_BUFFER_INCREMENT 256
#define PROCESSING_VAR "processing"
#define TEMP_IMAGE_VAR "mincresample-temporary-image"
#ifndef TRUE
#  define TRUE 1
#  define FALSE 0
#endif

/* Types used in program */
enum Interpolant_type { TRILINEAR, TRICUBIC, N_NEIGHBOUR, WINDOWED_SINC };

typedef double Coord_Vector[WORLD_NDIMS];

typedef struct {
   char *name;
   int mincid;
   int imgid;
   int maxid;
   int minid;
   int ndims;
   nc_type datatype;
   int is_signed;
   double vrange[2];                /* [0]=min, [1]=max */
   long nelements[MAX_VAR_DIMS];    /* Size of each dimension */
   int world_axes[MAX_VAR_DIMS];    /* Relates variable index to X, Y, Z 
                                       or NO_AXIS */
   int indices[VOL_NDIMS];        /* Indices of volume dimenions (subscripted
                                       from slowest to fastest) */
   int axes[WORLD_NDIMS];    /* Relates world X,Y,Z (index) to dimension 
                                order (value=0,1,2; 0=slowest varying) */
   int using_icv;            /* True if we are using an icv to read data */
   int icvid;                /* Id of icv (if used) */
   long slices_per_image;    /* Number of volume slices (row, column) per
                                minc file image */
   long images_per_file;     /* Number of minc file images in the file */
   int do_slice_renormalization; /* Flag indicating that we need to 
                                    loop through the data a second time, 
                                    recomputing the slices to normalize
                                    images properly */
   int keep_real_range;      /* Flag indicating whether we should keep
                                the real range of the input data or not */
} File_Info;

typedef struct {
   long size[SLICE_NDIMS];   /* Size of each dimension */
   double *data;             /* Pointer to slice data */
} Slice_Data;

typedef struct Volume_Data_Struct Volume_Data;
typedef int (*Interpolating_Function) 
     (Volume_Data *volume, Coord_Vector coord, double *result);
struct Volume_Data_Struct {
   nc_type datatype;         /* Type of data in volume */
   int is_signed;            /* Sign of data (TRUE if signed) */
   int use_fill;             /* TRUE if fill values should be used in
                                calculation of output image max/min */
   double fillvalue;         /* Value to return when out of bounds */
   double vrange[2];         /* [0]=min, [1]=max */
   double real_range[2];     /* Real min and max for current volume */
   int size[VOL_NDIMS];      /* Size of each dimension */
   void *data;               /* Pointer to volume data */
   double *scale;            /* Pointer to array of scales for slices */
   double *offset;           /* Pointer to array of offsets for slices */
   Interpolating_Function interpolant; /* Function Pointer */
};

typedef struct {
   File_Info *file;           /* Information about associated file */
   Volume_Data *volume;      /* Volume data for (input volume) */
   Slice_Data *slice;        /* Slice data for (output volume) */
   General_transform *voxel_to_world;
   General_transform *world_to_voxel;
} VVolume;

typedef struct {
   int axes[WORLD_NDIMS];    /* Relates world X,Y,Z (index) to dimension 
                                order (value=0,1,2; 0=slowest varying) */
   long nelements[WORLD_NDIMS]; /* These are subscripted by X, Y and Z */
   double step[WORLD_NDIMS];
   double start[WORLD_NDIMS];
   double dircos[WORLD_NDIMS][WORLD_NDIMS];
   double *coords[WORLD_NDIMS];
   char units[WORLD_NDIMS][MI_MAX_ATTSTR_LEN];
   char spacetype[WORLD_NDIMS][MI_MAX_ATTSTR_LEN];
} Volume_Definition;

typedef struct {
   int verbose;
} Program_Flags;

typedef struct {
   int invert_transform;
   char *file_name;
   char *file_contents;
   long buffer_length;
   General_transform *transformation;
} Transform_Info;

typedef struct {
   int clobber;
   int keep_real_range;
   nc_type datatype;
   int is_signed;
   double vrange[2];
   double fillvalue;
   double origin[3];
   Program_Flags flags;
   enum Interpolant_type interpolant_type;  /* Type of interpolation */
   Transform_Info transform_info;
   Volume_Definition volume_def;
   int v2format;                /* If non-zero, create a MINC 2.0 output */
} Arg_Data;

typedef struct {
   long last_index[VOL_NDIMS];
   long nelements[VOL_NDIMS];
   double *coords[VOL_NDIMS];
} Irregular_Transform_Data;

/* Macros used in program */

#define DO_TRANSFORM(result, transformation, coord) \
   general_transform_point(transformation, \
      coord[XCOORD], coord[YCOORD], coord[ZCOORD], \
      &result[XCOORD], &result[YCOORD], &result[ZCOORD])

#define DO_INVERSE_TRANSFORM(result, transformation, coord) \
   general_inverse_transform_point(transformation, \
      coord[XCOORD], coord[YCOORD], coord[ZCOORD], \
      &result[XCOORD], &result[YCOORD], &result[ZCOORD])

#define IS_LINEAR(transformation) \
   (get_transform_type(transformation)==LINEAR)

#define VECTOR_COPY(result, first) { \
   result[XCOORD] = first[XCOORD]; \
   result[YCOORD] = first[YCOORD]; \
   result[ZCOORD] = first[ZCOORD]; \
}

#define VECTOR_DIFF(result, first, second) { \
   result[XCOORD] = first[XCOORD] - second[XCOORD]; \
   result[YCOORD] = first[YCOORD] - second[YCOORD]; \
   result[ZCOORD] = first[ZCOORD] - second[ZCOORD]; \
}

#define VECTOR_ADD(result, first, second) { \
   result[XCOORD] = first[XCOORD] + second[XCOORD]; \
   result[YCOORD] = first[YCOORD] + second[YCOORD]; \
   result[ZCOORD] = first[ZCOORD] + second[ZCOORD]; \
}

#define VECTOR_SCALAR_MULT(result, vector, scalar) { \
   result[XCOORD] = vector[XCOORD] * (scalar); \
   result[YCOORD] = vector[YCOORD] * (scalar); \
   result[ZCOORD] = vector[ZCOORD] * (scalar); \
}

#ifdef INTERPOLATE
#  undef INTERPOLATE
#endif

#define INTERPOLATE(volume, coord, result) \
   (*volume->interpolant) (volume, coord, result)

#define VOLUME_VALUE(volume, slcind, rowind, colind, value) \
{ \
   long offset; \
 \
   offset = ((slcind)*volume->size[ROW_AXIS] + \
             (rowind))*volume->size[COL_AXIS] + (colind); \
   switch (volume->datatype) { \
   case NC_BYTE: \
      if (volume->is_signed) \
         value = *((signed char *) volume->data + offset); \
      else \
         value = *((unsigned char *) volume->data + offset); \
      break; \
   case NC_SHORT: \
      if (volume->is_signed) \
         value = *((signed short *) volume->data + offset); \
      else \
         value = *((unsigned short *) volume->data + offset); \
      break; \
   case NC_INT: \
      if (volume->is_signed) \
         value = *((signed int *) volume->data + offset); \
      else \
         value = *((unsigned int *) volume->data + offset); \
      break; \
   case NC_FLOAT: \
      value = *((float *) volume->data + offset); \
      break; \
   case NC_DOUBLE: \
      value = *((double *) volume->data + offset); \
      break; \
   } \
}

/* Function prototypes */

extern void resample_volumes(Program_Flags *program_flags,
                             VVolume *in_vol, VVolume *out_vol, 
                             General_transform *transformation);
extern int trilinear_interpolant(Volume_Data *volume, 
                                 Coord_Vector coord, double *result);
extern int tricubic_interpolant(Volume_Data *volume, 
                                Coord_Vector coord, double *result);
extern int nearest_neighbour_interpolant(Volume_Data *volume, 
                                         Coord_Vector coord, double *result);
extern int windowed_sinc_interpolant(Volume_Data *volume,
                                     Coord_Vector coord, double *result);

#define SINC_HALF_WIDTH_MAX 10
#define SINC_HALF_WIDTH_MIN 1

enum sinc_interpolant_window_t {
    SINC_WINDOW_NONE,
    SINC_WINDOW_HANNING,
    SINC_WINDOW_HAMMING
};

extern enum sinc_interpolant_window_t sinc_window_type;
extern int sinc_half_width;
