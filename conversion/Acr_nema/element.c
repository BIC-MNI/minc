/* ----------------------------- MNI Header -----------------------------------
@NAME       : element.c
@DESCRIPTION: Routines for doing acr_nema element operations.
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : $Log: element.c,v $
@MODIFIED   : Revision 1.1  1993-11-19 12:48:12  neelin
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
@INPUT      : group_id 
              element_id
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
public Acr_Element acr_create_element_short(int group_id, int element_id, 
                                            short value)
{
   long data_length;
   void *data;

   data_length = ACR_SIZEOF_SHORT;
   data = MALLOC(data_length);
   acr_put_short((long) 1, &value, data);

   return acr_create_element(group_id, element_id, data_length, data);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_create_element_long
@INPUT      : group_id 
              element_id
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
public Acr_Element acr_create_element_long(int group_id, int element_id, 
                                           long value)
{
   long data_length;
   void *data;

   data_length = ACR_SIZEOF_LONG;
   data = MALLOC(data_length);
   acr_put_long((long) 1, &value, data);

   return acr_create_element(group_id, element_id, data_length, data);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_create_element_numeric
@INPUT      : group_id 
              element_id
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
public Acr_Element acr_create_element_numeric(int group_id, int element_id, 
                                              double value)
{
   char string[256];
   long data_length;
   void *data;

   (void) sprintf(string, "%.15g", value);
   data_length = strlen(string);
   data = strdup(string);

   return acr_create_element(group_id, element_id, data_length, data);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_create_element_string
@INPUT      : group_id 
              element_id
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
public Acr_Element acr_create_element_string(int group_id, int element_id, 
                                             char *value)
{
   long data_length;
   void *data;

   data_length = strlen(value);
   data = strdup(value);

   return acr_create_element(group_id, element_id, data_length, data);
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
public short acr_get_element_short(Acr_Element element)
{
   short value;

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

