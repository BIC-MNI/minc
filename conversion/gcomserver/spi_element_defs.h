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

/* Element id's for ACR-NEMA */
GLOBAL_ELEMENT(ACR_Length_to_eom   , 0x0000, 0x0001);
GLOBAL_ELEMENT(ACR_Recognition_code, 0x0000, 0x0010);
GLOBAL_ELEMENT(ACR_Command         , 0x0000, 0x0100);
GLOBAL_ELEMENT(ACR_Message_id      , 0x0000, 0x0110);
GLOBAL_ELEMENT(ACR_Message_id_brt  , 0x0000, 0x0120);
GLOBAL_ELEMENT(ACR_Initiator       , 0x0000, 0x0200);
GLOBAL_ELEMENT(ACR_Receiver        , 0x0000, 0x0300);
GLOBAL_ELEMENT(ACR_Dataset_type    , 0x0000, 0x0800);
GLOBAL_ELEMENT(ACR_Status          , 0x0000, 0x0900);
GLOBAL_ELEMENT(ACR_Data_set_type   , 0x0008, 0x0040);
GLOBAL_ELEMENT(ACR_Data_set_subtype, 0x0008, 0x0041);
GLOBAL_ELEMENT(ACR_Patient_name    , 0x0010, 0x0010);
GLOBAL_ELEMENT(ACR_Study           , 0x0020, 0x0010);
GLOBAL_ELEMENT(ACR_Acquisition     , 0x0020, 0x0012);
GLOBAL_ELEMENT(ACR_Image           , 0x0020, 0x0013);

GLOBAL_ELEMENT(ACR_Image_location  , 0x0028, 0x0200);

/* Element id's for SPI */
GLOBAL_ELEMENT(SPI_Recognition_code, 0x0001, 0x0010);
GLOBAL_ELEMENT(SPI_Command         , 0x0001, 0x0018);
GLOBAL_ELEMENT(SPI_Status          , 0x0001, 0x0019);
GLOBAL_ELEMENT(SPI_Session_id      , 0x0001, 0x0028);
GLOBAL_ELEMENT(SPI_Dataset_type    , 0x0001, 0x0040);
GLOBAL_ELEMENT(SPI_Delay_time      , 0x0001, 0x0060);
GLOBAL_ELEMENT(SPI_Nr_data_objects , 0x0001, 0x0081);
GLOBAL_ELEMENT(SPI_Operator_text   , 0x0001, 0x1060);
GLOBAL_ELEMENT(SPI_Log_info        , 0x0001, 0x1070);
GLOBAL_ELEMENT(SPI_Volume_name     , 0x0001, 0x1080);
GLOBAL_ELEMENT(SPI_Creation_date   , 0x0001, 0x1081);
GLOBAL_ELEMENT(SPI_Creation_time   , 0x0001, 0x1082);
GLOBAL_ELEMENT(SPI_Volume_type     , 0x0001, 0x1083);
GLOBAL_ELEMENT(SPI_Volume_status   , 0x0001, 0x1084);
GLOBAL_ELEMENT(SPI_Space_left      , 0x0001, 0x1085);
GLOBAL_ELEMENT(SPI_Nr_exams        , 0x0001, 0x1086);
GLOBAL_ELEMENT(SPI_Nr_images       , 0x0001, 0x1087);
GLOBAL_ELEMENT(SPI_Closing_date    , 0x0001, 0x1088);
GLOBAL_ELEMENT(SPI_Perc_space_used , 0x0001, 0x1089);

/* Define group numbers for message stuff */
#define ACR_MESSAGE_GID 0
#define SPI_MESSAGE_GID 1
#define ACR_IDENTIFYING_GID 0x0008
#define ACR_IMAGE_GID 0x0028
#define SPI_SCANDATA_GID 0x8001
#define SPI_SPECTRADATA_GID 0x8101
#define SPI_RAWDATA_GID 0x8201
#define SPI_LABELDATA_GID 0x8301

/* Define recognition codes */
#define ACR_RECOGNITION_CODE "ACR-NEMA 1.0"
#define SPI_RECOGNITION_CODE "SPI Release 1 "

/* Define commands */
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

/* Define data object types */
#define ACR_IMAGE_OBJECT 0x0000
#define ACR_OTHER_OBJECT 0x0100

/* Define data object sub-types */
#define SPI_SCANDATA_OBJECT    "SCAN_DATA"
#define SPI_SPECTRADATA_OBJECT "SPECTRA_DATA"
#define SPI_RAWDATA_OBJECT     "RAW_DATA"
#define SPI_LABELDATA_OBJECT   "LAB_DATA"
