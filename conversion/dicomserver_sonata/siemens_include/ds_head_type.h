/*[-  HEADER FILE  -------------------------------------------------------------------------*/
/*
   Name:        ds_head_type.h

   Description: The header file defines the data set basic groups as unions for
                internal use (internal header).
                Unions are used to overlay a buffer and the group structure for more flexi-
                bility and compatibility. To expand a group the new item must only be
                defined at the structure end in header file "ds_head_acr_groups_types.h" or
                "ds_head_shadow_groups_types.h".


   Author:      THUMSER, Andreas (TH); Siemens AG UBMed CMS/SCE64; phone: 09131 844797
*/
/*]-----------------------------------------------------------------------------------------*/

#ifndef DS_HEAD_TYPE
#define DS_HEAD_TYPE

typedef union group_0008_tag
{
  char Ide_buf[LENGTH_GROUP_0008];			   /* fill-in */
  acr_identifying_t Ide;				   /* work area */
} group_0008_t;



typedef union group_0009_tag
{
  char Ide_buf[LENGTH_GROUP_0009];			   /* fill-in */
  shadow_identifying_t Ide;				   /* work area */
} group_0009_t;



typedef union group_0010_tag
{
  char Pat_buf[LENGTH_GROUP_0010];			   /* fill-in */
  acr_patient_t Pat;					   /* work area */
} group_0010_t;



typedef union group_0011_tag
{
  char Pat_buf[LENGTH_GROUP_0011];			   /* fill-in */
  shadow_patient_t Pat;					   /* work area */
} group_0011_t;



typedef union group_0013_tag
{
  char PatMod_buf[LENGTH_GROUP_0013];			   /* fill-in */
  shadow_patient_modify_t PatMod;			   /* work area */
} group_0013_t;



typedef union group_0018_tag
{
  char Acq_buf[LENGTH_GROUP_0018];			   /* fill-in */
  acr_acquisition_t Acq;				   /* work area */
} group_0018_t;



typedef union group_0019_part1_tag
{
  char Acq1_buf[LENGTH_GROUP_0019_PART1];		   /* fill-in */
  shadow_acquisition_cms_t CM;				   /* work area */
} group_0019_part1_t;

typedef union group_0019_part2_tag
{
  char Acq2_buf[LENGTH_GROUP_0019_PART2];		   /* fill-in */
  shadow_acquisition_ct_t Ct;				   /* CT common work area */
  shadow_acquisition_mr_t Mr;				   /* MR common work area */
} group_0019_part2_t;

typedef union group_0019_part3_tag
{
  char Acq3_buf[LENGTH_GROUP_0019_PART3];		   /* fill-in */
  shadow_acquisition_ct_conf_t Ct;			   /* CT configuration work area */
  shadow_acquisition_mr_conf_t Mr;			   /* MR configuration work area */
} group_0019_part3_t;

typedef union group_0019_part4_tag
{
  char Acq4_buf[LENGTH_GROUP_0019_PART4];		   /* fill-in */
  shadow_acquisition_acq_t CM;				   /* work area */
} group_0019_part4_t;

typedef struct group_0019_tag
{
  group_0019_part1_t Acq1;				   /* CMS subgroup */
  group_0019_part2_t Acq2;				   /* common subgroup */
  group_0019_part3_t Acq3;				   /* config. and adjust subgroup */
  group_0019_part4_t Acq4;				   /* acquisition subgroup */
} group_0019_t;



typedef union group_0020_tag
{
  char Rel_buf[LENGTH_GROUP_0020];			   /* fill-in */
  acr_relationship_t Rel;				   /* work area */
} group_0020_t;



typedef union group_0021_part1_tag
{
  char Rel1_buf[LENGTH_GROUP_0021_PART1];		   /* fill-in */
  shadow_relationship_med_cms_t CM;			   /* work area */
} group_0021_part1_t;

typedef union group_0021_part2_tag
{
  char Rel2_buf[LENGTH_GROUP_0021_PART2];		   /* fill-in */
  shadow_relationship_ct_t Ct;				   /* CT common work area */
  shadow_relationship_mr_t Mr;				   /* MR common work area */
} group_0021_part2_t;

typedef union group_0021_part3_tag
{
  char Rel2_buf[LENGTH_GROUP_0021_PART3];		   /* fill-in */
  shadow_relationship_ct_spe_t Ct;			   /* CT special work area */
  shadow_relationship_mr_spe_t Mr;			   /* MR special work area */
} group_0021_part3_t;

typedef struct group_0021_tag
{
  group_0021_part1_t Rel1;				   /* CMS subgroup */
  group_0021_part2_t Rel2;				   /* common subgroup */
  group_0021_part3_t Rel3;				   /* special subgroup */
} group_0021_t;



typedef union group_0028_tag
{
  char Pre_buf[LENGTH_GROUP_0028];			   /* fill-in */
  acr_presentation_t Pre;				   /* work area */
} group_0028_t;



typedef union group_0029_tag
{
  char Pre_buf[LENGTH_GROUP_0029];			   /* fill-in */
  shadow_presentation_t Pre;				   /* work area */
} group_0029_t;



typedef union group_0051_tag
{
  char Txt_buf[LENGTH_GROUP_0051];			   /* fill-in */
  image_text_t Txt;					   /* work area */
} group_0051_t;



typedef union fill_to_border_tag
{
  char Fill_buf[LENGTH_TO_FILL_K_BORDER];		   /* fill-in */
} fill_to_border_t;




/*******************/
/* internal header */
/*******************/

typedef struct header_tag
{
  group_0008_t G08;					   /* Identifying Information */
  group_0009_t G09;
  group_0010_t G10;					   /* Patient Information */
  group_0011_t G11;
  group_0013_t G13;                                        /* Patient Modify Information */
  group_0018_t G18;					   /* Acquisition Information */
  group_0019_t G19;
  group_0020_t G20;					   /* Relationship Information */
  group_0021_t G21;
  group_0028_t G28;					   /* Image Presentation Information */
  group_0029_t G29;
  group_0051_t G51;					   /* Image Text */
} header_t;

#endif


/*==========================================================================================*/

#ifdef DS_STC_TOOL

char *_STC_HELP[] =
{
#include <ds_head_acr_groups_types.h>
#include <ds_head_shadow_groups_types.h>
  0
};

#endif
