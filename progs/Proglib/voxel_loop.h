/* ----------------------------- MNI Header -----------------------------------
@NAME       : voxel_loop.h
@DESCRIPTION: Header file for voxel_loop.c
@GLOBALS    : 
@CREATED    : January 10, 1994 (Peter Neelin)
@MODIFIED   : $Log: voxel_loop.h,v $
@MODIFIED   : Revision 1.1  1994-12-14 10:18:21  neelin
@MODIFIED   : Initial revision
@MODIFIED   :
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
              start - vector specifying hyperslab start (each buffer of
                 data is a hyperslab from the file)
              count - vector specifying hyperslab count
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
      long start[], long count[]);

/* ----------------------------- MNI Header -----------------------------------
@NAME       : VoxelOutputFileFunction
@INPUT      : caller_data - pointer to client data.
              output_mincid - mincid for current output file
              output_curfile - current output file number (count from zero)
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Typedef for function called by voxel_loop to modify the header
              of an output file (in define mode)
---------------------------------------------------------------------------- */
typedef void (*VoxelOutputFileFunction) 
     (void *caller_data, int output_mincid, int output_curfile);

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
@OUTPUT     : output_data - array of pointers to output buffers. If 
                 extra buffers are requested by set_loop_accumulate, they will
                 follow the output buffers.
              start - vector specifying hyperslab start (each buffer of
                 data is a hyperslab from the file)
              count - vector specifying hyperslab count
@RETURNS    : (nothing)
@DESCRIPTION: Typedef for function called by voxel_loop to initialize data
              processing when accumulating over files (specified when calling
              set_loop_accumulate).
---------------------------------------------------------------------------- */
typedef void (*VoxelStartFunction) 
     (void *caller_data, long num_voxels,
      int output_num_buffers, int output_vector_length, double *output_data[],
      long start[], long count[]);

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
              start - vector specifying hyperslab start (each buffer of
                 data is a hyperslab from the file)
              count - vector specifying hyperslab count
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
      long start[], long count[]);


/* Structure used for option specification - don't touch its internals, use
   appropriate functions to set things */
typedef struct {
   int verbose;
   int clobber;
   int max_open_files;
   int check_all_input_dim_info;
   int convert_input_to_scalar;
   int output_vector_size;        /* 0 = same as input size */
   int input_mincid;
   long total_copy_space;
   VoxelOutputFileFunction output_file_function;
   int do_accumulate;
   int num_extra_buffers;
   VoxelStartFunction start_function;
   VoxelFinishFunction finish_function;
   VoxelFunction voxel_function;
   void *caller_data;
} Loop_Options;


/* Function declarations */
public void voxel_loop(int num_input_files, char *input_files[], 
                       int num_output_files, char *output_files[], 
                       char *arg_string, 
                       Loop_Options *loop_options,
                       VoxelFunction voxel_function, void *caller_data);
public void initialize_loop_options(Loop_Options *loop_options);
public void set_loop_clobber(Loop_Options *loop_options, 
                             int clobber);
public void set_loop_verbose(Loop_Options *loop_options, 
                             int verbose);
public void set_loop_max_open_files(Loop_Options *loop_options, 
                                    int max_open_files);
public void set_loop_convert_input_to_scalar(Loop_Options *loop_options, 
                                             int convert_input_to_scalar);
public void set_loop_output_vector_size(Loop_Options *loop_options, 
                                        int output_vector_size);
public void set_loop_first_input_mincid(Loop_Options *loop_options, 
                                        int input_mincid);
public void set_loop_buffer_size(Loop_Options *loop_options,
                                 long buffer_size);
public void set_loop_output_file_function
   (Loop_Options *loop_options,
    VoxelOutputFileFunction output_file_function);
public void set_loop_accumulate(Loop_Options *loop_options, 
                                int num_extra_buffers,
                                VoxelStartFunction start_function,
                                VoxelFinishFunction finish_function);
