/* ----------------------------- MNI Header -----------------------------------
@NAME       : element.h
@DESCRIPTION: Header file for acr-nema element code
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : $Log: element.h,v $
@MODIFIED   : Revision 1.1  1993-11-19 12:50:24  neelin
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

/* Element type */
typedef struct Acr_Element {
   int group_id;
   int element_id;
   long data_length;
   char *data_pointer;
   struct Acr_Element *next;
} *Acr_Element;


/* Functions */
public Acr_Element acr_create_element(int group_id, int element_id, 
                                      long data_length, char *data_pointer);
public void acr_delete_element(Acr_Element element);
public void acr_delete_element_list(Acr_Element element_list);
public void acr_set_element_id(Acr_Element element, 
                               int group_id, int element_id);
public void acr_set_element_data(Acr_Element element,
                                 long data_length, char *data_pointer);
public void acr_set_element_next(Acr_Element element, Acr_Element next);
public int acr_get_element_group(Acr_Element element);
public int acr_get_element_element(Acr_Element element);
public long acr_get_element_length(Acr_Element element);
public char *acr_get_element_data(Acr_Element element);
public long acr_get_element_total_length(Acr_Element element);
public Acr_Element acr_get_element_next(Acr_Element element);
public Acr_Status acr_input_element(Acr_File *afp, Acr_Element *element);
public Acr_Status acr_output_element(Acr_File *afp, Acr_Element element);
public Acr_Element acr_create_element_short(int group_id, int element_id, 
                                            short value);
public Acr_Element acr_create_element_long(int group_id, int element_id, 
                                           long value);
public Acr_Element acr_create_element_numeric(int group_id, int element_id, 
                                              double value);
public Acr_Element acr_create_element_string(int group_id, int element_id, 
                                             char *value);
public short acr_get_element_short(Acr_Element element);
public long acr_get_element_long(Acr_Element element);
public double acr_get_element_numeric(Acr_Element element);
public char *acr_get_element_string(Acr_Element element);
public int acr_get_element_numeric_array(Acr_Element element,
                                         int max_values, double values[]);
