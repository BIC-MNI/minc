/* ----------------------------- MNI Header -----------------------------------
@NAME       : mnitominc
@INPUT      : argc, argv - command line arguments
@OUTPUT     : (none)
@RETURNS    : error status
@DESCRIPTION: Converts an mni format file to a minc format file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 7, 1992 (Peter Neelin)
@MODIFIED   : 
 * $Log: mnitominc.c,v $
 * Revision 6.3  2008-01-17 02:33:02  rotor
 *  * removed all rcsids
 *  * removed a bunch of ^L's that somehow crept in
 *  * removed old (and outdated) BUGS file
 *
 * Revision 6.2  2008/01/12 19:08:14  stever
 * Add __attribute__ ((unused)) to all rcsid variables.
 *
 * Revision 6.1  1999/10/29 17:52:06  neelin
 * Fixed Log keyword
 *
 * Revision 6.0  1997/09/12 13:23:27  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:24:28  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:00:05  neelin
 * Release of minc version 0.4
 *
 * Revision 3.0  1995/05/15  19:31:02  neelin
 * Release of minc version 0.3
 *
 * Revision 2.2  1995/01/23  09:06:40  neelin
 * changed ncclose to miclose
 *
 * Revision 2.1  95/01/23  09:01:40  neelin
 * Changed nccreate to micreate.
 * 
 * Revision 2.0  94/09/28  10:33:17  neelin
 * Release of minc version 0.2
 * 
 * Revision 1.14  94/09/28  10:33:11  neelin
 * Pre-release
 * 
 * Revision 1.13  93/08/30  16:43:47  neelin
 * Added -slcstep option.
 * Fixed -field_of_view option.
 * Changed -xstep/-ystep to -colstep -rowstep.
 * 
 * Revision 1.12  93/08/11  15:24:20  neelin
 * Added RCS logging to source.
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

#include <stdlib.h>
#include <stdio.h>
#include <minc.h>
#include <float.h>
#include <string.h>
#include <math.h>
#include <ParseArgv.h>
#include <vax_conversions.h>
#include <time_stamp.h>
#include <minc_def.h>
#include "mnitominc.h"

/* Main program */

main(int argc, char *argv[])
{
   int cdfid, imgid, maxid, minid, patid, study, acqid;
   int dimvar[MAX_VAR_DIMS], twidth;
   long start[MAX_VAR_DIMS];
   long count[MAX_VAR_DIMS];
   long end[MAX_VAR_DIMS];
   long temp_coord[MAX_VAR_DIMS];
   int dim[MAX_VAR_DIMS];
   int fastdim;
   int iframe, islice, image_num, i;
   mni_header_type mni_header;
   mni_image_type mni_image;
   int zdim, tdim;
   double last_z, diff_z, curr_diff_z, first_z;
   double last_t, diff_t, curr_diff_t, first_t;
   int z_regular, t_regular;
   char *tm_stamp;

   /* Get time stamp */
   tm_stamp = time_stamp(argc, argv);

   /* Parse arguments and get header info from mni file */
   parse_args(argc, argv, &mni_header);

   /* Create the file */
   cdfid=micreate(outfilename, (clobber ? NC_CLOBBER : NC_NOCLOBBER));
   (void) miattputstr(cdfid, NC_GLOBAL, MIhistory, tm_stamp);

   /* Create the dimensions */
   zdim = tdim = -1;
   for (i=0; i<ndims; i++) {
      dim[i] = ncdimdef(cdfid, dimname[i], dimlength[i]);

      /* Create dimension variable if mni file or for image dimensions */
      if ((mni_header.file_type==MNI_FILE_TYPE) ||
          (i>=ndims-IMAGE_DIMS) || (zstep != 0.0)) {
         if (dimfixed[i])
            dimvar[i] = micreate_std_variable(cdfid, dimname[i], 
                                           NC_LONG, 0, NULL);
         else 
            dimvar[i] = micreate_std_variable(cdfid, dimname[i], 
                                           NC_DOUBLE, 1, &dim[i]);

         /* Add MIstep attribute */
         if (i==ndims-1) {
            (void) miattputdbl(cdfid, dimvar[i], MIstep, xstep);
            if ((mni_header.file_type == MNI_FILE_TYPE) &&
                (mni_header.scanner_id == MNI_STX_SCAN) &&
                (orientation == TRANSVERSE) &&
                ((mni_header.npixels == 128) ||
                 (mni_header.npixels == 256))) {
               (void) miattputdbl(cdfid, dimvar[i], MIstart, 
                                  mni_header.xstart);
            }
         }
         else if (i==ndims-2) {
            (void) miattputdbl(cdfid, dimvar[i], MIstep, ystep);
            if ((mni_header.file_type == MNI_FILE_TYPE) &&
                (mni_header.scanner_id == MNI_STX_SCAN) &&
                (orientation == TRANSVERSE) &&
                ((mni_header.npixels == 128) ||
                 (mni_header.npixels == 256))) {
               (void) miattputdbl(cdfid, dimvar[i], MIstart, 
                                  mni_header.ystart);
            }
         }
         else {
            (void) miattputdbl(cdfid, dimvar[i], MIstep, 0.0);
            (void) miattputdbl(cdfid, dimvar[i], MIstart, 0.0);
         }

         /* If MItime, then create dimwidth variable */
         if (STR_EQ(dimname[i], MItime)) {
            tdim=i;
            if (dimfixed[i])
               twidth = micreate_std_variable(cdfid, MItime_width, 
                                                NC_LONG, 0, NULL);
            else 
               twidth = micreate_std_variable(cdfid, MItime_width, 
                                              NC_DOUBLE, 1, &dim[i]);
            (void) miattputstr(cdfid, dimvar[i], MIunits, "seconds");
            (void) miattputstr(cdfid, twidth, MIunits, "seconds");
         }

         /* If not MItime, add attribute to describe spacetype */
         else {
            (void) miattputstr(cdfid, dimvar[i], MIunits, "mm");
            if (i<ndims-IMAGE_DIMS) zdim=i;
            if (mni_header.scanner_id==MNI_STX_SCAN)
               (void) miattputstr(cdfid, dimvar[i], MIspacetype, MI_TALAIRACH);
            else
               (void) miattputstr(cdfid, dimvar[i], MIspacetype, MI_NATIVE);
         }

      }     /* Dimension variable */

      /* Initialize coordinate vectors */
      start[i] = 0;
      count[i] = ((i<ndims-IMAGE_DIMS) ? 1 : dimlength[i]);
      end[i]   = dimlength[i];
   }

   /* Create the image */
   imgid=micreate_std_variable(cdfid, MIimage, 
                               (mni_header.pix_size==1 ? NC_BYTE : NC_SHORT),
                               ndims, dim);
   (void) miattputstr(cdfid, imgid, MIcomplete, MI_FALSE);
   (void) miattputstr(cdfid, imgid, MIsigntype, 
                      (mni_header.pix_size==1 ? MI_UNSIGNED : MI_SIGNED));
   (void) ncattput(cdfid, imgid, MIvalid_range, NC_DOUBLE, 2, 
                   mni_header.valid_range);
   if (mni_header.file_type==MNI_FILE_TYPE) {
      maxid = micreate_std_variable(cdfid, MIimagemax, 
                                    NC_DOUBLE, ndims-IMAGE_DIMS, dim);
      minid = micreate_std_variable(cdfid, MIimagemin, 
                                    NC_DOUBLE, ndims-IMAGE_DIMS, dim);
   }

   /* Add patient information */
   patid=micreate_group_variable(cdfid, MIpatient);
   (void) miattputstr(cdfid, patid, MIfull_name, mni_header.patient_name);
   (void) miattputstr(cdfid, patid, MIidentification, 
                      mni_header.patient_num);

   /* Add study information */
   study=micreate_group_variable(cdfid, MIstudy);
   (void) miattputstr(cdfid, study, MIstart_time, mni_header.start_time);
   if ((mni_header.file_type==MNI_FILE_TYPE) &&
       (mni_header.scanner_id!=MNI_MRI_SCAN) &&
       (mni_header.scanner_id!=MNI_STX_SCAN) &&
       (mni_header.npixels<=128))
      (void) miattputstr(cdfid, study, MImodality, MI_PET);
   else if (mni_header.scanner_id!=MNI_STX_SCAN)
      (void) miattputstr(cdfid, study, MImodality, MI_MRI);

   /* Add acquisition information */
   if ((mni_header.file_type==MNI_FILE_TYPE) &&
       (mni_header.scanner_id!=MNI_MRI_SCAN) &&
       (mni_header.scanner_id!=MNI_STX_SCAN) &&
       (mni_header.npixels<=128)) {
      acqid=micreate_group_variable(cdfid, MIacquisition);
      (void) miattputstr(cdfid, acqid, MIinjection_time, 
                         mni_header.injection_time);
      (void) miattputstr(cdfid, acqid, MIradionuclide, mni_header.isotope);
      (void) miattputdbl(cdfid, acqid, MIinjection_dose, mni_header.dose);
      (void) miattputstr(cdfid, acqid, MIdose_units, mni_header.dose_units);
   }

   /* End definition mode */
   (void) ncsetfill(cdfid, NC_NOFILL);
   (void) ncendef(cdfid);

   /* Get a buffer for reading images */
   mni_image.npixels=mni_header.npixels;
   mni_image.image_pix=mni_image.npixels*mni_image.npixels;
   mni_image.pix_size=mni_header.pix_size;
   mni_image.image_size=mni_image.image_pix*mni_image.pix_size;
   mni_image.image=MALLOC(mni_image.image_size);

   /* Write log message if needed */
   if (verbose) {
      (void) fprintf(stderr, "Converting slices:");
      (void) fflush(stderr);
   }

   /* Loop through the images */
   fastdim=ndims-IMAGE_DIMS-1;
   if (fastdim<0) fastdim=0;
   z_regular = t_regular = TRUE;
   for (iframe=0; iframe<nframes; iframe++) {
      for (islice=0; islice<nslices; islice++) {

         /* Print log message */
         if (verbose) {
            (void) fprintf(stderr, ".");
            (void) fflush(stderr);
         }

         /* Read in image */
         image_num = islice*nframes + iframe;
         if (get_mni_image(&mni_header, &mni_image, image_num)) {
            (void) fprintf(stderr, "%s: Error reading mni image.\n", pname);
            exit(ERROR_STATUS);
         }

         /* Write the image */
         (void) ncvarput(cdfid, imgid, start, count, mni_image.image);

         /* Overwrite the z position if user specified */
         if ((zdim >= 0) && (zstep != 0.0) && (islice != 0))
            mni_image.zposition = first_z + islice * zstep;

         /* Write the z position */
         if ((zdim>=0) && (iframe==0)) {
            (void) ncvarput1(cdfid, dimvar[zdim], 
                             mitranslate_coords(cdfid, 
                                                imgid, start,
                                                dimvar[zdim], 
                                    miset_coords(MAX_DIMS, 0L, temp_coord)),
                             &mni_image.zposition);
            if (islice>0) {
               curr_diff_z = mni_image.zposition - last_z;
               if ((islice>1) && !dblcmp(curr_diff_z, diff_z, EPSILON))
                  z_regular = FALSE;
               diff_z = curr_diff_z;
            }
            else
               first_z = mni_image.zposition;
            last_z=mni_image.zposition;
         }
         /* Write the image max and min and time */
         if (mni_header.file_type==MNI_FILE_TYPE) {
            (void) ncvarput1(cdfid, minid, start, &mni_image.minimum);
            (void) ncvarput1(cdfid, maxid, start, &mni_image.maximum);
            if ((tdim>=0) && (islice==0)) {
               (void) ncvarput1(cdfid, dimvar[tdim], 
                         mitranslate_coords(cdfid, 
                                            imgid, start,
                                            dimvar[tdim],
                            miset_coords(MAX_DIMS, 0L, temp_coord)),
                                &mni_image.time);
               (void) ncvarput1(cdfid, twidth, 
                         mitranslate_coords(cdfid, 
                                            imgid, start,
                                            twidth,
                            miset_coords(MAX_DIMS, 0L, temp_coord)),
                                &mni_image.time_length);
               if (iframe>0) {
                  curr_diff_t = mni_image.time - last_t;
                  if ((iframe>1) && !dblcmp(curr_diff_t, diff_t, EPSILON))
                     t_regular = FALSE;
                  diff_t = curr_diff_t;
               }
               else
                  first_t = mni_image.time;
               last_t=mni_image.time;
            }
         }

         /* Increment the counters */
         start[fastdim] += count[fastdim];
         i=fastdim;
         while ((i>0) && (start[i]>=end[i])) {
            start[i] = 0;
            i--;
            start[i] += count[i];
         }
      }
   }

   /* Write dimension attributes for z and t */
   if (zdim>=0) {
      if (nslices>1)
         diff_z = (last_z - first_z) / (double) (nslices - 1);
      else
         diff_z = 0.0;
      (void) miattputdbl(cdfid, dimvar[zdim], MIstep, diff_z);
      (void) miattputdbl(cdfid, dimvar[zdim], MIstart, first_z);
      if (z_regular)
         (void) miattputstr(cdfid, dimvar[zdim], MIspacing, MI_REGULAR);
      else
         (void) miattputstr(cdfid, dimvar[zdim], MIspacing, MI_IRREGULAR);
   }

   if (tdim>=0) {
      if (nframes>1)
         diff_t = (last_t - first_t) / (double) (nframes - 1);
      else
         diff_t = 0.0;
      (void) miattputdbl(cdfid, dimvar[tdim], MIstep, diff_t);
      (void) miattputdbl(cdfid, dimvar[tdim], MIstart, first_t);
      if (t_regular)
         (void) miattputstr(cdfid, dimvar[tdim], MIspacing, MI_REGULAR);
      else
         (void) miattputstr(cdfid, dimvar[tdim], MIspacing, MI_IRREGULAR);
   }

   /* Print end of log message */
   if (verbose) {
      (void) fprintf(stderr, "Done\n");
      (void) fflush(stderr);
   }

   /* Free the memory */
   FREE(mni_image.image);

   /* Close the file */
   (void) miattputstr(cdfid, imgid, MIcomplete, MI_TRUE);
   (void) miclose(cdfid);
   
   exit(NORMAL_STATUS);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : parse_args
@INPUT      : argc        - number of command line arguments
              argv        - array of arguments
@OUTPUT     : 
@RETURNS    : (nothing).
@DESCRIPTION: Parses command line arguments.
@METHOD     : 
@GLOBALS    : pname        - program name
              infilename   - mni file to read
              outfilename  - minc file to write
              clobber      - overwrite?
              dimname      - names of dimensions
              dimlength    - lengths of dimensions
              ndims        - number of dimensions
              orientation  - orientation of dimensions
              type         - type of output data
              signtype     - sign of output data
              sign         - string sign of output
              valid_range  - valid range of output data
              vrange_set   - valid range used?
@CALLS      : 
@CREATED    : December 7, 1992
@MODIFIED   : 
---------------------------------------------------------------------------- */
void parse_args(int argc, char *argv[], mni_header_type *mni_header)
{
   int i, j, npix;

   /* Parse the command line */
   pname=argv[0];
   if (ParseArgv(&argc, argv, argTable, 0)) {
      usage_error(pname);
   }

   /* Check number of arguments */
   if (argc!=3)
      usage_error(pname);

   /* Get filenames */
   infilename = argv[1];
   outfilename = argv[2];

   /* Get mni file header info */
   if (get_mni_header(infilename, mni_header)) {
      (void) fprintf(stderr, "%s: Error reading mni file header.\n", pname);
      usage_error(pname);
   }

   /* Get dimensions */

   /* Get fastest two dim sizes */
   for (i=0; i<IMAGE_DIMS; i++)

   dimlength[MAX_DIMS-i-1] = mni_header->npixels;

   /* Try to guess slowest two dim sizes if user doesn't give info */
   if ((dimlength[0]<=0) && (dimlength[1]<=0)) {

      /* If fewer than MAX_VOL_IMGS images, assume volume */
      if (mni_header->nimages<=MAX_VOL_IMGS) {
         dimlength[0]=1;
         dimlength[1]=mni_header->nimages;
      }

      /* Otherwise, divide by DEFAULT_NSLICES to get nframes */
      else {
         dimlength[1]=DEFAULT_NSLICES;
         dimlength[0]=mni_header->nimages / dimlength[1];
      }
   }
   /* User gave nframes */
   else if ((dimlength[0]>0) && (dimlength[1]<=0)) {
      dimlength[1]=mni_header->nimages / dimlength[0];
   }
   /* User gave nslices */
   else if ((dimlength[0]<=0) && (dimlength[1]>0)) {
      dimlength[0]=mni_header->nimages / dimlength[1];
   }
   /* User gave both */
   else {
      if (dimlength[0]*dimlength[1]!=mni_header->nimages) {
         (void) fprintf(stderr, 
      "%s: <nframes>*<nslices> is not equal to # of images in file.\n",
                 pname);
         exit(ERROR_STATUS);
      }
   }

   /* Check that nframes*nslices = nimages */
   if ((dimlength[0]*dimlength[1])!=mni_header->nimages) {
      (void) fprintf(stderr, 
              "%s: Cannot work out number of slices and frames.\n",
              pname);
      exit(ERROR_STATUS);
   }
   else if ((dimlength[0]<1) || (dimlength[1]<1)) {
      (void) fprintf(stderr, 
              "%s: Number of frames and slices must be greater than zero.\n",
              pname);
      exit(ERROR_STATUS);
   }

   /* Get orientation */
   if (orientation == DEF_ORIENT) {
      if (mni_header->orientation==MNI_CORONAL)
         orientation = CORONAL;
      else if (mni_header->orientation==MNI_SAGITTAL)
         orientation = SAGITTAL;
      else
         orientation = TRANSVERSE;
   }
   for (i=1; i<MAX_DIMS; i++) {
      dimname[i]=orientation_names[orientation][i-1];
   }

   /* Get number of dimensions and check for dimensions of length 1 */
   nframes = dimlength[0];
   nslices = dimlength[1];
   for (i=0, j=0; i<MAX_DIMS; i++) {
      if ((i>=MAX_DIMS-IMAGE_DIMS) || (dimlength[i]>1)) {
         dimlength[j] = dimlength[i];
         dimname[j]   = dimname[i];
         dimfixed[j]  = dimfixed[i];
         j++;
      }
   }
   ndims=j;

   /* Set x,y step for stereotaxic files */
   if ((mni_header->file_type == MNI_FILE_TYPE) && 
       (mni_header->scanner_id == MNI_STX_SCAN)) {
      npix = mni_header->npixels;
      switch (orientation) {
      case TRANSVERSE:
         mni_header->xstep = 
            MNI_DIR * MNI_STX_XFOV / ((double) npix);
         mni_header->ystep = 
            MNI_DIR * MNI_STX_YFOV / ((double) npix);
         mni_header->zstep_scale = MNI_STX_ZSCALE;
         if (npix==128) {
            mni_header->xstart = -(npix/2.0) * mni_header->xstep;
            mni_header->ystart = -(npix/2.0) * mni_header->ystep;
         }
         else if (npix==256) {
            mni_header->xstart = -(npix/2.0 + 0.5) * mni_header->xstep;
            mni_header->ystart = -(npix/2.0 + 0.5) * mni_header->ystep;
         }
         mni_header->xstart -= MNI_STX_XACLOC;
         mni_header->ystart -= MNI_STX_YACLOC;
         break;
      case SAGITTAL:
         mni_header->xstep = 
            MNI_DIR * MNI_STX_YFOV / ((double) npix);
         mni_header->ystep = 
            MNI_DIR * MNI_STX_ZFOV / ((double) npix);
         mni_header->zstep_scale = MNI_STX_XSCALE;
         break;
      case CORONAL:
         mni_header->xstep = 
            MNI_DIR * MNI_STX_XFOV / ((double) npix);
         mni_header->ystep = 
            MNI_DIR * MNI_STX_ZFOV / ((double) npix);
         mni_header->zstep_scale = MNI_STX_YSCALE;
         break;
      }
   }

   /* Field of view */
   if (xstep==0.0) xstep = field_of_view / (double) mni_header->npixels;
   if (ystep==0.0) ystep = field_of_view / (double) mni_header->npixels;
   if (xstep==0.0) xstep = mni_header->xstep;
   if (ystep==0.0) ystep = mni_header->ystep;

   /* Set sign of step according to file type */
   if (mni_header->file_type == MNI_FILE_TYPE) {
      xstep *= MNI_DIR;
      ystep *= MNI_DIR;
   }
   else if (mni_header->file_type == NIL_FILE_TYPE) {
      xstep *= NIL_DIR;
      ystep *= NIL_DIR;
   }

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : usage_error
@INPUT      : progname - program name
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Prints a usage error message and exits.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 7, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
void usage_error(char *progname)
{
   (void) fprintf(stderr, 
                "\nUsage: %s [<options>] <infile> <outfile>\n", progname);
   (void) fprintf(stderr,   
                  "       %s [-help]\n\n", progname);

   exit(ERROR_STATUS);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_mni_header
@INPUT      : file - name of mni format file
@OUTPUT     : mni_header - pointer to structure containing mni header info
@RETURNS    : TRUE if an error occurs
@DESCRIPTION: Opens an mni format file and reads the header information
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 8, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
int get_mni_header(char *file, mni_header_type *mni_header)
{
   char buffer[MNI_HEADER_SIZE];
   int matrix_type;
   short stemp;
   int i, j;

   if ((mni_header->fp=fopen(file, "r"))==NULL) {
      (void) fprintf(stderr, "Cannot open file %s\n", file);
      return(TRUE);
   }

   /* Read in the header block */
   if (fseek(mni_header->fp, 0, SEEK_SET) ||
       (fread(buffer, sizeof(char), MNI_HEADER_SIZE, mni_header->fp)
                          !=MNI_HEADER_SIZE)) {
      (void) fprintf(stderr, "Cannot read from file.\n");
      return(TRUE);
   }

   /* Get interesting information */

   /* Get offset to data */
   get_vax_short(1, &buffer[MNI_LOC_OFFSET], &(mni_header->offset));

   /* Get number of images */
   get_vax_short(1, &buffer[MNI_LOC_NIMAGES], &(mni_header->nimages));

   /* Get image size and file type info */
   matrix_type=buffer[MNI_LOC_MATYPE];
   switch (matrix_type) {
   case '3':
      mni_header->npixels=64;
      mni_header->file_type=MNI_FILE_TYPE;
      break;
   case '4':
      mni_header->npixels=64;
      mni_header->file_type=NIL_FILE_TYPE;
      break;
   case '5':
      mni_header->npixels=128;
      mni_header->file_type=MNI_FILE_TYPE;
      break;
   case '6':
      mni_header->npixels=128;
      mni_header->file_type=NIL_FILE_TYPE;
      break;
   case '7':
      mni_header->npixels=256;
      mni_header->file_type=MNI_FILE_TYPE;
      break;
   case '8':
      mni_header->npixels=256;
      mni_header->file_type=NIL_FILE_TYPE;
      break;
   default:
      (void) fprintf(stderr, "Unknown matrix format.\n");
      return(TRUE);
   }

   /* Get pixel size and valid range info from file type. */
   if (mni_header->file_type==MNI_FILE_TYPE) {
      mni_header->pix_size=1;
      mni_header->valid_range[0]=MNI_VMIN;
      mni_header->valid_range[1]=MNI_VMAX;
   }
   else {
      mni_header->pix_size=2;
      mni_header->valid_range[0]=NIL_VMIN;
      mni_header->valid_range[1]=NIL_VMAX;
   }

   /* Get image type */
   get_vax_short(1, &buffer[MNI_LOC_SCANNER_ID], &(mni_header->scanner_id));
   get_vax_short(1, &buffer[MNI_LOC_CORR_MASK], &stemp);
   if (stemp==MNI_POSITOME_MASK) mni_header->scanner_id=MNI_POS_SCAN;

   /* Get step sizes */
   if (mni_header->file_type==MNI_FILE_TYPE) {
      /* We have to deal with step for stereotaxic files after knowing
         orientation (i.e. after parsing args) */
      if (mni_header->scanner_id != MNI_STX_SCAN) {
         mni_header->xstep = mni_header->ystep = 
            MNI_FOV / ((double) mni_header->npixels);
      }
   }
   else
      mni_header->xstep = mni_header->ystep = 
         NIL_FOV / ((double) mni_header->npixels);

   /* Get image orientation */
   get_vax_short(1, &buffer[MNI_LOC_ORIENTATION], &(mni_header->orientation));

   /* Get Patient name */
   for (i=0; i<MNI_PATNAM_LENGTH; i++) {
      mni_header->patient_name[i]=buffer[MNI_LOC_PATNAM+i];
      if ((mni_header->patient_name[i] < MIN_VALID_CHAR) ||
          (mni_header->patient_name[i] > MAX_VALID_CHAR))
         mni_header->patient_name[i] = PADDING_CHAR;
   }
   mni_header->patient_name[i]='\0';

   /* Get Patient number */
   for (i=0; i<MNI_PATNUM_LENGTH; i++) {
      mni_header->patient_num[i]=buffer[MNI_LOC_PATNUM+i];
      if ((mni_header->patient_num[i] < MIN_VALID_CHAR) ||
          (mni_header->patient_num[i] > MAX_VALID_CHAR))
         mni_header->patient_num[i] = PADDING_CHAR;
   }
   mni_header->patient_num[i]='\0';

   /* Get acquisition time and date */
   for (i=0, j=0; i<MNI_ACQDAT_LENGTH; i++, j++) {
      mni_header->start_time[j]=buffer[MNI_LOC_ACQDAT+i];
      if ((mni_header->start_time[j] < MIN_VALID_CHAR) ||
          (mni_header->start_time[j] > MAX_VALID_CHAR))
         mni_header->start_time[j] = PADDING_CHAR;
   }
   mni_header->start_time[j++]=' ';
   for (i=0; i<MNI_ACQTIM_LENGTH; i++, j++) {
      mni_header->start_time[j]=buffer[MNI_LOC_ACQTIM+i];
      if ((mni_header->start_time[j] < MIN_VALID_CHAR) ||
          (mni_header->start_time[j] > MAX_VALID_CHAR))
         mni_header->start_time[j] = PADDING_CHAR;
   }
   mni_header->start_time[j]='\0';

   /* Get isotope name */
   for (i=0; i<MNI_ISOTOPE_LENGTH; i++) {
      mni_header->isotope[i]=buffer[MNI_LOC_ISOTOPE+i];
      if ((mni_header->isotope[i] < MIN_VALID_CHAR) ||
          (mni_header->isotope[i] > MAX_VALID_CHAR))
         mni_header->isotope[i] = PADDING_CHAR;
   }
   mni_header->isotope[i]='\0';

   /* Get dose */
   for (i=0; i<MNI_DOSE_LENGTH; i++) {
      mni_header->dose_string[i]=buffer[MNI_LOC_DOSE+i];
      if ((mni_header->dose_string[i] < MIN_VALID_CHAR) ||
          (mni_header->dose_string[i] > MAX_VALID_CHAR))
         mni_header->dose_string[i] = PADDING_CHAR;
   }
   mni_header->dose_string[i]='\0';
   mni_header->dose = atof(mni_header->dose_string);
   (void) strcpy(mni_header->dose_units, "mCurie");

   /* Get injection time */
   for (i=0; i<MNI_INJTIM_LENGTH; i++) {
      mni_header->injection_time[i]=buffer[MNI_LOC_INJTIM+i];
      if ((mni_header->injection_time[i] < MIN_VALID_CHAR) ||
          (mni_header->injection_time[i] > MAX_VALID_CHAR))
         mni_header->injection_time[i] = PADDING_CHAR;
   }
   mni_header->injection_time[i]='\0';

   return(FALSE);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_mni_image
@INPUT      : mni_header - pointer to structure containing file header info
                 and file pointer.
              image_num - number of image to read (counting from zero).
@OUTPUT     : mni_image - pointer to structure containing image information
                 (space for the image must already be allocated).
@RETURNS    : TRUE if an error occurs.
@DESCRIPTION: Read an image from an mni format file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 8, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
int get_mni_image(mni_header_type *mni_header, mni_image_type *mni_image,
                  int image_num)
{
   float qsc, zpos, time;
   short isea, tlen;
   long i, j, nrow, nread, image_bytes, bytes_read;
   unsigned char *image;
   double zero, denom, *valid_range;

   /* Check the image number */
   if (image_num >= mni_header->nimages) {
      (void) fprintf(stderr, "Tried to read past last image.\n");
      return(TRUE);
   }

   /* Read the image */
   if (fseek(mni_header->fp, mni_header->offset*BLOCK_SIZE + 
             image_num*mni_image->image_size, SEEK_SET)) {
      (void) fprintf(stderr, "Cannot seek to image.\n");
      return(TRUE);
   }
   nread=fread(mni_image->image, mni_header->pix_size, mni_image->image_pix,
             mni_header->fp);
   if (nread<=0) {
      (void) fprintf(stderr, "Cannot read image from file.\n");
      return(TRUE);
   }
   /* Check for read of only part of image (happens to last image of files
      created by cnvmri) */
   if (nread<mni_image->image_pix) {
      bytes_read = nread * mni_header->pix_size; 
      image_bytes = mni_image->image_pix * mni_header->pix_size;
      for (i = bytes_read; i < image_bytes; i++)
         mni_image->image[i]=0;
   }
   image = mni_image->image;
   nrow=mni_image->npixels*mni_image->pix_size;

   /* Get interesting stuff from the image */
   if (mni_header->file_type==MNI_FILE_TYPE) {
      get_vax_float(1, &image[MNI_XLOC_QSC+MNI_YLOC_QSC*nrow], &qsc);
      get_vax_short(1, &image[MNI_XLOC_ISEA+MNI_YLOC_ISEA*nrow], &isea);
      get_vax_float(1, &image[MNI_XLOC_ZPOS+MNI_YLOC_ZPOS*nrow], &zpos);
      get_vax_float(1, &image[MNI_XLOC_TIME+MNI_YLOC_TIME*nrow], &time);
      get_vax_short(1, &image[MNI_XLOC_TLEN+MNI_YLOC_TLEN*nrow], &tlen);
      mni_image->minimum = (MNI_VMIN-isea) * qsc;
      mni_image->maximum = (MNI_VMAX-isea) * qsc;
      mni_image->zposition = ((mni_header->scanner_id != MNI_STX_SCAN) ?
                              zpos :
                              zpos * mni_header->zstep_scale);
      mni_image->time = time*SEC_PER_MIN;
      mni_image->time_length = tlen;
   }
   else {
      mni_image->minimum = NIL_VMIN;
      mni_image->maximum = NIL_VMAX;
      mni_image->zposition = 0.0;
      mni_image->time = 0.0;
      mni_image->time_length = 0.0;
   }

   /* Do byte swap if needed */
   if (mni_image->pix_size==2) {
      get_vax_short(mni_image->image_pix, image, (short *) image);
   }

   /* Zero the fruit salad */
   valid_range = mni_header->valid_range;
   denom = mni_image->maximum - mni_image->minimum;
   if (denom == 0.0) {
      zero = valid_range[0];
   }
   else {
      zero = valid_range[0] - mni_image->minimum *
         (valid_range[1] - valid_range[0]) / denom;
   }
   if (zero < valid_range[0]) zero = valid_range[0];
   if (zero > valid_range[1]) zero = valid_range[1];
   zero = ROUND(zero);
   for (j=0; j < MNI_CORNER_YSIZE; j++)
      for (i=0; i < MNI_CORNER_XSIZE; i += mni_image->pix_size)
         if (mni_image->pix_size == 1) {
            *((unsigned char *) &image[j*nrow+i]) = zero;
         }
         else if (mni_image->pix_size == 2) {
            *((short *) &image[j*nrow+i]) = zero;
         }

   return(FALSE);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : dblcmp
@INPUT      : dbl1 - first value
              dbl2 - second value
              epsilon - maximum difference
@OUTPUT     : (none)
@RETURNS    : TRUE if values are the same within epsilon, FALSE otherwise.
@DESCRIPTION: Compares two double precision values. If their normalized
              difference is less than epsilon, then return TRUE, 
              otherwise FALSE.
              Returns the following :
                 abs((dbl1-dbl2)/(dbl1+dbl2)) < epsilon
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 14, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
int dblcmp(double dbl1, double dbl2, double epsilon)
{
   double diff, sum;

   diff = dbl1 - dbl2;
   sum = dbl1 + dbl2;
   diff = (diff < 0.0) ? -diff : diff;
   sum = (sum < 0.0) ? -sum : sum;
   if (sum==0.0) sum = 1.0;
   return((diff/sum) <= epsilon);

}
