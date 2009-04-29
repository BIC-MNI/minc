/* ----------------------------- MNI Header -----------------------------------
@NAME       : mincinfo
@INPUT      : argc, argv - command line arguments
@OUTPUT     : (none)
@RETURNS    : status
@DESCRIPTION: Program to dump minc header information to standard output.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : May 19, 1993 (Peter Neelin)
@MODIFIED   : 
 * $Log: mincinfo.c,v $
 * Revision 6.13  2009-04-29 13:58:46  rotor
 *  * fixed a stack smash in mincinfo.c
 *
 * Revision 6.12  2008/01/17 02:33:02  rotor
 *  * removed all rcsids
 *  * removed a bunch of ^L's that somehow crept in
 *  * removed old (and outdated) BUGS file
 *
 * Revision 6.11  2008/01/13 04:30:28  stever
 * Add braces around static initializers.
 *
 * Revision 6.10  2008/01/12 19:08:15  stever
 * Add __attribute__ ((unused)) to all rcsid variables.
 *
 * Revision 6.9  2007/12/11 12:43:01  rotor
 *  * added static to all global variables in main programs to avoid linking
 *       problems with libraries (compress in mincconvert and libz for example)
 *
 * Revision 6.8  2006/07/28 18:20:53  baghdadi
 * *** empty log message ***
 *
 * Revision 6.7  2006/07/28 17:51:01  baghdadi
 * Added option to print version of file
 * must use -minc_version -image_info
 *
 * Revision 6.6  2005/09/14 04:31:17  bert
 * include config.h
 *
 * Revision 6.5  2004/11/01 22:38:38  bert
 * Eliminate all references to minc_def.h
 *
 * Revision 6.4  2001/10/31 19:40:21  neelin
 * Fixed bug in printing of sign for default output - this was introduced
 * in the change to miget_datatype.
 *
 * Revision 6.3  2001/08/16 16:41:35  neelin
 * Added library functions to handle reading of datatype, sign and valid range,
 * plus writing of valid range and setting of default ranges. These functions
 * properly handle differences between valid_range type and image type. Such
 * difference can cause valid data to appear as invalid when double to float
 * conversion causes rounding in the wrong direction (out of range).
 * Modified voxel_loop, volume_io and programs to use these functions.
 *
 * Revision 6.2  2000/04/25 18:12:05  neelin
 * Added modified version of patch from Steve Robbins to allow use on
 * multiple input files.
 *
 * Revision 6.1  1999/10/19 14:45:24  neelin
 * Fixed Log subsitutions for CVS
 *
 * Revision 6.0  1997/09/12 13:23:35  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:24:36  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:00:38  neelin
 * Release of minc version 0.4
 *
 * Revision 3.1  1995/10/04  19:05:25  neelin
 * Fixed default_min for signed long.
 *
 * Revision 3.0  1995/05/15  19:31:31  neelin
 * Release of minc version 0.3
 *
 * Revision 2.3  1995/02/08  19:31:47  neelin
 * Moved ARGSUSED statements for irix 5 lint.
 *
 * Revision 2.2  1995/02/01  15:29:31  neelin
 * Fixed call of miexpand_file.
 *
 * Revision 2.1  95/01/23  13:47:46  neelin
 * Changed ncopen, ncclose to miopen, miclose. Added miexpand_file to get
 * header only when appropriate.
 * 
 * Revision 2.0  94/09/28  10:34:04  neelin
 * Release of minc version 0.2
 * 
 * Revision 1.10  94/09/28  10:34:00  neelin
 * Pre-release
 * 
 * Revision 1.9  93/08/11  15:45:53  neelin
 * Functions called by ParseArgv must check that nextArg is not NULL.
 * 
 * Revision 1.8  93/08/11  15:22:19  neelin
 * Added RCS logging in source.
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <minc.h>
#include <ParseArgv.h>

/* Constants */
#ifndef TRUE
#  define TRUE 1
#  define FALSE 0
#endif
#define MAX_NUM_OPTIONS 100

/* Name of program, for error reporting */
static char* exec_name;

static char *type_names[] = {
   NULL, "byte", "char", "short", "long", "float", "double"
};

/* Types */
typedef enum  {
   ENDLIST, IMAGE_INFO, DIMNAMES, VARNAMES,
   DIMLENGTH, VARTYPE, VARDIMS, VARATTS, VARVALUES,
   ATTTYPE, ATTVALUE, MINC_VERSION
} Option_code;
typedef struct {
   Option_code code;
   char *value;
} Option_type;

/* Macros */
#define REPORT_ERROR \
   {int s;if ((s=report_error())!=EXIT_SUCCESS) return s; goto error_label;}
#define CHK_ERR(code) if ((code) == MI_ERROR) {REPORT_ERROR}
#define RTN_ERR(code) if ((code) == MI_ERROR) {return MI_ERROR;}

/* Function prototypes */
static int process_file( char* filename, int header_only );
static int get_option(char *dst, char *key, char *nextarg);
static int report_error(void);
static int get_attname(int mincid, char *string, int *varid, char *name);
static int print_image_info(char *filename, int mincid);
/* Variables used for argument parsing */
static char *error_string = NULL;
static Option_type option_list[MAX_NUM_OPTIONS] = { { ENDLIST, "" } };
static int length_option_list = 0;

/* Argument table */
ArgvInfo argTable[] = {
   {"-image_info", ARGV_FUNC, (char *) get_option, (char *) IMAGE_INFO,
       "Print out the default information about the images."},
   {"-dimnames", ARGV_FUNC, (char *) get_option, (char *) DIMNAMES,
       "Print the names of the dimensions in the file."},
   {"-varnames", ARGV_FUNC, (char *) get_option, (char *) VARNAMES,
       "Print the names of the variables in the file."},
   {"-dimlength", ARGV_FUNC, (char *) get_option, (char *) DIMLENGTH,
       "Print the length of the specified dimension."},
   {"-vartype", ARGV_FUNC, (char *) get_option, (char *) VARTYPE,
       "Print the type of the specified variable."},
   {"-vardims", ARGV_FUNC, (char *) get_option, (char *) VARDIMS,
       "Print the dimension names for the specified variable."},
   {"-varatts", ARGV_FUNC, (char *) get_option, (char *) VARATTS,
       "Print the attribute names for the specified variable."},
   {"-varvalues", ARGV_FUNC, (char *) get_option, (char *) VARVALUES,
       "Print the values for the specified variable."},
   {"-atttype", ARGV_FUNC, (char *) get_option, (char *) ATTTYPE,
       "Print the type of the specified attribute (variable:attribute)."},
   {"-attvalue", ARGV_FUNC, (char *) get_option, (char *) ATTVALUE,
       "Print the value(s) of the specified attribute (variable:attribute)."},
   {"-error_string", ARGV_STRING, (char *) 0, (char *) &error_string,
       "Error to print on stdout (default = exit with error status)."},
   {"-minc_version", ARGV_FUNC, (char *) get_option, (char *) MINC_VERSION,
    "Print the minc file version, netcdf or hdf5."},
   {NULL, ARGV_END, NULL, NULL, NULL}
};

/* Main program */

int main(int argc, char *argv[])
{
   int ioption, ifile, header_only;
   int ret_value = EXIT_SUCCESS;

   exec_name = argv[0];

   /* Check arguments */
   if (ParseArgv(&argc, argv, argTable, 0) || (argc < 2)) {
      (void) fprintf(stderr, 
                     "\nUsage: %s [<options>] <mincfile> [<mincfile> ...]\n", 
                     exec_name);
      (void) fprintf(stderr,
                       "       %s -help\n\n", exec_name);
      exit(EXIT_FAILURE);
   }

   /* Check for no print options */
   if (option_list[0].code == ENDLIST) {
      option_list[0].code = IMAGE_INFO;
      option_list[0].value = NULL;
      length_option_list = 1;
   }

   /* Set error handling */
   if (error_string == NULL)
      ncopts = NC_VERBOSE | NC_FATAL;
   else
      ncopts = 0;

   /* Loop through print options, checking whether we need variable data */
   header_only = TRUE;
   for (ioption=0; ioption < length_option_list; ioption++) {
      if (option_list[ioption].code == VARVALUES)
         header_only = FALSE;
   }

   for (ifile=1; ifile < argc; ++ifile) {
      if (process_file( argv[ifile], header_only ) != EXIT_SUCCESS)
         ret_value = EXIT_FAILURE;
      if (argc > 2)
         printf("\n\n");
   }
   return ret_value;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : print_image_version
@INPUT      : (boolean)
@OUTPUT     : (none)
@RETURNS    : Exit status.
@DESCRIPTION: Prints out image netcdf or hdf5
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Apr 6, 2005 (Leila Baghdadi)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static int print_image_version(int Is_MINC2_File)
{
   if (Is_MINC2_File) {
     (void) printf("Version: 2 (HDF5)\n");
   }
   else {
     (void) printf("Version: 1 (netCDF)\n");
     
   }
    return EXIT_SUCCESS;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : process_file
@INPUT      : filename 
              header_only - TRUE if only header needs to be expanded
@OUTPUT     : (none)
@RETURNS    : Exit value for program
@DESCRIPTION: Runs mincinfo on one file
@METHOD     : (Adapted from old main of mincinfo.)
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 25, 2000 (Steve Robbins)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static int process_file( char* filename, int header_only )
{
   int mincid, varid, dimid;
   int ndims, dims[MAX_VAR_DIMS];
   nc_type datatype;
   long length, var_length, row_length;
   long start[MAX_VAR_DIMS], count[MAX_VAR_DIMS];
   int att_length;
   int idim, iatt, natts, option;
   int nvars, ivar, ival;
   char *string;
   char name[MAX_NC_NAME];
   char *cdata;
   double *ddata;
   int created_tempfile;
   char *tempfile;
   int Is_MINC2_File=0;

   /* Expand file */
   tempfile = miexpand_file(filename, NULL, header_only, &created_tempfile);
   if (tempfile == NULL) {
      (void) fprintf(stderr, "%s: Error expanding file \"%s\"\n",
                     exec_name, filename);
      return EXIT_FAILURE;
   }

   /* Open the file */
   mincid = miopen(tempfile, NC_NOWRITE);
   if (created_tempfile) {
      (void) remove(tempfile);
   }
   if (mincid == MI_ERROR) {
      (void) fprintf(stderr, "%s: Error opening file \"%s\"\n",
                     exec_name, tempfile);
      return EXIT_FAILURE;
   }

   /* check whether the file is Version 2 */
#ifdef MINC2
   if (MI2_ISH5OBJ(mincid)) {
     Is_MINC2_File = 1;
   }
#endif

   /* Loop through print options */
   for (option=0; option < length_option_list; option++) {
      string = option_list[option].value;
      switch (option_list[option].code) {
      case IMAGE_INFO:
         CHK_ERR(print_image_info(filename, mincid));
         break;
      case DIMNAMES:
         CHK_ERR(ncinquire(mincid, &ndims, NULL, NULL, NULL));
         for (idim=0; idim<ndims; idim++) {
            CHK_ERR(ncdiminq(mincid, idim, name, NULL));
            (void) printf("%s ", name);
         }
         (void) printf("\n");
         break;
      case VARNAMES:
         CHK_ERR(ncinquire(mincid, NULL, &nvars, NULL, NULL));
         for (ivar=0; ivar<nvars; ivar++) {
            CHK_ERR(ncvarinq(mincid, ivar, name, NULL, NULL, NULL, NULL));
            (void) printf("%s ", name);
         }
         (void) printf("\n");
         break;
      case DIMLENGTH:
         CHK_ERR(dimid = ncdimid(mincid, string));
         CHK_ERR(ncdiminq(mincid, dimid, NULL, &length));
         (void) printf("%d\n", (int) length);
         break;
      case VARTYPE:
         CHK_ERR(varid = ncvarid(mincid, string));
         CHK_ERR(ncvarinq(mincid, varid, NULL, &datatype, NULL, NULL, NULL));
         (void) printf("%s\n", type_names[datatype]);
         break;
      case VARDIMS:
         CHK_ERR(varid = ncvarid(mincid, string));
         CHK_ERR(ncvarinq(mincid, varid, NULL, NULL, &ndims, dims, NULL));
         for (idim=0; idim<ndims; idim++) {
            CHK_ERR(ncdiminq(mincid, dims[idim], name, NULL));
            (void) printf("%s ", name);
         }
         (void) printf("\n");
         break;
      case VARATTS:
         if (*string=='\0') {
            varid = NC_GLOBAL;
            CHK_ERR(ncinquire(mincid, NULL, NULL, &natts, NULL));
         }
         else {
            CHK_ERR(varid = ncvarid(mincid, string));
            CHK_ERR(ncvarinq(mincid, varid, NULL, NULL, NULL, NULL, &natts));
         }
         for (iatt=0; iatt<natts; iatt++) {
            CHK_ERR(ncattname(mincid, varid, iatt, name));
            (void) printf("%s ", name);
         }
         (void) printf("\n");
         break;
      case VARVALUES:
         CHK_ERR(varid = ncvarid(mincid, string));
         CHK_ERR(ncvarinq(mincid, varid, NULL, &datatype, &ndims, dims, NULL));
         var_length = 1;
         for (idim=0; idim<ndims; idim++) {
            CHK_ERR(ncdiminq(mincid, dims[idim], NULL, &length));
            start[idim] = 0;
            count[idim] = length;
            var_length *= length;
            if (idim==ndims-1)
               row_length = length;
         }
         if (datatype==NC_CHAR) {
            cdata = malloc(var_length*sizeof(char));
            CHK_ERR(ncvarget(mincid, varid, start, count, cdata));
            for (ival=0; ival<var_length; ival++) {
               (void) putchar((int) cdata[ival]);
               if (((ival+1) % row_length) == 0)
                  (void) putchar((int)'\n');
            }
            free(cdata);
         }
         else {
            ddata = malloc(var_length*sizeof(double));
            CHK_ERR(mivarget(mincid, varid, start, count, 
                             NC_DOUBLE, NULL, ddata));
            for (ival=0; ival<var_length; ival++) {
               (void) printf("%.20g\n", ddata[ival]);
            }
            free(ddata);
         }
         break;
      case ATTTYPE:
         CHK_ERR(get_attname(mincid, string, &varid, name));
         CHK_ERR(ncattinq(mincid, varid, name, &datatype, NULL));
         (void) printf("%s\n", type_names[datatype]);
         break;
      case ATTVALUE:
         CHK_ERR(get_attname(mincid, string, &varid, name));
         CHK_ERR(ncattinq(mincid, varid, name, &datatype, &att_length));
         if (datatype == NC_CHAR) {
            cdata = malloc((att_length+1)*sizeof(char));
            if (miattgetstr(mincid, varid, name, att_length+1, cdata)==NULL)
               {REPORT_ERROR}
            (void) printf("%s\n", cdata);
            free(cdata);
         }
         else {
            ddata = malloc(att_length * sizeof(double));
            CHK_ERR(miattget(mincid, varid, name, NC_DOUBLE, att_length,
                             ddata, NULL));
            for (iatt=0; iatt<att_length; iatt++) {
               (void) printf("%.20g ", ddata[iatt]);
            }
            (void) printf("\n");
            free(ddata);
         }
         break;
      case MINC_VERSION:
	CHK_ERR(print_image_version(Is_MINC2_File));
	break;
      default:
         (void) fprintf(stderr, "%s: Program bug!\n", exec_name);
         return EXIT_FAILURE;
      }
   error_label: ;
   }

   /* Close the file */
   (void) miclose(mincid);

   return EXIT_SUCCESS;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_option
@INPUT      : dst - client data passed by ParseArgv
              key - matching key in argv
              nextarg - argument following key in argv
@OUTPUT     : (none)
@RETURNS    : TRUE if nextarg is used, FALSE otherwise.
@DESCRIPTION: Gets command line options for information to print.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : May 19, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static int get_option(char *dst, char *key, char *nextarg)
     /* ARGSUSED */
{
   Option_code code;
   int return_value;

   /* Check number of options */
   if (length_option_list >= MAX_NUM_OPTIONS-1) {
      (void) fprintf(stderr, "Too many options - maximum is %d.\n", 
                     MAX_NUM_OPTIONS - 1);
      exit(EXIT_FAILURE);
   }

   /* Save option */
   code  = (Option_code) dst;
   option_list[length_option_list].code = code;
   if ((code == IMAGE_INFO) |
       (code == MINC_VERSION) || 
       (code == DIMNAMES) ||
       (code == VARNAMES)){
      option_list[length_option_list].value = NULL;
      return_value = FALSE;
   }
   else {
      /* Check for following argument */
      if (nextarg == NULL) {
         (void) fprintf(stderr, 
                        "\"%s\" option requires an additional argument\n",
                        key);
         return FALSE;
      }
      option_list[length_option_list].value = nextarg;
      return_value = TRUE;
   }
   length_option_list++;
   option_list[length_option_list].code = ENDLIST;

   return return_value;
   
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : report_error
@INPUT      : (none)
@OUTPUT     : (none)
@RETURNS    : Exit status.
@DESCRIPTION: Prints out the error message
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : May 19, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static int report_error(void)
{
   if (error_string == NULL) {
      (void) fprintf(stderr, "Error reading file.\n");
      return EXIT_FAILURE;
   }
   else {
      (void) fprintf(stdout, "%s\n", error_string);
      return EXIT_SUCCESS;
   }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_attname
@INPUT      : mincid - id of minc file
              string - string giving varname:attname
@OUTPUT     : varid - pointer to variale id
              name - name of attribute
@RETURNS    : MI_ERROR if an error occurs
@DESCRIPTION: Gets variable id and attribute name from a string of the
              form "varname:attname"
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : May 19, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static int get_attname(int mincid, char *string, int *varid, char *name)
{

#define ATT_SEP_CHAR ':'

   char *attname, varname[MAX_NC_NAME];
   int i;

   /* Get the variable name */
   for (i=0; (i < MAX_NC_NAME) && (string[i] != ATT_SEP_CHAR) 
           && (string[i] != '\0'); i++) {
      varname[i] = string[i];
   }
   if (string[i] != ATT_SEP_CHAR) {
      if (error_string == NULL) {
         (void) fprintf(stderr, "Invalid attribute name '%s'\n",
                        string);
      }
      return MI_ERROR;
   }
   varname[i] = '\0';
   attname = &string[i+1];
   
   /* Get varid and name */
   if (varname[0] == '\0') {
      *varid = NC_GLOBAL;
   }
   else if ((*varid = ncvarid(mincid, varname)) == MI_ERROR) {
      return MI_ERROR;
   }
   (void) strncpy(name, attname, MAX_NC_NAME-1);
   name[MAX_NC_NAME-1] = '\0';

   return MI_NOERROR;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : print_image_info
@INPUT      : mincid - id of minc file
@OUTPUT     : (none)
@RETURNS    : MI_ERROR if an error occurs
@DESCRIPTION: Prints information about image data in file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : May 19, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static int print_image_info(char *filename, int mincid)
{
   int imgid, ndims, dim[MAX_VAR_DIMS], varid;
   nc_type datatype;
   double valid_range[2];
   char *sign_type[] = {MI_UNSIGNED, MI_SIGNED};
   int sign_index;
   int is_signed;
   long length;
   int idim;
   char name[MAX_NC_NAME];
   int oldncopts;
   double dim_start, dim_step;

   /* Get information about variable */
   RTN_ERR(imgid = ncvarid(mincid, MIimage));
   RTN_ERR(ncvarinq(mincid, imgid, NULL, NULL, &ndims, dim, NULL));
   RTN_ERR(miget_datatype(mincid, imgid, &datatype, &is_signed));
   RTN_ERR(miget_valid_range(mincid, imgid, valid_range));

   /* Get sign index */
   sign_index = (is_signed ? 1 : 0);

   /* Write out image info line */
   (void) printf("file: %s\n", filename);
   (void) printf("image: %s %s %.20g to %.20g\n", 
                 sign_type[sign_index], type_names[datatype],
                 valid_range[0], valid_range[1]);

   /* Write out dimension names */
   (void) printf("image dimensions:");
   for (idim=0; idim<ndims; idim++) {
      RTN_ERR(ncdiminq(mincid, dim[idim], name, NULL));
      (void) printf(" %s", name);
   }
   (void) printf("\n");

   /* Write out dimension info */
   oldncopts = ncopts;
   ncopts = 0;
   (void) printf("    %-20s %8s %12s %12s\n", "dimension name", "length",
                 "step", "start");
   (void) printf("    %-20s %8s %12s %12s\n", "--------------", "------",
                 "----", "-----");
   for (idim=0; idim<ndims; idim++) {
      (void) ncdiminq(mincid, dim[idim], name, &length);
      (void) printf("    %-20s %8d", name, (int) length);
      varid = ncvarid(mincid, name);
      if (miattget1(mincid, varid, MIstep, NC_DOUBLE, &dim_step)!=MI_ERROR)
         (void) printf(" %12g", dim_step);
      else 
         (void) printf(" %12s", "unknown");
      if (miattget1(mincid, varid, MIstart, NC_DOUBLE, &dim_start)!=MI_ERROR)
         (void) printf(" %12g", dim_start);
      else 
         (void) printf(" %12s", "unknown");
      (void) printf("\n");
   }
   ncopts = oldncopts;

   return MI_NOERROR;

}

