/* ----------------------------- MNI Header -----------------------------------
@NAME       : spi_element_defs.h
@DESCRIPTION: Element definitions for spi
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 23, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */

/* Element id's for ACR-NEMA */
GLOBAL_ELEMENT(ACR_Length_to_eom   , 0x0, 0x0001);
GLOBAL_ELEMENT(ACR_Recognition_code, 0x0, 0x0010);
GLOBAL_ELEMENT(ACR_Command         , 0x0, 0x0100);
GLOBAL_ELEMENT(ACR_Message_id      , 0x0, 0x110);
GLOBAL_ELEMENT(ACR_Message_id_brt  , 0x0, 0x120);
GLOBAL_ELEMENT(ACR_Initiator       , 0x0, 0x200);
GLOBAL_ELEMENT(ACR_Receiver        , 0x0, 0x300);
GLOBAL_ELEMENT(ACR_Dataset_type    , 0x0, 0x800);
GLOBAL_ELEMENT(ACR_Status          , 0x0, 0x900);

/* Element id's for SPI */
GLOBAL_ELEMENT(SPI_Recognition_code, 0x1, 0x0010);
GLOBAL_ELEMENT(SPI_Command         , 0x1, 0x0018);
GLOBAL_ELEMENT(SPI_Session_id      , 0x1, 0x0028);
GLOBAL_ELEMENT(SPI_Nr_data_objects , 0x1, 0x0081);
GLOBAL_ELEMENT(SPI_Operator_text   , 0x1, 0x1060);
GLOBAL_ELEMENT(SPI_Log_info        , 0x1, 0x1070);

/* Define group numbers for message stuff */
#define ACR_MESSAGE_GID 0
#define SPI_MESSAGE_GID 1

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
