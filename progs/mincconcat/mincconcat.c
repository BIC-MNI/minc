/* ----------------------------- MNI Header -----------------------------------
@NAME       : mincconcat
@INPUT      : argc, argv - command line arguments
@OUTPUT     : (none)
@RETURNS    : error status
@DESCRIPTION: Program to allow concatentation of multiple minc files, either
              adding a new dimension, or expanding a dimension of size 1.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : March 7, 1995 (Peter Neelin)
@MODIFIED   : 
 * $Log: mincconcat.c,v $
 * Revision 6.7  2001-08-16 16:41:33  neelin
 * Added library functions to handle reading of datatype, sign and valid range,
 * plus writing of valid range and setting of default ranges. These functions
 * properly handle differences between valid_range type and image type. Such
 * difference can cause valid data to appear as invalid when double to float
 * conversion causes rounding in the wrong direction (out of range).
 * Modified voxel_loop, volume_io and programs to use these functions.
 *
 * Revision 6.6  2001/08/16 13:32:33  neelin
 * Partial fix for valid_range of different type from image (problems
 * arising from double to float conversion/rounding). NOT COMPLETE.
 *
 * Revision 6.5  2001/04/24 13:38:42  neelin
 * Replaced NC_NAT with MI_ORIGINAL_TYPE.
 *
 * Revision 6.4  2001/04/17 18:40:17  neelin
 * Modifications to work with NetCDF 3.x
 * In particular, changed NC_LONG to NC_INT (and corresponding longs to ints).
 * Changed NC_UNSPECIFIED to NC_NAT.
 * A few fixes to the configure script.
 *
 * Revision 6.3  2000/07/07 13:33:34  neelin
 * Added option -filelist to read file names from a file. This gets around
 * command-line length limits.
 *
 * Revision 6.2  1999/10/19 14:45:19  neelin
 * Fixed Log subsitutions for CVS
 *
 * Revision 6.1  1998/08/13 19:34:37  neelin
 * Always create concatenation coordinate variable subscription by
 * dimension.
 *
 * Revision 6.0  1997/09/12  13:24:15  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:25:13  neelin
 * Release of minc version 0.5
 *
 * Revision 4.1  1997/06/03  14:57:23  neelin
 * Really fixed dimension width name suffixes.
 *
 * Revision 4.0  1997/05/07  20:01:54  neelin
 * Release of minc version 0.4
 *
 * Revision 3.4  1997/04/21  20:28:45  neelin
 * Changed width suffix from _width to -width.
 *
 * Revision 3.3  1996/04/11  19:31:43  neelin
 * Added -sequential and -interleaved options.
 *
 * Revision 3.2  1995/11/16  13:18:16  neelin
 * Added include of math.h to get declaration of strtod under SunOs
 *
 * Revision 3.1  1995/09/29  12:59:06  neelin
 * Fixed bug in handling of image-min/max when these variables are not
 * present in the input file.
 *
 * Revision 3.0  1995/05/15  19:32:40  neelin
 * Release of minc version 0.3
 *
 * Revision 1.1  1995/05/11  12:35:53  neelin
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
static char rcsid[]="$Header: /private-cvsroot/minc/progs/mincconcat/mincconcat.c,v 6.7 2001-08-16 16:41:33 neelin Exp $";
#endif

#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <minc.h>
#include <ParseArgv.h>
#include <time_stamp.h>
#include <voxel_loop.h>
#include <minc_def.h>

/* Constants */
#ifndef TRUE
#  define TRUE 1
#endif
#ifndef FALSE
#  define FALSE 0
#endif

#define COORD_EPSILON (100.0 * FLT_EPSILON)

#define DIM_WIDTH_SUFFIX "-width"

/* Default ncopts values for error handling */
#define NC_OPTS_VAL NC_VERBOSE | NC_FATAL

/* Macros */
#define ABS(x) ( ((x) > 0) ? (x) : (-(x)) )

/* Double_Array structure */
typedef struct {
   int numvalues;
   double *values;
} Double_Array;

/* Concat_info structure */
typedef struct {
   char *output_file;
   int num_input_files;
   char *history;
   int clobber;
   int verbose;
   nc_type output_datatype;
   int output_is_signed;
   double output_valid_range[2];
   char *dimension_name;
   int *num_file_coords;
   int **file_to_dim_order;
   double **file_coords;
   double **file_widths;        /* Array of NULL pointers if no widths */
   int coords_specified;
   int dimension_in_input_file;
   int concat_dimension_length;
   int regular_spacing;
   int constant_width;
   int have_widths;
   double dim_step;
   double dim_start;
   int output_mincid, output_icvid;
   int is_floating_type;
   double global_minimum;
   double global_maximum;
   long max_memory_use_in_kb;
   int check_dim_info;
} Concat_Info;

/* Sort structure */
typedef struct {
   double coord;
   int curfile;
   int curcoord;
} Sort_Element;

/* Function prototypes */
public int main(int argc, char *argv[]);
public void get_arginfo(int argc, char *argv[],
                        int *num_input_files, char ***input_files,
                        Concat_Info *concat_info);
public int get_double_list(char *dst, char *key, char *nextarg);
public void get_concat_dim_name(Concat_Info *concat_info,
                                char *first_filename, int *first_mincid);
public void get_input_file_info(void *caller_data, int input_mincid,
                                int input_curfile, Loop_Info *loop_info);
public int get_image_dimension_id(int input_mincid, char *dimension_name);
public void do_concat(void *caller_data, long num_voxels, 
                      int input_num_buffers, int input_vector_length,
                      double *input_data[],
                      int output_num_buffers, int output_vector_length,
                      double *output_data[],
                      Loop_Info *loop_info);
public void sort_coords(Concat_Info *concat_info);
public int sort_function(const void *value1, const void *value2);
public void create_concat_file(int inmincid, Concat_Info *concat_info);
private void update_history(int mincid, char *arg_string);
public char **read_file_names(char *filelist, int *num_files);

/* Globals */
static int Sort_ascending = TRUE;
static int Sort_sequential = FALSE;

/* Main program */

public int main(int argc, char *argv[])
{
   Concat_Info *concat_info;
   Loop_Options *loop_options;
   int num_input_files;
   char **input_files;
   int first_mincid, imgid;
   double valid_range[2];
   nc_type file_datatype;

   /* Allocate the concat_info structure */
   concat_info = MALLOC(sizeof(*concat_info));

   /* Get argument information */
   get_arginfo(argc, argv, &num_input_files, &input_files, concat_info);

   /* Look for the dimension in the input file */
   get_concat_dim_name(concat_info, input_files[0], &first_mincid);

   /* Set up loop options */
   loop_options = create_loop_options();
   set_loop_verbose(loop_options, concat_info->verbose);
   set_loop_first_input_mincid(loop_options, first_mincid);
   set_loop_input_file_function(loop_options, get_input_file_info);
   set_loop_accumulate(loop_options, TRUE, 0, NULL, NULL);
   if (concat_info->dimension_in_input_file) {
      set_loop_dimension(loop_options, concat_info->dimension_name);
   }
   set_loop_buffer_size(loop_options, 
                        1024 * concat_info->max_memory_use_in_kb);
   set_loop_check_dim_info(loop_options,
                           concat_info->check_dim_info);

   /* Initialize global min and max */
   concat_info->global_minimum = DBL_MAX;
   concat_info->global_maximum = -DBL_MAX;

   /* Loop over files */
   voxel_loop(num_input_files, input_files, 0, NULL, NULL,
              loop_options, do_concat, concat_info);

   /* Close the output file */
   imgid = ncvarid(concat_info->output_mincid, MIimage);
   (void) miattputstr(concat_info->output_mincid, 
                      imgid, MIcomplete, MI_TRUE);
   if (concat_info->is_floating_type) {
      if ((concat_info->global_minimum == DBL_MAX) && 
          (concat_info->global_maximum == -DBL_MAX)) {
         concat_info->global_minimum = 0.0;
         concat_info->global_maximum = 1.0;
      }
      valid_range[0] = concat_info->global_minimum;
      valid_range[1] = concat_info->global_maximum;

      (void) miset_valid_range(concat_info->output_mincid, imgid,  
                               valid_range);

   }
   (void) miclose(concat_info->output_mincid);
   (void) miicv_free(concat_info->output_icvid);

   /* Free stuff */
   free_loop_options(loop_options);
   FREE(concat_info);

   exit(EXIT_SUCCESS);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_arginfo
@INPUT      : argc - number of command-line arguments
              argv - command-line arguments
@OUTPUT     : num_input_files - number of input files
              input_files - array of strings containing input file names
              concat_info - information for concatenating files
@RETURNS    : (nothing)
@DESCRIPTION: Routine to get information from arguments about input and 
              output files and concatenation.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : March 11, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void get_arginfo(int argc, char *argv[],
                        int *num_input_files, char ***input_files,
                        Concat_Info *concat_info)
{

   /* Argument variables */
   static int clobber = FALSE;
   static int verbose = TRUE;
   static nc_type datatype = MI_ORIGINAL_TYPE;
   static int is_signed = INT_MIN;
   static double valid_range[2] = {0.0, 0.0};
   static char *dimension_name = NULL;
   static double dimension_start = DBL_MAX;
   static double dimension_step = DBL_MAX;
   static double dimension_width = DBL_MAX;
   static Double_Array dimension_coords = {0, NULL};
   static Double_Array dimension_widths = {0, NULL};
   static int max_chunk_size_in_kb = 4 * 1024;
   static int check_dim_info = TRUE;
   static char *filelist = NULL;

   /* Argument table */
   static ArgvInfo argTable[] = {
      {NULL, ARGV_HELP, (char *) NULL, (char *) NULL, 
          "General options:"},
      {"-clobber", ARGV_CONSTANT, (char *) TRUE, (char *) &clobber, 
          "Overwrite existing file."},
      {"-noclobber", ARGV_CONSTANT, (char *) FALSE, (char *) &clobber, 
          "Do not overwrite existing file (default)."},
      {"-verbose", ARGV_CONSTANT, (char *) TRUE, (char *) &verbose, 
          "Print out log messages as processing is being done (default).\n"},
      {"-quiet", ARGV_CONSTANT, (char *) FALSE, (char *) &verbose, 
          "Do not print out any log messages."},
      {"-max_chunk_size_in_kb", ARGV_INT, (char *) 1, 
          (char *) &max_chunk_size_in_kb,
          "Specify the maximum size of the copy buffer (in kbytes)."},
      {"-filelist", ARGV_STRING, (char *) 1, (char *) &filelist,
       "Specify the name of a file containing input file names (- for stdin)."},

      {NULL, ARGV_HELP, (char *) NULL, (char *) NULL, 
          "Output type options:"},
      {"-filetype", ARGV_CONSTANT, (char *) MI_ORIGINAL_TYPE, (char *) &datatype,
          "Don't do any type conversion (default)."},
      {"-byte", ARGV_CONSTANT, (char *) NC_BYTE, (char *) &datatype,
          "Convert to  byte data"},
      {"-short", ARGV_CONSTANT, (char *) NC_SHORT, (char *) &datatype,
          "Convert to short integer data"},
      {"-int", ARGV_CONSTANT, (char *) NC_INT, (char *) &datatype,
          "Convert to 32-bit integer data"},
      {"-long", ARGV_CONSTANT, (char *) NC_INT, (char *) &datatype,
          "Superseded by -int"},
      {"-float", ARGV_CONSTANT, (char *) NC_FLOAT, (char *) &datatype,
          "Convert to single-precision floating-point data"},
      {"-double", ARGV_CONSTANT, (char *) NC_DOUBLE, (char *) &datatype,
          "Convert to double-precision floating-point data"},
      {"-signed", ARGV_CONSTANT, (char *) TRUE, (char *) &is_signed,
          "Convert to signed integer data"},
      {"-unsigned", ARGV_CONSTANT, (char *) FALSE, (char *) &is_signed,
          "Convert to unsigned integer data"},
      {"-valid_range", ARGV_FLOAT, (char *) 2, (char *) valid_range,
          "Valid range for output data (pixel values)"},

      {NULL, ARGV_HELP, (char *) NULL, (char *) NULL, 
          "Concatenation options:"},
      {"-concat_dimension", ARGV_STRING, (char *) 1, (char *) &dimension_name,
          "Concatenate along a given dimension."},
      {"-start", ARGV_FLOAT, (char *) 1, (char *) &dimension_start,
          "Starting coordinate for new dimension."},
      {"-step", ARGV_FLOAT, (char *) 1, (char *) &dimension_step,
          "Step size for new dimension."},
      {"-width", ARGV_FLOAT, (char *) 1, (char *) &dimension_width,
          "Sample width for new dimension."},
      {"-coordlist", ARGV_FUNC, (char *) get_double_list, 
          (char *) &dimension_coords,
          "Specify the dimension coordinates (\"<c1>,<c2>,...\")."},
      {"-widthlist", ARGV_FUNC, (char *) get_double_list, 
          (char *) &dimension_widths,
          "Specify the dimension widths (\"<w1>,<w2>,...\")."},
      {"-check_dimensions", ARGV_CONSTANT, (char *) TRUE, 
          (char *) &check_dim_info,
          "Check that files have matching dimensions (default)."},
      {"-nocheck_dimensions", ARGV_CONSTANT, (char *) FALSE, 
          (char *) &check_dim_info,
          "Do not check that files have matching dimensions."},
      {"-ascending", ARGV_CONSTANT, (char *) TRUE, 
          (char *) &Sort_ascending,
          "Sort coordinates in ascending order (default)."},
      {"-descending", ARGV_CONSTANT, (char *) FALSE, 
          (char *) &Sort_ascending,
          "Sort coordinates in descending order."},
      {"-interleaved", ARGV_CONSTANT, (char *) FALSE, 
          (char *) &Sort_sequential,
          "Sort coordinates in coordinate order, interleaving if necessary (default)."},
      {"-sequential", ARGV_CONSTANT, (char *) TRUE, 
          (char *) &Sort_sequential,
          "Sort coordinates in sequential file order."},

      {NULL, ARGV_END, NULL, NULL, NULL}
   };

   /* Local variables */
   char *output_file;
   char *history;
   char *pname;
   char **infiles;
   int nfiles;
   int ifile;

   /* Get the history information and program name */
   history = time_stamp(argc, argv);
   pname = argv[0];

   /* Call ParseArgv */
   if (ParseArgv(&argc, argv, argTable, 0) || (argc < 2)) {
      (void) fprintf(stderr, 
         "\nUsage: %s [<options>] [<infile1> ...] <outfile>\n", 
                     pname);
      (void) fprintf(stderr,   
           "       %s [-help]\n\n", pname);
      exit(EXIT_FAILURE);
   }
   output_file = argv[argc-1];

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

   /* Save the input file names */
   *num_input_files = nfiles;
   *input_files = infiles;

   /* Check that a dimension name was given if coords are specified */
   concat_info->coords_specified =
      ((dimension_start != DBL_MAX) || (dimension_step != DBL_MAX) ||
       (dimension_width != DBL_MAX) || (dimension_coords.numvalues > 0) ||
       (dimension_widths.numvalues > 0));
   if ((dimension_name == NULL) && concat_info->coords_specified) {
      (void) fprintf(stderr, 
   "A dimension name must be specified if dimension coords are given.\n");
      exit(EXIT_FAILURE);
   }

   /* Check that step and start are given */
   if (((dimension_start != DBL_MAX) && (dimension_step == DBL_MAX)) ||
       ((dimension_start == DBL_MAX) && (dimension_step != DBL_MAX))) {
      (void) fprintf(stderr, "Both dimension start and step must be given.\n");
      exit(EXIT_FAILURE);
   }

   /* Check for either regular or irregular spacing of coords */
   if ((dimension_step != DBL_MAX) && (dimension_coords.numvalues > 0)) {
      (void) fprintf(stderr, "Specify either dimension step or coords.\n");
      exit(EXIT_FAILURE);
   }

   /* Check for either regular or irregular widths */
   if ((dimension_width != DBL_MAX) && (dimension_widths.numvalues > 0)) {
      (void) fprintf(stderr, 
                     "Specify either dimension width or width list.\n");
      exit(EXIT_FAILURE);
   }

   /* Check that we have the same number of coords and widths */
   if ((dimension_coords.numvalues > 0) && (dimension_widths.numvalues > 0) &&
       (dimension_coords.numvalues != dimension_widths.numvalues)) {
      (void) fprintf(stderr,
                     "Specify the same number of coordinates and widths.\n");
      exit(EXIT_FAILURE);
   }

   /* Check that we have the same number of coordinates and files */
   if ((dimension_coords.numvalues > 0) && 
       (dimension_coords.numvalues != *num_input_files)) {
      (void) fprintf(stderr,
         "Number of coordinates does not match number of input files.\n");
      exit(EXIT_FAILURE);
   }

   /* Set defaults for start and step */
   if (dimension_start == DBL_MAX) dimension_start = 0;
   if (dimension_step == DBL_MAX) dimension_step = 1;

   /* Save the appropriate values in the concat_info structure */
   concat_info->output_file = output_file;
   concat_info->num_input_files = *num_input_files;
   concat_info->history = history;
   concat_info->clobber = clobber;
   concat_info->verbose = verbose;
   concat_info->max_memory_use_in_kb = max_chunk_size_in_kb;
   concat_info->check_dim_info = check_dim_info;
   concat_info->output_datatype = datatype;
   concat_info->output_is_signed = is_signed;
   concat_info->output_valid_range[0] = valid_range[0];
   concat_info->output_valid_range[1] = valid_range[1];
   if (dimension_name != NULL)
      concat_info->dimension_name = strdup(dimension_name);
   else
      concat_info->dimension_name = NULL;
   concat_info->output_mincid = MI_ERROR;
   concat_info->output_icvid = MI_ERROR;

   /* Fill in coordinate info. We allocate space even if coordinates
      aren't specified just in case we can't get the info from the 
      files. */
   concat_info->num_file_coords = MALLOC(sizeof(int) * (*num_input_files));
   concat_info->file_coords = MALLOC(sizeof(void *) * (*num_input_files));
   concat_info->file_widths = MALLOC(sizeof(void *) * (*num_input_files));
   for (ifile=0; ifile < *num_input_files; ifile++) {
      concat_info->num_file_coords[ifile] = 1;
      concat_info->file_coords[ifile] = MALLOC(sizeof(double));
      concat_info->file_coords[ifile][0] =
         ((dimension_coords.numvalues > 0) ?
          dimension_coords.values[ifile] : 
          dimension_start + dimension_step * ifile);
      if ((dimension_widths.numvalues > 0) || (dimension_width != DBL_MAX)) {
         concat_info->file_widths[ifile] = MALLOC(sizeof(double));
         concat_info->file_widths[ifile][0] =
            ((dimension_widths.numvalues > 0) ?
             dimension_widths.values[ifile] : dimension_width);
      }
      else
         concat_info->file_widths[ifile] = NULL;
   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_double_list
@INPUT      : dst - client data passed by ParseArgv
              key - matching key in argv
              nextarg - argument following key in argv
@OUTPUT     : (none)
@RETURNS    : TRUE since nextarg is used.
@DESCRIPTION: Gets a list (array) of double values.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : March 8, 1995 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int get_double_list(char *dst, char *key, char *nextarg)
{
#define VECTOR_SEPARATOR ','

   int num_elements;
   int num_alloc;
   double *double_list;
   double dvalue;
   char *cur, *end, *prev;
   Double_Array *double_array;

   /* Check for a following argument */
   if (nextarg == NULL) {
      (void) fprintf(stderr, 
                     "\"%s\" option requires an additional argument\n",
                     key);
      exit(EXIT_FAILURE);
   }

   /* Get pointers to array variables */
   double_array = (Double_Array *) dst;

   /* Set up pointers to end of string and first non-space character */
   end = nextarg + strlen(nextarg);
   cur = nextarg;
   while (isspace(*cur)) cur++;
   num_elements = 0;
   num_alloc = 0;
   double_list = NULL;

   /* Loop through string looking for doubles */
   while (cur!=end) {

      /* Get double */
      prev = cur;
      dvalue = strtod(prev, &cur);
      if (cur == prev) {
         (void) fprintf(stderr, 
            "expected vector of doubles for \"%s\", but got \"%s\"\n", 
                        key, nextarg);
         exit(EXIT_FAILURE);
      }

      /* Add the value to the list */
      num_elements++;
      if (num_elements > num_alloc) {
         num_alloc += 20;
         if (double_list == NULL) {
            double_list = 
               MALLOC(num_alloc * sizeof(*double_list));
         }
         else {
            double_list = 
               REALLOC(double_list, num_alloc * sizeof(*double_list));
         }
      }
      double_list[num_elements-1] = dvalue;

      /* Skip any spaces */
      while (isspace(*cur)) cur++;

      /* Skip an optional comma */
      if (*cur == VECTOR_SEPARATOR) cur++;

   }

   /* Update the global variables */
   double_array->numvalues = num_elements;
   if (double_array->values != NULL) {
      FREE(double_array->values);
   }
   double_array->values = double_list;

   return TRUE;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_concat_dim_name
@INPUT      : concat_info - concatenation information
              first_filename - name of first input file
@OUTPUT     : first_mincid - mincid for first input file
@RETURNS    : (nothing)
@DESCRIPTION: Routine to get the name of the concatenation dimension, if
              necessary.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : March 16, 1995 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void get_concat_dim_name(Concat_Info *concat_info,
                                char *first_filename, int *first_mincid)
{
   char *filename;
   int created_tempfile;
   int input_mincid, imgid, dimid;
   int ndims, dim[MAX_VAR_DIMS], min_ndims;
   char dimname[MAX_NC_NAME];

   /* Expand the file header and open the file */
   filename = miexpand_file(first_filename, NULL, TRUE, &created_tempfile);
   input_mincid = miopen(filename, NC_NOWRITE);
   if (created_tempfile) {
      (void) remove(filename);
   }
   FREE(filename);
   *first_mincid = input_mincid;

   /* Do we have to get the dimension name from the file? */
   if (concat_info->dimension_name == NULL) {
      concat_info->dimension_in_input_file = TRUE;
      imgid = ncvarid(input_mincid, MIimage);
      (void) ncvarinq(input_mincid, imgid, NULL, NULL, &ndims, dim, NULL);
      min_ndims = 3;
      if (ndims > 0) {
         (void) ncdiminq(input_mincid, dim[ndims-1], dimname, NULL);
         if (strcmp(dimname, MIvector_dimension) == 0) min_ndims++;
      }
      if (ndims < min_ndims) {
         (void) fprintf(stderr, 
                        "Cannot concatentate along image dimensions.\n");
         exit(EXIT_FAILURE);
      }
      (void) ncdiminq(input_mincid, dim[0], dimname, NULL);
      concat_info->dimension_name = strdup(dimname);
   }

   /* If the dimension name is given, then check the file */
   else {
      dimid = get_image_dimension_id(input_mincid, 
                                     concat_info->dimension_name);
      concat_info->dimension_in_input_file = (dimid != MI_ERROR);
   }

   /* If the dimension is in the file, then check that no coordinate
      info was given on the command line */
   if (concat_info->dimension_in_input_file && 
       concat_info->coords_specified) {
      (void) fprintf(stderr, 
   "Do not specify coordinates for a dimension in the input files.\n");
      exit(EXIT_SUCCESS);
   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_input_file_info
@INPUT      : caller_data - pointer to concat_info structure
              input_mincid - id of input minc file
              input_curfile - number of current input file
              loop_info - pointer to structure containing loop information
@OUTPUT     : caller_data - updated concat_info structure
@RETURNS    : (nothing)
@DESCRIPTION: Routine to get information from each input file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : March 9, 1995 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void get_input_file_info(void *caller_data, int input_mincid,
                                int input_curfile, Loop_Info *loop_info)
{
   Concat_Info *concat_info;
   int varid, dimid;
   int ndims, dim[MAX_VAR_DIMS];
   char dimname[MAX_NC_NAME], string[MI_MAX_ATTSTR_LEN];
   long dimlength;
   int regular;
   double dimstart, dimstep, dimwidth;
   long index;

   /* Get the concat_info structure pointer */
   concat_info = (Concat_Info *) caller_data;

   /* Check that the concatenation dimension is present */
   dimid = get_image_dimension_id(input_mincid, 
                                  concat_info->dimension_name);
   if ((concat_info->dimension_in_input_file && (dimid == MI_ERROR)) ||
       (!concat_info->dimension_in_input_file && (dimid != MI_ERROR))) {
      (void) fprintf(stderr, 
                     "Concatenation dimension is not present in all files.\n");
      exit(EXIT_FAILURE);
   }

   /* Get information on the concatenation dimension coordinates */

   if (concat_info->dimension_in_input_file) {

      /* Allocate the arrays */
      (void) ncdiminq(input_mincid, dimid, dimname, &dimlength);
      concat_info->num_file_coords[input_curfile] = dimlength;
      FREE(concat_info->file_coords[input_curfile]);
      concat_info->file_coords[input_curfile] =
         MALLOC(sizeof(double) * dimlength);

      /* Set defaults */
      if (!Sort_sequential || (input_curfile < 1)) {
         dimstart = 0;
      }
      else {
         index = concat_info->num_file_coords[input_curfile-1] - 1;
         dimstart = concat_info->file_coords[input_curfile-1][index] + 1;
      }
      dimstep = 1;
      regular = TRUE;

      /* Look for dimension variable */
      ncopts = 0;
      varid = ncvarid(input_mincid, dimname);
      if (!Sort_sequential && (varid != MI_ERROR)) {

         /* Find out if its regular or irregular */
         (void) ncvarinq(input_mincid, varid, NULL, NULL, &ndims, dim, NULL);
         regular = !((ndims == 1) && (dim[0] = dimid));
         if (miattgetstr(input_mincid, varid, MIspacing, sizeof(string),
                         string) != NULL) {
            if (strcmp(string, MI_IRREGULAR) == 0)
               regular = FALSE;
            else if (strcmp(string, MI_REGULAR) == 0)
               regular = TRUE;
         }

         /* Expand whole file if irregular dimension */
         if (!regular) {
            ncopts = NC_OPTS_VAL;
            input_mincid = get_info_whole_file(loop_info);
            ncopts = 0;
         }
         /* Otherwise just get start and step */
         else {
            (void) miattget1(input_mincid, varid, MIstart, NC_DOUBLE, 
                             &dimstart);
            (void) miattget1(input_mincid, varid, MIstep, NC_DOUBLE, 
                             &dimstep);
         }
      }       /* If dimension variable exists */

      /* Loop through indices, getting values */
      for (index=0; index < dimlength; index++) {
         concat_info->file_coords[input_curfile][index] =
            dimstart + dimstep * index;
         if (!regular) {
            (void) mivarget1(input_mincid, varid, &index, NC_DOUBLE, NULL,
                             &concat_info->file_coords[input_curfile][index]);
         }
      }

      /* Look for dimension width variable */
      (void) strcat(dimname, DIM_WIDTH_SUFFIX);
      varid = ncvarid(input_mincid, dimname);
      if (varid != MI_ERROR) {

         /* Set default width */
         dimwidth = dimstep;

         /* Find out if its regular or irregular */
         (void) ncvarinq(input_mincid, varid, NULL, NULL, &ndims, dim, NULL);
         regular = !((ndims == 1) && (dim[0] = dimid));
         if (miattgetstr(input_mincid, varid, MIspacing, sizeof(string),
                         string) != NULL) {
            if (strcmp(string, MI_IRREGULAR) == 0)
               regular = FALSE;
            else if (strcmp(string, MI_REGULAR) == 0)
               regular = TRUE;
         }

         /* Expand whole file if irregular dimension */
         if (!regular) {
            ncopts = NC_OPTS_VAL;
            input_mincid = get_info_whole_file(loop_info);
            ncopts = 0;
         }
         /* Otherwise just get width */
         else {
            (void) miattget1(input_mincid, varid, MIwidth, NC_DOUBLE, 
                             &dimwidth);
         }

         /* Allocate space for widths */
         concat_info->file_widths[input_curfile] =
            MALLOC(sizeof(double) * dimlength);

         /* Loop through indices, getting values */
         for (index=0; index < dimlength; index++) {
            concat_info->file_widths[input_curfile][index] = dimwidth;
            if (!regular) {
               (void) mivarget1(input_mincid, varid, &index, NC_DOUBLE, NULL,
                  &concat_info->file_widths[input_curfile][index]);
            }
         }
         
      }            /* If dimension width variable exists */

      ncopts = NC_OPTS_VAL;
   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_image_dimension_id
@INPUT      : input_mincid - id of input minc file
              dimension_name - name of dimension
@OUTPUT     : (none)
@RETURNS    : id of dimension in image variable or MI_ERROR if dimension
              does not subscript the image.
@DESCRIPTION: Routine to get the id of a dimension in a minc file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : March 9, 1995 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int get_image_dimension_id(int input_mincid, char *dimension_name)
{
   int dimid, ndims, dim[MAX_VAR_DIMS], idim;
   int found;

   ncopts = 0;
   dimid = ncdimid(input_mincid, dimension_name);
   ncopts = NC_OPTS_VAL;
   if (dimid == MI_ERROR) return MI_ERROR;

   /* Get image variable info */
   (void) ncvarinq(input_mincid, ncvarid(input_mincid, MIimage), NULL, NULL, 
                   &ndims, dim, NULL);

   /* Check to see if the dimension subscripts the image */
   found = FALSE;
   for (idim=0; idim < ndims; idim++) {
      if (dimid == dim[idim]) found = TRUE;
   }

   /* Return appropriate value */
   if (found) return dimid;
   else return MI_ERROR;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : do_concat
@INPUT      : caller_data - pointer to concat_info structure
              num_voxels - number of voxels to handle
              input_num_buffers - number of input buffers
              input_vector_length - number of input vector elements to handle
              input_data - array of pointers to input buffers
              output_num_buffers - number of output buffers
              output_vector_length - number of output vector elements to handle
              loop_info - pointer to loop info structure
@OUTPUT     : output_data - array of pointers to output buffers
@RETURNS    : (nothing)
@DESCRIPTION: Routine to do the concatenation work
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : March 9, 1995 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void do_concat(void *caller_data, long num_voxels, 
                      int input_num_buffers, int input_vector_length,
                      double *input_data[],
                      int output_num_buffers, int output_vector_length,
                      double *output_data[],
                      Loop_Info *loop_info)
     /* ARGSUSED */
{
   Concat_Info *concat_info;
   int input_mincid, output_mincid, outimgid, inimgid, varid, invarid;
   int ifile;
   int icoord;
   long mindex;
   char dimname[MAX_NC_NAME];
   long instart[MAX_VAR_DIMS], incount[MAX_VAR_DIMS];
   long outstart[MAX_VAR_DIMS], outcount[MAX_VAR_DIMS];
   long mmstart[MAX_VAR_DIMS];
   int inndims, indim[MAX_VAR_DIMS], dimid;
   int idim, odim, imm;
   char *varname;
   double value;

   /* Check that the arguments are as expected */
   if ((input_num_buffers != 1) || (output_num_buffers != 0)) {
      (void) fprintf(stderr, "Unexpected arguments to do_concat!!!\n");
      exit(EXIT_FAILURE);
   }

   /* Cast caller_data into concat_info pointer */
   concat_info = (Concat_Info *) caller_data;

   /* Get the mincid for the current input file */
   input_mincid = get_info_current_mincid(loop_info);
   ifile = get_info_current_file(loop_info);
   icoord = get_info_current_index(loop_info);
   inimgid = ncvarid(input_mincid, MIimage);

   /* Check if output file has been created */
   if (concat_info->output_mincid == MI_ERROR) {

      /* Sort the coords */
      sort_coords(concat_info);

      /* Create the output file */
      create_concat_file(input_mincid, concat_info);
   }

   /* Get output file info */
   output_mincid = concat_info->output_mincid;
   mindex = concat_info->file_to_dim_order[ifile][icoord];
   outimgid = ncvarid(output_mincid, MIimage);

   /* Write out the coordinates info */
   varid = ncvarid(output_mincid, concat_info->dimension_name);
   (void) mivarput1(output_mincid, varid, &mindex, NC_DOUBLE, NULL,
                    &concat_info->file_coords[ifile][icoord]);
   if (concat_info->have_widths) {
      (void) strcat(strcpy(dimname, concat_info->dimension_name), 
                    DIM_WIDTH_SUFFIX);
      varid = ncvarid(output_mincid, dimname);
      (void) mivarput1(output_mincid, varid, &mindex, NC_DOUBLE, NULL,
                       &concat_info->file_widths[ifile][icoord]);
   }

   /* Convert the input shape info into output shape info */
   get_info_shape(loop_info, MAX_VAR_DIMS, instart, incount);
   outstart[0] = mindex;
   outcount[0] = 1;
   odim = 1;
   (void) ncvarinq(input_mincid, inimgid, NULL, NULL, &inndims, indim, NULL);
   if (concat_info->dimension_in_input_file)
      dimid = ncdimid(input_mincid, concat_info->dimension_name);
   else
      dimid = MI_ERROR;
   for (idim=0; idim < inndims; idim++) {
      if (indim[idim] != dimid) {
         outstart[odim] = instart[idim];
         outcount[odim] = incount[idim];
         odim++;
      }
   }

   /* Copy the image max and min info from the input file */
   for (imm=0; imm < 2; imm++) {
      if (imm == 0) {
         varname = MIimagemin;
         value = 0.0;
      }
      else {
         varname = MIimagemax;
         value = 1.0;
      }
      ncopts = 0;
      invarid = ncvarid(input_mincid, varname);
      ncopts = NC_OPTS_VAL;
      if (invarid != MI_ERROR) {
         (void) mitranslate_coords(input_mincid, inimgid, instart,
                                   invarid, mmstart);
         (void) mivarget1(input_mincid, invarid, mmstart, NC_DOUBLE, NULL, 
                          &value);
      }
      varid = ncvarid(output_mincid, varname);
      (void) mitranslate_coords(output_mincid, outimgid, outstart,
                                varid, mmstart);
      (void) mivarput1(output_mincid, varid, mmstart, NC_DOUBLE, NULL, 
                       &value);

      /* Save global min and max */
      if (value < concat_info->global_minimum) 
         concat_info->global_minimum = value;
      if (value > concat_info->global_maximum) 
         concat_info->global_maximum = value;

   }

   /* Copy the data */
   (void) miicv_put(concat_info->output_icvid, outstart, outcount, 
                    input_data[0]);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : sort_coords
@INPUT      : concat_info - pointer to structure containing concat info
@OUTPUT     : concat_info - ordering information initialized
                 (file_to_dim_order)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to do sort the coordinates and store the appropriate
              information.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : March 9, 1995 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void sort_coords(Concat_Info *concat_info)
{
   int ifile;
   Sort_Element *sort_list;
   int concat_dimension_length;
   int index, icoord;
   double mean_step, mean_width, diff;
   int regular_spacing, constant_width, have_widths;

   /* Allocate space for the ordering information */
   concat_info->file_to_dim_order = 
      MALLOC(sizeof(void *) * (concat_info->num_input_files));
   concat_dimension_length = 0;
   for (ifile=0; ifile < concat_info->num_input_files; ifile++) {
      concat_dimension_length += concat_info->num_file_coords[ifile];
      concat_info->file_to_dim_order[ifile] = 
         MALLOC(sizeof(int) * concat_info->num_file_coords[ifile]);
   }
   concat_info->concat_dimension_length = concat_dimension_length;

   /* Set up sorting stuff */
   sort_list = MALLOC(sizeof(Sort_Element) * concat_dimension_length);
   index = 0;
   for (ifile=0; ifile < concat_info->num_input_files; ifile++) {
      for (icoord=0; icoord < concat_info->num_file_coords[ifile]; icoord++) {
         sort_list[index].coord = concat_info->file_coords[ifile][icoord];
         sort_list[index].curfile = ifile;
         sort_list[index].curcoord = icoord;
         index++;
      }
   }

   /* Do sort */
   qsort(sort_list, (size_t) concat_dimension_length, sizeof(Sort_Element), 
         sort_function);

   /* Store results of sort and figure out whether we have a regular 
      spacing and a constant width */
   if (concat_dimension_length > 1) {
      mean_step = 
         (sort_list[concat_dimension_length-1].coord - sort_list[0].coord) /
            (double) (concat_dimension_length - 1);
   }
   else {
      mean_step = 1;
   }
   if (concat_info->file_widths[0] != NULL)
      mean_width = concat_info->file_widths[0][0];
   regular_spacing = TRUE;
   constant_width = TRUE;
   have_widths = TRUE;
   for (index=0; index < concat_dimension_length; index++) {
      ifile = sort_list[index].curfile;
      icoord = sort_list[index].curcoord;
      if ((ifile < 0) || (ifile >= concat_info->num_input_files) ||
          (icoord < 0) || (icoord >= concat_info->num_file_coords[ifile])) {
         (void) fprintf(stderr, "Serious sorting bug!!!!\n");
         exit(EXIT_FAILURE);
      }
      concat_info->file_to_dim_order[ifile][icoord] = index;
      if (concat_info->file_widths[ifile] == NULL)
         have_widths = FALSE;
      if (index > 0) {
         diff = (sort_list[index].coord - sort_list[index-1].coord)
            - mean_step;
         if (mean_step != 0) diff = diff / mean_step;
         diff = ABS(diff);
         if (diff > COORD_EPSILON) regular_spacing = FALSE;
         if (have_widths) {
            diff = (concat_info->file_widths[ifile][icoord] - mean_width);
            if (mean_width != 0) diff = diff / mean_width;
            diff = ABS(diff);
            if (diff > COORD_EPSILON) constant_width = FALSE;
         }
      }
   }
   concat_info->regular_spacing = regular_spacing;
   concat_info->constant_width = constant_width;
   concat_info->have_widths = have_widths;
   concat_info->dim_step = mean_step;
   concat_info->dim_start = sort_list[0].coord;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : sort_function
@INPUT      : value1, value2 - two values to compare
@OUTPUT     : (none)
@RETURNS    : -1, 0, 1 for value1 <, ==, > value2
@DESCRIPTION: Routine to do do comparisons of Sort_Elements for qsort.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : March 9, 1995 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int sort_function(const void *value1, const void *value2)
{
   Sort_Element *element1, *element2;
   int return_value;

   element1 = (Sort_Element *) value1;
   element2 = (Sort_Element *) value2;

   if (element1->coord < element2->coord) return_value = -1;
   else if (element1->coord > element2->coord) return_value = 1;
   else if (element1->curfile < element2->curfile) return_value = -1;
   else if (element1->curfile > element2->curfile) return_value = 1;
   else if (element1->curcoord < element2->curcoord) return_value = -1;
   else if (element1->curcoord > element2->curcoord) return_value = 1;
   else return_value = 0;

   if (!Sort_ascending)
      return_value *= -1;

   return return_value;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_concat_file
@INPUT      : inmincid - id of sample input file
              concat_info - pointer to structure containing concat info
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to create the output concatentated minc file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : March 9, 1995 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void create_concat_file(int inmincid, Concat_Info *concat_info)
{
   int outmincid, outimgid, inimgid, coordid, widthid, invarid;
   int maxid, minid, icvid;
   int in_ndims, indim[MAX_VAR_DIMS], idim;
   int out_ndims, outdim[MAX_VAR_DIMS], out_nimgdims;
   int nexcluded, excluded_vars[20];
   nc_type datatype;
   char dimname[MAX_NC_NAME];
   long dimlength;
   double valid_range[2];

   /* Create the file */
   outmincid = micreate(concat_info->output_file,
                        (concat_info->clobber ? NC_CLOBBER : NC_NOCLOBBER));

   /* Get image variable id for input file */
   inimgid = ncvarid(inmincid, MIimage);

   /* Get the list of dimensions subscripting the image variable */
   (void) ncvarinq(inmincid, inimgid, NULL, &datatype, 
                   &in_ndims, indim, NULL);

   /* Figure out how many image dimensions we have */
   out_nimgdims = 2;
   if (in_ndims < out_nimgdims) {
      (void) fprintf(stderr, "Not enough dimensions in input file.\n");
      exit(EXIT_FAILURE);
   }
   (void) ncdiminq(inmincid, indim[in_ndims-1], dimname, NULL);
   if (strcmp(dimname, MIvector_dimension) == 0)
      out_nimgdims++;

   /* Set up the output minc file */

   /* Create the concatenation dimension */
   out_ndims = 0;
   outdim[out_ndims] = ncdimdef(outmincid, concat_info->dimension_name,
                                (long) concat_info->concat_dimension_length);
   ncopts = 0;
   coordid = micreate_std_variable(outmincid, concat_info->dimension_name,
                                   NC_DOUBLE, 1, &outdim[out_ndims]);
   ncopts = NC_OPTS_VAL;
   if (coordid == MI_ERROR)
      coordid = ncvardef(outmincid, concat_info->dimension_name,
                         NC_DOUBLE, 1, &outdim[out_ndims]);
   ncopts = 0;
   invarid = ncvarid(inmincid, concat_info->dimension_name);
   if (invarid != MI_ERROR)
      (void) micopy_all_atts(inmincid, invarid, outmincid, coordid);
   ncopts = NC_OPTS_VAL;
   (void) miattputstr(outmincid, coordid, MIspacing,
                      (concat_info->regular_spacing ?
                       MI_REGULAR : MI_IRREGULAR));
   (void) miattputdbl(outmincid, coordid, MIstart, concat_info->dim_start);
   (void) miattputdbl(outmincid, coordid, MIstep, concat_info->dim_step);
   if (concat_info->have_widths) {
      (void) strcat(strcpy(dimname, concat_info->dimension_name), 
                    DIM_WIDTH_SUFFIX);
      ncopts = 0;
      widthid = micreate_std_variable(outmincid, dimname,
                                      NC_DOUBLE, 1, &outdim[out_ndims]);
      ncopts = NC_OPTS_VAL;
      if (widthid == MI_ERROR) 
         widthid = ncvardef(outmincid, dimname,
                            NC_DOUBLE, 1, &outdim[out_ndims]);
      ncopts = 0;
      invarid = ncvarid(inmincid, dimname);
      if (invarid != MI_ERROR)
         (void) micopy_all_atts(inmincid, invarid, outmincid, widthid);
      (void) ncattdel(outmincid, widthid, MIwidth);
      ncopts = NC_OPTS_VAL;
      (void) miattputstr(outmincid, widthid, MIspacing,
                         (concat_info->constant_width ?
                          MI_REGULAR : MI_IRREGULAR));
      if (concat_info->constant_width)
         (void) miattputdbl(outmincid, widthid, MIwidth, 
                            concat_info->file_widths[0][0]);
   }
   out_ndims++;

   /* Loop, creating output dimensions */
   for (idim=0; idim < in_ndims; idim++) {

      /* Get the dimension info from the input file */
      (void) ncdiminq(inmincid, indim[idim], dimname, &dimlength);

      /* Check that this is not the concat dimension */
      if (strcmp(dimname, concat_info->dimension_name) != 0) {

         /* Copy the dimension */
         outdim[out_ndims] = ncdimdef(outmincid, dimname, dimlength);
         out_ndims++;
      }
   }

   /* Copy other variables in file */
   nexcluded = 0;
   ncopts = 0;
   excluded_vars[nexcluded] = inimgid;
   if (excluded_vars[nexcluded] != MI_ERROR) nexcluded++;
   excluded_vars[nexcluded] = ncvarid(inmincid, MIimagemax);
   if (excluded_vars[nexcluded] != MI_ERROR) nexcluded++;
   excluded_vars[nexcluded] = ncvarid(inmincid, MIimagemin);
   if (excluded_vars[nexcluded] != MI_ERROR) nexcluded++;
   (void) strcpy(dimname, concat_info->dimension_name);
   excluded_vars[nexcluded] = ncvarid(inmincid, dimname);
   if (excluded_vars[nexcluded] != MI_ERROR) nexcluded++;
   (void) strcat(dimname, DIM_WIDTH_SUFFIX);
   excluded_vars[nexcluded] = ncvarid(inmincid, dimname);
   if (excluded_vars[nexcluded] != MI_ERROR) nexcluded++;

   (void) micopy_all_var_defs(inmincid, outmincid, nexcluded, excluded_vars);
   ncopts = NC_OPTS_VAL;

   /* Add the time stamp to the history */
   update_history(outmincid, concat_info->history);
 
   /* Create the image and image-min/max variables */
   if (concat_info->output_datatype != MI_ORIGINAL_TYPE) {
      datatype = concat_info->output_datatype;
   }
   concat_info->is_floating_type = 
      ((datatype == NC_FLOAT) || (datatype == NC_DOUBLE));
   outimgid = micreate_std_variable(outmincid, MIimage, datatype, 
                                    out_ndims, outdim);
   (void) micopy_all_atts(inmincid, inimgid, outmincid, outimgid);
   if (concat_info->is_floating_type) {
      ncopts = 0;
      (void) ncattdel(outmincid, outimgid, MIsigntype);
      ncopts = NC_OPTS_VAL;
      valid_range[0] = 0;
      valid_range[1] = 1;
      (void) miset_valid_range(outmincid, outimgid,  
                               valid_range);
   }
   if (concat_info->output_datatype != MI_ORIGINAL_TYPE) {
      if (concat_info->output_is_signed)
         (void) miattputstr(outmincid, outimgid, MIsigntype, MI_SIGNED);
      else
         (void) miattputstr(outmincid, outimgid, MIsigntype, MI_UNSIGNED);
      if (concat_info->output_valid_range[1] > 
          concat_info->output_valid_range[0]) {
         (void) miset_valid_range(outmincid, outimgid,
                                  concat_info->output_valid_range);
      }
      else {
         ncopts = 0;
         (void) ncattdel(outmincid, outimgid, MIvalid_range);
         ncopts = NC_OPTS_VAL;
      }
   }
   (void) miattputstr(outmincid, outimgid, MIcomplete, MI_FALSE);
   maxid = micreate_std_variable(outmincid, MIimagemax, NC_DOUBLE, 
                                 out_ndims-out_nimgdims, outdim);
   minid = micreate_std_variable(outmincid, MIimagemin, NC_DOUBLE, 
                                 out_ndims-out_nimgdims, outdim);
   ncopts = 0;
   (void) micopy_all_atts(inmincid, ncvarid(inmincid, MIimagemax),
                          outmincid, maxid);
   (void) micopy_all_atts(inmincid, ncvarid(inmincid, MIimagemin),
                          outmincid, minid);
   ncopts = NC_OPTS_VAL;

   /* Put the file in data mode */
   (void) ncsetfill(outmincid, NC_NOFILL);
   (void) ncendef(outmincid);

   /* Copy over variable values */
   ncopts = 0;
   (void) micopy_all_var_values(inmincid, outmincid,
                                nexcluded, excluded_vars);
   ncopts = NC_OPTS_VAL;

   /* Create the icv and attach it */
   icvid = miicv_create();
   (void) miicv_setint(icvid, MI_ICV_TYPE, NC_DOUBLE);
   (void) miicv_setint(icvid, MI_ICV_DO_NORM, TRUE);
   (void) miicv_setint(icvid, MI_ICV_USER_NORM, TRUE);
   (void) miicv_attach(icvid, outmincid, outimgid);

   /* Return the file and icv id's */
   concat_info->output_mincid = outmincid;
   concat_info->output_icvid = icvid;
   


}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : update_history
@INPUT      : mincid - id of output minc file
              arg_string - string giving list of arguments
@OUTPUT     : (nothing)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to update the history global variable in the output 
              minc file
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 26, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void update_history(int mincid, char *arg_string)
{
   nc_type datatype;
   int att_length;
   char *string;

   /* Get the history attribute length */
   ncopts=0;
   if ((ncattinq(mincid, NC_GLOBAL, MIhistory, &datatype,
                 &att_length) == MI_ERROR) ||
       (datatype != NC_CHAR))
      att_length = 0;
   att_length += strlen(arg_string) + 1;

   /* Allocate a string and get the old history */
   string = MALLOC(att_length);
   string[0] = '\0';
   (void) miattgetstr(mincid, NC_GLOBAL, MIhistory, att_length, 
                      string);
   ncopts = NC_OPTS_VAL;

   /* Add the new command and put the new history. */
   (void) strcat(string, arg_string);
   (void) miattputstr(mincid, NC_GLOBAL, MIhistory, string);
   FREE(string);

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

