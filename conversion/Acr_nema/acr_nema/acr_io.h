/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_nema_io.h
@DESCRIPTION: Header file for acr_nema_io code.
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : $Log: acr_io.h,v $
@MODIFIED   : Revision 1.5  1994-04-07 10:05:10  neelin
@MODIFIED   : Added status ACR_ABNORMAL_END_OF_INPUT and changed some ACR_PROTOCOL_ERRORs
@MODIFIED   : to that or ACR_OTHER_ERROR.
@MODIFIED   : Added #ifdef lint to DEFINE_ELEMENT.
@MODIFIED   :
 * Revision 1.4  94/01/06  13:31:30  neelin
 * Changed acr_need_invert to a public function.
 * 
 * Revision 1.3  93/11/25  10:37:26  neelin
 * Added byte-ordering test.
 * 
 * Revision 1.2  93/11/24  11:26:22  neelin
 * Changed short to unsigned short.
 * 
 * Revision 1.1  93/11/19  12:50:15  neelin
 * Initial revision
 * 
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

/* Length of short and long for ACR-NEMA protocol */
#define ACR_SIZEOF_SHORT 2
#define ACR_SIZEOF_LONG  4

/* Status for io */
typedef enum {
   ACR_OK, ACR_END_OF_INPUT, ACR_PROTOCOL_ERROR, ACR_OTHER_ERROR, 
   ACR_ABNORMAL_END_OF_INPUT
} Acr_Status;

/* Functions */
public int acr_set_vax_byte_ordering(int has_vax_byte_ordering);
public int acr_need_invert(void);
public void acr_get_short(long nvals, void *input_value, 
                          unsigned short *mach_value);
public void acr_get_long(long nvals, void *input_value, long *mach_value);
public void acr_put_short(long nvals, unsigned short *mach_value, 
                          void *output_value);
public void acr_put_long(long nvals, long *mach_value, void *output_value);
public Acr_Status acr_test_byte_ordering(Acr_File *afp);
public Acr_Status acr_read_one_element(Acr_File *afp,
                                       int *group_id, int *element_id,
                                       long *data_length, char **data_pointer);
public Acr_Status acr_write_one_element(Acr_File *afp,
                                        int group_id, int element_id,
                                        long data_length, char *data_pointer);
