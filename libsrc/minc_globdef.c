/* ----------------------------- MNI Header -----------------------------------
@NAME       : globdef.c
@DESCRIPTION: File to define global variables for the minc package.
@GLOBALS    : minc_routine_name
@CREATED    : July 29, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/minc/libsrc/Attic/minc_globdef.c,v 1.2 1992-12-01 14:03:57 neelin Exp $ MINC (MNI)";
#endif

/* Variable for storing the name of the current routine */
char *minc_routine_name = "MINC";
int minc_call_depth;
int minc_callers_ncopts;
int minc_trash_var;         /* Just for getting rid of lint messages */

