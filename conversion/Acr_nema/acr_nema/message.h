/* ----------------------------- MNI Header -----------------------------------
@NAME       : message.h
@DESCRIPTION: Header file for acr-nema message code
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 16, 1993 (Peter Neelin)
@MODIFIED   : 
 * $Log: message.h,v $
 * Revision 6.2  2011-02-17 06:41:51  rotor
 *  * Fixed a HDF5 error output bug in testing code
 *
 * Revision 6.1  1999/10/29 17:51:54  neelin
 * Fixed Log keyword
 *
 * Revision 6.0  1997/09/12 13:23:59  neelin
 * Release of minc version 0.6
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
 * Revision 2.0  1994/09/28  10:36:18  neelin
 * Release of minc version 0.2
 *
 * Revision 1.4  94/09/28  10:35:53  neelin
 * Pre-release
 * 
 * Revision 1.3  93/11/24  11:27:10  neelin
 * Added dump message routine./
 * 
 * Revision 1.2  93/11/22  13:12:55  neelin
 * Changed to use new Acr_Element_Id stuff.
 * 
 * Revision 1.1  93/11/19  12:50:37  neelin
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

/* Group type */
typedef struct Acr_Message {
   int ngroups;
   long implicit_total_length;
   long explicit_total_length;
   long message_implicit_offset;
   long message_explicit_offset;
   Acr_Element message_length_element;
   Acr_Group list_head;
   Acr_Group list_tail;
} *Acr_Message;

/* Functions */
public Acr_Message acr_create_message(void);
public void acr_delete_message(Acr_Message message);
public void acr_message_reset(Acr_Message message);
public void acr_message_add_group(Acr_Message message, Acr_Group group);
public void acr_message_add_group_list(Acr_Message message, 
                                       Acr_Group group_list);
public Acr_Group acr_get_message_group_list(Acr_Message message);
public long acr_get_message_total_length(Acr_Message message,
                                         Acr_VR_encoding_type vr_encoding);
public int acr_get_message_ngroups(Acr_Message message);
public Acr_Status acr_input_message(Acr_File *afp, Acr_Message *message);
public Acr_Status acr_output_message(Acr_File *afp, Acr_Message message);
public void acr_dump_message(FILE *file_pointer, Acr_Message message);
