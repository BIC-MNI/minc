/* ----------------------------- MNI Header -----------------------------------
@NAME       : group.c
@DESCRIPTION: Routines for doing acr_nema group operations.
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
 * $Log: group.c,v $
 * Revision 6.12  2008/08/12 05:00:22  rotor
 *  * large number of changes from Claude (64 bit and updates)
 *
 * Revision 6.11  2006/04/09 15:29:43  bert
 * Add acr_insert_double()
 *
 * Revision 6.10  2005/05/09 15:34:46  bert
 * For acr_find_{short,int,long,double}, treat a zero-length element as if it were absent, and return the default value.
 *
 * Revision 6.9  2005/03/11 22:05:29  bert
 * Implement _acr_name_proc to allow printing of field names in dump_acr_nema
 *
 * Revision 6.8  2005/03/04 17:09:11  bert
 * Change several functions to return Acr_Status instead of void; lose public and private; Make insert_element() check the return value of acr_get_element_total_length()
 *
 * Revision 6.7  2004/10/29 13:08:42  rotor
 *  * rewrote Makefile with no dependency on a minc distribution
 *  * removed all references to the abominable minc_def.h
 *  * I should autoconf this really, but this is old code that
 *      is now replaced by Jon Harlaps PERL version..
 *
 * Revision 6.6  2002/12/08 22:31:34  neelin
 * When a last fragment is received, the dicom watchpoint is only updated when the next read happens, so the peek ahead will fail after the watchpoint test in acr_input_group_with_max.
 *
 * Revision 6.5  2001/11/08 14:17:05  neelin
 * Added acr_test_dicom_file to allow reading of DICOM part 10 format
 * files. This function also calls acr_test_byte_order to set up the stream
 * properly and can be used as a direct replacement for that function.
 * This set of changes does NOT include the ability to write part 10 files.
 *
 * Revision 6.4  2000/05/01 17:18:07  neelin
 * Modifications tohandle end-of-input properly, both on first group
 * read, and when ignoring protocol errors.
 *
 * Revision 6.3  2000/04/28 15:03:11  neelin
 * Added support for ignoring non-fatal protocol errors (cases where redundant
 * information is inconsistent). In particular, it is possible to ignore
 * differences between the group length element and the true group length.
 *
 * Revision 6.2  1999/10/29 17:51:53  neelin
 * Fixed Log keyword
 *
 * Revision 6.1  1998/11/06 19:41:06  neelin
 * Added functions acr_group_steal_element and acr_find_group.
 *
 * Revision 6.0  1997/09/12  13:23:59  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:25:00  neelin
 * Release of minc version 0.5
 *
 * Revision 4.2  1997/08/21  13:24:55  neelin
 * Pre-release
 *
 * Revision 4.1  1997/06/17  23:49:08  neelin
 * Added routines for inserting elements into a group list.
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
#include <string.h>
#include <acr_nema.h>

/* Private functions */
static void steal_element(Acr_Group group, Acr_Element element, 
                          Acr_Element previous);
static void remove_element(Acr_Group group, Acr_Element element, 
                           Acr_Element previous);
static Acr_Status insert_element(Acr_Group group, Acr_Element element, 
                                 Acr_Element previous);
static void update_group_length_element(Acr_Group group, 
                                        Acr_VR_encoding_type vr_encoding);
static Acr_Status acr_input_group_with_max(Acr_File *afp, Acr_Group *group, 
                                           int max_group_id);

acr_name_proc_t _acr_name_proc = NULL;


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
Acr_Group acr_create_group(int group_id)
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
void acr_delete_group(Acr_Group group)
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
void acr_delete_group_list(Acr_Group group_list)
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
Acr_Group acr_copy_group(Acr_Group group)
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
Acr_Group acr_copy_group_list(Acr_Group group_list)
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
@NAME       : steal_element
@INPUT      : group
              element - element to remove
              previous - pointer to previous element or NULL if beginning
                 of group element list
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Steal an element from a group (remove it without deleting it).
              The caller must free the element themselves.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 6, 1998 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static void steal_element(Acr_Group group, Acr_Element element, 
                          Acr_Element previous)
{
   Acr_Element next;

   /* Get pointer to next element */
   next = acr_get_element_next(element);

   /* Update the previous element or list head */
   if (previous != NULL)
      acr_set_element_next(previous, next);
   else
      group->list_head = next;

   /* Check for an element at the tail */
   if (next == NULL)
      group->list_tail = previous;

   /* Update the group fields */
   group->nelements--;
   group->implicit_total_length -= 
      acr_get_element_total_length(element, ACR_IMPLICIT_VR);
   group->explicit_total_length -= 
      acr_get_element_total_length(element, ACR_EXPLICIT_VR);

   /* Update the group length element */
   update_group_length_element(group, 
                               acr_get_element_vr_encoding(group->list_head));

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : remove_element
@INPUT      : group
              element - element to remove
              previous - pointer to previous element or NULL if beginning
                 of group element list
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Remove an element from a group. 
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June 17, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static void remove_element(Acr_Group group, Acr_Element element, 
                           Acr_Element previous)
{

   /* Get rid of the old element from the group */
   steal_element(group, element, previous);

   /* Delete the old element */
   acr_delete_element(element);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : insert_element
@INPUT      : group
              element - element to insert
              previous - pointer to previous element or NULL if beginning
                 of group element list
@OUTPUT     : (none)
@RETURNS    : Acr_Status
@DESCRIPTION: Insert an element into a group. 
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June 17, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static Acr_Status insert_element(Acr_Group group, Acr_Element element, 
                                 Acr_Element previous)
{
   Acr_Element next;
   long length;

   /* Update the pointers */
   if (previous != NULL) {        /* Middle or tail of list */
      next = acr_get_element_next(previous);
      acr_set_element_next(previous, element);
   }
   else {                         /* Head of list */
      next = group->list_head;
      group->list_head = element;
   }
   acr_set_element_next(element, next);

   /* Check for the tail */
   if (next == NULL) {
      group->list_tail = element;
   }

   /* Update the group fields */
   group->nelements++;
   length = acr_get_element_total_length(element, ACR_IMPLICIT_VR);
   if (length <= 0) {
      return (ACR_OTHER_ERROR);
   }
   group->implicit_total_length += length;

   length = acr_get_element_total_length(element, ACR_EXPLICIT_VR);
   if (length <= 0) {
      return (ACR_OTHER_ERROR);
   }
   group->explicit_total_length += length;


   /* Update the length element */
   update_group_length_element(group, 
                               acr_get_element_vr_encoding(group->list_head));
   return (ACR_OK);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_group_insert_element
@INPUT      : group
              element
@OUTPUT     : (none)
@RETURNS    : Acr_Status
@DESCRIPTION: Insert an element into a group. If an element of the same 
              id already exists in the list, it is removed and deleted.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June 17, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
Acr_Status acr_group_insert_element(Acr_Group group,
                                    Acr_Element element)
{
   Acr_Element next_element, prev_element, cur_element;
   int element_id;

   /* Check that the element belongs in this group */
   if (group->group_id != acr_get_element_group(element)) {
      (void) fprintf(stderr, 
          "ACR error: Cannot add element %d (group %d) to group %d\n",
                     acr_get_element_element(element),
                     acr_get_element_group(element),
                     group->group_id);
      exit(EXIT_FAILURE);
   }

   /* Get element id */
   element_id = acr_get_element_element(element);

   /* Check that new element has id > group length element id */
   if (element_id < ACR_EID_GRPLEN) {
      (void) fprintf(stderr, 
      "ACR error: Cannot add element id %d <= length id (%d) to group %d\n",
                     element_id, ACR_EID_GRPLEN, group->group_id);
      exit(EXIT_FAILURE);
   }

   /* Check whether the the element should be added after the last element */
   if (acr_get_element_element(group->list_tail) < element_id) {
      prev_element = group->list_tail;
      next_element = NULL;
   }

   /* Otherwise, search for the appropriate location */
   else {
      prev_element = NULL;
      next_element = group->list_head;
      while ((next_element != NULL) && 
             (acr_get_element_element(next_element) < element_id)) {
         prev_element = next_element;
         next_element = acr_get_element_next(next_element);
      }
   }

   /* Check for an existing element and get rid of it */
   if ((next_element != NULL) &&
       (acr_get_element_element(next_element) == element_id)) {

      /* Set pointers and get rid of the old element */
      cur_element = next_element;
      next_element = acr_get_element_next(cur_element);
      remove_element(group, cur_element, prev_element);

   }

   /* Insert the new element */
   return insert_element(group, element, prev_element);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_group_add_element
@INPUT      : group
              element
@OUTPUT     : (none)
@RETURNS    : Acr_Status
@DESCRIPTION: Add an element to an acr-nema group
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
Acr_Status acr_group_add_element(Acr_Group group, Acr_Element element)
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

   /* Insert the element at the tail */
   return insert_element(group, element, group->list_tail);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_group_remove_element
@INPUT      : group
              element_id
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Remove an element from a group.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June 17, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
void acr_group_remove_element(Acr_Group group, int element_id)
{
   Acr_Element next_element, prev_element;

   /* Search for the appropriate location */
   prev_element = NULL;
   next_element = group->list_head;
   while ((next_element != NULL) && 
          (acr_get_element_element(next_element) != element_id)) {
      prev_element = next_element;
      next_element = acr_get_element_next(next_element);
   }

   /* Check for an existing element and get rid of it */
   if ((next_element != NULL) &&
       (acr_get_element_element(next_element) == element_id)) {

      /* Set pointers and get rid of the old element */
      remove_element(group, next_element, prev_element);

   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_group_steal_element
@INPUT      : group
              element - the caller must pass an element so that we
                 know that they have a handle to it.
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Remove an element from a group without deleting it.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 6, 1998 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
void acr_group_steal_element(Acr_Group group, Acr_Element element)
{
   int element_id;
   Acr_Element next_element, prev_element;

   /* Get element id from element */
   element_id = acr_get_element_element(element);

   /* Search for the appropriate location */
   prev_element = NULL;
   next_element = group->list_head;
   while ((next_element != NULL) && 
          (acr_get_element_element(next_element) != element_id)) {
      prev_element = next_element;
      next_element = acr_get_element_next(next_element);
   }

   /* Check for an existing element and get rid of it */
   if ((next_element != NULL) &&
       (acr_get_element_element(next_element) == element_id)) {

      /* Set pointers and get rid of the old element */
      steal_element(group, next_element, prev_element);

   }

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
static void update_group_length_element(Acr_Group group, 
                                        Acr_VR_encoding_type vr_encoding)
{
   Acr_Long group_length;
   Acr_Element length_element;
   void *group_length_data;

   /* Get the element */
   length_element = group->list_head;
   if (length_element == NULL) return;
   if (acr_get_element_element(length_element) != ACR_EID_GRPLEN) return;

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
void acr_set_group_next(Acr_Group group, Acr_Group next)
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
int acr_get_group_group(Acr_Group group)
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
Acr_Element acr_get_group_element_list(Acr_Group group)
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
long acr_get_group_total_length(Acr_Group group,
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
int acr_get_group_nelements(Acr_Group group)
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
Acr_Group acr_get_group_next(Acr_Group group)
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
static Acr_Status acr_input_group_with_max(Acr_File *afp, Acr_Group *group, 
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
         if (acr_ignore_protocol_errors(afp)) {
            group_length = 0;
         }
         else {
            acr_delete_element(element);
            status = ACR_PROTOCOL_ERROR;
            return status;
         }
      }
      else {
         group_length = acr_get_element_long(element);
      }
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
         if ( status == ACR_REACHED_WATCHPOINT )
            status = ACR_OK;
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
      if (have_length_element && !acr_ignore_protocol_errors(afp)) {
         get_more_elements = (group_length > 0);
      }

   }

   /* Check that we got a full group */
   if (have_length_element && (group_length != 0) && 
       !acr_ignore_protocol_errors(afp)) {
      switch (status) {
      case ACR_OK:
         status = ACR_PROTOCOL_ERROR;
         break;
      case ACR_END_OF_INPUT:
         status = ACR_ABNORMAL_END_OF_INPUT; 
         break;
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
Acr_Status acr_input_group(Acr_File *afp, Acr_Group *group)
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
Acr_Status acr_output_group(Acr_File *afp, Acr_Group group)
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
Acr_Status acr_input_group_list(Acr_File *afp, Acr_Group *group_list,
                                int max_group_id)
{
   Acr_Group cur_group, next_group;
   Acr_Status status;

   /* Initialize the group list */
   *group_list = NULL;

   /* Read in the first group */
   status = acr_input_group_with_max(afp, &next_group, max_group_id);

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
@NAME       : acr_find_group
@INPUT      : group_list
              group_id
@OUTPUT     : (none)
@RETURNS    : gropu pointer
@DESCRIPTION: Find a group in a group list
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 6, 1998 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
Acr_Group acr_find_group(Acr_Group group_list, int group_id)
{
   Acr_Group group;
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

   return group;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_find_group_element
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
Acr_Element acr_find_group_element(Acr_Group group_list,
                                   Acr_Element_Id elid)
{
   Acr_Group group;

   /* Find the group */
   group = acr_find_group(group_list, elid->group_id);

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
void acr_dump_group_list(FILE *file_pointer, Acr_Group group_list)
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
Acr_Short acr_find_short(Acr_Group group_list, Acr_Element_Id elid, 
                         Acr_Short default_value)
{
   Acr_Element element;

   element = acr_find_group_element(group_list, elid);
   if (element != NULL && acr_get_element_length(element) > 0) 
      return acr_get_element_short(element);
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
Acr_Long acr_find_long(Acr_Group group_list, Acr_Element_Id elid, 
                       Acr_Long default_value)
{
   Acr_Element element;

   element = acr_find_group_element(group_list, elid);
   if (element != NULL && acr_get_element_length(element) > 0) 
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
int acr_find_int(Acr_Group group_list, Acr_Element_Id elid, 
                 int default_value)
{
   Acr_Element element;

   element = acr_find_group_element(group_list, elid);
   if (element != NULL && acr_get_element_length(element) > 0) 
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
Acr_Double acr_find_double(Acr_Group group_list, Acr_Element_Id elid, 
                           Acr_Double default_value)
{
   Acr_Element element;

   element = acr_find_group_element(group_list, elid);
   if (element != NULL && acr_get_element_length(element) > 0) 
      return (Acr_Double)acr_get_element_numeric(element);
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
Acr_String acr_find_string(Acr_Group group_list, Acr_Element_Id elid, 
                           Acr_String default_value)
{
   Acr_Element element;

   element = acr_find_group_element(group_list, elid);
   if (element != NULL)         /* Allow zero-length strings */
      return acr_get_element_string(element);
   else
      return default_value;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_insert_element_into_group_list
@INPUT      : group_list - list in which element should be inserted 
                 (can be NULL)
              element - element to insert
@OUTPUT     : group_list - modified group list
@RETURNS    : Acr_Status
@DESCRIPTION: Insert an element into a group list. If the group_list is NULL,
              then it is created. Note that the element is not copied, it is
              just inserted into the list, so it should not be modified after
              insertion into the list. If an element of the same id already 
              exists in the list, it is removed and deleted.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June 17, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
Acr_Status acr_insert_element_into_group_list(Acr_Group *group_list,
                                              Acr_Element element)
{
   Acr_Group group, next_group, prev_group;
   int group_id;

   /* Get group and element id */
   group_id = acr_get_element_group(element);

   /* Search for the appropriate group */
   prev_group = NULL;
   next_group = *group_list;
   while ((next_group != NULL) && 
          (acr_get_group_group(next_group) < group_id)) {
      prev_group = next_group;
      next_group = acr_get_group_next(next_group);
   }

   /* Check if we have the right group */
   if ((next_group != NULL) && 
       (acr_get_group_group(next_group) == group_id)) {
      group = next_group;
   }

   /* If not, create a new group and insert it in the list */
   else {

      /* Create a group */
      group = acr_create_group(group_id);

      /* Insert it in the list */
      acr_set_group_next(group, next_group);
      if (prev_group != NULL) {
         acr_set_group_next(prev_group, group);
      }
      else {
         *group_list = group;
      }

   }

   /* Insert the element into the appropriate group */
   return acr_group_insert_element(group, element);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_insert_short
@INPUT      : group_list - may be NULL if list empty
              elid
              value
@OUTPUT     : group_list - modified group list
@RETURNS    : Acr_Status
@DESCRIPTION: Creates and inserts an element into a group list.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June 17, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
Acr_Status acr_insert_short(Acr_Group *group_list, Acr_Element_Id elid, 
                            Acr_Short value)
{
   Acr_Element element;

   element = acr_create_element_short(elid, value);
   return acr_insert_element_into_group_list(group_list, element);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_insert_long
@INPUT      : group_list - may be NULL if list empty
              elid
              value
@OUTPUT     : group_list - modified group list
@RETURNS    : Acr_Status
@DESCRIPTION: Creates and inserts an element into a group list.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June 17, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
Acr_Status acr_insert_long(Acr_Group *group_list, Acr_Element_Id elid, 
                           Acr_Long value)
{
   Acr_Element element;

   element = acr_create_element_long(elid, value);
   return acr_insert_element_into_group_list(group_list, element);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_insert_double
@INPUT      : group_list - may be NULL if list empty
              elid
              nvalues
              values
@OUTPUT     : group_list - modified group list
@RETURNS    : Acr_Status
@DESCRIPTION: Creates and inserts an element into a group list.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 8, 2006 (Bert Vincent)
@MODIFIED   : 
---------------------------------------------------------------------------- */
Acr_Status acr_insert_double(Acr_Group *group_list, Acr_Element_Id elid,
                             int nvalues, Acr_Double *values)
{
    Acr_Element element;
    element = acr_create_element_double(elid, nvalues, values);
    return acr_insert_element_into_group_list(group_list, element);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_insert_numeric
@INPUT      : group_list - may be NULL if list empty
              elid
              value
@OUTPUT     : group_list - modified group list
@RETURNS    : Acr_Status
@DESCRIPTION: Creates and inserts an element into a group list.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June 17, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
Acr_Status acr_insert_numeric(Acr_Group *group_list, 
                              Acr_Element_Id elid, 
                              double value)
{
   Acr_Element element;

   element = acr_create_element_numeric(elid, value);
   return acr_insert_element_into_group_list(group_list, element);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_insert_string
@INPUT      : group_list - may be NULL if list empty
              elid
              value
@OUTPUT     : group_list - modified group list
@RETURNS    : Acr_Status
@DESCRIPTION: Creates and inserts an element into a group list.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June 17, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
Acr_Status acr_insert_string(Acr_Group *group_list, 
                             Acr_Element_Id elid, 
                             Acr_String value)
{
   Acr_Element element;

   element = acr_create_element_string(elid, value);
   return acr_insert_element_into_group_list(group_list, element);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_insert_sequence
@INPUT      : group_list - may be NULL if list empty
              elid
              itemlist
@OUTPUT     : group_list - modified group list
@RETURNS    : Acr_Status
@DESCRIPTION: Creates and inserts an element into a group list.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June 17, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
Acr_Status acr_insert_sequence(Acr_Group *group_list, 
                               Acr_Element_Id elid, 
                               Acr_Element itemlist)
{
   Acr_Element element;

   element = acr_create_element_sequence(elid, itemlist);
   return acr_insert_element_into_group_list(group_list, element);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_test_dicom_file
@INPUT      : afp
@OUTPUT     : (none)
@RETURNS    : status
@DESCRIPTION: Tests for a dicom file input, as well as setting the byte
              order and VR encoding.
@METHOD     : Check the byte order and t
              This function is in this file because it does not really have 
              a better place to live: It depends on the group reading code 
              (and so should not live in acr_io.c) and is independent of 
              the other dicom code (and so should not live there).
@GLOBALS    : 
@CALLS      : acr_test_byte_order, acr_input_group_list
@CREATED    : November 8, 2001 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
Acr_Status acr_test_dicom_file(Acr_File *afp)
{
#define DICOM_FILE_MAGIC_OFFSET 128
#define DICOM_MAGIC_STRING "DICM"
#define DICOM_MAGIC_LEN 4
#define DICOM_FILE_GROUP 0x2

   unsigned char buffer[DICOM_FILE_MAGIC_OFFSET+DICOM_MAGIC_LEN];
   Acr_Status status;
   long buflen;
   Acr_Group group_list;

   /* Read in up to magic */
   status = acr_read_buffer(afp, buffer, sizeof(buffer), &buflen);
   if (status != ACR_OK) return status;

   /* Check for the magic. If it is not there, put the data back and
      then test the byte order. */
   if (strncmp((char *) &buffer[DICOM_FILE_MAGIC_OFFSET], DICOM_MAGIC_STRING, 
               DICOM_MAGIC_LEN) != 0) {
      status = acr_unget_buffer(afp, buffer, buflen);
      if (status != ACR_OK) return status;
      return acr_test_byte_order(afp);
   }

   /* Read in group 2.
      We could get the transfer syntax in the group at this point, 
      or just use the test heuristic from acr_test_byte_order. It seems
      safer to trust the heuristic than the file writer, so we will ignore
      the contents of the group. */
   status = acr_test_byte_order(afp);
   if (status != ACR_OK) return status;
   status = acr_input_group_list(afp, &group_list, DICOM_FILE_GROUP);
   acr_delete_group_list(group_list);
   if (status != ACR_OK) return status;

   /* Test the byte order for the remainder of the file */
   status = acr_test_byte_order(afp);
   if (status != ACR_OK) return status;

   return status;
}

