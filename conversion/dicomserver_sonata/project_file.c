/* ----------------------------- MNI Header -----------------------------------
@NAME       : project_file.c
@DESCRIPTION: Code to do manipulate the project files (files containing
              info on what to do with files for each project).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 28, 1997 (Peter Neelin)
@MODIFIED   : 
 * $Log: project_file.c,v $
 * Revision 1.1  2003-08-15 19:52:55  leili
 * Initial revision
 *
 * Revision 1.1.1.1  2000/11/30 02:13:15  rhoge
 * imported sources to CVS repository on amoeba
 *
 * Revision 6.1  1999/10/29 17:51:56  neelin
 * Fixed Log keyword
 *
 * Revision 6.0  1997/09/12 13:24:27  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:25:26  neelin
 * Release of minc version 0.5
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

#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <dicomserver.h>

/* Function prototypes */
/************************************************/
/* Commented out by rhoge, put back in by leili */
 int gethostname (char *name, size_t  namelen); 
/************************************************/
/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_project_file
@INPUT      : project_name - name to use for project file
@OUTPUT     : file_prefix - string used as prefix for output files 
                 (can be NULL)
              output_uid - uid for created files (can be NULL). Set to 
                 INT_MIN if file not found.
              output_gid - gid for created files (can be NULL). Set to
                 INT_MIN if file not found.
              command_line - command to execute on new file (can be NULL)
              maxlen_command - maximum length for command_line
@RETURNS    : TRUE if an error occurs, FALSE otherwise.
@DESCRIPTION: Routine to read in default information for a given project.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 14, 1995 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int read_project_file(char *project_name, 
                             char *file_prefix, 
                             int *output_uid, int *output_gid,
                             char *command_line, int maxlen_command)
{
   char project_string[256];
   char output_default_file[256];
   char temp_file_prefix[256];
   int temp_uid, temp_gid;
   char temp_command_line[4];
   int ichar, ochar;
   int length, index;
   FILE *fp;
   char string[512];
   int project_name_given;

   /* Check that the user actually wants return values */
   if (file_prefix == NULL) file_prefix = temp_file_prefix;
   if (output_uid == NULL) output_uid = &temp_uid;
   if (output_gid == NULL) output_gid = &temp_gid;
   if ((command_line == NULL) || (maxlen_command <= 0)) {
      command_line = temp_command_line;
      maxlen_command = sizeof(temp_command_line);
   }

   /* Set some default values */
   file_prefix[0] = '\0';
   command_line[0] = '\0';
   *output_uid = *output_gid = INT_MIN;

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
   if (fgets(string, (int) sizeof(string), fp) == NULL) {
      return TRUE;
   }
   if (sscanf(string, "%s %d %d", file_prefix, output_uid, output_gid) != 3) {
      return TRUE;
   }
   (void) fgets(command_line, maxlen_command, fp);
   index = strlen(command_line) - 1;
   if ((index >= 0) && (command_line[index] == '\n'))
      command_line[index] = '\0';
   (void) fclose(fp);

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
            if (!read_project_file(name, NULL, NULL, NULL, NULL, 0)) {
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
