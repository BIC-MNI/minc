
/* ----------------------------- MNI Header -----------------------------------
@NAME       : decomp.c
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: This file implements a version of all the standard file input
              functions to do file uncompression transparently.  For instance,
              if a call to fopen( "test.dat", "r" ) is made, and test.dat
              does NOT exist, and test.dat.Z does, then the latter is opened,
              and uncompressed on the fly, as calls such as fread, fscanf,
              etc. are made.
@METHOD     : The code has been modified from a public domain utitily
              to uncompress .Z files.
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 29        David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  IN_DECOMP_C      /* to avoid getting the redefining stdio.h */

#include <def_mni.h>
#include <ctype.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>

/* --------- memory allocation macros -------------------------- */

#define  _MALLOC( n_items, type ) \
         ( (type *) malloc( (size_t) (n_items) * sizeof(type) ) )

#define  _CALLOC( n_items, type ) \
         ( (type *) calloc( (size_t) (n_items), sizeof(type) ) )

#define  _REALLOC( ptr, n_items, type ) \
         ( (type *) realloc( (void *) ptr, (size_t) (n_items) * sizeof(type) ) )

#define  _FREE( ptr ) \
         free( (void *) ptr )

/* -------------------------------------------------------------------------
       set local variables 
------------------------------------------------------------------------- */

#define BITS   16
#define HSIZE  (1L<<BITS)

private    void            check_initialized_bit_lookup();
private    unsigned short  compute_bits( int   n_bit,
                                         int   r_off,
                                         int   byte,
                                         int   byte_position );
private    int             _innum( int   **ptr,
                                   int   type,
                                   int   len,
                                   int   size,
                                   FILE  *iop,
                                   int   *eofptr );
private    int             _instr( register char *ptr,
                                   int           type,
                                   int           len,
                                   register FILE *iop,
                                   int           *eofptr );

typedef unsigned char        char_type;

private  char_type magic_header[] = { "\037\235" };        /* 1F 9D */

/* Defines for third byte of header */
#define BIT_MASK        0x1f
#define BLOCK_MASK        0x80
#define INIT_BITS 9                        /* initial number of bits/code */

/* There is a set of variables associated with each file being input.
   They are kept in structures associated with each file.
   To avoid the inefficient derefencing of pointers in the uncompression
   code, the variables are copied to and from the relevant structures as
   needed.  If you open a file, read it, and close it, then there will
   only be one copy.  If you open two files, and intersperse reads, then
   this will be inefficient, due to the context switch (copying to/from
   structures) every call to read.  */

#define  STATIC_VARIABLES \
VARIABLE(   FILE            *,file_ptr )\
VARIABLE(   int             ,file_position ) \
VARIABLE(   int             ,maxbits ) \
VARIABLE(   int             ,block_compress ) \
VARIABLE(   int             ,maxmaxcode ) \
VARIABLE(   int             ,n_bits ) \
VARIABLE(   int             ,maxcode ) \
VARIABLE(   int             ,free_ent ) \
VARIABLE(   int             ,finchar ) \
VARIABLE(   int             ,oldcode ) \
VARIABLE(   int             ,eof ) \
VARIABLE(   int             ,first_char_flag ) \
VARIABLE(   char_type       *,htab ) \
VARIABLE(   unsigned short  *,codetab ) \
VARIABLE(   char            *,stackp ) \
VARIABLE(   char            *,de_stack ) \
VARIABLE(   int             ,n_in_buffer ) \
VARIABLE(   int             ,buffer_index ) \
VARIABLE(   char            *,buffer )

#define  VARIABLE( type, var ) type var;

typedef  struct
{
    STATIC_VARIABLES
} compress_struct;

#undef  VARIABLE
#define  VARIABLE(type,var)  private  type  var;

STATIC_VARIABLES

/* ----------------------------- MNI Header -----------------------------------
@NAME       : copy_from_context
@INPUT      : compress      - a structure containing the uncompression vars.
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Copies all the variables from the structure to the local vars.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 29, 1991      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#undef  VARIABLE
#define  VARIABLE(type,var)  var = compress->var;

private  void  copy_from_context( compress_struct *compress )
{
    STATIC_VARIABLES
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : copy_to_context
@INPUT      : compress      - a structure containing the uncompression vars.
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Copies all the variables to the structure from the local vars.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 29, 1991      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#undef  VARIABLE
#define  VARIABLE(type,var)  compress->var = var;

private  void  copy_to_context( compress_struct *compress )
{
    STATIC_VARIABLES
}

/* ----------- the list of open files  -------------------- */

private  int  file_index = -1;

private  compress_struct  *compress_files;
private  int              n_files = 0;
private  int              n_files_alloced = 0;

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_struct
@INPUT      : file          
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Creates a structure containing uncompression variables for the
              given file.  The file also is stored within this structure.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 29, 1992      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  void  create_struct( FILE *file )
{
    /* check if we need to increase the allocation of the list */

    if( n_files >= n_files_alloced )
    {
        if( n_files_alloced == 0 )
        {
            n_files_alloced = 10;
            compress_files = _MALLOC( n_files_alloced, compress_struct );
        }
        else
        {
            n_files_alloced += 10;
            compress_files = _REALLOC( compress_files,
                                      n_files_alloced, compress_struct );
        }
    }

    /* store it in the list */

    compress_files[n_files].file_ptr = file;
    ++n_files;

    check_initialized_bit_lookup();
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : delete_struct
@INPUT      : file          
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Deletes the current structure containing uncompression variables
              for the given file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 29, 1992      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  void  delete_struct()
{
    int  i;

    if( file_index >= 0 && file_index < n_files )
    {
        --n_files;
        for( i = file_index;  i < n_files;  ++i )
            compress_files[i] = compress_files[i+1];

        file_ptr = (FILE *) 0;
        file_index = -1;
    }
    else
    {
        /* should never happen, but let's not print an error anyway */
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_file_context
@INPUT      : file
@OUTPUT     : 
@RETURNS    : TRUE if the file is being uncompressed, FALSE if normal file.
@DESCRIPTION: Copies the variables in the structure associated with the
              given file to the local variables.  First it may save the
              current local variables into the appropriate structure.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 29, 1992        David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  int  set_file_context( FILE *file )
{
    int   i;
    void  copy_to_context( compress_struct * );
    void  copy_from_context( compress_struct * );

    if( file == (FILE *) 0 )
        return( FALSE );

    if( file_ptr != file )
    {
        for( i = 0;  i < n_files;  ++i )
        {
            if( compress_files[i].file_ptr == file )
            {
                /* save previous context */

                if( file_index >= 0 && file_ptr != (FILE *) 0 )
                    copy_to_context( &compress_files[file_index] );

                /* setup context */

                copy_from_context( &compress_files[i] );
                file_index = i;
                return( TRUE );
            }
        }
        return( FALSE );
    }

    return( TRUE );
}

/* -------------------------------------------------------------------------
       code chopped and modified from public domain decomp

   Because it was copied and modified from public domain source, I haven't
add too many additional comments.
------------------------------------------------------------------------- */


# define MAXCODE(n_bits)        ((1L << (n_bits)) - 1)


/*
 * the next two codes should not be changed lightly, as they must not
 * lie within the contiguous general code space.
 */
#define FIRST        257        /* first free entry */
#define        CLEAR        256        /* table clear output code */

/* -------------------------------------------------------------------------
            MACROS  for speed
 ------------------------------------------------------------------------- */

private  int  bitlist_initialized = FALSE;

typedef  struct
{
    unsigned short   byte1[256], byte2[256], byte3[256];
} bit_table_struct;


private  bit_table_struct  *bit_table[BITS+1];

private    void            generate_bit_table( bit_table_struct  *table,
                                               int               n_bit,
                                               int               r_off );

private  void check_initialized_bit_lookup()
{
    int   n_bit, r_off;

    if( !bitlist_initialized )
    {
        bitlist_initialized = TRUE;

        for( n_bit = INIT_BITS;  n_bit <= BITS;  ++n_bit )
        {
            bit_table[n_bit] = _MALLOC( 8, bit_table_struct );

            for( r_off = 0;  r_off < 8;  ++r_off )
            {
                generate_bit_table( &bit_table[n_bit][r_off], n_bit, r_off );
            }
        }
    }
}

private  void  generate_bit_table(
    bit_table_struct  *table,
    int               n_bit,
    int               r_off )
{
    int             byte;

    for( byte = 0;  byte < 256;  ++byte )
    {
        table->byte1[byte] = compute_bits( n_bit, r_off, byte, 0 );
        table->byte2[byte] = compute_bits( n_bit, r_off, byte, 1 );
        table->byte3[byte] = compute_bits( n_bit, r_off, byte, 2 );
    }
}

private  unsigned short   compute_bits(
    int   n_bit,
    int   r_off,
    int   byte,
    int   byte_position )
{
    int      word;
    static   int  bit_mask[BITS+1] =
       { 0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f,
         0xff, 0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff, 0xffff,
       };

    word = (byte << (8*byte_position));
    word >>= r_off;
    word &= bit_mask[n_bit];
    return( (unsigned short) word );
}

/*****************************************************************
 * TAG( getcode )
 *
 * Read one code from the standard input.  If EOF, return -1.
 * Inputs:
 *         stdin
 * Outputs:
 *         code or -1 is returned.
 */

private  void  extract_more_characters()
{
    register int        code;
    int                 offset, i, incode;
    static  char_type   buf[BITS];
    static  int         code_buffer[8];
    char_type           *bp;
    bit_table_struct    *table;
    int                 b1, b2, b3;
    int                 n_codes_in_buffer;

    buffer_index = 0;
    n_in_buffer = 0;

read_block:

    n_codes_in_buffer = fread( buf, 1, n_bits, file_ptr );

    if ( n_codes_in_buffer < n_bits )
    {
        if( n_codes_in_buffer <= 0 )
        {
            eof = TRUE;
            return;
        }

        /* Round size down to integral number of codes */
        n_codes_in_buffer = (n_codes_in_buffer << 3) / n_bits;
    }
    else
        n_codes_in_buffer = 8;

    if( n_bits == 16 && n_codes_in_buffer == 16 )
    {
        code_buffer[0] = (buf[0]  | (buf[1] << 8));
        code_buffer[1] = (buf[2]  | (buf[3] << 8));
        code_buffer[2] = (buf[4]  | (buf[5] << 8));
        code_buffer[3] = (buf[6]  | (buf[7] << 8));
        code_buffer[4] = (buf[8]  | (buf[9] << 8));
        code_buffer[5] = (buf[10] | (buf[11] << 8));
        code_buffer[6] = (buf[12] | (buf[13] << 8));
        code_buffer[7] = (buf[14] | (buf[15] << 8));
    }
    else
    {
        offset = 0;

        for( i = 0;  i < n_codes_in_buffer;  ++i )
        {
                /*
                 * Get to the i'th byte.
                 */
            bp = &buf[offset >> 3];
            table = &bit_table[n_bits][offset & 7];
            b1 = table->byte1[*bp++];
            b2 = table->byte2[*bp++];
            b3 = table->byte3[*bp];
            code_buffer[i] = (b1 | b2 | b3);
            offset += n_bits;
        }
    }

    for( i = 0;  i < n_codes_in_buffer;  ++i )
    {
        code = code_buffer[i];

        if ( (code == CLEAR) && block_compress ) {
            for( code = 255; code >= 0; code-- )
                codetab[code] = 0;

            maxcode = MAXCODE (n_bits = INIT_BITS);
            free_ent = FIRST - 1;

            goto read_block;
        }

        incode = code;
        /*
         * Special case for KwKwK string.
         */
        if ( code >= free_ent ) {
            *stackp++ = finchar;
            code = oldcode;
        }

        /*
         * Generate output characters in reverse order
         */
        while ( code >= 256 ) {
            *stackp++ =  htab[code];
            code = codetab[code];
        }
        *stackp++ = finchar = htab[code];

        while( stackp > de_stack )
        {
            buffer[n_in_buffer++] = *--stackp;
        }

        /*
         * Generate the new entry.
         */
        if( first_char_flag )
            first_char_flag = FALSE;
        else if ( (free_ent) < maxmaxcode ) {
            codetab[free_ent] = (unsigned short)oldcode;
            htab[free_ent] = finchar;
            ++free_ent;
            /*
             * If the next entry will be too big for the current code
             * size, then we must increase the size.  This implies reading
             * a new buffer full, too.
             */
            if ( free_ent > maxcode ) {
                n_bits++;
                if ( n_bits == maxbits )
                    maxcode = maxmaxcode;        /* won't get any bigger now */
                else
                    maxcode = MAXCODE(n_bits);
                oldcode = incode;
                break;
            }
        }
        /*
         * Remember previous code.
         */
        oldcode = incode;
    }
}

#define  GET_CHAR( ch ) \
{ \
    void  extract_more_characters(); \
 \
    if( buffer_index < n_in_buffer ) \
    { \
        (ch) = buffer[buffer_index++]; \
            ++file_position;  \
    } \
    else if( !eof ) \
    { \
        extract_more_characters(); \
 \
        if( buffer_index < n_in_buffer ) \
        { \
            (ch) = buffer[buffer_index++]; \
            ++file_position;  \
        } \
        else \
        { \
            (ch) = EOF; \
            eof = TRUE; \
        } \
    } \
    else \
        (ch) = EOF; \
}

/*****************************************************************
 * TAG( main )
 *
 * Algorithm from "A Technique for High Performance Data Compression",
 * Terry A. Welch, IEEE Computer Vol 17, No 6 (June 1984), pp 8-19.
 *
 * Algorithm:
 *         Modified Lempel-Ziv method (LZW).  Basically finds common
 * substrings and replaces them with a variable size code.  This is
 * deterministic, and can be done on the fly.  Thus, the uncompression
 * procedure needs no input table, but tracks the way the table was built.
 */

/* ----------------------------- MNI Header -----------------------------------
@NAME       : file_is_in_compressed_format
@INPUT      : filename   - name of file to check
@OUTPUT     : 
@RETURNS    : TRUE if the file needs to be uncompressed
@DESCRIPTION: Checks if filename ends in .Z or if it does not exist, but
              filename.Z does.  Checks the first two bytes for magic 
              number meaning compressed format.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 29, 1992       David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  int  file_is_in_compressed_format( char filename[] )
{
    FILE     *file;
    int      compressed;

    compressed = FALSE;
    file = fopen( filename, "rb" );

    if( file != (FILE *) 0 )
    {
        if( fgetc(file) == (magic_header[0] & 0xFF) &&
            fgetc(file) == (magic_header[1] & 0xFF) )
        {
            compressed = TRUE;
        }

        (void) fclose( file );
    }

    return( compressed );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : initialize_compress_file
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Initializes the current file_ptr by allocating space, reading
              the magic number and the first uncompressed character.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  void  initialize_compress_file()
{
    int            fsize, hsize, code;
    struct  stat   statbuf;
    int            getcode();

    (void) fstat(fileno(file_ptr), &statbuf);
    fsize = statbuf.st_size;

    if ((fgetc(file_ptr) != (magic_header[0] & 0xFF)) ||
        (fgetc(file_ptr) != (magic_header[1] & 0xFF)))
    {
        (void) fprintf(stderr, "File not in compressed format\n" );
        return;
    }

    maxbits = fgetc(file_ptr);        /* set -b from file */
    block_compress = maxbits & BLOCK_MASK;
    maxbits &= BIT_MASK;
    maxmaxcode = 1L << maxbits;

    if(maxbits > BITS) {
        (void) fprintf(stderr,
                    "File compressed with %d bits, can only handle %d bits\n",
                    maxbits, BITS);
        return;
    }

    hsize = MIN(HSIZE, 258 + fsize); /* cannot have more codes than file size */

    htab = _CALLOC(hsize, char_type);
    if (htab == NULL)
    {
        (void) fprintf(stderr,"Not enough memory\n");
        return;
    }

    codetab = _CALLOC(hsize, unsigned short);
    if(codetab == NULL)
    {
        (void) fprintf(stderr, "Not enough memory\n");
        return;
    }

    de_stack = _MALLOC( hsize, char );
    n_bits = INIT_BITS;
    maxcode = MAXCODE(n_bits);

    buffer = _MALLOC( 64000, char );
    buffer_index = 0;
    n_in_buffer = 0;

    /*
     * As above, initialize the first 256 entries in the table.
     */

    for ( code = 255; code >= 0; code-- ) {
        codetab[code] = 0;
        htab[code] = (char_type)code;
    }

    free_ent = ((block_compress) ? FIRST : 256 );

    first_char_flag = TRUE;

    file_position = 0;

    stackp = de_stack;

    finchar = 0;

    eof = FALSE;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : terminate_compress_file
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Frees the space allocated for uncompression of the current file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 29, 1992       David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  void  terminate_compress_file()
{
    _FREE( de_stack );
    _FREE( htab );
    _FREE( codetab );
    _FREE( buffer );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_feof
@INPUT      : 
@OUTPUT     : 
@RETURNS    : TRUE if the current file has no more input characters.
@DESCRIPTION: 
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 29, 1992       David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  int  get_feof()
{
    return( stackp <= de_stack && eof );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : un_get_char
@INPUT      : 
@OUTPUT     : 
@RETURNS    : The character pushed back onto input.
@DESCRIPTION: Puts a character back on the uncompression input stream.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 29, 1992       David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  int  un_get_char(
    int  ch )
{
    int   i;

    if( buffer_index >= n_in_buffer )
    {
        buffer_index = 0;
        n_in_buffer = 1;
    }
    else if( buffer_index > 0 )
    {
        --buffer_index;
    }
    else
    {
        for( i = n_in_buffer;  i > 0;  --i )
            buffer[i] = buffer[i-1];
        buffer_index = 0;
    }

    buffer[buffer_index] = ch;

    if( file_position > 0 )
        --file_position;

    return( ch );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_char
@INPUT      : 
@OUTPUT     : 
@RETURNS    : the character or EOF
@DESCRIPTION: Gets the next character from the uncompression stream.
              It is a public function so fscanf_compress can read it.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 29, 1992       David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  int  get_char()
{
    int   ch;

    GET_CHAR( ch );

    return( ch );
}

/* ---------------------------------------------------------------------
      public functions
--------------------------------------------------------------------- */

/* ----------------------------- MNI Header -----------------------------------
@NAME       : should_open_compressed
@INPUT      : filename          - parameters to fopen()
              mode
@OUTPUT     : compressed_filename - the name of the compressed file to open
@RETURNS    : TRUE if the file is in compressed format
@DESCRIPTION: Checks if the file should be opened in compressed format,
              and fills in the name in compressed_filename, if so. 
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 29, 1992       David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  int  should_open_compressed( char *filename, char *mode,
                                      char *compressed_filename )
{
    FILE             *file;
    int              len;
    int              compressed;
    int              file_is_in_compressed_format( char [] );

    compressed = FALSE;

    /* only need to check for compression if reading file */

    if( mode[0] == 'r' )
    {
        /* check if last two characters are  .Z */
        len = strlen( filename );

        if( len >= 2 && strcmp(&filename[len-2],".Z") == 0 )
        {
            if( file_is_in_compressed_format(filename) )
            {
                (void) strcpy( compressed_filename, filename );
                compressed = TRUE;
            }
        }

        if( !compressed )
        {
            /* check if file exists */

            file = fopen( filename, "r" );

            if( file != (FILE *) 0 )   /* if file exists as given */
            {
                (void) fclose( file );
            }
            else            /* file does not exist, check if filename.Z does */
            {
                (void) sprintf( compressed_filename, "%s.Z", filename );

                if( file_is_in_compressed_format(compressed_filename) )
                {
                    compressed = TRUE;
                }
            }
        }
    }

    return( compressed );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : fopen_compress
@INPUT      : filename     - parameters from fopen()
              mode
@OUTPUT     : 
@RETURNS    : FILE *
@DESCRIPTION: Opens the file, possibly for transparent uncompression.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 29, 1992       David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  FILE  *fopen_compress( char *filename, char *mode )
{
    int              set_file_context( FILE * );
    FILE             *file;
    char             compressed_filename[300];
    void             create_struct( FILE * );
    void             initialize_compress_file();
    int              should_open_compressed( char *, char *, char * );

    if( should_open_compressed( filename, mode, compressed_filename ) )
    {
        file = fopen( compressed_filename, "rb" );

        create_struct( file );

        (void) set_file_context( file );

        initialize_compress_file();
    }
    else
    {
        file = fopen( filename, mode );
    }

    return( file );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : freopen_compress
@INPUT      : filename    - parameters to freopen
              mode
              file
@OUTPUT     : 
@RETURNS    : FILE *
@DESCRIPTION: Does the freopen(), with transparent uncompression.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 29, 1992       David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  FILE  *freopen_compress( const char *filename, const char *mode,
                                 FILE *file )
{
    int              set_file_context( FILE * );
    char             compressed_filename[300];
    void             create_struct( FILE * );
    void             initialize_compress_file();
    int              should_open_compressed( char *, char *, char * );

    if( should_open_compressed( (char *) filename, (char *) mode,
                                compressed_filename ) )
    {
        if( freopen( compressed_filename, "rb", file ) != file )
        {
            (void) printf( "Error in reopen_compress.\n" );
            return( file );
        }

        create_struct( file );

        (void) set_file_context( file );

        initialize_compress_file();
    }
    else
    {
        file = freopen( filename, mode, file );
    }

    return( file );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : fclose_compress
@INPUT      : file
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Performs the fclose(), and ends uncompression, if necessary.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 29, 1992       David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  int  fclose_compress( FILE *file )
{
    void  terminate_compress_file();
    void  delete_struct();

    if( set_file_context(file) )
    {
        terminate_compress_file();
        delete_struct();
    }

    return( fclose(file) );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ungetc_compress
@INPUT      : ch
              file
@OUTPUT     : 
@RETURNS    : Character unget'ed
@DESCRIPTION: Performs the ungetc, with transparent uncompression.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 29, 1992       David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  int  ungetc_compress( int  ch, FILE *file )
{
    if( set_file_context(file) )
        return( un_get_char(ch) );
    else
        return( ungetc(ch,file) );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : getw_compress
@INPUT      : file
@OUTPUT     : 
@RETURNS    : word input or EOF
@DESCRIPTION: Performs the getw, with transparent uncompression.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 29, 1992       David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  int  getw_compress( FILE *file )
{
    int  i;
    int  word, ch;
    char *ptr;

    if( set_file_context(file) )
    {
        ptr = (char *) &word;
        for( i = 0;  i < sizeof(int);  ++i )
        {
            GET_CHAR( ch )
            if( ch == EOF )
                return( EOF );
            ptr[i] = ch;
        }
        return( word );
    }
    else
        return( getw(file) );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : getc_compress
@INPUT      : file
@OUTPUT     : 
@RETURNS    : character input or EOF
@DESCRIPTION: Performs the getc, with transparent uncompression.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 29, 1992       David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  int  getc_compress( FILE *file )
{
    int  ch;
    if( set_file_context(file) )
    {
        GET_CHAR( ch )
        return( ch );
    }
    else
        return( getc(file) );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : fgetc_compress
@INPUT      : file
@OUTPUT     : 
@RETURNS    : character input or EOF
@DESCRIPTION: Performs the fgetc, with transparent uncompression.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 29, 1992       David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  int  fgetc_compress( FILE *file )
{
    int  ch;
    if( set_file_context(file) )
    {
        GET_CHAR( ch )
        return( ch );
    }
    else
        return( fgetc(file) );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : rewind_compress
@INPUT      : file
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Performs the rewind, with transparent uncompression.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 29, 1992       David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  rewind_compress( FILE *file )
{
    void    initialize_compress_file();
    void    terminate_compress_file();

    if( set_file_context(file) )
    {
        terminate_compress_file();

        rewind( file );

        initialize_compress_file();
    }
    else
        rewind( file );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : feof_compress
@INPUT      : file
@OUTPUT     : 
@RETURNS    : TRUE if EOF
@DESCRIPTION: Performs the feof, with transparent uncompression.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 29, 1992       David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  int  feof_compress( FILE *file )
{
    if( set_file_context(file) )
        return( get_feof() );
    else
        return( feof(file) );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ftell_compress
@INPUT      : file
@OUTPUT     : 
@RETURNS    : position within file.
@DESCRIPTION: Performs the ftell(), with transparent uncompression.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 29, 1992       David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  long  int  ftell_compress( FILE *file )
{
    if( set_file_context(file) )
        return( file_position );
    else
        return( ftell(file) );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : fgets_compress
@INPUT      : s
              n
              file
@OUTPUT     : 
@RETURNS    : s
@DESCRIPTION: Performs the fgets(), with transparent uncompression.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 29, 1992       David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  char *fgets_compress( char *s, int n, FILE *file )
{
    int  i, ch;

    if( set_file_context(file) )
    {
        /* get chars until n-1 read, EOF, or return found
           (return is placed in string) */

        for( i = 0;  i < n-1;  ++i )
        {
            GET_CHAR( ch )
            if( ch == EOF ) break;
            s[i] = ch;
            if( ch == '\n' ) break;
        }

        s[i] = 0;
        return( s );
    }
    else
        return( fgets( s, n, file ) );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_to_tmp_uncompressed
@INPUT      : file
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Takes a compressed file and uncompresses it to a temporary file,
              in order to do random seek access on it.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 29, 1992       David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  void  convert_to_tmp_uncompressed( FILE  *file )
{
    FILE    *tmp_file;
    char    tmp_filename[200];
    int     ch;
    void    initialize_compress_file();
    void    terminate_compress_file();

    terminate_compress_file();

    (void) rewind( file );

    initialize_compress_file();

    /* create a temporary file and decompress file into it */

    (void) tmpnam( tmp_filename );

    tmp_file = fopen( tmp_filename, "wb" );

    if( tmp_file != (FILE *) 0 )
    {
        GET_CHAR( ch )
        while( ch != EOF )
        {
            (void) fputc( ch, tmp_file );
            GET_CHAR( ch )
        }

        (void) rewind( tmp_file );

        /* now reopen the tmp_filename as the current file */

        if( freopen( tmp_filename, "rb", file ) != file )
        {
            (void) printf( "Error in convert_to_tmp_uncompressed.\n" );
        }
    }

    terminate_compress_file();
    delete_struct();
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : fseek_compress
@INPUT      : file
              pos
              flag
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Implements the fseek, with transparent uncompression.
              Since compressed files can only be read sequentially, it
              implements seeking forward as just reading and ignoring
              the required number of characters.

              If seeking a position earlier in the file, then we assume
              that the user is making random accesses and for efficiency
              the entire file is uncompressed to a temporary file, which
              replaces the compressed file, thereby allowing the user
              to make random fseek's on an ordinary file.  The penalty is
              the one-time uncompression of the entire file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 29, 1992       David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  int  fseek_compress( FILE *file, long int pos, int flag )
{
    int   result, i, ch;
    long  new_position;

    if( set_file_context(file) )
    {
        /* find out where the user is searching to */

        switch( flag )
        {
        case  SEEK_SET:
            new_position = pos;
            break;
        case  SEEK_CUR:
            new_position = file_position + pos;
            break;
        case  SEEK_END:  /* if seeking relative to the end, cause the
                            file to be decompressed to a tmp file,
                            since we don't know the size of decompressed file */
        default:
            new_position = -1;
            break;
        }

        if( new_position >= 0 ) /* skip characters until correct position */
        {
            if( new_position < file_position )
            {
                rewind_compress( file );
            }

            result = 0;
            for( i = file_position;  i < new_position;  ++i )
            {
                GET_CHAR( ch )
                if( ch == EOF )
                {
                    result = 1;
                    break;
                }
            }
        }
        else    /* uncompress the file, and use it */
        {
            convert_to_tmp_uncompressed( file_ptr );
            result = fseek( file, pos, flag );
        }
    }
    else
        result = fseek( file, pos, flag );

    return( result );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : fread_compress
@INPUT      : ptr
              sizef
              n
              file
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Implements the fread, with transparent uncompression.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 29, 1992       David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  int  fread_compress( char *ptr, size_t sizef, size_t n, FILE *file )
{
    int   result, n_chars, n_read, n_left, n_to_copy;
    void  extract_more_characters();

    if( set_file_context(file) )
    {
        n_chars = sizef * n;

        n_read = 0;

        while( n_read < n_chars && (!eof || buffer_index < n_in_buffer) )
        {
            if( buffer_index < n_in_buffer )
            {
                n_left = n_in_buffer - buffer_index;
                n_to_copy = MIN( n_chars - n_read, n_left );

                (void) memcpy( &ptr[n_read], &buffer[buffer_index], n_to_copy );
                buffer_index += n_to_copy;
                n_read += n_to_copy;
            }
            else
                extract_more_characters();
        }

        file_position += n_read;
        result = n_read / sizef;
    }
    else
    {
        result = fread( ptr, sizef, n, file );
    }

    return( result );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : fscanf.c
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: The remainder of this file is code chopped from doscan.c,
              public domain source code.  It has been modified slightly
              to take input from a compressed file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 
@MODIFIED   : April 29, 1992       David MacDonald
---------------------------------------------------------------------------- */

/* This is file DOSCAN.C */
/* This file may have been modified by DJ Delorie (Jan 1991).  If so,
** these modifications are Coyright (C) 1991 DJ Delorie, 24 Kirsten Ave,
** Rochester NH, 03867-2954, USA.
*/

#define _doscan _____doscan
#include </usr/include/stdio.h>
#include <ctype.h>
#undef _doscan

#define	SPC	01
#define	STP	02

#define	SHORT	0
#define	REGULAR	1
#define	LONG	2
#define	INT	0
#define	FLOAT	1

#include  <stdarg.h>

private char *_getccl(
    unsigned char *s );

private char _sctab[256] = {
	0,0,0,0,0,0,0,0,
	0,SPC,SPC,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	SPC,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
};

#define  get_character( file ) \
        ( ((file) == (FILE *) 0 ) ?   get_char()  : getc(file) )

#define  unget_character( ch, file ) \
        if( (file) == (FILE *) 0 ) \
           (void) un_get_char(ch); \
        else  \
           (void) ungetc(ch,file)

int  fscanf_compress(FILE *iop, const unsigned char *fmt, ... )
{
	register int ch;
	int nmatch, len, ch1;
	int **ptr, fileended, size;
        int  get_char();
        int  set_file_context( FILE * );
        va_list   ap;

        va_start( ap, fmt );

        if( set_file_context( iop ) )
            iop = (FILE *) 0;

	nmatch = 0;
	fileended = 0;
	for (;;) switch (ch = *fmt++) {
	case '\0': 
                va_end( ap );
		return (nmatch);
	case '%': 
		if ((ch = *fmt++) == '%')
			goto def;
		ptr = 0;
		if (ch != '*')
			ptr = &(va_arg( ap, int * ));
		else
			ch = *fmt++;
		len = 0;
		size = REGULAR;
		while (isdigit(ch)) {
			len = len*10 + ch - '0';
			ch = *fmt++;
		}
		if (len == 0)
			len = 30000;
		if (ch=='l') {
			size = LONG;
			ch = *fmt++;
		} else if (ch=='h') {
			size = SHORT;
			ch = *fmt++;
		} else if (ch=='[')
			fmt = (void *) _getccl( (void *) fmt);
		if (isupper(ch)) {
			ch = tolower(ch);
			size = LONG;
		}
		if (ch == '\0')
                {
			va_end( ap );
			return(-1);
		}
		if (_innum(ptr, ch, len, size, iop, &fileended) && ptr)
			nmatch++;
		if (fileended)
                {
			va_end( ap );
			return(nmatch? nmatch: -1);
		}
		break;

	case ' ':
	case '\n':
	case '\t': 
		while ((ch1 = get_character(iop))==' ' || ch1=='\t' || ch1=='\n')
			;
		if (ch1 != EOF)
			unget_character(ch1, iop);
		break;

	default: 
	def:
		ch1 = get_character(iop);
		if (ch1 != ch) {
			va_end( ap );
			if (ch1==EOF)
				return(-1);
			unget_character(ch1, iop);
			return(nmatch);
		}
	}
}

private   int _innum(
    int   **ptr,
    int   type,
    int   len,
    int   size,
    FILE  *iop,
    int   *eofptr )
{
	register char *np;
	char numbuf[64];
	register c, base;
	int expseen, scale, negflg, c1, ndigit;
	long lcval;

	if (type=='c' || type=='s' || type=='[')
		return(_instr(ptr? *(char **)ptr: (char *)NULL, type, len, iop, eofptr));
	lcval = 0;
	ndigit = 0;
	scale = INT;
	if (type=='e'||type=='f'||type=='g')
		scale = FLOAT;
	base = 10;
	if (type=='o')
		base = 8;
	else if (type=='x')
		base = 16;
	np = numbuf;
	expseen = 0;
	negflg = 0;
	while ((c = get_character(iop))==' ' || c=='\t' || c=='\n');
	if (c=='-') {
		negflg++;
		*np++ = c;
		c = get_character(iop);
		len--;
	} else if (c=='+') {
		len--;
		c = get_character(iop);
	}
	for ( ; --len>=0; *np++ = c, c = get_character(iop)) {
		if (isdigit(c)
		 || base==16 && ('a'<=c && c<='f' || 'A'<=c && c<='F')) {
			ndigit++;
			if (base==8)
				lcval <<=3;
			else if (base==10)
				lcval = ((lcval<<2) + lcval)<<1;
			else
				lcval <<= 4;
			c1 = c;
			if (isdigit(c))
				c -= '0';
			else if ('a'<=c && c<='f')
				c -= 'a'-10;
			else
				c -= 'A'-10;
			lcval += c;
			c = c1;
			continue;
		} else if (c=='.') {
			if (base!=10 || scale==INT)
				break;
			ndigit++;
			continue;
		} else if ((c=='e'||c=='E') && expseen==0) {
			if (base!=10 || scale==INT || ndigit==0)
				break;
			expseen++;
			*np++ = c;
			c = get_character(iop);
			if (c!='+'&&c!='-'&&('0'>c||c>'9'))
				break;
		} else
			break;
	}
	if (negflg)
		lcval = -lcval;
	if (c != EOF) {
		unget_character(c, iop);
		*eofptr = 0;
	} else
		*eofptr = 1;
 	if (ptr==NULL || np==numbuf || (negflg && np==numbuf+1) )/* gene dykes*/
		return(0);
	*np++ = 0;
	switch((scale<<4) | size) {

	case (FLOAT<<4) | SHORT:
	case (FLOAT<<4) | REGULAR:
		**(float **)ptr = atof(numbuf);
		break;

	case (FLOAT<<4) | LONG:
		**(double **)ptr = atof(numbuf);
		break;

	case (INT<<4) | SHORT:
		**(short **)ptr = lcval;
		break;

	case (INT<<4) | REGULAR:
		**(int **)ptr = lcval;
		break;

	case (INT<<4) | LONG:
		**(long **)ptr = lcval;
		break;
	}
	return(1);
}

private int _instr(
    register char *ptr,
    int           type,
    int           len,
    register FILE *iop,
    int           *eofptr )
{
	register ch;
	register char *optr;
	int ignstp;

	*eofptr = 0;
	optr = ptr;
	if (type=='c' && len==30000)
		len = 1;
	ignstp = 0;
	if (type=='s')
		ignstp = SPC;
	while ((ch = get_character(iop)) != EOF && _sctab[ch] & ignstp)
		;
	ignstp = SPC;
	if (type=='c')
		ignstp = 0;
	else if (type=='[')
		ignstp = STP;
	while (ch!=EOF && (_sctab[ch]&ignstp)==0) {
		if (ptr)
			*ptr++ = ch;
		if (--len <= 0)
			break;
		ch = get_character(iop);
	}
	if (ch != EOF) {
		if (len > 0)
			unget_character(ch, iop);
		*eofptr = 0;
	} else
		*eofptr = 1;
	if (ptr && ptr!=optr) {
		if (type!='c')
			*ptr++ = '\0';
		return(1);
	}
	return(0);
}

private char *_getccl(
    unsigned char *s )
{
	register c, t;

	t = 0;
	if (*s == '^') {
		t++;
		s++;
	}
	for (c = 0; c < (sizeof _sctab / sizeof _sctab[0]); c++)
		if (t)
			_sctab[c] &= ~STP;
		else
			_sctab[c] |= STP;
	if ((c = *s) == ']' || c == '-') {	/* first char is special */
		if (t)
			_sctab[c] |= STP;
		else
			_sctab[c] &= ~STP;
		s++;
	}
	while ((c = *s++) != ']') {
		if (c==0)
			return((char *)--s);
		else if (c == '-' && *s != ']' && s[-2] < *s) {
			for (c = s[-2] + 1; c < *s; c++)
				if (t)
					_sctab[c] |= STP;
				else
					_sctab[c] &= ~STP;
		} else if (t)
			_sctab[c] |= STP;
		else
			_sctab[c] &= ~STP;
	}
	return((char *)s);
}
