/*[-  HEADER FILE  -------------------------------------------------------------------------*/
/*
   Name:        ds_head_image_text_type.h

   Description: The header file defines the data set image text for internal
                use (internal image text area).

                The image text parameter names are not NEMA item names. Parameters with
                equal names can but must not contain equal information.

                The sign "->" means, this parameter is derivated from listed NEMA items.

                For more information please see document "Data Set Format" chapter "Group
                0051'H Image Text".


   Author:      THUMSER, Andreas (TH); Siemens AG UBMed CMS/SCE64; phone: 09131 844797
*/
/*]-----------------------------------------------------------------------------------------*/

#ifndef DS_HEAD_IMAGE_TEXT_TYPE
#define DS_HEAD_IMAGE_TEXT_TYPE

/* PRECOMPILER: define length constants */
#define DS_PATIENT_NUMBER_SIZE                                                             12

#define DS_PATIENT_DATE_SIZE                                                               11

#define DS_PATIENT_POSITION_SIZE                                                           11

#define DS_IMAGE_NUMBER_SIZE                                                               11
#define DS_IMAGE_NUMBER_TEXT                                                          "IMAGE"

#define DS_LABEL_SIZE                                                                       5

#define DS_DATE_OF_MEASUREMENT_SIZE                                                        11

#define DS_TIME_OF_MEASUREMENT_SIZE                                                         5

#define DS_TIME_OF_ACQUISITION_SIZE                                                        11
#define DS_TIME_OF_ACQUISITION_TEXT_CT                                                   "TI"
#define DS_TIME_OF_ACQUISITION_TEXT_MR                                                  "TA "

#define DS_NUMBER_OF_ACQUISITIONS_SIZE                                                     11
#define DS_NUMBER_OF_ACQUISITIONS_TEXT                                                   "AC"

#define DS_COMMENT_NO1_SIZE                                                                26

#define DS_COMMENT_NO2_SIZE                                                                26

#define DS_INSTALLATION_NAME_SIZE                                                          26

#define DS_SOFTWARE_VERSION_SIZE                                                           11

#define DS_MATRIX_SIZE                                                                     11

#define DS_TYPE_OF_MEASUREMENT_SIZE                                                        11

#define DS_SCAN_NUMBER_SIZE                                                                11
#define DS_SCAN_NUMBER_TEXT                                                            "SCAN"

#define DS_REPETITION_TIME_SIZE                                                            11
#define DS_REPETITION_TIME_TEXT                                                          "TR"

#define DS_ECHO_TIME_SIZE                                                                  11
#define DS_ECHO_TIME_TEXT                                                                "TE"

#define DS_GATING_AND_TRIGGER_SIZE                                                         11
#define DS_GATING_AND_TRIGGER_TEXT                                                       "TD"

#define DS_TUBE_CURRENT_SIZE                                                               11
#define DS_TUBE_CURRENT_TEXT                                                             "mA"

#define DS_TUBE_VOLTAGE_SIZE                                                               11
#define DS_TUBE_VOLTAGE_TEXT                                                             "kV"

#define DS_SLICE_THICKNESS_SIZE                                                            11
#define DS_SLICE_THICKNESS_TEXT                                                          "SL"

#define DS_SLICE_POSITION_SIZE                                                             11
#define DS_SLICE_POSITION_TEXT                                                           "SP"

#define DS_SLICE_ORIENTATION_NO1_SIZE                                                      11
#define DS_SLICE_ORIENTATION_NO2_SIZE                           DS_SLICE_ORIENTATION_NO1_SIZE
#define DS_COR_TEXT                                                                     "Cor"
#define DS_SAG_TEXT                                                                     "Sag"
#define DS_TRA_TEXT                                                                     "Tra"

#define DS_FIELD_OF_VIEW_SIZE                                                              11
#define DS_FIELD_OF_VIEW_TEXT                                                           "FoV"

#define DS_ZOOM_CENTER_SIZE                                                                11
#define DS_ZOOM_CENTER_TEXT                                                              "CE"
#define DS_ZOOM_CENTER_TEXT_MR                                                           "MF"

#define DS_GANTRY_TILT_SIZE                                                                11
#define DS_GANTRY_TILT_TEXT                                                              "GT"

#define DS_TABLE_POSITION_SIZE                                                             11
#define DS_TABLE_POSITION_TEXT                                                           "TP"

#define DS_MIP_HEADLINE_SIZE                                                                3
#define DS_MIP_HEADLINE_TEXT                                                            "VOI"

#define DS_MIP_LINE_SIZE                                                                   15
#define DS_MIP_LINE_TEXT                                                                "Lin"

#define DS_MIP_COLUMN_SIZE                                                                 15
#define DS_MIP_COLUMN_TEXT                                                              "Col"

#define DS_MIP_SLICE_SIZE                                                                  15
#define DS_MIP_SLICE_TEXT                                                               "Sli"

#define DS_STUDY_NUMBER_SIZE                                                               11
#define DS_STUDY_NUMBER_TEXT                                                          "STUDY"

#define DS_CONTRAST_SIZE                                                                    5
#define DS_CONTRAST_TEXT_CT                                                           "+C IV"
#define DS_CONTRAST_TEXT_MR                                                           "+C   "
#define DS_CONTRAST_TEXT_NONE                                                         "     "

#define DS_PATIENT_BIRTHDATE_SIZE                                                          11

#define DS_SEQUENCE_INFO_SIZE                                                              11

#define DS_SATURATION_REGIONS_SIZE                                                         11
#define DS_SATURATION_REGIONS_TEXT                                                      "SAT"

#define DS_DATA_SET_ID_SIZE                                                                26
#define DS_DATA_SET_ID_TEXT_STUDY                                                       "STU"
#define DS_DATA_SET_ID_TEXT_IMAGE                                                       "IMA"
#define DS_DATA_SET_ID_TEXT_DELIMITER                                                     "/"

#define DS_MAGNIFICATION_FACTOR_SIZE                                                       11
#define DS_MAGNIFICATION_FACTOR_TEXT                                                     "MF"

#define DS_MANUFACTURER_MODEL_SIZE                                                         26

#define DS_PATIENT_NAME_SIZE                                                               26

#define DS_TIME_OF_SCANNING_SIZE                                                            8



/* DECLARATION: image text type */

typedef struct image_text_tag
{
  char PatientNumber[DS_PATIENT_NUMBER_SIZE + 1];	   /* -> Patient Id */
  char PatientSexAndAge[DS_PATIENT_DATE_SIZE + 1];	   /* -> Patient Sex, Patient Age */
  char PatientPosition[DS_PATIENT_POSITION_SIZE + 1];	   /* -> Patient Rest Direction, ... */
  char ImageNumber[DS_IMAGE_NUMBER_SIZE + 1];		   /* -> Image */
  char Label[DS_LABEL_SIZE + 1];			   /* -> Archiving Mark Mask, ... */
  char DateOfMeasurement[DS_DATE_OF_MEASUREMENT_SIZE + 1]; /* -> Acquisition Date */
  char TimeOfMeasurement[DS_TIME_OF_MEASUREMENT_SIZE + 1]; /* -> Acquisition Time */
  char TimeOfAcquisition[DS_TIME_OF_ACQUISITION_SIZE + 1]; /* -> CT: Exposure Time -> MR:
							      Total Measurement Time */
  char NumberOfAcquisitions[DS_NUMBER_OF_ACQUISITIONS_SIZE + 1];	/* -> Number of Averages */
  char CommentNo1[DS_COMMENT_NO1_SIZE + 1];		   /* -> Procedure Description */
  char CommentNo2[DS_COMMENT_NO2_SIZE + 1];
  char InstallationName[DS_INSTALLATION_NAME_SIZE + 1];	   /* -> Institution ID */
  char SoftwareVersion[DS_SOFTWARE_VERSION_SIZE + 1];	   /* -> Software Version */
  char Matrix[DS_MATRIX_SIZE + 1];			   /* -> Rows, Columns */
  char TypeOfMeasurement[DS_TYPE_OF_MEASUREMENT_SIZE + 1]; /* -> Calculation Mode */
  char ScanNumber[DS_SCAN_NUMBER_SIZE + 1];		   /* -> Acquisition */
  char RepetitionTime[DS_REPETITION_TIME_SIZE + 1];	   /* -> Repetition Time */
  char EchoTime[DS_ECHO_TIME_SIZE + 1];			   /* -> Echo Time */
  char GatingAndTrigger[DS_GATING_AND_TRIGGER_SIZE + 1];   /* -> Signal Mask */
  char TubeCurrent[DS_TUBE_CURRENT_SIZE + 1];		   /* -> Exposure */
  char TubeVoltage[DS_TUBE_VOLTAGE_SIZE + 1];		   /* -> Generator Power */
  char SliceThickness[DS_SLICE_THICKNESS_SIZE + 1];	   /* -> Slice Thickness */
  char SlicePosition[DS_SLICE_POSITION_SIZE + 1];	   /* -> Image Distance */
  char SliceOrientationNo1[DS_SLICE_ORIENTATION_NO1_SIZE + 1];	/* -> Image Position, ... */
  char SliceOrientationNo2[DS_SLICE_ORIENTATION_NO2_SIZE + 1];
  char FieldOfView[DS_FIELD_OF_VIEW_SIZE + 1];		   /* -> Field of View */
  char ZoomCenter[DS_ZOOM_CENTER_SIZE + 1];		   /* -> Target */
  char GantryTilt[DS_GANTRY_TILT_SIZE + 1];		   /* -> Gantry Tilt */
  char TablePosition[DS_TABLE_POSITION_SIZE + 1];	   /* -> Location */
  char MipHeadLine[DS_MIP_HEADLINE_SIZE + 1];		   /* -> <string> */
  char MipLine[DS_MIP_LINE_SIZE + 1];			   /* -> MIP x Row */
  char MipColumn[DS_MIP_COLUMN_SIZE + 1];		   /* -> MIP x Column */
  char MipSlice[DS_MIP_SLICE_SIZE + 1];			   /* -> MIP x Slice */
  char StudyNumber[DS_STUDY_NUMBER_SIZE + 1];		   /* -> Study */
  char Contrast[DS_CONTRAST_SIZE + 1];			   /* -> Contrast Agent */
  char PatientBirthdate[DS_PATIENT_BIRTHDATE_SIZE + 1];    /* -> Patient Birthday */
  char SequenceInformation[DS_SEQUENCE_INFO_SIZE + 1];     /* -> Sequence File Owner, ... */
  char SaturationRegions[DS_SATURATION_REGIONS_SIZE + 1];  /* -> Saturation Regions, ... */
  char DataSetId[DS_DATA_SET_ID_SIZE + 1];                 /* -> Image, Study */
  char MagnificationFactor[DS_MAGNIFICATION_FACTOR_SIZE + 1]; /* -> Image Maginification Factor */
  char ManufacturerModel[DS_MANUFACTURER_MODEL_SIZE + 1];  /* -> Manufacturer Model */
  char PatientName[DS_PATIENT_NAME_SIZE + 1];              /* -> Patient Name */
  char TimeOfScanning[DS_TIME_OF_SCANNING_SIZE + 1];	   /* -> Acquisition Time */
} image_text_t;

#endif
