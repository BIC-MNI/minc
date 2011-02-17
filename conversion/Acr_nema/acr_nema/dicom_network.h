/* ----------------------------- MNI Header -----------------------------------
@NAME       : dicom_network.h
@DESCRIPTION: Header file for dicom network code
@METHOD     : 
@GLOBALS    : 
@CREATED    : February 10, 1997 (Peter Neelin)
@MODIFIED   : 
 * $Log: dicom_network.h,v $
 * Revision 6.7  2011-02-17 06:41:51  rotor
 *  * Fixed a HDF5 error output bug in testing code
 *
 * Revision 6.6  2001/03/19 18:30:33  neelin
 * Added function to set implementation uid and changed name of function
 * that gets it.
 *
 * Revision 6.5  2000/05/17 20:17:48  neelin
 * Added mechanism to allow testing of input streams for more data through
 * function acr_file_ismore.
 * This is used in dicom_client_routines to allow asynchronous transfer
 * of data, with testing for more input done before sending new messages.
 * Previous use of select for this was misguided, since select may report that
 * no data is waiting on the file descriptor while data is store in the file
 * pointer buffer (or Acr file pointer buffer).
 *
 * Revision 6.4  1999/10/29 17:51:51  neelin
 * Fixed Log keyword
 *
 * Revision 6.3  1998/11/11 17:05:03  neelin
 * Added pointer for client data to dicom structure.
 *
 * Revision 6.2  1998/03/23  20:16:37  neelin
 * Added functions.
 *
 * Revision 6.1  1997/10/20  22:52:46  neelin
 * Added support for implementation user information in association request.
 *
 * Revision 6.0  1997/09/12  13:23:59  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:25:00  neelin
 * Release of minc version 0.5
 *
 * Revision 4.1  1997/07/09  20:01:40  neelin
 * Added function acr_dicom_get_io_data.
 *
 * Revision 4.0  1997/05/07  20:01:23  neelin
 * Release of minc version 0.4
 *
 * Revision 1.2  1997/04/21  20:21:09  neelin
 * Updated the library to handle dicom messages.
 *
 * Revision 1.1  1997/02/20  16:38:17  neelin
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

/* PDU types */
#define ACR_PDU_ASSOC_RQ 0x01
#define ACR_PDU_ASSOC_AC 0x02
#define ACR_PDU_ASSOC_RJ 0x03
#define ACR_PDU_DATA_TF  0x04
#define ACR_PDU_REL_RQ   0x05
#define ACR_PDU_REL_RP   0x06
#define ACR_PDU_ABORT_RQ 0x07

/* Offset for unknown PDU item types */
#define ACR_UNKNOWN_PDU_ITEM_OFFSET 0xFF00

/* Element ids for PDU messages. These are artificial and are for internal
   use only, so they have negative group numbers. Unrecognized PDU items 
   are stored with element id 0xffxx, where xx is the PDU item type */
#define DCM_PDU_GRPID (-1)
#define DCM_PDU_ELEMENT(name, elid, vr) \
   ACRLIB_GLOBAL_ELEMENT(name, DCM_PDU_GRPID, elid, vr)
DCM_PDU_ELEMENT(DCM_PDU_Type,                            0x0010, US);
DCM_PDU_ELEMENT(DCM_PDU_Protocol_Version,                0x0020, US);
DCM_PDU_ELEMENT(DCM_PDU_Called_Ap_title,                 0x0030, UI);
DCM_PDU_ELEMENT(DCM_PDU_Calling_Ap_title,                0x0040, UI);
DCM_PDU_ELEMENT(DCM_PDU_Application_context,             0x0050, UI);
DCM_PDU_ELEMENT(DCM_PDU_Presentation_context,            0x0060, SQ);
DCM_PDU_ELEMENT(DCM_PDU_Presentation_context_reply,      0x0070, SQ);
DCM_PDU_ELEMENT(DCM_PDU_Presentation_context_list,       0x0080, SQ);
DCM_PDU_ELEMENT(DCM_PDU_Presentation_context_reply_list, 0x0090, SQ);
DCM_PDU_ELEMENT(DCM_PDU_Presentation_context_id,         0x0100, US);
DCM_PDU_ELEMENT(DCM_PDU_Abstract_syntax,                 0x0120, UI);
DCM_PDU_ELEMENT(DCM_PDU_Transfer_syntax,                 0x0130, UI);
DCM_PDU_ELEMENT(DCM_PDU_Maximum_length,                  0x0140, UL);
DCM_PDU_ELEMENT(DCM_PDU_Implementation_class_uid,        0x0141, UI);
DCM_PDU_ELEMENT(DCM_PDU_Implementation_version_name,     0x0142, UI);
DCM_PDU_ELEMENT(DCM_PDU_Result,                          0x0150, US);
DCM_PDU_ELEMENT(DCM_PDU_Source,                          0x0160, US);
DCM_PDU_ELEMENT(DCM_PDU_Reason,                          0x0170, US);

/* Function prototypes */
public int acr_uid_equal(char *uid1, char *uid2);
public char *acr_create_uid(void);
public void acr_set_implementation_uid(char *uid);
public char *acr_get_implementation_uid(void);
public Acr_Status acr_input_dicom_message(Acr_File *dicom_afp, 
                                          Acr_Message *message);
public Acr_Status acr_output_dicom_message(Acr_File *dicom_afp, 
                                           Acr_Message message);
public Acr_File *acr_initialize_dicom_input(void *io_data,
                                            int maxlength,
                                            Acr_Io_Routine io_routine);
public void acr_dicom_set_ismore_function(Acr_File *afp, 
                                          Acr_Ismore_Function ismore_function);
public Acr_File *acr_initialize_dicom_output(void *io_data,
                                            int maxlength,
                                            Acr_Io_Routine io_routine);
public void acr_close_dicom_file(Acr_File *afp);
public void *acr_dicom_get_io_data(Acr_File *afp);
public void acr_dicom_enable_trace(Acr_File *afp);
public void acr_dicom_disable_trace(Acr_File *afp);
public void acr_dicom_set_eof(Acr_File *afp);
public void acr_set_dicom_maximum_length(Acr_File *afp, 
                                         long maximum_length);
public void acr_set_dicom_pres_context_id(Acr_File *afp, 
                                          int presentation_context_id);
public int acr_get_dicom_pres_context_id(Acr_File *afp);
public void acr_set_dicom_client_data(Acr_File *afp, void *client_data);
public void *acr_get_dicom_client_data(Acr_File *afp);
