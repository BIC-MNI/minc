/* ----------------------------- MNI Header -----------------------------------
@NAME       : dicom_client_routines.c
@DESCRIPTION: High-level routines to simplify the job of connecting to a 
              dicom server.
@GLOBALS    : 
@CREATED    : May 6, 1997 (Peter Neelin)
@MODIFIED   : $Log: dicom_client_routines.c,v $
@MODIFIED   : Revision 6.2  1997-10-20 21:46:02  neelin
@MODIFIED   : Delete answering message when making association.
@MODIFIED   :
 * Revision 6.1  1997/09/15  16:50:59  neelin
 * Separated out connection timeouts from i/o timeouts and added functions
 * to change them.
 *
 * Revision 6.0  1997/09/12  13:23:59  neelin
 * Release of minc version 0.6
 *
 * Revision 1.2  1997/09/11  17:19:49  neelin
 * Modified creation of uids. Replaced cftime with strftime.
 *
 * Revision 1.1  1997/09/08  21:52:21  neelin
 * Initial revision
 *
 * Revision 1.4  1997/07/11  17:35:58  neelin
 * Changed around send and receive routines for data once again.
 *
 * Revision 1.3  1997/07/11  13:55:41  neelin
 * Fixed handling of message ids.
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

#ifndef lint
static char rcsid[]="$Header: /private-cvsroot/minc/conversion/Acr_nema/dicom_client_routines.c,v 6.2 1997-10-20 21:46:02 neelin Exp $";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>
#ifndef sun
#include <bstring.h>
#endif
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <acr_nema.h>

/* Constants */
#ifndef public
#  define public
#endif
#ifndef private
#  define private static
#endif
#ifndef INADDR_NONE
#  define INADDR_NONE             0xffffffff
#endif

/* Dicom definitions */
#define ACR_IMPLICIT_VR_LITTLE_END_UID "1.2.840.10008.1.2"
#define ACR_EXPLICIT_VR_LITTLE_END_UID "1.2.840.10008.1.2.1"
#define ACR_EXPLICIT_VR_BIG_END_UID    "1.2.840.10008.1.2.2"
#define ACR_APPLICATION_CONTEXT_UID    "1.2.840.10008.3.1.1.1"
#define ACR_C_STORE_RQ    0x0001
#define ACR_C_STORE_RSP   0x8001
#define ACR_ASSOC_RJ_CALLED_AP_TITLE_UNREC 7
#define ACR_ASSOC_RJ_NO_REASON 1
#define ACR_ASSOC_RJ_PERM 1
#define ACR_ASSOC_RJ_USER 1
#define ACR_ASSOC_PR_CN_ACCEPT 0
#define ACR_ASSOC_PR_CN_REJECT 1
#define ACR_PDU_ITEM_USER_INFORMATION 0x50
#define ACR_MESSAGE_GID 0
#define ACR_SUCCESS 0x0000

DEFINE_ELEMENT(static, ACR_Affected_SOP_class_UID     , 0x0000, 0x0002, UI);
DEFINE_ELEMENT(static, ACR_Command                    , 0x0000, 0x0100, US);
DEFINE_ELEMENT(static, ACR_Message_id                 , 0x0000, 0x0110, US);
DEFINE_ELEMENT(static, ACR_Message_id_brt             , 0x0000, 0x0120, US);
DEFINE_ELEMENT(static, ACR_Priority                   , 0x0000, 0x0700, US);
DEFINE_ELEMENT(static, ACR_Dataset_type               , 0x0000, 0x0800, US);
DEFINE_ELEMENT(static, ACR_Status                     , 0x0000, 0x0900, US);
DEFINE_ELEMENT(static, ACR_Affected_SOP_instance_UID  , 0x0000, 0x1000, UI);
#if 0
DEFINE_ELEMENT(static, ACR_Move_originator_AE_title   , 0x0000, 0x1031, AE);
#endif
DEFINE_ELEMENT(static, ACR_SOP_class_UID              , 0x0008, 0x0016, UI);
DEFINE_ELEMENT(static, ACR_SOP_instance_UID           , 0x0008, 0x0018, UI);
DEFINE_ELEMENT(static, ACR_Study_instance_UID         , 0x0020, 0x000d, UI);
DEFINE_ELEMENT(static, ACR_Series_instance_UID        , 0x0020, 0x000e, UI);
DEFINE_ELEMENT(static, ACR_Image_type                 , 0x0008, 0x0008, CS);
DEFINE_ELEMENT(static, ACR_Sequence_variant           , 0x0018, 0x0021, CS);
DEFINE_ELEMENT(static, ACR_Image_position             , 0x0020, 0x0032, DS);
DEFINE_ELEMENT(static, ACR_Image_orientation          , 0x0020, 0x0037, DS);
DEFINE_ELEMENT(static, ACR_Frame_of_reference_UID     , 0x0020, 0x0052, UI);
DEFINE_ELEMENT(static, ACR_Samples_per_pixel          , 0x0028, 0x0002, US);
DEFINE_ELEMENT(static, ACR_Photometric_interpretation , 0x0028, 0x0004, CS);

/* Globals for handling connection timeouts */
static int Timeout_length = 60 * 2 ;       /* Timeout in seconds */
static int Initial_timeout_length = 10;    /* Timeout for initial connection */
static int Connection_timeout = FALSE;
static Acr_File *Alarmed_afp = NULL;

/* Private functions */
private Acr_Message compose_assoc_request(char *called_ae, char *calling_ae,
                                          char *abstract_syntax_list[],
                                          char *transfer_syntax_list[]);
private int check_reply(Acr_Message message, 
                        int *presentation_context_id, 
                        char **transfer_syntax,
                        long *maximum_length);
private Acr_Status receive_message(Acr_File *afpin, Acr_Message *message);
private Acr_Status send_message(Acr_File *afpout, Acr_Message message);
private void timeout_handler(int sig);
private Acr_Message make_message(Acr_Group group_list);


/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_open_dicom_connection
@INPUT      : host - name of host to which we should connect
              port - string giving port number or name of service to which 
                 we should connect
              called_ae - Remote application entity requested
              calling_ae - Application entity making request
              abstract_syntax - 
              transfer_syntax - if NULL, then send the standard 3 are proposed
                 (implicit-little endian, explicit little, explict big)
@OUTPUT     : afpin - dicom file handle for input
              afpout - dicom file handle for output
@RETURNS    : TRUE if successful connection is made, FALSE otherwise.
@DESCRIPTION: Routine to open a dicom connection to a remote host.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : July 9, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int acr_open_dicom_connection(char *host, char *port,
                                     char *called_ae, char *calling_ae,
                                     char *abstract_syntax,
                                     char *transfer_syntax,
                                     Acr_File **afpin, Acr_File **afpout)
{
   FILE *fpin, *fpout;
   char *abstract_syntax_list[2] = {NULL, NULL};
   char *transfer_syntax_list[2] = {NULL, NULL};

   /* Make network connection */
   if (!acr_connect_to_host(host, port, &fpin, &fpout)) {
      return FALSE;
   }
   *afpin = acr_initialize_dicom_input(fpin, 0, acr_stdio_read);
   *afpout = acr_initialize_dicom_output(fpout, 0, acr_stdio_write);

   /* Establish association */
   abstract_syntax_list[0] = abstract_syntax;
   transfer_syntax_list[0] = transfer_syntax;
   if (!acr_make_dicom_association(*afpin, *afpout, called_ae, calling_ae,
                                   abstract_syntax_list, 
                                   transfer_syntax_list)) {
      acr_close_dicom_connection(*afpin, *afpout);
      return FALSE;
   }

   return TRUE;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_close_dicom_connection
@INPUT      : afpin - dicom file handle for input
              afpout - dicom file handle for output
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to close a dicom connection.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : July 9, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_close_dicom_connection(Acr_File *afpin, Acr_File *afpout)
{
   FILE *fpin, *fpout;

   /* Release the association */
   (void) acr_release_dicom_association(afpin, afpout);

   /* Get the file handles */
   fpin = (FILE *) acr_dicom_get_io_data(afpin);
   fpout = (FILE *) acr_dicom_get_io_data(afpout);

   /* Close the dicom streams */
   acr_close_dicom_file(afpin);
   acr_close_dicom_file(afpout);

   /* Close the file handles */
   (void) fclose(fpin);
   (void) fclose(fpout);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_connect_to_host
@INPUT      : host - name of host to which we should connect
              port - string giving port number or name of service to which 
                 we should connect
@OUTPUT     : fpin - file handle for input
              fpout - file handle for output
@RETURNS    : TRUE if successful connection is made, FALSE otherwise.
@DESCRIPTION: Routine to open a connection to a remote host.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : May 9, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int acr_connect_to_host(char *host, char *port,
                               FILE **fpin, FILE **fpout)
{
   struct servent *sp;
   struct hostent *hp;
   struct sockaddr_in server;
   int sock;

   /* Set default file pointers */
   *fpin = *fpout = NULL;

   /* Initialize the address structure */
   bzero((char *) &server, sizeof(server));

   /* Get the port, either as a number or as a service name */
   if ((*port >= '0') && (*port <= '9')) {
      server.sin_port = atoi(port);
   }
   else if ((sp = getservbyname(port, "tcp")) != NULL) {
      server.sin_port = sp->s_port;
   }
   else {
      (void) fprintf(stderr, "Service \"%s\" not found\n", port);
      return FALSE;
   }

   /* Look up the host, either as a name or as an IP address */
   if ((*host >= '0') && (*host <= '9')) {
      server.sin_addr.s_addr = inet_addr(host);
      server.sin_family = AF_INET;
      if (server.sin_addr.s_addr == INADDR_NONE) {
         (void) fprintf(stderr, "Badly formed IP address %s\n", host);
         return FALSE;
      }
   }
   else if ((hp = gethostbyname(host)) != NULL) {
      bcopy(hp->h_addr, (char *) &server.sin_addr, hp->h_length);
      server.sin_family = hp->h_addrtype;
   }
   else {
      (void) fprintf(stderr, "Unknown host: %s\n", host);
      return FALSE;
   }

   /* Open the connection */
   if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
      perror("Error getting socket");
      return FALSE;
   }
   (void) signal(SIGALRM, timeout_handler);
   (void) alarm(Initial_timeout_length);
   if (connect(sock, (struct sockaddr *) &server, sizeof (server)) < 0) {
      (void) alarm(0);
      (void) fprintf(stderr, "Unable to connect to %s: ", host);
      if (Connection_timeout) {
         (void) fprintf(stderr, "connection timed out\n");
      }
      else {
         perror(NULL);
      }
      return FALSE;
   }
   (void) alarm(0);

   /* Open file handles */
   if ((*fpin = fdopen(sock, "r")) == NULL) {
      (void) fprintf(stderr, "Error opening socket for read\n");
      return FALSE;
   }
   if ((*fpout = fdopen(sock, "w")) == NULL) {
      (void) fclose(*fpin);
      (void) fprintf(stderr, "Error opening socket for write\n");
      return FALSE;
   }

   /* Ignore SIGPIPES in case the output connection gets closed when
      we are doing output. */
   (void) signal(SIGPIPE, SIG_IGN);

   return TRUE;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_make_dicom_association
@INPUT      : afpin - input stream
              afpout - output stream
              abstract_syntax_list - NULL terminated list of abstract 
                 syntaxes
              transfer_syntax_list - NULL-terminated list of transfer 
                 syntaxes. If NULL or empty, then the 3 standard syntaxes
                 are proposed.
@OUTPUT     : (none)
@RETURNS    : Pointer to appropriate abstract syntax or NULL if association
              not made.
@DESCRIPTION: Routine to establish a dicom association.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : May 9, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public char *acr_make_dicom_association(Acr_File *afpin, Acr_File *afpout,
                                        char *called_ae, char *calling_ae,
                                        char *abstract_syntax_list[], 
                                        char *transfer_syntax_list[])
{
   Acr_Message message;
   char *transfer_syntax, *abstract_syntax;
   Acr_Status status;
   Acr_byte_order byte_order;
   Acr_VR_encoding_type vr_encoding;
   int isyntax, nsyntax;
   int presentation_context_id;
   long maximum_length;

   /* Compose a message */
   message = compose_assoc_request(called_ae, calling_ae,
                                   abstract_syntax_list,
                                   transfer_syntax_list);
   if (message == NULL) {
      return NULL;
   }

   /* Ship it out */
   status = send_message(afpout, message);
   acr_delete_message(message);
   if (status != ACR_OK) {
      acr_dicom_error(status, "Error sending association request");
      return NULL;
   }

   /* Wait for an answer */
   status = receive_message(afpin, &message);
   if (status != ACR_OK) {
      acr_dicom_error(status, "Error receiving association reply");
      return NULL;
   }

   /* Check it */
   if (!check_reply(message, &presentation_context_id, 
                    &transfer_syntax, &maximum_length)) {
      return NULL;
   }

   /* Set the presentation context id for the streams */
   acr_set_dicom_pres_context_id(afpin, presentation_context_id);
   acr_set_dicom_pres_context_id(afpout, presentation_context_id);

   /* Figure out which abstract syntax was accepted */
   for (nsyntax=0; abstract_syntax_list[nsyntax] != NULL; nsyntax++) {}
   isyntax = (presentation_context_id - 1) / 2;
   if ((isyntax < 0) || (isyntax >= nsyntax)) {
      (void) fprintf(stderr, "Invalid presentation context accepted.)\n");
      return NULL;
   }
   abstract_syntax = abstract_syntax_list[isyntax];

   /* Make sure that the i/o streams have the correct encoding and
      byte-order */
   if (acr_uid_equal(transfer_syntax, ACR_IMPLICIT_VR_LITTLE_END_UID)) {
      byte_order = ACR_LITTLE_ENDIAN;
      vr_encoding = ACR_IMPLICIT_VR;
   }
   else if (acr_uid_equal(transfer_syntax, ACR_EXPLICIT_VR_LITTLE_END_UID)) {
      byte_order = ACR_LITTLE_ENDIAN;
      vr_encoding = ACR_EXPLICIT_VR;
   }
   else if (acr_uid_equal(transfer_syntax, ACR_EXPLICIT_VR_BIG_END_UID)) {
      byte_order = ACR_BIG_ENDIAN;
      vr_encoding = ACR_EXPLICIT_VR;
   }
   else {
      (void) fprintf(stderr, "Unrecognized transfer syntax \"%s\"\n",
                     transfer_syntax);
      return NULL;
   }
   acr_set_byte_order(afpin, byte_order);
   acr_set_vr_encoding(afpin, vr_encoding);
   acr_set_byte_order(afpout, byte_order);
   acr_set_vr_encoding(afpout, vr_encoding);

   /* Set the maximum length */
   acr_set_dicom_maximum_length(afpout, maximum_length);

   /* Delete the input message */
   acr_delete_message(message);

   /* Return the abstract syntax */
   return abstract_syntax;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : compose_assoc_request
@INPUT      : called_ae - Remote application entity requested
              calling_ae - Application entity making request
              abstract_syntax_list - NULL terminated list of abstract 
                 syntaxes
              transfer_syntax_list - NULL-terminated list of transfer 
                 syntaxes. If NULL or empty, then the 3 standard syntaxes
                 are proposed.
@OUTPUT     : (none)
@RETURNS    : Message to be sent to remote host
@DESCRIPTION: Routine to compose an association request message. It only
              allows one abstract syntax to be sent.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : May 9, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Message compose_assoc_request(char *called_ae, char *calling_ae,
                                          char *abstract_syntax_list[],
                                          char *transfer_syntax_list[])
{
   Acr_Message message;
   Acr_Group group;
   Acr_Element item_list, sublist, subitem, element;
   char **syntax_list;
   int iabstract, itransfer, num_syntax;
   int cur_presentation_context_id;
   static char *standard_transfer_syntax[] = {
      ACR_IMPLICIT_VR_LITTLE_END_UID,
      ACR_EXPLICIT_VR_LITTLE_END_UID,
      ACR_EXPLICIT_VR_BIG_END_UID
   };
   static int num_standard_transfer_syntax =
      sizeof(standard_transfer_syntax) / sizeof(standard_transfer_syntax[0]);

   /* Check for an NULL or empty abstract syntax list */
   if ((abstract_syntax_list == NULL) || (abstract_syntax_list[0] == NULL)) {
      (void) fprintf(stderr, 
         "Error composing association request: Empty abstract syntax list\n");
      return NULL;
   }

   /* Build up the request */
   group = acr_create_group(DCM_PDU_GRPID);
   acr_group_add_element(group, 
       acr_create_element_short(DCM_PDU_Type, ACR_PDU_ASSOC_RQ));

   /* Add the caller and calling AE titles */
   acr_group_add_element(group,
      acr_create_element_string(DCM_PDU_Called_Ap_title, called_ae));
   acr_group_add_element(group,
      acr_create_element_string(DCM_PDU_Calling_Ap_title, calling_ae));

   /* Add the application context */
   acr_group_add_element(group,
      acr_create_element_string(DCM_PDU_Application_context,
                                ACR_APPLICATION_CONTEXT_UID));

   /* Work out the list of transfer syntaxes */
   if ((transfer_syntax_list == NULL) || (transfer_syntax_list[0] == NULL)) {
      syntax_list = standard_transfer_syntax;
      num_syntax = num_standard_transfer_syntax;
   }
   else {
      syntax_list = transfer_syntax_list;
      for (num_syntax=0; 
           transfer_syntax_list[num_syntax] != NULL; 
           num_syntax++) {}
   }

   /* Loop over abstract syntaxes (and presentation contexts) */
   item_list = NULL;
   for (iabstract = 0; abstract_syntax_list[iabstract] != NULL; iabstract++) {

      /* Work out the presentation context id */
      cur_presentation_context_id = (iabstract * 2) + 1;

      /* Create a presentation context */
      sublist = NULL;
      subitem = acr_create_element_short(DCM_PDU_Presentation_context_id,
                                         cur_presentation_context_id);
      sublist = acr_element_list_add(sublist, subitem);

      /* Add an abstract syntax */
      subitem = acr_create_element_string(DCM_PDU_Abstract_syntax, 
                                          abstract_syntax_list[iabstract]);
      sublist = acr_element_list_add(sublist, subitem);

      /* Add the transfer syntax */
      for (itransfer=0; itransfer < num_syntax; itransfer++) {
         subitem = acr_create_element_string(DCM_PDU_Transfer_syntax, 
                                             syntax_list[itransfer]);
         sublist = acr_element_list_add(sublist, subitem);
      }

      /* Add this presentation context to the list */
      item_list = acr_element_list_add(item_list, 
         acr_create_element_sequence(DCM_PDU_Presentation_context, sublist));

   }  /* End of loop over presentation contexts */

   /* Create the presentation context list element */
   element = 
      acr_create_element_sequence(DCM_PDU_Presentation_context_list, 
                                  item_list);
   acr_group_add_element(group, element);

   /* Add the user information */
   acr_group_add_element(group, 
                         acr_create_element_long(DCM_PDU_Maximum_length, 0L));

   /* Make a message and add this group */
   message = acr_create_message();
   acr_message_add_group(message, group);

   return message;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : check_reply
@INPUT      : message
@OUTPUT     : presentation_context_id
              transfer_syntax
              maximum_length -maximum length for dicom output
@RETURNS    : TRUE if reply is okay, FALSE otherwise.
@DESCRIPTION: Routine to check the reply from the remote host.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : May 9, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int check_reply(Acr_Message message, 
                        int *presentation_context_id, 
                        char **transfer_syntax,
                        long *maximum_length)
{
   Acr_Group group;
   Acr_Element element, item, sublist, subitem;
   int pdu_type;

   /* Set values in case of error */
   *transfer_syntax = NULL;
   *presentation_context_id = -1;
   *maximum_length = 0;

   /* Get the group */
   group = acr_get_message_group_list(message);

   /* See what they say */
   pdu_type = acr_find_short(group, DCM_PDU_Type, -1);
   switch (pdu_type) {
   case ACR_PDU_ASSOC_AC: 
      break;
   case ACR_PDU_ASSOC_RJ:
      (void) fprintf(stderr, "Association rejected: code %d\n",
                     acr_find_short(group, DCM_PDU_Reason, -1));
      return FALSE;
   case ACR_PDU_ABORT_RQ:
      (void) fprintf(stderr, "Association aborted: code %d\n",
                     acr_find_short(group, DCM_PDU_Reason, -1));
      return FALSE;
   default:   
      (void) fprintf(stderr, "Bad response to association request\n");
      return FALSE;
   }

   /* Get the presentation context list */
   element = 
      acr_find_group_element(group, DCM_PDU_Presentation_context_reply_list);
   if (element == NULL) {
      (void) fprintf(stderr, "Invalid response to assocation request\n");
      return FALSE;
   }

   /* Loop over list */
   for (item = (Acr_Element) acr_get_element_data(element);
        item != NULL;
        item = acr_get_element_next(item)) {

      if (!acr_element_is_sequence(item)) continue;

      /* Get presentation context info */
      sublist = (Acr_Element) acr_get_element_data(item);

      /* Check whether it was accepted */
      subitem = acr_find_element_id(sublist, DCM_PDU_Result);
      if ((subitem == NULL) || 
          (acr_get_element_short(subitem) != ACR_ASSOC_PR_CN_ACCEPT)) {
         continue;
      }

      /* Check the presentation context id */
      subitem = acr_find_element_id(sublist,
                                    DCM_PDU_Presentation_context_id);
      if (subitem == NULL) continue;
      *presentation_context_id = acr_get_element_short(subitem);

      /* Grab the transfer syntax */
      subitem = acr_find_element_id(sublist, DCM_PDU_Transfer_syntax);
      if ((subitem == NULL)) continue;
      *transfer_syntax = acr_get_element_string(subitem);

      /* If we get to here, we have an accepted syntax, so stop looping */
      break;
      
   }

   /* Check that we found a transfer syntax */
   if (*transfer_syntax == NULL) {
      (void) fprintf(stderr, "No presentation contexts were accepted\n");
      return FALSE;
   }

   /* Get the maximum length */
   *maximum_length = acr_find_long(group, DCM_PDU_Maximum_length, 0L);

   return TRUE;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : receive_message
@INPUT      : afpin - input stream
@OUTPUT     : message - message that was read in
@RETURNS    : status
@DESCRIPTION: Routine to receive messages
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : May 9, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Status receive_message(Acr_File *afpin, Acr_Message *message)
{
   Acr_Status status;

   Alarmed_afp = afpin;
   Connection_timeout = FALSE;
   (void) signal(SIGALRM, timeout_handler);
   (void) alarm(Timeout_length);
   status=acr_input_dicom_message(afpin, message);
   (void) alarm(0);
   if (Connection_timeout) {
      status = ACR_CONNECTION_TIMEDOUT;
   }

   return status;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : send_message
@INPUT      : afpout - output stream
              message - message to send
@OUTPUT     : (none)
@RETURNS    : status
@DESCRIPTION: Routine to send messages
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : May 9, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Status send_message(Acr_File *afpout, Acr_Message message)
{
   Acr_Status status;

   Alarmed_afp = afpout;
   Connection_timeout = FALSE;
   (void) signal(SIGALRM, timeout_handler);
   (void) alarm(Timeout_length);
   status = acr_output_dicom_message(afpout, message);
   (void) alarm(0);
   if (Connection_timeout) {
      status = ACR_CONNECTION_TIMEDOUT;
   }

   return status;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_set_client_timeout
@INPUT      : seconds - time in seconds to wait for i/o before timing out
@OUTPUT     : (none)
@RETURNS    : 
@DESCRIPTION: Routine to set the length of network timeouts. This time
              is used once the connection has been made. Note that although
              a double-precision argument is given, it is truncated to 
              integer before being used. A value of zero or less means that
              no timeout is used.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : September 15, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_set_client_timeout(double seconds)
{
   Timeout_length = (int) seconds;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_set_client_initial_timeout
@INPUT      : seconds - time in seconds to wait for initial connection 
                 before timing out
@OUTPUT     : (none)
@RETURNS    : 
@DESCRIPTION: Routine to set the length of initial network connection 
              timeouts. This time is used only when the initial connection
              is made. Note that although a double-precision argument is 
              given, it is truncated to integer before being used. A value 
              of zero or less means that no timeout is used.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : September 15, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_set_client_initial_timeout(double seconds)
{
   Initial_timeout_length = (int) seconds;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : timeout_handler
@INPUT      : 
@OUTPUT     : (none)
@RETURNS    : 
@DESCRIPTION: Routine to handle connection timeouts.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : March 10, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
/* ARGSUSED */
private void timeout_handler(int sig)
{
   Connection_timeout = TRUE;
   if (Alarmed_afp != NULL) {
      acr_dicom_set_eof(Alarmed_afp);
   }
   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_uid_equal
@INPUT      : uid1
              uid2
@OUTPUT     : (nothing)
@RETURNS    : TRUE if uid's are equal, FALSE otherwise
@DESCRIPTION: Responds to READYq message
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 21, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int acr_uid_equal(char *uid1, char *uid2)
{
   int len1, len2, i;

   len1 = strlen(uid1);
   len2 = strlen(uid2);

   /* Skip leading blanks */
   while (isspace(*uid1)) {uid1++;}
   while (isspace(*uid2)) {uid2++;}

   /* Skip trailing blanks */
   for (i=len1-1; (i >= 0) && isspace(uid1[i]); i++) {}
   if (isspace(uid1[i+1])) uid1[i+1] = '\0';
   for (i=len2-1; (i >= 0) && isspace(uid1[i]); i++) {}
   if (isspace(uid1[i+1])) uid1[i+1] = '\0';

   /* Compare the strings */
   return (strcmp(uid1, uid2) == 0);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_dicom_error
@INPUT      : status - status returned from dicom routine
              string - string to display before error message
@OUTPUT     : (nothing)
@RETURNS    : (nothing)
@DESCRIPTION: Prints out the string, followed by a colon, a blank and
              and error message based on the status.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 21, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void acr_dicom_error(Acr_Status status, char *string)
{
   char *error;

   /* Get the appropriate error string */
   error = acr_status_string(status);

   /* Print it out */
   (void) fprintf(stderr, "%s: %s\n", string, error);

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_release_dicom_association
@INPUT      : afpin - input stream
              afpout - output stream
@OUTPUT     : (none)
@RETURNS    : TRUE if release went smoothly.
@DESCRIPTION: Routine to shut down a dicom association.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : May 9, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int acr_release_dicom_association(Acr_File *afpin, Acr_File *afpout)
{
   Acr_Message message;
   Acr_Group group;
   Acr_Status status;
   int pdu_type;

   /* Compose a message */
   group = acr_create_group(DCM_PDU_GRPID);
   acr_group_add_element(group, 
       acr_create_element_short(DCM_PDU_Type, ACR_PDU_REL_RQ));
   message = acr_create_message();
   acr_message_add_group(message, group);

   /* Ship it out */
   status = send_message(afpout, message);
   acr_delete_message(message);
   if (status != ACR_OK) {
      acr_dicom_error(status, "Error sending release request");
      return FALSE;
   }

   /* Wait for an answer */
   status = receive_message(afpin, &message);
   if (status != ACR_OK) {
      acr_dicom_error(status, "Error receiving release reply");
      return FALSE;
   }

   /* Check it */
   pdu_type = acr_find_short(group, DCM_PDU_Type, -1);
   if (pdu_type != ACR_PDU_REL_RP) {
      (void) fprintf(stderr, "Bad reply to release request (PDU type %d)\n",
                     pdu_type);
      return FALSE;
   }

   return TRUE;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_send_group_list
@INPUT      : afpin - input stream
              afpout - output stream
              group_list - dicom dataset  to send
              sop_class_uid - this should just be abstract syntax
                 returned by make_association (or passed in to 
                 open_dicom_connection).
@OUTPUT     : (none)
@RETURNS    : TRUE if exchange went smoothly.
@DESCRIPTION: Routine to send a dicom data set and get the response.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : May 9, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int acr_send_group_list(Acr_File *afpin, Acr_File *afpout, 
                               Acr_Group group_list, 
                               char *sop_class_uid)
{
   static int message_id=0;
   int result;

   /* Send the message */
   if (!acr_transmit_group_list(afpout, group_list, sop_class_uid,
                                ++message_id)) {
      return FALSE;
   }

   /* Check the reply */
   result = acr_receive_reply(afpin);
   if (result < 0) return FALSE;
   if (result != message_id) {
      (void) fprintf(stderr, "Received reply to wrong message\n");
      return FALSE;
   }

   return TRUE;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_transmit_group_list
@INPUT      : afpout - output stream
              group_list - dicom dataset  to send
              sop_class_uid - this should just be abstract syntax
                 returned by make_association (or passed in to 
                 open_dicom_connection).
              message_id - Unique message id.
@OUTPUT     : (none)
@RETURNS    : TRUE if send went smoothly.
@DESCRIPTION: Routine to send a dicom data set. To check for the reply,
              call acr_receive_reply.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : May 9, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int acr_transmit_group_list(Acr_File *afpout, 
                                   Acr_Group group_list, 
                                   char *sop_class_uid,
                                   int message_id)
{
   Acr_Status status;
   Acr_Message message;
   Acr_Group output_group_list;
   char uid_buffer[64] = {'\0'};
   char *instance_uid;

   /* Get a UID for this object */
   instance_uid = acr_find_string(group_list, ACR_SOP_instance_UID, NULL);
   if (instance_uid == NULL) {
      instance_uid = strncpy(uid_buffer, acr_create_uid(), 
                             sizeof(uid_buffer)-1);
   }

   /* Create a command */
   output_group_list = acr_create_group(ACR_MESSAGE_GID);
   acr_group_add_element(output_group_list, 
       acr_create_element_string(ACR_Affected_SOP_class_UID, 
                                 sop_class_uid));
   acr_group_add_element(output_group_list, 
       acr_create_element_short(ACR_Command, ACR_C_STORE_RQ));
   acr_group_add_element(output_group_list, 
       acr_create_element_short(ACR_Message_id, message_id));
   acr_group_add_element(output_group_list, 
       acr_create_element_short(ACR_Priority, 0));
   acr_group_add_element(output_group_list, 
       acr_create_element_short(ACR_Dataset_type, 0));
   acr_group_add_element(output_group_list, 
       acr_create_element_string(ACR_Affected_SOP_instance_UID, instance_uid));

   /* Send the command */
   message = make_message(output_group_list);
   status = send_message(afpout, message);
   acr_delete_message(message);
   if (status != ACR_OK) {
      acr_dicom_error(status, "Error sending store request");
      return FALSE;
   }

   /* Add the class and instance UID's to the data group list */
   if (acr_find_group_element(group_list, ACR_SOP_class_UID) == NULL) {
      acr_insert_string(&group_list, ACR_SOP_class_UID, sop_class_uid);
   }
   if (acr_find_group_element(group_list, ACR_SOP_instance_UID) == NULL) {
      acr_insert_string(&group_list, ACR_SOP_instance_UID, instance_uid);
   }

   /* Add the study and series UID's to the group list */
   if (acr_find_group_element(group_list, ACR_Study_instance_UID) == NULL) {
      acr_insert_string(&group_list, ACR_Study_instance_UID, 
                        acr_create_uid());
   }
   if (acr_find_group_element(group_list, ACR_Series_instance_UID) == NULL) {
      acr_insert_string(&group_list, ACR_Series_instance_UID, 
                        acr_create_uid());
   }

   /* Make up some essential data if it is not already in the group list */
   if (acr_find_group_element(group_list, ACR_Image_type) == NULL) {
      acr_insert_string(&group_list, ACR_Image_type, 
                        "ORIGINAL\\PRIMARY\\UNDEFINED");
   }
   if (acr_find_group_element(group_list, ACR_Sequence_variant) == NULL) {
      acr_insert_string(&group_list, ACR_Sequence_variant, 
                        "NONE\\NONE");
   }
   if (acr_find_group_element(group_list, ACR_Image_position) == NULL) {
      acr_insert_string(&group_list, ACR_Image_position, 
                        "0\\0\\0");
   }
   if (acr_find_group_element(group_list, ACR_Image_orientation) == NULL) {
      acr_insert_string(&group_list, ACR_Image_orientation, 
                        "1\\0\\0\\0\\1\\0");
   }
   if (acr_find_group_element(group_list, ACR_Frame_of_reference_UID) 
       == NULL) {
      acr_insert_string(&group_list, ACR_Frame_of_reference_UID, 
                        acr_create_uid());
   }
   if (acr_find_group_element(group_list, ACR_Samples_per_pixel) == NULL) {
      acr_insert_short(&group_list, ACR_Samples_per_pixel, 1);
   }
   if (acr_find_group_element(group_list, ACR_Photometric_interpretation) 
       == NULL) {
      acr_insert_string(&group_list, ACR_Photometric_interpretation, 
                        "MONOCHROME2");
   }
   

   /* Send the data. Disconnect the message from the group list so that
      the latter is not deleted */
   message = make_message(group_list);
   status = send_message(afpout, message);
   acr_message_reset(message);
   acr_delete_message(message);
   if (status != ACR_OK) {
      acr_dicom_error(status, "Error sending store data");
      return FALSE;
   }

   return TRUE;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_receive_reply
@INPUT      : afpin - input stream
@OUTPUT     : (none)
@RETURNS    : Message id being reponded to or -1 if an error occurs
@DESCRIPTION: Routine to receive a reply to a send command.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : May 9, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int acr_receive_reply(Acr_File *afpin)
{
   Acr_Status status;
   Acr_Message message;
   Acr_Group reply_group;
   int command_status;
   int message_id;

   /* Wait for an answer */
   status = receive_message(afpin, &message);
   if (status != ACR_OK) {
      acr_dicom_error(status, "Error receiving store reply");
      return -1;
   }

   /* Check the reply */
   reply_group = acr_get_message_group_list(message);
   if (acr_find_short(reply_group, ACR_Command, -1) != ACR_C_STORE_RSP) {
      (void) fprintf(stderr, "Bad response to store request\n");
      return -1;
   }
   command_status = acr_find_short(reply_group, ACR_Status, -1);
   switch (command_status) {
   case ACR_SUCCESS:
      break;
   default:
      (void) fprintf(stderr, "Unrecognized store status (%d)\n", 
                     command_status);
      return -1;
   }
   message_id = acr_find_short(reply_group, ACR_Message_id_brt, -1);

   /* Delete the reply message */
   acr_delete_message(message);

   return message_id;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : make_message
@INPUT      : group_list
@OUTPUT     : (nothing)
@RETURNS    : output message.
@DESCRIPTION: Convert a group list into a message.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 24, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private Acr_Message make_message(Acr_Group group_list)
{
   Acr_Group next_group, group;
   Acr_Message output_message;

   /* Create the output message */
   output_message = acr_create_message();

   /* Loop through groups, adding them to the message */
   group = group_list;
   while (group != NULL) {
      next_group = acr_get_group_next(group);
      acr_set_group_next(group, NULL);
      acr_message_add_group(output_message, group);
      group = next_group;
   }

   return output_message;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_create_uid
@INPUT      : (none)
@OUTPUT     : (none)
@RETURNS    : pointer to internal buffer containing uid
@DESCRIPTION: Routine to generate a unique uid. The string is returned in
              an internal buffer space and will be overwritten by 
              subsequent calls.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 25, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public char *acr_create_uid(void)
{
   static char uid[64];
   time_t current_time;
   int offset;
   int maxlen;
   union {
      unsigned char ch[4];
      int ul;
   } host_id;
   static int counter = 0;

   /* Make up a new UID */
   host_id.ul = gethostid();
   current_time = time(NULL);
   (void) sprintf(uid, "1.%d.%d.%d.%d.%d.%d.%d", 
                  (int) 'I',(int) 'P',
                  (int) host_id.ch[0], (int) host_id.ch[1], 
                  (int) host_id.ch[2], (int) host_id.ch[3],
                  (int) getpid());
   offset = strlen(uid);
   maxlen = sizeof(uid) - 1 - offset;
   (void) strftime(&uid[offset], (size_t) maxlen, ".%Y%m%d%H%M%S", 
                   localtime(&current_time));
   (void) sprintf(&uid[strlen(uid)], ".%08d", counter++);

   return uid;
}
