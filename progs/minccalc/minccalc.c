/* ----------------------------- MNI Header -----------------------------------
minccalc.c
A expression parser that works voxel-by-voxel for minc files

Andrew Janke - rotor@cmr.uq.edu.au
Center for Magnetic Resonance
University of Queensland

Original Grammar and parser by
David Leonard - leonard@csee.uq.edu.au
Department of Computer Science
University of Queensland

Modifications by Peter Neelin - neelin@bic.mni.mcgill.ca
McConnell Brain Imaging Centre
Montreal Neurological Institute
McGill University

This is predominately a rehash of mincmath by Peter Neelin

 * $Log: minccalc.c,v $
 * Revision 1.9  2004-04-27 15:37:52  bert
 * Added -2 option
 *
 * Revision 1.8  2001/05/24 15:08:40  neelin
 * Added support for comments so that minccalc scripts can be created.
 *
 * Revision 1.7  2001/05/04 15:40:33  neelin
 * Added -outfile option.
 * Changed syntax of for to use curlies around first part.
 * Changed syntax of for and if to evaluate an expression in the body, rather
 * than an expression list in curlies.
 *
 * Revision 1.6  2001/05/02 16:27:19  neelin
 * Fixed handling of invalid values. Added NaN constant. Force copy of
 * data when assigning to a symbol so that s1 = s2 = expr does the right
 * thing (s1 and s2 should be separate copies of the data). Updated man
 * page.
 *
 * Revision 1.5  2001/05/02 01:38:15  neelin
 * Major changes to allow parallel evaluations. Created scalar_t type
 * along the lines of vector_t (reference counting, etc.). Compiles
 * and runs on basic standard deviation calculation (with test for invalid
 * data). Gives over 3 times speedup compared to old version (on linux box).
 * SD calculation is slightly under half the speed of mincaverage.
 * Changes are significant enough and testing is little enough that there
 * are probably lots of bugs left.
 *
 * Revision 1.4  2001/04/30 19:16:43  neelin
 * Added assignment operator, made symbol table global, added expression lists,
 * for loops, if operators and changed range operator to colon.
 *
 * Revision 1.3  2001/04/26 19:12:39  neelin
 * Finished up addition of operators and handling of invalid values.
 * This version seems to work.
 *
 * Revision 1.2  2001/04/24 18:17:09  neelin
 * Added CVS logging.
 *

Thu Dec 21 17:26:46 EST 2000 - Added use of voxel_loop
Thu Oct  5 17:09:12 EST 2000 - First alpha version
Mon May 28 01:00:01 EST 2000 - First minc version - Andrew Janke 
Mon May 21 01:01:01 EST 2000 - Original version "imgcalc" by David Leonard

---------------------------------------------------------------------------- */

#ifndef lint
static char rcsid[]="$Header: /private-cvsroot/minc/progs/minccalc/minccalc.c,v 1.9 2004-04-27 15:37:52 bert Exp $";
#endif

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <ParseArgv.h>
#include <voxel_loop.h>
#include <time_stamp.h>
#include <minc_def.h>
#include "node.h"

/* Constants */
#ifndef public
#  define public
#endif

#ifndef TRUE
#  define TRUE 1
#  define FALSE 0
#endif

/* Data values for invalid data and for uninitialized data */
#define INVALID_DATA -DBL_MAX
#define UNINITIALIZED_DATA DBL_MAX

/* Values for representing default case for command-line options */
#define DEFAULT_DBL DBL_MAX
#define DEFAULT_BOOL -1

/* Function prototypes */
public void do_math(void *caller_data, long num_voxels, 
                    int input_num_buffers, 
                    int input_vector_length, double *input_data[],
                    int output_num_buffers, int output_vector_length,
                    double *output_data[], Loop_Info *loop_info);
public char **read_file_names(char *filelist, int *num_files);
public char *read_expression_file(char *filename);
public int get_list_option(char *dst, char *key, int argc, char **argv);

/* Argument variables */
int Output_list_size = 0;
int Output_list_alloc = 0;
struct {
   char *symbol;
   char *file;
} *Output_list = NULL;
int      clobber =                     FALSE;
int      verbose =                     TRUE;
int      debug =                       FALSE;
int      is_signed =                   FALSE;
int      propagate_nan =               TRUE;
int      check_dim_info =              TRUE;
int      copy_all_header =             DEFAULT_BOOL;
int      use_nan_for_illegal_values =  TRUE;
int      max_buffer_size_in_kb =       4 * 1024;
double   valid_range[2] =              {0.0, 0.0};
double   constant =                    DEFAULT_DBL;
double   constant2[2] =                {DEFAULT_DBL, DEFAULT_DBL};
double   value_for_illegal_operations = DEFAULT_DBL;
nc_type  datatype =                    MI_ORIGINAL_TYPE;
char     *filelist =                   NULL;
char     *expr_file =                  NULL;
char     *expression =                 NULL;
int      eval_width =                  200;
#ifdef MINC2
int      minc2_format =                FALSE;
#endif /* MINC2 defined */

/* Argument table */
ArgvInfo argTable[] = {
   {NULL, ARGV_HELP, (char *) NULL, (char *) NULL, 
       "General options:"},
#ifdef MINC2
    {"-2", ARGV_CONSTANT, (char *) TRUE, (char *) &minc2_format,
     "Produce a MINC 2.0 format output file"},
#endif /* MINC2 defined */
   {"-clobber", ARGV_CONSTANT, (char *) TRUE, (char *) &clobber,
       "Overwrite existing file."},
   {"-noclobber", ARGV_CONSTANT, (char *) FALSE, (char *) &clobber,
       "Don't overwrite existing file (default)."},
   {"-no_clobber", ARGV_CONSTANT, (char *) FALSE, (char *) &clobber,
       "Synonym for -noclobber."},
   {"-verbose", ARGV_CONSTANT, (char *) TRUE, (char *) &verbose,
       "Print out log messages (default)."},
   {"-quiet", ARGV_CONSTANT, (char *) FALSE, (char *) &verbose,
       "Do not print out log messages."},
   {"-debug", ARGV_CONSTANT, (char *) TRUE, (char *) &debug,
       "Print out debugging messages."},
   {"-filelist", ARGV_STRING, (char *) 1, (char *) &filelist,
       "Specify the name of a file containing input file names (- for stdin)."},
   {"-copy_header", ARGV_CONSTANT, (char *) TRUE, (char *) &copy_all_header,
       "Copy all of the header from the first file."},
   {"-nocopy_header", ARGV_CONSTANT, (char *) FALSE, (char *) &copy_all_header,
       "Do not copy all of the header from the first file."},
   {"-filetype", ARGV_CONSTANT, (char *) MI_ORIGINAL_TYPE, (char *) &datatype,
       "Use data type of first file (default)."},
   {"-byte", ARGV_CONSTANT, (char *) NC_BYTE, (char *) &datatype,
       "Write out byte data."},
   {"-short", ARGV_CONSTANT, (char *) NC_SHORT, (char *) &datatype,
       "Write out short integer data."},
   {"-int", ARGV_CONSTANT, (char *) NC_INT, (char *) &datatype,
       "Write out 32-bit integer data."},
   {"-long", ARGV_CONSTANT, (char *) NC_INT, (char *) &datatype,
       "Superseded by -int."},
   {"-float", ARGV_CONSTANT, (char *) NC_FLOAT, (char *) &datatype,
       "Write out single-precision floating-point data."},
   {"-double", ARGV_CONSTANT, (char *) NC_DOUBLE, (char *) &datatype,
       "Write out double-precision floating-point data."},
   {"-signed", ARGV_CONSTANT, (char *) TRUE, (char *) &is_signed,
       "Write signed integer data."},
   {"-unsigned", ARGV_CONSTANT, (char *) FALSE, (char *) &is_signed,
       "Write unsigned integer data (default if type specified)."},
   {"-range", ARGV_FLOAT, (char *) 2, (char *) valid_range,
       "Valid range for output data."},
   {"-max_buffer_size_in_kb", ARGV_INT, (char *) 1, 
       (char *) &max_buffer_size_in_kb,
       "Specify the maximum size of the internal buffers (in kbytes)."},
   {"-check_dimensions", ARGV_CONSTANT, (char *) TRUE, 
       (char *) &check_dim_info,
       "Check that files have matching dimensions (default)."},
   {"-nocheck_dimensions", ARGV_CONSTANT, (char *) FALSE, 
       (char *) &check_dim_info,
       "Do not check that files have matching dimensions."},
   {"-ignore_nan", ARGV_CONSTANT, (char *) FALSE, (char *) &propagate_nan,
       "Ignore invalid data (NaN) for accumulations."},
   {"-propagate_nan", ARGV_CONSTANT, (char *) TRUE, (char *) &propagate_nan,
       "Invalid data in any file at a voxel produces a NaN (default)."},
   {"-nan", ARGV_CONSTANT, (char *) TRUE, 
       (char *) &use_nan_for_illegal_values,
       "Output NaN when an illegal operation is done (default)."},
   {"-zero", ARGV_CONSTANT, (char *) FALSE, 
       (char *) &use_nan_for_illegal_values,
       "Output zero when an illegal operation is done."},
   {"-illegal_value", ARGV_FLOAT, (char *) 1, 
       (char *) &value_for_illegal_operations,
       "Value to write out when an illegal operation is done."},
   {"-expression",  ARGV_STRING,  (char*)1,    (char*) &expression,
          "Expression to use in calculations."},  
   {"-expfile",  ARGV_STRING,  (char*)1,    (char*) &expr_file,
          "Name of file containing expression."}, 
   {"-outfile",  ARGV_GENFUNC,  (char*)get_list_option, (char*) &Output_list,
          "Symbol to save in an output file (2 args)."}, 
   {"-eval_width",  ARGV_INT,  (char*)1,    (char*) &eval_width,
          "Number of voxels to evaluate simultaneously."}, 
   {NULL, ARGV_END, NULL, NULL, NULL}
};

extern int yydebug;
sym_t      rootsym;
vector_t   A;
scalar_t   *Output_values;

/* Main program */
public int main(int argc, char *argv[]){
   char **infiles, **outfiles;
   int nfiles, nout;
   char *arg_string;
   Loop_Options *loop_options;
   char *pname;
   int i;
   ident_t ident;
   scalar_t scalar;

   /* Save time stamp and args */
   arg_string = time_stamp(argc, argv);

   /* Get arguments */
   pname = argv[0];
   if (ParseArgv(&argc, argv, argTable, 0) || (argc < 2)) {
      (void) fprintf(stderr, 
      "\nUsage: %s [options] [<in1.mnc> ...] <out.mnc>\n",
                     pname);
      (void) fprintf(stderr, 
        "       %s -help\n\n", pname);
      exit(EXIT_FAILURE);
   }

   /* Get output file names */
   nout = (Output_list == NULL ? 1 : Output_list_size);
   outfiles = MALLOC(nout * sizeof(*outfiles));
   if (Output_list == NULL) {
      outfiles[0] = argv[argc-1];
   }
   else {
      for (i=0; i < Output_list_size; i++) {
         outfiles[i] = Output_list[i].file;
      }
   }

   /* Get the list of input files either from the command line or
      from a file, or report an error if both are specified.
      Note that if -outfile is given then there is no output file name
      left on argv after option parsing. */

   nfiles = argc - 2;
   if (Output_list != NULL) nfiles++;
   if (filelist == NULL) {
      infiles = &argv[1];
   }
   else if (nfiles <= 0) {
      infiles = read_file_names(filelist, &nfiles);
      if (infiles == NULL) {
         (void) fprintf(stderr, 
                        "Error reading in file names from file \"%s\"\n",
                        filelist);
         exit(EXIT_FAILURE);
      }
   }
   else {
      (void) fprintf(stderr, 
                     "Do not specify both -filelist and input file names\n");
      exit(EXIT_FAILURE);
   }

   /* Make sure that we have something to process */
   if (nfiles == 0) {
      (void) fprintf(stderr, "No input files specified\n");
      exit(EXIT_FAILURE);
   }

   /* Get the expression from the file if needed */
   if ((expression == NULL) && (expr_file == NULL)) { 
      (void) fprintf(stderr, 
                     "An expression must be specified on the command line\n");
      exit(EXIT_FAILURE);
   }
   else if (expression == NULL) {
      expression = read_expression_file(expr_file);
   }

   /* Parse expression argument */
   if (debug) fprintf(stderr, "Feeding in expression %s\n", expression);
   lex_init(expression);
   if (debug) yydebug = 1; else yydebug = 0; 
   yyparse();
   lex_finalize();
   
   /* Optimize the expression tree */
   root = optimize(root);
   
   /* Setup the input vector from the input files */
   A = new_vector();
   for (i=0; i<nfiles; i++) {
      if (debug) fprintf(stderr,"Getting file[%d] %s\n", i, argv[i+1]);
      scalar = new_scalar(eval_width);
      vector_append(A, scalar);
      scalar_free(scalar);
   }
      
   /* Construct initial symbol table from the A vector. Since setting
      a symbol makes a copy, we have to get a handle to that copy. */
   rootsym = sym_enter_scope(NULL);
   ident = new_ident("A");
   sym_set_vector(eval_width, NULL, A, ident, rootsym);
   vector_free(A);
   A = sym_lookup_vector(ident, rootsym);
   if (A==NULL) {
      (void) fprintf(stderr, "Error initializing symbol table\n");
      exit(EXIT_FAILURE);
   }
   vector_incr_ref(A);

   /* Add output symbols to the table */
   if (Output_list == NULL) {
      Output_values = NULL;
   }
   else {
      Output_values = MALLOC(Output_list_size * sizeof(*Output_values));
      for (i=0; i < Output_list_size; i++) {
         ident = ident_lookup(Output_list[i].symbol);
         scalar = new_scalar(eval_width);
         sym_set_scalar(eval_width, NULL, scalar, ident, rootsym);
         scalar_free(scalar);
         Output_values[i] = sym_lookup_scalar(ident, rootsym);
      }
   }

   /* Set default copy_all_header according to number of input files */
   if (copy_all_header == DEFAULT_BOOL)
      copy_all_header = (nfiles == 1);

   if (value_for_illegal_operations == DEFAULT_DBL) {
      if (use_nan_for_illegal_values)
         value_for_illegal_operations = INVALID_DATA;
      else
         value_for_illegal_operations = 0.0;
   }

   /* Do math */
   loop_options = create_loop_options();
   set_loop_verbose(loop_options, verbose);
   set_loop_clobber(loop_options, clobber);
#ifdef MINC2
   set_loop_v2format(loop_options, minc2_format);
#endif /* MINC2 defined */
   set_loop_datatype(loop_options, datatype, is_signed, 
                     valid_range[0], valid_range[1]);
   set_loop_copy_all_header(loop_options, copy_all_header);
   set_loop_buffer_size(loop_options, (long) 1024 * max_buffer_size_in_kb);
   set_loop_check_dim_info(loop_options, check_dim_info);
   voxel_loop(nfiles, infiles, nout, outfiles, arg_string, loop_options,
              do_math, NULL);
   free_loop_options(loop_options);

   
   /* Clean up */
   vector_free(A);
   sym_leave_scope(rootsym);
   if (expr_file != NULL) FREE(expression);
   FREE(outfiles);
   if (Output_list != NULL) FREE(Output_list);
   if (Output_values != NULL) FREE(Output_values);
   exit(EXIT_SUCCESS);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : do_math
@INPUT      : Standard for voxel loop
@OUTPUT     : Standard for voxel loop
@RETURNS    : (nothing)
@DESCRIPTION: Routine doing math operations.
@METHOD     : 
@GLOBALS    : Output_values, A
@CALLS      : 
@CREATED    : April 25, 1995 (Peter Neelin)
@MODIFIED   : Thu Dec 21 17:08:40 EST 2000 (Andrew Janke - rotor@cmr.uq.edu.au)
---------------------------------------------------------------------------- */
public void do_math(void *caller_data, long num_voxels, 
                    int input_num_buffers, int input_vector_length,
                    double *input_data[],
                    int output_num_buffers, int output_vector_length,
                    double *output_data[],
                    Loop_Info *loop_info){
   long ivox, ibuff, ivalue, nvox;
   scalar_t scalar, *output_scalars;
   int num_output, iout;

   /* Check arguments */
   if ((output_num_buffers < 1) || 
       (output_vector_length != input_vector_length)) {
      (void) fprintf(stderr, "Bad arguments to do_math!\n");
      exit(EXIT_FAILURE);
   }
   
   /* Loop through the voxels */
   for (ivox=0; ivox < num_voxels*input_vector_length; ivox+=eval_width) {

      /* Figure out how many voxels to work on at once */
      nvox = eval_width;
      if (ivox + nvox > num_voxels) nvox = num_voxels - ivox;

      /* Copy the data into the A vector */
      for (ivalue=0; ivalue < nvox; ivalue++) {
         for (ibuff=0; ibuff < input_num_buffers; ibuff++){
            A->el[ibuff]->vals[ivalue] = input_data[ibuff][ivox+ivalue];
         }
      }

      /* Some debugging */
      if (debug) {
         (void) fprintf(stderr, "\n===New voxel===\n");
      }

      /* Evaluate the expression */
      scalar = eval_scalar((int) nvox, NULL, root, rootsym);

      /* Get the list of scalar values to write out */
      if (Output_values == NULL) {
         num_output = 1;
         output_scalars = &scalar;
      }
      else {
         num_output = Output_list_size;
         output_scalars = Output_values;
      }

      /* Copy the scalar values into the right buffers */
      for (iout=0; iout < num_output; iout++) {
         for (ivalue=0; ivalue < nvox; ivalue++) {
            output_data[iout][ivox+ivalue] = 
               output_scalars[iout]->vals[ivalue];
         }
      }

      /* Free things up */
      scalar_free(scalar);

      if (debug) {
         (void) printf("Voxel result = %g\n", output_data[0][ivox]);
      }
         

   }

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_file_names
@INPUT      : filelist - name of file from which to read names
@OUTPUT     : num_files - number of files read in
@RETURNS    : Pointer to a NULL-terminated array of file names
@DESCRIPTION: Reads in a list of file names from file filelist or stdin if 
              "-" is specified. Returns NULL if an error occurs. If
              no error occurs, then a pointer to an empty array is 
              returned and num_files is zero.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : March 8, 1995 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public char **read_file_names(char *filelist, int *num_files)
{
#define FILE_NAME_ALLOC_SIZE 10
   char **files;
   int array_size;
   int nfiles;
   FILE *fp;
   char line[PATH_MAX+1];
   int length;

   /* Open the file */
   if (strcmp(filelist, "-") == 0) {
      fp = stdin;
   }
   else {
      fp = fopen(filelist, "r");
      if (fp == NULL) {
         (void) fprintf(stderr, "Error opening file \"%s\"\n", filelist);
         return NULL;
      }
   }

   /* Allocate an initial array and NULL-terminate it */
   array_size = FILE_NAME_ALLOC_SIZE;
   files = MALLOC(sizeof(*files) * array_size);
   if (files == NULL) {
      (void) fprintf(stderr, "Error allocating memory\n");
      return NULL;
   }
   nfiles = 0;
   files[nfiles] = NULL;

   /* Read in file names */
   while (fgets(line, sizeof(line)/sizeof(line[0]), fp) != NULL) {

      /* Remove a trailing newline and check that there is a name */
      length = strlen(line);
      if ((length > 0) && (line[length-1] == '\n')) {
         line[length-1] = '\0';
         length--;
      }
      if (length == 0) continue;

      /* Make room for names if needed */
      while (nfiles >= array_size-1) {
         array_size += FILE_NAME_ALLOC_SIZE;
         files = REALLOC(files, sizeof(*files) * array_size);
         if (files == NULL) {
            (void) fprintf(stderr, "Error allocating memory\n");
            return NULL;
         }
      }

      /* Save the name, making sure that the list is NULL-terminated */
      files[nfiles] = strdup(line);
      if (files[nfiles] == NULL) {
         (void) fprintf(stderr, "Error allocating memory\n");
         return NULL;
      }
      nfiles++;
      files[nfiles] = NULL;
   }

   /* Close the file */
   (void) fclose(fp);

   /* Return the number of files */
   *num_files = nfiles;

   return files;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_expression_file
@INPUT      : filename - Name of file from which to read expression
@OUTPUT     : (none)
@RETURNS    : String containing expression - must be freed by caller.
@DESCRIPTION: Reads in an expression from a file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : May 3, 2001 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public char *read_expression_file(char *filename)
{
   struct stat statbuf;
   size_t size;
   FILE *fp;
   char *expression;
   int ichar;
   int beginning_of_line, in_comment;
   int ch;

#define ALLOC_SIZE 1024

   /* Set the default allocation size - zero means allocate as we go */
   size = 0;

   /* Check for reading from stdin */
   if (strcmp(filename, "-") == 0) {
      fp = stdin;
   }

   /* Otherwise read from file. Get allocation size from file size. */
   else {

      /* Get the file size */
      if (stat(filename, &statbuf) >= 0) {
         size = statbuf.st_size + 1;
      }

      /* Open the file */
      if ((fp=fopen(filename, "r")) == NULL) {
         (void) fprintf(stderr, "Unable to open expression file \"%s\"\n",
                        filename);
         exit(EXIT_FAILURE);
      }
      
   }

   /* Make sure that we are going to allocate something */
   if (size == 0) size = ALLOC_SIZE;

   /* Get space */
   expression = MALLOC(size * sizeof(*expression));

   /* Read the expression */
   ichar = 0;
   beginning_of_line = TRUE;
   in_comment = FALSE;
   while ((ch = getc(fp)) != EOF) {

      /* Check for newline to end comments */
      if (ch == '\n') {
         beginning_of_line = TRUE;
         in_comment = FALSE;
      }

      /* Check for comment character as first non-whitespace char of line */
      else if (beginning_of_line && (ch == '#')) {
         in_comment = TRUE;
         beginning_of_line = FALSE;
      }

      /* Check for first non-whitespace char of line */
      else if (!isspace(ch)) {
         beginning_of_line = FALSE;
      }

      /* If not in a comment, then save the character */
      if (!in_comment) {

         /* Check whether we need more space */
         if (ichar >= size-1) {
            size += ALLOC_SIZE;
            expression = REALLOC(expression, size * sizeof(expression));
         }

         /* Save the character */
         expression[ichar] = (char) ch;
         ichar++;
      }
   }
   expression[ichar] = '\0';

   /* Close the file */
   if (fp != stdin) {
      (void) fclose(fp);
   }

   /* Return the expression */
   return expression;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_list_option
@INPUT      : dst - client data passed by ParseArgv
              key - matching key in argv
              argc - number of arguments passed in
              argv - argument list
@OUTPUT     : (none)
@RETURNS    : Number of arguments left in argv list.
@DESCRIPTION: Gets arguments from the command line and appends them
              to a list, chosen based on key.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : May 3, 2001 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int get_list_option(char *dst, char *key, int argc, char **argv)
     /* ARGSUSED */
{
   enum {OPT_OUTPUT_SYMBOL} option_type;
   void **list;
   size_t entry_size;
   int *list_size, *list_alloc, index;
   int num_args, iarg;

   /* Check the key */
   list = (void **) dst;
   if (strcmp(key, "-outfile") == 0) {
      option_type = OPT_OUTPUT_SYMBOL;
      list_size = &Output_list_size;
      list_alloc = &Output_list_alloc;
      entry_size = sizeof(Output_list[0]);
      num_args = 2;
   }
   else {
      (void) fprintf(stderr, 
                     "Internal error - unrecognized key in get_list_option\n");
      exit(EXIT_FAILURE);
   }

   /* Check for following arguments */
   if (argc < num_args) {
      (void) fprintf(stderr, 
                     "\"%s\" option requires %d additional arguments\n",
                     key, num_args);
      exit(EXIT_FAILURE);
   }

   /* Get more space */
   (*list_size)++;
   if (*list_size > *list_alloc) {
      *list_alloc += 10;
      if (*list == NULL) {
         *list = 
            MALLOC(*list_alloc * entry_size);
      }
      else {
         *list = 
            REALLOC(*list, 
                    *list_alloc * entry_size);
      }
   }
   index = *list_size - 1;

   /* Save the values */
   if (option_type == OPT_OUTPUT_SYMBOL) {
      Output_list[index].symbol = argv[0];
      Output_list[index].file = argv[1];
   }

   /* Modify the argument list */
   if (num_args > 0) {
      for (iarg=0; iarg < (argc - num_args); iarg++) {
         argv[iarg] = argv[iarg + num_args];
      }
   }

   return argc - num_args;

}

