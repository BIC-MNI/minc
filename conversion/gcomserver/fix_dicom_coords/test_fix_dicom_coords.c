/* ----------------------------- MNI Header -----------------------------------
@NAME       : test_fix_dicom_coords.c
@INPUT      : argc, argv - name of one dicom stream file to read and fix
@OUTPUT     : Prints new (0x20, 0x32) and (0x20, 0x37) and (0x20, 0x1041) 
              values
@DESCRIPTION: Test program for fix_dicom_coords function.
@CREATED    : April 9, 2001 (Peter Neelin)
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
DEFINE_ELEMENT(static, ACR_Image_Number            , 0x0020, 0x0013, IS);
DEFINE_ELEMENT(static, SPI_Field_of_view           , 0x0019, 0x1000, DS);
DEFINE_ELEMENT(static, SPI_Angulation_of_cc_axis   , 0x0019, 0x1005, DS);
DEFINE_ELEMENT(static, SPI_Angulation_of_ap_axis   , 0x0019, 0x1006, DS);
DEFINE_ELEMENT(static, SPI_Angulation_of_lr_axis   , 0x0019, 0x1007, DS);
DEFINE_ELEMENT(static, SPI_Slice_orientation       , 0x0019, 0x100a, DS);
DEFINE_ELEMENT(static, SPI_Off_center_lr           , 0x0019, 0x100b, DS);
DEFINE_ELEMENT(static, SPI_Off_center_cc           , 0x0019, 0x100c, DS);
DEFINE_ELEMENT(static, SPI_Off_center_ap           , 0x0019, 0x100d, DS);

/* Function prototypes */
void calculate_dicom_coords(int orientation,
                            double angulation_lr, double angulation_ap,
                            double angulation_cc,
                            double off_centre_lr, double off_centre_ap,
                            double off_centre_cc,
                            double field_of_view, 
                            double position[],
                            double row_dircos[], double col_dircos[],
                            double *location);
int convert_modified_imagenum(char *string);


/* MAIN PROGRAM */
int main(int argc, char *argv[])
{
   char *filename, *pname;
   FILE *fp;
   Acr_File *afp;
   Acr_Group group_list;
   double position[3], row_dircos[3], col_dircos[3], location;
   double angulation_lr, angulation_ap, angulation_cc;
   double off_centre_lr, off_centre_ap, off_centre_cc;
   double field_of_view;
   int orientation;
   char imagenum[128];
   Acr_Status status;

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
   (void) acr_test_dicom_file(afp);

   /* Read in group list up to group */
   status = acr_input_group_list(afp, &group_list, ACR_Image_Number->group_id);
   if (status != ACR_OK) {
      (void) fprintf(stderr, "Error reading input: %s\n",
                     acr_status_string(status));
      exit(EXIT_FAILURE);
   }

   /* Free the afp and close the input */
   acr_file_free(afp);
   (void) fclose(fp);

   /* Get the important numbers */
   orientation = acr_find_int(group_list, SPI_Slice_orientation, 0);
   angulation_lr = acr_find_double(group_list, SPI_Angulation_of_lr_axis, 0.0);
   angulation_ap = acr_find_double(group_list, SPI_Angulation_of_ap_axis, 0.0);
   angulation_cc = acr_find_double(group_list, SPI_Angulation_of_cc_axis, 0.0);
   off_centre_lr = acr_find_double(group_list, SPI_Off_center_lr, 0.0);
   off_centre_ap = acr_find_double(group_list, SPI_Off_center_ap, 0.0);
   off_centre_cc = acr_find_double(group_list, SPI_Off_center_cc, 0.0);
   field_of_view = acr_find_double(group_list, SPI_Field_of_view, 0.0);

   /* Fix the values */
   calculate_dicom_coords(orientation, 
                          angulation_lr, angulation_ap, angulation_cc, 
                          off_centre_lr, off_centre_ap, off_centre_cc, 
                          field_of_view,
                          position, row_dircos, col_dircos, &location);

   /* Print the results */
   (void) printf("%.8g\\%.8g\\%.8g\t%.8g\\%.8g\\%.8g\\%.8g\\%.8g\\%.8g\t%.8g\n",
                 position[0], position[1], position[2],
                 row_dircos[0], row_dircos[1], row_dircos[2],
                 col_dircos[0], col_dircos[1], col_dircos[2],
                 location);

   /* Fix the image num string */
   (void) strcpy(imagenum, acr_find_string(group_list, ACR_Image_Number, ""));
   (void) convert_modified_imagenum(imagenum);
   (void) printf("\"%s\"\n", imagenum);

   return EXIT_SUCCESS;
}
