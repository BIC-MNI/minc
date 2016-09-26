/* ----------------------------- MNI Header -----------------------------------
@NAME       : dump_acr_nema.c
@DESCRIPTION: Program to dump the contents of an acr-nema file.
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 24, 1993 (Peter Neelin)
@MODIFIED   : 
 * $Log: dump_acr_nema.c,v $
 * Revision 6.7  2005/03/11 22:19:59  bert
 * add '-t' option to parse files which contain lists of field names with corresponding group and element id's.
 *
 * Revision 6.6  2004/10/29 13:08:41  rotor
 *  * rewrote Makefile with no dependency on a minc distribution
 *  * removed all references to the abominable minc_def.h
 *  * I should autoconf this really, but this is old code that
 *      is now replaced by Jon Harlaps PERL version..
 *
 * Revision 6.5  2001/11/08 14:17:05  neelin
 * Added acr_test_dicom_file to allow reading of DICOM part 10 format
 * files. This function also calls acr_test_byte_order to set up the stream
 * properly and can be used as a direct replacement for that function.
 * This set of changes does NOT include the ability to write part 10 files.
 *
 * Revision 6.4  2000/05/01 17:54:45  neelin
 * Fixed handling of test for byte order.
 *
 * Revision 6.3  2000/05/01 13:59:55  neelin
 * Added -e option to allow reading data streams with explicit VR.
 *
 * Revision 6.2  2000/04/28 15:02:01  neelin
 * Added more general argument processing (but not with ParseArgv).
 * Added support for ignoring non-fatal protocol errors.
 * Added support for user-specified byte-order.
 *
 * Revision 6.1  1999/10/29 17:51:51  neelin
 * Fixed Log keyword
 *
 * Revision 6.0  1997/09/12 13:23:59  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:25:00  neelin
 * Release of minc version 0.5
 *
 * Revision 4.1  1997/07/10  17:14:38  neelin
 * Added more status codes and function to return status string.
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
 * Revision 2.1  1995/02/06  14:12:55  neelin
 * Added argument to specify maximum group id to dump.
 *
 * Revision 2.0  94/09/28  10:36:09  neelin
 * Release of minc version 0.2
 * 
 * Revision 1.6  94/09/28  10:35:53  neelin
 * Pre-release
 * 
 * Revision 1.5  94/05/18  08:48:05  neelin
 * Changed some ACR_OTHER_ERROR's to ACR_ABNORMAL_END_OF_OUTPUT.
 * 
 * Revision 1.4  94/04/07  10:04:58  neelin
 * Added status ACR_ABNORMAL_END_OF_INPUT and changed some ACR_PROTOCOL_ERRORs
 * to that or ACR_OTHER_ERROR.
 * Added #ifdef lint to DEFINE_ELEMENT.
 * 
 * Revision 1.3  93/11/25  10:35:33  neelin
 * Added byte-order test and file free.
 * 
 * Revision 1.2  93/11/24  12:05:00  neelin
 * Write output to stdout instead of stderr.
 * 
 * Revision 1.1  93/11/24  11:25:01  neelin
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

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <acr_nema.h>

#define N_NAME 31

struct table_element {
    struct table_element *next;
    unsigned short el_id;
    char name[N_NAME+1];
};

struct table_group {
    struct table_group *next;
    struct table_element *list;
    unsigned short grp_id;
};

struct table_group *_head;

char *get_name(unsigned short grp_id, unsigned short el_id)
{
    struct table_group *tg_ptr;
    struct table_element *te_ptr;

    for (tg_ptr = _head; tg_ptr; tg_ptr = tg_ptr->next) {
        if (tg_ptr->grp_id == grp_id) {
            for (te_ptr = tg_ptr->list; te_ptr; te_ptr = te_ptr->next) {
                if (te_ptr->el_id == el_id) {
                    return (te_ptr->name);
                }
            }
        }
    }
    return (NULL);
}

void parse_table(char *filename)
{
    FILE *fp;
    char line[1024];
    char name[1024];
    unsigned int el_id;         /* Must be int for sscanf */
    unsigned int grp_id;        /* Must be int for sscanf */
    char vr[1024];
    struct table_element *te_ptr;
    struct table_group *tg_ptr;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        return;
    }

    while (fgets(line, sizeof(line), fp)) {
        if (!isalnum(line[0])) {
            continue;           /* Ignore */
        }
        if (sscanf(line, "%s %x %x %s\n", name, &grp_id, &el_id, vr) != 4) {
            continue;
        }

        for (tg_ptr = _head; tg_ptr; tg_ptr = tg_ptr->next) {
            if (tg_ptr->grp_id == grp_id) {
                break;
            }
        }
        if (tg_ptr == NULL) {
            tg_ptr = malloc(sizeof(struct table_group));
            tg_ptr->next = _head;
            _head = tg_ptr;
            tg_ptr->list = NULL;
            tg_ptr->grp_id = (unsigned short) grp_id;
        }

        te_ptr = malloc(sizeof(struct table_element));
        te_ptr->el_id = (unsigned short) el_id;
        te_ptr->next = tg_ptr->list;
        tg_ptr->list = te_ptr;
        strncpy(te_ptr->name, name, N_NAME);
    }
    fclose(fp);
}


#define UNKNOWN_VR_ENCODING ((Acr_VR_encoding_type) -1)

int main(int argc, char *argv[])
{
   char *pname;
   char *file = NULL;
   char *maxidstr = NULL;
   int ignore_errors = FALSE;
   Acr_byte_order byte_order = ACR_UNKNOWN_ENDIAN;
   Acr_VR_encoding_type vr_encoding = UNKNOWN_VR_ENCODING;
   FILE *fp;
   Acr_File *afp;
   Acr_Group group_list;
   Acr_Status status;
   char *status_string;
   int maxid;
   char *ptr;
   int iarg, argcounter;
   char *arg;
   char *usage = "Usage: %s [-h] [-i] [-b] [-l] [-e] [-t <table>] [<file> [<max group>]]\n";

   /* Check arguments */
   pname = argv[0];
   argcounter = 0;
   for (iarg=1; iarg < argc; iarg++) {
      arg = argv[iarg];
      if ((arg[0] == '-') && (arg[1] != '\0')) {
         if (arg[2] != '\0') {
            (void) fprintf(stderr, "Unrecognized option %s\n", arg);
            exit(EXIT_FAILURE);
         }
         switch (arg[1]) {
         case 't':
             if (iarg < argc - 1) {
                 parse_table(argv[++iarg]);
                 _acr_name_proc = get_name;
                 break;
             }
             /* Fall through */
         case 'h':
            (void) fprintf(stderr, "Options:\n");
            (void) fprintf(stderr, "   -h:\tPrint this message\n");
            (void) fprintf(stderr, "   -t <table>:\tUse table to decode element names\n");
            (void) fprintf(stderr, "   -i:\tIgnore protocol errors\n");
            (void) fprintf(stderr, "   -b:\tAssume big-endian data\n");
            (void) fprintf(stderr, "   -l:\tAssume little-endian data\n");
            (void) fprintf(stderr, "   -e:\tAssume explicit VR encoding\n\n");
            (void) fprintf(stderr, usage, pname);
            exit(EXIT_FAILURE);
            break;
         case 'i':
            ignore_errors = TRUE;
            break;
         case 'l':
            byte_order = ACR_LITTLE_ENDIAN;
            break;
         case 'b':
            byte_order = ACR_BIG_ENDIAN;
            break;
         case 'e':
            vr_encoding = ACR_EXPLICIT_VR;
            break;
         default:
            (void) fprintf(stderr, "Unrecognized option %s\n", arg);
            exit(EXIT_FAILURE);
         }
      }
      else {
         switch (argcounter) {
         case 0:
            file = arg; break;
         case 1:
            maxidstr = arg; break;
         default:
            (void) fprintf(stderr, usage, pname);
            exit(EXIT_FAILURE);
         }
         argcounter++;
      }
   }

   /* Open input file */
   if ((file != NULL) && (strcmp(file, "-") != 0)) {
      fp = fopen(file, "r");
      if (fp == NULL) {
         (void) fprintf(stderr, "%s: Error opening file %s\n",
                        pname, file);
         exit(EXIT_FAILURE);
      }
   }
   else {
      fp = stdin;
   }

   /* Look for max group id */
   if (maxidstr != NULL) {
      maxid = strtol(maxidstr, &ptr, 0);
      if (ptr == maxidstr) {
         (void) fprintf(stderr, "%s: Error in max group id (%s)\n", 
                        pname, maxidstr);
         exit(EXIT_FAILURE);
      }
   }
   else {
      maxid = 0;
   }

   /* Connect to input stream */
   afp=acr_file_initialize(fp, 0, acr_stdio_read);
   acr_set_ignore_errors(afp, ignore_errors);
   (void) acr_test_dicom_file(afp);
   if (byte_order != ACR_UNKNOWN_ENDIAN) {
      acr_set_byte_order(afp, byte_order);
   }
   if (vr_encoding != UNKNOWN_VR_ENCODING) {
      acr_set_vr_encoding(afp, vr_encoding);
   }

   /* Read in group list */
   status = acr_input_group_list(afp, &group_list, maxid);

   /* Free the afp */
   acr_file_free(afp);

   /* Dump the values */
   acr_dump_group_list(stdout, group_list);

   /* Print status information */
   if ((status != ACR_END_OF_INPUT) && (status != ACR_OK)) {
      status_string = acr_status_string(status);
      (void) fprintf(stderr, "Finished with status '%s'\n", status_string);
   }

   exit(EXIT_SUCCESS);
}
