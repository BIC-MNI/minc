/*[-  HEADER FILE  -------------------------------------------------------------------------*/
/*
   Name:        ds_include_files.h

   Description: The header file include all necessary header files for data set library
                functions. The "#ifndef XXX" - "#endif" construct in the different header
                files avoid a multiple type, parameter, constant or macro definition.
                Don't change the order because the header files are implemented as a
                hierachy and so not indepent of calling order.


   Author:      THUMSER, Andreas (TH); Siemens AG UBMed CMS/SCE64; phone: 09131 844797
*/
/*]-----------------------------------------------------------------------------------------*/

#ifndef DS_INCLUDE_FILES
#define DS_INCLUDE_FILES


/* PRECOMILER: sepecial include files */
#include <STC_Common_Status.h>


/* PRECOMILER: data set library include files */
#include <ds_head_constants.h>
#include <ds_transformation.h>

#include <ds_messages.h>

#include <ds_date.h>

#include <ds_head_basic_types.h>
#include <ds_head_acr_groups_types.h>
#include <ds_head_shadow_groups_types.h>
#include <ds_head_image_text_type.h>
#include <ds_head_type.h>

#include <ds_transformation_control.h>

#include <ds_functions.h>

#endif
