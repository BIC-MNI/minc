/*[-  HEADER FILE  -------------------------------------------------------------------------*/
/*
   Name:        ds_head_basic_types.h

   Description: The header file defines the basic types for internal header.
                Each enumerator identifier is build up as a type qualification and the
                enumerator item value defined in document [DS Item Format] paragraphs "item
                "item contents".

                As exampel: Rotation_CC

                Rotation --> identifier for item Rotation Direction (0018,1140
                CC       --> counter clockwise rotation

                No enumerator value is set to zero to avoid problems e.g. if an automatic
                initialication during allocation was done. Zore is not a unique
                identification, because several item values equal zero are defined.
                
                The used bit masks are not defined with C-language type bit array, because
                the internal representation is compiler dependent. To handle not CMS
                created bit masks (e.g. form foreign manufacturer NEMA data set) the
                data set library macros DS_BIT_xxx() are used.


   Author:      THUMSER, Andreas (TH); Siemens AG UBMed CMS/SCE64; phone: 09131 844797
*/
/*]-----------------------------------------------------------------------------------------*/

#ifndef DS_HEAD_BASIC_TYPES
#define DS_HEAD_BASIC_TYPES


typedef enum cardiac_code_tag
{
  Cardiac_Code_COMMON = 1,
  Cardiac_Code_CONFIRM = 2,
  Cardiac_Code_DIASTOLE = 3,
  Cardiac_Code_NONE = 4,
  Cardiac_Code_SYSTOLE = 5,
  Cardiac_Code_UNDEFINED = Enum_UNDEFINED
} cardiac_code_t;

typedef enum 
{
  Calculation_m_A = 20,
  Calculation_m_NONE = 1,
  Calculation_m_PC = 21,
  Calculation_m_PU = 22,
  Calculation_m_UNDEFINED = Enum_UNDEFINED
} calculation_mode_m_t;


typedef enum calculation_mode_s_tag
{
  Calculation_s_BSP = 1,
  Calculation_s_IRS = 2,
  Calculation_s_NONE = 3,
  Calculation_s_SUN = 4,
  Calculation_s_VAX = 5,
  Calculation_s_UNDEFINED = Enum_UNDEFINED
} calculation_mode_s_t;


typedef struct calculation_mode_tag
{
  calculation_mode_m_t M;
  calculation_mode_s_t S;
} calculation_mode_t;


typedef enum compression_code_tag
{
  Compression_DONE = 2,
  Compression_NONE = 1,
  Compression_UNDEFINED = Enum_UNDEFINED
} compression_code_t;


typedef enum contrast_tag
{
  Contrast_NONE = 1,
  Contrast_APPLIED = 2,
  Contrast_UNDEFINED = Enum_UNDEFINED
} contrast_t;


typedef enum data_object_subtype_m_tag
{
  Object_m_CT = 1,
  Object_m_MRS = 2,
  Object_m_MRU = 3,
  Object_m_UNDEFINED = Enum_UNDEFINED
} data_object_subtype_m_t;


typedef enum data_object_subtype_d_tag
{
  Object_d_P = 1,
  Object_d_U = 2,
  Object_d_UNDEFINED = Enum_UNDEFINED
} data_object_subtype_d_t;


typedef enum data_object_subtype_s_tag
{
  Object_s_NONE = 1,
  Object_s_UNDEFINED = Enum_UNDEFINED
} data_object_subtype_s_t;


typedef struct data_object_subtype_tag
{
  data_object_subtype_m_t M;
  data_object_subtype_d_t D;
  data_object_subtype_s_t S;
} data_object_subtype_t;


typedef enum data_set_subtype_m_tag
{
  Set_m_IMAGE = 1,
  Set_m_PLOT = 2,
  Set_m_RAW = 3,
  Set_m_SPECT = 4,
  Set_m_TEXT = 5,
  Set_m_UNDEFINED = Enum_UNDEFINED
} data_set_subtype_m_t;


typedef enum data_set_subtype_s_tag
{
  /* NOTE: data_set_subtype_s_tag */
  /* If this basic data type "data_set_subtype_s_t" is changed please check also the basic
     data type "measurement_mode_s_t" and the sequence "determine image type" in data set
     library function "ds_get_image_text_type()". */

  /* To find a free enum number you can use the following command "sort -t= +1 <Return> <set
     list> <Control D>" */
     
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


typedef struct data_set_subtype_tag
{
  data_set_subtype_m_t M;
  data_set_subtype_s_t S;
} data_set_subtype_t;


typedef struct gradient_delay_time_tag
{
  double X;
  double Y;
  double Z;
} gradient_delay_time_t;


typedef struct field_of_view_tag
{
  double Height;
  double Width;
} field_of_view_t;

typedef struct filter_parameter_tag
{
  double Value1;
  double Value2;
  double Value3;
  double Value4;
} filter_parameter_t;


typedef enum filter_type_tag
{
  Filter_EXTERNAL = 5,
  Filter_FERMI = 1,
  Filter_GAUSS = 2,
  Filter_HANNING = 3,
  Filter_NONE = 4,
  Filter_UNDEFINED = Enum_UNDEFINED
} filter_type_t;


typedef enum filter_type_image_tag
{
  Filter_Image_NO1 = 1,
  Filter_Image_NONE = 4,
  Filter_Image_UNDEFINED = Enum_UNDEFINED
} filter_type_image_t;


typedef enum gate_phase_tag
{
  Gate_EXPIRATION = 1,
  Gate_INSPIRATION = 2,
  Gate_UNDEFINED = Enum_UNDEFINED
} gate_phase_t;

typedef enum geometry_tag
{
  Geometry_CURVED = 3,
  Geometry_PLANAR = 1,
  Geometry_UNRAVEL = 2,
  Geometry_UNDEFINED = Enum_UNDEFINED
} geometry_t;


typedef enum image_format_tag
{
  Format_RECT = 1,
  Format_UNDEFINED = Enum_UNDEFINED
} image_format_t;


typedef struct image_id_tag
{
  char gap[LENGTH_LABEL + 1];
  char PatientName[LENGTH_LABEL + 1];
  char PatientId[LENGTH_LABEL + 1];
  long Image;
} image_id_t;


typedef struct image_location_tag
{
  double Sag;
  double Cor;
  double Tra;
} image_location_t;


typedef struct int_point_tag
{
  long X;
  long Y;
  long Z;
} int_point_t;


typedef enum laterality_tag
{
  Laterality_L = 1,
  Laterality_NO = 2,
  Laterality_R = 3,
  Laterality_UNDEFINED = Enum_UNDEFINED
} laterality_t;


typedef enum measurement_mode_m_tag
{
  Measurement_m_ADJU = 1,
  Measurement_m_EXAM = 2,
  Measurement_m_TEST = 3,
  Measurement_m_UNDEFINED = Enum_UNDEFINED
} measurement_mode_m_t;


typedef enum measurement_mode_s_tag
{
  Measurement_s_NONE = 1,
  Measurement_s_DYNA = 14,
  Measurement_s_HIST = 19,
  Measurement_s_PLOT = 18,
  Measurement_s_QUAL = 17,
  Measurement_s_ROT = 16,
  Measurement_s_SCAN = 10,
  Measurement_s_SINC = 11,
  Measurement_s_SINZ = 12,
  Measurement_s_STAT = 15,
  Measurement_s_R2D = 22,
  Measurement_s_R3D = 23,
  Measurement_s_TOPO = 13,
  Measurement_s_UNDEFINED = Enum_UNDEFINED
} measurement_mode_s_t;					   /* check also data_set_subtype_s_t */


typedef struct measurement_mode_tag
{
  measurement_mode_m_t M;
  measurement_mode_s_t S;
} measurement_mode_t;


typedef enum modality_tag
{
  Modality_CT = 1,
  Modality_MR = 2,
  Modality_UNDEFINED = Enum_UNDEFINED
} modality_t;


typedef enum nucleus_tag
{
  Nucleus_C = 1,
  Nucleus_F = 2,
  Nucleus_H = 3,
  Nucleus_N = 4,
  Nucleus_NA = 5,
  Nucleus_P = 6,
  Nucleus_UNDEFINED = Enum_UNDEFINED
} nucleus_t;


typedef struct object_orientation_tag
{
  double Phi;
  double Theta;
  double Radius;
} object_orientation_t;


typedef struct object_threshold_tag
{
  long LowerBoundary;
  long UpperBoundary;
} object_threshold_t;


typedef enum order_of_slices_tag
{
  Slice_Order_ASCENDING = 1,
  Slice_Order_DECREASING = 2,
  Slice_Order_FREE = 3,
  Slice_Order_INTERLEAVED = 4,
  Slice_Order_NONE = 5,
  Slice_Order_UNDEFINED = Enum_UNDEFINED
} order_of_slices_t;


typedef struct patient_orientation_tag
{
  char Y[LENGTH_ORIENTATION + 1];			   /* up - down */
  char X[LENGTH_ORIENTATION + 1];			   /* left - right */
  char Z[LENGTH_ORIENTATION + 1];			   /* back - front */
} patient_orientation_t;


typedef enum patient_phase_tag
{
  Phase_ADULT = 1,
  Phase_CHILD = 2,
  Phase_UNDEFINED = Enum_UNDEFINED
} patient_phase_t;


typedef enum patient_position_tag
{
  Position_LEFT = 1,
  Position_PRONE = 2,
  Position_RIGHT = 3,
  Position_SUPINE = 4,
  Position_UNDEFINED = Enum_UNDEFINED
} patient_position_t;


typedef enum patient_region_tag
{
  Region_BODY = 1,
  Region_HEAD = 2,
  Region_UNDEFINED = Enum_UNDEFINED
} patient_region_t;


typedef struct pixel_size_tag
{
  double Row;
  double Col;
} pixel_size_t;


typedef enum pixel_quality_mode_tag
{
  Pixel_Quality_ESTIMATED = 1,
  Pixel_Quality_EXACT = 2,
  Pixel_Quality_NONE = 3,
  Pixel_Quality_UNDEFINED = Enum_UNDEFINED
} pixel_quality_mode_t;


typedef struct pixel_quality_code_tag
{
  pixel_quality_mode_t Min;
  pixel_quality_mode_t Mean;
  pixel_quality_mode_t Max;
} pixel_quality_code_t;


typedef struct pixel_quality_value_tag
{
  long Min;
  long Mean;
  long Max;
} pixel_quality_value_t;


typedef struct reference_tag
{
  image_id_t One;
  image_id_t Two;
  image_id_t Three;
} reference_t;


typedef enum rest_direction_tag
{
  Rest_FEET = 1,
  Rest_HEAD = 2,
  Rest_UNDEFINED = Enum_UNDEFINED
} rest_direction_t;


typedef enum rotation_direction_tag
{
  Rotation_NO = 1,
  Rotation_CC = 2,
  Rotation_CW = 3,
  Rotation_UNDEFINED = Enum_UNDEFINED
} rotation_direction_t;


typedef struct sar_sed_tag
{
  double Lim;
  double Cal;
  double Det;
} sar_sed_t;


typedef enum save_code_tag
{
  Save_DONE = 1,
  Save_MARKED = 2,
  Save_NOT = 3,
  Save_UNDEFINED = Enum_UNDEFINED
} save_code_t;


typedef enum sex_tag
{
  Sex_F = 1,
  Sex_M = 2,
  Sex_O = 3,
  Sex_UNDEFINED = Enum_UNDEFINED
} sex_t;


typedef enum storage_mode_tag
{
  Storage_COMPRESS = 1,
  Storage_EXPANDED = 2,
  Storage_MIP_MPR = 5,
  Storage_REDUCED = 3,
  Storage_XDR = 4,
  Storage_UNDEFINED = Enum_UNDEFINED
} storage_mode_t;


typedef enum study_type_tag
{
  Study_Type_CRE = 1,
  Study_Type_MEA = 2,
  Study_Type_MIP = 3,
  Study_Type_MPR = 4,
  Study_Type_RAW = 5,
  Study_Type_UNDEFINED = Enum_UNDEFINED
} study_type_t;


typedef struct target_point_tag
{
  double X;
  double Y;
} target_point_t;


typedef enum view_direction_tag
{
  View_FEET = 1,
  View_HEAD = 2,
  View_AtoP = 3,
  View_LtoR = 4,
  View_PtoA = 5,
  View_RtoL = 6,
  View_UNDEFINED = Enum_UNDEFINED
} view_direction_t;


typedef struct windows_tag
{
  long X;
  long Y;
} windows_t;


typedef enum window_style_tag
{
  Style_DOUBLE = 1,
  Style_HIGH = 2,
  Style_NONE = 3,
  Style_STD_1 = 4,
  Style_STD_2 = 5,
  Style_UNDEFINED = Enum_UNDEFINED
} window_style_t;

#endif

