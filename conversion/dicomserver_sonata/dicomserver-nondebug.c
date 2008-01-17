/* ----------------------------- MNI Header -----------------------------------
@NAME       : dicomserver.c
@DESCRIPTION: Program to receive images from Siemens Vision.
@GLOBALS    : 
@CREATED    : January 28, 1997 (Peter Neelin)
@MODIFIED   : 

Modified by R. Hoge Feb. 2000 to handle acquisition loop dynamic scans
on Siemens Sonata system

 * $Log: dicomserver-nondebug.c,v $
 * Revision 1.3  2008-01-17 02:33:01  rotor
 *  * removed all rcsids
 *  * removed a bunch of ^L's that somehow crept in
 *  * removed old (and outdated) BUGS file
 *
 * Revision 1.2  2008/01/12 19:08:14  stever
 * Add __attribute__ ((unused)) to all rcsid variables.
 *
 * Revision 1.1.1.1  2003/08/15 19:52:55  leili
 * Leili's dicom server for sonata
 *
 * Revision 1.9  2002/03/22 19:19:36  rhoge
 * Numerous fixes -
 * - handle Numaris 4 Dicom patient name
 * - option to cleanup input files
 * - command option
 * - list-only option
 * - debug mode
 * - user supplied name, idstr
 * - anonymization
 *
 * Revision 1.8  2001/12/31 18:27:21  rhoge
 * modifications for dicomreader processing of Numaris 4 dicom files - at
 * this point code compiles without warning, but does not deal with
 * mosaiced files.  Also will probably not work at this time for Numaris
 * 3 .ima files.  dicomserver may also not be functional...
 *
 * Revision 1.7  2001/08/29 20:55:53  rhoge
 * added -help option
 *
 * Revision 1.6  2001/07/19 17:43:46  rhoge
 * added -nofork command line option to prevent forking
 *
 * Revision 1.5  2001/02/26 13:37:59  rhoge
 * redirect sderr to /dev/null if no logging - prevents hangups due to
 * printfs dumping text into connection stream?
 *
 * Revision 1.4  2001/02/26 06:17:42  rhoge
 * changed tmp dir assignment to be consistent, always uses stdio.h stuff.
 * also added command-line flags for destination dir, logging level, and
 * retention of dicom files (are put in dest dir - must be specified)
 *
 * Revision 1.3  2000/12/14 21:14:58  rhoge
 * added macro (NO_FORK) to prevent forking if uncommented
 *
 * Revision 1.2  2000/12/13 13:22:18  rhoge
 * experimental changes to forking code for minc file creation - should
 * be no change
 *
 * Revision 1.1.1.1  2000/11/30 02:13:15  rhoge
 * imported sources to CVS repository on amoeba
 *
 * Revision 6.1  1999/10/29 17:51:55  neelin
 * Fixed Log keyword
 *
 * Revision 6.0  1997/09/12 13:24:27  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:25:26  neelin
 * Release of minc version 0.5
 *
 * Revision 4.2  1997/07/10  17:35:35  neelin
 * Changed error handling and fixed message deletion.
 *
 * Revision 4.1  1997/07/08  23:15:09  neelin
 * Added support for C_ECHO command.
 *
 * Revision 4.0  1997/05/07  20:06:20  neelin
 * Release of minc version 0.4
 *
 * Revision 1.2  1997/03/11  13:10:48  neelin
 * Working version of dicomserver.
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

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <dicomserver.h>

/* Global for minc history */
extern char *minc_history;

/* State of server. Note that DISCONNECTING is used for a high-level
   protocol error and TERMINATING is used for a low-level error or
   end of input */
typedef enum {
   WAITING_FOR_ASSOCIATION, WAITING_FOR_DATA, DISCONNECTING, TERMINATING
} Server_state;

/* stuff added by rhoge */

#define EXTREME_LOGGING 10 /* rhoge */

/* Do we do logging? */
/********************/
//int Do_logging = 0;    /* Belong to rhoge, commented out by leili*/
/*added by Leili:*/
int Do_logging = 
#ifndef DO_HIGH_LOGGING
  LOW_LOGGING;
# else
  HIGH_LOGGING;
# endif 
/******************/

/* Changed by leili from False to True */
int Fork = TRUE;
/*****************/

/* Do we keep files or are they temporary? */
static int Keep_files = 
#ifndef KEEP_FILES
   FALSE;
#else
   TRUE;
#endif
/******************/
/* added by Leili */
/* In what directory do we run? */
static char *run_dir = "/data/fmri/transfer/tmp";
/******************/

/* Globals for handling connection timeouts */
int Connection_timeout = FALSE;
Acr_File *Alarmed_afp = NULL;
File_Type file_type = N4DCM ; /* type of input files */
int N4_OFFSET;
char *pname;
int main(int argc, char *argv[])
{
   char *pname;
   Acr_File *afpin, *afpout;
   Acr_Status status;
   Server_state state;
   int acr_command;
   Acr_Group group_list;
   Acr_Message input_message, output_message;
   int exit_status;
   char exit_string[256];
   char **file_list;
   Data_Object_Info **file_info_list;
   int num_files, num_files_alloc;
   static char file_prefix_string[L_tmpnam+1] = "dicomserver";
   char *file_prefix = file_prefix_string;
   //Added by Leili 
   char *file_prefix2 = "/usr/local/dicom_keep/"; 
   char *temp_dir;
   int continue_looping;
   FILE *fptemp;
   char last_file_name[256];
   char *project_name = NULL;
   char logfilename[256];
   int pdu_type;
   int process_files, have_extra_file;
   Acr_byte_order byte_order;
   Acr_VR_encoding_type vr_encoding;
   int pres_context_id;
   long maximum_length;
   pid_t server_pid, child_pid;
   int statptr;

   /* added by rhoge */
   int ix;
   int UseArgDir = 0;
   char OutDir[128];

   /* Get server process id */
   server_pid = getpid();
   
   /*************************/
   
   /* Change to tmp directory -
      note that this will be default file destination if no aetitle
      or command-line specification are received (rhoge) */
   /* commented out by leili */
   
   //(void) chdir(P_tmpdir); 
   
   /* added by leili */
   /* change to tmp directory */
   if (run_dir != NULL) {
    (void) chdir(run_dir);
     } 
   /*****************************/

   /* Create minc history string */
   {
      char *string;
      string = "dicomserver";
      minc_history = time_stamp(1, &string);
   }

   /* read in all the input pars and file names */

   for (ix = 1; ix<argc; ix++) { 
    
      if (!strncmp(argv[ix],"-destdir",8)) { 
       ix++; 
       UseArgDir = 1; 
       strcpy(OutDir,argv[ix]);  
        (void) strcat(OutDir, "/");  /* make sure path ends with slash */  
    }    
        else if (!strncmp(argv[ix],"-log",4)) { 
         ix++; 
         Do_logging = atoi(argv[ix]); 
       } 
      else if (!strncmp(argv[ix],"-fork",5)) {  
        Fork = TRUE; 
      } 
      else if (!strncmp(argv[ix],"-keep_dcm",9)) {  
        Keep_files = TRUE; 
      } 
      else if (!strncmp(argv[ix],"-help",9)) {  
         (void) fprintf(stderr,"Usage:  dicomserver [<options>]\nOptions:\n\t-log <n>\n\t-fork\n\t-keep_dcm\n\t-help\n"); 
         exit(EXIT_SUCCESS); 
       } 
       else { 
         (void) fprintf(stderr,"Usage:  dicomserver [<options>]\nOptions:\n\t-log <n>\n\t-fork\n\t-keep_dcm\n\t-help\n"); 
         exit(EXIT_FAILURE); 
	 } 
	 }
   
   /* Re-open stderr if we are logging */
   if (Do_logging > NO_LOGGING) {
      (void) sprintf(logfilename, "/data/fmri/transfer/logs/dicomserver-%d.log", 
                     (int) getpid());
      (void) freopen(logfilename, "w", stderr);
      setbuf(stderr, NULL);
   } else {
      (void) sprintf(logfilename, "/dev/null");
      (void) freopen(logfilename, "w", stderr);
      setbuf(stderr, NULL);
   }

   // set up type of connection (Syngo?)
   file_type = N4DCM; // default
   N4_OFFSET = 0; 

   /* Print message at start */
   pname = argv[0];
   if (Do_logging >= LOW_LOGGING) {
      (void) fprintf(stderr, "%s: Started dicom server.\n", pname);
   }

   /* Make connection */
   open_connection(argc, argv, &afpin, &afpout);
   

   /* Check that the connection was made */
   if ((afpin == NULL) || (afpout == NULL)) {
      (void) fprintf(stderr, "%s: Error opening connection.\n", pname);
      exit(EXIT_FAILURE);
   }
   
     

   /* Print connection message */
   if (Do_logging >= HIGH_LOGGING) {
      (void) fprintf(stderr, "%s: Connection accepted.\n", pname);
   }

#ifdef DO_INPUT_TRACING
   /* Enable input tracing */
   acr_dicom_enable_trace(afpin);
   acr_dicom_enable_trace(afpout);
#endif

   /* Create file prefix. Create the temporary file to avoid file name 
      clashes */
   /*****************************/
   /* Commented out by leili */ 
   //temp_dir = NULL;
   //if ( !Keep_files) {
   //   (void) tmpnam(file_prefix);
   //   if (mkdir(file_prefix, (mode_t) 0777)) {
   //      (void) fprintf(stderr, 
   //         "%s: Unable to create directory for temporary files.\n",
   //                     pname);
   //      perror(pname);
   //      exit(EXIT_FAILURE);
   //   }
   //   temp_dir = strdup(file_prefix);
   //   (void) strcat(file_prefix, "/dicom");
   //}
   
   // else {
   //  file_prefix=strdup(OutDir);

   //  if (mkdir(file_prefix, (mode_t) 0777)) {
   //    (void) fprintf(stderr, 
   //         "Directory %s exists...\n",file_prefix);
   //   }
   //  temp_dir = strdup(file_prefix);
   //   (void) strcat(file_prefix, "/dicom");
   // }
   /****************************/
   /* Added by Leili           */
   temp_dir = NULL;
   if (! Keep_files){
     //temp_dir = tempnam(run_dir, NULL);
    temp_dir = tempnam(run_dir, NULL);
    if(mkdir(temp_dir, (mode_t) 0777)){
      (void) fprintf(stderr, 
            "%s: Unable to create directory for temporary files.\n",
                        pname);
      perror(pname);
      exit(EXIT_FAILURE);
    }
    (void)strcpy(file_prefix, temp_dir);
    (void) strcat(file_prefix, "/");
    //(void) strcat(file_prefix, "/dicom");
   } 
   /*****************************/
   (void) fprintf(stderr, "About to begin the space allocation \n"); /* Leili */

   /* Get space for file lists */
   num_files_alloc = FILE_ALLOC_INCREMENT;
   file_list = MALLOC((size_t) num_files_alloc * sizeof(*file_list));
   file_info_list = MALLOC(num_files_alloc * sizeof(*file_info_list));
    
   
   /* Loop while reading messages */
   state = WAITING_FOR_ASSOCIATION;
   continue_looping = TRUE;
   num_files = 0;

   (void) fprintf(stderr, "About to start reading a message \n"); /* Leili */
   while (continue_looping) {

      /* Wait for any children that have finished */
      while ((child_pid=wait3(&statptr, WNOHANG, NULL)) > 0) {}

      /* If there are children, slow down the processing */
      (void) fprintf(stderr, "If we have children, slow down \n"); /* Leili */
      if (child_pid == 0) {
         (void) sleep((unsigned int) SERVER_SLEEP_TIME);
      }
      (void) fprintf(stderr, "Start reading the message\n"); /* Leili */
      /* Read in the message */
      Alarmed_afp = afpin;
      (void) signal(SIGALRM, timeout_handler);
      (void) alarm(CONNECTION_TIMEOUT);
  
   if (Do_logging > HIGH_LOGGING) 
     fprintf(stderr,"\nWaiting for dicom message...\n"); /* rhoge */
      
   /* chokes here at end of asynchronous transfer */
      status=acr_input_dicom_message(afpin, &input_message); 
      (void) fprintf(stderr, "The status of the message is: %s \n", acr_status_string(status)); /* Leili */ 

      if (Do_logging > HIGH_LOGGING) { /* rhoge */
	fprintf(stderr,"\nGot message.\n");
	fprintf(stderr,"\nCalling `alarm'...\n");
      }

      (void) alarm(0);

      if (Do_logging > HIGH_LOGGING) 
	fprintf(stderr,"\nAlarm called.\n"); /* rhoge */
      
      (void) fprintf(stderr,"Checking for an error  \n"); /* Leili */
      /* Check for error */
      if (status != ACR_OK) {
         continue_looping = FALSE;
         state = TERMINATING;
         break;
      }

      /* Set flags indicating whether we should do anything with the files
         and whether the file lists contain an extra file */
      process_files = FALSE;
      have_extra_file = FALSE;
      /* Get group list */
      group_list = acr_get_message_group_list(input_message);
      
      /* Get PDU type. Default is data transfer */
      pdu_type = acr_find_short(group_list, DCM_PDU_Type, ACR_PDU_DATA_TF);
      (void) fprintf(stderr,"We got the pdu and it is: %d\n", pdu_type); /* Leili */
  

      /* Deal with PDU state */
      switch (pdu_type) {
      
      /* Associate request */
      case ACR_PDU_ASSOC_RQ:

	if (Do_logging > HIGH_LOGGING) /* rhoge */
	  fprintf(stderr,"\n Associate request is ACR_PDU_ASSOC_RQ\n");

         if (state != WAITING_FOR_ASSOCIATION) {
            status = ACR_HIGH_LEVEL_ERROR;
            state = DISCONNECTING;
            break;
         }

         /* Work out reply and get connection info */
         (void) fprintf(stderr,"About to go to reply program\n"); /* Leili */
         output_message = associate_reply(input_message, &project_name, 
                                          &pres_context_id, &byte_order,
                                          &vr_encoding, &maximum_length);
	 (void) fprintf(stderr,"We finished with the reply program\n"); /* Leili */

         /* Modify the input and output streams according to the 
            connection info */
         acr_set_byte_order(afpin, byte_order);
         acr_set_vr_encoding(afpin, vr_encoding);
         acr_set_byte_order(afpout, byte_order);
         acr_set_vr_encoding(afpout, vr_encoding);
         acr_set_dicom_pres_context_id(afpout, pres_context_id);
         acr_set_dicom_maximum_length(afpout, maximum_length);

         /* Get ready for files */
         num_files = 0;
         state = WAITING_FOR_DATA;

         break;

      /* Release */
      case ACR_PDU_REL_RQ:

	if (Do_logging > HIGH_LOGGING) /* rhoge */
	  fprintf(stderr,"\nAssociate request is ACR_PDU_REL_RQ\n");

         if (state != WAITING_FOR_DATA) {
            status = ACR_HIGH_LEVEL_ERROR;
            state = DISCONNECTING;
            break;
         }
         output_message = release_reply(input_message);
         state = TERMINATING;
         process_files = TRUE;

	 if (Do_logging > HIGH_LOGGING) /* rhoge */
	   fprintf(stderr,
		   "\nReceived release request; process flag set.\n");

         break;

      /* Abort */
      case ACR_PDU_ABORT_RQ:

	 if (Do_logging > HIGH_LOGGING) /* rhoge */
	   fprintf(stderr,"\nAssociate request is ACR_PDU_ABORT_RQ\n");

         output_message = abort_reply(input_message);
         state = TERMINATING;
         break;

      /* Data transfer */
      case ACR_PDU_DATA_TF:

	if (Do_logging > HIGH_LOGGING) /* rhoge */
	   fprintf(stderr,"\nAssociate request is ACR_PDU_DATA_TF\n");

         /* Check state */
         if (state != WAITING_FOR_DATA) {
            status = ACR_HIGH_LEVEL_ERROR;
            state = DISCONNECTING;
            break;
         }

         /* Check command and compose a reply */
         acr_command = acr_find_short(group_list, ACR_Command, -1);
         switch (acr_command) {
         case ACR_C_STORE_RQ:
         case ACR_C_ECHO_RQ:
            output_message = data_reply(input_message);
            break;
         default:
            status = ACR_HIGH_LEVEL_ERROR;
            state = DISCONNECTING;
            break;
         }

         /* Carry on only if we have a store command */
         if (acr_command != ACR_C_STORE_RQ) break;

         /* Get rid of the command groups */
         group_list = skip_command_groups(group_list);

         /* Was the data attached to the command? If not, read in the next
            message - it should contain the data */
         if (group_list == NULL) {

            /* Delete the previous message */
            if (input_message != NULL)
               acr_delete_message(input_message);

            /* Read the data and check the status */
            Alarmed_afp = afpin;
            (void) signal(SIGALRM, timeout_handler);
            (void) alarm(CONNECTION_TIMEOUT);
            status=acr_input_dicom_message(afpin, &input_message);
            (void) alarm(0);
            if (status != ACR_OK) {
               state = DISCONNECTING;
               break;
            }

            /* Check that we have a data PDU */
            group_list = acr_get_message_group_list(input_message);
            if (acr_find_short(group_list, DCM_PDU_Type, ACR_PDU_DATA_TF)
                != ACR_PDU_DATA_TF) {
               status = ACR_HIGH_LEVEL_ERROR;
               state = DISCONNECTING;
               break;
            }

            /* Skip command groups and check for no data */
            group_list = skip_command_groups(group_list);
            if (group_list == NULL) break;

         }
         (void) fprintf(stderr,"End of while loop, we are out of the loop \n"); /* Leili */

         /* Extend file list if necessary */
         if (num_files >= num_files_alloc) {
            num_files_alloc = num_files + FILE_ALLOC_INCREMENT;
            file_list = REALLOC(file_list, 
                                num_files_alloc * sizeof(*file_list));
            file_info_list = 
               REALLOC(file_info_list, 
                       num_files_alloc * sizeof(*file_info_list));
         }
         file_list[num_files] = NULL;
         file_info_list[num_files] = 
            MALLOC(sizeof(*file_info_list[num_files]));

         /* Save the object */
         //save_transferred_object(group_list, 
         //                        file_prefix, &file_list[num_files],
         //                        file_info_list[num_files]);
         /* Added by leili */
         save_transferred_object(group_list, 
                                 file_prefix2, &file_list[num_files],
                                 file_info_list[num_files]);
         
         (void) fprintf(stderr,"Finished saving transfered object number %d \n", (num_files)+1); /* Leili */
         num_files++;

         if (Do_logging >= LOW_LOGGING) {
            (void) fprintf(stderr, "   Copied %s\n", file_list[num_files-1]);
         }
         (void) fprintf(stderr,"Number of files are: %d \n", num_files); /* Leili */

	 if (Do_logging > HIGH_LOGGING) { /* rhoge */
	   fprintf(stderr,
		   "About to check if we're done file group\n");

	   fprintf(stderr,"\nnum_files: \t%d",num_files);
	   fprintf(stderr,"\nfile_info_list[num_files-1]->study_id: \t%.6f",
		   file_info_list[num_files-1]->study_id);
	   fprintf(stderr,"\nfile_info_list[0]->study_id: \t%.6f",
		   file_info_list[0]->study_id);
	   fprintf(stderr,"\nfile_info_list[num_files-1]->acq_id: \t%d",
		   file_info_list[num_files-1]->acq_id);
	   fprintf(stderr,"\nfile_info_list[0]->acq_id: \t%d",
		   file_info_list[0]->acq_id);

	 }	 

         /* Check whether we have reached the end of a group of files */
         if (num_files > 1) {
            if ((file_info_list[num_files-1]->study_id != 
                 file_info_list[0]->study_id) ||
                (file_info_list[num_files-1]->acq_id != 
                 file_info_list[0]->acq_id)) {
               process_files = TRUE;
               have_extra_file = TRUE;

	       if (Do_logging > HIGH_LOGGING) /* rhoge */
		 fprintf(stderr,
			 "\nend of file group detected, process flag set.\n");
            }
         }

	 if (Do_logging > HIGH_LOGGING) /* rhoge */
	   fprintf(stderr,
		   "\ndone checking for end of file group\n");

         break;

         /* Unknown command */
         default:

	   if (Do_logging > HIGH_LOGGING) /* rhoge */
	     fprintf(stderr,"\nAssociate request is default (an error)\n");
            status = ACR_HIGH_LEVEL_ERROR;
            state = DISCONNECTING;
            break;

      }        /* End of switch on pdu_type */
      
      (void) fprintf(stderr,"We break and came out since we didn't have the store command\n"); /* Leili */

      /* Delete input message */
      if (input_message != NULL)
         acr_delete_message(input_message);

      /* Use the files if we have a complete acquisition */
      if (process_files) {

	if (Do_logging > HIGH_LOGGING) /* rhoge */
	  fprintf(stderr,"\nEntered the `process files' block...\n");

         /* Log the fact */
         if (Do_logging >= LOW_LOGGING) {
            (void) fprintf(stderr, "\nCopied one acquisition.\n");
         }

         /* Check for file from next acquisition */
         if (have_extra_file) num_files--;

	 /* uncomment to prevent forking */
	 /* #define NO_FORK */

	 if (Fork) {
	   /* Fork child to process the files */
	   child_pid = fork();
	 } else {
	   child_pid = 0;
	 }

         if (child_pid > 0) {      /* Parent process */
            if (Do_logging >= LOW_LOGGING) {
               (void) fprintf(stderr, 
                              "Forked process to create minc files.\n");
            }

         }                         /* Error forking */
         else if (child_pid < 0) {
            (void) fprintf(stderr, 
                           "Error forking child to create minc file\n");
            return;
         }
         else {                    /* Child process */
	     /*******************************************/
             /* Added by Leili                          */
	     /* Close file descriptors to avoid buffering problems.
                STDERR is sometimes left open, therefore this command 
                maybe needed */    
	     if(Fork){
              int fd;
              for(fd=getdtablesize()-1; fd>= 0; fd--) {
		if(fd != 2){
                  (void) close(fd);
                }
              }
             }
             /******************************************/ 
	     /* Do something with the files */
             (void) fprintf(stderr, "We are about to do something with the files.\n"); /* Leili */
             (void) fprintf(stderr, "The project name is:%s\n", project_name); /* Leili */
             (void) fprintf(stderr, "The pname is:%s\n", pname); /* Leili */
	     use_the_files(project_name, num_files, file_list, 
			   file_info_list,UseArgDir,OutDir);
             
 
            /* Remove the temporary files */
            cleanup_files(num_files, file_list);

            /* Remove the temporary directory if the server has finished */
	    /* (this does not seem to work all the time) */
            if ((temp_dir != NULL) && (kill(server_pid, 0) != 0)) {
               cleanup_files(1, &temp_dir);
            }

            /* Print message about child finishing */
            if (Do_logging >= LOW_LOGGING) {
               (void) fprintf(stderr, "Minc creation process finished.\n");
            }

	    if (Fork) {
	      /* Exit from child */
	      exit(EXIT_SUCCESS);
	    }

         }         /* End of child process */

         /* Put blank line in log file */
         if (Do_logging >= LOW_LOGGING) {
            (void) fprintf(stderr, "\n");
         }

         /* Reset the lists */
         free_list(num_files, file_list, file_info_list);
         if (have_extra_file) {
            file_list[0] = file_list[num_files];
            file_info_list[0] = file_info_list[num_files];
            file_list[num_files] = NULL;
            file_info_list[num_files] = NULL;
         }
         num_files = (have_extra_file ? 1 : 0);
      } // end of if(process files)

      if (Do_logging > HIGH_LOGGING) /* rhoge */
	fprintf(stderr,
		"\n just passed `process files' loop\n");

      /* Check for disconnection */
      if (state == DISCONNECTING) {
         continue_looping = FALSE;
         break;
      }

      /* Send reply */
      Alarmed_afp = afpout;
      (void) signal(SIGALRM, timeout_handler);
      (void) alarm(CONNECTION_TIMEOUT);
      status = acr_output_dicom_message(afpout, output_message);
      (void) alarm(0);

      /* Delete output message */
      if (output_message != NULL)
         acr_delete_message(output_message);

      if (status != ACR_OK) {
         state = TERMINATING;
         break;
      }

      if (Do_logging > HIGH_LOGGING) /* rhoge */
	fprintf(stderr,
		"\nbottom of loop over messages\n");

   }        /* End of loop over messages */

   if (Do_logging > HIGH_LOGGING) /* rhoge */
     fprintf(stderr,
	     "\njust out of loop over messages\n");

   /* Free the input and output streams */
   acr_close_dicom_file(afpin);
   acr_close_dicom_file(afpout);

   /* Save name of first file in last set transferred */
   if ((num_files > 0) && (file_list[0] != NULL)) {
      last_file_name[sizeof(last_file_name) - 1] = '\0';
      (void) strncpy(last_file_name, file_list[0], sizeof(last_file_name)-1);
   }
   else {
      last_file_name[0] = '\0';
   }

   /* Clean up files, if needed */
   if (num_files > 0) {
      cleanup_files(num_files, file_list);
      free_list(num_files, file_list, file_info_list);
      num_files = 0;
   }
   FREE(file_list);
   FREE(file_info_list);
   
   /* Remove the file prefix directory (this only happens if it is empty). */
   cleanup_files(1, &temp_dir);
   FREE(temp_dir);

   /* Check for connection timeout */
   if (Connection_timeout) {
      (void) fprintf(stderr, "Connection timed out.\n");
   }

   /* Print final message */
   if ((status == ACR_OK) || (status == ACR_END_OF_INPUT)) {
      (void) sprintf(exit_string, "Finished transfer.");
      exit_status = EXIT_SUCCESS;
   }
   else {
      (void) sprintf(exit_string, "%s. Disconnecting.", 
                     acr_status_string(status));
      exit_status = EXIT_FAILURE;
   }
   (void) fprintf(stderr, "The logging set to be %d\n", Do_logging); /* Leili */
   (void) fprintf(stderr, "The status of the message is: %s \n", acr_status_string(status)); /* Leili */

   if (Do_logging >= LOW_LOGGING) {
      (void) fprintf(stderr, "\n%s: %s\n", pname, exit_string);
   }

   if ((status != ACR_OK) && (status != ACR_END_OF_INPUT)) {
      if (SYSTEM_LOG != NULL) {
         if ((fptemp = fopen(SYSTEM_LOG, "w")) != NULL) {
            if ((int) strlen(last_file_name) > 0) {
               (void) fprintf(fptemp, "%s: File \"%s\"\n",
                              pname, last_file_name);
            }
            (void) fprintf(fptemp, "%s: %s\n", pname, exit_string);
            (void) fclose(fptemp);
         }
      }
   }

   /* Free the project_name string */
   if (project_name != NULL) FREE(project_name);

   exit(exit_status);

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
public void timeout_handler(int sig)
{
   Connection_timeout = TRUE;
   if (Alarmed_afp != NULL) {
      acr_dicom_set_eof(Alarmed_afp);
   }
   return;
}
/* ----------------------------- MNI Header -----------------------------------
@NAME       : skip_command_groups
@INPUT      : group_list
@OUTPUT     : (none)
@RETURNS    : Pointer to head of group list
@DESCRIPTION: Skips over command groups in a group list, returning the rest
              of the list.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : March 7, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Group skip_command_groups(Acr_Group group_list)
{
   while ((group_list != NULL) &&
          ((acr_get_group_group(group_list) == DCM_PDU_GRPID) ||
           (acr_get_group_group(group_list) == ACR_MESSAGE_GID))) {
      group_list = acr_get_group_next(group_list);
   }

   return group_list;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : cleanup_files
@INPUT      : num_files - number of files in list
              file_list - array of file names
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Removes files.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void cleanup_files(int num_files, char *file_list[])
{
   int i;

   if (Keep_files) return;

   for (i=0; i < num_files; i++) {
      if (file_list[i] != NULL) {
         (void) remove(file_list[i]);
      }
   }

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : free_list
@INPUT      : num_files - number of files in list
              file_list - array of file names
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Frees up things pointed to in pointer arrays. Does not free
              the arrays themselves.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void free_list(int num_files, char **file_list, 
                      Data_Object_Info **file_info_list)
{
   int i;

   for (i=0; i < num_files; i++) {
      if (file_list[i] != NULL) {
         FREE(file_list[i]);
      }
      if (file_info_list[i] != NULL) {
         FREE(file_info_list[i]);
      }
   }

   return;
}

