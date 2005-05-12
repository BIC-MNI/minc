/* ----------------------------- MNI Header -----------------------------------
@NAME       : pms_element_defs.h
@DESCRIPTION: Element definitions for Philips Medical Systems (no, really)
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : March 11, 2005
@MODIFIED   : 
@COPYRIGHT  :
              Copyright (C) 2005 Robert D. Vincent, 
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

/* This information was derived from the publically available "DICOM
 * Conformance Statement" for the Philips Medical Systems MR Intera
 * 10.1, 10 April 2003.
 * 
 * However, not all Philips scanners use the fixed element numbers
 * implied here.  The upper eight bits of the element ID's are not
 * entirely deterministic.  It is necessary to root around in the
 * image for the private groups.
 */

#define PMS_PRIVATE_GROUP_ID 0x2001

GLOBAL_ELEMENT(PMS_Chemical_Shift                     , 0x2001, 0x1001, FL);
GLOBAL_ELEMENT(PMS_Chemical_Shift_Number_MR           , 0x2001, 0x1002, IS);
GLOBAL_ELEMENT(PMS_Diffusion_B_Factor                 , 0x2001, 0x1003, FL);
GLOBAL_ELEMENT(PMS_Diffusion_Direction                , 0x2001, 0x1004, CS);
GLOBAL_ELEMENT(PMS_Image_Enhanced                     , 0x2001, 0x1006, CS);
GLOBAL_ELEMENT(PMS_Image_Type_ED_ES                   , 0x2001, 0x1007, CS);
GLOBAL_ELEMENT(PMS_Phase_number                       , 0x2001, 0x1008, IS);
/* xx09 is present but not defined */
GLOBAL_ELEMENT(PMS_Slice_Number_MR, /* Slice index in series */
               0x2001, 0x100a, IS);
GLOBAL_ELEMENT(PMS_Slice_Orientation, /* SAGITTAL, TRANSVERSAL, e.g. */
               0x2001, 0x100b, CS);
/* xx0c, xx0e, xx0f, xx10 are present but not defined */
GLOBAL_ELEMENT(PMS_Diffusion_Echo_Time                , 0x2001, 0x1011, FL);
GLOBAL_ELEMENT(PMS_Dynamic_Series                     , 0x2001, 0x1012, CS);
GLOBAL_ELEMENT(PMS_EPI_Factor                         , 0x2001, 0x1013, SL);
GLOBAL_ELEMENT(PMS_Number_of_Echoes                   , 0x2001, 0x1014, SL);
GLOBAL_ELEMENT(PMS_Number_of_Locations                , 0x2001, 0x1015, SS);
GLOBAL_ELEMENT(PMS_Number_of_PC_Locations             , 0x2001, 0x1016, SS);
GLOBAL_ELEMENT(PMS_Number_of_Phases_MR                , 0x2001, 0x1017, SL);
GLOBAL_ELEMENT(PMS_Number_of_Slices_MR                , 0x2001, 0x1018, SL);
GLOBAL_ELEMENT(PMS_Partial_Matrix_Scanned             , 0x2001, 0x1019, CS);
GLOBAL_ELEMENT(PMS_PC_Velocity                        , 0x2001, 0x101a, FL);
GLOBAL_ELEMENT(PMS_Prepulse_Delay                     , 0x2001, 0x101b, FL);
GLOBAL_ELEMENT(PMS_Prepulse_Type                      , 0x2001, 0x101c, CS);
GLOBAL_ELEMENT(PMS_Reconstruction_Number              , 0x2001, 0x101d, IS);
/* xx1e is present but not defined */
GLOBAL_ELEMENT(PMS_Respiration_Sync                   , 0x2001, 0x101f, CS);
GLOBAL_ELEMENT(PMS_Scanning_Technique_Description_MR  , 0x2001, 0x1020, CS);
GLOBAL_ELEMENT(PMS_SPIR                               , 0x2001, 0x1021, CS);
GLOBAL_ELEMENT(PMS_Water_Fat_Shift                    , 0x2001, 0x1022, FL);
/* xx23 xx24 both present but undefined 
   xx23 appears to be the flip angle, as in DICOM (0018,1314)
 */
GLOBAL_ELEMENT(PMS_Echo_Time_Display                  , 0x2001, 0x1025, SH);
GLOBAL_ELEMENT(PMS_Number_of_Stack_Slices             , 0x2001, 0x102d, SS);
GLOBAL_ELEMENT(PMS_Stack_Radial_Angle                 , 0x2001, 0x1032, FL);
GLOBAL_ELEMENT(PMS_Stack_Radial_Axis                  , 0x2001, 0x1033, CS);
GLOBAL_ELEMENT(PMS_Stack_Slice_Number                 , 0x2001, 0x1035, SS);
GLOBAL_ELEMENT(PMS_Stack_Type                         , 0x2001, 0x1036, CS);
/* xx52, xx5f both present but undefined */
GLOBAL_ELEMENT(PMS_Number_of_Stacks                   , 0x2001, 0x1060, SL);
GLOBAL_ELEMENT(PMS_Examination_Source                 , 0x2001, 0x1063, CS);

/* additional undefined elements include:
   xx60-xx62, xx6e, xx7b, xx81-xx8b, 9000

   xx83 appears to be the imaging frequency, as in DICOM (0018,0084)
   
 */


