/* ----------------------------- MNI Header -----------------------------------
@NAME       : file_io.h
@DESCRIPTION: Header file for doing io from acr_nema code.
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 9, 1993 (Peter Neelin)
@MODIFIED   : $Log: file_io.h,v $
@MODIFIED   : Revision 1.1  1993-11-10 10:33:33  neelin
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

/* Define constants */

#ifndef public
#  define public
#endif

/* Define io routine prototype */

typedef int (*Acr_Io_Routine)
     (void *user_data, void *buffer, int nbytes);

/* Structure used for reading and writing in acr_nema routines */

typedef struct {
   void *user_data;
   Acr_Io_Routine io_routine;
   int maxlength;
   unsigned char *start;
   unsigned char *end;
   unsigned char *ptr;
   int length;
} Acr_File;

/* Macros for getting and putting a character */

#define acr_getc(afp)   ( ((afp)->ptr < (afp)->end) ? \
                         *((afp)->ptr++) : \
                         acr_file_read_more(afp) )

#define acr_putc(c,afp) ( ((afp)->ptr < (afp)->end) ? \
                         (int) (*((afp)->ptr++) = (unsigned char) (c)) : \
                         acr_file_write_more(afp, c) )

/* Function definitions */

public Acr_File *acr_file_initialize(void *user_data,
                                     int maxlength,
                                     Acr_Io_Routine io_routine);
public void acr_file_free(Acr_File *afp);
public int acr_file_read_more(Acr_File *afp);
public int acr_file_write_more(Acr_File *afp, int character);
public int acr_file_flush(Acr_File *afp);
public int acr_stdio_read(void *user_data, void *buffer, int nbytes);
public int acr_stdio_write(void *user_data, void *buffer, int nbytes);

                       
