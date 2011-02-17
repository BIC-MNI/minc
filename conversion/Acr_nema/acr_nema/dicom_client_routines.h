/* ----------------------------- MNI Header -----------------------------------
@NAME       : dicom_client_routines.h
@DESCRIPTION: Header for dicom_client_routines.c
@GLOBALS    : 
@CREATED    : July 9, 1997 (Peter Neelin)
@MODIFIED   : 
 * $Log: dicom_client_routines.h,v $
 * Revision 6.6  2011-02-17 06:41:51  rotor
 *  * Fixed a HDF5 error output bug in testing code
 *
 * Revision 6.5  1999/10/29 17:51:50  neelin
 * Fixed Log keyword
 *
 * Revision 6.4  1998/11/13 15:55:27  neelin
 * Modifications to support asynchronous transfers.
 *
 * Revision 6.3  1998/03/23  20:17:17  neelin
 * Removed some functions.
 *
 * Revision 6.2  1997/10/20  23:22:38  neelin
 * Added routine acr_dicom_close_no_release to close a connection that does
 * not have an association.
 *
 * Revision 6.1  1997/09/15  16:50:59  neelin
 * Separated out connection timeouts from i/o timeouts and added functions
 * to change them.
 *
 * Revision 6.0  1997/09/12  13:23:59  neelin
 * Release of minc version 0.6
 *
 * Revision 1.1  1997/09/08  21:52:21  neelin
 * Initial revision
 *
 * Revision 1.3  1997/07/11  17:35:58  neelin
 * Changed around send and receive routines for data once again.
 *
 * Revision 1.2  1997/07/11  13:23:37  neelin
 * Made changes so that code will compile on sun OS.
 * Separated out receive_reply from send_group_list.
 *
 * Revision 1.1  1997/07/10  17:39:51  neelin
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

/* Function prototypes */
public int acr_open_dicom_connection(char *host, char *port,
                                     char *called_ae, char *calling_ae,
                                     char *abstract_syntax,
                                     char *transfer_syntax,
                                     Acr_File **afpin, Acr_File **afpout);
public void acr_close_dicom_no_release(Acr_File *afpin, Acr_File *afpout);
public void acr_close_dicom_connection(Acr_File *afpin, Acr_File *afpout);
public int acr_connect_to_host(char *host, char *port,
                               FILE **fpin, FILE **fpout);
public char *acr_make_dicom_association(Acr_File *afpin, Acr_File *afpout,
                                        char *called_ae, char *calling_ae,
                                        char *abstract_syntax_list[], 
                                        char *transfer_syntax_list[]);
public void acr_set_client_timeout(Acr_File *afp, double seconds);
public void acr_set_client_initial_timeout(double seconds);
public void acr_set_client_max_outstanding(Acr_File *afp, int max);
public int acr_get_client_max_outstanding(Acr_File *afp);
public void acr_dicom_error(Acr_Status status, char *string);
public int acr_release_dicom_association(Acr_File *afpin, Acr_File *afpout);
public int acr_send_group_list(Acr_File *afpin, Acr_File *afpout, 
                               Acr_Group group_list, 
                               char *sop_class_uid);
public int acr_transmit_group_list(Acr_File *afpout, 
                                   Acr_Group group_list, 
                                   char *sop_class_uid,
                                   int message_id);
public int acr_receive_reply(Acr_File *afpin);
