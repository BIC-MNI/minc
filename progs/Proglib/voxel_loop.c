/* ----------------------------- MNI Header -----------------------------------
@NAME       : voxel_loop.c
@DESCRIPTION: Routines to loop through a file doing an operation on a single
              voxel.
@METHOD     : 
@GLOBALS    : 
@CREATED    : January 10, 1994 (Peter Neelin)
@MODIFIED   : $Log: voxel_loop.c,v $
@MODIFIED   : Revision 1.1  1994-12-14 10:17:19  neelin
@MODIFIED   : Initial revision
@MODIFIED   :
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

#ifndef lint
static char rcsid[]="$Header: /private-cvsroot/minc/progs/Proglib/Attic/voxel_loop.c,v 1.1 1994-12-14 10:17:19 neelin Exp $";
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <minc.h>
#include <minc_def.h>
#include <voxel_loop.h>
#include <nd_loop.h>

#ifndef public
#  define public
#endif
#ifndef private
#  define private static
#endif

#ifndef TRUE
#  define TRUE 1
#  define FALSE 0
#endif

/* Default ncopts values for error handling */
#define NC_OPTS_VAL NC_VERBOSE | NC_FATAL

/* Epsilon for coordinate comparisons */
#define COORD_EPSILON FLT_EPSILON * 10.0

/* Macros */
#define ABS(x)  ( ((x) > (0)) ? (x) : (-(x)))

/* Typedefs */
typedef struct {
   int clobber_output;
   int num_input_files;
   int num_output_files;
   char **input_files;
   char **output_files;
   int input_all_open;
   int output_all_open;
   int *input_mincid;
   int *output_mincid;
   int *input_icvid;
   int *output_icvid;
   int current_input_file_number;
   int current_output_file_number;
} Loopfile_Info;

/* Function prototypes */
private void check_input_files(Loop_Options *loop_options,
                               Loopfile_Info *loopfile_info);
private void get_dim_info(int mincid, int *ndims, long size[], 
                          char dimname[][MAX_NC_NAME],
                          double start[], double step[]);
private void setup_output_files(Loop_Options *loop_options, 
                                Loopfile_Info *loopfile_info,
                                char *arg_string);
private long get_vector_length(int mincid);
private void setup_variables(int inmincid, int outmincid,
                             int output_curfile,
                             char *arg_string, Loop_Options *loop_options);
private void update_history(int mincid, char *arg_string);
private void setup_icvs(Loopfile_Info *loopfile_info);
private void do_voxel_loop(Loop_Options *loop_options,
                           Loopfile_Info *loopfile_info);
private void setup_looping(Loop_Options *loop_options, 
                           Loopfile_Info *loopfile_info,
                           int *ndims,
                           long block_start[], long block_end[], 
                           long block_incr[], long *block_num_voxels,
                           long chunk_incr[], long *chunk_num_voxels);
private void initialize_loopfile_info(int num_input_files,
                                      char *input_files[],
                                      int num_output_files,
                                      char *output_files[],
                                      Loop_Options *loop_options,
                                      Loopfile_Info *loopfile_info);
private void cleanup_loopfile_info(Loopfile_Info *loopfile_info);
private int get_input_mincid(Loopfile_Info *loopfile_info,
                             int file_num);
private int get_output_mincid(Loopfile_Info *loopfile_info,
                              int file_num);
private int create_output_file(Loopfile_Info *loopfile_info,
                               int file_num);
private int get_input_icvid(Loopfile_Info *loopfile_info,
                            int file_num);
private int get_output_icvid(Loopfile_Info *loopfile_info,
                             int file_num);
private int create_input_icvid(Loopfile_Info *loopfile_info,
                               int file_num);
private int create_output_icvid(Loopfile_Info *loopfile_info,
                                int file_num);


/* ----------------------------- MNI Header -----------------------------------
@NAME       : voxel_loop
@INPUT      : num_input_files - number of input files.
              input_files - array of names of input files.
              num_output_files - number of output files.
              output_files - array of names of output files.
              arg_string - string for history.
              loop_options - pointer to structure containing loop options.
              voxel_function - user function to process a group of voxels.
                 See description in header file.
              caller_data - data that will be passed to voxel_function
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to loop through the voxels of a file and call a function
              to operate on each voxel.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 10, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void voxel_loop(int num_input_files, char *input_files[], 
                       int num_output_files, char *output_files[], 
                       char *arg_string, 
                       Loop_Options *loop_options,
                       VoxelFunction voxel_function, void *caller_data)
{
   Loop_Options loop_options_struct;
   Loopfile_Info loopfile_info_struct;
   Loopfile_Info *loopfile_info = &loopfile_info_struct;

   /* Check that there is at least one input file */
   if (num_input_files < 1) {
      (void) fprintf(stderr, "There must be at least one input file.\n");
      exit(EXIT_FAILURE);
   }
   if (num_output_files < 0) {
      (void) fprintf(stderr, "Negative number of output files!\n");
      exit(EXIT_FAILURE);
   }

   /* Initialize loop options if needed */
   if (loop_options == NULL) {
      loop_options = &loop_options_struct;
      initialize_loop_options(loop_options);
   }
   loop_options->voxel_function = voxel_function;
   loop_options->caller_data = caller_data;

   /* Initialize looping info */
   initialize_loopfile_info(num_input_files, input_files,
                            num_output_files, output_files,
                            loop_options, loopfile_info);

   /* Check that input files match */
   check_input_files(loop_options, loopfile_info);

   /* Set up variables in output file */
   setup_output_files(loop_options, loopfile_info, arg_string);

   /* Setup icv's */
   setup_icvs(loopfile_info);

   /* Loop through the voxels */
   do_voxel_loop(loop_options, loopfile_info);

   /* Clean up looping info */
   cleanup_loopfile_info(loopfile_info);
                 
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : check_input_files
@INPUT      : loop_options - Options for loops
              loopfile_info - Information describing looping stuff and files
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to check input files for consistency.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 30, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void check_input_files(Loop_Options *loop_options,
                               Loopfile_Info *loopfile_info)
{
   int ifile, idim;
   int first_ndims, ndims;
   long first_size[MAX_VAR_DIMS], size[MAX_VAR_DIMS];
   char first_dimname[MAX_VAR_DIMS][MAX_NC_NAME];
   char dimname[MAX_VAR_DIMS][MAX_NC_NAME];
   double first_start[MAX_VAR_DIMS], start[MAX_VAR_DIMS];
   double first_step[MAX_VAR_DIMS], step[MAX_VAR_DIMS];
   double start_diff, step_diff;

   /* Get information for first file */
   get_dim_info(get_input_mincid(loopfile_info, 0), 
                &first_ndims, first_size, first_dimname,
                first_start, first_step);

   /* Loop over files */
   for (ifile = 1; ifile < loopfile_info->num_input_files; ifile++) {
      /* Get dimension information for this file */
      get_dim_info(get_input_mincid(loopfile_info, ifile), 
                   &ndims, size, dimname, start, step);

      /* Check number of dimensions */
      if (ndims != first_ndims) {
         (void) fprintf(stderr, 
            "Files %s and %s have different numbers of dimensions\n",
                        loopfile_info->input_files[ifile],
                        loopfile_info->input_files[0]);
         exit(EXIT_FAILURE);
      }

      /* Loop over dimensions */
      for (idim = 0; idim < first_ndims; idim++) {

         /* Check dimension sizes */
         if (size[idim] != first_size[idim]) {
            (void) fprintf(stderr, 
               "Files %s and %s have different sizes of dimensions\n",
                           loopfile_info->input_files[ifile],
                           loopfile_info->input_files[0]);
            exit(EXIT_FAILURE);
         }

         /* Check optional dimension stuff */
         if (loop_options->check_all_input_dim_info) {

            /* Check names */
            if (strcmp(dimname[idim], first_dimname[idim]) != 0) {
               (void) fprintf(stderr, 
                  "Files %s and %s have different dimension names\n",
                              loopfile_info->input_files[ifile],
                              loopfile_info->input_files[0]);
               exit(EXIT_FAILURE);
            }

            /* Check coordinates */
            start_diff = start[idim] - first_start[idim];
            if (first_start[idim] != 0.0) start_diff /= first_start[idim];
            step_diff = step[idim] - first_step[idim];
            if (first_step[idim] != 0.0) step_diff /= first_step[idim];
            if ((ABS(start_diff) > COORD_EPSILON) ||
                (ABS(step_diff) > COORD_EPSILON)) {
               (void) fprintf(stderr, 
                  "Files %s and %s have different dimension coordinates\n",
                              loopfile_info->input_files[ifile],
                              loopfile_info->input_files[0]);
               exit(EXIT_FAILURE);
            }

         }      /* If check all dimension info */

      }      /* End of loop over files */

   }      /* End of loop over files */   
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_dim_info
@INPUT      : mincid - Minc id of file
@OUTPUT     : ndims - number of dimensions
              size  - array of sizes of dimensions
              dimname - array of dimension names
              start - array of starts for dimensions
              step  - array of steps for dimensions
@RETURNS    : (nothing)
@DESCRIPTION: Routine to get dimension information for a file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 30, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void get_dim_info(int mincid, int *ndims, long size[], 
                          char dimname[][MAX_NC_NAME],
                          double start[], double step[])
{
   int imgid, varid;
   int idim;
   int dim[MAX_VAR_DIMS];
   char string[MAX_NC_NAME];
   char *thename;
   int old_ncopts;

   /* Get image variable info */
   imgid = ncvarid(mincid, MIimage);
   (void) ncvarinq(mincid, imgid, NULL, NULL, ndims, dim, NULL);

   /* Loop through dimensions */
   for (idim=0; idim < *ndims; idim++) {
      if (dimname == NULL) thename = string;
      else thename = dimname[idim];
      (void) ncdiminq(mincid, dim[idim], thename, &size[idim]);

      /* Get coordinate info */
      if (start != NULL) start[idim] = 0.0;
      if (step != NULL) step[idim] = 1.0;
      old_ncopts = ncopts; ncopts = 0;
      varid = ncvarid(mincid, thename);
      if (varid != MI_ERROR) {
         if (start != NULL)
            (void) miattget1(mincid, varid, MIstart, NC_DOUBLE, &start[idim]);
         if (step != NULL)
            (void) miattget1(mincid, varid, MIstep, NC_DOUBLE, &step[idim]);
         }
      ncopts = old_ncopts;
   }
   
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : setup_output_files
@INPUT      : loop_options - Options controlling looping
              loopfile_info - Looping information
              arg_string - string for history
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to setup the the output files
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 30, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void setup_output_files(Loop_Options *loop_options, 
                                Loopfile_Info *loopfile_info,
                                char *arg_string)
{
   int inmincid, outmincid;
   int ifile;

   /* Get mincid for first input file */
   inmincid = get_input_mincid(loopfile_info, 0);

   /* Create output files */
   for (ifile=0; ifile < loopfile_info->num_output_files; ifile++) {
      outmincid = create_output_file(loopfile_info, ifile);
      setup_variables(inmincid, outmincid, ifile, arg_string, loop_options);
   }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_vector_length
@INPUT      : mincid - minc file id
@OUTPUT     : (none)
@RETURNS    : Length of vector dimension or zero if no such dimension.
@DESCRIPTION: Routine to get the length of the vector dimension in a minc file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 30, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private long get_vector_length(int mincid)
{
   int imgid;
   int ndims;
   int dim[MAX_VAR_DIMS];
   char dimname[MAX_NC_NAME];
   long vector_length;

   /* Get image variable id */
   imgid = ncvarid(mincid, MIimage);

   /* Get the image dimension info */
   (void) ncvarinq(mincid, imgid, NULL, NULL, &ndims, dim, NULL);

   /* Check for vector dimension */
   (void) ncdiminq(mincid, dim[ndims-1], dimname, &vector_length);
   if ((strcmp(dimname, MIvector_dimension) != 0) || (ndims <= 2)) {
      vector_length = 0;
   }

   return vector_length;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : setup_variables
@INPUT      : inmincid - input minc file id
              outmincid - output minc file id
              output_curfile - current output file number (counting from zero)
              arg_string - string for history
              loop_options - options controlling loop behaviour
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to setup the variables in the output file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 10, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void setup_variables(int inmincid, int outmincid,
                             int output_curfile,
                             char *arg_string, Loop_Options *loop_options)
{
   int inimgid, outimgid, maxid, minid;
   int indim[MAX_VAR_DIMS], outdim[MAX_VAR_DIMS];
   nc_type datatype;
   int idim, odim, in_ndims, out_ndims, in_nimgdims, out_nimgdims;
   char dimname[MAX_NC_NAME];
   int nexcluded, excluded_vars[MAX_VAR_DIMS];
   long dimlength;
   int input_vector_length;
   int changing_vector_dim;

   /* Get image variable id for input file */
   inimgid = ncvarid(inmincid, MIimage);

   /* Get the list of dimensions subscripting the image variable */
   (void) ncvarinq(inmincid, inimgid, NULL, &datatype, &in_ndims, indim, NULL);

   /* Get the length of the input vector dimension */
   input_vector_length = get_vector_length(inmincid);
   if (loop_options->convert_input_to_scalar && (input_vector_length > 0)) {
      input_vector_length = 0;
      in_ndims--;
   }

   /* Get the number of image dimensions in the input file */
   in_nimgdims = (input_vector_length == 0 ? 2 : 3);

   /* Are we changing the length of the vector dimension (or removing it?).
      For an output vector size of 1, we don't want an output vector
      dimension. */
   changing_vector_dim = 
      ((loop_options->output_vector_size != 0) &&
       (loop_options->output_vector_size != input_vector_length));
   if (loop_options->output_vector_size == 1)
      changing_vector_dim = (input_vector_length != 0);

   /* Work out number of output dimensions and image dimensions */
   out_ndims = in_ndims;
   out_nimgdims = in_nimgdims;
   if (changing_vector_dim && (input_vector_length == 0)) {
      out_ndims++;
      out_nimgdims++;
   }
   else if (changing_vector_dim && (loop_options->output_vector_size <= 1)) {
      out_ndims--;
      out_nimgdims--;
   }
      
   /* Set up the output minc file */

   /* Loop, creating output dimensions */
   odim = 0;
   for (idim=0; idim < in_ndims; idim++) {

      /* Check for a change in vector dimension length */
      if ((idim != in_ndims-1) || (input_vector_length == 0) || 
          !changing_vector_dim) {

         /* Copy the dimension */
         (void) ncdiminq(inmincid, indim[idim], dimname, &dimlength);
         outdim[odim] = ncdimdef(outmincid, dimname, dimlength);
         odim++;
      }
   }

   /* Create the output vector dimension if needed */
   if (changing_vector_dim && (loop_options->output_vector_size > 1)) {
      outdim[odim] = ncdimdef(outmincid, MIvector_dimension, 
                              (long) loop_options->output_vector_size);
   }

   /* Copy other variables in file */
   nexcluded = 0;
   excluded_vars[nexcluded] = inimgid;
   if (excluded_vars[nexcluded] != MI_ERROR) nexcluded++;
   excluded_vars[nexcluded] = ncvarid(inmincid, MIimagemax);
   if (excluded_vars[nexcluded] != MI_ERROR) nexcluded++;
   excluded_vars[nexcluded] = ncvarid(inmincid, MIimagemin);
   if (excluded_vars[nexcluded] != MI_ERROR) nexcluded++;
   (void) micopy_all_var_defs(inmincid, outmincid, nexcluded, excluded_vars);

   /* Add the time stamp to the history */
   update_history(outmincid, arg_string);
 
  /* Create the image and image-min/max variables */
   outimgid = micreate_std_variable(outmincid, MIimage, datatype, 
                                    out_ndims, outdim);
   (void) micopy_all_atts(inmincid, inimgid, outmincid, outimgid);
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

   /* Call the user's function if needed */
   if (loop_options->output_file_function != NULL) {
      loop_options->output_file_function(loop_options->caller_data,
                                         outmincid, output_curfile);
   }

   /* Put the file in data mode */
   (void) ncsetfill(outmincid, NC_NOFILL);
   (void) ncendef(outmincid);

   /* Copy over variable values */
   (void) micopy_all_var_values(inmincid, outmincid,
                                nexcluded, excluded_vars);

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
@NAME       : setup_icvs
@INPUT      : loopfile_info - looping information
@OUTPUT     : (nothing)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to set up the input and output icv's.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 30, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void setup_icvs(Loopfile_Info *loopfile_info)
{
   int ifile;
   int icvid;

   /* Loop through input icv's, setting their values. Attaching is
      done by get_input_icvid. */
   for (ifile=0; ifile < loopfile_info->num_input_files; ifile++) {
      icvid = create_input_icvid(loopfile_info, ifile);
      (void) miicv_setint(icvid, MI_ICV_TYPE, NC_DOUBLE);
      (void) miicv_setint(icvid, MI_ICV_DO_NORM, TRUE);
      (void) miicv_setint(icvid, MI_ICV_USER_NORM, TRUE);
      (void) miicv_setint(icvid, MI_ICV_DO_FILLVALUE, TRUE);
   }

   /* Loop through output icv's, setting their values. Attaching is
      done by get_input_icvid. */
   for (ifile=0; ifile < loopfile_info->num_output_files; ifile++) {
      icvid = create_output_icvid(loopfile_info, ifile);
      (void) miicv_setint(icvid, MI_ICV_TYPE, NC_DOUBLE);
      (void) miicv_setint(icvid, MI_ICV_DO_NORM, TRUE);
      (void) miicv_setint(icvid, MI_ICV_USER_NORM, TRUE);
   }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : do_voxel_loop
@INPUT      : loop_options - user options for looping
              loopfile_info - information on files used in loop
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to loop through the voxels and do something to each one
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 10, 1994 (Peter Neelin)
@MODIFIED   : November 30, 1994 (P.N.)
---------------------------------------------------------------------------- */
private void do_voxel_loop(Loop_Options *loop_options,
                           Loopfile_Info *loopfile_info)
{
   long block_start[MAX_VAR_DIMS], block_end[MAX_VAR_DIMS];
   long block_incr[MAX_VAR_DIMS];
   long block_cur[MAX_VAR_DIMS], block_curcount[MAX_VAR_DIMS];
   long chunk_start[MAX_VAR_DIMS], chunk_end[MAX_VAR_DIMS];
   long chunk_incr[MAX_VAR_DIMS];
   long chunk_cur[MAX_VAR_DIMS], chunk_curcount[MAX_VAR_DIMS];
   double **input_buffers, **output_buffers, **extra_buffers;
   double **results_buffers;
   long chunk_num_voxels, block_num_voxels, ivox;
   int outmincid, imgid, maxid, minid;
   double *data, minimum, maximum;
   int ifile, ibuff, ndims, idim;
   int num_input_files, num_output_files;
   int num_input_buffers, num_output_buffers, num_extra_buffers;
   int input_vector_length, output_vector_length;
   int modify_vector_count;

   /* Get number of files, buffers, etc. */
   num_input_files = loopfile_info->num_input_files;
   num_output_files = loopfile_info->num_output_files;
   num_input_buffers = (loop_options->do_accumulate ? 1 : num_input_files);
   num_extra_buffers = loop_options->num_extra_buffers;
   num_output_buffers = num_output_files + num_extra_buffers;
   input_vector_length = 
      get_vector_length(get_input_mincid(loopfile_info, 0));
   if ((input_vector_length == 0) || (loop_options->convert_input_to_scalar))
      input_vector_length = 1;
   if (num_output_files > 0) {
      output_vector_length = 
         get_vector_length(get_output_mincid(loopfile_info, 0));
      if (output_vector_length == 0) output_vector_length = 1;
   }
   else
      output_vector_length = 1;
   modify_vector_count = (input_vector_length != output_vector_length);

   /* Get block and chunk looping information */
   setup_looping(loop_options, loopfile_info, &ndims,
                 block_start, block_end, 
                 block_incr, &block_num_voxels,
                 chunk_incr, &chunk_num_voxels);

   /* Allocate space for buffers */
   input_buffers = MALLOC(sizeof(*input_buffers) * num_input_buffers);
   for (ibuff=0; ibuff < num_input_buffers; ibuff++) {
      input_buffers[ibuff] = MALLOC(sizeof(double) * chunk_num_voxels *
                                    input_vector_length);
   }
   if (num_output_buffers > 0) {
      results_buffers = MALLOC(sizeof(*results_buffers) * num_output_buffers);
   }
   else
      results_buffers = NULL;
   if (num_output_files > 0) {
      output_buffers = MALLOC(sizeof(output_buffers) * num_output_files);
      for (ibuff=0; ibuff < num_output_files; ibuff++) {
         output_buffers[ibuff] = MALLOC(sizeof(double) * block_num_voxels *
                                        output_vector_length);
         results_buffers[ibuff] = output_buffers[ibuff];
      }
   }
   else
      output_buffers = NULL;
   if (num_extra_buffers > 0) {
      extra_buffers = MALLOC(sizeof(extra_buffers) * num_extra_buffers);
      for (ibuff=0; ibuff < num_extra_buffers; ibuff++) {
         extra_buffers[ibuff] = MALLOC(sizeof(double) * chunk_num_voxels *
                                       output_vector_length);
         results_buffers[ibuff+num_output_files] = extra_buffers[ibuff];
      }
   }
   else
      extra_buffers = NULL;

   /* Print log message */
   if (loop_options->verbose) {
      (void) fprintf(stderr, "Processing:");
      (void) fflush(stderr);
   }

   /* Loop through blocks (image-max/min do not vary over blocks) */

   nd_begin_looping(block_start, block_cur, ndims);

   while (!nd_end_of_loop(block_cur, block_end, ndims)) {

      nd_update_current_count(block_cur, block_incr, block_end,
                              block_curcount, ndims);

      /* Set results_buffers to beginning of output buffers */
      for (ifile=0; ifile < num_output_files; ifile++) {
         results_buffers[ifile] = output_buffers[ifile];
      }

      /* Loop through chunks (space for input buffers) */
      for (idim=0; idim < ndims; idim++) {
         chunk_start[idim] = block_cur[idim];
         chunk_end[idim] = block_cur[idim] + block_curcount[idim];
      }

      nd_begin_looping(chunk_start, chunk_cur, ndims);

      while (!nd_end_of_loop(chunk_cur, chunk_end, ndims)) {

         nd_update_current_count(chunk_cur, chunk_incr, chunk_end,
                                 chunk_curcount, ndims);

         /* Print log message */
         if (loop_options->verbose) {
            (void) fprintf(stderr, ".");
            (void) fflush(stderr);
         }

         /* Initialize results buffers if necessary */
         if (loop_options->do_accumulate) {
            for (ibuff=0; ibuff < num_output_buffers; ibuff++) {
               loop_options->start_function(loop_options->caller_data,
                                            chunk_num_voxels,
                                            num_output_buffers,
                                            loop_options->output_vector_size,
                                            results_buffers,
                                            chunk_cur, chunk_curcount);
            }
         }

         /* Get the input buffers and accumulate them if needed */
         for (ifile=0; ifile < num_input_files; ifile++) {
            ibuff = (loop_options->do_accumulate ? 0 : ifile);
            (void) miicv_get(get_input_icvid(loopfile_info, ifile),
                             chunk_cur, chunk_curcount, 
                             input_buffers[ibuff]);
            if (loop_options->do_accumulate) {
               loop_options->voxel_function(loop_options->caller_data,
                                            chunk_num_voxels, 
                                            num_input_buffers, 
                                            input_vector_length,
                                            input_buffers,
                                            num_output_buffers, 
                                            output_vector_length,
                                            results_buffers,
                                            chunk_cur, chunk_curcount);
            }
         }

         /* Do something with the buffers or finish accumulation */
         if (loop_options->do_accumulate) {
            loop_options->finish_function(loop_options->caller_data,
                                          chunk_num_voxels, 
                                          num_output_buffers,
                                          output_vector_length,
                                          results_buffers,
                                          chunk_cur, chunk_curcount);
         }
         else {
            loop_options->voxel_function(loop_options->caller_data,
                                         chunk_num_voxels, 
                                         num_input_buffers, 
                                         input_vector_length,
                                         input_buffers,
                                         num_output_buffers, 
                                         output_vector_length,
                                         results_buffers,
                                         chunk_cur, chunk_curcount);
         }

         /* Increment results_buffers through output buffers */
         for (ifile=0; ifile < num_output_files; ifile++) {
            results_buffers[ifile] += chunk_num_voxels;
         }

         nd_increment_loop(chunk_cur, chunk_start, chunk_incr, 
                           chunk_end, ndims);

      }     /* End of loop through chunks */

      /* Write out output buffers */

      for (ifile=0; ifile < num_output_files; ifile++) {
         outmincid = get_output_mincid(loopfile_info, ifile);
         maxid = ncvarid(outmincid, MIimagemax);
         minid = ncvarid(outmincid, MIimagemin);
         data = output_buffers[ifile];

         /* Find the max and min */
         minimum = DBL_MAX;
         maximum = -DBL_MAX;
         for (ivox=0; ivox < block_num_voxels*output_vector_length; ivox++) {
            if (data[ivox] != -DBL_MAX) {
               if (data[ivox] < minimum) minimum = data[ivox];
               if (data[ivox] > maximum) maximum = data[ivox];
            }
         }
         if ((minimum == DBL_MAX) && (maximum == -DBL_MAX)) {
            minimum = 0.0;
            maximum = 0.0;
         }

         /* Write out the max and min */
         (void) mivarput1(outmincid, maxid, block_cur, 
                          NC_DOUBLE, NULL, &maximum);
         (void) mivarput1(outmincid, minid, block_cur, 
                          NC_DOUBLE, NULL, &minimum);

         /* Write out the values */
         if (modify_vector_count)
            block_curcount[ndims-1] = output_vector_length;
         (void) miicv_put(get_output_icvid(loopfile_info, ifile), 
                          block_cur, block_curcount, data);
      }          /* End of loop through output files */

      nd_increment_loop(block_cur, block_start, block_incr, block_end, ndims);

   }     /* End of loop through chunks */

   /* Data has been completely written */
   for (ifile=0; ifile < num_output_files; ifile++) {
      outmincid = get_output_mincid(loopfile_info, ifile);
      imgid = ncvarid(outmincid, MIimage);
      (void) miattputstr(outmincid, imgid, MIcomplete, MI_TRUE);
   }

   /* Print log message */
   if (loop_options->verbose) {
      (void) fprintf(stderr, "Done\n");
      (void) fflush(stderr);
   }

   /* Free buffers */
   for (ibuff=0; ibuff < num_input_buffers; ibuff++) {
      FREE(input_buffers[ibuff]);
   }
   FREE(input_buffers);
   if (num_output_buffers > 0) {
      results_buffers = MALLOC(sizeof(*results_buffers) * num_output_buffers);
   }
   if (num_output_files > 0) {
      for (ibuff=0; ibuff < num_output_files; ibuff++) {
         FREE(output_buffers[ibuff]);
      }
      FREE(output_buffers);
   }
   if (num_extra_buffers > 0) {
      for (ibuff=0; ibuff < num_extra_buffers; ibuff++) {
         FREE(extra_buffers[ibuff]);
      }
      FREE(extra_buffers);
   }

   return;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : setup_looping
@INPUT      : loop_options - users options controlling looping
              loopfile_info - information on files
@OUTPUT     : ndims - number of dimensions
              block_start - vector specifying start of block
              block_end - end of block
              block_incr - increment for stepping through blocks
              block_num_voxels - number of voxels in block
              chunk_incr - increment for stepping through chunks
              chunk_num_voxels - number of voxels in chunk
@RETURNS    : (nothing)
@DESCRIPTION: Routine to set up vectors giving blocks and chunks through
              which we will loop.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 2, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void setup_looping(Loop_Options *loop_options, 
                           Loopfile_Info *loopfile_info,
                           int *ndims,
                           long block_start[], long block_end[], 
                           long block_incr[], long *block_num_voxels,
                           long chunk_incr[], long *chunk_num_voxels)
{
   int inmincid;
   int total_ndims, scalar_ndims, idim;
   int input_vector_length, output_vector_length;
   int vector_data;
   int nimgdims;
   long size[MAX_VAR_DIMS];
   long max_voxels_in_buffer;

   /* Get input mincid */
   inmincid = get_input_mincid(loopfile_info, 0);

   /* Get number of dimensions and their sizes */
   get_dim_info(inmincid, &total_ndims, size, NULL, NULL, NULL);

   /* Get vector lengths */
   input_vector_length = get_vector_length(inmincid);
   if (loopfile_info->num_output_files > 0)
      output_vector_length = 
         get_vector_length(get_output_mincid(loopfile_info, 0));
   else
      output_vector_length = 0;

   /* Check for vector data and whether we are adding a dimension */
   vector_data = ((input_vector_length > 0) || (output_vector_length > 0));
   if ((input_vector_length == 0) && (output_vector_length > 0)) {
      total_ndims++;
      size[total_ndims-1] = 1;
   }
   scalar_ndims = (vector_data ? total_ndims - 1 : total_ndims);

   /* Get number of image dimensions */
   nimgdims = (vector_data ? 3 : 2);

   /* Set vector lengths properly */
   if (input_vector_length <= 0) input_vector_length = 1;
   if (output_vector_length <= 0) output_vector_length = 1;

   /* Set vectors */
   *block_num_voxels = 1;
   for (idim=0; idim < total_ndims; idim++) {
      block_start[idim] = 0;
      block_end[idim] = size[idim];
      if (idim < total_ndims - nimgdims)
         block_incr[idim] = 1;
      else 
         block_incr[idim] = size[idim];
      *block_num_voxels *= block_incr[idim];
      chunk_incr[idim] = 1;
   }
   if (vector_data) {
      *block_num_voxels /= input_vector_length;
      idim = total_ndims-1;
      chunk_incr[idim] = block_incr[idim];
   }

   /* Figure out chunk size */
   *chunk_num_voxels = 1;
   max_voxels_in_buffer = 
      (loop_options->total_copy_space/sizeof(double) - 
       loopfile_info->num_output_files * *block_num_voxels *
       output_vector_length) / 
          (loopfile_info->num_input_files * input_vector_length + 
           loop_options->num_extra_buffers * output_vector_length);
   if (max_voxels_in_buffer > 0) {
      for (idim=scalar_ndims-1; idim >= 0; idim--) {
         chunk_incr[idim] = max_voxels_in_buffer / *chunk_num_voxels;
         if (chunk_incr[idim] > block_incr[idim])
            chunk_incr[idim] = block_incr[idim];
         *chunk_num_voxels *= chunk_incr[idim];
      }
   }

   /* Set ndims */
   *ndims = total_ndims;
                
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : initialize_loopfile_info
@INPUT      : num_input_files - number of input files
              input_files - list of input file names
              num_output_files - list of output file names
              output_files - list of output file names
              loop_options - user options for looping
@OUTPUT     : loopfile_info - looping information
@RETURNS    : (nothing)
@DESCRIPTION: Routine to set up looping information for these files.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 30, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void initialize_loopfile_info(int num_input_files,
                                      char *input_files[],
                                      int num_output_files,
                                      char *output_files[],
                                      Loop_Options *loop_options,
                                      Loopfile_Info *loopfile_info)
{
   int num_free_files, num_files, ifile;

   /* Save clobber info */
   loopfile_info->clobber_output = loop_options->clobber;

   /* Save number of input and output files */
   loopfile_info->num_input_files = num_input_files;
   loopfile_info->num_output_files = num_output_files;

   /* Save input file names (just copy pointers, not strings) */
   if (num_input_files > 0) {
      loopfile_info->input_files = MALLOC(sizeof(char *) * num_input_files);
      for (ifile=0; ifile < num_input_files; ifile++)
         loopfile_info->input_files[ifile] = input_files[ifile];
   }
   else
      loopfile_info->input_files = NULL;

   /* Save output file names (just copy pointers, not strings) */
   if (num_output_files > 0) {
      loopfile_info->output_files = MALLOC(sizeof(char *) * num_output_files);
      for (ifile=0; ifile < num_output_files; ifile++)
         loopfile_info->output_files[ifile] = output_files[ifile];
   }
   else
      loopfile_info->output_files = NULL;

   /* Keep track of number of files that we can open */
   num_free_files = loop_options->max_open_files;
   if (num_free_files > MAX_NC_OPEN) num_free_files = MAX_NC_OPEN;

   /* Check to see if we can open output files */
   if (num_output_files < num_free_files) {
      loopfile_info->output_all_open = TRUE;
      num_files = num_output_files;
   }
   else {
      loopfile_info->output_all_open = FALSE;
      num_files = 1;
   }
   num_free_files -= num_files;
   loopfile_info->output_mincid = MALLOC( sizeof(int) * num_files);
   loopfile_info->output_icvid = MALLOC( sizeof(int) * num_files);
   for (ifile=0; ifile < num_files; ifile++) {
      loopfile_info->output_mincid[ifile] = MI_ERROR;
      loopfile_info->output_icvid[ifile] = MI_ERROR;
   }
   loopfile_info->current_input_file_number = -1;

   /* Check to see if we can open input files */
   if (num_input_files < num_free_files) {
      loopfile_info->input_all_open = TRUE;
      num_files = num_input_files;
   }
   else {
      loopfile_info->input_all_open = FALSE;
      num_files = 1;
   }
   num_free_files -= num_files;
   loopfile_info->input_mincid = MALLOC( sizeof(int) * num_files);
   loopfile_info->input_icvid = MALLOC( sizeof(int) * num_files);
   for (ifile=0; ifile < num_files; ifile++) {
      loopfile_info->input_mincid[ifile] = MI_ERROR;
      loopfile_info->input_icvid[ifile] = MI_ERROR;
   }
   loopfile_info->current_output_file_number = -1;

   /* Check for an already open input file */
   if (loop_options->input_mincid != MI_ERROR) {
      loopfile_info->input_mincid[0] = loop_options->input_mincid;
      loopfile_info->current_input_file_number = 0;
   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : cleanup_loopfile_info
@INPUT      : loopfile_info - looping information
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to clean up looping information.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 30, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void cleanup_loopfile_info(Loopfile_Info *loopfile_info)
{
   int num_files, ifile;

   /* Close input files and free icv's */
   if (loopfile_info->input_all_open)
      num_files = loopfile_info->num_input_files;
   else
      num_files = 1;
   for (ifile=0; ifile < num_files; ifile++) {
      (void) miicv_free(loopfile_info->input_icvid[ifile]);
      (void) miclose(loopfile_info->input_mincid[ifile]);
   }

   /* Close output files and free icv's */
   if (loopfile_info->output_all_open)
      num_files = loopfile_info->num_output_files;
   else
      num_files = 1;
   for (ifile=0; ifile < num_files; ifile++) {
      (void) miicv_free(loopfile_info->output_icvid[ifile]);
      (void) miclose(loopfile_info->output_mincid[ifile]);
   }

   /* Free input arrays */
   if (loopfile_info->input_files != NULL)
      FREE(loopfile_info->input_files);
   if (loopfile_info->input_mincid != NULL)
      FREE(loopfile_info->input_mincid);
   if (loopfile_info->input_icvid != NULL)
      FREE(loopfile_info->input_icvid);

   /* Free output arrays */
   if (loopfile_info->output_files != NULL)
      FREE(loopfile_info->output_files);
   if (loopfile_info->output_mincid != NULL)
      FREE(loopfile_info->output_mincid);
   if (loopfile_info->output_icvid != NULL)
      FREE(loopfile_info->output_icvid);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_input_mincid
@INPUT      : loopfile_info - looping information
              file_num - input file number
@OUTPUT     : (none)
@RETURNS    : Id of minc file
@DESCRIPTION: Routine to get the minc id for an input file. The file number
              corresponds to the file's position in the input_files list
              (counting from zero).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 30, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int get_input_mincid(Loopfile_Info *loopfile_info,
                             int file_num)
{
   int index;

   /* Check for bad file_num */
   if ((file_num < 0) || (file_num >= loopfile_info->num_input_files)) {
      (void) fprintf(stderr, "Bad input file number %d\n", file_num);
      exit(EXIT_FAILURE);
   }

   /* Check to see if all files are open or not */
   if (loopfile_info->input_all_open) {
      index = file_num;
   }
   else {
      index = 0;
      if ((loopfile_info->input_mincid[index] != MI_ERROR) &&
          (loopfile_info->current_input_file_number != file_num)) {
         (void) miclose(loopfile_info->input_mincid[index]);
         loopfile_info->input_mincid[index] = MI_ERROR;
      }
      loopfile_info->current_input_file_number = file_num;
   }

   /* Open the file if it hasn't been already */
   if (loopfile_info->input_mincid[index] == MI_ERROR) {
      loopfile_info->input_mincid[index] =
         miopen(loopfile_info->input_files[file_num], NC_NOWRITE);
   }

   return loopfile_info->input_mincid[index];
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_output_mincid
@INPUT      : loopfile_info - looping information
              file_num - output file number
@OUTPUT     : (none)
@RETURNS    : Id of minc file
@DESCRIPTION: Routine to get the minc id for an output file. The file number
              corresponds to the file's position in the output_files list
              (counting from zero).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 30, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int get_output_mincid(Loopfile_Info *loopfile_info,
                              int file_num)
{
   int index;

   /* Check for bad file_num */
   if ((file_num < 0) || (file_num >= loopfile_info->num_output_files)) {
      (void) fprintf(stderr, "Bad output file number %d\n", file_num);
      exit(EXIT_FAILURE);
   }

   /* Check to see if all files are open or not */
   if (loopfile_info->output_all_open) {
      index = file_num;
   }
   else {
      index = 0;
      if ((loopfile_info->output_mincid[index] != MI_ERROR) &&
          (loopfile_info->current_output_file_number != file_num)) {
         (void) miclose(loopfile_info->output_mincid[index]);
         loopfile_info->output_mincid[index] = MI_ERROR;
      }
      loopfile_info->current_output_file_number = file_num;
   }

   /* Open the file if it hasn't been already */
   if (loopfile_info->output_mincid[index] == MI_ERROR) {
      loopfile_info->output_mincid[index] =
         miopen(loopfile_info->output_files[file_num], NC_WRITE);
   }

   return loopfile_info->output_mincid[index];
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_output_file
@INPUT      : loopfile_info - looping information
              file_num - output file number
@OUTPUT     : (none)
@RETURNS    : Id of minc file
@DESCRIPTION: Routine to create an output file. The file number
              corresponds to the file's position in the output_files list
              (counting from zero).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 30, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int create_output_file(Loopfile_Info *loopfile_info,
                               int file_num)
{
   int index;

   /* Check for bad file_num */
   if ((file_num < 0) || (file_num >= loopfile_info->num_output_files)) {
      (void) fprintf(stderr, "Bad output file number %d for create.\n", 
                     file_num);
      exit(EXIT_FAILURE);
   }

   /* Check to see if all files are open or not */
   if (loopfile_info->output_all_open) {
      index = file_num;
   }
   else {
      index = 0;
      if ((loopfile_info->output_mincid[index] != MI_ERROR) &&
          (loopfile_info->current_output_file_number != file_num)) {
         (void) miclose(loopfile_info->output_mincid[index]);
         loopfile_info->output_mincid[index] = MI_ERROR;
      }
      loopfile_info->current_output_file_number = file_num;
   }

   /* Create the file */
   if (loopfile_info->output_mincid[index] != MI_ERROR) {
      (void) fprintf(stderr, "File %s has already been created\n",
                     loopfile_info->output_files[file_num]);
      exit(EXIT_FAILURE);
   }
   loopfile_info->output_mincid[index] =
      micreate(loopfile_info->output_files[file_num], 
               (loopfile_info->clobber_output ? 
                NC_CLOBBER : NC_NOCLOBBER));

   return loopfile_info->output_mincid[index];
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_input_icvid
@INPUT      : loopfile_info - looping information
              file_num - input file number
@OUTPUT     : (none)
@RETURNS    : Id of icv for the specified file
@DESCRIPTION: Routine to get the icv id for an input file. The file number
              corresponds to the file's position in the input_files list
              (counting from zero).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 30, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int get_input_icvid(Loopfile_Info *loopfile_info,
                            int file_num)
{
   int mincid, icv_mincid, icvid;
   int index;

   /* Check for bad file_num */
   if ((file_num < 0) || (file_num >= loopfile_info->num_input_files)) {
      (void) fprintf(stderr, "Bad input file number %d\n", file_num);
      exit(EXIT_FAILURE);
   }

   /* Check to see if all files are open or not - get the correct index */
   if (loopfile_info->input_all_open) {
      index = file_num;
   }
   else {
      index = 0;
   }

   /* Check to see if the icv is attached to the correct minc file. If
      not, re-attach it. */
   icvid = loopfile_info->input_icvid[index];
   mincid = get_input_mincid(loopfile_info, file_num);
   if (icvid != MI_ERROR)
      (void) miicv_inqint(icvid, MI_ICV_CDFID, &icv_mincid);
   else
      icv_mincid = MI_ERROR;
   if (mincid != icv_mincid) {
      (void) miicv_attach(icvid, mincid, ncvarid(mincid, MIimage));
   }
   return icvid;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_output_icvid
@INPUT      : loopfile_info - looping information
              file_num - output file number
@OUTPUT     : (none)
@RETURNS    : Id of icv for the specified file
@DESCRIPTION: Routine to get the icv id for an output file. The file number
              corresponds to the file's position in the output_files list
              (counting from zero).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 30, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int get_output_icvid(Loopfile_Info *loopfile_info,
                             int file_num)
{
   int mincid, icv_mincid, icvid;
   int index;

   /* Check for bad file_num */
   if ((file_num < 0) || (file_num >= loopfile_info->num_output_files)) {
      (void) fprintf(stderr, "Bad output file number %d\n", file_num);
      exit(EXIT_FAILURE);
   }

   /* Check to see if all files are open or not - get the correct index */
   if (loopfile_info->output_all_open) {
      index = file_num;
   }
   else {
      index = 0;
   }

   /* Check to see if the icv is attached to the correct minc file. If
      not, re-attach it. */
   icvid = loopfile_info->output_icvid[index];
   mincid = get_output_mincid(loopfile_info, file_num);
   if (icvid != MI_ERROR)
      (void) miicv_inqint(icvid, MI_ICV_CDFID, &icv_mincid);
   else
      icv_mincid = MI_ERROR;
   if (mincid != icv_mincid) {
      (void) miicv_attach(icvid, mincid, ncvarid(mincid, MIimage));
   }
   return icvid;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_input_icvid
@INPUT      : loopfile_info - looping information
              file_num - input file number
@OUTPUT     : (none)
@RETURNS    : Id of icv for the specified file
@DESCRIPTION: Routine to create the icv id for an input file. The file number
              corresponds to the file's position in the input_files list
              (counting from zero). If the icv already exists, just 
              return it. If there are too many files, then only one
              icv will be used.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 30, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int create_input_icvid(Loopfile_Info *loopfile_info,
                               int file_num)
{
   int index;

   /* Check for bad file_num */
   if ((file_num < 0) || (file_num >= loopfile_info->num_input_files)) {
      (void) fprintf(stderr, "Bad input file number %d\n", file_num);
      exit(EXIT_FAILURE);
   }

   /* Check to see if all files are open or not - get the correct index */
   if (loopfile_info->input_all_open) {
      index = file_num;
   }
   else {
      index = 0;
   }

   /* Check to see if icv exists - if not create it */
   if (loopfile_info->input_icvid[index] == MI_ERROR) {
      loopfile_info->input_icvid[index] = miicv_create();
   }

   return loopfile_info->input_icvid[index];
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_output_icvid
@INPUT      : loopfile_info - looping information
              file_num - output file number
@OUTPUT     : (none)
@RETURNS    : Id of icv for the specified file
@DESCRIPTION: Routine to create the icv id for an output file. The file number
              corresponds to the file's position in the output_files list
              (counting from zero). If the icv already exists, just 
              return it. If there are too many files, then only one
              icv will be used.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 30, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int create_output_icvid(Loopfile_Info *loopfile_info,
                                int file_num)
{
   int index;

   /* Check for bad file_num */
   if ((file_num < 0) || (file_num >= loopfile_info->num_output_files)) {
      (void) fprintf(stderr, "Bad output file number %d\n", file_num);
      exit(EXIT_FAILURE);
   }

   /* Check to see if all files are open or not - get the correct index */
   if (loopfile_info->output_all_open) {
      index = file_num;
   }
   else {
      index = 0;
   }

   /* Check to see if icv exists - if not create it */
   if (loopfile_info->output_icvid[index] == MI_ERROR) {
      loopfile_info->output_icvid[index] = miicv_create();
   }

   return loopfile_info->output_icvid[index];
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : initialize_loop_options
@INPUT      : loop_options - user options for looping
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to initialize the loop options structure.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 6, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void initialize_loop_options(Loop_Options *loop_options)
{
   loop_options->clobber = FALSE;
   loop_options->verbose = FALSE;
   loop_options->max_open_files = MAX_NC_OPEN - 2;
   loop_options->check_all_input_dim_info = TRUE;
   loop_options->convert_input_to_scalar = FALSE;
   loop_options->output_vector_size = 0;
   loop_options->input_mincid = MI_ERROR;
   loop_options->total_copy_space = 4 * 1024 * 1024;
   loop_options->output_file_function = NULL;
   loop_options->do_accumulate = FALSE;
   loop_options->num_extra_buffers = 0;
   loop_options->start_function = NULL;
   loop_options->finish_function = NULL;
   loop_options->voxel_function = NULL;
   loop_options->caller_data = NULL;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_loop_clobber
@INPUT      : loop_options - user options for looping
              verbose - TRUE if output files should be clobbered
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to turn output clobber on or off
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 6, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void set_loop_clobber(Loop_Options *loop_options, 
                             int clobber)
{
   loop_options->clobber = clobber;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_loop_verbose
@INPUT      : loop_options - user options for looping
              verbose - TRUE if logging should be done.
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to turn logging on or off.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 6, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void set_loop_verbose(Loop_Options *loop_options, 
                             int verbose)
{
   loop_options->verbose = verbose;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_loop_max_open_files
@INPUT      : loop_options - user options for looping
              max_open_files - maximum number of open files allowed (between
                 1 and MAX_NC_OPEN)
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to set the maximum number of open minc files.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 6, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void set_loop_max_open_files(Loop_Options *loop_options, 
                                    int max_open_files)
{
   if ((max_open_files <= 0) || (max_open_files > MAX_NC_OPEN)) {
      (void) fprintf(stderr, 
                     "Bad number of files %d in set_loop_max_open_files\n",
                     max_open_files);
      exit(EXIT_FAILURE);
   }

   loop_options->max_open_files = max_open_files;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_loop_convert_input_to_scalar
@INPUT      : loop_options - user options for looping
              convert_input_to_scalar - TRUE if input should be converted
                 to scalar values
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to allow input to be converted to scalar values
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 6, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void set_loop_convert_input_to_scalar(Loop_Options *loop_options, 
                                             int convert_input_to_scalar)
{
   loop_options->convert_input_to_scalar = convert_input_to_scalar;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_loop_output_vector_size
@INPUT      : loop_options - user options for looping
              output_vector_size - length of vector dimension for output.
                 0 means no vector dimension.
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to turn set the length of the vector dimension for 
              output.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 6, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void set_loop_output_vector_size(Loop_Options *loop_options, 
                                        int output_vector_size)
{
   if (output_vector_size < 0) {
      (void) fprintf(stderr, 
                     "Bad vector size %d in set_loop_output_vector_size\n",
                     output_vector_size);
      exit(EXIT_FAILURE);
   }
   loop_options->output_vector_size = output_vector_size;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_loop_first_input_mincid
@INPUT      : loop_options - user options for looping
              input_mincid - id of first minc file (already opened).
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to turn allow the user to pass in an already opened minc
              file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 6, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void set_loop_first_input_mincid(Loop_Options *loop_options, 
                                        int input_mincid)
{
   loop_options->input_mincid = input_mincid;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_loop_buffer_size
@INPUT      : loop_options - user options for looping
              buffer_size - maximum amount of buffer space to use (in bytes).
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to turn set a limit on the amount of buffer space used.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 6, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void set_loop_buffer_size(Loop_Options *loop_options,
                                 long buffer_size)
{
   if (buffer_size <= 0) {
      (void) fprintf(stderr, "Bad buffer size %d in set_loop_buffer_size\n",
                     (int) buffer_size);
   }

   loop_options->total_copy_space = buffer_size;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_loop_output_file_function
@INPUT      : loop_options - user options for looping
              output_file_function - function to be called for each
                 output file so that the user can modify the header (file
                 is in define mode).
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to allow the user to define a function to be called
              for each output file so that things can be added to the header. 
              The file will be in define mode and should remain that way.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 6, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void set_loop_output_file_function
   (Loop_Options *loop_options,
    VoxelOutputFileFunction output_file_function)
{
   loop_options->output_file_function = output_file_function;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_loop_accumulate
@INPUT      : loop_options - user options for looping
              num_extra_buffers - number of extra buffers to allocate.
              start_function - function to be called before looping with 
                 all output and extra buffers as arguments. NULL means
                 don't call any function.
              finish_function - function to be called after looping with
                 all output and extra buffers as arguments. NULL means
                 don't call any function.
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to turn allow an accumulation mode of looping. Instead
              of loading all input files and then calling the voxel routine,
              the voxel routine is called after for each input file. The
              user can provide a start_function that is called to initialize
              the output buffers and a finish_function that is called to
              finish calculations. The user can also ask for extra buffers.
              These are treated like output buffers (at the end of the list)
              but are not written out.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 6, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void set_loop_accumulate(Loop_Options *loop_options, 
                                int num_extra_buffers,
                                VoxelStartFunction start_function,
                                VoxelFinishFunction finish_function)
{
   if (num_extra_buffers < 0) {
      (void) fprintf(stderr, 
                     "Bad num_extra_buffers %d in set_loop_accumulate\n",
                     num_extra_buffers);
      exit(EXIT_FAILURE);
   }

   loop_options->do_accumulate = TRUE;
   loop_options->num_extra_buffers = num_extra_buffers;
   loop_options->start_function = start_function;
   loop_options->finish_function = finish_function;
   
}

