/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_acr_nema.c
@DESCRIPTION: Program to read an image from an acr-nema file and write it
              on stdout.
@METHOD     : 
@GLOBALS    : 
@CREATED    : March 14, 1994 (Peter Neelin)
@MODIFIED   : $Log: read_acr_nema.c,v $
@MODIFIED   : Revision 1.1  1994-03-14 16:02:49  neelin
@MODIFIED   : Initial revision
@MODIFIED   :
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

/* Define some constants */
#define ACR_IMAGE_EID 0x10
#define ACR_IMAGE_GID 0x7fe0

/* Define element id's */
DEFINE_ELEMENT(static, ACR_rows          , 0x0028, 0x0010);
DEFINE_ELEMENT(static, ACR_columns       , 0x0028, 0x0011);
DEFINE_ELEMENT(static, ACR_bits_allocated, 0x0028, 0x0100);
DEFINE_ELEMENT(static, ACR_image_location, 0x0028, 0x0200);

int main(int argc, char *argv[])
{
   char *pname, *filename;
   FILE *fp;
   Acr_File *afp;
   Acr_Group group_list;
   Acr_Element element;
   long element_length;
   Acr_Element_Id element_id;
   unsigned short *image;
   int nrows, ncols, bits_alloc;

   /* Check arguments */
   pname = argv[0];
   if ((argc > 2) || (argc < 1)) {
      (void) fprintf(stderr, "Usage: %s [<file>]\n", pname);
      exit(EXIT_FAILURE);
   }

   /* Get file name */
   if (argc == 2)
      filename = argv[1];
   else 
      filename = NULL;

   /* Open input file */
   if (filename != NULL) {
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
   (void) acr_test_byte_ordering(afp);

   /* Read in group list up to group */
   (void) acr_input_group_list(afp, &group_list, 0);

   /* Free the afp */
   acr_file_free(afp);

   /* Get image size and location */
   nrows = acr_find_short(group_list, ACR_rows, 0);
   ncols = acr_find_short(group_list, ACR_columns, 0);
   bits_alloc = acr_find_short(group_list, ACR_bits_allocated, 0);
   if ((nrows <= 0) || (ncols <= 0)) {
      (void) fprintf(stderr, "%s: Invalid imaged size %d x %d\n",
                     pname, nrows, ncols);
      exit(EXIT_FAILURE);
   }
   if (bits_alloc != 16) {
      (void) fprintf(stderr, "%s: Pixels not stored as short ints\n",
                     pname);
      exit(EXIT_FAILURE);
   }
   element_id = MALLOC(sizeof(*element_id));
   element_id->element_id = ACR_IMAGE_EID;
   element_id->group_id = acr_find_short(group_list, ACR_image_location,
                                         ACR_IMAGE_GID);
   

   /* Look for image and print it out */
   image = (unsigned short *) MALLOC(nrows*ncols*sizeof(*image));
   element = acr_find_group_element(group_list, element_id);
   if (element != NULL) {
      acr_get_short(nrows*ncols, (void *) acr_get_element_data(element),
                    image);
      (void) fwrite(image, sizeof(*image),
                    (size_t) nrows*ncols, stdout);
   }

   FREE(element_id);
   FREE(image);

   exit(EXIT_SUCCESS);

}
