/* ----------------------------- MNI Header -----------------------------------
@NAME       : mincresample
@INPUT      : argc, argv - command line arguments
@OUTPUT     : (none)
@RETURNS    : error status
@DESCRIPTION: Program to resample a minc file along different spatial
              coordinate axes.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 8, 1993 (Peter Neelin)
@MODIFIED   : $Log: mincresample.c,v $
@MODIFIED   : Revision 1.16  1994-09-28 10:32:33  neelin
@MODIFIED   : Pre-release
@MODIFIED   :
 * Revision 1.15  94/03/17  14:12:09  neelin
 * Exit with failure if no argument given for -transformation or -like.
 * .,
 * 
 * Revision 1.14  94/03/15  16:44:21  neelin
 * Changed default from -clobber to -noclobber.
 * 
 * Revision 1.13  93/11/04  15:13:13  neelin
 * Added support for irregularly spaced dimensions.
 * 
 * Revision 1.12  93/11/03  14:32:44  neelin
 * Turn off fill for output file.
 * 
 * Revision 1.11  93/11/03  12:32:17  neelin
 * Change ncopen, nccreate and ncclose to miopen, micreate and miclose.
 * 
 * Revision 1.10  93/11/02  11:23:06  neelin
 * Handle imagemax/min potentially varying over slices (for vector data, etc.)
 * 
 * Revision 1.9  93/10/12  12:47:50  neelin
 * Use volume_io.h instead of def_mni.h
 * 
 * Revision 1.8  93/09/16  09:56:36  neelin
 * Added use of open_file_with_default_suffix in get_transformation to
 * append appropriate suffix for xfm files.
 * 
 * Revision 1.7  93/08/11  14:28:19  neelin
 * Modified get_arginfo and check_imageminmax to modify type of volume (not
 * file) so that output volume gets the input volume type by default when
 * an icv is used on input.
 * 
 * Revision 1.6  93/08/11  13:27:59  neelin
 * Converted to use Dave MacDonald's General_transform code.
 * Fixed bug in get_slice - for non-linear transformations coord was
 * transformed, then used again as a starting coordinate.
 * Handle files that have image-max/min that doesn't vary over slices.
 * Handle files that have image-max/min varying over row/cols.
 * Allow volume to extend to voxel edge for -nearest_neighbour interpolation.
 * Handle out-of-range values (-fill values from a previous mincresample, for
 * example).
 * Save transformation file as a string attribute to processing variable.
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

#ifndef lint
static char rcsid[]="$Header: /private-cvsroot/minc/progs/mincresample/mincresample.c,v 1.16 1994-09-28 10:32:33 neelin Exp $";
#endif

#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include <minc.h>
#include <ParseArgv.h>
#include <time_stamp.h>
#include <volume_io.h>
#include <minc_def.h>
#include "mincresample.h"

/* Main program */

public int main(int argc, char *argv[])
{
   VVolume in_vol_struct, out_vol_struct;
   VVolume *in_vol = &in_vol_struct, *out_vol = &out_vol_struct;
   General_transform transformation;
   Program_Flags program_flags;

   /* Get argument information */
   get_arginfo(argc, argv, &program_flags, in_vol, out_vol, &transformation);

   /* Do the resampling */
   resample_volumes(&program_flags, in_vol, out_vol, &transformation);

   /* Finish up */
   finish_up(in_vol, out_vol);

   exit(EXIT_SUCCESS);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_arginfo
@INPUT      : argc - number of command-line arguments
              argv - command-line arguments
@OUTPUT     : program_flags - data for program execution
              in_vol - description of input volume.
              out_vol - description of output volume.
              transformation - description of world transformation
@RETURNS    : (nothing)
@DESCRIPTION: Routine to get information from arguments about input and 
              output files and transfomation. Sets up all structures
              completely (including allocating space for data).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 8, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void get_arginfo(int argc, char *argv[],
                        Program_Flags *program_flags,
                        VVolume *in_vol, VVolume *out_vol, 
                        General_transform *transformation)
{
   /* Argument parsing information */
   static Arg_Data args={
      FALSE,                  /* Clobber */
      NC_SHORT,               /* Type will be modified anyway */
      INT_MIN,                /* Flag that is_signed has not been set */
      {-DBL_MAX, -DBL_MAX},   /* Flag that range not set */
      FILL_DEFAULT,           /* Flag indicating that fillvalue not set */
      {TRUE},                 /* Verbose */
      trilinear_interpolant,
      {NULL, NULL, 0, NULL},  /* Transformation info is empty at beginning.
                                 Transformation must be set before invoking
                                 argument parsing */
      {
          {2, 1, 0},          /* Axis order */
          {0, 0, 0},          /* nelements will be modified */
          {1.0, 1.0, 1.0},    /* Default step */
          {0.0, 0.0, 0.0},    /* Default start */
          {{1.0, 0.0, 0.0},   /* Default direction cosines */
           {0.0, 1.0, 0.0},
           {0.0, 0.0, 1.0}},
          {NULL, NULL, NULL}, /* Pointers to coordinate arrays */
          {"", "", ""},       /* units */
          {"", "", ""}        /* spacetype */
       }
   };

   static ArgvInfo argTable[] = {
      {"-clobber", ARGV_CONSTANT, (char *) TRUE, 
          (char *) &args.clobber,
          "Overwrite existing file."},
      {"-noclobber", ARGV_CONSTANT, (char *) FALSE, 
          (char *) &args.clobber,
          "Do not overwrite existing file (default)."},
      {"-verbose", ARGV_CONSTANT, (char *) TRUE,
          (char *) &args.flags.verbose,
          "Print out log messages as processing is being done (default).\n"},
      {"-quiet", ARGV_CONSTANT, (char *) FALSE,
          (char *) &args.flags.verbose,
          "Do not print out any log messages.\n"},
      {"-transformation", ARGV_FUNC, (char *) get_transformation, 
          (char *) &args.transform_info,
          "File giving world transformation. (Default = identity)."},
      {"-like", ARGV_FUNC, (char *) get_model_file, 
          (char *) &args.volume_def,
          "Specifies a model file for the resampling."},
      {"-nelements", ARGV_INT, (char *) 3, 
          (char *) args.volume_def.nelements,
          "Number of elements along each dimension (X, Y, Z)"},
      {"-xnelements", ARGV_INT, (char *) 0, 
          (char *) &args.volume_def.nelements[X],
          "Number of elements along the X dimension"},
      {"-ynelements", ARGV_INT, (char *) 0, 
          (char *) &args.volume_def.nelements[Y],
          "Number of elements along the Y dimension"},
      {"-znelements", ARGV_INT, (char *) 0, 
          (char *) &args.volume_def.nelements[Z],
          "Number of elements along the Z dimension"},
      {"-step", ARGV_FLOAT, (char *) 3, 
          (char *) args.volume_def.step,
          "Step size along each dimension (X, Y, Z)"},
      {"-xstep", ARGV_FLOAT, (char *) 0, 
          (char *) &args.volume_def.step[X],
          "Step size along the X dimension"},
      {"-ystep", ARGV_FLOAT, (char *) 0, 
          (char *) &args.volume_def.step[Y],
          "Step size along the Y dimension"},
      {"-zstep", ARGV_FLOAT, (char *) 0, 
          (char *) &args.volume_def.step[Z],
          "Step size along the Z dimension"},
      {"-start", ARGV_FLOAT, (char *) 3, 
          (char *) args.volume_def.start,
          "Start point along each dimension (X, Y, Z)"},
      {"-xstart", ARGV_FLOAT, (char *) 0, 
          (char *) &args.volume_def.start[X],
          "Start point along the X dimension"},
      {"-ystart", ARGV_FLOAT, (char *) 0, 
          (char *) &args.volume_def.start[Y],
          "Start point along the Y dimension"},
      {"-zstart", ARGV_FLOAT, (char *) 0, 
          (char *) &args.volume_def.start[Z],
          "Start point along the Z dimension"},
      {"-dircos", ARGV_FLOAT, (char *) 9, 
          (char *) args.volume_def.dircos,
          "Direction cosines along each dimension (X, Y, Z)"},
      {"-xdircos", ARGV_FLOAT, (char *) 3, 
          (char *) args.volume_def.dircos[X],
          "Direction cosines along the X dimension"},
      {"-ydircos", ARGV_FLOAT, (char *) 3, 
          (char *) args.volume_def.dircos[Y],
          "Direction cosines along the Y dimension"},
      {"-zdircos", ARGV_FLOAT, (char *) 3, 
          (char *) args.volume_def.dircos[Z],
          "Direction cosines along the Z dimension"},
      {"-transverse", ARGV_FUNC, (char *) get_axis_order, 
          (char *) &args.volume_def,
          "Write out transverse slices"},
      {"-sagittal", ARGV_FUNC, (char *) get_axis_order, 
          (char *) &args.volume_def,
          "Write out sagittal slices"},
      {"-coronal", ARGV_FUNC, (char *) get_axis_order, 
          (char *) &args.volume_def,
          "Write out coronal slices"},
      {"-byte", ARGV_CONSTANT, (char *) NC_BYTE, (char *) &args.datatype,
          "Write out byte data"},
      {"-short", ARGV_CONSTANT, (char *) NC_SHORT, (char *) &args.datatype,
          "Write out short integer data"},
      {"-long", ARGV_CONSTANT, (char *) NC_LONG, (char *) &args.datatype,
          "Write out long integer data"},
      {"-float", ARGV_CONSTANT, (char *) NC_FLOAT, (char *) &args.datatype,
          "Write out single-precision floating-point data"},
      {"-double", ARGV_CONSTANT, (char *) NC_DOUBLE, (char *) &args.datatype,
          "Write out double-precision floating-point data"},
      {"-signed", ARGV_CONSTANT, (char *) TRUE, (char *) &args.is_signed,
          "Write signed integer data"},
      {"-unsigned", ARGV_CONSTANT, (char *) FALSE, (char *) &args.is_signed,
          "Write unsigned integer data"},
      {"-range", ARGV_FLOAT, (char *) 2, (char *) args.vrange,
          "Valid range for output data"},
      {"-nofill", ARGV_FUNC, (char *) get_fillvalue, 
          (char *) &args.fillvalue,
          "Use value zero for points outside of input volume"},
      {"-fill", ARGV_FUNC, (char *) get_fillvalue, 
          (char *) &args.fillvalue,
          "Use a fill value for points outside of input volume"},
      {"-fillvalue", ARGV_FLOAT, (char *) 0, 
          (char *) &args.fillvalue,
          "Specify a fill value for points outside of input volume"},
      {"-trilinear", ARGV_CONSTANT, (char *) trilinear_interpolant, 
          (char *) &args.interpolant,
          "Do trilinear interpolation"},
      {"-tricubic", ARGV_CONSTANT, (char *) tricubic_interpolant, 
          (char *) &args.interpolant,
          "Do tricubic interpolation"},
      {"-nearest_neighbour", ARGV_CONSTANT, 
          (char *) nearest_neighbour_interpolant, (char *) &args.interpolant,
          "Do nearest neighbour interpolation"},
      {NULL, ARGV_END, NULL, NULL, NULL}
   };

   /* Other variables */
   int save_argc, iarg, idim, index;
   int in_vindex, out_vindex;  /* Volume indices (0, 1 or 2) */
   int in_findex, out_findex;  /* File indices (0 to ndims-1) */
   long size, total_size;
   char **save_argv, *infile, *outfile;
   File_Info *fp;
   char *tm_stamp, *pname;

   /* Initialize the to identity transformation */
   create_linear_transform(transformation, NULL);
   args.transform_info.transformation = transformation;

   /* Get the time stamp */
   tm_stamp = time_stamp(argc, argv);

   /* Save the default values and the arguments */
   pname=argv[0];
   save_argv=MALLOC((argc+1)*sizeof(*save_argv));
   for (iarg=0; iarg<=argc; iarg++) {
      save_argv[iarg] = argv[iarg];
   }
   save_argc = argc;

   /* Call ParseArgv once to remove all parameters */
   if (ParseArgv(&argc, argv, argTable, 0) || (argc!=3)) {
      (void) fprintf(stderr, 
                     "\nUsage: %s [<options>] <infile> <outfile>\n", pname);
      (void) fprintf(stderr,   
                     "       %s [-help]\n\n", pname);
      exit(EXIT_FAILURE);
   }
   infile = argv[1];
   outfile = argv[2];

   /* Check input file for default argument information */
   in_vol->file = MALLOC(sizeof(File_Info));
   get_file_info(infile, &args.volume_def, in_vol->file);

   /* Save the voxel_to_world transformation information */
   in_vol->voxel_to_world = MALLOC(sizeof(General_transform));
   in_vol->world_to_voxel = MALLOC(sizeof(General_transform));
   get_voxel_to_world_transf(&args.volume_def, in_vol->voxel_to_world);
   create_inverse_general_transform(in_vol->voxel_to_world,
                                    in_vol->world_to_voxel);

   /* Get input volume data information */
   in_vol->slice = NULL;
   in_vol->volume = MALLOC(sizeof(Volume_Data));
   in_vol->volume->datatype = in_vol->file->datatype;
   in_vol->volume->is_signed = in_vol->file->is_signed;
   in_vol->volume->vrange[0] = in_vol->file->vrange[0];
   in_vol->volume->vrange[1] = in_vol->file->vrange[1];
   if (args.fillvalue == FILL_DEFAULT) {
      in_vol->volume->fillvalue = 0.0;
      in_vol->volume->use_fill = TRUE;
   }
   else {
      in_vol->volume->fillvalue = args.fillvalue;
      in_vol->volume->use_fill = FALSE;
   }
   in_vol->volume->interpolant = args.interpolant;

   /* Check min/max variables */
   fp = in_vol->file;
   fp->using_icv=FALSE;
   if ((fp->maxid != MI_ERROR) && (fp->minid != MI_ERROR) &&
       (fp->datatype!=NC_FLOAT) && (fp->datatype!=NC_DOUBLE)) {
      check_imageminmax(fp, in_vol->volume);
   }

   /* Get space for volume data */
   total_size = 1;
   for (idim=0; idim < WORLD_NDIMS; idim++) {
      index = args.volume_def.axes[idim];
      size = args.volume_def.nelements[idim];
      total_size *= size;
      in_vol->volume->size[index] = size;
   }
   in_vol->volume->data = MALLOC((size_t) total_size * 
                                 nctypelen(in_vol->volume->datatype));

   /* Get space for slice scale and offset */
   in_vol->volume->scale = 
      MALLOC(sizeof(double) * in_vol->volume->size[SLC_AXIS]);
   in_vol->volume->offset = 
      MALLOC(sizeof(double) * in_vol->volume->size[SLC_AXIS]);

   /* Save the program flags */
   *program_flags = args.flags;

   /* Set the default output file datatype */
   args.datatype = in_vol->file->datatype;

   /* Call ParseArgv again to get args.volume_def data (and datatype) again */
   if (ParseArgv(&save_argc, save_argv, argTable, 0)) {
      (void) fprintf(stderr, "%s: Argument parsing error!\n", pname);
      exit(EXIT_FAILURE);
   }
   FREE(save_argv);

   /* Explicitly force output files to have regular spacing */
   for (idim=0; idim < WORLD_NDIMS; idim++) {
      if (args.volume_def.coords[idim] != NULL) {
         FREE(args.volume_def.coords[idim]);
         args.volume_def.coords[idim] = NULL;
      }
   }

   /* Check to see if sign and range have been explicitly set. If not set
      them now */
   if (args.is_signed == INT_MIN) {
      if (args.datatype == in_vol->file->datatype)
         args.is_signed = in_vol->file->is_signed;
      else
         args.is_signed = (args.datatype != NC_BYTE);
   }
   if (args.vrange[0] == -DBL_MAX) {
      if ((args.datatype == in_vol->file->datatype) &&
          (args.is_signed == in_vol->file->is_signed)) {
         args.vrange[0] = in_vol->file->vrange[0];
         args.vrange[1] = in_vol->file->vrange[1];
      }
      else {
         args.vrange[0] = get_default_range(MIvalid_min, args.datatype, 
                                            args.is_signed);
         args.vrange[1] = get_default_range(MIvalid_max, args.datatype, 
                                            args.is_signed);
      }
   }

   /* Set up the file description for the output file */
   out_vol->file = MALLOC(sizeof(File_Info));
   out_vol->file->ndims = in_vol->file->ndims;
   out_vol->file->datatype = args.datatype;
   out_vol->file->is_signed = args.is_signed;
   out_vol->file->vrange[0] = args.vrange[0];
   out_vol->file->vrange[1] = args.vrange[1];
   for (idim=0; idim < out_vol->file->ndims; idim++) {
      out_vol->file->nelements[idim] = in_vol->file->nelements[idim];
      out_vol->file->world_axes[idim] = in_vol->file->world_axes[idim];
   }

   /* Get space for output slice */
   out_vol->volume = NULL;
   out_vol->slice = MALLOC(sizeof(Slice_Data));

   /* Loop through list of axes, getting size of volume and slice */
   total_size = 1;
   for (idim=0; idim < WORLD_NDIMS; idim++) {
      
      /* Get the index for input and output volumes */
      in_vindex = in_vol->file->axes[idim];       /* 0, 1 or 2 */
      out_vindex = args.volume_def.axes[idim];    /* 0, 1 or 2 */
      in_findex = in_vol->file->indices[in_vindex];     /* 0 to ndims-1 */
      out_findex = in_vol->file->indices[out_vindex];   /* 0 to ndims-1 */
      size = args.volume_def.nelements[idim];

      /* Update output axes and indices and nelements */
      out_vol->file->nelements[out_findex] = size;
      out_vol->file->world_axes[out_findex] = idim;
      out_vol->file->axes[idim] = out_vindex;
      out_vol->file->indices[out_vindex] = out_findex;

      /* Update slice size */
      if (out_vindex != 0) {
         out_vol->slice->size[out_vindex-1] = size;
         total_size *= size;
      }
   }
   out_vol->slice->data = MALLOC((size_t) total_size * sizeof(double));

   /* Create the output file */
   create_output_file(outfile, args.clobber, &args.volume_def, 
                      in_vol->file, out_vol->file,
                      tm_stamp, &args.transform_info);
   
   /* Save the voxel_to_world transformation information */
   out_vol->voxel_to_world = MALLOC(sizeof(General_transform));
   out_vol->world_to_voxel = MALLOC(sizeof(General_transform));
   get_voxel_to_world_transf(&args.volume_def, out_vol->voxel_to_world);
   create_inverse_general_transform(out_vol->voxel_to_world,
                                    out_vol->world_to_voxel);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : check_imageminmax
@INPUT      : fp - pointer to file description
              volume - pointer to volume description
@OUTPUT     : 
@RETURNS    : (nothing)
@DESCRIPTION: Routine to check that MIimagemax and MIimagemin do not vary
              over volume rows and columns. If they do, set up an icv to
              handle it.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 5, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void check_imageminmax(File_Info *fp, Volume_Data *volume)
{
   int ndims, idim, dim[MAX_VAR_DIMS], imgdim[MAX_VAR_DIMS];
   int ivar, varid;

   /* Get MIimage dimensions */
   (void) ncvarinq(fp->mincid, fp->imgid, NULL, NULL, &ndims, imgdim, NULL);

   /* Check MIimagemax/min dimensions */
   for (ivar=0; ivar<2; ivar++) {
      varid = (ivar==0 ? fp->maxid : fp->minid);
      (void) ncvarinq(fp->mincid, varid, NULL, NULL, &ndims, dim, NULL);
      for (idim=0; idim < ndims; idim++) {
         if ((dim[idim] == imgdim[fp->indices[ROW_AXIS]]) ||
             (dim[idim] == imgdim[fp->indices[COL_AXIS]])) {
            fp->using_icv = TRUE;
         }
      }        /* End loop over MIimagemax/min dimensions */
   }        /* End loop over variables MIimagemax/min */

   /* Set up an icv if needed to handle values varying over slice dims. */
   if (fp->using_icv) {

      /* Change type to floating point so that there is no loss of 
         precision (except possibly for long values). */
      if (volume->datatype != NC_DOUBLE)
         volume->datatype = NC_FLOAT;
      volume->is_signed = TRUE;

      /* Create the icv */
      fp->icvid = miicv_create();
      (void) miicv_setint(fp->icvid, MI_ICV_TYPE, volume->datatype);
      (void) miicv_setstr(fp->icvid, MI_ICV_SIGN, 
                          (volume->is_signed ? MI_SIGNED : MI_UNSIGNED));
      (void) miicv_setint(fp->icvid, MI_ICV_DO_NORM, TRUE);
      (void) miicv_setint(fp->icvid, MI_ICV_DO_FILLVALUE, TRUE);
      (void) miicv_attach(fp->icvid, fp->mincid, fp->imgid);

      /* Get max and min for doing valid range checking */
      (void) miicv_inqdbl(fp->icvid, MI_ICV_NORM_MIN, &volume->vrange[0]);
      (void) miicv_inqdbl(fp->icvid, MI_ICV_NORM_MAX, &volume->vrange[1]);
         
   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_file_info
@INPUT      : filename - name of file to read
@OUTPUT     : volume_def - description of volume
              file_info - description of file
@RETURNS    : (nothing)
@DESCRIPTION: Routine to get information about the volume definition of
              a minc file. 
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 9, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void get_file_info(char *filename, 
                          Volume_Definition *volume_def,
                          File_Info *file_info)
{
   int dim[MAX_VAR_DIMS], dimid, status, length;
   int axis_counter, idim, jdim, cur_axis;
   int varndims, vardim[MAX_VAR_DIMS];
   long varstart, varcount, dimlength;
   char attstr[MI_MAX_ATTSTR_LEN];
   char dimname[MAX_NC_NAME];
   double vrange[2];
   enum {UNKNOWN, REGULAR, IRREGULAR} coord_spacing;

   /* Open the minc file */
   file_info->mincid = miopen(filename, NC_NOWRITE);
   file_info->name = filename;

   /* Get variable identifiers */
   file_info->imgid = ncvarid(file_info->mincid, MIimage);
   ncopts = 0;
   file_info->maxid = ncvarid(file_info->mincid, MIimagemax);
   file_info->minid = ncvarid(file_info->mincid, MIimagemin);
   ncopts = NC_VERBOSE | NC_FATAL;

   /* Get information about datatype dimensions of variable */
   (void) ncvarinq(file_info->mincid, file_info->imgid, NULL, 
                   &file_info->datatype, &file_info->ndims, dim, NULL);

   /* Get sign attriubte */
   if (file_info->datatype == NC_BYTE)
      file_info->is_signed = FALSE;
   else
      file_info->is_signed = TRUE;
   ncopts = 0;
   if ((miattgetstr(file_info->mincid, file_info->imgid, MIsigntype, 
                    MI_MAX_ATTSTR_LEN, attstr) != NULL)) {
      if (strcmp(attstr, MI_SIGNED) == 0)
         file_info->is_signed = TRUE;
      else if (strcmp(attstr, MI_UNSIGNED) == 0)
         file_info->is_signed = FALSE;
   }
   ncopts = NC_VERBOSE | NC_FATAL;

   /* Get valid max and min */
   ncopts = 0;
   status=miattget(file_info->mincid, file_info->imgid, MIvalid_range, 
                   NC_DOUBLE, 2, vrange, &length);
   if ((status!=MI_ERROR) && (length==2)) {
      if (vrange[1] > vrange[0]) {
         file_info->vrange[0] = vrange[0];
         file_info->vrange[1] = vrange[1];
      }
      else {
         file_info->vrange[0] = vrange[1];
         file_info->vrange[1] = vrange[0];
      }
   }
   else {
      status=miattget1(file_info->mincid, file_info->imgid, MIvalid_max, 
                       NC_DOUBLE, &(file_info->vrange[1]));
      if (status==MI_ERROR)
         file_info->vrange[1] =
            get_default_range(MIvalid_max, file_info->datatype, 
                              file_info->is_signed);
  
      status=miattget1(file_info->mincid, file_info->imgid, MIvalid_min, 
                       NC_DOUBLE, &(file_info->vrange[0]));
      if (status==MI_ERROR)
         file_info->vrange[0] =
            get_default_range(MIvalid_min, file_info->datatype, 
                              file_info->is_signed);
   }
   ncopts = NC_VERBOSE | NC_FATAL;

   /* Set variables for keeping track of spatial dimensions */
   axis_counter = 0;                   /* Keeps track of values for axes */

   /* Initialize volume definition variables */
   for (idim=0; idim < WORLD_NDIMS; idim++) {
      volume_def->axes[idim] = NO_AXIS;
      volume_def->step[idim] = 1.0;
      volume_def->start[idim] = 0.0;
      for (jdim=0; jdim < WORLD_NDIMS; jdim++) {
         if (jdim==idim)
            volume_def->dircos[idim][jdim] = 1.0;
         else
            volume_def->dircos[idim][jdim] = 0.0;
      }
      if (volume_def->coords[idim] != NULL) {
         FREE(volume_def->coords[idim]);
         volume_def->coords[idim] = NULL;
      }
      (void) strcpy(volume_def->units[idim], "mm");
      (void) strcpy(volume_def->spacetype[idim], MI_NATIVE);
   }

   /* Loop through dimensions, getting dimension information */

   for (idim=0; idim < file_info->ndims; idim++) {

      /* Get size of dimension */
      (void) ncdiminq(file_info->mincid, dim[idim], dimname, 
                      &file_info->nelements[idim]);

      /* Check variable name */
      cur_axis = NO_AXIS;
      if (strcmp(dimname, MIxspace)==0)
         cur_axis = XAXIS;
      else if (strcmp(dimname, MIyspace)==0)
         cur_axis = YAXIS;
      else if (strcmp(dimname, MIzspace)==0)
         cur_axis = ZAXIS;

      /* Save world axis info */
      file_info->world_axes[idim] = cur_axis;

      /* Check for spatial dimension */
      if (cur_axis == NO_AXIS) continue;

      /* Set axis */
      if (volume_def->axes[cur_axis] != NO_AXIS) {
         (void) fprintf(stderr, "Repeated spatial dimension %s in file %s.\n",
                 dimname, filename);
         exit(EXIT_FAILURE);
      }
      volume_def->axes[cur_axis] = axis_counter++;

      /* Save spatial axis specific info */
      file_info->axes[cur_axis] = volume_def->axes[cur_axis];
      file_info->indices[volume_def->axes[cur_axis]] = idim;
      volume_def->nelements[cur_axis] = file_info->nelements[idim];

      /* Check for existence of variable */
      ncopts = 0;
      dimid = ncvarid(file_info->mincid, dimname);
      ncopts = NC_VERBOSE | NC_FATAL;
      if (dimid == MI_ERROR) continue;
             
      /* Get attributes from variable */
      ncopts = 0;
      (void) miattget1(file_info->mincid, dimid, MIstep, 
                       NC_DOUBLE, &volume_def->step[cur_axis]);
      if (volume_def->step[cur_axis] == 0.0)
         volume_def->step[cur_axis] = 1.0;
      (void) miattget1(file_info->mincid, dimid, MIstart, 
                       NC_DOUBLE, &volume_def->start[cur_axis]);
      (void) miattget(file_info->mincid, dimid, MIdirection_cosines, 
                      NC_DOUBLE, WORLD_NDIMS, 
                      volume_def->dircos[cur_axis], NULL);
      (void) miattgetstr(file_info->mincid, dimid, MIunits, 
                         MI_MAX_ATTSTR_LEN, volume_def->units[cur_axis]);
      (void) miattgetstr(file_info->mincid, dimid, MIspacetype, 
                         MI_MAX_ATTSTR_LEN, volume_def->spacetype[cur_axis]);
      ncopts = NC_VERBOSE | NC_FATAL;

      /* Look for irregular coordinates for dimension variable */
      ncopts = 0;
      coord_spacing = UNKNOWN;
      dimlength = volume_def->nelements[cur_axis];
      if (miattgetstr(file_info->mincid, dimid, MIspacing, MI_MAX_ATTSTR_LEN,
                       attstr) != NULL) {
         if (strcmp(attstr, MI_IRREGULAR) == 0)
            coord_spacing = IRREGULAR;
         else if (strcmp(attstr, MI_REGULAR) == 0)
            coord_spacing = REGULAR;
      }
      if (ncvarinq(file_info->mincid, dimid, NULL, NULL, 
                   &varndims, vardim, NULL) == MI_ERROR) {
         ncopts = NC_VERBOSE | NC_FATAL;
         continue;
      }
      if ((coord_spacing != REGULAR) && 
          (varndims == 1) && (vardim[0] == dim[idim])) {
         coord_spacing = IRREGULAR;
      }
      if ((coord_spacing == UNKNOWN) || (dimlength <= 1)) {
         coord_spacing = REGULAR;
      }
      if (coord_spacing == IRREGULAR) {
         volume_def->coords[cur_axis] = MALLOC(sizeof(double) * dimlength);
         varstart = 0;
         varcount = dimlength;
         if (mivarget(file_info->mincid, dimid, &varstart, &varcount,
                      NC_DOUBLE, MI_SIGNED, volume_def->coords[cur_axis])
                   == MI_ERROR) {
            ncopts = NC_VERBOSE | NC_FATAL;
            FREE(volume_def->coords[cur_axis]);
            volume_def->coords[cur_axis] = NULL;
            continue;
         }
         volume_def->start[cur_axis] = volume_def->coords[cur_axis][0];
         if (dimlength > 1) {
            volume_def->step[cur_axis] = 
               (volume_def->coords[cur_axis][dimlength-1] - 
                            volume_def->coords[cur_axis][0]) /
                               (dimlength - 1);
            if (volume_def->step[cur_axis] == 0.0)
               volume_def->step[cur_axis] = 1.0;
         }
      }
      ncopts = NC_VERBOSE | NC_FATAL;

   }   /* End of loop over dimensions */

   /* Check that we have the correct number of spatial dimensions */
   if (axis_counter != WORLD_NDIMS) {
      (void) fprintf(stderr, 
                     "Incorrect number of spatial dimensions in file %s.\n",
                     filename);
         exit(EXIT_FAILURE);
   }

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_output_file
@INPUT      : filename - name of file to create
              clobber - flag indicating whether any existing file should be
                 overwritten
              volume_def - description of volume
              in_file - description of input file
              out_file - description of output file
              tm_stamp - string describing program invocation
              transformation - transformation to be applied to data
@OUTPUT     : (nothing) 
@RETURNS    : (nothing)
@DESCRIPTION: Routine to create an minc output file and set up its parameters
              properly.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 9, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void create_output_file(char *filename, int clobber, 
                               Volume_Definition *volume_def,
                               File_Info *in_file,
                               File_Info *out_file,
                               char *tm_stamp, 
                               Transform_Info *transform_info)
{
   int ndims, in_dims[MAX_VAR_DIMS], out_dims[MAX_VAR_DIMS];
   char dimname[MAX_NC_NAME];
   int nmaxmin_dims, nimage_dims, axis, idim, dimid, varid, itrans;
   int att_length;
   nc_type datatype;
   int nexcluded, excluded_vars[10];
   char *string;
   int dim_exists, is_volume_dimension;
   int in_index, out_index;

   /* Save the file name */
   out_file->name = filename;

   /* Create the list of excluded variables */
   nexcluded = 0;
   ncopts = 0;
   if ((varid=ncvarid(in_file->mincid, MIxspace)) != MI_ERROR)
      excluded_vars[nexcluded++] = varid;
   if ((varid=ncvarid(in_file->mincid, MIyspace)) != MI_ERROR)
      excluded_vars[nexcluded++] = varid;
   if ((varid=ncvarid(in_file->mincid, MIzspace)) != MI_ERROR)
      excluded_vars[nexcluded++] = varid;
   if ((varid=ncvarid(in_file->mincid, MIimage)) != MI_ERROR)
      excluded_vars[nexcluded++] = varid;
   if ((varid=ncvarid(in_file->mincid, MIimagemax)) != MI_ERROR)
      excluded_vars[nexcluded++] = varid;
   if ((varid=ncvarid(in_file->mincid, MIimagemin)) != MI_ERROR)
      excluded_vars[nexcluded++] = varid;
   ncopts = NC_VERBOSE | NC_FATAL;

   /* Create the file */
   out_file->mincid = micreate(filename, 
                               (clobber ? NC_CLOBBER : NC_NOCLOBBER));
 
   /* Copy all other variable definitions */
   (void) micopy_all_var_defs(in_file->mincid, out_file->mincid, 
                              nexcluded, excluded_vars);

   /* Add the time stamp */
   ncopts=0;
   if ((ncattinq(out_file->mincid, NC_GLOBAL, MIhistory, &datatype,
                 &att_length) == MI_ERROR) ||
       (datatype != NC_CHAR))
      att_length = 0;
   att_length += strlen(tm_stamp) + 1;
   string = MALLOC(att_length);
   string[0] = '\0';
   (void) miattgetstr(out_file->mincid, NC_GLOBAL, MIhistory, att_length, 
                      string);
   ncopts=NC_VERBOSE | NC_FATAL;
   (void) strcat(string, tm_stamp);
   (void) miattputstr(out_file->mincid, NC_GLOBAL, MIhistory, string);
   FREE(string);

   /* Get the dimension ids from the input file */
   (void) ncvarinq(in_file->mincid, in_file->imgid, NULL, NULL, 
                   &ndims, in_dims, NULL);

   /* Check for screw-up on number of dimensions */
   if (ndims != out_file->ndims) {
      (void) fprintf(stderr, 
                     "Error in number of dimensions for output file.\n");
      exit(EXIT_FAILURE);
   }

   /* Create the dimensions for the image variable */
   for (out_index=0; out_index<ndims; out_index++) {

      /* Check to see if this is a volume dimension */
      is_volume_dimension = (out_file->world_axes[out_index] != NO_AXIS);

      /* Get the input index */
      if (!is_volume_dimension) 
         in_index = out_index;
      else {
         axis = out_file->world_axes[out_index];
         if ((axis<0) || (axis>=WORLD_NDIMS)) {
            (void) fprintf(stderr,
                           "Error creating dimensions for output file.\n");
            exit(EXIT_FAILURE);
         }
         in_index = in_file->indices[in_file->axes[axis]];
      }

      /* Get the dimension name from the input file */
      (void) ncdiminq(in_file->mincid, in_dims[in_index], dimname, NULL);

      /* Check to see if the dimension already exists */
      ncopts = 0;
      out_dims[out_index] = ncdimid(out_file->mincid, dimname);
      ncopts = NC_VERBOSE | NC_FATAL;
      dim_exists = (out_dims[out_index] != MI_ERROR);

      /* If we have a volume dimension and it exists already with the wrong
         size, then we must rename it */
      if (is_volume_dimension && dim_exists && 
          (out_file->nelements[out_index] != in_file->nelements[in_index])) {
         string = MALLOC(MAX_NC_NAME);
         ncopts = 0;
         idim = 0;
         do {
            (void) sprintf(string, "%s%d", dimname, idim);
            idim++;
         } while (ncdimid(out_file->mincid, string) != MI_ERROR);
         ncopts = NC_VERBOSE | NC_FATAL;
         (void) ncdimrename(out_file->mincid, out_dims[out_index], string);
         FREE(string);
         out_dims[out_index] = ncdimdef(out_file->mincid, dimname, 
                                        out_file->nelements[out_index]);
      }
      else if (!dim_exists)
         out_dims[out_index] = ncdimdef(out_file->mincid, dimname, 
                                        out_file->nelements[out_index]);

      /* If this is a volume dimension, create a variable */
      if (is_volume_dimension) {

         /* Create the variable */
         dimid = micreate_group_variable(out_file->mincid, dimname);
         (void) miattputdbl(out_file->mincid, dimid, MIstep, 
                            volume_def->step[axis]);
         (void) miattputdbl(out_file->mincid, dimid, MIstart, 
                            volume_def->start[axis]);
         (void) ncattput(out_file->mincid, dimid, MIdirection_cosines, 
                         NC_DOUBLE, WORLD_NDIMS, volume_def->dircos[axis]);
         (void) miattputstr(out_file->mincid, dimid, MIunits, 
                            volume_def->units[axis]);
         (void) miattputstr(out_file->mincid, dimid, MIspacetype, 
                            volume_def->spacetype[axis]);

      }       /* If volume dimension */
   }       /* Loop over dimensions */

   /* Create the image variable */
   out_file->imgid = micreate_std_variable(out_file->mincid, MIimage, 
                                           out_file->datatype,
                                           ndims, out_dims);
   (void) micopy_all_atts(in_file->mincid, in_file->imgid,
                          out_file->mincid, out_file->imgid);
   (void) miattputstr(out_file->mincid, out_file->imgid, MIcomplete,
                      MI_FALSE);
   (void) ncattput(out_file->mincid, out_file->imgid, MIvalid_range, 
                   NC_DOUBLE, 2, out_file->vrange);
   if (out_file->is_signed)
      (void) miattputstr(out_file->mincid, out_file->imgid,
                         MIsigntype, MI_SIGNED);
   else
      (void) miattputstr(out_file->mincid, out_file->imgid,
                         MIsigntype, MI_UNSIGNED);

   /* Create the image max and min variables. These do not vary over
      the volume rows and columns even if they are not image dimensions,
      so we have to copy down the elements of the array (excluding image
      dimensions). Compute number of slices (row,columns) in an image
      (minc file def'n) and number of images in the file. */
   nimage_dims = 2;
   ncdiminq(out_file->mincid, out_dims[ndims-1], dimname, NULL);
   if (strcmp(dimname, MIvector_dimension)==0)
      nimage_dims++;
   nmaxmin_dims = 0;
   out_file->slices_per_image = 1;
   out_file->images_per_file = 1;
   for (idim=0; idim<ndims; idim++) {
      if ((idim != out_file->indices[COL_AXIS]) &&
          (idim != out_file->indices[ROW_AXIS])) {
         if (idim < ndims-nimage_dims) {
            out_dims[nmaxmin_dims] = out_dims[idim];
            nmaxmin_dims++;
            out_file->images_per_file *= out_file->nelements[idim];
         }
         else {
            out_file->slices_per_image *= out_file->nelements[idim];
         }
      }
   }
   out_file->do_slice_renormalization =
      ((out_file->datatype != NC_FLOAT) && 
       (out_file->datatype != NC_DOUBLE) &&
       (out_file->slices_per_image > 1));

   /* Create the variables */
   out_file->maxid = micreate_std_variable(out_file->mincid, MIimagemax,
                                           NC_DOUBLE, nmaxmin_dims, out_dims);
   if (in_file->maxid != MI_ERROR)
      (void) micopy_all_atts(in_file->mincid, in_file->maxid,
                             out_file->mincid, out_file->maxid);
   out_file->minid = micreate_std_variable(out_file->mincid, MIimagemin,
                                           NC_DOUBLE, nmaxmin_dims, out_dims);
   if (in_file->minid != MI_ERROR)
      (void) micopy_all_atts(in_file->mincid, in_file->minid,
                             out_file->mincid, out_file->minid);

   /* Add transformation information to image processing variable if 
      a transformation is given on the command line */

   if (transform_info->file_name != NULL) {

      ncopts = 0;

      /* Get id of processing variable (create it if needed) */
      varid = ncvarid(out_file->mincid, PROCESSING_VAR);
      if (varid == MI_ERROR) {
         varid = ncvardef(out_file->mincid, PROCESSING_VAR, NC_LONG, 0, NULL);
         (void) miadd_child(out_file->mincid, 
                            ncvarid(out_file->mincid, MIrootvariable), varid);
      }

      /* Look for an unused transformation attribute */
      string = MALLOC(MI_MAX_ATTSTR_LEN);
      itrans = 0;
      do {
         (void) sprintf(string, "transformation%d-filename", itrans);
         itrans++;
      } while (ncattinq(out_file->mincid, varid, string,
                        NULL, NULL) != MI_ERROR);
      itrans--;

      /* Reset error handling */
      ncopts = NC_VERBOSE | NC_FATAL;

      /* Add the attributes describing the transformation */
      (void) miattputstr(out_file->mincid, varid, string, 
                         transform_info->file_name);
      (void) sprintf(string, "transformation%d-filedata", itrans);
      (void) miattputstr(out_file->mincid, varid, string,
                         transform_info->file_contents);
   }         /* If transform specified on command line */

   /* Get into data mode */
   (void) ncsetfill(out_file->mincid, NC_NOFILL);
   (void) ncendef(out_file->mincid);

   /* Copy all the other data */
   (void) micopy_all_var_values(in_file->mincid, out_file->mincid,
                                nexcluded, excluded_vars);

   /* Create and attach an icv */
   out_file->using_icv = TRUE;
   out_file->icvid = miicv_create();
   (void) miicv_setint(out_file->icvid, MI_ICV_TYPE, NC_DOUBLE);
   (void) miicv_setint(out_file->icvid, MI_ICV_DO_NORM, TRUE);
   (void) miicv_setint(out_file->icvid, MI_ICV_USER_NORM, TRUE);
   (void) miicv_attach(out_file->icvid, out_file->mincid, out_file->imgid);

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_voxel_to_world_transf
@INPUT      : volume_def - description of volume
@OUTPUT     : voxel_to_world - transformation
@RETURNS    : (nothing)
@DESCRIPTION: Routine to convert a Volume definition specification of sampling
              to a voxel-to-world transformation
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 9, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void get_voxel_to_world_transf(Volume_Definition *volume_def, 
                                      General_transform *voxel_to_world)
{
   int idim, jdim, cur_dim, vol_axis;
   int irregular;
   long ielement, dimlength;
   Transform matrix;
   Irregular_Transform_Data *irreg_transf_data;
   General_transform linear_transf, irreg_transf;

   /* Make an identity matrix */
   make_identity_transform(&matrix);

   /* Loop over rows of matrix */
   for (idim=0; idim<WORLD_NDIMS; idim++) {

      /* Loop over columns of matrix */
      for (jdim=0; jdim<WORLD_NDIMS; jdim++) {
         cur_dim = volume_def->axes[jdim];

         /* Get rotation/scale components of matrix */
         Transform_elem(matrix, idim, cur_dim) = 
            volume_def->step[jdim] * volume_def->dircos[jdim][idim];

         /* Get translation components */
         Transform_elem(matrix, idim, VOL_NDIMS) +=
            volume_def->start[jdim] * volume_def->dircos[jdim][idim];
      }
   }

   /* Save the general transform */
   create_linear_transform(voxel_to_world, &matrix);

   /* Check for an irregularly spaced dimension */
   irregular = FALSE;
   for (idim=0; idim < WORLD_NDIMS; idim++) {
      if (volume_def->coords[idim] != NULL)
         irregular = TRUE;
   }

   /* If we have an irregularly spaced dimension, then create the appropriate
      transform */
   if (irregular) {
      irreg_transf_data = MALLOC(sizeof(*irreg_transf_data));

      /* Loop through the axes */
      for (idim=0; idim < WORLD_NDIMS; idim++) {

         vol_axis = volume_def->axes[idim];
         irreg_transf_data->last_index[vol_axis] = 0;

         /* Check whether the axis is irregularly spaced or not */
         if (volume_def->coords[idim] == NULL) {
            irreg_transf_data->nelements[vol_axis] = 0;
            irreg_transf_data->coords[vol_axis] = NULL;
         }
         else {

            /* If irregular then get the number of elements and
               allocate space */
            dimlength = volume_def->nelements[idim];
            irreg_transf_data->nelements[vol_axis] = dimlength;
            irreg_transf_data->coords[vol_axis] =
               MALLOC(sizeof(double) * dimlength);

            /* Normalize the coordinates to give first coord 0 and
               last coord n-1 (so that we can concat with the linear
               transform already created */
            for (ielement=0; ielement < dimlength; ielement++) {
               irreg_transf_data->coords[vol_axis][ielement] = 
                  (volume_def->coords[idim][ielement] - 
                              volume_def->start[idim]) /
                                 volume_def->step[idim];
            }
         }
      }

      /* Create an irregular transform and free the data (we only free
         the Irregular_Transform_Data structure, not the coord pointers,
         since these arrays are not copied) */
      create_user_transform(&irreg_transf, (void *) irreg_transf_data,
                            sizeof(*irreg_transf_data),
                            irregular_transform_function,
                            irregular_inverse_transform_function);
      FREE(irreg_transf_data);

      /* Concatenate the linear transform with the irregular transform */
      copy_general_transform(voxel_to_world, &linear_transf);
      delete_general_transform(voxel_to_world);
      concat_general_transforms(&irreg_transf, &linear_transf,
                                voxel_to_world);

   }

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : irregular_transform_function
@INPUT      : user_data - pointer to user data
              x, y, z   - coordinate to transform
@OUTPUT     : x_trans, y_trans, z_trans - resulting coordinate
@RETURNS    : (nothin)
@DESCRIPTION: Routine to transform irregularly spaced coordinate to a 
              regular spacing.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 4, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void irregular_transform_function(void *user_data,
                                         Real x,
                                         Real y,
                                         Real z,
                                         Real *x_trans,
                                         Real *y_trans,
                                         Real *z_trans)
{
   Irregular_Transform_Data *irreg_transf_data;
   int idim;
   long dimlength, index;
   double coord[VOL_NDIMS], coord_transf[VOL_NDIMS];
   double frac;

   /* Get the pointer to the data */
   irreg_transf_data = (Irregular_Transform_Data *) user_data;

   /* Get the coordinate */
   coord[0] = x; coord[1] = y; coord[2] = z;

   /* Loop through axes, renormalizing coordinate */
   for (idim=0; idim < VOL_NDIMS; idim++) {
      dimlength = irreg_transf_data->nelements[idim];
      if (dimlength <= 1) {
         coord_transf[idim] = coord[idim];
      }
      else {

         /* For irregular dimension, do linear interpolation between coords */
         index = (long) coord[idim];
         if (index < 0) index = 0;
         if (index > dimlength-2) index = dimlength-2;
         frac = coord[idim] - index;
         coord_transf[idim] = 
            (1.0 - frac) * irreg_transf_data->coords[idim][index] + 
            frac * irreg_transf_data->coords[idim][index + 1];

      }
   }

   /* Save the coordinates */
   *x_trans = coord_transf[0];
   *y_trans = coord_transf[1];
   *z_trans = coord_transf[2];

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : irregular_inverse_transform_function
@INPUT      : user_data - pointer to user data
              x, y, z   - coordinate to transform
@OUTPUT     : x_trans, y_trans, z_trans - resulting coordinate
@RETURNS    : (nothin)
@DESCRIPTION: Routine to transform irregularly spaced coordinate to a 
              regular spacing.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 4, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void irregular_inverse_transform_function(void *user_data,
                                                 Real x,
                                                 Real y,
                                                 Real z,
                                                 Real *x_trans,
                                                 Real *y_trans,
                                                 Real *z_trans)
{
   Irregular_Transform_Data *irreg_transf_data;
   int idim, not_found;
   long dimlength, index;
   double coord[VOL_NDIMS], coord_transf[VOL_NDIMS];
   double first, next, step, frac;

   /* Get the pointer to the data */
   irreg_transf_data = (Irregular_Transform_Data *) user_data;

   /* Get the coordinate */
   coord[0] = x; coord[1] = y; coord[2] = z;

   /* Loop through axes, renormalizing coordinate */
   for (idim=0; idim < VOL_NDIMS; idim++) {
      dimlength = irreg_transf_data->nelements[idim];
      if (dimlength <= 1) {
         coord_transf[idim] = coord[idim];
      }
      else {

         /* Search for the closest index (checking for out-of-range values) */
         index = irreg_transf_data->last_index[idim];
         if (index < 0) index = 0;
         if (index > dimlength-2) index = dimlength-2;
         not_found = TRUE;
         while (not_found) {
            if (coord[idim] < irreg_transf_data->coords[idim][index]) {
               if (index > 0) index--;
               else {
                  index = 0;
                  not_found = FALSE;
               }
            }
            else if (coord[idim] > irreg_transf_data->coords[idim][index+1]) {
               if (index < dimlength-2) index++;
               else {
                  index = dimlength-2;
                  not_found = FALSE;
               }
            }
            else {
               not_found = FALSE;
            }
            
         }
         irreg_transf_data->last_index[idim] = index;

         /* Do linear interpolation */
         first = irreg_transf_data->coords[idim][index];
         next = irreg_transf_data->coords[idim][index + 1];
         step = next - first;
         if (step == 0.0)
            frac = 0.0;
         else
            frac = (coord[idim] - first) / step;
         coord_transf[idim] = 
            (1.0 - frac) * index + frac * (index + 1);

      }
   }

   /* Save the coordinates */
   *x_trans = coord_transf[0];
   *y_trans = coord_transf[1];
   *z_trans = coord_transf[2];

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_default_range
@INPUT      : what     - MIvalid_min means get default min, MIvalid_min means 
                 get default min
              datatype - type of variable
              is_signed   - TRUE if variable is signed
@OUTPUT     : (none)
@RETURNS    : default maximum or minimum for the datatype
@DESCRIPTION: Return the defaults maximum or minimum for a given datatype
              and sign.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 10, 1992 (Peter Neelin)
@MODIFIED   : February 10, 1993 (Peter Neelin)
                 - ripped off from MINC code
---------------------------------------------------------------------------- */
public double get_default_range(char *what, nc_type datatype, int is_signed)
{
   double limit;

   if (strcmp(what, MIvalid_max)==0) {
      switch (datatype) {
      case NC_LONG:  limit = (is_signed) ? LONG_MAX : ULONG_MAX; break;
      case NC_SHORT: limit = (is_signed) ? SHRT_MAX : USHRT_MAX; break;
      case NC_BYTE:  limit = (is_signed) ? SCHAR_MAX : UCHAR_MAX; break;
      default: limit = DEFAULT_MAX; break;
      }
   }
   else if (strcmp(what, MIvalid_min)==0) {
      switch (datatype) {
      case NC_LONG:  limit = (is_signed) ? LONG_MIN : 0; break;
      case NC_SHORT: limit = (is_signed) ? SHRT_MIN : 0; break;
      case NC_BYTE:  limit = (is_signed) ? SCHAR_MIN : 0; break;
      default: limit = DEFAULT_MIN; break;
      }
   }
   else {
      limit = 0.0;
   }

   return limit;
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : finish_up
@INPUT      : in_vol - input volume
              out_vol - output volume
@OUTPUT     : (nothing) 
@RETURNS    : (nothing)
@DESCRIPTION: Routine to finish up at end of program, closing files, etc.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 15, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void finish_up(VVolume *in_vol, VVolume *out_vol)
{
   File_Info *in_file, *out_file;

   /* Get file info pointers */
   in_file = in_vol->file;
   out_file = out_vol->file;

   /* Close the output file */
   (void) miattputstr(out_file->mincid, out_file->imgid, MIcomplete, MI_TRUE);
   if (out_file->using_icv) {
      (void) miicv_free(out_file->icvid);
   }
   (void) miclose(out_file->mincid);

   /* Close the input file */
   if (in_file->using_icv) {
      (void) miicv_free(in_file->icvid);
   }
   (void) miclose(in_file->mincid);

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_transformation
@INPUT      : dst - Pointer to client data from argument table
              key - argument key
              nextArg - argument following key
@OUTPUT     : (nothing) 
@RETURNS    : TRUE so that ParseArgv will discard nextArg, unless there
              is no following argument.
@DESCRIPTION: Routine called by ParseArgv to read in a transformation file
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 15, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int get_transformation(char *dst, char *key, char *nextArg)
{     /* ARGSUSED */
   Transform_Info *transform_info;
   General_transform *transformation;
   General_transform input_transformation;
   FILE *fp;
   int ch, index;

   /* Check for following argument */
   if (nextArg == NULL) {
      (void) fprintf(stderr, 
                     "\"%s\" option requires an additional argument\n",
                     key);
      exit(EXIT_FAILURE);
   }

   /* Get pointer to transform info structure */
   transform_info = (Transform_Info *) dst;

   /* Save file name */
   transform_info->file_name = nextArg;
   transformation = transform_info->transformation;

   /* Open the file */
   if (strcmp(nextArg, "-") == 0) {
      /* Create a temporary for standard input */
      fp=tmpfile();
      if (fp==NULL) {
         (void) fprintf(stderr, "Error opening temporary file.\n");
         exit(EXIT_FAILURE);
      }
      while ((ch=getc(stdin))!=EOF) (void) putc(ch, fp);
      rewind(fp);
   }
   else {
      if (open_file_with_default_suffix(nextArg,
                    get_default_transform_file_suffix(),
                    READ_FILE, ASCII_FORMAT, &fp) != OK) {
         (void) fprintf(stderr, "Error opening transformation file %s.\n",
                        nextArg);
         exit(EXIT_FAILURE);
      }
   }

   /* Read in the file for later use */
   if (transform_info->file_contents == NULL) {
      transform_info->file_contents = MALLOC(TRANSFORM_BUFFER_INCREMENT);
      transform_info->buffer_length = TRANSFORM_BUFFER_INCREMENT;
   }
   for (index = 0; (ch=getc(fp)) != EOF; index++) {
      if (index >= transform_info->buffer_length-1) {
         transform_info->buffer_length += TRANSFORM_BUFFER_INCREMENT;
         transform_info->file_contents = 
            REALLOC(transform_info->file_contents, 
                    transform_info->buffer_length);
      }
      transform_info->file_contents[index] = ch;
   }
   transform_info->file_contents[index] = '\0';
   rewind(fp);

   /* Read the file */
   if (input_transform(fp, &input_transformation)!=OK) {
      (void) fprintf(stderr, "Error reading transformation file.\n");
      exit(EXIT_FAILURE);
   }
   (void) close_file(fp);

   /* Get rid of the old one */
   delete_general_transform(transformation);

   /* Invert the transformation */
   create_inverse_general_transform(&input_transformation, transformation);

   return TRUE;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_model_file
@INPUT      : dst - Pointer to client data from argument table
              key - argument key
              nextArg - argument following key
@OUTPUT     : (nothing) 
@RETURNS    : TRUE so that ParseArgv will discard nextArg unless there
              is no following argument.
@DESCRIPTION: Routine called by ParseArgv to read in a model file (-like)
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 15, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int get_model_file(char *dst, char *key, char *nextArg)
{      /* ARGSUSED */
   Volume_Definition *volume_def;
   File_Info file;

   /* Check for following argument */
   if (nextArg == NULL) {
      (void) fprintf(stderr, 
                     "\"%s\" option requires an additional argument\n",
                     key);
      exit(EXIT_FAILURE);
   }

   /* Get pointer to volume definition structure */
   volume_def = (Volume_Definition *) dst;

   /* Get file information */
   get_file_info(nextArg, volume_def, &file);

   /* Close the file */
   (void) miclose(file.mincid);

   return TRUE;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_axis_order
@INPUT      : dst - Pointer to client data from argument table
              key - argument key
              nextArg - argument following key
@OUTPUT     : (nothing) 
@RETURNS    : FALSE so that ParseArgv will not discard nextArg
@DESCRIPTION: Routine called by ParseArgv to get the axis order
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 15, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int get_axis_order(char *dst, char *key, char *nextArg)
{      /* ARGSUSED */
   Volume_Definition *volume_def;

   /* Get pointer to client data */
   volume_def = (Volume_Definition *) dst;

   /* Check key for order */
   if (strcmp(key, "-transverse") == 0) {
      volume_def->axes[Z] = 0;
      volume_def->axes[Y] = 1;
      volume_def->axes[X] = 2;
   }
   else if (strcmp(key, "-sagittal") == 0) {
      volume_def->axes[X] = 0;
      volume_def->axes[Z] = 1;
      volume_def->axes[Y] = 2;
   }
   else if (strcmp(key, "-coronal") == 0) {
      volume_def->axes[Y] = 0;
      volume_def->axes[Z] = 1;
      volume_def->axes[X] = 2;
   }

   return FALSE;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_fillvalue
@INPUT      : dst - Pointer to client data from argument table
              key - argument key
              nextArg - argument following key
@OUTPUT     : (nothing) 
@RETURNS    : FALSE so that ParseArgv will not discard nextArg
@DESCRIPTION: Routine called by ParseArgv to set the fill value
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 15, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int get_fillvalue(char *dst, char *key, char *nextArg)
{      /* ARGSUSED */
   double *dptr;

   /* Get pointer to client data */
   dptr = (double *) dst;

   /* Check key for fill value to set */
   if (strcmp(key, "-fill") == 0) {
      *dptr = -DBL_MAX;
   }
   else if (strcmp(key, "-nofill") == 0) {
      *dptr = FILL_DEFAULT;
   }

   return FALSE;
}
