/*------------------------------------------------------------------------------*
 *										*
 *	File Name :	STC_Common_Status.h					*
 *										*
 *	Author :	M. Rohbrecht	CMS / SME 24	Tel. ... 9131 84 4790	*
 *	Author :	C. Schaefer	CMS / SME 24	Tel. ... 9131 84 4790	*
 *	Author :	J.H. Siebert	CMS / SME 24	Tel. ... 9131 84 4790	*
 *										*
 *	Description :	Based on the definition of the structure of a status	*
 *			file, the common (CMS) part can be found in the include	*
 *			module `STC_Status.h`.					*
 *										*
 *			The contents of this include file is in consistency 	*
 *			with the definition of the document 'Contents of	*
 *			Status-, Configuration,- and Common Files' for the part	*
 *			of 'Status File'. 					*
 *										*
 *			The informations for 'General Installation' and 'Fixed	*
 *			Device Conditions' are found inhere.			*
 *										*
 *------------------------------------------------------------------------------*
 *										*
 *	Last Change :	July 9th 1990		Creation			*
 *										*
 *------------------------------------------------------------------------------*
 *										*
 *	Last Change :	August 6th 1990		Integration of exposure data	*
 *						for company S.E.P.P.		*
 *										*
 *------------------------------------------------------------------------------*
 *										*
 *	Last Change :	October 22nd 1990	Integration of device data and 	*
 *						additional camera data.		*
 *										*
 *------------------------------------------------------------------------------*
 *										*
 *	Last Change :	December 11th 1990	Integration of modality 	*
 *						subtype (enum + variable).	*
 *										*
 *------------------------------------------------------------------------------*
 *										*
 *	Last Change :	January 17th 1991	Integration of		 	*
 *						- STC_Config_Hostname		*
 *						- STC_Config_Consoles		*
 *						- STC_Config_Host_Dbhost	*
 *						- STC_Config_Host_Dbserver	*
 *						- STC_Config_Host_Camera	*
 *										*
 *------------------------------------------------------------------------------*
 *										*
 *	Last Change :	January 18th 1991	Integration of		 	*
 *						- STC_Camera_1_Code		*
 *						- STC_Camera_2_Code		*
 *------------------------------------------------------------------------------*
 *										*
 *	Last Change :	February 15th 1991	by J. H. Siebert		*
 *						Integration of		 	*
 *						- Country Code for Japan	*
 *						- __STC_Enum_UNDEFINED		*
 *										*
 *------------------------------------------------------------------------------*
 *										*
 *	Last Change :	February 22nd 1991	by C. Schaefer			*
 *						Integration of		 	*
 *						- STC_Maxicam_Auto_Filmtransport*
 *										*
 *------------------------------------------------------------------------------*
 *										*
 *	Last Change :	March 12th 1991	by C. Schaefer				*
 *						Integration of		 	*
 *						- STC_Maxicam_Density		*
 *						- STC_Maxicam_LUT		*
 *						- STC_Num_Of_Cameras		*
 *										*
 *------------------------------------------------------------------------------*
 *	Last Change :					(J.H. Siebert)		*
 *	May 16th 1991	STC_PACS_NodeList introduced				*
 *	June 25th 1991	STC_PACS_NodeFlag_t is now an int (before: enum),	*
 *			__STC_PACS_NodeActiveSUN introduced			*
 *	July, 9th 1991	Integration of STC_Customer, STC_City, STC_Destrict	*
 *	Aug, 14th 1991	Integration of STC_dsvWinAutoCorr			*
 *										*
 *------------------------------------------------------------------------------*
 *	STC_Device_Net_Installed seems not to be used				*
 *------------------------------------------------------------------------------*
 *										*
 *	Last Change :	Sep 9th 1991	by C. Schaefer				*
 *						Integration of STC_VCR_Norm 	*
 *------------------------------------------------------------------------------*
 *	Last Change :					(J.H. Siebert)		*
 *	November 4th 1991	New:						*
 *				__STC_PACS_Country_Other, __STC_PACS_Country_UK	*
 *				STC_DefaultHospitalInstall,			*
 *				   STC_CustomerHospitalInstall			*
 *				STC_MIN_WINAUT_CORR, STC_MAX_WINAUT_CORR	*
 *				STC_InstallCountry_t STC_Installation_Country	*
 *										*
 *      30Apr92    CHARM 205149 J.H.Siebert                                     *
 *				Country and PACS code with additional countries *
 *------------------------------------------------------------------------------*/

#ifndef BASIC_STC_DEFINE
#define	BASIC_STC_DEFINE	TRUE

#ifndef	__STC_Enum_UNDEFINED
#define __STC_Enum_UNDEFINED	(-19222)
#endif

#define	CHAR	char
#define	UL	unsigned long
#define	US	unsigned short
#define	SS	short
#define	DO	double
#define	SL	long int


/*------------------------------------------------------------------------------*/

#define	__STC_BASIC_DATA_LEN		8192

/*------------------------------------------------------------------------------*
 *	For those variables of the status file where bitwise definitions are	*
 *	needed, no enums are defined, but #define statements.			*
 *------------------------------------------------------------------------------*/

#define	__STC_Type_None				0
#define	__STC_Type_MINICAM			1
#define	__STC_Type_MAXICAM			2

#define	__STC_LISA_Present			1
#define	__STC_LISA_Not_Present			0

#define	__STC_Plotter_Present			1
#define	__STC_Plotter_Not_Present		0

#define	__STC_Input_Mouse			1
#define	__STC_Input_Trackball			2

#define	__STC_HostnameLen			8	/*** Ex 16Bytes ?***/


/******************\
* Defines for PACS *
*                  *
\******************/

#define	__STC_PACS_Country_None		    "000"
#define	__STC_PACS_Country_Other	    "999"
#define	__STC_PACS_Country_USA		    "001"
/* START CHANGE CHARM 205149 J.H.Siebert 30Apr92 */
#define	__STC_PACS_Country_France	    "033"
#define	__STC_PACS_Country_Italy	    "039"
/* END CHANGE CHARM 205149 J.H.Siebert 30Apr92 */
#define	__STC_PACS_Country_Germany	    "049"
#define	__STC_PACS_Country_Japan	    "081"
#define	__STC_PACS_Country_UK		    __STC_PACS_Country_Other

/*** NEW May, 16th 1991 J.H. Siebert***/

#define __STC_PACS_NUMOF_NODES		16

#define __STC_PACS_NODE_LOG_LEN		__STC_HostnameLen
#define __STC_PACS_NODE_PHYS_LEN	__STC_PACS_NODE_LOG_LEN

/* defines of flags */
typedef int	STC_PACS_NodeFlag_t;
#define    __STC_PACS_NodeNULL		0
#define    __STC_PACS_NodeActive 	1
#define    __STC_PACS_NodeSUN		8
#define    __STC_PACS_NodeFlag_UNDEFINED  __STC_Enum_UNDEFINED

typedef enum
{
    __STC_PACS_NodeOwn = 0,
    __STC_PACS_NodeRmt1, __STC_PACS_NodeRmt2, __STC_PACS_NodeRmt3,
    __STC_PACS_NodeRmt4, __STC_PACS_NodeRmt5, __STC_PACS_NodeRmt6,
    __STC_PACS_NodeRmt7,  __STC_PACS_NodeRmt8, __STC_PACS_NodeRmt9,
    __STC_PACS_NodeRmt10, __STC_PACS_NodeRmt11, __STC_PACS_NodeRmt12,
    __STC_PACS_NodeRmt13, __STC_PACS_NodeRmt14, __STC_PACS_NodeRmt15,
    __STC_PACS_Node_UNDEFINED = __STC_Enum_UNDEFINED
} STC_PACS_NodeNum_t;


typedef struct
{
    STC_PACS_NodeFlag_t		flag;	/* PACSNET configurationbits */
    char log[__STC_PACS_NODE_LOG_LEN+1];		/* logical name */
/***    char log_ext[65-__STC_PACS_NODE_LOG_LEN];***/	/* logical name extension*/

    char phys[__STC_PACS_NODE_PHYS_LEN+1];		/* physical name */
/***    char phys_ext[65-__STC_PACS_NODE_PHYS_LEN];***/	/* physical name extension*/

} STC_PACS_node_t;

typedef struct
{
    STC_PACS_node_t node[__STC_PACS_NUMOF_NODES];
} STC_PACS_nodelist_t;


/*****************************************************************/


#define	__STC_Net_Present			1
#define	__STC_Net_Not_Present			0

/*------------------------------------------------------------------------------*
 *										*
 *	In case of a defined and limited value range enum definitions are used.	*
 *										*
 *------------------------------------------------------------------------------*/

	typedef	enum
	{
		__STC_Modality_CT = 1,
		__STC_Modality_MR,
		__STC_Modality_UNDEFINED = __STC_Enum_UNDEFINED
		
	}	STC_Modality_t;


	typedef	enum
	{
		__STC_Modality_Sub_M2 = 0,
		__STC_Modality_Sub_P8 = 1,
		__STC_Modality_Sub_UNDEFINED = __STC_Enum_UNDEFINED
		
	}	STC_Modality_Sub_t;


/*------------------------------------------------------------------------------*/

	typedef	enum
	{
		__STC_Weight_Kg = 1,
		__STC_Weight_lbs,
		__STC_Weight_UNDEFINED = __STC_Enum_UNDEFINED
		
	}	STC_Weight_t;

/*------------------------------------------------------------------------------*/

	typedef	enum
	{
		__STC_Main_Console = 1,
		__STC_Satellite_Console,
		__STC_Console_UNDEFINED = __STC_Enum_UNDEFINED
		
	}	STC_Console_t;
	
/*------------------------------------------------------------------------------*/

	typedef	enum
	{
		__STC_Language_English = 1,
		__STC_Language_German,
		__STC_Language_UNDEFINED = __STC_Enum_UNDEFINED
		
	}	STC_Language_t;

/*------------------------------------------------------------------------------*/

	typedef	enum
	{
		__STC_Device_Type_Unknown = 1,
		__STC_Device_Type_Magnetic_Disk,
		__STC_Device_Type_Magnetic_Tape,
		__STC_Device_Type_Optical_Disk,
		__STC_Device_Type_UNDEFINED = __STC_Enum_UNDEFINED
		
	}	STC_Device_t;

/*------------------------------------------------------------------------------*/

	typedef	enum
	{
		__STC_Net_Installed = 1,
		__STC_Net_Not_Installed,
		__STC_Net_UNDEFINED = __STC_Enum_UNDEFINED
		
	}	STC_Net_t;

/*------------------------------------------------------------------------------*/

	typedef	enum
	{
		__STC_Minicam_Matrix_512 = 1,
		__STC_Minicam_Matrix_484,
		__STC_Minicam_Matrix_UNDEFINED = __STC_Enum_UNDEFINED
		
	}	STC_Minicam_Matrix_t;
	
/*------------------------------------------------------------------------------*/

	typedef	enum
	{
		__STC_VCR_NORM_PAL = 1,
		__STC_VCR_NORM_NTSC,
		__STC_VCR_NORM_UNDEFINED = __STC_Enum_UNDEFINED
		
	}	STC_VCR_Norm_t;
	
/*------------------------------------------------------------------------------*/

	typedef	enum
	{
		__STC_List_Lokalizer_F = 1,
		__STC_List_Lokalizer_A,
		__STC_List_Lokalizer_N,
		__STC_List_Lokalizer_UNDEFINED = __STC_Enum_UNDEFINED
		
	}	STC_List_Lokalizer_Default_t;
	
	typedef	enum
	{
		__STC_NULL_VERSION = 0,
		__STC_91E5_VERSION,
		__STC_91E7_VERSION,
		__STC_91E11_VERSION,
		__STC_Version_UNDEFINED = __STC_Enum_UNDEFINED
		
	}	STC_Version_t;
	
#define	__STC_INFO_VERSION_LEN	32

	typedef	struct
	{
	    int flag;
	    char version[__STC_INFO_VERSION_LEN+1];
	} STC_Info_t;
	
	
	typedef struct
	{
	    char space[__STC_BASIC_DATA_LEN -
	    	(sizeof(STC_Version_t) + sizeof(STC_Info_t))];
	    STC_Info_t	info;
	    STC_Version_t Version;
	}  structInfo_t;
	
#define	__STC_CustomerLen	26
#define	__STC_CityLen		26
#define	__STC_DestrictLen	26

#define	__STC_InstallationLen	26
#define	__STC_CustomerHospitalNUM	3

/*********************************\
*                                 *
* MR requirement (ui_window, ...) *
*                                 *
\*********************************/

#define STC_MIN_WINAUT_CORR	0
#define STC_MAX_WINAUT_CORR	5

	typedef struct
	{
	    int	center;
	    int	width;
	    int	part;
	}  STC_dsvWinAutoCorr_t;
	

/***************************\
*                           *
* MR requirement (M. Gress) *
*                           *
\***************************/

/* START CHANGE CHARM 205149 J.H.Siebert 30Apr92 */

typedef enum
{
    __STC_InstallCountry_USA = 1,
    __STC_InstallCountry_GERMANY,
    __STC_InstallCountry_UK,
    __STC_InstallCountry_JAPAN,
    __STC_InstallCountry_FRANCE,
    __STC_InstallCountry_ITALY,
    __STC_InstallCountry_OTHERS = 99,
    __STC_InstallCountry_UNDEFINED  = __STC_Enum_UNDEFINED
} STC_InstallCountry_t;

/* END CHANGE CHARM 205149 J.H.Siebert 30Apr92 */


	
/*------------------------------------------------------------------------------*
 *										*
 *	Union and structure definition of the 8 KByte of common status file	*
 *	data									*
 *										*
 *------------------------------------------------------------------------------*/

union	__STC_BASIC_DATA
{
    char		space	[__STC_BASIC_DATA_LEN];	/* 8 blocks for map	*/
    
    structInfo_t	structInfo;
    
    struct blablabla			/* Name needed only for STC tool for 	*/
					/* naming within the debugger - Dummy	*/
    {

/*------------------------------------------------------------------------------*
 *										*
 *	Manufacturer and installation						*
 *										*
 *------------------------------------------------------------------------------*/
 
 
	CHAR	STC_Manufacturer [8+1];		/* Name of Manufacturer, e.g.	*/
						/* SIEMENS.			*/
						
	CHAR	STC_Manufacturer_Model [26+1];	/* Manufacturer's model, e.g.	*/
						/* SOMATOM, MAGNETOM		*/
						
	CHAR	STC_Installation_Name [__STC_InstallationLen+1];
						/* Name of hospital, instal.	*/

	CHAR	STC_Software_Version [8+1];	/* Software version for image 	*/
						/* text				*/

	UL	STC_Int_Software_Version;	/* Internal software version	*/


/*------------------------------------------------------------------------------*
 *	PACS data								*
 *------------------------------------------------------------------------------*/
						
	CHAR	STC_PACS_Country_Code [3+1];	/* Country code for PACS	*/
						/* installation			*/
	
	CHAR	STC_PACS_Identification [3+1];	/* Manufacturer PACS identifi-	*/
						/* cation			*/
						
	CHAR	STC_PACS_Modality [4+1];	/* Modality PACS identification */
	
#ifndef VERSION_NO_PACS
	STC_PACS_nodelist_t	STC_PACS_NodeList; /* List of nodes in net */
#endif

	UL	STC_Serial_Number;		/* Serial number of installation*/
	
	UL	STC_System_Code;		/* System Code - to be defined	*/

	STC_Modality_t	STC_Modality;		/* System Modality		*/
	
	STC_Language_t	STC_System_Language;	/* User language		*/
	
	STC_Weight_t	STC_Weight_System;	/* Weight system		*/

	UL	STC_Net_Frequency;		/* Net frequency in Hz		*/

	STC_Console_t	STC_Console_Type;	/* Either main or satellite	*/
	

/*------------------------------------------------------------------------------*
 *										*
 *	Camera data								*
 *										*
 *------------------------------------------------------------------------------*/
 
 
 	UL	STC_Camera_Display_Birthday;	/* Text superimposing of birth-	*/
 						/* day				*/
						/* Yes = 1			*/
						/* No = 0			*/

	UL	STC_Camera_1_Type;		/* Type of instal. camera no. 1	*/
						/* <no camera> = 0		*/
						/* MINICAM = 1			*/
						/* MAXICAM = 2			*/
						/* . . . 			*/
	
	UL	STC_Camera_2_Type;		/* Type of instal. camera no. 2	*/
						/* <no camera> = 0		*/
						/* MINICAM = 1			*/
						/* MAXICAM = 2			*/
						/* . . . 			*/
						
	UL	STC_Camera_1_Code;		/* Code for type of instal.	*/
						/* camera no. 1			*/
						/* <no camera> = 0		*/
						/* DIGICAM = 1			*/
						/* . . . 			*/
	
	UL	STC_Camera_2_Code;		/* Code for type of instal.	*/
						/* camera no. 2			*/
						/* <no camera> = 0		*/
						/* DIGICAM = 1			*/
						/* . . . 			*/
	
	CHAR	STC_Camera_1_Name [40+1];	/* Name of camera no. 1		*/
	
	CHAR	STC_Camera_2_Name [40+1];	/* Name of camera no. 2		*/
	
	UL	STC_Camera_1_Copy_Range;	/* Copy range for exposure	*/
	
	UL	STC_Camera_1_Low_Level_Timer;	/* Low level timer		*/
	
	UL	STC_Camera_1_High_Level_Timer;	/* High level timer		*/
	
	UL	STC_Camera_2_Copy_Range;	/* Copy range for exposure	*/
	
	UL	STC_Camera_2_Low_Level_Timer;	/* Low level timer		*/
	
	UL	STC_Camera_2_High_Level_Timer;	/* High level timer		*/
	
	
/*------------------------------------------------------------------------------*
 *										*
 *	Special entries for MINICAM only					*
 *										*
 *	A maximum of 100 different film formats is possible. The actual valid	*
 *	number of formats is inquired within the installation. Only those ele-	*
 *	ments are supplied during instalation.					*
 *										*
 *------------------------------------------------------------------------------*/
	
	
	UL	STC_Minicam_Num_of_Formats;	/* Numver of valid formats	*/
	
	
	UL	STC_Minicam_Filmsize [100];
	
	UL	STC_Minicam_Format [100];
	
	UL	STC_Minicam_Format_Code [100];
	
	US	STC_Minicam_Row [100];
	
	US	STC_Minicam_Col [100];
	
	STC_Minicam_Matrix_t	STC_Minicam_Matrixsize;
	
	
/*------------------------------------------------------------------------------*
 *										*
 *	Lokalizer components							*
 *										*
 *------------------------------------------------------------------------------*/


	STC_List_Lokalizer_Default_t	STC_List_Lokalizer_Default;
	
	
/*------------------------------------------------------------------------------*
 *										*
 *	SMIV hardware components						*
 *										*
 *------------------------------------------------------------------------------*/
 
 
	UL	STC_LISA_Present;		/* Is component LISA installed	*/
						/* Yes = 1			*/
						/* No = 0			*/
						/* . . . 			*/

	UL	STC_Plotter_Present;		/* Is a plotter installed	*/
						/* Yes = 1			*/
						/* No = 0			*/
						/* . . . 			*/
						

	UL	STC_SIAM_Memsize;		/* Number of MBytes for SIAM	*/
	
	UL	STC_Num_of_Mem_Board;		/* Number of boards for additi-	*/
						/* onal imager memory		*/

	UL	STC_Add_Mem_Size;		/* Number of MByte per board	*/
						/* for additional imager memory	*/


/*------------------------------------------------------------------------------*
 *										*
 *	Graphical input medium							*
 *										*
 *------------------------------------------------------------------------------*/
 
 
	UL	STC_Graphic_Input_Type;		/* Kind of graphical input :	*/
						/* Mouse = 1			*/
						/* Trackball = 2		*/
						/* . . . 			*/

	UL	STC_Graphic_Input_Character [3]; /* Factors for hysteresis	*/
						/* curve oF sensitivity of	*/
						/* graphical input		*/


/*------------------------------------------------------------------------------*
 *										*
 *	Patient storage device data						*
 *										*
 *------------------------------------------------------------------------------*/


	STC_Device_t	STC_Device_Type [10];	/* Kind of device :		*/
						/* Magnetic Disk		*/
						/* Magnetic Tape		*/
						/* Optical Disk			*/
						/* . . .			*/
	
	CHAR	STC_Device_Manufacturer_Name [10] [19+1];
						/* Name of Device manufacturer	*/
	
	UL	STC_Device_Net_Installed;	/* Is a Net installed :		*/
						/* Yes = 1			*/
						/* No = 0			*/
						/* . . . 			*/


/*------------------------------------------------------------------------------*
 *										*
 *	Installation status encoding						*
 *										*
 *------------------------------------------------------------------------------*/
 
 
	UL	STC_Installation_Status;	/* Encoded status of already	*/
						/* done	installation steps (e.g.*/
						/* first installation, partial 	*/
						/* installation, etc.)		*/


/*------------------------------------------------------------------------------*
 *										*
 *	Modality sub-type. This field contains informations about		*
 *	variants of the modality described in STC_Modality (e.g.		*
 *	STC_Modality == MR and STC_Modality_Sub == STC_Modality_Sub_P8). 	*
 *	This field can be used by both, CT and MR modalities.			*
 *										*
 *										*
 *------------------------------------------------------------------------------*/
 
	STC_Modality_Sub_t	STC_Modality_Sub;	/* System Modality
							   Sub-type */


/*------------------------------------------------------------------------------*
 *										*
 *	Host configuration data.						*
 *										*
 *										*
 *------------------------------------------------------------------------------*/
 
	CHAR	STC_Config_Hostname      [__STC_HostnameLen+1];	/* Name of own host	   */

	CHAR	STC_Config_Consoles   [8][__STC_HostnameLen+1];	/* Name of other hosts	   */

	CHAR	STC_Config_Host_Dbhost   [__STC_HostnameLen+1];	/* Name of database host   */

	CHAR	STC_Config_Host_Dbserver [__STC_HostnameLen+1];	/* Name of database server */

	CHAR	STC_Config_Host_Camera   [__STC_HostnameLen+1];	/* Name of camera host	   */


/*------------------------------------------------------------------------------*
 *	Special entries for MAXICAM only					*
 *------------------------------------------------------------------------------*/
	
	UL	STC_Maxicam_Auto_Filmtransport;	/*			   */
	UL	STC_Maxicam_Density;		/*			   */
	UL	STC_Maxicam_LUT;		/*			   */

	UL	STC_Num_Of_Cameras;		/*			   */

/*------------------------------------------------------------------------------*
 *	CT requirement				 				*
 *------------------------------------------------------------------------------*/

	CHAR	STC_Customer	[__STC_CustomerLen+1];	/* Name of customer	*/
	CHAR	STC_City	[__STC_CityLen+1];	/* Name of city		*/
	CHAR	STC_Destrict	[__STC_DestrictLen+1];	/* Name of destrict	*/
	
	
/*------------------------------------------------------------------------------*
 *	MR requirement				 				*
 *------------------------------------------------------------------------------*/

	STC_dsvWinAutoCorr_t	STC_dsvWinAutoCorr;

/*------------------------------------------------------------------------------*
 *	MR requirement				 				*
 *------------------------------------------------------------------------------*/

	STC_VCR_Norm_t	STC_VCR_Norm;	/* Videonorm of Camera	*/
	
/*------------------------------------------------------------------------------*
 *	New requirement				 				*
 *------------------------------------------------------------------------------*/

	STC_InstallCountry_t	STC_Installation_Country;
						

	CHAR	STC_DefaultHospitalInstall[__STC_InstallationLen+1];
						/* Name of default hospital	*/
	CHAR	STC_CustomerHospitalInstall [__STC_CustomerHospitalNUM] [__STC_InstallationLen+1];
						/* Name of customer hospital, 
						via ui_text installed	*/


    }	s;
};

/*------------------------------------------------------------------------------*/

struct	STC_COMMON
{
	union	__STC_BASIC_DATA	Basic;
};

/*------------------------------------------------------------------------------*/

#endif

/*------------------------------------------------------------------------------*/

