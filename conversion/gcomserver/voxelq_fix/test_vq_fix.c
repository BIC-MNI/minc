/* ----------------------------- MNI Header -----------------------------------
@NAME       : test_vq_fix.c
@INPUT      : argc, argv - name of one dicom stream file to read fix
@OUTPUT     : Prints modified (0x20, 0x32) and (0x20, 0x37) values
@DESCRIPTION: Test program for voxelq_fix_coords function.
@CREATED    : March 19, 2001 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */

#ifndef public
#  define public
#  define private static
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <acr_nema.h>

/* Dicom definitions */
DEFINE_ELEMENT(static, ACR_Image_position             , 0x0020, 0x0032, DS);
DEFINE_ELEMENT(static, ACR_Image_orientation          , 0x0020, 0x0037, DS);

/* Function prototypes */
void voxelq_fix_coords(double position[], 
                       double row_dircos[], double col_dircos[]);


/* MAIN PROGRAM */
int main(int argc, char *argv[])
{
   char *filename, *pname;
   FILE *fp;
   Acr_File *afp;
   Acr_Element element;
   Acr_Group group_list;
   double position[3], orientation[6];

   /* Check arguments */
   pname = argv[0];
   if (argc != 2) {
      (void) fprintf(stderr, "Usage: %s <filename>\n", pname);
      return EXIT_FAILURE;
   }
   filename = argv[1];

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
   (void) acr_test_byte_order(afp);

   /* Read in group list up to group */
   (void) acr_input_group_list(afp, &group_list, ACR_Image_position->group_id);

   /* Free the afp and close the input */
   acr_file_free(afp);
   (void) fclose(fp);

   /* Find the position */
   element = acr_find_group_element(group_list, ACR_Image_position);
   if (element == NULL) {
      (void) fprintf(stderr, "Cannot find position\n");
   }
   if (acr_get_element_numeric_array(element, 3, position) != 3) {
      (void) fprintf(stderr, "Wrong number of values in position\n");
   }

   /* Find the orientation */
   element = acr_find_group_element(group_list, ACR_Image_orientation);
   if (element == NULL) {
      (void) fprintf(stderr, "Cannot find orientation\n");
   }
   if (acr_get_element_numeric_array(element, 6, orientation) != 6) {
      (void) fprintf(stderr, "Wrong number of values in orientation\n");
   }

   /* Fix the values */
   voxelq_fix_coords(position, &orientation[0], &orientation[3]);

   /* Print the results */
   (void) printf("%.8g\\%.8g\\%.8g\t%.8g\\%.8g\\%.8g\\%.8g\\%.8g\\%.8g\n",
                 position[0], position[1], position[2],
                 orientation[0], orientation[1], orientation[2],
                 orientation[3], orientation[4], orientation[5]);

   return EXIT_SUCCESS;
}
