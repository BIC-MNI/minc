/*[-  HEADER FILE  -------------------------------------------------------------------------*/
/*
   Name:        ds_transformation.h

   Description: The header file defines the common types, constants and macros for the
                data set library.


   Author:      THUMSER, Andreas (TH); Siemens AG UBMed CMS/SCE64; phone: 09131 844797
*/
/*]-----------------------------------------------------------------------------------------*/

#ifndef DS_TRANSFORMATION
#define DS_TRANSFORMATION



/*==  VERSION CONTROL  ====================================================================*/
/* PRECOMPILER: define version control contants */
/*   NOTE: versions */
/* 
     The following list shows the used version codes in chronological order:
   
     Date         | Doc     DS      IH      ACR     SPI    notes
     -------------+----------------------------------------------------------------
     1989-JUL-03  | V0.2    -       -       1.0     01.00  
     1990-MAR-10  | V1.1    -       VA1     2.0     01.00  
     1990-MAR-12  | V1.1    V1.2    VA1     2.0     01.00  
     1990-APR-27  | V1.3    V1.3    VA3     2.0     01.00  internal version
     1990-AUG-01  | V1.4    V1.4    VA4     2.0     01.00
     1990-OCT-01  | V1.5    V1.5    VA5     2.0     01.00
     1990-OCT-11  | V1.5    V1.51   VA5     2.0     01.00  M 15
     1990-OCT-15  | V1.5    V1.52   VA5     2.0     01.00  M 13, M19
     1990-NOV-16  | V1.5    V1.53   VA53    2.0     01.00  parts of M16
     1990-NOV-23  | V1.5    V1.54   VA53    2.0     01.00  M 28
     1990-DEC-07  | V1.5    V1.55   VA55    2.0     01.00  M31, parts of M16, M30
     1991-JAN-31  | V2.0    V2.0    VB      2.0     01.00  M*, som, num2, ...
     1991-FEB-19  | V2.0    V2.1    VB      2.0     01.00  M 40
     1991-FEB-28  | V2.0    V2.2    VB2     2.0     01.00  M 38, M 39, M 40, M 45
     1991-MAR-14  | V2.0    V2.3    VB2     2.0     01.00  M 41, M 48
     1991-MAR-28  | V2.0    V2.4    VB2     2.0     01.00  M*, som, num2, ...
     1991-APR-17  | V2.0    V2.5    VB3     2.0     01.00  M*
     1991-MAY-23  | V2.0    V2.6    VB4     2.0     01.00  parts of M49
     1991-JUL-31  | V2.0    V2.7    VB5     2.0     01.00  M*, db_set, num1, ...
     1991-AUG-08  | V2.0    V2.8    VB5     2.0     01.00  M 65
     1991-SEP-12  | V2.0    V2.9    VB5     2.0     01.00  Image Text: M80, 73, 66, 64, 60, 59
     1991-OCT-10  | V2.0    V2.10   VB5     2.0     01.00  M 86
     1991-NOV     | V3.0    V3.0    VB6     2.0     01.00  M*, som0


     labels:
     -------
     Doc  Documentation Version ("Phone Book" Version)
     DS   Data Set Library (Software) Version
     IH   Internal Header Version
     ACR  NEMA Standard Version
     SPI  SPI Standard Version
*/

/*   version strings */
/*     NOTE: Software Version String */
/*   The constant "DS_SOFTWARE_VERSION_STRING" is not longer supported. The software
     version is set in this and future software releases in data set library function 
     "ds_info_get_software_version" directly. In this way several modul compilations
     after changing this common used data set library header file are avoided. */

/*     constant definition */
#define HEADER_VERSION_STRING                                                      "VB6     "
#define RECOGNITION_CODE_STRING                                                "ACR-NEMA 2.0"
#define SPI_VERSION_STRING                                               "SPI VERSION 01.00 "



/*==  CONSTANTS  ==========================================================================*/

/* PRECOMPILER: define common non-numeric constants */

/*   logical constants */
#ifndef TRUE
#define TRUE                                                                                1
#endif

#ifndef FALSE
#define FALSE                                                                               0
#endif


/*   common status file constants */
#define DS_STATUS_FILE_TYPE                                                struct STC_COMMON


/*   common nil pointer constants */
#define DS_CHAR_NIL                                                              ((char *) 0)
#define DS_DOUBLE_NIL                                                          ((double *) 0)
#define DS_INT_NIL                                                                ((int *) 0)
#define DS_LONG_NIL                                                              ((long *) 0)
#define DS_SHORT_NIL                                                            ((short *) 0)
#define DS_STATUS_FILE_NIL                                        ((DS_STATUS_FILE_TYPE *) 0)
#define DS_VOID_NIL                                                              ((void *) 0)


/* PRECOMPILER: define status area element codes */
/*   NOTE: list of status abbreviations */
/*
   PRE == PREparation of item value e.g. adapt, collect information
   KEY == build up item KEY
   BUF == transfere item value into nema data set BUFfer
   SEP == SEParation of item from nema data set buffer
   SET == SET back item value for internal use
   POS == POSition item value into interanl header e.g. split, convert information
   CH1 == Check a first parameter
   CH2 == Check a second  parameter
*/

/*   error handling constants */
#define PRE                                                                                 0
#define KEY                                                                                 1
#define BUF                                                                                 2
#define SEP                                                                                 0
#define SET                                                                                 1
#define POS                                                                                 2
#define CH1                                                                                 0
#define CH2                                                                                 1


/*   sign constants */
#define DELIMITER_DS_SIGN                                                                '\\'
#define INITALIZE_DS_SIGN                                                                 ' '
#define NON_PRINT_ABLE_DS_SIGN                                                            '~'
#define OVERLAY_DS_SIGN                                                                   '/'
#define PADDING_DS_SIGN                                                                   ' '
#define UNDEFINED_TEXT_DS_SIGN                                                            ' '


/*   string constants */
#define IMAGE_GRAPHIC_FORMAT_CMS                                                       "CMS "


/*   end-of constants */
#define EOAL                                                                         (-19223)
#define END_OF_DATA_SET                                                              (-19224)


/*   coordinate system constants */
#define AAA                                                                                 0
#define BBB                                                                                 1
#define CCC                                                                                 2
#define XXX                                                                                 0
#define YYY                                                                                 1
#define ZZZ                                                                                 2
#define SAG                                                                                 0
#define COR                                                                                 1
#define TRA                                                                                 2
#define ROW                                                                                 0
#define COLUMN                                                                              1
#define DOWN                                                                                0
#define RIGHT                                                                               1


/*   common numeric contants */
#define DS_DO_NOT_FILL                                                                  (-1L)
#define DS_MAX_CHAR                                                                 (256 + 1)
#define DS_SAVE_FACTOR                                                                 (4096)
#define DS_UID_SIZE                                                                      (26)
#define DS_WORK_CHAR                                                                 (32 + 1)



/*==  RUNTIME CONTROL  ====================================================================*/

/* PRECOMPILER: define debug constants */

/*   NOTE: debug constants */
/*
     The constant 'DEBUGGING_BY_THUMSER' is used to compile or not compile C-source code
     for debugging. If you change the '#undef'-command in a '#define'-command all parts
     with debug code are compiled. This is usefull to activate calls of function printf()
     etc.

     The macro 'DS_ERROR_CONDITION' controls the call of function ds_initiate_error_handling().
     During normal run time this macro is defined in a matter, that only if a error
     or warning raised the function "ds_initiate_error_handling()" will be actived. If the
     debug switch 'DEBUGGING_BY_THUMSER' is defined for every item a message is printed to
     standard output device. Note that in this case the interface status is not updated
     correctly. Please use the shown messages to anlyse the item transformation
     results.

*/
#undef DEBUGGING_BY_THUMSER

#ifndef DEBUGGING_BY_THUMSER
#define DS_ERROR_CONDITION                                   (!(StatusList[0] & 0x0001) || \
                                                              !(StatusList[1] & 0x0001) || \
                                                                  !(StatusList[2] & 0x0001))
#else
#define DS_ERROR_CONDITION                                                              TRUE
#endif


#define ds_initiate_error_handling(Group, Element, Error, Status, Return)                   \
            ds_init_error_handling(__FILE__, __LINE__, Group, Element, Error, Status, Return)
            
            
/*-----------------------------------------------------------------------------------------*/


/* PRECOMPILER: define interacive constants */

/*   NOTE: interacive constants */
/*   The constant 'DS_INTERACTIVE' is used to compile or not compile C-source code for a
     interactive use of separation functions. If you change the '#undef'-command in a
     '#define'-command all parts with interactive code are compiled. */
#undef DS_INTERACTIVE
/*-----------------------------------------------------------------------------------------*/


/* PRECOMPILER: define Exception Handler */

/*   NOTE: Exception Handler */
/*   Three makros are implemented to raise the exception handler. You can choise one for your
     current work by precompiler statement #define and #undefine, respectively.


     DS_RAISE_NORMAL
     The raise is done with a goto statement. Only to raise an exception handler the goto
     statement is used in data set library software. No other actions will be done.
   
   
     DS_RAISE_TRACE
     The raise is done with a goto statement also, but before flow control goes to the raised
     exception handler a printf() call is done. The following text is printed:
   
     "<file>", line <line number>: raise ==> <status>
     
     The print out of this message can be controlled by file '/tmp/ds_trace_on'. If the file
     exist the print out is done otherwise nothing is done. In this manner the switch can be 
     done during runtime without new compilation or process restart.
     
     If 'DS_RAISE_TRACE' defined an item trace is possible too. For more information about
     this topic see description of function 'ds_finally_interface_status_update()' in modul
     'ds_mixed.c'.
   
   
     DS_RAISE_DBX
     The raise is done with a goto statement also, but before flow control goes to the raised
     exception handler a ds_stop_dbx() is done. This function has no other function than to
     stop point for debugger. Use during a dbx session the command 'stop in ds_stop_dbx'.
     The debugger stops program execution everytime a exception is raised. A check of current
     active parameters is now possible.
*/
#undef   DS_RAISE_NORMAL
#define  DS_RAISE_TRACE
#undef   DS_RAISE_DBX

#ifdef DS_RAISE_NORMAL
#define DS_RAISE_EXCEPTION(x)                                                          goto x
#endif


#ifdef DS_RAISE_TRACE
#define DS_RAISE_EXCEPTION(x)                                                             { \
                                              ds_trace_control(__FILE__, __LINE__, Status); \
                                                                                    goto x; \
                                                                                            }
#endif


#ifdef DS_RAISE_DBX
#define DS_RAISE_EXCEPTION(x)                                                             { \
                                                                             ds_stop_dbx(); \
                                                                                    goto x; \
                                                                                            }
#endif


/* PRECOMPILER: define Return Code Handler */

/*   NOTE: Return Code Handler */
/*   To set a special return code the makro 'DS_SET_RETURN_CODE()' is available. Three forms
     of this makro are implemented to manage the handling during setting of a special return
     code. You can choise one for your current work by precompiler statement #define and
     #undefine, respectively.


     DS_RETURN_NORMAL
     The return code is set with a normal assign statement. No other actions will be done.
   
   
     DS_RETURN_TRACE
     The return code is set with an assign statement also, but in addition a printf() call 
     is done. The following text is printed:
   
     "<file>", line <line number>: return ==> <status>
     
     The print out of this message can be controlled by file '/tmp/ds_return_on'. If the file
     exist the print out is done otherwise nothing is done. In this manner the switch can be
     done during runtime without new compilation or process restart.
   
   
     DS_RETURN_DBX
     The return code is set with an assign statement also, but in additon a call of function
     ds_stop_dbx() is done. This function has no other function than to stop point for debugger.
     Use during a dbx session the command 'stop in ds_stop_dbx'. The debugger stops program 
     execution everytime a special return code is set. A check of current active parameters 
     is now possible.
*/
#undef   DS_RETURN_NORMAL
#define  DS_RETURN_TRACE
#undef   DS_RETURN_DBX

#ifdef DS_RETURN_NORMAL
#define DS_SET_RETURN_CODE(x)                                                ReturnCode = (x);
#endif


#ifdef DS_RETURN_TRACE
#define DS_SET_RETURN_CODE(x)                                                             { \
                                                ds_return_control(__FILE__, __LINE__, (x)); \
                                                                          ReturnCode = (x); \
                                                                                            }
#endif


#ifdef DS_RETURN_DBX
#define DS_SET_RETURN_CODE(x)                                                             { \
                                                                             ds_stop_dbx(); \
                                                                          ReturnCode = (x); \
                                                                                            }
#endif
/*-----------------------------------------------------------------------------------------*/


/* PRECOMPILER: define length of NEMA item values */

/*   NOTE: length of NEAM item values control */
/*
     If you want to use other lengths for NEMA item values as the CMS defined standard
     lengths then change the following precompiler statement '#undef DS_USE_NOT_STAN-
     DARD_LENGTH' to '#define DS_USE_NOT_STANDARD_LENGTH'.

     A reason to do this is do minimize the NEAM Data Set Length during data set build up.
     If you want a minimal NEAM Data Set length define the following xxx_LENGTH
     identifier with a replacement of 2. Then the Data Set Library functions get and
     use the minimal necessary length of each item.

     If you use a replacement less than two then a fatal error occur.
*/

#undef DS_USE_NOT_STANDARD_LENGTH
#ifndef DS_USE_NOT_STANDARD_LENGTH

#define AGE_LENGTH                                                                 LENGTH_AGE
#define DATE_LENGTH                                                                       10L
#define DIAGNOSIS_LENGTH                                                     LENGTH_DIAGNOSIS
#define DIRECTION_LENGTH                                                     LENGTH_DIRECTION
#define COMMENT_LENGTH                                                         LENGTH_COMMENT
#define FILE_NAME_LENGTH                                                     LENGTH_FILE_NAME
#define FILTER_ID_LENGTH                                                     LENGTH_FILTER_ID
#define HEADER_VERSION_LENGTH                                           LENGTH_HEADER_VERSION
#define INTEGER_NUMBER_LENGTH                                                              6L
#define LABEL_LENGTH                                                             LENGTH_LABEL
#define LITTLE_IDENT_LENGTH                                                                4L
#define LONG_IDENT_LENGTH                                                                 12L
#define NUCLEUS_LENGTH                                                         LENGTH_NUCLEUS
#define MANUFACTURER_LENGTH                                               LENGTH_MANUFACTURER
#define MIDDLE_IDENT_LENGTH                                                                8L
#define ORIENTATION_LENGTH                                                 LENGTH_ORIENTATION
#define PATIENT_ID_LENGTH                                                   LENGTH_PATIENT_ID
#define REAL_NUMBER_LENGTH                                                                14L
#define SEQUENCE_INFO_LENGTH                                             LENGTH_SEQUENCE_INFO
#define SHORT_IDENT_LENGTH                                                                 2L
#define SOFTWARE_VERSION_LENGTH                                       LENGTH_SOFTWARE_VERSION
#define SPI_VERSION_LENGTH                                                                18L
#define TIME_LENGTH                                                                       12L

#else

#define AGE_LENGTH                                                                         2L
#define DIAGNOSIS_LENGTH                                                                   2L
#define DIRECTION_LENGTH                                                                   2L
#define COMMENT_LENGTH                                                                     2L
#define FILE_NAME_LENGTH                                                                   2L
#define FILTER_ID_LENGTH                                                                   2L
#define HEADER_VERSION_LENGTH                                                              2L
#define INTEGER_NUMBER_LENGTH                                                              2L
#define LABEL_LENGTH                                                                       2L
#define LITTLE_IDENT_LENGTH                                                                2L
#define LONG_IDENT_LENGTH                                                                  2L
#define NUCLEUS_LENGTH                                                                     2L
#define MANUFACTURER_LENGTH                                                                2L
#define MIDDLE_IDENT_LENGTH                                                                2L
#define ORIENTATION_LENGTH                                                                 2L
#define PATIENT_ID_LENGTH                                                                  2L
#define REAL_NUMBER_LENGTH                                                                 2L
#define SEQUENCE_INFO_LENGTH                                                               2L
#define SHORT_IDENT_LENGTH                                                                 2L
#define SOFTWARE_VERSION_LENGTH                                                            2L
#define SPI_VERSION_LENGTH                                                                 2L

#define DATE_LENGTH                                                                       10L
#define TIME_LENGTH                                                                       12L

#endif
/*-----------------------------------------------------------------------------------------*/



/*==  TYPES  ==============================================================================*/

/* DECLARATION: declare types */
/*   - undependent types */

typedef enum data_area_type_tag
{
  Area_Type_BYTE = 1,
  Area_Type_LONG = 2,
  Area_Type_SHORT = 3,
  Area_Type_UNDEFINED = Enum_UNDEFINED
} data_area_type_t;


typedef enum data_set_format_tag
{
  Set_format_BLOCK = 1,
  Set_format_NEMA = 2,
  Set_format_UNDEFINED = Enum_UNDEFINED
} data_set_format_t;


typedef struct image_orientation_tag
{
  double RowX;
  double RowY;
  double RowZ;
  double ColX;
  double ColY;
  double ColZ;
} image_orientation_t;


typedef enum image_class_tag
{
  Image_Class_CORONAL = 10,
  Image_Class_SAGITTAL = 20,
  Image_Class_TRANSVERSAL = 30,
  Image_Class_UNDEFINED = Enum_UNDEFINED
} image_class_t;


typedef enum image_text_type_tag
{
  Image_Text_Type_BLACK = 1,
  Image_Text_Type_MR_RAW = 2,
  Image_Text_Type_NONE = 3,
  Image_Text_Type_NORMAL = 4,
  Image_Text_Type_CT_REBUILD = 5,
  
  Image_Text_Type_UNDEFINED = Enum_UNDEFINED
} image_text_type_t;


typedef enum item_quality_tag
{
  Item_Quality_AFTER_IN_FOLLOWING_GROUP = 10,
  Item_Quality_AFTER_IN_THIS_GROUP = 11,
  Item_Quality_BEFORE = 20,
  Item_Quality_END_OF_DATA_SET = 50,
  Item_Quality_EQUAL = 30,
  Item_Quality_OUT_OF_RANGE = 99,

  Item_Quality_UNDEFINED = Enum_UNDEFINED
} item_quality_t;



typedef char nema_patient_place_t[2][4];

typedef enum overlay_number_tag
{
  Overlay_6000 = 1,
  Overlay_6002 = 2,
  Overlay_6004 = 3,
  Overlay_6006 = 4,

  Overlay_UNDEFINED = Enum_UNDEFINED
} overlay_number_t;


typedef union quart_as_tag				   /* four bytes as different bit
							      patterns */
{
  char AsBytes[4];					   /* as four bytes */
  float AsFloat;					   /* as float */
  long AsLong;						   /* as long */
  short AsShort[2];					   /* as two short */
} quart_as_t;


typedef enum swap_mode_tag
{
  Swap_NO = 0,
  Swap_YES = 1,
  Swap_UNDEFINED = Enum_UNDEFINED
} swap_mode_t;


typedef struct transformation_list_tag
{
  int Index;
  double Factor;
} transformation_list_t[3];


typedef double matrix_3x3_t[3][3];


typedef enum update_mode_tag
{
  Update_ELEMENT = 1,
  Update_FOUND = 2,
  Update_GROUP = 3,
  Update_UNDEFINED = Enum_UNDEFINED
} update_mode_t;


typedef enum value_mode_tag
{
  Value_IS_NORMAL = 1,
  Value_IS_LAST = 2,
  Value_UNDEFINED = Enum_UNDEFINED
} value_mode_t;


typedef enum value_representation_tag
{
  Representation_AN = 10,
  Representation_AT = 11,
  Representation_BI = 20,
  Representation_BR = 21,
  Representation_BY = 22,
  Representation_BX = 23,
  Representation_UNDEFINED = Enum_UNDEFINED
} value_representation_t;


typedef double vector_t[3];


/*   - dependent types */

typedef struct nema_image_place_tag
{
  nema_patient_place_t PatientOrientation;
  vector_t ImagePosition;
  vector_t ImageOrientation[2];
} nema_image_place_t;



/*==  MACROS  =============================================================================*/

/* PRECOMPILER: define macros */

  /* NOTE: define macros */
  /*
  If you want to use a "Mathematics" macro don't forget to insert the precompiler line
  "#include <math.h>".
  */
/*-----------------------------------------------------------------------------------------*/


/*   Bit Manipulation */
#define DS_BIT_CLEAR(Variable, BitNumber)     ((Variable) = ((Variable) & ~(1<<(BitNumber))))
#define DS_BIT_TEST(Variable, BitNumber)               (((Variable) & (1<<(BitNumber))) != 0)
#define DS_BIT_SET(Variable, BitNumber)        ((Variable) = ((Variable) | (1<<(BitNumber))))
#define DS_BIT_TOGGLE(Variable, BitNumber)                                   (for future use)
/*-----------------------------------------------------------------------------------------*/


/*   Mathematics */
#define DS_MAX(a,b)                                                         ((a)>(b)?(a):(b))
#define DS_MIN(a,b)                                                         ((a)>(b)?(b):(a))


#define DS_I_ABS(x)                                                    ((x) < 0 ? -(x) : (x))

#define DS_R_ABS(x)                                                                 fabs((x))
#define DS_R_EQUAL(x, y)                 (fabs((double)(x) - (double)(y)) <= (double)0.00001)
#define DS_R_GREATER(x, y)                                                        ((x) > (y))
#define DS_R_LESS(x, y)                                                           ((x) < (y))

#define DS_V_EQUAL(x, y)                                   ds_vector_check_if_equal((x), (y))

#define DS_G_SIN(x)                                                 (sin(((M_PI/180.0)*(x))))
#define DS_G_COS(x)                                                 (cos(((M_PI/180.0)*(x))))
#define DS_G_TAN(x)                                                 (tan(((M_PI/180.0)*(x))))
#define DS_G_TAN2(x,y)                                         (tan2((M_PI/180.0)*((x),(y))))

#define DS_G_ASIN(x)                                                 ((180.0/M_PI)*asin((x)))
#define DS_G_ACOS(x)                                                 ((180.0/M_PI)*acos((x)))
#define DS_G_ATAN(x)                                                 ((180.0/M_PI)*atan((x)))
#define DS_G_ATAN2(x,y)                                         ((180.0/M_PI)*atan2((x),(y)))

#define DS_R_SIN(x)                                                               (sin((x))))
#define DS_R_COS(x)                                                               (cos((x))))
#define DS_R_TAN(x)                                                               (tan((x))))
#define DS_R_TAN2(x,y)                                                       (tan2((x),(y))))

#define DS_R_ASIN(x)                                                              (asin((x)))
#define DS_R_ACOS(x)                                                              (acos((x)))
#define DS_R_ATAN(x)                                                              (atan((x)))
#define DS_R_ATAN2(x,y)                                                      (atan2((x),(y)))

/*-----------------------------------------------------------------------------------------*/


/*   Transformation Matrix Handling */
#define DS_SET_TRANSFORMATION_LIST(a_fac, a_ind, b_fac, b_ind, c_fac, c_ind)                \
                                                                                          { \
                                                  TransformationList[AAA].Index  = (a_ind); \
                                                  TransformationList[AAA].Factor = (a_fac); \
                                                  TransformationList[BBB].Index  = (b_ind); \
                                                  TransformationList[BBB].Factor = (b_fac); \
                                                  TransformationList[CCC].Index  = (c_ind); \
                                                  TransformationList[CCC].Factor = (c_fac); \
                                                                                            }
/*-----------------------------------------------------------------------------------------*/


/*   Image Class / History Label */
#define DS_SET_HISTORY_LABEL(ClassLabel, No1Label, No2Label)                                \
                       strncpy(Header->G51.Txt.SliceOrientationNo1, (ClassLabel), (int) 3); \
		                                     if (!(DS_R_EQUAL(SliceAngleNo1, 0.0))) \
		                                                                          { \
		                              Header->G51.Txt.SliceOrientationNo1[3] = '>'; \
                   strncpy(&(Header->G51.Txt.SliceOrientationNo1[4]), (No1Label), (int) 3); \
		                                                                          } \
		                                     if (!(DS_R_EQUAL(SliceAngleNo2, 0.0))) \
		                                                                          { \
		                              Header->G51.Txt.SliceOrientationNo2[3] = '>'; \
		   strncpy(&(Header->G51.Txt.SliceOrientationNo2[4]), (No2Label), (int) 3); \
	                                                                                    }
/*-----------------------------------------------------------------------------------------*/


/*   String Initialization */
#define DS_SET_STRING_DEFAULT(String, DefaultSign, StringLength)                            \
	                             memset((String), (DefaultSign), (int) (StringLength)); \
	                                                        String[(StringLength)] = '\0'

#define DS_SET_STRING_UNDEFINED(String, FillSign, StringLength)                             \
	                                memset((String), (FillSign), (int) (StringLength)); \
	                                                     String[(StringLength)] = '\0'; \
	                                                         String[0] = String_UNDEFINED

#define DS_SET_STRING_VALUE(String, FillString, StringLength)                               \
                                       bcopy((FillString), (String), (int) (StringLength)); \
                                                                String[(StringLength)] = '\0'

#define DS_SET_STRING_CHANGEABLE(String)                                                    \
                                  ds_string_delete_leading_characters(String, String, ' '); \
                                     ds_string_delete_tailing_characters(String, String, ' ')
/*-----------------------------------------------------------------------------------------*/



/*==  FUNCTIONS  ==========================================================================*/

/* NOTE: define system library functions */
/*
  The following system library functions are defined here to avoid an error message during
  lint task.
*/

/* DECLARATION: define special parameter */
/*char *sprintf();*/



/*========================================================================================*/


#endif
