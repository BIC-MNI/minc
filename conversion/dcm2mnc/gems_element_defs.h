/* ----------------------------- MNI Header -----------------------------------
@NAME       : gems_element_defs.h
@DESCRIPTION: Element definitions for GE Medical Systems
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : March 14, 2005
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

/* This information was derived from the publically available "Advance(TM)
 * 4.05 Conformance Statement for DICOM V3.0" from GE Medical Systems,
 * copyright 2000 by GE Medical Systems.
 */

GLOBAL_ELEMENT(GEMS_Acqu_private_creator_id, 0x0019, 0x0010, SH);
GLOBAL_ELEMENT(GEMS_Frame_acq_start        , 0x0019, 0x106c, DT);
GLOBAL_ELEMENT(GEMS_Frame_acq_duration     , 0x0019, 0x106d, SL);
GLOBAL_ELEMENT(GEMS_Image_slice_number     , 0x0019, 0x10a6, SL);
GLOBAL_ELEMENT(GEMS_Fast_phases            , 0x0019, 0x10f2, SS);

GLOBAL_ELEMENT(GEMS_Sers_private_creator_id, 0x0025, 0x0010, SH);
GLOBAL_ELEMENT(GEMS_Images_in_series       , 0x0025, 0x1007, SL);

