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
static char rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Prog_utils/string.c,v 1.7 1995-07-31 13:44:42 david Exp $";
#endif

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
    char   string[],
    char   ending[] )
{
    int      len_string, len_ending;
    BOOLEAN  ending_present;

    len_string = strlen( string );
    len_ending = strlen( ending );

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

public    void   strip_outer_blanks(
    char  str[],
    char  stripped[] )
{
    int  i, first_non_blank, last_non_blank, len;

    len = strlen( str );

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

    for_inclusive( i, first_non_blank, last_non_blank )
        stripped[i-first_non_blank] = str[i];

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
    int   i;

    if( string == (char *) NULL )
        return( -1 );

    i = 0;
    while( string[i] != (char) 0 )
    {
        if( string[i] == ch )
            return( i );
        ++i;
    }

    return( -1 );
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
    char   string[] )
{
    int      i;
    BOOLEAN  blank;

    blank = TRUE;
    i = 0;
    while( string[i] != (char) 0 )
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
