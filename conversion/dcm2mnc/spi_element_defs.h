/* ----------------------------- MNI Header -----------------------------------
@NAME       : spi_element_defs.h
@DESCRIPTION: Element definitions for Siemens "Standard Product Interconnect" 
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

/* Element number for images */
#define SPI_IMAGE_ELEMENT 0x0010

/* Element id's for SPI */
GLOBAL_ELEMENT(SPI_Net_frequency                      , 0x0019, 0x1010, IS);
GLOBAL_ELEMENT(SPI_Measurement_mode                   , 0x0019, 0x1020, CS);
GLOBAL_ELEMENT(SPI_Calculation_mode                   , 0x0019, 0x1030, CS);
GLOBAL_ELEMENT(SPI_Noise_level                        , 0x0019, 0x1050, IS);
GLOBAL_ELEMENT(SPI_Number_of_data_bytes               , 0x0019, 0x1060, IS);
GLOBAL_ELEMENT(SPI_Total_measurement_time_nom         , 0x0019, 0x1210, DS);
GLOBAL_ELEMENT(SPI_Total_measurement_time_cur         , 0x0019, 0x1211, DS);
GLOBAL_ELEMENT(SPI_Start_delay_time                   , 0x0019, 0x1212, DS);
GLOBAL_ELEMENT(SPI_Dwell_time                         , 0x0019, 0x1213, DS);
GLOBAL_ELEMENT(SPI_Number_of_phases                   , 0x0019, 0x1214, IS);
GLOBAL_ELEMENT(SPI_Sequence_control_mask              , 0x0019, 0x1216, UL);
GLOBAL_ELEMENT(SPI_Measurement_status_mask            , 0x0019, 0x1218, UL);
GLOBAL_ELEMENT(SPI_Number_of_fourier_lines_nom        , 0x0019, 0x1220, IS);
GLOBAL_ELEMENT(SPI_Number_of_fourier_lines_current    , 0x0019, 0x1221, IS);
GLOBAL_ELEMENT(SPI_Number_of_fourier_lines_after_zero , 0x0019, 0x1226, IS);
GLOBAL_ELEMENT(SPI_First_measured_fourier_line        , 0x0019, 0x1228, IS);
GLOBAL_ELEMENT(SPI_Acquisition_columns                , 0x0019, 0x1230, LO);
GLOBAL_ELEMENT(SPI_Reconstruction_columns             , 0x0019, 0x1231, LO);
GLOBAL_ELEMENT(SPI_Number_of_averages                 , 0x0019, 0x1250, IS);
GLOBAL_ELEMENT(SPI_Flip_angle                         , 0x0019, 0x1260, DS);
GLOBAL_ELEMENT(SPI_Number_of_prescans                 , 0x0019, 0x1270, IS);
GLOBAL_ELEMENT(SPI_Magnetic_field_strength            , 0x0019, 0x1412, DS);
GLOBAL_ELEMENT(SPI_ADC_voltage                        , 0x0019, 0x1414, DS);
GLOBAL_ELEMENT(SPI_ADC_offset                         , 0x0019, 0x1416, DS);
GLOBAL_ELEMENT(SPI_Transmitter_amplitude              , 0x0019, 0x1420, DS);
GLOBAL_ELEMENT(SPI_Transmitter_calibration            , 0x0019, 0x1424, DS);
GLOBAL_ELEMENT(SPI_Receiver_amplifier_gain            , 0x0019, 0x1451, DS);
GLOBAL_ELEMENT(SPI_Receiver_preamplifier_gain         , 0x0019, 0x1452, DS);
GLOBAL_ELEMENT(SPI_Phase_gradient_amplitude           , 0x0019, 0x1470, DS);
GLOBAL_ELEMENT(SPI_Readout_gradient_amplitude         , 0x0019, 0x1471, DS);
GLOBAL_ELEMENT(SPI_Selection_gradient_amplitude       , 0x0019, 0x1472, DS);
GLOBAL_ELEMENT(SPI_Parameter_file_name                , 0x0019, 0x1510, CS);
GLOBAL_ELEMENT(SPI_Sequence_file_name                 , 0x0019, 0x1511, LO);
GLOBAL_ELEMENT(SPI_Sequence_file_owner                , 0x0019, 0x1512, CS);
GLOBAL_ELEMENT(SPI_Sequence_description               , 0x0019, 0x1513, CS);

GLOBAL_ELEMENT(SPI_Image_position                     , 0x0021, 0x1160, DS);
GLOBAL_ELEMENT(SPI_Image_normal                       , 0x0021, 0x1161, DS);
GLOBAL_ELEMENT(SPI_Image_row                          , 0x0021, 0x116a, DS);
GLOBAL_ELEMENT(SPI_Image_column                       , 0x0021, 0x116b, DS);
GLOBAL_ELEMENT(SPI_Number_of_3D_raw_partitions_nominal, 0x0021, 0x1330, IS);
GLOBAL_ELEMENT(SPI_Number_of_3d_raw_part_cur          , 0x0021, 0x1331, IS);
GLOBAL_ELEMENT(SPI_Number_of_3D_image_partitions      , 0x0021, 0x1334, IS);
GLOBAL_ELEMENT(SPI_Actual_3D_partition_number         , 0x0021, 0x1336, IS);
GLOBAL_ELEMENT(SPI_Number_of_slices_nominal           , 0x0021, 0x1340, IS);
GLOBAL_ELEMENT(SPI_Number_of_slices_cur               , 0x0021, 0x1341, IS);
GLOBAL_ELEMENT(SPI_Current_slice_number               , 0x0021, 0x1342, IS);
GLOBAL_ELEMENT(SPI_Order_of_slices                    , 0x0021, 0x134f, IS);
GLOBAL_ELEMENT(SPI_Number_of_echoes                   , 0x0021, 0x1370, IS);

GLOBAL_ELEMENT(SPI_Protocol                           , 0x0029, 0x1020, CS);


