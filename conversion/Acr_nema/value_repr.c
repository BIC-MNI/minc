/* ----------------------------- MNI Header -----------------------------------
@NAME       : value_repr.c
@DESCRIPTION: Routines for doing acr_nema VR and value conversion operations.
@METHOD     : 
@GLOBALS    : 
@CREATED    : January 31, 1997 (Peter Neelin)
@MODIFIED   : $Log: value_repr.c,v $
@MODIFIED   : Revision 4.0  1997-05-07 20:01:23  neelin
@MODIFIED   : Release of minc version 0.4
@MODIFIED   :
 * Revision 1.2  1997/04/21  20:21:09  neelin
 * Updated the library to handle dicom messages.
 *
 * Revision 1.1  1997/02/11  16:23:43  neelin
 * Initial revision
 *
@COPYRIGHT  :
              Copyright 1997 Peter Neelin, McConnell Brain Imaging Centre, 
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

/* Types for VR table entries and conversion functions */
typedef struct Acr_VR_Entry Acr_VR_Entry;
typedef double (*Acr_VR_Numeric_Function)
     (Acr_VR_Entry *vr_entry, Acr_byte_order byte_order, 
      char *data, long data_length);
typedef char * (*Acr_VR_String_Function)
     (Acr_VR_Entry *vr_entry, Acr_byte_order byte_order, 
      char *data, long data_length);

struct Acr_VR_Entry {
   Acr_VR_Type vr_code;
   char *vr_name;
   Acr_VR_Numeric_Function convert_to_numeric;
   Acr_VR_String_Function convert_to_string;
};

/* Private functions */
private void check_table_integrity();
private Acr_VR_Entry *get_vr_entry(Acr_VR_Type vr_code);
private Acr_VR_Type find_vr_name(char *vr_name);
private double return_zero(Acr_VR_Entry *vr_entry, 
                           Acr_byte_order byte_order,
                           char *data, long data_length);
private double string_to_numeric(Acr_VR_Entry *vr_entry, 
                                 Acr_byte_order byte_order,
                                 char *data, long data_length);
private double get_short(Acr_VR_Entry *vr_entry, 
                         Acr_byte_order byte_order,
                         char *data, long data_length);
private double get_long(Acr_VR_Entry *vr_entry, 
                        Acr_byte_order byte_order,
                        char *data, long data_length);
private double get_float(Acr_VR_Entry *vr_entry, 
                         Acr_byte_order byte_order,
                         char *data, long data_length);
private double get_double(Acr_VR_Entry *vr_entry, 
                          Acr_byte_order byte_order,
                          char *data, long data_length);
private double guess_numeric_type(Acr_VR_Entry *vr_entry, 
                                  Acr_byte_order byte_order,
                                  char *data, long data_length);
private void extend_internal_buffer(int length);
private char *return_empty_string(Acr_VR_Entry *vr_entry, 
                                  Acr_byte_order byte_order,
                                  char *data, long data_length);
private char *return_the_string(Acr_VR_Entry *vr_entry, 
                                Acr_byte_order byte_order,
                                char *data, long data_length);
private char *numeric_to_string(Acr_VR_Entry *vr_entry, 
                                Acr_byte_order byte_order,
                                char *data, long data_length);

/* Table of VRs and conversion routines */
static Acr_VR_Entry VR_table[] = {
   {ACR_VR_UNKNOWN, "\0\0", guess_numeric_type, return_the_string },
   {ACR_VR_AE, "AE",        return_zero,        return_the_string },
   {ACR_VR_AS, "AS",        string_to_numeric,  return_the_string },
   {ACR_VR_AT, "AT",        get_long,           numeric_to_string },
   {ACR_VR_CS, "CS",        string_to_numeric,  return_the_string },
   {ACR_VR_DA, "DA",        string_to_numeric,  return_the_string },
   {ACR_VR_DS, "DS",        string_to_numeric,  return_the_string },
   {ACR_VR_DT, "DT",        string_to_numeric,  return_the_string },
   {ACR_VR_FL, "FL",        get_float,          numeric_to_string },
   {ACR_VR_FD, "FD",        get_double,         numeric_to_string },
   {ACR_VR_IS, "IS",        string_to_numeric,  return_the_string },
   {ACR_VR_LO, "LO",        string_to_numeric,  return_the_string },
   {ACR_VR_LT, "LT",        string_to_numeric,  return_the_string },
   {ACR_VR_OB, "OB",        return_zero,        return_empty_string },
   {ACR_VR_OW, "OW",        return_zero,        return_empty_string },
   {ACR_VR_PN, "PN",        return_zero,        return_the_string },
   {ACR_VR_SH, "SH",        string_to_numeric,  return_the_string },
   {ACR_VR_SL, "SL",        get_long,           numeric_to_string },
   {ACR_VR_SQ, "SQ",        return_zero,        return_empty_string },
   {ACR_VR_SS, "SS",        get_short,          numeric_to_string },
   {ACR_VR_ST, "ST",        string_to_numeric,  return_the_string },
   {ACR_VR_TM, "TM",        string_to_numeric,  return_the_string },
   {ACR_VR_UI, "UI",        return_zero,        return_the_string },
   {ACR_VR_UL, "UL",        get_long,           numeric_to_string },
   {ACR_VR_US, "US",        get_short,          numeric_to_string },
   {ACR_VR_NUM_TYPES, NULL, NULL,               NULL}
};

/* Table for unknown VRs */
static Acr_VR_Entry *unknown_VR_table = NULL;
static int unknown_VR_table_length = 0;
static int table_integrity_checked = FALSE;

/* Macros */
#define CHECK_TABLE_INTEGRITY \
{if (!table_integrity_checked) {check_table_integrity();}}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : check_table_integrity
@INPUT      : (none)
@OUTPUT     : (none)
@RETURNS    : (none)
@DESCRIPTION: Checks the table for consistency and exits with failure if
              a problem is found.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 31, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void check_table_integrity()
{
   int ientry;

   /* Don't do anything if check has already been performed */
   if (table_integrity_checked) return;

   /* Check size of table */
   if (sizeof(VR_table)/sizeof(VR_table[0]) != ACR_VR_NUM_TYPES+1) {
      (void) fprintf(stderr, "Internal error: VR table size is wrong!\n");
      exit(EXIT_FAILURE);
   }

   /* Check each entry for the correct VR code */
   for (ientry=0; ientry < ACR_VR_NUM_TYPES+1; ientry++) {
      if (VR_table[ientry].vr_code != ientry) {
         (void) fprintf(stderr, "Internal error: VR code mismatch in table\n");
         exit(EXIT_FAILURE);
      }
   }

   /* Set flag */
   table_integrity_checked = TRUE;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_vr_entry
@INPUT      : vr_code - the internal VR code to look up
@OUTPUT     : (none)
@RETURNS    : Pointer to VR table entry
@DESCRIPTION: Looks up a VR table entry in the appropriate table based
              on the internal VR code.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 31, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_VR_Entry *get_vr_entry(Acr_VR_Type vr_code)
{
   CHECK_TABLE_INTEGRITY;

   /* Check to see if we have a bogus vr_code */
   if ((vr_code < 0) || 
       (vr_code >= ACR_VR_NUM_TYPES+unknown_VR_table_length)) {
      return NULL;
   }

   /* Check to see if vr_code is in standard table or not */
   if (vr_code < ACR_VR_NUM_TYPES) {
      return &VR_table[vr_code];
   }
   else {
      return &unknown_VR_table[vr_code-ACR_VR_NUM_TYPES];
   }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : find_vr_name
@INPUT      : vr_name - the 2-letter VR name to look up
@OUTPUT     : (none)
@RETURNS    : Internal VR code
@DESCRIPTION: Looks up a VR table entry in the appropriate table based
              on the 2-letter VR name. If nothing is found, -1 is returned.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 31, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_VR_Type find_vr_name(char *vr_name)
{
   int ientry;

   CHECK_TABLE_INTEGRITY;

   /* Loop through standard VR table */
   for (ientry=0; ientry < ACR_VR_NUM_TYPES; ientry++) {
      if ((VR_table[ientry].vr_name[0] == vr_name[0]) &&
          (VR_table[ientry].vr_name[0] == vr_name[0])) {
         return (Acr_VR_Type) ientry;
      }
   }

   /* Loop through unknown VR table */
   for (ientry=0; ientry < unknown_VR_table_length; ientry++) {
      if ((unknown_VR_table[ientry].vr_name[0] == vr_name[0]) &&
          (unknown_VR_table[ientry].vr_name[0] == vr_name[0])) {
         return (Acr_VR_Type) ientry+ACR_VR_NUM_TYPES;
      }
   }

   return (Acr_VR_Type) -1;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_vr_name
@INPUT      : vr_code - the internal VR code
@OUTPUT     : (none)
@RETURNS    : VR name
@DESCRIPTION: Gets the name corresponding to a VR code, returning a pointer
              to the table string (don't free it!). NULL is returned if the
              code is not found.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 31, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public char *acr_get_vr_name(Acr_VR_Type vr_code)
{
   Acr_VR_Entry *entry;

   if ((entry = get_vr_entry(vr_code)) == NULL) {
      return NULL;
   }
   return entry->vr_name;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_test_vr_name
@INPUT      : vr_name - the 2-letter VR name to look up
@OUTPUT     : (none)
@RETURNS    : TRUE if vr_name is found
@DESCRIPTION: Checks to see if VR name is standard.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 31, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int acr_test_vr_name(char *vr_name)
{
   Acr_VR_Type vr_code;

   vr_code = find_vr_name(vr_name);
   return ((vr_code >= 0) && (vr_code < ACR_VR_NUM_TYPES));
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_lookup_vr_name
@INPUT      : vr_name - the 2-letter VR name to look up
@OUTPUT     : (none)
@RETURNS    : Internal VR code
@DESCRIPTION: Looks up a VR name in the table. If it does not exist, the
              name is added to the unknown table.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 31, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_VR_Type acr_lookup_vr_name(char *vr_name)
{
   Acr_VR_Type vr_code;
   int ientry;
   char *string;

   /* Look up the name and return the matching code if found */
   vr_code = find_vr_name(vr_name);
   if (vr_code >= 0) {
      return vr_code;
   }

   /* If name is not found, add it to the unknown table */
   if (unknown_VR_table_length <= 0) {
      unknown_VR_table_length = 1;
      unknown_VR_table = MALLOC((size_t) unknown_VR_table_length * 
                                sizeof(*unknown_VR_table));
   }
   else {
      unknown_VR_table_length++;
      unknown_VR_table = REALLOC(unknown_VR_table,
                                 (size_t) unknown_VR_table_length * 
                                 sizeof(*unknown_VR_table));
   }

   /* Fill in the entry */
   ientry = unknown_VR_table_length-1;
   unknown_VR_table[ientry].vr_code = ientry + ACR_VR_NUM_TYPES;
   string = MALLOC(3);
   string[0] = vr_name[0];
   string[1] = vr_name[1];
   string[2] = '\0';
   unknown_VR_table[ientry].vr_name = string;
   unknown_VR_table[ientry].convert_to_numeric = return_zero;
   unknown_VR_table[ientry].convert_to_string = return_empty_string;

   /* Return the code */
   return unknown_VR_table[ientry].vr_code;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_numeric_vr
@INPUT      : vr_code - the internal VR code
              byte_order - ACR_BIG_ENDIAN or ACR_LITTLE_ENDIAN
              data - the data to convert
              data_length - the length of the data
@OUTPUT     : (none)
@RETURNS    : The converted value.
@DESCRIPTION: Convert a value to a number according to its VR.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 31, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public double acr_get_numeric_vr(Acr_VR_Type vr_code, 
                                 Acr_byte_order byte_order,
                                 char *data, long data_length)
{
   Acr_VR_Entry *entry;

   if ((entry = get_vr_entry(vr_code)) == NULL) {
      return return_zero(entry, byte_order, data, data_length);
   }
   return entry->convert_to_numeric(entry, byte_order, data, data_length);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_get_string_vr
@INPUT      : vr_code - the internal VR code
              byte_order - ACR_BIG_ENDIAN or ACR_LITTLE_ENDIAN
              data - the data to convert
              data_length - the length of the data
@OUTPUT     : (none)
@RETURNS    : The converted value.
@DESCRIPTION: Convert a value to a string according to its VR. The pointer
              returned is to an internal buffer.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 31, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public char *acr_get_string_vr(Acr_VR_Type vr_code, 
                               Acr_byte_order byte_order,
                               char *data, long data_length)
{
   Acr_VR_Entry *entry;

   if ((entry = get_vr_entry(vr_code)) == NULL) {
      return return_empty_string(entry, byte_order, data, data_length);
   }
   return entry->convert_to_string(entry, byte_order, data, data_length);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : return_zero
              string_to_numeric
              get_short
              get_long
              get_float
              get_double
@INPUT      : vr_entry - pointer to VR table entry
              byte_order - ACR_BIG_ENDIAN or ACR_LITTLE_ENDIAN
              data - a pointer to the actual data
              data_length - number of bytes in the data
@OUTPUT     : (none)
@RETURNS    : Numeric equivalent of data
@DESCRIPTION: Routines to convert values from a DICOM form to a number
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 31, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */

/* ARGSUSED */
private double return_zero(Acr_VR_Entry *vr_entry, 
                           Acr_byte_order byte_order,
                           char *data, long data_length)
{
   return 0.0;
}

/* ARGSUSED */
private double string_to_numeric(Acr_VR_Entry *vr_entry, 
                                 Acr_byte_order byte_order,
                                 char *data, long data_length)
{
   return atof((char *) data);
}

/* ARGSUSED */
private double get_short(Acr_VR_Entry *vr_entry, 
                         Acr_byte_order byte_order,
                         char *data, long data_length)
{
   unsigned short value;

   if (data_length == ACR_SIZEOF_SHORT) {
      acr_get_short(byte_order, 1, data, &value);
      return (double) value;
   }
   else {
      return 0.0;
   }
}

/* ARGSUSED */
private double get_long(Acr_VR_Entry *vr_entry, 
                        Acr_byte_order byte_order,
                        char *data, long data_length)
{
   long value;

   if (data_length == ACR_SIZEOF_LONG) {
      acr_get_long(byte_order, 1, data, &value);
      return (double) value;
   }
   else {
      return 0.0;
   }
}

/* ARGSUSED */
private double get_float(Acr_VR_Entry *vr_entry, 
                         Acr_byte_order byte_order,
                         char *data, long data_length)
{
   float value;

   if (data_length == ACR_SIZEOF_FLOAT) {
      acr_get_float(byte_order, 1, data, &value);
      return (double) value;
   }
   else {
      return 0.0;
   }
}

/* ARGSUSED */
private double get_double(Acr_VR_Entry *vr_entry, 
                          Acr_byte_order byte_order,
                          char *data, long data_length)
{
   double value;

   if (data_length == ACR_SIZEOF_DOUBLE) {
      acr_get_double(byte_order, 1, data, &value);
      return (double) value;
   }
   else {
      return 0.0;
   }
}

/* ARGSUSED */
private double guess_numeric_type(Acr_VR_Entry *vr_entry, 
                                  Acr_byte_order byte_order,
                                  char *data, long data_length)
{
   switch (data_length) {
   case ACR_SIZEOF_SHORT:
      return get_short(vr_entry, byte_order, data, data_length);
      break;
   case ACR_SIZEOF_LONG:
      return get_long(vr_entry, byte_order, data, data_length);
      break;
   default:
      return string_to_numeric(vr_entry, byte_order, data, data_length);
   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : return_empty_string
              return_the_string
              numeric_to_string
@INPUT      : vr_entry - pointer to VR table entry
              byte_order - ACR_BIG_ENDIAN or ACR_LITTLE_ENDIAN
              data - a pointer to the actual data
              data_length - number of bytes in the data
@OUTPUT     : (none)
@RETURNS    : String equivalent of data
@DESCRIPTION: Routines to convert values from a DICOM form to a string.
              If temporary space is needed, an internal buffer is used.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 31, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define LINE_LENGTH 256

static char *internal_string_buffer = NULL;
static int length_of_internal_string = 0;

private void extend_internal_buffer(int length)
{

   if (length+1 > length_of_internal_string) {
      length_of_internal_string = length+1;
      if (internal_string_buffer == NULL)
         internal_string_buffer = MALLOC((size_t) length_of_internal_string);
      else
         internal_string_buffer = REALLOC(internal_string_buffer,
                                          (size_t) length_of_internal_string);
   }
}

/* ARGSUSED */
private char *return_empty_string(Acr_VR_Entry *vr_entry, 
                                  Acr_byte_order byte_order,
                                  char *data, long data_length)
{
   extend_internal_buffer(LINE_LENGTH);

   internal_string_buffer[0] = '\0';

   return internal_string_buffer;
}

/* ARGSUSED */
private char *return_the_string(Acr_VR_Entry *vr_entry, 
                                Acr_byte_order byte_order,
                                char *data, long data_length)
{
   return (char *) data;
}

/* ARGSUSED */
private char *numeric_to_string(Acr_VR_Entry *vr_entry, 
                                Acr_byte_order byte_order,
                                char *data, long data_length)
{
   extend_internal_buffer(LINE_LENGTH);
   (void) sprintf(internal_string_buffer, "%.6g",
                  vr_entry->convert_to_numeric(vr_entry, byte_order, 
                                               data, data_length));
   return internal_string_buffer;
}

