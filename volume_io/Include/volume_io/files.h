#ifndef  DEF_FILES
#define  DEF_FILES

#ifndef lint
static char files_rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Include/volume_io/files.h,v 1.5 1994-11-25 14:19:40 david Exp $";
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : files.h
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Types for use with the general file io routines of the library.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#include  <stdio.h>
#include  <basic.h>

typedef  enum  { ASCII_FORMAT, BINARY_FORMAT }          File_formats;

typedef  enum  { READ_FILE, WRITE_FILE, APPEND_FILE }   IO_types;

#endif
