/* ----------------------------- MNI Header -----------------------------------
@NAME       : copy_data
@DESCRIPTION: File containing routines to copy data when reshaping.
@METHOD     : 
@GLOBALS    : 
@CREATED    : October 25, 1994 (Peter Neelin)
@MODIFIED   : $Log: copy_data.c,v $
@MODIFIED   : Revision 3.1  1995-10-03 13:34:14  neelin
@MODIFIED   : Fixed bug in truncate_input_vectors - was not handling out-of-range
@MODIFIED   : start values properly.
@MODIFIED   :
 * Revision 3.0  1995/05/15  19:32:36  neelin
 * Release of minc version 0.3
 *
 * Revision 1.5  1995/03/20  13:32:03  neelin
 * Fixed -normalize option.
 *
 * Revision 1.4  1994/12/02  09:08:56  neelin
 * Moved nd_loop to proglib.
 *
 * Revision 1.3  94/11/23  11:46:38  neelin
 * Handle image-min/max properly when using icv for normalization.
 * 
 * Revision 1.2  94/11/22  08:45:11  neelin
 * Fixed handling of normalization for number of image dimensions > 2.
 * Added appropriate default values of image-max and image-min.
 * 
 * Revision 1.1  94/11/02  16:21:09  neelin
 * Initial revision
 * 
@COPYRIGHT  :
              Copyright 1994 Peter Neelin, McConnell Brain Imaging Centre, 
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
static char rcsid[]="$Header: /private-cvsroot/minc/progs/mincreshape/copy_data.c,v 3.1 1995-10-03 13:34:14 neelin Exp $";
#endif

#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include <minc.h>
#include <minc_def.h>
#include <nd_loop.h>
#include "mincreshape.h"

/* ----------------------------- MNI Header -----------------------------------
@NAME       : copy_data
@INPUT      : reshape_info - information for reshaping volume
@OUTPUT     : (none)
@RETURNS    : (none)
@DESCRIPTION: Copies data from one input volume to another, reorganizing
              it according to the reshaping info.
@METHOD     :
@GLOBALS    :
@CALLS      :
@CREATED    : October 25, 1994 (Peter Neelin)
@MODIFIED   :
---------------------------------------------------------------------------- */
public void copy_data(Reshape_info *reshape_info)
{
   int idim, odim, out_ndims;
   long block_begin[MAX_VAR_DIMS], block_end[MAX_VAR_DIMS];
   long block_count[MAX_VAR_DIMS];
   long block_cur_start[MAX_VAR_DIMS], block_cur_count[MAX_VAR_DIMS];
   long chunk_begin[MAX_VAR_DIMS], chunk_end[MAX_VAR_DIMS];
   long chunk_count[MAX_VAR_DIMS];
   long chunk_cur_start[MAX_VAR_DIMS], chunk_cur_count[MAX_VAR_DIMS];
   long total_size;
   long num_min_values, num_max_values, num_values;
   double fillvalue, *minmax_buffer;
   void *chunk_data;

   /* Get number of dimensions */
   out_ndims = reshape_info->output_ndims;

   /* Set up variables for looping through blocks */
   for (odim=0; odim < out_ndims; odim++) {
      idim = reshape_info->map_out_to_in[odim];
      block_begin[odim] = 0;
      block_end[odim] = ABS(reshape_info->input_count[idim]);
      if (reshape_info->dim_used_in_block[odim])
         block_count[odim] = ABS(reshape_info->input_count[idim]);
      else
         block_count[odim] = 1;
   }

   /* Figure out size of chunks and allocate space */
   total_size = nctypelen(reshape_info->output_datatype);
   for (odim=0; odim < out_ndims; odim++) {
      total_size *= reshape_info->chunk_count[odim];
   }
   chunk_data = MALLOC(total_size);

   /* Get enough space for image-min and max values for a block */
   get_num_minmax_values(reshape_info, NULL, block_count, 
                         &num_min_values, &num_max_values);
   num_values = ((num_min_values > num_max_values) ?
                 num_min_values : num_max_values);
   if (num_values > 0)
      minmax_buffer = MALLOC(num_values * sizeof(double));
   else
      minmax_buffer = NULL;

   /* Print log message */
   if (reshape_info->verbose) {
      (void) fprintf(stderr, "Copying chunks:");
      (void) fflush(stderr);
   }

   /* Loop through blocks */

   nd_begin_looping(block_begin, block_cur_start, out_ndims);
   while (!nd_end_of_loop(block_cur_start, block_end, out_ndims)) {

      /* Set up count for current block */
      nd_update_current_count(block_cur_start, block_count, block_end,
                              block_cur_count, out_ndims);

      /* Set up chunk begin, end and count */
      for (odim=0; odim < out_ndims; odim++) {
         chunk_begin[odim] = block_cur_start[odim];
         chunk_end[odim] = chunk_begin[odim] + block_cur_count[odim];
         chunk_count[odim] = reshape_info->chunk_count[odim];
      }

      /* Set up icv for normalization, set output image-max/min and 
         calculate pixel fill value to use for current block */
      handle_normalization(reshape_info, block_cur_start, block_cur_count,
                           minmax_buffer, &fillvalue);

      /* Loop through chunks */

      nd_begin_looping(chunk_begin, chunk_cur_start, out_ndims);
      while (!nd_end_of_loop(chunk_cur_start, chunk_end, out_ndims)) {

         /* Set up count for current chunk */
         nd_update_current_count(chunk_cur_start, chunk_count, chunk_end,
                                 chunk_cur_count, out_ndims);

         /* Print log message for chunk */
         if (reshape_info->verbose) {
            (void) fprintf(stderr, ".");
            (void) fflush(stderr);
         }

         /* Copy the chunk */
         copy_the_chunk(reshape_info, 
                        chunk_cur_start, chunk_cur_count, chunk_data,
                        fillvalue);

         /* Increment chunk loop count */
         nd_increment_loop(chunk_cur_start, chunk_begin, chunk_count,
                           chunk_end, out_ndims);

      }

      /* Increment block loop count */
      nd_increment_loop(block_cur_start, block_begin, block_count,
                        block_end, out_ndims);

   }

   /* Free the chunk space */
   FREE(chunk_data);

   /* Print ending log message */
   if (reshape_info->verbose) {
      (void) fprintf(stderr, "Done.\n");
      (void) fflush(stderr);
   }


}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_num_minmax_values
@INPUT      : reshape_info - information for reshaping volume
              block_start - start for a block (or NULL)
              block_count - count for a block
@OUTPUT     : num_min_values - number of image-min values to be read
              num_max_values - number of image-max values to be read
@RETURNS    : (nothing)
@DESCRIPTION: Gets the number of image-min and image-max values that
              correspond to a block. If block_start is NULL, then
              it is assumed to translate to an input start of [0,0,...]. 
              Note that only the true number of values for the specified 
              block is computed (specifying a hyperslab that goes beyond 
              file extents does not give a bigger number of values).
@METHOD     :
@GLOBALS    :
@CALLS      :
@CREATED    : October 25, 1994 (Peter Neelin)
@MODIFIED   :
---------------------------------------------------------------------------- */
public void get_num_minmax_values(Reshape_info *reshape_info,
                                  long *block_start, long *block_count,
                                  long *num_min_values, long *num_max_values)
{
   int iloop, idim, ndims;
   int varid, inimgid;
   long size;
   long minmax_count[MAX_VAR_DIMS];
   long input_block_start[MAX_VAR_DIMS];
   long input_block_count[MAX_VAR_DIMS];
   long *num_values;

   /* Check for icv normalization */
   if (reshape_info->do_icv_normalization) {
      *num_min_values = 0;
      *num_max_values = 0;
      return;
   }

   /* Translate output block count to input count */
   translate_output_to_input(reshape_info, block_start, block_count, 
                             input_block_start, input_block_count);
   if (block_start != NULL) {
      truncate_input_vectors(reshape_info, input_block_start, 
                             input_block_count);
   }
   inimgid = ncvarid(reshape_info->inmincid, MIimage);

   /* Loop over image-min and image-max */

   for (iloop=0; iloop < 2; iloop++) {

      /* Get varid and pointer to return value */
      ncopts = 0;
      switch (iloop) {
      case 0: 
         varid = ncvarid(reshape_info->inmincid, MIimagemin); 
         num_values = num_min_values;        /* Pointer to long */
         break;
      case 1: 
         varid = ncvarid(reshape_info->inmincid, MIimagemax);
         num_values = num_max_values;        /* Pointer to long */
         break;
      }
      ncopts = NCOPTS_DEFAULT;

      /* Translate block count to min or max count and work out the
         total number of values. */
      size = 0;
      if (varid != MI_ERROR) {
         (void) ncvarinq(reshape_info->inmincid, varid, NULL, NULL, 
                         &ndims, NULL, NULL);
         (void) mitranslate_coords(reshape_info->inmincid,
                                   inimgid, input_block_count,
                                   varid, minmax_count);
         size = 1;
         for (idim=0; idim < ndims; idim++) {
            size *= minmax_count[idim];
         }
      }
      *num_values = size;
   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : handle_normalization
@INPUT      : reshape_info - information for reshaping volume
              block_start - start of current block
              block_count - count for current block
              minmax_buffer - buffer space for getting min and max
                 values
@OUTPUT     : fillvalue - pixel fill value to use for this block
@RETURNS    : (none)
@DESCRIPTION: Sets up icv for normalization to ensure that block is
              internally normalized. Output image-max and min are set.
              The appropriate pixel fill value is calculated for this
              min and max (applies to the whole block).
@METHOD     :
@GLOBALS    :
@CALLS      :
@CREATED    : October 25, 1994 (Peter Neelin)
@MODIFIED   :
---------------------------------------------------------------------------- */
public void handle_normalization(Reshape_info *reshape_info,
                                 long *block_start,
                                 long *block_count,
                                 double *minmax_buffer,
                                 double *fillvalue)
{
   int iloop;
   int inmincid, inimgid, varid, icvid;
   long minmax_start[MAX_VAR_DIMS];
   double minimum, maximum, *extreme, valid_min, valid_max, denom;
   char *varname;

   /* Get input minc id, image id and icv id*/
   inmincid = reshape_info->inmincid;
   inimgid = ncvarid(inmincid, MIimage);
   icvid = reshape_info->icvid;

   /* Get input min and max for block */
   get_block_min_and_max(reshape_info, block_start, block_count,
                         minmax_buffer, &minimum, &maximum);

   /* Modify the icv if necessary */
   if (reshape_info->do_block_normalization) {
      (void) miicv_detach(icvid);
      (void) miicv_setdbl(icvid, MI_ICV_IMAGE_MIN, minimum);
      (void) miicv_setdbl(icvid, MI_ICV_IMAGE_MAX, maximum);
      (void) miicv_setint(icvid, MI_ICV_USER_NORM, TRUE);
      (void) miicv_setint(icvid, MI_ICV_DO_NORM, TRUE);
      (void) miicv_attach(icvid, inmincid, inimgid);
   }

   /* Save the image max and min for the block */
   for (iloop=0; iloop < 2; iloop++) {

      /* Get varid and pointer to min or max value */
      switch (iloop) {
      case 0: 
         varname = MIimagemin;
         extreme = &minimum;
         break;
      case 1: 
         varname = MIimagemax;
         extreme = &maximum;
         break;
      }

      /* Save the value */
      ncopts = 0;
      varid = ncvarid(reshape_info->outmincid, varname);
      ncopts = NCOPTS_DEFAULT;
      if (varid != MI_ERROR) {
         (void) mitranslate_coords(reshape_info->outmincid, 
                                   reshape_info->outimgid, block_start,
                                   varid, minmax_start);
         (void) mivarput1(reshape_info->outmincid, varid, 
                          minmax_start, NC_DOUBLE, NULL, extreme);
      }
   }

   /* Calculate the pixel fill value */
   *fillvalue = ((reshape_info->fillvalue == NOFILL) ? 0.0 :
                 reshape_info->fillvalue);
   if ((reshape_info->output_datatype != NC_FLOAT) &&
       (reshape_info->output_datatype != NC_DOUBLE) &&
       (*fillvalue != FILL)) {
      (void) miicv_inqdbl(icvid, MI_ICV_VALID_MIN, &valid_min);
      (void) miicv_inqdbl(icvid, MI_ICV_VALID_MAX, &valid_max);
      denom = maximum - minimum;
      if (denom == 0.0) {
         *fillvalue = valid_min;
      }
      else {
         *fillvalue = (*fillvalue - minimum) * 
            (valid_max - valid_min) / denom + valid_min;
      }
   }


}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_block_min_and_max
@INPUT      : reshape_info - information for reshaping volume
              block_start - start of current block
              block_count - count for current block
              minmax_buffer - buffer space for getting min and max
                 values
@OUTPUT     : minimum - input minimum for block
              maximum - input maximum for block
@RETURNS    : (none)
@DESCRIPTION: Gets the min and max for the input file for a given output
              block.
@METHOD     :
@GLOBALS    :
@CALLS      :
@CREATED    : October 25, 1994 (Peter Neelin)
@MODIFIED   :
---------------------------------------------------------------------------- */
public void get_block_min_and_max(Reshape_info *reshape_info,
                                  long *block_start,
                                  long *block_count,
                                  double *minmax_buffer,
                                  double *minimum,
                                  double *maximum)
{
   int iloop;
   long num_min_values, num_max_values, ivalue;
   int inmincid, inimgid, varid, icvid;
   long minmax_start[MAX_VAR_DIMS], minmax_count[MAX_VAR_DIMS];
   long input_block_start[MAX_VAR_DIMS], input_block_count[MAX_VAR_DIMS];
   double *extreme;
   long num_values;
   char *varname;
   double sign, default_extreme;

   /* Get input minc id, image id and icv id*/
   inmincid = reshape_info->inmincid;
   inimgid = ncvarid(inmincid, MIimage);
   icvid = reshape_info->icvid;

   /* Is the icv doing the normalization? */
   if (reshape_info->do_icv_normalization) {
      (void) miicv_inqdbl(icvid, MI_ICV_NORM_MIN, minimum);
      (void) miicv_inqdbl(icvid, MI_ICV_NORM_MAX, maximum);
      return;
   }

   /* Translate output block count to input count */
   translate_output_to_input(reshape_info, block_start, block_count,
                             input_block_start, input_block_count);
   truncate_input_vectors(reshape_info, input_block_start, input_block_count);

   /* Get number of min and max values */
   get_num_minmax_values(reshape_info, block_start, block_count, 
                         &num_min_values, &num_max_values);

   /* Loop over image-min and image-max getting block min and max */

   for (iloop=0; iloop < 2; iloop++) {

      /* Get varid and pointer to min or max value */
      switch (iloop) {
      case 0: 
         num_values = num_min_values;
         varname = MIimagemin;
         extreme = minimum;
         sign = -1.0;
         default_extreme = 0.0;
         break;
      case 1: 
         num_values = num_max_values;
         varname = MIimagemax;
         extreme = maximum;
         sign = +1.0;
         default_extreme = 1.0;
         break;
      }

      /* Get values from file */
      if (num_values > 0) {
         varid = ncvarid(inmincid, varname);
         (void) mitranslate_coords(inmincid,
                                   inimgid, input_block_start,
                                   varid, minmax_start);
         (void) mitranslate_coords(inmincid,
                                   inimgid, input_block_count,
                                   varid, minmax_count);
         (void) mivarget(reshape_info->inmincid, varid, 
                         minmax_start, minmax_count, 
                         NC_DOUBLE, NULL, minmax_buffer);
         *extreme = minmax_buffer[0];
         for (ivalue=1; ivalue < num_values; ivalue++) {
            if ((minmax_buffer[ivalue] * sign) > (*extreme * sign))
               *extreme = minmax_buffer[ivalue];
         }
      }
      else {
         *extreme = default_extreme;
      }
      if (reshape_info->need_fillvalue && 
          (reshape_info->fillvalue == NOFILL) && 
          (0.0 > (*extreme * sign)))
         *extreme = 0.0;
      
   }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : truncate_input_vectors
@INPUT      : reshape_info - information for reshaping volume
              input_start - start of input hyperslab (or NULL)
              input_count - count for input hyperslab
@OUTPUT     : input_start  - start of input hyperslab (or NULL)
              input_count  - count for input hyperslab
@RETURNS    : (nothing)
@DESCRIPTION: Input_start and input_count are truncated to specify a 
              legal hyperslab for the input file (if not specified, 
              input_start is assumed to be [0,0,...]).
@METHOD     :
@GLOBALS    :
@CALLS      :
@CREATED    : October 25, 1994 (Peter Neelin)
@MODIFIED   :
---------------------------------------------------------------------------- */
public void truncate_input_vectors(Reshape_info *reshape_info,
                                   long *input_start,
                                   long *input_count)
{
   int idim;
   long first, last;        /* last is actually last_index+1 */

   /* Check for NULL vectors */
   if (input_count == NULL) return;

   /* Loop through input dimensions */
   for (idim=0; idim < reshape_info->input_ndims; idim++) {
      first = ( (input_start != NULL) ? input_start[idim] : 0 );
      last = first + input_count[idim];
      if (first < 0)
         first = 0;
      else if (first >= reshape_info->input_size[idim])
         first = reshape_info->input_size[idim] - 1;
      if (last < 0)
         last = 0;
      else if (last >= reshape_info->input_size[idim])
         last = reshape_info->input_size[idim];
      if (input_start != NULL)
         input_start[idim] = first;
      input_count[idim] = last - first;
   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : translate_output_to_input
@INPUT      : reshape_info - information for reshaping volume
              output_start - start of output hyperslab (or NULL)
              output_count - count for output hyperslab
@OUTPUT     : input_start  - start of input hyperslab (or NULL)
              input_count  - count for input hyperslab
@RETURNS    : (nothing)
@DESCRIPTION: Translates an output start and count to an input start and 
              count. If output_start or input_start are NULL, then only the
              count is translated.
@METHOD     :
@GLOBALS    :
@CALLS      :
@CREATED    : October 25, 1994 (Peter Neelin)
@MODIFIED   :
---------------------------------------------------------------------------- */
public void translate_output_to_input(Reshape_info *reshape_info,
                                      long *output_start,
                                      long *output_count,
                                      long *input_start,
                                      long *input_count)
{
   int idim, odim;

   /* Check for NULL vectors */
   if ((input_count == NULL) || (output_count == NULL)) return;

   /* Loop through input dimensions */
   for (idim=0; idim < reshape_info->input_ndims; idim++) {
      odim = reshape_info->map_in_to_out[idim];
      input_count[idim] = ((odim >= 0) ? output_count[odim] : 1);
      if ((input_start != NULL) && (output_start != NULL)) {
         input_start[idim] = reshape_info->input_start[idim];
         if (odim >= 0) {
            if (reshape_info->input_count[idim] > 0)
               input_start[idim] += output_start[odim];
            else
               input_start[idim] -=
                  (output_start[odim] + output_count[odim] - 1);
         }
      }
   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : translate_input_to_output
@INPUT      : reshape_info - information for reshaping volume
              input_start  - start of output hyperslab (or NULL)
              input_count  - count for output hyperslab
@OUTPUT     : output_start - start of input hyperslab (or NULL)
              output_count - count for input hyperslab
@RETURNS    : (nothing)
@DESCRIPTION: Translates an input start and count to an output start and 
              count. If output_start or input_start are NULL, then only the
              count is translated.
@METHOD     :
@GLOBALS    :
@CALLS      :
@CREATED    : October 25, 1994 (Peter Neelin)
@MODIFIED   :
---------------------------------------------------------------------------- */
public void translate_input_to_output(Reshape_info *reshape_info,
                                      long *input_start,
                                      long *input_count,
                                      long *output_start,
                                      long *output_count)
{
   int idim, odim;

   /* Check for NULL vectors */
   if ((input_count == NULL) || (output_count == NULL)) return;

   /* Loop through output dimensions */
   for (odim=0; odim < reshape_info->output_ndims; odim++) {
      idim = reshape_info->map_out_to_in[odim];
      output_count[odim] = input_count[idim];
      if ((input_start != NULL) && (output_start != NULL)) {
         if (reshape_info->input_count[idim] > 0)
            output_start[odim] = input_start[idim] - 
               reshape_info->input_start[idim];
         else
            output_start[odim] = reshape_info->input_start[idim] - 
               (input_start[idim] + input_count[idim] - 1);
      }
   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : copy_the_chunk
@INPUT      : reshape_info - information for reshaping volume
              chunk_start - start of current block
              chunk_count - count for current block
              chunk_data - pointer to enough space for chunk
              fillvalue - pixel value to zero volume, if necessary.
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Copies the chunk from the input file to the output file.
@METHOD     :
@GLOBALS    :
@CALLS      :
@CREATED    : October 25, 1994 (Peter Neelin)
@MODIFIED   :
---------------------------------------------------------------------------- */
public void copy_the_chunk(Reshape_info *reshape_info,
                           long chunk_start[],
                           long chunk_count[],
                           void *chunk_data,
                           double fillvalue)
{
   int idim, odim, in_ndims, out_ndims;
   long input_start[MAX_VAR_DIMS], input_count[MAX_VAR_DIMS];
   long output_start[MAX_VAR_DIMS], output_count[MAX_VAR_DIMS];
   long input_imap[MAX_VAR_DIMS], output_imap[MAX_VAR_DIMS];
   void *output_origin;
   int datatype_size;
   long total_size, ipix, first, last;
   int zero_data, really_copy_the_data;
   union {
      char c; short s; long l; float f; double d;
   } value_buffer;

   /* Get number of dimensions */
   out_ndims = reshape_info->output_ndims;
   in_ndims = reshape_info->input_ndims;

   /* Get size of output datatype */
   datatype_size = nctypelen(reshape_info->output_datatype);

   /* Create input start and count */
   translate_output_to_input(reshape_info, chunk_start, chunk_count,
                             input_start, input_count);

   /* Find out if we need to zero the volume and if we need to copy any
      data */
   zero_data = FALSE;
   really_copy_the_data = TRUE;
   total_size = 1;
   for (idim=0; idim < in_ndims; idim++) {
      first = input_start[idim];
      last = input_start[idim] + input_count[idim] - 1;
      if ((first < 0) || (last >= reshape_info->input_size[idim]))
         zero_data = TRUE;
      if ((last < 0) || (first >= reshape_info->input_size[idim]))
         really_copy_the_data = FALSE;
      total_size *= input_count[idim];
   }

   /* Make sure that input vectors are legal and translate them back 
      to output */
   truncate_input_vectors(reshape_info, input_start, input_count);
   translate_input_to_output(reshape_info, input_start, input_count,
                             output_start, output_count);

   /* Write out zero data if needed */
   if (zero_data) {
      convert_value_from_double(fillvalue, 
                                reshape_info->output_datatype,
                                reshape_info->output_is_signed,
                                &value_buffer);
      for (ipix=0; ipix < total_size; ipix++) {
         (void) memcpy((char *)chunk_data + ipix*datatype_size,
                       &value_buffer, datatype_size);
      }
      (void) ncvarput(reshape_info->outmincid, reshape_info->outimgid,
                      chunk_start, chunk_count, chunk_data);
   }

   /* Set up hypothetical imap variable for input */
   for (idim=in_ndims-1; idim >= 0; idim--) {
      input_imap[idim] = ((idim == in_ndims-1) ? 
                          datatype_size :
                          input_imap[idim+1] * input_count[idim+1]);
   }

   /* Create output imap variable from input one (re-ordering dimensions and
      flipping). Also work out the chunk origin (point to byte for output
      [0,0,0...]). */
   output_origin = chunk_data;
   for (odim=0; odim < out_ndims; odim++) {
      idim = reshape_info->map_out_to_in[odim];
      if (reshape_info->input_count[idim] > 0) {
         output_imap[odim] = input_imap[idim];
      }
      else {
         output_imap[odim] = -input_imap[idim];
         output_origin = 
            (void *) ((char *)output_origin - 
                      (output_count[odim] - 1) * output_imap[odim]);
      }
   }

   /* Should we really copy the data? */
   if (really_copy_the_data) {

      /* Read in the data */
      (void) miicv_get(reshape_info->icvid, input_start, input_count, 
                       chunk_data);
   
      /* Write it out */
      (void) ncvarputg(reshape_info->outmincid, reshape_info->outimgid,
                       output_start, output_count, NULL, output_imap, 
                       output_origin);

   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_value_from_double
@INPUT      : dvalue - double value to convert
              datatype - type of desired value
              is_signed - TRUE if desired value is signed
@OUTPUT     : ptr - pointer to converted value
@RETURNS    : (nothing)
@DESCRIPTION: Converts a value from double to some other value.
@METHOD     :
@GLOBALS    :
@CALLS      :
@CREATED    : October 25, 1994 (Peter Neelin)
@MODIFIED   :
---------------------------------------------------------------------------- */
public void convert_value_from_double(double dvalue, 
                                      nc_type datatype, int is_signed,
                                      void *ptr)
{
   switch (datatype) {
   case NC_BYTE :
      if (!is_signed) {
         dvalue = MAX(0, dvalue);
         dvalue = MIN(UCHAR_MAX, dvalue);
         *((unsigned char *) ptr) = ROUND(dvalue);
      }
      else {
         dvalue = MAX(SCHAR_MIN, dvalue);
         dvalue = MIN(SCHAR_MAX, dvalue);
         *((signed char *) ptr) = ROUND(dvalue);
      }
      break;
   case NC_SHORT :
      if (!is_signed) {
         dvalue = MAX(0, dvalue);
         dvalue = MIN(USHRT_MAX, dvalue);
         *((unsigned short *) ptr) = ROUND(dvalue);
      }
      else {
         dvalue = MAX(SHRT_MIN, dvalue);
         dvalue = MIN(SHRT_MAX, dvalue);
         *((signed short *) ptr) = ROUND(dvalue);
      }
      break;
   case NC_LONG :
      if (!is_signed) {
         dvalue = MAX(0, dvalue);
         dvalue = MIN(ULONG_MAX, dvalue);
         *((unsigned long *) ptr) = ROUND(dvalue);
      }
      else {
         dvalue = MAX(LONG_MIN, dvalue);
         dvalue = MIN(LONG_MAX, dvalue);
         *((signed long *) ptr) = ROUND(dvalue);
      }
      break;
   case NC_FLOAT :
      dvalue = MAX(-FLT_MAX,dvalue);
      *((float *) ptr) = MIN(FLT_MAX,dvalue);
      break;
   case NC_DOUBLE :
      *((double *) ptr) = dvalue;
      break;
   }
}
