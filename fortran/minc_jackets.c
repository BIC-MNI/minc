/* ----------------------------- MNI Header -----------------------------------
@NAME       : minc_jackets.c
@DESCRIPTION: File containing source for creating jacket routines for 
              interfacing minc C routines with fortran.
@METHOD     : Provides a standard source file for fortran jacket routines
              that can be compiled on different machines, creating
              fortran-callable C. The following is extracted from 
              jackets.src in the netcdf distribution (with the first
              line modified to MINC) :
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 28, 1993
@MODIFIED   : 
 * $Log: minc_jackets.c,v $
 * Revision 6.1  2001-04-10 19:42:11  neelin
 * Adding irix fortran wrappers to package for backwards compatibility
 *
 * Revision 6.1  1999/10/19 14:45:06  neelin
 * Fixed Log subsitutions for CVS
 *
 * Revision 6.0  1997/09/12 13:24:34  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:25:35  neelin
 * Release of minc version 0.5
 * 
 * Revision 4.0  1997/05/07  20:06:39  neelin
 * Release of minc version 0.4
 * 
 * Revision 3.1  1997/01/14  19:46:53  neelin
 * Added wrappers for miopn, micre and miclos.
 * 
 * Revision 3.0  1995/05/15  19:32:47  neelin
 * Release of minc version 0.3
 * 
 * Revision 2.1  1995/02/08  14:07:58  neelin
 * Removed declaration of miicv_put function.
 * 
 * Revision 2.0  1994/09/28  10:38:47  neelin
 * Release of minc version 0.2
 * 
 * Revision 1.6  94/09/28  10:38:28  neelin
 * Pre-release
 * 
 * Revision 1.5  93/08/11  15:08:48  neelin
 * Added RCS logging in sources code.
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

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/minc/fortran/Attic/minc_jackets.c,v 6.1 2001-04-10 19:42:11 neelin Exp $ MINC (MNI)";
#endif

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <minc.h>





#include "netcdf_jacket_code.c"

/* Define type for smallest integer */
#if (INT_MAX == SHRT_MAX)
#  define NC_INT NC_SHORT
#else
#  define NC_INT NC_LONG
#endif

/* ------------ IRIX FORTRAN jackets for MINC Functions ------------ */

/* From netcdf_convenience.c */

/*
 * Fortran version of miopen
 */
int
miopn_(path, mode, rcode, pathlen)
    char	*path;	
    int		pathlen;
    int		*mode;	
    int		*rcode;	
{
   char *name;
   int mincid;

   *rcode = 0;

   /* Copy the path */
   name = malloc(pathlen + 1);
   (void) nstrncpy (name, path, pathlen);

   /* Call the C function */
   mincid = miopen(name, *mode);
   free(name);
   if (mincid == MI_ERROR) {
      *rcode = ncerr;
   }
   return mincid;

}


/*
 * Fortran version of micreate
 */
int
micre_(path, cmode, rcode, pathlen)
    char	*path;	
    int		pathlen;
    int		*cmode;	
    int		*rcode;	
{
   char *name;
   int mincid;

   *rcode = 0;

   /* Copy the path */
   name = malloc(pathlen + 1);
   (void) nstrncpy (name, path, pathlen);

   /* Call the C function */
   mincid = micreate(name, *cmode);
   free(name);
   if (mincid == MI_ERROR) {
      *rcode = ncerr;
   }
   return mincid;

}


/*
 * Fortran version of miclose
 */
void
miclos_(cdfid, rcode)
    int		*cdfid;	
    int		*rcode;	
{

   *rcode = 0;
   if (miclose(*cdfid) == MI_ERROR) {
      *rcode = ncerr;
   }

}


/*
 * Fortran version of miattget
 */
void
miagt_(cdfid, varid, attname, datatype, max_length, value, att_length, rcode, attnamelen)
    int		*cdfid;	
    int		*varid;	
    char	*attname;	
    int		attnamelen;
    int		*datatype;	
    int		*max_length;	
    void	*value;	
    int		*att_length;	
    int		*rcode;	
{
   char name[MAX_NC_NAME + 1];

   (void) nstrncpy (name, attname, attnamelen);
   *rcode = 0;
#ifdef FORTRAN_HAS_NO_BYTE
   if ((nc_type) *datatype == NC_BYTE) {
      if (miattget (*cdfid, *varid - 1, name, NC_INT, *max_length, value,
                    att_length) == MI_ERROR) {
         *rcode = ncerr;
      }
	   return;
   }				/* else */
#endif				/* FORTRAN_HAS_NO_BYTE */
#ifdef FORTRAN_HAS_NO_SHORT
   if ((nc_type) *datatype == NC_SHORT) {
      if (miattget (*cdfid, *varid - 1, name, NC_INT, *max_length, value,
                    att_length) == MI_ERROR) {
         *rcode = ncerr;
      }
	   return;
   }				/* else */
#endif				/* FORTRAN_HAS_NO_SHORT */

   if (miattget (*cdfid, *varid - 1, name, *datatype, *max_length, value,
                 att_length) == MI_ERROR) {
      *rcode = ncerr;
   }
}

/*
 * Fortran version of miattget1
 */
void
miagt1_(cdfid, varid, attname, datatype, value, rcode, attnamelen)
    int		*cdfid;	
    int		*varid;	
    char	*attname;	
    int		attnamelen;
    int		*datatype;	
    void	*value;	
    int		*rcode;	
{
   char name[MAX_NC_NAME + 1];

   (void) nstrncpy (name, attname, attnamelen);
   *rcode = 0;
#ifdef FORTRAN_HAS_NO_BYTE
   if ((nc_type) *datatype == NC_BYTE) {
      if (miattget1 (*cdfid, *varid - 1, name, NC_INT, value) == MI_ERROR) {
         *rcode = ncerr;
      }
	   return;
   }				/* else */
#endif				/* FORTRAN_HAS_NO_BYTE */
#ifdef FORTRAN_HAS_NO_SHORT
   if ((nc_type) *datatype == NC_SHORT) {
      if (miattget1 (*cdfid, *varid - 1, name, NC_INT, value) == MI_ERROR) {
         *rcode = ncerr;
      }
	   return;
   }				/* else */
#endif				/* FORTRAN_HAS_NO_SHORT */

   if (miattget1 (*cdfid, *varid - 1, name, *datatype, value) == MI_ERROR) {
      *rcode = ncerr;
   }
}


/*
 * Fortran version of mivarget
 */
void
mivgt_(cdfid, varid, start, count, datatype, datasign, value, rcode, datasignlen)
    int		*cdfid;	
    int		*varid;	
    long		*start;	
    long		*count;	
    int		*datatype;	
    char	*datasign;	
    int		datasignlen;
    void	*value;	
    int		*rcode;	
{
   char sign[MAX_NC_NAME + 1];

   long ncount[MAX_VAR_DIMS], nstart[MAX_VAR_DIMS];
   int i, ndims;

   (void) nstrncpy (sign, datasign, datasignlen);
   if (ncvarinq (*cdfid, *varid - 1, NULL, NULL,
                 &ndims, NULL, NULL) == -1) {
   	*rcode = ncerr;
	   return;
   }
   for (i = 0; i < ndims; i++) {
	   ncount[i] = count[i];
   	nstart[i] = start[i] - 1;
   }
   revlongs (ncount, ndims);
   revlongs (nstart, ndims);

   *rcode = 0;
#ifdef FORTRAN_HAS_NO_BYTE
   if ((nc_type) *datatype == NC_BYTE) {
      if (mivarget (*cdfid, *varid - 1, nstart, ncount, NC_INT, sign, value) 
                        == MI_ERROR) {
   	   *rcode = ncerr;
      }
   	return;
   }				/* else */
#endif				/* FORTRAN_HAS_NO_BYTE */
#ifdef FORTRAN_HAS_NO_SHORT
   if ((nc_type) *datatype == NC_SHORT) {
      if (mivarget (*cdfid, *varid - 1, nstart, ncount, NC_INT, sign, value) 
                        == MI_ERROR) {
   	   *rcode = ncerr;
      }
	   return;
   }				/* else */
#endif				/* FORTRAN_HAS_NO_SHORT */
   if (mivarget (*cdfid, *varid - 1, nstart, ncount, *datatype, sign, value) 
                     == MI_ERROR) {
   	*rcode = ncerr;
   }
}

/*
 * Fortran version of mivarget1
 */
void
mivgt1_(cdfid, varid, indices, datatype, datasign, value, rcode, datasignlen)
    int		*cdfid;	
    int		*varid;	
    long		*indices;	
    int		*datatype;	
    char	*datasign;	
    int		datasignlen;
    void	*value;	
    int		*rcode;	
{
   char sign[MAX_NC_NAME + 1];
   long nindices[MAX_VAR_DIMS], i;
   int ndims;

   (void) nstrncpy (sign, datasign, datasignlen);
   if (ncvarinq (*cdfid, *varid - 1, NULL, NULL,
       &ndims, NULL, NULL) == -1) {
   	*rcode = ncerr;
	   return;
   }
   for (i = 0; i < ndims; i++) {
   	nindices[i] = indices[i] - 1;
   }
   revlongs (nindices, ndims);
   *rcode = 0;
#ifdef FORTRAN_HAS_NO_BYTE
   if ((nc_type) *datatype == NC_BYTE) {
      if (mivarget1 (*cdfid, *varid - 1, nindices, NC_INT, sign, value) 
                        == MI_ERROR) {
      	*rcode = ncerr;
      }
   	return;
   }				/* else */
#endif				/* FORTRAN_HAS_NO_BYTE */
#ifdef FORTRAN_HAS_NO_SHORT
   if ((nc_type) *datatype == NC_SHORT) {
      if (mivarget1 (*cdfid, *varid - 1, nindices, NC_INT, sign, value) 
                        == MI_ERROR) {
   	   *rcode = ncerr;
      }
   	return;
   }				/* else */
#endif				/* FORTRAN_HAS_NO_SHORT */
   if (mivarget1 (*cdfid, *varid - 1, nindices, *datatype, sign, value) 
                     == MI_ERROR) {
   	*rcode = ncerr;
   }
}

/*
 * Fortran version of mivarput
 */
void
mivpt_(cdfid, varid, start, count, datatype, datasign, value, rcode, datasignlen)
    int		*cdfid;	
    int		*varid;	
    long		*start;	
    long		*count;	
    int		*datatype;	
    char	*datasign;	
    int		datasignlen;
    void	*value;	
    int		*rcode;	
{
   char sign[MAX_NC_NAME + 1];

   long ncount[MAX_VAR_DIMS], nstart[MAX_VAR_DIMS];
   int i, ndims;

   (void) nstrncpy (sign, datasign, datasignlen);
   if (ncvarinq (*cdfid, *varid - 1, NULL, NULL,
                 &ndims, NULL, NULL) == -1) {
   	*rcode = ncerr;
	   return;
   }
   for (i = 0; i < ndims; i++) {
	   ncount[i] = count[i];
   	nstart[i] = start[i] - 1;
   }
   revlongs (ncount, ndims);
   revlongs (nstart, ndims);

   *rcode = 0;
#ifdef FORTRAN_HAS_NO_BYTE
   if ((nc_type) *datatype == NC_BYTE) {
      if (mivarput (*cdfid, *varid - 1, nstart, ncount, NC_INT, sign, value) 
                        == MI_ERROR) {
   	   *rcode = ncerr;
      }
   	return;
   }				/* else */
#endif				/* FORTRAN_HAS_NO_BYTE */
#ifdef FORTRAN_HAS_NO_SHORT
   if ((nc_type) *datatype == NC_SHORT) {
      if (mivarput (*cdfid, *varid - 1, nstart, ncount, NC_INT, sign, value) 
                        == MI_ERROR) {
   	   *rcode = ncerr;
      }
	   return;
   }				/* else */
#endif				/* FORTRAN_HAS_NO_SHORT */
   if (mivarput (*cdfid, *varid - 1, nstart, ncount, *datatype, sign, value) 
                     == MI_ERROR) {
   	*rcode = ncerr;
   }
}

/*
 * Fortran version of mivarput1
 */
void
mivpt1_(cdfid, varid, indices, datatype, datasign, value, rcode, datasignlen)
    int		*cdfid;	
    int		*varid;	
    long		*indices;	
    int		*datatype;	
    char	*datasign;	
    int		datasignlen;
    void	*value;	
    int		*rcode;	
{
   char sign[MAX_NC_NAME + 1];
   long nindices[MAX_VAR_DIMS], i;
   int ndims;

   (void) nstrncpy (sign, datasign, datasignlen);
   if (ncvarinq (*cdfid, *varid - 1, NULL, NULL,
       &ndims, NULL, NULL) == -1) {
   	*rcode = ncerr;
	   return;
   }
   for (i = 0; i < ndims; i++) {
   	nindices[i] = indices[i] - 1;
   }
   revlongs (nindices, ndims);
   *rcode = 0;
#ifdef FORTRAN_HAS_NO_BYTE
   if ((nc_type) *datatype == NC_BYTE) {
      if (mivarput1 (*cdfid, *varid - 1, nindices, NC_INT, sign, value) 
                        == MI_ERROR) {
      	*rcode = ncerr;
      }
   	return;
   }				/* else */
#endif				/* FORTRAN_HAS_NO_BYTE */
#ifdef FORTRAN_HAS_NO_SHORT
   if ((nc_type) *datatype == NC_SHORT) {
      if (mivarput1 (*cdfid, *varid - 1, nindices, NC_INT, sign, value) 
                        == MI_ERROR) {
   	   *rcode = ncerr;
      }
   	return;
   }				/* else */
#endif				/* FORTRAN_HAS_NO_SHORT */
   if (mivarput1 (*cdfid, *varid - 1, nindices, *datatype, sign, value) 
                     == MI_ERROR) {
   	*rcode = ncerr;
   }
}

/*
 * Fortran version of miset_coords
 */
void
mistcd_(nvals, value, coords, rcode)
    int		*nvals;	
    long		*value;	
    long		*coords;	
    int		*rcode;	
{
   *rcode=0;
   if (miset_coords(*nvals, *value, coords) == NULL) {
      *rcode=ncerr;
      return;
   }
   return;
}

/*
 * Fortran version of mitranslate_coords
 */
void
mitrcd_(cdfid, invar, incoords, outvar, outcoords, rcode)
    int		*cdfid;	
    int		*invar;	
    long		*incoords;	
    long		*outvar;	
    long		*outcoords;	
    int		*rcode;	
{
   int ndims1, ndims2, i;
   long nincoords[MAX_VAR_DIMS];

   *rcode=0;
   if ((ncvarinq(*cdfid, *invar - 1, NULL, NULL, &ndims1, NULL, NULL)
                                  == -1) ||
       (ncvarinq(*cdfid, *outvar - 1, NULL, NULL, &ndims2, NULL, NULL)
                                  == -1)) {
   	*rcode = ncerr;
	   return;
   }
   for (i = 0; i < ndims1; i++) {
	   nincoords[i] = incoords[i];
   }
   revlongs (nincoords, ndims1);

   if (mitranslate_coords(*cdfid, *invar - 1, nincoords,
                          *outvar - 1, outcoords) == NULL) {
      *rcode=ncerr;
      return;
   }

   revlongs (outcoords, ndims2);

   return;
}

/*
 * Fortran version of micopy_all_atts
 */
void
micaat_(incdfid, invarid, outcdfid, outvarid, rcode)
    int		*incdfid;	
    int		*invarid;	
    int		*outcdfid;	
    int		*outvarid;	
    int		*rcode;	
{
   *rcode=0;
   if (micopy_all_atts(*incdfid, *invarid - 1, *outcdfid, *outvarid - 1)
                           == MI_ERROR) {
      *rcode=ncerr;
      return;
   }
   return;
}

/*
 * Fortran version of micopy_var_def
 */
int
micvd_(incdfid, invarid, outcdfid, rcode)
    int		*incdfid;	
    int		*invarid;	
    int		*outcdfid;	
    int		*rcode;	
{
   int outvarid;

   *rcode=0;
   if ((outvarid=micopy_var_def(*incdfid, *invarid - 1, *outcdfid))
                           == MI_ERROR) {
      *rcode=ncerr;
      return MI_ERROR;
   }
   return (outvarid + 1);
}

/*
 * Fortran version of micopy_var_values
 */
void
micvv_(incdfid, invarid, outcdfid, outvarid, rcode)
    int		*incdfid;	
    int		*invarid;	
    int		*outcdfid;	
    int		*outvarid;	
    int		*rcode;	
{
   *rcode=0;
   if (micopy_var_values(*incdfid, *invarid - 1, *outcdfid, *outvarid - 1)
                           == MI_ERROR) {
      *rcode=ncerr;
      return;
   }
   return;
}

/*
 * Fortran version of micopy_all_var_defs
 */
void
micavd_(incdfid, outcdfid, nexclude, excluded_vars, rcode)
    int		*incdfid;	
    int		*outcdfid;	
    int		*nexclude;	
    int		*excluded_vars;	
    int		*rcode;	
{
   int i, *excl;

   *rcode=0;
   excl=malloc((*nexclude) * sizeof(*excl));
   for (i=0; i<(*nexclude); i++)
      excl[i] = excluded_vars[i] - 1;
   if (micopy_all_var_defs(*incdfid, *outcdfid, *nexclude, excl)
                           == MI_ERROR) {
      free(excl);
      *rcode=ncerr;
      return;
   }
   free(excl);
   return;
}

/*
 * Fortran version of micopy_all_var_values
 */
void
micavv_(incdfid, outcdfid, nexclude, excluded_vars, rcode)
    int		*incdfid;	
    int		*outcdfid;	
    int		*nexclude;	
    int		*excluded_vars;	
    int		*rcode;	
{
   int i, *excl;

   *rcode=0;
   excl=malloc((*nexclude) * sizeof(*excl));
   for (i=0; i<(*nexclude); i++)
      excl[i] = excluded_vars[i] - 1;
   if (micopy_all_var_values(*incdfid, *outcdfid, *nexclude, excl)
                           == MI_ERROR) {
      free(excl);
      *rcode=ncerr;
      return;
   }
   free(excl);
   return;
}

/* From minc_convenience.c */

/*
 * Fortran version of miattput_pointer
 */
void
miaptp_(cdfid, varid, attname, ptrvarid, rcode, attnamelen)
    int		*cdfid;	
    int		*varid;	
    char	*attname;	
    int		attnamelen;
    int		*ptrvarid;	
    int		*rcode;	
{
   char name[MAX_NC_NAME + 1];

   (void) nstrncpy (name, attname, attnamelen);
   *rcode=0;
   if (miattput_pointer(*cdfid, *varid - 1, name, *ptrvarid - 1)
                           == MI_ERROR) {
      *rcode=ncerr;
      return;
   }
   return;
}

/*
 * Fortran version of miattget_pointer
 */
int
miagtp_(cdfid, varid, attname, rcode, attnamelen)
    int		*cdfid;	
    int		*varid;	
    char	*attname;	
    int		attnamelen;
    int		*rcode;	
{
   int ptrvarid;
   char name[MAX_NC_NAME + 1];

   (void) nstrncpy (name, attname, attnamelen);
   *rcode=0;
   if ((ptrvarid = miattget_pointer(*cdfid, *varid - 1, name))
                           == MI_ERROR) {
      *rcode=ncerr;
      return (MI_ERROR);
   }
   return (ptrvarid + 1);
}

/*
 * Fortran version of miadd_child
 */
void
michld_(cdfid, parent_varid, child_varid, rcode)
    int		*cdfid;	
    int		*parent_varid;	
    int		*child_varid;	
    int		*rcode;	
{
   *rcode=0;
   if (miadd_child(*cdfid, *parent_varid - 1, *child_varid - 1)
                           == MI_ERROR) {
      *rcode=ncerr;
      return;
   }
   return;
}

/*
 * Fortran version of micreate_std_variable
 */
int
mistdv_(cdfid, varname, datatype, ndims, dimarray, rcode, varnamelen)
    int		*cdfid;	
    char	*varname;	
    int		varnamelen;
    int		*datatype;	
    int		*ndims;	
    int		*dimarray;	
    int		*rcode;	
{
   int varid, i, dimid[MAX_VAR_DIMS];
   char name[MAX_NC_NAME + 1];

   *rcode = 0;
   (void) nstrncpy (name, varname, varnamelen);
   for (i = 0; i < *ndims; i++)
      dimid[i] = dimarray[i] - 1;
   reverse (dimid, *ndims);
   if ((varid = micreate_std_variable (*cdfid, name, (nc_type) *datatype, 
                                       *ndims, dimid)) == MI_ERROR) {
      *rcode = ncerr;
      return (MI_ERROR);
   }
   return (varid + 1);
}

/*
 * Fortran version of micreate_std_variable
 */
int
migrpv_(cdfid, varname, rcode, varnamelen)
    int		*cdfid;	
    char	*varname;	
    int		varnamelen;
    int		*rcode;	
{
   int varid;
   char name[MAX_NC_NAME + 1];

   *rcode = 0;
   (void) nstrncpy (name, varname, varnamelen);
   if ((varid = micreate_group_variable (*cdfid, name)) == MI_ERROR) {
      *rcode = ncerr;
      return (MI_ERROR);
   }
   return (varid + 1);
}


/* From image_conversion.c */

/*
 * Fortran version of miicv_create
 */
int
icvcrt_(rcode)
    int		*rcode;	
{
   int icvid;

   *rcode = 0;
   if ((icvid = miicv_create()) == MI_ERROR) {
      *rcode = ncerr;
      return (MI_ERROR);
   }
#ifdef FORTRAN_HAS_NO_SHORT
   if (miicv_setint (icvid, MI_ICV_TYPE, NC_INT) == MI_ERROR) {
   	   *rcode = ncerr;
         return
   }
#endif				/* FORTRAN_HAS_NO_SHORT */
   return (icvid);
}

/*
 * Fortran version of miicv_free
 */
void
icvfre_(icvid, rcode)
    int		*icvid;	
    int		*rcode;	
{
   *rcode = 0;
   if (miicv_free(*icvid) == MI_ERROR) {
      *rcode = ncerr;
      return;
   }
   return;
}

/*
 * Fortran version of miicv_setdbl
 */
void
icvstd_(icvid, icv_property, value, rcode)
    int		*icvid;	
    int		*icv_property;	
    double	*value;	
    int		*rcode;	
{
   double icv_value;

   *rcode = 0;
   icv_value = *value;
#ifdef FORTRAN_HAS_NO_BYTE
   if ((*icv_property==MI_ICV_TYPE) && ((nc_type) icv_value == NC_BYTE)) {
      icv_value = NC_INT;
   }
#endif				/* FORTRAN_HAS_NO_BYTE */
#ifdef FORTRAN_HAS_NO_SHORT
   if ((*icv_property==MI_ICV_TYPE) && ((nc_type) icv_value == NC_SHORT)) {
      icv_value = NC_INT;
   }
#endif				/* FORTRAN_HAS_NO_SHORT */
   if (miicv_setdbl(*icvid, *icv_property, icv_value) == MI_ERROR) {
      *rcode = ncerr;
      return;
   }
   return;
}

/*
 * Fortran version of miicv_setint
 */
void
icvsti_(icvid, icv_property, value, rcode)
    int		*icvid;	
    int		*icv_property;	
    int		*value;	
    int		*rcode;	
{
   int icv_value;

   *rcode = 0;
   icv_value = *value;
#ifdef FORTRAN_HAS_NO_BYTE
   if ((*icv_property==MI_ICV_TYPE) && ((nc_type) icv_value == NC_BYTE)) {
      icv_value = NC_INT;
   }
#endif				/* FORTRAN_HAS_NO_BYTE */
#ifdef FORTRAN_HAS_NO_SHORT
   if ((*icv_property==MI_ICV_TYPE) && ((nc_type) icv_value == NC_SHORT)) {
      icv_value = NC_INT;
   }
#endif				/* FORTRAN_HAS_NO_SHORT */
   if (miicv_setint(*icvid, *icv_property, icv_value) == MI_ERROR) {
      *rcode = ncerr;
      return;
   }
   return;
}

/*
 * Fortran version of miicv_setstr
 */
void
icvsts_(icvid, icv_property, icvvalue, rcode, icvvaluelen)
    int		*icvid;	
    int		*icv_property;	
    char	*icvvalue;	
    int		icvvaluelen;
    int		*rcode;	
{
   char value[MAX_NC_NAME + 1];

   *rcode = 0;
   (void) nstrncpy (value, icvvalue, icvvaluelen);
   if (miicv_setstr(*icvid, *icv_property, value) == MI_ERROR) {
      *rcode = ncerr;
      return;
   }
   return;
}

/*
 * Fortran version of miicv_inqdbl
 */
void
icvnqd_(icvid, icv_property, value, rcode)
    int		*icvid;	
    int		*icv_property;	
    double	*value;	
    int		*rcode;	
{
   *rcode = 0;
   if (miicv_inqdbl(*icvid, *icv_property, value) == MI_ERROR) {
      *rcode = ncerr;
      return;
   }
   if (*icv_property == MI_ICV_VARID) {
      *value += 1;
   }
   return;
}

/*
 * Fortran version of miicv_inqint
 */
void
icvnqi_(icvid, icv_property, value, rcode)
    int		*icvid;	
    int		*icv_property;	
    int		*value;	
    int		*rcode;	
{
   *rcode = 0;
   if (miicv_inqint(*icvid, *icv_property, value) == MI_ERROR) {
      *rcode = ncerr;
      return;
   }
   if (*icv_property == MI_ICV_VARID) {
      *value += 1;
   }
   return;
}

/*
 * Fortran version of miicv_setstr
 */
void
icvnqs_(icvid, icv_property, icvvalue, lenstr, rcode, icvvaluelen)
    int		*icvid;	
    int		*icv_property;	
    char	*icvvalue;	
    int		icvvaluelen;
    int		*lenstr;	
    int		*rcode;	
{
   char value[MI_MAX_ATTSTR_LEN];
   int i;

   *rcode = 0;
   if (miicv_inqstr(*icvid, *icv_property, value) == MI_ERROR) {
      *rcode = ncerr;
      return;
   }
   (void) strncpy(icvvalue, value, *lenstr);
   for (i=strlen(value); i < *lenstr; i++)
      icvvalue[i]=' ';
   return;
}

/*
 * Fortran version of miicv_ndattach
 */
void
icvnda_(icvid, cdfid, varid, rcode)
    int		*icvid;	
    int		*cdfid;	
    int		*varid;	
    int		*rcode;	
{
   *rcode = 0;
   if (miicv_ndattach(*icvid, *cdfid, *varid - 1) == MI_ERROR) {
      *rcode = ncerr;
      return;
   }
   return;
}

/*
 * Fortran version of miicv_detach
 */
void
icvdet_(icvid, rcode)
    int		*icvid;	
    int		*rcode;	
{
   *rcode = 0;
   if (miicv_detach(*icvid) == MI_ERROR) {
      *rcode = ncerr;
      return;
   }
   return;
}

/*
 * Fortran version of miicv_get
 */
void
icvget_(icvid, start, count, value, rcode)
    int		*icvid;	
    long		*start;	
    long		*count;	
    void	*value;	
    int		*rcode;	
{
   int cdfid, varid, i, ndims;
   long ncount[MAX_VAR_DIMS], nstart[MAX_VAR_DIMS];

   *rcode = 0;
   if ((miicv_inqint(*icvid, MI_ICV_CDFID, &cdfid) == MI_ERROR) ||
       (miicv_inqint(*icvid, MI_ICV_VARID, &varid) == MI_ERROR) ||
       (ncvarinq (cdfid, varid, NULL, NULL, &ndims, NULL, NULL) 
                           == MI_ERROR)) {
      *rcode = ncerr;
      return;
   }
   for (i = 0; i < ndims; i++) {
	   ncount[i] = count[i];
   	nstart[i] = start[i] - 1;
   }
   revlongs (ncount, ndims);
   revlongs (nstart, ndims);

   if (miicv_get(*icvid, nstart, ncount, value) == MI_ERROR) {
   	*rcode = ncerr;
   }

   return;
}

/*
 * Fortran version of miicv_put
 */
void
icvput_(icvid, start, count, value, rcode)
    int		*icvid;	
    long		*start;	
    long		*count;	
    void	*value;	
    int		*rcode;	
{
   int cdfid, varid, i, ndims;
   long ncount[MAX_VAR_DIMS], nstart[MAX_VAR_DIMS];

   *rcode = 0;
   if ((miicv_inqint(*icvid, MI_ICV_CDFID, &cdfid) == MI_ERROR) ||
       (miicv_inqint(*icvid, MI_ICV_VARID, &varid) == MI_ERROR) ||
       (ncvarinq (cdfid, varid, NULL, NULL, &ndims, NULL, NULL) 
                           == MI_ERROR)) {
      *rcode = ncerr;
      return;
   }
   for (i = 0; i < ndims; i++) {
	   ncount[i] = count[i];
   	nstart[i] = start[i] - 1;
   }
   revlongs (ncount, ndims);
   revlongs (nstart, ndims);

   if (miicv_put(*icvid, nstart, ncount, value) == MI_ERROR) {
   	*rcode = ncerr;
   }

   return;
}

/* From dim_conversion.c */

/*
 * Fortran version of miicv_attach
 */
void
icvatt_(icvid, cdfid, varid, rcode)
    int		*icvid;	
    int		*cdfid;	
    int		*varid;	
    int		*rcode;	
{
   *rcode = 0;
   if (miicv_attach(*icvid, *cdfid, *varid - 1) == MI_ERROR) {
      *rcode = ncerr;
      return;
   }
   return;
}

