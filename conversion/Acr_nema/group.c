/* ----------------------------- MNI Header -----------------------------------
@NAME       : group.c
@DESCRIPTION: Routines for doing acr_nema group operations.
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : $Log: group.c,v $
@MODIFIED   : Revision 3.1  1997-04-21 20:21:09  neelin
@MODIFIED   : Updated the library to handle dicom messages.
@MODIFIED   :
 * Revision 3.0  1995/05/15  19:32:12  neelin
 * Release of minc version 0.3
 *
 * Revision 2.2  1995/02/08  21:16:06  neelin
 * Changes to make irix 5 lint happy.
 *
 * Revision 2.1  1995/01/04  08:10:16  neelin
 * Improved string printing in dump function (longer strings and replace
 * carriage returns, linefeeds and formfeeds by spaces).
 *
 * Revision 2.0  94/09/28  10:36:16  neelin
 * Release of minc version 0.2
 * 
 * Revision 1.11  94/09/28  10:35:45  neelin
 * Pre-release
 * 
 * Revision 1.10  94/04/07  10:05:04  neelin
 * Added status ACR_ABNORMAL_END_OF_INPUT and changed some ACR_PROTOCOL_ERRORs
 * to that or ACR_OTHER_ERROR.
 * Added #ifdef lint to DEFINE_ELEMENT.
 * 
 * Revision 1.9  93/12/10  09:20:32  neelin
 * Added acr_find_<type> routines.
 * 
 * Revision 1.8  93/12/08  09:04:59  neelin
 * Fixed memory leak in acr_input_group_with_max.
 * Fixed acr_input_group_list (didn't stop reading when reached max group).
 * 
 * Revision 1.7  93/11/30  08:57:42  neelin
 * Added group and group list copy routines.
 * 
 * Revision 1.6  93/11/26  18:47:51  neelin
 * Added group and group list copy routines.
 * 
 * Revision 1.5  93/11/25  10:36:57  neelin
 * Fixed input_group_list (wasn't checking max properly).
 * 
 * Revision 1.4  93/11/24  12:05:12  neelin
 * Changed format of dump.
 * 
 * Revision 1.3  93/11/24  11:25:38  neelin
 * Added some group list stuff (dump, input_group_list).
 * 
 * Revision 1.2  93/11/22  13:11:58  neelin
 * Changed to use new Acr_Element_Id stuff
 * 
 * Revision 1.1  93/11/19  12:48:52  neelin
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
#include <ctype.h>
#include <minc_def.h>
#include <acr_nema.h>

/* Private functions */
private void update_group_length_element(Acr_Group group, 
                                         Acr_VR_encoding_type vr_encoding);
private Acr_Status acr_input_group_with_max(Acr_File *afp, Acr_Group *group, 
                                            int max_group_id);


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
@MODIFIED   : February 4, 1997 (P.N.)
---------------------------------------------------------------------------- */
public Acr_Group acr_create_group(int group_id)
{
   Acr_Group group;
   Acr_Element length_element;
   long group_length = 0;

   /* Allocate the group */
   group = MALLOC(sizeof(*group));

   /* Create a length element */
   group_length = 0;
   length_element = 
      acr_create_element(group_id, ACR_EID_GRPLEN, ACR_VR_UL,
                         (long) ACR_SIZEOF_LONG, 
                         acr_memdup((size_t) ACR_SIZEOF_LONG, &group_length));

   /* Assign fields */
   group->group_id = group_id;
   group->nelements = 1;
   group->implicit_total_length = 
      acr_get_element_total_length(length_element, ACR_IMPLICIT_VR);
   group->explicit_total_length = 
      acr_get_element_total_length(length_element, ACR_EXPLICIT_VR);
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

   /* Check for null group */
   if (group_list == NULL) return;

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
@NAME       : acr_copy_group
@INPUT      : group
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Makes a copy of an acr-nema group structure
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 26, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Group acr_copy_group(Acr_Group group)
{
   Acr_Group copy;
   Acr_Element cur;

   /* Create the group */
   copy = acr_create_group(acr_get_group_group(group));

   /* Get the second element (first element is always there) */
   cur = acr_get_element_next(group->list_head);
   while (cur != NULL) {
      acr_group_add_element(copy, acr_copy_element(cur));
      cur = acr_get_element_next(cur);
   }

   return copy;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_copy_group_list
@INPUT      : group_list
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Make a copy of a group list
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 26, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Group acr_copy_group_list(Acr_Group group_list)
{
   Acr_Group copy_list;
   Acr_Group copy_group;
   Acr_Group cur;

   /* Create first group */
   copy_list = copy_group = acr_copy_group(group_list);

   /* Loop through groups */
   cur = acr_get_group_next(group_list);
   while (cur != NULL) {
      acr_set_group_next(copy_group, acr_copy_group(cur));
      copy_group = acr_get_group_next(copy_group);
      cur = acr_get_group_next(cur);
   }

   return copy_list;

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
   group->implicit_total_length += 
      acr_get_element_total_length(element, ACR_IMPLICIT_VR);
   group->explicit_total_length += 
      acr_get_element_total_length(element, ACR_EXPLICIT_VR);

   /* Update the length element */
   update_group_length_element(group, 
                               acr_get_element_vr_encoding(group->list_head));

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : update_group_length_element
@INPUT      : group
              vr_encoding - ACR_IMPLICIT_VR or ACR_EXPLICIT_VR
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Update the length element of the group according to the VR type
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 14, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void update_group_length_element(Acr_Group group, 
                                         Acr_VR_encoding_type vr_encoding)
{
   long group_length;
   Acr_Element length_element;
   void *group_length_data;

   /* Get the element */
   length_element = group->list_head;
   if (length_element == NULL) return;

   /* Calculate the appropriate length */
   if (vr_encoding == ACR_IMPLICIT_VR) {
      group_length = group->implicit_total_length - 
         acr_get_element_total_length(length_element, ACR_IMPLICIT_VR);
   }
   else {
      group_length = group->explicit_total_length -
         acr_get_element_total_length(length_element, ACR_EXPLICIT_VR);
   }

   /* Update the element */
   group_length_data = acr_get_element_data(length_element);
   acr_put_long(acr_get_element_byte_order(length_element),
                1, &group_length, group_length_data);

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
              vr_encoding - ACR_EXPLICIT_VR or ACR_IMPLICIT_VR
@OUTPUT     : (none)
@RETURNS    : total length of group
@DESCRIPTION: Get total length of group
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public long acr_get_group_total_length(Acr_Group group,
                                       Acr_VR_encoding_type vr_encoding)
{
   if (vr_encoding == ACR_IMPLICIT_VR) 
      return group->implicit_total_length;
   else
      return group->explicit_total_length;
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
@NAME       : acr_input_group_with_max
@INPUT      : afp - acr file pointer
              max_group_id - maximum group id to read in. If <= 0 then any
                 group is read in.
@OUTPUT     : group
@RETURNS    : status
@DESCRIPTION: Read in an acr-nema group with an optional maximum group id.
              If group id exceeds max, then *group is set to NULL. If an
              error occurs, then a group may still be returned. This routine
              will stop reading when it reaches a watchpoint.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Status acr_input_group_with_max(Acr_File *afp, Acr_Group *group, 
                                            int max_group_id)
{
   int group_id, element_id, next_group_id;
   long group_length;
   Acr_Status status;
   Acr_Element element;
   int have_length_element;
   int get_more_elements;
   Acr_VR_encoding_type vr_encoding;

   /* Initialize the group pointer */
   *group = NULL;

   /* Look ahead at the next element */
   status = acr_peek_at_next_element_id(afp, &group_id, &element_id);
   if (status != ACR_OK) return status;

   /* Check for a group past the limit */
   if ((max_group_id > 0) && (group_id > max_group_id)) {
      return status;
   }

   /* Check for a length element */
   have_length_element = (element_id == ACR_EID_GRPLEN);

   /* Read the length element and check it */
   if (have_length_element) {
      status = acr_input_element(afp, &element);
      if (status != ACR_OK) {
         acr_delete_element(element);
         return status;
      }
      if ((acr_get_element_element(element) != ACR_EID_GRPLEN) || 
          (acr_get_element_length(element) != ACR_SIZEOF_LONG)) {
         acr_delete_element(element);
         status = ACR_PROTOCOL_ERROR;
         return status;
      }
      group_length = acr_get_element_long(element);
      acr_delete_element(element);
   }

   /* Create the group */
   *group = acr_create_group(group_id);

   /* Set the VR encoding and the byte ordering for the length element 
      according to the input stream. If the vr_encoding is implicit, then
      make the VR unknown. Note that the group will always have a length
      element even if the input stream does not. */
   element = acr_get_group_element_list(*group);
   vr_encoding = acr_get_vr_encoding(afp);
   acr_set_element_vr_encoding(element, vr_encoding);
   acr_set_element_byte_order(element, acr_get_byte_order(afp));
   if (vr_encoding == ACR_IMPLICIT_VR) {
      acr_set_element_vr(element, ACR_VR_UNKNOWN);
   }

   /* Loop through elements, adding them to the list */
   get_more_elements = (have_length_element ? (group_length > 0) : TRUE);
   while (get_more_elements) {

      /* Check for a watchpoint */
      if (acr_get_io_watchpoint(afp) <= 0) {
         get_more_elements = FALSE;
         break;
      }

      /* Look ahead at next element */
      status = acr_peek_at_next_element_id(afp, &next_group_id, &element_id);
      if ((status != ACR_OK) || (next_group_id != group_id)) {
         get_more_elements = FALSE;
         break;
      }

      /* Read in the next element */
      status = acr_input_element(afp, &element);
      if (status != ACR_OK) {
         get_more_elements = FALSE;
      }

      /* Add it to the group */
      if (element != NULL) {
         acr_group_add_element(*group, element);
         if (have_length_element) {
            group_length -= 
               acr_get_element_total_length(element, acr_get_vr_encoding(afp));
         }
      }

      /* Check group length */
      if (have_length_element) {
         get_more_elements = (group_length > 0);
      }

   }

   /* Check that we got a full group */
   if (have_length_element && (group_length != 0)) {
      switch (status) {
      case ACR_OK:
         status = ACR_PROTOCOL_ERROR; break;
      case ACR_END_OF_INPUT:
         status = ACR_ABNORMAL_END_OF_INPUT; break;
      }
   }

   return status;
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

   return acr_input_group_with_max(afp, group, 0);

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

   /* Update the length element */
   update_group_length_element(group, acr_get_vr_encoding(afp));

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
      status = ACR_OTHER_ERROR;
      return status;
   }

   return status;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_input_group_list
@INPUT      : afp - acr file pointer
              max_group_id - maximum group id to read in. If <= 0 then all
                 groups are read in. If max_group_id is not a group id in the
                 input stream, then the input stream is left with the first
                 element of the next group missing (it gets read here and
                 is not put back), so no more groups can be read in.
@OUTPUT     : group_list
@RETURNS    : status
@DESCRIPTION: Read in a list of acr-nema groups. If a watchpoint is set
              on the input handle, then group reading will stop when it is
              reached, although at least one group will be read in.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 24, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Status acr_input_group_list(Acr_File *afp, Acr_Group *group_list,
                                       int max_group_id)
{
   Acr_Group cur_group, next_group;
   Acr_Status status;

   /* Initialize the group list */
   *group_list = NULL;

   /* Read in the first group */
   status = acr_input_group_with_max(afp, &next_group, max_group_id);
   if ((status != ACR_OK) || (next_group == NULL)) return status;

   /* Set up pointers */
   *group_list = cur_group = next_group;

   /* Loop, reading groups */
   while ((status == ACR_OK) && (cur_group != NULL) && 
          ((max_group_id <= 0) ||
           (acr_get_group_group(cur_group) < max_group_id))) {

      /* Check for a watchpoint */
      if (acr_get_io_watchpoint(afp) <= 0) {
         break;
      }

      /* Read in the next group */
      status = acr_input_group_with_max(afp, &next_group, max_group_id);

      /* Add it to the list */
      acr_set_group_next(cur_group, next_group);
      cur_group = next_group;

   }

   return status;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_find_element_in_group_list
@INPUT      : group_list
              elid
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
                                          Acr_Element_Id elid)
{
   Acr_Group group;
   int next_id, group_id;

   /* Get group and element id */
   group_id = elid->group_id;

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
   return acr_find_element_id(acr_get_group_element_list(group), elid);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_dump_group_list
@INPUT      : file_pointer - where output should go
              group_list
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Dump information from an acr-nema group list
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 24, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_dump_group_list(FILE *file_pointer, Acr_Group group_list)
{
   Acr_Group cur_group;

   /* Check for empty list */
   cur_group = group_list;
   if (cur_group == NULL) {
      (void) fprintf(file_pointer,"\nEmpty group list\n\n");
      return;
   }

   /* Loop over groups */
   while (cur_group != NULL) {

      /* Print the group id */
      (void) fprintf(file_pointer, "\nGroup 0x%04x :\n\n", 
                     acr_get_group_group(cur_group));

      /* Print the elements */
      acr_dump_element_list(file_pointer,
                            acr_get_group_element_list(cur_group));

      /* Go to the next group */
      cur_group = acr_get_group_next(cur_group);
   }

   /* Print a blank line after dump */
   (void) fprintf(file_pointer, "\n");

   /* Flush the buffer */
   (void) fflush(file_pointer);

   return;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_find_short
@INPUT      : group_list
              elid
              default_value
@OUTPUT     : (none)
@RETURNS    : Element value or default_value if element not found
@DESCRIPTION: Find an element in a group list and return its value (assuming
              that it is stored as a binary short).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int acr_find_short(Acr_Group group_list, Acr_Element_Id elid, 
                          int default_value)
{
   Acr_Element element;

   element = acr_find_group_element(group_list, elid);
   if (element != NULL) 
      return (int) acr_get_element_short(element);
   else
      return default_value;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_find_long
@INPUT      : group_list
              elid
              default_value
@OUTPUT     : (none)
@RETURNS    : Element value or default_value if element not found
@DESCRIPTION: Find an element in a group list and return its value (assuming
              that it is stored as a binary long).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public long acr_find_long(Acr_Group group_list, Acr_Element_Id elid, 
                          long default_value)
{
   Acr_Element element;

   element = acr_find_group_element(group_list, elid);
   if (element != NULL) 
      return acr_get_element_long(element);
   else
      return default_value;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_find_int
@INPUT      : group_list
              elid
              default_value
@OUTPUT     : (none)
@RETURNS    : Element value or default_value if element not found
@DESCRIPTION: Find an element in a group list and return its value (assuming
              that it is stored as an ascii integer).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int acr_find_int(Acr_Group group_list, Acr_Element_Id elid, 
                        int default_value)
{
   Acr_Element element;

   element = acr_find_group_element(group_list, elid);
   if (element != NULL) 
      return (int) acr_get_element_numeric(element);
   else
      return default_value;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_find_double
@INPUT      : group_list
              elid
              default_value
@OUTPUT     : (none)
@RETURNS    : Element value or default_value if element not found
@DESCRIPTION: Find an element in a group list and return its value (assuming
              that it is stored as an ascii double).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public double acr_find_double(Acr_Group group_list, Acr_Element_Id elid, 
                              double default_value)
{
   Acr_Element element;

   element = acr_find_group_element(group_list, elid);
   if (element != NULL) 
      return acr_get_element_numeric(element);
   else
      return default_value;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_find_string
@INPUT      : group_list
              elid
              default_value
@OUTPUT     : (none)
@RETURNS    : Element value or default_value if element not found
@DESCRIPTION: Find an element in a group list and return its value (assuming
              that it is stored as an ascii string).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public char *acr_find_string(Acr_Group group_list, Acr_Element_Id elid, 
                             char *default_value)
{
   Acr_Element element;

   element = acr_find_group_element(group_list, elid);
   if (element != NULL) 
      return acr_get_element_string(element);
   else
      return default_value;
}

