/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_nema.h
@DESCRIPTION: Header file for ACR-NEMA code.
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
 * $Log: acr_nema.h,v $
 * Revision 6.3  2005/02/16 19:22:32  bert
 * Autoconfiscation
 *
 * Revision 6.2  2004/10/29 13:08:41  rotor
 *  * rewrote Makefile with no dependency on a minc distribution
 *  * removed all references to the abominable minc_def.h
 *  * I should autoconf this really, but this is old code that
 *      is now replaced by Jon Harlaps PERL version..
 *
 * Revision 6.1  1999/10/29 17:51:50  neelin
 * Fixed Log keyword
 *
 * Revision 6.0  1997/09/12 13:23:59  neelin
 * Release of minc version 0.6
 *
 * Revision 5.1  1997/09/08  21:53:14  neelin
 * Added dicom_client_routines.
 *
 * Revision 5.0  1997/08/21  13:25:00  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:01:23  neelin
 * Release of minc version 0.4
 *
 * Revision 3.1  1997/04/21  20:21:09  neelin
 * Updated the library to handle dicom messages.
 *
 * Revision 3.0  1995/05/15  19:32:12  neelin
 * Release of minc version 0.3
 *
 * Revision 2.0  1994/09/28  10:36:08  neelin
 * Release of minc version 0.2
 *
 * Revision 1.6  94/09/28  10:35:51  neelin
 * Pre-release
 * 
 * Revision 1.5  94/09/23  16:42:37  neelin
 * Changed acr_nema_io to acr_io and acr_nema_test to acr_test.
 * 
 * Revision 1.4  94/01/06  13:31:27  neelin
 * Changed acr_need_invert to a public function.
 * 
 * Revision 1.3  93/12/08  09:07:02  neelin
 * 
 * Revision 1.2  93/11/24  11:26:09  neelin
 * Added TRUE and FALSE.
 * 
 * Revision 1.1  93/11/19  12:49:34  neelin
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

/* Define constants */
#ifndef public
#  define public
#endif
#ifndef private
#  define private static
#endif

#ifndef TRUE
#  define TRUE 1
#endif
#ifndef FALSE
#  define FALSE 0
#endif

/* Include files */
#include <acr_nema/file_io.h>
#include <acr_nema/acr_io.h>
#include <acr_nema/value_repr.h>
#include <acr_nema/element.h>
#include <acr_nema/group.h>
#include <acr_nema/message.h>
#include <acr_nema/dicom_network.h>
#include <acr_nema/dicom_client_routines.h>

/* these are pinched from minc_def.h */
#define MALLOC(size) ((void *) malloc(size))
#define FREE(ptr) free(ptr)
#define REALLOC(ptr, size) ((void *) realloc(ptr, size))
#define CALLOC(nelem, elsize) ((void *) calloc(nelem, elsize))
