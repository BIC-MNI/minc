/* ----------------------------- MNI Header -----------------------------------
@NAME       : message.c
@DESCRIPTION: Routines for doing acr_nema message operations.
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 16, 1993 (Peter Neelin)
@MODIFIED   : $Log: message.c,v $
@MODIFIED   : Revision 1.1  1993-11-19 12:49:09  neelin
@MODIFIED   : Initial revision
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

#include <stdlib.h>
#include <stdio.h>
#include <minc_def.h>
#include <acr_nema.h>

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
   message->ngroups = 0;
   message->total_length = 0;
   message->message_length_offset = 0;
   message->list_head = NULL;
   message->list_tail = NULL;

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
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_message_add_group(Acr_Message message, Acr_Group group)
{
   Acr_Element length_element;
   int element_id;
   long message_length, message_offset;
   void *message_length_data;

   /* If first message, then check for message length element */
   if (message->ngroups == 0) {
      length_element = NULL;
      if (acr_get_group_group(group) == ACR_GID_MESSLEN) {
         length_element = acr_get_group_element_list(group);
         if (length_element != NULL) {
            element_id = acr_get_element_element(length_element);
            message_offset = acr_get_element_total_length(length_element);
         }
         else {
            element_id = 0;
            message_offset = 0;
         }
         while ((element_id != ACR_EID_MESSLEN) &&
                (length_element != NULL)) {
            length_element = acr_get_element_next(length_element);
            if (length_element != NULL) {
               element_id = acr_get_element_element(length_element);
               message_offset += acr_get_element_total_length(length_element);
            }
         }
      }

      /* Check for length element not found and that it had the correct
         length */
      if ((length_element == NULL) ||
          (acr_get_element_length(length_element) != ACR_SIZEOF_LONG)) {
         (void) fprintf(stderr, 
    "ACR error: First group added to message must contain message length\n");
         exit(EXIT_FAILURE);
      }

      /* Set up the message length info */
      message->message_length_offset = message_offset;
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
   }
   message->ngroups++;
   message->total_length += acr_get_group_total_length(group);
      
   /* Update the length element */
   length_element = message->message_length_element;
   message_length = message->total_length - message->message_length_offset;
   message_length_data = acr_get_element_data(length_element);
   acr_put_long(1, &message_length, message_length_data);

   return;
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
@OUTPUT     : (none)
@RETURNS    : total length of message
@DESCRIPTION: Get total length of message
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 16, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public long acr_get_message_total_length(Acr_Message message)
{
   return message->total_length;
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
@DESCRIPTION: Read in an acr-nema message
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 16, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Status acr_input_message(Acr_File *afp, Acr_Message *message)
{
   Acr_Status status;
   Acr_Group group;
   Acr_Element length_element;
   long message_length;
   long group_length;

   /* Initialize the message pointer */
   *message = NULL;

   /* Read in the first group */
   status = acr_input_group(afp, &group);

   /* Check status */
   if (status != ACR_OK) return status;

   /* Check that it contains the message length */
   length_element = 
      acr_find_group_element(group, ACR_GID_MESSLEN, ACR_EID_MESSLEN);

   if ((length_element == NULL) ||
       (acr_get_element_length(length_element) != ACR_SIZEOF_LONG)) {
      acr_delete_group(group);
      status = ACR_PROTOCOL_ERROR;
      return status;
   }

   /* Get the message length from the element */
   acr_get_long(1, acr_get_element_data(length_element), &message_length);

   /* Create the message and add the group (this will modify the message 
      length element value to include only the first group) */
   *message = acr_create_message();
   acr_message_add_group(*message, group);
   acr_get_long(1, acr_get_element_data(length_element), &group_length);
   message_length -= group_length;

   /* Loop through elements, adding them to the list */
   while (message_length > 0) {
      status = acr_input_group(afp, &group);
      if (status != ACR_OK) {
         acr_delete_message(*message);
         if (status == ACR_END_OF_INPUT) status = ACR_PROTOCOL_ERROR;
         return status;
      }
      acr_message_add_group(*message, group);
      message_length -= acr_get_group_total_length(group);
   }

   /* Check that we got a full message */
   if (message_length != 0) {
      acr_delete_message(*message);
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
      status = ACR_OTHER_ERROR;
      return status;
   }

   /* Check for a bogus message (the true number of groups is different from
      ngroups) */
   if ((igroup < ngroups) || (next != NULL)) {
      status = ACR_PROTOCOL_ERROR;
      return status;
   }

   return status;

}

