/* ----------------------------- MNI Header -----------------------------------
@NAME       : open_connection.c
@DESCRIPTION: File containing routines to open a decnet connection.
@GLOBALS    : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : $Log: open_connection.c,v $
@MODIFIED   : Revision 1.2  1993-11-25 13:26:55  neelin
@MODIFIED   : Working version.
@MODIFIED   :
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
#include <bstring.h>
#include <dn/defs.h>
#include <acr_nema.h>
#include <minc_def.h>

/* Function prototypes */
int ioctl (int fildes, int request, ...);

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
   nread = read(io_data->fd, buffer, (unsigned int) nbytes);
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
   nwritten = write(io_data->fd, buffer, (unsigned int) nbytes);
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
public int open_connection(int argc, char *argv[], 
                           Acr_File **afpin, Acr_File **afpout)
{         /* ARGSUSED */
   SessionData sd;
   Io_data *io_data;
   int link;
   long maxlength;

   /* Accept the connection and get the maximum buffer length */
   link = fileno(stdin);
   bzero((void *) &sd, (int) sizeof(sd));
   (void) ioctl(link, SES_ACCEPT, &sd);
   (void) ioctl(link, SES_MAX_IO, &maxlength);

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

}

