/* ----------------------------- MNI Header -----------------------------------
@NAME       : file_io.c
@DESCRIPTION: Routines for doing io from acr_nema code.
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 9, 1993 (Peter Neelin)
@MODIFIED   : $Log: file_io.c,v $
@MODIFIED   : Revision 1.5  1994-04-08 10:32:00  neelin
@MODIFIED   : Fixed io tracing.
@MODIFIED   :
 * Revision 1.4  93/11/25  10:36:14  neelin
 * Added file free and ungetc
 * 
 * Revision 1.3  93/11/23  16:02:12  neelin
 * Corrected header comment in acr_file_flush.
 * 
 * Revision 1.2  93/11/23  13:21:42  neelin
 * Added fflush for stdio write routine.
 * 
 * Revision 1.1  93/11/10  10:33:22  neelin
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
#include <minc_def.h>
#include <file_io.h>

/* Define some constants */
#define ACR_MAX_BUFFER_LENGTH (8*1024)
#ifndef TRUE
#  define TRUE 1
#endif
#ifndef FALSE
#  define FALSE 0
#endif

/* Stream type */
#define ACR_UNKNOWN_STREAM 0
#define ACR_READ_STREAM    1
#define ACR_WRITE_STREAM   2

/* Stuff for input and output tracing */
static char *Input_trace_file = "acr_file_input_XXXXXX";
static char *Output_trace_file = "acr_file_output_XXXXXX";
static int Do_input_trace = FALSE;
static int Do_output_trace = FALSE;

public void acr_enable_input_trace(void)
{
   Do_input_trace = TRUE;
   return;
}

public void acr_disable_input_trace(void)
{
   Do_input_trace = FALSE;
   return;
}

public void acr_enable_output_trace(void)
{
   Do_output_trace = TRUE;
   return;
}

public void acr_disable_output_trace(void)
{
   Do_output_trace = FALSE;
   return;
}

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
   afp->stream_type = ACR_UNKNOWN_STREAM;

   /* Allocate the buffer */
   afp->start = MALLOC((size_t) afp->maxlength);
   afp->length = afp->maxlength;

   /* Set ptr and end to start so that we can detect beginning of i/o */
   afp->end = afp->start;
   afp->ptr = afp->end;

   /* Set the trace file pointer to null */
   afp->tracefp = NULL;

   return afp;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_file_free
@INPUT      : afp
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Frees the Acr_File structure, flushing the buffer if needed.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 9, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_file_free(Acr_File *afp)
{
   if (afp != NULL) {
      if (afp->stream_type == ACR_WRITE_STREAM) {
         (void) acr_file_flush(afp);
      }
      if (afp->start != NULL) {
         FREE(afp->start);
      }
      if (afp->tracefp != NULL) {
         (void) fclose(afp->tracefp);
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
   char trace_file[128];

   /* Check the pointer */
   if (afp == NULL) {
      return EOF;
   }

   /* Check the stream type */
   switch (afp->stream_type) {
   case ACR_UNKNOWN_STREAM:
      afp->stream_type = ACR_READ_STREAM; break;
   case ACR_READ_STREAM:
      break;
   case ACR_WRITE_STREAM:
   default:
      return EOF;
   }

   /* Read in another buffer-full */
   nread=afp->io_routine(afp->user_data, afp->start, afp->maxlength);

   /* Do tracing */
   if (Do_input_trace) {
      if (afp->tracefp == NULL) {
         (void) strcpy(trace_file, Input_trace_file);
         (void) mktemp(trace_file);
         afp->tracefp = fopen(trace_file, "w");
         if (afp->tracefp != NULL) {
            (void) fprintf(stderr, "Opened input trace file %s.\n", 
                           trace_file);
            (void) fflush(stderr);
         }
         else {
            (void) fprintf(stderr, "Error opening input trace file %s.\n",
                           trace_file);
            (void) fflush(stderr);
         }
      }
      if (nread > 0) {
         (void) fwrite(afp->start, sizeof(char), nread, afp->tracefp);
         (void) fflush(afp->tracefp);
      }
   }

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
@RETURNS    : EOF if an error occurs, otherwise 0.
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
   char trace_file[128];

   /* Check the pointer */
   if (afp == NULL) {
      return EOF;
   }

   /* Check the stream type */
   switch (afp->stream_type) {
   case ACR_UNKNOWN_STREAM:
      afp->stream_type = ACR_WRITE_STREAM; break;
   case ACR_WRITE_STREAM:
      break;
   case ACR_READ_STREAM:
   default:
      return EOF;
   }

   /* Check for something to write */
   length = afp->ptr - afp->start;
   if (length > 0) {
      if (afp->io_routine(afp->user_data, afp->start, length) != length) {
         return EOF;
      }

      /* Do trace, if needed */
      if (Do_output_trace) {
         if (afp->tracefp == NULL) {
            (void) strcpy(trace_file, Output_trace_file);
            (void) mktemp(trace_file);
            afp->tracefp = fopen(trace_file, "w");
            if (afp->tracefp != NULL) {
               (void) fprintf(stderr, "Opened output trace file %s.\n", 
                              trace_file);
               (void) fflush(stderr);
            }
            else {
               (void) fprintf(stderr, "Error opening output trace file %s.\n",
                              trace_file);
               (void) fflush(stderr);
            }
         }
         (void) fwrite(afp->start, sizeof(char), length, afp->tracefp);
         (void) fflush(afp->tracefp);
      }

   }

   /* Reset the buffer */
   afp->ptr = afp->start;
   afp->length = afp->maxlength;
   afp->end = afp->start + afp->length;

   return 0;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_ungetc
@INPUT      : c - character to unget
              afp - Acr_File pointer
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Puts a character back into the input stream. This command is
              fragile - it will only work if it can back up on the current
              buffer.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 25, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int acr_ungetc(int c, Acr_File *afp)
{

   /* Check the pointer */
   if (afp == NULL) {
      return EOF;
   }

   /* Check to see if we can put the character back */
   if (afp->ptr > afp->start) {
      afp->ptr--;
      *afp->ptr = c;
   }
   else {
      return EOF;
   }

   return (int) *afp->ptr;

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

   /* Write the data */
   nwritten = fwrite(buffer, sizeof(char), (size_t) nbytes, fp);
   if (nwritten < 0) nwritten = 0;

   /* Flush the buffer */
   (void) fflush(fp);

   return nwritten;
}
