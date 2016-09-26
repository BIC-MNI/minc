/* ----------------------------- MNI Header -----------------------------------
@NAME       : group.h
@DESCRIPTION: Header file for acr-nema group code
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : 
 * $Log: group.h,v $
 * Revision 6.7  2008/08/12 05:00:23  rotor
 *  * large number of changes from Claude (64 bit and updates)
 *
 * Revision 6.6  2006/04/09 15:29:43  bert
 * Add acr_insert_double()
 *
 * Revision 6.5  2005/03/11 22:05:29  bert
 * Implement _acr_name_proc to allow printing of field names in dump_acr_nema
 *
 * Revision 6.4  2005/03/04 00:08:08  bert
 * Cleanup headers, mostly by getting rid of the infernal 'public' and using extern instead
 *
 * Revision 6.3  2001/11/08 14:17:06  neelin
 * Added acr_test_dicom_file to allow reading of DICOM part 10 format
 * files. This function also calls acr_test_byte_order to set up the stream
 * properly and can be used as a direct replacement for that function.
 * This set of changes does NOT include the ability to write part 10 files.
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
 * Revision 2.0  1994/09/28  10:36:17  neelin
 * Release of minc version 0.2
 *
 * Revision 1.6  94/09/28  10:35:52  neelin
 * Pre-release
 * 
 * Revision 1.5  93/12/10  09:20:44  neelin
 * Added acr_find_<type> routines.
 * 
 * Revision 1.4  93/11/26  18:48:04  neelin
 * Added group and group list copy routines.
 * 
 * Revision 1.3  93/11/24  11:26:38  neelin
 * Added group list stuff (dump and input).
 * 
 * Revision 1.2  93/11/22  13:12:41  neelin
 * Changed to use new Acr_Element_Id stuff.
 * 
 * Revision 1.1  93/11/19  12:50:32  neelin
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

#ifndef _ACR_GROUP_H_
#define _ACR_GROUP_H_ 1

/* Group type */
typedef struct Acr_Group {
   int group_id;
   int nelements;
   long implicit_total_length;
   long explicit_total_length;
   Acr_Element list_head;
   Acr_Element list_tail;
   struct Acr_Group *next;
} *Acr_Group;

/* Group length element id */
#define ACR_EID_GRPLEN 0

/* Functions */
extern Acr_Group acr_create_group(int group_id);
extern void acr_delete_group(Acr_Group group);
extern void acr_delete_group_list(Acr_Group group_list);
extern Acr_Group acr_copy_group(Acr_Group group);
extern Acr_Group acr_copy_group_list(Acr_Group group_list);
extern Acr_Status acr_group_insert_element(Acr_Group group,
                                           Acr_Element element);
extern Acr_Status acr_group_add_element(Acr_Group group, Acr_Element element);
extern void acr_group_remove_element(Acr_Group group, int element_id);
extern void acr_group_steal_element(Acr_Group group, Acr_Element element);
extern void acr_set_group_next(Acr_Group group, Acr_Group next);
extern int acr_get_group_group(Acr_Group group);
extern Acr_Element acr_get_group_element_list(Acr_Group group);
extern long acr_get_group_total_length(Acr_Group group,
                                       Acr_VR_encoding_type vr_encoding);
extern int acr_get_group_nelements(Acr_Group group);
extern Acr_Group acr_get_group_next(Acr_Group group);
extern Acr_Status acr_input_group(Acr_File *afp, Acr_Group *group);
extern Acr_Status acr_output_group(Acr_File *afp, Acr_Group group);
extern Acr_Status acr_input_group_list(Acr_File *afp, Acr_Group *group_list,
                                       int max_group_id);
extern Acr_Group acr_find_group(Acr_Group group_list, int group_id);
extern Acr_Element acr_find_group_element(Acr_Group group_list,
                                          Acr_Element_Id elid);
extern void acr_dump_group_list(FILE *file_pointer, Acr_Group group_list);
extern Acr_Short acr_find_short(Acr_Group group_list, Acr_Element_Id elid, 
                                Acr_Short default_value);
extern Acr_Long acr_find_long(Acr_Group group_list, Acr_Element_Id elid, 
                              Acr_Long default_value);
extern int acr_find_int(Acr_Group group_list, Acr_Element_Id elid, 
                        int default_value);
extern Acr_Double acr_find_double(Acr_Group group_list, Acr_Element_Id elid, 
                                  Acr_Double default_value);
extern Acr_String acr_find_string(Acr_Group group_list, Acr_Element_Id elid, 
                                  Acr_String default_value);
extern Acr_Status acr_insert_element_into_group_list(Acr_Group *group_list,
                                                     Acr_Element element);
extern Acr_Status acr_insert_short(Acr_Group *group_list, Acr_Element_Id elid, 
                                   Acr_Short value);
extern Acr_Status acr_insert_long(Acr_Group *group_list, Acr_Element_Id elid, 
                                  Acr_Long value);
extern Acr_Status acr_insert_double(Acr_Group *group_list, Acr_Element_Id elid,
                                    int nvalues, Acr_Double *values);
extern Acr_Status acr_insert_numeric(Acr_Group *group_list, 
                                     Acr_Element_Id elid, 
                                     double value);
extern Acr_Status acr_insert_string(Acr_Group *group_list, 
                                    Acr_Element_Id elid, 
                                    Acr_String value);
extern Acr_Status acr_insert_sequence(Acr_Group *group_list,
                                      Acr_Element_Id elid, 
                                      Acr_Element itemlist);
extern Acr_Status acr_test_dicom_file(Acr_File *afp);

typedef char *(*acr_name_proc_t)(unsigned int grp_id, unsigned int el_id);

extern acr_name_proc_t _acr_name_proc;

#endif /* _ACR_GROUP_H_ */
