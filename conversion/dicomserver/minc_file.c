/* ----------------------------- MNI Header -----------------------------------
@NAME       : minc_file.c
@DESCRIPTION: Code to do minc file handling.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 28, 1997 (Peter Neelin)
@MODIFIED   : 
 * $Log: minc_file.c,v $
 * Revision 6.1  1999-10-29 17:51:55  neelin
 * Fixed Log keyword
 *
 * Revision 6.0  1997/09/12 13:24:27  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:25:26  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:06:20  neelin
 * Release of minc version 0.4
 *
 * Revision 1.1  1997/03/04  20:56:47  neelin
 * Initial revision
 *
@COPYRIGHT  :
              Copyright 1997 Peter Neelin, McConnell Brain Imaging Centre, 
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

#include <dicomserver.h>
#include <ctype.h>

/* Global for minc history (sorry, but it was kludged in afterwards) */
char *minc_history = NULL;

/* Define mri dimension names */
static char *mri_dim_names[] = {
   NULL, "echo_time", MItime, "phase_number", "chemical_shift", NULL};

/* Macros */
#define STRLEN(s) ((int) strlen(s))

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_minc_file
@INPUT      : minc_file - name of file to create. If NULL, a name is 
                 generated internally.
              clobber - if TRUE, any existing file will be overwritten.
              general_info - information for creating the file.
              file_prefix - string providing any directory or prefix 
                 for internally generated filename (if it is a directory,
                 then it must contain the last "/")
@OUTPUT     : output_file_name - returns a pointer to an internal area
                 containing the file name of the created file if minc_file
                 is NULL, or simply a pointer to minc_file. If NULL, then
                 nothing is returned.
@RETURNS    : id of image conversion variable (MI_ERROR in case of error).
@DESCRIPTION: Routine to create the minc file.
@METHOD     : 
@GLOBALS    : 
CALLS       : 
@CREATED    : November 26, 1993 (Peter Neelin)
@MODIFIED   :
---------------------------------------------------------------------------- */
public int create_minc_file(char *minc_file, int clobber, 
                            General_Info *general_info,
                            char *file_prefix, char **output_file_name)
{
   static char temp_name[256];
   char patient_name[256];
   char *filename;
   int minc_clobber;
   int mincid, icvid;
   Mri_Index imri;
   char scan_label[MRI_NDIMS][20];

   /* Prefixes for creating file name */
   static char *scan_prefix[MRI_NDIMS] = 
      {"sl", "e", "d", "p", "cs"};

   /* Turn off fatal errors */
   ncopts = NCOPTS_DEFAULT;

   /* Create the file name if needed */
   if (minc_file != NULL) {
      filename = minc_file;
   }
   else {
      /* Get patient name */
      string_to_filename(general_info->patient.name, patient_name,
                         sizeof(patient_name));
      if (STRLEN(patient_name) == 0) {
         (void) strcpy(patient_name, "unknown");
      }

      /* Get strings for echo number, etc. */
      for (imri=0; imri < MRI_NDIMS; imri++) {
         if ((general_info->size[imri] < general_info->total_size[imri]) &&
             (general_info->size[imri] == 1)) {
            (void) sprintf(scan_label[imri], "_%s%d", scan_prefix[imri],
                           general_info->default_index[imri]+1);
         }
         else {
            (void) strcpy(scan_label[imri], "");
         }
      }

      /* Create file name */
      (void) sprintf(temp_name, "%s%s_%s_%s%s%s%s%s%s_mri.mnc", 
                     file_prefix,
                     patient_name,
                     general_info->study.study_id, 
                     general_info->study.acquisition_id,
                     scan_label[SLICE],
                     scan_label[ECHO],
                     scan_label[TIME],
                     scan_label[PHASE],
                     scan_label[CHEM_SHIFT]);
      filename = temp_name;
   }

   /* Set output file name */
   if (output_file_name != NULL)
      *output_file_name = filename;

   /* Set the clobber value */
   if (clobber) minc_clobber = NC_CLOBBER;
   else minc_clobber = NC_NOCLOBBER;

   /* Create the file */
   mincid = micreate(filename, minc_clobber);
   if (mincid == MI_ERROR) return MI_ERROR;

   /* Set up variables */
   setup_minc_variables(mincid, general_info);

   /* Put the file in data mode */
   (void) ncsetfill(mincid, NC_NOFILL);
   if (ncendef(mincid) == MI_ERROR) {
      return MI_ERROR;
   }

   /* Create the icv */
   icvid = miicv_create();

   /* Set the type and range */
   (void) miicv_setint(icvid, MI_ICV_TYPE, NC_SHORT);
   if (general_info->is_signed)
      (void) miicv_setstr(icvid, MI_ICV_SIGN, MI_SIGNED);
   else
      (void) miicv_setstr(icvid, MI_ICV_SIGN, MI_UNSIGNED);
   (void) miicv_setdbl(icvid, MI_ICV_VALID_MIN, general_info->pixel_min);
   (void) miicv_setdbl(icvid, MI_ICV_VALID_MAX, general_info->pixel_max);

   /* Attach the icv */
   (void) miicv_attach(icvid, mincid, ncvarid(mincid, MIimage));

   return icvid;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : setup_minc_variables
@INPUT      : mincid
              general_info
@OUTPUT     : general_info
@RETURNS    : (nothing)
@DESCRIPTION: Routine to setup minc variables.
@METHOD     : 
@GLOBALS    : 
CALLS       : 
@CREATED    : November 26, 1993 (Peter Neelin)
@MODIFIED   :
---------------------------------------------------------------------------- */
public void setup_minc_variables(int mincid, General_Info *general_info)
{
   Mri_Index imri;
   Volume_Index ivol;
   World_Index iworld;
   int ndims;
   int dim[MAX_VAR_DIMS];
   long dimsize;
   char *dimname;
   int varid, imgid, dicomvar;
   double valid_range[2];
   char name[MAX_NC_NAME];
   int index;
   int regular;
   double separation, diff;
   Acr_Group cur_group;
   Acr_Element cur_element;
   int length;
   char *data;
   nc_type datatype;
   int is_char;
   int ich;

   /* Define the spatial dimension names */
   static char *spatial_dimnames[WORLD_NDIMS] = {MIxspace, MIyspace, MIzspace};

   /* Create the dimensions from slowest to fastest */

   ndims=0;
   /* Create the non-spatial dimensions (from slowest to fastest) */
   for (imri=MRI_NDIMS-1; (int) imri > SLICE; imri--) {
      dimsize = general_info->size[imri];
      if (general_info->size[imri] > 1) {
         dimname = mri_dim_names[imri];
         dim[ndims] = ncdimdef(mincid, dimname, dimsize);
         if (imri == TIME) {
            varid = micreate_std_variable(mincid, dimname, NC_DOUBLE, 1, 
                                          &dim[ndims]);
            (void) miattputstr(mincid, varid, MIunits, "s");
         }
         else if (imri == ECHO) {
            varid = ncvardef(mincid, dimname, NC_DOUBLE, 1, &dim[ndims]);
            (void) miattputstr(mincid, varid, MIvartype, MI_DIMENSION);
            (void) miattputstr(mincid, varid, MIspacing, MI_IRREGULAR);
            (void) miattputstr(mincid, varid, MIunits, "s");
         }
         general_info->image_index[imri] = ndims;
         ndims++;
      }
   }

   /* Next the spatial dimensions */
   for (ivol=0; ivol < VOL_NDIMS; ivol++) {
      switch (ivol) {
      case VSLICE: 
         dimsize = general_info->size[SLICE];
         iworld = general_info->slice_world;
         break;
      case VROW: 
         dimsize = general_info->nrows;
         iworld = general_info->row_world;
         break;
      case VCOLUMN: 
         dimsize = general_info->ncolumns;
         iworld = general_info->column_world;
         break;
      }
      dimname = spatial_dimnames[iworld];
      dim[ndims] = ncdimdef(mincid, dimname, dimsize);
      if (ivol == VSLICE) {
         varid = micreate_std_variable(mincid, dimname, NC_DOUBLE, 
                                       1, &dim[ndims]);
         /* Check for regular slices */
         regular = TRUE;
         separation = general_info->step[general_info->slice_world];
         for (index=1; index < general_info->size[SLICE]; index++) {
            diff = general_info->coordinates[SLICE][index] -
               general_info->coordinates[SLICE][index-1] - separation;
            if (diff < 0.0) diff = -diff;
            if (separation != 0.0) diff /= separation;
            if (diff > COORDINATE_EPSILON) {
               regular = FALSE;
               break;
            }
         }
         if (regular)
            (void) miattputstr(mincid, varid, MIspacing, MI_REGULAR);
      }
      else
         varid = micreate_std_variable(mincid, dimname, NC_LONG, 0, NULL);
      (void) miattputdbl(mincid, varid, MIstep, 
                         general_info->step[iworld]);
      (void) miattputdbl(mincid, varid, MIstart, 
                         general_info->start[iworld]);
      (void) miattputstr(mincid, varid, MIspacetype, MI_NATIVE);
      (void) ncattput(mincid, varid, MIdirection_cosines, 
                      NC_DOUBLE, WORLD_NDIMS,
                      general_info->dircos[iworld]);
      if (ivol == VSLICE) {
         general_info->image_index[SLICE] = ndims;
      }
      ndims++;
   }

   /* Set up image variable */
   imgid = micreate_std_variable(mincid, MIimage, general_info->datatype,
                                 ndims, dim);
   if (general_info->is_signed)
      (void) miattputstr(mincid, imgid, MIsigntype, MI_SIGNED);
   else
      (void) miattputstr(mincid, imgid, MIsigntype, MI_UNSIGNED);
   valid_range[0] = general_info->pixel_min;
   valid_range[1] = general_info->pixel_max;
   (void) ncattput(mincid, imgid, MIvalid_range, NC_DOUBLE, 2, valid_range);
   (void) miattputstr(mincid, imgid, MIcomplete, MI_FALSE);

   /* Create image max and min variables */
   varid = micreate_std_variable(mincid, MIimagemin, NC_DOUBLE, ndims-2, dim);
   if (STRLEN(general_info->units) > 0)
      (void) miattputstr(mincid, varid, MIunits, general_info->units);
   varid = micreate_std_variable(mincid, MIimagemax, NC_DOUBLE, ndims-2, dim);
   if (STRLEN(general_info->units) > 0)
      (void) miattputstr(mincid, varid, MIunits, general_info->units);

   /* Create the patient variable */
   varid = micreate_group_variable(mincid, MIpatient);
   if (STRLEN(general_info->patient.name) > 0)
      (void) miattputstr(mincid, varid, MIfull_name, 
                         general_info->patient.name);
   if (STRLEN(general_info->patient.identification) > 0)
      (void) miattputstr(mincid, varid, MIidentification, 
                         general_info->patient.identification);
   if (STRLEN(general_info->patient.birth_date) > 0)
      (void) miattputstr(mincid, varid, MIbirthdate, 
                         general_info->patient.birth_date);
   if (STRLEN(general_info->patient.sex) > 0)
      (void) miattputstr(mincid, varid, MIsex, 
                         general_info->patient.sex);
   if (general_info->patient.weight != -DBL_MAX) 
      (void) miattputdbl(mincid, varid, MIweight, 
                         general_info->patient.weight);

   /* Create the study variable */
   varid = micreate_group_variable(mincid, MIstudy);
   if (STRLEN(general_info->study.start_time) > 0)
      (void) miattputstr(mincid, varid, MIstart_time, 
                         general_info->study.start_time);
   if (STRLEN(general_info->study.modality) > 0)
      (void) miattputstr(mincid, varid, MImodality, 
                         general_info->study.modality);
   if (STRLEN(general_info->study.institution) > 0)
      (void) miattputstr(mincid, varid, MIinstitution, 
                         general_info->study.institution);
   if (STRLEN(general_info->study.station_id) > 0)
      (void) miattputstr(mincid, varid, MIstation_id, 
                         general_info->study.station_id);
   if (STRLEN(general_info->study.ref_physician) > 0)
      (void) miattputstr(mincid, varid, MIreferring_physician, 
                         general_info->study.ref_physician);
   if (STRLEN(general_info->study.procedure) > 0)
      (void) miattputstr(mincid, varid, MIprocedure, 
                         general_info->study.procedure);
   if (STRLEN(general_info->study.study_id) > 0)
      (void) miattputstr(mincid, varid, MIstudy_id, 
                         general_info->study.study_id);
   if (STRLEN(general_info->study.acquisition_id) > 0)
      (void) miattputstr(mincid, varid, "acquisition_id", 
                         general_info->study.acquisition_id);

   /* Create acquisition variable */
   varid = micreate_group_variable(mincid, MIacquisition);
   if (STRLEN(general_info->acq.scan_seq) > 0)
      (void) miattputstr(mincid, varid, MIscanning_sequence, 
                         general_info->acq.scan_seq);
   if (general_info->acq.rep_time != -DBL_MAX)
      (void) miattputdbl(mincid, varid, MIrepetition_time, 
                         general_info->acq.rep_time);
   if ((general_info->acq.echo_time != -DBL_MAX) &&
       (general_info->size[ECHO] <= 1))
      (void) miattputdbl(mincid, varid, MIecho_time, 
                         general_info->acq.echo_time);
   if (general_info->acq.inv_time != -DBL_MAX)
      (void) miattputdbl(mincid, varid, MIinversion_time, 
                         general_info->acq.inv_time);
   if (general_info->acq.flip_angle != -DBL_MAX)
      (void) miattputdbl(mincid, varid, "flip_angle", 
                         general_info->acq.flip_angle);
   if (general_info->acq.num_avg != -DBL_MAX)
      (void) miattputdbl(mincid, varid, MInum_averages, 
                         general_info->acq.num_avg);
   if (general_info->acq.imaging_freq != -DBL_MAX)
      (void) miattputdbl(mincid, varid, MIimaging_frequency, 
                         general_info->acq.imaging_freq);
   if (STRLEN(general_info->acq.imaged_nucl) > 0)
      (void) miattputstr(mincid, varid, MIimaged_nucleus, 
                         general_info->acq.imaged_nucl);
   if (STRLEN(general_info->acq.comments) > 0)
      (void) miattputstr(mincid, varid, MIcomments, 
                         general_info->acq.comments);

   /* Create the dicom info variable */
   varid = ncvardef(mincid, "dicominfo", NC_LONG, 0, NULL);
   (void) miattputstr(mincid, varid, MIvartype, MI_GROUP);
   (void) miattputstr(mincid, varid, MIvarid, 
                      "MNI DICOM information variable");
   (void) miadd_child(mincid, ncvarid(mincid, MIrootvariable), varid);
   if (STRLEN(general_info->image_type_string) > 0)
      (void) miattputstr(mincid, varid, "image_type", 
                         general_info->image_type_string);
   (void) miattputdbl(mincid, varid, "window_min", general_info->window_min);
   (void) miattputdbl(mincid, varid, "window_max", general_info->window_max);

   /* Put group info in header */
   cur_group = general_info->group_list;
   dicomvar = ncvardef(mincid, DICOM_ROOT_VAR, NC_LONG, 0, NULL);
   (void) miattputstr(mincid, dicomvar, MIvartype, MI_GROUP);
   (void) miattputstr(mincid, dicomvar, MIvarid, "MNI DICOM variable");
   (void) miadd_child(mincid, ncvarid(mincid, MIrootvariable), dicomvar);
   while (cur_group != NULL) {

      /* Create variable for group */
      (void) sprintf(name, "dicom_0x%04x", acr_get_group_group(cur_group));
      varid = ncvardef(mincid, name, NC_LONG, 0, NULL);
      (void) miattputstr(mincid, varid, MIvartype, MI_GROUP);
      (void) miattputstr(mincid, varid, MIvarid, "MNI DICOM variable");
      (void) miadd_child(mincid, dicomvar, varid);

      /* Loop through elements of group */
      cur_element = acr_get_group_element_list(cur_group);
      while (cur_element != NULL) {
         (void) sprintf(name, "el_0x%04x", 
                        acr_get_element_element(cur_element));
         is_char = TRUE;
         length = acr_get_element_length(cur_element);
         data = acr_get_element_data(cur_element);
         for (ich=0; ich < length; ich++) {
            if (!isprint((int) data[ich])) {
               is_char = FALSE;
               break;
            }
         }
         if (is_char)
            datatype = NC_CHAR;
         else
            datatype = NC_BYTE;
         ncattput(mincid, varid, name, datatype, length, data);
         
         cur_element = acr_get_element_next(cur_element);
      }
      cur_group = acr_get_group_next(cur_group);
   }

   /* Create the history attribute */
   if (minc_history != NULL) {
      (void) miattputstr(mincid, NC_GLOBAL, MIhistory, minc_history);
   }

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : save_minc_image
@INPUT      : icvid
              general_info
              file_info
              image
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to save the image in the minc file
@METHOD     : 
@GLOBALS    : 
CALLS       : 
@CREATED    : November 26, 1993 (Peter Neelin)
@MODIFIED   :
---------------------------------------------------------------------------- */
public void save_minc_image(int icvid, General_Info *general_info, 
                            File_Info *file_info, Image_Data *image)
{
   int mincid, imgid;
   long start[MAX_VAR_DIMS], count[MAX_VAR_DIMS];
   int file_index, array_index;
   int idim;
   Mri_Index imri;
   char *dimname;
   unsigned short pvalue, pmax, pmin;
   double dvalue, maximum, minimum, scale, offset;
   long ipix, imagepix;

   /* Get the minc file id */
   (void) miicv_inqint(icvid, MI_ICV_CDFID, &mincid);
   (void) miicv_inqint(icvid, MI_ICV_VARID, &imgid);

   /* Create start and count variables */
   idim = 0;
   for (imri=MRI_NDIMS-1; (int) imri >= 0; imri--) {
      if (general_info->image_index[imri] >= 0) {
         file_index = general_info->image_index[imri];
         if (general_info->size[imri] > 1) {
            array_index = search_list(file_info->index[imri], 
                                      general_info->indices[imri],
                                      general_info->size[imri],
                                      general_info->search_start[imri]);
            if (array_index < 0) array_index = 0;
            general_info->search_start[imri] = array_index;
         }
         else {
            array_index = 0;
         }
         start[file_index] = array_index;
         count[file_index] = 1;
         idim++;
      }
   }
   start[idim] = 0;
   start[idim+1] = 0;
   count[idim] = general_info->nrows;
   count[idim+1] = general_info->ncolumns;

   /* Write out slice position */
   switch (general_info->slice_world) {
   case XCOORD: dimname = MIxspace; break;
   case YCOORD: dimname = MIyspace; break;
   case ZCOORD: dimname = MIzspace; break;
   default: dimname = MIzspace;
   }
   (void) mivarput1(mincid, ncvarid(mincid, dimname), 
                    &start[general_info->image_index[SLICE]], 
                    NC_DOUBLE, NULL, &file_info->coordinate[SLICE]);

   /* Write out time of slice, if needed */
   if (general_info->size[TIME] > 1) {
      (void) mivarput1(mincid, ncvarid(mincid, mri_dim_names[TIME]), 
                       &start[general_info->image_index[TIME]], 
                       NC_DOUBLE, NULL, &file_info->coordinate[TIME]);
   }

   /* Write out echo time of slice, if needed */
   if (general_info->size[ECHO] > 1) {
      (void) mivarput1(mincid, ncvarid(mincid, mri_dim_names[ECHO]), 
                       &start[general_info->image_index[ECHO]], 
                       NC_DOUBLE, NULL, &file_info->coordinate[ECHO]);
   }

   /* Search image for max and min */
   imagepix = general_info->nrows * general_info->ncolumns;
   pmax = 0;
   pmin = USHRT_MAX;
   for (ipix=0; ipix < imagepix; ipix++) {
      pvalue = image->data[ipix];
      if (pvalue > pmax) pmax = pvalue;
      if (pvalue < pmin) pmin = pvalue;
   }

   /* Re-scale the images */
   if (pmax > pmin)
      scale = (general_info->pixel_max - general_info->pixel_min) /
         ((double) pmax - (double) pmin);
   else
      scale = 0.0;
   offset = general_info->pixel_min - scale * (double) pmin;
   for (ipix=0; ipix < imagepix; ipix++) {
      dvalue = image->data[ipix];
      image->data[ipix] = dvalue * scale + offset;
   }

   /* Calculate new intensity max and min */
   if (general_info->pixel_max > general_info->pixel_min)
      scale = (file_info->slice_max - file_info->slice_min) /
         (general_info->pixel_max - general_info->pixel_min);
   else
      scale = 0.0;
   offset = file_info->slice_min - scale * general_info->pixel_min;
   minimum = (double) pmin * scale + offset;
   maximum = (double) pmax * scale + offset;

   /* Write out the max and min values */
   (void) mivarput1(mincid, ncvarid(mincid, MIimagemin), start, NC_DOUBLE,
                    NULL, &minimum);
   (void) mivarput1(mincid, ncvarid(mincid, MIimagemax), start, NC_DOUBLE,
                    NULL, &maximum);

   /* Write out the image */
   (void) miicv_put(icvid, start, count, image->data);

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : close_minc_file
@INPUT      : icvid - value returned by create_minc_file
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to close the minc file.
@METHOD     : 
@GLOBALS    : 
CALLS       : 
@CREATED    : November 30, 1993 (Peter Neelin)
@MODIFIED   :
---------------------------------------------------------------------------- */
public void close_minc_file(int icvid)
{
   int mincid;

   /* Get the minc file id */
   (void) miicv_inqint(icvid, MI_ICV_CDFID, &mincid);

   /* Write out the complete attribute */
   (void) miattputstr(mincid, ncvarid(mincid, MIimage), MIcomplete, MI_TRUE);

   /* Close the file */
   (void) miclose(mincid);

   (void) miicv_free(icvid);

   return;
}
