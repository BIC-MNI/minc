/* ----------------------------- MNI Header -----------------------------------
@NAME       : ecat_file.c
@DESCRIPTION: File containing routines to read ECAT image files
@GLOBALS    : 
@CREATED    : January 4, 1996 (Peter Neelin)
@MODIFIED   : 
 * $Log: ecat_file.c,v $
 * Revision 6.2.2.1  2005-02-15 19:59:54  bert
 * Initial checkin on 1.X branch
 *
 * Revision 6.2  2005/01/19 19:46:00  bert
 * Changes from Anthonin Reilhac
 *
 * Revision 6.1  1999/10/29 17:52:01  neelin
 * Fixed Log keyword
 *
 * Revision 6.0  1997/09/12 13:24:22  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:25:21  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:06:04  neelin
 * Release of minc version 0.4
 *
 * Revision 1.1  1996/01/18  14:52:14  neelin
 * Initial revision
 *
@COPYRIGHT  :
              Copyright 1996 Peter Neelin, McConnell Brain Imaging Centre, 
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

#ifndef lint
static char rcsid[]="$Header: /private-cvsroot/minc/conversion/ecattominc/ecat_file.c,v 6.2.2.1 2005-02-15 19:59:54 bert Exp $";
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include "ecat_file.h"
#include "machine_indep.h"

/*#include <vax_conversions.h>*/

/* Set some standard macros */
#ifndef SEEK_SET
#  define SEEK_SET 0
#endif
#ifndef TRUE
#  define TRUE 1
#  define FALSE 0
#endif

/* Constants */
#define BLOCK_SIZE 512
#define MAIN_HEADER_SIZE BLOCK_SIZE
#define SUBHEADER_SIZE BLOCK_SIZE
#define DIRBLOCK_SIZE BLOCK_SIZE
#define MAGIC_STRING "MATRIX"
#define FIRST_DIRBLOCK 2
#define DRBLK_NEXT 1
#define DRBLK_NUMUSED 3
#define DRBLK_SKIP 4
#define DRBLK_WIDTH 4
#define DRBLK_PTR 1

/* Header definition types */
typedef enum {
   ecat_byte, ecat_short, ecat_long, ecat_float, ecat_char
} Ecat_type;

typedef struct {
   Ecat_field_name name;
   int offset;
   int length;
   Ecat_type type;
   char *description;
} Ecat_field_description_type;

typedef struct {
   int initialized;
   int num_entries;
   Ecat_field_description_type *fields;
   Ecat_field_name *file_order;
} Ecat_header_table_type;

typedef struct {
   Ecat_header_table_type *main_header;
   Ecat_header_table_type *subheader;
} Ecat_header_description_type;

/* Header definition */
#include "ecat_header_definition.h"

/* ECAT file Type */
struct Ecat_file {
  FILE *file_pointer;
  Ecat_header_description_type *header_description;
  int vax_byte_order;
  int num_planes;
  int num_frames;
  int num_bed_positions;
  int num_gates;
  int num_volumes;
  unsigned char *main_header;
  long cur_subhdr_offset;
  unsigned char *subheader;
  int num_subhdrs;
  long *subhdr_offsets;
};

typedef enum {
   ECAT_MAIN_HEADER, ECAT_SUBHEADER
} Ecat_which_header;

/* Private functions */
private Ecat_field_name ecat_list_fields(Ecat_file *file, 
                                         Ecat_which_header which_header, 
                                         int index);
private int ecat_get_field_length(Ecat_file *file, 
                                  Ecat_which_header which_header, 
                                  Ecat_field_name field);
private char *ecat_get_field_description(Ecat_file *file, 
                                         Ecat_which_header which_header, 
                                         Ecat_field_name field);
private int ecat_get_value(Ecat_file *file,
                           Ecat_which_header which_header,
                           int volume_number, int slice_number,
                           Ecat_field_name field,
                           int index,
                           int *ivalue, double *fvalue, char *svalue);
private int ecat_read_subhdr(Ecat_file *file, int volume, int slice);
private int ecat_lookup_field(Ecat_header_table_type *table,
                              Ecat_field_name field,
                              int *offset, int *length, Ecat_type *type, 
                              char **description);
private void ecat_initialize_table(Ecat_header_table_type *table);
private int ecat_table_entry_compare(const void *v1, const void *v2);
private int ecat_table_offset_compare(const void *v1, const void *v2);
private int ecat_read_directory(Ecat_file *file);
private long get_dirblock(Ecat_file *file, long *dirblock, int offset);
private int ecat_get_subhdr_offset(Ecat_file *file, int volume, int slice, 
                                   long *offset);



/* ----------------------------- MNI Header -----------------------------------
@NAME       : ecat_open
@INPUT      : filename - name of file to open
@OUTPUT     : (none)
@RETURNS    : Pointer to ECAT file descriptor or NULL if an error occurs.
@DESCRIPTION: Routine to open an ECAT file (for reading only), given 
              its pathname.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 4, 1996 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Ecat_file *ecat_open(char *filename)
{
   Ecat_file *file;

   /* Allocate space for an ecat file structure */
   file = (void *) MALLOC(sizeof(*file));


   file->main_header = (void *) MALLOC(MAIN_HEADER_SIZE);
   file->subheader = (void *) MALLOC(SUBHEADER_SIZE);
   file->cur_subhdr_offset = -1;
   file->subhdr_offsets = NULL;

   /* Open the file */
   if ((file->file_pointer=fopen(filename, "rb")) == NULL) {
      ecat_close(file);
      return NULL;
   }

   /* Read in the main header */
   if (fread(file->main_header, sizeof(char), (size_t) MAIN_HEADER_SIZE, 
             file->file_pointer) != MAIN_HEADER_SIZE) {
      ecat_close(file);
      return NULL;
   }

   /* Figure out which type of file we are using */
   if (strncmp((char *)file->main_header, MAGIC_STRING, 
               strlen(MAGIC_STRING)) == 0) {
      file->header_description = ECAT_VER_7;
      file->vax_byte_order = FALSE;
   }
   else {
      file->header_description = ECAT_VER_PRE7;
      file->vax_byte_order = TRUE;
   }

   /* Get the number of frames, slices, bed positions and gates */
   if (ecat_get_main_value(file, ECAT_Num_Planes, 0, 
                           &file->num_planes, NULL, NULL) ||
       ecat_get_main_value(file, ECAT_Num_Frames, 0, 
                           &file->num_frames, NULL, NULL) ||
       ecat_get_main_value(file, ECAT_Num_Bed_Pos, 0, 
                           &file->num_bed_positions, NULL, NULL) ||
       ecat_get_main_value(file, ECAT_Num_Gates, 0, 
                           &file->num_gates, NULL, NULL)) {
      ecat_close(file);
      return NULL;
   }
   file->num_volumes = file->num_frames;
   if (file->num_volumes < file->num_bed_positions)
      file->num_volumes = file->num_bed_positions;
   if (file->num_volumes < file->num_gates)
      file->num_volumes = file->num_gates;


   /* Read the directory structure */
   if (ecat_read_directory(file)) {
      ecat_close(file);
      return NULL;
   }
   /* Return the file pointer */

   return file;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ecat_close
@INPUT      : file - ecat file pointer
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to close an ECAT file and free the associated structures
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 4, 1996 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void ecat_close(Ecat_file *file)
{
   if (file==NULL) return;
   if (file->file_pointer != NULL) {
      (void) fclose(file->file_pointer);
   }
   if (file->subhdr_offsets != NULL) {
      FREE(file->subhdr_offsets);
   }
   if (file->main_header != NULL) 
      FREE(file->main_header);
   if (file->subheader != NULL) 
      FREE(file->subheader);
   FREE(file);

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ecat_get_num_{planes,frames,bed_positions,gates}
@INPUT      : file - ecat file pointer
@OUTPUT     : (none)
@RETURNS    : Number of * for an ECAT file.
@DESCRIPTION: Routine to get the number of planes (slices), frames, 
              bed_positions or gates in an ECAT file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 4, 1996 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int ecat_get_num_planes(Ecat_file *file)
{
   return file->num_planes;
}

public int ecat_get_num_frames(Ecat_file *file)
{
   return file->num_frames;
}

public int ecat_get_num_bed_positions(Ecat_file *file)
{
   return file->num_bed_positions;
}

public int ecat_get_num_gates(Ecat_file *file)
{
   return file->num_gates;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ecat_list_main
@INPUT      : file - ecat file pointer
              index - index into field list
@OUTPUT     : (none)
@RETURNS    : Next Ecat_field_name value, ECAT_No_Field if index is too large.
@DESCRIPTION: Routine to list the fields in an ECAT main header.
              Should be called repeatedly with increasing values of index
              (starting from 0) until field ECAT_No_Field is returned.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 4, 1996 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Ecat_field_name ecat_list_main(Ecat_file *file, int index)
{
   return ecat_list_fields(file, ECAT_MAIN_HEADER, index);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ecat_list_subhdr
@INPUT      : file - ecat file pointer
              index - index into field list
@OUTPUT     : (none)
@RETURNS    : Next Ecat_field_name value, ECAT_No_Field if index is too large.
@DESCRIPTION: Routine to list the fields in an ECAT subheader.
              Should be called repeatedly with increasing values of index
              (starting from 0) until field ECAT_No_Field is returned.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 4, 1996 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Ecat_field_name ecat_list_subhdr(Ecat_file *file, int index)
{
   return ecat_list_fields(file, ECAT_SUBHEADER, index);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ecat_list_fields
@INPUT      : file - ecat file pointer
              which_header - header in which to look
              index - index into field list
@OUTPUT     : (none)
@RETURNS    : Next Ecat_field_name value, ECAT_No_Field if index is too large.
@DESCRIPTION: Routine to list the fields in an ECAT subheader.
              Should be called repeatedly with increasing values of index
              (starting from 0) until field ECAT_No_Field is returned.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 4, 1996 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Ecat_field_name ecat_list_fields(Ecat_file *file, 
                                         Ecat_which_header which_header, 
                                         int index)
{
   Ecat_header_table_type *table;

   /* Get the appropriate header */
   switch (which_header) {
   case ECAT_MAIN_HEADER: 
      table = file->header_description->main_header;
      break;
   case ECAT_SUBHEADER:
      table = file->header_description->subheader;
      break;
   default:
      return ECAT_No_Field;
   }

   /* Initialize the table, if needed */
   if (!table->initialized) {
      ecat_initialize_table(table);
   }

   /* Check the index */
   if ((index < 0) || (index >= table->num_entries)) {
      return ECAT_No_Field;
   }

   /* Return the fields in file order */
   return table->file_order[index];

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ecat_get_main_field_length
@INPUT      : file - ecat file pointer
              field - ecat field name
@OUTPUT     : (none)
@RETURNS    : Length of field. Returns -1 if the field is not found.
@DESCRIPTION: Routine to get the length of a field from the ECAT main 
              header. Length is the number of elements, not the number of
              bytes.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 4, 1996 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int ecat_get_main_field_length(Ecat_file *file, 
                                      Ecat_field_name field)
{
   return ecat_get_field_length(file, ECAT_MAIN_HEADER, field);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ecat_get_subhdr_field_length
@INPUT      : file - ecat file pointer
              field - ecat field name
@OUTPUT     : (none)
@RETURNS    : Length of field. Returns -1 if the field is not found.
@DESCRIPTION: Routine to get the length of a field from an ECAT subheader.
              Length is the number of elements, not the number of bytes.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 4, 1996 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int ecat_get_subhdr_field_length(Ecat_file *file, 
                                        Ecat_field_name field)
{
   return ecat_get_field_length(file, ECAT_SUBHEADER, field);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ecat_get_field_length
@INPUT      : file - ecat file pointer
              which_header - header in which to look
              field - ecat field name
@OUTPUT     : (none)
@RETURNS    : Length of field. Returns -1 if the field is not found.
@DESCRIPTION: Routine to get the length of a field from an ECAT
              header. Length is the number of elements, not the number of
              bytes.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 4, 1996 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int ecat_get_field_length(Ecat_file *file, 
                                  Ecat_which_header which_header, 
                                  Ecat_field_name field)
{
   Ecat_header_table_type *table;
   Ecat_type type;
   int length;

   /* Get the appropriate header */
   switch (which_header) {
   case ECAT_MAIN_HEADER: 
      table = file->header_description->main_header;
      break;
   case ECAT_SUBHEADER:
      table = file->header_description->subheader;
      break;
   default:
      return -1;
   }

   /* Look for the field description */
   if (ecat_lookup_field(table, field, NULL, &length, &type, NULL)) {
      return -1;
   }

   /* Return the length. If we have a string, then return 1. */
   if (type == ecat_char)
      return 1;
   else
      return length;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ecat_get_main_field_description
@INPUT      : file - ecat file pointer
              field - ecat field name
@OUTPUT     : (none)
@RETURNS    : Description of field. Returns NULL if the field is not found.
@DESCRIPTION: Routine to get the description of a field from the ECAT main 
              header. The string returned should not be modified.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 4, 1996 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public char *ecat_get_main_field_description(Ecat_file *file, 
                                             Ecat_field_name field)
{
   return ecat_get_field_description(file, ECAT_MAIN_HEADER, field);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ecat_get_subhdr_field_description
@INPUT      : file - ecat file pointer
              field - ecat field name
@OUTPUT     : (none)
@RETURNS    : Description of field. Returns NULL if the field is not found.
@DESCRIPTION: Routine to get the description of a field from an ECAT subheader.
              The string returned should not be modified.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 4, 1996 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public char *ecat_get_subhdr_field_description(Ecat_file *file, 
                                               Ecat_field_name field)
{
   return ecat_get_field_description(file, ECAT_SUBHEADER, field);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ecat_get_field_description
@INPUT      : file - ecat file pointer
              which_header - header in which to look
              field - ecat field name
@OUTPUT     : (none)
@RETURNS    : Description of field. Returns NULL if the field is not found.
@DESCRIPTION: Routine to get the descrition of a field from an ECAT header.
              The string returned should not be modified.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 4, 1996 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private char *ecat_get_field_description(Ecat_file *file, 
                                         Ecat_which_header which_header, 
                                         Ecat_field_name field)
{
   Ecat_header_table_type *table;
   char *description;

   /* Get the appropriate header */
   switch (which_header) {
   case ECAT_MAIN_HEADER: 
      table = file->header_description->main_header;
      break;
   case ECAT_SUBHEADER:
      table = file->header_description->subheader;
      break;
   default:
      return NULL;
   }

   /* Look for the field description */
   if (ecat_lookup_field(table, field, NULL, NULL, NULL, &description)) {
      return NULL;
   }

   /* Return the description */
   return description;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ecat_get_main_value
@INPUT      : file - ecat file pointer
              field - ecat field name
              index - index for multi-valued fields (counting from zero)
@OUTPUT     : ivalue - integer value
              fvalue - floating-point value
              svalue - string value
@RETURNS    : FALSE if successful, TRUE otherwise
@DESCRIPTION: Routine to get a field value from the ECAT main header.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 4, 1996 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int ecat_get_main_value(Ecat_file *file, 
                               Ecat_field_name field, int index,
                               int *ivalue, double *fvalue, char *svalue)
{

   return ecat_get_value(file, ECAT_MAIN_HEADER, 0, 0, field, index, 
                         ivalue, fvalue, svalue);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ecat_get_subhdr_value
@INPUT      : file - ecat file pointer
              volume - frame or bed position for subheader (from 0)
              slice - slice number (counting from 0)
              field - ecat field name
              index - index for multi-valued fields (counting from zero)
@OUTPUT     : ivalue - integer value
              fvalue - floating-point value
              svalue - string value
@RETURNS    : FALSE if successful, TRUE otherwise
@DESCRIPTION: Routine to get a field value from the ECAT main header.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 4, 1996 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int ecat_get_subhdr_value(Ecat_file *file, int volume, int slice,
                                 Ecat_field_name field, int index,
                                 int *ivalue, double *fvalue, char *svalue)
{
   return ecat_get_value(file, ECAT_SUBHEADER, volume, slice, field, index, 
                         ivalue, fvalue, svalue);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ecat_get_value
@INPUT      : file - ECAT file pointer
              which_header - header in which to look
              volume_number - frame or bed position for subheader 
                 (counting from 0)
              slice_number - number of slice for subheader (counting from 0)
              field - field to look for
              index - index for multi-valued fields (counting from zero)
@OUTPUT     : ivalue - integer value
              fvalue - floating-point value
              svalue - string value
@RETURNS    : FALSE if successful, TRUE otherwise
@DESCRIPTION: Routine to look up a field value in a header table.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 4, 1996 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int ecat_get_value(Ecat_file *file,
                           Ecat_which_header which_header,
                           int volume_number, int slice_number,
                           Ecat_field_name field,
                           int index,
                           int *ivalue, double *fvalue, char *svalue)
{
   /* Sizes of ECAT types */
   static int ecat_type_sizes[] = {1, 2, 4, 4, 1};

   /* Variables */
   unsigned char *header;
   Ecat_header_table_type *table;
   int offset, length;
   Ecat_type type;
   unsigned char byte_value;
   short short_value;
   long long_value;
   float float_value;
   char string[ECAT_MAX_STRING_LENGTH];

   /* Get the appropriate header */
   switch (which_header) {
   case ECAT_MAIN_HEADER: 
      header = file->main_header;
      table = file->header_description->main_header;
      break;
   case ECAT_SUBHEADER:
      if (ecat_read_subhdr(file, volume_number, slice_number)) {
         return TRUE;
      }
      header = file->subheader;
      table = file->header_description->subheader;
      break;
   default:
      return TRUE;
   }

   /* Look for the field description */
   if (ecat_lookup_field(table, field, &offset, &length, &type, NULL)) {
      return TRUE;
   }

   /* Check the index */
   if ((index < 0) || (index >= length) || 
       ((type == ecat_char) && (index > 0))) {
      return TRUE;
   }
   offset += index * ecat_type_sizes[type];

   /* Get the value and convert it */
   switch (type) {
   case ecat_byte:
     byte_value = header[offset];
     if (ivalue != NULL) *ivalue = byte_value;
     if (fvalue != NULL) *fvalue = byte_value;
     if (svalue != NULL) (void) sprintf(svalue, "%d", (int) byte_value);
     break;
   case ecat_short:
     if (file->vax_byte_order) {
	get_vax_short(1, &header[offset], &short_value);
     }
     else {
       get_short_value(& header[offset], &short_value);
     }
     if (ivalue != NULL) *ivalue = short_value;
     if (fvalue != NULL) *fvalue = short_value;
     if (svalue != NULL) (void) sprintf(svalue, "%d", (int) short_value);
     break;
   case ecat_long:
     if (file->vax_byte_order) {
       get_vax_long(1, &header[offset], &long_value);
     }
     else {
       get_long_value(& header[offset], &long_value);
     }
      if (ivalue != NULL) *ivalue = long_value;
      if (fvalue != NULL) *fvalue = long_value;
      if (svalue != NULL) (void) sprintf(svalue, "%d", (int) long_value);
      break;
   case ecat_float:
      offset += index * sizeof(float);
      if (file->vax_byte_order) {
	get_vax_float(1, &header[offset], &float_value);
      }
      else {
	get_long_value(& header[offset], &float_value);
      }
      if (ivalue != NULL) *ivalue = float_value;
      if (fvalue != NULL) *fvalue = float_value;
      if (svalue != NULL) (void) sprintf(svalue, "%.7g", 
                                         (double) float_value);
      break;
   case ecat_char:
      (void) memcpy(string, &header[offset], length);
      string[length] = '\0';
      if (ivalue != NULL) *ivalue = atoi(svalue);
      if (fvalue != NULL) *fvalue = atof(svalue);
      if (svalue != NULL) (void) strcpy(svalue, string);
      break;
   default:
      return TRUE;
   }

   return FALSE;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ecat_read_subhdr
@INPUT      : file - ecat file pointer
              volume - frame or bed position for subheader (from 0)
              slice - slice number (counting from 0)
@OUTPUT     : file - file structure is modified to include new subheader
@RETURNS    : FALSE if successful, TRUE otherwise
@DESCRIPTION: Routine to read in the appropriate subheader, if needed.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 4, 1996 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int ecat_read_subhdr(Ecat_file *file, int volume, int slice)
{
   long offset;

   /* Get subheader offset */
   if (ecat_get_subhdr_offset(file, volume, slice, &offset)) {
      return TRUE;
   }

   /* Do we need to read in the header? */
   if (offset == file->cur_subhdr_offset) {
      return FALSE;
   }

   /* If so, do it */
   if (fseek(file->file_pointer, offset, SEEK_SET) ||
       (fread(file->subheader, sizeof(char), (size_t) SUBHEADER_SIZE,
              file->file_pointer) != SUBHEADER_SIZE)) {
      return TRUE;
   }
   file->cur_subhdr_offset = offset;

   return FALSE;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ecat_lookup_field
@INPUT      : table - table containing header fields
              field - field to look for
@OUTPUT     : offset - offset into header
              length - length of field (number of values, not bytes)
              type - type of values
              description - string describing field (ptr is passed back
                 by reference)
@RETURNS    : FALSE if successful, TRUE otherwise
@DESCRIPTION: Routine to look up a field value in a header table. All
              output arguments can be NULL to indicate that the 
              corresponding value should not be returned.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 4, 1996 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int ecat_lookup_field(Ecat_header_table_type *table,
                              Ecat_field_name field,
                              int *offset, int *length, Ecat_type *type, 
                              char **description)
{
   Ecat_field_description_type field_description, *result;

   /* Initialize the table, if needed */
   if (!table->initialized) {
      ecat_initialize_table(table);
   }

   /* Set up a dummy entry */
   field_description.name = field;

   /* Search for the field */
   result = bsearch(&field_description, table->fields, table->num_entries, 
                    sizeof(table->fields[0]), ecat_table_entry_compare);

   if (result == NULL) {
      return TRUE;
   }

   /* Store the results */
   if (offset != NULL) *offset = result->offset;
   if (length != NULL) *length = result->length;
   if (type != NULL) *type = result->type;
   if (description != NULL) *description = result->description;

   return FALSE;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ecat_initialize_table
@INPUT      : table - table containing header fields
@OUTPUT     : table - modified appropriately
@RETURNS    : (nothing)
@DESCRIPTION: Routine to initialize the header table.
@METHOD     : Initialized should be set to TRUE, num_entries should already 
              be set and fields should point to a list. The fields list will
              be sorted and the file_order field will be set.
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 4, 1996 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void ecat_initialize_table(Ecat_header_table_type *table)
{
   Ecat_field_description_type *file_order_list;
   int iel;

   /* Check that table is not already initialized */
   if (table->initialized) return;

   /* Sort the field list by field name for binary searching */
   qsort(table->fields, table->num_entries, sizeof(table->fields[0]), 
         ecat_table_entry_compare);

   /* Get a list of field names in file order by copying the field list 
      and sorting it by offset */
   file_order_list = MALLOC(table->num_entries * sizeof(*file_order_list));
   for (iel=0; iel < table->num_entries; iel++) {
      file_order_list[iel] = table->fields[iel];
   }
   qsort(file_order_list, table->num_entries, sizeof(file_order_list[0]),
         ecat_table_offset_compare);
   table->file_order = MALLOC(table->num_entries * 
                              sizeof(table->file_order[0]));
   for (iel=0; iel < table->num_entries; iel++) {
      table->file_order[iel] = file_order_list[iel].name;
   }
   FREE(file_order_list);

   /* Mark the table as initialized */
   table->initialized = TRUE;

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ecat_table_entry_compare
@INPUT      : v1 - first value
@OUTPUT     : v2 - second value
@RETURNS    : (nothing)
@DESCRIPTION: Routine to compare field list elements for sorting by name.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 4, 1996 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int ecat_table_entry_compare(const void *v1, const void *v2)
{
   Ecat_field_description_type *first, *second;

   /* Get pointers */
   first  = (Ecat_field_description_type *) v1;
   second = (Ecat_field_description_type *) v2;

   /* Compare field names */
   return ((int) first->name - (int) second->name);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ecat_table_offset_compare
@INPUT      : v1 - first value
@OUTPUT     : v2 - second value
@RETURNS    : (nothing)
@DESCRIPTION: Routine to compare field list elements for sorting by offset.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 4, 1996 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int ecat_table_offset_compare(const void *v1, const void *v2)
{
   Ecat_field_description_type *first, *second;

   /* Get pointers */
   first  = (Ecat_field_description_type *) v1;
   second = (Ecat_field_description_type *) v2;

   /* Compare field names */
   return ((int) first->offset - (int) second->offset);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ecat_read_directory
@INPUT      : file - ecat file pointer
@OUTPUT     : (nothing)
@RETURNS    : FALSE if successful, TRUE otherwise
@DESCRIPTION: Routine to read in the ECAT subheader directory
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 4, 1996 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int ecat_read_directory(Ecat_file *file)
{
   int ientry, start_entry, num_used, num_alloc;
   long nextblock, dirblock[DIRBLOCK_SIZE / sizeof(long)];

   /* Allocate space for the subheader offset array */
   num_alloc = file->num_volumes;
   if (file->header_description == ECAT_VER_PRE7)
      num_alloc *= file->num_planes;
   file->num_subhdrs = 0;
   file->subhdr_offsets = 
      MALLOC(num_alloc * sizeof(file->subhdr_offsets[0]));

  

   /* Reading directory blocks until done */
   nextblock = FIRST_DIRBLOCK;
   do {

      /* Read in the block */
      if (fseek(file->file_pointer, (nextblock - 1) * BLOCK_SIZE, SEEK_SET) ||
          (fread(dirblock, sizeof(char), sizeof(dirblock),
                 file->file_pointer) != sizeof(dirblock))) {
         return TRUE;
      }


      /* Get a pointer to the next block and the number of entries used */
      nextblock = get_dirblock(file, dirblock, DRBLK_NEXT);
      num_used = get_dirblock(file, dirblock, DRBLK_NUMUSED);

      /* Increment the number of subheaders */
      start_entry = file->num_subhdrs;
      file->num_subhdrs += num_used;
      if (num_alloc < file->num_subhdrs) {
         num_alloc = file->num_subhdrs;
         REALLOC(file->subhdr_offsets, 
                 num_alloc * sizeof(file->subhdr_offsets[0]));
      }

      /* Save the offsets */
      for (ientry=0; ientry < num_used; ientry++) {
         file->subhdr_offsets[start_entry + ientry] = BLOCK_SIZE *
            (get_dirblock(file, dirblock, 
                          DRBLK_SKIP + ientry * DRBLK_WIDTH + DRBLK_PTR) - 1);
      }
      
   } while (nextblock > FIRST_DIRBLOCK);

   return FALSE;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_dirblock
@INPUT      : file - ecat file pointer
              dirblock - directory block
              offset - offset (in longwords) into the block
@OUTPUT     : (nothing)
@RETURNS    : directory block value
@DESCRIPTION: Routine to get a value from an ECAT directory block.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 4, 1996 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private long get_dirblock(Ecat_file *file, long *dirblock, int offset)
{
   long value;

   if (file->header_description == ECAT_VER_PRE7) {
     /*get_vax_long(1, &dirblock[offset], &value);*/
   }
   else if (file->header_description == ECAT_VER_7) {
     /* value = dirblock[offset];*/
      get_long_value(&dirblock[offset], &value); 
   }
   else {
      return 0;
   }

   return value;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ecat_get_image
@INPUT      : file - ecat file pointer
              volume - frame or bed position (from 0)
              slice - slice number (counting from 0)
@OUTPUT     : image
@RETURNS    : FALSE if successful, TRUE otherwise
@DESCRIPTION: Routine to get an image from an ECAT file
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 4, 1996 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int ecat_get_image(Ecat_file *file, int volume, int slice, 
                          short *image)
{
   long file_offset;
   int xsize, ysize, zsize, data_type, bytes_per_pixel;
   long image_npix, image_size, array_offset, ipix;
   unsigned char *bimage;

   /* Get the image size and type */
   if (ecat_get_subhdr_value(file, volume, slice, ECAT_X_Dimension, 0,
                             &xsize, NULL, NULL) ||
       ecat_get_subhdr_value(file, volume, slice, ECAT_Y_Dimension, 0,
                             &ysize, NULL, NULL) ||
       ecat_get_subhdr_value(file, volume, slice, ECAT_Data_Type, 0,
                             &data_type, NULL, NULL) ||
       (xsize <= 0) || (ysize <= 0)) {
      return TRUE;
   }

   /* Figure out the image size */
   switch (data_type) {
   case 1:
      bytes_per_pixel = 1; break;
   case 2:
   case 6:
      bytes_per_pixel = 2; break;
   default:
      return TRUE;
   }
   image_npix = xsize * ysize;
   image_size = image_npix * bytes_per_pixel;

   /* Look for a z size */
   zsize = 0;
   (void) ecat_get_subhdr_value(file, volume, slice, ECAT_Z_Dimension, 0,
                                &zsize, NULL, NULL);

   /* Check the that the slice is in range */
   if ((slice < 0) || ((zsize > 0) && (slice > zsize))) {
      return TRUE;
   }

   /* Find the appropriate subheader */
   if (ecat_get_subhdr_offset(file, volume, slice, &file_offset)) {
      return TRUE;
   }

   /* Adjust the offset appropriately */
   file_offset += BLOCK_SIZE;
   if (zsize > 0) {
      file_offset += image_size * slice;
   }

   /* Calculate image size and offsets */
   array_offset = image_npix * (sizeof(short) - bytes_per_pixel);
   bimage = (unsigned char *) image;

   /* Read in the image */
   if (fseek(file->file_pointer, file_offset, SEEK_SET) ||
       (fread(&bimage[array_offset], (size_t) bytes_per_pixel, 
              (size_t) image_npix, file->file_pointer) != image_npix)) {
      return TRUE;
   }

   /* Transform the image to the right type */
   switch (bytes_per_pixel) {
   case 1:
      for (ipix=0; ipix<image_npix; ipix++) {
         image[ipix] = bimage[array_offset+ipix];
      }
      break;
   case 2:
      if (file->header_description == ECAT_VER_PRE7) {
	/*get_vax_short(image_npix, image, image);*/
      }
      else {
	for (ipix=0; ipix<image_npix; ipix++) {
	  get_short_value(&bimage[ipix * bytes_per_pixel], &image[ipix]); 
	  /*image[ipix] = bimage[array_offset+ipix];*/
	}	
      }
      break;
   default:
      return TRUE;
   }

   return FALSE;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : ecat_get_subhdr_offset
@INPUT      : file - ecat file pointer
              volume - frame or bed position for subheader (from 0)
              slice - slice number (counting from 0)
@OUTPUT     : offset - offset in bytes to appropriate subheader
@RETURNS    : FALSE if successful, TRUE otherwise
@DESCRIPTION: Routine to calculate the offset to the appropriate subheader.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 4, 1996 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int ecat_get_subhdr_offset(Ecat_file *file, int volume, int slice, 
                                   long *offset)
{
   int subheader_number;

   /* Check to see if volume and slice are within bounds */
   if ((volume < 0) || (volume >= file->num_volumes) ||
       (slice < 0) || (slice >= file->num_planes)) {
      return TRUE;
   }

   /* Calculate subheader number */
   if (file->header_description == ECAT_VER_7) {
      subheader_number = volume;
   }
   else if (file->header_description == ECAT_VER_PRE7) {
      subheader_number = volume * file->num_planes + slice;
   }
   else {
      return TRUE;
   }

   /* Check the subheader number */
   if ((subheader_number < 0) || (subheader_number >= file->num_subhdrs)) {
      return TRUE;
   }

   /* Get the offset for this subheader */
   *offset = file->subhdr_offsets[subheader_number];

   return FALSE;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : malloc_check
@INPUT      : size - number of bytes to allocate
@OUTPUT     : (nothing)
@RETURNS    : pointer to memory
@DESCRIPTION: Routine to allocate memory. It will never return NULL - the
              program will exit first.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 4, 1996 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void *malloc_check(size_t size)
{
   void *ptr;

   ptr = malloc(size);
   if (ptr == NULL) {
      (void) fprintf(stderr, "Out of memory.\n");
      exit(EXIT_FAILURE);
   }
   return ptr;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : realloc_check
@INPUT      : ptr - old pointer
              size - number of bytes to allocate
@OUTPUT     : (nothing)
@RETURNS    : pointer to memory
@DESCRIPTION: Routine to re-allocate memory. It will never return NULL - the
              program will exit first.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 4, 1996 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void *realloc_check(void *ptr, size_t size)
{
   ptr = realloc(ptr, size);
   if (ptr == NULL) {
      (void) fprintf(stderr, "Out of memory.\n");
      exit(EXIT_FAILURE);
   }
   return ptr;
}

