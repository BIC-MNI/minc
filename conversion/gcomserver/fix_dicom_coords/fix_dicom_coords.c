/* ----------------------------- MNI Header -----------------------------------
@NAME       : fix_dicom_coords.c
@DESCRIPTION: Routine to corretly set the dicom image coordinate information
              based on the Philips MR shadow fields.
@GLOBALS    : (none)
@CREATED    : April 9, 2001 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef TRUE
#  define TRUE 1
#endif
#ifndef FALSE
#  define FALSE 0
#endif

/* World dimensions */
typedef enum { XCOORD = 0, YCOORD, ZCOORD, WORLD_NDIMS } World_Index;

/* Define spi constants */
#define SPI_TRANSVERSE_ORIENTATION 1
#define SPI_SAGITTAL_ORIENTATION   2
#define SPI_CORONAL_ORIENTATION    3

/* Define other constants */
#define private static

/* Public Function prototypes */
void calculate_dicom_coords(int orientation,
                            double angulation_lr, double angulation_ap,
                            double angulation_cc,
                            double off_centre_lr, double off_centre_ap,
                            double off_centre_cc,
                            double field_of_view, 
                            double position[],
                            double row_dircos[], double col_dircos[],
                            double *location);
int convert_imagenum(char *string);

/* Private function prototypes */
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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_direction_cosines
@INPUT      : orientation - shadow field (0x19, 0x100a)
              angulation_lr - Angle of rotation about lr (X) axis
                 shadow field (0x19, 0x1007)
              angulation_ap - Angle of rotation about ap (Y) axis
                 shadow field (0x19, 0x1006)
              angulation_cc - Angle of rotation about cc (Z) axis
                 shadow field (0x19, 0x1005)
              off_centre_lr - Centre of image along lr (X)
                 shadow field (0x19, 0x100b)
              off_centre_ap - Centre of image along ap (Y)
                 shadow field (0x19, 0x100d)
              off_centre_cc - Centre of image along cc (Z)
                 shadow field (0x19, 0x100c)
              field_of_view - Width of field of view
                 shadow field (0x19, 0x1000)
@OUTPUT     : position - dicom field (0x20, 0x32)
              row_dircos, col_dircos - dicom field (0x20, 0x37)
              location - retired dicom field (0x20, 0x1041)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to calculate dicom image position, orientation (row
              and column direction cosines) and slice location from
              Philips Gyroscan MR shadow fields.
@METHOD     : 
@GLOBALS    : 
@CALLS      : get_direction_cosines
              calculate_image_position
              calculate_slice_location
@CREATED    : April 9, 2001 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
void calculate_dicom_coords(int orientation,
                            double angulation_lr, double angulation_ap,
                            double angulation_cc,
                            double off_centre_lr, double off_centre_ap,
                            double off_centre_cc,
                            double field_of_view, 
                            double position[],
                            double row_dircos[], double col_dircos[],
                            double *location)
{
   double dircos[WORLD_NDIMS][WORLD_NDIMS];
   World_Index row_world, column_world, iworld;
   double centre[3];

   /* 
    * Work out the axis direction cosines
    */
   get_direction_cosines(orientation, 
                         angulation_ap, angulation_lr, angulation_cc,
                         FALSE, dircos);

   /*
    * Map the world axes to the image axes
    */
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
   for (iworld=0; iworld < WORLD_NDIMS; iworld++) {
      row_dircos[iworld] = dircos[row_world][iworld];
      col_dircos[iworld] = dircos[column_world][iworld];
   }

   /*
    * Calculate the dicom image position
    */
   centre[XCOORD] = off_centre_lr;
   centre[YCOORD] = off_centre_ap;
   centre[ZCOORD] = off_centre_cc;
   calculate_image_position(orientation, field_of_view, field_of_view, 
                            centre, dircos, position);

   /*
    * Calculate the slice location.
    */
   calculate_slice_location(orientation, position, dircos, location);
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
@CALLS      : sin, cos, fabs, convert_coordinate
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
@CALLS      : fabs, convert_coordinate
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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_imagenum
@INPUT      : string - string to be converted in place
@OUTPUT     : string - modified date string
@RETURNS    : TRUE if string was modified, FALSE otherwise.
@DESCRIPTION: Routine to convert an image number from gyroscan format to 
              a valid dicom integer by making it smaller, if necessary.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : March 5, 2001 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
int convert_imagenum(char *string)
{
   int istart, iend, iold, inew, oldlen, ipos;
   static Acr_max_IS_len = 8;
   static int chars_to_remove[] = {1,2,5,6,13};
   static int nchars_to_remove = 
      sizeof(chars_to_remove) / sizeof(chars_to_remove[0]);
   int ichar;

   /* Check for NULL string */
   if (string == NULL) return FALSE;

   /* Get the original length of the string */
   oldlen = strlen(string);

   /* Check for zero-length string */
   if (oldlen == 0) return FALSE;

   /* Find the start of the number */
   for (istart=0; (string[istart] != '\0') && 
           (string[istart] < '0' || string[istart] > '9'); istart++) {}

   /* Find the end of the string */
   for (iend=istart; (string[iend] >= '0') && (string[iend] <= '9'); iend++) {}

   /* Does anything need to be fixed? If not, then return. */
   if (iend-istart <= Acr_max_IS_len) {
      return FALSE;
   }

   /* Copy the string */
   inew = 0;
   iold = istart;
   ichar = nchars_to_remove - 1;
   ipos = iend-iold;
   while ((chars_to_remove[ichar] > ipos) && (ichar > 0)) {
      ichar--;
   }
   while (string[iold] >= '0' && string[iold] <= '9') {

      /* Get position from end of string */
      ipos = iend-iold;

      /* Skip any chars to the left of the highest position, and skip
         the specified character positions and skip any leading zeros */
      if ((ipos > chars_to_remove[ichar]) && (ichar == nchars_to_remove-1)) {}
      else if (ipos == chars_to_remove[ichar]) {
         if (ichar > 0) ichar--;
      }
      else if ((inew == 0) && (string[iold] == '0')) {}
      else {

         if (inew != iold)
            string[inew] = string[iold];
         inew++;

      }

      iold++;
   }

   /* Check for an empty string */
   if (inew == 0) {
      string[inew] = '0';
      inew++;
   }

   /* Pad the string with NULs */
   do {
      string[inew] = '\0';
      inew++;
   } while (inew < oldlen);

   return TRUE;
}

