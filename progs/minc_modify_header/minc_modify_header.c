/* ----------------------------- MNI Header -----------------------------------
@NAME       : minc_modify_header
@INPUT      : argc, argv - command line arguments
@OUTPUT     : (none)
@RETURNS    : error status
@DESCRIPTION: Modifies the header of a minc file, in place if possible.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : March 31, 1995 (Peter Neelin)
@MODIFIED   : 
 * $Log: minc_modify_header.c,v $
 * Revision 6.6  2003-11-14 16:52:24  stever
 * More last-minute fixes.
 *
 * Revision 6.5  2001/04/17 18:40:16  neelin
 * Modifications to work with NetCDF 3.x
 * In particular, changed NC_LONG to NC_INT (and corresponding longs to ints).
 * Changed NC_UNSPECIFIED to NC_NAT.
 * A few fixes to the configure script.
 *
 * Revision 6.4  2000/12/13 16:19:37  neelin
 * Removed debugging statements
 *
 * Revision 6.3  2000/11/03 16:35:40  neelin
 * Modified -dinsert option to allow multiple double values to be
 * inserted in an attribute.
 *
 * Revision 6.2  2000/09/13 14:12:35  neelin
 * Added support for bzipped files (thanks to Steve Robbins).
 *
 * Revision 6.1  1999/10/19 14:45:17  neelin
 * Fixed Log subsitutions for CVS
 *
 * Revision 6.0  1997/09/12 13:24:07  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:25:06  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:01:35  neelin
 * Release of minc version 0.4
 *
 * Revision 3.1  1995/11/16  13:16:19  neelin
 * Added include of math.h to get declaration of strtod under SunOs
 *
 * Revision 3.0  1995/05/15  19:32:21  neelin
 * Release of minc version 0.3
 *
 * Revision 1.2  1995/04/04  19:10:56  neelin
 * Fixed handling of compressed files.
 *
 * Revision 1.1  1995/04/04  15:51:42  neelin
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

#ifndef lint
static char rcsid[]="$Header: /private-cvsroot/minc/progs/minc_modify_header/minc_modify_header.c,v 6.6 2003-11-14 16:52:24 stever Exp $";
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <minc.h>
#include <ParseArgv.h>
#include <minc_def.h>

/* Constants */
#define MINC_EXTENSION ".mnc"
#define BZIP_EXTENSION ".bz"
#define BZIP2_EXTENSION ".bz2"
#define GZIP_EXTENSION ".gz"
#define COMPRESS_EXTENSION ".Z"
#define PACK_EXTENSION ".z"
#define ZIP_EXTENSION ".zip"
#ifndef TRUE
#  define TRUE 1
#endif
#ifndef FALSE
#  define FALSE 0
#endif

/* Typedefs */
typedef enum {Insert_attribute, Delete_attribute} Attribute_Action;

/* Functions */
int get_attribute(char *dst, char *key, char *nextarg);

/* Argument variables */
int attribute_list_size = 0;
int attribute_list_alloc = 0;
struct {
   Attribute_Action action;
   char *variable;
   char *attribute;
   char *value;
   int num_doubles;
   double *double_values;
} *attribute_list = NULL;

/* Argument table */
ArgvInfo argTable[] = {
   {NULL, ARGV_HELP, NULL, NULL,
       "Options for specifying attribute values by name."},
   {"-sinsert", ARGV_FUNC, (char *) get_attribute, NULL,
       "Insert string attribute (<var>:<attr>=<value>)."},
   {"-dinsert", ARGV_FUNC, (char *) get_attribute, NULL,
       "Insert a double precision attribute (<var>:<attr>=<value>(,...))."},
   {"-delete", ARGV_FUNC, (char *) get_attribute, NULL,
       "Delete an attribute (<var>:<attr>)."},
   {NULL, ARGV_END, NULL, NULL, NULL}
};

/* Main program */

int main(int argc, char *argv[])
{
   char *pname;
   char *filename, *tempfile, *newfile;
   char string[1024];
   char *variable_name, *attribute_name;
   int created_tempfile;
   int done_redef;
   int iatt;
   int mincid, varid;
   int variable_exists, attribute_exists;
   nc_type attribute_type, new_type;
   int attribute_length, new_length;
   void *new_value;
   int total_length, alloc_length, ival;
   char *zeros;
   int old_ncopts;

   /* Parse the command line */
   pname=argv[0];
   if (ParseArgv(&argc, argv, argTable, 0) || (argc != 2)) {
      (void) fprintf(stderr, "\nUsage: %s [<options>] <file.mnc>\n", 
                     pname);
      (void) fprintf(stderr,   "       %s [-help]\n\n", pname);
      exit(EXIT_FAILURE);
   }
   filename = argv[1];

   /* Create temp file name. First try looking for minc extension, then
      a compression extension. Chop off the unwanted extension. */
   (void) strncpy(string, filename, sizeof(string)-1);
   tempfile = strstr(string, MINC_EXTENSION);
   if (tempfile != NULL) {
      tempfile += strlen(MINC_EXTENSION);
      if (*tempfile == '\0')
         tempfile = NULL;
   }
   else {
      tempfile = strstr(string, GZIP_EXTENSION);
      if (tempfile == NULL)
         tempfile = strstr(string, BZIP_EXTENSION);
      if (tempfile == NULL)
         tempfile = strstr(string, BZIP2_EXTENSION);
      if (tempfile == NULL)
         tempfile = strstr(string, COMPRESS_EXTENSION);
      if (tempfile == NULL)
         tempfile = strstr(string, PACK_EXTENSION);
      if (tempfile == NULL)
         tempfile = strstr(string, ZIP_EXTENSION);
   }
   if (tempfile != NULL) {
      *tempfile = '\0';
      tempfile = string;
   }

   /* If tempfile == NULL, then either we have a minc file or we don't know 
      how to edit the file in place. Check that it is a minc file. */
   if (tempfile == NULL) {
      newfile = miexpand_file(filename, tempfile, TRUE, &created_tempfile);
      if (created_tempfile) {
         if (newfile != NULL) {
            (void) remove(newfile);
            FREE(newfile);
         }
         (void) fprintf(stderr, "Cannot edit file \"%s\" in place.\n",
                        filename);
         exit(EXIT_FAILURE);
      }
   }

   /* Expand the file. */
   newfile = miexpand_file(filename, tempfile, FALSE, &created_tempfile);
   if (newfile == NULL) {
      (void) fprintf(stderr, "Error decompressing file \"%s\"\n",
                     filename);
      exit(EXIT_FAILURE);
   }

   /* If a new file was created, get rid of the old one */
   if (created_tempfile) {
      (void) remove(filename);
   }

   /* Open the file */
   mincid = miopen(newfile, NC_WRITE);

   /* Loop through attribute list, modifying values */
   done_redef = FALSE;
   ncopts = NC_VERBOSE;
   zeros = NULL;
   alloc_length = 0;
   for (iatt=0; iatt < attribute_list_size; iatt++) {

      /* Get variable and attribute name */
      variable_name = attribute_list[iatt].variable;
      attribute_name = attribute_list[iatt].attribute;

      /* Check for attribute existence */
      if (strlen(variable_name) == 0) {
         varid = NC_GLOBAL;
         variable_exists = TRUE;
      }
      else {
         old_ncopts = ncopts; ncopts = 0;
         varid = ncvarid(mincid, variable_name);
         ncopts = old_ncopts;
         variable_exists = (varid != MI_ERROR);
      }
      attribute_type = NC_CHAR;
      attribute_length = 0;
      if (variable_exists) {
         old_ncopts = ncopts; ncopts = 0;
         attribute_exists = 
            (ncattinq(mincid, varid, attribute_name,
                      &attribute_type, &attribute_length) != MI_ERROR);
         ncopts = old_ncopts;
      }
      else
         attribute_exists = FALSE;

      /* Are we inserting or deleting? */
      if (attribute_list[iatt].action == Insert_attribute) {
         if (attribute_list[iatt].value != NULL) {
            new_type = NC_CHAR;
            new_length = strlen(attribute_list[iatt].value)+1;
            new_value = (void *) attribute_list[iatt].value;
         }
         else {
            new_type = NC_DOUBLE;
            new_length = attribute_list[iatt].num_doubles;
            new_value = (void *) attribute_list[iatt].double_values;
         }
         total_length = attribute_length*nctypelen(attribute_type);
         if (!attribute_exists ||
             (total_length < new_length*nctypelen(new_type))) {
            if (! done_redef) {
               done_redef = TRUE;
               (void) ncredef(mincid);
            }
         }
         else if (!done_redef && attribute_exists && (total_length > 0)) {
            if (total_length > alloc_length) {
               if (zeros != NULL) FREE(zeros);
               zeros = MALLOC(total_length);
               alloc_length = total_length;
               for (ival=0; ival < alloc_length; ival++)
                  zeros[ival] = '\0';
            }
            (void) ncattput(mincid, varid, attribute_name, NC_CHAR,
                            total_length, zeros);
            (void) ncsync(mincid);
            
         }
         if (!variable_exists) {
            old_ncopts = ncopts; ncopts = 0;
            varid = micreate_group_variable(mincid, variable_name);
            ncopts = old_ncopts;
            if (varid == MI_ERROR) {
               varid = ncvardef(mincid, variable_name, NC_INT,
                                0, NULL);
            }
            variable_exists = (varid != MI_ERROR);
         }
         if (variable_exists) {
            (void) ncattput(mincid, varid, attribute_name,
                            new_type, new_length, new_value);
         }

      }    /* Insert_attribute */

      else if (attribute_list[iatt].action == Delete_attribute) {

         if (attribute_exists) {
            if (! done_redef) {
               done_redef = TRUE;
               (void) ncredef(mincid);
            }
            (void) ncattdel(mincid, varid, attribute_name);
         }
               

      }    /* Delete_attribute */

      else {
         (void) fprintf(stderr, "Program error: unknown action %d\n",
                        (int) attribute_list[iatt].action);
         exit(EXIT_FAILURE);
      }
      

   }
   ncopts = NC_VERBOSE | NC_FATAL;

   /* Close the file */
   (void) miclose(mincid);

   /* Free stuff */
   FREE(newfile);
   if (zeros != NULL) FREE(zeros);

   exit(EXIT_SUCCESS);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_attribute
@INPUT      : dst - client data passed by ParseArgv
              key - matching key in argv
              nextarg - argument following key in argv
@OUTPUT     : (none)
@RETURNS    : TRUE because nextarg is used.
@DESCRIPTION: Gets attributes from command line. Syntax for argument is
              "<var>:<att>=<value>". Numeric values are converted to
              double precision.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : May 3, 1994 (Peter Neelin)
@MODIFIED   : March 31, 1995 (P.N)
                 - grabbed from rawtominc
---------------------------------------------------------------------------- */
int get_attribute(char *dst, char *key, char *nextarg)
     /* ARGSUSED */
{
   int need_double;
   Attribute_Action action;
   char *format;
   char *variable;
   char *attribute;
   char *value;
   char *end, *cur;
   double *dvalues;
   int num_doubles, ivalue;

   /* Check the key */
   if (strcmp(key, "-sinsert") == 0) {
      action = Insert_attribute;
      need_double = FALSE;
      format = "<var>:<attr>=<val>";
   }
   else if (strcmp(key, "-dinsert") == 0) {
      action = Insert_attribute;
      need_double = TRUE;
      format = "<var>:<attr>=<val>";
   }
   else if (strcmp(key, "-delete") == 0) {
      action = Delete_attribute;
      need_double = FALSE;
      format = "<var>:<attr>";
   }

   /* Check for a following argument */
   if (nextarg == NULL) {
      (void) fprintf(stderr, 
                     "\"%s\" option requires an additional argument\n",
                     key);
      exit(EXIT_FAILURE);
   }

   /* Get the variable name */
   variable = nextarg;
   attribute = strchr(variable, ':');
   if (attribute == NULL) {
      (void) fprintf(stderr, 
                     "%s option requires argument %s\n", 
                     key, format);
      exit(EXIT_FAILURE);
   }
   *attribute = '\0';
   attribute++;

   /* Get the value */
   value = NULL;
   num_doubles = 0;
   dvalues = NULL;
   if (action == Insert_attribute) {
      value = strchr(attribute, '=');
      if (value == NULL) {
         (void) fprintf(stderr, 
                        "%s option requires argument <var>:<attr>=<val>\n", 
                        key);
         exit(EXIT_FAILURE);
      }
      *value = '\0';
      value++;

      /* Convert to double precision */
      if (need_double) {

         /* Count the commas */
         num_doubles = 1;
         for (cur=value; *cur != '\0'; cur++) {
            if (*cur == ',') num_doubles++;
         }

         /* Allocate a list */
         dvalues = MALLOC(sizeof(*dvalues) * num_doubles);

         /* Loop over values */
         cur = value;
         for (ivalue=0; ivalue < num_doubles; ivalue++) {

            /* Get the value */
            dvalues[ivalue] = strtod(cur, &end);
            if (end == cur) {
               (void) fprintf(stderr, 
                              "\"%s\" option requires a numeric argument\n",
                              key);
               exit(EXIT_FAILURE);
            }
            cur = end;

            /* Skip whitespace and the comma */
            while (isspace((int) *cur)) {cur++;}
            if ((*cur != '\0') && (*cur != ',')) {
               (void) fprintf(stderr, 
                              "\"%s\" option requires a numeric argument\n",
                              key);
               exit(EXIT_FAILURE);
            }
            else if (*cur == ',') {
               cur++;
            }
            
         }       /* End of loop over double values */

         /* Clear the value string */
         value = NULL;

      }        /* Endif need_double */
   }        /* Endif insert */

   /* Save the information */
   attribute_list_size++;
   if (attribute_list_size > attribute_list_alloc) {
      attribute_list_alloc += 10;
      if (attribute_list == NULL) {
         attribute_list = 
            MALLOC(attribute_list_alloc * sizeof(*attribute_list));
      }
      else {
         attribute_list = 
            REALLOC(attribute_list, 
                    attribute_list_alloc * sizeof(*attribute_list));
      }
   }
   attribute_list[attribute_list_size-1].action = action;
   attribute_list[attribute_list_size-1].variable = variable;
   attribute_list[attribute_list_size-1].attribute = attribute;
   attribute_list[attribute_list_size-1].value = value;
   attribute_list[attribute_list_size-1].num_doubles = num_doubles;
   attribute_list[attribute_list_size-1].double_values = dvalues;

   return TRUE;

}

