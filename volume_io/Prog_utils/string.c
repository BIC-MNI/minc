
#include  <def_mni.h>

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

public  Boolean  string_ends_in(
    char   string[],
    char   ending[] )
{
    int      len_string, len_ending;
    Boolean  ending_present;

    len_string = strlen( string );
    len_ending = strlen( ending );

    if( len_ending > len_string )
    {
        ending_present = FALSE;
    }
    else
    {
        ending_present = (strcmp( &string[len_string-len_ending], ending ) ==0);
    }

    return( ending_present );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : strip_blanks
@INPUT      : str
@OUTPUT     : stripped
@RETURNS    : 
@DESCRIPTION: This should be called strip_leading_and_trailing_blanks.
            : Creates a new string which is the original str without any
            : leading or trailing blanks.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public    void   strip_blanks(
    char  str[],
    char  stripped[] )
{
    int  i, first_non_blank, last_non_blank, len;

    len = strlen( str );

    first_non_blank = 0;
    while( first_non_blank < len && str[first_non_blank] == ' ' )
    {
        ++first_non_blank;
    }

    last_non_blank = len-1;
    while( last_non_blank >= 0 && str[last_non_blank] == ' ' )
    {
        --last_non_blank;
    }

    for_inclusive( i, first_non_blank, last_non_blank )
    {
        stripped[i-first_non_blank] = str[i];
    }

    stripped[last_non_blank - first_non_blank + 1] = (char) 0;
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
    char    string[],
    char    ch )
{
    int   i, len;

    len = strlen( string );

    for_less( i, 0, len )
    {
        if( string[i] == ch ) break;
    }

    if( i >= len )
    {
        i = -1;
    }

    return( i );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_upper_case_string
@INPUT      : string
@OUTPUT     : upper_case
@RETURNS    : 
@DESCRIPTION: Converts every lower case character in string to upper case,
            : with the result in "upper_case".
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  get_upper_case_string(
    char    string[],
    char    upper_case[] )
{
    int   i, len;

    len = strlen( string );

    for_less( i, 0, len )
    {
        upper_case[i] = GET_UPPER_CASE( string[i] );
    }

    upper_case[i] = (char) 0;
}
