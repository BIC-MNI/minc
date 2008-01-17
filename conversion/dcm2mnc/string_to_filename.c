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
 * Revision 1.4  2008-01-17 02:33:01  rotor
 *  * removed all rcsids
 *  * removed a bunch of ^L's that somehow crept in
 *  * removed old (and outdated) BUGS file
 *
 * Revision 1.3  2008/01/12 19:08:14  stever
 * Add __attribute__ ((unused)) to all rcsid variables.
 *
 * Revision 1.2  2005/03/03 18:59:16  bert
 * Fix handling of image position so that we work with the older field (0020, 0030) as well as the new (0020, 0032)
 *
 * Revision 1.1  2005/02/17 16:38:11  bert
 * Initial checkin, revised DICOM to MINC converter
 *
 * Revision 1.1.1.1  2003/08/15 19:52:55  leili
 * Leili's dicom server for sonata
 *
 * Revision 1.2  2002/03/22 19:19:36  rhoge
 * Numerous fixes -
 * - handle Numaris 4 Dicom patient name
 * - option to cleanup input files
 * - command option
 * - list-only option
 * - debug mode
 * - user supplied name, idstr
 * - anonymization
 *
 * Revision 1.1.1.1  2000/11/30 02:13:15  rhoge
 * imported sources to CVS repository on amoeba
 *
 * Revision 6.1  1999/10/29 17:52:00  neelin
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

#include "dcm2mnc.h"
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
void string_to_filename(const char *string, char *filename, int maxlen)
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

void string_to_initials(char *string, char *filename, int maxlen)
{
  /* function added by R. Hoge to convert name-like strings to
     initials in environment where confidentiality policy prohibits
     use of names in file-names */

   int length, isrc, idst;
   int ch;
   int first_found, sep_found, multi_word, comma_found, comma_found2, in_word;

   /* Get string length */
   length = strlen(string);
   if (length > maxlen-1) length = maxlen - 1;

   /* do first pass to look for multi-words, commas */

   first_found = FALSE;
   sep_found = FALSE;
   multi_word = FALSE;
   comma_found = FALSE;

   for (isrc=0; isrc < length; isrc++) {
      ch = string[isrc];

      /* if we hit a separator after finding a first alphanum, treat
	 as multi words */

      /* alphanumeric expressions (including underscores, hyphens) are treated 
	 as discrete words - note that we won't print hyphens */

      /* examples of single words:

  	   test5
	   test-5
	   test_5

	 examples of multi words:

	   test 5
	   snr_test 5
	   test,5
	   snr-test 5

       */

      if (sep_found && isalnum(ch)) {
	multi_word = TRUE;
      }
      
      if (first_found && !(isalnum(ch)||ch=='-'||ch=='_')) {
	sep_found = TRUE;
      }

      if (isalnum(ch)) {
	first_found = TRUE;
      }

      // Numaris 4 used caret (^) to separate names (Last^first)
      if (ch == ',' || ch == '^') {
	comma_found = TRUE;
      }
   }

   /* if Patient name is only a single word, then just strip out
      non-alphanumeric characters 

      examples:  

      snrtest1   -> snrtest1
      snrtest-1  -> snrtest1
      snr_test-2 -> snr_test2

      note that hyphens are omitted, because these
      are used as delimiters in filename */

   if (!multi_word) {

     idst = 0;
     for (isrc=0; isrc < length; isrc++) {
       ch = string[isrc];
       
       if (isalnum(ch) || ch=='_') {
         filename[idst++] = tolower(ch);
       }
     }

     /* Add terminating '\0' */
     filename[idst++] = '\0';
     
   } else { /* multiple words */
     
     if (!comma_found) {

       /* examples of multi-word no comma:  

	  john doe        -> jd
	  john edward doe -> jed
	  john doe-smith  -> jds
	  my snr_test     -> mst 
	  john doe 12     -> jd12
	  john12 smith    -> j12s
	  12john smith    -> 12js
	  john doe test2b -> jst2b

	  note that underscores are treated as separators here,
          contiguous digits are all printed, and digits
          are treated as printable separators */

       /* Loop through characters */
       idst = 0;
       in_word = FALSE;
       for (isrc=0; isrc < length; isrc++) {
	 ch = string[isrc];
	 if (isalpha(ch) && !in_word) {
	   in_word = TRUE;
	   filename[idst++] = tolower(ch);
	 }
	 else if (isdigit(ch)) {
	   in_word = FALSE;
	   filename[idst++] = ch;
	 }
	 else if (!isalnum(ch)) {
	   in_word = FALSE;
	 }
       }
       
       /* Add terminating '\0' */
       filename[idst++] = '\0';
     
     } else { /* multiple words with comma separation */

       /* examples of multi-word with comma:  

	  doe, john       -> jd
	  doe,john        -> jd
	  doe-smith, john -> jds

	  note that we treat stuff before the comma as the LAST name */

       /* we do two passes:  all the stuff after the comma THEN 
	  all the stuff before the comma */

       idst = 0;

       /* Loop through characters, writing those after comma*/

       comma_found2 = FALSE;
       in_word = FALSE;
       for (isrc=0; isrc < length; isrc++) {
	 ch = string[isrc];

	 if (isalpha(ch) && !in_word) {
	   in_word = TRUE;
	   if (comma_found2) filename[idst++] = tolower(ch);
	 }
	 else if (isdigit(ch)) {
	   in_word = FALSE;
	   if (comma_found2) filename[idst++] = ch;
	 }
	 else if (!isalnum(ch)) {
	   in_word = FALSE;
	 }

	 // numaris 4 uses Last^First
	 if (ch == ',' || ch == '^') {
	   comma_found2 = TRUE;
	 }
       }

       /* now write characters/initials before the comma*/
       
       comma_found2 = FALSE;
       in_word = FALSE;
       for (isrc=0; isrc < length; isrc++) {
	 ch = string[isrc];

	 if (isalpha(ch) && !in_word) {
	   in_word = TRUE;
	   if (!comma_found2) filename[idst++] = tolower(ch);
	 }
	 else if (isdigit(ch)) {
	   in_word = FALSE;
	   if (!comma_found2) filename[idst++] = ch;
	 }
	 else if (!isalnum(ch)) {
	   in_word = FALSE;
	 }

	 // Numaris 4 uses Last^First
	 if (ch == ',' || ch == '^') {
	   comma_found2 = TRUE;
	 }
       }
       
       /* Add terminating '\0' */
       filename[idst++] = '\0';
     
     }
   }

   return;
}


