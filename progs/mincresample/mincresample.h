/* ----------------------------- MNI Header -----------------------------------
@NAME       : mincresample.h
@DESCRIPTION: Header file for mincresample.c
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 8, 1993 (Peter Neelin)
@MODIFIED   : $Log: mincresample.h,v $
@MODIFIED   : Revision 1.6  1993-08-11 13:34:20  neelin
@MODIFIED   : Converted to use Dave MacDonald's General_transform code.
@MODIFIED   : Fixed bug in get_slice - for non-linear transformations coord was
@MODIFIED   : transformed, then used again as a starting coordinate.
@MODIFIED   : Handle files that have image-max/min that doesn't vary over slices.
@MODIFIED   : Handle files that have image-max/min varying over row/cols.
@MODIFIED   : Allow volume to extend to voxel edge for -nearest_neighbour interpolation.
@MODIFIED   : Handle out-of-range values (-fill values from a previous mincresample, for
@MODIFIED   : example).
@MODIFIED   : Save transformation file as a string attribute to processing variable.
@MODIFIED   :
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
#define DEFAULT_MAX 1.0
#define DEFAULT_MIN 0.0
#define FILL_DEFAULT DBL_MAX   /* Fillvalue indicating -nofill */
#define SMALL_VALUE (100.0*FLT_MIN)   /* A small floating-point value */
#define TRANSFORM_BUFFER_INCREMENT 256
#define PROCESSING_VAR "processing"
#define TEMP_IMAGE_VAR "mincresample-temporary-image"
#ifndef TRUE
#  define TRUE 1
#  define FALSE 0
#endif

/* Types used in program */
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
   int size[VOL_NDIMS];      /* Size of each dimension */
   void *data;               /* Pointer to volume data */
   double *scale;            /* Pointer to array of scales for slices */
   double *offset;           /* Pointer to array of offsets for slices */
   Interpolating_Function interpolant;
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
   char units[WORLD_NDIMS][MI_MAX_ATTSTR_LEN];
   char spacetype[WORLD_NDIMS][MI_MAX_ATTSTR_LEN];
} Volume_Definition;

typedef struct {
   int verbose;
} Program_Flags;

typedef struct {
   char *file_name;
   char *file_contents;
   long buffer_length;
   General_transform *transformation;
} Transform_Info;

typedef struct {
   int clobber;
   nc_type datatype;
   int is_signed;
   double vrange[2];
   double fillvalue;
   Program_Flags flags;
   Interpolating_Function interpolant;
   Transform_Info transform_info;
   Volume_Definition volume_def;
} Arg_Data;

/* Macros used in program */

#define DO_TRANSFORM(result, transformation, coord) \
   general_transform_point(transformation, \
      coord[XCOORD], coord[YCOORD], coord[ZCOORD], \
      &result[XCOORD], &result[YCOORD], &result[ZCOORD])

#define IS_LINEAR(transformation) \
   (get_transform_type(transformation)==LINEAR)

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
   result[XCOORD] = vector[XCOORD] * scalar; \
   result[YCOORD] = vector[YCOORD] * scalar; \
   result[ZCOORD] = vector[ZCOORD] * scalar; \
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
   case NC_LONG: \
      if (volume->is_signed) \
         value = *((signed long *) volume->data + offset); \
      else \
         value = *((unsigned long *) volume->data + offset); \
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

public void get_arginfo(int argc, char *argv[],
                        Program_Flags *program_flags,
                        VVolume *in_vol, VVolume *out_vol, 
                        General_transform *transformation);
public void check_imageminmax(File_Info *fp);
public void get_file_info(char *filename, 
                          Volume_Definition *volume_def,
                          File_Info *file_info);
public void create_output_file(char *filename, int clobber, 
                               Volume_Definition *volume_def,
                               File_Info *in_file,
                               File_Info *out_file,
                               char *tm_stamp, 
                               Transform_Info *transform_info);
public void get_voxel_to_world_transf(Volume_Definition *volume_def, 
                                      General_transform *voxel_to_world);
public double get_default_range(char *what, nc_type datatype, int is_signed);
public void finish_up(VVolume *in_vol, VVolume *out_vol);
public int get_transformation(char *dst, char *key, char *nextArg);
public int get_model_file(char *dst, char *key, char *nextArg);
public int get_axis_order(char *dst, char *key, char *nextArg);
public int get_fillvalue(char *dst, char *key, char *nextArg);
public void resample_volumes(Program_Flags *program_flags,
                             VVolume *in_vol, VVolume *out_vol, 
                             General_transform *transformation);
public void load_volume(File_Info *file, long start[], long count[], 
                        Volume_Data *volume);
public void get_slice(long slice_num, VVolume *in_vol, VVolume *out_vol, 
                      General_transform *transformation, 
                      double *minimum, double *maximum);
public int trilinear_interpolant(Volume_Data *volume, 
                                 Coord_Vector coord, double *result);
public int tricubic_interpolant(Volume_Data *volume, 
                                Coord_Vector coord, double *result);
public int do_Ncubic_interpolation(Volume_Data *volume, 
                                   long index[], int cur_dim, 
                                   double frac[], double *result);
public int nearest_neighbour_interpolant(Volume_Data *volume, 
                                         Coord_Vector coord, double *result);

