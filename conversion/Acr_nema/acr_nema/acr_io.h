/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_nema_io.h
@DESCRIPTION: Header file for acr_nema_io code.
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
 * $Log: acr_io.h,v $
 * Revision 6.4  2008/08/12 05:00:23  rotor
 *  * large number of changes from Claude (64 bit and updates)
 *
 * Revision 6.3  2005/03/04 00:08:08  bert
 * Cleanup headers, mostly by getting rid of the infernal 'public' and using extern instead
 *
 * Revision 6.2  2000/04/28 15:03:11  neelin
 * Added support for ignoring non-fatal protocol errors (cases where redundant
 * information is inconsistent). In particular, it is possible to ignore
 * differences between the group length element and the true group length.
 *
 * Revision 6.1  1999/10/29 17:51:50  neelin
 * Fixed Log keyword
 *
 * Revision 6.0  1997/09/12 13:23:59  neelin
 * Release of minc version 0.6
 *
 * Revision 5.1  1997/09/08  21:53:31  neelin
 * Added status ACR_CONNECTION_TIMEDOUT.
 *
 * Revision 5.0  1997/08/21  13:25:00  neelin
 * Release of minc version 0.5
 *
 * Revision 4.1  1997/07/10  17:14:38  neelin
 * Added more status codes and function to return status string.
 *
 * Revision 4.0  1997/05/07  20:01:23  neelin
 * Release of minc version 0.4
 *
 * Revision 3.1  1997/04/21  20:21:09  neelin
 * Updated the library to handle dicom messages.
 *
 * Revision 3.0  1995/05/15  19:32:12  neelin
 * Release of minc version 0.3
 *
 * Revision 2.0  1994/09/28  10:36:07  neelin
 * Release of minc version 0.2
 *
 * Revision 1.8  94/09/28  10:35:40  neelin
 * Pre-release
 * 
 * Revision 1.7  94/05/18  08:48:17  neelin
 * Changed some ACR_OTHER_ERROR's to ACR_ABNORMAL_END_OF_OUTPUT.
 * 
 * Revision 1.6  94/04/07  10:23:28  neelin
 * Added ACR_HIGH_LEVEL_ERROR.
 * 
 * Revision 1.5  94/04/07  10:05:10  neelin
 * Added status ACR_ABNORMAL_END_OF_INPUT and changed some ACR_PROTOCOL_ERRORs
 * to that or ACR_OTHER_ERROR.
 * Added #ifdef lint to DEFINE_ELEMENT.
 * 
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
#define ACR_SIZEOF_SHORT  2
#define ACR_SIZEOF_LONG   4
#define ACR_SIZEOF_FLOAT  4
#define ACR_SIZEOF_DOUBLE 8

/* Define the C data types for ACR-NEMA protocol.
   Note: "numeric" functions use the C double datatype directly. */

typedef unsigned short Acr_Short;
typedef unsigned int Acr_Long;  /* 4 bytes in both 32 and 64 bits */
typedef float Acr_Float;
typedef double Acr_Double;
typedef char * Acr_String;

/* Undefined element length value for VRs of SQ, OB, OW */
#define ACR_UNDEFINED_ELEMENT_LENGTH 0xFFFFFFFFU

/* Tags for sequence items */
#define ACR_ITEM_GROUP 0xfffe
#define ACR_ITEM_TAG 0xe000
#define ACR_ITEM_DELIMITER 0xe00d
#define ACR_SEQ_DELIMITER 0xe0dd

/* Flag for data length to indicate variable length elements */
#define ACR_VARIABLE_LENGTH -1

/* Byte-ordering options */
typedef enum {
   ACR_UNKNOWN_ENDIAN = 0,
   ACR_LITTLE_ENDIAN,
   ACR_BIG_ENDIAN
} Acr_byte_order;

/* VR encoding options */
typedef enum {
   ACR_UNKNOWN_VR = 0,
   ACR_EXPLICIT_VR,
   ACR_IMPLICIT_VR
} Acr_VR_encoding_type;

/* Status for io */
typedef enum {
   ACR_OK, 
   ACR_END_OF_INPUT, 
   ACR_PROTOCOL_ERROR, 
   ACR_OTHER_ERROR, 
   ACR_ABNORMAL_END_OF_INPUT, 
   ACR_HIGH_LEVEL_ERROR,
   ACR_ABNORMAL_END_OF_OUTPUT, 
   ACR_REACHED_WATCHPOINT,
   ACR_IO_ERROR,
   ACR_NO_VR_SPECIFIED,
   ACR_PDU_UID_TOO_LONG,
   ACR_CONNECTION_TIMEDOUT
} Acr_Status;

/* Functions */
extern void acr_set_byte_order(Acr_File *afp, 
                               Acr_byte_order byte_order);
extern Acr_byte_order acr_get_byte_order(Acr_File *afp);
extern int acr_get_machine_byte_order(void);
extern int acr_need_invert(Acr_byte_order byte_order);
extern void acr_set_vr_encoding(Acr_File *afp, 
                                Acr_VR_encoding_type vr_encoding);
extern Acr_VR_encoding_type acr_get_vr_encoding(Acr_File *afp);
extern void acr_set_ignore_errors(Acr_File *afp, 
                                  int ignore_nonfatal_protocol_errors);
extern int acr_ignore_protocol_errors(Acr_File *afp);
extern void acr_reverse_byte_order(long nvals, size_t value_size, 
                                   void *input_values, void *output_values);
extern void acr_get_short(Acr_byte_order byte_order, 
                          long nvals, void *input_value, 
                          Acr_Short *mach_value);
extern void acr_get_long(Acr_byte_order byte_order, 
                         long nvals, void *input_value, Acr_Long *mach_value);
extern void acr_get_float(Acr_byte_order byte_order, 
                          long nvals, void *input_value, Acr_Float *mach_value);
extern void acr_get_double(Acr_byte_order byte_order, 
                           long nvals, void *input_value, Acr_Double *mach_value);
extern void acr_put_short(Acr_byte_order byte_order, 
                          long nvals, Acr_Short *mach_value, 
                          void *output_value);
extern void acr_put_long(Acr_byte_order byte_order, 
                         long nvals, Acr_Long *mach_value, void *output_value);
extern void acr_put_float(Acr_byte_order byte_order, 
                          long nvals, Acr_Float *mach_value, void *output_value);
extern void acr_put_double(Acr_byte_order byte_order, 
                           long nvals, Acr_Double *mach_value, void *output_value);
extern Acr_Status acr_skip_input_data(Acr_File *afp, long nbytes_to_skip);
extern Acr_Status acr_read_buffer(Acr_File *afp, unsigned char buffer[],
                                  long nbytes_to_read, long *nbytes_read);
extern Acr_Status acr_unget_buffer(Acr_File *afp, unsigned char buffer[],
                                   long nbytes_to_unget);
extern Acr_Status acr_write_buffer(Acr_File *afp, unsigned char buffer[],
                                   long nbytes_to_write, long *nbytes_written);
extern Acr_Status acr_test_byte_order(Acr_File *afp);
extern void acr_copy_file_encoding(Acr_File *afp1, Acr_File *afp2);
extern int acr_get_element_header_size(char vr_name[2], 
                                       Acr_VR_encoding_type vr_encoding);
extern Acr_Status acr_peek_at_next_element_id(Acr_File *afp,
                                              int *group_id, int *element_id);
extern Acr_Status acr_read_one_element(Acr_File *afp,
                                       int *group_id, int *element_id,
                                       char vr_name[],
                                       long *data_length, char **data_pointer);
extern Acr_Status acr_write_one_element(Acr_File *afp,
                                        int group_id, int element_id,
                                        char vr_name[],
                                        long data_length, char *data_pointer);
extern char *acr_status_string(Acr_Status status);
