/* ----------------------------- MNI Header -----------------------------------
@NAME       : scx_file.h
@DESCRIPTION: Header file for routines that read scanditonix image files
@GLOBALS    : 
@CREATED    : January 8, 1993 (Peter Neelin)
@MODIFIED   : 
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
