/* ----------------------------- MNI Header -----------------------------------
@NAME       : project_file.c
@DESCRIPTION: Code to do manipulate the project files (files containing
              info on what to do with files for each project).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 14, 1995 (Peter Neelin)
@MODIFIED   : 
 * $Log: project_file.c,v $
 * Revision 6.3  2000-01-31 13:57:38  neelin
 * Added keyword to project file to allow definition of the local AEtitle.
 * A simple syntax allows insertion of the host name into the AEtitle.
 *
 * Revision 6.2  1999/10/29 17:52:04  neelin
 * Fixed Log keyword
 *
 * Revision 6.1  1997/09/12 23:13:28  neelin
 * Added ability to convert gyrocom images to dicom images.
 *
 * Revision 6.0  1997/09/12  13:23:50  neelin
 * Release of minc version 0.6
 *
 * Revision 5.1  1997/09/11  13:09:40  neelin
 * Added more complicated syntax for project files so that different things
 * can be done to the data. The old syntax is still supported.
 *
 * Revision 5.0  1997/08/21  13:24:50  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:01:07  neelin
 * Release of minc version 0.4
 *
 * Revision 3.0  1995/05/15  19:31:44  neelin
 * Release of minc version 0.3
 *
 * Revision 1.1  1995/02/14  18:12:26  neelin
 * Initial revision
 *
@COPYRIGHT  :
              Copyright 1995 Peter Neelin, McConnell Brain Imaging Centre, 
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
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>
#include <gcomserver.h>

/* Macros */
#define STREQ(s1, s2) (strcmp(s1, s2) == 0)
#define STRCOPY(s1, s2, maxlen) \
{ (void) strncpy(s1, s2, (maxlen)-1); s1[(maxlen)-1] = '\0'; }

/* Function prototypes */
int gethostname (char *name, int namelen);

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_project_file
@INPUT      : project_name - name to use for project file
@OUTPUT     : project_info - information about the project returned in
                 a user-allocated structure. If NULL, then no data is
                 returned.
@RETURNS    : TRUE if an error occurs, FALSE otherwise.
@DESCRIPTION: Routine to read in information for a given project.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 14, 1995 (Peter Neelin)
@MODIFIED   : September 9, 1997 (P.N.)
---------------------------------------------------------------------------- */
public int read_project_file(char *project_name, 
                             Project_File_Info *project_info)
{
   Project_File_Info temp_project_info;
   char project_string[256];
   char output_default_file[256];
   int ichar, ochar;
   int length;
   FILE *fp;
   char string[512];
   int project_name_given;
   char *keyword, *value, *ptr, *error;
   int oldformat;
   int iline;

   /* Check that the user actually wants return values */
   if (project_info == NULL) {
      project_info = &temp_project_info;
   }

   /* Set some default values */
   project_info->type = PROJECT_DIRECTORY;
   project_info->info.directory.file_prefix[0] = '\0';
   project_info->info.directory.command_line[0] = '\0';
   project_info->info.directory.output_uid = INT_MIN;
   project_info->info.directory.output_gid = INT_MIN;

   /* Copy the project name, removing spaces */
   if (project_name != NULL)
      length = strlen(project_name);
   else
      length = 0;
   for (ichar=0, ochar=0; 
        (ichar < length) && (ochar < sizeof(project_string)-1);
        ichar++) {
      if (isprint((int) project_name[ichar]) && 
          !isspace((int) project_name[ichar])) {
         project_string[ochar] = (char) toupper((int) project_name[ichar]);
         ochar++;
      }
   }
   project_string[ochar] = '\0';

   /* Get the host name if there is no project string */
   project_name_given = (strlen(project_string) > (size_t) 0);
   if (!project_name_given)
      (void) gethostname(project_string, sizeof(project_string) - 1);
   (void) sprintf(output_default_file, "%s/%s%s", 
                  OUTPUT_DEFAULT_FILE_DIR, OUTPUT_DEFAULT_FILE_PREFIX,
                  project_string);

   /* Open and read the defaults file - if it isn't there then return TRUE
      if the caller gave a project name */
   if ((fp=fopen(output_default_file, "r")) == NULL) {
      return project_name_given;
   }

   /* Loop over lines of the file */
   iline = 0;
   project_info->type = PROJECT_UNKNOWN;
   while (fgets(string, (int) sizeof(string), fp) != NULL) {

      /* Remove the newline, or read in the rest of the line 
         if it is too long */
      if (string[strlen(string) - 1] == '\n') {
         string[strlen(string) - 1] = '\0';
      }
      else {
         while ((getc(fp) != (int) '\n') && !feof(fp)) {}
      }

      /* Ignore comments and blank lines (don't even count them as lines) */
      if (string[0] == '#') continue;
      for (ptr=string; (*ptr != '\0') && isspace((int) *ptr); ptr++) {}
      if (*ptr == '\0') continue;

      /* If this is the first line, figure out whether we have an old
         or new format file (look for the "=") */
      ptr = strchr(string, (int) '=');
      if (iline == 0) {
         oldformat = (ptr == NULL);
      }

      /* Read old format file */
      if (oldformat) {
         switch (iline) {
         case 0:
            project_info->type = PROJECT_DIRECTORY;
            if (sscanf(string, "%s %d %d", 
                       project_info->info.directory.file_prefix, 
                       &project_info->info.directory.output_uid, 
                       &project_info->info.directory.output_gid) != 3) {
               (void) fclose(fp);
               return TRUE;
            }
            break;
         case 1:
            (void) strcpy(project_info->info.directory.command_line, string);
            break;
         default:
            (void) fprintf(stderr, 
               "Old format project file (%s) contains extra lines\n",
                           project_string);
            (void) fclose(fp);
            return TRUE;
         }
      }

      /* Read new format file */
      else {                /* New format */

         /* Check for missing '=' */
         if (ptr == NULL) {
            (void) fprintf(stderr, "Project file error (%s): syntax error\n",
                           project_string);
            (void) fclose(fp);
            return TRUE;
         }

         /* Get keyword and value (remove leading and trailing whitespace) */
         for (keyword=string; isspace((int) *keyword); keyword++) {}
         if (*keyword == '=') keyword--;
         for (value = ptr+1; isspace((int) *value); value++) {}
         for (ptr--; (ptr > keyword) && isspace((int) *ptr); ptr--) {}
         ptr++;
         *ptr = '\0';
         for (ptr=&value[strlen(value)-1]; 
              (ptr > value) && isspace((int) *ptr); ptr--) {}
         ptr++;
         *ptr = '\0';

         /* Set up error string */
         error = NULL;

         /* Look for the project type on the first line */
         if (iline == 0) {
            if (STREQ(keyword, "Type")) {
               if (STREQ(value, "Directory")) {
                  project_info->type = PROJECT_DIRECTORY;
               }
               else if (STREQ(value, "Dicom")) {
                  project_info->type = PROJECT_DICOM;
                  project_info->info.dicom.hostname[0] = '\0';
                  project_info->info.dicom.port[0] = '\0';
                  project_info->info.dicom.AEtitle[0] = '\0';
                  project_info->info.dicom.LocalAEtitle[0] = '\0';
                  project_info->info.dicom.afpin = NULL;
                  project_info->info.dicom.afpout = NULL;
               }
               else {
                  error = "Unknown project type";
               }
            }
            else {
               error = "Type must be given on first line";
            }
         }

         /* Get other keywords */
         else {                           /* Subsequent lines */

            /* Check that project type is defined */
            if (project_info->type == PROJECT_UNKNOWN) {
               error = "Type must be given on first line";
            }

            /* Keywords for directory type */
            else if (project_info->type == PROJECT_DIRECTORY) {
               if (STREQ("Prefix", keyword)) {
                  STRCOPY(project_info->info.directory.file_prefix,
                          value, LONG_LINE);
               }
               else if (STREQ("Uid", keyword)) {
                  project_info->info.directory.output_uid = atoi(value);
               }
               else if (STREQ("Gid", keyword)) {
                  project_info->info.directory.output_gid = atoi(value);
               }
               else if (STREQ("Command", keyword)) {
                  STRCOPY(project_info->info.directory.command_line,
                          value, LONG_LINE);
               }
               else {
                  error = "Unrecognized keyword for given type";
               }
            }

            /* Keywords for dicom type */
            else if (project_info->type == PROJECT_DICOM) {
               if (STREQ("Host", keyword)) {
                  STRCOPY(project_info->info.dicom.hostname,
                          value, SHORT_LINE);
               }
               else if (STREQ("Port", keyword)) {
                  STRCOPY(project_info->info.dicom.port,
                          value, SHORT_LINE);
               }
               else if (STREQ("AEtitle", keyword)) {
                  STRCOPY(project_info->info.dicom.AEtitle,
                          value, SHORT_LINE);
               }
               else if (STREQ("LocalAEtitle", keyword)) {
                  STRCOPY(project_info->info.dicom.LocalAEtitle,
                          value, SHORT_LINE);
               }
               else {
                  error = "Unrecognized keyword for given type";
               }
            }

         }          /* Subsequent lines */

         /* Check for error */
         if (error != NULL) {
            (void) fprintf(stderr, "Project file error (%s), keyword %s: %s\n",
                           project_string, keyword, error);
            (void) fclose(fp);
            return TRUE;
         }

      }             /* New format */

      /* Increment the line counter */
      iline++;
   }

   /* Close the file */
   (void) fclose(fp);

   /* Check that something was found */
   if (project_info->type == PROJECT_UNKNOWN) {
      return TRUE;
   }

   /* Check that minimal set of keywords was found */
   if (project_info->type == PROJECT_DICOM) {
      if ((project_info->info.dicom.hostname[0] == '\0') ||
          (project_info->info.dicom.port[0] == '\0') ||
          (project_info->info.dicom.AEtitle[0] == '\0')) {
         (void) fprintf(stderr, "Project file (%s) missing options\n",
                        project_string);
         return TRUE;
      }
   }

   return FALSE;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_project_option_string
@INPUT      : (none)
@OUTPUT     : project_option_string - string containing list of options
                 for project name
              maxlen_project_option - maximum length for the string (including
                 '\0' at end)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to get a list of possibilities for the project name
              (looking for appropriately named files).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 14, 1995 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void get_project_option_string(char *project_option_string,
                                      int maxlen_project_option)
{
   DIR *dirp;
   struct dirent *dp;
   int length;
   char *name, *filler;
   int compare_length;

   /* Set up the string */
   if (maxlen_project_option > 0) {
      project_option_string[0] = '\0';
      length = 1;
   }

   /* Open directory */
   if ((dirp = opendir(OUTPUT_DEFAULT_FILE_DIR)) == NULL)
      return;

   /* Loop through directory entries */
   compare_length = strlen(OUTPUT_DEFAULT_FILE_PREFIX);
   while ((dp = readdir(dirp)) != NULL) {

      /* Check for an entry with the right prefix */
      if (strncmp(OUTPUT_DEFAULT_FILE_PREFIX, 
                  dp->d_name, compare_length) == 0) {

         /* Check for an uppercase letter */
         if ((strlen(dp->d_name) > (size_t) compare_length) &&
             (isupper(dp->d_name[compare_length]))) {
            name = &dp->d_name[compare_length];

            /* Check that we can read the project file */
            if (!read_project_file(name, NULL)) {
               if (length > 1)
                  filler = ", ";
               else
                  filler = "";
               if ((strlen(name) + length + strlen(filler)) 
                   < (size_t) maxlen_project_option) {
                  (void) strcat(strcat(project_option_string, filler), name);
                  length += strlen(filler) + strlen(name);
               }
            }        /* We can read the project file */
            
         }        /* Found uppercase letter */
         
      }        /* Found file matching prefix */

   }        /* Loop over files */
}
