/* ----------------------------- MNI Header -----------------------------------
@NAME       : dump_acr_nema.c
@DESCRIPTION: Program to dump the contents of an acr-nema file.
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 24, 1993 (Peter Neelin)
@MODIFIED   : $Log: dump_acr_nema.c,v $
@MODIFIED   : Revision 5.0  1997-08-21 13:25:00  neelin
@MODIFIED   : Release of minc version 0.5
@MODIFIED   :
 * Revision 4.1  1997/07/10  17:14:38  neelin
 * Added more status codes and function to return status string.
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
 * Revision 2.1  1995/02/06  14:12:55  neelin
 * Added argument to specify maximum group id to dump.
 *
 * Revision 2.0  94/09/28  10:36:09  neelin
 * Release of minc version 0.2
 * 
 * Revision 1.6  94/09/28  10:35:53  neelin
 * Pre-release
 * 
 * Revision 1.5  94/05/18  08:48:05  neelin
 * Changed some ACR_OTHER_ERROR's to ACR_ABNORMAL_END_OF_OUTPUT.
 * 
 * Revision 1.4  94/04/07  10:04:58  neelin
 * Added status ACR_ABNORMAL_END_OF_INPUT and changed some ACR_PROTOCOL_ERRORs
 * to that or ACR_OTHER_ERROR.
 * Added #ifdef lint to DEFINE_ELEMENT.
 * 
 * Revision 1.3  93/11/25  10:35:33  neelin
 * Added byte-order test and file free.
 * 
 * Revision 1.2  93/11/24  12:05:00  neelin
 * Write output to stdout instead of stderr.
 * 
 * Revision 1.1  93/11/24  11:25:01  neelin
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
#include <minc_def.h>
#include <acr_nema.h>

int main(int argc, char *argv[])
{
   char *pname;
   FILE *fp;
   Acr_File *afp;
   Acr_Group group_list;
   Acr_Status status;
   char *status_string;
   int maxid;
   char *string, *ptr;

   /* Check arguments */
   pname = argv[0];
   if ((argc > 3) || 
       ((argc == 2) && (strncmp(argv[1], "-h", (size_t) 2) == 0))) {
      (void) fprintf(stderr, "Usage: %s [<file> [<max group>]]\n", pname);
      exit(EXIT_FAILURE);
   }

   /* Open input file */
   if ((argc >= 2) && (strcmp(argv[1],"-") != 0)) {
      fp = fopen(argv[1], "r");
      if (fp == NULL) {
         (void) fprintf(stderr, "%s: Error opening file %s\n",
                        pname, argv[1]);
         exit(EXIT_FAILURE);
      }
   }
   else {
      fp = stdin;
   }

   /* Look for max group id */
   if (argc >= 3) {
      string = argv[2];
      maxid = strtol(string, &ptr, 0);
      if (ptr == string) {
         (void) fprintf(stderr, "%s: Error in max group id (%s)\n", 
                        pname, string);
         exit(EXIT_FAILURE);
      }
   }
   else {
      maxid = 0;
   }

   /* Connect to input stream */
   afp=acr_file_initialize(fp, 0, acr_stdio_read);
   (void) acr_test_byte_order(afp);

   /* Read in group list */
   status = acr_input_group_list(afp, &group_list, maxid);

   /* Free the afp */
   acr_file_free(afp);

   /* Dump the values */
   acr_dump_group_list(stdout, group_list);

   /* Print status information */
   if ((status != ACR_END_OF_INPUT) && (status != ACR_OK)) {
      status_string = acr_status_string(status);
      (void) fprintf(stderr, "Finished with status '%s'\n", status_string);
   }

   exit(EXIT_SUCCESS);
}
