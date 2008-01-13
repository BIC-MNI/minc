/* ----------------------------- MNI Header -----------------------------------
@NAME       : mincreshape.h
@DESCRIPTION: Header file for mincreshape.c
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : March 11, 1994 (Peter Neelin)
@MODIFIED   : 
 * $Log: mincreshape.h,v $
 * Revision 6.5  2008-01-13 09:38:54  stever
 * Avoid compiler warnings about functions and variables that are defined
 * but not used.  Remove some such functions and variables,
 * conditionalize some, and move static declarations out of header files
 * into C files.
 *
 * Revision 6.4  2004/11/01 22:38:39  bert
 * Eliminate all references to minc_def.h
 *
 * Revision 6.3  2001/12/06 14:12:45  neelin
 * Trivial change to definition of NCOPTS_DEFAULT.
 *
 * Revision 6.2  1999/10/19 14:45:29  neelin
 * Fixed Log subsitutions for CVS
 *
 * Revision 6.1  1998/08/19 13:57:50  neelin
 * Added ARG_SEPARATOR to replace string literal for comma.
 *
 * Revision 6.0  1997/09/12  13:24:12  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:25:10  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:01:44  neelin
 * Release of minc version 0.4
 *
 * Revision 3.0  1995/05/15  19:32:36  neelin
 * Release of minc version 0.3
 *
 * Revision 1.3  1994/12/02  09:08:57  neelin
 * Moved nd_loop to proglib.
 *
 * Revision 1.2  94/11/23  11:47:08  neelin
 * Handle image-min/max properly when using icv for normalization.
 * 
 * Revision 1.1  94/11/02  16:22:00  neelin
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

/* Constants used in program */
#define NOFILL DBL_MAX   /* Fillvalue indicating -nofill */
#define FILL -DBL_MAX    /* Fillvalue for -fill */
#define NCOPTS_DEFAULT (NC_VERBOSE | NC_FATAL)
#define DEFAULT_MAX_CHUNK_SIZE_IN_KB (1024*4)
#define DIM_WIDTH_SUFFIX "-width"
#define ARG_SEPARATOR ','
#define VECTOR_SEPARATOR ','
#ifndef TRUE
#  define TRUE 1
#  define FALSE 0
#endif

/* Types used in program */

typedef struct {
   int verbose;
   int icvid, inmincid, outmincid, outimgid;
   nc_type output_datatype;
   int output_is_signed;
   int input_ndims;                  /* Number of input dimensions */
   int output_ndims;                 /* Number of output dimensions */
   long input_size[MAX_VAR_DIMS];    /* Size of input volume */
   long input_start[MAX_VAR_DIMS];   /* Start of desired hyperslab */
   long input_count[MAX_VAR_DIMS];   /* Size of desired hyperslab 
                                        (<0 means 1 and remove dimension) */ 
   int map_out_to_in[MAX_VAR_DIMS];  /* Map output dimension index to input */
   int map_in_to_out[MAX_VAR_DIMS];  /* Map input dimension index to output
                                        (-1 means no mapping) */
   int dim_used_in_block[MAX_VAR_DIMS]; /* TRUE if output dim used in block */
   int chunk_count[MAX_VAR_DIMS];    /* Specifies count for chunk hyperslab */
   int need_fillvalue;               /* TRUE if we will need a fill value */
   double fillvalue;                 /* Value to fill with (FILL_DEFAULT
                                        means fill with real value zero) */
   int do_block_normalization;       /* Normalize slices to block max/min */
   int do_icv_normalization;         /* Use icv for normalization */

   /* Note that a block is a hyperslab of the output volume in which all
      values are normalized the same way. A chunk is a hyperslab that is
      copied in one piece (smaller than or equal to a block). */

} Reshape_info;

typedef struct {
   int nentries;
   char *name[MAX_VAR_DIMS];
   long size[MAX_VAR_DIMS];
} Dimsize_list;

typedef struct {
   int nentries;
   char *name[MAX_VAR_DIMS];
   long start[MAX_VAR_DIMS];
   long count[MAX_VAR_DIMS];
} Axis_ranges;

/* Macros used in program */
#define ISSPACE(ch) (isspace((int)ch))
#define ABS(x) (((x) >= 0) ? (x) : (-(x)))
#define  MAX( x, y )  ( ((x) >= (y)) ? (x) : (y) )
#define  MIN( x, y )  ( ((x) <= (y)) ? (x) : (y) )

/* Function prototypes */
extern void copy_data(Reshape_info *reshape_info);
