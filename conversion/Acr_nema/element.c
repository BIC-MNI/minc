/* ----------------------------- MNI Header -----------------------------------
@NAME       : element.c
@DESCRIPTION: Routines for doing acr_nema element operations.
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : $Log: element.c,v $
@MODIFIED   : Revision 4.0  1997-05-07 20:01:23  neelin
@MODIFIED   : Release of minc version 0.4
@MODIFIED   :
 * Revision 3.2  1997/04/21  20:21:09  neelin
 * Updated the library to handle dicom messages.
 *
 * Revision 3.1  1995/11/16  13:13:49  neelin
 * Added include of math.h to get declaration of strtod under SunOs
 *
 * Revision 3.0  1995/05/15  19:32:12  neelin
 * Release of minc version 0.3
 *
 * Revision 2.0  1994/09/28  10:36:10  neelin
 * Release of minc version 0.2
 *
 * Revision 1.8  94/09/28  10:35:44  neelin
 * Pre-release
 * 
 * Revision 1.7  93/11/30  12:19:15  neelin
 * Handle MALLOC returning NULL because of extremely large data element length.
 * 
 * Revision 1.6  93/11/30  08:57:28  neelin
 * Added element copy routine.
 * 
 * Revision 1.5  93/11/26  18:47:36  neelin
 * Added element copy routine.
 * 
 * Revision 1.4  93/11/25  10:35:34  neelin
 * Ensure that strings have an even length (pad with space).
 * 
 * Revision 1.3  93/11/24  11:25:14  neelin
 * Changed short to unsigned short.
 * 
 * Revision 1.2  93/11/22  13:11:34  neelin
 * Added Acr_Element_Id code.
 * 
 * Revision 1.1  93/11/19  12:48:12  neelin
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
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <minc_def.h>
#include <acr_nema.h>

/* Private functions */
private void delete_element_data(Acr_Element element);
private Acr_Element create_element_mem(Acr_Element_Id elid,
                                       Acr_VR_Type vr_code, 
                                       size_t value_size, void *value);

/* Macros */
#define SIZEOF_ARRAY(a) (sizeof(a)/sizeof(a[0]))


/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_create_element
@INPUT      : group_id 
              element_id
              vr_code - Internal VR code
              data_length - if < 0, then data is assumed to be a pointer to
                 a list of elements in the sequence
              data_pointer - pointer to actual data or to list of elements
@OUTPUT     : (none)
@RETURNS    : Pointer to element structure
@DESCRIPTION: Creates an acr-nema element structure. If data_length is 
              negative, then the element is a sequence and the data_pointer
              must point to a list of elements. In this case, the element is
              assumed to have variable length. If the element is an
              ACR_VR_SQ, then it is possible to change it to have fixed length
              by calling acr_set_element_variable_length with FALSE.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : February 4, 1997 (P.N.)
---------------------------------------------------------------------------- */
public Acr_Element acr_create_element(int group_id, int element_id, 
                                      Acr_VR_Type vr_code, 
                                      long data_length, char *data_pointer)
{
   Acr_Element element;

   /* Allocate the element. We set the data pointer to NULL so that 
      acr_set_element_data does not try to free an unitialized pointer */
   element = MALLOC(sizeof(*element));
   element->data_pointer = NULL;

   /* Assign fields */
   acr_set_element_id(element, group_id, element_id);
   acr_set_element_vr(element, vr_code);
   acr_set_element_vr_encoding(element, ACR_EXPLICIT_VR);
   acr_set_element_byte_order(element, acr_get_machine_byte_order());
   acr_set_element_variable_length(element, (data_length < 0));
   acr_set_element_next(element, NULL);
   acr_set_element_data(element, data_length, data_pointer);

   return element;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : delete_element_data
@INPUT      : element
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Deletes element data, but leaves element in an intermediate
              state with dangling pointers and incorrect lengths.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 4, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void delete_element_data(Acr_Element element)
{
   char *data_pointer;

   data_pointer = acr_get_element_data(element);
   if (data_pointer != NULL) {
      if (acr_element_is_sequence(element)) {
         acr_delete_element_list((Acr_Element) data_pointer);
      }
      else {
         FREE(data_pointer);
      }
   }

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_delete_element
@INPUT      : element
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Deletes an acr-nema element structure (freeing the data pointer)
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : February 4, 1997 (P.N.)
---------------------------------------------------------------------------- */
public void acr_delete_element(Acr_Element element)
{

   if (element == NULL) return;
   delete_element_data(element);
   FREE(element);

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_delete_element_list
@INPUT      : element_list
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Delete a list of acr-nema elements
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_delete_element_list(Acr_Element element_list)
{
   Acr_Element next, cur;

   if (element_list == NULL) return;

   /* Loop through the list, deleting elements */
   next = element_list;
   while (next != NULL) {
      cur = next;
      next = acr_get_element_next(cur);
      acr_delete_element(cur);
   }

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_element_list_add
@INPUT      : element_list - pointer to element list or NULL
              element - element to add to list
@OUTPUT     : (none)
@RETURNS    : Pointer to new element list
@DESCRIPTION: Adds an element to a list of elements. If element_list is NULL,
              then a new list is created.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 11, 1997 (P.N.)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Element acr_element_list_add(Acr_Element element_list, 
                                        Acr_Element element)
{
   Acr_Element current;

   /* Add the element to the list */
   if (element_list == NULL) {
      element_list = element;
   }
   else {
      current = element_list;
      while (acr_get_element_next(current) != NULL) {
         current = acr_get_element_next(current);
      }
      acr_set_element_next(current, element);
   }

   return element_list;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_set_element_id
@INPUT      : element
              group_id
              element_id
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Set group and element id of an acr-nema element
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_set_element_id(Acr_Element element,
                               int group_id, int element_id)
{
   element->group_id = group_id;
   element->element_id = element_id;
   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_set_element_vr
@INPUT      : element
              group_id
              vr_code
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Set VR code for an element
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 4, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_set_element_vr(Acr_Element element,
                               Acr_VR_Type vr_code)
{
   element->vr_code = (short) vr_code;
   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_set_element_vr_encoding
@INPUT      : element
              group_id
              vr_encoding - ACR_EXPLICIT_ENCODING or ACR_IMPLICIT_ENCODING
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Set encoding type for element VR
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 4, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_set_element_vr_encoding(Acr_Element element,
                                        Acr_VR_encoding_type vr_encoding)
{
   element->uses_explicit_vr = (vr_encoding == ACR_EXPLICIT_VR);
   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_set_element_byte_order
@INPUT      : element
              group_id
              byte_order - ACR_LITTLE_ENDIAN or ACR_BIG_ENDIAN
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Set byte order for an element
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 14, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_set_element_byte_order(Acr_Element element,
                                       Acr_byte_order byte_order)
{
   element->has_little_endian_order = (byte_order == ACR_LITTLE_ENDIAN);
   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_set_element_variable_length
@INPUT      : element
              group_id
              has_variable_length
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Set flag indicating whether element has variable length or not.
              This can only be set if the element contains sequence data.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 4, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_set_element_variable_length(Acr_Element element,
                                            int has_variable_length)
{
   if (acr_element_is_sequence(element)) {
      element->has_variable_length = (has_variable_length != FALSE);
   }
   else {
      element->has_variable_length = FALSE;
   }
   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_set_element_data
@INPUT      : element
              data_length - if < 0, then data is treated as a list of
                 elements
              data_pointer
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Set data length and pointer of an acr-nema element. A negative
              length means that this element is a sequence and the data 
              pointer points to a list of elements (items).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : February 4, 1997 (P.N.)
---------------------------------------------------------------------------- */
public void acr_set_element_data(Acr_Element element,
                                 long data_length, char *data_pointer)
{
   Acr_Element item, last, last2;
   long last_length;

   /* Free the old data if needed */
   if (element->data_pointer != NULL) {
      delete_element_data(element);
   }

   /* Set the pointer and check for a sequence */
   element->data_pointer = data_pointer;
   element->is_sequence = (data_length < 0);

   /* If we have a sequence, work out its length and set each item to not
      use explicit VR */
   if (element->is_sequence) {
      data_length = 0;
      last = last2 = NULL;
      for (item = (Acr_Element) data_pointer; 
           item != NULL; 
           item=acr_get_element_next(item)) {
         last_length = acr_get_element_total_length(item, ACR_IMPLICIT_VR);
         data_length += last_length;
         acr_set_element_vr_encoding(item, ACR_IMPLICIT_VR);
         last2 = last;
         last = item;
      }

      /* Check for a last delimiting item - remove it so that we don't
         have to worry about switching between defined and undefined 
         lengths - it will always be added later */
      if ((last != NULL) &&
          (acr_get_element_group(last) == ACR_ITEM_GROUP) &&
          ((acr_get_element_element(last) == ACR_ITEM_DELIMITER) ||
           (acr_get_element_element(last) == ACR_SEQ_DELIMITER))) {
         if (last2 != NULL) {
            acr_set_element_next(last2, NULL);
            data_length -= last_length;
         }
         else {
            element->data_pointer = NULL;
            data_length = 0;
         }
         acr_delete_element(last);
      }

   }

   /* Save the length */
   element->data_length = data_length;

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_set_element_next
@INPUT      : element
              next
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Set pointer to next element for an acr-nema element
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_set_element_next(Acr_Element element, Acr_Element next)
{
   element->next = next;
   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_element_group
@INPUT      : element
@OUTPUT     : (none)
@RETURNS    : group id
@DESCRIPTION: Get group id for element
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int acr_get_element_group(Acr_Element element)
{
   return element->group_id;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_element_element
@INPUT      : element
@OUTPUT     : (none)
@RETURNS    : element id
@DESCRIPTION: Get element id for element
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int acr_get_element_element(Acr_Element element)
{
   return element->element_id;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_element_vr
@INPUT      : element
@OUTPUT     : (none)
@RETURNS    : element VR code
@DESCRIPTION: Get VR code for element
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 4, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_VR_Type acr_get_element_vr(Acr_Element element)
{
   return (Acr_VR_Type) element->vr_code;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_element_vr_encoding
@INPUT      : element
@OUTPUT     : (none)
@RETURNS    : ACR_EXPLICIT_ENCODING or ACR_IMPLICIT_ENCODING
@DESCRIPTION: Get VR encoding type for element
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 4, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_VR_encoding_type acr_get_element_vr_encoding(Acr_Element element)
{
   return (element->uses_explicit_vr ? ACR_EXPLICIT_VR : ACR_IMPLICIT_VR);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_element_is_sequence
@INPUT      : element
@OUTPUT     : (none)
@RETURNS    : TRUE if element stores a sequence of items
@DESCRIPTION: 
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 4, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int acr_element_is_sequence(Acr_Element element)
{
   return element->is_sequence;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_element_byte_order
@INPUT      : element
@OUTPUT     : (none)
@RETURNS    : ACR_BIG_ENDIAN or ACR_LITTLE_ENDIAN
@DESCRIPTION: 
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 17, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_byte_order acr_get_element_byte_order(Acr_Element element)
{
   return (element->has_little_endian_order ? 
           ACR_LITTLE_ENDIAN : ACR_BIG_ENDIAN);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_element_has_variable_length
@INPUT      : element
@OUTPUT     : (none)
@RETURNS    : TRUE if element has variable length representation
@DESCRIPTION: 
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 4, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int acr_element_has_variable_length(Acr_Element element)
{
   return element->has_variable_length;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_element_length
@INPUT      : element
@OUTPUT     : (none)
@RETURNS    : data_length
@DESCRIPTION: Get data length for element
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public long acr_get_element_length(Acr_Element element)
{
   long data_length;

   data_length = element->data_length;

   if (acr_element_has_variable_length(element)) {
      data_length += 2 * ACR_SIZEOF_SHORT + ACR_SIZEOF_LONG;
   }

   return data_length;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_element_data
@INPUT      : element
@OUTPUT     : (none)
@RETURNS    : data_pointer
@DESCRIPTION: Get data pointer for element
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public char *acr_get_element_data(Acr_Element element)
{
   return element->data_pointer;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_element_total_length
@INPUT      : element
              vr_encoding - ACR_IMPLICIT_VR or ACR_EXPLICIT_VR
@OUTPUT     : (none)
@RETURNS    : total length for element
@DESCRIPTION: Get total length for element in ACR-NEMA representation
              depending on VR
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public long acr_get_element_total_length(Acr_Element element,
                                         Acr_VR_encoding_type vr_encoding)
{
   return acr_get_element_length(element) + 
      acr_get_element_header_size(acr_get_vr_name(acr_get_element_vr(element)),
                                  vr_encoding);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_element_next
@INPUT      : element
@OUTPUT     : (none)
@RETURNS    : next element
@DESCRIPTION: Get next element for element
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Element acr_get_element_next(Acr_Element element)
{
   return element->next;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_copy_element
@INPUT      : element
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Copies an acr-nema element structure
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 26, 1993 (Peter Neelin)
@MODIFIED   : February 4, 1997 (P.N.)
---------------------------------------------------------------------------- */
public Acr_Element acr_copy_element(Acr_Element element)
{
   Acr_Element copy;
   long length;
   unsigned char *data;
   Acr_Element olditem, newitem, newlist;

   /* Check for NULL element */
   if (element == NULL) return NULL;

   /* Copy a sequence */
   if (acr_element_is_sequence(element)) {
      length = -1;
      olditem = (Acr_Element) acr_get_element_data(element);
      newitem = NULL;
      while (olditem != NULL) {
         if (newitem == NULL) {
            newlist = acr_copy_element(olditem);
            newitem = newlist;
         }
         else {
            acr_set_element_next(newitem, acr_copy_element(olditem));
            newitem = acr_get_element_next(newitem);
         }
         olditem = olditem->next;
      }
      data = (unsigned char *) newlist;
   }

   /* Or copy a value */
   else {
      length = acr_get_element_length(element);
      data = MALLOC(length+1);
      if (data == NULL) {
         length = 0;
         data = MALLOC(length+1);
      }
      data[length] = '\0';
      if (length > 0) {
         (void) memcpy(data, acr_get_element_data(element), length);
      }
   }

   /* Create the new element */
   copy = acr_create_element(acr_get_element_group(element), 
                             acr_get_element_element(element),
                             acr_get_element_vr(element),
                             length, (void *) data);
   acr_set_element_vr_encoding(copy, acr_get_element_vr_encoding(element));
   acr_set_element_byte_order(copy, acr_get_element_byte_order(element));
   acr_set_element_variable_length(copy,
                                   acr_element_has_variable_length(element));


   return copy;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_input_element
@INPUT      : afp - acr file pointer
@OUTPUT     : element
@RETURNS    : status
@DESCRIPTION: Read in an acr-nema element
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Status acr_input_element(Acr_File *afp, Acr_Element *element)
{
   int group_id, element_id, item_gid, item_elid;
   long data_length;
   char vr_name[2];
   char *data_pointer;
   Acr_Status status;
   int is_sequence, more_to_read, found_delimiter, has_variable_length;
   Acr_Element item, itemlist, previtem;
   Acr_VR_Type vr_code;
   Acr_VR_encoding_type old_vr_encoding;

   /* Set element in case of error */
   *element = NULL;

   /* Read in the value */
   status = acr_read_one_element(afp, &group_id, &element_id, vr_name,
                                 &data_length, &data_pointer);

   if (status != ACR_OK) {
      return status;
   }

   /* Get VR code */
   vr_code = acr_lookup_vr_name(vr_name);

   /* Check for sequence */
   is_sequence = (data_length < 0);
   if ((data_length > 0)  && (data_pointer == NULL)) {
      is_sequence = TRUE;
   }
   has_variable_length = (data_length < 0);

   /* If we have a sequence, read in all the items and store them as a 
      list of elements. We must turn off explicit VR encoding for items. */
   if (is_sequence) {
      more_to_read = TRUE;
      itemlist = NULL;
      old_vr_encoding = acr_get_vr_encoding(afp);
      acr_set_vr_encoding(afp, ACR_IMPLICIT_VR);
      while (more_to_read) {

         /* Read in an item */
         status = acr_input_element(afp, &item);
         if (item == NULL) break;

         /* If we know the length of the whole sequence, check it */
         if ((status == ACR_OK) && (data_length > 0)) {
            data_length -= acr_get_element_total_length(item, ACR_IMPLICIT_VR);
            if (data_length < 0) status = ACR_PROTOCOL_ERROR;
         }

         /* Look for delimiter */
         item_gid = acr_get_element_group(item);
         item_elid = acr_get_element_element(item);
         found_delimiter =
            ((item_gid == ACR_ITEM_GROUP) &&
             ((item_elid == ACR_ITEM_DELIMITER) ||
              (item_elid == ACR_SEQ_DELIMITER)));

         /* Add the item to the list if it is not a delimiter */
         if (!found_delimiter) {
            if (itemlist == NULL) {
               itemlist = item;
               previtem = item;
            }
            else {
               acr_set_element_next(previtem, item);
            }
         }

         /* Check for end of items */
         if ((data_length == 0) || found_delimiter || (status != ACR_OK)) {
            more_to_read = FALSE;
         }
      }        /* End of loop over items */

      /* Restore the VR encoding */
      acr_set_vr_encoding(afp, old_vr_encoding);

      /* Save the item list as the data */
      data_pointer = (char *) itemlist;
   }

   /* Create the element */
   *element = acr_create_element(group_id, element_id, vr_code, 
                                 (is_sequence ? -1 : data_length),
                                 data_pointer);
   acr_set_element_vr_encoding(*element, acr_get_vr_encoding(afp));
   acr_set_element_byte_order(*element, acr_get_byte_order(afp));
   if (is_sequence && !has_variable_length) {
      acr_set_element_variable_length(*element, FALSE);
   }

   return status;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_output_element
@INPUT      : afp - acr file pointer
              element
@OUTPUT     : (none)
@RETURNS    : status
@DESCRIPTION: Write out an acr-nema element. The byte ordering of the element
              data is changed to match that of the output stream
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Status acr_output_element(Acr_File *afp, Acr_Element element)
{
   char *vr_name;
   Acr_VR_Type vr_code;
   Acr_Status status;
   long data_length;
   int is_sequence, has_variable_length;
   char *data_pointer;
   int item_id;
   Acr_Element item;
   Acr_VR_encoding_type old_vr_encoding;

   /* Get info from element */
   vr_code = acr_get_element_vr(element);
   vr_name = acr_get_vr_name(vr_code);
   is_sequence = acr_element_is_sequence(element);
   has_variable_length = acr_element_has_variable_length(element);
   if (is_sequence && has_variable_length) {
      data_length = ACR_VARIABLE_LENGTH;
   }
   else {
      data_length = acr_get_element_length(element);
   }

   /* Get the data. */
   if (is_sequence) {
      data_pointer = NULL;
   }
   else {
      data_pointer = acr_get_element_data(element);
   }

   /* Convert the element byte order */
   acr_convert_element_byte_order(element, acr_get_byte_order(afp));

   /* Write out the element */
   status = acr_write_one_element(afp, 
                                  acr_get_element_group(element),
                                  acr_get_element_element(element),
                                  vr_name, data_length, data_pointer);
   if (status != ACR_OK) return status;

   /* Write out items if we have a sequence */
   if (is_sequence) {

      /* Set the encoding to implicit for items */
      old_vr_encoding = acr_get_vr_encoding(afp);
      acr_set_vr_encoding(afp, ACR_IMPLICIT_VR);

      /* Loop over items */
      item = (Acr_Element) acr_get_element_data(element);
      while (item != NULL) {
         status = acr_output_element(afp, item);
         if (status != ACR_OK) return status;
         item = acr_get_element_next(item);
      }

      /* Write out the delimiter if needed */
      if (has_variable_length) {
         if (acr_get_element_group(element) == ACR_ITEM_GROUP) {
            item_id = ACR_ITEM_DELIMITER;
         }
         else {
            item_id = ACR_SEQ_DELIMITER;
         }
         status = acr_write_one_element(afp, ACR_ITEM_GROUP, item_id,
                                        ACR_VR_UNKNOWN, 0L, NULL);
         if (status != ACR_OK) return status;
      }

      /* Restore the VR encoding */
      acr_set_vr_encoding(afp, old_vr_encoding);

   }

   return status;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_convert_element_byte_order
@INPUT      : element
              byte_order - ACR_BIG_ENDIAN or ACR_LITTLE_ENDIAN
@OUTPUT     : (none)
@RETURNS    : status
@DESCRIPTION: Converts the data within an element to a specified byte_order.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_convert_element_byte_order(Acr_Element element, 
                                           Acr_byte_order byte_order)
{
   Acr_byte_order element_byte_order;
   long nvalues;
   size_t value_size;
   
   /* Get the element and stream byte orders and change the element
      byte order to match that of the stream */
   element_byte_order = acr_get_element_byte_order(element);
   acr_set_element_byte_order(element, byte_order);

   /* Change byte order of data in place */
   if (!acr_element_is_sequence(element)) {

      /* Look for types that might need byte swapping */
      switch (acr_get_element_vr(element)) {
      case ACR_VR_SS:
      case ACR_VR_US:
      case ACR_VR_OW:
         value_size = ACR_SIZEOF_SHORT; break;
      case ACR_VR_SL:
      case ACR_VR_UL:
         value_size = ACR_SIZEOF_LONG; break;
      case ACR_VR_FL:
         value_size = ACR_SIZEOF_FLOAT; break;
      case ACR_VR_FD:
         value_size = ACR_SIZEOF_DOUBLE; break;
      default:
         value_size = 1; break;
      }

      /* Reverse byte order */
      if (value_size > 1) {
         if (element_byte_order != byte_order) {
            nvalues = acr_get_element_length(element) / value_size;
            acr_reverse_byte_order(nvalues, value_size, 
                                   acr_get_element_data(element), NULL);
         }
      }
   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_match_element_id
@INPUT      : elid - element id to check
              element - element to check
@OUTPUT     : (none)
@RETURNS    : TRUE if the ids match, FALSE otherwise
@DESCRIPTION: Compares the group and element id of an element id structure to
              that of an element. Returns TRUE if they are the same.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 12, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int acr_match_element_id(Acr_Element_Id elid,
                                Acr_Element element)
{
   return ((elid->group_id == acr_get_element_group(element)) &&
           (elid->element_id == acr_get_element_element(element)));
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_find_element_id
@INPUT      : elid - element id to check
              element_list - element list to search
@OUTPUT     : (none)
@RETURNS    : Pointer to element found or NULL
@DESCRIPTION: Searches an element list for an element id. As a side effect,
              the VR type of the element is set if it is previously unknown
              and it is defined in the element id structure.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 12, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Element acr_find_element_id(Acr_Element element_list,
                                       Acr_Element_Id elid)
{
   Acr_Element element;

   /* Look for the element */
   element = element_list;
   while ((element != NULL) && !acr_match_element_id(elid, element)) {
      element = acr_get_element_next(element);
   }

   /* Set the VR type if it is unknown */
   if ((element != NULL) &&
       (acr_get_element_vr(element) == ACR_VR_UNKNOWN) &&
       (elid->vr_code != ACR_VR_UNKNOWN)) {
      acr_set_element_vr(element, elid->vr_code);
   }

   return element;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_memdup
@INPUT      : value_size - number of bytes to copy
              value - value to copy
@OUTPUT     : (none)
@RETURNS    : Pointer to copy of data
@DESCRIPTION: Allocates memory and makes a copy of some memory
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 14, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void *acr_memdup(size_t value_size, void *value)
{
   char *copy, *original;
   size_t i;

   original = (char *) value;
   copy = (char *) MALLOC(value_size);
   for (i=0; i<value_size; i++) {
      copy[i] = original[i];
   }
   return (void *) copy;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_element_mem
@INPUT      : elid
              vr_code - internal VR code for element
              value_size
              value
@OUTPUT     : (none)
@RETURNS    : Pointer to element structure
@DESCRIPTION: Creates an acr-nema element structure containing data of the
              specified size. The data is copied into a new memory location.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 14, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Element create_element_mem(Acr_Element_Id elid,
                                       Acr_VR_Type vr_code, 
                                       size_t value_size, void *value)
{
   return acr_create_element(elid->group_id, elid->element_id, vr_code,
                             value_size, acr_memdup(value_size, value));
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_create_element_short
@INPUT      : elid
              value
@OUTPUT     : (none)
@RETURNS    : Pointer to element structure
@DESCRIPTION: Creates an acr-nema element structure containing one short.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 17, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Element acr_create_element_short(Acr_Element_Id elid,
                                            unsigned short value)
{
   return create_element_mem(elid, ACR_VR_US, sizeof(value), (void *) &value);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_create_element_long
@INPUT      : elid
              value
@OUTPUT     : (none)
@RETURNS    : Pointer to element structure
@DESCRIPTION: Creates an acr-nema element structure containing one long.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 17, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Element acr_create_element_long(Acr_Element_Id elid,
                                           long value)
{
   return create_element_mem(elid, ACR_VR_UL, sizeof(value), (void *) &value);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_create_element_numeric
@INPUT      : elid
              value
@OUTPUT     : (none)
@RETURNS    : Pointer to element structure
@DESCRIPTION: Creates an acr-nema element structure containing one ascii 
              numeric.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 17, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Element acr_create_element_numeric(Acr_Element_Id elid,
                                              double value)
{
   char string[256];
   Acr_Element element;

   (void) sprintf(string, "%.15g", value);
   element = acr_create_element_string(elid, string);
   acr_set_element_vr(element, ACR_VR_DS);
   return element;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_create_element_string
@INPUT      : elid
              value
@OUTPUT     : (none)
@RETURNS    : Pointer to element structure
@DESCRIPTION: Creates an acr-nema element structure containing an ascii string.
              Note that the string is duplicated for the element structure.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 17, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Element acr_create_element_string(Acr_Element_Id elid,
                                             char *value)
{
   long data_length;
   long alloc_length;
   char *data;
   int pad;

   /* Get string length and check for an odd length */
   data_length = strlen(value);
   if ((data_length % 2) == 0)
      pad = FALSE;
   else {
      pad = TRUE;
      data_length++;
   }

   /* Allocate the string and copy it */
   alloc_length = data_length + 1;
   data = (char *) MALLOC(alloc_length);
   (void) strcpy(data, value);

   /* Pad the end with a blank if needed */
   if (pad) {
      data[data_length - 1] = ' ';
      data[data_length] = '\0';
   }

   return acr_create_element(elid->group_id, elid->element_id, 
                             ACR_VR_ST, data_length, data);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_create_element_sequence
@INPUT      : elid
              value
@OUTPUT     : (none)
@RETURNS    : Pointer to element structure
@DESCRIPTION: Creates an acr-nema element structure containing a sequence of
              items.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 12, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Element acr_create_element_sequence(Acr_Element_Id elid,
                                               Acr_Element itemlist)
{
   return acr_create_element(elid->group_id, elid->element_id, 
                             ACR_VR_SQ, -1L, (char *) itemlist);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_element_short
@INPUT      : element
@OUTPUT     : (none)
@RETURNS    : Value from element structure
@DESCRIPTION: Gets a single short from an element structure. Returns zero
              if an error occurs.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 17, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public unsigned short acr_get_element_short(Acr_Element element)
{
   unsigned short value;

   value = acr_get_numeric_vr(acr_get_element_vr(element),
                              acr_get_element_byte_order(element),
                              acr_get_element_data(element),
                              acr_get_element_length(element));

   return value;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_element_long
@INPUT      : element
@OUTPUT     : (none)
@RETURNS    : Value from element structure
@DESCRIPTION: Gets a single long from an element structure. Returns zero
              if an error occurs.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 17, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public long acr_get_element_long(Acr_Element element)
{
   long value;

   value = acr_get_numeric_vr(acr_get_element_vr(element),
                              acr_get_element_byte_order(element),
                              acr_get_element_data(element),
                              acr_get_element_length(element));

   return value;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_element_numeric
@INPUT      : element
@OUTPUT     : (none)
@RETURNS    : Value from element structure
@DESCRIPTION: Gets a single ascii numeric from an element structure. 
              Returns zero if an error occurs.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 17, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public double acr_get_element_numeric(Acr_Element element)
{
   double value;

   value = acr_get_numeric_vr(acr_get_element_vr(element),
                              acr_get_element_byte_order(element),
                              acr_get_element_data(element),
                              acr_get_element_length(element));

   return value;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_element_string
@INPUT      : element
@OUTPUT     : (none)
@RETURNS    : Value from element structure
@DESCRIPTION: Gets an string from an element structure. 
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 17, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public char *acr_get_element_string(Acr_Element element)
{

   return acr_get_string_vr(acr_get_element_vr(element),
                            acr_get_element_byte_order(element),
                            acr_get_element_data(element),
                            acr_get_element_length(element));

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_element_short_array
@INPUT      : element
              max_values - maximum number of values to return
@OUTPUT     : values - array of values found
@RETURNS    : Number of values found
@DESCRIPTION: Gets an array of shorts from an element structure. If the 
              number of values in the element is greater than max_values, 
              then only max_values values are extracted, but the total
              number of values in the element is returned.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 17, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public long acr_get_element_short_array(Acr_Element element, long max_values, 
                                        unsigned short values[])
{
   long nvalues;

   /* Check VR of element */
   switch (acr_get_element_vr(element)) {
   case ACR_VR_SS:
   case ACR_VR_US:
   case ACR_VR_OW:
   case ACR_VR_UNKNOWN:
      break;
   default:
      return (long) 0;
   }

   /* Get number of values in element */
   nvalues = acr_get_element_length(element) / ACR_SIZEOF_SHORT;

   /* Check the maximum number of values */
   if (max_values > nvalues) max_values = nvalues;

   /* Get the data */
   acr_get_short(acr_get_element_byte_order(element),
                 max_values, acr_get_element_data(element), values);

   /* Return the number of values in the structure */
   return nvalues;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_element_numeric_array_separator
@INPUT      : character - character to add to list or EOF if not adding 
                 anything
@OUTPUT     : (none)
@RETURNS    : Pointer to array of separators, ending with an EOF
@DESCRIPTION: Adds to the list of separators for a numeric array, or
              returns a pointer to the array which is terminated with an EOF.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 27, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int *acr_element_numeric_array_separator(int character)
{
   static int *separator_list = NULL;
   static int nseparators = 0;
   static int default_list[] = {',', '\\', '/'};
   int isep;

   /* Create the default list */
   if (separator_list == NULL) {
      nseparators = SIZEOF_ARRAY(default_list);
      separator_list = 
         MALLOC((size_t) (nseparators + 1) * sizeof(*separator_list));
      for (isep = 0; isep < nseparators; isep++) {
         separator_list[isep] = default_list[isep];
      }
      separator_list[nseparators] = EOF;
   }

   /* Add the character to the list if it is not already there */
   if (character != EOF) {
      for (isep=0; isep < nseparators; isep++) {
         if (character == separator_list[isep]) break;
      }
      if (character != separator_list[isep]) {
         nseparators++;
         separator_list = 
            REALLOC(separator_list, 
                    (size_t) (nseparators + 1) * sizeof(*separator_list));
         separator_list[nseparators - 1] = character;
         separator_list[nseparators] = EOF;
      }
   }

   /* Return the list */
   return separator_list;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_element_numeric_array
@INPUT      : element
              max_values - maximum number of values to return
@OUTPUT     : values - array of values found
@RETURNS    : Number of values found
@DESCRIPTION: Gets an array of ascii numbers from an element structure. 
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 17, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int acr_get_element_numeric_array(Acr_Element element,
                                         int max_values, double values[])
{
   char *start, *end, *cur, *prev;
   int *separator_list;
   int nvalues, isep;

   /* Get separator list */
   separator_list = acr_element_numeric_array_separator(EOF);

   /* Set up pointers to end of string and first non-space character */
   start = (char *) acr_get_element_data(element);
   end = start + acr_get_element_length(element);
   cur = start;
   while (isspace((int) *cur)) cur++;
   nvalues = 0;

   /* Loop through string looking for numbers */
   while (cur<end) {

      /* Get number */
      prev = cur;
      if (nvalues < max_values) {
         values[nvalues] = strtod(prev, &cur);
      }
      else {
         (void) strtod(prev, &cur);
      }
      if (cur == prev) {
         return nvalues;
      }
      nvalues++;

      /* Skip any spaces */
      while (isspace(*cur)) cur++;

      /* Skip an optional separator */
      for (isep=0; separator_list[isep] != EOF; isep++) {
         if (*cur == separator_list[isep]) {
            cur++;
            break;
         }
      }

   }

   return nvalues;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_dump_element_list
@INPUT      : file_pointer - where output should go
              element_list
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Dump information from an acr-nema element list
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 12, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_dump_element_list(FILE *file_pointer, 
                                  Acr_Element element_list)
{
#define INDENT_AMOUNT 3
   Acr_Element cur_element;
   long element_length;
   int printable;
   int i;
   char *string, copy[1024];
   static current_indent_level = 0;
   Acr_VR_Type vr_code;

   /* Check that we have something to print */
   if (element_list == NULL) return;

   /* Increment indent level */
   current_indent_level += INDENT_AMOUNT;

   /* Loop over elements */
   cur_element = element_list;
   while (cur_element != NULL) {

      /* Indent the line */
      for (i=0; i < current_indent_level; i++) {
         (void) putc((int) ' ', file_pointer);
      }

      /* Print the element id */
      (void) fprintf(file_pointer, 
                     "0x%04x  0x%04x  length = %d :",
                     acr_get_element_group(cur_element),
                     acr_get_element_element(cur_element),
                     (int) acr_get_element_length(cur_element));

      /* Print value if needed */
      vr_code = acr_get_element_vr(cur_element);
      if (acr_element_is_sequence(cur_element)) {
         (void) fprintf(file_pointer, " VR=%s", 
                        acr_get_vr_name(vr_code));
         if (acr_get_element_data(cur_element) == NULL) {
            (void) fprintf(file_pointer, " (empty sequence)");
         }
         else {
            (void) fprintf(file_pointer, " (sequence)");
         }
         (void) putc((int) '\n', file_pointer);
         acr_dump_element_list(file_pointer, 
            (Acr_Element) acr_get_element_data(cur_element));
      }
      else if (vr_code != ACR_VR_UNKNOWN) {
         (void) fprintf(file_pointer, " VR=%s, ",
                        acr_get_vr_name(vr_code));
         switch (vr_code) {
         case ACR_VR_SS:
         case ACR_VR_US:
            (void) fprintf(file_pointer, "short = %d (0x%04x)",
                           (int) acr_get_element_short(cur_element),
                           (int) acr_get_element_short(cur_element));
            break;
         case ACR_VR_AT:
         case ACR_VR_SL:
         case ACR_VR_UL:
            (void) fprintf(file_pointer, "long = %d (0x%08x)",
                           (int) acr_get_element_long(cur_element),
                           (int) acr_get_element_long(cur_element));
            break;
         case ACR_VR_OB:
         case ACR_VR_OW:
            break;
         default:
            (void) fprintf(file_pointer, "value = \"%s\"",
                           acr_get_element_string(cur_element));
            break;
         }
         (void) putc((int) '\n', file_pointer);
      }
      else {
         element_length = acr_get_element_length(cur_element);
         switch (element_length) {
         case ACR_SIZEOF_SHORT:
            (void) fprintf(file_pointer, " short = %d (0x%04x)",
                           (int) acr_get_element_short(cur_element),
                           (int) acr_get_element_short(cur_element));
            break;
         case ACR_SIZEOF_LONG:
            (void) fprintf(file_pointer, " long = %d (0x%08x)",
                           (int) acr_get_element_long(cur_element),
                           (int) acr_get_element_long(cur_element));
            break;
         }

         /* Remove trailing NULs from string */
         string = acr_get_element_string(cur_element);
         while ((element_length > 0) && (string[element_length-1] == '\0')) {
            element_length--;
         }

         /* Print string if short enough and is printable */
         if ((element_length > 0) && (element_length < sizeof(copy)-1)) {
            printable = TRUE;
            for (i=0; i < element_length; i++) {
               if (! isprint((int) string[i])) {
                  printable = FALSE;
                  copy[i] = ' ';
               }
               else if ((string[i] == '\n') ||
                        (string[i] == '\r') ||
                        (string[i] == '\f'))
                  copy[i] = ' ';
               else
                  copy[i] = string[i];
            }
            copy[i] = '\0';

            if (printable) {
               (void) fprintf(file_pointer, " string = \"%s\"", copy);
            }

         }

         /* End line */
         (void) fprintf(file_pointer, "\n");

      }         /* if is_sequence ... else  */
                                                     
      cur_element = acr_get_element_next(cur_element);
   }

   /* Decrement indent level */
   current_indent_level -= INDENT_AMOUNT;

}
