/* ----------------------------- MNI Header -----------------------------------
@NAME       : dicom_network.c
@DESCRIPTION: Routines for doing dicom network communications
@METHOD     : 
@GLOBALS    : 
@CREATED    : February 10, 1997 (Peter Neelin)
@MODIFIED   : $Log: dicom_network.c,v $
@MODIFIED   : Revision 1.2  1997-04-21 20:21:09  neelin
@MODIFIED   : Updated the library to handle dicom messages.
@MODIFIED   :
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

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>
#include <minc_def.h>
#include <acr_nema.h>

/* Constants */
#define ACR_COMMAND_GRPID 0x0
#define PDU_HEADER_LEN (2+ACR_SIZEOF_LONG)
#define PDU_ITEM_HEADER_LEN (2+ACR_SIZEOF_SHORT)
#define ASSOC_RQ_LEN 74
#define ASSOC_RJ_LEN 10
#define ABORT_RQ_LEN 10
#define DATA_TF_LEN 6
#define MAX_PDU_STRING_LENGTH 1024
#define DICOM_NETWORK_BYTE_ORDER ACR_BIG_ENDIAN

/* PDU item types */
#define PDU_ITEM_APPLICATION_CONTEXT  0x10
#define PDU_ITEM_PRESENTATION_CONTEXT 0x20
#define PDU_ITEM_PRES_CONTEXT_REPLY   0x21
#define PDU_ITEM_ABSTRACT_SYNTAX      0x30
#define PDU_ITEM_TRANSFER_SYNTAX      0x40
#define PDU_ITEM_USER_INFORMATION     0x50
#define PDU_ITEM_MAXIMUM_LENGTH       0x51

/* Mask for getting info out of PDV message control header */
#define PDV_COMMAND_PDV_MASK 0x1
#define PDV_LAST_FRAGMENT_MASK 0x2

/* Types to allow 2-level hierarchy of i/o streams */
typedef enum {DICOM_INPUT=0xbead, DICOM_OUTPUT} Dicom_IO_stream_type;
typedef struct {
   Dicom_IO_stream_type stream_type;
   Acr_File *real_afp;           /* Pointer to real input stream */
   Acr_File *virtual_afp;        /* Pointer to message stream to which this
                                    io data is attached */
   int presentation_context_id;
   long pdu_watchpoint;          /* Distance from current watchpoint to
                                    PDU watchpoint for input stream */
   long maximum_length;          /* Maximum PDU length (excluding header) */
   int writing_command;          /* True if writing command portion */
   long data_length;             /* Length of data portion of message */
} Acr_Dicom_IO;

/* Private functions */
private Acr_Status read_pdu_header(Acr_File *afp, int *pdu_type, 
                                   long *pdu_length);
private Acr_Status read_assoc_rq(Acr_File *afp, Acr_Group group);
private Acr_Status read_assoc_rq_ac(Acr_File *afp, Acr_Group group, 
                                 int is_request);
private Acr_Status read_data_tf(Acr_File *dicom_afp, Acr_Group group);
private Acr_Status read_rel_rq(Acr_File *afp, Acr_Group group);
private Acr_Status read_abort_rq(Acr_File *afp, Acr_Group group);
private Acr_Status read_assoc_ac(Acr_File *afp, Acr_Group group);
private Acr_Status read_assoc_rj(Acr_File *afp, Acr_Group group);
private Acr_Status read_rel_rp(Acr_File *afp, Acr_Group group);
private Acr_Status read_pdu_item(Acr_File *afp, Acr_Element *item);
private Acr_Status read_uid_item(Acr_File *afp, Acr_Element_Id elid, 
                                 Acr_Element *item);
private Acr_Status read_long_item(Acr_File *afp, Acr_Element_Id elid, 
                                 Acr_Element *item);
private Acr_Status read_unknown_item(Acr_File *afp, int item_type, 
                                     Acr_Element *item);
private Acr_Status read_pres_context_item(Acr_File *afp, Acr_Element *item);
private Acr_Status read_pres_context_reply_item(Acr_File *afp, 
                                                Acr_Element *item);
private Acr_Status read_user_info_item(Acr_File *afp, Acr_Element *item);
private char *get_uid_string(char *buffer, int length);
private Acr_Element pdu_create_element_uid(Acr_Element_Id elid,
                                           char *value, int length);
private Acr_Element pdu_create_element_short(Acr_Element_Id elid,
                                             void *input_value);
private Acr_Status write_assoc_rq(Acr_File *afp, Acr_Group group);
private Acr_Status write_assoc_ac(Acr_File *afp, Acr_Group group);
private Acr_Status write_assoc_rq_ac(Acr_File *afp, Acr_Group group, 
                                     int is_request, long *length);
private Acr_Status write_assoc_rj(Acr_File *afp, Acr_Group group_list);
private Acr_Status write_rel_rq(Acr_File *afp, Acr_Group group_list);
private Acr_Status write_rel_rp(Acr_File *afp, Acr_Group group_list);
private Acr_Status write_abort_rq(Acr_File *afp, Acr_Group group_list);
private Acr_Status write_fixed_length_pdu(Acr_File *afp, int pdu_type, 
                                          int result, int source, int reason);
private Acr_Status write_data_tf(Acr_File *dicom_afp, Acr_Message message);
private Acr_Status write_uid_item(Acr_File *afp, Acr_Element element, 
                                  int item_type, long *length);
private Acr_Status write_pres_context_item(Acr_File *afp, Acr_Element item,
                                           int is_request, long *length);
private Acr_Status write_user_info_item(Acr_File *afp, Acr_Group group,
                                        long *length);
private Acr_Status write_unknown_item(Acr_File *afp, int item_type,
                                      long data_length, char *data_pointer, 
                                      long *length);
private void pdu_copy_uid(char *string, char *buffer, int length);
private Acr_File *initialize_dicom_stream(void *io_data, int maxlength,
                                          Acr_Io_Routine io_routine,
                                          Dicom_IO_stream_type stream_type);
private Acr_Dicom_IO *get_dicom_io_pointer(Acr_File *afp);
private Acr_File *get_dicom_real_afp(Acr_File *afp);
private long get_dicom_pdu_watchpoint(Acr_File *afp);
private void set_dicom_pdu_watchpoint(Acr_File *afp, long pdu_watchpoint);
private void dicom_reset(Acr_File *afp);
private void dicom_setup_output(Acr_File *afp, 
                                long command_length, long data_length);
private int dicom_input_routine(void *io_data, void *buffer, int nbytes);
private int dicom_output_routine(void *io_data, void *buffer, int nbytes);

/* Macros */
#define EXTRACT_UID(group, elid, value, length) \
   acr_group_add_element(group, pdu_create_element_uid(elid, value, length))
#define EXTRACT_SHORT(group, elid, value) \
   acr_group_add_element(group, pdu_create_element_short(elid, value))
#define SAVE_SHORT(group, elid, value) \
   acr_group_add_element(group, \
      acr_create_element_short(elid, (unsigned short) (value)))
#define GET_SHORT(input, output) \
   acr_get_short(DICOM_NETWORK_BYTE_ORDER, (long) 1, input, output)
#define GET_LONG(input, output) \
   acr_get_long(DICOM_NETWORK_BYTE_ORDER, (long) 1, input, output)
#define PUT_SHORT(input, output) \
   acr_put_short(DICOM_NETWORK_BYTE_ORDER, (long) 1, input, output)
#define PUT_LONG(input, output) \
   acr_put_long(DICOM_NETWORK_BYTE_ORDER, (long) 1, input, output)
#define COPY_UID(group, elid, buffer, length) \
   pdu_copy_uid(acr_find_string(group, elid, ""), (char *) buffer, length)


/*****************************************************************************/
/*************************** INPUT ROUTINES **********************************/
/*****************************************************************************/

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_input_dicom_message
@INPUT      : dicom_afp - acr file pointer for dicom input
@OUTPUT     : message - NULL if no message is read
@RETURNS    : status of input
@DESCRIPTION: Reads in a dicom message and returns it in a message structure.
@METHOD     : Although dicom connection, etc. syntaxes differ from the 
              message syntax, this routine and the ones following put it all 
              in the same form.
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 10, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Status acr_input_dicom_message(Acr_File *dicom_afp, 
                                          Acr_Message *message)
{
   int pdu_type;
   long pdu_length;
   long old_watchpoint;
   Acr_File *afp;
   Acr_Group group;
   Acr_Status status;

   /* Set message to NULL in case of error */
   *message = NULL;

   /* Get real afp */
   afp = get_dicom_real_afp(dicom_afp);
   if (afp == NULL) {
      (void) fprintf(stderr, "Bad dicom file pointer\n");
      exit(EXIT_FAILURE);
   }

   /* Set a watchpoint before we do anything so that we don't get stuck
      reading too much */
   old_watchpoint = acr_get_io_watchpoint(afp);
   acr_set_io_watchpoint(afp, PDU_HEADER_LEN);

   /* Read in PDU type and length */
   status = read_pdu_header(afp, &pdu_type, &pdu_length);
   if (old_watchpoint != ACR_NO_WATCHPOINT)
      old_watchpoint -= PDU_HEADER_LEN - acr_get_io_watchpoint(afp);
   if (status != ACR_OK) {
      acr_set_io_watchpoint(afp, old_watchpoint);
      return status;
   }

   /* Create the message and add the PDU type */
   group = acr_create_group(DCM_PDU_GRPID);
   SAVE_SHORT(group, DCM_PDU_Type, pdu_type);

   /* Set a watchpoint for reading */
   acr_set_io_watchpoint(afp, pdu_length);

   /* Call the appropriate routine */
   switch (pdu_type) {
   case ACR_PDU_ASSOC_RQ:
      status = read_assoc_rq(afp, group);
      break;
   case ACR_PDU_ASSOC_AC:
      status = read_assoc_ac(afp, group);
      break;
   case ACR_PDU_ASSOC_RJ:
      status = read_assoc_rj(afp, group);
      break;
   case ACR_PDU_DATA_TF:
      status = read_data_tf(dicom_afp, group);
      break;
   case ACR_PDU_REL_RQ:
      status = read_rel_rq(afp, group);
      break;
   case ACR_PDU_REL_RP:
      status = read_rel_rp(afp, group);
      break;
   case ACR_PDU_ABORT_RQ:
      status = read_abort_rq(afp, group);
      break;
   default:
      status = acr_skip_input_data(afp, pdu_length);
      break;
   }

   /* Check that we read exactly up to the watchpoint */
   if (acr_get_io_watchpoint(afp) != 0) {
      switch (status) {
      case ACR_OK:
         status = ACR_PROTOCOL_ERROR; break;
      case ACR_END_OF_INPUT:
         status = ACR_ABNORMAL_END_OF_INPUT; break;
      }
   }
   if (old_watchpoint != ACR_NO_WATCHPOINT)
      old_watchpoint -= pdu_length - acr_get_io_watchpoint(afp);
   acr_set_io_watchpoint(afp, old_watchpoint);

   /* Create the message */
   *message = acr_create_message();
   acr_message_add_group_list(*message, group);

   return status;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_pdu_header
@INPUT      : afp - acr file pointer
@OUTPUT     : pdu_type
              pdu_length
@RETURNS    : status of input
@DESCRIPTION: Reads in the header of a dicom PDU and returns the appropriate
              values.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 10, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Status read_pdu_header(Acr_File *afp, int *pdu_type, 
                                   long *pdu_length)
{
   unsigned char buffer[PDU_HEADER_LEN];
   Acr_Status status;

   /* Read in PDU type and length */
   status = acr_read_buffer(afp, buffer, sizeof(buffer), NULL);
   if (status != ACR_OK) return status;
   *pdu_type = (int) buffer[0];
   GET_LONG(&buffer[2], pdu_length);

   return status;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_assoc_rq
@INPUT      : afp - acr file pointer
@OUTPUT     : group - group to which pdu information should be added
@RETURNS    : status of input
@DESCRIPTION: Reads in an association request
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 10, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Status read_assoc_rq(Acr_File *afp, Acr_Group group)
{
   return read_assoc_rq_ac(afp, group, TRUE);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_assoc_rq_ac
@INPUT      : afp - acr file pointer
              is_request - TRUE if we should handle request type PDU, FALSE if
                 we should handle accept type PDU
@OUTPUT     : group - group to which pdu information should be added
@RETURNS    : status of input
@DESCRIPTION: Reads in an association request or accept PDU
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 10, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Status read_assoc_rq_ac(Acr_File *afp, Acr_Group group, 
                                 int is_request)
{
   unsigned char buffer[ASSOC_RQ_LEN - PDU_HEADER_LEN];
   Acr_Element item, presentation_context_list, nextitem;
   int have_pres_context_request;
   Acr_Status status;

   /* Read in header */
   status = acr_read_buffer(afp, buffer, sizeof(buffer), NULL);
   if (status != ACR_OK) return status;

   /* Extract appropriate info and add it to the group */
   EXTRACT_SHORT(group, DCM_PDU_Protocol_Version, &buffer[6-PDU_HEADER_LEN]);
   if (is_request) {
      EXTRACT_UID(group, DCM_PDU_Called_Ap_title, 
                  (char *) &buffer[10-PDU_HEADER_LEN], 16);
      EXTRACT_UID(group, DCM_PDU_Calling_Ap_title, 
                  (char *) &buffer[26-PDU_HEADER_LEN], 16);
   }

   /* Loop, reading items, until the watchpoint is reached */
   presentation_context_list = NULL;
   while (acr_get_io_watchpoint(afp) > 0) {

      /* Read in the item (or item list) */
      status = read_pdu_item(afp, &item);
      if (status != ACR_OK) {
         acr_delete_element_list(item);
         acr_delete_element_list(presentation_context_list);
         return status;
      }

      /* Loop over items, checking that we got an acceptable one and
         add it to the appropriate thing */
      for (nextitem = item; nextitem != NULL; 
           nextitem = acr_get_element_next(nextitem)) {
         if (acr_match_element_id(DCM_PDU_Presentation_context, nextitem) ||
             acr_match_element_id(DCM_PDU_Presentation_context_reply, 
                                  nextitem)) {
            presentation_context_list = 
               acr_element_list_add(presentation_context_list, nextitem);
            have_pres_context_request = 
               acr_match_element_id(DCM_PDU_Presentation_context, nextitem);
         }
         else {
            acr_group_add_element(group, nextitem);
         }
      }
   }

   /* Add the presentation context list to the group */
   if (have_pres_context_request) {
      acr_group_add_element(group,
         acr_create_element_sequence(DCM_PDU_Presentation_context_list, 
                                     presentation_context_list));
   }
   else {
      acr_group_add_element(group,
         acr_create_element_sequence(DCM_PDU_Presentation_context_reply_list, 
                                     presentation_context_list));
   }

   return status;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_data_tf
@INPUT      : dicom_afp - acr file pointer for DICOM message level
@OUTPUT     : group - group to which pdu information should be added
@RETURNS    : status of input
@DESCRIPTION: Reads in a data PDU
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 10, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Status read_data_tf(Acr_File *dicom_afp, Acr_Group group)
{
   Acr_Status status;
   Acr_Message message;
   Acr_Group cur, new;
   Acr_File *real_afp;
   long pdu_watchpoint;

   /* Reset the virtual input stream */
   dicom_reset(dicom_afp);

   /* Set the watchpoint to some very large number (other than 
      ACR_NO_WATCHPOINT) so that acr_input_message will be happy knowing
      that there is a watchpoint, even though we don't know what it really
      is until we get close to it. */
   acr_set_io_watchpoint(dicom_afp, LONG_MAX-1);

   /* Get the real afp */
   real_afp = get_dicom_real_afp(dicom_afp);
   if (real_afp == NULL) {
      (void) fprintf(stderr, "Bad dicom file pointer\n");
      exit(EXIT_FAILURE);
   }

   /* Save the PDU watchpoint and set a watchpoint to force the read 
      routine to look for a PDV header */
   pdu_watchpoint = acr_get_io_watchpoint(real_afp);
   acr_set_io_watchpoint(real_afp, (long) 0);
   set_dicom_pdu_watchpoint(dicom_afp, pdu_watchpoint);

   /* Read in the message */
   status = acr_input_message(dicom_afp, &message);

   /* Add the presentation context id to the pdu group */
   SAVE_SHORT(group, DCM_PDU_Presentation_context_id,
              (unsigned short) acr_get_dicom_pres_context_id(dicom_afp));

   /* Get groups and tack them onto the group list that we already have */
   if (message != NULL) {
      new = acr_get_message_group_list(message);
      cur = group;
      while (acr_get_group_next(cur) != NULL) {
         cur = acr_get_group_next(cur);
      }
      acr_set_group_next(cur, new);
      acr_message_reset(message);
      acr_delete_message(message);
   }

   /* Check that we read exactly up to the watchpoint */
   if (acr_get_io_watchpoint(real_afp) != 0) {
      switch (status) {
      case ACR_OK:
         status = ACR_PROTOCOL_ERROR; break;
      case ACR_END_OF_INPUT:
         status = ACR_ABNORMAL_END_OF_INPUT; break;
      }
   }

   /* Restore the PDU watchpoint */
   acr_set_io_watchpoint(real_afp, get_dicom_pdu_watchpoint(dicom_afp));

   return status;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_rel_rq
@INPUT      : afp - acr file pointer
@OUTPUT     : group - group to which pdu information should be added
@RETURNS    : status of input
@DESCRIPTION: Reads in a release request
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 10, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
/* ARGSUSED */
private Acr_Status read_rel_rq(Acr_File *afp, Acr_Group group)
{
   return acr_skip_input_data(afp, (long) ACR_SIZEOF_LONG);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_abort_rq
@INPUT      : afp - acr file pointer
@OUTPUT     : group - group to which pdu information should be added
@RETURNS    : status of input
@DESCRIPTION: Reads in an abort request
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 10, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Status read_abort_rq(Acr_File *afp, Acr_Group group)
{
   unsigned char buffer[ABORT_RQ_LEN - PDU_HEADER_LEN];
   Acr_Status status;

   /* Read in request header */
   status = acr_read_buffer(afp, buffer, sizeof(buffer), NULL);
   if (status != ACR_OK) return status;

   /* Get values */
   SAVE_SHORT(group, DCM_PDU_Source, buffer[8 - PDU_HEADER_LEN]);
   SAVE_SHORT(group, DCM_PDU_Reason, buffer[9 - PDU_HEADER_LEN]);

   return status;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_assoc_ac
@INPUT      : afp - acr file pointer
@OUTPUT     : group - group to which pdu information should be added
@RETURNS    : status of input
@DESCRIPTION: Reads in an association accept PDU
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 10, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Status read_assoc_ac(Acr_File *afp, Acr_Group group)
{
   return read_assoc_rq_ac(afp, group, FALSE);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_assoc_rj
@INPUT      : afp - acr file pointer
@OUTPUT     : group - group to which pdu information should be added
@RETURNS    : status of input
@DESCRIPTION: Reads in an association reject PDU
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 10, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Status read_assoc_rj(Acr_File *afp, Acr_Group group)
{
   unsigned char buffer[ASSOC_RJ_LEN - PDU_HEADER_LEN];
   Acr_Status status;

   /* Read in request header */
   status = acr_read_buffer(afp, buffer, sizeof(buffer), NULL);
   if (status != ACR_OK) return status;

   /* Get values */
   SAVE_SHORT(group, DCM_PDU_Result, buffer[7 - PDU_HEADER_LEN]);
   SAVE_SHORT(group, DCM_PDU_Source, buffer[8 - PDU_HEADER_LEN]);
   SAVE_SHORT(group, DCM_PDU_Reason, buffer[9 - PDU_HEADER_LEN]);

   return status;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_rel_rp
@INPUT      : afp - acr file pointer
@OUTPUT     : group - group to which pdu information should be added
@RETURNS    : status of input
@DESCRIPTION: Reads in a release reply
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 10, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
/* ARGSUSED */
private Acr_Status read_rel_rp(Acr_File *afp, Acr_Group group)
{
   return acr_skip_input_data(afp, (long) ACR_SIZEOF_LONG);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_pdu_item
@INPUT      : afp - acr file pointer
@OUTPUT     : item - element representing value read in
@RETURNS    : status of input
@DESCRIPTION: Reads in any type of PDU item and returns it as an element.
              The element may either by a real DICOM-type element or may
              be a sequence item.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 12, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Status read_pdu_item(Acr_File *afp, Acr_Element *item)
{
   unsigned char buffer[PDU_ITEM_HEADER_LEN];
   unsigned short item_length;
   int item_type;
   long old_watchpoint;
   Acr_Status status;

   /* Read in PDU item type and length */
   status = acr_read_buffer(afp, buffer, sizeof(buffer), NULL);
   if (status != ACR_OK) return status;
   item_type = (int) buffer[0];
   GET_SHORT(&buffer[2], &item_length);

   /* Set a watchpoint for reading after saving the previous watchpoint */
   old_watchpoint = acr_get_io_watchpoint(afp);
   acr_set_io_watchpoint(afp, (long) item_length);

   /* Call the appropriate routine */
   switch (item_type) {
   case PDU_ITEM_APPLICATION_CONTEXT:
      status = read_uid_item(afp, DCM_PDU_Application_context, item);
      break;
   case PDU_ITEM_PRESENTATION_CONTEXT:
      status = read_pres_context_item(afp, item);
      break;
   case PDU_ITEM_PRES_CONTEXT_REPLY:
      status = read_pres_context_reply_item(afp, item);
      break;
   case PDU_ITEM_ABSTRACT_SYNTAX:
      status = read_uid_item(afp, DCM_PDU_Abstract_syntax, item);
      break;
   case PDU_ITEM_TRANSFER_SYNTAX:
      status = read_uid_item(afp, DCM_PDU_Transfer_syntax, item);
      break;
   case PDU_ITEM_USER_INFORMATION:
      status = read_user_info_item(afp, item);
      break;
   case PDU_ITEM_MAXIMUM_LENGTH:
      status = read_long_item(afp, DCM_PDU_Maximum_length, item);
      break;
   default:
      status = read_unknown_item(afp, item_type, item);
      break;
   }

   /* Make sure that we read the right amount of data */
   if ((status == ACR_OK) && (acr_get_io_watchpoint(afp) != 0)) {
      status = ACR_PROTOCOL_ERROR;
   }

   /* Set the old watchpoint */
   if (old_watchpoint != ACR_NO_WATCHPOINT)
      old_watchpoint -= (long) item_length - acr_get_io_watchpoint(afp);
   acr_set_io_watchpoint(afp, old_watchpoint);

   return status;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_uid_item
@INPUT      : afp - acr file pointer
              elid - id of element to be created
@OUTPUT     : item - element representing value read in
@RETURNS    : status of input
@DESCRIPTION: Reads in a UID item and creates the corresponding element.
              Data is read up to the next watchpoint.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 12, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Status read_uid_item(Acr_File *afp, Acr_Element_Id elid, 
                                 Acr_Element *item)
{
   unsigned char buffer[MAX_PDU_STRING_LENGTH];
   long length;
   Acr_Status status;

   /* Set default item */
   *item = NULL;

   /* Figure out how much data to read */
   length = acr_get_io_watchpoint(afp);
   if (length > MAX_PDU_STRING_LENGTH) return ACR_OTHER_ERROR;

   /* Read in the buffer */
   status = acr_read_buffer(afp, buffer, length, NULL);
   if (status != ACR_OK) return status;

   /* Pull out the uid and create the item */
   *item = pdu_create_element_uid(elid, (char *) buffer, (int) length);

   return ACR_OK;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_long_item
@INPUT      : afp - acr file pointer
              elid - id of element to be created
@OUTPUT     : item - element representing value read in
@RETURNS    : status of input
@DESCRIPTION: Reads in an item of type long and creates the corresponding 
              element.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 12, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Status read_long_item(Acr_File *afp, Acr_Element_Id elid, 
                                 Acr_Element *item)
{
   unsigned char buffer[ACR_SIZEOF_LONG];
   long value;
   Acr_Status status;

   /* Set default item */
   *item = NULL;

   /* Check that the right amount of data is to be read */
   if (acr_get_io_watchpoint(afp) != ACR_SIZEOF_LONG) 
      return ACR_PROTOCOL_ERROR;

   /* Read in the buffer and get the value */
   status = acr_read_buffer(afp, buffer, (long) ACR_SIZEOF_LONG, NULL);
   if (status != ACR_OK) return status;
   GET_LONG((void *) buffer, &value);

   /* Create the item */
   *item = acr_create_element_long(elid, value);

   return ACR_OK;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_unknown_item
@INPUT      : afp - acr file pointer
              item_type - type code for item to be read in (should be 
                 a value between 0 and 255)
@OUTPUT     : item - element representing value read in
@RETURNS    : status of input
@DESCRIPTION: Reads in an item of unknown type and creates an element with
              element id = item_type + ACR_UNKNOWN_PDU_ITEM_OFFSET
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 12, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Status read_unknown_item(Acr_File *afp, int item_type, 
                                     Acr_Element *item)
{
   Acr_Status status;
   long length;
   unsigned char *data_pointer;

   /* Set default item */
   *item = NULL;

   /* Figure out how much data to read */
   length = acr_get_io_watchpoint(afp);

   /* Get space for the data. Add a NUL character to the end in case it is
      a string */
   data_pointer = MALLOC((size_t) length+1);
   data_pointer[length] = '\0';

   /* Read in the buffer and get the value */
   status = acr_read_buffer(afp, data_pointer, length, NULL);
   if (status != ACR_OK) {
      FREE(data_pointer);
      return status;
   }

   /* Figure out the element id */
   item_type += ACR_UNKNOWN_PDU_ITEM_OFFSET;

   /* Create the item */
   *item = acr_create_element(DCM_PDU_GRPID, item_type, ACR_VR_UNKNOWN, 
                              length, (char *) data_pointer);

   return ACR_OK;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_pres_context_item
@INPUT      : afp - acr file pointer
@OUTPUT     : item - element representing value read in
@RETURNS    : status of input
@DESCRIPTION: Reads in a presentation context item for an associate request 
              and creates the corresponding element. Data is read up to the 
              next watchpoint.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 12, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Status read_pres_context_item(Acr_File *afp, Acr_Element *item)
{
   unsigned char buffer[4];
   Acr_Element itemlist = NULL, newitem;
   Acr_Status status;
   int nitems;

   /* Set default item */
   *item = NULL;

   /* Read in buffer */
   status = acr_read_buffer(afp, buffer, sizeof(buffer), NULL);
   if (status != ACR_OK) return status;

   /* Get presentation context id and save it in the item list */
   newitem = acr_create_element_short(DCM_PDU_Presentation_context_id, 
                                      (unsigned short) buffer[0]);
   itemlist = acr_element_list_add(itemlist, newitem);

   /* Read in items until we reach watchpoint */
   nitems = 0;
   while (acr_get_io_watchpoint(afp) > 0) {

      /* Read in the item and add it to the list */
      status = read_pdu_item(afp, &newitem);
      if (newitem != NULL) {
         itemlist = acr_element_list_add(itemlist, newitem);
      }

      /* Check for a read error */
      if (status != ACR_OK) {
         acr_delete_element_list(itemlist);
         return status;
      }

      /* Make sure that we got the right thing */
      if (((nitems == 0) && 
           !acr_match_element_id(DCM_PDU_Abstract_syntax, newitem)) ||
          ((nitems > 0) &&
           !acr_match_element_id(DCM_PDU_Transfer_syntax, newitem))) {
         acr_delete_element_list(itemlist);
         return ACR_PROTOCOL_ERROR;
      }
      nitems++;
   }

   /* Create the item */
   *item = acr_create_element_sequence(DCM_PDU_Presentation_context,
                                       itemlist);

   return ACR_OK;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_pres_context_reply_item
@INPUT      : afp - acr file pointer
@OUTPUT     : item - element representing value read in
@RETURNS    : status of input
@DESCRIPTION: Reads in a presentation context item for an associate accept 
              and creates the corresponding element. Data is read up to the 
              next watchpoint.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 12, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Status read_pres_context_reply_item(Acr_File *afp, 
                                                Acr_Element *item)
{
   unsigned char buffer[4];
   Acr_Element itemlist = NULL, newitem;
   Acr_Status status;

   /* Set default item */
   *item = NULL;

   /* Read in buffer */
   status = acr_read_buffer(afp, buffer, sizeof(buffer), NULL);
   if (status != ACR_OK) return status;

   /* Get presentation context id and result and save them in the item list */
   newitem = acr_create_element_short(DCM_PDU_Presentation_context_id, 
                                      (unsigned short) buffer[0]);
   itemlist = acr_element_list_add(itemlist, newitem);
   newitem = acr_create_element_short(DCM_PDU_Result, 
                                      (unsigned short) buffer[2]);
   itemlist = acr_element_list_add(itemlist, newitem);

   /* Read in items until we reach watchpoint */
   while (acr_get_io_watchpoint(afp) > 0) {

      /* Read in the item and add it to the list */
      status = read_pdu_item(afp, &newitem);
      if (newitem != NULL) {
         itemlist = acr_element_list_add(itemlist, newitem);
      }

      /* Check for a read error */
      if (status != ACR_OK) {
         acr_delete_element_list(itemlist);
         return status;
      }

      /* Make sure that we got the right thing */
      if (!acr_match_element_id(DCM_PDU_Transfer_syntax, newitem)) {
         acr_delete_element_list(itemlist);
         return ACR_PROTOCOL_ERROR;
      }
   }

   /* Create the item */
   *item = acr_create_element_sequence(DCM_PDU_Presentation_context_reply,
                                       itemlist);

   return ACR_OK;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_user_info_item
@INPUT      : afp - acr file pointer
@OUTPUT     : item - element list representing values read in
@RETURNS    : status of input
@DESCRIPTION: Reads in a user info item and creates the corresponding 
              element list. Data is read up to the next watchpoint.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 12, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Status read_user_info_item(Acr_File *afp, Acr_Element *item)
{
   Acr_Status status;
   Acr_Element newitem;

   /* Set default item */
   *item = NULL;

   /* Read in items until we reach watchpoint */
   while (acr_get_io_watchpoint(afp) > 0) {

      /* Read in the item */
      status = read_pdu_item(afp, &newitem);

      /* Check for a read error */
      if (status != ACR_OK) {
         acr_delete_element_list(newitem);
         return status;
      }

      /* Add the item to the list */
      *item = acr_element_list_add(*item, newitem);

   }

   return ACR_OK;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_uid_string
@INPUT      : buffer - pointer to start of string
              length - length of string
@OUTPUT     : (none)
@RETURNS    : Pointer to temporary string space containing a copy of the
              string without leading and trailing blanks.
@DESCRIPTION: Removes leading and trailing blanks from a string and copies it
              into temporary space.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 12, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private char *get_uid_string(char *buffer, int length)
{
   static char string[MAX_PDU_STRING_LENGTH];
   int start, end, ichar;

   /* Check for maximum string length */
   if (length >= MAX_PDU_STRING_LENGTH) length = MAX_PDU_STRING_LENGTH - 1;

   /* Skip leading spaces */
   for (start=0; (start < length) && isspace((int) buffer[start]); start++) {}

   /* Look for the last non-space */
   for (end=length-1; (end >= 0) && isspace((int) buffer[end]); end--) {}

   /* Copy the string */
   for (ichar=start; ichar <= end; ichar++) {
      string[ichar-start] = buffer[ichar];
   }

   /* Finish off the string. Pad the string with a blank if needed to give
      an even length */
   if (((end-start+1) % 2) != 0) {      /* Odd length */
      string[++end] = ' ';
   }
   string[end+1] = '\0';

   return string;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : pdu_create_element_uid
@INPUT      : elid - Element id structure
              value - pointer to start of string
              length - length of string
@OUTPUT     : (none)
@RETURNS    : Pointer to newly created element
@DESCRIPTION: Creates an element containing a string of fixed length. Leading
              and trailing spaces are removed.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 12, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Element pdu_create_element_uid(Acr_Element_Id elid,
                                           char *value, int length)
{
   Acr_Element item;

   item = acr_create_element_string(elid, get_uid_string(value, length));
   acr_set_element_vr(item, ACR_VR_UI);

   return item;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : pdu_create_element_short
@INPUT      : elid - Element id structure
              value - pointer to location containing short
@OUTPUT     : (none)
@RETURNS    : Pointer to newly created element
@DESCRIPTION: Creates an element containing a short which is converted from
              the input format.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 12, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Element pdu_create_element_short(Acr_Element_Id elid,
                                             void *input_value)
{
   unsigned short svalue;

   GET_SHORT(input_value, &svalue);
   return acr_create_element_short(elid, svalue);
}

/*****************************************************************************/
/*************************** OUTPUT ROUTINES *********************************/
/*****************************************************************************/

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_output_dicom_message
@INPUT      : dicom_afp - acr file pointer for dicom output
              message - the message to write out
@OUTPUT     : (none)
@RETURNS    : status of output
@DESCRIPTION: Writes out a dicom message.
@METHOD     : Although dicom connection, etc. syntaxes differ from the 
              message syntax, this routine and the ones following take a 
              message in the same form.
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 10, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Status acr_output_dicom_message(Acr_File *dicom_afp, 
                                           Acr_Message message)
{
   int pdu_type;
   Acr_Group group_list;
   Acr_File *afp;
   Acr_Status status;

   /* Get real afp */
   afp = get_dicom_real_afp(dicom_afp);
   if (afp == NULL) {
      (void) fprintf(stderr, "Bad dicom file pointer\n");
      exit(EXIT_FAILURE);
   }

   /* Get group list */
   group_list = acr_get_message_group_list(message);

   /* Switch on pdu type. If the type is not found, then assume that 
      we are writing data. */
   pdu_type = acr_find_short(group_list, DCM_PDU_Type, ACR_PDU_DATA_TF);
   switch (pdu_type) {
   case ACR_PDU_ASSOC_RQ:
      status = write_assoc_rq(afp, group_list);
      break;
   case ACR_PDU_ASSOC_AC:
      status = write_assoc_ac(afp, group_list);
      break;
   case ACR_PDU_ASSOC_RJ:
      status = write_assoc_rj(afp, group_list);
      break;
   case ACR_PDU_DATA_TF:
      status = write_data_tf(dicom_afp, message);
      break;
   case ACR_PDU_REL_RQ:
      status = write_rel_rq(afp, group_list);
      break;
   case ACR_PDU_REL_RP:
      status = write_rel_rp(afp, group_list);
      break;
   case ACR_PDU_ABORT_RQ:
      status = write_abort_rq(afp, group_list);
      break;
   default:
      (void) fprintf(stderr, "Unrecognized pdu type for output (%d)\n", 
                     pdu_type);
      status = ACR_PROTOCOL_ERROR;
   }

   /* Flush the output buffer */
   if ((acr_file_flush(afp) == EOF) && (status == ACR_OK)) {
      status = ACR_ABNORMAL_END_OF_OUTPUT;
   }

   return status;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : write_assoc_rq
@INPUT      : afp - acr file pointer
              group - info to write
@OUTPUT     : (none)
@RETURNS    : status of output
@DESCRIPTION: Writes out an association request
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 10, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Status write_assoc_rq(Acr_File *afp, Acr_Group group)
{
   return write_assoc_rq_ac(afp, group, TRUE, NULL);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : write_assoc_ac
@INPUT      : afp - acr file pointer
              group - info to write
@OUTPUT     : (none)
@RETURNS    : status of output
@DESCRIPTION: Writes out an association accept
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 10, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Status write_assoc_ac(Acr_File *afp, Acr_Group group)
{
   return write_assoc_rq_ac(afp, group, FALSE, NULL);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : write_assoc_rq_ac
@INPUT      : afp - acr file pointer
              group - info to write
              is_request - TRUE if we should handle request type PDU, FALSE if
                 we should handle accept type PDU
@OUTPUT     : length - if non-NULL, then nothing is written and the value
                 is incremented by the amount of data written out.
@RETURNS    : status of output
@DESCRIPTION: Writes out an association request or accept PDU
@METHOD     : Can be used to write out a PDU or calculate its length. If
              length is non-NULL, then length calculation is done and nothing
              is written out. Otherwise the PDU is written out. This routine
              calls itself to work out the PDU length.
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 10, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Status write_assoc_rq_ac(Acr_File *afp, Acr_Group group, 
                                     int is_request, long *length)
{
   unsigned char buffer[ASSOC_RQ_LEN];
   long pdu_length;
   int do_write;
   int offset;
   unsigned short svalue;
   Acr_Element element;
   Acr_Status status;
   Acr_Element_Id elid;

   /* Should we write or calculate length */
   do_write = (length == NULL);

   /* Write out the first part of the PDU */
   if (do_write) {

      /* Zero the buffer */
      for (offset=0; offset<sizeof(buffer); offset++) {
         buffer[offset] = 0;
      }

      /* Put things in the appropriate places */
      buffer[0] = acr_find_short(group, DCM_PDU_Type, 0);
      pdu_length = 0;
      (void) write_assoc_rq_ac(afp, group, is_request, &pdu_length);
      pdu_length -= PDU_HEADER_LEN;
      PUT_LONG(&pdu_length, &buffer[2]);
      svalue = acr_find_short(group, DCM_PDU_Protocol_Version, 1);
      PUT_SHORT(&svalue, &buffer[6]);
      COPY_UID(group, DCM_PDU_Called_Ap_title, &buffer[10], 16);
      COPY_UID(group, DCM_PDU_Calling_Ap_title, &buffer[26], 16);

      /* Write out the first partr */
      status = acr_write_buffer(afp, buffer, sizeof(buffer), NULL);
      if (status != ACR_OK) return status;
   }

   /* Add on the length of the first part of the PDU */
   else {
      *length += sizeof(buffer);
   }

   /* Write out the application context */
   status = write_uid_item(afp, 
      acr_find_group_element(group, DCM_PDU_Application_context), 
                           PDU_ITEM_APPLICATION_CONTEXT, length);
   if (status != ACR_OK) return status;

   /* Write presentation context items */
   elid = (is_request ? DCM_PDU_Presentation_context_list :
           DCM_PDU_Presentation_context_reply_list);
   element = acr_find_group_element(group, elid);
   if ((element == NULL) || !acr_element_is_sequence(element)) {
      return ACR_PROTOCOL_ERROR;
   }
   element = (Acr_Element) acr_get_element_data(element);
   while (element != NULL) {
      status = write_pres_context_item(afp, element, is_request, length);
      if (status != ACR_OK) return status;
      element = acr_get_element_next(element);
   }

   /* Write the user info item */
   status = write_user_info_item(afp, group, length);

   return status;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : write_assoc_rj
@INPUT      : afp - acr file pointer
              group_list
@OUTPUT     : (none)
@RETURNS    : status of output
@DESCRIPTION: Writes out an associate rejection
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 10, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Status write_assoc_rj(Acr_File *afp, Acr_Group group_list)
{
   int result, source, reason;

   result = acr_find_short(group_list, DCM_PDU_Result, 1);
   source = acr_find_short(group_list, DCM_PDU_Source, 1);
   reason = acr_find_short(group_list, DCM_PDU_Reason, 1);

   return write_fixed_length_pdu(afp, ACR_PDU_ASSOC_RJ, 
                                 result, source, reason);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : write_rel_rq
@INPUT      : afp - acr file pointer
              group_list
@OUTPUT     : (none)
@RETURNS    : status of output
@DESCRIPTION: Writes out a release request 
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 10, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
/* ARGSUSED */
private Acr_Status write_rel_rq(Acr_File *afp, Acr_Group group_list)
{
   return write_fixed_length_pdu(afp, ACR_PDU_REL_RQ, 0, 0, 0);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : write_rel_rp
@INPUT      : afp - acr file pointer
              group_list
@OUTPUT     : (none)
@RETURNS    : status of output
@DESCRIPTION: Writes out a release reply
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 10, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
/* ARGSUSED */
private Acr_Status write_rel_rp(Acr_File *afp, Acr_Group group_list)
{
   return write_fixed_length_pdu(afp, ACR_PDU_REL_RP, 0, 0, 0);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : write_abort_rq
@INPUT      : afp - acr file pointer
              group_list
@OUTPUT     : (none)
@RETURNS    : status of output
@DESCRIPTION: Writes out an abort request 
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 10, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Status write_abort_rq(Acr_File *afp, Acr_Group group_list)
{
   int source, reason;

   source = acr_find_short(group_list, DCM_PDU_Source, 0);
   reason = acr_find_short(group_list, DCM_PDU_Reason, 0);

   return write_fixed_length_pdu(afp, ACR_PDU_ABORT_RQ, 0, source, reason);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : write_fixed_length_pdu
@INPUT      : afp - acr file pointer
              pdu_type
              result - value for result field
              source - value for source field
              reason - value for reason field
@OUTPUT     : (none)
@RETURNS    : status of output
@DESCRIPTION: Writes out a fixed length PDU (release request or reply 
              or an abort request or an associate reject)
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 10, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Status write_fixed_length_pdu(Acr_File *afp, int pdu_type, 
                                          int result, int source, int reason)
{
   unsigned char buffer[ABORT_RQ_LEN];
   long pdu_length;
   int i;

   /* Zero the buffer */
   for (i=0; i<sizeof(buffer); i++) {
      buffer[i] = 0;
   }

   /* Put things in the appropriate places */
   buffer[0] = (unsigned char) pdu_type;
   pdu_length = ABORT_RQ_LEN - PDU_HEADER_LEN;
   PUT_LONG(&pdu_length, &buffer[2]);
   buffer[7] = (unsigned char) result;
   buffer[8] = (unsigned char) source;
   buffer[9] = (unsigned char) reason;
   
   /* Write it out  */
   return acr_write_buffer(afp, buffer, sizeof(buffer), NULL);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : write_data_tf
@INPUT      : dicom_afp - acr file pointer for DICOM messages
              message - message to write out
@OUTPUT     : (none)
@RETURNS    : status of output
@DESCRIPTION: Writes out a data PDU
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 10, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Status write_data_tf(Acr_File *dicom_afp, Acr_Message message)
{
   Acr_Group pdu_group, command_group, data_group;
   Acr_Element element;
   Acr_Message temp_message;
   long pdu_length, command_length, data_length;
   Acr_VR_encoding_type vr_encoding;
   Acr_Status status;

   /* Look for the pdu group, the command group and the first data group */
   pdu_group = acr_get_message_group_list(message);
   if (acr_get_group_group(pdu_group) == DCM_PDU_GRPID) {
      command_group = acr_get_group_next(pdu_group);
   }
   else {
      command_group = pdu_group;
      pdu_group = NULL;
   }
   if (command_group == NULL) return ACR_PROTOCOL_ERROR;
   if (acr_get_group_group(command_group) == ACR_COMMAND_GRPID) {
      data_group = acr_get_group_next(command_group);
   }
   else {
      data_group = command_group;
      command_group = NULL;
   }

   /* Check that we have something to write */
   if ((command_group == NULL) && (data_group == NULL)) 
      return ACR_PROTOCOL_ERROR;

   /* Set the presentation context id for the stream */
   if ((pdu_group != NULL)) {
      element = acr_find_group_element(pdu_group, 
                                       DCM_PDU_Presentation_context_id);
      if (element != NULL) {
         acr_set_dicom_pres_context_id(dicom_afp, 
                                       (int) acr_get_element_short(element));
      }
   }

   /* Set up the virtual output stream for writing */
   vr_encoding = acr_get_vr_encoding(dicom_afp);
   if (pdu_group != NULL)
      pdu_length = acr_get_group_total_length(pdu_group, vr_encoding);
   else
      pdu_length = 0;
   if (command_group != NULL)
      command_length = acr_get_group_total_length(command_group, vr_encoding);
   else
      command_length = 0;
   if (data_group != NULL)
      data_length = acr_get_message_total_length(message, vr_encoding) - 
         command_length - pdu_length;
   else 
      data_length = 0;
   if ((command_length <= 0) && (data_length <= 0)) {
      return ACR_PROTOCOL_ERROR;
   }
   dicom_setup_output(dicom_afp, command_length, data_length);

   /* Create a temporary message without the PDU group */
   if (pdu_group != NULL) {
      temp_message = acr_create_message();
      acr_message_add_group_list(temp_message, ((command_group != NULL) ? 
                                                command_group : data_group));
   }
   else {
      temp_message = message;
   }

   /* Write out the message */
   status = acr_output_message(dicom_afp, temp_message);

   /* Delete the temporary message without touching the groups (which are
      still part of the original message */
   if (temp_message != message) {
      acr_message_reset(temp_message);
      acr_delete_message(temp_message);
   }

   return status;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : write_uid_item
@INPUT      : afp - acr file pointer
              element - element to write out
              item_type - code identifying item to be written out
@OUTPUT     : length - if non-NULL, then nothing is written and the value
                 is incremented by the amount of data written out.
@RETURNS    : status of output 
@DESCRIPTION: Writes out a uid item.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 12, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Status write_uid_item(Acr_File *afp, Acr_Element element, 
                                  int item_type, long *length)
{
   unsigned char buffer[PDU_ITEM_HEADER_LEN];
   long item_length;
   Acr_Status status;
   int uid_length;
   int do_write;
   char *uid;
   unsigned short svalue;

   /* Check element */
   if (element == NULL) return ACR_PROTOCOL_ERROR;

   /* Check whether we are writing or counting */
   do_write = (length == NULL);

   /* Get the uid pointer */
   uid = acr_get_element_string(element);
   if (uid == NULL) return ACR_PROTOCOL_ERROR;

   /* Write item header */
   if (do_write) {
      buffer[0] = (unsigned char) item_type;
      buffer[1] = 0;
      item_length = 0;
      (void) write_uid_item(afp, element, item_type, &item_length);
      item_length -= PDU_ITEM_HEADER_LEN;
      svalue = (unsigned short) item_length;
      PUT_SHORT(&svalue, &buffer[2]);
      status = acr_write_buffer(afp, buffer, sizeof(buffer), NULL);
      if (status != ACR_OK) return status;
   }
   else {
      *length += sizeof(buffer);
   }

   /* Get the uid length, skipping trailing blanks */
   uid_length = strlen(uid);
   while ((uid_length > 0) && isspace(uid[uid_length-1])) {uid_length--;}

   /* Write out the uid */
   if (do_write) {
      status = acr_write_buffer(afp, (unsigned char *) uid, uid_length, NULL);
      if (status != ACR_OK) return status;
   }
   else {
      *length += uid_length;
   }

#if 0
   /* Check that we wrote an even number of bytes */
   if ((uid_length % 2) != 0) {
      if (do_write) {
         (void) acr_putc((int) ' ', afp);
      }
      else {
         *length += 1;
      }
   }
#endif

   return ACR_OK;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : write_pres_context_item
@INPUT      : afp - acr file pointer
              item - information to write out
@OUTPUT     : length - if non-NULL, then routine computes length of output
@RETURNS    : status of output
@DESCRIPTION: Writes out a presentation context item (or reply item).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 12, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Status write_pres_context_item(Acr_File *afp, Acr_Element item,
                                           int is_request, long *length)
{
   unsigned char buffer[PDU_ITEM_HEADER_LEN+4];
   long item_length;
   Acr_Status status;
   int do_write;
   Acr_Element itemlist, subitem;
   unsigned short svalue;
   
   /* Check element */
   if ((item == NULL) || (!acr_element_is_sequence(item))) {
      return ACR_PROTOCOL_ERROR;
   }
   itemlist = (Acr_Element) acr_get_element_data(item);

   /* Check whether we are writing or counting */
   do_write = (length == NULL);

   /* Write item header */
   if (do_write) {
      buffer[0] = (is_request ? 
                   PDU_ITEM_PRESENTATION_CONTEXT :
                   PDU_ITEM_PRES_CONTEXT_REPLY);
      buffer[1] = 0;
      item_length = 0;
      (void) write_pres_context_item(afp, item, is_request, &item_length);
      item_length -= PDU_ITEM_HEADER_LEN;
      svalue = (unsigned short) item_length;
      PUT_SHORT(&svalue, &buffer[2]);
      subitem = acr_find_element_id(itemlist, DCM_PDU_Presentation_context_id);
      if (subitem == NULL) return ACR_PROTOCOL_ERROR;
      buffer[4] = (unsigned char) acr_get_element_short(subitem);
      buffer[5] = 0;
      if (!is_request) {
         subitem = acr_find_element_id(itemlist, DCM_PDU_Result);
         if (subitem == NULL) return ACR_PROTOCOL_ERROR;
         buffer[6] = (unsigned char) acr_get_element_short(subitem);
      }
      else
         buffer[6] = 0;
      buffer[7] = 0;
      status = acr_write_buffer(afp, buffer, sizeof(buffer), NULL);
      if (status != ACR_OK) return status;
   }
   else {
      *length += sizeof(buffer);
   }

   /* Write out the transfer syntaxes */
   if (is_request) {
      subitem = acr_find_element_id(itemlist, DCM_PDU_Abstract_syntax);
      if (subitem == NULL) return ACR_PROTOCOL_ERROR;
      status = write_uid_item(afp, subitem, PDU_ITEM_ABSTRACT_SYNTAX, length);
      if (status != ACR_OK) return status;
      subitem = itemlist;
      while (subitem != NULL) {
         if (acr_match_element_id(DCM_PDU_Transfer_syntax, subitem)) {
            status = write_uid_item(afp, subitem, PDU_ITEM_TRANSFER_SYNTAX, 
                                    length);
            if (status != ACR_OK) return status;
         }
         subitem = acr_get_element_next(subitem);
      }
   }
   else {
      subitem = acr_find_element_id(itemlist, DCM_PDU_Transfer_syntax);
      if (subitem == NULL) return ACR_PROTOCOL_ERROR;
      status = write_uid_item(afp, subitem, PDU_ITEM_TRANSFER_SYNTAX, length);
      if (status != ACR_OK) return status;
   }
       
   return ACR_OK;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : write_user_info_item
@INPUT      : afp - acr file pointer
              group - information to write out
@OUTPUT     : length - if non-NULL, then routine computes length of output
@RETURNS    : status of output
@DESCRIPTION: Writes out a user info item
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 12, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Status write_user_info_item(Acr_File *afp, Acr_Group group,
                                        long *length)
{
   unsigned char buffer[2*PDU_ITEM_HEADER_LEN+ACR_SIZEOF_LONG];
   long item_length, maximum_length;
   Acr_Status status;
   int do_write;
   unsigned short svalue;
   Acr_Element element;
   int item_type;
   
   /* Check whether we are writing or counting */
   do_write = (length == NULL);

   /* Write item header */
   if (do_write) {
      buffer[0] = PDU_ITEM_USER_INFORMATION;
      buffer[1] = 0;
      item_length = 0;
      (void) write_user_info_item(afp, group, &item_length);
      item_length -= PDU_ITEM_HEADER_LEN;
      svalue = (unsigned short) item_length;
      PUT_SHORT(&svalue, &buffer[2]);
      buffer[4] = PDU_ITEM_MAXIMUM_LENGTH;
      buffer[5] = 0;
      svalue = ACR_SIZEOF_LONG;
      PUT_SHORT(&svalue, &buffer[6]);
      maximum_length = acr_find_long(group, DCM_PDU_Maximum_length, 0);
      PUT_LONG(&maximum_length, &buffer[8]);
      status = acr_write_buffer(afp, buffer, sizeof(buffer), NULL);
      if (status != ACR_OK) return status;
   }
   else {
      *length += sizeof(buffer);
   }

   /* Write any unknown items with an item type > PDU_ITEM_USER_INFORMATION */
   if (acr_get_group_group(group) == DCM_PDU_GRPID) {

      /* Loop over all elements */
      for (element = acr_get_group_element_list(group);
           element != NULL;
           element = acr_get_element_next(element)) {

         /* Check that the element is unknown and that it is an user 
            info item */
         item_type = acr_get_element_element(element);
         if (item_type < ACR_UNKNOWN_PDU_ITEM_OFFSET) 
            continue;
         item_type -= ACR_UNKNOWN_PDU_ITEM_OFFSET;
         if (item_type < PDU_ITEM_USER_INFORMATION) 
            continue;

         /* Write out the value */
         status = write_unknown_item(afp, item_type, 
                                     acr_get_element_length(element),
                                     acr_get_element_data(element), length);
         if (status != ACR_OK) return status;
      }
   }

   return ACR_OK;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : write_unknown_item
@INPUT      : afp - acr file pointer
              item_type - code identifying item to be written out
              data_length - length of data to be written
              data_pointer - pointer to data
@OUTPUT     : length - if non-NULL, then nothing is written and the value
                 is incremented by the amount of data written out.
@RETURNS    : status of output 
@DESCRIPTION: Writes out an item of unknown type.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 12, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Status write_unknown_item(Acr_File *afp, int item_type,
                                      long data_length, char *data_pointer, 
                                      long *length)
{
   unsigned char buffer[PDU_ITEM_HEADER_LEN];
   long item_length;
   int do_write;
   Acr_Status status;
   unsigned short svalue;

   /* Check whether we are writing or counting */
   do_write = (length == NULL);

   /* Write item header */
   if (do_write) {
      buffer[0] = (unsigned char) item_type;
      buffer[1] = 0;
      item_length = 0;
      (void) write_unknown_item(afp, item_type, data_length, data_pointer, 
                                &item_length);
      item_length -= PDU_ITEM_HEADER_LEN;
      svalue = (unsigned short) item_length;
      PUT_SHORT(&svalue, &buffer[2]);
      status = acr_write_buffer(afp, buffer, sizeof(buffer), NULL);
      if (status != ACR_OK) return status;
   }
   else {
      *length += sizeof(buffer);
   }

   /* Write out the data */
   if (do_write) {
      status = acr_write_buffer(afp, (unsigned char *) data_pointer,
                                data_length, NULL);
      if (status != ACR_OK) return status;
   }
   else {
      *length += data_length;
   }

   return ACR_OK;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : pdu_copy_uid
@INPUT      : string
              length - maximum length of uid
@OUTPUT     : buffer
@RETURNS    : (nothing)
@DESCRIPTION: Copies a uid from string to buffer, padding with trailing blanks
              up to length.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 12, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void pdu_copy_uid(char *string, char *buffer, int length)
{
   int i;

   for (i=0; (i < length) && (string[i] != '\0'); i++) {
      buffer[i] = string[i];
   }
   for (; (i < length); i++) {
      buffer[i] = ' ';
   }
}

/*****************************************************************************/
/************************** DICOM I/O STREAM *********************************/
/*****************************************************************************/

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_initialize_dicom_input
@INPUT      : io_data - pointer to data for read and write routines
              maxlength - maximum length for a single read or write
                 (zero or negative means use internal maximum).
              io_routine - routine to read or write data
@OUTPUT     : (none)
@RETURNS    : pointer to Acr_File structure created
@DESCRIPTION: Sets up the routines for reading dicom messages from an input
              stream.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 18, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_File *acr_initialize_dicom_input(void *io_data,
                                            int maxlength,
                                            Acr_Io_Routine io_routine)
{
   return initialize_dicom_stream(io_data, maxlength, io_routine, 
                                  DICOM_INPUT);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_initialize_dicom_output
@INPUT      : io_data - pointer to data for read and write routines
              maxlength - maximum length for a single read or write
                 (zero or negative means use internal maximum).
              io_routine - routine to read or write data
@OUTPUT     : (none)
@RETURNS    : pointer to Acr_File structure created
@DESCRIPTION: Sets up the routines for writing dicom messages to an output
              stream.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 18, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_File *acr_initialize_dicom_output(void *io_data,
                                            int maxlength,
                                            Acr_Io_Routine io_routine)
{
   return initialize_dicom_stream(io_data, maxlength, io_routine, 
                                  DICOM_OUTPUT);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_close_dicom_file
@INPUT      : afp 
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Closes a dicom output stream.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 18, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_close_dicom_file(Acr_File *afp)
{
   Acr_Dicom_IO *stream_data;

   /* Get the structure pointer */
   stream_data = get_dicom_io_pointer(afp);

   /* Flush the virtual stream */
   (void) acr_file_flush(afp);

   /* Close the real stream */
   if (stream_data != NULL) {
      acr_file_free(stream_data->real_afp);
      FREE(stream_data);
   }

   /* Free the virtual stream */
   acr_file_free(afp);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_dicom_enable_trace / acr_dicom_disable_trace
@INPUT      : afp 
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Enables / disables trace on a dicom stream (on the real stream)
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 18, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_dicom_enable_trace(Acr_File *afp)
{
   Acr_Dicom_IO *stream_data;

   /* Get the structure pointer */
   stream_data = get_dicom_io_pointer(afp);
   acr_file_enable_trace((stream_data == NULL) ? 
                         afp : stream_data->real_afp);

}

public void acr_dicom_disable_trace(Acr_File *afp)
{
   Acr_Dicom_IO *stream_data;

   /* Get the structure pointer */
   stream_data = get_dicom_io_pointer(afp);
   acr_file_disable_trace((stream_data == NULL) ? 
                          afp : stream_data->real_afp);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_dicom_set_eof
@INPUT      : afp
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Tells the input or output stream that it has reached the end
              of file so that no more data will be read or written. This can
              be useful for preventing further reading or writing if an
              alarm goes off, for example.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 18, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_dicom_set_eof(Acr_File *afp)
{
   Acr_Dicom_IO *stream_data;

   /* Get the structure pointer */
   stream_data = get_dicom_io_pointer(afp);

   /* Set eof on both afp's */
   acr_file_set_eof(afp);
   acr_file_set_eof(stream_data->real_afp);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : initialize_dicom_stream
@INPUT      : io_data - pointer to data for read and write routines
              maxlength - maximum length for a single read or write
                 (zero or negative means use internal maximum).
              io_routine - routine to read or write data
              stream_type - DICOM_INPUT or DICOM_OUTPUT
@OUTPUT     : (none)
@RETURNS    : pointer to Acr_File structure created
@DESCRIPTION: Sets up the routines for dicom i/o
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 18, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_File *initialize_dicom_stream(void *io_data, int maxlength,
                                          Acr_Io_Routine io_routine,
                                          Dicom_IO_stream_type stream_type)
{
   Acr_File *real_afp, *virtual_afp;
   Acr_Dicom_IO *stream_data;
   Acr_Io_Routine virtual_io_routine;

   /* Create the real input stream */
   real_afp = acr_file_initialize(io_data, maxlength, io_routine);

   /* Create the virtual afp data */
   stream_data = MALLOC(sizeof(*stream_data));
   stream_data->stream_type = stream_type;
   stream_data->real_afp = real_afp;
   stream_data->presentation_context_id = 0;
   stream_data->pdu_watchpoint = ACR_NO_WATCHPOINT;
   stream_data->maximum_length = LONG_MAX;
   stream_data->writing_command = FALSE;
   stream_data->data_length = 0;

   /* Create the virtual input stream */
   if (stream_type == DICOM_INPUT) {
      virtual_io_routine = dicom_input_routine;
   }
   else {
      virtual_io_routine = dicom_output_routine;
   }
   virtual_afp = acr_file_initialize((void *) stream_data, 0,
                                     virtual_io_routine);
   stream_data->virtual_afp = virtual_afp;

   return virtual_afp;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_dicom_io_pointer
@INPUT      : afp
@OUTPUT     : (none)
@RETURNS    : pointer to Acr_Dicom_IO structure or NULL if an error occurs
@DESCRIPTION: Gets the pointer to the Acr_Dicom_IO structure contained within
              the afp.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 18, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Dicom_IO *get_dicom_io_pointer(Acr_File *afp)
{
   Acr_Dicom_IO *stream_data;

   stream_data = (Acr_Dicom_IO *) acr_file_get_io_data(afp);
   if ((stream_data == NULL) || 
       ((stream_data->stream_type != DICOM_INPUT) &&
        (stream_data->stream_type != DICOM_OUTPUT))) {
      return NULL;
   }
   return stream_data;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_dicom_real_afp
@INPUT      : afp
@OUTPUT     : (none)
@RETURNS    : pointer to Acr_File for real input or output
@DESCRIPTION: Gets the pointer to the Acr_File structure that is really 
              connected to a stdio input or output stream.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 18, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_File *get_dicom_real_afp(Acr_File *afp)
{
   Acr_Dicom_IO *stream_data;

   /* Get the structure pointer */
   stream_data = get_dicom_io_pointer(afp);
   if (stream_data == NULL) return NULL;

   /* Return the afp */
   return stream_data->real_afp;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_dicom_pdu_watchpoint
@INPUT      : afp
@OUTPUT     : (none)
@RETURNS    : Distance from current position to pdu watchpoint on real stream
@DESCRIPTION: Returns the distance to the PDU watchpoint on the real i/o 
              stream.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 18, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private long get_dicom_pdu_watchpoint(Acr_File *afp)
{
   Acr_Dicom_IO *stream_data;
   long current_watchpoint;

   stream_data = get_dicom_io_pointer(afp);
   if (stream_data == NULL) return ACR_NO_WATCHPOINT;

   current_watchpoint = acr_get_io_watchpoint(stream_data->real_afp);
   if ((stream_data->pdu_watchpoint == ACR_NO_WATCHPOINT) ||
       (current_watchpoint == ACR_NO_WATCHPOINT)) {
      return ACR_NO_WATCHPOINT;
   }
   else {
      return stream_data->pdu_watchpoint + current_watchpoint;
   }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_dicom_pdu_watchpoint
@INPUT      : afp
              pdu_watchpoint - distance to pdu watchpoint
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Sets the watchpoint for the PDU relative to the current position.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 18, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void set_dicom_pdu_watchpoint(Acr_File *afp, long pdu_watchpoint)
{
   Acr_Dicom_IO *stream_data;
   long current_watchpoint;

   stream_data = get_dicom_io_pointer(afp);
   if (stream_data == NULL) return;

   current_watchpoint = acr_get_io_watchpoint(stream_data->real_afp);
   if (current_watchpoint == ACR_NO_WATCHPOINT) {
      acr_set_io_watchpoint(stream_data->real_afp, pdu_watchpoint);
      stream_data->pdu_watchpoint = 0;
   }
   else {
      stream_data->pdu_watchpoint = pdu_watchpoint - current_watchpoint;
   }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_set_dicom_maximum_length
@INPUT      : afp
              maximum_length - if <= 0, then a very large value is used
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Sets the maximum PDU length for a dicom stream
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 18, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_set_dicom_maximum_length(Acr_File *afp, 
                                         long maximum_length)
{
   Acr_Dicom_IO *stream_data;

   /* Get the structure pointer */
   stream_data = get_dicom_io_pointer(afp);
   if (stream_data == NULL) return;

   /* Set the maximum length */
   if (maximum_length <= 0)
      maximum_length = LONG_MAX;
   stream_data->maximum_length = maximum_length;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_set_dicom_pres_context_id
@INPUT      : afp
              presentation_context_id - presentation context id to use
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Sets the presentation context id for an i/o stream.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 18, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_set_dicom_pres_context_id(Acr_File *afp, 
                                          int presentation_context_id)
{
   Acr_Dicom_IO *stream_data;

   /* Get the structure pointer */
   stream_data = get_dicom_io_pointer(afp);
   if (stream_data == NULL) return;

   /* Set the id */
   stream_data->presentation_context_id = presentation_context_id;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_dicom_pres_context_id
@INPUT      : afp
@OUTPUT     : (none)
@RETURNS    : presentation context id
@DESCRIPTION: Gets the presentation context id for an i/o stream.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 18, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int acr_get_dicom_pres_context_id(Acr_File *afp)
{
   Acr_Dicom_IO *stream_data;

   /* Get the structure pointer */
   stream_data = get_dicom_io_pointer(afp);
   if (stream_data == NULL) return 0;

   /* Set the id */
   return stream_data->presentation_context_id;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : dicom_reset
@INPUT      : afp
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Resets the dicom (virtual) stream
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 18, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void dicom_reset(Acr_File *afp)
{
   Acr_Dicom_IO *stream_data;

   /* Get the structure pointer */
   stream_data = get_dicom_io_pointer(afp);
   if (stream_data == NULL) return;

   /* Do the reset and mark this as the first time */
   acr_file_reset(afp);
   stream_data->pdu_watchpoint = ACR_NO_WATCHPOINT;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : dicom_setup_output
@INPUT      : afp
              command_length - length of command portion of message
              data_length - length of data portion of message
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Sets up the dicom virtual stream to handle data output
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 18, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void dicom_setup_output(Acr_File *afp, 
                                long command_length, long data_length)
{
   Acr_Dicom_IO *stream_data;

   /* Reset the stream */
   dicom_reset(afp);

   /* Get the structure pointer */
   stream_data = get_dicom_io_pointer(afp);
   if (stream_data == NULL) return;

   /* Set real afp watchpoint to zero to force a PDU header right away */
   stream_data->pdu_watchpoint = 0;
   acr_set_io_watchpoint(stream_data->real_afp, 0);

   /* Figure out whether we are starting with a command or data, save the
      data length and set the watchpoint so that we can figure out when 
      to change from command to data */
   if (command_length > 0) {
      stream_data->writing_command = TRUE;
      stream_data->data_length = data_length;
      acr_set_io_watchpoint(afp, command_length);
   }
   else {
      stream_data->writing_command = FALSE;
      stream_data->data_length = data_length;
      acr_set_io_watchpoint(afp, data_length);
   }
   
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : dicom_input_routine
@INPUT      : io_data - should be a FILE * pointer
              nbytes - number of bytes to read
@OUTPUT     : buffer - buffer into which we will read
@RETURNS    : Number of bytes read.
@DESCRIPTION: Dicom input routine for reading from a real input stream
              into a virtual input stream.
@METHOD     : We use three watchpoints. One on the real stream to mark the
              end of the PDV. One in stream data structure to mark the end
              of the PDU (this is relative to the end of the PDV watchpoint). 
              One is set on the virtual stream when we find the last fragment 
              of a message to mark the end of the message data.
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 18, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int dicom_input_routine(void *io_data, void *buffer, int nbytes)
{
   unsigned char header_buffer[DATA_TF_LEN];
   Acr_Dicom_IO *stream_data;
   long nread;
   int pdu_type;
   long pdu_length, pdv_length;
   long watchpoint_distance;
   Acr_Status status;
   int presentation_context_id, control_header;
   Acr_File *real_afp;

   /* Get file pointer */
   if (io_data == NULL) return 0;
   stream_data = (Acr_Dicom_IO *) io_data;
   real_afp = stream_data->real_afp;

   /* Get distance to next watchpoint on real stream */
   watchpoint_distance = acr_get_io_watchpoint(real_afp);

   /* Check whether we need to read in some more headers */
   if (watchpoint_distance <= 0) {

      /* Do we need to read in a PDU header? */
      if (stream_data->pdu_watchpoint <= 0) {
         acr_set_io_watchpoint(real_afp, 
                               PDU_HEADER_LEN + sizeof(header_buffer));
         status = read_pdu_header(real_afp, &pdu_type, &pdu_length);
         if ((status != ACR_OK) || (pdu_type != ACR_PDU_DATA_TF)) {
            return -1;
         }
         stream_data->pdu_watchpoint = pdu_length;
      }

      /* Read in the PDV header */
      acr_set_io_watchpoint(real_afp, sizeof(header_buffer));
      status = acr_read_buffer(real_afp, header_buffer, 
                               sizeof(header_buffer), NULL);
      if (status != ACR_OK) return -1;
      GET_LONG((void *) header_buffer, &pdv_length);
      pdv_length -= 2;
      presentation_context_id = (int) header_buffer[ACR_SIZEOF_LONG];
      control_header = (int) header_buffer[ACR_SIZEOF_LONG+1];

      /* Set the watchpoint for this PDV */
      acr_set_io_watchpoint(real_afp, pdv_length);
      watchpoint_distance = pdv_length;
      stream_data->pdu_watchpoint -= pdv_length + sizeof(header_buffer);
      if (stream_data->pdu_watchpoint < 0) return -1;

      /* Save the presentation context id */
      acr_set_dicom_pres_context_id(stream_data->virtual_afp, 
                                    presentation_context_id);

      /* Check whether this is the last fragment. If it is, set the
         watchpoint. */
      if (control_header & PDV_LAST_FRAGMENT_MASK) {
         acr_set_io_watchpoint(stream_data->virtual_afp, pdv_length);
      }
   }

   /* Make sure that we don't read too far */
   if (nbytes > watchpoint_distance) {
      nbytes = watchpoint_distance;
   }

   /* Read the data into the buffer */
   status = acr_read_buffer(real_afp, buffer, (long) nbytes, &nread);

   return (int) nread;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : dicom_output_routine
@INPUT      : io_data - should be a FILE * pointer
              nbytes - number of bytes to write
@OUTPUT     : buffer - buffer into which we will write
@RETURNS    : Number of bytes written.
@DESCRIPTION: Dicom output routine for writing a virtual output stream into
              a real output stream.
@METHOD     : We use three watchpoints. One one the real stream to mark the
              end of the next PDV. One stored in the stream data structure to
              indicate the end of the PDU (this is relative to the end of 
              the PDV watchpoint). One on the virtual stream to mark
              the end of the command or of the data.
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 18, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int dicom_output_routine(void *io_data, void *buffer, int nbytes)
{
   unsigned char pdu_buffer[PDU_HEADER_LEN];
   unsigned char header_buffer[DATA_TF_LEN];
   Acr_Dicom_IO *stream_data;
   Acr_File *real_afp, *virtual_afp;
   long pdu_length, pdv_length;
   long virtual_watchpoint, real_watchpoint, data_length;
   long nwritten;
   int total_written, bytes_to_write;
   Acr_Status status;
   int message_control_header;

   /* Get file pointer */
   if (io_data == NULL) return 0;
   stream_data = (Acr_Dicom_IO *) io_data;
   real_afp = stream_data->real_afp;
   virtual_afp = stream_data->virtual_afp;

   /* Get distances to watchpoints */
   real_watchpoint = acr_get_io_watchpoint(real_afp);
   virtual_watchpoint = acr_get_io_watchpoint(virtual_afp);

   /* Make sure that we have not gone too far */
   if (virtual_watchpoint < 0) {
      return -1;
   }

   /* Loop until we have written out the whole buffer */
   total_written = 0;
   while (nbytes > 0) {

      /* Check whether we need to write out more headers */
      if (real_watchpoint <= 0) {

         /* Do we need to write out a PDU header? */
         if (stream_data->pdu_watchpoint <= 0) {

            /* Figure out pdu length */
            pdu_length = 0;

            /* Get data length */
            data_length = (stream_data->writing_command ? 
                           stream_data->data_length : 
                           virtual_watchpoint + nbytes);

            /* Add in command length if needed */
            if (stream_data->writing_command) {
               pdu_length += sizeof(header_buffer) + 
                  virtual_watchpoint + nbytes;
            }

            /* Add in data length if needed, making sure that we can
               get some data into this pdu */
            if ((data_length > 0) && (pdu_length+sizeof(header_buffer) < 
                                      stream_data->maximum_length)) {
               pdu_length += sizeof(header_buffer) + data_length;
            }

            /* Check that we haven't exceeded the maximum length */
            if (pdu_length > stream_data->maximum_length) {
               pdu_length = stream_data->maximum_length;
            }

            /* Unset the watchpoint while we write the PDU header */
            acr_set_io_watchpoint(real_afp, ACR_NO_WATCHPOINT);

            /* Write out the buffer */
            pdu_buffer[0] = ACR_PDU_DATA_TF;
            pdu_buffer[1] = 0;
            PUT_LONG(&pdu_length, &pdu_buffer[2]);
            status = acr_write_buffer(real_afp, pdu_buffer, 
                                      sizeof(pdu_buffer), NULL);
            if (status != ACR_OK) return total_written;

            /* Keep track of the pdu watchpoint */
            stream_data->pdu_watchpoint = pdu_length;

         }      /* If pdu_watchpoint <= 0 */

         /* Get info for PDV header */
         message_control_header = 0;
         if (stream_data->writing_command) {
            message_control_header |= PDV_COMMAND_PDV_MASK;
         }

         /* Get length of PDV, making sure that it will fit in the PDU */
         pdv_length = virtual_watchpoint + nbytes;
         if ((pdv_length + sizeof(header_buffer)) > 
             stream_data->pdu_watchpoint) {
            pdv_length = stream_data->pdu_watchpoint - sizeof(header_buffer);
         }
         else if (!stream_data->writing_command || 
                  (stream_data->data_length <= 0)) {
            message_control_header |= PDV_LAST_FRAGMENT_MASK;
         }
         pdv_length += 2;

         /* Write out PDV header */
         PUT_LONG(&pdv_length, &header_buffer[0]);
         header_buffer[4] = 
            (unsigned char) stream_data->presentation_context_id;
         header_buffer[5] = (unsigned char) message_control_header;
         status = acr_write_buffer(real_afp, header_buffer, 
                                   sizeof(header_buffer), NULL);
         if (status != ACR_OK) return total_written;

         /* Set the watchpoint for this PDV */
         pdv_length -= 2;
         acr_set_io_watchpoint(real_afp, pdv_length);
         real_watchpoint = pdv_length;
         stream_data->pdu_watchpoint -= pdv_length + sizeof(header_buffer);
         if (stream_data->pdu_watchpoint < 0) return total_written;

         /* Check for a mistake */
         if (real_watchpoint <= 0) return total_written;

      }         /* If real_watchpoint <= 0 */

      /* Write out data */
      bytes_to_write = nbytes;
      if (nbytes >= real_watchpoint) {
         bytes_to_write = real_watchpoint;
      }
      status = acr_write_buffer(real_afp, buffer, bytes_to_write, &nwritten);
      total_written += nwritten;
      if (nwritten < bytes_to_write) return total_written;
      nbytes -= bytes_to_write;
      buffer = (void *) ((char *) buffer + bytes_to_write);
      real_watchpoint -= bytes_to_write;

   }           /* While nbytes > 0 */

   /* Check for end of command part */
   if (stream_data->writing_command && (virtual_watchpoint == 0)) {
      acr_set_io_watchpoint(virtual_afp, stream_data->data_length);
      stream_data->writing_command = FALSE;
   }

   return total_written;
}

