/*[-  HEADER FILE  -------------------------------------------------------------------------*/
/*
   Name:        ds_head_shadow_groups_types.h

   Description: The header file defines the SPI and CMS defined data set basic groups
                (odd group numbers) as structures for internal use (internal header).


   Author:      THUMSER, Andreas (TH); Siemens AG UBMed CMS/SCE64; phone: 09131 844797
*/
/*]-----------------------------------------------------------------------------------------*/

#ifndef DS_HEAD_SHADOW_GROUPS_TYPES
#define DS_HEAD_SHADOW_GROUPS_TYPES

/******************************************/
/* Identifying Information (Group 0009'H) */
/******************************************/

typedef struct shadow_identifying_tag
{
  data_object_subtype_t DataObjectSubtype;		   /* (0009,1041)    8  AT EV 2NS-SPI */
  long NumberOfMeasurements;				   /* (0009,1200)    6  AN FF 3NS-CMS */
  storage_mode_t StorageMode;				   /* (0009,1210)    8  AT EV 2DS-CMS */
  long EvaluationMask;					   /* (0009,1212)    4  BD FF 2DS-CMS */
  long Gap1212;
  ds_date_t LastMoveDate;				   /* (0009,1226)   10  AT DF 3NS-CMS */
  ds_time_t LastMoveTime;				   /* (0009,1227)   12  AT DF 3NS-CMS */
  char GeneratorIdentificationLabel[LENGTH_LABEL + 1];	   /* (0009,1310)   26  AT DF 2DS-CMS */
  char GantryIdentificationLabel[LENGTH_LABEL + 1];	   /* (0009,1311)   26  AT DF 2DS-CMS */
  char XRayTubeIdentificationLabel[LENGTH_LABEL + 1];	   /* (0009,1312)   26  AT DF 2DS-CMS */
  char DetectorIdentificationLabel[LENGTH_LABEL + 1];	   /* (0009,1313)   26  AT DF 2DS-CMS */
  char DASIdentificationLabel[LENGTH_LABEL + 1];	   /* (0009,1314)   26  AT DF 2DS-CMS */
  char SMIIdentificationLabel[LENGTH_LABEL + 1];	   /* (0009,1315)   26  AT DF 2DS-CMS */
  char CPUIdentificationLabel[LENGTH_LABEL + 1];	   /* (0009,1316)   26  AT DF 2DS-CMS */
  char HeaderVersion[LENGTH_HEADER_VERSION + 1];	   /* (0009,1320)    8  AT DF 2DS-CMS */
} shadow_identifying_t;



/**************************************/
/* Patient Information (Group 0011'H) */
/**************************************/

typedef struct shadow_patient_tag
{
  char Organ[LENGTH_LABEL + 1];				   /* (0011,1010)   26  AT FF 3NS-SPI */
  ds_date_t RegistrationDate;				   /* (0011,1110)   10  AT DF 2NS-CMS */
  ds_time_t RegistrationTime;				   /* (0011,1111)   12  AT DF 2NS-CMS */
  long UsedPatientWeight;				   /* (0011,1123)    4  AN FF 2NS-CMS */
  long OrganCode;                                          /* (0011,1012)    6  AN FF 3NS-CMS */
} shadow_patient_t;



/*********************************************/
/* Patient Modify Information (Group 0013'H) */
/*********************************************/

typedef struct shadow_patient_modify_tag
{
  char ModifyingPhysician[LENGTH_LABEL + 1];		   /* (0013,1000)   26  AT FF 2NS-CMS */
  ds_date_t ModificationDate;				   /* (0013,1010)   10  AT DF 2NS-CMS */
  ds_time_t ModificationTime;				   /* (0013,1012)   12  AT DF 2NS-CMS */
  char PatientName[LENGTH_LABEL + 1];			   /* (0013,1020)   26  AT FF 3NS-CMS */
  char PatientId[LENGTH_PATIENT_ID + 1];		   /* (0013,1022)   12  AT FF 3NS-CMS */
  ds_date_t PatientBirthdate;				   /* (0013,1030)   10  AT DF 3NS-CMS */
  long PatientWeight;					   /* (0013,1031)    6  AN FF 3NS-CMS */
  char PatientMaidenName[LENGTH_LABEL + 1];		   /* (0013,1032)   26  AT FF 3NS-CMS */
  char ReferringPhysician[LENGTH_LABEL + 1];		   /* (0013,1033)   26  AT FF 3NS-CMS */
  char AdmittingDiagnosis[LENGTH_DIAGNOSIS + 1];	   /* (0013,1034)   40  AT FF 3DS-CMS */
  sex_t PatientSex;					   /* (0013,1035)    2  AT EV 3NS-CMS */
  char ProcedureDescription_1[LENGTH_COMMENT + 1];	   /* (0013,1040)   52  AT FF 3NS-CMS */
  char ProcedureDescription_2[LENGTH_COMMENT + 1];
  rest_direction_t RestDirection;			   /* (0013,1042)    4  AT EV 3NS-CMS */
  patient_position_t PatientPosition;			   /* (0013,1044)    8  AT EV 3NS-CMS */
  view_direction_t ViewDirection;			   /* (0013,1046)    4  AT EV 2NS-CMS */
} shadow_patient_modify_t;



/******************************************/
/* Acquisition Information (Group 0019'H) */
/******************************************/

typedef struct shadow_acquisition_cms_tag		   /* CMS shadowsubgroup */
{
  long NetFrequency;					   /* (0019,1010)    6  AN FF 3NS-CMS */
  measurement_mode_t MeasurementMode;			   /* (0019,1020)    8  AT EV 2DS-CMS */
  calculation_mode_t CalculationMode;			   /* (0019,1030)    8  AT EV 2NS-CMS */
  long Gap1040;
  long NoiseLevel;					   /* (0019,1050)    6  AN FF 3NS-CMS */
  long NumberOfDataBytes;				   /* (0019,1060)    6  AN FF 2NS-CMS */
} shadow_acquisition_cms_t;


typedef struct shadow_acquisition_ct_tag		   /* CT shadowsubgroup */
{
  double SourceSideCollimatorAperture;			   /* (0019,1110)   14  AN FF 3NS-CMS */
  double DetectorSideCollimatorAperture;		   /* (0019,1111)   14  AN FF 3NS-CMS */
  long ExposureTime;					   /* (0019,1120)    6  AN FF 3NS-CMS */
  long Exposure;					   /* (0019,1121)    6  AN FF 3NS-CMS */
  double GeneratorPower;				   /* (0019,1125)   14  AN FF 3NS-CMS */
  long GeneratorVoltage;				   /* (0019,1126)    6  AN FF 3NM-CMS */
  long GeneratorVoltageDual;
  long MasterControlMask;				   /* (0019,1140)    4  BD HX 3NM-CMS */
  long Gap1140;
  short ProcessingMask[5];				   /* (0019,1142)    2  BI HX 3NM-CMS */
  short Gap1142[3];
  long NumberOfVirtuellChannels;			   /* (0019,1162)    6  AN FF 3NS-CMS */
  long NumberOfReadings;				   /* (0019,1170)    6  AN FF 3NS-CMS */
  long NumberOfProjections;				   /* (0019,1174)    6  AN FF 3NS-CMS */
  long NumberOfBytes;					   /* (0019,1175)    6  AN FF 3NS-CMS */
  long ReconstructionAlgorithmSet[3];                      /* (0019,1180)    6  AN FF 3NS-CMS */
  long Gap1180[2];
  long ReconstructionAlgorithmIndex;                       /* (0019,1181)    6  AN FF 3NS-CMS */
  char RegenerationSoftwareVersion[LENGTH_SOFTWARE_VERSION + 1]; /* (0019,1182)    8  AT FF 3NS-CMS */
} shadow_acquisition_ct_t;


typedef struct shadow_acquisition_mr_tag		   /* MR shadowsubgroup */
{
  double TotalMeasurementTime;				   /* (0019,1210)   14  AN FF 3NS-CMS */
  double StartDelayTime;				   /* (0019,1212)   14  AN FF 3NS-CMS */
  long NumberOfPhases;					   /* (0019,1214)    6  AN FF 2DS-CMS */
  long SequenceControlMask[2];				   /* (0019,1216)    4  BD HX 3NM-CMS */
  long Gap1216[2];
  long NumberOfFourierLinesNominal;			   /* (0019,1220)    6  AN FF 3NS-CMS */
  long NumberOfFourierLinesCurrent;			   /* (0019,1221)    6  AN FF 3NS-CMS */
  long NumberOfFourierLinesAfterZero;			   /* (0019,1226)    6  AN FF 3NS-CMS */
  long FirstMeasuredFourierLine;			   /* (0019,1228)    6  AN FF 3NS-CMS */
  long AcquisitionColumns;				   /* (0019,1230)    6  AN FF 3NS-CMS */
  long ReconstructionColumns;				   /* (0019,1231)    6  AN FF 3NS-CMS */
  long NumberOfAverages;				   /* (0019,1250)    6  AN FF 3NS-CMS */
  double FlipAngle;					   /* (0019,1260)   14  AN FF 3NS-CMS */
  long NumberOfPrescans;				   /* (0019,1270)    6  AN FF 3NS-CMS */
  filter_type_t FilterTypeRawData;			   /* (0019,1281)    8  AT EV 2DS-CMS */
  filter_parameter_t FilterParameterRawData;		   /* (0019,1282)   14  AN FF 3NM-CMS */
  filter_type_image_t FilterTypeImageData;		   /* (0019,1283)    8  AT EV 2DS-CMS */
  int Pad1;                                                /* Dummy for byte alignament */
  filter_parameter_t FilterParameterImageData;		   /* (0019,1284)   14  AN FF 3NM-CMS */
  filter_type_t FilterTypePhaseCorrection;		   /* (0019,1285)    8  AT EV 2DS-CMS */
  int Pad2;                                                /* Dummy for byte alignament */
  filter_parameter_t FilterParameterPhaseCorrection;	   /* (0019,1286)   14  AN FF 3NM-CMS */
  long NumberOfSaturationRegions;			   /* (0019,1290)    6  AN FF 2DS-CMS */
  /*  short Pad4[5];                                                /* Dummy for byte alignment */
  int Pad3;                                                /* Dummy for byte alignament */
  double ImageRotationAngle;				   /* (0019,1294)   14  AN FF 3NS-CMS */
  double DwellTime;					   /* (0019,1213)   14  AN FF 3NS-CMS */
  long CoilIdMask[3];					   /* (0019,1296)    4  BD HX 3NM-CMS */
  long Gap1296[2];
  int Pad4;                                                /* Dummy for byte alignament */
  image_location_t CoilPosition;			   /* (0019,1298)   14  AN FF 2NM-CMS */
  double TotalMeasurementTimeCur;			   /* (0019,1211)   14  AN FF 3NS-CMS*/
  long MeasurementStatusMask;				   /* (0019,1218)    4  BD FF 2DS-CMS */
} shadow_acquisition_mr_t;


typedef struct shadow_acquisition_ct_conf_tag		   /* CT configuration and adjust
							      shadowsubgroup */
{
  long DistanceSourceToSourceSideCollimator;		   /* (0019,1310)    6  AN FF 3NS-CMS */
  long DistanceSourceToDetectorSideCollimator;		   /* (0019,1311)    6  AN FF 3NS-CMS */
  long NumberOfPossibleChannels;			   /* (0019,1320)    6  AN FF 3NS-CMS */
  long MeanChannelNumber;				   /* (0019,1321)    6  AN FF 3NS-CMS */
  double DetectorSpacing;				   /* (0019,1322)   14  AN FF 3NS-CMS */
  double ReadingIntegrationTime;			   /* (0019,1324)   14  AN FF 3NS-CMS */
  double DetectorAlignment;				   /* (0019,1350)   14  AN FF 3NS-CMS */
  double FocusAlignment;				   /* (0019,1360)   14  AN FF 3NS-CMS */
  long FocalSpotDeflectionAmplitude;			   /* (0019,1365)    4  BD HX 3NS-CMS */
  long FocalSpotDeflectionPhase;			   /* (0019,1366)    4  BD HX 3NS-CMS */
  long FocalSpotDeflectionOffset;			   /* (0019,1367)    4  BD HX 3NS-CMS */
  double WaterScalingFactor;				   /* (0019,1370)   14  AN FF 3NS-CMS */
  double InterpolationFactor;				   /* (0019,1371)   14  AN FF 3NS-CMS */
  patient_region_t PatientRegion;			   /* (0019,1380)    4  AT EV 3NS-CMS */
  patient_phase_t PatientPhaseOfLife;			   /* (0019,1382)    8  AT EV 3NS-CMS */
  double DetectorCenter;				   /* (0019,1323)   14  AN FF 3NS-CMS */
} shadow_acquisition_ct_conf_t;


typedef struct shadow_acquisition_mr_conf_tag		   /* MR configuration and adjust
							      shadowsubgroup */
{
  double MagneticFieldStrength;				   /* (0019,1412)   14  AN FF 3NS-CMS */
  double ADCVoltage;					   /* (0019,1414)   14  AN FF 3NS-CMS */
  double TransmitterAmplitude;				   /* (0019,1420)   14  AN FF 3NS-CMS */
  long NumberOfTransmitterAmplitudes;			   /* (0019,1421)    6  AN FF 3NS-CMS */
  int Pad1;                                                /* Dummy for byte alignment */
  double TransmitterCalibration;			   /* (0019,1424)   14  AN FF 3NS-CMS */
  double ReceiverTotalGain;				   /* (0019,1450)   14  AN FF 3NS-CMS */
  double ReceiverAmplifierGain;				   /* (0019,1451)   14  AN FF 3NS-CMS */
  double ReceiverPreamplifierGain;			   /* (0019,1452)   14  AN FF 3NS-CMS */
  double ReceiverCableAttenuation;			   /* (0019,1454)   14  AN FF 3NS-CMS */
  double ReconstructionScaleFactor;			   /* (0019,1460)   14  AN FF 3NS-CMS */
  double PhaseGradientAmplitude;			   /* (0019,1470)   14  AN FF 3NS-CMS */
  double ReadoutGradientAmplitude;			   /* (0019,1471)   14  AN FF 3NS-CMS */
  double SelectionGradientAmplitude;			   /* (0019,1472)   14  AN FF 3NS-CMS */
  gradient_delay_time_t GradientDelayTime;		   /* (0019,1480)   14  AN FF 3NS-CMS */
  char SensitivityCorrectionLabel[LENGTH_LABEL + 1];	   /* (0019,1490)   26  AT FF 2DS-CMS */
  int Pad2;                                                /* Dummy for byte alignment */
  double ADCOffset[2];					   /* (0019,1416)   14  AN FF 3NM-CMS */
  double TransmitterAttenuator;                            /* (0019,1422)   14  AN FF 3NS-CMS */
  double TransmitterReference;                             /* (0019,1426)   14  AN FF 3NS-CMS */
  double ReceiverReferenceGain;                            /* (0019,1455)   14  AN FF 3NS-CMS */
  long ReceiverFilterFrequency;                            /* (0019,1456)    6  AN FF 3NS-CMS */
  int Pad3;                                                /* Dummy for byte alignment */
  double ReferenceScaleFactor;                             /* (0019,1462)   14  AN FF 3NS-CMS */
  double TotalGradientDelayTime;                           /* (0019,1482)   14  AN FF 3NS-CMS */
  long RfWatchdogMask;                                     /* (0019,14A0)    4  BD HX 3NM-CMS */
  int Pad4;                                                /* Dummy for byte alignment */
  double RfPowerErrorIndicator;                            /* (0019,14A2)   14  AN FF 3NS-CMS */
  sar_sed_t SarWholeBody;                                  /* (0019,14A5)   14  AN FF 3NS-CMS */
  sar_sed_t Sed;                                           /* (0019,14A6)   14  AN FF 3NS-CMS */
  long AdjustmentStatusMask;				   /* (0019,14B0)    4 BD FF 2DS-CMS */
} shadow_acquisition_mr_conf_t;


typedef struct shadow_acquisition_acq_tag		   /* acquisition shadowsubgroup */
{
  char ParameterFileName[LENGTH_FILE_NAME + 1];		   /* (0019,1510)   64  AT FF 3NS-CMS */
  char SequenceFileName[LENGTH_FILE_NAME + 1];		   /* (0019,1511)   64  AT FF 3NS-CMS */
  char SequenceFileOwner[LENGTH_SEQUENCE_INFO + 1];        /* (0019,1512)    8  AT FF 2DS-CMS */
  char SequenceDescription[LENGTH_SEQUENCE_INFO + 1];      /* (0019,1513)    8  AT FF 2DS-CMS */
} shadow_acquisition_acq_t;


/*******************************************/
/* Relationship Information (Group 0021'H) */
/*******************************************/

typedef struct shadow_relationship_med_cms_tag		   /* MED and CMS shadowsubgroups */
{
  double Gap1010;
  target_point_t Target;				   /* (0021,1011)   30  AT FF 2NM-MED */
  short RoiMask;					   /* (0021,1020)    2  BI HX 2NS-MED */
  int Pad1;                                                /* Dummy for byte alignment */
  field_of_view_t FoV;					   /* (0021,1120)   30  AN FF 2NM-CMS */
  view_direction_t ViewDirection;			   /* (0021,1130)    4  AT EV 2NS-CMS */
  rest_direction_t RestDirection;			   /* (0021,1132)    4  AT EV 2NS-CMS */
  image_location_t ImagePosition;			   /* (0021,1160)   14  AN FF 2NM-CMS */
  image_location_t ImageNormal;				   /* (0021,1161)   14  AN FF 2NM-CMS */
  double ImageDistance;					   /* (0021,1163)   14  AN FF 2NM-CMS */
  short ImagePositioningHistoryMask;			   /* (0021,1165)    2  BI HX 2DS-CMS */
  int Pad2;                                                /* Dummy for byte alignment */
  image_location_t ImageRow;				   /* (0021,116A)   14  AN FF 2NM-CMS */
  image_location_t ImageColumn;				   /* (0021,116B)   14  AN FF 2NM-CMS */
  patient_orientation_t PatientOrientationSet1;		   /* (0021,1170)    4  AT EV 2NM-CMS */
  patient_orientation_t PatientOrientationSet2;		   /* (0021,1171)    4  AT EV 2NM-CMS */
  char StudyName[LENGTH_LABEL + 1];			   /* (0021,1180)   26  AT FF 3NS-CMS */
  study_type_t StudyType;				   /* (0021,1182)    4  AT EV 3NS-CMS */
  double ImageMagnificationFactor;                         /* (0021,1122)   14  AN FF 2DS-CMS */
} shadow_relationship_med_cms_t;


typedef struct shadow_relationship_ct_tag		   /* CT common shadowsubgroups */
{
  long RotationAngle;					   /* (0021,1210)    6  AN FF 3NS-CMS */
  long StartAngle;					   /* (0021,1211)    6  AN FF 3NS-CMS */
  long TubePosition;					   /* (0021,1230)    6  AN FF 3NS-CMS */
  long LengthOfTopogram;				   /* (0021,1232)    6  AN FF 3NS-CMS */
  double CorrectionFactor;				   /* (0021,1234)   14  AN FF 3NS-CMS */
  long MaximumTablePosition;				   /* (0021,1236)    6  AN FF 3NS-CMS */
  long TableMoveDirectionCode;				   /* (0021,1240)    6  AN FF 3NS-CMS */
  long VectorStartRow;					   /* (0021,1250)    6  AN FF 3NS-CMS */
  long VectorRowStep;					   /* (0021,1251)    6  AN FF 3NS-CMS */
  long VectorStartColumn;				   /* (0021,1252)    6  AN FF 3NS-CMS */
  long VectorColumnStep;				   /* (0021,1253)    6  AN FF 3NS-CMS */
  long VoiStartRow;					   /* (0021,1245)    6  AN FF 3NS-CMS */
  long VoiStopRow;					   /* (0021,1246)    6  AN FF 3NS-CMS */
  long VoiStartColumn;					   /* (0021,1247)    6  AN FF 3NS-CMS */
  long VoiStopColumn;					   /* (0021,1248)    6  AN FF 3NS-CMS */
  long VoiStartSlice;					   /* (0021,1249)    6  AN FF 3NS-CMS */
  long VoiStopSlice;					   /* (0021,124A)    6  AN FF 3NS-CMS */
  long RangeTypeCode;					   /* (0021,1260)    6  AN FF 3NS-CMS */
  long ReferenceTypeCode;				   /* (0021,1262)    6  AN FF 3NS-CMS */
  object_orientation_t ObjectOrientation;		   /* (0021,1270)   14  AN FF 3NS-CMS */
  object_orientation_t LightOrientation;		   /* (0021,1272)   14  AN FF 3NS-CMS */
  double LightBrightness;				   /* (0021,1275)   14  AN FF 3NS-CMS */
  double LightContrast;					   /* (0021,1276)   14  AN FF 3NS-CMS */
  object_threshold_t OverlayThreshold;			   /* (0021,127A)   12  AN FF 3NM-CMS */
  object_threshold_t SurfaceThreshold;			   /* (0021,127B)   12  AN FF 3NM-CMS */
  object_threshold_t GreyScaleThreshold;		   /* (0021,127C)   12  AN FF 3NM-CMS */
} shadow_relationship_ct_t;


typedef struct shadow_relationship_mr_tag		   /* MR common shadowsubgroups */
{
  long PhaseCorRowSeq;					   /* (0021,1320)    6  AN FF 3NS-CMS */
  long PhaseCorColSeq;					   /* (0021,1321)    6  AN FF 3NS-CMS */
  long PhaseCorRowRec;					   /* (0021,1322)    6  AN FF 3NS-CMS */
  long PhaseCorColRec;					   /* (0021,1324)    6  AN FF 3NS-CMS */
  long NumberOf3DRawPartNom;				   /* (0021,1330)    6  AN FF 3NS-CMS */
  long NumberOf3DRawPartCur;				   /* (0021,1331)    6  AN FF 3NS-CMS */
  long NumberOf3DImaPart;				   /* (0021,1334)    6  AN FF 3NS-CMS */
  long Actual3DImaPartNumber;				   /* (0021,1336)    6  AN FF 3NS-CMS */
  long Gap1338;
  long NumberOfSlicesNom;				   /* (0021,1340)    6  AN FF 3NS-CMS */
  long NumberOfSlicesCur;				   /* (0021,1341)    6  AN FF 3NS-CMS */
  long CurrentSliceNumber;				   /* (0021,1342)    6  AN FF 3NS-CMS */
  long CurrentGroupNumber;				   /* (0021,1343)    6  AN FF 3NS-CMS */
  long MipStartRow;					   /* (0021,1345)    6  AN FF 3NS-CMS */
  long MipStopRow;					   /* (0021,1346)    6  AN FF 3NS-CMS */
  long MipStartColumn;					   /* (0021,1347)    6  AN FF 3NS-CMS */
  long MipStopColumn;					   /* (0021,1348)    6  AN FF 3NS-CMS */
  long MipStartSlice;					   /* (0021,1349)    6  AN FF 3NS-CMS */
  long MipStopSlice;					   /* (0021,134A)    6  AN FF 3NS-CMS */
  long SignalMask;					   /* (0021,1350)    4  BI HX 2DS-CMS */
  long Gap1350;
  long DelayAfterTrigger;				   /* (0021,1352)    6  AN FF 3NS-CMS */
  long RRInterval;					   /* (0021,1353)    6  AN FF 3NS-CMS */
  int Pad1;                                                /* Dummy for byte alignment */
  double NumberOfTriggerPulses;				   /* (0021,1354)   14  AN FF 3NS-CMS */
  double RepetitionTime;				   /* (0021,1356)   14  AN FF 3NS-CMS */
  gate_phase_t GatePhase;				   /* (0021,1357)   12  AT EV 3NS-CMS */
  int Pad2;                                                /* Dummy for byte alignment */
  double GateThreshold;					   /* (0021,1358)   14  AN FF 3NS-CMS */
  double GateRatio;					   /* (0021,1359)   14  AN FF 3NS-CMS */
  long NumberOfInterpolatedImages;			   /* (0021,1360)    6  AN FF 3NS-CMS */
  long NumberOfEchoes;					   /* (0021,1370)    6  AN FF 3NS-CMS */
  double SecondEchoTime;				   /* (0021,1372)   14  AN FF 3NS-CMS */
  double SecondRepetitionTime;				   /* (0021,1373)   14  AN FF 3NS-CMS */
  long CardiacCode;				           /* (0021,1380)    6  AN FF 3NS-CMS */
  int Pad3;                                                /* Dummy for byte alignment */
  double CurrentSliceDistanceFactor;                       /* (0021,1344)   14  AN FF 3NS-CMS */
  order_of_slices_t OrderOfSlices;                         /* (0021,134F)   12  AT EV 3NS-CMS */
  int Pad4;                                                /* Dummy for byte alignment */
  double SlabThickness;					   /* (0021,1339)    6  AN FF 3NS-CMS */
} shadow_relationship_mr_t;


typedef struct shadow_relationship_ct_spe_tag		   /* CT special shadowsubgroups */
{
  long EvaluationMask[2];				   /* (0021,2220)    4  BD FF 2DM-CMS */
  long Gap2220[2];
  short ExtendedProcessingMask[7];			   /* (0021,2230)    4  BD FF 2DM-CMS */
  short Gap2230[3];
  long CreationMask[2];					   /* (0021,2210)    2  BI HX 3NM-CMS
 */
  long Gap2210[2];
} shadow_relationship_ct_spe_t;


typedef struct shadow_relationship_mr_spe_tag		   /* MR special shadowsubgroups */
{
  double EpiReconstructionPhase;			   /* (0019,12A0)   14  AN FF 3NS-CMS */
  double EpiReconstructionSlope;			   /* (0019,12A1)   14  AN FF 3NS-CMS */
  double EpiCapacity[6];				   /* (0019,14C1)   14  AN FF 3NM-CMS */
  double EpiInductance[3];				   /* (0019,14C2)   14  AN FF 3NM-CMS */
  long EpiSwitchConfigurationCode[3];			   /* (0019,14C3)    6  AN FF 3NM-CMS */
  long EpiSwitchHardwareCode[3];			   /* (0019,14C4)    6  AN FF 3NM-CMS */
  long EpiSwitchDelayTime[6];				   /* (0019,14C5)    6  AN FF 3NM-CMS */
  char EpiFileName[LENGTH_FILE_NAME + 1];		   /* (0019,1514)   64  AT FF 3NS-CMS */
  
} shadow_relationship_mr_spe_t;


/*************************************************/
/* Image Presentation Information (Group 0029'H) */
/*************************************************/

typedef struct shadow_presentation_tag
{
  window_style_t WindowStyle;				   /* (0029,1110)    8  AT EV 2DS-CMS */
  pixel_quality_code_t PixelQualityCode;		   /* (0029,1120)   12  AT FF 2DM-CMS */
  pixel_quality_value_t PixelQualityValue;		   /* (0029,1122)    6  AN FF 3NM-CMS */
  save_code_t ArchiveCode;				   /* (0029,1150)    8  AN EV 3NS-CMS */
  save_code_t ExposureCode;				   /* (0029,1151)    8  AN EV 3NS-CMS */
  long SortCode;					   /* (0029,1152)    6  AN FF 3NS-CMS */
  long Splash;						   /* (0029,1160)    6  AN FF 3NS-CMS */
} shadow_presentation_t;

#endif




/*==  HELP TEXT  ===========================================================================*/

#ifdef DS_STC_TOOL

"G09.Ide.DataObjectSubtype.M",
"Data Object Subtype (0009,0041) - main: CT | MR Signed | MR Unsigned",
"G09.Ide.DataObjectSubtype.D",
"Data Object Subtype (0009,0041) - defined by: Predefined | defined by User",
"G09.Ide.DataObjectSubtype.S - sub: NONE",
"Data Object Subtype (0009,0041)",
"G09.Ide.NumberOfMeasurements", "Number of Measurements (0009,1200)",
"G09.Ide.StorageMode", 
"Storage Mode (0009,1210): COMPRESS | EXPANDED | MIP_MPR | REDUCED | XDR",
"G09.Ide.EvaluationMask", "Evaluation Mask - Image (0009,1212)",
"G09.Ide.LastMoveDate.Year", "Last Move Date (0009,1226)",
"G09.Ide.LastMoveTime.Hour", "Last Move Time (0009,1227)",
"G09.Ide.GeneratorIdentificationLabel", "Generator Identification Label (0009,1310)",
"G09.Ide.GantryIdentificationLabel", "Gantry Identification Label (0009,1311)",
"G09.Ide.XRayTubeIdentificationLabel", "X-Ray Tube Identification Label (0009,1312)",
"G09.Ide.DetectorIdentificationLabel", "Detector Identification Label (0009,1313)",
"G09.Ide.DASIdentificationLabel", "DAS Identification Label (0009,1314)",
"G09.Ide.SMIIdentificationLabel", "SMI Identification Label (0009,1315)",
"G09.Ide.CPUIdentificationLabel", "CPU Identification Label (0009,1316)",
"G09.Ide.HeaderVersion", "Header Version (0009,1320)",

"G11.Pat.Organ", "Organ (0011,1010)",
"G11.Pat.RegistrationDate.Year", "Registration Date (0011,1110)",
"G11.Pat.RegistrationTime.Hour", "Registration Time (0011,1111)",
"G11.Pat.UsedPatientWeight", "Used Patient Weight (0011,1123) in kg",
"G11.Pat.OrganCode", "Organ Code (0011,1140)",

"G13.PatMod.ModifyingPhysician", "Modifying Physician (0013,1000)",
"G13.PatMod.ModificationDate.Year", "Modification Date (0013,1022)",
"G13.PatMod.ModificationTime.Hour", "Modification Time (0013,1010)",
"G13.PatMod.PatientName", "former Patient Name (0013,1020)",
"G13.PatMod.PatientId", "former Patient Id (0013,1022)",
"G13.PatMod.PatientBirthdate.Year", "former Patient Birthdate (0013,1030)",
"G13.PatMod.PatientWeight", "former Patient Weight (0013,1031)",
"G13.PatMod.PatientMaidenName", "former Patient Maiden Name (0013,1032)",
"G13.PatMod.ReferringPhysician", "former Referring Physician (0013,1033)",
"G13.PatMod.AdmittingDiagnosis", "former Admitting Diagnosis (0013,1034)",
"G13.PatMod.PatientSex", "former Patient Sex (0013,1035)",
"G13.PatMod.ProcedureDescription_1", "former Procedure Description_1 (0013,1040)",
"G13.PatMod.ProcedureDescription_2", "former Procedure Description_2",
"G13.PatMod.RestDirection", "former Rest Direction (0013,1042)",
"G13.PatMod.PatientPosition", "former Patient Position (0013,1044)",
"G13.PatMod.ViewDirection", "former View Direction (0013,1046)",

"G19.Acq1.CM.NetFrequency", "Net Frequency (0019,1010) in Hz",
"G19.Acq1.CM.MeasurementMode.M",
"Measurement Mode (0019,1020) - main: ADJUstment | EXAMination | TEST",
"G19.Acq1.CM.MeasurementMode.S",
"Measurement Mode (0019,1020) - subreason",
"G19.Acq1.CM.CalculationMode.M",
"CalculationMode (0019,1030) - main: NONE | A| PC | PU",
"G19.Acq1.CM.CalculationMode.S",
"Calculation Mode (0019,1030) - sub: NONE |BSP | IRS | SUN | VAX",
"G19.Acq1.CM.NoiseLevel", "Noise Level (0019,1050)",
"G19.Acq1.CM.NumberOfDataBytes", "Number of Data Bytes (0019,1060)",

"G19.Acq2.Ct.SourceSideCollimatorAperture",
"Source Side Collimator Aperture (0019,1110) in mm",
"G19.Acq2.Ct.DetectorSideCollimatorAperture",
"Detector Side Collimator Aperture (0019,1111) in mm",
"G19.Acq2.Ct.ExposureTime", "current Exposure Time (0019,1120) in msec",
"G19.Acq2.Ct.Exposure", "current Exposure (0019,1121) in mAs",
"G19.Acq2.Ct.GeneratorPower", "current Generator Power (0019,1125) in kW",
"G19.Acq2.Ct.GeneratorVoltage", "current Generator Voltage (0019,1126) in kV",
"G19.Acq2.Ct.GeneratorVoltageDual", "second value of Generator Voltage (0019,1126) in kV",
"G19.Acq2.Ct.MasterControlMask", "Master Control Mask (0019,1140)",
"G19.Acq2.Ct.ProcessingMask", "Processing Mask (0019,1142)",
"G19.Acq2.Ct.NumberOfVirtuellChannels", "Number of Virtuell Channels (0019,1162)",
"G19.Acq2.Ct.NumberOfReadings", "Number of Readings (0019,1170)",
"G19.Acq2.Ct.NumberOfProjections", "Number of Projections (0019,1174)",
"G19.Acq2.Ct.NumberOfBytes", "Number of Bytes (0019,1175)",
"G19.Acq2.Ct.ReconstructionAlgorithmSet", 
"Reconstruction Algorithm Set (0019,1180): smooth, standard, sharp RPF number",
"G19.Acq2.Ct.ReconstructionAlgorithmIndex", "Reconstruction Algorithm Index (0019,1181)",
"G19.Acq2.Ct.RegenerationSoftwareVersion", "Regeneration Software Version (0019,1182)",

"G19.Acq2.Mr.TotalMeasurementTime", "Total Measurement Time - nominal (0019,1210) in sec",
"G19.Acq2.Mr.TotalMeasurementTimeCur", "Total Measurement Time - current (0019,1211) in sec",
"G19.Acq2.Mr.StartDelayTime", "Start Delay Time (0019,1212) in sec",
"G19.Acq2.Mr.DwellTime", "Dwell Time (0019,1213) in usec",
"G19.Acq2.Mr.NumberOfPhases", "Number of Phases (0019,1214)",
"G19.Acq2.Mr.SequenceControlMask", "Sequence Control Mask (0019,1216)",
"G19.Acq2.Mr.MeasurementStatusMask", "Measurement Status Mask (0019,1218)",
"G19.Acq2.Mr.NumberOfFourierLinesNominal", "nominal Number of Fourier Lines (0019,1220)",
"G19.Acq2.Mr.NumberOfFourierLinesCurrent", "current Number of Fourier Lines (0019,1221)",
"G19.Acq2.Mr.NumberOfFourierLinesAfterZero", "Number of Fourier Lines after Zero (0019,1226)",
"G19.Acq2.Mr.FirstMeasuredFourierLine", "First Measured Fourier Line (0019,1228)",
"G19.Acq2.Mr.AcquisitionColumns", "Acquisition Columns (0019,1230)",
"G19.Acq2.Mr.ReconstructionColumns", "Reconstruction Columns (0019,1231)",
"G19.Acq2.Mr.NumberOfAverages", "current Number of Averages (0019,1250)",
"G19.Acq2.Mr.FlipAngle", "Flip Angle (0019,1260) in degree",
"G19.Acq2.Mr.NumberOfPrescans", "Number of Prescans (0019,1270) in main loop",
"G19.Acq2.Mr.FilterTypeRawData", 
"Filter Type Raw Data (0019,1281): NONE | EXTERNAL | FERMI | GAUSS |HANNING",
"G19.Acq2.Mr.FilterParameterRawData.Value1", "Filter Parameter Raw Data (0019,1282)",
"G19.Acq2.Mr.FilterTypeImageData", 
"Filter Type Image Data (0019,1283): NONE | NO1",
"G19.Acq2.Mr.FilterParameterImageData.Value1", "Filter Parameter Image Data (0019,1284)",
"G19.Acq2.Mr.FilterTypePhaseCorrection", 
"Filter Type Phase Correction (0019,1285): NONE | EXTERNAL | FERMI | GAUSS |HANNING",
"G19.Acq2.Mr.FilterParameterPhaseCorrection", "Filter Parameter Phase Correction (0019,1286)",
"G19.Acq2.Mr.NumberOfSaturationRegions", "Number of Saturation Regions (0019,1290)",
"G19.Acq2.Mr.ImageRotationAngle",
"Image Rotation Angle (0019,1294) for readout direction in radiant",
"G19.Acq2.Mr.CoilIdMask", "Coil ID Mask (0019,1296)",
"G19.Acq2.Mr.CoilPosition.Sag", "Coil Position (0019,1298) vector (sag, cor, tra) in mm",
"G21.Rel3.Mr.EpiReconstructionPhase", "EPI Reconstruction Phase (0019,12A0)",
"G21.Rel3.Mr.EpiReconstructionSlope", "EPI Reconstruction Slope (0019,12A1)",

"G19.Acq3.Ct.DistanceSourceToSourceSideCollimator",
"Distance Source to Source Side Collimator (0019,1310) in mm",
"G19.Acq3.Ct.DistanceSourceToDetectorSideCollimator",
"Distance Source to Detector Side Collimator (0019,1311) in mm",
"G19.Acq3.Ct.NumberOfPossibleChannels", "Number of Possible Channels (0019,1320)",
"G19.Acq3.Ct.MeanChannelNumber", "Mean Channel Number (0019,1321)",
"G19.Acq3.Ct.DetectorSpacing", "Detector Spacing (0019,1322) in minutes",
"G19.Acq3.Ct.DetectorCenter", "Detector Center (0019,1323) in channels as real",
"G19.Acq3.Ct.ReadingIntegrationTime", "Reading Integration Time (0019,1324) in ms",
"G19.Acq3.Ct.DetectorAlignment", "Detector Alignment (0019,1350) in mm",
"G19.Acq3.Ct.FocusAlignment", "Focus Alignment (0019,1360) in channel digits",
"G19.Acq3.Ct.FocalSpotDeflectionAmplitude", 
"Focal Spot Deflection Amplitude (0019,1365) in DMS digits",
"G19.Acq3.Ct.FocalSpotDeflectionPhase", 
"Focal Spot Deflection Phase (0019,1366) in DMS digits",
"G19.Acq3.Ct.FocalSpotDeflectionOffset", 
"Focal Spot Deflection Offset (0019,1367) in DMS digits",
"G19.Acq3.Ct.WaterScalingFactor", "Water Scaling Factor (0019,1370)",
"G19.Acq3.Ct.InterpolationFactor", "Interpolation Factor (0019,1371)",
"G19.Acq3.Ct.PatientRegion", "Patient Region (0019,1380): BODY | HEAD",
"G19.Acq3.Ct.PatientPhaseOfLife", "Patient Phase of Life (0019,1382): ADULT | CHILD",

"G19.Acq3.Mr.MagneticFieldStrength", "Magnetic Field Strength (0019,1412) in T",
"G19.Acq3.Mr.ADCVoltage", "ADC Voltage (0019,1414) in V",
"G19.Acq3.Mr.ADCOffset", "ADC Offset (0019,1416): real value, imaginary value",
"G19.Acq3.Mr.TransmitterAmplitude", "Transmitter Amplitude (0019,1420) in V",
"G19.Acq3.Mr.NumberOfTransmitterAmplitudes", "Number of Transmitter Amplitudes (0019,1421)",
"G19.Acq3.Mr.TransmitterAttenuator", "Transmitter Attenuator (0019,1422) in dB",
"G19.Acq3.Mr.TransmitterCalibration", "Transmitter Calibration (0019,1424) in V",
"G19.Acq3.Mr.TransmitterReference", "Transmitter Reference (0019,1426) in V",
"G19.Acq3.Mr.ReceiverTotalGain", "Receiver Total Gain (0019,1450) in dB",
"G19.Acq3.Mr.ReceiverAmplifierGain", "Receiver Amplifier Gain (0019,1451) in dB",
"G19.Acq3.Mr.ReceiverPreamplifierGain", "Receiver Preamplifier Gain (0019,1452) in dB",
"G19.Acq3.Mr.ReceiverCableAttenuation", "Receiver Cable Attenuation (0019,1454) in dB",
"G19.Acq3.Mr.ReceiverReferenceGain", "Receiver Reference Gain (0019,1455) in dB",
"G19.Acq3.Mr.ReceiverFilterFrequency", "Receiver Filter Frequency (0019,1456) in Hz",
"G19.Acq3.Mr.ReconstructionScaleFactor", "Reconstruction Scale Factor (0019,1460)",
"G19.Acq3.Mr.ReferenceScaleFactor", "Reference Scale Factor (0019,1462)",
"G19.Acq3.Mr.PhaseGradientAmplitude", "Phase Gradient Amplitude (0019,1470) in mT/m",
"G19.Acq3.Mr.ReadoutGradientAmplitude", "Readout Gradient Amplitude (0019,1471) in mT/m",
"G19.Acq3.Mr.SelectionGradientAmplitude", "Selection Gradient Amplitude (0019,1472) in mT/m",
"G19.Acq3.Mr.GradientDelayTime.X", "Gradient Delay Time (0019,1480) in usec",
"G19.Acq3.Mr.TotalGradientDelayTime", "Total Gradient Delay Time (0019,1482) in usec",
"G19.Acq3.Mr.SensitivityCorrectionLabel", "Sensitivity Correction Label (0019,1490)",
"G19.Acq3.Mr.RfWatchdogMask", "RF Watchdog Mask (0019,14A0)",
"G19.Acq3.Mr.RfPowerErrorIndicator", "RF Power Error Indicator (0019,14A2)",
"G19.Acq3.Mr.SarWholeBody.Lim", 
"Specific Absorption Rate (SAR) - whole body (0019,14A5) - limit in W/kg",
"G19.Acq3.Mr.SarWholeBody.Cal", 
"Specific Absorption Rate (SAR) - whole body (0019,14A5) - calculated value in W/kg",
"G19.Acq3.Mr.SarWholeBody.Det", 
"Specific Absorption Rate (SAR) - whole body (0019,14A5) - detected value in W/kg",
"G19.Acq3.Mr.Sed.Lim", 
"Specific Energy Dose (SED) (0019,14A6) - limit in Wmin/kg",
"G19.Acq3.Mr.Sed.Cal", 
"Specific Energy Dose (SED) (0019,14A6) - calculated value in Wmin/kg",
"G19.Acq3.Mr.Sed.Det", 
"Specific Energy Dose (SED) (0019,14A6) - detected value in Wmin/kg",
"G19.Acq3.Mr.AdjustmentStatusMask", "Adjustment Status Mask (0019,14B0)",
"G21.Rel3.Mr.EpiCapacity", "EPI Capacity (0019,14C1) in nF",
"G21.Rel3.Mr.EpiInductance", "EPI Inductance (0019,14C2) in uH",
"G21.Rel3.Mr.EpiSwitchConfigurationCode", 
"EPI Switch Configuration Code (0019,14C3): 0 (without) | 1 (two) | 2 (four)",
"G21.Rel3.Mr.EpiSwitchHardwareCode", 
"EPI Switch Hardware Code (0019,14C4): 0 (no) | 1 (thyristor) | 2 (transistor)",
"G21.Rel3.Mr.EpiSwitchDelayTime", "EPI Switch Delay Time (0019,14C5) in ns",

"G19.Acq4.CM.ParameterFileName", "Parameter File Name (0019,1510)",
"G19.Acq4.CM.SequenceFileName", "Sequence File Name (0019,1511)",
"G19.Acq4.CM.SequenceFileOwner", "Sequence File Owner (0019,1512)",
"G19.Acq4.CM.SequenceDescription", "Sequence Description (0019,1513)",
"G21.Rel3.Mr.EpiFileName", "EPI File Name (0019,1514)",

"G21.Rel1.CM.Target.X", "Target (0021,1011)",
"G21.Rel1.CM.RoiMask", "ROI Mask (0021,1020)",
"G21.Rel1.CM.FoV.Height", "Field of View (0021,1120) - height in mm",
"G21.Rel1.CM.FoV.Width", "Field of View (0021,1120) - width in mm",
"G21.Rel1.CM.ImageMagnificationFactor", "Image Magnification Factor (0021,1122)",
"G21.Rel1.CM.ViewDirection",
"View Direction (0021,1130): HEAD | FEET | AtoP | PtoA | RtoL | LtoR",
"G21.Rel1.CM.RestDirection", "Rest Direction (0021,1132): HEAD | FEET",
"G21.Rel1.CM.ImagePosition.Sag", "Image Position (0021,1160) vector (sag, cor, tra) in mm",
"G21.Rel1.CM.ImageNormal.Sag", "Image Normal (0021,1161) cosinus directions (sag, cor, tra)",
"G21.Rel1.CM.ImageDistance", "Image Distance (0021,1163) in mm",
"G21.Rel1.CM.ImagePositioningHistoryMask", "Image Positioning History Mask (0021,1165)",
"G21.Rel1.CM.ImageRow.Sag", "Image Row (0021,116A) cosinus directions (sag, cor, tra)",
"G21.Rel1.CM.ImageColumn.Sag", "Image Column (0021,116B) cosinus directions (sag, cor, tra)",
"G21.Rel1.CM.PatientOrientationSet1.Y",
"Patient Orientation Set 1 (0021,1170): Y = up, X = left, Z = back",
"G21.Rel1.CM.PatientOrientationSet2.Y",
"Patient Orientation Set 2 (0021,1171): Y = down, X = right, Z = front",
"G21.Rel1.CM.StudyName", "Study Name (0021,1180)",
"G21.Rel1.CM.StudyType", "Study Type (0021,1182): CRE | MEA | MIP | MPR | RAW",

"G21.Rel2.Ct.RotationAngle", "Rotation Angle (0021,1210) in degree",
"G21.Rel2.Ct.StartAngle", "Start Angle (0021,1211) in degree (0.0 == 03:00)",
"G21.Rel2.Ct.TubePosition", "Tube Position (0021,1230) in degree (0.0 == 03:00)",
"G21.Rel2.Ct.LengthOfTopogram", "Length of Topogram (0021,1232) in mm",
"G21.Rel2.Ct.CorrectionFactor", "Correction Factor (0021,1234) for topogram in x-direction",
"G21.Rel2.Ct.MaximumTablePosition", "Maximum Table Position (0021,1236) in mm",
"G21.Rel2.Ct.TableMoveDirectionCode", "Tabel Move Direction Code (0019,1240): -1 (into) | 1 (out)",
"G21.Rel2.Ct.VoiStartRow", "VOI Start Row (0021,1245)",
"G21.Rel2.Ct.VoiStopRow", "VOI Stop Row (0021,1246)",
"G21.Rel2.Ct.VoiStartColumn", "VOI Start Column (0021,1247)",
"G21.Rel2.Ct.VoiStopColumn", "VOI Stop Column (0021,1248)",
"G21.Rel2.Ct.VoiStartSlice", "VOI Start Slice (0021,1249)",
"G21.Rel2.Ct.VoiStopSlice", "VOI Stop Slice (0021,124A)",
"G21.Rel2.Ct.VectorStartRow", "Vector Start Row(0021,1250)",
"G21.Rel2.Ct.VectorRowStep", "Vector Row Step(0021,1251)",
"G21.Rel2.Ct.VectorStartColumn", "Vector Start Column(0021,1252)",
"G21.Rel2.Ct.VectorColumnStep", "Vector Column Step(0021,1253)",
"G21.Rel2.Ct.RangeTypeCode", "Range Type Code (0021,1260): 0 (sector) | 1 (parallel)",
"G21.Rel2.Ct.ReferenceTypeCode", 
"Reference Type Code (0021,1262): 0 (topo) | 1 (tra) | 2 (sag) | 3 (cor)",
"G21.Rel2.Ct.ObjectOrientation.Phi", 
"Object Orientation (0021,1270) vector (phi, theta, 1.0) in degree",
"G21.Rel2.Ct.LightOrientation.Phi", 
"Light Orientation (0021,1272) vector (phi, theta, 1.0) in degree",
"G21.Rel2.Ct.LightBrightness", "Light Brightness (0021,1275)",
"G21.Rel2.Ct.LightContrast", "Light Contrast (0021,1276)",
"G21.Rel2.Ct.OverlayThreshold.LowerBoundary", 
"Overlay Threshold (0021,127A) boundaries (lower, upper)",
"G21.Rel2.Ct.SurfaceThreshold.LowerBoundary", 
"Surface Threshold (0021,127B) boundaries (lower, upper)",
"G21.Rel2.Ct.GreyScaleThreshold.LowerBoundary", 
"Grey Scale Threshold (0021,127C) boundaries (lower, upper)",


"G21.Rel2.Mr.PhaseCorRowSeq", "Phase Corrections Rows (0021,1320) of actual sequence",
"G21.Rel2.Mr.PhaseCorColSeq", "Phase Corrections Columns (0021,1321) of actual sequence",
"G21.Rel2.Mr.PhaseCorRowRec", "Phase Corrrections Rows (0021,1322) of actual reconstruction",
"G21.Rel2.Mr.PhaseCorColRec", "Phase Corrrections Columns (0021,1324) of actual reconstruction",
"G21.Rel2.Mr.NumberOf3DRawPartNom", "nominal Number of 3D Raw Partitions (0021,1330)",
"G21.Rel2.Mr.NumberOf3DRawPartCur", "current Number of 3D Raw Partitions (0021,1331)",
"G21.Rel2.Mr.NumberOf3DImaPart", "Number of 3D Image Partitions (0021,1334)",
"G21.Rel2.Mr.Actual3DImaPartNumber", "Actual 3D Image Partitions Number (0021,1336)",
"G21.Rel2.Mr.SlabThickness", "Slab Thickness (0021,1339) in mm",
"G21.Rel2.Mr.NumberOfSlicesNom", "nominal Number of Slices (0021,1340)",
"G21.Rel2.Mr.NumberOfSlicesCur", "current Number of Slices (0021,1341)",
"G21.Rel2.Mr.CurrentSliceNumber", "Current Slice Number (0021,1342)",
"G21.Rel2.Mr.CurrentGroupNumber", "Current Group Number (0021,1343)",
"G21.Rel2.Mr.CurrentSliceDistanceFactor", "Current Slice Distance Factor (0021,1344)",
"G21.Rel2.Mr.MipStartRow", "MIP Start Row (0021,1345)",
"G21.Rel2.Mr.MipStopRow", "MIP Stop Row (0021,1346)",
"G21.Rel2.Mr.MipStartColumn", "MIP Start Column (0021,1347)",
"G21.Rel2.Mr.MipStopColumn", "MIP Stop Column (0021,1348)",
"G21.Rel2.Mr.MipStartSlice", "MIP Start Slice (0021,1349)",
"G21.Rel2.Mr.MipStopSlice", "MIP StopSlice (0021,134A)",
"G21.Rel2.Mr.OrderOfSlices", 
"Order of Slices (0021,134F): NONE | ASCENDING | DECREASING | FREE | INTERLEAVED",
"G21.Rel2.Mr.SignalMask", "Signal Mask (0021,1350)",
"G21.Rel2.Mr.DelayAfterTrigger", "Delay After Trigger (0021,1352) in msec",
"G21.Rel2.Mr.RRInterval", "RR Interval (0021,1353) in msec",
"G21.Rel2.Mr.NumberOfTriggerPulses", "Number of Trigger Pulses (0021,1354)",
"G21.Rel2.Mr.RepetitionTime", "effective Repetition Time (0021,1356) in msec",
"G21.Rel2.Mr.GatePhase", "Gate Phase (0021,1357): EXPIRATION | INSPIRATION",
"G21.Rel2.Mr.GateThreshold", "Gate Threshold (0021,1358) in %",
"G21.Rel2.Mr.GateRatio", "Gate Ratio (0021,1359) in %",
"G21.Rel2.Mr.NumberOfInterpolatedImages", "Number of Interpolated Images (0021,1360)",
"G21.Rel2.Mr.NumberOfEchoes", "total Number of Echoes (0021,1370)",
"G21.Rel2.Mr.SecondEchoTime", "second Echo Time (0021,1372) in msec",
"G21.Rel2.Mr.SecondRepetitionTime", "second Repetition Time (0021,1373) in msec",
"G21.Rel2.Mr.CardiacCode", "Cardiac Code (0021,1380)",

"G21.Rel3.Ct.CreationMask", "Creation Mask - Service (0021,2210)",
"G21.Rel3.Ct.EvaluationMask", "Evaluation Mask - Service (0021,2220)",
"G21.Rel3.Ct.ExtendedProcessingMask", "Extended Processing Mask (0021,2230)",

"G29.Pre.WindowStyle",
"Window Style (0029,1110): NONE | DOUBLE | HIGH | STD 1 | STD 2",
"G29.Pre.PixelQualityCode.Min",
"Pixel Quality Code (0029,1120): NONE | ESTIMATED | EXACT",
"G29.Pre.PixelQualityValue.Min", "Pixel Quality Value (0029,1122)",
"G29.Pre.ArchiveCode", "Archive Code (0029,1150): NOT | MARKED | DONE",
"G29.Pre.ExposureCode", "Exposure Code (0029,1151): NOT | MARKED | DONE",
"G29.Pre.SortCode", 
"Sort Code (0029,1152): 0 (CH) | 1 (AN) | 2 (TE) | 3 (TR) | 4 (TD) | 5 (CA)",
"G29.Pre.Splash", "Splash (0029,1160)",

"G51.Txt_buf", "Image Text (0051,1010)",
"Gap.Fill_buf", "reserved: gap to fill Internal Header to kByte border",
#endif
