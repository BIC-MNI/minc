/* ----------------------------- MNI Header -----------------------------------
@NAME       : minc_error.c
@DESCRIPTION: File containing routines to do error handling for MINC package.
              Should be called through macros in minc_private.h
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 7, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/minc/libsrc/minc_error.c,v 1.3 1992-12-01 14:03:54 neelin Exp $ MINC (MNI)";
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
semiprivate int MI_return()
{ 
   ncopts = MI_CALLERS_NCOPTS; 
   return(TRUE); 
}
semiprivate int MI_return_error()
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

