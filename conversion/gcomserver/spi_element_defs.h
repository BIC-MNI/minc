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

/* Define standard UID's */
#define ACR_MR_IMAGE_STORAGE_UID       "1.2.840.10008.5.1.4.1.1.4"
#define ACR_EXPLICIT_VR_BIG_END_UID    "1.2.840.10008.1.2.2"
#define ACR_EXPLICIT_VR_LITTLE_END_UID "1.2.840.10008.1.2.1"
#define ACR_IMPLICIT_VR_LITTLE_END_UID "1.2.840.10008.1.2"
#define ACR_APPLICATION_CONTEXT_UID    "1.2.840.10008.3.1.1.1"

/* Define constants for accepting association */
#define ACR_ASSOC_RJ_CALLED_AP_TITLE_UNREC 7
#define ACR_ASSOC_RJ_NO_REASON 1
#define ACR_ASSOC_RJ_PERM 1
#define ACR_ASSOC_RJ_USER 1
#define ACR_ASSOC_PR_CN_ACCEPT 0
#define ACR_ASSOC_PR_CN_REJECT 1
#define ACR_PDU_ITEM_USER_INFORMATION 0x50

/* Element number for images */
#define SPI_IMAGE_ELEMENT 0x0010

/* Define recognition codes */
#define ACR_RECOGNITION_CODE "ACR-NEMA 1.0"
#define SPI_RECOGNITION_CODE "SPI Release 1 "

/* Define commands */
#define ACR_C_STORE_RQ    0x0001
#define ACR_C_STORE_RSP   0x8001
#define ACR_UNKNOWN_COMMAND 0x0000
#define SENDq               0x0001
#define SENDp               0x8001
#define CANCELq             0x0FFF
#define CANCELp             0x8FFF
#define SPI_UNKNOWN_COMMAND 0x0000
#define GCBEGINq            0x0028
#define GCBEGINp            0x8028
#define READYq              0x0008
#define READYp              0x8008
#define GCENDq              0x0029
#define GCENDp              0x8029

/* Define dataset type */
#define ACR_NULL_DATASET 0x0101

/* Define status codes */
#define ACR_SUCCESS 0x0000
#define ACR_REFUSED 0xaf00
#define SPI_MEDIUM_NOT_AVAIL 0xa0000e10

/* Define group numbers for message stuff */
#define ACR_MESSAGE_GID 0
#define SPI_MESSAGE_GID 1
#define ACR_IDENTIFYING_GID 0x0008
#define ACR_IMAGE_GID 0x0028
#define SPI_ACTUAL_IMAGE_GID 0x7fe0
#define SPI_SCANDATA_GID 0x8001
#define SPI_SPECTRADATA_GID 0x8101
#define SPI_RAWDATA_GID 0x8201
#define SPI_LABELDATA_GID 0x8301

/* Define data object types */
#define ACR_IMAGE_OBJECT 0x0000
#define ACR_OTHER_OBJECT 0x0100

/* Define data object sub-types */
#define SPI_SCANDATA_OBJECT    "SCAN_DATA"
#define SPI_SPECTRADATA_OBJECT "SPECTRA_DATA"
#define SPI_RAWDATA_OBJECT     "RAW_DATA"
#define SPI_LABELDATA_OBJECT   "LAB_DATA"

/* Define acr-nema constants */
#define ACR_MODALITY_MR "MR"

/* Define spi constants */
#define SPI_TRANSVERSE_ORIENTATION 1
#define SPI_SAGITTAL_ORIENTATION   2
#define SPI_CORONAL_ORIENTATION    3
#define SPI_DEFAULT_IMAGE_TYPE     2

/* Element id's for ACR-NEMA */
GLOBAL_ELEMENT(ACR_Length_to_eom             , 0x0000, 0x0001, UL);
GLOBAL_ELEMENT(ACR_Affected_SOP_class_UID    , 0x0000, 0x0002, UI);
GLOBAL_ELEMENT(ACR_Recognition_code          , 0x0000, 0x0010, LO);
GLOBAL_ELEMENT(ACR_Command                   , 0x0000, 0x0100, US);
GLOBAL_ELEMENT(ACR_Message_id                , 0x0000, 0x0110, US);
GLOBAL_ELEMENT(ACR_Message_id_brt            , 0x0000, 0x0120, US);
GLOBAL_ELEMENT(ACR_Initiator                 , 0x0000, 0x0200, LO);
GLOBAL_ELEMENT(ACR_Receiver                  , 0x0000, 0x0300, LO);
GLOBAL_ELEMENT(ACR_Priority                  , 0x0000, 0x0700, US);
GLOBAL_ELEMENT(ACR_Dataset_type              , 0x0000, 0x0800, US);
GLOBAL_ELEMENT(ACR_Status                    , 0x0000, 0x0900, US);
GLOBAL_ELEMENT(ACR_Affected_SOP_instance_UID , 0x0000, 0x1000, UI);

GLOBAL_ELEMENT(ACR_Data_set_type             , 0x0008, 0x0040, US);
GLOBAL_ELEMENT(ACR_Data_set_subtype          , 0x0008, 0x0041, LO);
GLOBAL_ELEMENT(ACR_Patient_name              , 0x0010, 0x0010, PN);
GLOBAL_ELEMENT(ACR_Study                     , 0x0020, 0x0010, IS);
GLOBAL_ELEMENT(ACR_Series                    , 0x0020, 0x0011, IS);
GLOBAL_ELEMENT(ACR_Acquisition               , 0x0020, 0x0012, IS);
GLOBAL_ELEMENT(ACR_Image                     , 0x0020, 0x0013, IS);
GLOBAL_ELEMENT(ACR_Image_location            , 0x0028, 0x0200, US);
GLOBAL_ELEMENT(ACR_Study_date            , 0x0008, 0x0020, DA);
GLOBAL_ELEMENT(ACR_Study_time            , 0x0008, 0x0030, TM);
GLOBAL_ELEMENT(ACR_Modality              , 0x0008, 0x0060, CS);
GLOBAL_ELEMENT(ACR_Manufacturer          , 0x0008, 0x0070, LO);
GLOBAL_ELEMENT(ACR_Institution_id        , 0x0008, 0x0080, LO);
GLOBAL_ELEMENT(ACR_Referring_physician   , 0x0008, 0x0090, PN);
GLOBAL_ELEMENT(ACR_Station_id            , 0x0008, 0x1010, SH);
GLOBAL_ELEMENT(ACR_Procedure_description , 0x0008, 0x1030, LO);
GLOBAL_ELEMENT(ACR_Manufacturer_model    , 0x0008, 0x1090, LO);
GLOBAL_ELEMENT(ACR_Patient_identification, 0x0010, 0x0020, LO);
GLOBAL_ELEMENT(ACR_Patient_birth_date    , 0x0010, 0x0030, DA);
GLOBAL_ELEMENT(ACR_Patient_sex           , 0x0010, 0x0040, CS);
GLOBAL_ELEMENT(ACR_Patient_weight        , 0x0010, 0x1030, DS);
GLOBAL_ELEMENT(ACR_Scanning_sequence     , 0x0018, 0x0020, CS);
GLOBAL_ELEMENT(ACR_Slice_thickness       , 0x0018, 0x0050, DS);
GLOBAL_ELEMENT(ACR_Repetition_time       , 0x0018, 0x0080, DS);
GLOBAL_ELEMENT(ACR_Echo_time             , 0x0018, 0x0081, DS);
GLOBAL_ELEMENT(ACR_Inversion_time        , 0x0018, 0x0082, DS);
GLOBAL_ELEMENT(ACR_Nr_of_averages        , 0x0018, 0x0083, DS);
GLOBAL_ELEMENT(ACR_Imaging_frequency     , 0x0018, 0x0084, DS);
GLOBAL_ELEMENT(ACR_Imaged_nucleus        , 0x0018, 0x0085, SH);
GLOBAL_ELEMENT(ACR_Echo_number           , 0x0018, 0x0086, IS);
GLOBAL_ELEMENT(ACR_Spacing_between_slices, 0x0018, 0x0088, DS);
GLOBAL_ELEMENT(ACR_Echo_train_length     , 0x0018, 0x0091, IS);
GLOBAL_ELEMENT(ACR_Acq_comments          , 0x0018, 0x4000, LT);
GLOBAL_ELEMENT(ACR_Rows                  , 0x0028, 0x0010, US);
GLOBAL_ELEMENT(ACR_Columns               , 0x0028, 0x0011, US);
GLOBAL_ELEMENT(ACR_Pixel_size            , 0x0028, 0x0030, DS);
GLOBAL_ELEMENT(ACR_Bits_allocated        , 0x0028, 0x0100, US);
GLOBAL_ELEMENT(ACR_Bits_stored           , 0x0028, 0x0101, US);
GLOBAL_ELEMENT(ACR_Smallest_pixel_value  , 0x0028, 0x0104, US);
GLOBAL_ELEMENT(ACR_Largest_pixel_value   , 0x0028, 0x0105, US);
GLOBAL_ELEMENT(ACR_Pixel_data            , 0x7fe0, 0x0010, OW);

/* Element id's for SPI */
GLOBAL_ELEMENT(SPI_Recognition_code, 0x0001, 0x0010, LO);
GLOBAL_ELEMENT(SPI_Command         , 0x0001, 0x0018, US);
GLOBAL_ELEMENT(SPI_Status          , 0x0001, 0x0019, UL);
GLOBAL_ELEMENT(SPI_Session_id      , 0x0001, 0x0028, US);
GLOBAL_ELEMENT(SPI_Dataset_type    , 0x0001, 0x0040, US);
GLOBAL_ELEMENT(SPI_Delay_time      , 0x0001, 0x0060, US);
GLOBAL_ELEMENT(SPI_Nr_data_objects , 0x0001, 0x0081, US);
GLOBAL_ELEMENT(SPI_Operator_text   , 0x0001, 0x1060, ST);
GLOBAL_ELEMENT(SPI_Log_info        , 0x0001, 0x1070, ST);
GLOBAL_ELEMENT(SPI_Volume_name     , 0x0001, 0x1080, LO);
GLOBAL_ELEMENT(SPI_Creation_date   , 0x0001, 0x1081, DA);
GLOBAL_ELEMENT(SPI_Creation_time   , 0x0001, 0x1082, TM);
GLOBAL_ELEMENT(SPI_Volume_type     , 0x0001, 0x1083, CS);
GLOBAL_ELEMENT(SPI_Volume_status   , 0x0001, 0x1084, CS);
GLOBAL_ELEMENT(SPI_Space_left      , 0x0001, 0x1085, DS);
GLOBAL_ELEMENT(SPI_Nr_exams        , 0x0001, 0x1086, DS);
GLOBAL_ELEMENT(SPI_Nr_images       , 0x0001, 0x1087, DS);
GLOBAL_ELEMENT(SPI_Closing_date    , 0x0001, 0x1088, DA);
GLOBAL_ELEMENT(SPI_Perc_space_used , 0x0001, 0x1089, DS);

GLOBAL_ELEMENT(SPI_Field_of_view           , 0x0019, 0x1000, DS);
GLOBAL_ELEMENT(SPI_Angulation_of_cc_axis   , 0x0019, 0x1005, DS);
GLOBAL_ELEMENT(SPI_Angulation_of_ap_axis   , 0x0019, 0x1006, DS);
GLOBAL_ELEMENT(SPI_Angulation_of_lr_axis   , 0x0019, 0x1007, DS);
GLOBAL_ELEMENT(SPI_Slice_orientation       , 0x0019, 0x100a, DS);
GLOBAL_ELEMENT(SPI_Off_center_lr           , 0x0019, 0x100b, DS);
GLOBAL_ELEMENT(SPI_Off_center_cc           , 0x0019, 0x100c, DS);
GLOBAL_ELEMENT(SPI_Off_center_ap           , 0x0019, 0x100d, DS);
GLOBAL_ELEMENT(SPI_Number_of_slices        , 0x0019, 0x100f, IS);
GLOBAL_ELEMENT(SPI_Slice_factor            , 0x0019, 0x1010, DS);
GLOBAL_ELEMENT(SPI_Flip_angle              , 0x0019, 0x101a, DS);
GLOBAL_ELEMENT(SPI_Number_of_dynamic_scans , 0x0019, 0x101b, IS);
GLOBAL_ELEMENT(SPI_dynamic_scan_begin_time , 0x0019, 0x1022, DS);
GLOBAL_ELEMENT(SPI_Number_of_phases        , 0x0019, 0x1069, IS);
GLOBAL_ELEMENT(SPI_Nr_of_chemical_shifts   , 0x0019, 0x1080, IS);
GLOBAL_ELEMENT(SPI_Number_of_echoes        , 0x0019, 0x10cf, IS);
GLOBAL_ELEMENT(SPI_Recon_resolution        , 0x0019, 0x1145, DS);
GLOBAL_ELEMENT(SPI_Reconstruction_number   , 0x0021, 0x1000, IS);
GLOBAL_ELEMENT(SPI_Image_type              , 0x0021, 0x1010, IS);
GLOBAL_ELEMENT(SPI_Slice_number            , 0x0021, 0x1020, IS);
GLOBAL_ELEMENT(SPI_Echo_number             , 0x0021, 0x1030, IS);
GLOBAL_ELEMENT(SPI_Chemical_shift_number   , 0x0021, 0x1035, IS);
GLOBAL_ELEMENT(SPI_Phase_number            , 0x0021, 0x1040, IS);
GLOBAL_ELEMENT(SPI_Dynamic_scan_number     , 0x0021, 0x1050, IS);
GLOBAL_ELEMENT(SPI_Ext_scale_minimum       , 0x0029, 0x1010, DS);
GLOBAL_ELEMENT(SPI_Ext_scale_maximum       , 0x0029, 0x1011, DS);
GLOBAL_ELEMENT(SPI_Ext_scale_units         , 0x0029, 0x1020, ST);
GLOBAL_ELEMENT(SPI_Fp_min                  , 0x0029, 0x1110, DS);
GLOBAL_ELEMENT(SPI_Fp_max                  , 0x0029, 0x1120, DS);
GLOBAL_ELEMENT(SPI_Fp_scaled_minimum       , 0x0029, 0x1130, DS);
GLOBAL_ELEMENT(SPI_Fp_scaled_maximum       , 0x0029, 0x1140, DS);
GLOBAL_ELEMENT(SPI_Fp_window_minimum       , 0x0029, 0x1150, DS);
GLOBAL_ELEMENT(SPI_Fp_window_maximum       , 0x0029, 0x1160, DS);

