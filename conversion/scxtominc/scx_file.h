/* ----------------------------- MNI Header -----------------------------------
@NAME       : scx_file.h
@DESCRIPTION: Header file for routines that read scanditonix image files
@GLOBALS    : 
@CREATED    : January 8, 1993 (Peter Neelin)
@MODIFIED   : $Log: scx_file.h,v $
@MODIFIED   : Revision 3.0  1995-05-15 19:31:05  neelin
@MODIFIED   : Release of minc version 0.3
@MODIFIED   :
 * Revision 2.0  1994/09/28  10:33:48  neelin
 * Release of minc version 0.2
 *
 * Revision 1.4  94/09/28  10:33:33  neelin
 * Pre-release
 * 
 * Revision 1.3  93/08/11  15:27:45  neelin
 * Added RCS logging in source.
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

#ifndef public
#define public
#endif
#ifndef private
#define private static
#endif

#ifndef SCX_FILE_HEADER_DEFINITION_H
typedef struct {int dummy;}  scx_block_type;
typedef struct {int dummy;}  scx_mnemonic_type;
typedef enum scx_mnem_types_enum {
   scx_time, scx_byte, scx_string, scx_short_float, scx_date, scx_float, 
   scx_word, scx_long
} scx_mnem_types;
#endif

typedef struct {
   FILE *file_pointer;
   scx_block_type *blocks;
   scx_mnemonic_type *mnemonics;
   int num_mnemonics;
   void *header;
} scx_file;

/* Routine declarations */
public scx_file *scx_open(char *filename);
public void scx_close(scx_file *file);
public int scx_get_mnem(scx_file *file, char *mnem, int multiplicity,
                        long *lvalue, float *fvalue, char *svalue);
public int scx_get_image(scx_file *file, int image_num, short *image);
public char *scx_list_mnems(scx_file *file,int index, char *mname, 
                            int *mmult, scx_mnem_types *mtype);
