/* ----------------------------- MNI Header -----------------------------------
@NAME       : dicom_element_defs.h
@DESCRIPTION: Element definitions for dicom
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 28, 1997 (Peter Neelin)
@MODIFIED   : 
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

/* Define standard UID's */
#define FAVORITE_ABSTRACT_SYNTAX       ACR_MR_IMAGE_STORAGE_UID
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

/* Define group numbers  */
#define ACR_MESSAGE_GID 0
#define ACR_ACTUAL_IMAGE_GID 0x7fe0

/* Define commands */
#define ACR_C_STORE_RQ    0x0001
#define ACR_C_STORE_RSP   0x8001
#define ACR_C_ECHO_RQ     0x0030
#define ACR_C_ECHO_RSP    0x8030

/* Define dataset type */
#define ACR_NULL_DATASET 0x0101

/* Define status codes */
#define ACR_SUCCESS 0x0000

/* Define data object types */
#define ACR_IMAGE_OBJECT 0x0000
#define ACR_OTHER_OBJECT 0x0100

/* Define acr-nema constants */
#define ACR_MODALITY_MR "MR"

/* Element id's for DICOM */
GLOBAL_ELEMENT(ACR_Affected_SOP_class_UID    , 0x0000, 0x0002, UI);
GLOBAL_ELEMENT(ACR_Command                   , 0x0000, 0x0100, US);
GLOBAL_ELEMENT(ACR_Message_id                , 0x0000, 0x0110, US);
GLOBAL_ELEMENT(ACR_Message_id_brt            , 0x0000, 0x0120, US);
GLOBAL_ELEMENT(ACR_Priority                  , 0x0000, 0x0700, US);
GLOBAL_ELEMENT(ACR_Dataset_type              , 0x0000, 0x0800, US);
GLOBAL_ELEMENT(ACR_Status                    , 0x0000, 0x0900, US);
GLOBAL_ELEMENT(ACR_Affected_SOP_instance_UID , 0x0000, 0x1000, UI);
GLOBAL_ELEMENT(ACR_Move_originator_AE_title  , 0x0000, 0x1031, AE);

GLOBAL_ELEMENT(ACR_Image_type            , 0x0008, 0x0008, CS);
GLOBAL_ELEMENT(ACR_Study_date            , 0x0008, 0x0020, DA);
GLOBAL_ELEMENT(ACR_Series_date           , 0x0008, 0x0021, DA);
GLOBAL_ELEMENT(ACR_Acquisition_date      , 0x0008, 0x0022, DA);
GLOBAL_ELEMENT(ACR_Study_time            , 0x0008, 0x0030, TM);
GLOBAL_ELEMENT(ACR_Series_time           , 0x0008, 0x0031, TM);
GLOBAL_ELEMENT(ACR_Acquisition_time      , 0x0008, 0x0032, TM);
GLOBAL_ELEMENT(ACR_Modality              , 0x0008, 0x0060, CS);
GLOBAL_ELEMENT(ACR_Manufacturer          , 0x0008, 0x0070, LO);
GLOBAL_ELEMENT(ACR_Institution_id        , 0x0008, 0x0080, LO);
GLOBAL_ELEMENT(ACR_Referring_physician   , 0x0008, 0x0090, PN);
GLOBAL_ELEMENT(ACR_Station_id            , 0x0008, 0x1010, SH);
GLOBAL_ELEMENT(ACR_Procedure_description , 0x0008, 0x1030, LO);
GLOBAL_ELEMENT(ACR_Performing_physician  , 0x0008, 0x1050, PN);
GLOBAL_ELEMENT(ACR_Operators_name        , 0x0008, 0x1070, PN);
GLOBAL_ELEMENT(ACR_Manufacturer_model    , 0x0008, 0x1090, LO);

GLOBAL_ELEMENT(ACR_Patient_name          , 0x0010, 0x0010, PN);
GLOBAL_ELEMENT(ACR_Patient_identification, 0x0010, 0x0020, LO);
GLOBAL_ELEMENT(ACR_Patient_birth_date    , 0x0010, 0x0030, DA);
GLOBAL_ELEMENT(ACR_Patient_sex           , 0x0010, 0x0040, CS);
GLOBAL_ELEMENT(ACR_Patient_age           , 0x0010, 0x1010, AS);
GLOBAL_ELEMENT(ACR_Patient_weight        , 0x0010, 0x1030, DS);

GLOBAL_ELEMENT(ACR_Scanning_sequence     , 0x0018, 0x0020, CS);
GLOBAL_ELEMENT(ACR_MR_acquisition_type   , 0x0018, 0x0023, CS);
GLOBAL_ELEMENT(ACR_Sequence_name         , 0x0018, 0x0024, CS);
GLOBAL_ELEMENT(ACR_Slice_thickness       , 0x0018, 0x0050, DS);
GLOBAL_ELEMENT(ACR_Repetition_time       , 0x0018, 0x0080, DS);
GLOBAL_ELEMENT(ACR_Echo_time             , 0x0018, 0x0081, DS);
GLOBAL_ELEMENT(ACR_Inversion_time        , 0x0018, 0x0082, DS);
GLOBAL_ELEMENT(ACR_Nr_of_averages        , 0x0018, 0x0083, DS);
GLOBAL_ELEMENT(ACR_Imaging_frequency     , 0x0018, 0x0084, DS);
GLOBAL_ELEMENT(ACR_Imaged_nucleus        , 0x0018, 0x0085, SH);
GLOBAL_ELEMENT(ACR_Echo_number           , 0x0018, 0x0086, IS);
GLOBAL_ELEMENT(ACR_Magnetic_field_strength,0x0018, 0x0087, DS);
GLOBAL_ELEMENT(ACR_Spacing_between_slices, 0x0018, 0x0088, DS);
GLOBAL_ELEMENT(ACR_Number_of_phase_encoding_steps, 0x0018, 0x0089, IS);
GLOBAL_ELEMENT(ACR_Echo_train_length     , 0x0018, 0x0091, IS);
GLOBAL_ELEMENT(ACR_Percent_sampling      , 0x0018, 0x0093, DS);
GLOBAL_ELEMENT(ACR_Percent_phase_field_of_view, 0x0018, 0x0094, DS);
GLOBAL_ELEMENT(ACR_Pixel_bandwidth       , 0x0018, 0x0095, DS);
GLOBAL_ELEMENT(ACR_Device_serial_number  , 0x0018, 0x1000, LO);
GLOBAL_ELEMENT(ACR_Software_versions     , 0x0018, 0x1020, LO);
GLOBAL_ELEMENT(ACR_Protocol_name         , 0x0018, 0x1030, LO);
GLOBAL_ELEMENT(ACR_Receiving_coil        , 0x0018, 0x1250, SH);
GLOBAL_ELEMENT(ACR_Transmitting_coil     , 0x0018, 0x1251, SH);
GLOBAL_ELEMENT(ACR_Acquisition_matrix    , 0x0018, 0x1310, US);
GLOBAL_ELEMENT(ACR_Phase_encoding_direction, 0x0018, 0x1312, CS);
GLOBAL_ELEMENT(ACR_Flip_angle            , 0x0018, 0x1314, DS);
GLOBAL_ELEMENT(ACR_SAR                   , 0x0018, 0x1316, DS);
GLOBAL_ELEMENT(ACR_Acq_comments          , 0x0018, 0x4000, LT);
GLOBAL_ELEMENT(ACR_Patient_position      , 0x0018, 0x5100, CS);

GLOBAL_ELEMENT(ACR_Study                 , 0x0020, 0x0010, SH);
GLOBAL_ELEMENT(ACR_Series                , 0x0020, 0x0011, IS);
GLOBAL_ELEMENT(ACR_Acquisition           , 0x0020, 0x0012, IS);
GLOBAL_ELEMENT(ACR_Image                 , 0x0020, 0x0013, IS);
GLOBAL_ELEMENT(ACR_Image_position_patient, 0x0020, 0x0032, DS);
GLOBAL_ELEMENT(ACR_Image_orientation_patient,
	                                   0x0020, 0x0037, DS);
GLOBAL_ELEMENT(ACR_Acquisitions_in_series, 0x0020, 0x1001, IS);

GLOBAL_ELEMENT(ACR_Rows                  , 0x0028, 0x0010, US);
GLOBAL_ELEMENT(ACR_Columns               , 0x0028, 0x0011, US);
GLOBAL_ELEMENT(ACR_Pixel_size            , 0x0028, 0x0030, DS);
GLOBAL_ELEMENT(ACR_Bits_allocated        , 0x0028, 0x0100, US);
GLOBAL_ELEMENT(ACR_Bits_stored           , 0x0028, 0x0101, US);
GLOBAL_ELEMENT(ACR_Smallest_pixel_value  , 0x0028, 0x0106, US);
GLOBAL_ELEMENT(ACR_Largest_pixel_value   , 0x0028, 0x0107, US);
GLOBAL_ELEMENT(ACR_Image_location        , 0x0028, 0x0200, US);
GLOBAL_ELEMENT(ACR_Window_centre         , 0x0028, 0x1050, DS);
GLOBAL_ELEMENT(ACR_Window_width          , 0x0028, 0x1051, DS);

//GLOBAL_ELEMENT(ACR_Pixel_data, ACR_ACTUAL_IMAGE_GID, 0x0010, UNKNOWN);
GLOBAL_ELEMENT(ACR_Pixel_data            , 0x7fe0, 0x0010, OW);

#include <spi_element_defs.h>
#include <ext_element_defs.h>




