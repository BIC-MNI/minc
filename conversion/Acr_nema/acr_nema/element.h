/* ----------------------------- MNI Header -----------------------------------
@NAME       : element.h
@DESCRIPTION: Header file for acr-nema element code
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
 * $Log: element.h,v $
 * Revision 6.4  2006-04-09 15:28:40  bert
 * Add functions acr_get_element_double_array and acr_create_element_double
 *
 * Revision 6.3  2005/03/04 00:08:08  bert
 * Cleanup headers, mostly by getting rid of the infernal 'public' and using extern instead
 *
 * Revision 6.2  2004/10/29 13:08:41  rotor
 *  * rewrote Makefile with no dependency on a minc distribution
 *  * removed all references to the abominable minc_def.h
 *  * I should autoconf this really, but this is old code that
 *      is now replaced by Jon Harlaps PERL version..
 *
 * Revision 6.1  1999/10/29 17:51:52  neelin
 * Fixed Log keyword
 *
 * Revision 6.0  1997/09/12 13:23:59  neelin
 * Release of minc version 0.6
 *
 * Revision 5.2  1997/09/02  22:51:52  neelin
 * Fixed padding of of UI strings and got rid of acr_string_pad_char
 * function.
 *
 * Revision 5.1  1997/08/22  15:08:34  neelin
 * Added routine acr_string_pad_char to set character used for padding
 * strings to an even number of bytes.
 *
 * Revision 5.0  1997/08/21  13:25:00  neelin
 * Release of minc version 0.5
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
 * Revision 2.0  1994/09/28  10:36:11  neelin
 * Release of minc version 0.2
 *
 * Revision 1.7  94/09/28  10:35:51  neelin
 * Pre-release
 * 
 * Revision 1.6  94/04/07  10:05:11  neelin
 * Added status ACR_ABNORMAL_END_OF_INPUT and changed some ACR_PROTOCOL_ERRORs
 * to that or ACR_OTHER_ERROR.
 * Added #ifdef lint to DEFINE_ELEMENT.
 * 
 * Revision 1.5  93/11/26  18:47:48  neelin
 * Added element copy routine.
 * 
 * Revision 1.4  93/11/24  11:26:35  neelin
 * Changed short to unsigned short.
 * 
 * Revision 1.3  93/11/23  11:36:21  neelin
 * Changed GLOBAL_ELEMENT macro to allow definition or declaration of
 * global variables.
 * 
 * Revision 1.2  93/11/22  13:12:21  neelin
 * Added Acr_Element_Id code.
 * 
 * Revision 1.1  93/11/19  12:50:24  neelin
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

#ifndef _ACR_ELEMENT_H_
#define _ACR_ELEMENT_H_

/* Element type */
typedef struct Acr_Element {
   int group_id;
   int element_id;
   long data_length;
   char *data_pointer;
   struct Acr_Element *next;
   short vr_code;
   unsigned int is_sequence:1;
   unsigned int has_variable_length:1;
   unsigned int has_little_endian_order:1;
   unsigned int uses_explicit_vr:1;
} *Acr_Element;

/* Structure for specifying element id's */
typedef struct Acr_Element_Id {
   int group_id;
   int element_id;
   Acr_VR_Type vr_code;
} *Acr_Element_Id;

/* Macros for creating element id's (class should be nothing or static) */
#ifndef lint
#define DEFINE_ELEMENT(class, name, group, element, vr) \
   static struct Acr_Element_Id name##_struct = \
      {group, element, ACR_VR_##vr}; \
   class Acr_Element_Id name = &name##_struct
#else
#define DEFINE_ELEMENT(class, name, group, element, vr) \
   class Acr_Element_Id name = (void *) 0
#endif

/* Macro for creating global elements. If GLOBAL_ELEMENT_DEFINITION is
   defined then we define the variables, otherwise we just declare them */
#ifdef GLOBAL_ELEMENT_DEFINITION
#  define GLOBAL_ELEMENT(name, group, element, vr) \
      DEFINE_ELEMENT(,name, group, element, vr)
#else
#  define GLOBAL_ELEMENT(name, group, element, vr) \
      extern Acr_Element_Id name
#endif

/* Macro for creating global elements for the ACR-NEMA library. 
   If ACR_LIBRARY_GLOBAL_ELEMENT_DEFINITION is defined then we define 
   the variables, otherwise we just declare them */
#ifdef ACR_LIBRARY_GLOBAL_ELEMENT_DEFINITION
#  define ACRLIB_GLOBAL_ELEMENT(name, group, element, vr) \
      DEFINE_ELEMENT(,name, group, element, vr)
#else
#  define ACRLIB_GLOBAL_ELEMENT(name, group, element, vr) \
      extern Acr_Element_Id name
#endif

/* Global element definition for items */
ACRLIB_GLOBAL_ELEMENT(ACR_Sequence_Item, ACR_ITEM_GROUP, 
                      ACR_ITEM_TAG, UNKNOWN);

/* Functions */
extern Acr_Element acr_create_element(int group_id, int element_id, 
                                      Acr_VR_Type vr_code, 
                                      long data_length, char *data_pointer);
extern void acr_delete_element(Acr_Element element);
extern void acr_delete_element_list(Acr_Element element_list);
extern Acr_Element acr_element_list_add(Acr_Element element_list, 
                                        Acr_Element element);
extern void acr_set_element_id(Acr_Element element,
                               int group_id, int element_id);
extern void acr_set_element_vr(Acr_Element element,
                               Acr_VR_Type vr_code);
extern void acr_set_element_vr_encoding(Acr_Element element,
                                        Acr_VR_encoding_type vr_encoding);
extern void acr_set_element_byte_order(Acr_Element element,
                                       Acr_byte_order byte_order);
extern void acr_set_element_variable_length(Acr_Element element,
                                            int has_variable_length);
extern void acr_set_element_data(Acr_Element element,
                                 long data_length, char *data_pointer);
extern void acr_set_element_next(Acr_Element element, Acr_Element next);
extern int acr_get_element_group(Acr_Element element);
extern int acr_get_element_element(Acr_Element element);
extern Acr_VR_Type acr_get_element_vr(Acr_Element element);
extern Acr_VR_encoding_type acr_get_element_vr_encoding(Acr_Element element);
extern int acr_element_is_sequence(Acr_Element element);
extern Acr_byte_order acr_get_element_byte_order(Acr_Element element);
extern int acr_element_has_variable_length(Acr_Element element);
extern long acr_get_element_length(Acr_Element element);
extern char *acr_get_element_data(Acr_Element element);
extern long acr_get_element_total_length(Acr_Element element,
                                         Acr_VR_encoding_type vr_encoding);
extern Acr_Element acr_get_element_next(Acr_Element element);
extern Acr_Element acr_copy_element(Acr_Element element);
extern Acr_Status acr_input_element(Acr_File *afp, Acr_Element *element);
extern Acr_Status acr_output_element(Acr_File *afp, Acr_Element element);
extern void acr_convert_element_byte_order(Acr_Element element, 
                                           Acr_byte_order byte_order);
extern int acr_match_element_id(Acr_Element_Id elid,
                                Acr_Element element);
extern Acr_Element acr_find_element_id(Acr_Element element_list,
                                       Acr_Element_Id elid);
extern void *acr_memdup(size_t value_size, void *value);
extern Acr_Element acr_create_element_short(Acr_Element_Id elid,
                                            unsigned short value);
extern Acr_Element acr_create_element_long(Acr_Element_Id elid,
                                           long value);
extern Acr_Element acr_create_element_double(Acr_Element_Id elid,
                                             int nvalues,
                                             double *values);
extern Acr_Element acr_create_element_numeric(Acr_Element_Id elid,
                                              double value);
extern Acr_Element acr_create_element_string(Acr_Element_Id elid,
                                             char *value);
extern Acr_Element acr_create_element_sequence(Acr_Element_Id elid,
                                               Acr_Element itemlist);
extern unsigned short acr_get_element_short(Acr_Element element);
extern long acr_get_element_long(Acr_Element element);
extern double acr_get_element_numeric(Acr_Element element);
extern char *acr_get_element_string(Acr_Element element);
extern long acr_get_element_short_array(Acr_Element element, long max_values, 
                                        unsigned short values[]);
extern long acr_get_element_double_array(Acr_Element element, long max_values, 
                                         double values[]);
extern int *acr_element_numeric_array_separator(int character);
extern int acr_get_element_numeric_array(Acr_Element element,
                                         int max_values, double values[]);
extern void acr_dump_element_list(FILE *file_pointer, 
                                  Acr_Element element_list);

#endif /* _ACR_ELEMENT_H_ */
