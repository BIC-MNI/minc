#ifndef _SIEMENS_HEADER_DEFS_H_
#define _SIEMENS_HEADER_DEFS_H_ 1

/* CONSTANTS */

#define N_STRING 26
#define N_AGE 4
#define N_DIAGNOSIS 40
#define N_FILENAME 64
#define N_NUCLEUS 8
#define N_MANUFACTURER 8
#define N_ORIENTATION 3
#define N_PATIENTID 12
#define N_SEQ_INFO 8
#define N_SWVERSION 8

typedef double flt64_t;

/* TYPES */
typedef struct 
{
    int32_t Year;                  /* four digits e.g. 1989 */
    int32_t Month;                 /* 1 - 12 */
    int32_t Day;                   /* 1 - 31 */
} ima_date_t;


typedef struct 
{
    int32_t Hour;                  /* 0 - 23 */
    int32_t Minute;                /* 0 - 59 */
    int32_t Second;                /* 0 - 59 */
    int32_t Fraction;              /* 0 - 999 */
} ima_time_t;

typedef struct 
{
    int32_t M;
    int32_t S;
} data_set_subtype_t;

typedef struct
{
    flt64_t X;
    flt64_t Y;
    flt64_t Z;
} ima_vector_t;

typedef enum 
{
    Slice_Order_ASCENDING = 1,
    Slice_Order_DECREASING = 2,
    Slice_Order_FREE = 3,
    Slice_Order_INTERLEAVED = 4,
    Slice_Order_NONE = 5,
} order_of_slices_t;

typedef enum 
{
    Position_LEFT = 1,
    Position_PRONE = 2,
    Position_RIGHT = 3,
    Position_SUPINE = 4,
} patient_position_t;

typedef struct
{
    flt64_t Row;
    flt64_t Col;
} pixel_size_t;

/* Analogous to DICOM fields (0028, 1050) and (0028, 1051) */
typedef struct 
{
    int32_t X;
    int32_t Y;
} windows_t;

/*****************************************************/
/* Identifying Information (From DICOM group 0x0008) */
/*****************************************************/

struct ima_acr_0008             /* Item# FOffs SOffs */
{
    ima_date_t StudyDate;       /* 0020  0000 */
    ima_date_t AcquisitionDate; /* 0022  000C */
    ima_date_t ImageDate;       /* 0023  0018 */
    ima_time_t StudyTime;       /* 0030  0024 */
    ima_time_t AcquisitionTime; /* 0032  0034 */
    ima_time_t ImageTime;       /* 0033  0044 */
    data_set_subtype_t DataSetSubtype; /* 0041  0054 */
    int32_t Modality;           /* 0060   005C */
    char Manufacturer[N_MANUFACTURER + 1]; /* 0070  0060  */
    char InstitutionName[N_STRING + 1]; /* 0080 0069 */
    char ReferringPhysician[N_STRING + 1]; /* 0090  0084 */
    char StationID[N_STRING + 1]; /* 1010  009F */
    char ProcedureDescription1[N_STRING + 1]; /* 1030  00BA */
    char ProcedureDescription2[N_STRING + 1]; /* XXXX  00D5 */
    char AdmittingDiagnosis[N_DIAGNOSIS + 1]; /* 1080  00F0 */
    char ModelName[N_STRING + 1]; /* 1090  0119 */
    char pad1[76];
};


/**************************************/
/* Patient Information (Group 0x0010) */
/**************************************/

struct ima_acr_0010             /* Item# FOffs SOffs */
{
    char PatientName[N_STRING + 1]; /* 0010  0300 */
    char PatientID[N_PATIENTID + 1]; /* 0020  031B */
    ima_date_t PatientDOB;      /* 0030  0328 */
    int32_t PatientSex;         /* 0040  0334 */
    char PatientMaidenName[N_STRING + 1]; /* 1005  0338 */
    char PatientAge[N_AGE + 1]; /* 1010  0353 */
    flt64_t PatientSize;        /* 1020  0358 */
    int32_t PatientWeight;      /* 1030  0360 */
    char pad1[156];
};

/******************************************/
/* Acquisition Information (Group 0x0018) */
/******************************************/

struct ima_acr_0018
{
    int32_t Contrast;           /* 0010  0600 */
    char pad1[4];               /* Dummy padding */
    flt64_t SliceThickness;     /* 0050  0608 */
    int32_t GeneratorVoltage;   /* 0060  0610 */
    int32_t GeneratorVoltageDual; /* 0060  0614 */
    flt64_t RepetitionTime;     /* 0080  0618 */
    flt64_t EchoTime;           /* 0081  0620 */
    flt64_t InversionTime;      /* 0082  0628 */
    int32_t NumberOfAverages;   /* 0083  0630 */
    char pad2[4];               /* Dummy padding */
    flt64_t FrequencyMHz;       /* 0084  0638 */
    char pad3[4];               /* 0085  0640 */
    int32_t EchoNumber;         /* 0086  0644 */
    int32_t DataCollectionDiameter; /* 0090  0648 */
    char SerialNumber[N_STRING + 1]; /* 1000  064C */
    char SoftwareVersion[N_SWVERSION + 1]; /* 1020 0667 */
    char pad4[61];               /* Dummy padding */
    ima_date_t CalibrationDate;  /* 1200  06B0 */
    ima_time_t CalibrationTime;  /* 1201  06BC */
    char ConvolutionKernel[N_STRING + 1]; /* 1210 */
    char ReceivingCoil[N_STRING + 1]; /* 1250  06E7 */
    char pad5[N_STRING + 1]; /* XXXX   */
    patient_position_t PatientPosition; /* 5100 1824 */
    char ImagedNucleus[N_NUCLEUS + 1]; /* 0085  0724 */
    char pad6[80];              /* Pad to 384 bytes */
};

/*******************************************/
/* Relationship Information (Group 0x0020) */
/*******************************************/

struct ima_acr_0020
{
    int32_t Study;              /* 0010 */
    char pad1[4];               /*  */
    int32_t Acquisition;        /* 0012 */
    int32_t Image;              /* 0013 */
    int32_t ImagePosition[3];   /* 0030 */
    char pad2[4];               /* Dummy padding */
    flt64_t ImageOrientation[6]; /* 0035 */
    int32_t Location;           /* 0050 */
    int32_t Laterality;         /* 0060 */
    int32_t ImageGeometryType;  /* 0070 */
    int32_t AcquisitionsInSeries; /* 1001 */
    char pad3[416];             /* Pad to 512 bytes */
};


/*************************************************/
/* Image Presentation Information (Group 0x0028) */
/*************************************************/

struct ima_acr_0028
{
    int16_t ImageDimension;     /* 0005 */
    int16_t Rows;               /* 0010 */
    int16_t Columns;            /* 0011 */
    pixel_size_t PixelSize;     /* 0030 */
    int32_t ImageFormat;        /* 0040 */
    int32_t CompressionCode;    /* 0060 */
    int16_t BitsAllocated;      /* 0100 */
    int16_t BitsStored;         /* 0101 */
    int16_t HighBit;            /* 0102 */
    int16_t PixelRepresentation; /* 0103 */
    windows_t WindowCenter;     /* 1050 */
    windows_t WindowWidth;      /* 1051 */
    int32_t RescaleIntercept;   /* 1052 */
    int32_t RescaleSlope;       /* 1053 */
    char pad1[192];             /* Pad to 256 bytes */
};

/* SIEMENS SPECIFIC DATA - This may include proprietary stuff!!! */

typedef enum
{
    Rest_FEET = 1,
    Rest_HEAD = 2
} rest_direction_t;

typedef enum
{
    View_FEET = 1,
    View_HEAD = 2,
    View_AtoP = 3,
    View_LtoR = 4,
    View_PtoA = 5,
    View_RtoL = 6
} view_direction_t;

typedef struct
{
    flt64_t X;
    flt64_t Y;
    flt64_t Z;
} gradient_delay_time_t;

typedef struct
{
    flt64_t Height;
    flt64_t Width;
} field_of_view_t;

typedef struct
{
    char Y[N_ORIENTATION + 1]; /* up - down */
    char X[N_ORIENTATION + 1]; /* left - right */
    char Z[N_ORIENTATION + 1]; /* back - front */
} patient_orientation_t;

/***** Siemens private group structures *****/

/* Identifying information (Private group 0x0009) */
struct ima_siemens_0009         /* Item# FOffs SOffs */
{
    char pad1[28];
    ima_date_t LastMoveDate;    /* 1226  019C */
    ima_time_t LastMoveTime;    /* 1227  01A8 */
    char pad2[328];
};

/* Patient information (Private group 0x0011) */
struct ima_siemens_0011
{
    char pad1[12];
    ima_date_t RegistrationDate; /* 1110  */
    ima_time_t RegistrationTime; /* 1111  */
    char pad2[88];
};

/* Acquisition Information (Private group 0x0019) */
struct ima_siemens_0019
{
    int32_t NetFrequency;       /* 1010 */
    int32_t MeasurementMode;    /* 1020 */
    int32_t CalculationMode;    /* 1030 */
    char pad1[4];               /* Unknown?? */
    int32_t NoiseLevel;         /* 1050 */
    int32_t NumberOfDataBytes;  /* 1060 */
    char pad2[104];
    flt64_t TotalMeasurementTime; /* 1210 */
    flt64_t StartDelayTime;     /* 1212 */
    int32_t NumberOfPhases;     /* 1214 */
    int32_t SequenceControlMask[2]; /* 1216 */
    char pad3[8];               /*  */
    int32_t NumberOfFourierLinesNominal; /* 1220 */
    int32_t NumberOfFourierLinesCurrent; /* 1221 */
    int32_t NumberOfFourierLinesAfterZero; /* 1226 */
    int32_t FirstMeasuredFourierLine; /* 1228 */
    int32_t AcquisitionColumns; /* 1230 */
    int32_t ReconstructionColumns; /* 1231 */
    int32_t NumberOfAverages;   /* 1250 */
    flt64_t FlipAngle;          /* 1260 */
    int32_t NumberOfPrescans;      /* 1270 */
    char pad4[116];             /* Dummy padding */
    int32_t NumberOfSaturationRegions; /* 1290 */
    char pad5[4];                   /* Dummy for byte alignament */
    flt64_t ImageRotationAngle; /* 1294 */
    flt64_t DwellTime;          /* 1213 */
    char pad6[24];
    ima_vector_t CoilPosition; /* 1298 */
    flt64_t TotalMeasurementTimeCur; /* 1211 */
    char pad7[240];
    flt64_t MagneticFieldStrength; /* 1412 */
    flt64_t ADCVoltage;          /* 1414 */
    flt64_t TransmitterAmplitude; /* 1420 */
    int32_t NumberOfTransmitterAmplitudes; /* 1421 */
    char pad8[4];               /* Dummy padding */
    flt64_t TransmitterCalibration; /* 1424 */
    flt64_t ReceiverTotalGain;   /* 1450 */
    flt64_t ReceiverAmplifierGain; /* 1451 */
    flt64_t ReceiverPreamplifierGain; /* 1452 */
    flt64_t ReceiverCableAttenuation; /* 1454 */
    flt64_t ReconstructionScaleFactor; /* 1460 */
    flt64_t PhaseGradientAmplitude; /* 1470 */
    flt64_t ReadoutGradientAmplitude; /* 1471 */
    flt64_t SelectionGradientAmplitude; /* 1472 */
    gradient_delay_time_t GradientDelayTime; /* 1480 */
    char SensitivityCorrectionLabel[N_STRING + 1]; /* 1490 */
    char pad9[4];               /* Dummy padding */
    flt64_t ADCOffset[2];        /* 1416 */
    flt64_t TransmitterAttenuator; /* 1422 */
    flt64_t TransmitterReference; /* 1426 */
    flt64_t ReceiverReferenceGain; /* 1455 */
    int32_t ReceiverFilterFrequency; /* 1456 */
    char pad10[4];               /* Dummy padding */
    flt64_t ReferenceScaleFactor; /* 1462 */
    flt64_t TotalGradientDelayTime; /* 1482 */
    int32_t RfWatchdogMask;        /* 14A0 */
    char pad11[156];
    char ParameterFileName[N_FILENAME + 1]; /* 1510 */
    char SequenceFileName[N_FILENAME + 1]; /* 1511 */
    char SequenceFileOwner[N_SEQ_INFO + 1]; /* 1512 */
    char SequenceDescription[N_SEQ_INFO + 1]; /* 1513 */
    char pad12[108];
};


/*******************************************/
/* Relationship Information (Group 0021)   */
/*******************************************/

struct ima_siemens_0021
{
    char pad1[24];              /* Dummy padding */
    int16_t RoiMask;            /* 1020 */
    char pad2[4];               /* Dummy padding */
    field_of_view_t FoV;        /* 1120  0EA0 */
    view_direction_t ViewDirection; /* 1130  0EB0 */
    rest_direction_t RestDirection; /* 1132  0EB4 */
    ima_vector_t ImagePosition; /* 1160  0EB8 */
    ima_vector_t ImageNormal; /* 1161  0ED0 */
    flt64_t ImageDistance;      /* 1163  0EE8 */
    int16_t ImagePositioningHistoryMask; /* 1165  0EF0 */
    char pad3[4];                   /* Dummy padding */
    ima_vector_t ImageRow;  /* 116A  0EF8 */
    ima_vector_t ImageColumn; /* 116B 0F10 */
    patient_orientation_t PatientOrientationSet1; /* 1170 */
    patient_orientation_t PatientOrientationSet2; /* 1171 */
    char StudyName[N_STRING + 1]; /* 1180 */
    int32_t StudyType;          /* 1182 */
    flt64_t ImageMagnificationFactor; /* 1122 */
    char pad4[24];
    int32_t PhaseCorRowSeq;        /* 1320 */
    int32_t PhaseCorColSeq;        /* 1321 */
    int32_t PhaseCorRowRec;        /* 1322 */
    int32_t PhaseCorColRec;        /* 1324 */
    int32_t NumberOf3DRawPartNom;  /* 1330 */
    int32_t NumberOf3DRawPartCur;  /* 1331 */
    int32_t NumberOf3DImaPart;     /* 1334 */
    int32_t Actual3DImaPartNumber; /* 1336 */
    char pad5[4];
    int32_t NumberOfSlicesNom;     /* 1340 */
    int32_t NumberOfSlicesCur;     /* 1341 */
    int32_t CurrentSliceNumber;    /* 1342 */
    int32_t CurrentGroupNumber;    /* 1343 */
    int32_t MipStartRow;           /* 1345 */
    int32_t MipStopRow;            /* 1346 */
    int32_t MipStartColumn;        /* 1347 */
    int32_t MipStopColumn;         /* 1348 */
    int32_t MipStartSlice;         /* 1349 */
    int32_t MipStopSlice;          /* 134A */
    int32_t SignalMask;            /* 1350 */
    char pad6[4];
    int32_t DelayAfterTrigger;     /* 1352 */
    int32_t RRInterval;            /* 1353 */
    char pad7[4];               /* Dummy padding */
    flt64_t NumberOfTriggerPulses; /* 1354 */
    flt64_t RepetitionTime;     /* 1356 */
    int32_t GatePhase;          /* 1357 */
    char pad8[4];               /* Dummy padding */
    flt64_t GateThreshold;      /* 1358 */
    flt64_t GateRatio;          /* 1359 */
    int32_t NumberOfInterpolatedImages; /* 1360 */
    int32_t NumberOfEchoes;     /* 1370 */
    flt64_t SecondEchoTime;     /* 1372 */
    flt64_t SecondRepetitionTime; /* 1373 */
    int32_t CardiacCode;        /* 1380 */
    char pad9[4];               /* Dummy padding */
    flt64_t CurrentSliceDistanceFactor; /* 1344 */
    order_of_slices_t OrderOfSlices; /* 134F */
    char pad10[4];              /* Dummy padding */
    flt64_t SlabThickness;      /* 1339 */
    char pad11[829];            /* Padding */
};

/* Siemens IMA header - total size is 0x1800 bytes */

typedef struct
{                               /* Offset - Description */
    struct ima_acr_0008 G08;    /* 0x0000 - Identifying Information */
    struct ima_siemens_0009 G09; /* 0x0180 - Siemens specific */
    struct ima_acr_0010 G10;    /* 0x0300 - Patient Information */
    struct ima_siemens_0011 G11; /* 0x0400 - Siemens specific */
    char G13[0x0180];           /* 0x0480 - Siemens specific */
    struct ima_acr_0018 G18;    /* 0x0600 - Acquisition Information */
    struct ima_siemens_0019 G19; /* 0x0780 - Siemens specific */
    struct ima_acr_0020 G20;   /* 0x0C80 - Relationship Information */
    struct ima_siemens_0021 G21;    /* 0x0E80 - Siemens specific */
    struct ima_acr_0028 G28; /* 0x1380 - Image Presentation Information */
} siemens_header_t;

#endif /* _SIEMENS_HEADER_DEFS_H_ */
