/*[-  HEADER FILE  -------------------------------------------------------------------------*/
/*
   Name:        ds_transformation_control.h

   Description: The header file defines the types, constants and macros necessary to
                control a data set transformation.

                                                  NOTE
                The discussion about graphic representation has not finished in the
                moment (1989-DEC). A final declaration is possible after publishing the
                graphic discussion results.


   Author:      THUMSER, Andreas (TH); Siemens AG UBMed CMS/SCE64; phone: 09131 844797
*/
/*]-----------------------------------------------------------------------------------------*/

#ifndef DS_TRANSFORMATION_CONTROL
#define DS_TRANSFORMATION_CONTROL


/*==  "Interpace Pointer List"  ============================================================*/

/*[
   Description: Entity "Interpace Pointer List"

   The most important part to control the transformation is the Interface
   Pointer List. To use the data set library build up function "ds_build_up_
   cms_nema_data_set() to export an Internal Data Set or the data set library
   separation function "ds_separate_common_data_set() to import a NEMA Data
   Set this structure must be filled with data to control the transformation.
   This structure is the link between the different data areas in the host
   memory.

   The following C-code fragment shows an example to fill an Interface Pointer
   List before a NEMA Data Set is build up and separate respectively.
   The example areas are named 'MyHead', 'MyGraphic', 'MyData' for Internal
   Data Set and 'NemaSet' for the NEMA Data Set. The Interface Pointer List
   'MyInterface' is used.
   Please note such areas must be available and filled with valid data before
   a data set transformation can be done. It is the user's responsibility to
   ensure that enough storage is available for the different areas.

   To avoid nested comments in the source file the C++ comment sign "//" is
   used.

   // SEQUENCE: build up CMS defined NEMA Data Set
     ...
     // update interface pointer list
     MyInterface.In.HeadAddress = &MyHead;                    // pointer to head area
     MyInterface.In.HeadLength = sizeof(header_t);            // length of head area
     MyInterface.In.GraphicAddress = MyGraphic_p;             // pointer to graphic area
                                                              // or DS_VOID_NIL
     MyInterface.In.GraphicLength = MyGraphicLength;          // length of graphic area
                                                              // or Integer_UNDEFINED
                                                              // or 0
     MyInterface.In.DataAddress = (void *)MyData_p;           // pointer to data area
     MyInterface.In.DataLength = MyDataLength;                // length of data area 1)

     MyInterface.Ex.HeadAddress = MyNema_p;                   // pointer to NEMA area
     MyInterface.Ex.HeadLength = Integer_UNDEFINED;           // filled during build up
     MyInterface.Ex.GraphicAddress = DS_VOID_NIL;             // filled during build up
     MyInterface.Ex.GraphicLength = Integer_UNDEFINED;        // filled during build up
     MyInterface.Ex.DataAddress = DS_VOID_NIL;                // filled during build up
     MyInterface.Ex.DataLength = Integer_UNDEFINED;           // filled during build up
     MyInterface.Ex.DataGroup = (short) Integer_UNDEFINED;    // filled during build up

     MyInterface.CntStatus.Owner = DS_owner_UNDEFINED;        // initialization
     MyInterface.CntStatus.Modality = <required modality>;    // p r e s e t t i n g !!
     MyInterface.CntStatus.Code = DS_code_UNDEFINED;          // initialization
     MyInterface.CntStatus.StatusFileAddress = MyFileAddress; // pointer to mapped status file
     MyInterface.CntStatus.StopItem.GroupNumber = 0;          // initialization
     MyInterface.CntStatus.StopItem.ElementNumber = 0;        // initialization
     ...
     
     1) The pure data length in bytes is also stored into item Number of Data Bytes
        (0019'H,1060'H).
        

   // SEQUENCE: separate common Data Set
     ...
     // update interface pointer list
     MyInterface.In.HeadAddress = &MyHead;                    // pointer to head area
     MyInterface.In.HeadLength = sizeof(header_t);            // length of head area
     MyInterface.In.GraphicAddress = MyGraphic_p;             // pointer to graphic area
     MyInterface.In.GraphicLength = Integer_UNDEFINED;        // filled during separation
     MyInterface.In.DataAddress = (void *)MyData_p;           // pointer to data area
     MyInterface.In.DataLength = Integer_UNDEFINED;           // filled during separation

     MyInterface.Ex.HeadAddress = MyNema_p;                   // pointer to NEMA area
     MyInterface.Ex.HeadLength = Integer_UNDEFINED;           // filled during separation
     MyInterface.Ex.GraphicAddress = DS_VOID_NIL;             // filled during separation
     MyInterface.Ex.GraphicLength = Integer_UNDEFINED;        // filled during separation
     MyInterface.Ex.DataAddress = DS_VOID_NIL;                // filled during separation
     MyInterface.Ex.DataLength = Integer_UNDEFINED;           // filled during separation
     MyInterface.Ex.DataGroup = (short) Integer_UNDEFINED;    // filled during separation

     MyInterface.CntStatus.Owner = DS_owner_UNDEFINED;        // initialization or presetting
     MyInterface.CntStatus.Modality = Modality_UNDEFINED;     // initialization or presetting
     MyInterface.CntStatus.Code = DS_code_UNDEFINED;          // initialization
     MyInterface.CntStatus.StatusFileAddress = DS_STATUS_FILE_NIL; // not used during separation
     MyInterface.CntStatus.StopItem.GroupNumber = 0;          // initialization
     MyInterface.CntStatus.StopItem.ElementNumber = 0;        // initialization
     ...
*/


/* DECLARATION: declare the interpace pointer list types */

typedef enum transformation_status_tag
{

/*   NOTE: enum transformation_status_tag */
/*   The data set library defined transformation control status is supported by native
     language support (nls). This means it is possible to use this code for a detailed status
     messages after a transformation task. For example the following lines

      // SEQUENCE: transformate data set
      MyStatus = ds_separate_common_data_set(&MyInterface);

      // SEQUENCE: show transformation task status
      printf("\ninfo: separate data set: (%s)", nls_message((int) MyStatus));
      printf("\n                         (%s)", nls_message((int) MyInterface.CntStatus.Code));

      print after transformatin task with error

      info: separate data set: ret: (DS_RET_WITH_ERROR an error occur during transformation task)
                                    (DS_CODE_SET_OUT_OF_ORDER data set out of order)


*/

  DS_code_DATA_SET_OUT_OF_ORDER =
  DS_CODE_SET_OUT_OF_ORDER,				   /* Error - groups or elements not
							      in ascending order. Build up or
							      separation was aborted. */
  DS_code_END_OF_DATA_SET_FOUND =
  DS_CODE_END_OF_SET_FOUND,				   /* Error - End Of Data set was
							      encountered.  Build up or
							      separation was aborted. */
  DS_code_FATAL_ERROR =
  DS_CODE_FATAL_ERROR,					   /* Error - fatal error occured
							      during data set transformation
							      Build up or separation was
							      aborted. */
  DS_code_FIRST_ELEMENT_IN_GROUP_INVALID =
  DS_CODE_FIRST_ELEMENT_INVALID,			   /* Error - first element in group
							      is not element Group Length
							      (x'H,0000'H).  Build up or
							      separation was aborted. */
  DS_code_INVALID_MODALITY =
  DS_CODE_INVALID_MODALITY,				   /* Error - desired modality is not
							      supported.  Build up or
							      separation was aborted. */
  DS_code_INVALID_SOM0_PARAMETERS =
  DS_CODE_INVALID_SOM0_PARAMETERS,			   /* Error - given data set can not
							      transformed.  Build up or
							      separation was aborted. */
  DS_code_INVALID_INTERFACE =
  DS_CODE_INVALID_INTERFACE,				   /* Error - the interface pointer
							      list dosen't contain length
							      information. Separation of
							      internal data set was aborted. */
  DS_code_NO_OWNER =
  DS_CODE_NO_OWNER,					   /* Error - no owner entries found
							      in shadow groups. Build up or
							      separation was aborted.  */
  DS_code_TRANSFORMATION_NOT_POSSIBLE =
  DS_CODE_TRANSFORM_NOT_POSSIBLE,			   /* Error - The transformation is
							      not possible, because required
							      dependencies are not fulfilled.
							      Please see data set library
							      function descriptions
							      "ds_*_check_if_transformation_pos
							      sible()" for more information.  */
  DS_code_UNKNOWN_COMPRESSION_CODE =
  DS_CODE_UNKNOWN_COMPRESSION,				   /* Error - compression code is
							      unknown or not supported.  Build
							      up or separation was aborted. */
  DS_code_UNKNOWN_DATA_SET =
  DS_CODE_UNKNOWN_DATA_SET,				   /* Error - data set format is
							      unknown.  Build up or separation
							      was aborted. */
  DS_code_UNKNOWN_HEADER =
  DS_CODE_UNKNOWN_HEADER,				   /* Error - header format is
							      unknown.  Build up or separation
							      was aborted. */
  DS_code_UNKNOWN_OVERLAY =
  DS_CODE_UNKNOWN_OVERLAY,				   /* Warning - a given overlay number
							      is invalid. The build up task
							      omitted the build up of this
							      overlay group. */
  DS_code_MISSING_TYP_1_VALUE =
  DS_CODE_MISSING_TYP_1,				   /* Warning - not all type 1 NEMA
							      items could be build up, that
							      means an invalid NEMA Data Set
							      was build up or separated. */
  DS_code_MULTIPLE_ERROR =
  DS_CODE_MULTIPLE_ERROR,				   /* Warning - several errors
							      detected. This means more than
							      one type 1 item values are
							      missing or a type 1 value is
							      missing and one or more other
							      item values are not filled. An
							      invalid NEMA Data Set was build
							      up or separated. */
  DS_code_MULTIPLE_FATAL_ERROR =
  DS_CODE_MULTIPLE_FATAL_ERROR,				   /* Error - several fatal errors
							      detected. This means more than
							      one error occures to abort the
							      transformation. No external data
							      set is filled. */
  DS_code_NOT_ALL_FILLED =
  DS_CODE_NOT_ALL_FILLED,				   /* Warning - not all Internal Data
							      Set parameters could be filled
							      or not all required NEMA items
							      could be built up. This
							      parameters are set 'undefined'.
							      That means an valid NEMA Data
							      Set was build up, but it is
							      possible that this data set
							      contains to less information for
							      special CMS internal use. */

  DS_code_NORMAL = DS_CODE_NORMAL,			   /* successful transformation */
  DS_code_UNDEFINED = DS_CODE_UNDEFINED			   /* initialization value */

} transformation_status_t;



typedef enum data_set_owner_tag
{
  DS_owner_CMS_NEMA = 1,				   /* The data set is NEMA defined
							      with CMS expansions. The format
							      is defined in [DS File Format]
							      and [DS Item Format] */
  DS_owner_NEMA = 2,					   /* The data set is NEMA defined.
							      Possible available expansions
							      are unknown and not analyzed.
							      The format is defined in [NEMA
							      Image], [NEMA Image88] and [NEMA
							      Image89]. */
  DS_owner_NUMARIS2_FRAMED = 3,				   /* The data set is Siemens defined
							      with a NEMA frame. The format is
							      defined in [MR Header]. */
  DS_owner_SOMARIS1_FRAMED = 4,				   /* The data set is Siemens defined
							      with a NEMA frame. The format is
							      defined in [CT Header] and [CT
							      Image]. */
  DS_owner_SOMARIS1_BLOCKED = 5,			   /* The data set is Siemens defined
							      as a binary data stream. The
							      format is defined in [CT
							      Header]. */
  DS_owner_SOMARIS0_FRAMED = 6,				   /* The data set is Siemens defined
							      with a NEMA frame. The format is
							      defined in [CT DRH Header] and
							      [CT Image]. */
  DS_owner_SOMARIS0_BLOCKED = 7,			   /* The data set is Siemens defined
							      as a binary data stream. The
							      format is defined in [CT DRH
							      Header]. */
  DS_owner_CMS_BLOCKED = 8,				   /* The data set is CMS defined
							      as a binary data stream. The
							      format is defined in [DS File
							      Format], [DS Item Format] and
							      [DS Reference Manual]. */
  DS_owner_NUMARIS1_FRAMED = 9,				   /* The data set is Siemens defined
							      with a NEMA frame. The format is
							      defined in [MR NUM1 Header]. */
  DS_owner_NUMARIS1_BLOCKED = 10,			   /* The data set is Siemens defined
							      with a binary data stream. The
							      format is defined in [MR NUM1
							      Header]. */
  DS_owner_UNKNOWN = -99,				   /* The data set format is unknown.
							      No separation is possible */

  DS_owner_UNDEFINED = Enum_UNDEFINED			   /* initialization */
} data_set_owner_t;



typedef struct keyword_tag
{
  short GroupNumber;					   /* item group number */
  short ElementNumber;					   /* item element number */
} keyword_t;


typedef struct control_area_ex_tag			   /* - NEMA Data Set - */
{
  char *HeadAddress;					   /* header area address (base
							      address of NEMA Data Set byte
							      stream, is also base address of
							      item key of item Group Length
							      (0008'H, 0000'H)) */
  long HeadLength;					   /* header area length in byte */
  char *GraphicAddress;					   /* graphic area address (base
							      address of item key of item
							      Group Length (6021'H, 0000'H)) */
  long GraphicLength;					   /* graphic area length in byte */
  char *DataAddress;					   /* data area address (base address
							      of item key of item Group Length
							      (<DataGroup>, 0000'H)) */
  long DataLength;					   /* data length in byte */
  short DataGroup;					   /* NEMA data group */
} control_area_ex_t;


typedef struct control_area_in_tag			   /* - Internal Data Set - */
{
  header_t *HeadAddress;				   /* header area address */
  long HeadLength;					   /* header area length in byte */
  char *GraphicAddress;					   /* graphic area address */
  long GraphicLength;					   /* graphic area length in byte */
  void *DataAddress;					   /* data area address */
  long DataLength;					   /* data length in byte */
} control_area_in_t;


typedef struct control_status_tag
{
  data_set_owner_t Owner;				   /* Format of source data set. This
							      parameter must be initialized by
							      separation with the known owner
							      or with 'DS_owner_UNDEFINED'. If
							      'DS_owner_UNDEFINED' is used,
							      the data set library will
							      determine the data set owner. If
							      a preset owner not equal the
							      real owner of data set the
							      separation is aborted. During
							      build up the owner is always set
							      by data set library functions to
							      'DS_owner_CMS_NEMA'. */
  modality_t Modality;					   /* Modality of source data set.
							      This parameter must be
							      initialized by separation with
							      the required modality or with
							      'Modality_UNDEFINED'. If
							      'Modality_UNDEFINED' is used,
							      the data set library will
							      determine the data set modality
							      during separation. If a preset
							      owner not equal the real owner
							      of data set the separation is
							      aborted. During build up the
							      modality must be always preset
							      with a supported modality value
							      ('Modality_CT' or
							      'Modality_MR'). */
  DS_STATUS_FILE_TYPE *StatusFileAddress;		   /* Common Status File address. This
							      parameter can be set if a status
							      file mapped outside of the data
							      set library. Otherwise this
							      parameter must be set to nil
							      (DS_STATUS_FILE_NIL). If acssess
							      to a status file possible the
							      Unique Identifier
							      (0009'H,1015'H) is build up with
							      common status file information
							      otherwise the non PACS
							      presettings are used. */
  transformation_status_t Code;				   /* Detailed status code. This
							      parameter is set by data set
							      library. */
  keyword_t StopItem;					   /* Last handled item. If an fatal
							      error occur during
							      transformation this parameter
							      contains the error produced item
							      group and element number. This
							      two parameters must be
							      initlaized with 0. */
} control_status_t;



typedef struct interface_pointer_list_tag
{
  control_area_ex_t Ex;					   /* Internal Data Set information */
  control_area_in_t In;					   /* NEMA Data Set information */
  control_status_t CntStatus;				   /* common status information */
} interface_pointer_list_t;
/*]*/


/*==  "NEMA Parser Pointer List"  ==========================================================*/

/*[
   Description: Entity "NEMA Parser Pointer List"

   The NEMA data set parser works as an anticipationed scanner. In this manner it is possible
   to analyse the current work item in his NEMA data set context. Detection of end of
   data set, group change, subgroup change, generation of subgroup owner list, position
   control during parsing etc. can be done with the same data set library features.

   The NEMA parser pointer list contains several pointers (addresses) to special locations
   in a NEMA data set, like begin of data set, end of data set, end of current worked
   group, end of group containing next item. In addition the item keys of current
   worked item and the following next item are stored.

   But the most importent information for a application programmer is the pointer to
   the item value of the current worked item. This pointer is used to split, convert or
   fill the item for internal use.

   For clearness, the 'current worked item' is the item currently analysed, transfered,
   tested etc. by the running seperation.
   The 'next item' is the item directly following the worked item in the Nema data set.
   Durring item separation it is used to approach to the given (searched) item. But this
   subject is transperent for a application which works in a level higher as level of
   function ds_seperate_given_item().


   The following table shows a comparision of work and next/this item parameter:

   parameter                parser base              parser current location
   =========================================================================
   item group number        WorkKey.GroupNumber      NextKey.GroupNumber
   item element number      WorkKey.ElementNumber    NextKey.ElementNumber
   item value length        WorkKey.ValueLength      NextKey.ValueLength
   begin of item key        -                        BeginOfNextKey
   begin of item value      BeginOfWorkValue         -
   end of group
   containing item          EndOfWorkGroup           EndOfThisGroup

   The work item is the current item used by the separation application but the base of
   scanning by parser.
   The next/this item is the item current analyzed by the parser. The parameter 'EndOf-
   ThisGroup' is not named 'EndOfNextGroup' to avoid confusion during reading source
   code. The term 'next' discribe not the right situation for this pointer.
*/


/* DECLARATION: declare the NEMA parser pointer list types*/

typedef struct item_key_tag
{
  short GroupNumber;					   /* item group number */
  short ElementNumber;					   /* item element number */
  long ValueLength;					   /* item value length */
} item_key_t;


typedef struct nema_parser_pointer_list_tag
{
  item_key_t WorkKey;					   /* current worked item */
  item_key_t NextKey;					   /* next item in data set */
  char *BeginOfWorkValue;				   /* points to first byte of item
							      value */
  char *BeginOfNextKey;					   /* points to first byte of next
							      item key */
  char *EndOfWorkGroup;					   /* points to first byte of group
							      following the current worked
							      group. The last address in this
							      group is EndOfWorkGroup - 1. */
  char *EndOfThisGroup;					   /* points to first byte of group
							      following group containing next
							      item. The last address in this
							      group is EndOfThisGroup - 1. */
  char *BeginOfDataSet;					   /* points to first byte of data
							      set. */
  char *EndOfDataSet;					   /* points to first byte behint the
							      NEMA data set. The last address
							      in the data set is EndOfDataSet
							      - 1. The address EndOfDataSet is
							      not part of the data set. The
							      pointer is only used as stop
							      mark during data set parsing.
							      Don't dereference this pointer
							      to avoid segmentation fault. */
} nema_parser_pointer_list_t;
/*]*/


/*==  "Shadow Subgroup Owner List"  ========================================================*/

/*[
   Description: Entity "Shadow Subgroup Owner List"

   In each odd-numbered group expect the command information shadow group 1, a fixed block
   of data element numbers is reserved to identify the "ownership" of sets of data elements.
   The elements 0010'H to 007F'H are reserved to identify the manufacturer sets of elements.
   Each identifier element (0010'H to 007F'H) is a type 1 free-formatted (FF) single (S)
   ASCII string (AT) and contains e.g. the OWNER_STRING_SPI (SPI Recognition Code) for blocks
   of data elements reserved by SPI or the OWNER_STRING_MED for blocks of data elements
   reserved by Siemens UBMed.

   The shodow subgroup owner list is used to fit subgroup location order in the NEMA data
   set byte stream to the CMS program separation order without a parser reset at change
   from one shadow subgroup to the next. This is necessary for higher performance.

   The shadow subgroup owner list is a two-dimensional array. In the first column are stored
   the subgroup numbers found by function ds_get_subgroup_owner_code() in ascent order as
   item element base number.
   In the second column are stored the CMS defined subgroup owner numbers.
   The last element of the shadow subgroup owner list must be marked with the end of list
   element END_OF_OWNER_LIST.
   The length of the list must be equal the number of listed shadow owner strings.

   The following graphic shows the shodow subgroup owner list format and an example for a
   CMS defined Identifiying Information (0009'H) shodow group:

   OWNER_NUMBERS      SUBGROUP_NUMBERS        'ListCounter'
   +------------------+------------------+
   |ItemElementBase   |OwnerNumber       |             0
   +------------------+------------------+
   |ItemElementBase   |OwnerNumber       |             1
   +------------------+------------------+
   |ItemElementBase   |OwnerNumber       |             2
   +------------------+------------------+
   |END_OF_OWNER_LIST |END_OF_OWNER_LIST |             3
   +------------------+------------------+
   .....                                              ..
   +------------------+------------------+
   |END_OF_OWNER_LIST |END_OF_OWNER_LIST |     MAX_OWNER
   +------------------+------------------+

   OWNER_NUMBERS      SUBGROUP_NUMBERS       'ListCounter'
   +------------------+------------------+
   | 0x1000           |OWNER_NUMBER_SPI_1|             0
   +------------------+------------------+
   | 0x1100           |OWNER_NUMBER_MED  |             1
   +------------------+------------------+
   | 0x1200           |OWNER_NUMBER_CMS  |             2
   +------------------+------------------+
   | 0x1300           |OWNER_NUMBER_LAB  |             3
   +------------------+------------------+
   |END_OF_OWNER_LIST |END_OF_OWNER_LIST |             4
   +------------------+------------------+
   .....                                              ..
   +------------------+------------------+
   |END_OF_OWNER_LIST |END_OF_OWNER_LIST |     MAX_OWNER
   +------------------+------------------+


   With this list it is possible to handle the shadow group flexibility defined in
   [SPI Image] document 4. This means it is possible to parse a NEMA data set
   without reset the parser at change from one shadow subgroup to the next.
*/
/*]*/


/* DECLARATION: declare the supported shodow subgroup owner */

#define OWNER_STRING_ACQU                                              "SIEMENS CM VA0  ACQU"
#define OWNER_STRING_CMS                                               "SIEMENS CM VA0  CMS "
#define OWNER_STRING_CT                                                "SIEMENS CT VA0  GEN "
#define OWNER_STRING_CT_COAD                                           "SIEMENS CT VA0  COAD"
#define OWNER_STRING_CT_IMA                                            "SIEMENS CT VA0  IMA "
#define OWNER_STRING_CT_RAW                                            "SIEMENS CT VA0  RAW "
#define OWNER_STRING_LAB                                               "SIEMENS CM VA0  LAB "
#define OWNER_STRING_MED                                                       "SIEMENS MED "
#define OWNER_STRING_MR                                                "SIEMENS MR VA0  GEN "
#define OWNER_STRING_MR_COAD                                           "SIEMENS MR VA0  COAD"
#define OWNER_STRING_MR_IMA                                            "SIEMENS MR VA0  IMA "
#define OWNER_STRING_MR_RAW                                            "SIEMENS MR VA0  RAW "
#define OWNER_STRING_MR_SPEC                                           "SIEMENS MR VA0  SPEC"
#define OWNER_STRING_SPI_1                                                   "SPI RELEASE 1 "
#define OWNER_STRING_SPI_2                                                             "SPI "
#define OWNER_STRING_SPI_3                                                   "SPI Release 1 "
#define OWNER_STRING_SPI_4                                               "SPI-VERSION 01.00 "
#define OWNER_STRING_SPI_5                                                   "SPI_RELEASE_1_"


/* NOTE: shodow subgroup owner list */
/* Don't change the order of the following define-blocks! */

#define PRE                                                                                 0
#define OWNER_NUMBER_ACQU                                                                   0
#define OWNER_NUMBER_CMS                                                                    1
#define OWNER_NUMBER_CT                                                                     2
#define OWNER_NUMBER_CT_COAD                                                                3
#define OWNER_NUMBER_CT_IMA                                                                 4
#define OWNER_NUMBER_CT_RAW                                                                 5
#define OWNER_NUMBER_LAB                                                                    6
#define OWNER_NUMBER_MED                                                                    7
#define OWNER_NUMBER_MR                                                                     8
#define OWNER_NUMBER_MR_COAD                                                                9
#define OWNER_NUMBER_MR_IMA                                                                10
#define OWNER_NUMBER_MR_RAW                                                                11
#define OWNER_NUMBER_MR_SPEC                                                               12
#define OWNER_NUMBER_SPI_1                                                                 13
#define OWNER_NUMBER_SPI_2                                                                 14
#define OWNER_NUMBER_SPI_3                                                                 15
#define OWNER_NUMBER_SPI_4                                                                 16
#define OWNER_NUMBER_SPI_5                                                                 17


#define MAX_OWNER                                                                      17 + 2
#define OWNER_STRING_SPI                                                   OWNER_STRING_SPI_1

#define END_OF_OWNER_LIST                                                                (-1)
#define OWNER_NUMBERS                                                                       0
#define SUBGROUP_NUMBERS                                                                    1

typedef long shadow_owner_list_t[MAX_OWNER][2];


#endif
