
/* ----------------------------- MNI Header -----------------------------------
@NAME       : stdio.h
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: An include file that #redefines the standard file input functions
              to do file uncompression transparently.  For instance, if a call
              to fopen( "test.dat", "r" ) is made, and test.dat does NOT exist,
              and test.dat.Z does, then the latter is opened, and decompressed
              on the fly, as calls such as fread, fscanf, etc. are made.
@METHOD     : To use this, you need to include -I$(DIR), where DIR contains
              this stdio.h file (to override the system one in /usr/include).
              Second you need to link in the files decomp.c and fscanf.c.
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 29, 1992       David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#include  </usr/include/stdio.h>

#ifndef  IN_DECOMP_C
#ifndef  DONT_USE_COMPRESSION

/* redefine the standard file input functions */

#define  freopen   freopen_compress
#define  fopen     fopen_compress
#define  fread(ptr,s,n,file)     fread_compress((void*)(ptr),s,n,file )
#undef   feof
#define  feof      feof_compress
#define  fgetc     fgetc_compress
#undef   getc
#define  getc      getc_compress
#define  ungetc    ungetc_compress
#define  fgets     fgets_compress
#define  fseek     fseek_compress
#define  fclose    fclose_compress
#define  ftell     ftell_compress
#define  rewind    rewind_compress
#define  getw      getw_compress
#define  fscanf    fscanf_compress

extern FILE  *fopen_compress( char *filename, char *mode );

extern FILE  *freopen_compress( const char *filename, const char *mode,
                                         FILE *file );

extern  int  fclose_compress( FILE *file );

extern  int  ungetc_compress( int  ch, FILE *file );

extern  int  getw_compress( FILE *file );

extern  int  getc_compress( FILE *file );

extern  int  fgetc_compress( FILE *file );

extern  void  rewind_compress( FILE *file );

extern  int  feof_compress( FILE *file );

extern  long  int  ftell_compress( FILE *file );

extern  char *fgets_compress( char *s, int n, FILE *file );

extern  int  fseek_compress( FILE *file, long int pos, int flag );

extern  int  fread_compress( char *ptr, size_t sizef, size_t n, FILE *file );

#endif
#endif
