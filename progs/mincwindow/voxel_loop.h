/* ----------------------------- MNI Header -----------------------------------
@NAME       : voxel_loop.h
@DESCRIPTION: Header file for voxel_loop.c
@GLOBALS    : 
@CREATED    : January 10, 1994 (Peter Neelin)
@MODIFIED   : $Log: voxel_loop.h,v $
@MODIFIED   : Revision 1.1  1994-01-11 15:16:09  neelin
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

typedef void (*VoxelFunction) (void *voxel_data, long nvoxels, double *data);

public void voxel_loop(int inmincid, int outmincid, char *arg_string,
                       VoxelFunction voxel_function, void *voxel_data);

