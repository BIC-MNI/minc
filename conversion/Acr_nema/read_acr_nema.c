/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_acr_nema.c
@DESCRIPTION: Program to read an image from an acr-nema file and write it
              on stdout.
@METHOD     : 
@GLOBALS    : 
@CREATED    : March 14, 1994 (Peter Neelin)
@MODIFIED   : $Log: read_acr_nema.c,v $
@MODIFIED   : Revision 6.0  1997-09-12 13:23:59  neelin
@MODIFIED   : Release of minc version 0.6
@MODIFIED   :
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
 * Revision 2.0  1994/09/28  10:36:19  neelin
 * Release of minc version 0.2
 *
 * Revision 1.3  94/09/28  10:35:55  neelin
 * Pre-release
 * 
 * Revision 1.2  94/04/07  10:05:08  neelin
 * Added status ACR_ABNORMAL_END_OF_INPUT and changed some ACR_PROTOCOL_ERRORs
 * to that or ACR_OTHER_ERROR.
 * Added #ifdef lint to DEFINE_ELEMENT.
 * 
 * Revision 1.1  94/03/14  16:02:49  neelin
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

/* Define some constants */
#define ACR_IMAGE_EID 0x10
#define ACR_IMAGE_GID 0x7fe0

/* Define element id's */
DEFINE_ELEMENT(static, ACR_rows          , 0x0028, 0x0010, US);
DEFINE_ELEMENT(static, ACR_columns       , 0x0028, 0x0011, US);
DEFINE_ELEMENT(static, ACR_bits_allocated, 0x0028, 0x0100, US);
DEFINE_ELEMENT(static, ACR_image_location, 0x0028, 0x0200, US);

int main(int argc, char *argv[])
{
   char *pname, *filename;
   FILE *fp;
   Acr_File *afp;
   Acr_Group group_list;
   Acr_Element element;
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
   (void) acr_test_byte_order(afp);

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
      if (acr_get_element_short_array(element, (long) nrows*ncols, image) != 
          nrows*ncols) {
         (void) fprintf(stderr, "Incorrect image size\n");
      }
      (void) fwrite(image, sizeof(*image),
                    (size_t) nrows*ncols, stdout);
   }

   FREE(element_id);
   FREE(image);

   exit(EXIT_SUCCESS);

}
