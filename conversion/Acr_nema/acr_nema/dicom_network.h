/* ----------------------------- MNI Header -----------------------------------
@NAME       : dicom_network.h
@DESCRIPTION: Header file for dicom network code
@METHOD     : 
@GLOBALS    : 
@CREATED    : February 10, 1997 (Peter Neelin)
@MODIFIED   : $Log: dicom_network.h,v $
@MODIFIED   : Revision 1.1  1997-02-20 16:38:17  neelin
@MODIFIED   : Initial revision
@MODIFIED   :
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

/* Element ids for PDU messages. These are artificial and are for internal
   use only, so they have negative group numbers. */
#define DCM_PDU_GRPID (-1)
#define DCM_PDU_ELEMENT(name, elid) \
   ACRLIB_GLOBAL_ELEMENT(name, DCM_PDU_GRPID, elid)
DCM_PDU_ELEMENT(DCM_PDU_Type,                            0x0010);
DCM_PDU_ELEMENT(DCM_PDU_Protocol_Version,                0x0020);
DCM_PDU_ELEMENT(DCM_PDU_Called_Ap_title,                 0x0030);
DCM_PDU_ELEMENT(DCM_PDU_Calling_Ap_title,                0x0040);
DCM_PDU_ELEMENT(DCM_PDU_Application_context,             0x0050);
DCM_PDU_ELEMENT(DCM_PDU_Presentation_context,            0x0060);
DCM_PDU_ELEMENT(DCM_PDU_Presentation_context_reply,      0x0070);
DCM_PDU_ELEMENT(DCM_PDU_Presentation_context_list,       0x0080);
DCM_PDU_ELEMENT(DCM_PDU_Presentation_context_reply_list, 0x0090);
DCM_PDU_ELEMENT(DCM_PDU_Presentation_context_id,         0x0100);
DCM_PDU_ELEMENT(DCM_PDU_Presentation_context_result,     0x0110);
DCM_PDU_ELEMENT(DCM_PDU_Abstract_syntax,                 0x0120);
DCM_PDU_ELEMENT(DCM_PDU_Transfer_syntax,                 0x0130);
DCM_PDU_ELEMENT(DCM_PDU_Maximum_length,                  0x0140);
DCM_PDU_ELEMENT(DCM_PDU_Result,                          0x0150);
DCM_PDU_ELEMENT(DCM_PDU_Source,                          0x0160);
DCM_PDU_ELEMENT(DCM_PDU_Reason,                          0x0170);

/* Function prototypes */
public Acr_Status acr_input_dicom_message(Acr_File *dicom_afp, 
                                          Acr_Message *message);
public Acr_Status acr_output_dicom_message(Acr_File *dicom_afp, 
                                           Acr_Message message);
public Acr_File *acr_initialize_dicom_input(void *io_data,
                                            int maxlength,
                                            Acr_Io_Routine io_routine);
public Acr_File *acr_initialize_dicom_output(void *io_data,
                                            int maxlength,
                                            Acr_Io_Routine io_routine);
public void acr_close_dicom_file(Acr_File *afp);
public void acr_dicom_enable_trace(Acr_File *afp);
public void acr_dicom_disable_trace(Acr_File *afp);
public void acr_set_dicom_maximum_length(Acr_File *afp, 
                                         long maximum_length);
public void acr_set_dicom_pres_context_id(Acr_File *afp, 
                                          int presentation_context_id);
public int acr_get_dicom_pres_context_id(Acr_File *afp);
