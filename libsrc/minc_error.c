/* ----------------------------- MNI Header -----------------------------------
@NAME       : minc_error.c
@DESCRIPTION: File containing routines to do error handling for MINC package.
              Should be called through macros in minc_private.h
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 7, 1992 (Peter Neelin)
@MODIFIED   : 
 * $Log: minc_error.c,v $
 * Revision 6.2.2.1  2004-09-28 20:23:40  bert
 * Minor portability fixes for Windows
 *
 * Revision 6.2  2001/04/17 18:40:13  neelin
 * Modifications to work with NetCDF 3.x
 * In particular, changed NC_LONG to NC_INT (and corresponding longs to ints).
 * Changed NC_UNSPECIFIED to NC_NAT.
 * A few fixes to the configure script.
 *
 * Revision 6.1  1999/10/19 14:45:09  neelin
 * Fixed Log subsitutions for CVS
 *
 * Revision 6.0  1997/09/12 13:24:54  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:25:53  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:07:52  neelin
 * Release of minc version 0.4
 *
 * Revision 3.0  1995/05/15  19:33:12  neelin
 * Release of minc version 0.3
 *
 * Revision 2.0  1994/09/28  10:38:04  neelin
 * Release of minc version 0.2
 *
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
static char rcsid[] = "$Header: /private-cvsroot/minc/libsrc/minc_error.c,v 6.2.2.1 2004-09-28 20:23:40 bert Exp $ MINC (MNI)";
#endif

#include "minc_private.h"
#include <errno.h>

SEMIPRIVATE int MI_save_routine_name(char *name)
{
   MI_ROUTINE_VAR = name; 
   MI_CALLERS_NCOPTS = ncopts; 
   ncopts &= ~(NC_FATAL); 
   return(TRUE);
}
SEMIPRIVATE int MI_return(void)
{ 
   ncopts = MI_CALLERS_NCOPTS; 
   return(TRUE); 
}
SEMIPRIVATE int MI_return_error(void)
{ 
   ncopts = MI_CALLERS_NCOPTS; 
   MI_LOG_PKG_ERROR2(ncerr, "MINC package entry point"); 
   return(TRUE);
}
SEMIPRIVATE void MI_log_pkg_error2(int p1, char *p2)
{
   ncerr = p1;
   if (ncopts & NC_VERBOSE) {
      (void) fprintf(stderr, "%s: ", MI_ROUTINE_VAR);
      (void) fprintf(stderr, p2);
      (void) fputc('\n', stderr);
      (void) fflush(stderr);
   }
   if ((ncopts & NC_FATAL) && (ncerr != NC_NOERR)) {
      exit(ncopts);
   }
   return;
}
SEMIPRIVATE void MI_log_pkg_error3(int p1, char *p2, char *p3)
{ 
   ncerr = p1;
   if (ncopts & NC_VERBOSE) {
      (void) fprintf(stderr, "%s: ", MI_ROUTINE_VAR);
      (void) fprintf(stderr, p2, p3);
      (void) fputc('\n', stderr);
      (void) fflush(stderr);
   }
   if ((ncopts & NC_FATAL) && (ncerr != NC_NOERR)) {
      exit(ncopts);
   }
   return;
}
SEMIPRIVATE void MI_log_sys_error1(char *p1)
{
   char *message;
   int errnum = errno;

   if (ncopts & NC_VERBOSE) {
      (void) fprintf(stderr, "%s", MI_ROUTINE_VAR);
      (void) fprintf(stderr, p1);
      if (errnum == 0) {
         ncerr = NC_NOERR;
         (void) fputc('\n', stderr);
      }
      else {
         ncerr = NC_SYSERR;
         message = strerror(errnum);
         if (message == NULL) message = "Unknown error";
         (void) fprintf(stderr, ": %s\n", message);
      }
      (void) fflush(stderr);
   }

   if (ncopts & NC_FATAL) {
      exit(ncopts);
   }

   return;
}

