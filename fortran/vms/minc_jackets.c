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
static char rcsid[] = "$Header: /private-cvsroot/minc/fortran/vms/Attic/minc_jackets.c,v 1.1 1999-08-17 13:24:03 neelin Exp $ MINC (MNI)";
#endif

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <minc.h>

#include descrip




#include "netcdf_jacket_code.c"

/* Define type for smallest integer */
#if (INT_MAX == SHRT_MAX)
#  define NC_INT NC_SHORT
#else
#  define NC_INT NC_LONG
#endif

/* ------------ VMS FORTRAN jackets for MINC Functions ------------ */

/* From netcdf_convenience.c */


/*
 * Fortran version of miattget
 */
void
miagt(cdfid, varid, attnamed, datatype, max_length, value, att_length, rcode)
    int		*cdfid;	
    int		*varid;	
    struct dsc$descriptor_s * attnamed;	
    int		*datatype;	
    int		*max_length;	
    void	*value;	
    int		*att_length;	
    int		*rcode;	
{
    char	*attname	= attnamed->dsc$a_pointer;
    int		attnamelen	= attnamed->dsc$w_length;
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
miagt1(cdfid, varid, attnamed, datatype, value, rcode)
    int		*cdfid;	
    int		*varid;	
    struct dsc$descriptor_s * attnamed;	
    int		*datatype;	
    void	*value;	
    int		*rcode;	
{
    char	*attname	= attnamed->dsc$a_pointer;
    int		attnamelen	= attnamed->dsc$w_length;
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
mivgt(cdfid, varid, start, count, datatype, datasignd, value, rcode)
    int		*cdfid;	
    int		*varid;	
    long		*start;	
    long		*count;	
    int		*datatype;	
    struct dsc$descriptor_s * datasignd;	
    void	*value;	
    int		*rcode;	
{
    char	*datasign	= datasignd->dsc$a_pointer;
    int		datasignlen	= datasignd->dsc$w_length;
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
mivgt1(cdfid, varid, indices, datatype, datasignd, value, rcode)
    int		*cdfid;	
    int		*varid;	
    long		*indices;	
    int		*datatype;	
    struct dsc$descriptor_s * datasignd;	
    void	*value;	
    int		*rcode;	
{
    char	*datasign	= datasignd->dsc$a_pointer;
    int		datasignlen	= datasignd->dsc$w_length;
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
mivpt(cdfid, varid, start, count, datatype, datasignd, value, rcode)
    int		*cdfid;	
    int		*varid;	
    long		*start;	
    long		*count;	
    int		*datatype;	
    struct dsc$descriptor_s * datasignd;	
    void	*value;	
    int		*rcode;	
{
    char	*datasign	= datasignd->dsc$a_pointer;
    int		datasignlen	= datasignd->dsc$w_length;
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
mivpt1(cdfid, varid, indices, datatype, datasignd, value, rcode)
    int		*cdfid;	
    int		*varid;	
    long		*indices;	
    int		*datatype;	
    struct dsc$descriptor_s * datasignd;	
    void	*value;	
    int		*rcode;	
{
    char	*datasign	= datasignd->dsc$a_pointer;
    int		datasignlen	= datasignd->dsc$w_length;
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
mistcd(nvals, value, coords, rcode)
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
mitrcd(cdfid, invar, incoords, outvar, outcoords, rcode)
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
micaat(incdfid, invarid, outcdfid, outvarid, rcode)
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
micvd(incdfid, invarid, outcdfid, rcode)
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
micvv(incdfid, invarid, outcdfid, outvarid, rcode)
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
micavd(incdfid, outcdfid, nexclude, excluded_vars, rcode)
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
micavv(incdfid, outcdfid, nexclude, excluded_vars, rcode)
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
miaptp(cdfid, varid, attnamed, ptrvarid, rcode)
    int		*cdfid;	
    int		*varid;	
    struct dsc$descriptor_s * attnamed;	
    int		*ptrvarid;	
    int		*rcode;	
{
    char	*attname	= attnamed->dsc$a_pointer;
    int		attnamelen	= attnamed->dsc$w_length;
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
miagtp(cdfid, varid, attnamed, rcode)
    int		*cdfid;	
    int		*varid;	
    struct dsc$descriptor_s * attnamed;	
    int		*rcode;	
{
    char	*attname	= attnamed->dsc$a_pointer;
    int		attnamelen	= attnamed->dsc$w_length;
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
michld(cdfid, parent_varid, child_varid, rcode)
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
mistdv(cdfid, varnamed, datatype, ndims, dimarray, rcode)
    int		*cdfid;	
    struct dsc$descriptor_s * varnamed;	
    int		*datatype;	
    int		*ndims;	
    int		*dimarray;	
    int		*rcode;	
{
    char	*varname	= varnamed->dsc$a_pointer;
    int		varnamelen	= varnamed->dsc$w_length;
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
migrpv(cdfid, varnamed, rcode)
    int		*cdfid;	
    struct dsc$descriptor_s * varnamed;	
    int		*rcode;	
{
    char	*varname	= varnamed->dsc$a_pointer;
    int		varnamelen	= varnamed->dsc$w_length;
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
icvcrt(rcode)
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
icvfre(icvid, rcode)
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
icvstd(icvid, icv_property, value, rcode)
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
icvsti(icvid, icv_property, value, rcode)
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
icvsts(icvid, icv_property, icvvalued, rcode)
    int		*icvid;	
    int		*icv_property;	
    struct dsc$descriptor_s * icvvalued;	
    int		*rcode;	
{
    char	*icvvalue	= icvvalued->dsc$a_pointer;
    int		icvvaluelen	= icvvalued->dsc$w_length;
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
icvnqd(icvid, icv_property, value, rcode)
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
icvnqi(icvid, icv_property, value, rcode)
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
icvnqs(icvid, icv_property, icvvalued, lenstr, rcode)
    int		*icvid;	
    int		*icv_property;	
    struct dsc$descriptor_s * icvvalued;	
    int		*lenstr;	
    int		*rcode;	
{
    char	*icvvalue	= icvvalued->dsc$a_pointer;
    int		icvvaluelen	= icvvalued->dsc$w_length;
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
icvnda(icvid, cdfid, varid, rcode)
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
icvdet(icvid, rcode)
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
icvget(icvid, start, count, value, rcode)
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

public int miicv_put(int icvid, long start[], long count[], void *values);
/*
 * Fortran version of miicv_put
 */
void
icvput(icvid, start, count, value, rcode)
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
icvatt(icvid, cdfid, varid, rcode)
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

