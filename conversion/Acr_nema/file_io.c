/* ----------------------------- MNI Header -----------------------------------
@NAME       : file_io.c
@DESCRIPTION: Routines for doing io from acr_nema code.
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 9, 1993 (Peter Neelin)
@MODIFIED   : 
 * $Log: file_io.c,v $
 * Revision 6.2  1999-10-29 17:51:53  neelin
 * Fixed Log keyword
 *
 * Revision 6.1  1998/11/11 16:26:49  neelin
 * Increased default buffer size to 64K.
 *
 * Revision 6.0  1997/09/12  13:23:59  neelin
 * Release of minc version 0.6
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
 * Revision 2.1  1995/02/08  21:16:06  neelin
 * Changes to make irix 5 lint happy.
 *
 * Revision 2.0  1994/09/28  10:36:13  neelin
 * Release of minc version 0.2
 *
 * Revision 1.7  94/09/28  10:35:42  neelin
 * Pre-release
 * 
 * Revision 1.6  94/04/08  10:34:47  neelin
 * Added include of string.h
 * 
 * Revision 1.5  94/04/08  10:32:00  neelin
 * Fixed io tracing.
 * 
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
#include <string.h>
#include <limits.h>
#include <minc_def.h>
#include <file_io.h>

/* Define some constants */
#define ACR_MAX_BUFFER_LENGTH (64*1024)
#define ACR_BUFFER_MARGIN 64
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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_file_enable_trace
@INPUT      : afp
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Turns on tracing for an i/o stream
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 18, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_file_enable_trace(Acr_File *afp)
{
   afp->do_trace = TRUE;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_file_disable_trace
@INPUT      : afp
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Turns off tracing for an i/o stream
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 18, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_file_disable_trace(Acr_File *afp)
{
   afp->do_trace = FALSE;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_file_initialize
@INPUT      : io_data - pointer to data for read and write routines
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
@MODIFIED   : February 5, 1997 (P.N.)
---------------------------------------------------------------------------- */
public Acr_File *acr_file_initialize(void *io_data,
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
   afp->io_data = io_data;
   afp->io_routine = io_routine;
   if ((maxlength < ACR_MAX_BUFFER_LENGTH) && (maxlength > 0))
      afp->maxlength = maxlength;
   else
      afp->maxlength = ACR_MAX_BUFFER_LENGTH;
   afp->stream_type = ACR_UNKNOWN_STREAM;
   afp->buffer_length = ACR_MAX_BUFFER_LENGTH+ACR_BUFFER_MARGIN;
   afp->reached_eof = FALSE;
   afp->watchpoint_set = FALSE;
   afp->bytes_to_watchpoint = 0;
   afp->client_data = NULL;

   /* Allocate the buffer */
   afp->start = MALLOC((size_t) afp->buffer_length);

   /* Set ptr and end to start so that we can detect beginning of i/o */
   afp->length = 0;
   afp->end = afp->start;
   afp->ptr = afp->end;

   /* Set the trace file pointer to null */
   afp->do_trace = FALSE;
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
      if (afp->client_data != NULL) {
         FREE(afp->client_data);
      }
      FREE(afp);
   }
   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_file_reset
@INPUT      : afp
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Resets the input or output stream, discarding anything that
              was buffered. Any watchpoint is unset. The eof flag is unset.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 18, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_file_reset(Acr_File *afp)
{
   afp->watchpoint_set = FALSE;
   afp->bytes_to_watchpoint = 0;
   afp->length = 0;
   afp->end = afp->start;
   afp->ptr = afp->end;
   afp->reached_eof = FALSE;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_file_set_eof
@INPUT      : afp
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Tells the input or output stream that it has reached the end
              of file so that no more data will be read or written. This can
              be useful for preventing further reading or writing if an
              alarm goes off, for example.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : March 10, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_file_set_eof(Acr_File *afp)
{
   afp->reached_eof = TRUE;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_file_set_client_data
@INPUT      : afp
              client_data - pointer to data for client use
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Associates some client data with an io stream. The pointer is
              assumed to point to data that should be freed when the stream
              is closed. Note that this mechanism is designed only to be
              used by the acr_io routines and is not a general-purpose
              mechanism.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 14, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_file_set_client_data(Acr_File *afp, void *client_data)
{
   if (afp->client_data != NULL) {
      FREE(afp->client_data);
   }
   afp->client_data = client_data;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_file_get_client_data
@INPUT      : afp
@OUTPUT     : (none)
@RETURNS    : pointer to client data
@DESCRIPTION: Gets the pointer to client data for the given input stream.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 14, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void *acr_file_get_client_data(Acr_File *afp)
{
   return afp->client_data;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_file_read_more
@INPUT      : afp - Acr_File pointer
@OUTPUT     : (none)
@RETURNS    : The next character.
@DESCRIPTION: Gets more input. If the watchpoint is reached, then EOF is 
              returned. This routine will not read past the watchpoint.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 9, 1993 (Peter Neelin)
@MODIFIED   : February 5, 1997 (P.N.)
---------------------------------------------------------------------------- */
public int acr_file_read_more(Acr_File *afp)
{
   int nread, ichar;
   char trace_file[128];
   unsigned char *margin_ptr;
   long byte_shift, bytes_to_read, watchpoint_distance;

   /* Check the pointer */
   if (afp == NULL) {
      return EOF;
   }

   /* Check whether we really need more */
   if (afp->ptr < afp->end) {
      return (int) *(afp->ptr++);
   }

   /* Check for EOF */
   if (afp->reached_eof) return EOF;

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

   /* Work out the amount to read */
   bytes_to_read = afp->maxlength;
   if (afp->watchpoint_set) {
      watchpoint_distance = (afp->start + afp->bytes_to_watchpoint - afp->end);
      if (watchpoint_distance <= 0) {
         return EOF;
      }
      else if (watchpoint_distance < bytes_to_read) {
         bytes_to_read = watchpoint_distance;
      }
   }

   /* Check whether we need to start a new buffer for next read */
   if ((afp->end + bytes_to_read) > (afp->start + afp->buffer_length)) {

      /* Check that things are consistent */
      if ((afp->end - ACR_BUFFER_MARGIN) <= afp->start) {
         (void) fprintf(stderr, "Internal error copying afp buffer margin\n");
         exit(EXIT_FAILURE);
      }

      /* Copy the data down from the top of the buffer */
      margin_ptr = afp->end - ACR_BUFFER_MARGIN;
      for (ichar=0; ichar < ACR_BUFFER_MARGIN; ichar++) {
         afp->start[ichar] = margin_ptr[ichar];
      }

      /* Update the structure */
      byte_shift = margin_ptr - afp->start;
      afp->end = afp->start + ACR_BUFFER_MARGIN;
      afp->ptr = afp->end;
      afp->length = afp->end - afp->start;
      if (afp->watchpoint_set) {
         afp->bytes_to_watchpoint -= byte_shift;
      }
   }


   /* Read in another buffer-full */
   nread=afp->io_routine(afp->io_data, afp->end, bytes_to_read);

   /* Do tracing */
   if (afp->do_trace) {
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
         (void) fwrite(afp->end, sizeof(char), nread, afp->tracefp);
         (void) fflush(afp->tracefp);
      }
   }

   /* Check for EOF */
   if (nread <= 0) {
      afp->reached_eof = TRUE;
      return EOF;
   }

   /* Set up variables */
   afp->end += nread;
   afp->length += nread;

   /* Return the next value */
   return (int) *(afp->ptr++);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_file_write_more
@INPUT      : afp - Acr_File pointer
@OUTPUT     : (none)
@RETURNS    : The character or EOF.
@DESCRIPTION: Writes out the buffer and the character. This routine will
              write past the watchpoint, but will flush the buffer when it
              reaches the watchpoint.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 9, 1993 (Peter Neelin)
@MODIFIED   : February 5, 1997 (P.N.)
---------------------------------------------------------------------------- */
public int acr_file_write_more(Acr_File *afp, int character)
{

   /* Check the pointer */
   if (afp == NULL) {
      return EOF;
   }

   /* Check whether we need to flush the buffer */
   if (afp->ptr < afp->end) {
      return (int) (*(afp->ptr++) = (unsigned char) character);
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
@MODIFIED   : February 5, 1997 (P.N.)
---------------------------------------------------------------------------- */
public int acr_file_flush(Acr_File *afp)
{
   int length, nwritten;
   char trace_file[128];

   /* Check the pointer */
   if (afp == NULL) {
      return EOF;
   }

   /* Check for EOF */
   if (afp->reached_eof) return EOF;

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

      /* Do trace, if needed */
      if (afp->do_trace) {
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

      /* Write the data */
      nwritten = afp->io_routine(afp->io_data, afp->start, length);
      if (nwritten != length) {
         (void) fprintf(stderr, "Output error: wrote only %d bytes of %d\n",
                        nwritten, length);
         afp->reached_eof = TRUE;
         return EOF;
      }

   }

   /* Reset the buffer */
   if (afp->watchpoint_set) {
      afp->bytes_to_watchpoint -= length;
   }
   afp->ptr = afp->start;
   afp->length = afp->maxlength;
   if ((afp->bytes_to_watchpoint > 0) && 
       (afp->bytes_to_watchpoint < afp->length)) {
      afp->length = afp->bytes_to_watchpoint;
   }
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
@MODIFIED   : February 5, 1997 (P.N.)
---------------------------------------------------------------------------- */
public int acr_ungetc(int c, Acr_File *afp)
{

   /* Check the pointer */
   if (afp == NULL) {
      return EOF;
   }

   /* Check the stream type */
   if (afp->stream_type != ACR_READ_STREAM) {
      return EOF;
   }

   /* Check to see if we can put the character back */
   if (afp->ptr > afp->start) {
      afp->ptr--;
      *afp->ptr = (unsigned char) c;
   }
   else {
      return EOF;
   }

   return (int) *afp->ptr;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_file_get_io_data
@INPUT      : afp - Acr_File pointer
@OUTPUT     : (none)
@RETURNS    : pointer to io data
@DESCRIPTION: Gets back pointer that was passed in to acr_file_initialize.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 17, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void *acr_file_get_io_data(Acr_File *afp)
{
   return afp->io_data;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_set_io_watchpoint
@INPUT      : afp - Acr_File pointer
              bytes_to_watchpoint - number of bytes from current position
                 to watchpoint
@OUTPUT     : (none)
@RETURNS    : nothing
@DESCRIPTION: Sets a watchpoint in the i/o stream relative to the current
              input position. If bytes_to_watchpoint is equal to 
              ACR_NO_WATCHPOINT, then the watchpoint is unset.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 5, 1997 (P.N.)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_set_io_watchpoint(Acr_File *afp, long bytes_to_watchpoint)
{
   if (afp == NULL) return;

   /* Unset watchpoint */
   if (bytes_to_watchpoint == ACR_NO_WATCHPOINT) {
      afp->watchpoint_set = FALSE;
      afp->bytes_to_watchpoint = ACR_NO_WATCHPOINT;
   }

   /* Set watchpoint */
   else {
      afp->watchpoint_set = TRUE;
      afp->bytes_to_watchpoint = bytes_to_watchpoint + afp->ptr - afp->start;

      /* For writing, check if we need to move the end of the buffer to
         force a flush at the watchpoint */
      if ((afp->stream_type == ACR_WRITE_STREAM) &&
          (bytes_to_watchpoint >= 0) &&
          (afp->bytes_to_watchpoint < afp->length)) {
         afp->length = afp->bytes_to_watchpoint;
         afp->end = afp->start + afp->length;
      }
   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_io_watchpoint
@INPUT      : afp - Acr_File pointer
@OUTPUT     : (none)
@RETURNS    : number of bytes from current position to watchpoint
@DESCRIPTION: Checks the watchpoint and returns the number of bytes between
              the current position and the watchpoint. If the current position
              is past the watchpoint, then a negative number is returned. If 
              the watchpoint is not set, then ACR_NO_WATCHPOINT is returned.
              This constant is a very large positive number (ie. the 
              watchpoint is always in the future).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 5, 1997 (P.N.)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public long acr_get_io_watchpoint(Acr_File *afp)
{
   if ((afp == NULL) || !afp->watchpoint_set) return ACR_NO_WATCHPOINT;

   return (afp->start + afp->bytes_to_watchpoint - afp->ptr);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_stdio_read
@INPUT      : io_data - should be a FILE * pointer
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
public int acr_stdio_read(void *io_data, void *buffer, int nbytes)
{
   FILE *fp;
   int nread;

   /* Get file pointer */
   if (io_data == NULL) return 0;
   fp = (FILE *) io_data;

   /* Read the data */
   nread = fread(buffer, sizeof(char), (size_t) nbytes, fp);
   if (nread < 0) nread = 0;

   return nread;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_stdio_write
@INPUT      : io_data - should be a FILE * pointer
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
public int acr_stdio_write(void *io_data, void *buffer, int nbytes)
{
   FILE *fp;
   int nwritten;

   /* Get file pointer */
   if (io_data == NULL) return 0;
   fp = (FILE *) io_data;

   /* Write the data */
   nwritten = fwrite(buffer, sizeof(char), (size_t) nbytes, fp);
   if (nwritten < 0) nwritten = 0;

   /* Flush the buffer */
   if (nwritten > 0) {
      (void) fflush(fp);
   }

   return nwritten;
}

