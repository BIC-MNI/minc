/* ----------------------------- MNI Header -----------------------------------
@NAME       : group.c
@DESCRIPTION: Routines for doing acr_nema group operations.
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : $Log: group.c,v $
@MODIFIED   : Revision 1.1  1993-11-19 12:48:52  neelin
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
@NAME       : acr_create_group
@INPUT      : group_id 
@OUTPUT     : (none)
@RETURNS    : Pointer to group structure
@DESCRIPTION: Creates an acr-nema group structure
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Group acr_create_group(int group_id)
{
   Acr_Group group;
   Acr_Element length_element;
   void *group_length_data;
   long group_length = 0;

   /* Allocate the group */
   group = MALLOC(sizeof(*group));

   /* Create a length element */
   group_length_data = MALLOC(ACR_SIZEOF_LONG);
   acr_put_long(1, &group_length, group_length_data);
   length_element = 
      acr_create_element(group_id, ACR_EID_GRPLEN, 
                         (long) ACR_SIZEOF_LONG,
                         (char *) group_length_data);

   /* Assign fields */
   group->group_id = group_id;
   group->nelements = 1;
   group->total_length = acr_get_element_total_length(length_element);
   group->group_length_offset = group->total_length;
   group->list_head = length_element;
   group->list_tail = length_element;
   group->next = NULL;

   return group;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_delete_group
@INPUT      : group
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Deletes an acr-nema group structure (freeing the element list)
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_delete_group(Acr_Group group)
{
   acr_delete_element_list(group->list_head);

   FREE(group);

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_delete_group_list
@INPUT      : group_list
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Delete a list of acr-nema group
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_delete_group_list(Acr_Group group_list)
{
   Acr_Group next, cur;

   /* Loop through the list, deleting groups */
   next = group_list;
   do {
      cur = next;
      next = cur->next;
      acr_delete_group(cur);
   } while (next != NULL);

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_group_add_element
@INPUT      : group
              element
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Add an element to an acr-nema group
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_group_add_element(Acr_Group group, Acr_Element element)
{
   Acr_Element length_element;
   long group_length, element_length;
   void *group_length_data;

   /* Check that the element belongs in this group */
   if (group->group_id != acr_get_element_group(element)) {
      (void) fprintf(stderr, 
          "ACR error: Cannot add element %d (group %d) to group %d\n",
                     acr_get_element_element(element),
                     acr_get_element_group(element),
                     group->group_id);
      exit(EXIT_FAILURE);
   }

   /* Update the group structure */
   acr_set_element_next(group->list_tail, element);
   group->list_tail = element;
   group->nelements++;
   element_length = acr_get_element_total_length(element);
   group->total_length += element_length;

   /* Update the length element */
   length_element = group->list_head;
   group_length = group->total_length - group->group_length_offset;
   group_length_data = acr_get_element_data(length_element);
   acr_put_long(1, &group_length, group_length_data);

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_set_group_next
@INPUT      : group
              next
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Set pointer to next group for an acr-nema group
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_set_group_next(Acr_Group group, Acr_Group next)
{
   group->next = next;
   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_group_group
@INPUT      : group
@OUTPUT     : (none)
@RETURNS    : group id
@DESCRIPTION: Get group id for group
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int acr_get_group_group(Acr_Group group)
{
   return group->group_id;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_group_element_list
@INPUT      : group
@OUTPUT     : (none)
@RETURNS    : element list
@DESCRIPTION: Get element list for group
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Element acr_get_group_element_list(Acr_Group group)
{
   return group->list_head;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_group_total_length
@INPUT      : group
@OUTPUT     : (none)
@RETURNS    : total length of group
@DESCRIPTION: Get total length of group
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public long acr_get_group_total_length(Acr_Group group)
{
   return group->total_length;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_group_nelements
@INPUT      : group
@OUTPUT     : (none)
@RETURNS    : number of elements in group
@DESCRIPTION: Get number of elements in group
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int acr_get_group_nelements(Acr_Group group)
{
   return group->nelements;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_group_next
@INPUT      : group
@OUTPUT     : (none)
@RETURNS    : next group
@DESCRIPTION: Get next group for group
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Group acr_get_group_next(Acr_Group group)
{
   return group->next;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_input_group
@INPUT      : afp - acr file pointer
@OUTPUT     : group
@RETURNS    : status
@DESCRIPTION: Read in an acr-nema group
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Status acr_input_group(Acr_File *afp, Acr_Group *group)
{
   int group_id, element_id;
   long data_length;
   char *data_pointer;
   long group_length;
   Acr_Status status;
   Acr_Element element;

   /* Initialize the group pointer */
   *group = NULL;

   /* Read the length element and check it */
   status = acr_read_one_element(afp, &group_id, &element_id,
                                 &data_length, &data_pointer);
   if (status != ACR_OK) return status;
   if ((element_id != ACR_EID_GRPLEN) || 
       (data_length != ACR_SIZEOF_LONG)) {
      FREE(data_pointer);
      status = ACR_PROTOCOL_ERROR;
      return status;
   }

   /* Get the group length from the element */
   acr_get_long(1, data_pointer, &group_length);
   FREE(data_pointer);

   /* Create the group */
   *group = acr_create_group(group_id);

   /* Loop through elements, adding them to the list */
   while (group_length > 0) {
      status = acr_input_element(afp, &element);
      if (status != ACR_OK) {
         acr_delete_group(*group);
         if (status == ACR_END_OF_INPUT) status = ACR_PROTOCOL_ERROR;
         return status;
      }
      acr_group_add_element(*group, element);
      group_length -= acr_get_element_total_length(element);
   }

   /* Check that we got a full group */
   if (group_length != 0) {
      acr_delete_group(*group);
      status = ACR_PROTOCOL_ERROR;
      return status;
   }

   return status;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_output_group
@INPUT      : afp - acr file pointer
              group
@OUTPUT     : (none)
@RETURNS    : status
@DESCRIPTION: Write out an acr-nema group
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Status acr_output_group(Acr_File *afp, Acr_Group group)
{
   long ielement, nelements;
   Acr_Element cur, next;
   Acr_Status status;

   /* Loop through the elements of the group, writing them out */
   nelements = acr_get_group_nelements(group);
   next = acr_get_group_element_list(group);
   for (ielement=0; ielement < nelements && next != NULL; ielement++) {
      cur = next;
      next = cur->next;
      status = acr_output_element(afp, cur);
      if (status != ACR_OK) {
         return status;
      }
   }

   /* Check for a bogus group (the true number of elements is different from
      nelements) */
   if ((ielement < nelements) || (next != NULL)) {
      status = ACR_PROTOCOL_ERROR;
      return status;
   }

   return status;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_find_element_in_group_list
@INPUT      : group_list
              group_id
              element_id
@OUTPUT     : (none)
@RETURNS    : element pointer
@DESCRIPTION: Find an element in a group list
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Element acr_find_group_element(Acr_Group group_list,
                                          int group_id, int element_id)
{
   Acr_Group group;
   Acr_Element element;
   int next_id;

   /* Search through groups for group id */
   group = group_list;
   if (group != NULL)
      next_id = acr_get_group_group(group);
   else 
      next_id = 0;
   while ((next_id != group_id) && (group != NULL)) {
      group = acr_get_group_next(group);
      if (group != NULL)
         next_id = acr_get_group_group(group);
   }

   /* If not found return NULL */
   if (group == NULL) return NULL;

   /* Search through element list for element */
   element = acr_get_group_element_list(group);
   next_id = acr_get_element_element(element);
   while ((next_id != element_id) && (element != NULL)) {
      element = acr_get_element_next(element);
      if (element != NULL)
         next_id = acr_get_element_element(element);
   }

   /* If not found return NULL */
   if (element == NULL) return NULL;

   /* Check for invalid group id in element */
   if (acr_get_element_group(element) != group_id) {
      (void) fprintf(stderr,
         "ACR error: Found element %d (group %d) in group %d\n",
                     acr_get_element_element(element),
                     acr_get_element_group(element),
                     group_id);
      exit(EXIT_FAILURE);
   }

   return element;
}

