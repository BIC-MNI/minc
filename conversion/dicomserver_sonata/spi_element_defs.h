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
#define SPI_VISION_AE_TITLE "MRC16505"  /* changed by Leili from OC1SA1MR010000 to MRC16505 */
#define SPI_VISION_IMPLEMENTATION_UID "1.3.12.2.1107.5.2" /* Chaged by leili from 2.16.840.1.113669.2.931128 to 1.3.12.2.1107.5.2 */

/* Element number for images */
#define SPI_IMAGE_ELEMENT 0x0010

/* Define spi constants */
#define SPI_TRANSVERSE_ORIENTATION 1
#define SPI_SAGITTAL_ORIENTATION   2
#define SPI_CORONAL_ORIENTATION    3

/* Element id's for SPI */
GLOBAL_ELEMENT(SPI_Acquisition_columns                , 0x0019, 0x1230, LO);
GLOBAL_ELEMENT(SPI_Reconstruction_columns             , 0x0019, 0x1231, LO);
GLOBAL_ELEMENT(SPI_Sequence_file_name                 , 0x0019, 0x1511, LO);
GLOBAL_ELEMENT(SPI_Image_position                     , 0x0021, 0x1160, DS);
GLOBAL_ELEMENT(SPI_Image_normal                       , 0x0021, 0x1161, DS);
GLOBAL_ELEMENT(SPI_Image_row                          , 0x0021, 0x116a, DS);
GLOBAL_ELEMENT(SPI_Image_column                       , 0x0021, 0x116b, DS);
GLOBAL_ELEMENT(SPI_Number_of_3D_raw_partitions_nominal, 0x0021, 0x1330, IS);
GLOBAL_ELEMENT(SPI_Number_of_3D_image_partitions      , 0x0021, 0x1334, IS);
GLOBAL_ELEMENT(SPI_Actual_3D_partition_number         , 0x0021, 0x1336, IS);
GLOBAL_ELEMENT(SPI_Number_of_slices_nominal           , 0x0021, 0x1340, IS);
GLOBAL_ELEMENT(SPI_Current_slice_number               , 0x0021, 0x1342, IS);
GLOBAL_ELEMENT(SPI_Number_of_echoes                   , 0x0021, 0x1370, IS);

// added by rhoge:
GLOBAL_ELEMENT(SPI_Registration_date     , 0x0011, 0x1110, DA);
GLOBAL_ELEMENT(SPI_Registration_time     , 0x0011, 0x1111, TM);
GLOBAL_ELEMENT(SPI_Parameter_file_name   , 0x0019, 0x1510, CS);
GLOBAL_ELEMENT(SPI_Protocol              , 0x0029, 0x1020, CS);

// (these appear to be public groups, and should be moved 
//  to dicom_element_defs.h ? )
GLOBAL_ELEMENT(SPI_Manufacturer          , 0x0008, 0x0070, CS);
GLOBAL_ELEMENT(SPI_Manufacturer_model    , 0x0008, 0x1090, CS);
GLOBAL_ELEMENT(SPI_Device_serial_number  , 0x0018, 0x1000, CS);
GLOBAL_ELEMENT(SPI_Software_version      , 0x0018, 0x1020, CS);
GLOBAL_ELEMENT(SPI_Receiving_coil        , 0x0018, 0x1250, CS);

GLOBAL_ELEMENT(SPI_Calibration_date                   , 0x0018, 0x1200, DA);
GLOBAL_ELEMENT(SPI_Total_measurement_time_cur         , 0x0019, 0x1211, DS);
GLOBAL_ELEMENT(SPI_Nominal_number_of_fourier_lines    , 0x0019, 0x1220, IS);
GLOBAL_ELEMENT(SPI_Number_of_fourier_lines_current    , 0x0019, 0x1221, IS);
GLOBAL_ELEMENT(SPI_Number_of_fourier_lines_after_zero , 0x0019, 0x1226, IS);
GLOBAL_ELEMENT(SPI_Number_of_3d_raw_part_cur          , 0x0021, 0x1331, IS);
GLOBAL_ELEMENT(SPI_Order_of_slices                    , 0x0021, 0x134f, IS);
GLOBAL_ELEMENT(SPI_First_measured_fourier_line        , 0x0019, 0x1228, IS);
GLOBAL_ELEMENT(SPI_Number_of_prescans                 , 0x0019, 0x1270, IS);
GLOBAL_ELEMENT(SPI_Magnetic_field_strength            , 0x0019, 0x1412, DS);
GLOBAL_ELEMENT(SPI_ADC_voltage                        , 0x0019, 0x1414, DS);
GLOBAL_ELEMENT(SPI_ADC_offset                         , 0x0019, 0x1416, DS);
GLOBAL_ELEMENT(SPI_Transmitter_amplitude              , 0x0019, 0x1420, DS);
GLOBAL_ELEMENT(SPI_Receiver_amplifier_gain            , 0x0019, 0x1451, DS);
GLOBAL_ELEMENT(SPI_Receiver_preamplifier_gain         , 0x0019, 0x1452, DS);
GLOBAL_ELEMENT(SPI_Phase_gradient_amplitude           , 0x0019, 0x1470, DS);
GLOBAL_ELEMENT(SPI_Readout_gradient_amplitude         , 0x0019, 0x1471, DS);
GLOBAL_ELEMENT(SPI_Selection_gradient_amplitude       , 0x0019, 0x1472, DS);
GLOBAL_ELEMENT(SPI_Sequence_file_owner                , 0x0019, 0x1512, CS);
GLOBAL_ELEMENT(SPI_Sequence_description               , 0x0019, 0x1513, CS);
GLOBAL_ELEMENT(SPI_Number_of_slices_cur               , 0x0021, 0x1341, IS);
GLOBAL_ELEMENT(SPI_Window_center                      , 0x0028, 0x1050, IS);
GLOBAL_ELEMENT(SPI_Window_width                       , 0x0028, 0x1051, IS);


