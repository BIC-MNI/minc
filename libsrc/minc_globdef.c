/* ----------------------------- MNI Header -----------------------------------
@NAME       : globdef.c
@DESCRIPTION: File to define global variables for the minc package.
@GLOBALS    : minc_routine_name
@CREATED    : July 29, 1992 (Peter Neelin)
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
static char rcsid[] = "$Header: /private-cvsroot/minc/libsrc/Attic/minc_globdef.c,v 1.3 1993-07-20 12:17:44 neelin Exp $ MINC (MNI)";
#endif

/* Variable for storing the name of the current routine */
char *minc_routine_name = "MINC";
int minc_call_depth;
int minc_callers_ncopts;
int minc_trash_var;         /* Just for getting rid of lint messages */

