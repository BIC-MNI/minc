/* ----------------------------- MNI Header -----------------------------------
@NAME       : element.c
@DESCRIPTION: Routines for doing acr_nema element operations.
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : $Log: element.c,v $
@MODIFIED   : Revision 1.7  1993-11-30 12:19:15  neelin
@MODIFIED   : Handle MALLOC returning NULL because of extremely large data element length.
@MODIFIED   :
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
#include <minc_def.h>
#include <acr_nema.h>

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_create_element
@INPUT      : group_id 
              element_id
              data_length
              data_pointer
@OUTPUT     : (none)
@RETURNS    : Pointer to element structure
@DESCRIPTION: Creates an acr-nema element structure
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Element acr_create_element(int group_id, int element_id, 
                                      long data_length, char *data_pointer)
{
   Acr_Element element;

   /* Allocate the element */
   element = MALLOC(sizeof(*element));

   /* Assign fields */
   element->group_id = group_id;
   element->element_id = element_id;
   element->data_length = data_length;
   element->data_pointer = data_pointer;
   element->next = NULL;

   return element;
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
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_delete_element(Acr_Element element)
{
   if (element->data_pointer != NULL)
      FREE(element->data_pointer);

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

   /* Loop through the list, deleting elements */
   next = element_list;
   do {
      cur = next;
      next = cur->next;
      acr_delete_element(cur);
   } while (next != NULL);

   return;
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
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Element acr_copy_element(Acr_Element element)
{
   Acr_Element copy;
   long length;
   unsigned char *data;

   /* Copy the data */
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

   /* Create the new element */
   copy = acr_create_element(acr_get_element_group(element), 
                             acr_get_element_element(element),
                             length, (void *) data);

   return copy;
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
@NAME       : acr_set_element_data
@INPUT      : element
              data_length
              data_pointer
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Set data length and pointer of an acr-nema element
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_set_element_data(Acr_Element element,
                                 long data_length, char *data_pointer)
{

   /* Free the old data if needed */
   if (element->data_pointer == NULL) {
      FREE(element->data_pointer);
   }

   element->data_length = data_length;
   element->data_pointer = data_pointer;
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
   return element->data_length;
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
@OUTPUT     : (none)
@RETURNS    : total length for element in ACR-NEMA representation
@DESCRIPTION: Get total length for element in ACR-NEMA representation
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public long acr_get_element_total_length(Acr_Element element)
{
   return element->data_length + 8;
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
   int group_id, element_id;
   long data_length;
   char *data_pointer;
   Acr_Status status;

   *element = NULL;
   status = acr_read_one_element(afp, &group_id, &element_id,
                                 &data_length, &data_pointer);
   if (status == ACR_OK) {
      *element = acr_create_element(group_id, element_id, 
                                    data_length, data_pointer);
   }

   return status;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_output_element
@INPUT      : afp - acr file pointer
              element
@OUTPUT     : (none)
@RETURNS    : status
@DESCRIPTION: Write out an acr-nema element
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Status acr_output_element(Acr_File *afp, Acr_Element element)
{
   return acr_write_one_element(afp, 
                                element->group_id,
                                element->element_id,
                                element->data_length,
                                element->data_pointer);

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
   long data_length;
   void *data;

   data_length = ACR_SIZEOF_SHORT;
   data = MALLOC(data_length);
   acr_put_short((long) 1, &value, data);

   return acr_create_element(elid->group_id, elid->element_id, 
                             data_length, data);
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
   long data_length;
   void *data;

   data_length = ACR_SIZEOF_LONG;
   data = MALLOC(data_length);
   acr_put_long((long) 1, &value, data);

   return acr_create_element(elid->group_id, elid->element_id, 
                             data_length, data);
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

   (void) sprintf(string, "%.15g", value);
   return acr_create_element_string(elid, string);

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
                             data_length, data);
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

   if (acr_get_element_length(element) != ACR_SIZEOF_SHORT) return 0;

   acr_get_short((long) 1, acr_get_element_data(element), &value);
   
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

   if (acr_get_element_length(element) != ACR_SIZEOF_LONG) return 0;

   acr_get_long((long) 1, acr_get_element_data(element), &value);
   
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

   value = atof((char *) acr_get_element_data(element));
   
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

   return (char *) acr_get_element_data(element);
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
   int nvalues, isep;
   static char separator_list[] = {',', '\\'};
   static int nseparators = 
      sizeof(separator_list) / sizeof(separator_list[0]);

   /* Set up pointers to end of string and first non-space character */
   start = (char *) acr_get_element_data(element);
   end = start + strlen(start);
   cur = start;
   while (isspace((int) *cur)) cur++;
   nvalues = 0;

   /* Loop through string looking for numbers */
   while (cur!=end) {

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
      for (isep=0; isep < nseparators; isep++) {
         if (*cur == separator_list[isep]) {
            cur++;
            break;
         }
      }

   }

   return nvalues;
}

