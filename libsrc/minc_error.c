/* ----------------------------- MNI Header -----------------------------------
@NAME       : minc_error.c
@DESCRIPTION: File containing routines to do error handling for MINC package.
              Should be called through macros in minc_private.h
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 7, 1992 (Peter Neelin)
@MODIFIED   : $Log: minc_error.c,v $
@MODIFIED   : Revision 2.0  1994-09-28 10:38:04  neelin
@MODIFIED   : Release of minc version 0.2
@MODIFIED   :
 * Revision 1.7  94/09/28  10:37:16  neelin
 * Pre-release
 * 
 * Revision 1.6  93/08/11  12:06:24  neelin
 * Added RCS logging in source.
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
static char rcsid[] = "$Header: /private-cvsroot/minc/libsrc/minc_error.c,v 2.0 1994-09-28 10:38:04 neelin Exp $ MINC (MNI)";
#endif

#include <minc_private.h>

/* NetCDF error handling routines */
void nc_serror(char *fmt, ...) ;
void NCadvise(int err, char *fmt,...) ;


semiprivate int MI_save_routine_name(char *name)
{
   MI_ROUTINE_VAR = name; 
   MI_CALLERS_NCOPTS = ncopts; 
   ncopts &= ~(NC_FATAL); 
   return(TRUE);
}
semiprivate int MI_return(void)
{ 
   ncopts = MI_CALLERS_NCOPTS; 
   return(TRUE); 
}
semiprivate int MI_return_error(void)
{ 
   ncopts = MI_CALLERS_NCOPTS; 
   MI_LOG_PKG_ERROR2(ncerr, "MINC package entry point"); 
   return(TRUE);
}
semiprivate void MI_log_pkg_error2(int p1, char *p2)
{ 
   MI_NC_ROUTINE_VAR = MI_ROUTINE_VAR; 
   NCadvise(p1,p2); 
   return;
}
semiprivate void MI_log_pkg_error3(int p1, char *p2, char *p3)
{ 
   MI_NC_ROUTINE_VAR = MI_ROUTINE_VAR; 
   NCadvise(p1,p2,p3); 
}
semiprivate void MI_log_sys_error1(char *p1)
{ 
   MI_NC_ROUTINE_VAR = MI_ROUTINE_VAR; 
   nc_serror(p1); 
}

