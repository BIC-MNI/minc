/* ----------------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

#include  <internal_volume_io.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Prog_utils/string.c,v 1.8 1995-10-19 15:46:46 david Exp $";
#endif

private  const  STRING  empty_string = "";

public  STRING  alloc_string(
    int   length )
{
    STRING   str;

    ALLOC( str, length+1 );

    return( str );
}

public  STRING  create_string(
    STRING    initial )
{
    STRING   str;

    if( initial == NULL )
        initial = empty_string;

    str = alloc_string( string_length(initial) );

    (void) strcpy( str, initial );

    return( str );
}

public  void  delete_string(
    STRING   string )
{
    if( string != NULL )
        FREE( string );
}

public  STRING  concat_strings(
    STRING   str1,
    STRING   str2 )
{
    STRING  str;

    if( str1 == NULL )
        str1 = empty_string;

    if( str2 == NULL )
        str2 = empty_string;

    ALLOC( str, string_length(str1) + string_length(str2) + 1 );

    (void) strcpy( str, str1 );
    (void) strcat( str, str2 );

    return( str );
}

public  void  replace_string(
    STRING   *string,
    STRING   new_string )
{
    delete_string( *string );
    *string = new_string;
}

public  void  concat_char_to_string(
    STRING   *string,
    char     ch )
{
    int  len;

    len = string_length( *string );

    if( *string == NULL )
        *string = alloc_string( 1 );
    else
        SET_ARRAY_SIZE( *string, len+1, len+2, 1 );

    (*string)[len] = ch;
    (*string)[len+1] = END_OF_STRING;
}

public  void  concat_to_string(
    STRING   *string,
    STRING   str2 )
{
    STRING  new_string;

    new_string = concat_strings( *string, str2 );
    replace_string( string, new_string );
}

public  int  string_length(
    STRING   string )
{
    if( string == NULL )
        return( 0 );
    else
        return( (int) strlen( string ) );
}

public  BOOLEAN  equal_strings(
    STRING   str1,
    STRING   str2 )
{
    if( str1 == NULL )
        str1 = empty_string;
    if( str2 == NULL )
        str2 = empty_string;

    return( strcmp( str1, str2 ) == 0 );
}

public  BOOLEAN  is_lower_case(
    char  ch )
{
    return( ch >= 'a' && ch <= 'z' );
}

public  BOOLEAN  is_upper_case(
    char  ch )
{
    return( ch >= 'A' && ch <= 'Z' );
}

public  char  get_lower_case(
    char   ch )
{
    if( is_upper_case( ch ) )
        return( (char) ((ch)+'a'-'A') );
    else
        return( ch );
}

public  char  get_upper_case(
    char   ch )
{
    if( is_lower_case( ch ) )
        return( (char) ((ch)+'A'-'a') );
    else
        return( ch );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : string_ends_in
@INPUT      : string
            : ending
@OUTPUT     : 
@RETURNS    : TRUE if string ends in "ending"
@DESCRIPTION: Checks if the string ends in ending, e.g., 
            : string_ends_in( "main.c", ".c" ) returns true.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  BOOLEAN  string_ends_in(
    STRING   string,
    STRING   ending )
{
    int      len_string, len_ending;
    BOOLEAN  ending_present;

    len_string = string_length( string );
    len_ending = string_length( ending );

    if( len_ending > len_string )
        ending_present = FALSE;
    else
        ending_present = (strcmp( &string[len_string-len_ending], ending ) ==0);

    return( ending_present );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : strip_outer_blanks
@INPUT      : str
@OUTPUT     : stripped
@RETURNS    : 
@DESCRIPTION: Creates a new string which is the original str without any
            : leading or trailing blanks.  Output argument may be the same
              pointer as input.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public    STRING   strip_outer_blanks(
    STRING  str )
{
    STRING  stripped;
    int  i, first_non_blank, last_non_blank, len;

    len = string_length( str );

    /* --- skip leading blanks */

    first_non_blank = 0;
    while( first_non_blank < len && str[first_non_blank] == ' ' )
    {
        ++first_non_blank;
    }

    /* --- skip trailing blanks */

    last_non_blank = len-1;
    while( last_non_blank >= 0 && str[last_non_blank] == ' ' )
    {
        --last_non_blank;
    }

    /* --- now copy string, without leading or trailing blanks */

    if( first_non_blank > last_non_blank )
        last_non_blank = first_non_blank - 1;

    stripped = alloc_string( last_non_blank - first_non_blank + 1 );

    for_inclusive( i, first_non_blank, last_non_blank )
        stripped[i-first_non_blank] = str[i];

    stripped[last_non_blank - first_non_blank + 1] = END_OF_STRING;

    return( stripped );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : find_character
@INPUT      : string
            : ch
@OUTPUT     : 
@RETURNS    : index of ch within string or -1
@DESCRIPTION: Finds the index of the given character within the string.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  int  find_character(
    STRING    string,
    char      ch )
{
    int   i;

    if( string == NULL )
        return( -1 );

    i = 0;
    while( string[i] != END_OF_STRING )
    {
        if( string[i] == ch )
            return( i );
        ++i;
    }

    return( -1 );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : make_string_upper_case
@INPUT      : string
@OUTPUT     : string
@RETURNS    : 
@DESCRIPTION: Converts every lower case character in string to upper case.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  make_string_upper_case(
    STRING    string )
{
    int   i, len;

    len = string_length( string );

    for_less( i, 0, len )
    {
        string[i] = get_upper_case( string[i] );
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : blank_string
@INPUT      : string
@OUTPUT     : 
@RETURNS    : TRUE if string is blank
@DESCRIPTION: Checks to see if the string is blank; only contains space,
              tabs, and newlines.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  BOOLEAN  blank_string(
    STRING   string )
{
    int      i;
    BOOLEAN  blank;

    if( string == NULL )
        string = empty_string;

    blank = TRUE;
    i = 0;
    while( string[i] != END_OF_STRING )
    {
        if( string[i] != ' ' && string[i] != '\t' && string[i] != '\n' )
        {
            blank = FALSE;
            break;
        }
        ++i;
    }

    return( blank );
}
