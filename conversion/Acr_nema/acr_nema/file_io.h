/* ----------------------------- MNI Header -----------------------------------
@NAME       : file_io.h
@DESCRIPTION: Header file for doing io from acr_nema code.
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 9, 1993 (Peter Neelin)
@MODIFIED   : $Log: file_io.h,v $
@MODIFIED   : Revision 5.0  1997-08-21 13:25:00  neelin
@MODIFIED   : Release of minc version 0.5
@MODIFIED   :
 * Revision 4.0  1997/05/07  20:01:23  neelin
 * Release of minc version 0.4
 *
 * Revision 3.1  1997/04/21  20:21:09  neelin
 * Updated the library to handle dicom messages.
 *
 * Revision 3.0  1995/05/15  19:32:12  neelin
 * Release of minc version 0.3
 *
 * Revision 2.0  1994/09/28  10:36:14  neelin
 * Release of minc version 0.2
 *
 * Revision 1.4  94/09/28  10:35:43  neelin
 * Pre-release
 * 
 * Revision 1.3  94/04/08  10:32:10  neelin
 * Fixed io tracing.
 * 
 * Revision 1.2  93/11/25  10:37:43  neelin
 * Added file free and ungetc and trace.
 * 
 * Revision 1.1  93/11/10  10:33:33  neelin
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

#define ACR_NO_WATCHPOINT LONG_MAX

/* Define io routine prototype */

typedef int (*Acr_Io_Routine)
     (void *io_data, void *buffer, int nbytes);

/* Structure used for reading and writing in acr_nema routines */

typedef struct {
   void *io_data;
   Acr_Io_Routine io_routine;
   int maxlength;               /* Maximum length of read request */
   unsigned char *start;
   unsigned char *end;
   unsigned char *ptr;
   int length;                  /* Length of actual data in buffer */
   int buffer_length;           /* Length of allocated buffer */
   int stream_type;
   int reached_eof;             /* TRUE if we have reached end of file */
   int do_trace;                /* TRUE if file should be traced */
   FILE *tracefp;
   int watchpoint_set;          /* TRUE if a watchpoint is set */
   long bytes_to_watchpoint;    /* number of bytes from start of buffer
                                   to watchpoint */
   void *client_data;           /* Data that can be set by calling routines */
} Acr_File;

/* Macros for getting and putting a character */

#define acr_getc(afp)   ( ((afp)->ptr < (afp)->end) ? \
                         *((afp)->ptr++) : \
                         acr_file_read_more(afp) )

#define acr_putc(c,afp) ( ((afp)->ptr < (afp)->end) ? \
                         (int) (*((afp)->ptr++) = (unsigned char) (c)) : \
                         acr_file_write_more(afp, c) )

/* Function definitions */
public void acr_file_enable_trace(Acr_File *afp);
public void acr_file_disable_trace(Acr_File *afp);
public Acr_File *acr_file_initialize(void *io_data,
                                     int maxlength,
                                     Acr_Io_Routine io_routine);
public void acr_file_free(Acr_File *afp);
public void acr_file_reset(Acr_File *afp);
public void acr_file_set_eof(Acr_File *afp);
public void acr_file_set_client_data(Acr_File *afp, void *client_data);
public void *acr_file_get_client_data(Acr_File *afp);
public int acr_file_read_more(Acr_File *afp);
public int acr_file_write_more(Acr_File *afp, int character);
public int acr_file_flush(Acr_File *afp);
public int acr_ungetc(int c, Acr_File *afp);
public void *acr_file_get_io_data(Acr_File *afp);
public void acr_set_io_watchpoint(Acr_File *afp, long bytes_to_watchpoint);
public long acr_get_io_watchpoint(Acr_File *afp);
public int acr_stdio_read(void *io_data, void *buffer, int nbytes);
public int acr_stdio_write(void *io_data, void *buffer, int nbytes);
