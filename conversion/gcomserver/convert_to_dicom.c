/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_to_dicom.c
@DESCRIPTION: Routines to convert GYROCOM images and convert them to DICOM.
@GLOBALS    : 
@CREATED    : September 12, 1997 (Peter Neelin)
@MODIFIED   : 
 * $Log: convert_to_dicom.c,v $
 * Revision 1.13  2001-02-19 22:03:13  neelin
 * Port to linux.
 *
 * Revision 1.12  2000/10/31 00:53:13  neelin
 * Changed largest and smallest pixel values to largest and smallest
 * pixel values in series.
 *
 * Revision 1.11  2000/10/30 22:03:50  neelin
 * Modifications to set correct direction cosines and position.
 *
 * Revision 1.10  2000/06/14 18:24:07  neelin
 * Added UseSafeOrientations keyword to project files to allow forcing of
 * direction cosines to standard (safe) ones, and modified convert_to_dicom
 * so that this is no longer the default behaviour.
 *
 * Revision 1.9  2000/02/21 23:48:13  neelin
 * More changes to improve dicom conformance for MNH PACS system.
 * Allow UID prefix to be defined in project file. Define SOP instance UID in
 * addition to study and series instance UIDs and frame-of-reference UID and
 * make sure that these are all the constant for the same image data.
 * Set series number from acquisition number.
 * Set study description from part of comment field.
 *
 * Revision 1.8  2000/02/09 23:53:52  neelin
 * Added code to create the series description using the orientation and the
 * scanning sequence.
 *
 * Revision 1.7  2000/02/03 13:41:32  neelin
 * Modified Study, Series and Frame-of-reference UIDs. The first has the
 * form <prefix>.1.<study>, series has the form <prefix>.2.<study>.<series>.<echo>
 * and f-o-r has the form <prefix>.3.<study>.<series>. The echo is preserved
 * in the series UID for the sake of the Picker viewing stations. The frame-of-
 * reference UID contains the series since the coordinates are not necessarily
 * consistent between acquisitions. The <prefix> is the ASCII (decimal)
 * representation of 'I'.'P' followed by the IP address, and then the process
 * id.
 *
 * Revision 1.6  2000/01/31 14:06:44  neelin
 * Truncate image number if it is longer than the DICOM standard allows.
 *
 * Revision 1.5  2000/01/13 19:04:41  neelin
 * Added accession number to DICOM message, taken from patient comment field.
 *
 * Revision 1.4  1999/10/29 17:52:02  neelin
 * Fixed Log keyword
 *
 * Revision 1.3  1997/10/21 00:06:53  neelin
 * Fixed coordinate stuff.
 *
 * Revision 1.2  1997/09/17  13:15:54  neelin
 * Changes to unpack packed images and to get coordinate directions right.
 *
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
static char rcsid[]="$Header: /private-cvsroot/minc/conversion/gcomserver/convert_to_dicom.c,v 1.13 2001-02-19 22:03:13 neelin Exp $";
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

#define MAKE_POSITIVE_ZEROS

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
DEFINE_ELEMENT(static, ACR_SOP_instance_UID           , 0x0008, 0x0018, UI);
DEFINE_ELEMENT(static, ACR_Accession_number           , 0x0008, 0x0050, SH);
DEFINE_ELEMENT(static, ACR_Study_description          , 0x0008, 0x1030, LO);
DEFINE_ELEMENT(static, ACR_Series_description         , 0x0008, 0x103e, LO);
DEFINE_ELEMENT(static, ACR_Patient_comments           , 0x0010, 0x4000, SH);
DEFINE_ELEMENT(static, ACR_Sequence_variant           , 0x0018, 0x0021, CS);
DEFINE_ELEMENT(static, ACR_Image_Number               , 0x0020, 0x0013, IS);
DEFINE_ELEMENT(static, ACR_Study_instance_UID         , 0x0020, 0x000d, UI);
DEFINE_ELEMENT(static, ACR_Series_instance_UID        , 0x0020, 0x000e, UI);
DEFINE_ELEMENT(static, ACR_Image_position             , 0x0020, 0x0032, DS);
DEFINE_ELEMENT(static, ACR_Image_orientation          , 0x0020, 0x0037, DS);
DEFINE_ELEMENT(static, ACR_Frame_of_reference_UID     , 0x0020, 0x0052, UI);
DEFINE_ELEMENT(static, ACR_Slice_location             , 0x0020, 0x1041, DS);

/* Dicom constants */
#define ACR_MAX_IS_LEN 12

/* Function prototypes */
private void convert_image(Acr_Group *group_list);
private void convert_date(char *string);
private void convert_time(char *string);
private void convert_coordinate(double coord[WORLD_NDIMS]);
private void get_direction_cosines(int orientation,
                                   double angulation_ap, double angulation_lr,
                                   double angulation_cc, 
                                   int use_safe_orientations,
                                   double dircos[WORLD_NDIMS][WORLD_NDIMS]);
private void calculate_image_position(int orientation,
                                      double row_fov,
                                      double column_fov,
                                      double centre[WORLD_NDIMS],
                                      double dircos[WORLD_NDIMS][WORLD_NDIMS],
                                      double position[WORLD_NDIMS]);
private void calculate_slice_location(int orientation,
                                      double position[WORLD_NDIMS],
                                      double dircos[WORLD_NDIMS][WORLD_NDIMS],
                                      double *location);

/* Acr-nema elements to be removed */
DEFINE_ELEMENT(static, ACR_Recognition_Code_Ret       , 0x0008, 0x0010, SH);
DEFINE_ELEMENT(static, ACR_Image_Position_Ret         , 0x0020, 0x0030, DS);
DEFINE_ELEMENT(static, ACR_Image_Orientation_Ret      , 0x0020, 0x0035, DS);
DEFINE_ELEMENT(static, ACR_Location_Ret               , 0x0020, 0x0050, DS);
DEFINE_ELEMENT(static, ACR_Modified_Image_ID          , 0x0020, 0x3402, LO);
DEFINE_ELEMENT(static, ACR_Modified_Image_Date        , 0x0020, 0x3403, LO);
DEFINE_ELEMENT(static, ACR_Modified_Image_Time        , 0x0020, 0x3405, LO);
DEFINE_ELEMENT(static, ACR_Image_Dimensions           , 0x0028, 0x0005, US);
DEFINE_ELEMENT(static, ACR_Compression_Code           , 0x0028, 0x0060, SH);
DEFINE_ELEMENT(static, ACR_Image_Location             , 0x0028, 0x0200, US);

/* Retired elements to remove from group list */
static Acr_Element_Id *Elements_to_remove[] = {
   &ACR_Recognition_Code_Ret, 
   &ACR_Data_set_type,
   &ACR_Data_set_subtype,
   &ACR_Image_Position_Ret, 
   &ACR_Image_Orientation_Ret, 
   &ACR_Location_Ret,
   &ACR_Modified_Image_ID,
   &ACR_Modified_Image_Date,
   &ACR_Modified_Image_Time,
   &ACR_Image_Dimensions,
   &ACR_Compression_Code,
   &ACR_Image_Location,
   NULL
};


#ifdef MAKE_POSITIVE_ZEROS
/* ----------------------------- MNI Header -----------------------------------
@NAME       : make_positive_zeros
@INPUT      : values
              nvalues
@OUTPUT     : values
@RETURNS    : (nothing)
@DESCRIPTION: Ensures that zeros are positive (needed for linux).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 29, 2001 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void make_positive_zeros(double values[], int nvalues)
{
   int i;

   for (i=0; i < nvalues; i++) {
      if (values[i] == 0.0) values[i] = 0.0;
   }
}
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_to_dicom
@INPUT      : group_list
              uid_prefix
              use_safe_orientations - TRUE if dir cos should be realigned
                 to safe values for machines that do not handle angled axes
@OUTPUT     : (nothing)
@RETURNS    : output message.
@DESCRIPTION: Convert a group list so that its elements are dicom-conformant.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June 13, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void convert_to_dicom(Acr_Group group_list, char *uid_prefix,
                             int use_safe_orientations)
{
   Acr_Element element;
   Acr_Group group;
   double value;
   char string[256], *ptr, *imagenum;
   char comment[256];
   double dircos[WORLD_NDIMS][WORLD_NDIMS];
   union {
      unsigned char ch[4];
      int ul;
   } host_id;
   int study, series, echo;
   int orientation;
   int row_world, column_world;
   int index;
   double field_of_view, centre[WORLD_NDIMS], position[WORLD_NDIMS];
   double location;

   /* Set up uid prefix: strip off any trailing blanks or . and then either
      copy the string or make one up if needed. */
   index = strlen(uid_prefix)-1;
   while ((index >= 0) && 
          ((uid_prefix[index] == ' ') || (uid_prefix[index] == '.'))) {
      index--;
   }
   index++;
   uid_prefix[index] = '\0';
   if (index > 0) {
      (void) strcpy(string, uid_prefix);
   }
   else {
      host_id.ul = gethostid();
      (void) sprintf(string, "1.%d.%d.%d.%d.%d.%d.%d", 
                     (int) 'I',(int) 'P',
                     (int) host_id.ch[0], (int) host_id.ch[1], 
                     (int) host_id.ch[2], (int) host_id.ch[3], (int) getpid());
   }
   ptr = &string[strlen(string)];

   /* Get the image number. Check that it is not too long - just chop it off
      if it is. */
   imagenum = acr_find_string(group_list, ACR_Image_Number, NULL);
   if ((imagenum != NULL) && (strlen(imagenum) > (size_t) ACR_MAX_IS_LEN)) {
      imagenum[ACR_MAX_IS_LEN] = '\0';
      acr_insert_string(&group_list, ACR_Image_Number, imagenum);
      imagenum = acr_find_string(group_list, ACR_Image_Number, NULL);
   }
   if (imagenum == NULL) imagenum = "1";

   /* Set study, series and frame of reference UID's.
      Note that the series UID includes echo for the sake of viewing
      stations (notably Picker) than have no notion of echo and try
      to amalgamate the whole thing into a single volume and get upset
      with the duplicate slice positions. */
   study = acr_find_int(group_list, ACR_Study, 0);
   series = acr_find_int(group_list, ACR_Acquisition, 0);
   echo = acr_find_int(group_list, SPI_Echo_number, 1);
   if (study < 1) study = 1;
   if (series < 1) series = 1;
   if (echo < 1) echo = 1;
   (void) sprintf(ptr, ".1.%d", study);
   acr_insert_string(&group_list, ACR_Study_instance_UID, string);
   (void) sprintf(ptr, ".2.%d.%d.%d", study, series, echo);
   acr_insert_string(&group_list, ACR_Series_instance_UID, string);
   (void) sprintf(ptr, ".3.%d", study);
   acr_insert_string(&group_list, ACR_Frame_of_reference_UID, string);
   (void) sprintf(ptr, ".4.%d.%d.%s", study, series, imagenum);
   acr_insert_string(&group_list, ACR_SOP_instance_UID, string);

   /* Fix the series number */
   acr_insert_numeric(&group_list, ACR_Series, (double) series);
   
   /* Update dates and times */
   convert_date(acr_find_string(group_list, ACR_Study_date, NULL));
   convert_date(acr_find_string(group_list, ACR_Patient_birth_date, NULL));
   convert_time(acr_find_string(group_list, ACR_Study_time, NULL));

   /* Make up some essential data */
   acr_insert_string(&group_list, ACR_Image_type, 
                     "ORIGINAL\\PRIMARY\\UNDEFINED");
   acr_insert_string(&group_list, ACR_Sequence_variant, 
                     "NONE\\NONE");

   /* Put in the accession number (hospital patient id taken from comment
      field at MNI) */
   acr_insert_string(&group_list, ACR_Accession_number,
                     acr_find_string(group_list, ACR_Patient_comments, ""));

   /* Get the comment field and use it to set the study descriptions */
   (void) strncpy(comment, 
                  acr_find_string(group_list, ACR_Acq_comments, ""),
                  sizeof(comment));
   comment[sizeof(comment)-1] = '\0';
   if (strlen(comment) >= (size_t) 10) {
      ptr = &comment[9];
      while (*ptr == ' ') {ptr--;}
      ptr++;
      *ptr = '\0';
      acr_insert_string(&group_list, ACR_Study_description, comment);
   }

   /* Make up a series description from orientation, scanning sequence */
   if (strlen(acr_find_string(group_list, ACR_Series_description, "")) == 0) {
      orientation = acr_find_int(group_list, SPI_Slice_orientation, 0);
      switch (orientation) {
         case SPI_SAGITTAL_ORIENTATION:
            ptr = "SAGITTAL"; break;
         case SPI_CORONAL_ORIENTATION:
            ptr = "CORONAL"; break;
         case SPI_TRANSVERSE_ORIENTATION:
         default:
            ptr = "TRANSVERSE"; break;
      }
      (void) sprintf(string, "%s %s", ptr, 
                     acr_find_string(group_list, ACR_Scanning_sequence, ""));
      acr_insert_string(&group_list, ACR_Series_description, string);
   }

   /* Get image orientation */
   orientation = acr_find_int(group_list, SPI_Slice_orientation, 0);
   get_direction_cosines(orientation, 
      acr_find_double(group_list, SPI_Angulation_of_ap_axis, 0.0),
      acr_find_double(group_list, SPI_Angulation_of_lr_axis, 0.0),
      acr_find_double(group_list, SPI_Angulation_of_cc_axis, 0.0),
                         use_safe_orientations,
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
#ifdef MAKE_POSITIVE_ZEROS
   make_positive_zeros((double *) dircos, WORLD_NDIMS * WORLD_NDIMS);
#endif
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
   field_of_view = acr_find_double(group_list, SPI_Field_of_view, 0);
   calculate_image_position(orientation, field_of_view, field_of_view, 
                            centre, dircos, position);
#ifdef MAKE_POSITIVE_ZEROS
   make_positive_zeros(position, WORLD_NDIMS);
#endif
   (void) sprintf(string, "%.8g\\%.8g\\%.8g",
                  position[XCOORD], position[YCOORD], position[ZCOORD]);
   acr_insert_string(&group_list, ACR_Image_position, string);

   /* Add slice location */
   calculate_slice_location(orientation, position, dircos, &location);
#ifdef MAKE_POSITIVE_ZEROS
   make_positive_zeros(&location, 1);
#endif
   (void) sprintf(string, "%.8g", location);
   acr_insert_string(&group_list, ACR_Slice_location, string);

   /* Fix pixel size field */
   element = acr_find_group_element(group_list, ACR_Pixel_size);
   if (element != NULL) {
      if (acr_get_element_numeric_array(element, 1, &value) >= 1) {
#ifdef MAKE_POSITIVE_ZEROS
	 make_positive_zeros(&value, 1);
#endif
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

   /* Rename smallest and largest pixel value elements */
   element = acr_find_group_element(group_list, ACR_Smallest_pixel_value);
   if (element != NULL) {
      acr_set_element_id(element, 
                         ACR_Smallest_pixel_value_in_series->group_id,
                         ACR_Smallest_pixel_value_in_series->element_id);
   }
   element = acr_find_group_element(group_list, ACR_Largest_pixel_value);
   if (element != NULL) {
      acr_set_element_id(element, 
                         ACR_Largest_pixel_value_in_series->group_id,
                         ACR_Largest_pixel_value_in_series->element_id);
   }

   /* Remove some retired elements */
   for (index=0; Elements_to_remove[index] != NULL; index++) {
      group = acr_find_group(group_list, 
                             (*Elements_to_remove[index])->group_id);
      if (group == NULL) continue;
      acr_group_remove_element(group, 
                               (*Elements_to_remove[index])->element_id);
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
@DESCRIPTION: Routine to convert coordinates to and from DICOM.
@METHOD     : Since this operation is its own inverse, we do not provide a
              flag to indicate which direction we are going. If this
              ever changes, then the interface will have to be changed.
              (It would have been a good idea to begin with, but we all 
              know how code evolves...)
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
              use_safe_orientations - TRUE if dir cos should be realigned
                 to safe values for machines that do not handle angled axes
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
                                   int use_safe_orientations,
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

   /* Kludge to handle the fact the the Picker software does not seem
      to handle rotated volumes properly */
   if (use_safe_orientations) {
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

   }       /* End if use_safe_orientations */

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : calculate_image_position
@INPUT      : orientation - indicates orientation of slice
              row_fov - field-of-view for rows
              column_fov - field-of-view for columns
              centre - coordinate of centre of slice as a rotated Philips
                 coordinate
              dircos - direction cosines for axes
@OUTPUT     : position - calculated position coordinate for slice
@RETURNS    : (nothing)
@DESCRIPTION: Routine to calculate the position (top left) coordinate for 
              a slice given its centre, field-of-view and direction cosines.
@METHOD     : Direction cosines are converted back to Philips coordinate
              convention before being used to rotate the centre. The final
              position is converted to DICOM convention at the end.
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
   double row_offset, column_offset;
   double biggest, flip;
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

   /* Work out positive direction cosines and direction of row and column
      offsets. */
   for (iloop=0; iloop < WORLD_NDIMS; iloop++) {

      /* Switch the direction cosine back to Philips orientation */
      for (jloop=0; jloop < WORLD_NDIMS; jloop++) {
         pos_dircos[iloop][jloop] = dircos[iloop][jloop];
      }
      convert_coordinate(pos_dircos[iloop]);

      /* Find biggest component and figure out if we need to flip the
         direction cosine */
      biggest = pos_dircos[iloop][0];
      for (jloop=1; jloop < WORLD_NDIMS; jloop++) {
         if (fabs(biggest) < fabs(pos_dircos[iloop][jloop])) {
            biggest = pos_dircos[iloop][jloop];
         }
      }
      flip = ((biggest >= 0.0) ? +1.0 : -1.0);

      /* Make the direction cosine positive */
      for (jloop=0; jloop < WORLD_NDIMS; jloop++) {
         pos_dircos[iloop][jloop] *= flip;
      }

      /* Work out row and column offsets */
      if (iloop == row_world) {
         row_offset = flip * row_fov / 2.0;
      }
      if (iloop == column_world) {
         column_offset = flip * column_fov / 2.0;
      }

   }

   /* Calculate position - note that centres are given in rotated frame */
   coord[slice_world] = centre[slice_world];
   coord[row_world] = centre[row_world] - row_offset;
   coord[column_world] = centre[column_world] - column_offset;

   /* Rotate the coordinate according to the direction cosines */
   for (iloop=0; iloop < WORLD_NDIMS; iloop++) {
      position[iloop] = 0.0;
      for (jloop=0; jloop < WORLD_NDIMS; jloop++) {
         position[iloop] += pos_dircos[jloop][iloop] * coord[jloop];
      }
   }

   /* Convert the coordinate back to DICOM */
   convert_coordinate(position);

}
      
/* ----------------------------- MNI Header -----------------------------------
@NAME       : calculate_slice_location
@INPUT      : orientation - indicates orientation of slice
              position - position of slice
              dircos - direction cosines for axes
@OUTPUT     : location - offset perpendicular to slice 
@RETURNS    : (nothing)
@DESCRIPTION: Routine to calculate the location of the slice along an
              axis perpendicular to it.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : July 11, 2000 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void calculate_slice_location(int orientation,
                                      double position[WORLD_NDIMS],
                                      double dircos[WORLD_NDIMS][WORLD_NDIMS],
                                      double *location)
{
   double distance;
   int index, slice_world;

   /* Figure out which direction cosine to use */
   switch (orientation) {
   case SPI_SAGITTAL_ORIENTATION:
      slice_world = XCOORD;
      break;
   case SPI_CORONAL_ORIENTATION:
      slice_world = YCOORD;
      break;
   case SPI_TRANSVERSE_ORIENTATION:
   default:
      slice_world = ZCOORD;
      break;
   }

   /* Project the position onto the appropriate direction cosine */
   distance = 0.0;
   for (index=0; index < WORLD_NDIMS; index++) {
      distance += dircos[slice_world][index] * position[index];
   }

   /* Check for a negative direction cosine */
   if (dircos[slice_world][slice_world] < 0.0) {
      distance *= -1.0;
   }

   /* Save the result */
   *location = distance;
   
}
