
#ifndef  DEF_VOLUME_IO
#define  DEF_VOLUME_IO

#ifndef lint
static char volume_io_rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Include/volume_io.h,v 1.6 1994-11-25 14:19:51 david Exp $";
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
