#ifndef  DEF_IO
#define  DEF_IO

#include  <stdio.h>
#include  <def_mni.h>

typedef  enum  { ASCII_FORMAT, BINARY_FORMAT }          File_formats;

typedef  enum  { READ_FILE, WRITE_FILE, APPEND_FILE }   IO_types;

/* ---------------- PRINT -------------------------------------- */

#define  PRINT  (void) printf

/* Error handling */

#define  PRINT_ERROR  (void) printf

Boolean  file_exists( char [] );
void     remove_file( char [] );
Status   open_file( char [], IO_types, File_formats, FILE ** );
Status   open_file_with_default_suffix( char [], char[], IO_types, File_formats,
                                        FILE ** );
Status   set_file_position( FILE *, long );
Status   close_file( FILE * );
void     extract_directory( char [], char [] );
void     get_absolute_filename( char [], char [], char [] );
void     strip_off_directories( char [], char [] );
Status   flush_file( FILE * );
Status   input_character( FILE *, char * );
Status   unget_character( FILE *, char );
Status   input_nonwhite_character( FILE *, char * );
Status   output_character( FILE *, char );
Status   skip_input_until( FILE *, char );
Status   output_string( FILE *, char [] );
Status   input_string( FILE *, char [], int, char );
Status   input_quoted_string( FILE *, char [], int );
Status   output_quoted_string( FILE *, char [] );
Status   input_binary_data( FILE *, void *, size_t, int );
Status   output_binary_data( FILE *, void *, size_t, int );
Status   input_newline( FILE * );
Status   output_newline( FILE * );
Status   input_line( FILE *, char [], int );
Status   input_boolean( FILE *, Boolean * );
Status   output_boolean( FILE *, Boolean );
Status   input_short( FILE *, short * );
Status   output_short( FILE *, short );
Status   input_int( FILE *, int * );
Status   output_int( FILE *, int );
Status   input_real( FILE *, Real * );
Status   output_real( FILE *, Real );
Status   input_float( FILE *, float * );
Status   output_float( FILE *, float );
Status   input_double( FILE *, double * );
Status   output_double( FILE *, double );
Status   io_binary_data( FILE *, IO_types, void *, size_t, int );
Status   io_newline( FILE *, IO_types, File_formats );
Status   io_quoted_string( FILE *, IO_types, File_formats, char [], int );
Status   io_boolean( FILE *, IO_types, File_formats, Boolean * );
Status   io_short( FILE *, IO_types, File_formats, short * );
Status   io_unsigned_char( FILE *, IO_types, File_formats, unsigned char *c );
Status   io_unsigned_chars( FILE *, IO_types, File_formats, int,
                            unsigned char *[] );
Status   io_int( FILE *, IO_types, File_formats, int * );
Status   io_double( FILE *, IO_types, File_formats, double * );
Status   io_float( FILE *, IO_types, File_formats, float * );
Status   io_real( FILE *, IO_types, File_formats, Real * );
Status   io_ints( FILE *, IO_types, File_formats, int, int *[] );

#endif
