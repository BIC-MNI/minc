Siemens_header_entry Siemens_header_table[] = {
{0x0008, 0x0020, &Siemens_header.G08.Ide.StudyDate, create_ds_date_t_element, 1},
{0x0008, 0x0022, &Siemens_header.G08.Ide.AcquisitionDate, create_ds_date_t_element, 1},
{0x0008, 0x0023, &Siemens_header.G08.Ide.ImageDate, create_ds_date_t_element, 1},
{0x0008, 0x0030, &Siemens_header.G08.Ide.StudyTime, create_ds_time_t_element, 1},
{0x0008, 0x0032, &Siemens_header.G08.Ide.AcquisitionTime, create_ds_time_t_element, 1},
{0x0008, 0x0033, &Siemens_header.G08.Ide.ImageTime, create_ds_time_t_element, 1},
{0x0008, 0x0041, &Siemens_header.G08.Ide.DataSetSubtype, create_data_set_subtype_t_element, 1},
{0x0008, 0x0060, &Siemens_header.G08.Ide.Modality, create_modality_t_element, 1},
{0x0008, 0x0070, &Siemens_header.G08.Ide.Manufacturer, create_char_element, LENGTH_MANUFACTURER + 1},
{0x0008, 0x0080, &Siemens_header.G08.Ide.InstitutionID, create_char_element, LENGTH_LABEL + 1},
{0x0008, 0x0090, &Siemens_header.G08.Ide.ReferringPhysician, create_char_element, LENGTH_LABEL + 1},
{0x0008, 0x1010, &Siemens_header.G08.Ide.StationID, create_char_element, LENGTH_LABEL + 1},
{0x0008, 0x1080, &Siemens_header.G08.Ide.AdmittingDiagnosis, create_char_element, LENGTH_DIAGNOSIS + 1},
{0x0008, 0x1090, &Siemens_header.G08.Ide.ManufacturerModel, create_char_element, LENGTH_LABEL + 1},
{0x0009, 0x1041, &Siemens_header.G09.Ide.DataObjectSubtype, create_data_object_subtype_t_element, 1},
{0x0009, 0x1210, &Siemens_header.G09.Ide.StorageMode, create_storage_mode_t_element, 1},
{0x0009, 0x1226, &Siemens_header.G09.Ide.LastMoveDate, create_ds_date_t_element, 1},
{0x0009, 0x1227, &Siemens_header.G09.Ide.LastMoveTime, create_ds_time_t_element, 1},
{0x0009, 0x1316, &Siemens_header.G09.Ide.CPUIdentificationLabel, create_char_element, LENGTH_LABEL + 1},
{0x0009, 0x1320, &Siemens_header.G09.Ide.HeaderVersion, create_char_element, LENGTH_HEADER_VERSION + 1},
{0x0010, 0x0010, &Siemens_header.G10.Pat.PatientName, create_char_element, LENGTH_LABEL + 1},
{0x0010, 0x0020, &Siemens_header.G10.Pat.PatientId, create_char_element, LENGTH_PATIENT_ID + 1},
{0x0010, 0x0030, &Siemens_header.G10.Pat.PatientBirthdate, create_ds_date_t_element, 1},
{0x0010, 0x0040, &Siemens_header.G10.Pat.PatientSex, create_sex_t_element, 1},
{0x0010, 0x1010, &Siemens_header.G10.Pat.PatientAge, create_char_element, LENGTH_AGE + 1},
{0x0010, 0x1030, &Siemens_header.G10.Pat.PatientWeight, create_long_element, 1},
{0x0011, 0x1110, &Siemens_header.G11.Pat.RegistrationDate, create_ds_date_t_element, 1},
{0x0011, 0x1111, &Siemens_header.G11.Pat.RegistrationTime, create_ds_time_t_element, 1},
{0x0011, 0x1123, &Siemens_header.G11.Pat.UsedPatientWeight, create_long_element, 1},
{0x0018, 0x0010, &Siemens_header.G18.Acq.Contrast, create_contrast_t_element, 1},
{0x0018, 0x0050, &Siemens_header.G18.Acq.SliceThickness, create_double_element, 1},
{0x0018, 0x0080, &Siemens_header.G18.Acq.RepetitionTime, create_double_element, 1},
{0x0018, 0x0081, &Siemens_header.G18.Acq.EchoTime, create_double_element, 1},
{0x0018, 0x0083, &Siemens_header.G18.Acq.NumberOfAverages, create_long_element, 1},
{0x0018, 0x0084, &Siemens_header.G18.Acq.ImagingFrequency, create_double_element, 1},
{0x0018, 0x0085, &Siemens_header.G18.Acq.ImagedNucleus, create_char_element, LENGTH_NUCLEUS + 1},
{0x0018, 0x0086, &Siemens_header.G18.Acq.EchoNumber, create_long_element, 1},
{0x0018, 0x0090, &Siemens_header.G18.Acq.DataCollectionDiameter, create_long_element, 1},
{0x0018, 0x1000, &Siemens_header.G18.Acq.DeviceSerialNumber, create_char_element, LENGTH_LABEL + 1},
{0x0018, 0x1020, &Siemens_header.G18.Acq.SoftwareVersion, create_char_element, LENGTH_SOFTWARE_VERSION + 1},
{0x0018, 0x1200, &Siemens_header.G18.Acq.CalibrationDate, create_ds_date_t_element, 1},
{0x0018, 0x1201, &Siemens_header.G18.Acq.CalibrationTime, create_ds_time_t_element, 1},
{0x0018, 0x1250, &Siemens_header.G18.Acq.ReceivingCoil, create_char_element, LENGTH_LABEL + 1},
{0x0018, 0x5100, &Siemens_header.G18.Acq.PatientPosition, create_patient_position_t_element, 1},
{0x0019, 0x1010, &Siemens_header.G19.Acq1.CM.NetFrequency, create_long_element, 1},
{0x0019, 0x1020, &Siemens_header.G19.Acq1.CM.MeasurementMode, create_measurement_mode_t_element, 1},
{0x0019, 0x1030, &Siemens_header.G19.Acq1.CM.CalculationMode, create_calculation_mode_t_element, 1},
{0x0019, 0x1050, &Siemens_header.G19.Acq1.CM.NoiseLevel, create_long_element, 1},
{0x0019, 0x1060, &Siemens_header.G19.Acq1.CM.NumberOfDataBytes, create_long_element, 1},
{0x0019, 0x1210, &Siemens_header.G19.Acq2.Mr.TotalMeasurementTime, create_double_element, 1},
{0x0019, 0x1211, &Siemens_header.G19.Acq2.Mr.TotalMeasurementTimeCur, create_double_element, 1},
{0x0019, 0x1212, &Siemens_header.G19.Acq2.Mr.StartDelayTime, create_double_element, 1},
{0x0019, 0x1213, &Siemens_header.G19.Acq2.Mr.DwellTime, create_double_element, 1},
{0x0019, 0x1214, &Siemens_header.G19.Acq2.Mr.NumberOfPhases, create_long_element, 1},
{0x0019, 0x1220, &Siemens_header.G19.Acq2.Mr.NumberOfFourierLinesNominal, create_long_element, 1},
{0x0019, 0x1221, &Siemens_header.G19.Acq2.Mr.NumberOfFourierLinesCurrent, create_long_element, 1},
{0x0019, 0x1226, &Siemens_header.G19.Acq2.Mr.NumberOfFourierLinesAfterZero, create_long_element, 1},
{0x0019, 0x1228, &Siemens_header.G19.Acq2.Mr.FirstMeasuredFourierLine, create_long_element, 1},
{0x0019, 0x1230, &Siemens_header.G19.Acq2.Mr.AcquisitionColumns, create_long_element, 1},
{0x0019, 0x1231, &Siemens_header.G19.Acq2.Mr.ReconstructionColumns, create_long_element, 1},
{0x0019, 0x1250, &Siemens_header.G19.Acq2.Mr.NumberOfAverages, create_long_element, 1},
{0x0019, 0x1260, &Siemens_header.G19.Acq2.Mr.FlipAngle, create_double_element, 1},
{0x0019, 0x1270, &Siemens_header.G19.Acq2.Mr.NumberOfPrescans, create_long_element, 1},
{0x0019, 0x1281, &Siemens_header.G19.Acq2.Mr.FilterTypeRawData, create_filter_type_t_element, 1},
{0x0019, 0x1282, &Siemens_header.G19.Acq2.Mr.FilterParameterRawData, create_filter_parameter_t_element, 1},
{0x0019, 0x1283, &Siemens_header.G19.Acq2.Mr.FilterTypeImageData, create_filter_type_image_t_element, 1},
{0x0019, 0x1285, &Siemens_header.G19.Acq2.Mr.FilterTypePhaseCorrection, create_filter_type_t_element, 1},
{0x0019, 0x1290, &Siemens_header.G19.Acq2.Mr.NumberOfSaturationRegions, create_long_element, 1},
{0x0019, 0x1294, &Siemens_header.G19.Acq2.Mr.ImageRotationAngle, create_double_element, 1},
{0x0019, 0x1298, &Siemens_header.G19.Acq2.Mr.CoilPosition, create_image_location_t_element, 1},
{0x0019, 0x1412, &Siemens_header.G19.Acq3.Mr.MagneticFieldStrength, create_double_element, 1},
{0x0019, 0x1414, &Siemens_header.G19.Acq3.Mr.ADCVoltage, create_double_element, 1},
{0x0019, 0x1416, &Siemens_header.G19.Acq3.Mr.ADCOffset, create_double_element, 2},
{0x0019, 0x1420, &Siemens_header.G19.Acq3.Mr.TransmitterAmplitude, create_double_element, 1},
{0x0019, 0x1421, &Siemens_header.G19.Acq3.Mr.NumberOfTransmitterAmplitudes, create_long_element, 1},
{0x0019, 0x1422, &Siemens_header.G19.Acq3.Mr.TransmitterAttenuator, create_double_element, 1},
{0x0019, 0x1424, &Siemens_header.G19.Acq3.Mr.TransmitterCalibration, create_double_element, 1},
{0x0019, 0x1426, &Siemens_header.G19.Acq3.Mr.TransmitterReference, create_double_element, 1},
{0x0019, 0x1450, &Siemens_header.G19.Acq3.Mr.ReceiverTotalGain, create_double_element, 1},
{0x0019, 0x1451, &Siemens_header.G19.Acq3.Mr.ReceiverAmplifierGain, create_double_element, 1},
{0x0019, 0x1452, &Siemens_header.G19.Acq3.Mr.ReceiverPreamplifierGain, create_double_element, 1},
{0x0019, 0x1454, &Siemens_header.G19.Acq3.Mr.ReceiverCableAttenuation, create_double_element, 1},
{0x0019, 0x1455, &Siemens_header.G19.Acq3.Mr.ReceiverReferenceGain, create_double_element, 1},
{0x0019, 0x1456, &Siemens_header.G19.Acq3.Mr.ReceiverFilterFrequency, create_long_element, 1},
{0x0019, 0x1460, &Siemens_header.G19.Acq3.Mr.ReconstructionScaleFactor, create_double_element, 1},
{0x0019, 0x1462, &Siemens_header.G19.Acq3.Mr.ReferenceScaleFactor, create_double_element, 1},
{0x0019, 0x1470, &Siemens_header.G19.Acq3.Mr.PhaseGradientAmplitude, create_double_element, 1},
{0x0019, 0x1471, &Siemens_header.G19.Acq3.Mr.ReadoutGradientAmplitude, create_double_element, 1},
{0x0019, 0x1472, &Siemens_header.G19.Acq3.Mr.SelectionGradientAmplitude, create_double_element, 1},
{0x0019, 0x1480, &Siemens_header.G19.Acq3.Mr.GradientDelayTime, create_gradient_delay_time_t_element, 1},
{0x0019, 0x1482, &Siemens_header.G19.Acq3.Mr.TotalGradientDelayTime, create_double_element, 1},
{0x0019, 0x1490, &Siemens_header.G19.Acq3.Mr.SensitivityCorrectionLabel, create_char_element, LENGTH_LABEL + 1},
{0x0019, 0x14a0, &Siemens_header.G19.Acq3.Mr.RfWatchdogMask, create_long_element, 1},
{0x0019, 0x14a2, &Siemens_header.G19.Acq3.Mr.RfPowerErrorIndicator, create_double_element, 1},
{0x0019, 0x14a5, &Siemens_header.G19.Acq3.Mr.SarWholeBody, create_sar_sed_t_element, 1},
{0x0019, 0x14a6, &Siemens_header.G19.Acq3.Mr.Sed, create_sar_sed_t_element, 1},
{0x0019, 0x14b0, &Siemens_header.G19.Acq3.Mr.AdjustmentStatusMask, create_long_element, 1},
{0x0019, 0x1510, &Siemens_header.G19.Acq4.CM.ParameterFileName, create_char_element, LENGTH_FILE_NAME + 1},
{0x0019, 0x1511, &Siemens_header.G19.Acq4.CM.SequenceFileName, create_char_element, LENGTH_FILE_NAME + 1},
{0x0019, 0x1512, &Siemens_header.G19.Acq4.CM.SequenceFileOwner, create_char_element, LENGTH_SEQUENCE_INFO + 1},
{0x0019, 0x1513, &Siemens_header.G19.Acq4.CM.SequenceDescription, create_char_element, LENGTH_SEQUENCE_INFO + 1},
{0x0020, 0x0010, &Siemens_header.G20.Rel.Study, create_long_element, 1},
{0x0020, 0x0012, &Siemens_header.G20.Rel.Acquisition, create_long_element, 1},
{0x0020, 0x0013, &Siemens_header.G20.Rel.Image, create_long_element, 1},
{0x0020, 0x0050, &Siemens_header.G20.Rel.Location, create_long_element, 1},
{0x0020, 0x0070, &Siemens_header.G20.Rel.ImageGeometryType, create_geometry_t_element, 1},
{0x0020, 0x1001, &Siemens_header.G20.Rel.AcquisitionsInSeries, create_long_element, 1},
{0x0020, 0x1020, &Siemens_header.G20.Rel.Reference, create_reference_t_element, 1},
{0x0021, 0x1011, &Siemens_header.G21.Rel1.CM.Target, create_target_point_t_element, 1},
{0x0021, 0x1020, &Siemens_header.G21.Rel1.CM.RoiMask, create_short_element, 1},
{0x0021, 0x1120, &Siemens_header.G21.Rel1.CM.FoV, create_field_of_view_t_element, 1},
{0x0021, 0x1122, &Siemens_header.G21.Rel1.CM.ImageMagnificationFactor, create_double_element, 1},
{0x0021, 0x1130, &Siemens_header.G21.Rel1.CM.ViewDirection, create_view_direction_t_element, 1},
{0x0021, 0x1132, &Siemens_header.G21.Rel1.CM.RestDirection, create_rest_direction_t_element, 1},
{0x0021, 0x1160, &Siemens_header.G21.Rel1.CM.ImagePosition, create_image_location_t_element, 1},
{0x0021, 0x1161, &Siemens_header.G21.Rel1.CM.ImageNormal, create_image_location_t_element, 1},
{0x0021, 0x1163, &Siemens_header.G21.Rel1.CM.ImageDistance, create_double_element, 1},
{0x0021, 0x1165, &Siemens_header.G21.Rel1.CM.ImagePositioningHistoryMask, create_short_element, 1},
{0x0021, 0x116a, &Siemens_header.G21.Rel1.CM.ImageRow, create_image_location_t_element, 1},
{0x0021, 0x116b, &Siemens_header.G21.Rel1.CM.ImageColumn, create_image_location_t_element, 1},
{0x0021, 0x1170, &Siemens_header.G21.Rel1.CM.PatientOrientationSet1, create_patient_orientation_t_element, 1},
{0x0021, 0x1171, &Siemens_header.G21.Rel1.CM.PatientOrientationSet2, create_patient_orientation_t_element, 1},
{0x0021, 0x1180, &Siemens_header.G21.Rel1.CM.StudyName, create_char_element, LENGTH_LABEL + 1},
{0x0021, 0x1182, &Siemens_header.G21.Rel1.CM.StudyType, create_study_type_t_element, 1},
{0x0021, 0x1322, &Siemens_header.G21.Rel2.Mr.PhaseCorRowRec, create_long_element, 1},
{0x0021, 0x1324, &Siemens_header.G21.Rel2.Mr.PhaseCorColRec, create_long_element, 1},
{0x0021, 0x1330, &Siemens_header.G21.Rel2.Mr.NumberOf3DRawPartNom, create_long_element, 1},
{0x0021, 0x1331, &Siemens_header.G21.Rel2.Mr.NumberOf3DRawPartCur, create_long_element, 1},
{0x0021, 0x1334, &Siemens_header.G21.Rel2.Mr.NumberOf3DImaPart, create_long_element, 1},
{0x0021, 0x1336, &Siemens_header.G21.Rel2.Mr.Actual3DImaPartNumber, create_long_element, 1},
{0x0021, 0x1339, &Siemens_header.G21.Rel2.Mr.SlabThickness, create_double_element, 1},
{0x0021, 0x1340, &Siemens_header.G21.Rel2.Mr.NumberOfSlicesNom, create_long_element, 1},
{0x0021, 0x1341, &Siemens_header.G21.Rel2.Mr.NumberOfSlicesCur, create_long_element, 1},
{0x0021, 0x1342, &Siemens_header.G21.Rel2.Mr.CurrentSliceNumber, create_long_element, 1},
{0x0021, 0x1343, &Siemens_header.G21.Rel2.Mr.CurrentGroupNumber, create_long_element, 1},
{0x0021, 0x1344, &Siemens_header.G21.Rel2.Mr.CurrentSliceDistanceFactor, create_double_element, 1},
{0x0021, 0x134f, &Siemens_header.G21.Rel2.Mr.OrderOfSlices, create_order_of_slices_t_element, 1},
{0x0021, 0x1356, &Siemens_header.G21.Rel2.Mr.RepetitionTime, create_double_element, 1},
{0x0021, 0x1370, &Siemens_header.G21.Rel2.Mr.NumberOfEchoes, create_long_element, 1},
{0x0028, 0x0005, &Siemens_header.G28.Pre.ImageDimension, create_short_element, 1},
{0x0028, 0x0010, &Siemens_header.G28.Pre.Rows, create_short_element, 1},
{0x0028, 0x0011, &Siemens_header.G28.Pre.Columns, create_short_element, 1},
{0x0028, 0x0030, &Siemens_header.G28.Pre.PixelSize, create_pixel_size_t_element, 1},
{0x0028, 0x0040, &Siemens_header.G28.Pre.ImageFormat, create_image_format_t_element, 1},
{0x0028, 0x0060, &Siemens_header.G28.Pre.CompressionCode, create_compression_code_t_element, 1},
{0x0028, 0x0100, &Siemens_header.G28.Pre.BitsAllocated, create_short_element, 1},
{0x0028, 0x0101, &Siemens_header.G28.Pre.BitsStored, create_short_element, 1},
{0x0028, 0x0102, &Siemens_header.G28.Pre.HighBit, create_short_element, 1},
{0x0028, 0x0103, &Siemens_header.G28.Pre.PixelRepresentation, create_short_element, 1},
{0x0028, 0x1050, &Siemens_header.G28.Pre.WindowCenter, create_windows_t_element, 1},
{0x0028, 0x1051, &Siemens_header.G28.Pre.WindowWidth, create_windows_t_element, 1},
{0x0028, 0x1052, &Siemens_header.G28.Pre.RescaleIntercept, create_long_element, 1},
{0x0028, 0x1053, &Siemens_header.G28.Pre.RescaleSlope, create_long_element, 1},
{0x0029, 0x1110, &Siemens_header.G29.Pre.WindowStyle, create_window_style_t_element, 1},
{0x0029, 0x1120, &Siemens_header.G29.Pre.PixelQualityCode, create_pixel_quality_code_t_element, 1},
{0x0029, 0x1152, &Siemens_header.G29.Pre.SortCode, create_long_element, 1},
{0, 0, NULL, NULL, 0}
};


/* Functions needed for this table:

      create_calculation_mode_t_element
      create_char_element
      create_compression_code_t_element
      create_contrast_t_element
      create_data_object_subtype_t_element
      create_data_set_subtype_t_element
      create_double_element
      create_ds_date_t_element
      create_ds_time_t_element
      create_field_of_view_t_element
      create_filter_parameter_t_element
      create_filter_type_image_t_element
      create_filter_type_t_element
      create_gate_phase_t_element
      create_geometry_t_element
      create_gradient_delay_time_t_element
      create_image_format_t_element
      create_image_location_t_element
      create_laterality_t_element
      create_long_element
      create_measurement_mode_t_element
      create_modality_t_element
      create_object_orientation_t_element
      create_object_threshold_t_element
      create_order_of_slices_t_element
      create_patient_orientation_t_element
      create_patient_phase_t_element
      create_patient_position_t_element
      create_patient_region_t_element
      create_pixel_quality_code_t_element
      create_pixel_quality_value_t_element
      create_pixel_size_t_element
      create_reference_t_element
      create_rest_direction_t_element
      create_rotation_direction_t_element
      create_sar_sed_t_element
      create_save_code_t_element
      create_sex_t_element
      create_short_element
      create_storage_mode_t_element
      create_study_type_t_element
      create_target_point_t_element
      create_view_direction_t_element
      create_window_style_t_element
      create_windows_t_element

 */

