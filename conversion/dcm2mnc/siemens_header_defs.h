#ifndef _SIEMENS_HEADER_DEFS_H_
#define _SIEMENS_HEADER_DEFS_H_ 1

/* CONSTANTS */

/* Define total length of the group objects so that we can make certain
 * all of our structures are properly sized and aligned.
 */
#define LENGTH_GROUP_0008 0x0180
#define LENGTH_GROUP_0010 0x0100
#define LENGTH_GROUP_0018 0x0180
#define LENGTH_GROUP_0020 0x0200
#define LENGTH_GROUP_0028 0x0100

/* siemens-specific header lengths.  We don't parse these. */
#define LENGTH_GROUP_0009 0x0180
#define LENGTH_GROUP_0011 0x0080
#define LENGTH_GROUP_0013 0x0180
#define LENGTH_GROUP_0019_PART1 0x0080
#define LENGTH_GROUP_0019_PART2 0x0200
#define LENGTH_GROUP_0019_PART3 0x0180
#define LENGTH_GROUP_0019_PART4 0x0100
#define LENGTH_GROUP_0021_PART1 0x0100
#define LENGTH_GROUP_0021_PART2 0x0100
#define LENGTH_GROUP_0021_PART3 0x0300
#define LENGTH_GROUP_0029 0x0100
#define LENGTH_GROUP_0051 0x0280

/* define length of common header strings */
#define LENGTH_LABEL 26

/* define length of special header stings */
#define LENGTH_AGE 4
#define LENGTH_COMMENT 26
#define LENGTH_DIAGNOSIS 40
#define LENGTH_DIRECTION 4
#define LENGTH_FILE_NAME 64
#define LENGTH_FILTER_ID 12
#define LENGTH_HEADER_VERSION 8
#define LENGTH_NUCLEUS 8
#define LENGTH_MANUFACTURER 8
#define LENGTH_ORIENTATION 3
#define LENGTH_PATIENT_ID 12
#define LENGTH_SEQUENCE_INFO 8
#define LENGTH_SOFTWARE_VERSION 8

/* define Xxx_UNDEFINED constants */
#define Enum_UNDEFINED (-19222)
#define Integer_UNDEFINED (-19222)
#define String_UNDEFINED '?'
#define Real_UNDEFINED (-19222.0)

/* TYPES */
typedef struct 
{
    long Year;                  /* four digits e.g. 1989 */
    long Month;                 /* 1 - 12 */
    long Day;                   /* 1 - 31 */
} ds_date_t;


typedef struct 
{
    long Hour;                  /* 0 - 23 */
    long Minute;                /* 0 - 59 */
    long Second;                /* 0 - 59 */
    long Fraction;              /* 0 - 999 */
} ds_time_t;

typedef enum 
{
    Compression_DONE = 2,
    Compression_NONE = 1,
    Compression_UNDEFINED = Enum_UNDEFINED
} compression_code_t;


typedef enum 
{
    Contrast_NONE = 1,
    Contrast_APPLIED = 2,
    Contrast_UNDEFINED = Enum_UNDEFINED
} contrast_t;


typedef enum 
{
    Set_m_IMAGE = 1,
    Set_m_PLOT = 2,
    Set_m_RAW = 3,
    Set_m_SPECT = 4,
    Set_m_TEXT = 5,
    Set_m_UNDEFINED = Enum_UNDEFINED
} data_set_subtype_m_t;


typedef enum 
{
    /* NOTE: data_set_subtype_s_tag */
    /* If this basic data type "data_set_subtype_s_t" is changed please
       check also the basic data type "measurement_mode_s_t" and the
       sequence "determine image type" in data set library function
       "ds_get_image_text_type()". */

    /* To find a free enum number you can use the following command
       "sort -t= +1 <Return> <set list> <Control D>" */
     
    Set_s_NONE = 1,
    Set_s_DYNA = 14,
    Set_s_HIS = 19,
    Set_s_HISC = 60,
    Set_s_PLOT = 18,
    Set_s_QUAL = 17,
    Set_s_ROT = 16,
    Set_s_SCAN = 10,
    Set_s_SINC = 11,
    Set_s_SIN = 12,
    Set_s_STAT = 15,
    Set_s_R2D = 62,
    Set_s_R3D = 63,
    Set_s_TOPO = 13,
    Set_s_CAR = 20,
    Set_s_MC = 21,
    Set_s_BLK = 41,
    Set_s_FPA = 42,
    Set_s_PROJ = 43,
    Set_s_READ = 44,
    Set_s_VFLO = 45,
    Set_s_VFPA = 46,
    Set_s_VSUM = 47,
    Set_s_CFL = 51,
    Set_s_CSH = 52,
    Set_s_UNDEFINED = Enum_UNDEFINED
} data_set_subtype_s_t;


typedef struct 
{
    data_set_subtype_m_t M;
    data_set_subtype_s_t S;
} data_set_subtype_t;

typedef enum 
{
    Geometry_CURVED = 3,
    Geometry_PLANAR = 1,
    Geometry_UNRAVEL = 2,
    Geometry_UNDEFINED = Enum_UNDEFINED
} geometry_t;

typedef enum 
{
    Format_RECT = 1,
    Format_UNDEFINED = Enum_UNDEFINED
} image_format_t;

typedef struct
{
    double Sag;
    double Cor;
    double Tra;
} image_location_t;

/* Analogous to DICOM (0020, 0060) */
typedef enum 
{
    Laterality_L = 1,
    Laterality_NO = 2,
    Laterality_R = 3,
    Laterality_UNDEFINED = Enum_UNDEFINED
} laterality_t;

/* Corresponds to DICOM (0008, 0060) */
typedef enum 
{
    Modality_CT = 1,
    Modality_MR = 2,
    Modality_UNDEFINED = Enum_UNDEFINED
} modality_t;

/* Corresponds to DICOM (0018,0085) */
typedef enum 
{
    Nucleus_C = 1,
    Nucleus_F = 2,
    Nucleus_H = 3,
    Nucleus_N = 4,
    Nucleus_NA = 5,
    Nucleus_P = 6,
    Nucleus_UNDEFINED = Enum_UNDEFINED
} imaged_nucleus_t;

typedef enum 
{
    Slice_Order_ASCENDING = 1,
    Slice_Order_DECREASING = 2,
    Slice_Order_FREE = 3,
    Slice_Order_INTERLEAVED = 4,
    Slice_Order_NONE = 5,
    Slice_Order_UNDEFINED = Enum_UNDEFINED
} order_of_slices_t;

typedef enum 
{
    Position_LEFT = 1,
    Position_PRONE = 2,
    Position_RIGHT = 3,
    Position_SUPINE = 4,
    Position_UNDEFINED = Enum_UNDEFINED
} patient_position_t;

typedef struct 
{
    double Row;
    double Col;
} pixel_size_t;

typedef enum 
{
    Rotation_NO = 1,
    Rotation_CC = 2,
    Rotation_CW = 3,
    Rotation_UNDEFINED = Enum_UNDEFINED
} rotation_direction_t;

typedef enum 
{
    Sex_F = 1,
    Sex_M = 2,
    Sex_O = 3,
    Sex_UNDEFINED = Enum_UNDEFINED
} sex_t;

/* Analogous to DICOM fields (0028, 1050) and (0028, 1051) */
typedef struct 
{
    long X;
    long Y;
} windows_t;

/*****************************************************/
/* Identifying Information (From DICOM group 0x0008) */
/*****************************************************/

typedef struct
{
    ds_date_t StudyDate;        /* (0008,0020)   10  AT DF 2NS-NEM */
    ds_date_t AcquisitionDate;  /* (0008,0022)   10  AT DF 2NS-CMS */
    ds_date_t ImageDate;        /* (0008,0023)   10  AT DF 2NS-CMS */
    ds_time_t StudyTime;        /* (0008,0030)   12  AT DF 2NS-NEM */
    ds_time_t AcquisitionTime;  /* (0008,0032)   12  AT DF 2NS-CMS */
    ds_time_t ImageTime;        /* (0008,0033)   12  AT DF 2NS-CMS */
    data_set_subtype_t DataSetSubtype; /* (0008,0041)    8  AT EV 2NS-CMS */
    modality_t Modality;        /* (0008,0060)    2  AT EV 1NS-CMS */
    char Manufacturer[LENGTH_MANUFACTURER + 1]; /* (0008,0070)    8  AT FF 2NS-NEM */
    char InstitutionID[LENGTH_LABEL + 1]; /* (0008,0080)   26  AT FF 2NS-NEM */
    char ReferringPhysician[LENGTH_LABEL + 1]; /* (0008,0090)   26  AT FF 2NS-NEM */
    char StationID[LENGTH_LABEL + 1]; /* (0008,1010)   26  AT FF 2NS-NEM */
    char ProcedureDescription_1[LENGTH_COMMENT + 1]; /* (0008,1030)   52  AT FF 2DS-CMS */
    char ProcedureDescription_2[LENGTH_COMMENT + 1]; /*  */
    char AdmittingDiagnosis[LENGTH_DIAGNOSIS + 1]; /* (0008,1080)   40  AT FF 2DS-CMS */
    char ManufacturerModel[LENGTH_LABEL + 1]; /* (0008,1090)   26  AT FF 2DS-CMS */
} acr_identifying_t;


/**************************************/
/* Patient Information (Group 0x0010) */
/**************************************/

typedef struct
{
    char PatientName[LENGTH_LABEL + 1]; /* (0010,0010)   26  AT FF 2NS-NEM */
    char PatientId[LENGTH_PATIENT_ID + 1]; /* (0010,0020)   12  AT FF 2NS-NEM */
    ds_date_t PatientBirthdate; /* (0010,0030)   10  AT DF 2NS-NEM */
    sex_t PatientSex;           /* (0010,0040)    2  AT EV 2NS-NEM */
    char PatientMaidenName[LENGTH_LABEL + 1]; /* (0010,1005)   26  AT FF 2DS-CMS */
    char PatientAge[LENGTH_AGE + 1]; /* (0010,1010)    4  AT DF 2NS-CMS */
    double PatientSize;         /* (0010,1020)    6  AN FF 3NS-NEM */
    long PatientWeight;         /* (0010,1030)    6  AN FF 2NS-CMS */
} acr_patient_t;



/******************************************/
/* Acquisition Information (Group 0x0018) */
/******************************************/

typedef struct
{
    contrast_t Contrast;        /* (0018,0010)    8  AT EV 2DS-NEM */
    int Pad1;                   /* Dummy for byte alignment */
    double SliceThickness;      /* (0018,0050)   14  AN FF 2NS-NEM */
    long GeneratorVoltage;      /* (0018,0060)    6  AN FF 2NM-NEM */
    long GeneratorVoltageDual;  /* (0018,0060) */
    double RepetitionTime;      /* (0018,0080)   14  AN FF 2NS-NEM */
    double EchoTime;            /* (0018,0081)   14  AN FF 2NS-NEM */
    double InversionTime;       /* (0018,0082)   14  AN FF 2NS-NEM */
    long NumberOfAverages;      /* (0018,0083)    6  AN FF 3NS-NEM */
    int Pad2;                   /* Dummy for byte alignment */
    double ImagingFrequency;    /* (0018,0084)   14  AN FF 2NS-NEM */
    imaged_nucleus_t Pad3;      /* (0018,0085) */
    long EchoNumber;            /* (0018,0086)    6  AN FF 3NS-CMS */
    long DataCollectionDiameter; /* (0018,0090)    6  AN FF 3NS-NEM */
    char DeviceSerialNumber[LENGTH_LABEL + 1]; /* (0018,1000)   26  AT FF 2DS-CMS */
    char SoftwareVersion[LENGTH_SOFTWARE_VERSION + 1]; /* (0018,1020)    8  AT FF 2DS-CMS */
    long DistanceSourceToDetector; /* (0018,1110)    6  AN FF 3NS-NEM */
    long DistanceSourceToPatient; /* (0018,1111)    6  AN FF 3NS-NEM */
    long GantryTilt;            /* (0018,1120)    6  AN FF 2NS-CMS */
    long TableHeight;           /* (0018,1130)    6  AN FF 3NS-NEM */
    rotation_direction_t RotationDirection; /* (0018,1140)    2  AT EV 3NS-NEM */
    long ExposureTime;          /* (0018,1150)    6  AN FF 2NS-CMS */
    long Exposure;              /* (0018,1152)    6  AN FF 2NS-CMS */
    char FilterIdLabel[LENGTH_FILTER_ID + 1]; /* (0018,1160)   12  AT FF 3NS-NEM */
    int Pad4;                   /* Dummy for byte alignment */
    double GeneratorPower;      /* (0018,1170)   14  AN FF 2NS-CMS */
    double FocalSpot;           /* (0018,1190    14  AN FF 3NS-NEM) */
    ds_date_t CalibrationDate;  /* (0018,1200)   10  AT DF 3NS-NEM */
    ds_time_t CalibrationTime;  /* (0018,1201)   12  AT DF 3NS-NEM */
    char ConvolutionKernel[LENGTH_LABEL + 1]; /* (0018,1210)   12  AT DF 3NS-NEM */
    char ReceivingCoil[LENGTH_LABEL + 1]; /* (0018,1250)   26  AT FF 2DS-CMS */
    char Gap1251[LENGTH_LABEL + 1];
    patient_position_t PatientPosition; /* (0018,5100)    8  AT EV 2NS-CMS */
    char ImagedNucleus[LENGTH_NUCLEUS + 1]; /* (0018,0085)    8  AT FF 2NS-NEM */
} acr_acquisition_t;



/*******************************************/
/* Relationship Information (Group 0x0020) */
/*******************************************/

typedef struct
{
    long Study;                 /* (0020,0010)    6  AN FF 2NS-NEM */
    long Gap0011;               /*  */
    long Acquisition;           /* (0020,0012)    6  AN FF 2DS-NEM */
    long Image;                 /* (0020,0013)    6  AN FF 2DS-NEM */
    long ImagePosition[3];      /* (0020,0030) */
    int Pad1;                   /* Dummy for byte alignment */
    double ImageOrientation[6]; /* (0020,0035) */
    long Location;              /* (0020,0050)    6  AN FF 3NS-NEM */
    laterality_t Laterality;    /* (0020,0060)    2  AT EV 2DS-NEM */
    geometry_t ImageGeometryType; /* (0020,0070)    8  AT EV 2DS-NEM */
    long AcquisitionsInSeries;  /* (0020,1001)    6  AN FF 3NS-NEM */
} acr_relationship_t;


/*************************************************/
/* Image Presentation Information (Group 0x0028) */
/*************************************************/

typedef struct
{
    short ImageDimension;       /* (0028,0005)    2  BI HX 1DS-NEM */
    short Rows;                 /* (0028,0010)    2  BI HX 1NS-NEM */
    short Columns;              /* (0028,0011)    2  BI HX 1NS-NEM */
    pixel_size_t PixelSize;     /* (0028,0030)   30  AN FF 2NM-NEM */
    image_format_t ImageFormat; /* (0028,0040)    4  AT EV 1DS-NEM */
    compression_code_t CompressionCode; /* (0028,0060)    4  AT EV 1DS-NEM */
    short BitsAllocated;        /* (0028,0100)    2  BI HX 1DS-NEM */
    short BitsStored;           /* (0028,0101)    2  BI HX 1DS-NEM */
    short HighBit;              /* (0028,0102)    2  BI HX 1DS-NEM */
    short PixelRepresentation;  /* (0028,0103)    2  BI HX 1DS-NEM */
    windows_t WindowCenter;     /* (0028,1050)   12  AN DF 2NM-MED */
    windows_t WindowWidth;      /* (0028,1051)   12  AN DF 2NM-MED */
    long RescaleIntercept;      /* (0028,1052)    6  AN FF 2NS-MED */
    long RescaleSlope;          /* (0028,1053)    6  AN FF 2NS-MED */
} acr_presentation_t;

/* SIEMENS SPECIFIC DATA - This may include proprietary stuff!!! */
typedef enum
{
    Storage_COMPRESS = 1,
    Storage_EXPANDED = 2,
    Storage_MIP_MPR = 5,
    Storage_REDUCED = 3,
    Storage_XDR = 4,
    Storage_UNDEFINED = Enum_UNDEFINED
} storage_mode_t;

typedef enum
{
    Study_Type_CRE = 1,
    Study_Type_MEA = 2,
    Study_Type_MIP = 3,
    Study_Type_MPR = 4,
    Study_Type_RAW = 5,
    Study_Type_UNDEFINED = Enum_UNDEFINED
} study_type_t;

typedef enum
{
    Object_m_CT = 1,
    Object_m_MRS = 2,
    Object_m_MRU = 3,
    Object_m_UNDEFINED = Enum_UNDEFINED
} data_object_subtype_m_t;

typedef enum
{
    Object_d_P = 1,
    Object_d_U = 2,
    Object_d_UNDEFINED = Enum_UNDEFINED
} data_object_subtype_d_t;


typedef enum
{
    Object_s_NONE = 1,
    Object_s_UNDEFINED = Enum_UNDEFINED
} data_object_subtype_s_t;


typedef struct
{
    data_object_subtype_m_t M;
    data_object_subtype_d_t D;
    data_object_subtype_s_t S;
} data_object_subtype_t;

typedef enum
{
    Rest_FEET = 1,
    Rest_HEAD = 2,
    Rest_UNDEFINED = Enum_UNDEFINED
} rest_direction_t;

typedef enum
{
    View_FEET = 1,
    View_HEAD = 2,
    View_AtoP = 3,
    View_LtoR = 4,
    View_PtoA = 5,
    View_RtoL = 6,
    View_UNDEFINED = Enum_UNDEFINED
} view_direction_t;

typedef struct
{
    int M;
    int S;
} measurement_mode_t;

typedef enum
{
    Filter_EXTERNAL = 5,
    Filter_FERMI = 1,
    Filter_GAUSS = 2,
    Filter_HANNING = 3,
    Filter_NONE = 4,
    Filter_UNDEFINED = Enum_UNDEFINED
} filter_type_t;

typedef struct
{
    int M;
    int S;
} calculation_mode_t;

typedef struct
{
    double Value1;
    double Value2;
    double Value3;
    double Value4;
} filter_parameter_t;

typedef enum
{
    Filter_Image_NO1 = 1,
    Filter_Image_NONE = 4,
    Filter_Image_UNDEFINED = Enum_UNDEFINED
} filter_type_image_t;

typedef enum
{
    Region_BODY = 1,
    Region_HEAD = 2,
    Region_UNDEFINED = Enum_UNDEFINED
} patient_region_t;

typedef enum
{
    Phase_ADULT = 1,
    Phase_CHILD = 2,
    Phase_UNDEFINED = Enum_UNDEFINED
} patient_phase_t;

typedef struct
{
    double X;
    double Y;
    double Z;
} gradient_delay_time_t;

typedef struct
{
    double Lim;
    double Cal;
    double Det;
} sar_sed_t;

typedef struct
{
    double X;
    double Y;
} target_point_t;

typedef struct
{
    double Height;
    double Width;
} field_of_view_t;

typedef struct
{
    char Y[LENGTH_ORIENTATION + 1]; /* up - down */
    char X[LENGTH_ORIENTATION + 1]; /* left - right */
    char Z[LENGTH_ORIENTATION + 1]; /* back - front */
} patient_orientation_t;

typedef struct
{
    double Phi;
    double Theta;
    double Radius;
} object_orientation_t;

typedef enum
{
    Gate_EXPIRATION = 1,
    Gate_INSPIRATION = 2,
    Gate_UNDEFINED = Enum_UNDEFINED
} gate_phase_t;

typedef struct
{
    long LowerBoundary;
    long UpperBoundary;
} object_threshold_t;

typedef enum
{
    Style_DOUBLE = 1,
    Style_HIGH = 2,
    Style_NONE = 3,
    Style_STD_1 = 4,
    Style_STD_2 = 5,
    Style_UNDEFINED = Enum_UNDEFINED
} window_style_t;

typedef enum
{
    Pixel_Quality_ESTIMATED = 1,
    Pixel_Quality_EXACT = 2,
    Pixel_Quality_NONE = 3,
    Pixel_Quality_UNDEFINED = Enum_UNDEFINED
} pixel_quality_mode_t;

typedef struct
{
    pixel_quality_mode_t Min;
    pixel_quality_mode_t Mean;
    pixel_quality_mode_t Max;
} pixel_quality_code_t;

typedef struct
{
    long Min;
    long Mean;
    long Max;
} pixel_quality_value_t;

typedef enum
{
    Save_DONE = 1,
    Save_MARKED = 2,
    Save_NOT = 3,
    Save_UNDEFINED = Enum_UNDEFINED
} save_code_t;

/******************************************/
/* Acquisition Information (Group 0019) */
/******************************************/

typedef struct shadow_acquisition_cms_tag /* CMS shadowsubgroup */
{
    long NetFrequency;          /* (0019,1010)    6  AN FF 3NS-CMS */
    measurement_mode_t MeasurementMode; /* (0019,1020)    8  AT EV 2DS-CMS */
    calculation_mode_t CalculationMode; /* (0019,1030)    8  AT EV 2NS-CMS */
    long Gap1040;               /*  */
    long NoiseLevel;            /* (0019,1050)    6  AN FF 3NS-CMS */
    long NumberOfDataBytes;     /* (0019,1060)    6  AN FF 2NS-CMS */
} shadow_acquisition_cms_t;


typedef struct shadow_acquisition_ct_tag /* CT shadowsubgroup */
{
    double SourceSideCollimatorAperture; /* (0019,1110)   14  AN FF 3NS-CMS */
    double DetectorSideCollimatorAperture; /* (0019,1111)   14  AN FF 3NS-CMS */
    long ExposureTime;          /* (0019,1120)    6  AN FF 3NS-CMS */
    long Exposure;              /* (0019,1121)    6  AN FF 3NS-CMS */
    double GeneratorPower;      /* (0019,1125)   14  AN FF 3NS-CMS */
    long GeneratorVoltage;      /* (0019,1126)    6  AN FF 3NM-CMS */
    long GeneratorVoltageDual;  /*  */
    long MasterControlMask;     /* (0019,1140)    4  BD HX 3NM-CMS */
    long Gap1140;               /*  */
    short ProcessingMask[5];    /* (0019,1142)    2  BI HX 3NM-CMS */
    short Gap1142[3];           /*  */
    long NumberOfVirtuellChannels; /* (0019,1162)    6  AN FF 3NS-CMS */
    long NumberOfReadings;      /* (0019,1170)    6  AN FF 3NS-CMS */
    long NumberOfProjections;   /* (0019,1174)    6  AN FF 3NS-CMS */
    long NumberOfBytes;         /* (0019,1175)    6  AN FF 3NS-CMS */
    long ReconstructionAlgorithmSet[3]; /* (0019,1180)    6  AN FF 3NS-CMS */
    long Gap1180[2];            /*  */
    long ReconstructionAlgorithmIndex; /* (0019,1181)    6  AN FF 3NS-CMS */
    char RegenerationSoftwareVersion[LENGTH_SOFTWARE_VERSION + 1]; /* (0019,1182)    8  AT FF 3NS-CMS */
} shadow_acquisition_ct_t;


typedef struct shadow_acquisition_mr_tag /* MR shadowsubgroup */
{
    double TotalMeasurementTime; /* (0019,1210)   14  AN FF 3NS-CMS */
    double StartDelayTime;      /* (0019,1212)   14  AN FF 3NS-CMS */
    long NumberOfPhases;        /* (0019,1214)    6  AN FF 2DS-CMS */
    long SequenceControlMask[2]; /* (0019,1216)    4  BD HX 3NM-CMS */
    long Gap1216[2];            /*  */
    long NumberOfFourierLinesNominal; /* (0019,1220)    6  AN FF 3NS-CMS */
    long NumberOfFourierLinesCurrent; /* (0019,1221)    6  AN FF 3NS-CMS */
    long NumberOfFourierLinesAfterZero; /* (0019,1226)    6  AN FF 3NS-CMS */
    long FirstMeasuredFourierLine; /* (0019,1228)    6  AN FF 3NS-CMS */
    long AcquisitionColumns;    /* (0019,1230)    6  AN FF 3NS-CMS */
    long ReconstructionColumns; /* (0019,1231)    6  AN FF 3NS-CMS */
    long NumberOfAverages;      /* (0019,1250)    6  AN FF 3NS-CMS */
    double FlipAngle;           /* (0019,1260)   14  AN FF 3NS-CMS */
    long NumberOfPrescans;      /* (0019,1270)    6  AN FF 3NS-CMS */
    filter_type_t FilterTypeRawData; /* (0019,1281)    8  AT EV 2DS-CMS */
    filter_parameter_t FilterParameterRawData; /* (0019,1282)   14  AN FF 3NM-CMS */
    filter_type_image_t FilterTypeImageData; /* (0019,1283)    8  AT EV 2DS-CMS */
    int Pad1;                   /* Dummy for byte alignament */
    filter_parameter_t FilterParameterImageData; /* (0019,1284)   14  AN FF 3NM-CMS */
    filter_type_t FilterTypePhaseCorrection; /* (0019,1285)    8  AT EV 2DS-CMS */
    int Pad2;                   /* Dummy for byte alignament */
    filter_parameter_t FilterParameterPhaseCorrection; /* (0019,1286)   14  AN FF 3NM-CMS */
    long NumberOfSaturationRegions; /* (0019,1290)    6  AN FF 2DS-CMS */
    int Pad3;                   /* Dummy for byte alignament */
    double ImageRotationAngle;  /* (0019,1294)   14  AN FF 3NS-CMS */
    double DwellTime;           /* (0019,1213)   14  AN FF 3NS-CMS */
    long CoilIdMask[3];         /* (0019,1296)    4  BD HX 3NM-CMS */
    long Gap1296[2];
    int Pad4;                   /* Dummy for byte alignament */
    image_location_t CoilPosition; /* (0019,1298)   14  AN FF 2NM-CMS */
    double TotalMeasurementTimeCur; /* (0019,1211)   14  AN FF 3NS-CMS*/
    long MeasurementStatusMask; /* (0019,1218)    4  BD FF 2DS-CMS */
} shadow_acquisition_mr_t;


typedef struct /* CT configuration and adjust shadow subgroup */
{
    long DistanceSourceToSourceSideCollimator; /* (0019,1310)    6  AN FF 3NS-CMS */
    long DistanceSourceToDetectorSideCollimator; /* (0019,1311)    6  AN FF 3NS-CMS */
    long NumberOfPossibleChannels; /* (0019,1320)    6  AN FF 3NS-CMS */
    long MeanChannelNumber;     /* (0019,1321)    6  AN FF 3NS-CMS */
    double DetectorSpacing;     /* (0019,1322)   14  AN FF 3NS-CMS */
    double ReadingIntegrationTime; /* (0019,1324)   14  AN FF 3NS-CMS */
    double DetectorAlignment;   /* (0019,1350)   14  AN FF 3NS-CMS */
    double FocusAlignment;      /* (0019,1360)   14  AN FF 3NS-CMS */
    long FocalSpotDeflectionAmplitude; /* (0019,1365)    4  BD HX 3NS-CMS */
    long FocalSpotDeflectionPhase; /* (0019,1366)    4  BD HX 3NS-CMS */
    long FocalSpotDeflectionOffset; /* (0019,1367)    4  BD HX 3NS-CMS */
    double WaterScalingFactor;  /* (0019,1370)   14  AN FF 3NS-CMS */
    double InterpolationFactor; /* (0019,1371)   14  AN FF 3NS-CMS */
    patient_region_t PatientRegion; /* (0019,1380)    4  AT EV 3NS-CMS */
    patient_phase_t PatientPhaseOfLife; /* (0019,1382)    8  AT EV 3NS-CMS */
    double DetectorCenter;      /* (0019,1323)   14  AN FF 3NS-CMS */
} shadow_acquisition_ct_conf_t;


typedef struct /* MR configuration and adjust shadow subgroup */ 
{
    double MagneticFieldStrength; /* (0019,1412)   14  AN FF 3NS-CMS */
    double ADCVoltage;          /* (0019,1414)   14  AN FF 3NS-CMS */
    double TransmitterAmplitude; /* (0019,1420)   14  AN FF 3NS-CMS */
    long NumberOfTransmitterAmplitudes; /* (0019,1421)    6  AN FF 3NS-CMS */
    int Pad1;                   /* Dummy for byte alignment */
    double TransmitterCalibration; /* (0019,1424)   14  AN FF 3NS-CMS */
    double ReceiverTotalGain;   /* (0019,1450)   14  AN FF 3NS-CMS */
    double ReceiverAmplifierGain; /* (0019,1451)   14  AN FF 3NS-CMS */
    double ReceiverPreamplifierGain; /* (0019,1452)   14  AN FF 3NS-CMS */
    double ReceiverCableAttenuation; /* (0019,1454)   14  AN FF 3NS-CMS */
    double ReconstructionScaleFactor; /* (0019,1460)   14  AN FF 3NS-CMS */
    double PhaseGradientAmplitude; /* (0019,1470)   14  AN FF 3NS-CMS */
    double ReadoutGradientAmplitude; /* (0019,1471)   14  AN FF 3NS-CMS */
    double SelectionGradientAmplitude; /* (0019,1472)   14  AN FF 3NS-CMS */
    gradient_delay_time_t GradientDelayTime; /* (0019,1480)   14  AN FF 3NS-CMS */
    char SensitivityCorrectionLabel[LENGTH_LABEL + 1]; /* (0019,1490)   26  AT FF 2DS-CMS */
    int Pad2;                   /* Dummy for byte alignment */
    double ADCOffset[2];        /* (0019,1416)   14  AN FF 3NM-CMS */
    double TransmitterAttenuator; /* (0019,1422)   14  AN FF 3NS-CMS */
    double TransmitterReference; /* (0019,1426)   14  AN FF 3NS-CMS */
    double ReceiverReferenceGain; /* (0019,1455)   14  AN FF 3NS-CMS */
    long ReceiverFilterFrequency; /* (0019,1456)    6  AN FF 3NS-CMS */
    int Pad3;                   /* Dummy for byte alignment */
    double ReferenceScaleFactor; /* (0019,1462)   14  AN FF 3NS-CMS */
    double TotalGradientDelayTime; /* (0019,1482)   14  AN FF 3NS-CMS */
    long RfWatchdogMask;        /* (0019,14A0)    4  BD HX 3NM-CMS */
} shadow_acquisition_mr_conf_t;


typedef struct /* acquisition shadow subgroup */
{
    char ParameterFileName[LENGTH_FILE_NAME + 1]; /* (0019,1510)   64  AT FF 3NS-CMS */
    char SequenceFileName[LENGTH_FILE_NAME + 1]; /* (0019,1511)   64  AT FF 3NS-CMS */
    char SequenceFileOwner[LENGTH_SEQUENCE_INFO + 1]; /* (0019,1512)    8  AT FF 2DS-CMS */
    char SequenceDescription[LENGTH_SEQUENCE_INFO + 1]; /* (0019,1513)    8  AT FF 2DS-CMS */
} shadow_acquisition_acq_t;


/*******************************************/
/* Relationship Information (Group 0021) */
/*******************************************/

typedef struct shadow_relationship_med_cms_tag /* MED and CMS shadowsubgroups */
{
    double Gap1010;
    target_point_t Target;      /* (0021,1011)   30  AT FF 2NM-MED */
    short RoiMask;              /* (0021,1020)    2  BI HX 2NS-MED */
    int Pad1;                   /* Dummy for byte alignment */
    field_of_view_t FoV;        /* (0021,1120)   30  AN FF 2NM-CMS */
    view_direction_t ViewDirection; /* (0021,1130)    4  AT EV 2NS-CMS */
    rest_direction_t RestDirection; /* (0021,1132)    4  AT EV 2NS-CMS */
    image_location_t ImagePosition; /* (0021,1160)   14  AN FF 2NM-CMS */
    image_location_t ImageNormal; /* (0021,1161)   14  AN FF 2NM-CMS */
    double ImageDistance;       /* (0021,1163)   14  AN FF 2NM-CMS */
    short ImagePositioningHistoryMask; /* (0021,1165)    2  BI HX 2DS-CMS */
    int Pad2;                   /* Dummy for byte alignment */
    image_location_t ImageRow;  /* (0021,116A)   14  AN FF 2NM-CMS */
    image_location_t ImageColumn; /* (0021,116B)   14  AN FF 2NM-CMS */
    patient_orientation_t PatientOrientationSet1; /* (0021,1170)    4  AT EV 2NM-CMS */
    patient_orientation_t PatientOrientationSet2; /* (0021,1171)    4  AT EV 2NM-CMS */
    char StudyName[LENGTH_LABEL + 1]; /* (0021,1180)   26  AT FF 3NS-CMS */
    study_type_t StudyType;     /* (0021,1182)    4  AT EV 3NS-CMS */
    double ImageMagnificationFactor; /* (0021,1122)   14  AN FF 2DS-CMS */
} shadow_relationship_med_cms_t;


typedef struct shadow_relationship_ct_tag /* CT common shadowsubgroups */
{
    long RotationAngle;         /* (0021,1210)    6  AN FF 3NS-CMS */
    long StartAngle;            /* (0021,1211)    6  AN FF 3NS-CMS */
    long TubePosition;          /* (0021,1230)    6  AN FF 3NS-CMS */
    long LengthOfTopogram;      /* (0021,1232)    6  AN FF 3NS-CMS */
    double CorrectionFactor;    /* (0021,1234)   14  AN FF 3NS-CMS */
    long MaximumTablePosition;  /* (0021,1236)    6  AN FF 3NS-CMS */
    long TableMoveDirectionCode; /* (0021,1240)    6  AN FF 3NS-CMS */
    long VectorStartRow;        /* (0021,1250)    6  AN FF 3NS-CMS */
    long VectorRowStep;         /* (0021,1251)    6  AN FF 3NS-CMS */
    long VectorStartColumn;     /* (0021,1252)    6  AN FF 3NS-CMS */
    long VectorColumnStep;      /* (0021,1253)    6  AN FF 3NS-CMS */
    long VoiStartRow;           /* (0021,1245)    6  AN FF 3NS-CMS */
    long VoiStopRow;            /* (0021,1246)    6  AN FF 3NS-CMS */
    long VoiStartColumn;        /* (0021,1247)    6  AN FF 3NS-CMS */
    long VoiStopColumn;         /* (0021,1248)    6  AN FF 3NS-CMS */
    long VoiStartSlice;         /* (0021,1249)    6  AN FF 3NS-CMS */
    long VoiStopSlice;          /* (0021,124A)    6  AN FF 3NS-CMS */
    long RangeTypeCode;         /* (0021,1260)    6  AN FF 3NS-CMS */
    long ReferenceTypeCode;     /* (0021,1262)    6  AN FF 3NS-CMS */
    object_orientation_t ObjectOrientation; /* (0021,1270)   14  AN FF 3NS-CMS */
    object_orientation_t LightOrientation; /* (0021,1272)   14  AN FF 3NS-CMS */
    double LightBrightness;     /* (0021,1275)   14  AN FF 3NS-CMS */
    double LightContrast;       /* (0021,1276)   14  AN FF 3NS-CMS */
    object_threshold_t OverlayThreshold; /* (0021,127A)   12  AN FF 3NM-CMS */
    object_threshold_t SurfaceThreshold; /* (0021,127B)   12  AN FF 3NM-CMS */
    object_threshold_t GreyScaleThreshold; /* (0021,127C)   12  AN FF 3NM-CMS */
} shadow_relationship_ct_t;


typedef struct shadow_relationship_mr_tag		   /* MR common shadowsubgroups */
{
    long PhaseCorRowSeq;        /* (0021,1320)    6  AN FF 3NS-CMS */
    long PhaseCorColSeq;        /* (0021,1321)    6  AN FF 3NS-CMS */
    long PhaseCorRowRec;        /* (0021,1322)    6  AN FF 3NS-CMS */
    long PhaseCorColRec;        /* (0021,1324)    6  AN FF 3NS-CMS */
    long NumberOf3DRawPartNom;  /* (0021,1330)    6  AN FF 3NS-CMS */
    long NumberOf3DRawPartCur;  /* (0021,1331)    6  AN FF 3NS-CMS */
    long NumberOf3DImaPart;     /* (0021,1334)    6  AN FF 3NS-CMS */
    long Actual3DImaPartNumber; /* (0021,1336)    6  AN FF 3NS-CMS */
    long Gap1338;
    long NumberOfSlicesNom;     /* (0021,1340)    6  AN FF 3NS-CMS */
    long NumberOfSlicesCur;     /* (0021,1341)    6  AN FF 3NS-CMS */
    long CurrentSliceNumber;    /* (0021,1342)    6  AN FF 3NS-CMS */
    long CurrentGroupNumber;    /* (0021,1343)    6  AN FF 3NS-CMS */
    long MipStartRow;           /* (0021,1345)    6  AN FF 3NS-CMS */
    long MipStopRow;            /* (0021,1346)    6  AN FF 3NS-CMS */
    long MipStartColumn;        /* (0021,1347)    6  AN FF 3NS-CMS */
    long MipStopColumn;         /* (0021,1348)    6  AN FF 3NS-CMS */
    long MipStartSlice;         /* (0021,1349)    6  AN FF 3NS-CMS */
    long MipStopSlice;          /* (0021,134A)    6  AN FF 3NS-CMS */
    long SignalMask;            /* (0021,1350)    4  BI HX 2DS-CMS */
    long Gap1350;
    long DelayAfterTrigger;     /* (0021,1352)    6  AN FF 3NS-CMS */
    long RRInterval;            /* (0021,1353)    6  AN FF 3NS-CMS */
    int Pad1;                   /* Dummy for byte alignment */
    double NumberOfTriggerPulses; /* (0021,1354)   14  AN FF 3NS-CMS */
    double RepetitionTime;      /* (0021,1356)   14  AN FF 3NS-CMS */
    gate_phase_t GatePhase;     /* (0021,1357)   12  AT EV 3NS-CMS */
    int Pad2;                   /* Dummy for byte alignment */
    double GateThreshold;       /* (0021,1358)   14  AN FF 3NS-CMS */
    double GateRatio;           /* (0021,1359)   14  AN FF 3NS-CMS */
    long NumberOfInterpolatedImages; /* (0021,1360)    6  AN FF 3NS-CMS */
    long NumberOfEchoes;        /* (0021,1370)    6  AN FF 3NS-CMS */
    double SecondEchoTime;      /* (0021,1372)   14  AN FF 3NS-CMS */
    double SecondRepetitionTime; /* (0021,1373)   14  AN FF 3NS-CMS */
    long CardiacCode;           /* (0021,1380)    6  AN FF 3NS-CMS */
    int Pad3;                   /* Dummy for byte alignment */
    double CurrentSliceDistanceFactor; /* (0021,1344)   14  AN FF 3NS-CMS */
    order_of_slices_t OrderOfSlices; /* (0021,134F)   12  AT EV 3NS-CMS */
    int Pad4;                   /* Dummy for byte alignment */
    double SlabThickness;       /* (0021,1339)    6  AN FF 3NS-CMS */
} shadow_relationship_mr_t;


typedef struct shadow_relationship_ct_spe_tag /* CT special shadowsubgroups */
{
    long EvaluationMask[2];     /* (0021,2220)    4  BD FF 2DM-CMS */
    long Gap2220[2];
    short ExtendedProcessingMask[7]; /* (0021,2230)    4  BD FF 2DM-CMS */
    short Gap2230[3];
    long CreationMask[2];       /* (0021,2210)    2  BI HX 3NM-CMS
 */
    long Gap2210[2];
} shadow_relationship_ct_spe_t;


typedef struct shadow_relationship_mr_spe_tag /* MR special shadowsubgroups */
{
    double EpiReconstructionPhase; /* (0019,12A0)   14  AN FF 3NS-CMS */
    double EpiReconstructionSlope; /* (0019,12A1)   14  AN FF 3NS-CMS */
    double EpiCapacity[6];      /* (0019,14C1)   14  AN FF 3NM-CMS */
    double EpiInductance[3];    /* (0019,14C2)   14  AN FF 3NM-CMS */
    long EpiSwitchConfigurationCode[3]; /* (0019,14C3)    6  AN FF 3NM-CMS */
    long EpiSwitchHardwareCode[3]; /* (0019,14C4)    6  AN FF 3NM-CMS */
    long EpiSwitchDelayTime[6]; /* (0019,14C5)    6  AN FF 3NM-CMS */
    char EpiFileName[LENGTH_FILE_NAME + 1]; /* (0019,1514)   64  AT FF 3NS-CMS */
} shadow_relationship_mr_spe_t;


/*************************************************/
/* Image Presentation Information (Group 0029) */
/*************************************************/

typedef struct shadow_presentation_tag
{
    window_style_t WindowStyle; /* (0029,1110)    8  AT EV 2DS-CMS */
    pixel_quality_code_t PixelQualityCode; /* (0029,1120)   12  AT FF 2DM-CMS */
    pixel_quality_value_t PixelQualityValue; /* (0029,1122)    6  AN FF 3NM-CMS */
    save_code_t ArchiveCode;    /* (0029,1150)    8  AN EV 3NS-CMS */
    save_code_t ExposureCode;   /* (0029,1151)    8  AN EV 3NS-CMS */
    long SortCode;              /* (0029,1152)    6  AN FF 3NS-CMS */
    long Splash;                /* (0029,1160)    6  AN FF 3NS-CMS */
} shadow_presentation_t;


typedef union
{
    char raw_buf[LENGTH_GROUP_0008];
    acr_identifying_t Ide;        /* work area */
} group_0008_t;

typedef union
{
    char raw_buf[LENGTH_GROUP_0010];
    acr_patient_t Pat;            /* work area */
} group_0010_t;

typedef union
{
    char raw_buf[LENGTH_GROUP_0018];
    acr_acquisition_t Acq;        /* work area */
} group_0018_t;

typedef union
{
    char raw_buf[LENGTH_GROUP_0020];
    acr_relationship_t Rel;       /* work area */
} group_0020_t;

typedef union
{
    char raw_buf[LENGTH_GROUP_0028];
    acr_presentation_t Pre;       /* work area */
} group_0028_t;

/* SIEMENS-SPECIFIC */

typedef union
{
    char raw_buf[LENGTH_GROUP_0009];
} group_0009_t;

typedef union
{
    char raw_buf[LENGTH_GROUP_0011];
} group_0011_t;

typedef union
{
    char raw_buf[LENGTH_GROUP_0013];
} group_0013_t;

typedef union
{
    char raw_buf[LENGTH_GROUP_0019_PART1];
    shadow_acquisition_cms_t CM;  /* work area */
} group_0019_part1_t;           /*  */

typedef union
{
    char raw_buf[LENGTH_GROUP_0019_PART2];
    shadow_acquisition_ct_t Ct;   /* CT common work area */
    shadow_acquisition_mr_t Mr;   /* MR common work area */
} group_0019_part2_t;

typedef union
{
    char raw_buf[LENGTH_GROUP_0019_PART3];
    shadow_acquisition_ct_conf_t Ct; /* CT configuration work area */
    shadow_acquisition_mr_conf_t Mr; /* MR configuration work area */
} group_0019_part3_t;

typedef union
{
    char raw_buf[LENGTH_GROUP_0019_PART4]; /* fill-in */
    shadow_acquisition_acq_t CM; /* work area */
} group_0019_part4_t;

typedef struct
{
    group_0019_part1_t Acq1;    /* CMS subgroup */
    group_0019_part2_t Acq2;    /* common subgroup */
    group_0019_part3_t Acq3;    /* config. and adjust subgroup */
    group_0019_part4_t Acq4;    /* acquisition subgroup */
} group_0019_t;

typedef union
{
    char raw_buf[LENGTH_GROUP_0021_PART1];
    shadow_relationship_med_cms_t CM; /* work area */
} group_0021_part1_t;

typedef union
{
    char raw_buf[LENGTH_GROUP_0021_PART2];
    shadow_relationship_ct_t Ct;  /* CT common work area */
    shadow_relationship_mr_t Mr;  /* MR common work area */
} group_0021_part2_t;

typedef union
{
    char raw_buf[LENGTH_GROUP_0021_PART3];
    shadow_relationship_ct_spe_t Ct; /* CT special work area */
    shadow_relationship_mr_spe_t Mr; /* MR special work area */
} group_0021_part3_t;

typedef struct
{
    group_0021_part1_t Rel1;    /* CMS subgroup */
    group_0021_part2_t Rel2;    /* common subgroup */
    group_0021_part3_t Rel3;    /* special subgroup */
} group_0021_t;

typedef union
{
    char raw_buf[LENGTH_GROUP_0029];
    shadow_presentation_t Pre;
} group_0029_t;


typedef union
{
    char raw_buf[LENGTH_GROUP_0051];
} group_0051_t;

/* Siemens IMA header - total size is 0x1800 bytes */

typedef struct header_tag
{                               /* Offset - Description */
    group_0008_t G08;           /* 0x0000 - Identifying Information */
    group_0009_t G09;           /* 0x0180 - Siemens specific */
    group_0010_t G10;           /* 0x0300 - Patient Information */
    group_0011_t G11;           /* 0x0400 - Siemens specific */
    group_0013_t G13;           /* 0x0480 - Siemens specific */
    group_0018_t G18;           /* 0x0600 - Acquisition Information */
    group_0019_t G19;           /* 0x0780 - Siemens specific */
    group_0020_t G20;           /* 0x0C80 - Relationship Information */
    group_0021_t G21;           /* 0x0E80 - Siemens specific */
    group_0028_t G28;           /* 0x1380 - Image Presentation Information */
    group_0029_t G29;           /* 0x1480 - Siemens specific */
    group_0051_t G51;           /* 0x1580 - Siemens specific */
} siemens_header_t;

#endif /* _SIEMENS_HEADER_DEFS_H_ */
