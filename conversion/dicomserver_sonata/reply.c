/* ----------------------------- MNI Header -----------------------------------
@NAME       : reply.c
@DESCRIPTION: Routines for dealing with dicom messages.
@GLOBALS    : 
@CREATED    : January 28, 1997 (Peter Neelin)
@MODIFIED   : 
 * $Log: reply.c,v $
 * Revision 1.1  2003-08-15 19:52:55  leili
 * Initial revision
 *
 * Revision 1.2  2001/02/26 06:14:39  rhoge
 * modified to allow target directory to be passed as AE title (only 16 chars)
 *
 * Revision 1.1.1.1  2000/11/30 02:13:15  rhoge
 * imported sources to CVS repository on amoeba
 *
 * Revision 6.3  1999/10/29 17:51:57  neelin
 * Fixed Log keyword
 *
 * Revision 6.2  1999/08/05 20:01:16  neelin
 * Check for broken Siemens software using a list of implementation UIDs.
 *
 * Revision 6.1  1998/05/19  19:27:43  neelin
 * Test for Siemens Vision machine by looking for implementation uid
 * rather than AE title
 *
 * Revision 6.0  1997/09/12  13:24:27  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:25:26  neelin
 * Release of minc version 0.5
 *
 * Revision 4.1  1997/07/08  23:15:09  neelin
 * Added support for C_ECHO command.
 *
 * Revision 4.0  1997/05/07  20:06:20  neelin
 * Release of minc version 0.4
 *
 * Revision 1.1  1997/03/04  20:56:47  neelin
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

#include <dicomserver.h>

extern int Do_logging;

/* List of implementation UIDs for Siemens Vision scanners with broken 
   handling of transfer syntax. These should be in ascending order of
   software version, since the UID for version VB33A is used to identify 
   a change in element use. */
static char *SPI_Vision_Implementation_UIDs[] = {
   "2.16.840.1.113669.2.931128",
   "1.3.12.2.1107.5.1995.1",          /* Version VB33A */
   /* Added By Leili  */
   "1.3.12.2.1107.5.2",               /* Version MREASE_VA21A */ 
   NULL
};
/* Index into above array for version VB33A */
#define SPI_VISION_VB33A_INDEX 1
/* Global to indicate whether we have a pre VB33A version */
int SPI_Vision_version_pre33A = TRUE;

/* Macros */
#define SAVE_SHORT(group, elid, value) \
   acr_group_add_element(group, \
      acr_create_element_short(elid, (unsigned short) (value)))

/* ----------------------------- MNI Header -----------------------------------
@NAME       : uid_equal
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
private int uid_equal(char *uid1, char *uid2)
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
@NAME       : associate_reply
@INPUT      : input_message
@OUTPUT     : project_name - name to use for project file
              pres_context_id - presentation context id to use for output. If
                 this is < 0, then an error occurred.
              byte_order - byte order to use for messages
              vr_encoding - VR encoding to use for messages
              maximum_length - maximum length of output PDU's
@RETURNS    : output_message
@DESCRIPTION: Responds to an associate request message
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Message associate_reply(Acr_Message input_message, 
                                   char **project_name,
                                   int *pres_context_id,
                                   Acr_byte_order *byte_order,
                                   Acr_VR_encoding_type *vr_encoding,
                                   long *maximum_length)
{
   Acr_Group group, input_group;
   Acr_Element element, item, subitem, sublist;
   Acr_Element out_item, out_subitem, out_sublist, out_list;
   /***********************************/
   /* Added by Leili */
   Acr_Element my_element_test;
   /***********************************/
   int found_best;
   int best_pres_context_id, cur_pres_context_id;
   int best_transfer_syntax_priority;
   int use_implicit_little_endian;
   char *best_abstract_syntax, *best_transfer_syntax, *cur_syntax;
   int impuid;

   /* Print log message */
   if (Do_logging >= HIGH_LOGGING) {
      (void) fprintf(stderr, "\n\nReceived associate request message:\n");
      acr_dump_message(stderr, input_message);
   }

   /* Set default presentation context id to flag error */
   *pres_context_id = -1;

   /* Free project_name string if needed */
   if (*project_name != NULL) FREE(*project_name);

   /* Get the group list */
   input_group = acr_get_message_group_list(input_message);
      
   /* Get the project name from the DICOM application name */
   *project_name = strdup(acr_find_string(input_group, 
                                          DCM_PDU_Called_Ap_title, ""));
       
   /* Check that the project file is okay. If it is not found, try the host
      name */

   /* changed by rhoge so as not to clobber the aetitle in the
      unlikely event that it is being used to pass a file name (starts
      with `/' or `~') - this functionality is of limited usefulness, since 
      the AE title can not be longer than 16 characters */

   if (read_project_file(*project_name, NULL, NULL, NULL, NULL, 0)
       && !(!strncmp(*project_name,"/",1)||!strncmp(*project_name,"~",1))) { 
     FREE(*project_name);
     *project_name = NULL;
     if (read_project_file(*project_name, NULL, NULL, NULL, NULL, 0)) {
       return associate_reply_reject(input_message, 
				     ACR_ASSOC_RJ_CALLED_AP_TITLE_UNREC);
     }
   }
     
   /* Check for Siemens Vision implementation that lies about its 
      transfer syntaxes */

     /**********************************************/
     /* The if and else statement is commented out by leili */
     // if (0) {

     use_implicit_little_endian = FALSE;
     for (impuid=0; SPI_Vision_Implementation_UIDs[impuid] != NULL; impuid++) {
       if (uid_equal(acr_find_string(input_group, 
				     DCM_PDU_Implementation_class_uid, ""),
		     SPI_Vision_Implementation_UIDs[impuid])) {
         use_implicit_little_endian = TRUE;
         (void) fprintf(stderr, "just made the implicit little endian true\n ");
	   SPI_Vision_version_pre33A = (impuid < SPI_VISION_VB33A_INDEX);
         break;
       }
     }
     // } else {
     
     //use_implicit_little_endian = FALSE;

     //}
     /**********************************************/

     
   /* Get maximum length */
   *maximum_length = acr_find_long(input_group, DCM_PDU_Maximum_length, 0L);

   /* Get presentation context list */
   best_pres_context_id = -1;
   best_abstract_syntax = NULL;
   element = 
      acr_find_group_element(input_group, DCM_PDU_Presentation_context_list);
   if ((element == NULL) || !acr_element_is_sequence(element)) {
      (void) fprintf(stderr, "No presentation context list found\n");
      return associate_reply_reject(input_message, ACR_ASSOC_RJ_NO_REASON);
   }
   
   /* Loop over presentation contexts */
   found_best = FALSE;
   for (item = (Acr_Element) acr_get_element_data(element);
        (item != NULL) && acr_element_is_sequence(item) && !found_best;
        item = acr_get_element_next(item)) {

      /* Get presentation context info */
      sublist = (Acr_Element) acr_get_element_data(item);

      /* Get abstract syntax */
      subitem = acr_find_element_id(sublist, 
                                    DCM_PDU_Abstract_syntax);
      if (subitem == NULL) continue;
      cur_syntax = acr_get_element_string(subitem);

      /* Check whether this is either MR abstract syntax or we have
         already found one (we take the first one if we cannot find
         the one that we want) */
      if (uid_equal(cur_syntax, FAVORITE_ABSTRACT_SYNTAX))
         found_best = TRUE;
      else if (best_abstract_syntax != NULL) 
         continue;

      /* Save the abstract syntax */
      best_abstract_syntax = cur_syntax;
      /* Get presentation context id */
      subitem = acr_find_element_id(sublist, 
                                    DCM_PDU_Presentation_context_id);
      if (subitem != NULL)
         best_pres_context_id = acr_get_element_short(subitem);
      else {
         (void) fprintf(stderr, 
                        "No presentation context - internal error\n");
         return associate_reply_reject(input_message, ACR_ASSOC_RJ_NO_REASON);
      }

      /* Look for an appropriate transfer syntax */
      best_transfer_syntax = NULL;
      best_transfer_syntax_priority = 0;
      for (subitem = sublist;
           subitem != NULL;
           subitem=acr_find_element_id(acr_get_element_next(subitem), 
                                       DCM_PDU_Transfer_syntax)) {

         /* Check for syntaxes in descending order of preference */
         cur_syntax = acr_get_element_string(subitem);
         if (uid_equal(cur_syntax, ACR_EXPLICIT_VR_BIG_END_UID)) {
            if (best_transfer_syntax_priority < 3) {
               best_transfer_syntax_priority = 3;
               best_transfer_syntax = cur_syntax;
            }
         }
         else if (uid_equal(cur_syntax, ACR_EXPLICIT_VR_LITTLE_END_UID)) {
            if (best_transfer_syntax_priority < 2) {
               best_transfer_syntax_priority = 2;
               best_transfer_syntax = cur_syntax;
            }
         }
         else if (uid_equal(cur_syntax, ACR_IMPLICIT_VR_LITTLE_END_UID)) {
            if (best_transfer_syntax_priority < 1) {
               best_transfer_syntax_priority = 1;
               best_transfer_syntax = cur_syntax;
            }
         }

      }         /* Loop over transfer syntaxes */
   
   }            /* Loop over presentation contexts */
       

   /* Check for machines that lie about their ability to do 
      different transfer syntaxes */
   if (use_implicit_little_endian) {
      best_transfer_syntax = ACR_IMPLICIT_VR_LITTLE_END_UID;
   }


   /* Check that we found something useful */
   if ((best_pres_context_id < 0) || (best_abstract_syntax == NULL) ||
       (best_transfer_syntax == NULL)) {
      (void) fprintf(stderr, 
                     "Did not find understandable presentation context\n");
      return associate_reply_reject(input_message, ACR_ASSOC_RJ_NO_REASON);
   }
   
   /****************************************************************************/
   /* Set the transfer syntax information */
   /* This part was commented out by rick, put it back in the program by leili */

     if (uid_equal(best_transfer_syntax, ACR_EXPLICIT_VR_BIG_END_UID)) {
         *byte_order = ACR_BIG_ENDIAN;
         *vr_encoding = ACR_EXPLICIT_VR;
      }
      else if (uid_equal(best_transfer_syntax, ACR_EXPLICIT_VR_LITTLE_END_UID)) {
      
      /****************************************/
      /* this two lines were in rick's version */
  
      *byte_order = ACR_LITTLE_ENDIAN;
      *vr_encoding = ACR_EXPLICIT_VR;
      /******************************************/
         }
         else if (uid_equal(best_transfer_syntax, ACR_IMPLICIT_VR_LITTLE_END_UID)) {
            *byte_order = ACR_LITTLE_ENDIAN;
            *vr_encoding = ACR_IMPLICIT_VR;
         }
         else {
            (void) fprintf(stderr, 
                           "Did not understand transfer syntax.\n");
            return associate_reply_reject(input_message, ACR_ASSOC_RJ_NO_REASON);
         }
    
     /*************************************************************************/
   /* Create the reply */
   group = acr_create_group(DCM_PDU_GRPID);

   /* Save the PDU type */
   SAVE_SHORT(group, DCM_PDU_Type, ACR_PDU_ASSOC_AC);

   /* Save the caller and calling AE titles */
   acr_group_add_element(group,
      acr_create_element_string(DCM_PDU_Called_Ap_title,
         acr_find_string(input_group, DCM_PDU_Called_Ap_title, "")));

   /****************************************/
   /* Added by Leili */
   my_element_test =acr_create_element_string(DCM_PDU_Called_Ap_title,acr_find_string(input_group, DCM_PDU_Called_Ap_title, ""));
   (void) fprintf(stderr, "This is the called Ap_title: %s \n", my_element_test->data_pointer);
   /*****************************************/
 
   acr_group_add_element(group,
      acr_create_element_string(DCM_PDU_Calling_Ap_title,
         acr_find_string(input_group, DCM_PDU_Calling_Ap_title, "")));

   /****************************************/
   /* Added by Leili */
   my_element_test =acr_create_element_string(DCM_PDU_Calling_Ap_title,acr_find_string(input_group, DCM_PDU_Calling_Ap_title, ""));
   (void) fprintf(stderr, "This is the calling Ap_title: %s \n", my_element_test->data_pointer);
   /*****************************************/

   /* Add the application context name */
   acr_group_add_element(group,
      acr_create_element_string(DCM_PDU_Application_context,
         acr_find_string(input_group, DCM_PDU_Application_context, 
                         ACR_APPLICATION_CONTEXT_UID)));

   /* Loop over presentation contexts */
   item = (Acr_Element) 
      acr_get_element_data
         (acr_find_group_element(input_group, 
                                 DCM_PDU_Presentation_context_list));
   out_list = NULL;
   for (;(item != NULL); item = acr_get_element_next(item)) {

      if (!acr_element_is_sequence(item)) continue;

      /* Get presentation context info */
      sublist = (Acr_Element) acr_get_element_data(item);

      /* Save the id */
      subitem = acr_find_element_id(sublist, 
                                    DCM_PDU_Presentation_context_id);
      if (subitem == NULL) continue;
      cur_pres_context_id = acr_get_element_short(subitem);

      /* Create the presentation context */
      out_sublist = NULL;
      out_subitem = acr_create_element_short(DCM_PDU_Presentation_context_id,
                                             cur_pres_context_id);
      out_sublist = acr_element_list_add(out_sublist, out_subitem);

      /* Accept or reject */
      out_subitem = acr_create_element_short(DCM_PDU_Result,
         ((cur_pres_context_id == best_pres_context_id) ?
          ACR_ASSOC_PR_CN_ACCEPT : ACR_ASSOC_PR_CN_REJECT));
      out_sublist = acr_element_list_add(out_sublist, out_subitem);
        
      /* Add the transfer syntax */
      out_subitem = acr_create_element_string(DCM_PDU_Transfer_syntax,
                                              best_transfer_syntax);
      out_sublist = acr_element_list_add(out_sublist, out_subitem);

      /* Add this context to the list */
      out_item = 
         acr_create_element_sequence(DCM_PDU_Presentation_context_reply, 
                                     out_sublist);
      out_list = acr_element_list_add(out_list, out_item);
   }

   /* Create the presentation context list element */
   element = 
      acr_create_element_sequence(DCM_PDU_Presentation_context_reply_list, 
                                  out_list);
   acr_group_add_element(group, element);

   /* Add the user information */
   acr_group_add_element(group, 
                         acr_create_element_long(DCM_PDU_Maximum_length, 0L));

   /* Set the presentation context id to indicate success */
   *pres_context_id = best_pres_context_id;

   return make_message(group);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : associate_reply_reject
@INPUT      : input_message
@OUTPUT     : reason
@RETURNS    : output_message
@DESCRIPTION: Responds to an associate request message with a rejection
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 21, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
/* ARGSUSED */
public Acr_Message associate_reply_reject(Acr_Message input_message, 
                                          int reason)
{
   Acr_Group group;

   /* Create the reply */
   group = acr_create_group(DCM_PDU_GRPID);

   /* Save the PDU type */
   SAVE_SHORT(group, DCM_PDU_Type, ACR_PDU_ASSOC_RJ);

   /* Give the result, source and reason */
   SAVE_SHORT(group, DCM_PDU_Result, ACR_ASSOC_RJ_PERM);
   SAVE_SHORT(group, DCM_PDU_Source, ACR_ASSOC_RJ_USER);
   SAVE_SHORT(group, DCM_PDU_Reason, reason);

   return make_message(group);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : release_reply
@INPUT      : input_message
@OUTPUT     : (nothing)
@RETURNS    : output_message
@DESCRIPTION: Responds to READYq message
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Message release_reply(Acr_Message input_message)
{
   Acr_Group group;

   /* Print log message */
   if (Do_logging >= HIGH_LOGGING) {
      (void) fprintf(stderr, "\n\nReceived release request message:\n");
      acr_dump_message(stderr, input_message);
   }

   /* Create the reply */
   group = acr_create_group(DCM_PDU_GRPID);

   /* Save the PDU type */
   SAVE_SHORT(group, DCM_PDU_Type, ACR_PDU_REL_RP);

   return make_message(group);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : abort_reply
@INPUT      : input_message
@OUTPUT     : (nothing)
@RETURNS    : output_message
@DESCRIPTION: Responds to GCENDq message
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Message abort_reply(Acr_Message input_message)
{
   Acr_Group group;

   /* Print log message */
   if (Do_logging >= HIGH_LOGGING) {
      (void) fprintf(stderr, "\n\nReceived abort message:\n");
      acr_dump_message(stderr, input_message);
   }

   /* Create the reply */
   group = acr_copy_group_list(acr_get_message_group_list(input_message));

   return make_message(group);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : data_reply
@INPUT      : input_message
              file_prefix
@OUTPUT     : new_file_name (must be freed by caller)
@RETURNS    : output_message
@DESCRIPTION: Responds to SENDq message
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Message data_reply(Acr_Message input_message)
{
   Acr_Group group, input_group;
   int reply_command;
   

   /* Print log message */
   if (Do_logging >= HIGH_LOGGING) {
      (void) fprintf(stderr, "\n\nReceived data message:\n");
      acr_dump_message(stderr, input_message);
   }

   /* Get the input group list */
   input_group = acr_get_message_group_list(input_message);


   /* Figure out the reply that we need */
   switch (acr_find_short(input_group, ACR_Command, -1)) {
   case ACR_C_STORE_RQ:
      reply_command = ACR_C_STORE_RSP; break;
   case ACR_C_ECHO_RQ:
      reply_command = ACR_C_ECHO_RSP; break;
   default:
      reply_command = ACR_C_ECHO_RSP; break;
   }

   /* Create the reply */
   group = acr_create_group(ACR_MESSAGE_GID);

   /* Save appropriate stuff */
   acr_group_add_element(group,
      acr_create_element_string(ACR_Affected_SOP_class_UID, 
         acr_find_string(input_group, ACR_Affected_SOP_class_UID, "")));
   SAVE_SHORT(group, ACR_Command, reply_command);
   SAVE_SHORT(group, ACR_Message_id_brt, 
              acr_find_short(input_group, ACR_Message_id, 0));
   SAVE_SHORT(group, ACR_Dataset_type, ACR_NULL_DATASET);
   SAVE_SHORT(group, ACR_Status, ACR_SUCCESS);
   if (reply_command == ACR_C_STORE_RSP) {
      acr_group_add_element(group,
         acr_create_element_string(ACR_Affected_SOP_instance_UID, 
            acr_find_string(input_group, ACR_Affected_SOP_instance_UID, "")));
   }
   
   return make_message(group);

}

