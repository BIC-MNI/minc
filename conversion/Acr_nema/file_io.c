/* ----------------------------- MNI Header -----------------------------------
@NAME       : file_io.c
@DESCRIPTION: Routines for doing io from acr_nema code.
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 9, 1993 (Peter Neelin)
@MODIFIED   : $Log: file_io.c,v $
@MODIFIED   : Revision 1.1  1993-11-10 10:33:22  neelin
@MODIFIED   : Initial revision
@MODIFIED   :
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
#include <minc_def.h>
#include <file_io.h>

/* Define some constants */
#define ACR_MAX_BUFFER_LENGTH (8*1024)

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_file_initialize
@INPUT      : user_data - pointer to data for read and write routines
              maxlength - maximum length for a single read or write
                 (zero or negative means use internal maximum).
              io_routine - routine to read or write data
@OUTPUT     : (none)
@RETURNS    : pointer to Acr_File structure created
@DESCRIPTION: Sets up the routines for reading/writing from an input
              or output stream. A given Acr_File can only do input or
              output, not both.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 9, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_File *acr_file_initialize(void *user_data,
                                     int maxlength,
                                     Acr_Io_Routine io_routine)
{
   Acr_File *afp;

   /* Check that an io routine is given */
   if (io_routine == NULL) {
      return NULL;
   }

   /* Allocate the strcture */
   afp = MALLOC(sizeof(*afp));

   /* Initialize fields */
   afp->user_data = user_data;
   afp->io_routine = io_routine;
   if ((maxlength < ACR_MAX_BUFFER_LENGTH) && (maxlength > 0))
      afp->maxlength = maxlength;
   else
      afp->maxlength = ACR_MAX_BUFFER_LENGTH;

   /* Allocate the buffer */
   afp->start = MALLOC((size_t) afp->maxlength);
   afp->length = afp->maxlength;

   /* Set ptr and end to start so that we can detect beginning of i/o */
   afp->end = afp->start;
   afp->ptr = afp->end;

   return afp;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_file_free
@INPUT      : afp - Acr_File pointer to free
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Frees the Acr_File structure
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 9, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_file_free(Acr_File *afp)
{
   if (afp != NULL) {
      if (afp->start != NULL) {
         FREE(afp->start);
      }
      FREE(afp);
   }
   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_file_read_more
@INPUT      : afp - Acr_File pointer
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Gets more input.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 9, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int acr_file_read_more(Acr_File *afp)
{
   int nread;

   /* Check the pointer */
   if (afp == NULL) {
      return EOF;
   }

   /* Read in another buffer-full */
   nread=afp->io_routine(afp->user_data, afp->start, afp->maxlength);

   /* Check for EOF */
   if (nread <= 0) {
      afp->end = afp->start + afp->maxlength;
      afp->ptr = afp->end;
      return EOF;
   }

   /* Set up variables */
   afp->ptr = afp->start;
   afp->length = nread;
   afp->end = afp->start + afp->length;

   return (int) *(afp->ptr++);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_file_write_more
@INPUT      : afp - Acr_File pointer
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Writes out the buffer and the character.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 9, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int acr_file_write_more(Acr_File *afp, int character)
{

   /* Check the pointer */
   if (afp == NULL) {
      return EOF;
   }

   /* Flush the buffer */
   if (acr_file_flush(afp) == EOF) {
      return EOF;
   }

   /* Write the character into the buffer */
   return (int) (*(afp->ptr++) = (unsigned char) character);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_file_flush
@INPUT      : afp - Acr_File pointer
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Flushes the buffer.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 9, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int acr_file_flush(Acr_File *afp)
{
   int length;

   /* Check the pointer */
   if (afp == NULL) {
      return EOF;
   }

   /* Check for something to write */
   length = afp->ptr - afp->start;
   if (length > 0) {
      if (afp->io_routine(afp->user_data, afp->start, length) != length) {
         return EOF;
      }
   }

   /* Reset the buffer */
   afp->ptr = afp->start;
   afp->length = afp->maxlength;
   afp->end = afp->start + afp->length;

   return 0;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_stdio_read
@INPUT      : user_data - should be a FILE * pointer
              nbytes - number of bytes to read
@OUTPUT     : buffer - buffer into which we will read
@RETURNS    : Number of bytes read.
@DESCRIPTION: Acr io routine for reading from a stdio FILE pointer
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 9, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int acr_stdio_read(void *user_data, void *buffer, int nbytes)
{
   FILE *fp;
   int nread;

   /* Get file pointer */
   if (user_data == NULL) return 0;
   fp = (FILE *) user_data;

   /* Read the data */
   nread = fread(buffer, sizeof(char), (size_t) nbytes, fp);
   if (nread < 0) nread = 0;

   return nread;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_stdio_write
@INPUT      : user_data - should be a FILE * pointer
              buffer - buffer from which we will write
              nbytes - number of bytes to write
@OUTPUT     : (nothing)
@RETURNS    : Number of bytes written.
@DESCRIPTION: Acr io routine for writing to a stdio FILE pointer
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 9, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int acr_stdio_write(void *user_data, void *buffer, int nbytes)
{
   FILE *fp;
   int nwritten;

   /* Get file pointer */
   if (user_data == NULL) return 0;
   fp = (FILE *) user_data;

   /* Read the data */
   nwritten = fwrite(buffer, sizeof(char), (size_t) nbytes, fp);
   if (nwritten < 0) nwritten = 0;

   return nwritten;
}
