#ifndef  DEF_VOLUME_IO
#define  DEF_VOLUME_IO

#ifdef __cplusplus
extern "C" {
#endif

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
@VERSION    : $Header: /private-cvsroot/minc/volume_io/Include/volume_io.h,v 1.11.2.3 2005-04-03 18:02:54 stever Exp $
---------------------------------------------------------------------------- */

/* ----------------------------- MNI Header -----------------------------------
@NAME       : volume_io.h
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: A set of macros and definitions useful for all MNI programs.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : July 15, 1991       David MacDonald
@MODIFIED   :  
---------------------------------------------------------------------------- */

#ifndef VIO_PREFIX_NAMES
#define VIO_PREFIX_NAMES 0      /* Allow old-fashioned namespace pollution */
#endif /* VIO_PREFIX_NAMES */

#include  <volume_io/basic.h>
#include  <volume_io/string_funcs.h>
#include  <volume_io/files.h>
#include  <volume_io/arrays.h>
#include  <volume_io/geom_structs.h>
#include  <volume_io/progress.h>
#include  <volume_io/geometry.h>
#include  <volume_io/volume.h>
#include  <volume_io/transforms.h>

#ifndef  VIOAPI

#if defined(_MSC_VER)
#define       VIOAPI   __declspec(dllimport)
#else
#define       VIOAPI   extern
#endif /* _MSC_VER not defined */
#endif /* VIOAPI not defined */

#include  <volume_io/vol_io_prototypes.h>

#ifdef __cplusplus
}
#endif

#endif /* DEF_VOLUME_IO */
