
#ifndef  DEF_VOLUME_IO
#define  DEF_VOLUME_IO

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

#ifndef lint
static char volume_io_rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Include/volume_io.h,v 1.7 1995-07-31 13:44:37 david Exp $";
#endif

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

#include  <basic.h>
#include  <string_funcs.h>
#include  <files.h>
#include  <arrays.h>
#include  <geom_structs.h>
#include  <progress.h>
#include  <geometry.h>
#include  <volume.h>
#include  <transforms.h>

#ifndef  public
#define       public   extern
#define       public_was_defined_here
#endif

#include  <vol_io_prototypes.h>

#ifdef  public_was_defined_here
#undef       public
#undef       public_was_defined_here
#endif

#endif
