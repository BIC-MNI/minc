/* ----------------------------- MNI Header -----------------------------------
@NAME       : extract_acr_nema.c
@DESCRIPTION: Program to extract an element from an acr-nema file.
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 24, 1993 (Peter Neelin)
@MODIFIED   : 
 * $Log: extract_acr_nema.c,v $
 * Revision 6.6  2004/10/29 13:08:41  rotor
 *  * rewrote Makefile with no dependency on a minc distribution
 *  * removed all references to the abominable minc_def.h
 *  * I should autoconf this really, but this is old code that
 *      is now replaced by Jon Harlaps PERL version..
 *
 * Revision 6.5  2001/11/08 14:17:05  neelin
 * Added acr_test_dicom_file to allow reading of DICOM part 10 format
 * files. This function also calls acr_test_byte_order to set up the stream
 * properly and can be used as a direct replacement for that function.
 * This set of changes does NOT include the ability to write part 10 files.
 *
 * Revision 6.4  2000/05/01 17:54:45  neelin
 * Fixed handling of test for byte order.
 *
 * Revision 6.3  2000/05/01 13:59:55  neelin
 * Added -e option to allow reading data streams with explicit VR.
 *
 * Revision 6.2  2000/04/28 15:02:01  neelin
 * Added more general argument processing (but not with ParseArgv).
 * Added support for ignoring non-fatal protocol errors.
 * Added support for user-specified byte-order.
 *
 * Revision 6.1  1999/10/29 17:51:52  neelin
 * Fixed Log keyword
 *
 * Revision 6.0  1997/09/12 13:23:59  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:25:00  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:01:23  neelin
 * Release of minc version 0.4
 *
 * Revision 3.1  1997/04/21  20:21:09  neelin
 * Updated the library to handle dicom messages.
 *
 * Revision 3.0  1995/05/15  19:32:12  neelin
 * Release of minc version 0.3
 *
 * Revision 2.0  1994/09/28  10:36:12  neelin
 * Release of minc version 0.2
 *
 * Revision 1.4  94/09/28  10:35:54  neelin
 * Pre-release
 * 
 * Revision 1.3  94/03/14  16:13:04  neelin
 * Changed name in header.
 * 
 * Revision 1.2  93/11/25  10:36:05  neelin
 * Added byte-order test and file free.
 * 
 * Revision 1.1  93/11/24  11:25:23  neelin
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <acr_nema.h>

#define UNKNOWN_VR_ENCODING ((Acr_VR_encoding_type) -1)

int main(int argc, char *argv[])
{
   char *pname;
   char *filename = NULL;
   char *grpstr = NULL;
   char *elemstr = NULL;
   int ignore_errors = FALSE;
   Acr_byte_order byte_order = ACR_UNKNOWN_ENDIAN;
   Acr_VR_encoding_type vr_encoding = UNKNOWN_VR_ENCODING;
   FILE *fp;
   Acr_File *afp;
   Acr_Group group_list;
   Acr_Element element;
   long element_length;
   char *ptr;
   Acr_Element_Id element_id;
   int iarg, argcounter;
   char *arg;
   char *usage = 
      "Usage: %s [-h] [-i] [-b] [-l] [-e] [<file>] <group id> <element id>\n";

   /* Check arguments */
   pname = argv[0];
   argcounter = 0;
   for (iarg=1; iarg < argc; iarg++) {
      arg = argv[iarg];
      if ((arg[0] == '-') && (arg[1] != '\0')) {
         if (arg[2] != '\0') {
            (void) fprintf(stderr, "Unrecognized option %s\n", arg);
            exit(EXIT_FAILURE);
         }
         switch (arg[1]) {
         case 'h':
            (void) fprintf(stderr, "Options:\n");
            (void) fprintf(stderr, "   -h:\tPrint this message\n");
            (void) fprintf(stderr, "   -i:\tIgnore protocol errors\n");
            (void) fprintf(stderr, "   -b:\tAssume big-endian data\n");
            (void) fprintf(stderr, "   -l:\tAssume little-endian data\n");
            (void) fprintf(stderr, "   -e:\tAssume explicit VR encoding\n\n");
            (void) fprintf(stderr, usage, pname);
            exit(EXIT_FAILURE);
            break;
         case 'i':
            ignore_errors = TRUE;
            break;
         case 'l':
            byte_order = ACR_LITTLE_ENDIAN;
            break;
         case 'b':
            byte_order = ACR_BIG_ENDIAN;
            break;
         case 'e':
            vr_encoding = ACR_EXPLICIT_VR;
            break;
         default:
            (void) fprintf(stderr, "Unrecognized option %s\n", arg);
            exit(EXIT_FAILURE);
         }
      }
      else {
         switch (argcounter) {
         case 0:
            grpstr = arg; break;
         case 1:
            elemstr = arg; break;
         case 2:
            filename = grpstr;
            grpstr = elemstr;
            elemstr = arg;
            break;
         default:
            (void) fprintf(stderr, usage, pname);
            exit(EXIT_FAILURE);
         }
         argcounter++;
      }
   }

   /* Get element id (group and element) */
   if ((grpstr == NULL) || (elemstr == NULL)) {
      (void) fprintf(stderr, usage, pname);
      exit(EXIT_FAILURE);
   }
   element_id = MALLOC(sizeof(*element_id));
   element_id->group_id = strtol(grpstr, &ptr, 0);
   if (ptr == grpstr) {
      (void) fprintf(stderr, "%s: Error in group id (%s)\n", pname, grpstr);
      exit(EXIT_FAILURE);
   }
   element_id->element_id = strtol(elemstr, &ptr, 0);
   if (ptr == elemstr) {
      (void) fprintf(stderr, "%s: Error in element id (%s)\n", pname, elemstr);
      exit(EXIT_FAILURE);
   }

   /* Open input file */
   if ((filename != NULL) && (strcmp(filename,"-") != 0))  {
      fp = fopen(filename, "r");
      if (fp == NULL) {
         (void) fprintf(stderr, "%s: Error opening file %s\n",
                        pname, filename);
         exit(EXIT_FAILURE);
      }
   }
   else {
      fp = stdin;
   }

   /* Connect to input stream */
   afp=acr_file_initialize(fp, 0, acr_stdio_read);
   acr_set_ignore_errors(afp, ignore_errors);
   (void) acr_test_dicom_file(afp);
   if (byte_order != ACR_UNKNOWN_ENDIAN) {
      acr_set_byte_order(afp, byte_order);
   }
   if (vr_encoding != UNKNOWN_VR_ENCODING) {
      acr_set_vr_encoding(afp, vr_encoding);
   }

   /* Read in group list up to group */
   (void) acr_input_group_list(afp, &group_list, element_id->group_id);

   /* Free the afp */
   acr_file_free(afp);

   /* Look for element */
   element = acr_find_group_element(group_list, element_id);

   /* Print out value of element */
   if (element != NULL) {
      element_length = acr_get_element_length(element);
      if (element_length > 0) {
         (void) fwrite(acr_get_element_data(element), sizeof(char),
                       (size_t) element_length, stdout);
      }
   }

   exit(EXIT_SUCCESS);

}
