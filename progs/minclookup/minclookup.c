/* ----------------------------- MNI Header -----------------------------------
@NAME       : minclookup
@INPUT      : argc, argv - command line arguments
@OUTPUT     : (none)
@RETURNS    : status
@DESCRIPTION: Program to perform lookup table operations on a scalar minc
              file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 6, 1994 (Peter Neelin)
@MODIFIED   : 
 * $Log: minclookup.c,v $
 * Revision 6.8  2007-12-11 12:43:01  rotor
 *  * added static to all global variables in main programs to avoid linking
 *       problems with libraries (compress in mincconvert and libz for example)
 *
 * Revision 6.7  2005/08/26 21:07:17  bert
 * Use #if rather than #ifdef with MINC2 symbol, and be sure to include config.h whereever MINC2 is used
 *
 * Revision 6.6  2004/12/03 21:57:08  bert
 * Include config.h
 *
 * Revision 6.5  2004/11/01 22:38:38  bert
 * Eliminate all references to minc_def.h
 *
 * Revision 6.4  2004/05/20 21:52:08  bert
 * Revised man pages
 *
 * Revision 6.3  2001/04/24 13:38:43  neelin
 * Replaced NC_NAT with MI_ORIGINAL_TYPE.
 *
 * Revision 6.2  2001/04/17 18:40:20  neelin
 * Modifications to work with NetCDF 3.x
 * In particular, changed NC_LONG to NC_INT (and corresponding longs to ints).
 * Changed NC_UNSPECIFIED to NC_NAT.
 * A few fixes to the configure script.
 *
 * Revision 6.1  1999/10/19 14:45:24  neelin
 * Fixed Log subsitutions for CVS
 *
 * Revision 6.0  1997/09/12 13:24:13  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:25:12  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:01:47  neelin
 * Release of minc version 0.4
 *
 * Revision 3.2  1996/07/10  16:58:37  neelin
 * Added -lut_string option and added special handling of duplicated first
 * or last entries.
 *
 * Revision 3.1  1996/07/10  14:38:03  neelin
 * Added options to set output file type, sign and range.
 *
 * Revision 3.0  1995/05/15  19:32:39  neelin
 * Release of minc version 0.3
 *
 * Revision 1.6  1995/03/21  14:26:42  neelin
 * changed usage message.
 *
 * Revision 1.5  1995/03/21  14:00:10  neelin
 * Changed calls to voxel_loop routines.
 *
 * Revision 1.4  1995/02/08  19:31:47  neelin
 * Moved ARGSUSED statements for irix 5 lint.
 *
 * Revision 1.3  1994/12/14  09:22:30  neelin
 * Removed debugging code.
 *
 * Revision 1.2  94/12/13  16:34:02  neelin
 * Added sorting of lookup file.
 * 
 * Revision 1.1  94/12/09  15:31:00  neelin
 * Initial revision
 * 
---------------------------------------------------------------------------- */

#ifndef lint
static char rcsid[]="$Header: /private-cvsroot/minc/progs/minclookup/minclookup.c,v 6.8 2007-12-11 12:43:01 rotor Exp $";
#endif

#if HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <float.h>
#include <minc.h>
#include <ParseArgv.h>
#include <time_stamp.h>
#include <voxel_loop.h>

#ifndef TRUE
#  define TRUE 1
#  define FALSE 0
#endif

#define DEFAULT_RANGE DBL_MAX
#define NCOPTS_DEFAULT NC_VERBOSE | NC_FATAL

/* Types */
typedef enum {LU_TABLE, LU_GRAY, LU_HOTMETAL, LU_SPECTRAL} Lookup_Type;

/* Lookup table structure */
typedef struct {
   int nentries;
   int vector_length;
   double *table;
   int free_data;
} Lookup_Table;

/* Structure for lookup information */
typedef struct {
   Lookup_Table *lookup_table;
   double *null_value;
   int invert;
   int discrete;
   double range[2];
} Lookup_Data;

/* Structure for sorting the lookup table */
typedef struct {
   double key;
   int index;
} Sort_Key;

/* Function prototypes */
static Lookup_Table *read_lookup_table(char *lookup_file, char *lookup_string);
static double *get_values_from_string(char *string, int array_size,
                                      double *array, int *nread);
static double *get_null_value(int vector_length, char *null_value_string);
static void get_full_range(int mincid, double lookup_range[2]);
static void do_lookup(void *caller_data, long num_voxels,
                      int input_num_buffers, int input_vector_length,
                      double *input_data[],
                      int output_num_buffers, int output_vector_length,
                      double *output_data[], Loop_Info *loop_info);
static void lookup_in_table(double index, Lookup_Table *lookup_table,
                            int discrete_values, double null_value[],
                            double output_value[]);
static char *get_next_line(char *line, int linelen, FILE *fp, char **string);
static int sorting_function(const void *value1, const void *value2);

/* Lookup tables */
static double gray_lookup_values[] = {
   0.0, 0.0, 0.0, 0.0,
   1.0, 1.0, 1.0, 1.0
};
static Lookup_Table gray_lookup = {
   sizeof(gray_lookup_values)/sizeof(gray_lookup_values[0])/4,
   3,
   gray_lookup_values,
   FALSE
};

static double spectral_lookup_values[] = {
   0.00, 0.0000,0.0000,0.0000,
   0.05, 0.4667,0.0000,0.5333,
   0.10, 0.5333,0.0000,0.6000,
   0.15, 0.0000,0.0000,0.6667,
   0.20, 0.0000,0.0000,0.8667,
   0.25, 0.0000,0.4667,0.8667,
   0.30, 0.0000,0.6000,0.8667,
   0.35, 0.0000,0.6667,0.6667,
   0.40, 0.0000,0.6667,0.5333,
   0.45, 0.0000,0.6000,0.0000,
   0.50, 0.0000,0.7333,0.0000,
   0.55, 0.0000,0.8667,0.0000,
   0.60, 0.0000,1.0000,0.0000,
   0.65, 0.7333,1.0000,0.0000,
   0.70, 0.9333,0.9333,0.0000,
   0.75, 1.0000,0.8000,0.0000,
   0.80, 1.0000,0.6000,0.0000,
   0.85, 1.0000,0.0000,0.0000,
   0.90, 0.8667,0.0000,0.0000,
   0.95, 0.8000,0.0000,0.0000,
   1.00, 0.8000,0.8000,0.8000
};
static Lookup_Table spectral_lookup = {
   sizeof(spectral_lookup_values)/sizeof(spectral_lookup_values[0])/4,
   3,
   spectral_lookup_values,
   FALSE
};

static double hotmetal_lookup_values[] = {
   0.00, 0.0, 0.0, 0.0,
   0.25, 0.5, 0.0, 0.0,
   0.50, 1.0, 0.5, 0.0,
   0.75, 1.0, 1.0, 0.5,
   1.00, 1.0, 1.0, 1.0
};
static Lookup_Table hotmetal_lookup = {
   sizeof(hotmetal_lookup_values)/sizeof(hotmetal_lookup_values[0])/4,
   3,
   hotmetal_lookup_values,
   FALSE
};

/* Argument variables */
static int clobber = FALSE;
static int verbose = TRUE;
static nc_type datatype = MI_ORIGINAL_TYPE;
static int is_signed = FALSE;
static double valid_range[2] = {0.0, 0.0};
static int buffer_size = 10 * 1024;
static char *lookup_file = NULL;
static char *lookup_string = NULL;
static Lookup_Type lookup_type = LU_GRAY;
static int invert_table = FALSE;
static double lookup_range[2] = {DEFAULT_RANGE, DEFAULT_RANGE};
static double lookup_min = DEFAULT_RANGE;
static double lookup_max = DEFAULT_RANGE;
static int discrete_lookup = FALSE;
static char *null_value_string = NULL;
#if MINC2
static int minc2_format = FALSE;
#endif /* MINC2 */

/* Argument table */
ArgvInfo argTable[] = {
#if MINC2
    {"-2", ARGV_CONSTANT, (char *) TRUE, (char *) &minc2_format,
     "Produce a MINC 2.0 format output file"},
#endif /* MINC2 */
   {"-clobber", ARGV_CONSTANT, (char *) TRUE, (char *) &clobber,
       "Overwrite existing file."},
   {"-noclobber", ARGV_CONSTANT, (char *) FALSE, (char *) &clobber,
       "Don't overwrite existing file (default)."},
   {"-verbose", ARGV_CONSTANT, (char *) TRUE, (char *) &verbose,
       "Print out log messages (default)."},
   {"-quiet", ARGV_CONSTANT, (char *) FALSE, (char *) &verbose,
       "Do not print out log messages."},
   {"-buffer_size", ARGV_INT, (char *) 1, (char *) &buffer_size,
       "Set the internal buffer size (in kb)."},
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
   {"-valid_range", ARGV_FLOAT, (char *) 2, (char *) valid_range,
       "Valid range for output data."},
   {"-gray", ARGV_CONSTANT, (char *) LU_GRAY, (char *) &lookup_type,
       "Use a grayscale lookup table (default)."},
   {"-grey", ARGV_CONSTANT, (char *) LU_GRAY, (char *) &lookup_type,
       "Use a grayscale lookup table."},
   {"-hotmetal", ARGV_CONSTANT, (char *) LU_HOTMETAL, (char *) &lookup_type,
       "Use a hot metal lookup table."},
   {"-spectral", ARGV_CONSTANT, (char *) LU_SPECTRAL, (char *) &lookup_type,
       "Use a spectral lookup table."},
   {"-invert", ARGV_CONSTANT, (char *) TRUE, (char *) &invert_table,
       "Invert the lookup table (only applies to -continuous)."},
   {"-noinvert", ARGV_CONSTANT, (char *) FALSE, (char *) &invert_table,
       "Don't invert the lookup table."},
   {"-range", ARGV_FLOAT, (char *) 2, (char *) lookup_range,
       "Min and max for lookup table (default from file)."},
   {"-minimum", ARGV_FLOAT, (char *) 1, (char *) &lookup_min,
       "Minimum for continuous lookup table."},
   {"-maximum", ARGV_FLOAT, (char *) 1, (char *) &lookup_max,
       "Maximum for continuous lookup table."},
   {"-lookup_table", ARGV_STRING, (char *) 1, (char *) &lookup_file,
       "File containing the lookup table (use - for stdin)."},
   {"-lut_string", ARGV_STRING, (char *) 1, (char *) &lookup_string,
       "String containing the lookup table, with \";\" to separate lines."},
   {"-discrete", ARGV_CONSTANT, (char *) TRUE, (char *) &discrete_lookup,
       "Lookup table has discrete (integer) entries - range is ignored."},
   {"-continuous", ARGV_CONSTANT, (char *) FALSE, (char *) &discrete_lookup,
       "Lookup table is continuous from 0 to 1 (default)."},
   {"-null_value", ARGV_STRING, (char *) 1, (char *) &null_value_string,
       "Specify a vector value for entries missing from a discrete lookup."},
   {NULL, ARGV_END, NULL, NULL, NULL}
};
/* Main program */

int main(int argc, char *argv[])
{
   char *infile, *outfile;
   char *arg_string;
   Loop_Options *loop_options;
   int inmincid;
   Lookup_Data lookup_data;

   /* Save time stamp and args */
   arg_string = time_stamp(argc, argv);

   /* Get arguments */
   if (ParseArgv(&argc, argv, argTable, 0) || (argc != 3)) {
      (void) fprintf(stderr, "\nUsage: %s [options] <in.mnc> <out.mnc>\n",
                     argv[0]);
      (void) fprintf(stderr,   "       %s -help\n\n",
                     argv[0]);
      exit(EXIT_FAILURE);
   }
   infile = argv[1];
   outfile = argv[2];

   /* Get the appropriate lookup table */
   if ((lookup_file != NULL) || (lookup_string != NULL)) {
      lookup_data.lookup_table = read_lookup_table(lookup_file, lookup_string);
   }
   else {
      switch (lookup_type) {
      case LU_GRAY:
         lookup_data.lookup_table = &gray_lookup;
         break;
      case LU_HOTMETAL:
         lookup_data.lookup_table = &hotmetal_lookup;
         break;
      case LU_SPECTRAL:
         lookup_data.lookup_table = &spectral_lookup;
         break;
      }
   }

   /* Get the null value */
   lookup_data.null_value = 
      get_null_value(lookup_data.lookup_table->vector_length, 
                     null_value_string);

   /* Open the input file and get the range */
   inmincid = miopen(infile, NC_NOWRITE);
   if (!discrete_lookup && (lookup_range[0] == DEFAULT_RANGE)) {
      get_full_range(inmincid, lookup_range);
   }
   if (lookup_min != DEFAULT_RANGE)
      lookup_range[0] = lookup_min;
   if (lookup_max != DEFAULT_RANGE)
      lookup_range[1] = lookup_max;

   /* Set up lookup information */
   lookup_data.invert = invert_table;
   lookup_data.range[0] = lookup_range[0];
   lookup_data.range[1] = lookup_range[1];
   lookup_data.discrete = discrete_lookup;

   /* Set up looping options */
   loop_options = create_loop_options();
   set_loop_clobber(loop_options, clobber);
   set_loop_verbose(loop_options, verbose);
#if MINC2
   set_loop_v2format(loop_options, minc2_format);
#endif /* MINC2 */
   set_loop_datatype(loop_options, datatype, is_signed, 
                     valid_range[0], valid_range[1]);
   set_loop_convert_input_to_scalar(loop_options, TRUE);
   set_loop_output_vector_size(loop_options, 
                               lookup_data.lookup_table->vector_length);
   set_loop_buffer_size(loop_options, (long) buffer_size * 1024);
   set_loop_first_input_mincid(loop_options, inmincid);

   /* Do loop */
   voxel_loop(1, &infile, 1, &outfile, arg_string, loop_options,
              do_lookup, (void *) &lookup_data);

   /* Free stuff */
   if (lookup_data.null_value != NULL) free(lookup_data.null_value);
   if (lookup_data.lookup_table->free_data) {
      free(lookup_data.lookup_table->table);
      free(lookup_data.lookup_table);
   }

   exit(EXIT_SUCCESS);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_lookup_table
@INPUT      : lookup_filename - name of file from which to read lookup table
                 ("-" means use stdin)
              lookup_string - string from which to read lookup table if
                 lookup_filename is NULL
@OUTPUT     : (nothing)
@RETURNS    : Pointer to lookup table
@DESCRIPTION: Routine to read in a lookup table from a file or stdin. If
              the filename is NULL, then data is read from the lookup_string.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 8, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static Lookup_Table *read_lookup_table(char *lookup_filename, 
                                       char *lookup_string)
{
   Lookup_Table *lookup_table;
   FILE *fp;
   double *table, *row, *new_table;
   int nentries, table_nvalues, nvalues, ivalue, ientry;
   char line[1000];
   Sort_Key *sort_table;
   int need_sort;
   int old_offset, new_offset;
   char *lut_string;

   /* Check for null file name */
   if ((lookup_filename == NULL) && (lookup_string == NULL)) return NULL;

   /* Open the file */
   if (lookup_filename == NULL) {
      fp = NULL;
   }
   else if (strcmp(lookup_filename, "-") == 0) {
      fp = stdin;
   }
   else {
      fp = fopen(lookup_filename, "r");
      if (fp == NULL) {
         (void) fprintf(stderr, "Unable to open lookup file \"%s\".\n",
                        lookup_filename);
         exit(EXIT_FAILURE);
      }
   }

   /* Read in the table */

   /* Read the first line and get the vector length from that*/
   nentries = 0;
   lut_string = lookup_string;
   if (get_next_line(line, sizeof(line), fp, &lut_string) == NULL) {
      if (fp != NULL)
         (void) fprintf(stderr, "Lookup file %s is empty.\n", lookup_filename);
      else
         (void) fprintf(stderr, "Lookup string is empty.\n");
      exit(EXIT_FAILURE);
   }
   row = get_values_from_string(line, 0, NULL, &table_nvalues);
   if (table_nvalues < 2) {
      (void) fprintf(stderr, "First line has fewer than 2 values.\n");
      if (row != NULL) free(row);
      exit(EXIT_FAILURE);
   }
   table = malloc(sizeof(*table) * table_nvalues);
   for (ivalue=0; ivalue < table_nvalues; ivalue++)
      table[ivalue] = row[ivalue];
   nentries++;
   need_sort = FALSE;
   while (get_next_line(line, sizeof(line), fp, &lut_string) != NULL) {
      (void) get_values_from_string(line, table_nvalues, row, &nvalues);
      if (nvalues != table_nvalues) {
         (void) fprintf(stderr, 
                        "Wrong number of values on line %d.\n",
                        nentries+1);
         free(row);
         free(table);
         exit(EXIT_FAILURE);
      }
      table = realloc(table, sizeof(*table) * table_nvalues * (nentries+1));
      for (ivalue=0; ivalue < table_nvalues; ivalue++) {
         table[ivalue + nentries*table_nvalues] = row[ivalue];
      }
      nentries++;

      /* Check for need to sort */
      if (table[(nentries-2)*table_nvalues] > 
          table[(nentries-1)*table_nvalues]) {
         need_sort = TRUE;
      }
   }

   /* Close the input file */
   if (fp != NULL) {
      (void) fclose(fp);
   }

   /* Do sorting if needed */
   if (need_sort) {

      /* Set up sorting table */
      sort_table = malloc(sizeof(*sort_table) * nentries);
      for (ientry=0; ientry < nentries; ientry++) {
         sort_table[ientry].key = table[ientry*table_nvalues];
         sort_table[ientry].index = ientry;
      }

      /* Sort the sorting table */
      qsort((void *) sort_table, nentries, sizeof(*sort_table),
            sorting_function);

      /* Copy the table */
      new_table = malloc(sizeof(*table) * table_nvalues * nentries);
      for (ientry=0; ientry < nentries; ientry++) {
         new_offset = ientry * table_nvalues;
         old_offset = sort_table[ientry].index * table_nvalues;
         for (ivalue=0; ivalue < table_nvalues; ivalue++) {
            new_table[new_offset + ivalue] = table[old_offset + ivalue];
         }
      }
      free(table);
      table = new_table;
      free(sort_table);
   }

   /* Allocate space for the lookup table and set initial values */
   lookup_table = malloc(sizeof(*lookup_table));
   lookup_table->free_data = TRUE;
   lookup_table->nentries = nentries;
   lookup_table->vector_length = table_nvalues - 1;
   lookup_table->table = table;

   /* Return the lookup table */
   return lookup_table;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_next_line
@INPUT      : linelen - length of line to read in
              fp - file pointer or NULL if data should be read from a string
              string - pointer string containing data
@OUTPUT     : line - line that has been read in
              string - pointer is advanced to character following ";"
@RETURNS    : pointer to line or NULL if end of data occurs
@DESCRIPTION: Routine to get the next line from a file or from a string
              that uses ";" as a line separator.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : July 10, 1996 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
#define LOOKUP_LINE_SEPARATOR ';'
static char *get_next_line(char *line, int linelen, FILE *fp, char **string)
{
   int count;

   /* Read from the file if appropriate */
   if (fp != NULL) {
      return fgets(line, linelen, fp);
   }

   /* Otherwise search the string for the ";", copying characters */
   else { 

      /* Check for an empty string */
      if (**string == '\0') return NULL;

      /* Loop over characters */
      count = 0;
      while ((**string != '\0') && (**string != LOOKUP_LINE_SEPARATOR)) {
         if (count < linelen-1) {
            line[count] = **string;
            count++;
         }
         (*string)++;
      }

      /* Terminate the line and move past the ";" */
      line[count] = '\0';
      if (**string != '\0')
         (*string)++;

      /* Return the line */
      return line;

   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : sorting_function
@INPUT      : value1 - pointer to first value
              value2 - pointer to second value
@OUTPUT     : (nothing)
@RETURNS    : -1, 0 or 1 for value1 <, ==, > value2
@DESCRIPTION: Routine to compare two values for sorting. The value is a
              pointer to a structure that has a double precision primary
              key. If that is equal then an integer secondary key is used.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 8, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static int sorting_function(const void *value1, const void *value2)
{
   Sort_Key *key1, *key2;

   key1 = (Sort_Key *) value1;
   key2 = (Sort_Key *) value2;

   if (key1->key == key2->key) {
      if (key1->index == key2->index) return 0;
      else if (key1->index < key2->index) return -1;
      else return 1;
   }
   else if (key1->key < key2->key) return -1;
   else return 1;
   
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_values_from_string
@INPUT      : string - string containing values
              array_size - maximum number of values (<=0 means allocate a new
                 array )
@OUTPUT     : array - array into which values are written (can be NULL if
                 array_size <= 0)
              nread - number of values read
@RETURNS    : Pointer to array of values
@DESCRIPTION: Routine to convert a string to an array of values 
              (floating point). If an existing array is passed in
              (array_size > 0), then up to array_size values are copied into 
              it. Otherwise, a new array is created. Normally, the number of
              values read is returned - if an error occurs (non-numeric string
              for example), then zero is returned.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 8, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static double *get_values_from_string(char *string, int array_size,
                                      double *array, int *nread)
{
#define VECTOR_SEPARATOR ','

   char *cur, *prev, *end;
   int num_read, num_alloc;
   double dvalue;

   /* Set up variables */
   num_read = 0;
   if (array_size <= 0) {
      num_alloc = 0;
      array = NULL;
   }
   else {
      num_alloc = array_size;
   }

   /* Skip leading white space */
   end = string + strlen(string);
   cur = string;
   while (isspace(*cur)) cur++;

   /* Loop through string looking for doubles */
   while (cur!=end) {

      /* Get double */
      prev = cur;
      dvalue = strtod(prev, &cur);
      if (cur == prev) {
         *nread = 0;
         if (array_size <= 0 && array != NULL) {
            free(array);
         }
         return NULL;
      }

      /* Add the value to the list */
      num_read++;
      if (num_read > num_alloc) {
         if (array_size <= 0) {
            num_alloc += 1;
            if (array == NULL) {
               array = malloc(num_alloc * sizeof(*array));
            }
            else {
               array = realloc(array, num_alloc * sizeof(*array));
            }
         }
         else {
            *nread = num_read;
            return array;
         }
      }
      array[num_read-1] = dvalue;

      /* Skip any spaces */
      while (isspace(*cur)) cur++;

      /* Skip an optional comma */
      if (*cur == VECTOR_SEPARATOR) cur++;

   }

   *nread = num_read;
   return array;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_null_value
@INPUT      : vector_length - desired vector length
              string - string from which we should get null values
@OUTPUT     : (nothing)
@RETURNS    : Pointer to array of values
@DESCRIPTION: Routine to convert a string to an array of values 
              to be used as a null value for the lookup table. Returns 
              NULL if string is NULL
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 8, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static double *get_null_value(int vector_length, char *string)
{
   int num_read;
   double *values;

   /* Check for NULL string */
   if (string == NULL) return NULL;

   /* Read values */
   values = get_values_from_string(string, 0, NULL, &num_read);

   /* Check the number of values read */
   if (num_read != vector_length) {
      if (values != NULL) free(values);
      (void) fprintf(stderr, 
                     "Null value does not match lookup table (%d values).\n",
                     num_read);
      exit(EXIT_FAILURE);
   }

   return values;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_full_range
@INPUT      : mincid - id of the input minc file
@OUTPUT     : range - 2-value array giving real range of input values
@RETURNS    : (nothing)
@DESCRIPTION: Routine to get the full real range of an input file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 8, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static void get_full_range(int mincid, double range[2])
{
   char *string;
   int varid;
   int ivar, idim, ndims;
   int dim[MAX_VAR_DIMS];
   double sign, *extreme, *values;
   long start[MAX_VAR_DIMS], count[MAX_VAR_DIMS], num_values;
   long ivalue, length;

   /* Loop over image-min and image-max variables */
   range[0] = 0.0;
   range[1] = 1.0;
   for (ivar=0; ivar < 2; ivar++) {
      if (ivar==0) {
         string = MIimagemin;
         sign = -1.0;
         extreme = &range[0];
      }
      else {
         string = MIimagemax;
         sign = +1.0;
         extreme = &range[1];
      }
      ncopts = 0;
      varid = ncvarid(mincid, string);
      ncopts = NCOPTS_DEFAULT;
      if (varid != MI_ERROR) {
         (void) ncvarinq(mincid, varid, NULL, NULL, &ndims, dim, NULL);
         num_values = 1;
         for (idim=0; idim < ndims; idim++) {
            (void) ncdiminq(mincid, dim[idim], NULL, &length);
            start[idim] = 0;
            count[idim] = length;
            num_values *= length;
         }
         if (num_values > 0) {
            values = malloc(num_values * sizeof(*values));
            (void) mivarget(mincid, varid, start, count, 
                            NC_DOUBLE, NULL, values);
            *extreme = values[0];
            for (ivalue=0; ivalue < num_values; ivalue++) {
               if ((values[ivalue] * sign) > (*extreme * sign))
                  *extreme = values[ivalue];
            }
            free(values);
         }

      }          /* If variable is found */

   }          /* Loop over image-min/max */
   

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : do_lookup
@INPUT      : caller_data - pointer to structure containing lookup info
              num_voxels - number of voxels to work on
              input_num_buffers - number of input buffers
              input_vector_length - length of input vector dimension
              input_data - vector of pointers to input buffer data
              output_num_buffers - number of output buffers
              output_vector_length - length of output vector dimension
              start - vector specifying start of hyperslab (not used)
              count - vector specifying count of hyperslab (not used)
@OUTPUT     : output_data - vector of pointers to output buffer data
@RETURNS    : (nothing)
@DESCRIPTION: Routine to loop through an array of voxels and do a lookup
              table conversion on them.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 8, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static void do_lookup(void *caller_data, long num_voxels,
                      int input_num_buffers, int input_vector_length,
                      double *input_data[],
                      int output_num_buffers, int output_vector_length,
                      double *output_data[], Loop_Info *loop_info)
     /* ARGSUSED */
{
   Lookup_Data *lookup_data;
   long ivoxel;
   double lookup_value, scale, offset, denom;

   /* Get pointer to lookup info */
   lookup_data = (Lookup_Data *) caller_data;

   /* Check that values correspond */
   if ((input_num_buffers != 1) || (output_num_buffers != 1) ||
       (input_vector_length != 1) || 
       (output_vector_length != lookup_data->lookup_table->vector_length)) {
      (void) fprintf(stderr, "Bad internal values.\n");
      exit(EXIT_FAILURE);
   }

   /* Calculate a scale and offset for input values */
   if (lookup_data->discrete) {
      scale = 1.0;
      offset = 0.0;
   }
   else {
      denom = (lookup_data->range[1] - lookup_data->range[0]);
      if (denom == 0.0) 
         scale = 0.0;
      else
         scale = 1.0 / denom;
      if (!lookup_data->invert) {
         offset = -lookup_data->range[0] * scale;
      }
      else {
         scale = -scale;
         offset = -lookup_data->range[1] * scale;
      }
   }

   /* Loop through the voxels */
   for (ivoxel=0; ivoxel < num_voxels; ivoxel++) {

      /* Convert input to a lookup value */
      lookup_value = input_data[0][ivoxel] * scale + offset;

      /* Look it up */
      lookup_in_table(lookup_value, lookup_data->lookup_table,
                      lookup_data->discrete, lookup_data->null_value,
                      &output_data[0][ivoxel*output_vector_length]);
   }

   return;
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : lookup_in_table
@INPUT      : index - value to look up in table
              lookup_table - the lookup table (big surprise!)
              discrete_values - flag indicating whether the table should
                 be considered continuous in the range 0 to 1 (FALSE) or 
                 discrete, with integer values that should be rounded (TRUE).
                 If the table is continuous, then interpolation between
                 table entries is done (the first and last values propagate
                 beyond the bounds). If the table is discrete, then the 
                 null_value is used for indices that are not found.
              null_value - array specifying the null values to use if
                 the index is not found in the table. Only applies to
                 discrete valued tables (may be NULL otherwise).
@OUTPUT     : output_value - vector of output values.
@RETURNS    : (nothing)
@DESCRIPTION: Routine to look up a value in the table.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 8, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static void lookup_in_table(double index, Lookup_Table *lookup_table,
                            int discrete_values, double null_value[],
                            double output_value[])
{
   int vector_length, nentries;
   int start, length, mid;
   int offset, offset1, offset2, ivalue;
   double value1, value2, *result, frac, rfrac, denom;

   /* Check for bad lookup table */
   nentries = lookup_table->nentries;
   vector_length = lookup_table->vector_length;
   if ((nentries < 1) || (vector_length < 1)) {
      (void) fprintf(stderr, "Bad table size %d x %d\n", 
                     nentries, vector_length);
      exit(EXIT_FAILURE);
   }

   /* Round values if needed */
   if (discrete_values) index = rint(index);

   /* Search the table for the value */
   start = 0;
   length = nentries;
   while (length > 1) {
      mid = start + length / 2;
      offset = mid*(vector_length+1);
      if (index < lookup_table->table[offset]) {
         length = mid - start;
      }
      else {
         length = start + length - mid;
         start = mid;
      }
   }

   /* Add a special check for the end of the table */
   if (nentries > 1) {
      offset1 = vector_length+1;
      offset2 = (nentries-2) * (vector_length+1);
      if ((start == 0) && (index == lookup_table->table[offset1]))
         start = 1;
      else if ((start == nentries-1) && 
               (index == lookup_table->table[offset2]))
         start = nentries-2;
   }

   /* Save the value */
   offset = start*(vector_length+1);
   if (discrete_values) {
      if (index == lookup_table->table[offset])
         result = &lookup_table->table[offset+1];
      else
         result = null_value;
      for (ivalue=0; ivalue < vector_length; ivalue++) {
         if (result != NULL)
            output_value[ivalue] = result[ivalue];
         else 
            output_value[ivalue] = 0.0;
      }
   }
   else {
      offset1 = offset;
      if (start < nentries - 1)
         offset2 = offset + vector_length + 1;
      else
         offset2 = offset;
      value1 = lookup_table->table[offset1];
      value2 = lookup_table->table[offset2];
      denom = value2 - value1;
      if (denom != 0.0)
         frac = (index - value1) / denom;
      else
         frac = 0.0;
      if (frac < 0.0) frac = 0.0;
      if (frac > 1.0) frac = 1.0;
      rfrac = 1.0 - frac;
      for (ivalue=0; ivalue < vector_length; ivalue++) {
         output_value[ivalue] = 
            rfrac * lookup_table->table[offset1 + 1 + ivalue] +
            frac  * lookup_table->table[offset2 + 1 + ivalue];
      }
   }
}

