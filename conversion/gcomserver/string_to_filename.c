/* ----------------------------- MNI Header -----------------------------------
@NAME       : string_to_filename.c
@DESCRIPTION: Code to convert a string to something that can be used in a 
              file name.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 10, 1993 (Peter Neelin)
@MODIFIED   : $Log: string_to_filename.c,v $
@MODIFIED   : Revision 1.2  1994-09-28 10:35:05  neelin
@MODIFIED   : Pre-release
@MODIFIED   :
 * Revision 1.1  93/12/10  15:35:18  neelin
 * Initial revision
 * 
@COPYRIGHT  :
              Copyright 1993 Peter Neelin, McConnell Brain Imaging Centre, 
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

#include <gcomserver.h>
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

