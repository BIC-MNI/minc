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

This is predominately a rehash of mincmath by Peter Neelin

Mon May 21 01:01:01 EST 2000 - Original version "imgcalc" by David Leonard
Mon May 28 01:00:01 EST 2000 - First minc version - Andrew Janke 
Thu Oct  5 17:09:12 EST 2000 - First alpha version
Thu Dec 21 17:26:46 EST 2000 - Added use of voxel_loop
---------------------------------------------------------------------------- */

#ifndef lint
static char rcsid[]="$Header: /private-cvsroot/minc/progs/minccalc/minccalc.c,v 1.1 2001-04-24 18:14:03 neelin Exp $";
#endif

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
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

/* Structure for window information */
typedef struct {
   int num_constants;
   double constants[2];
   int propagate_nan;
   double illegal_value;
} Math_Data;

/* Function prototypes */
public void do_math(void *caller_data, long num_voxels, 
                    int input_num_buffers, 
                    int input_vector_length, double *input_data[],
                    int output_num_buffers, int output_vector_length,
                    double *output_data[], Loop_Info *loop_info);
public char **read_file_names(char *filelist, int *num_files);

/* Argument variables */
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
const char    *expression = NULL;

/* Argument table */
ArgvInfo argTable[] = {
   {NULL, ARGV_HELP, (char *) NULL, (char *) NULL, 
       "General options:"},
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
   {NULL, ARGV_END, NULL, NULL, NULL}
};

extern int yydebug;
sym_t      rootsym;
vector_t   A;

/* Main program */
public int main(int argc, char *argv[]){
   char **infiles, **outfiles;
   int nfiles, nout;
   char *arg_string;
   Math_Data math_data;
   Loop_Options *loop_options;
   char *pname;
   int i;
      

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
   nout = 1;
   outfiles = &argv[argc-1];

   /* Get the list of input files either from the command line or
      from a file, or report an error if both are specified */

   nfiles = argc - 2;
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

   /* Parse expression argument */
   if (expression == NULL) { 
      (void) fprintf(stderr, 
                     "An expression must be specified on the command line\n");
      exit(EXIT_FAILURE);
   }
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
      vector_append(A, 0);
      }
      
   /* Construct initial symbol table from the A vector */
   rootsym = sym_enter_vector(A, new_ident("A"), NULL);
   
   /* Set default copy_all_header according to number of input files */
   if (copy_all_header == DEFAULT_BOOL)
      copy_all_header = (nfiles == 1);

   if (value_for_illegal_operations != DEFAULT_DBL)
      math_data.illegal_value = value_for_illegal_operations;
   else if (use_nan_for_illegal_values)
      math_data.illegal_value = INVALID_DATA;
   else
      math_data.illegal_value = 0.0;

   /* Do math */
   loop_options = create_loop_options();
   set_loop_verbose(loop_options, verbose);
   set_loop_clobber(loop_options, clobber);
   set_loop_datatype(loop_options, datatype, is_signed, valid_range[0], valid_range[1]);
   set_loop_copy_all_header(loop_options, copy_all_header);
   set_loop_buffer_size(loop_options, (long) 1024 * max_buffer_size_in_kb);
   set_loop_check_dim_info(loop_options, check_dim_info);
   voxel_loop(nfiles, infiles, nout, outfiles, arg_string, loop_options,
              do_math, (void *) &math_data);
   free_loop_options(loop_options);

   
   /* Clean up */
   sym_leave(rootsym);
   exit(EXIT_SUCCESS);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : do_math
@INPUT      : Standard for voxel loop
@OUTPUT     : Standard for voxel loop
@RETURNS    : (nothing)
@DESCRIPTION: Routine doing math operations.
@METHOD     : 
@GLOBALS    : 
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
   long ivox, i;

   /* Check arguments */
   if ((output_num_buffers != 1) || 
       (output_vector_length != input_vector_length)) {
      (void) fprintf(stderr, "Bad arguments to do_math!\n");
      exit(EXIT_FAILURE);
   }
   
   /* Loop through the voxels */
   for (ivox=0; ivox < num_voxels*input_vector_length; ivox++) {
      
      for (i=0; i < input_num_buffers; i++){
         A->el[i] = input_data[i][ivox];
      }
      
      output_data[0][ivox] = eval_scalar(root, rootsym);
         
/*      if (debug) fprintf(stderr, "%e - %e = %e\n", value1, value2, output_data[0][ivox]); */

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

