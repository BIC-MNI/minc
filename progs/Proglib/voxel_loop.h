/* ----------------------------- MNI Header -----------------------------------
@NAME       : voxel_loop.h
@DESCRIPTION: Header file for voxel_loop.c
@GLOBALS    : 
@CREATED    : January 10, 1994 (Peter Neelin)
@MODIFIED   : $Log: voxel_loop.h,v $
@MODIFIED   : Revision 1.2  1995-03-21 14:06:39  neelin
@MODIFIED   : Improved interface and added lots of functionality (much for the benefit
@MODIFIED   : of mincconcat).
@MODIFIED   :
 * Revision 1.1  94/12/14  10:18:21  neelin
 * Initial revision
 * 
 * Revision 2.0  94/09/28  10:36:28  neelin
 * Release of minc version 0.2
 * 
 * Revision 1.3  94/09/28  10:36:22  neelin
 * Pre-release
 * 
 * Revision 1.2  94/01/12  10:19:19  neelin
 * Added logging. Turned off filling. Added miclose for files.
 * 
 * Revision 1.1  94/01/11  15:16:09  neelin
 * Initial revision
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

#ifndef public
#  define public
#endif
#ifndef private
#  define private static
#endif

/* Includes */
#include <minc.h>

/* Incomplete structure typedefs */

typedef struct Loop_Info Loop_Info;
typedef struct Loop_Options Loop_Options;

/* User function typedefs */

/* ----------------------------- MNI Header -----------------------------------
@NAME       : VoxelFunction
@INPUT      : caller_data - pointer to client data.
              num_voxels - number of voxels to process.
              input_num_buffers - number of input buffers to handle
                 on this call - either total number of input files or 1 
                 (for accumulating over files).
              input_vector_length - length of input vector. This will be
                 1 unless the output vector size is set to something > 0
                 and the input consists of vector volumes (and we're not
                 converting to scalar). See set_loop_output_vector_size.
              input_data - array of pointers to input buffers (1 for
                 each input file, unless we are accumulating).
              output_num_buffers - number of output buffers to handle
                 on this call - will be the total number of output files
                 unless we are accumulating over files (see 
                 set_loop_accumulate).
              output_vector_length - length of output vector = 1 or as set
                 by set_loop_output_vector_size.
              loop_info - pointer that can be passed to functions returning
                 looping information
@OUTPUT     : output_data - array of pointers to output buffers. Set values 
                 to -DBL_MAX to represent illegal, out-of-range values. If 
                 extra buffers are requested by set_loop_accumulate, they will
                 follow the output buffers. 
@RETURNS    : (nothing)
@DESCRIPTION: Typedef for function called by voxel_loop to process data.
---------------------------------------------------------------------------- */
typedef void (*VoxelFunction) 
     (void *caller_data, long num_voxels, 
      int input_num_buffers, int input_vector_length, double *input_data[],
      int output_num_buffers, int output_vector_length, double *output_data[],
      Loop_Info *loop_info);

/* ----------------------------- MNI Header -----------------------------------
@NAME       : VoxelInputFileFunction
@INPUT      : caller_data - pointer to client data.
              input_mincid - mincid for current input file
              input_curfile - current input file number (count from zero)
              loop_info - pointer that can be passed to functions returning
                 looping information
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Typedef for function called by voxel_loop to get information
              for each input file.
---------------------------------------------------------------------------- */
typedef void (*VoxelInputFileFunction) 
     (void *caller_data, int input_mincid, int input_curfile,
      Loop_Info *loop_info);

/* ----------------------------- MNI Header -----------------------------------
@NAME       : VoxelOutputFileFunction
@INPUT      : caller_data - pointer to client data.
              output_mincid - mincid for current output file
              output_curfile - current output file number (count from zero)
              loop_info - pointer that can be passed to functions returning
                 looping information
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Typedef for function called by voxel_loop to modify the header
              of an output file (in define mode)
---------------------------------------------------------------------------- */
typedef void (*VoxelOutputFileFunction) 
     (void *caller_data, int output_mincid, int output_curfile,
      Loop_Info *loop_info);

/* ----------------------------- MNI Header -----------------------------------
@NAME       : VoxelStartFunction
@INPUT      : caller_data - pointer to client data.
              num_voxels - number of voxels to process.
              output_num_buffers - number of output buffers to handle
                 on this call - will be the total number of output files
                 plus the number of extra buffers requested by 
                 set_loop_accumulate.
              output_vector_length - length of output vector = 1 or as set
                 by set_loop_output_vector_size.
              loop_info - pointer that can be passed to functions returning
                 looping information
@OUTPUT     : output_data - array of pointers to output buffers. If 
                 extra buffers are requested by set_loop_accumulate, they will
                 follow the output buffers.
@RETURNS    : (nothing)
@DESCRIPTION: Typedef for function called by voxel_loop to initialize data
              processing when accumulating over files (specified when calling
              set_loop_accumulate).
---------------------------------------------------------------------------- */
typedef void (*VoxelStartFunction) 
     (void *caller_data, long num_voxels,
      int output_num_buffers, int output_vector_length, double *output_data[],
      Loop_Info *loop_info);

/* ----------------------------- MNI Header -----------------------------------
@NAME       : VoxelFinishFunction
@INPUT      : caller_data - pointer to client data.
              num_voxels - number of voxels to process.
              output_num_buffers - number of output buffers to handle
                 on this call - will be the total number of output files
                 plus the number of extra buffers requested by 
                 set_loop_accumulate.
              output_vector_length - length of output vector = 1 or as set
                 by set_loop_output_vector_size.
              loop_info - pointer that can be passed to functions returning
                 looping information
@OUTPUT     : output_data - array of pointers to output buffers. If 
                 extra buffers are requested by set_loop_accumulate, they will
                 follow the output buffers.
@RETURNS    : (nothing)
@DESCRIPTION: Typedef for function called by voxel_loop to finish data
              processing when accumulating over files (specified when calling
              set_loop_accumulate).
---------------------------------------------------------------------------- */
typedef void (*VoxelFinishFunction) 
     (void *caller_data, long num_voxels,
      int output_num_buffers, int output_vector_length, double *output_data[],
      Loop_Info *loop_info);


/* Function declarations */
public void voxel_loop(int num_input_files, char *input_files[], 
                       int num_output_files, char *output_files[], 
                       char *arg_string, 
                       Loop_Options *loop_options,
                       VoxelFunction voxel_function, void *caller_data);
public Loop_Options *create_loop_options(void);
public void free_loop_options(Loop_Options *loop_options);
public void set_loop_clobber(Loop_Options *loop_options, 
                             int clobber);
public void set_loop_verbose(Loop_Options *loop_options, 
                             int verbose);
public void set_loop_datatype(Loop_Options *loop_options, 
                              nc_type datatype, int is_signed,
                              double valid_min, double valid_max);
public void set_loop_max_open_files(Loop_Options *loop_options, 
                                    int max_open_files);
public void set_loop_check_dim_info(Loop_Options *loop_options, 
                                    int check_dim_info);
public void set_loop_convert_input_to_scalar(Loop_Options *loop_options, 
                                             int convert_input_to_scalar);
public void set_loop_output_vector_size(Loop_Options *loop_options, 
                                        int output_vector_size);
public void set_loop_first_input_mincid(Loop_Options *loop_options, 
                                        int input_mincid);
public void set_loop_buffer_size(Loop_Options *loop_options,
                                 long buffer_size);
public void set_loop_dimension(Loop_Options *loop_options,
                               char *dimension_name);
public void set_loop_input_file_function
   (Loop_Options *loop_options,
    VoxelInputFileFunction input_file_function);
public void set_loop_output_file_function
   (Loop_Options *loop_options,
    VoxelOutputFileFunction output_file_function);
public void set_loop_copy_all_header(Loop_Options *loop_options, 
                                     int copy_all_header);
public void set_loop_accumulate(Loop_Options *loop_options, 
                                int do_accumulation,
                                int num_extra_buffers,
                                VoxelStartFunction start_function,
                                VoxelFinishFunction finish_function);
public void get_info_shape(Loop_Info *loop_info, int ndims,
                           long start[], long count[]);
public int get_info_current_file(Loop_Info *loop_info);
public int get_info_current_mincid(Loop_Info *loop_info);
public int get_info_current_index(Loop_Info *loop_info);
public int get_info_whole_file(Loop_Info *loop_info);
