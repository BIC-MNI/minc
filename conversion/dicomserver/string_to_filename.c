/* ----------------------------- MNI Header -----------------------------------
@NAME       : string_to_filename.c
@DESCRIPTION: Code to convert a string to something that can be used in a 
              file name.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 10, 1997 (Peter Neelin)
@MODIFIED   : 
 * $Log: string_to_filename.c,v $
 * Revision 6.1  1999-10-29 17:52:00  neelin
 * Fixed Log keyword
 *
 * Revision 6.0  1997/09/12 13:24:27  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:25:26  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:06:20  neelin
 * Release of minc version 0.4
 *
 * Revision 1.1  1997/03/04  20:56:47  neelin
 * Initial revision
 *
@COPYRIGHT  :
              Copyright 1997 Peter Neelin, McConnell Brain Imaging Centre, 
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

#include <dicomserver.h>
#include <ctype.h>

#define SEPARATOR '_'

/* ----------------------------- MNI Header -----------------------------------
@NAME       : string_to_filename
@INPUT      : string - string to convert
              maxlen - maximum length of output string (including terminating
                 '\0')
@OUTPUT     : filename - output string
@RETURNS    : (nothing)
@DESCRIPTION: Routine to convert a string to something that can be used in
              a filename
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void string_to_filename(char *string, char *filename, int maxlen)
{
   int length, isrc, idst;
   int ch;
   int found_first, need_separator;

   /* Get string length */
   length = strlen(string);
   if (length > maxlen-1) length = maxlen - 1;

   /* Loop through characters */
   idst = 0;
   found_first = FALSE;
   need_separator = FALSE;
   for (isrc=0; isrc < length; isrc++) {
      ch = string[isrc];
      if (isalnum(ch)) {
         found_first = TRUE;
         if (need_separator) {
            filename[idst++] = SEPARATOR;
            need_separator = FALSE;
         }
         filename[idst++] = tolower(ch);
      }
      else if (found_first) {
         need_separator = TRUE;
      }
   }

   /* Add terminating '\0' */
   filename[idst++] = '\0';

   return;
}

