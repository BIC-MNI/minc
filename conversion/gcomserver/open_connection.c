/* ----------------------------- MNI Header -----------------------------------
@NAME       : open_connection.c
@DESCRIPTION: File containing routines to open a decnet connection.
@GLOBALS    : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : 
 * $Log: open_connection.c,v $
 * Revision 6.3  2001-02-20 00:55:38  neelin
 * Fixes for port to linux decnet.
 *
 * Revision 6.1  1999/10/29 17:52:04  neelin
 * Fixed Log keyword
 *
 * Revision 6.0  1997/09/12 13:23:50  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:24:50  neelin
 * Release of minc version 0.5
 *
 * Revision 4.1  1997/06/13  22:08:13  neelin
 * Modifications to get gcomserver working with modified Acr_nema library.
 *
 * Revision 4.0  1997/05/07  20:01:07  neelin
 * Release of minc version 0.4
 *
 * Revision 3.0  1995/05/15  19:31:44  neelin
 * Release of minc version 0.3
 *
 * Revision 2.5  1995/02/14  18:12:26  neelin
 * Added project names and defaults files (using volume name).
 * Added process id to log file name.
 * Moved temporary files to subdirectory.
 *
 * Revision 2.4  1995/02/09  13:51:26  neelin
 * Mods for irix 5 lint.
 *
 * Revision 2.3  1995/02/08  19:31:47  neelin
 * Moved ARGSUSED statements for irix 5 lint.
 *
 * Revision 2.2  1994/12/07  09:45:59  neelin
 * Fixed called to ioctl to get rid of type mismatch warning messages.
 *
 * Revision 2.1  94/12/07  08:20:10  neelin
 * Added support for irix 5 decnet.
 * 
 * Revision 2.0  94/09/28  10:35:32  neelin
 * Release of minc version 0.2
 * 
 * Revision 1.5  94/09/28  10:34:50  neelin
 * Pre-release
 * 
 * Revision 1.4  94/01/18  14:23:41  neelin
 * Changed bzero to memset.
 * 
 * Revision 1.3  93/11/30  14:42:13  neelin
 * Copies to minc format.
 * 
 * Revision 1.2  93/11/25  13:26:55  neelin
 * Working version.
 * 
 * Revision 1.1  93/11/23  14:11:54  neelin
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
#include <memory.h>
#ifdef sgi
#  include <dn/defs.h>
#  define USE_4DDN
#endif
#include <acr_nema.h>
#include <minc_def.h>

/* Decnet functions. If DNIOCTL is defined, then we have irix 5.
   Otherwise we have linux or irix4. We do not need the ioctl stuff
   for linux */
#ifdef DNIOCTL
#  define GCOM_IOCTL DNIOCTL
#  define GCOM_READ DNREAD
#  define GCOM_WRITE DNWRITE
#else
#  define GCOM_READ read
#  define GCOM_WRITE write
#  ifdef USE_4DDN
#     define GCOM_IOCTL ioctl
int ioctl (int fildes, int request, ...);
#  endif
#endif

/* Make sure that maximum write length is defined */
#ifndef DN_MAX_IO
#  define DN_MAX_IO 1460
#endif

/* Structure for passing info around */
typedef struct {
   int fd;
} Io_data;

/* ----------------------------- MNI Header -----------------------------------
@NAME       : input_routine
@INPUT      : user_data - should be a FILE * pointer
              nbytes - number of bytes to read
@OUTPUT     : buffer - buffer into which we will read
@RETURNS    : Number of bytes read.
@DESCRIPTION: Acr io routine for reading from a decnet connection
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int input_routine(void *user_data, void *buffer, int nbytes)
{
   Io_data *io_data;
   int nread;

   /* Get data pointer */
   if (user_data == NULL) return 0;
   io_data = (Io_data *) user_data;

   /* Read the data */
   nread = GCOM_READ(io_data->fd, buffer, (unsigned int) nbytes);
   if (nread < 0) nread = 0;

   return nread;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : output_routine
@INPUT      : user_data - should be a FILE * pointer
              buffer - buffer into which we will read
              nbytes - number of bytes to read
@OUTPUT     : (nothin)
@RETURNS    : Number of bytes written.
@DESCRIPTION: Acr io routine for writing to a decnet connection
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int output_routine(void *user_data, void *buffer, int nbytes)
{
   Io_data *io_data;
   int nwritten;

   /* Get data pointer */
   if (user_data == NULL) return 0;
   io_data = (Io_data *) user_data;

   /* Write the data */
   nwritten = GCOM_WRITE(io_data->fd, buffer, (unsigned int) nbytes);
   if (nwritten < 0) nwritten = 0;

   return nwritten;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : open_connection
@INPUT      : argc - number of command-line arguments
              argv - array of command-line arguments
@OUTPUT     : afpin - Acr file pointer for input
              outpin - Acr file pointer for output
@RETURNS    : FALSE on success, TRUE on failure
@DESCRIPTION: Opens the connection for reading writing spi messages.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
/* ARGSUSED */
public void open_connection(int argc, char *argv[], 
                            Acr_File **afpin, Acr_File **afpout)
{

#ifdef USE_4DDN
   SessionData sd;
   int link;
#endif
   Io_data *io_data;
   long maxlength;

   /* If there are no arguments, then assume that we are just using 
      stdin and stdout */
   if (argc == 1) {
      *afpin = acr_file_initialize((void *) stdin, 0, acr_stdio_read);
      *afpout = acr_file_initialize((void *) stdout, 0, acr_stdio_write);
   }

   /* Otherwise, use decnet routines */
   else {

#ifdef USE_4DDN
      /* Accept the connection and get the maximum buffer length */
      link = fileno(stdin);
      (void) memset((void *) &sd, 0, sizeof(sd));
      (void) GCOM_IOCTL(link, SES_ACCEPT, (char *) &sd);
      maxlength = 0;
      if ((GCOM_IOCTL(link, SES_MAX_IO, (char *) &maxlength) == -1) ||
          (maxlength <= 0)) {
         maxlength = DN_MAX_IO;
      }
#else
      maxlength = DN_MAX_IO;
#endif   /* USE_4DDN */

      /* Set up input */
      io_data = MALLOC(sizeof(*io_data));
      io_data->fd = fileno(stdin);
      *afpin = acr_file_initialize((void *) io_data, (int) maxlength, 
                                   input_routine);

      /* Set up output */
      io_data = MALLOC(sizeof(*io_data));
      io_data->fd = fileno(stdout);
      *afpout = acr_file_initialize((void *) io_data, (int) maxlength, 
                                    output_routine);

   }        /* If decnet else */

}

