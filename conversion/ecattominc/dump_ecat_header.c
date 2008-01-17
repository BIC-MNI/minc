/* ----------------------------- MNI Header -----------------------------------
@NAME       : dump_ecat_header
@INPUT      : argc, argv - command line arguments
@OUTPUT     : (none)
@RETURNS    : error status
@DESCRIPTION: Dump ECAT file header information
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 10, 1996 (Peter Neelin)
@MODIFIED   : 
 * $Log: dump_ecat_header.c,v $
 * Revision 6.4  2008-01-17 02:33:01  rotor
 *  * removed all rcsids
 *  * removed a bunch of ^L's that somehow crept in
 *  * removed old (and outdated) BUGS file
 *
 * Revision 6.3  2008/01/12 19:08:14  stever
 * Add __attribute__ ((unused)) to all rcsid variables.
 *
 * Revision 6.2  2005/01/19 19:46:00  bert
 * Changes from Anthonin Reilhac
 *
 * Revision 6.1  1999/10/29 17:52:00  neelin
 * Fixed Log keyword
 *
 * Revision 6.0  1997/09/12 13:24:22  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:25:21  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:06:04  neelin
 * Release of minc version 0.4
 *
 * Revision 1.1  1996/01/18  14:52:14  neelin
 * Initial revision
 *
@COPYRIGHT  :
              Copyright 1996 Peter Neelin, McConnell Brain Imaging Centre, 
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
#include <errno.h>
#include "ecat_file.h"

/* Main program */

int main(int argc, char *argv[])
{
   Ecat_file *fp;
   char *pname, *filename, *fieldname;
   int index, volume, slice, iheader, ifield;
   Ecat_field_name field;
   char *description;
   int length;
   int error;
   char svalue[ECAT_MAX_STRING_LENGTH];

   /* Check the arguments */
   pname = argv[0];
   if ((argc < 2) || (argc > 6)) {
      (void) fprintf(stderr, 
         "Usage: %s <ecat file> [<field> [<index> [<volume> <slice>]]]\n",
                     pname);
      return EXIT_FAILURE;
   }
   filename = argv[1];
   fieldname = ((argc > 2) ? argv[2] : (char *) NULL);
   index = ((argc > 3) ? atoi(argv[3]) : 0);
   volume = ((argc > 4) ? atoi(argv[4]) : 0);
   slice = ((argc > 5) ? atoi(argv[5]) : 0);
   if ((fieldname != NULL) && (strlen(fieldname) == 0))
      fieldname = NULL;

   /* Open the file */
   fp = ecat_open(filename);
   if (fp == NULL) {
      (void) fprintf(stderr, "%s: Error opening file \"%s\"", 
                     pname, filename);
      if (errno != 0) {
         (void) fprintf(stderr, " - ");
         perror(NULL);
      }
      else {
         (void) fprintf(stderr, "\n");
      }
      exit(EXIT_FAILURE);
   }

   /* Loop over main header then appropriate subheader */
   for (iheader = 0; iheader < 2; iheader++) {

      /* Print out header type */
      if (fieldname == NULL) {
         if (iheader == 0) {
            (void) printf("Main header:\n");
         }
         else {
            (void) printf("Subheader (volume %d, slice %d):\n", volume, slice);
         }
      }

      /* Loop over fields */
      ifield = -1;
      do {
         ifield++;

         /* Get information on the next field */
         if (iheader == 0) {
            field = ecat_list_main(fp, ifield);
            description = ecat_get_main_field_description(fp, field);
            length = ecat_get_main_field_length(fp, field);
            error = ecat_get_main_value(fp, field, index, 
                                        NULL, NULL, svalue);
         }
         else {
            field = ecat_list_subhdr(fp, ifield);
            description = ecat_get_subhdr_field_description(fp, field);
            length = ecat_get_subhdr_field_length(fp, field);
            error = ecat_get_subhdr_value(fp, volume, slice, field, index, 
                                          NULL, NULL, svalue);
         }

         /* Check for the end of list */
         if (field == ECAT_No_Field) continue;

         /* If we are looking for a specific field, check to see if we've
            found it */
         if ((fieldname != NULL) && (description != NULL) &&
             (strcmp(fieldname, description) != 0)) {
            continue;
         }

         /* Look for an error in reading the value */
         if (error) {
            (void) fprintf(stderr, "   Error retrieving field \"%s\".\n",
                           (description == NULL ? "<unknown>" : description));
            continue;
         }

         /* Print out appropriate results */
         if ((fieldname == NULL) || (strcmp(fieldname, description) == 0)) {
            (void) printf("   %s", description);
            if (length > 1)
               (void) printf("[%d/%d]", index, length);
            (void) printf(" = %s\n", svalue);
         }

      } while (field != ECAT_No_Field);

   }     /* End of loop over headers */

   /* Close the file */
   ecat_close(fp);

   return EXIT_SUCCESS;
}


