/* ----------------------------- MNI Header -----------------------------------
@NAME       : ecat_header_definition.h
@DESCRIPTION: Header file containing header description for ECAT files
@GLOBALS    : 
@CREATED    : January 4, 1996 (Peter Neelin)
@MODIFIED   : 
 * $Log: ecat_header_definition.h,v $
 * Revision 6.1  1999-10-29 17:52:01  neelin
 * Fixed Log keyword
 *
 * Revision 6.0  1997/09/12 13:24:22  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:25:21  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:06:04  neelin
 * Release of minc version 0.4
 *
 * Revision 1.2  1996/03/26  15:58:18  neelin
 * Added some more header values.
 *
 * Revision 1.1  1996/01/18  14:52:14  neelin
 * Initial revision
 *
@COPYRIGHT  :
              Copyright 1996 Peter Neelin, McConnell Brain Imaging Centre, 
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

/* Macro to set up header definition tables */
#define INITIAL_HEADER_TABLE(field_list) \
{FALSE, sizeof(field_list)/sizeof(field_list[0]), field_list, NULL}

/* Main header and sub header descriptions for ECAT version 7 files */
static Ecat_field_description_type version_7_main_list[] =
{
   {ECAT_Magic_Number, 0, 14, ecat_char, "Magic_Number"},
   {ECAT_Original_Filename, 14, 32, ecat_char, "Original_Filename"},
   {ECAT_Sw_Version, 46, 1, ecat_short, "Sw_Version"},
   {ECAT_System_Type, 48, 1, ecat_short, "System_Type"},
   {ECAT_File_Type, 50, 1, ecat_short, "File_Type"},
   {ECAT_Serial_Number, 52, 10, ecat_char, "Serial_Number"},
   {ECAT_Scan_Start_Time, 62, 1, ecat_long, "Scan_Start_Time"},
   {ECAT_Isotope_Name, 66, 8, ecat_char, "Isotope_Name"},
   {ECAT_Isotope_Halflife, 74, 1, ecat_float, "Isotope_Halflife"},
   {ECAT_Radiopharmaceutical, 78, 32, ecat_char, "Radiopharmaceutical"},
   {ECAT_Gantry_Tilt, 110, 1, ecat_float, "Gantry_Tilt"},
   {ECAT_Gantry_Rotation, 114, 1, ecat_float, "Gantry_Rotation"},
   {ECAT_Bed_Elevation, 118, 1, ecat_float, "Bed_Elevation"},
   {ECAT_Intrinsic_Tilt, 122, 1, ecat_float, "Intrinsic_Tilt"},
   {ECAT_Wobble_Speed, 126, 1, ecat_short, "Wobble_Speed"},
   {ECAT_Transm_Source_Type, 128, 1, ecat_short, "Transm_Source_Type"},
   {ECAT_Distance_Scanned, 130, 1, ecat_float, "Distance_Scanned"},
   {ECAT_Transaxial_Fov, 134, 1, ecat_float, "Transaxial_Fov"},
   {ECAT_Angular_Compression, 138, 1, ecat_short, "Angular_Compression"},
   {ECAT_Coin_Samp_Mode, 140, 1, ecat_short, "Coin_Samp_Mode"},
   {ECAT_Axial_Samp_Mode, 142, 1, ecat_short, "Axial_Samp_Mode"},
   {ECAT_Calibration_Factor, 144, 1, ecat_float, "Calibration_Factor"},
   {ECAT_Calibration_Units, 148, 1, ecat_short, "Calibration_Units"},
   {ECAT_Calibration_Units_Label, 150, 1, ecat_short, "Calibration_Units_Label"},
   {ECAT_Compression_Code, 152, 1, ecat_short, "Compression_Code"},
   {ECAT_Study_Type, 154, 12, ecat_char, "Study_Type"},
   {ECAT_Patient_Id, 166, 16, ecat_char, "Patient_Id"},
   {ECAT_Patient_Name, 182, 32, ecat_char, "Patient_Name"},
   {ECAT_Patient_Sex, 214, 1, ecat_char, "Patient_Sex"},
   {ECAT_Patient_Dexterity, 215, 1, ecat_char, "Patient_Dexterity"},
   {ECAT_Patient_Age, 216, 1, ecat_float, "Patient_Age"},
   {ECAT_Patient_Height, 220, 1, ecat_float, "Patient_Height"},
   {ECAT_Patient_Weight, 224, 1, ecat_float, "Patient_Weight"},
   {ECAT_Patient_Birth_Date, 228, 1, ecat_long, "Patient_Birth_Date"},
   {ECAT_Physician_Name, 232, 32, ecat_char, "Physician_Name"},
   {ECAT_Operator_Name, 264, 32, ecat_char, "Operator_Name"},
   {ECAT_Study_Description, 296, 32, ecat_char, "Study_Description"},
   {ECAT_Acquision_Type, 328, 1, ecat_short, "Acquision_Type"},
   {ECAT_Patient_Orientation, 330, 1, ecat_short, "Patient_Orientation"},
   {ECAT_Facility_Name, 332, 20, ecat_char, "Facility_Name"},
   {ECAT_Num_Planes, 352, 1, ecat_short, "Num_Planes"},
   {ECAT_Num_Frames, 354, 1, ecat_short, "Num_Frames"},
   {ECAT_Num_Gates, 356, 1, ecat_short, "Num_Gates"},
   {ECAT_Num_Bed_Pos, 358, 1, ecat_short, "Num_Bed_Pos"},
   {ECAT_Init_Bed_Position, 360, 1, ecat_float, "Init_Bed_Position"},
   {ECAT_Bed_Position, 364, 15, ecat_float, "Bed_Position"},
   {ECAT_Plane_Separation, 424, 1, ecat_float, "Plane_Separation"},
   {ECAT_Lwr_Sctr_Thres, 428, 1, ecat_short, "Lwr_Sctr_Thres"},
   {ECAT_Lwr_True_Thres, 430, 1, ecat_short, "Lwr_True_Thres"},
   {ECAT_Upr_True_Thres, 432, 1, ecat_short, "Upr_True_Thres"},
   {ECAT_User_Process_Code, 434, 10, ecat_char, "User_Process_Code"},
   {ECAT_Acquisition_Mode, 444, 1, ecat_short, "Acquisition_Mode"},
   {ECAT_Bin_Size, 446, 1, ecat_float, "Bin_Size"},
   {ECAT_Branching_Fraction, 450, 1, ecat_float, "Branching_Fraction"},
   {ECAT_Dose_Start_Time, 454, 1, ecat_long, "Dose_Start_Time"},
   {ECAT_Dosage, 458, 1, ecat_float, "Dosage"},
   {ECAT_Well_Counter_Corr_Factor, 462, 1, ecat_float, "Well_Counter_Corr_Factor"},
   {ECAT_Data_Units, 466, 32, ecat_char, "Data_Units"},
   {ECAT_Septa_State, 498, 1, ecat_short, "Septa_State"}
};

static Ecat_field_description_type version_7_subhdr_list[] =
{
   {ECAT_Data_Type, 0, 1, ecat_short, "Data_Type"},
   {ECAT_Num_Dimensions, 2, 1, ecat_short, "Num_Dimensions"},
   {ECAT_X_Dimension, 4, 1, ecat_short, "X_Dimension"},
   {ECAT_Y_Dimension, 6, 1, ecat_short, "Y_Dimension"},
   {ECAT_Z_Dimension, 8, 1, ecat_short, "Z_Dimension"},
   {ECAT_X_Offset, 14, 1, ecat_float, "X_Offset"},
   {ECAT_Y_Offset, 18, 1, ecat_float, "Y_Offset"},
   {ECAT_Z_Offset, 10, 1, ecat_float, "Z_Offset"},
   {ECAT_Recon_Zoom, 22, 1, ecat_float, "Recon_Zoom"},
   {ECAT_Scale_Factor, 26, 1, ecat_float, "Scale_Factor"},
   {ECAT_Image_Min, 30, 1, ecat_short, "Image_Min"},
   {ECAT_Image_Max, 32, 1, ecat_short, "Image_Max"},
   {ECAT_X_Pixel_Size, 34, 1, ecat_float, "X_Pixel_Size"},
   {ECAT_Y_Pixel_Size, 38, 1, ecat_float, "Y_Pixel_Size"},
   {ECAT_Z_Pixel_Size, 42, 1, ecat_float, "Z_Pixel_Size"},
   {ECAT_Frame_Duration, 46, 1, ecat_long, "Frame_Duration"},
   {ECAT_Frame_Start_Time, 50, 1, ecat_long, "Frame_Start_Time"},
   {ECAT_Filter_Code, 54, 1, ecat_short, "Filter_Code"},
   {ECAT_X_Resolution, 56, 1, ecat_float, "X_Resolution"},
   {ECAT_Y_Resolution, 60, 1, ecat_float, "Y_Resolution"},
   {ECAT_Z_Resolution, 64, 1, ecat_float, "Z_Resolution"},
   {ECAT_X_Rotation_Angle, 68, 1, ecat_float, "X_Rotation_Angle"},
   {ECAT_Y_Rotation_Angle, 72, 1, ecat_float, "Y_Rotation_Angle"},
   {ECAT_Z_Rotation_Angle, 76, 1, ecat_float, "Z_Rotation_Angle"},
   {ECAT_Decay_Corr_Fctr, 80, 1, ecat_float, "Decay_Corr_Fctr"},
   {ECAT_Corrections_Applied, 84, 1, ecat_long, "Corrections_Applied"},
   {ECAT_Gate_Duration, 88, 1, ecat_long, "Gate_Duration"},
   {ECAT_R_Wave_Offset, 92, 1, ecat_long, "R_Wave_Offset"},
   {ECAT_Num_Accepted_Beats, 96, 1, ecat_long, "Num_Accepted_Beats"},
   {ECAT_Filter_Cutoff_Frequency, 100, 1, ecat_float, "Filter_Cutoff_Frequency"},
   {ECAT_Filter_Dc_Component, 104, 1, ecat_float, "Filter_Dc_Component"},
   {ECAT_Filter_Ramp_Slope, 108, 1, ecat_float, "Filter_Ramp_Slope"},
   {ECAT_Filter_Order, 112, 1, ecat_short, "Filter_Order"},
   {ECAT_Filter_Scatter_Fraction, 114, 1, ecat_float, "Filter_Scatter_Fraction"},
   {ECAT_Filter_Scatter_Slope, 118, 1, ecat_float, "Filter_Scatter_Slope"},
   {ECAT_Annotation, 122, 40, ecat_char, "Annotation"},
   {ECAT_Da_X_Rotation_Angle, 162, 1, ecat_float, "Da_X_Rotation_Angle"},
   {ECAT_Da_Y_Rotation_Angle, 166, 1, ecat_float, "Da_Y_Rotation_Angle"},
   {ECAT_Da_Z_Rotation_Angle, 170, 1, ecat_float, "Da_Z_Rotation_Angle"},
   {ECAT_Da_X_Translation, 174, 1, ecat_float, "Da_X_Translation"},
   {ECAT_Da_Y_Translation, 178, 1, ecat_float, "Da_Y_Translation"},
   {ECAT_Da_Z_Translation, 182, 1, ecat_float, "Da_Z_Translation"},
   {ECAT_Da_X_Scale_Factor, 186, 1, ecat_float, "Da_X_Scale_Factor"},
   {ECAT_Da_Y_Scale_Factor, 190, 1, ecat_float, "Da_Y_Scale_Factor"},
   {ECAT_Da_Z_Scale_Factor, 194, 1, ecat_float, "Da_Z_Scale_Factor"},
   {ECAT_Rfilter_Cutoff, 198, 1, ecat_float, "Rfilter_Cutoff"},
   {ECAT_Rfilter_Resolution, 202, 1, ecat_float, "Rfilter_Resolution"},
   {ECAT_Rfilter_Code, 206, 1, ecat_short, "Rfilter_Code"},
   {ECAT_Rfilter_Order, 208, 1, ecat_short, "Rfilter_Order"},
   {ECAT_Zfilter_Cutoff, 210, 1, ecat_float, "Zfilter_Cutoff"},
   {ECAT_Zfilter_Resolution, 214, 1, ecat_float, "Zfilter_Resolution"},
   {ECAT_Zfilter_Code, 218, 1, ecat_short, "Zfilter_Code"},
   {ECAT_Zfilter_Order, 220, 1, ecat_short, "Zfilter_Order"}
};

static Ecat_header_table_type 
version_7_main_table = INITIAL_HEADER_TABLE(version_7_main_list);

static Ecat_header_table_type 
version_7_subhdr_table = INITIAL_HEADER_TABLE(version_7_subhdr_list);

static Ecat_header_description_type version_7_description = 
{
   &version_7_main_table, 
   &version_7_subhdr_table
};

static Ecat_header_description_type *ECAT_VER_7 = &version_7_description;

/* Main header and sub header descriptions for ECAT files before version 7 */
static Ecat_field_description_type version_pre7_main_list[] =
{
   {ECAT_Original_Filename, 28, 20, ecat_char, "Original_Filename"},
   {ECAT_Sw_Version, 48, 1, ecat_short, "Sw_Version"},
   {ECAT_Data_Type, 50, 1, ecat_short, "Data_type"},
   {ECAT_System_Type, 52, 1, ecat_short, "System_Type"},
   {ECAT_File_Type, 54, 1, ecat_short, "File_Type"},
   {ECAT_Serial_Number, 56, 10, ecat_char, "Serial_Number"},
   {ECAT_Scan_Start_Day, 66, 1, ecat_short, "Scan_Start_Day"},
   {ECAT_Scan_Start_Month, 68, 1, ecat_short, "Scan_Start_Month"},
   {ECAT_Scan_Start_Year, 70, 1, ecat_short, "Scan_Start_Year"},
   {ECAT_Scan_Start_Hour, 72, 1, ecat_short, "Scan_Start_Hour"},
   {ECAT_Scan_Start_Minute, 74, 1, ecat_short, "Scan_Start_Minute"},
   {ECAT_Scan_Start_Second, 76, 1, ecat_short, "Scan_Start_Second"},
   {ECAT_Isotope_Name, 78, 8, ecat_char, "Isotope_Name"},
   {ECAT_Isotope_Halflife, 86, 1, ecat_float, "Isotope_Halflife"},
   {ECAT_Radiopharmaceutical, 90, 32, ecat_char, "Radiopharmaceutical"},
   {ECAT_Gantry_Tilt, 122, 1, ecat_float, "Gantry_Tilt"},
   {ECAT_Gantry_Rotation, 126, 1, ecat_float, "Gantry_Rotation"},
   {ECAT_Bed_Elevation, 130, 1, ecat_float, "Bed_Elevation"},
   {ECAT_Rot_Source_Speed, 134, 1, ecat_short, "Rot_Source_Speed"},
   {ECAT_Wobble_Speed, 136, 1, ecat_short, "Wobble_Speed"},
   {ECAT_Transm_Source_Type, 138, 1, ecat_short, "Transm_Source_Type"},
   {ECAT_Distance_Scanned, 140, 1, ecat_float, "Distance_Scanned"},
   {ECAT_Transaxial_Fov, 144, 1, ecat_float, "Transaxial_Fov"},
   {ECAT_Coin_Samp_Mode, 150, 1, ecat_short, "Coin_Samp_Mode"},
   {ECAT_Axial_Samp_Mode, 152, 1, ecat_short, "Axial_Samp_Mode"},
   {ECAT_Calibration_Factor, 154, 1, ecat_float, "Calibration_Factor"},
   {ECAT_Calibration_Units, 158, 1, ecat_short, "Calibration_Units"},
   {ECAT_Compression_Code, 160, 1, ecat_short, "Compression_Code"},
   {ECAT_Study_Type, 162, 12, ecat_char, "Study_Type"},
   {ECAT_Patient_Id, 174, 16, ecat_char, "Patient_Id"},
   {ECAT_Patient_Name, 190, 32, ecat_char, "Patient_Name"},
   {ECAT_Patient_Sex, 222, 1, ecat_char, "Patient_Sex"},
   {ECAT_Patient_Age, 223, 10, ecat_char, "Patient_Age"},
   {ECAT_Patient_Height, 233, 10, ecat_char, "Patient_Height"},
   {ECAT_Patient_Weight, 243, 10, ecat_char, "Patient_Weight"},
   {ECAT_Patient_Dexterity, 253, 1, ecat_char, "Patient_Dexterity"},
   {ECAT_Physician_Name, 254, 32, ecat_char, "Physician_Name"},
   {ECAT_Operator_Name, 286, 32, ecat_char, "Operator_Name"},
   {ECAT_Study_Description, 318, 32, ecat_char, "Study_Description"},
   {ECAT_Acquision_Type, 350, 1, ecat_short, "Acquision_Type"},
   {ECAT_Facility_Name, 356, 20, ecat_char, "Facility_Name"},
   {ECAT_Num_Planes, 376, 1, ecat_short, "Num_Planes"},
   {ECAT_Num_Frames, 378, 1, ecat_short, "Num_Frames"},
   {ECAT_Num_Gates, 380, 1, ecat_short, "Num_Gates"},
   {ECAT_Num_Bed_Pos, 382, 1, ecat_short, "Num_Bed_Pos"},
   {ECAT_Init_Bed_Position, 384, 1, ecat_float, "Init_Bed_Position"},
   {ECAT_Bed_Position, 388, 15, ecat_float, "Bed_Position"},
   {ECAT_Plane_Separation, 448, 1, ecat_float, "Plane_Separation"},
   {ECAT_Lwr_Sctr_Thres, 452, 1, ecat_short, "Lwr_Sctr_Thres"},
   {ECAT_Lwr_True_Thres, 454, 1, ecat_short, "Lwr_True_Thres"},
   {ECAT_Upr_True_Thres, 456, 1, ecat_short, "Upr_True_Thres"},
   {ECAT_User_Process_Code, 462, 10, ecat_char, "User_Process_Code"}
};

static Ecat_field_description_type version_pre7_subhdr_list[] =
{
   {ECAT_Data_Type, 126, 1, ecat_short, "Data_Type"},
   {ECAT_Num_Dimensions, 128, 1, ecat_short, "Num_Dimensions"},
   {ECAT_X_Dimension, 132, 1, ecat_short, "X_Dimension"},
   {ECAT_Y_Dimension, 134, 1, ecat_short, "Y_Dimension"},
   {ECAT_X_Offset, 160, 1, ecat_float, "X_Offset"},
   {ECAT_Y_Offset, 164, 1, ecat_float, "Y_Offset"},
   {ECAT_Recon_Zoom, 168, 1, ecat_float, "Recon_Zoom"},
   {ECAT_Scale_Factor, 172, 1, ecat_float, "Scale_Factor"},
   {ECAT_Image_Min, 176, 1, ecat_short, "Image_Min"},
   {ECAT_Image_Max, 178, 1, ecat_short, "Image_Max"},
   {ECAT_X_Pixel_Size, 184, 1, ecat_float, "X_Pixel_Size"},
   {ECAT_Y_Pixel_Size, 184, 1, ecat_float, "Y_Pixel_Size"}, /* Same location */
   {ECAT_Z_Pixel_Size, 188, 1, ecat_float, "Z_Pixel_Size"},
   {ECAT_Frame_Duration, 192, 1, ecat_long, "Frame_Duration"},
   {ECAT_Frame_Start_Time, 196, 1, ecat_long, "Frame_Start_Time"},
   {ECAT_Filter_Code, 236, 1, ecat_short, "Filter_Code"},
   {ECAT_Z_Rotation_Angle, 296, 1, ecat_float, "Z_Rotation_Angle"},
   {ECAT_Decay_Corr_Fctr, 304, 1, ecat_float, "Decay_Corr_Fctr"},
   {ECAT_Calibration_Factor, 388, 1, ecat_float, "Calibration_Factor"},
   {ECAT_Filter_Cutoff_Frequency, 396, 1, ecat_float, "Filter_Cutoff_Frequency"},
   {ECAT_Filter_Dc_Component, 400, 1, ecat_float, "Filter_Dc_Component"},
   {ECAT_Filter_Ramp_Slope, 404, 1, ecat_float, "Filter_Ramp_Slope"},
   {ECAT_Annotation, 420, 40, ecat_char, "Annotation"}
};

static Ecat_header_table_type 
version_pre7_main_table = INITIAL_HEADER_TABLE(version_pre7_main_list);

static Ecat_header_table_type 
version_pre7_subhdr_table = INITIAL_HEADER_TABLE(version_pre7_subhdr_list);

static Ecat_header_description_type version_pre7_description = 
{
   &version_pre7_main_table, 
   &version_pre7_subhdr_table
};

static Ecat_header_description_type *ECAT_VER_PRE7 = &version_pre7_description;

