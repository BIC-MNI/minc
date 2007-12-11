/* ----------------------------- MNI Header -----------------------------------
@NAME       : vff2mnc.h
@DESCRIPTION: Header file that includes things needed for vff2mnc
@METHOD     : 
@GLOBALS    : 
@CREATED    : Jul 2006 (Leila Baghdadi)
@MODIFIED   : 
 * $Log: vff2mnc.h,v $
 * Revision 1.2  2007-12-11 12:43:01  rotor
 *  * added static to all global variables in main programs to avoid linking
 *       problems with libraries (compress in mincconvert and libz for example)
 *
 * Revision 1.1  2007/01/16 18:38:18  baghdadi
 * header file for vff2mnc
 *
 * Adopted from vfftominc (perl script) of John G. Sled.
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

#include <minc2.h>

#ifndef TRUE
#  define TRUE 1
#endif
#ifndef FALSE
#  define FALSE 0
#endif

#define MAX_VFF_DIMS 3
#define MAX_BUF_LINE 80
#define MAX_BUF_TEXT 1000
#define MAX_DESCRIPTION 2

typedef char string_t[511+1];
#define STRING_T_LEN (sizeof(string_t) - 1)

#ifndef S_ISDIR
#define S_ISDIR(x) (((x) & _S_IFMT) == _S_IFDIR)
#endif

#define CHKMEM(x) \
    if ((x) == NULL) \
        (fprintf(stderr, "Out of memory at %s:%d\n", __FILE__, __LINE__), \
         exit(-1))

#define TESTRPT(msg, val) (fprintf(stderr, \
                                  "Error reported on line #%d, %s: %d\n", \
                                  __LINE__, msg, val), r)

struct globals {
    char *minc_history;  /* Global for minc history */
    char *pname;         /* program name */
    int little_endian;
    char * dirname;
    int List;
   
};

extern struct globals G;

struct vff_attrs {
  int y_bin;
  int z_bin;
  int bands;
  double center_of_rotation;
  double central_slice;
  double rfan_y;
  double rfan_z;
  double angle_increment;
  int reverse_order;
  int bits;
  int day;
  int month;
  int year;
  string_t cmd_line;
};

struct mnc_vars {
  mitype_t mnc_type;   
  int mnc_ndims;              /* MINC image dimension count */  
  long mnc_start[MAX_VFF_DIMS]; /* MINC data starts */
  long mnc_count[MAX_VFF_DIMS]; /* MINC data counts */
  char *mnc_hist;             /* MINC history */
  double mnc_srange[2];       /* MINC image min/max */
  double mnc_starts[MAX_VFF_DIMS];
  double mnc_steps[MAX_VFF_DIMS];
};
