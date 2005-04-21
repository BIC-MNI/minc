#ifndef _SIEMENS_HEADER_DEFS_H_
#define _SIEMENS_HEADER_DEFS_H_ 1

/* CONSTANTS */

#define N_STRING 26
#define N_AGE 4
#define N_DIAGNOSIS 40
#define N_NUCLEUS 8
#define N_MANUFACTURER 8
#define N_ORIENTATION 3
#define N_PATIENTID 12
#define N_SWVERSION 8

typedef double flt64_t;

/* ACR-NEMA specific types */

/* Analogous to DICOM field (0028, 0030) */
typedef struct pixel_spacing
{
    flt64_t row;
    flt64_t col;
} pixel_spacing_t;

/* Analogous to DICOM fields (0028, 1050) and (0028, 1051) */
typedef struct window
{
    int32_t x;
    int32_t y;
} window_t;

/* IMA specific types */
typedef struct ima_date
{
    int32_t year;               /* Full year including century */
    int32_t month;              /* Month from 1(Jan) to 12(Dec) */
    int32_t day;                /* Day of month from 1 to 31 */
} ima_date_t;

typedef struct ima_time
{
    int32_t hour;               /* Hour from 0 to 23 */
    int32_t minute;             /* Minute from 0 to 59 */
    int32_t second;             /* Second from 0 to 59 */
    int32_t msec;               /* Milliseconds from 0 to 999 */
} ima_time_t;

typedef struct ima_vector
{
    flt64_t x;
    flt64_t y;
    flt64_t z;
} ima_vector_t;

typedef enum ima_slice_order
{
    SO_ASCENDING = 1,
    SO_DESCENDING = 2,
    SO_FREE = 3,
    SO_INTERLEAVED = 4,
    SO_NONE = 5
} ima_slice_order_t;

typedef enum ima_position
{
    PP_LEFT = 1,
    PP_PRONE = 2,
    PP_RIGHT = 3,
    PP_SUPINE = 4,
} ima_position_t;

typedef enum
{
    RD_FEET = 1,
    RD_HEAD = 2
} ima_rest_direction_t;

typedef enum
{
    VD_FEET = 1,
    VD_HEAD = 2,
    VD_AtoP = 3,
    VD_LtoR = 4,
    VD_PtoA = 5,
    VD_RtoL = 6
} ima_view_direction_t;

typedef struct
{
    flt64_t height;
    flt64_t width;
} ima_field_of_view_t;

typedef struct
{
    char y[N_ORIENTATION + 1]; /* up - down */
    char x[N_ORIENTATION + 1]; /* left - right */
    char z[N_ORIENTATION + 1]; /* back - front */
} ima_orientation_t;

/*
 * Identifying information group 0x0008 
 */
struct ima_acr_0008             /* Item# FOffs SOffs */
{
    ima_date_t StudyDate;       /* 0020  0000 */
    ima_date_t AcquisitionDate; /* 0022  000C */
    ima_date_t ContentDate;     /* 0023  0018 */
    ima_time_t StudyTime;       /* 0030  0024 */
    ima_time_t AcquisitionTime; /* 0032  0034 */
    ima_time_t ContentTime;     /* 0033  0044 */
    char pad1[8];
    int32_t Modality;           /* 0060   005C */
    char Manufacturer[N_MANUFACTURER + 1]; /* 0070  0060  */
    char InstitutionName[N_STRING + 1]; /* 0080 0069 */
    char PhysicianName[N_STRING + 1]; /* 0090  0084 */
    char StationName[N_STRING + 1]; /* 1010  009F */
    char StudyDescription[N_STRING + 1]; /* 1030  00BA */
    char pad2[N_STRING + 1];
    char AdmittingDiagnoses[N_DIAGNOSIS + 1]; /* 1080  00F0 */
    char ModelName[N_STRING + 1]; /* 1090  0119 */
    char pad3[76];
};


/*
 * Patient information group 0x0010
 */
struct ima_acr_0010             /* Item# FOffs SOffs */
{
    char PatientName[N_STRING + 1]; /* 0010  0300 */
    char PatientID[N_PATIENTID + 1]; /* 0020  031B */
    ima_date_t PatientDOB;      /* 0030  0328 */
    int32_t PatientSex;         /* 0040  0334 */
    char PatientBirthName[N_STRING + 1]; /* 1005  0338 */
    char PatientAge[N_AGE + 1]; /* 1010  0353 */
    flt64_t PatientSize;        /* 1020  0358 */
    int32_t PatientWeight;      /* 1030  0360 */
    char pad1[156];
};

/*
 * Acquisition information group 0x0018
 */
struct ima_acr_0018
{                               /* Item# FOffs SOffs */
    char pad1[8];               /* Dummy padding */
    flt64_t SliceThickness;     /* 0050  0608 */
    char pad2[8];
    flt64_t RepetitionTime;     /* 0080  0618 */
    flt64_t EchoTime;           /* 0081  0620 */
    flt64_t InversionTime;      /* 0082  0628 */
    int32_t NumberOfAverages;   /* 0083  0630 */
    char pad3[4];               /* Dummy padding */
    flt64_t ImagingFrequency;   /* 0084  0638 */
    char pad4[4];               /* 0085  0640 */
    int32_t EchoNumber;         /* 0086  0644 */
    int32_t DataCollectionDiameter; /* 0090  0648 */
    char SerialNumber[N_STRING + 1]; /* 1000  064C */
    char SoftwareVersion[N_SWVERSION + 1]; /* 1020 0667 */
    char pad5[61];               /* Dummy padding */
    ima_date_t CalibrationDate;  /* 1200  06B0 */
    ima_time_t CalibrationTime;  /* 1201  06BC */
    char pad6[N_STRING + 1];
    char ReceiveCoilName[N_STRING + 1]; /* 1250  06E7 */
    char pad7[N_STRING + 1];
    ima_position_t PatientPosition; /* 5100  0720 */
    char ImagedNucleus[N_NUCLEUS + 1]; /* 0085  0724 */
    char pad8[80];              /* Pad to 384 bytes */
};

/*
 * Relationship information group 0x0020
 */
struct ima_acr_0020
{                               /* Item# FOffs */
    int32_t StudyID;            /* 0010  0C80 */
    char pad1[4];               /*  */
    int32_t AcquisitionNumber;  /* 0012  0C88 */
    int32_t InstanceNumber;     /* 0013  0C8C */
    int32_t ImagePosition[3];   /* 0030  0C90 */
    char pad2[4];               /* Dummy padding */
    flt64_t ImageOrientation[6]; /* 0035  0C94 */
    int32_t Location;           /* 0050  0CD0 */
    int32_t Laterality;         /* 0060  0CD4 */
    char pad3[4];
    int32_t AcquisitionsInSeries; /* 1001  0CDC */
    char pad4[416];             /* Pad to 512 bytes */
};

/*
 * Image presentation Information group 0x0028
 */
struct ima_acr_0028
{                               /* Item# FOffs */
    int16_t ImageDimension;     /* 0005  1380 */
    int16_t Rows;               /* 0010  1382 */
    int16_t Columns;            /* 0011  1384 */
    char pad1[2];
    pixel_spacing_t PixelSpacing; /* 0030  1388 */
    char pad2[8];
    int16_t BitsAllocated;      /* 0100  13A0 */
    int16_t BitsStored;         /* 0101  13A2 */
    int16_t HighBit;            /* 0102  13A4 */
    int16_t PixelRepresentation; /* 0103  13A6 */
    window_t WindowCenter;      /* 1050  13A8 */
    window_t WindowWidth;       /* 1051  13B0 */
    int32_t RescaleIntercept;   /* 1052  13B8 */
    int32_t RescaleSlope;       /* 1053  13BC */
    char pad3[192];             /* Pad to 256 bytes */
};

/***** Siemens private group structures *****/

/*
 * Siemens acquisition group 0x0019 
 */
struct ima_siemens_0019
{
    char pad1[20];              /* Padding */
    int32_t NumberOfDataBytes;  /* 1060 */
    char pad2[140];
    int32_t FourierLinesNominal; /* 1220 */
    char pad3[4];
    int32_t FourierLinesAfterZero; /* 1226 */
    int32_t FirstMeasuredFourierLine; /* 1228 */
    int32_t AcquisitionColumns; /* 1230 */
    int32_t ReconstructionColumns; /* 1231 */
    int32_t NumberOfAverages;   /* 1250 */
    flt64_t FlipAngle;          /* 1260 */
    int32_t NumberOfPrescans;   /* 1270 */
    char pad4[116];             /* Dummy padding */
    int32_t SaturationRegions;  /* 1290 */
    char pad5[316];             /* Dummy padding */
    flt64_t MagneticFieldStrength; /* 1412 */
    char pad6[631];             /* Dummy padding */
};

/*
 * Siemens Relationship group 0x0021
 */ 
struct ima_siemens_0021
{
    char pad1[32];              /* Dummy padding */
    ima_field_of_view_t FieldOfView;    /* 1120  0EA0 */
    ima_view_direction_t ViewDirection; /* 1130  0EB0 */
    ima_rest_direction_t RestDirection; /* 1132  0EB4 */
    ima_vector_t ImagePosition; /* 1160  0EB8 */
    ima_vector_t ImageNormal;   /* 1161  0ED0 */
    flt64_t ImageDistance;      /* 1163  0EE8 */
    char pad3[8];               /* Dummy padding */
    ima_vector_t ImageRow;      /* 116A  0EF8 */
    ima_vector_t ImageColumn;   /* 116B 0F10 */
    ima_orientation_t OrientationSet1; /* 1170 0F28 */
    ima_orientation_t OrientationSet2; /* 1171 0F34 */
    char StudyName[N_STRING + 1]; /* 1180 0F40 */
    int32_t StudyType;          /* 1182 */
    flt64_t ImageMagnificationFactor; /* 1122 */
    char pad4[40];
    int32_t NumberOf3DRawPartNom; /* 1330 */
    int32_t NumberOf3DRawPartCur; /* 1331 */
    int32_t NumberOf3DImaPart;  /* 1334 */
    int32_t Actual3DImaPartNumber; /* 1336 */
    char pad5[4];
    int32_t NumberOfSlicesNom;  /* 1340 */
    int32_t NumberOfSlicesCur;  /* 1341 */
    int32_t CurrentSliceNumber; /* 1342 */
    int32_t CurrentGroupNumber; /* 1343 */
    char pad7[88];              /* Dummy padding */
    int32_t NumberOfEchoes;     /* 1370 */
    char pad9[32];              /* Dummy padding */
    ima_slice_order_t SliceOrder; /* 134F */
    char pad10[4];              /* Dummy padding */
    flt64_t SlabThickness;      /* 1339 */
    char pad11[829];            /* Padding */
};

/* this is a work in progress, based on David Clunie's website */

#define PATIENT_NUMBER_SIZE 12
#define PATIENT_DATE_SIZE 11
#define PATIENT_POSITION_SIZE 11
#define IMAGE_NUMBER_SIZE 11
#define IMAGE_NUMBER_TEXT "IMAGE"
#define LABEL_SIZE 5
#define DATE_OF_MEASUREMENT_SIZE 11
#define TIME_OF_MEASUREMENT_SIZE 5
#define TIME_OF_ACQUISITION_SIZE 11
#define TIME_OF_ACQUISITION_TEXT_CT "TI"
#define TIME_OF_ACQUISITION_TEXT_MR "TA "
#define NUMBER_OF_ACQUISITIONS_SIZE 11
#define NUMBER_OF_ACQUISITIONS_TEXT "AC"
#define COMMENT_NO1_SIZE 26
#define COMMENT_NO2_SIZE 26
#define INSTALLATION_NAME_SIZE 26
#define SOFTWARE_VERSION_SIZE 11
#define MATRIX_SIZE 11
#define TYPE_OF_MEASUREMENT_SIZE 11
#define SCAN_NUMBER_SIZE 11
#define SCAN_NUMBER_TEXT "SCAN"
#define REPETITION_TIME_SIZE 11
#define REPETITION_TIME_TEXT "TR"
#define ECHO_TIME_SIZE 11
#define ECHO_TIME_TEXT "TE"
#define GATING_AND_TRIGGER_SIZE 11
#define GATING_AND_TRIGGER_TEXT "TD"
#define TUBE_CURRENT_SIZE 11
#define TUBE_CURRENT_TEXT "mA"
#define TUBE_VOLTAGE_SIZE 11
#define TUBE_VOLTAGE_TEXT "kV"
#define SLICE_THICKNESS_SIZE 11
#define SLICE_THICKNESS_TEXT "SL"
#define SLICE_POSITION_SIZE 11
#define SLICE_POSITION_TEXT "SP"
#define SLICE_ORIENTATION_NO1_SIZE 11
#define SLICE_ORIENTATION_NO2_SIZE SLICE_ORIENTATION_NO1_SIZE
#define COR_TEXT "Cor"
#define SAG_TEXT "Sag"
#define TRA_TEXT "Tra"
#define FIELD_OF_VIEW_SIZE 11
#define FIELD_OF_VIEW_TEXT "FoV"
#define ZOOM_CENTER_SIZE 11
#define ZOOM_CENTER_TEXT "CE"
#define ZOOM_CENTER_TEXT_MR "MF"
#define GANTRY_TILT_SIZE 11
#define GANTRY_TILT_TEXT "GT"
#define TABLE_POSITION_SIZE 11
#define TABLE_POSITION_TEXT "TP"
#define MIP_HEADLINE_SIZE 3
#define MIP_HEADLINE_TEXT "VOI"
#define MIP_LINE_SIZE 15
#define MIP_LINE_TEXT "Lin"
#define MIP_COLUMN_SIZE 15
#define MIP_COLUMN_TEXT "Col"
#define MIP_SLICE_SIZE 15
#define MIP_SLICE_TEXT "Sli"
#define STUDY_NUMBER_SIZE 11
#define STUDY_NUMBER_TEXT "STUDY"
#define CONTRAST_SIZE 5
#define CONTRAST_TEXT_CT "+C IV"
#define CONTRAST_TEXT_MR "+C   "
#define CONTRAST_TEXT_NONE "     "
#define PATIENT_BIRTHDATE_SIZE 11
#define SEQUENCE_INFO_SIZE 11
#define SATURATION_REGIONS_SIZE 11
#define SATURATION_REGIONS_TEXT "SAT"
#define DATA_SET_ID_SIZE 26
#define DATA_SET_ID_TEXT_STUDY "STU"
#define DATA_SET_ID_TEXT_IMAGE "IMA"
#define DATA_SET_ID_TEXT_DELIMITER "/"
#define MAGNIFICATION_FACTOR_SIZE 11
#define MAGNIFICATION_FACTOR_TEXT "MF"
#define MANUFACTURER_MODEL_SIZE 26
#define PATIENT_NAME_SIZE 26
#define TIME_OF_SCANNING_SIZE 8

typedef struct text_info
{
    char PatientNumber[PATIENT_NUMBER_SIZE + 1]; /* Patient Id */
    char PatientSexAndAge[PATIENT_DATE_SIZE + 1]; /* Patient Sex, Patient Age */
    char PatientPosition[PATIENT_POSITION_SIZE + 1]; /* Patient Rest Direction, ... */
    char ImageNumber[IMAGE_NUMBER_SIZE + 1]; /* Image */
    char Label[LABEL_SIZE + 1]; /* Archiving Mark Mask, ... */
    char DateOfMeasurement[DATE_OF_MEASUREMENT_SIZE + 1]; /* Acquisition Date */
    char TimeOfMeasurement[TIME_OF_MEASUREMENT_SIZE + 1]; /* Acquisition Time */
    char TimeOfAcquisition[TIME_OF_ACQUISITION_SIZE + 1]; /* CT: Exposure Time MR:
                                                             Total Measurement Time */
    char NumberOfAcquisitions[NUMBER_OF_ACQUISITIONS_SIZE + 1]; /* Number of Averages */
    char CommentNo1[COMMENT_NO1_SIZE + 1]; /* Procedure Description */
    char CommentNo2[COMMENT_NO2_SIZE + 1];
    char InstallationName[INSTALLATION_NAME_SIZE + 1]; /* Institution ID */
    char SoftwareVersion[SOFTWARE_VERSION_SIZE + 1]; /* Software Version */
    char Matrix[MATRIX_SIZE + 1]; /* Rows, Columns */
    char TypeOfMeasurement[TYPE_OF_MEASUREMENT_SIZE + 1]; /* Calculation Mode */
    char ScanNumber[SCAN_NUMBER_SIZE + 1]; /* Acquisition */
    char RepetitionTime[REPETITION_TIME_SIZE + 1]; /* Repetition Time */
    char EchoTime[ECHO_TIME_SIZE + 1]; /* Echo Time */
    char GatingAndTrigger[GATING_AND_TRIGGER_SIZE + 1]; /* Signal Mask */
    char TubeCurrent[TUBE_CURRENT_SIZE + 1]; /* Exposure */
    char TubeVoltage[TUBE_VOLTAGE_SIZE + 1]; /* Generator Power */
    char SliceThickness[SLICE_THICKNESS_SIZE + 1];	   /* Slice Thickness */
    char SlicePosition[SLICE_POSITION_SIZE + 1]; /* Image Distance */
    char SliceOrientationNo1[SLICE_ORIENTATION_NO1_SIZE + 1]; /* Image Position, ... */
    char SliceOrientationNo2[SLICE_ORIENTATION_NO2_SIZE + 1];
    char FieldOfView[FIELD_OF_VIEW_SIZE + 1]; /* Field of View */
    char ZoomCenter[ZOOM_CENTER_SIZE + 1]; /* Target */
    char GantryTilt[GANTRY_TILT_SIZE + 1]; /* Gantry Tilt */
    char TablePosition[TABLE_POSITION_SIZE + 1]; /* Location */
    char MipHeadLine[MIP_HEADLINE_SIZE + 1]; /* <string> */
    char MipLine[MIP_LINE_SIZE + 1]; /* MIP x Row */
    char MipColumn[MIP_COLUMN_SIZE + 1]; /* MIP x Column */
    char MipSlice[MIP_SLICE_SIZE + 1]; /* MIP x Slice */
    char StudyNumber[STUDY_NUMBER_SIZE + 1]; /* Study */
    char Contrast[CONTRAST_SIZE + 1]; /* Contrast Agent */
    char PatientBirthdate[PATIENT_BIRTHDATE_SIZE + 1]; /* Patient Birthday */
    char SequenceInformation[SEQUENCE_INFO_SIZE + 1]; /* Sequence File Owner, ... */
    char SaturationRegions[SATURATION_REGIONS_SIZE + 1]; /* Saturation Regions, ... */
    char DataSetId[DATA_SET_ID_SIZE + 1]; /* Image, Study */
    char MagnificationFactor[MAGNIFICATION_FACTOR_SIZE + 1]; /* Image Maginification Factor */
    char ManufacturerModel[MANUFACTURER_MODEL_SIZE + 1]; /* Manufacturer Model */
    char PatientName[PATIENT_NAME_SIZE + 1]; /* Patient Name */
    char TimeOfScanning[TIME_OF_SCANNING_SIZE + 1]; /* Acquisition Time */
} text_info_t;

#if 0
struct text_info {
    char PatientID[12+1];       /* 5504 */
    char PatientSex[1];         /* 5517 */
    char PatientAge[3];         /* 5518 */
    char PatientAgeUnits[1];    /* 5521 */
    char pad1[7];               /* 5522 */
    char PatientPosition[12];   /* 5529 */
    char ImageNumberFlag[5];    /* 5541 */
    char ImageNumber[3];        /* 5546 */
    char pad2[10];              /* 5551 */
    char Date[11+1];            /* 5559 */
    char Time[5+1];             /* 5571 */
    char AcquisitionTimeFlag[6]; /* 5577 */
    char AcquisitionTime[5+1];  /* 5583 */
    char AcquisitionCountFlag[6]; /* 5589 */
    char AcquisitionCount[5+1]; /* 5595 */
    char Annotation[27];         /* 5601 */
    char AdmittingDiagnosis[27]; /* 5628 */
    char Organization[27];      /* 5655 */
    char Station[12];           /* 5682 */
    char AcquisitionMatrixPhase[3]; /* 5695 */
    char AcquisitionMatrixPhaseAxis[1];
    char AcquisitionMatrixFreq[3];
    char AcquisitionMatrixFreq0[1];
    char AcquisitionMatrixFreqS[1];
    char Sequence[8];
    char FlipAngle[3];
    char ScanNumberFlag[4];
    char ScanNumberA[3];
    char ScanNumberB[3];
    char RepetitionTimeFlag[2];
    char RepetitionTime[7];
    char EchoTimeFlag[2];
    char EchoTime[5];
    char EchoNumber[1];
    char SliceThicknessFlag[2];
    char SliceThickness[7];
    char SlicePositionFlag[2];
    char SlicePosition[7];
    char AngleFlag1[3];
    char AngleFlag2[1];
    char AngleFlag3[3];
    char Angle[4];
    char FOVFlag[3];
    char FOVH[3];
    char FOVV[3];
    char TablePositionFlag[2];
    char TablePosition[7];
    char StudyNumberFlag[5];
    char StudyNumber[2];
    char DOBDD[2];
    char DOBMM[3];
    char DOBYYYY[4];
    char StudyNumberFlag2[3];
    char ImageNumberFlag2[3];
    char StudyNumber2[2];
    char ImageNumber2[2];
    char StudyImageNumber3[5];
    char ModelName[15];
    char PatientName[27];       /* 6058 */
    char ScanStartTimeHH[3];    /* 6085 */
    char ScanStartTimeMM[3];    /* 6088 */
    char ScanStartTimeSS[3];    /* 6091 */
};
#endif

/* Siemens IMA header - total size is 0x1800 bytes */

typedef struct
{                               /* Offset - Description */
    struct ima_acr_0008 G08;    /* 0x0000 - Identifying Information */
    char G09[0x0180];           /* 0x0180 - Siemens specific */
    struct ima_acr_0010 G10;    /* 0x0300 - Patient Information */
    char G11[0x0080];           /* 0x0400 - Siemens specific */
    char G13[0x0180];           /* 0x0480 - Siemens specific */
    struct ima_acr_0018 G18;    /* 0x0600 - Acquisition Information */
    struct ima_siemens_0019 G19; /* 0x0780 - Siemens specific */
    struct ima_acr_0020 G20;   /* 0x0C80 - Relationship Information */
    struct ima_siemens_0021 G21;    /* 0x0E80 - Siemens specific */
    struct ima_acr_0028 G28; /* 0x1380 - Image Presentation Information */
    char G29[0x0100];           /* 0x1480 - Siemens specific */
    text_info_t ti;
} ima_header_t;

#endif /* _SIEMENS_HEADER_DEFS_H_ */
