/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /private-cvsroot/minc/progs/mincdump/mincdump.h,v 1.1 2004/04/27 15:35:15 bert Exp $
 *********************************************************************/


/* error checking macro */
#define NC_CHECK(status)    {\
	    int nc_status = status;\
	    if(nc_status < 0)\
	        error(nc_strerror(nc_status));\
	}

#define  Printf  (void) printf

typedef int boolean;
enum {false=0, true=1};

struct ncdim {			/* dimension */
    char name[NC_MAX_NAME];
    long size;
};

struct ncvar {			/* variable */
    char name[NC_MAX_NAME];
    nc_type type;
    int ndims;
    int dims[MAX_VAR_DIMS];
    int natts;
    boolean has_fillval;
    double fillval;
};

struct ncatt {			/* attribute */
    int var;
    char name[NC_MAX_NAME];
    nc_type type;
    int len;
    char *string;		/* for text attributes (type = NC_CHAR) */
    double *vals;		/* for numeric attributes of all types */
};

typedef
enum {LANG_C, LANG_F} Nclang; 

struct fspec {			/* specification for how to format dump */

    char *name;			/* name specified with -n or derived from
				 * file name */

    boolean header_only;	/* if true, don't print any variable data */

    boolean coord_vals;		/* if true, print header and coordinate
				 * dimension values (values of variables
				 * that are also dimensions), but no other
				 * variable data */

    boolean brief_data_cmnts;	/* if true, put // comments in data section
				 * identifying variable and indices, useful
				 * for navigating through large
				 * multi-dimensional data lists.  */

    boolean full_data_cmnts;	/* if true, put // comments in data section
				 * identifying every value, useful for
				 * navigating through large
				 * multi-dimensional data lists.  */

    Nclang data_lang;		/* Specifies index conventions used in data
				 * comments, either LANG_C (C, 0-based,
				 * column major) or LANG_F (Fortran,
				 * 1-based, row major) */

    int nlvars;			/* Number of variables specified with -v
				 * option on command line */

    char** lvars;		/* list of variable names specified with -v
				 * option on command line */
};
