/* ----------------------------- MNI Header -----------------------------------
@NAME       : ecat_file.h
@DESCRIPTION: Header file for routines that read ECAT image files
@GLOBALS    : 
@CREATED    : January 4, 1996 (Peter Neelin)
@MODIFIED   : 
 * $Log: ecat_file.h,v $
 * Revision 6.2  2005-01-19 19:46:01  bert
 * Changes from Anthonin Reilhac
 *
 * Revision 6.1  1999/10/29 17:52:01  neelin
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

#ifndef public
#define public
#endif
#ifndef private
#define private static
#endif

#define ECAT_MAX_STRING_LENGTH 64


/*memory allocation*/
#define MALLOC(size) ((void *) malloc_check(size))
#define FREE(ptr) free(ptr)
#define REALLOC(ptr, size) ((void *) realloc_check(ptr, size))
#define CALLOC(nelem, elsize) ((void *) calloc(nelem, elsize))
public void *malloc_check(size_t size);
public void *realloc_check(void *ptr, size_t size);

typedef enum {
   ECAT_No_Field,
   ECAT_Magic_Number,
   ECAT_Original_Filename,
   ECAT_Sw_Version,
   ECAT_System_Type,
   ECAT_File_Type,
   ECAT_Serial_Number,
   ECAT_Scan_Start_Time,
   ECAT_Isotope_Name,
   ECAT_Isotope_Halflife,
   ECAT_Radiopharmaceutical,
   ECAT_Gantry_Tilt,
   ECAT_Gantry_Rotation,
   ECAT_Bed_Elevation,
   ECAT_Intrinsic_Tilt,
   ECAT_Wobble_Speed,
   ECAT_Transm_Source_Type,
   ECAT_Distance_Scanned,
   ECAT_Transaxial_Fov,
   ECAT_Angular_Compression,
   ECAT_Coin_Samp_Mode,
   ECAT_Axial_Samp_Mode,
   ECAT_Calibration_Factor,
   ECAT_Calibration_Units,
   ECAT_Calibration_Units_Label,
   ECAT_Compression_Code,
   ECAT_Study_Type,
   ECAT_Patient_Id,
   ECAT_Patient_Name,
   ECAT_Patient_Sex,
   ECAT_Patient_Dexterity,
   ECAT_Patient_Age,
   ECAT_Patient_Height,
   ECAT_Patient_Weight,
   ECAT_Patient_Birth_Date,
   ECAT_Physician_Name,
   ECAT_Operator_Name,
   ECAT_Study_Description,
   ECAT_Acquision_Type,
   ECAT_Patient_Orientation,
   ECAT_Facility_Name,
   ECAT_Num_Planes,
   ECAT_Num_Frames,
   ECAT_Num_Gates,
   ECAT_Num_Bed_Pos,
   ECAT_Init_Bed_Position,
   ECAT_Bed_Position,
   ECAT_Plane_Separation,
   ECAT_Lwr_Sctr_Thres,
   ECAT_Lwr_True_Thres,
   ECAT_Upr_True_Thres,
   ECAT_User_Process_Code,
   ECAT_Acquisition_Mode,
   ECAT_Bin_Size,
   ECAT_Branching_Fraction,
   ECAT_Dose_Start_Time,
   ECAT_Dosage,
   ECAT_Well_Counter_Corr_Factor,
   ECAT_Data_Units,
   ECAT_Septa_State,
   ECAT_Data_Type,
   ECAT_Num_Dimensions,
   ECAT_X_Dimension,
   ECAT_Y_Dimension,
   ECAT_Z_Dimension,
   ECAT_X_Offset,
   ECAT_Y_Offset,
   ECAT_Z_Offset,
   ECAT_Recon_Zoom,
   ECAT_Scale_Factor,
   ECAT_Image_Min,
   ECAT_Image_Max,
   ECAT_X_Pixel_Size,
   ECAT_Y_Pixel_Size,
   ECAT_Z_Pixel_Size,
   ECAT_Frame_Duration,
   ECAT_Frame_Start_Time,
   ECAT_Filter_Code,
   ECAT_X_Resolution,
   ECAT_Y_Resolution,
   ECAT_Z_Resolution,
   ECAT_X_Rotation_Angle,
   ECAT_Y_Rotation_Angle,
   ECAT_Z_Rotation_Angle,
   ECAT_Decay_Corr_Fctr,
   ECAT_Corrections_Applied,
   ECAT_Gate_Duration,
   ECAT_R_Wave_Offset,
   ECAT_Num_Accepted_Beats,
   ECAT_Filter_Cutoff_Frequency,
   ECAT_Filter_Dc_Component,
   ECAT_Filter_Ramp_Slope,
   ECAT_Filter_Order,
   ECAT_Filter_Scatter_Fraction,
   ECAT_Filter_Scatter_Slope,
   ECAT_Annotation,
   ECAT_Da_X_Rotation_Angle,
   ECAT_Da_Y_Rotation_Angle,
   ECAT_Da_Z_Rotation_Angle,
   ECAT_Da_X_Translation,
   ECAT_Da_Y_Translation,
   ECAT_Da_Z_Translation,
   ECAT_Da_X_Scale_Factor,
   ECAT_Da_Y_Scale_Factor,
   ECAT_Da_Z_Scale_Factor,
   ECAT_Rfilter_Cutoff,
   ECAT_Rfilter_Resolution,
   ECAT_Rfilter_Code,
   ECAT_Rfilter_Order,
   ECAT_Zfilter_Cutoff,
   ECAT_Zfilter_Resolution,
   ECAT_Zfilter_Code,
   ECAT_Zfilter_Order,
   ECAT_Scan_Start_Day,
   ECAT_Scan_Start_Month,
   ECAT_Scan_Start_Year,
   ECAT_Scan_Start_Hour,
   ECAT_Scan_Start_Minute,
   ECAT_Scan_Start_Second,
   ECAT_Rot_Source_Speed
} Ecat_field_name;

typedef struct Ecat_file Ecat_file;

/* Routine declarations */
public Ecat_file *ecat_open(char *filename);
public void ecat_close(Ecat_file *file);
public int ecat_get_num_planes(Ecat_file *file);
public int ecat_get_num_frames(Ecat_file *file);
public int ecat_get_num_bed_positions(Ecat_file *file);
public int ecat_get_num_gates(Ecat_file *file);
public Ecat_field_name ecat_list_main(Ecat_file *file, int index);
public Ecat_field_name ecat_list_subhdr(Ecat_file *file, int index);
public int ecat_get_main_field_length(Ecat_file *file, 
                                      Ecat_field_name field);
public int ecat_get_subhdr_field_length(Ecat_file *file, 
                                        Ecat_field_name field);
public char *ecat_get_main_field_description(Ecat_file *file, 
                                             Ecat_field_name field);
public char *ecat_get_subhdr_field_description(Ecat_file *file, 
                                               Ecat_field_name field);
public int ecat_get_main_value(Ecat_file *file, 
                               Ecat_field_name field, int index,
                               int *ivalue, double *fvalue, char *svalue);
public int ecat_get_subhdr_value(Ecat_file *file, int volume, int slice,
                                 Ecat_field_name field, int index,
                                 int *ivalue, double *fvalue, char *svalue);
public int ecat_get_image(Ecat_file *file, int volume, int slice, 
                          short *image);
