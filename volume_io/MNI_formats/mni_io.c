#include  <internal_volume_io.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/minc/volume_io/MNI_formats/mni_io.c,v 1.5 1994-11-25 14:20:25 david Exp $";
#endif

static   const char      COMMENT_CHAR1 = '%';
static   const char      COMMENT_CHAR2 = '#';

/* ----------------------------- MNI Header -----------------------------------
@NAME       : mni_get_nonwhite_character
@INPUT      : file
@OUTPUT     : ch
@RETURNS    : OK or END_OF_FILE
@DESCRIPTION: Gets the next non white space character from the MNI file
              (i.e., tags or transforms).  This routine handles comment
              characters, and is thus the base routine for all MNI tag or
              transform file input.  Any part of a line starting with a
              comment character is ignored.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  mni_get_nonwhite_character(
    FILE   *file,
    char   *ch )
{
    BOOLEAN  in_comment;
    Status   status;

    in_comment = FALSE;

    do
    {
        status = input_character( file, ch );
        if( status == OK )
            if( *ch == COMMENT_CHAR1 || *ch == COMMENT_CHAR2 )
                in_comment = TRUE;
            else if( *ch == '\n' )
                in_comment = FALSE;
    }
    while( status == OK &&
           (in_comment || *ch == ' ' || *ch == '\t' || *ch == '\n') );

    if( status == ERROR )
        status = END_OF_FILE;

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : mni_skip_expected_character
@INPUT      : file
              expected_ch
@OUTPUT     : 
@RETURNS    : OK or ERROR
@DESCRIPTION: Gets the next nonwhite character.  If it is the expected
              character, fine, otherwise print an error message.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  mni_skip_expected_character(
    FILE   *file,
    char   expected_ch )
{
    char     ch;
    Status   status;

    status = mni_get_nonwhite_character( file, &ch );

    if( status == OK )
    {
        if( ch != expected_ch )
        {
            print( "Expected '%c', found '%c'.\n", expected_ch, ch );
            status = ERROR;
        }
    }
    else
    {
        print( "Expected '%c', found end of file.\n", expected_ch );
    }

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : mni_input_line
@INPUT      : file
              max_length
@OUTPUT     : string
@RETURNS    : OK or END_OF_FILE
@DESCRIPTION: Inputs a line of text from a file.  The carriage return is
              read, but not placed in the string.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  mni_input_line(
    FILE   *file,
    char   string[],
    int    max_length )
{
    Status   status;
    int      len;
    char     ch;

    len = 0;

    status = input_character( file, &ch );

    while( status == OK && len < max_length-1 && ch != '\n' )
    {
        string[len] = ch;
        ++len;
        status = input_character( file, &ch );
    }

    string[len] = (char) 0;

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : mni_input_string
@INPUT      : file
              max_length
              termination_char1
              termination_char2
@OUTPUT     : string
@RETURNS    : OK or END_OF_FILE
@DESCRIPTION: Inputs a string from the file, up to the next occurrence of
              one of the termination characters or a carriage return.  If
              the first nonwhite character is a '"', then the termination
              characters become '"'.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  mni_input_string(
    FILE   *file,
    char   string[],
    int    max_length,
    char   termination_char1,
    char   termination_char2 )
{
    Status   status;
    int      len;
    char     ch;
    BOOLEAN  quoted;

    len = 0;
    status = mni_get_nonwhite_character( file, &ch );

    if( status == OK && ch == '"' )
    {
        quoted = TRUE;
        status = mni_get_nonwhite_character( file, &ch );
        termination_char1 = '"';
        termination_char2 = '"';
    }
    else
        quoted = FALSE;

    while( status == OK &&
           len < max_length-1 &&
           ch != termination_char1 && ch != termination_char2 && ch != '\n' )
    {
        string[len] = ch;
        ++len;
        status = input_character( file, &ch );
    }

    if( !quoted )
        (void) unget_character( file, ch );

    while( len > 0 && string[len-1] == ' ' )
        --len;

    string[len] = (char) 0;

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : mni_input_keyword_and_equal_sign
@INPUT      : file
              keyword
              print_error_message - whether to print error messages
@OUTPUT     : 
@RETURNS    : OK or ERROR
@DESCRIPTION: Inputs the desired keyword from the file and an equal sign.
              If there is no match, then an error message may be printed.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  mni_input_keyword_and_equal_sign(
    FILE         *file,
    const char   keyword[],
    BOOLEAN      print_error_message )
{
    Status     status;
    STRING     str;

    status = mni_input_string( file, str, MAX_STRING_LENGTH, '=', 0 );

    if( status == END_OF_FILE )
        return( status );

    if( status != OK || strcmp( str, keyword ) != 0 ||
        mni_skip_expected_character( file, '=' ) != OK )
    {
        if( print_error_message )
            print( "Expected \"%s =\"\n", keyword );
        status = ERROR;
    }

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : unget_string
@INPUT      : file
              str
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Places the first nonblank character of the string back onto
              the input stream, as an approximation to pushing the entire
              string back on the input stream, which only happens in error
              situations.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  void  unget_string(
    FILE    *file,
    char    str[] )
{
    int  len;

    len = 0;

    while( str[len] == ' ' || str[len] == '\t' )
        ++len;

    if( str[len] != (char) 0 )
        (void) unget_character( file, str[len] );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : mni_input_double
@INPUT      : file
@OUTPUT     : d
@RETURNS    : OK or ERROR
@DESCRIPTION: Inputs an ascii representation of a double value.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  mni_input_double(
    FILE    *file,
    double  *d )
{
    Status   status;
    STRING   str;

    status = mni_input_string( file, str, MAX_STRING_LENGTH, ' ', ';' );

    if( status == OK && sscanf( str, "%lf", d ) != 1 )
    {
        unget_string( file, str );
        status = ERROR;
    }

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : mni_input_doubles
@INPUT      : file
@OUTPUT     : n
              doubles
@RETURNS    : OK or ERROR
@DESCRIPTION: Inputs an arbitrary number of double values, up to the next
              semicolon.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  mni_input_doubles(
    FILE    *file,
    int     *n,
    double  *doubles[] )
{
    double  d;

    *n = 0;

    while( mni_input_double( file, &d ) != ERROR )
    {
        ADD_ELEMENT_TO_ARRAY( *doubles, *n, d, DEFAULT_CHUNK_SIZE );
    }

    return( mni_skip_expected_character( file, ';' ) );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : mni_input_int
@INPUT      : file
@OUTPUT     : i
@RETURNS    : OK or ERROR
@DESCRIPTION: Inputs an integer from an ascii file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  mni_input_int(
    FILE    *file,
    int     *i )
{
    Status   status;
    STRING   str;

    status = mni_input_string( file, str, MAX_STRING_LENGTH, ' ', ';' );

    if( status == OK && sscanf( str, "%d", i ) != 1 )
    {
        unget_string( file, str );
        status = ERROR;
    }

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : output_comments
@INPUT      : file
              comments
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Outputs a string to the file, in comment format, by placing
              a comment at the beginning of the string, and after each
              carriage return.  An extra carriage return is placed after
              the comments, if the comments do not end in a carriage return.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  output_comments(
    FILE   *file,
    char   comments[] )
{
    int   i, len;

    if( comments != (char *) NULL )
    {
        len = strlen( comments );

        (void) output_character( file, COMMENT_CHAR1 );
        for( i = 0;  i < len;  ++i )
        {
            (void) output_character( file, comments[i] );
            if( comments[i] == '\n' && i < len - 1 )
                (void) output_character( file, COMMENT_CHAR1 );
        }

        if( len > 0 && comments[len-1] != '\n' )
            (void) output_character( file, '\n' );
    }
}
