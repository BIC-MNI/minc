#ifndef  DEF_FILES
#define  DEF_FILES

/* ----------------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

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

#ifndef lint
static char files_rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Include/volume_io/files.h,v 1.6 1995-07-31 13:44:33 david Exp $";
#endif

#include  <stdio.h>
#include  <basic.h>

typedef  enum  { ASCII_FORMAT, BINARY_FORMAT }          File_formats;

typedef  enum  { READ_FILE, WRITE_FILE, APPEND_FILE }   IO_types;

#endif
