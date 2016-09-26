/* ----------------------------- MNI Header -----------------------------------
@NAME       : cat_acr_nema.c
@DESCRIPTION: Program to read in an ACR-NEMA file and then dump it out.
              This can fix a buggy file.
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 9, 2000 (Peter Neelin)
@MODIFIED   : 
 * $Log: copy_acr_nema.c,v $
 * Revision 6.5  2004/10/29 13:08:41  rotor
 *  * rewrote Makefile with no dependency on a minc distribution
 *  * removed all references to the abominable minc_def.h
 *  * I should autoconf this really, but this is old code that
 *      is now replaced by Jon Harlaps PERL version..
 *
 * Revision 6.4  2001/11/08 14:17:05  neelin
 * Added acr_test_dicom_file to allow reading of DICOM part 10 format
 * files. This function also calls acr_test_byte_order to set up the stream
 * properly and can be used as a direct replacement for that function.
 * This set of changes does NOT include the ability to write part 10 files.
 *
 * Revision 6.3  2000/11/09 17:00:03  neelin
 * Minor lint fix.
 *
 * Revision 6.2  2000/11/09 16:57:23  neelin
 * Fixed bug in writing to stdout.
 *
 * Revision 6.1  2000/11/09 15:56:49  neelin
 * Added new program copy_acr_nema to read in acr-nema file, ignoring errors,
 * and then write it out again. This will fix any errors in group lengths,
 * etc.
 *
@COPYRIGHT  :
              Copyright 2000 Peter Neelin, McConnell Brain Imaging Centre, 
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
   char *infile = NULL;
   char *outfile = NULL;
   int ignore_errors = TRUE;
   Acr_byte_order byte_order = ACR_UNKNOWN_ENDIAN;
   Acr_VR_encoding_type vr_encoding = UNKNOWN_VR_ENCODING;
   FILE *fp;
   Acr_File *afp;
   Acr_Group group_list, cur_group;
   Acr_Status status, tmpstatus, outstatus;
   char *status_string;
   int iarg, argcounter;
   char *arg;
   char *usage = "Usage: %s [-h] [-b] [-l] [-e] <in> <out>\n";

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
            (void) fprintf(stderr, "   -b:\tAssume big-endian data\n");
            (void) fprintf(stderr, "   -l:\tAssume little-endian data\n");
            (void) fprintf(stderr, "   -e:\tAssume explicit VR encoding\n\n");
            (void) fprintf(stderr, usage, pname);
            exit(EXIT_FAILURE);
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
            infile = arg; break;
         case 1:
            outfile = arg; break;
         default:
            (void) fprintf(stderr, usage, pname);
            exit(EXIT_FAILURE);
         }
         argcounter++;
      }
   }

   /* Open input file */
   if ((infile != NULL) && (strcmp(infile, "-") != 0)) {
      fp = fopen(infile, "r");
      if (fp == NULL) {
         (void) fprintf(stderr, "%s: Error opening file %s\n",
                        pname, infile);
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
   byte_order = acr_get_byte_order(afp);
   if (vr_encoding != UNKNOWN_VR_ENCODING) {
      acr_set_vr_encoding(afp, vr_encoding);
   }
   vr_encoding = acr_get_vr_encoding(afp);

   /* Read in group list */
   status = acr_input_group_list(afp, &group_list, 0);

   /* Free the afp */
   acr_file_free(afp);

   /* Open the output file */
   if ((outfile != NULL) && (strcmp(outfile, "-") != 0)) {
      fp = fopen(outfile, "w");
      if (fp == NULL) {
         (void) fprintf(stderr, "%s: Error opening output file %s\n",
                        pname, outfile);
         exit(EXIT_FAILURE);
      }
   }
   else {
      fp = stdout;
   }


   /* Connect to output stream */
   afp=acr_file_initialize(fp, 0, acr_stdio_write);
   acr_set_ignore_errors(afp, ignore_errors);
   acr_set_byte_order(afp, byte_order);
   acr_set_vr_encoding(afp, vr_encoding);

   /* Write out the groups */
   outstatus = ACR_OK;
   for (cur_group=group_list; cur_group != NULL; 
        cur_group = acr_get_group_next(cur_group)) {

      /* Read in the next group */
      tmpstatus = acr_output_group(afp, cur_group);

      if (tmpstatus != ACR_OK)
         outstatus = tmpstatus;
   }

   /* Free the afp */
   acr_file_free(afp);

   /* Print status information */
   if ((status != ACR_END_OF_INPUT) && (status != ACR_OK)) {
      status_string = acr_status_string(status);
      (void) fprintf(stderr, "Finished with input status '%s'\n", 
                     status_string);
   }
   if (outstatus != ACR_OK) {
      status_string = acr_status_string(outstatus);
      (void) fprintf(stderr, "Finished with output status '%s'\n", 
                     status_string);
   }

   exit(EXIT_SUCCESS);
}
