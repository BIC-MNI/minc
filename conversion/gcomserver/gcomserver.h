/* ----------------------------- MNI Header -----------------------------------
@NAME       : gcomserver.h
@DESCRIPTION: Header file that includes things needed for gcomserver.
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 23, 1993 (Peter Neelin)
@MODIFIED   : $Log: gcomserver.h,v $
@MODIFIED   : Revision 1.2  1993-11-25 13:27:25  neelin
@MODIFIED   : Working version.
@MODIFIED   :
 * Revision 1.1  93/11/23  14:13:43  neelin
 * Initial revision
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <minc_def.h>
#include <acr_nema.h>
#include <spi_element_defs.h>
#include <gcom_prototypes.h>

#ifndef TRUE
#  define TRUE 1
#endif
#ifndef FALSE
#  define FALSE 0
#endif

/* Define logging constants */
#define NO_LOGGING   0
#define LOW_LOGGING  1
#define HIGH_LOGGING 2

