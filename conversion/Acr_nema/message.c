/* ----------------------------- MNI Header -----------------------------------
@NAME       : message.c
@DESCRIPTION: Routines for doing acr_nema message operations.
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 16, 1993 (Peter Neelin)
@MODIFIED   : 
 * $Log: message.c,v $
 * Revision 6.7  2004-10-29 13:08:42  rotor
 *  * rewrote Makefile with no dependency on a minc distribution
 *  * removed all references to the abominable minc_def.h
 *  * I should autoconf this really, but this is old code that
 *      is now replaced by Jon Harlaps PERL version..
 *
 * Revision 6.6  2002/12/08 21:43:08  neelin
 * Fixed excessive memory freeing on error when reading message (seen in linux)
 *
 * Revision 6.5  2002/11/13 03:00:27  neelin
 * Fixed an unterminated comment
 *
 * Revision 6.4  1999/10/29 17:51:53  neelin
 * Fixed Log keyword
 *
 * Revision 6.3  1998/03/10 17:06:29  neelin
 * Added code to acr_input_message so that if we reach the watchpoint and
 * more message is expected, we keep on reading.
 * 
 * Revision 6.2  1998/03/09  19:30:23  neelin
 * Fixed bug in acr_input_message where the last group added to the input
 * message was being deleted followed by the message itself when a
 * message length error occurred. When an input error occurs the message
 * should not be deleted.
 *
 * Revision 6.1  1998/02/18  20:27:13  neelin
 * Minor bug fix.
 *
 * Revision 6.0  1997/09/12  13:23:59  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:25:00  neelin
 * Release of minc version 0.5
 *
 * Revision 4.1  1997/06/13  21:27:16  neelin
 * Made use of message length to figure out how much to read in -
 * previously was not checking it, so if watchpoint was not set, we would
 * read indefinitely.
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
 * Revision 2.0  1994/09/28  10:36:17  neelin
 * Release of minc version 0.2
 *
 * Revision 1.6  94/09/28  10:35:49  neelin
 * Pre-release
 * 
 * Revision 1.5  94/05/18  08:48:12  neelin
 * Changed some ACR_OTHER_ERROR's to ACR_ABNORMAL_END_OF_OUTPUT.
 * 
 * Revision 1.4  94/04/07  10:05:06  neelin
 * Added status ACR_ABNORMAL_END_OF_INPUT and changed some ACR_PROTOCOL_ERRORs
 * to that or ACR_OTHER_ERROR.
 * Added #ifdef lint to DEFINE_ELEMENT.
 * 
 * Revision 1.3  93/11/24  11:25:59  neelin
 * Added dump_message.
 * 
 * Revision 1.2  93/11/22  13:12:09  neelin
 * Changed to use new Acr_Element_Id stuff
 * 
 * Revision 1.1  93/11/19  12:49:09  neelin
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

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <acr_nema.h>

/* Message length group and element id */
#define ACR_GID_MESSLEN 0
#define ACR_EID_MESSLEN 1

DEFINE_ELEMENT(static, ACR_Message_length, 
               ACR_GID_MESSLEN, ACR_EID_MESSLEN, UL);

/* Private functions */
private void update_message_length_element(Acr_Message message, 
                                           Acr_VR_encoding_type vr_encoding);

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_create_message
@INPUT      : (none)
@OUTPUT     : (none)
@RETURNS    : Pointer to message structure
@DESCRIPTION: Creates an acr-nema message structure.
@METHOD     : No groups are created here. Message length is checked when
              groups are added.
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 16, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Message acr_create_message(void)
{
   Acr_Message message;

   /* Allocate the message */
   message = MALLOC(sizeof(*message));

   /* Assign fields */
   acr_message_reset(message);

   return message;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_delete_message
@INPUT      : message
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Deletes an acr-nema message structure (freeing the group list)
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 16, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_delete_message(Acr_Message message)
{
   if (message->list_head != NULL) {
      acr_delete_group_list(message->list_head);
   }

   FREE(message);

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_message_reset
@INPUT      : message
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Resets the message to empty without freeing any existing 
              group list. Thus the group list can be used elsewhere 
              without being deleted when the message is deleted.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 18, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_message_reset(Acr_Message message)
{
   message->ngroups = 0;
   message->implicit_total_length = 0;
   message->explicit_total_length = 0;
   message->message_implicit_offset = 0;
   message->message_explicit_offset = 0;
   message->message_length_element = NULL;
   message->list_head = NULL;
   message->list_tail = NULL;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_message_add_group
@INPUT      : message
              group
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Add a group to an acr-nema message
@METHOD     : If this is the first group, then look for a message length
              element. If none is found, then report an error.
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 16, 1993 (Peter Neelin)
@MODIFIED   : February 7, 1997 (P.N.)
---------------------------------------------------------------------------- */
public void acr_message_add_group(Acr_Message message, Acr_Group group)
{
   Acr_Element length_element;
   int element_id;
   long message_implicit_offset, message_explicit_offset;

   /* If first message, then check for message length element */
   if (message->ngroups == 0) {
      length_element = NULL;
      message_implicit_offset = 0;
      message_explicit_offset = 0;
      if (acr_get_group_group(group) == ACR_GID_MESSLEN) {
         length_element = acr_get_group_element_list(group);
         if (length_element != NULL) {
            element_id = acr_get_element_element(length_element);
            message_implicit_offset = 
               acr_get_element_total_length(length_element, ACR_IMPLICIT_VR);
            message_explicit_offset = 
               acr_get_element_total_length(length_element, ACR_EXPLICIT_VR);
         }
         else {
            element_id = 0;
            message_implicit_offset = 0;
            message_explicit_offset = 0;
         }
         while ((element_id != ACR_EID_MESSLEN) &&
                (length_element != NULL)) {
            length_element = acr_get_element_next(length_element);
            if (length_element != NULL) {
               element_id = acr_get_element_element(length_element);
               message_implicit_offset += 
                  acr_get_element_total_length(length_element, 
                                               ACR_IMPLICIT_VR);
               message_explicit_offset += 
                  acr_get_element_total_length(length_element,
                                               ACR_EXPLICIT_VR);
            }
         }
      }

      /* Check for length element found but not of correct length */
      if ((length_element != NULL) &&
          (acr_get_element_length(length_element) != ACR_SIZEOF_LONG)) {
         (void) fprintf(stderr, 
    "ACR error: First message group contains length element of wrong size\n");
         exit(EXIT_FAILURE);
      }

      /* Set up the message length info */
      message->message_implicit_offset = message_implicit_offset;
      message->message_explicit_offset = message_explicit_offset;
      message->message_length_element = length_element;

   }

   /* Add group (check for empty list) */
   if (message->ngroups == 0) {
      message->list_head = group;
      message->list_tail = group;
   }
   else {
      acr_set_group_next(message->list_tail, group);
      message->list_tail = group;
      acr_set_group_next(group, NULL);
   }
   message->ngroups++;
   message->implicit_total_length += 
      acr_get_group_total_length(group, ACR_IMPLICIT_VR);
   message->explicit_total_length += 
      acr_get_group_total_length(group, ACR_EXPLICIT_VR);
      
   /* Update the length element */
   if (message->message_length_element != NULL) {
      update_message_length_element(message,
         acr_get_element_vr_encoding(message->message_length_element));
   }

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : update_message_length_element
@INPUT      : message
              vr_encoding - ACR_IMPLICIT_VR or ACR_EXPLICIT_VR
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Update the length element of the message according to the VR type
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 14, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void update_message_length_element(Acr_Message message, 
                                           Acr_VR_encoding_type vr_encoding)
{
   long message_length;
   Acr_Element length_element;
   void *message_length_data;

   /* Get the element */
   length_element = message->message_length_element;
   if (length_element == NULL) return;

   /* Calculate the appropriate length */
   if (vr_encoding == ACR_IMPLICIT_VR) {
      message_length = message->implicit_total_length - 
         message->message_implicit_offset;
   }
   else {
      message_length = message->explicit_total_length -
         message->message_explicit_offset;
   }

   /* Update the element */
   message_length_data = acr_get_element_data(length_element);
   acr_put_long(acr_get_element_byte_order(length_element),
                1, &message_length, message_length_data);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_message_add_group_list
@INPUT      : message
              group_list
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Add a group list to an acr-nema message
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 12, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_message_add_group_list(Acr_Message message, 
                                       Acr_Group group_list)
{
   Acr_Group next_group, group;

   /* Loop through groups, adding them to the message */
   group = group_list;
   while (group != NULL) {
      next_group = acr_get_group_next(group);
      acr_set_group_next(group, NULL);
      acr_message_add_group(message, group);
      group = next_group;
   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_message_group_list
@INPUT      : message
@OUTPUT     : (none)
@RETURNS    : group list
@DESCRIPTION: Get group list for message
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 16, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Group acr_get_message_group_list(Acr_Message message)
{
   return message->list_head;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_message_total_length
@INPUT      : message
              vr_encoding - ACR_EXPLICIT_VR or ACR_IMPLICIT_VR
@OUTPUT     : (none)
@RETURNS    : total length of message
@DESCRIPTION: Get total length of message depending on type of VR encoding
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 16, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public long acr_get_message_total_length(Acr_Message message,
                                         Acr_VR_encoding_type vr_encoding)
{
   if (vr_encoding == ACR_IMPLICIT_VR) 
      return message->implicit_total_length;
   else
      return message->explicit_total_length;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_message_ngroups
@INPUT      : message
@OUTPUT     : (none)
@RETURNS    : number of groups in message
@DESCRIPTION: Get number of groups in message
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 16, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int acr_get_message_ngroups(Acr_Message message)
{
   return message->ngroups;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_input_message
@INPUT      : afp - acr file pointer
@OUTPUT     : message
@RETURNS    : status
@DESCRIPTION: Read in an acr-nema message. The amount of input to read is
              determined either by setting a watchpoint or by having messages
              that contain their own length. One cannot rely on finding an
              EOF (as for a file) since messages usually come over a 
              connection.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 16, 1993 (Peter Neelin)
@MODIFIED   : February 7, 1997 (P.N.)
---------------------------------------------------------------------------- */
public Acr_Status acr_input_message(Acr_File *afp, Acr_Message *message)
{
   Acr_Status status;
   Acr_Group group;
   Acr_Element length_element;
   long message_length;
   long group_length;
   long watchpoint;
   int get_more_groups;

   /* Initialize the message pointer */
   *message = NULL;

   /* Read in the first group */
   status = acr_input_group(afp, &group);

   /* Check status */
   if (status != ACR_OK) {
      if (group != NULL) acr_delete_group(group);
      return status;
   }

   /* Check whether it contains the message length. If there is no 
      watchpoint, then it must contain the length, otherwise we don't
      know when to stop reading (we would if the input was from a file, but
      messages don't usually come from a file) */
   length_element = 
      acr_find_group_element(group, ACR_Message_length);
   if ((acr_get_io_watchpoint(afp) == ACR_NO_WATCHPOINT) &&
       ((length_element == NULL) ||
        (acr_get_element_length(length_element) != ACR_SIZEOF_LONG))) {
      acr_delete_group(group);
      status = ACR_PROTOCOL_ERROR;
      return status;
   }

   /* Get the message length from the element */
   if (length_element != NULL) {
      acr_get_long(acr_get_element_byte_order(length_element), (long) 1, 
                   acr_get_element_data(length_element), &message_length);
   }

   /* Create the message and add the group (this will modify the message 
      length element value to include only the first group) */
   *message = acr_create_message();
   acr_message_add_group(*message, group);

   /* Correct message_length for the length of the first group. Note
      that adding the group had the side effect of changing the 
      message length to include only the first group */
   if (length_element != NULL) {
      acr_get_long(acr_get_element_byte_order(length_element), (long) 1, 
                   acr_get_element_data(length_element), &group_length);
      message_length -= group_length;
   }

   /* Loop through elements, adding them to the list */
   get_more_groups = ((length_element != NULL) ? (message_length > 0) : TRUE);
   while (get_more_groups) {

      /* Check for a watchpoint. If we have reached it, but the message
         length indicates that there is more to come, then move it along
         so that we can keep reading. */
      watchpoint = acr_get_io_watchpoint(afp);
      if ((watchpoint == 0) && 
          (length_element != NULL) && (message_length > 0)) {
         acr_set_io_watchpoint(afp, LONG_MAX-1);
      }
      else if (watchpoint <= 0) {
         get_more_groups = FALSE;
         break;
      }

      /* If we reach the end of a fragment and PDU, but we need more data */

      /* Read in the next group and check for an error */
      status = acr_input_group(afp, &group);

      /* Add the group to the message */
      if (group != NULL) {
         acr_message_add_group(*message, group);
      }

      /* Check the status */
      if (status != ACR_OK) {
         if (status == ACR_END_OF_INPUT) status = ACR_ABNORMAL_END_OF_INPUT;
         return status;
      }

      /* Keep track of remaining bytes to read, if necessary */
      if (length_element != NULL) {
         message_length -= 
            acr_get_group_total_length(group, acr_get_vr_encoding(afp));
         get_more_groups = (message_length > 0);
      }
   }

   /* Check that we got a full message */
   if ((length_element != NULL) && (message_length != 0)) {
      status = ACR_PROTOCOL_ERROR;
      return status;
   }

   return status;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_output_message
@INPUT      : afp - acr file pointer
              message
@OUTPUT     : (none)
@RETURNS    : status
@DESCRIPTION: Write out an acr-nema message
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 16, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Status acr_output_message(Acr_File *afp, Acr_Message message)
{
   long igroup, ngroups;
   Acr_Group cur, next;
   Acr_Status status;

   /* Update the length element */
   update_message_length_element(message, acr_get_vr_encoding(afp));

   /* Loop through the groups of the message, writing them out */
   ngroups = acr_get_message_ngroups(message);
   next = acr_get_message_group_list(message);
   for (igroup=0; igroup < ngroups && next != NULL; igroup++) {
      cur = next;
      next = cur->next;
      status = acr_output_group(afp, cur);
      if (status != ACR_OK) {
         return status;
      }
   }

   /* Flush the buffer */
   if (acr_file_flush(afp) == EOF) {
      status = ACR_ABNORMAL_END_OF_OUTPUT;
      return status;
   }

   /* Check for a bogus message (the true number of groups is different from
      ngroups) */
   if ((igroup < ngroups) || (next != NULL)) {
      status = ACR_OTHER_ERROR;
      return status;
   }

   return status;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_dump_message
@INPUT      : file_pointer - where output should go
              group_list
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Dump information from an acr-nema message
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 24, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_dump_message(FILE *file_pointer, Acr_Message message)
{

   acr_dump_group_list(file_pointer, acr_get_message_group_list(message));

   return;
}
