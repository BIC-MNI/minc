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
---------------------------------------------------------------------------- */

#ifndef lint
static char rcsid[]="$Header: /private-cvsroot/minc/progs/mincinfo/mincinfo.c,v 1.3 1993-06-07 15:03:03 neelin Exp $";
#endif

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <minc.h>
#include <ParseArgv.h>

/* Constants */
#define public
#ifndef TRUE
#  define TRUE 1
#  define FALSE 0
#endif
#define MAX_NUM_OPTIONS 100
char *type_names[] = {
   NULL, "byte", "char", "short", "long", "float", "double"
};
double default_min[][2] = {
   0.0, 0.0,
   0.0, SCHAR_MIN,
   0.0, 0.0,
   0.0, SHRT_MIN,
   0.0, LONG_MAX,
   0.0, 0.0,
   0.0, 0.0,
};
double default_max[][2] = {
   0.0, 0.0,
   UCHAR_MAX, SCHAR_MAX,
   0.0, 0.0,
   USHRT_MAX, SHRT_MAX,
   ULONG_MAX, LONG_MAX,
   1.0, 1.0,
   1.0, 1.0
};

/* Types */
typedef enum  {
   ENDLIST, IMAGE_INFO, DIMNAMES, VARNAMES,
   DIMLENGTH, VARTYPE, VARDIMS, VARATTS, VARVALUES,
   ATTTYPE, ATTVALUE
} Option_code;
typedef struct {
   Option_code code;
   char *value;
} Option_type;

/* Macros */
#define CHK_ERR(code) if ((code) == MI_ERROR) report_error();

/* Function prototypes */
public int main(int argc, char *argv[]);
public int get_option(char *dst, char *key, char *nextarg);
public void report_error(void);
public void get_attname(int mincid, char *string, int *varid, char *name);
public void print_image_info(char *filename, int mincid);

/* Variables used for argument parsing */
char *error_string = NULL;
Option_type option_list[MAX_NUM_OPTIONS] = {ENDLIST};
int length_option_list = 0;

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
   {NULL, ARGV_END, NULL, NULL, NULL}
};

/* Main program */

public int main(int argc, char *argv[])
{
   char *filename;
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

   /* Check arguments */
   if (ParseArgv(&argc, argv, argTable, 0) || (argc != 2)) {
      (void) fprintf(stderr, 
                     "\nUsage: %s [<options>] <mincfile>\n", argv[0]);
      (void) fprintf(stderr,
                       "       %s -help\n\n", argv[0]);
      exit(EXIT_FAILURE);
   }
   filename  = argv[1];

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

   /* Open the file */
   mincid = ncopen(filename, NC_NOWRITE);

   /* Loop through print options */
   for (option=0; option < length_option_list; option++) {
      string = option_list[option].value;
      switch (option_list[option].code) {
      case IMAGE_INFO:
         print_image_info(filename, mincid);
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
         get_attname(mincid, string, &varid, name);
         CHK_ERR(ncattinq(mincid, varid, name, &datatype, NULL));
         (void) printf("%s\n", type_names[datatype]);
         break;
      case ATTVALUE:
         get_attname(mincid, string, &varid, name);
         CHK_ERR(ncattinq(mincid, varid, name, &datatype, &att_length));
         if (datatype == NC_CHAR) {
            cdata = malloc((att_length+1)*sizeof(char));
            if (miattgetstr(mincid, varid, name, att_length+1, cdata)==NULL)
               report_error();
            (void) printf("%s\n", cdata);
         }
         else {
            ddata = malloc(att_length * sizeof(double));
            CHK_ERR(miattget(mincid, varid, name, NC_DOUBLE, att_length,
                             ddata, NULL));
            for (iatt=0; iatt<att_length; iatt++) {
               (void) printf("%.20g ", ddata[iatt]);
            }
            (void) printf("\n");
         }
         break;
      default:
         (void) fprintf(stderr, "%s: Program bug!\n", argv[0]);
         return EXIT_FAILURE;
      }
   }

   /* Close the file */
   (void) ncclose(mincid);

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
public int get_option(char *dst, char *key, char *nextarg)
{           /* ARGSUSED */
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
       (code == DIMNAMES) ||
       (code == VARNAMES)){
      option_list[length_option_list].value = NULL;
      return_value = FALSE;
   }
   else {
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
@RETURNS    : (none)
@DESCRIPTION: Prints out the error message
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : May 19, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void report_error(void)
{
   if (error_string == NULL) {
      (void) fprintf(stderr, "Error reading file.\n");
      exit(EXIT_FAILURE);
   }
   else {
      (void) fprintf(stdout, "%s\n", error_string);
      exit(EXIT_SUCCESS);
   }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_attname
@INPUT      : mincid - id of minc file
              string - string giving varname:attname
@OUTPUT     : varid - pointer to variale id
              name - name of attribute
@RETURNS    : (none)
@DESCRIPTION: Gets variable id and attribute name from a string of the
              form "varname:attname"
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : May 19, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void get_attname(int mincid, char *string, int *varid, char *name)
{
   char *cptr;

   /* Check for : */
   cptr = strchr(string, ':');
   if (cptr == NULL) {
      if (error_string != NULL) report_error();
      (void) fprintf(stderr, "Invalid attribute name '%s'\n",
                     string);
      exit(EXIT_FAILURE);
   }
   *cptr = '\0';
   cptr++;

   /* Get varid and name */
   if (*string == '\0') {
      *varid = NC_GLOBAL;
   }
   else {
      CHK_ERR(*varid=ncvarid(mincid, string));
   }
   (void) strncpy(name, cptr, MAX_NC_NAME-1);

   return;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : print_image_info
@INPUT      : mincid - id of minc file
@OUTPUT     : (none)
@RETURNS    : (none)
@DESCRIPTION: Prints information about image data in file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : May 19, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void print_image_info(char *filename, int mincid)
{
   int imgid, ndims, dim[MAX_VAR_DIMS], varid;
   nc_type datatype;
   double valid_range[2];
   char sign_type[MI_MAX_ATTSTR_LEN];
   int sign_index;
   int att_length;
   long length;
   int idim;
   char name[MAX_NC_NAME];
   int oldncopts;
   double dim_start, dim_step;

   /* Get information about variable */
   CHK_ERR(imgid = ncvarid(mincid, MIimage));
   CHK_ERR(ncvarinq(mincid, imgid, NULL, &datatype, &ndims, dim, NULL));

   oldncopts = ncopts;
   ncopts = 0;

   /* Look for signtype */
   if ((miattgetstr(mincid, imgid, MIsigntype, MI_MAX_ATTSTR_LEN, sign_type)
                  == NULL) || ((strcmp(sign_type, MI_UNSIGNED)!=0) && 
                               (strcmp(sign_type, MI_SIGNED)!=0))) {
      if (datatype == NC_BYTE)
         (void) strcpy(sign_type, MI_UNSIGNED);
      else
         (void) strcpy(sign_type, MI_SIGNED);
   }
   sign_index = (strcmp(sign_type, MI_UNSIGNED) == 0) ? 0 : 1;

   /* Get valid range */
   if ((miattget(mincid, imgid, MIvalid_range, NC_DOUBLE, 2, valid_range,
                 &att_length) == MI_ERROR) || (att_length != 2)) {
      if (miattget1(mincid, imgid, MIvalid_min, NC_DOUBLE, 
                    &valid_range[0]) == MI_ERROR)
         valid_range[0] = default_min[datatype][sign_index];
      if (miattget1(mincid, imgid, MIvalid_max, NC_DOUBLE, 
                    &valid_range[1]) == MI_ERROR)
         valid_range[1] = default_max[datatype][sign_index];
   }

   ncopts = oldncopts;

   /* Write out image info line */
   (void) printf("file: %s\n", filename);
   (void) printf("image: %s %s %.20g to %.20g\n", 
                 sign_type, type_names[datatype],
                 valid_range[0], valid_range[1]);

   /* Write out dimension names */
   (void) printf("image dimensions:");
   for (idim=0; idim<ndims; idim++) {
      CHK_ERR(ncdiminq(mincid, dim[idim], name, NULL));
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

   return;
}

