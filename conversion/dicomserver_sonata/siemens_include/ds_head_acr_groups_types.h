/*[-  HEADER FILE  -------------------------------------------------------------------------*/
/*
   Name:        ds_head_acr_groups_types.h

   Description: The header file defines the NEMA defined data set basic groups (even group
                numbers) as structures for internal use (internal header).


   Author:      THUMSER, Andreas (TH); Siemens AG UBMed CMS/SCE64; phone: 09131 844797
*/
/*]-----------------------------------------------------------------------------------------*/

#ifndef DS_HEAD_ACR_GROUPS_TYPES
#define DS_HEAD_ACR_GROUPS_TYPES

/******************************************/
/* Identifying Information (Group 0008'H) */
/******************************************/

typedef struct acr_identifying_tag
{
  ds_date_t StudyDate;					   /* (0008,0020)   10  AT DF 2NS-NEM */
  ds_date_t AcquisitionDate;				   /* (0008,0022)   10  AT DF 2NS-CMS */
  ds_date_t ImageDate;					   /* (0008,0023)   10  AT DF 2NS-CMS */
  ds_time_t StudyTime;					   /* (0008,0030)   12  AT DF 2NS-NEM */
  ds_time_t AcquisitionTime;				   /* (0008,0032)   12  AT DF 2NS-CMS */
  ds_time_t ImageTime;					   /* (0008,0033)   12  AT DF 2NS-CMS */
  data_set_subtype_t DataSetSubtype;			   /* (0008,0041)    8  AT EV 2NS-CMS */
  modality_t Modality;					   /* (0008,0060)    2  AT EV 1NS-CMS */
  char Manufacturer[LENGTH_MANUFACTURER + 1];		   /* (0008,0070)    8  AT FF 2NS-NEM */
  char InstitutionID[LENGTH_LABEL + 1];			   /* (0008,0080)   26  AT FF 2NS-NEM */
  char ReferringPhysician[LENGTH_LABEL + 1];		   /* (0008,0090)   26  AT FF 2NS-NEM */
  char StationID[LENGTH_LABEL + 1];			   /* (0008,1010)   26  AT FF 2NS-NEM */
  char ProcedureDescription_1[LENGTH_COMMENT + 1];	   /* (0008,1030)   52  AT FF 2DS-CMS */
  char ProcedureDescription_2[LENGTH_COMMENT + 1];
  char AdmittingDiagnosis[LENGTH_DIAGNOSIS + 1];	   /* (0008,1080)   40  AT FF 2DS-CMS */
  char ManufacturerModel[LENGTH_LABEL + 1];		   /* (0008,1090)   26  AT FF 2DS-CMS */
} acr_identifying_t;


/**************************************/
/* Patient Information (Group 0010'H) */
/**************************************/

typedef struct acr_patient_tag
{
  char PatientName[LENGTH_LABEL + 1];		           /* (0010,0010)   26  AT FF 2NS-NEM */
  char PatientId[LENGTH_PATIENT_ID + 1];		   /* (0010,0020)   12  AT FF 2NS-NEM */
  ds_date_t PatientBirthdate;				   /* (0010,0030)   10  AT DF 2NS-NEM */
  sex_t PatientSex;					   /* (0010,0040)    2  AT EV 2NS-NEM */
  char PatientMaidenName[LENGTH_LABEL + 1];		   /* (0010,1005)   26  AT FF 2DS-CMS */
  char PatientAge[LENGTH_AGE + 1];			   /* (0010,1010)    4  AT DF 2NS-CMS */
  double PatientSize;					   /* (0010,1020)    6  AN FF 3NS-NEM */
  long PatientWeight;					   /* (0010,1030)    6  AN FF 2NS-CMS */
} acr_patient_t;



/******************************************/
/* Acquisition Information (Group 0018'H) */
/******************************************/

typedef struct acr_acquisition_tag
{
  contrast_t Contrast;					   /* (0018,0010)    8  AT EV 2DS-NEM */
  int Pad1;					           /* Dummy for byte alignment */
  double SliceThickness;				   /* (0018,0050)   14  AN FF 2NS-NEM */
  long GeneratorVoltage;				   /* (0018,0060)    6  AN FF 2NM-NEM */
  long GeneratorVoltageDual;				   /* (0018,0060) */
  double RepetitionTime;				   /* (0018,0080)   14  AN FF 2NS-NEM */
  double EchoTime;					   /* (0018,0081)   14  AN FF 2NS-NEM */
  double InversionTime;					   /* (0018,0082)   14  AN FF 2NS-NEM */
  long NumberOfAverages;				   /* (0018,0083)    6  AN FF 3NS-NEM */
  int Pad2;					           /* Dummy for byte alignment */
  double ImagingFrequency;				   /* (0018,0084)   14  AN FF 2NS-NEM */
  nucleus_t Gap0085;
  long EchoNumber;					   /* (0018,0086)    6  AN FF 3NS-CMS */
  long DataCollectionDiameter;				   /* (0018,0090)    6  AN FF 3NS-NEM */
  char DeviceSerialNumber[LENGTH_LABEL + 1];		   /* (0018,1000)   26  AT FF 2DS-CMS */
  char SoftwareVersion[LENGTH_SOFTWARE_VERSION + 1];	   /* (0018,1020)    8  AT FF 2DS-CMS */
  long DistanceSourceToDetector;			   /* (0018,1110)    6  AN FF 3NS-NEM */
  long DistanceSourceToPatient;				   /* (0018,1111)    6  AN FF 3NS-NEM */
  long GantryTilt;					   /* (0018,1120)    6  AN FF 2NS-CMS */
  long TableHeight;					   /* (0018,1130)    6  AN FF 3NS-NEM */
  rotation_direction_t RotationDirection;		   /* (0018,1140)    2  AT EV 3NS-NEM */
  long ExposureTime;					   /* (0018,1150)    6  AN FF 2NS-CMS */
  long Exposure;					   /* (0018,1152)    6  AN FF 2NS-CMS */
  char FilterIdLabel[LENGTH_FILTER_ID + 1];		   /* (0018,1160)   12  AT FF 3NS-NEM */
  int Pad3;					           /* Dummy for byte alignment */
  double GeneratorPower;				   /* (0018,1170)   14  AN FF 2NS-CMS */
  double FocalSpot;					   /* (0018,1190    14  AN FF 3NS-NEM) */
  ds_date_t CalibrationDate;				   /* (0018,1200)   10  AT DF 3NS-NEM */
  ds_time_t CalibrationTime;				   /* (0018,1201)   12  AT DF 3NS-NEM */
  char ConvolutionKernel[LENGTH_LABEL + 1];		   /* (0018,1210)   12  AT DF 3NS-NEM */
  char ReceivingCoil[LENGTH_LABEL + 1];			   /* (0018,1250)   26  AT FF 2DS-CMS */
  char Gap1251[LENGTH_LABEL + 1];
  patient_position_t PatientPosition;			   /* (0018,5100)    8  AT EV 2NS-CMS */
  char ImagedNucleus[LENGTH_NUCLEUS + 1];		   /* (0018,0085)    8  AT FF 2NS-NEM */
} acr_acquisition_t;



/*******************************************/
/* Relationship Information (Group 0020'H) */
/*******************************************/

typedef struct acr_relationship_tag
{
  long Study;						   /* (0020,0010)    6  AN FF 2NS-NEM */
  long Gap0011;
  long Acquisition;					   /* (0020,0012)    6  AN FF 2DS-NEM */
  long Image;						   /* (0020,0013)    6  AN FF 2DS-NEM */
  long Gap0030[3];
  int Pad1;                                                /* Dummy for byte alignment */
  double Gap0035[6];
  long Location;					   /* (0020,0050)    6  AN FF 3NS-NEM */
  laterality_t Laterality;				   /* (0020,0060)    2  AT EV 2DS-NEM */
  geometry_t ImageGeometryType;				   /* (0020,0070)    8  AT EV 2DS-NEM */
  long AcquisitionsInSeries;				   /* (0020,1001)    6  AN FF 3NS-NEM */
  reference_t Reference;				   /* (0020,1020)    n  AT FF 3NM-NEM */
} acr_relationship_t;



/*************************************************/
/* Image Presentation Information (Group 0028'H) */
/*************************************************/

typedef struct acr_presentation_tag
{
  short ImageDimension;					   /* (0028,0005)    2  BI HX 1DS-NEM */
  short Rows;						   /* (0028,0010)    2  BI HX 1NS-NEM */
  short Columns;					   /* (0028,0011)    2  BI HX 1NS-NEM */
  pixel_size_t PixelSize;				   /* (0028,0030)   30  AN FF 2NM-NEM */
  image_format_t ImageFormat;				   /* (0028,0040)    4  AT EV 1DS-NEM */
  compression_code_t CompressionCode;			   /* (0028,0060)    4  AT EV 1DS-NEM */
  short BitsAllocated;					   /* (0028,0100)    2  BI HX 1DS-NEM */
  short BitsStored;					   /* (0028,0101)    2  BI HX 1DS-NEM */
  short HighBit;					   /* (0028,0102)    2  BI HX 1DS-NEM */
  short PixelRepresentation;				   /* (0028,0103)    2  BI HX 1DS-NEM */
  windows_t WindowCenter;				   /* (0028,1050)   12  AN DF 2NM-MED */
  windows_t WindowWidth;				   /* (0028,1051)   12  AN DF 2NM-MED */
  long RescaleIntercept;				   /* (0028,1052)    6  AN FF 2NS-MED */
  long RescaleSlope;					   /* (0028,1053)    6  AN FF 2NS-MED */
} acr_presentation_t;

#endif




/*==  HELP TEXT  ===========================================================================*/

#ifdef DS_STC_TOOL

"G08.Ide.StudyDate.Year", "Study Date (0008,0020)",
"G08.Ide.AcquisitionDate.Year", "Acquisition Date (0008,0022)",
"G08.Ide.ImageDate.Year", "Image Date (0008,0023)",
"G08.Ide.StudyTime.Hour", "Study Time (0008,0030)",
"G08.Ide.AcquisitionTime.Hour", "Acquisition Time (0008,0032)",
"G08.Ide.ImageTime.Hour", "Image Time (0008,0033)",
"G08.Ide.DataSetSubtype.M", "Data Set Subtype (0008,0041)",
"G08.Ide.Modality", "Modality (0008,0060)",
"G08.Ide.Manufacturer", "Manufacturer (0008,0070)",
"G08.Ide.InstitutionID", "Institution ID (0008,0080)",
"G08.Ide.ReferringPhysician", "Referring Physician (0008,0090)",
"G08.Ide.StationID", "Station ID (0008,1010)",
"G08.Ide.ProcedureDescription_1", "Procedure Description (0008,1030) - first part",
"G08.Ide.ProcedureDescription_2", "Procedure Description (0008,1030) - second part",
"G08.Ide.AdmittingDiagnosis", "Admitting Diagnosis (0008,1080)",
"G08.Ide.ManufacturerModel", "Manufacturer Model (0008,1090)",

"G10.Pat.PatientName", "Patient Name (0010,0010)",
"G10.Pat.PatientId", "Patient Id (0010,0020)",
"G10.Pat.PatientBirthdate.Year", "Patient Birthdate (0010,0030)",
"G10.Pat.PatientSex", "Patient Sex (0010,0040): Female | Male | Others",
"G10.Pat.PatientMaidenName", "Patient Maiden Name (0010,1005)",
"G10.Pat.PatientAge", "Patient Age (0010,1010) in Years | Months | Days",
"G10.Pat.PatientSize", "Patient Size (0010,1020) in meters",
"G10.Pat.PatientWeight", "Patient Weight (0010,1030) in kilograms",

"G18.Acq.Contrast",  "Contrast (0018,0010): APPLIED | NONE",
"G18.Acq.SliceThickness",  "Slice Thickness (0018,0050) in mm",
"G18.Acq.GeneratorVoltage",  "nominal Generator Voltage (0018,0060) in kV",
"G18.Acq.GeneratorVoltageDual",  "second value of Generator Voltage (0018,0060) in kV",
"G18.Acq.RepetitionTime",  "Repetition Time (0018,0080) in msec",
"G18.Acq.EchoTime",  "Echo Time (0018,0081) in msec",
"G18.Acq.InversionTime",  "Inversion Time (0018,0082) in msec",
"G18.Acq.NumberOfAverages",  "nominal Number of Averages (0018,0083)",
"G18.Acq.ImagingFrequency",  "Imaging Frequency (0018,0084) in MHz",
"G18.Acq.ImagedNucleus",  "Imaged Nucleus (0018,0085)",
"G18.Acq.EchoNumber",  "Echo Number (0018,0086)",
"G18.Acq.DataCollectionDiameter",  "Data Collection Diameter (0018,0090) in mm",
"G18.Acq.DeviceSerialNumber",  "Device Serial Number (0018,1000)",
"G18.Acq.SoftwareVersion",  "Software Version (0018,1020)",
"G18.Acq.DistanceSourceToDetector",  "Distance Source to Detector (0018,1110)in mm",
"G18.Acq.DistanceSourceToPatient",  "Distance Source to Patient (0018,1111) in mm",
"G18.Acq.GantryTilt",  "Gantry Tilt (0018,1120) in degrees",
"G18.Acq.TableHeight",  "Table Height (0018,1130) in mm",
"G18.Acq.RotationDirection",  "Rotation Direction (0018,1140): Counter Clock | Clock Wise | NO",
"G18.Acq.ExposureTime",  "nominal Exposure Time (0018,1150) in msec",
"G18.Acq.Exposure",  "nominal Exposure (0018,1152) in mAs",
"G18.Acq.FilterIdLabel",  "Filter Id Label (0018,1160)",
"G18.Acq.GeneratorPower",  "nominal Generator Power (0018,1170) in kW",
"G18.Acq.FocalSpot",  "Focal Spot (0018,1190) in mm",
"G18.Acq.CalibrationDate.Year",  "Calibration Date (0018,1200)",
"G18.Acq.CalibrationTime.Hour",  "Calibration Time (0018,1201)",
"G18.Acq.ConvolutionKernel",  "Convolution Kernel (0018,1210)",
"G18.Acq.ReceivingCoil",  "Receiving Coil (0018,1250)",
"G18.Acq.PatientPosition",  "Patient Position (0018,5100)",
  
"G20.Rel.Study",  "Study (0020,0010)",
"G20.Rel.Acquisition",  "Acquisition (0020,0012)",
"G20.Rel.Image",  "Image (0020,0013)",
"G20.Rel.Location",  "Location (0020,0050) in mm",
"G20.Rel.Laterality",  "Laterality (0020,0060)",
"G20.Rel.ImageGeometryType",  
"Image Geometry Type (0020,0070): PLANAR | UNRAVEL | CURVED",
"G20.Rel.AcquisitionsInSeries",  "Acquisitions in Series (0020,1001)",
"G20.Rel.Reference.One.Image",  "Reference (0020,1020)",

"G28.Pre.ImageDimension",  "Image Dimension (0028,0005)",
"G28.Pre.Rows",  "Rows (0028,0010)",
"G28.Pre.Columns",  "Columns (0028,0011)",
"G28.Pre.PixelSize.Row",  "Pixel Size (0028,0030) in mm",
"G28.Pre.ImageFormat",  "Image Format (0028,0040)",
"G28.Pre.CompressionCode",  "Compression Code (0028,0060)",
"G28.Pre.BitsAllocated",  "Bits Allocated (0028,0100)",
"G28.Pre.BitsStored",  "Bits Stored (0028,0101)",
"G28.Pre.HighBit",  "High Bit (0028,0102)",
"G28.Pre.PixelRepresentation",  "Pixel Representation (0028,0103)",
"G28.Pre.WindowCenter.X",  "Window Center (0028,1050)",
"G28.Pre.WindowWidth.X",  "Window Width (0028,1051)",
"G28.Pre.RescaleIntercept",  "Rescale Intercept (0028,1052)",
"G28.Pre.RescaleSlope",  "Rescale Slope (0028,1053)",

#endif
