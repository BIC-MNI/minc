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
@VERSION    : $Header: /private-cvsroot/minc/volume_io/Include/volume_io/files.h,v 1.9 2004-10-04 20:23:51 bert Exp $
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

#include  <stdio.h>
#include  <volume_io/basic.h>

typedef  enum  { ASCII_FORMAT, BINARY_FORMAT }          VIO_File_formats;

typedef  enum  { READ_FILE, WRITE_FILE, APPEND_FILE }   VIO_IO_types;

#ifndef MINC_PLAY_NICE
typedef VIO_File_formats File_formats;
typedef VIO_IO_types IO_types;
#endif /* MINC_PLAY_NICE */

#endif /* DEF_FILES */
