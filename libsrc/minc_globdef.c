/* ----------------------------- MNI Header -----------------------------------
@NAME       : globdef.c
@DESCRIPTION: File to define global variables for the minc package.
@GLOBALS    : minc_routine_name
@CREATED    : July 29, 1992 (Peter Neelin)
@MODIFIED   : $Log: minc_globdef.c,v $
@MODIFIED   : Revision 6.0  1997-09-12 13:24:54  neelin
@MODIFIED   : Release of minc version 0.6
@MODIFIED   :
 * Revision 5.0  1997/08/21  13:25:53  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:07:52  neelin
 * Release of minc version 0.4
 *
 * Revision 3.0  1995/05/15  19:33:12  neelin
 * Release of minc version 0.3
 *
 * Revision 2.0  1994/09/28  10:38:05  neelin
 * Release of minc version 0.2
 *
 * Revision 1.5  94/09/28  10:37:17  neelin
 * Pre-release
 * 
 * Revision 1.4  93/08/11  12:06:26  neelin
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
static char rcsid[] = "$Header: /private-cvsroot/minc/libsrc/Attic/minc_globdef.c,v 6.0 1997-09-12 13:24:54 neelin Rel $ MINC (MNI)";
#endif

/* Variable for storing the name of the current routine */
char *minc_routine_name = "MINC";
int minc_call_depth;
int minc_callers_ncopts;
int minc_trash_var;         /* Just for getting rid of lint messages */

