/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_io.c
@DESCRIPTION: Routines for doing basic acr_nema operations (reading and
              writing an element).
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
 * $Log: acr_io.c,v $
 * Revision 6.4  2000-05-01 17:54:02  neelin
 * Improved testing of input stream to figure out byte order for both
 * implicit and expicit VR.
 *
 * Revision 6.3  2000/04/28 15:03:10  neelin
 * Added support for ignoring non-fatal protocol errors (cases where redundant
 * information is inconsistent). In particular, it is possible to ignore
 * differences between the group length element and the true group length.
 *
 * Revision 6.2  1999/10/29 17:51:49  neelin
 * Fixed Log keyword
 *
 * Revision 6.1  1999/10/27 20:13:15  neelin
 * Generalized acr_test_byte_order to recognize groups without a length element.
 *
 * Revision 6.0  1997/09/12 13:23:59  neelin
 * Release of minc version 0.6
 *
 * Revision 5.1  1997/09/08  21:53:31  neelin
 * Added status ACR_CONNECTION_TIMEDOUT.
 *
 * Revision 5.0  1997/08/21  13:25:00  neelin
 * Release of minc version 0.5
 *
 * Revision 4.1  1997/07/10  17:14:38  neelin
 * Added more status codes and function to return status string.
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
 * Revision 2.1  1995/02/08  21:16:06  neelin
 * Changes to make irix 5 lint happy.
 *
 * Revision 2.0  1994/09/28  10:36:06  neelin
 * Release of minc version 0.2
 *
 * Revision 1.9  94/09/28  10:35:39  neelin
 * Pre-release
 * 
 * Revision 1.8  94/09/23  16:42:35  neelin
 * Changed acr_nema_io to acr_io and acr_nema_test to acr_test.
 * 
 * Revision 1.7  94/05/18  08:47:43  neelin
 * Changed some ACR_OTHER_ERROR's to ACR_ABNORMAL_END_OF_OUTPUT.
 * 
 * Revision 1.6  94/04/07  10:03:40  neelin
 * Added status ACR_ABNORMAL_END_OF_INPUT and changed some ACR_PROTOCOL_ERRORs
 * to that or ACR_OTHER_ERROR.
 * Added #ifdef lint to DEFINE_ELEMENT.
 * 
 * Revision 1.5  94/01/06  13:30:57  neelin
 * Changed acr_need_invert to a public function.
 * 
 * Revision 1.4  93/11/30  12:18:34  neelin
 * Handle MALLOC returning NULL because of extremely large data element length.
 * 
 * Revision 1.3  93/11/25  10:34:34  neelin
 * Added routine to test byte-ordering of input.
 * 
 * Revision 1.2  93/11/24  11:24:48  neelin
 * Changed short to unsigned short.
 * 
 * Revision 1.1  93/11/19  12:47:35  neelin
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

#define ACR_BYTE_ORDER_DEFAULT ACR_LITTLE_ENDIAN

#define ACR_VR_ENCODING_DEFAULT ACR_IMPLICIT_VR

/* Define types */
typedef struct {
   Acr_byte_order byte_order;
   Acr_VR_encoding_type vr_encoding;
   int ignore_nonfatal_protocol_errors;
} *Data_Info;

/* Private functions */
private int test_vr(char vr_to_test[2], char *vr_list[]);
private int is_sequence_vr(char vr_to_test[2]);
private int is_special_vr(char vr_to_test[2]);
private Data_Info get_data_info(Acr_File *afp);
private void invert_values(Acr_byte_order byte_order, 
                           long nvals, size_t value_size, 
                           void *input_value, void *mach_value);

/* Macros */
#define SIZEOF_ARRAY(a) (sizeof(a)/sizeof(a[0]))

/* ----------------------------- MNI Header -----------------------------------
@NAME       : is_sequence_vr
              is_special_vr
@INPUT      : vr_to_test - Two character array containing value representation
@OUTPUT     : (none)
@RETURNS    : TRUE if vr is in appropriate list
@DESCRIPTION: These routines test VR against various lists. is_sequence_vr
              checks for a sequence and is_special_vr checks for a VR
              with different fields
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 29, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int test_vr(char vr_to_test[2], char *vr_list[])
{
   int found_special, i;

   found_special = FALSE;
   for (i=0; vr_list[i] != NULL; i++) {
      if ((vr_to_test[0] == vr_list[i][0]) &&
          (vr_to_test[1] == vr_list[i][1])) {
         found_special = TRUE;
         break;
      }
   }

   return found_special;
}

private int is_sequence_vr(char vr_to_test[2])
{
   static char *sequence_vrs[] = {"SQ", NULL};
   return test_vr(vr_to_test, sequence_vrs);
}

private int is_special_vr(char vr_to_test[2])
{
   static char *special_vrs[] = {"OB", "OW", "SQ", NULL};
   return test_vr(vr_to_test, special_vrs);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_data_info
@INPUT      : afp - i/o stream
@OUTPUT     : (none)
@RETURNS    : Pointer to data info
@DESCRIPTION: Checks that the i/o stream has the appropriate structure as
              client data and returns a pointer to the structure.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 14, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Data_Info get_data_info(Acr_File *afp)
{
   Data_Info data_info;

   data_info = acr_file_get_client_data(afp);
   if (data_info == NULL) {
      data_info = MALLOC(sizeof(*data_info));
      data_info->byte_order = ACR_BYTE_ORDER_DEFAULT;
      data_info->vr_encoding = ACR_VR_ENCODING_DEFAULT;
      data_info->ignore_nonfatal_protocol_errors = FALSE;
      acr_file_set_client_data(afp, (void *) data_info);
   }
   return data_info;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_set_byte_order
@INPUT      : afp - i/o stream
              byte_order - ACR_LITTLE_ENDIAN or ACR_BIG_ENDIAN.
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Allows a user to set the byte ordering for an i/o stream.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 29, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_set_byte_order(Acr_File *afp, 
                               Acr_byte_order byte_order)
{
   Data_Info data_info;

   /* Get data info pointer */
   data_info = get_data_info(afp);

   /* Set the byte ordering */
   data_info->byte_order = byte_order;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_byte_order
@INPUT      : afp - i/o stream
@OUTPUT     : (none)
@RETURNS    : Byte ordering of stream
@DESCRIPTION: Allows one to get the byte ordering for an i/o stream.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 29, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_byte_order acr_get_byte_order(Acr_File *afp)
{
   Data_Info data_info;

   /* Get data info pointer */
   data_info = get_data_info(afp);

   /* Return the byte ordering */
   return data_info->byte_order;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_machine_byte_order
@INPUT      : (none)
@OUTPUT     : (none)
@RETURNS    : Byte ordering for this machine.
@DESCRIPTION: Gets the byte ordering for the machine on which the program is
              running.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 14, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int acr_get_machine_byte_order(void)
{
   int dummy = 1;
   char *ptr = (char *) &dummy;

   if ((int) ptr[0] == 1)
      return ACR_LITTLE_ENDIAN;
   else if ((int) ptr[sizeof(int)-1] == 1)
      return ACR_BIG_ENDIAN;
   else {
      (void) fprintf(stderr, 
         "Internal error: Cannot figure out machine byte order!\n");
      exit(EXIT_FAILURE);
      return ACR_BIG_ENDIAN;
   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_need_invert
@INPUT      : byte_order - byte_order of foreign data
@OUTPUT     : (none)
@RETURNS    : TRUE if need to invert shorts and longs
@DESCRIPTION: Indicates whether we need to swap bytes for shorts and longs
              to convert between the given byte ordering and the machine
              byte ordering.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : January 29, 1997 (P.N.)
---------------------------------------------------------------------------- */
public int acr_need_invert(Acr_byte_order byte_order)
{
   return (acr_get_machine_byte_order() != byte_order);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_set_vr_encoding
@INPUT      : afp - i/o stream
              vr_encoding - ACR_EXPLICIT_VR or ACR_IMPLICIT_VR
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Allows a user to set the vr encoding type for an i/o stream.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 29, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_set_vr_encoding(Acr_File *afp, 
                                Acr_VR_encoding_type vr_encoding)
{
   Data_Info data_info;

   /* Get data info pointer */
   data_info = get_data_info(afp);

   /* Set the VR encoding */
   data_info->vr_encoding = vr_encoding;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_vr_encoding
@INPUT      : afp - i/o stream
@OUTPUT     : (none)
@RETURNS    : VR encoding of stream
@DESCRIPTION: Allows one to get the vr encoding for an i/o stream
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 29, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_VR_encoding_type acr_get_vr_encoding(Acr_File *afp)
{
   Data_Info data_info;

   /* Get data info pointer */
   data_info = get_data_info(afp);

   /* Return the VR encoding */
   return data_info->vr_encoding;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_set_ignore_errors
@INPUT      : afp - i/o stream
              ignore_nonfatal_protocol_errors - if TRUE then non-fatal
                 protocol errors will be ignored
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Allows a user to indicate whether to ignore protocol errors
              that can be ignored.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 28, 2000 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_set_ignore_errors(Acr_File *afp, 
                                  int ignore_nonfatal_protocol_errors)
{
   Data_Info data_info;

   /* Get data info pointer */
   data_info = get_data_info(afp);

   /* Set the flag */
   data_info->ignore_nonfatal_protocol_errors = 
      ignore_nonfatal_protocol_errors;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_ignore_protocol_errors
@INPUT      : afp - i/o stream
@OUTPUT     : (none)
@RETURNS    : TRUE if stream is set to ignore nonfatal protocol errors
@DESCRIPTION: Allows one to get the ignore errors flag for a stream
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 28, 2000 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int acr_ignore_protocol_errors(Acr_File *afp)
{
   Data_Info data_info;

   /* Get data info pointer */
   data_info = get_data_info(afp);

   /* Return the VR encoding */
   return data_info->ignore_nonfatal_protocol_errors;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_reverse_byte_order
@INPUT      : nvals - number of values to invert
              value_size - length of each value
              input_values - pointer to array of input values
@OUTPUT     : output_values - pointer to array of inverted values or NULL
@RETURNS    : (nothing)
@DESCRIPTION: Reverses byte-ordering of an array of values. Will reverse
              an array in place if input_values and output_values point to
              the same array or if output_values is NULL.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 14, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_reverse_byte_order(long nvals, size_t value_size, 
                                   void *input_values, void *output_values)
{
   long i, jlow, jhigh;
   char *ptr1, *ptr2, v0, v1;
   int nbytes;

   /* Get data pointers and check whether output_values is NULL */
   ptr1 = (char *) input_values;
   ptr2 = (char *) output_values;
   if (ptr2 == NULL) ptr2 = ptr1;

   /* Copy values from both ends at the same time and stop in the middle */
   nbytes = (value_size+1)/2;
   for (i=0; i<nvals; i++) {
      for (jlow=0; jlow<nbytes; jlow++) {
         jhigh = value_size - jlow - 1;
         v0 = ptr1[jhigh];
         v1 = ptr1[jlow];
         ptr2[jlow] = v0;
         ptr2[jhigh] = v1;
      }
      ptr1 += value_size;
      ptr2 += value_size;
   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : invert_values
@INPUT      : byte_order - byte ordering for input values
              nvals - number of values to invert
              value_size - length of each value
              input_value - pointer to array of input values
@OUTPUT     : mach_value - pointer to array of inverted values
@RETURNS    : (nothing)
@DESCRIPTION: Reverses byte-ordering of an array of values to match machine
              byte order if necessary, otherwise the values are just copied.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 31, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void invert_values(Acr_byte_order byte_order, 
                           long nvals, size_t value_size, 
                           void *input_value, void *mach_value)
{
   long i;
   char *ptr1, *ptr2;

   /* Check whether a flip is needed */
   if (acr_need_invert(byte_order)) {
      acr_reverse_byte_order(nvals, value_size, input_value, mach_value);
   }
   else {
      ptr1 = (char *) input_value;
      ptr2 = (char *) mach_value;
      for (i=0; i<nvals*value_size; i++) {
         ptr2[i] = ptr1[i];
      }
   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_short
@INPUT      : byte_order - byte ordering for input values
              nvals - number of values to convert to short
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
public void acr_get_short(Acr_byte_order byte_order, 
                          long nvals, void *input_value, 
                          unsigned short *mach_value)
{
   invert_values(byte_order, nvals, (size_t) ACR_SIZEOF_SHORT, 
                 input_value, mach_value);
   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_long
@INPUT      : byte_order - byte ordering for input values
              nvals - number of values to convert to long
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
public void acr_get_long(Acr_byte_order byte_order, 
                         long nvals, void *input_value, long *mach_value)
{
   invert_values(byte_order, nvals, (size_t) ACR_SIZEOF_LONG, 
                 input_value, mach_value);
   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_float
@INPUT      : byte_order - byte ordering for input values
              nvals - number of values to convert to float
              input_value - pointer to array of input values
@OUTPUT     : mach_value - pointer to array of floats
@RETURNS    : (nothing)
@DESCRIPTION: Converts input values to floats. This will only work properly
              on machines that support IEEE floating-point representation.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 4, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_get_float(Acr_byte_order byte_order, 
                          long nvals, void *input_value, float *mach_value)
{
   invert_values(byte_order, nvals, (size_t) ACR_SIZEOF_FLOAT, 
                 input_value, mach_value);
   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_double
@INPUT      : byte_order - byte ordering for input values
              nvals - number of values to convert to double
              input_value - pointer to array of input values
@OUTPUT     : mach_value - pointer to array of doubles
@RETURNS    : (nothing)
@DESCRIPTION: Converts input values to doubles. This will only work properly
              on machines that support IEEE floating-point representation.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 4, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_get_double(Acr_byte_order byte_order, 
                           long nvals, void *input_value, double *mach_value)
{
   invert_values(byte_order, nvals, (size_t) ACR_SIZEOF_DOUBLE, 
                 input_value, mach_value);
   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_put_short
@INPUT      : byte_order - byte ordering for output values
              nvals - number of values to convert from short
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
public void acr_put_short(Acr_byte_order byte_order, 
                          long nvals, unsigned short *mach_value, 
                          void *output_value)
{
   invert_values(byte_order, nvals, (size_t) ACR_SIZEOF_SHORT, 
                 mach_value, output_value);
   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_put_long
@INPUT      : byte_order - byte ordering for output values
              nvals - number of values to convert from long
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
public void acr_put_long(Acr_byte_order byte_order, 
                         long nvals, long *mach_value, void *output_value)
{
   invert_values(byte_order, nvals, (size_t) ACR_SIZEOF_LONG, 
                 mach_value, output_value);
   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_put_float
@INPUT      : byte_order - byte ordering for output values
              nvals - number of values to convert from float
              mach_value - pointer to array of longs
@OUTPUT     : output_value - pointer to array of output values
@RETURNS    : (nothing)
@DESCRIPTION: Converts floats to output values.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_put_float(Acr_byte_order byte_order, 
                          long nvals, float *mach_value, void *output_value)
{
   invert_values(byte_order, nvals, (size_t) ACR_SIZEOF_FLOAT, 
                 mach_value, output_value);
   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_put_double
@INPUT      : byte_order - byte ordering for output values
              nvals - number of values to convert from double
              mach_value - pointer to array of longs
@OUTPUT     : output_value - pointer to array of output values
@RETURNS    : (nothing)
@DESCRIPTION: Converts doubles to output values.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_put_double(Acr_byte_order byte_order, 
                           long nvals, double *mach_value, void *output_value)
{
   invert_values(byte_order, nvals, (size_t) ACR_SIZEOF_DOUBLE, 
                 mach_value, output_value);
   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_skip_input_data
@INPUT      : afp
              nbytes_to_skip
@OUTPUT     : (none)
@RETURNS    : Input status. If an error occurs on the first byte, then
              ACR_END_OF_INPUT is returned, if an error occurs elsewhere,
              then ACR_ABNORMAL_END_OF_INPUT is returned, otherwise ACR_OK
              is returned.
@DESCRIPTION: Skips over input data.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 12, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Status acr_skip_input_data(Acr_File *afp, long nbytes_to_skip)
{
   long i;
   int ch;

   for (i=0; i < nbytes_to_skip; i++) {
      ch = acr_getc(afp);
      if (ch == EOF) {
         break;
      }
   }

   /* Return the status */
   if (i >= nbytes_to_skip) {
      return ACR_OK;
   }
   else if (acr_get_io_watchpoint(afp) <= 0) {
      return ACR_REACHED_WATCHPOINT;
   }
   else if (i == 0) {
      return ACR_END_OF_INPUT;
   }
   else {
      return ACR_ABNORMAL_END_OF_INPUT;
   }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_read_buffer
@INPUT      : afp
              nbytes_to_read
@OUTPUT     : buffer
              nbytes_read - if NULL, then this value is ignored, otherwise
                 the number of bytes actually read in is returned.
@RETURNS    : Input status. If an error occurs on the first byte, then
              ACR_END_OF_INPUT is returned, if an error occurs elsewhere,
              then ACR_ABNORMAL_END_OF_INPUT is returned, otherwise ACR_OK
              is returned.
@DESCRIPTION: Reads in a buffer of data and optionally returns the number 
              of bytes read
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 12, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Status acr_read_buffer(Acr_File *afp, unsigned char buffer[],
                                  long nbytes_to_read, long *nbytes_read)
{
   long i;
   int ch;

   for (i=0; i < nbytes_to_read; i++) {
      ch = acr_getc(afp);
      if (ch == EOF) {
         break;
      }
      buffer[i] = (unsigned char) ch;
   }

   /* Save the number of bytes read */
   if (nbytes_read != NULL) {
      *nbytes_read = i;
   }

   /* Return the status */
   if (i >= nbytes_to_read) {
      return ACR_OK;
   }
   else if (acr_get_io_watchpoint(afp) <= 0) {
      return ACR_REACHED_WATCHPOINT;
   }
   else if (i == 0) {
      return ACR_END_OF_INPUT;
   }
   else {
      return ACR_ABNORMAL_END_OF_INPUT;
   }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_unget_buffer
@INPUT      : afp
              nbytes_to_unget
              buffer
@OUTPUT     : (none)
@RETURNS    : Unget status.
@DESCRIPTION: Puts a buffer of data back into the input stream.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 12, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Status acr_unget_buffer(Acr_File *afp, unsigned char buffer[],
                                   long nbytes_to_unget)
{
   long i;

   for (i=nbytes_to_unget-1; i >= 0; i--) {
      if (acr_ungetc((int) buffer[i], afp) == EOF) {
         break;
      }
   }

   /* Return the status */
   if (i >= 0) {
      return ACR_IO_ERROR;
   }
   else {
      return ACR_OK;
   }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_write_buffer
@INPUT      : afp
              nbytes_to_write
              buffer
@OUTPUT     : nbytes_written
@RETURNS    : Output status.
@DESCRIPTION: Writes out a buffer of data and optionally returns the number 
              of bytes written
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 12, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Status acr_write_buffer(Acr_File *afp, unsigned char buffer[],
                                   long nbytes_to_write, long *nbytes_written)
{
   long i;

   for (i=0; i < nbytes_to_write; i++) {
      if (acr_putc(buffer[i], afp) == EOF) {
         break;
      }
   }

   /* Save the number of bytes written */
   if (nbytes_written != NULL) {
      *nbytes_written = i;
   }

   /* Return the status */
   if (i < nbytes_to_write) {
      return ACR_ABNORMAL_END_OF_OUTPUT;
   }
   else {
      return ACR_OK;
   }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_test_byte_order
@INPUT      : afp
@OUTPUT     : (none)
@RETURNS    : status.
@DESCRIPTION: Tests input for byte ordering to use. The test is done by 
              looking at the length of the first element. First a test is
              done for implicit VR, assuming that the length of the first 
              element is less than 64K and greater than zero, and we try 
              the two possible byte orders. If the VR encoding is explicit, 
              then we have two shortwords (2-bytes), both of which are 
              non-zero and the longword (4 bytes) will be greater than 64K. 
              In this case, we test the 2-byte length looking for a length
              that is less than 256 bytes. If that fails, than we revert
              to the original byte order.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : January 29, 1997 (P.N.)
---------------------------------------------------------------------------- */
public Acr_Status acr_test_byte_order(Acr_File *afp)
{
   long buflen;
   unsigned char buffer[2*ACR_SIZEOF_SHORT+ACR_SIZEOF_LONG];
   unsigned long data_length;
   unsigned short data_length2;
   Acr_Status status;
   Acr_byte_order byte_order, old_byte_order;

#define ACR_TEST_MAX USHRT_MAX
#define ACR_TEST_MAX2 UCHAR_MAX

   /* Save old byte ordering */
   old_byte_order = acr_get_byte_order(afp);

   /* Read in group id, element id and length of data */
   status = acr_read_buffer(afp, buffer, SIZEOF_ARRAY(buffer), &buflen);
   if (status != ACR_OK) return status;

   /* Put the characters back */
   status = acr_unget_buffer(afp, buffer, buflen);
   if (status != ACR_OK) return status;

   /* Test data length (the first element should be a group length).
      Try big-endian ordering first. */
   byte_order = ACR_BIG_ENDIAN;
   acr_set_byte_order(afp, byte_order);
   acr_get_long(byte_order, 1, &buffer[2*ACR_SIZEOF_SHORT], 
                (long *) &data_length);

   /* If that doesn't work, set it to little-endian ordering. */
   if (data_length >= ACR_TEST_MAX) {
      byte_order = ACR_LITTLE_ENDIAN;
      acr_set_byte_order(afp, byte_order);
      acr_get_long(byte_order, 1, &buffer[2*ACR_SIZEOF_SHORT], 
                   (long *) &data_length);
   }

   /* If one of them worked, then it means that we have implicit VR 
      encoding since we didn't look for a VR field */
   if (data_length < ACR_TEST_MAX) {
      acr_set_vr_encoding(afp, ACR_IMPLICIT_VR);
   }

   /* Otherwise we probably have explicit vr encoding. */
   else {
      acr_set_vr_encoding(afp, ACR_EXPLICIT_VR);

      /* Check the length in this case to see if it small. The default
         will be little endian. */
      byte_order = ACR_BIG_ENDIAN;
      acr_set_byte_order(afp, byte_order);
      acr_get_short(byte_order, 1, &buffer[3*ACR_SIZEOF_SHORT], 
                   &data_length2);
      if (data_length2 >= ACR_TEST_MAX2) {
         byte_order = ACR_LITTLE_ENDIAN;
         acr_set_byte_order(afp, byte_order);
         acr_get_short(byte_order, 1, &buffer[3*ACR_SIZEOF_SHORT], 
                       &data_length2);
      }
      if (data_length2 >= ACR_TEST_MAX2) {
         acr_set_byte_order(afp, old_byte_order);
      }
   }

   return ACR_OK;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_copy_file_encoding
@INPUT      : afp1 - source stream
@OUTPUT     : afp2 - target stream
@RETURNS    : (nothing)
@DESCRIPTION: Copies the byte ordering and VR encoding from one i/o stream
              to another.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 14, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_copy_file_encoding(Acr_File *afp1, Acr_File *afp2)
{
   acr_set_byte_order(afp2, acr_get_byte_order(afp1));
   acr_set_vr_encoding(afp2, acr_get_vr_encoding(afp1));
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_element_header_size
@INPUT      : vr_name - 2-letter name of Vr
              vr_encoding - ACR_EXPLICIT_VR or ACR_IMPLICIT_VR
@OUTPUT     : (none)
@RETURNS    : length of header
@DESCRIPTION: Calculates the length of the element header (excluding data)
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 4, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int acr_get_element_header_size(char vr_name[2], 
                                       Acr_VR_encoding_type vr_encoding)
{
   int length;

   length = 2*ACR_SIZEOF_SHORT + ACR_SIZEOF_LONG;
   if ((vr_encoding == ACR_EXPLICIT_VR) && is_special_vr(vr_name)) {
      length += ACR_SIZEOF_LONG;
   }
   return length;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_peek_at_next_element_id
@INPUT      : afp - Acr_File pointer from which to read
@OUTPUT     : group_id
              element_id 
@RETURNS    : Status
@DESCRIPTION: Peeks ahead to get the group and element ids of the next 
              element. The file position is restored. If a read error occurs,
              then group_id and element_id are set to INT_MIN and the status
              is returned.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 5, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Status acr_peek_at_next_element_id(Acr_File *afp,
                                              int *group_id, int *element_id)
{
   long buflen;
   unsigned char buffer[2*ACR_SIZEOF_SHORT];
   unsigned short svalue;
   Acr_Status status, status2;
   Acr_byte_order byte_order;

   /* Set default values */
   status = ACR_OK;
   *group_id = INT_MIN;
   *element_id = INT_MIN;

   /* Read in the values */
   status = acr_read_buffer(afp, buffer, SIZEOF_ARRAY(buffer), &buflen);

   /* Put them back */
   status2 = acr_unget_buffer(afp, buffer, buflen);
   if (status == ACR_OK) status = status2;

   /* Check for input error */
   if (status != ACR_OK) return status;

   /* Get the id's */
   byte_order = acr_get_byte_order(afp);
   acr_get_short(byte_order, 1, &buffer[0], &svalue);
   *group_id = svalue;
   acr_get_short(byte_order, 1, &buffer[ACR_SIZEOF_SHORT], &svalue);
   *element_id = svalue;

   return status;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_read_one_element
@INPUT      : afp - Acr_File pointer from which to read
@OUTPUT     : group_id - ACR-NEMA group id
              element_id - ACR-NEMA element id
              vr_name - 2 character string giving value representation.
                 Two NULs are returned if VR is unknown.
              data_length - length of data to follow. Value 
                 ACR_VARIABLE_LENGTH is returned for undefined length elements
                 in which case the data portion is not read in.
              data_pointer - pointer to data. Space is allocated by this
                 routine. One additional byte is allocated and set to
                 zero so that the data can be treated as a string. If a
                 sequence is encountered, then NULL is returned.
@RETURNS    : Status.
@DESCRIPTION: Routine to read in one ACR-NEMA element.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : January 29, 1997 (P.N.)
---------------------------------------------------------------------------- */
public Acr_Status acr_read_one_element(Acr_File *afp,
                                       int *group_id, int *element_id,
                                       char vr_name[],
                                       long *data_length, char **data_pointer)
{
   long buflen;
   unsigned char buffer[2*ACR_SIZEOF_SHORT+ACR_SIZEOF_LONG];
   unsigned short grpid, elid, sval;
   unsigned long datalen;
   size_t size_allocated;
   int offset;
   Acr_byte_order byte_order;
   Acr_Status status;

   /* Get byte ordering */
   byte_order = acr_get_byte_order(afp);

   /* Read in group id, element id and length of data */
   status = acr_read_buffer(afp, buffer, SIZEOF_ARRAY(buffer), &buflen);
   if (status != ACR_OK) return status;
   offset = 0;
   acr_get_short(byte_order, 1, &buffer[offset], &grpid);
   offset += ACR_SIZEOF_SHORT;
   *group_id = grpid;
   acr_get_short(byte_order, 1, &buffer[offset], &elid);
   offset += ACR_SIZEOF_SHORT;
   *element_id = elid;

   /* Look for VR and length of data */
   if (acr_get_vr_encoding(afp) == ACR_IMPLICIT_VR) {
      vr_name[0] = '\0';
      vr_name[1] = '\0';
      acr_get_long(byte_order, 1, &buffer[offset], (long *) &datalen);
      offset += ACR_SIZEOF_LONG;
   }
   else {
      vr_name[0] = buffer[offset++];
      vr_name[1] = buffer[offset++];
      acr_get_short(byte_order, 1, &buffer[offset], &sval);
      offset += ACR_SIZEOF_SHORT;
      datalen = sval;
   }

   /* Read in length for special VR's */
   if (is_special_vr(vr_name)) {
      status = acr_read_buffer(afp, buffer, (long) ACR_SIZEOF_LONG, NULL);
      if (status != ACR_OK) return ACR_ABNORMAL_END_OF_INPUT;
      acr_get_long(byte_order, 1, &buffer[0], (long *) &datalen);
   }

   /* Check for undefined length */
   if (datalen == ACR_UNDEFINED_ELEMENT_LENGTH) {
      *data_length = ACR_VARIABLE_LENGTH;
      *data_pointer = NULL;
      return ACR_OK;
   }
   *data_length = datalen;

   /* Check for sequence VR */
   if (is_sequence_vr(vr_name)) {
      *data_pointer = NULL;
      return ACR_OK;
   }
   
   /* Allocate space for the data and null-terminate it */
   size_allocated = *data_length + 1;
   *data_pointer = MALLOC(size_allocated);
   if (*data_pointer == NULL) {
      *data_length = 0;
      size_allocated = *data_length + 1;
      *data_pointer = MALLOC(size_allocated);
   }
   (*data_pointer)[*data_length] = '\0';

   /* Read in the data */
   status = acr_read_buffer(afp, (unsigned char *) *data_pointer, 
                            *data_length, NULL);
   if (status != ACR_OK) {
      FREE(*data_pointer);
      return ACR_ABNORMAL_END_OF_INPUT;
   }

   return ACR_OK;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_write_one_element
@INPUT      : afp - Acr_File pointer from which to read
              group_id - ACR-NEMA group id
              element_id - ACR-NEMA element id
              vr_name - 2 character string giving value representation.
                 It is an error to pass in two NULs if explicit VR is used
                 (ACR_NO_VR_SPECIFIED is returned).
              data_length - length of data to follow. If set to 
                 ACR_VARIABLE_LENGTH, then the data portion is not written out.
              data_pointer - pointer to data. If NULL, then no data is
                 written.
@OUTPUT     : (nothing)
@RETURNS    : Status.
@DESCRIPTION: Routine to write out one ACR-NEMA element.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : January 29, 1997 (P.N.)
---------------------------------------------------------------------------- */
public Acr_Status acr_write_one_element(Acr_File *afp,
                                        int group_id, int element_id,
                                        char vr_name[],
                                        long data_length, char *data_pointer)
{
   long buflen;
   unsigned char buffer[2*ACR_SIZEOF_SHORT+2*ACR_SIZEOF_LONG];
   unsigned short grpid, elid, sval;
   unsigned long datalen;
   int offset;
   Acr_byte_order byte_order;
   Acr_Status status;

   buflen = sizeof(buffer)/sizeof(buffer[0]) - ACR_SIZEOF_LONG;

   /* Get byte ordering */
   byte_order = acr_get_byte_order(afp);

   /* Get the group id and element id */
   offset = 0;
   grpid = (unsigned short) group_id;
   acr_put_short(byte_order, 1, &grpid, &buffer[offset]);
   offset += ACR_SIZEOF_SHORT;
   elid = (unsigned short) element_id;
   acr_put_short(byte_order, 1, &elid, &buffer[offset]);
   offset += ACR_SIZEOF_SHORT;

   /* Check data length */
   if (data_length == ACR_VARIABLE_LENGTH)
      datalen = ACR_UNDEFINED_ELEMENT_LENGTH;
   else
      datalen = data_length;

   /* Check whether we need VR */
   if (acr_get_vr_encoding(afp) == ACR_IMPLICIT_VR) {
      acr_put_long(byte_order, 1, (long *) &datalen, &buffer[offset]);
      offset += ACR_SIZEOF_LONG;
   }
   else {
      if (vr_name[0] == '\0') return ACR_NO_VR_SPECIFIED;
      buffer[offset++] = vr_name[0];
      buffer[offset++] = vr_name[1];
      if (!is_special_vr(vr_name)) {
         sval = (unsigned short) datalen;
         acr_put_short(byte_order, 1, &sval, &buffer[offset]);
         offset += ACR_SIZEOF_SHORT;
      }
      else {
         sval = 0;
         acr_put_short(byte_order, 1, &sval, &buffer[offset]);
         offset += ACR_SIZEOF_SHORT;
         acr_put_long(byte_order, 1, (long *) &datalen, &buffer[offset]);
         offset += ACR_SIZEOF_LONG;
         buflen += ACR_SIZEOF_LONG;
      }
   }

   /* Write it out */
   status = acr_write_buffer(afp, buffer, buflen, NULL);
   if (status != ACR_OK) return status;

   if ((data_length == ACR_VARIABLE_LENGTH) || (data_pointer == NULL)) {
      return ACR_OK;
   }

   /* Write out the data */
   status = acr_write_buffer(afp, (unsigned char *) data_pointer, 
                             data_length, NULL);
   if (status != ACR_OK) return status;

   return ACR_OK;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_status_string
@INPUT      : status - status code to look up
@OUTPUT     : (nothing)
@RETURNS    : Pointer to string describing status.
@DESCRIPTION: Routine to get a string that describes a status value.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : July 10, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public char *acr_status_string(Acr_Status status)
{
   char *status_string;

   switch (status) {
   case ACR_OK:
      status_string = "No error"; break;
   case ACR_END_OF_INPUT:
      status_string = "End of input"; break;
   case ACR_PROTOCOL_ERROR:
      status_string = "Protocol error"; break;
   case ACR_OTHER_ERROR:
      status_string = "Other error"; break;
   case ACR_ABNORMAL_END_OF_INPUT:
      status_string = "Abnormal end of input"; break;
   case ACR_HIGH_LEVEL_ERROR:
      status_string = "High-level error"; break;
   case ACR_ABNORMAL_END_OF_OUTPUT: 
      status_string = "Abnormal end of output"; break;
   case ACR_REACHED_WATCHPOINT:
      status_string = "Reached watchpoint"; break;
   case ACR_IO_ERROR:
      status_string = "I/O error"; break;
   case ACR_NO_VR_SPECIFIED:
      status_string = "VR not specified on output"; break;
   case ACR_PDU_UID_TOO_LONG:
      status_string = "Input PDU UID too long"; break;
   case ACR_CONNECTION_TIMEDOUT:
      status_string = "Connection timed out"; break;
   default:
      status_string = "Unknown status"; break;
   }

   return status_string;
}

