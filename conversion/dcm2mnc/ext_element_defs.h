/* ----------------------------- MNI Header -----------------------------------
@NAME       : ext_element_defs.h
@DESCRIPTION: Element definitions for extra elements needed for mosaics, etc.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 2001 (Rick Hoge)
@MODIFIED   : 
@COPYRIGHT  :
              Copyright 1993 Peter Neelin, McConnell Brain Imaging Centre, 
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

/* Element id's for EXT */
GLOBAL_ELEMENT(EXT_Mosaic_rows                        , 0x0023, 0x0001, LO);
GLOBAL_ELEMENT(EXT_Mosaic_columns                     , 0x0023, 0x0002, LO);
GLOBAL_ELEMENT(EXT_Slices_in_file                     , 0x0023, 0x0003, LO);
GLOBAL_ELEMENT(EXT_Sub_image_rows                     , 0x0023, 0x0004, US);
GLOBAL_ELEMENT(EXT_Sub_image_columns                  , 0x0023, 0x0005, US);
GLOBAL_ELEMENT(EXT_MrProt_dump                        , 0x0023, 0x0006, LO);
GLOBAL_ELEMENT(EXT_Diffusion_b_value                  , 0x0023, 0x0007, LO);


