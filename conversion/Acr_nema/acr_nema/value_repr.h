/* ----------------------------- MNI Header -----------------------------------
@NAME       : value_repr.h
@DESCRIPTION: Header file for acr-nema VR code
@METHOD     : 
@GLOBALS    : 
@CREATED    : January 31, 1997 (Peter Neelin)
@MODIFIED   : $Log: value_repr.h,v $
@MODIFIED   : Revision 6.0  1997-09-12 13:23:59  neelin
@MODIFIED   : Release of minc version 0.6
@MODIFIED   :
 * Revision 5.0  1997/08/21  13:25:00  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:01:23  neelin
 * Release of minc version 0.4
 *
 * Revision 1.2  1997/04/21  20:21:09  neelin
 * Updated the library to handle dicom messages.
 *
 * Revision 1.1  1997/02/11  16:23:43  neelin
 * Initial revision
 *
@COPYRIGHT  :
              Copyright 1997 Peter Neelin, McConnell Brain Imaging Centre, 
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

typedef enum {
   ACR_VR_UNKNOWN,
   ACR_VR_AE,
   ACR_VR_AS,
   ACR_VR_AT,
   ACR_VR_CS,
   ACR_VR_DA,
   ACR_VR_DS,
   ACR_VR_DT,
   ACR_VR_FL,
   ACR_VR_FD,
   ACR_VR_IS,
   ACR_VR_LO,
   ACR_VR_LT,
   ACR_VR_OB,
   ACR_VR_OW,
   ACR_VR_PN,
   ACR_VR_SH,
   ACR_VR_SL,
   ACR_VR_SQ,
   ACR_VR_SS,
   ACR_VR_ST,
   ACR_VR_TM,
   ACR_VR_UI,
   ACR_VR_UL,
   ACR_VR_US,
   ACR_VR_NUM_TYPES
} Acr_VR_Type;

/* Function prototypes */
public char *acr_get_vr_name(Acr_VR_Type vr_code);
public int acr_test_vr_name(char *vr_name);
public Acr_VR_Type acr_lookup_vr_name(char *vr_name);
public double acr_get_numeric_vr(Acr_VR_Type vr_code, 
                                 Acr_byte_order byte_order,
                                 char *data, long data_length);
public char *acr_get_string_vr(Acr_VR_Type vr_code, 
                               Acr_byte_order byte_order,
                               char *data, long data_length);
