/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_nema_io.c
@DESCRIPTION: Routines for doing basic acr_nema operations (reading and
              writing an element).
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : $Log: acr_io.c,v $
@MODIFIED   : Revision 1.1  1993-11-19 12:47:35  neelin
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
#include <memory.h>
#include <minc_def.h>
#include <acr_nema.h>

/* Define constants */
#if (!defined(TRUE) || !defined(FALSE))
#  define TRUE 1
#  define FALSE 0
#endif
#ifndef private
#  define private static
#endif

/* Private functions */
private int acr_need_invert(void);

/* Private variables */
static int acr_vax_byte_ordering = TRUE;


/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_set_vax_byte_ordering
@INPUT      : has_vax_byte_ordering - TRUE for input with vax ordering, 
              FALSE otherwise.
@OUTPUT     : (none)
@RETURNS    : Previous value
@DESCRIPTION: Allows a user to specify whether input has vax byte ordering
              or not.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int acr_set_vax_byte_ordering(int has_vax_byte_ordering)
{
   int previous;

   previous = acr_vax_byte_ordering;
   acr_vax_byte_ordering = has_vax_byte_ordering;
   return previous;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_need_invert
@INPUT      : (none)
@OUTPUT     : (none)
@RETURNS    : TRUE if need to invert shorts and longs
@DESCRIPTION: Indicates whether we need to swap bytes for shorts and longs.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int acr_need_invert(void)
{

#if (defined(vax))
   return !acr_vax_byte_ordering;
#else
   return acr_vax_byte_ordering;
#endif

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_short
@INPUT      : nvals - number of values to convert to short
              input_value - pointer to array of input values
@OUTPUT     : mach_value - pointer to array of shorts
@RETURNS    : (nothing)
@DESCRIPTION: Converts input values to shorts.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_get_short(long nvals, void *input_value, short *mach_value)
{
   long i;
   char *ptr1, *ptr2, v0, v1;

   /* Check whether a flip is needed */
   if (acr_need_invert()) {
      ptr1 = (char *) input_value;
      ptr2 = (char *) mach_value;
      for (i=0; i<nvals; i++) {
         v0 = ptr1[1];
         v1 = ptr1[0];
         ptr2[0] = v0;
         ptr2[1] = v1;
         ptr1 += ACR_SIZEOF_SHORT;
         ptr2 += sizeof(mach_value[0]);
      }
   }
   else {
      (void) memcpy((void *) mach_value, input_value, 
                    (size_t) (nvals*sizeof(short)));
   }

   return ;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_long
@INPUT      : nvals - number of values to convert to long
              input_value - pointer to array of input values
@OUTPUT     : mach_value - pointer to array of longs
@RETURNS    : (nothing)
@DESCRIPTION: Converts input values to longs.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_get_long(long nvals, void *input_value, long *mach_value)
{
   long i;
   char *ptr1, *ptr2, v0, v1;

   /* Check whether a flip is needed */
   if (acr_need_invert()) {
      ptr1 = (char *) input_value;
      ptr2 = (char *) mach_value;
      for (i=0; i<nvals; i++) {
         v0 = ptr1[3];
         v1 = ptr1[0];
         ptr2[0] = v0;
         ptr2[3] = v1;
         v0 = ptr1[2];
         v1 = ptr1[1];
         ptr2[1] = v0;
         ptr2[2] = v1;
         ptr1 += ACR_SIZEOF_LONG;
         ptr2 += sizeof(mach_value[0]);
      }
   }
   else {
      (void) memcpy((void *) mach_value, input_value, 
                    (size_t) (nvals*sizeof(long)));
   }

   return ;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_put_short
@INPUT      : nvals - number of values to convert from short
              mach_value - pointer to array of shorts
@OUTPUT     : output_value - pointer to array of output values
@RETURNS    : (nothing)
@DESCRIPTION: Converts shorts to output values.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_put_short(long nvals, short *mach_value, void *output_value)
{
   long i;
   char *ptr1, *ptr2, v0, v1;

   /* Check whether a flip is needed */
   if (acr_need_invert()) {
      ptr1 = (char *) mach_value;
      ptr2 = (char *) output_value;
      for (i=0; i<nvals; i++) {
         v0 = ptr1[1];
         v1 = ptr1[0];
         ptr2[0] = v0;
         ptr2[1] = v1;
         ptr1 += sizeof(mach_value[0]);
         ptr2 += ACR_SIZEOF_SHORT;
      }
   }
   else {
      (void) memcpy(output_value, (void *) mach_value, 
                    (size_t) (nvals*sizeof(short)));
   }

   return ;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_put_long
@INPUT      : nvals - number of values to convert from long
              mach_value - pointer to array of longs
@OUTPUT     : output_value - pointer to array of output values
@RETURNS    : (nothing)
@DESCRIPTION: Converts longs to output values.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_put_long(long nvals, long *mach_value, void *output_value)
{
   long i;
   char *ptr1, *ptr2, v0, v1;

   /* Check whether a flip is needed */
   if (acr_need_invert()) {
      ptr1 = (char *) mach_value;
      ptr2 = (char *) output_value;
      for (i=0; i<nvals; i++) {
         v0 = ptr1[3];
         v1 = ptr1[0];
         ptr2[0] = v0;
         ptr2[3] = v1;
         v0 = ptr1[2];
         v1 = ptr1[1];
         ptr2[1] = v0;
         ptr2[2] = v1;
         ptr1 += sizeof(mach_value[0]);
         ptr2 += ACR_SIZEOF_LONG;
      }
   }
   else {
      (void) memcpy(output_value, (void *) mach_value, 
                    (size_t) (nvals*sizeof(long)));
   }

   return ;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_read_one_element
@INPUT      : afp - Acr_File pointer from which to read
@OUTPUT     : group_id - ACR-NEMA group id
              element_id - ACR-NEMA element id
              data_length - length of data to follow
              data_pointer - pointer to data. Space is allocated by this
                 routine. One additional byte is allocated and set to
                 zero so that the data can be treated as a string.
@RETURNS    : Status.
@DESCRIPTION: Routine to read in one ACR-NEMA element.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Status acr_read_one_element(Acr_File *afp,
                                       int *group_id, int *element_id,
                                       long *data_length, char **data_pointer)
{
   int ch, i, buflen;
   unsigned char buffer[2*ACR_SIZEOF_SHORT+ACR_SIZEOF_LONG];
   short grpid, elid;
   size_t size_allocated;

   buflen = sizeof(buffer)/sizeof(buffer[0]);

   /* Read in group id, element id and length of data */
   for (i=0; i < buflen; i++) {
      ch = acr_getc(afp);
      if (ch == EOF) {
         if (i == 0)
            return ACR_END_OF_INPUT;
         else
            return ACR_PROTOCOL_ERROR;
      }
      buffer[i] = ch;
   }
   acr_get_short(1, &buffer[0], &grpid);
   *group_id = grpid;
   acr_get_short(1, &buffer[2], &elid);
   *element_id = elid;
   acr_get_long(1, &buffer[4], data_length);

   /* Allocate space for the data and null-terminate it */
   size_allocated = *data_length + 1;
   *data_pointer = MALLOC(size_allocated);
   (*data_pointer)[*data_length] = '\0';

   /* Read in the data */
   for (i=0; i < *data_length; i++) {
      ch = acr_getc(afp);
      if (ch == EOF) {
         FREE(*data_pointer);
         return ACR_PROTOCOL_ERROR;
      }
      (*data_pointer)[i] = ch;
   }

   return ACR_OK;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_write_one_element
@INPUT      : afp - Acr_File pointer from which to read
              group_id - ACR-NEMA group id
              element_id - ACR-NEMA element id
              data_length - length of data to follow
              data_pointer - pointer to data.
@OUTPUT     : (nothing)
@RETURNS    : Status.
@DESCRIPTION: Routine to write out one ACR-NEMA element.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Status acr_write_one_element(Acr_File *afp,
                                        int group_id, int element_id,
                                        long data_length, char *data_pointer)
{
   int ch, i, buflen;
   unsigned char buffer[2*ACR_SIZEOF_SHORT+ACR_SIZEOF_LONG];
   short grpid, elid;

   buflen = sizeof(buffer)/sizeof(buffer[0]);

   /* Write out group id, element id and length of data */
   grpid = group_id;
   acr_put_short(1, &grpid, &buffer[0]);
   elid = element_id;
   acr_put_short(1, &elid, &buffer[2]);
   acr_put_long(1, &data_length, &buffer[4]);
   for (i=0; i < buflen; i++) {
      ch = acr_putc(buffer[i], afp);
      if (ch == EOF) {
         return ACR_OTHER_ERROR;
      }
   }

   /* Write out the data */
   for (i=0; i < data_length; i++) {
      ch = acr_putc(data_pointer[i], afp);
      if (ch == EOF) {
         return ACR_OTHER_ERROR;
      }
   }

   return ACR_OK;
}

