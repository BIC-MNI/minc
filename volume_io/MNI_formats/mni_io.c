#include  <def_mni.h>

static   const char      COMMENT_CHAR1 = '%';
static   const char      COMMENT_CHAR2 = '#';

public  Status  mni_get_nonwhite_character(
    FILE   *file,
    char   *ch )
{
    Boolean  in_comment;
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
            (void) fprintf( stderr, "Expected '%c', found '%c'.\n",
                            expected_ch, ch );
            status = ERROR;
        }
    }
    else
    {
        (void) fprintf( stderr, "Expected '%c', found end of file.\n",
                        expected_ch );
    }

    return( status );
}

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
    Boolean  quoted;

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

public  Status  mni_input_keyword_and_equal_sign(
    FILE         *file,
    const char   keyword[],
    Boolean      print_error_message )
{
    Status     status;
    String     str;

    status = mni_input_string( file, str, MAX_STRING_LENGTH, '=', 0 );

    if( status == END_OF_FILE )
        return( status );

    if( status != OK ||
        strcmp( str, keyword ) != 0 ||
        mni_skip_expected_character( file, '=' ) != OK )
    {
        if( print_error_message )
            print( "Expected \"%s =\"\n", keyword );
        status = ERROR;
    }

    return( status );
}

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

public  Status  mni_input_double(
    FILE    *file,
    double  *d )
{
    Status   status;
    String   str;

    status = mni_input_string( file, str, MAX_STRING_LENGTH, ' ', ';' );

    if( status == OK && sscanf( str, "%lf", d ) != 1 )
    {
        unget_string( file, str );
        status = ERROR;
    }

    return( status );
}

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

public  Status  mni_input_int(
    FILE    *file,
    int     *i )
{
    Status   status;
    String   str;

    status = mni_input_string( file, str, MAX_STRING_LENGTH, ' ', ';' );

    if( status == OK && sscanf( str, "%d", i ) != 1 )
    {
        unget_string( file, str );
        status = ERROR;
    }

    return( status );
}

public  void  output_comments(
    FILE   *file,
    char   comments[] )
{
    int   i, len;

    if( comments != (char *) NULL )
    {
        len = strlen( comments );

        (void) fputc( COMMENT_CHAR1, file );
        for( i = 0;  i < len;  ++i )
        {
            (void) fputc( comments[i], file );
            if( comments[i] == '\n' )
                (void) fputc( COMMENT_CHAR1, file );
        }
    }
}
