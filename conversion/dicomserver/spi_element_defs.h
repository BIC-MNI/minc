/* ----------------------------- MNI Header -----------------------------------
@NAME       : spi_element_defs.h
@DESCRIPTION: Element definitions for spi
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 23, 1993 (Peter Neelin)
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

/* Define siemens-specific stuff for associations and messages */
#define SPI_VISION_AE_TITLE "OC1SA1MR010000"

/* Element number for images */
#define SPI_IMAGE_ELEMENT 0x0010

/* Define spi constants */
#define SPI_TRANSVERSE_ORIENTATION 1
#define SPI_SAGITTAL_ORIENTATION   2
#define SPI_CORONAL_ORIENTATION    3

/* Element id's for SPI */
GLOBAL_ELEMENT(SPI_Sequence_file_name                 , 0x0019, 0x1511);
GLOBAL_ELEMENT(SPI_Image_position                     , 0x0021, 0x1160);
GLOBAL_ELEMENT(SPI_Image_normal                       , 0x0021, 0x1161);
GLOBAL_ELEMENT(SPI_Image_row                          , 0x0021, 0x116a);
GLOBAL_ELEMENT(SPI_Image_column                       , 0x0021, 0x116b);
GLOBAL_ELEMENT(SPI_Number_of_3D_raw_partitions_nominal, 0x0021, 0x1330);
GLOBAL_ELEMENT(SPI_Actual_3D_partition_number         , 0x0021, 0x1336);
GLOBAL_ELEMENT(SPI_Number_of_slices_nominal           , 0x0021, 0x1340);
GLOBAL_ELEMENT(SPI_Current_slice_number               , 0x0021, 0x1342);
GLOBAL_ELEMENT(SPI_Number_of_echoes                   , 0x0021, 0x1370);

