/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_to_dicom.c
@DESCRIPTION: Routines to convert GYROCOM images and convert them to DICOM.
@GLOBALS    : 
@CREATED    : September 12, 1997 (Peter Neelin)
@MODIFIED   : $Log: convert_to_dicom.c,v $
@MODIFIED   : Revision 1.2  1997-09-17 13:15:54  neelin
@MODIFIED   : Changes to unpack packed images and to get coordinate directions right.
@MODIFIED   :
 * Revision 1.1  1997/09/12  23:14:11  neelin
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

#ifndef lint
static char rcsid[]="$Header: /private-cvsroot/minc/conversion/gcomserver/convert_to_dicom.c,v 1.2 1997-09-17 13:15:54 neelin Exp $";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <minc_def.h>
#include <acr_nema.h>
#include <spi_element_defs.h>

#ifndef public
#  define public
#endif
#ifndef private
#  define private static
#endif

#ifndef TRUE
#  define TRUE 1
#endif
#ifndef FALSE
#  define TRUE 0
#endif

/* World dimensions */
typedef enum { XCOORD = 0, YCOORD, ZCOORD, WORLD_NDIMS } World_Index;

/* Dicom definitions */
DEFINE_ELEMENT(static, ACR_Image_type                 , 0x0008, 0x0008, CS);
DEFINE_ELEMENT(static, ACR_Sequence_variant           , 0x0018, 0x0021, CS);
DEFINE_ELEMENT(static, ACR_Study_instance_UID         , 0x0020, 0x000d, UI);
DEFINE_ELEMENT(static, ACR_Series_instance_UID        , 0x0020, 0x000e, UI);
DEFINE_ELEMENT(static, ACR_Image_position             , 0x0020, 0x0032, DS);
DEFINE_ELEMENT(static, ACR_Image_orientation          , 0x0020, 0x0037, DS);
DEFINE_ELEMENT(static, ACR_Frame_of_reference_UID     , 0x0020, 0x0052, UI);

/* Function prototypes */
private void convert_image(Acr_Group *group_list);
private void convert_date(char *string);
private void convert_time(char *string);
private void convert_coordinate(double coord[WORLD_NDIMS]);
private void get_direction_cosines(int orientation,
                                   double angulation_ap, double angulation_lr,
                                   double angulation_cc, 
                                   double dircos[WORLD_NDIMS][WORLD_NDIMS]);
private void calculate_image_position(int orientation,
                                      double row_fov,
                                      double column_fov,
                                      double centre[WORLD_NDIMS],
                                      double dircos[WORLD_NDIMS][WORLD_NDIMS],
                                      double position[WORLD_NDIMS]);

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_to_dicom
@INPUT      : group_list
@OUTPUT     : (nothing)
@RETURNS    : output message.
@DESCRIPTION: Convert a group list so that its elements are dicom-conformant.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June 13, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void convert_to_dicom(Acr_Group group_list)
{
   Acr_Element element;
   double value;
   char string[64], *ptr;
   double dircos[WORLD_NDIMS][WORLD_NDIMS];
   union {
      unsigned char ch[4];
      int ul;
   } host_id;
   int study, series, echo;
   int orientation;
   int row_world, column_world;
   double field_of_view, centre[WORLD_NDIMS], position[WORLD_NDIMS];

   /* Set up uid prefix */
   host_id.ul = gethostid();
   (void) sprintf(string, "1.%d.%d.%d.%d.%d.%d.%d", 
                  (int) 'I',(int) 'P',
                  (int) host_id.ch[0], (int) host_id.ch[1], 
                  (int) host_id.ch[2], (int) host_id.ch[3], (int) getpid());
   ptr = &string[strlen(string)];

   /* Set study, series and frame of reference UID's */
   study = acr_find_int(group_list, ACR_Study, 0);
   series = acr_find_int(group_list, ACR_Acquisition, 0);
   echo = acr_find_int(group_list, SPI_Echo_number, 1);
   if (study < 1) study = 1;
   if (series < 1) series = 1;
   if (echo < 1) echo = 1;
   (void) sprintf(ptr, ".%d.%d.%d.1", study, series, echo);
   acr_insert_string(&group_list, ACR_Study_instance_UID, string);
   (void) sprintf(ptr, ".%d.%d.%d.2", study, series, echo);
   acr_insert_string(&group_list, ACR_Series_instance_UID, string);
   (void) sprintf(ptr, ".%d.%d.%d.3", study, series, echo);
   acr_insert_string(&group_list, ACR_Frame_of_reference_UID, string);
   
   /* Update dates and times */
   convert_date(acr_find_string(group_list, ACR_Study_date, NULL));
   convert_date(acr_find_string(group_list, ACR_Patient_birth_date, NULL));
   convert_time(acr_find_string(group_list, ACR_Study_time, NULL));

   /* Make up some essential data */
   acr_insert_string(&group_list, ACR_Image_type, 
                     "ORIGINAL\\PRIMARY\\UNDEFINED");
   acr_insert_string(&group_list, ACR_Sequence_variant, 
                     "NONE\\NONE");

   /* Get image orientation */
   orientation = acr_find_int(group_list, SPI_Slice_orientation, 0);
   get_direction_cosines(orientation, 
      acr_find_double(group_list, SPI_Angulation_of_ap_axis, 0.0),
      acr_find_double(group_list, SPI_Angulation_of_lr_axis, 0.0),
      acr_find_double(group_list, SPI_Angulation_of_cc_axis, 0.0),
                         dircos);
   switch (orientation) {
   case SPI_SAGITTAL_ORIENTATION:
      row_world = YCOORD;
      column_world = ZCOORD;
      break;
   case SPI_CORONAL_ORIENTATION:
      row_world = XCOORD;
      column_world = ZCOORD;
      break;
   case SPI_TRANSVERSE_ORIENTATION:
   default:
      row_world = XCOORD;
      column_world = YCOORD;
      break;
   }
   (void) sprintf(string, "%.8g\\%.8g\\%.8g\\%.8g\\%.8g\\%.8g",
                  dircos[row_world][XCOORD],
                  dircos[row_world][YCOORD],
                  dircos[row_world][ZCOORD],
                  dircos[column_world][XCOORD],
                  dircos[column_world][YCOORD],
                  dircos[column_world][ZCOORD]);
   acr_insert_string(&group_list, ACR_Image_orientation, string);

   /* Add image position */
   centre[XCOORD] = acr_find_double(group_list, SPI_Off_center_lr, 0.0);
   centre[YCOORD] = acr_find_double(group_list, SPI_Off_center_ap, 0.0);
   centre[ZCOORD] = acr_find_double(group_list, SPI_Off_center_cc, 0.0);
   convert_coordinate(centre);
   field_of_view = acr_find_double(group_list, SPI_Field_of_view, 0);
   calculate_image_position(orientation, field_of_view, field_of_view, 
                            centre, dircos, position);
   (void) sprintf(string, "%.8g\\%.8g\\%.8g",
                  position[XCOORD], position[YCOORD], position[ZCOORD]);
   acr_insert_string(&group_list, ACR_Image_position, string);

   /* Fix pixel size field */
   element = acr_find_group_element(group_list, ACR_Pixel_size);
   if (element != NULL) {
      if (acr_get_element_numeric_array(element, 1, &value) >= 1) {
         (void) sprintf(string, "%.6G\\%.6G", value, value);
         acr_insert_string(&group_list, ACR_Pixel_size, string);
      }
   }

   /* Put in slice separation */
   value = acr_find_double(group_list, SPI_Slice_factor, 1.0);
   if (value == 0.0) value = 1.0;
   value *= acr_find_double(group_list, ACR_Slice_thickness, 0.0);
   if (value != 0.0) {
      acr_insert_numeric(&group_list, ACR_Spacing_between_slices, value);
   }

   /* Put in the echo number and number of echoes */
   value = acr_find_int(group_list, SPI_Echo_number, -1);
   if (value >= 0) {
      acr_insert_numeric(&group_list, ACR_Echo_number, value);
   }
   value = acr_find_int(group_list, SPI_Number_of_echoes, -1);
   if (value >= 0) {
      acr_insert_numeric(&group_list, ACR_Echo_train_length, value);
   }

   /* Fix the image, if necessary */
   convert_image(&group_list);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_image
@INPUT      : group_list
@OUTPUT     : group_list
@RETURNS    : (nothing)
@DESCRIPTION: Routine to unpack the image.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : September 16, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void convert_image(Acr_Group *group_list)
{
   /* Define some constants */
#define PACK_BITS 12
#define PACK_BYTES 3
#define PACK_MASK 0x0F
#define PACK_SHIFT 4

   /* Variables */
   int bits_alloc, bits_stored, nrows, ncolumns;
   long imagepix, ipix;
   Acr_byte_order byte_order;
   int need_byte_flip;
   Acr_Element element;
   unsigned char *packed;
   unsigned char pixel[2][2];
   unsigned char temp_byte;
   unsigned short *image;

   /* Look for image data */
   element = acr_find_group_element(*group_list, ACR_Pixel_data);
   if (element == NULL) return;

   /* Get basic image information */
   bits_alloc = acr_find_short(*group_list, ACR_Bits_allocated, 0);
   bits_stored = acr_find_short(*group_list, ACR_Bits_stored, 0);
   nrows = acr_find_short(*group_list, ACR_Rows, 0);
   ncolumns = acr_find_short(*group_list, ACR_Columns, 0);
   byte_order = acr_get_element_byte_order(element);
   imagepix = nrows * ncolumns;

   /* Look for packed data that we know how to unpack */
   if ((bits_alloc == PACK_BITS) && (bits_stored <= bits_alloc) &&
       ((imagepix % 2) == 0)) {

      /* Check whether we need to flip bytes */
      need_byte_flip = acr_need_invert(byte_order);

      /* Get pointer to data */
      packed = (unsigned char *) acr_get_element_data(element);

      /* Allocate space for new data */
      image = MALLOC(sizeof(*image) * imagepix);

      /* Loop over pixels, extracting data */
      for (ipix=0; ipix < imagepix; ipix+=2) {

         pixel[0][0] = packed[0];
         pixel[0][1] = packed[1] & PACK_MASK;
         pixel[1][0] = (packed[1] >> PACK_SHIFT) |
            ((packed[2] & PACK_MASK) << PACK_SHIFT);
         pixel[1][1] = packed[2] >> PACK_SHIFT;
         if (need_byte_flip) {
            temp_byte = pixel[0][0];
            pixel[0][0] = pixel[0][1];
            pixel[0][1] = temp_byte;
            temp_byte = pixel[1][0];
            pixel[1][0] = pixel[1][1];
            pixel[1][1] = temp_byte;
         }
         image[ipix]   = *((unsigned short *) pixel[0]);
         image[ipix+1] = *((unsigned short *) pixel[1]);
         packed += PACK_BYTES;

      }        /* loop over pixels */

      /* Replace the image */
      element = acr_create_element(ACR_Pixel_data->group_id,
                                   ACR_Pixel_data->element_id,
                                   ACR_Pixel_data->vr_code,
                                   imagepix * sizeof(*image),
                                   (char *) image);
      acr_insert_element_into_group_list(group_list, element);

      /* Replace the number of bits allocated */
      acr_insert_short(group_list, ACR_Bits_allocated, 
                       (short) (sizeof(short) * CHAR_BIT));


   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_date
@INPUT      : string - string to be converted in place
@OUTPUT     : string - modified date string
@RETURNS    : (nothing)
@DESCRIPTION: Routine to convert a date string from gyroscan format to dicom.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June 13, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void convert_date(char *string)
{
   char *in, *out;

   /* Check for NULL */
   if (string == NULL) return;

   /* Loop over string, looking for separators */
   for (in = out = string; *in != '\0'; in++) {
      if (*in != '.') {
         *out = *in;
         out++;
      }
   }

   /* Pad with spaces */
   while (out < in) {
      *out = ' ';
      out++;
   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_time
@INPUT      : string - string to be converted in place
@OUTPUT     : string - modified time string
@RETURNS    : (nothing)
@DESCRIPTION: Routine to convert a time string from gyroscan format to dicom.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June 13, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void convert_time(char *string)
{
   char *in, *out;
   int nsep, nchar;

   /* Check for NULL */
   if (string == NULL) return;

   /* Loop over string, looking for separators */
   nsep = 0;
   nchar = 0;
   for (in = out = string; *in != '\0'; in++) {

      /* Found separator */
      if (*in == ':') {
         nsep++;
         nchar = 0;
      }

      /* Not a separator */
      else {

         /* Add in a . in the seconds field */
         if ((nsep == 2) && (nchar == 2) && (*in != '.')) {
            *out = '.';
            out++;
         }
         *out = *in;
         out++;
         nchar++;
      }
   }

   /* Pad with spaces */
   while (out < in) {
      *out = ' ';
      out++;
   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_coordinate
@INPUT      : coord - coordinate according to old convention
@OUTPUT     : coord - coordinate converted to DICOM convention
@RETURNS    : (nothing)
@DESCRIPTION: Routine to convert coordinates to DICOM
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : October 18, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void convert_coordinate(double coord[WORLD_NDIMS])
{
   coord[XCOORD] *= -1.0;
   coord[YCOORD] *= -1.0;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_direction_cosines
@INPUT      : orientation
              angulation_ap - Angle of rotation about ap (Y) axis
              angulation_lr - Angle of rotation about lr (X) axis
              angulation_cc - Angle of rotation about cc (Z) axis
@OUTPUT     : dircos - array of direction cosines
@RETURNS    : (nothing)
@DESCRIPTION: Routine to compute direction cosines from angles
@METHOD     : The rotation matrices are designed to be pre-multiplied by row 
              vectors. The rows of the final rotation matrix are the 
              direction cosines.
@GLOBALS    : 
@CALLS      : 
@CREATED    : October 18, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void get_direction_cosines(int orientation,
                                   double angulation_ap, double angulation_lr,
                                   double angulation_cc, 
                                   double dircos[WORLD_NDIMS][WORLD_NDIMS])
{
   World_Index iloop, jloop, kloop, axis_loop, axis;
   double rotation[WORLD_NDIMS][WORLD_NDIMS];
   double temp[WORLD_NDIMS][WORLD_NDIMS];
   double angle, sinangle, cosangle;
   /* Array giving order of rotations - cc, lr, ap */
   static World_Index rotation_axis[WORLD_NDIMS]={ZCOORD, XCOORD, YCOORD};

   /* Start with identity matrix */
   for (iloop=0; iloop < WORLD_NDIMS; iloop++) {
      for (jloop=0; jloop < WORLD_NDIMS; jloop++) {
         if (iloop == jloop)
            dircos[iloop][jloop] = 1.0;
         else
            dircos[iloop][jloop] = 0.0;
      }
   }

   /* Loop through angles */
   for (axis_loop=0; axis_loop < WORLD_NDIMS; axis_loop++) {

      /* Get axis */
      axis = rotation_axis[axis_loop];

      /* Get angle */
      switch (axis) {
      case XCOORD: angle = angulation_lr; break;
      case YCOORD: angle = angulation_ap; break;
      case ZCOORD: angle = angulation_cc; break;
      }
      angle = angle / 180.0 * M_PI;
      if (angle == 0.0) {
         cosangle = 1.0;
         sinangle = 0.0;
      }
      else {
         cosangle = cos(angle);
         sinangle = sin(angle);
      }

      /* Build up rotation matrix (make identity, then stuff in sin and cos) */
      for (iloop=0; iloop < WORLD_NDIMS; iloop++) {
         for (jloop=0; jloop < WORLD_NDIMS; jloop++) {
            if (iloop == jloop)
               rotation[iloop][jloop] = 1.0;
            else
               rotation[iloop][jloop] = 0.0;
         }
      }
      rotation[(YCOORD+axis)%WORLD_NDIMS][(YCOORD+axis)%WORLD_NDIMS] 
         = cosangle;
      rotation[(YCOORD+axis)%WORLD_NDIMS][(ZCOORD+axis)%WORLD_NDIMS] 
         = sinangle;
      rotation[(ZCOORD+axis)%WORLD_NDIMS][(YCOORD+axis)%WORLD_NDIMS] 
         = -sinangle;
      rotation[(ZCOORD+axis)%WORLD_NDIMS][(ZCOORD+axis)%WORLD_NDIMS] 
         = cosangle;

      /* Multiply this by the previous matrix and then save the result */
      for (iloop=0; iloop < WORLD_NDIMS; iloop++) {
         for (jloop=0; jloop < WORLD_NDIMS; jloop++) {
            temp[iloop][jloop] = 0.0;
            for (kloop=0; kloop < WORLD_NDIMS; kloop++)
               temp[iloop][jloop] += 
                  (dircos[iloop][kloop] * rotation[kloop][jloop]);
         }
      }
      for (iloop=0; iloop < WORLD_NDIMS; iloop++)
         for (jloop=0; jloop < WORLD_NDIMS; jloop++)
            dircos[iloop][jloop] = temp[iloop][jloop];

   }

   /* Convert the coordinates to the DICOM standard */
   for (iloop=0; iloop < WORLD_NDIMS; iloop++) {
      convert_coordinate(dircos[iloop]);
   }

   /* Invert direction cosines to account for flipped dimensions */
   for (iloop=0; iloop < WORLD_NDIMS; iloop++) {
      if ((iloop == XCOORD) && (orientation == SPI_SAGITTAL_ORIENTATION))
         continue;
      if ((iloop == ZCOORD) && (orientation == SPI_TRANSVERSE_ORIENTATION))
         continue;
      for (jloop=0; jloop < WORLD_NDIMS; jloop++) {
         dircos[iloop][jloop] *= -1.0;
      }
   }

#if 1
   /* Kludge to handle the fact the the Picker software does not seem
      to handle rotated volumes properly */
   {
      double biggest;
      int dimused[WORLD_NDIMS];

      /* Force all direction cosines to be along major axes */

      /* Keep track of axes that have been used (in case of 45 degree 
         rotations) */
      for (iloop=0; iloop < WORLD_NDIMS; iloop++) 
         dimused[iloop] = FALSE;

      /* Loop over all direction cosines */
      for (iloop=0; iloop < WORLD_NDIMS; iloop++) {

         /* Start with the first unused dimension */
         for (kloop=0; dimused[kloop]; kloop++) {}
         biggest = dircos[iloop][kloop];

         /* Loop over all dimensions, looking for the biggest unused one */
         for (jloop=0; jloop < WORLD_NDIMS; jloop++) {
            if ((fabs(biggest) < fabs(dircos[iloop][jloop])) &&
                !dimused[jloop]) {
               kloop = jloop;
               biggest = dircos[iloop][kloop];
            }
            dircos[iloop][jloop] = 0.0;
         }

         /* Set the biggest coordinate to +/- 1 */
         if (biggest > 0.0)
            dircos[iloop][kloop] = 1.0;
         else 
            dircos[iloop][kloop] = -1.0;
         dimused[kloop] = TRUE;

      }       /* End of loop over dimension cosines */
   }
#endif

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : calculate_image_position
@INPUT      : orientation - indicates orientation of slice
              row_fov - field-of-view for rows
              column_fov - field-of-view for columns
              centre - coordinate of centre of slice
              dircos - direction cosines for axes
@OUTPUT     : position - calculated position coordinate for slice
@RETURNS    : (nothing)
@DESCRIPTION: Routine to calculate the position (top left) coordinate for 
              a slice given its centre, field-of-view and direction cosines.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : September 2, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void calculate_image_position(int orientation,
                                      double row_fov,
                                      double column_fov,
                                      double centre[WORLD_NDIMS],
                                      double dircos[WORLD_NDIMS][WORLD_NDIMS],
                                      double position[WORLD_NDIMS])
{
   double coord[WORLD_NDIMS], pos_dircos[WORLD_NDIMS][WORLD_NDIMS];
   double biggest;
   int slice_world, row_world, column_world;
   World_Index iloop, jloop;

   /* Figure out indices */
   switch (orientation) {
   case SPI_SAGITTAL_ORIENTATION:
      slice_world = XCOORD;
      row_world = ZCOORD;
      column_world = YCOORD;
      break;
   case SPI_CORONAL_ORIENTATION:
      slice_world = YCOORD;
      row_world = ZCOORD;
      column_world = XCOORD;
      break;
   case SPI_TRANSVERSE_ORIENTATION:
   default:
      slice_world = ZCOORD;
      row_world = YCOORD;
      column_world = XCOORD;
      break;
   }

   /* Work out positive direction cosines */
   for (iloop=0; iloop < WORLD_NDIMS; iloop++) {
      biggest = dircos[iloop][0];
      for (jloop=1; jloop < WORLD_NDIMS; jloop++) {
         if (fabs(biggest) < fabs(dircos[iloop][jloop])) {
            biggest = dircos[iloop][jloop];
         }
      }
      for (jloop=0; jloop < WORLD_NDIMS; jloop++) {
         if (biggest >= 0.0)
            pos_dircos[iloop][jloop] = dircos[iloop][jloop];
         else
            pos_dircos[iloop][jloop] = -dircos[iloop][jloop];
      }
   }

   /* Calculate position - note that centres are given in rotated frame */
   coord[slice_world] = centre[slice_world];
   coord[row_world] = centre[row_world] - row_fov/2.0;
   coord[column_world] = centre[column_world] - column_fov/2.0;

   /* Rotate the coordinate according to the direction cosines */
   for (iloop=0; iloop < WORLD_NDIMS; iloop++) {
      position[iloop] = 0.0;
      for (jloop=0; jloop < WORLD_NDIMS; jloop++) {
         position[iloop] += pos_dircos[jloop][iloop] * coord[jloop];
      }
   }

}
      
