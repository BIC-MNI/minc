/* ----------------------------- MNI Header -----------------------------------
@NAME       : gyro_read.c
@DESCRIPTION: Code to read gyrocom files and get info from them.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 25, 1993 (Peter Neelin)
@MODIFIED   : 
 * $Log: gyro_read.c,v $
 * Revision 6.2  2001-04-09 23:02:49  neelin
 * Modified copyright notice, removing permission statement since copying,
 * etc. is probably not permitted by our non-disclosure agreement with
 * Philips.
 *
 * Revision 6.1  1999/10/29 17:52:03  neelin
 * Fixed Log keyword
 *
 * Revision 6.0  1997/09/12 13:23:50  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:24:50  neelin
 * Release of minc version 0.5
 *
 * Revision 4.1  1997/06/13  22:08:13  neelin
 * Modifications to get gcomserver working with modified Acr_nema library.
 *
 * Revision 4.0  1997/05/07  20:01:07  neelin
 * Release of minc version 0.4
 *
 * Revision 3.1  1995/08/02  13:41:36  neelin
 * Fixed bug in direction cosine inversion (in test cases, this code was never
 * called, so it does not seem to be an important bug).
 *
 * Revision 3.0  1995/05/15  19:31:44  neelin
 * Release of minc version 0.3
 *
 * Revision 2.7  1995/03/21  15:43:19  neelin
 * Corrected setting of default image type.
 *
 * Revision 2.6  1995/02/14  18:12:26  neelin
 * Added project names and defaults files (using volume name).
 * Added process id to log file name.
 * Moved temporary files to subdirectory.
 *
 * Revision 2.5  1995/02/08  19:31:47  neelin
 * Moved ARGSUSED statements for irix 5 lint.
 *
 * Revision 2.4  1994/12/12  09:05:57  neelin
 * Changed comment in calculate_slice_start (code is the same)
 *
 * Revision 2.3  94/12/07  08:20:22  neelin
 * Fixed some lint messages.
 * 
 * Revision 2.2  94/11/21  08:07:59  neelin
 * Modified code to properly calculate start from centre locations, then
 * changed calculation back to old way because it worked.
 * Added a ncsetfill(mincid, NC_NOFILL).
 * 
 * Revision 2.1  94/10/20  13:50:11  neelin
 * Write out direction cosines to support rotated volumes.
 * Store single slices as 1-slice volumes (3D instead of 2D).
 * Changed storing of minc history (get args for gyrotominc).
 * 
 * Revision 2.0  94/09/28  10:35:27  neelin
 * Release of minc version 0.2
 * 
 * Revision 1.10  94/09/28  10:34:59  neelin
 * Pre-release
 * 
 * Revision 1.9  94/05/24  15:06:35  neelin
 * Break up multiple echoes or time frames into separate files for 2 echoes
 * or 2 frames (put in 1 file for more).
 * Changed units of repetition time, echo time, etc to seconds.
 * Save echo times in dimension variable when appropriate.
 * Changed to file names to end in _mri.mnc.
 * 
 * Revision 1.8  94/03/15  14:24:01  neelin
 * Changed image-max/min to use fp_scaled_max/min instead of ext_scale_max/min
 * Added acquisition:comments attribute
 * Changed reading of configuration file to allow execution of a command on
 * the minc file.
 * 
 * Revision 1.7  94/03/14  16:44:30  neelin
 * Changed scale to be fp_scaled_min/max instead of ext_scale_min/max.
 * Check units for millirad and change to radians.
 * 
 * Revision 1.6  94/01/17  15:05:46  neelin
 * Added some acquisition parameters (flip angle) and fixed error in writing
 * of scanning sequence.
 * 
 * Revision 1.5  94/01/14  11:37:18  neelin
 * Fixed handling of multiple reconstructions and image types. Add spiinfo variable with extra info (including window min/max). Changed output
 * file name to include reconstruction number and image type number.
 * 
 * Revision 1.4  94/01/06  14:18:44  neelin
 * Added image unpacking (3 bytes -> 2 pixels).
 * 
 * Revision 1.3  93/12/14  16:36:49  neelin
 * Fixed axis direction and start position.
 * 
 * Revision 1.2  93/12/10  15:35:07  neelin
 * Improved file name generation from patient name. No buffering on stderr.
 * Added spi group list to minc header.
 * Optionally read a defaults file to get output minc directory and owner.
 * 
 * Revision 1.1  93/11/30  14:41:04  neelin
 * Initial revision
 * 
@COPYRIGHT  :
              Copyright 1993 Peter Neelin, McConnell Brain Imaging Centre, 
              Montreal Neurological Institute, McGill University.
---------------------------------------------------------------------------- */

#include <gcomserver.h>
#include <math.h>

/* Define strings for spi image types */
static struct {
   int image_type;
   char *name;
} Spi_image_type_strings[] = {
   0,  "SE_R",
   1,  "SE_I",
   2,  "SE_M",
   3,  "SE_P",
   13, "SE/CR",
   4,  "SE_T2",
   5,  "SE_RHO",
   6,  "IR_R",
   7,  "IR_I",
   8,  "IR_M",
   9,  "IR_P",
   14, "IR/CR",
   10, "IR_T2",
   11, "IR_RHO",
   12, "T1",
   15, "T2",
   16, "RHO",
   17, "FFE/CR",
   18, "FFE/R",
   19, "FFE/I",
   20, "FFE/M",
   21, "FFE/P",
   22, "Spectra",
   23, "PCA_R",
   24, "PCA_I",
   25, "PCA_M",
   26, "PCA_P",
   27, "Derived"
};

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_gyro
@INPUT      : filename - name of gyrocom file to read
              max_group - maximum group number to read
@OUTPUT     : (none)
@RETURNS    : group list read in from file
@DESCRIPTION: Routine to read in a group list from a file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 25, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Group read_gyro(char *filename, int max_group)
{
   FILE *fp;
   Acr_File *afp;
   Acr_Group group_list;

   /* Open the file */
   fp = fopen(filename, "r");
   if (fp == NULL) return NULL;

   /* Connect to input stream */
   afp=acr_file_initialize(fp, 0, acr_stdio_read);
   (void) acr_test_byte_order(afp);

   /* Read in group list */
   (void) acr_input_group_list(afp, &group_list, max_group);

   /* Close the file */
   acr_file_free(afp);
   (void) fclose(fp);

   return group_list;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_file_info
@INPUT      : group_list - input data
@OUTPUT     : file_info - file-specific info
              general_info - general information about files
@RETURNS    : (nothing)
@DESCRIPTION: Routine to extract information from a group list
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 25, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void get_file_info(Acr_Group group_list, File_Info *file_info,
                          General_Info *general_info)
{
   Acr_Element element;
   int data_set_type;
   Mri_Index imri;
   World_Index iworld, jworld;
   char *string;
   int nrows;
   int ncolumns;
   int study_id, acq_id, rec_num, image_type;
   int image_group;
   int length, maxlen;
   int cur_index;
   int index;
   Orientation orientation;
   double default_slice_pos;
   double angulation_ap, angulation_lr, angulation_cc;
   char *image_type_string;
   char image_type_string_buf[20];
   Acr_Element_Id mri_index_list[MRI_NDIMS];
   Acr_Element_Id mri_total_list[MRI_NDIMS];
   Acr_Element_Id off_center_elid[WORLD_NDIMS];

   /* Directions of axes for different orientations */
   static double axis_direction[NUM_ORIENTATIONS][WORLD_NDIMS] = {
      -1.0, -1.0, +1.0,
      +1.0, -1.0, -1.0,
      -1.0, -1.0, -1.0};

   /* Array of elements for mri dimensions */
   mri_index_list[SLICE] = SPI_Slice_number;
   mri_index_list[ECHO] = SPI_Echo_number;
   mri_index_list[TIME] = SPI_Dynamic_scan_number;
   mri_index_list[PHASE] = SPI_Phase_number;
   mri_index_list[CHEM_SHIFT] = SPI_Chemical_shift_number;
   mri_total_list[SLICE] = SPI_Number_of_slices;
   mri_total_list[ECHO] = SPI_Number_of_echoes;
   mri_total_list[TIME] = SPI_Number_of_dynamic_scans;
   mri_total_list[PHASE] = SPI_Number_of_phases;
   mri_total_list[CHEM_SHIFT] = SPI_Nr_of_chemical_shifts;

   /* Array of elements giving off center for each axis */
   off_center_elid[XCOORD] = SPI_Off_center_lr; 
   off_center_elid[YCOORD] = SPI_Off_center_ap;
   off_center_elid[ZCOORD] = SPI_Off_center_cc;

   /* Look for data set type */
   element = acr_find_group_element(group_list, ACR_Data_set_type);
   if (element != NULL)
      data_set_type = acr_get_element_short(element);

   if ((element == NULL) || (data_set_type != ACR_IMAGE_OBJECT)) {
      file_info->valid = FALSE;
      return;
   }

   /* Get stuff that must be in file */
   file_info->bits_alloc = find_short(group_list, ACR_Bits_allocated, 0);
   file_info->bits_stored = find_short(group_list, ACR_Bits_stored, 0);
   nrows = find_short(group_list, ACR_Rows, 0);
   ncolumns = find_short(group_list, ACR_Columns, 0);
   if (nrows <= 0)
      nrows = find_int(group_list, SPI_Recon_resolution, 0);
   if (ncolumns <= 0)
      ncolumns = find_int(group_list, SPI_Recon_resolution, 0);
   image_group = find_short(group_list, ACR_Image_location, INT_MIN);

   /* Check for necessary values not found */
   if ((nrows <= 0) || (ncolumns <= 0) ||
       (file_info->bits_stored <= 0) ||
       (file_info->bits_alloc <= 0) || 
       (image_group < 0)) {
      file_info->valid = FALSE;
      return;
   }

   /* Get study, acq, rec, image type id's */
   element = acr_find_group_element(group_list, ACR_Study);
   if (element != NULL)
      study_id = (int) acr_get_element_numeric(element);
   else
      study_id = 0;
   element = acr_find_group_element(group_list, ACR_Acquisition);
   if (element != NULL)
      acq_id = (int) acr_get_element_numeric(element);
   else
      acq_id = 0;
   element = acr_find_group_element(group_list, SPI_Reconstruction_number);
   if (element != NULL)
      rec_num = (int) acr_get_element_numeric(element);
   else
      rec_num = 1;
   element = acr_find_group_element(group_list, SPI_Image_type);
   if (element != NULL)
      image_type = (int) acr_get_element_numeric(element);
   else
      image_type = SPI_DEFAULT_IMAGE_TYPE;

   /* Get pixel value information */
   file_info->pixel_min = find_short(group_list, ACR_Smallest_pixel_value, 0);
   file_info->pixel_max = find_short(group_list, ACR_Largest_pixel_value, 
                                     (1 << file_info->bits_stored) - 1);
   file_info->slice_min = find_double(group_list, SPI_Fp_scaled_minimum,
                                      (double) file_info->pixel_min);
   file_info->slice_max = find_double(group_list, SPI_Fp_scaled_maximum, 
                                      (double) file_info->pixel_max);

   /* Get window min and max */
   file_info->window_min =
      find_double(group_list, SPI_Fp_window_minimum, file_info->slice_min);
   file_info->window_max =
      find_double(group_list, SPI_Fp_window_maximum, file_info->slice_max);

   /* Get image indices */
   for (imri=0; imri < MRI_NDIMS; imri++) {
      file_info->index[imri] = 
         find_int(group_list, mri_index_list[imri], 1) - 1;
      if (file_info->index[imri] < 0)
         file_info->index[imri] = 0;
   }

   /* Set up general info on first pass */
   if (!general_info->initialized) {

      /* Get row and columns sizes */
      general_info->nrows = nrows;
      general_info->ncolumns = ncolumns;

      /* Save the study, acqusition, reconstruction and image type 
         identifiers */
      general_info->study_id = study_id;
      general_info->acq_id = acq_id;
      general_info->rec_num = rec_num;
      general_info->image_type = image_type;

      /* Get a name for the image type (if not found, use the number) */
      image_type_string = NULL;
      for (index=0; index < ARRAY_SIZE(Spi_image_type_strings); index++) {
         if (image_type == Spi_image_type_strings[index].image_type) {
            image_type_string = Spi_image_type_strings[index].name;
            break;
         }            
      }
      if (image_type_string == NULL) {
         image_type_string = image_type_string_buf;
         (void) sprintf(image_type_string, "%d", image_type);
      }
      (void) strncpy(general_info->image_type_string, 
                     image_type_string, sizeof(Cstring) - 1);

      /* Get dimension information */
      for (imri=0; imri < MRI_NDIMS; imri++) {
         general_info->size[imri] = 1;
         general_info->first[imri] = file_info->index[imri];
         general_info->total_size[imri] = 
            find_int(group_list, mri_total_list[imri], 1);
         if (general_info->total_size[imri] < 1)
            general_info->total_size[imri] = 1;
         general_info->position[imri] = NULL;
         general_info->image_index[imri] = -1;
      }

      /* Set direction cosines from rotation angles */
      angulation_ap = find_double(group_list, SPI_Angulation_of_ap_axis, 0.0);
      angulation_lr = find_double(group_list, SPI_Angulation_of_lr_axis, 0.0);
      angulation_cc = find_double(group_list, SPI_Angulation_of_cc_axis, 0.0);
      get_direction_cosines(angulation_ap, angulation_lr, angulation_cc,
                            general_info->dircos);

      /* Get spatial orientation */
      switch (find_int(group_list, SPI_Slice_orientation, 0)) {
      case SPI_SAGITTAL_ORIENTATION:
         orientation = SAGITTAL; break;
      case SPI_CORONAL_ORIENTATION:
         orientation = CORONAL; break;
      case SPI_TRANSVERSE_ORIENTATION:
      default:
         orientation = TRANSVERSE; break;
      }
      get_orientation_info(orientation, general_info->dircos,
                           &general_info->slice_world,
                           &general_info->row_world,
                           &general_info->column_world);

      /* Get step information. Use field-of-view since pixel size doesn't
         seem to be correct */
#if 1
      general_info->step[general_info->row_world] = 
         find_double(group_list, SPI_Field_of_view, (double) nrows)
            / (double) nrows;
      if (general_info->step[general_info->row_world] == 0.0) {
         general_info->step[general_info->row_world] = 1.0;
      }
      general_info->step[general_info->column_world] = 
         general_info->step[general_info->row_world];
#else      
      string = find_string(group_list, ACR_Pixel_size, "");
      general_info->step[general_info->column_world] = strtod(string, &end);
      if (end == string) {
         general_info->step[general_info->column_world] = 1.0;
      }
      else {
         string = end;
         if ((*string == '\\') || (*string == ',')) string++;
         general_info->step[general_info->row_world] = strtod(string, &end);
         if (end == string)
            general_info->step[general_info->row_world] = 1.0;
      }
#endif
      general_info->step[general_info->slice_world] = 
         find_double(group_list, ACR_Slice_thickness, 1.0);
      general_info->step[XCOORD] *= axis_direction[orientation][XCOORD];
      general_info->step[YCOORD] *= axis_direction[orientation][YCOORD];
      general_info->step[ZCOORD] *= axis_direction[orientation][ZCOORD];

      /* Make sure that direction cosines point the right way (dot product
         of direction cosine and axis is positive) and that step has proper
         sign */
      for (iworld = XCOORD; iworld < WORLD_NDIMS; iworld++) {
         if (general_info->dircos[iworld][iworld] < 0.0) {
            general_info->step[iworld] *= -1.0;
            for (jworld = XCOORD; jworld < WORLD_NDIMS; jworld++) {
               general_info->dircos[iworld][jworld] *= -1.0;
            }
         }
      }

      /* Get centre information (start info is calculated farther down) */
      general_info->centre[XCOORD] = 
         find_double(group_list, SPI_Off_center_lr, 0.0);
      general_info->centre[YCOORD] = 
         find_double(group_list, SPI_Off_center_ap, 0.0);
      general_info->centre[ZCOORD] = 
         find_double(group_list, SPI_Off_center_cc, 0.0);
      calculate_slice_start(general_info->slice_world,
                            general_info->row_world,
                            general_info->column_world,
                            general_info->nrows,
                            general_info->ncolumns,
                            general_info->centre,
                            general_info->step,
                            general_info->dircos,
                            general_info->start);

      /* Keep track of range of slices */
      general_info->slicepos_first = 
         general_info->centre[general_info->slice_world];
      general_info->slicepos_last = 
         general_info->centre[general_info->slice_world];
      general_info->sliceindex_first = file_info->index[SLICE];
      general_info->sliceindex_last = file_info->index[SLICE];

      /* Save slice step info */
      iworld = general_info->slice_world;
      general_info->slice_step = general_info->step[iworld];
      general_info->slice_start = general_info->centre[iworld] -
         file_info->index[SLICE] * general_info->slice_step;

      /* Set data type and range */
      if (file_info->bits_alloc <= 8)
         general_info->datatype = NC_BYTE;
      else
         general_info->datatype = NC_SHORT;
      general_info->is_signed = ((general_info->datatype == NC_SHORT) &&
                                 (file_info->bits_stored < 16));
      general_info->pixel_min = file_info->pixel_min;
      general_info->pixel_max = file_info->pixel_max;

      /* Save display window info */
      general_info->window_min = file_info->window_min;
      general_info->window_max = file_info->window_max;

      /* Maximum length for strings */
      maxlen = sizeof(Cstring) - 1;

      /* Get intensity units */
      (void) strncpy(general_info->units,
              find_string(group_list, SPI_Ext_scale_units, ""), maxlen);
      if (strcmp(general_info->units, "millirad") == 0) {
         (void) strncpy(general_info->units, "radians", maxlen);
      }

      /* Get patient info */
      (void) strncpy(general_info->patient.name,
              find_string(group_list, ACR_Patient_name, ""), maxlen);
      (void) strncpy(general_info->patient.identification,
              find_string(group_list, ACR_Patient_identification, ""), maxlen);
      (void) strncpy(general_info->patient.birth_date,
              find_string(group_list, ACR_Patient_birth_date, ""), maxlen);
      string = find_string(group_list, ACR_Patient_sex, "");
      if (*string == 'M') 
         (void) strncpy(general_info->patient.sex, MI_MALE, maxlen);
      else if (*string == 'F') 
         (void) strncpy(general_info->patient.sex, MI_FEMALE, maxlen);
      else if (*string == 'O') 
         (void) strncpy(general_info->patient.sex, MI_OTHER, maxlen);
      else 
         (void) strncpy(general_info->patient.sex, "", maxlen);
      general_info->patient.weight = 
         find_double(group_list, ACR_Patient_weight, -DBL_MAX);

      /* Get study info */
      (void) strncpy(general_info->study.start_time, 
              find_string(group_list, ACR_Study_date, ""), maxlen - 1);
      length = strlen(general_info->study.start_time);
      general_info->study.start_time[length] = ' ';
      length++;
      (void) strncpy(&general_info->study.start_time[length], 
              find_string(group_list, ACR_Study_time, ""), maxlen - length);
      string = find_string(group_list, ACR_Modality, "");
      if (strcmp(string, ACR_MODALITY_MR) == 0)
         (void) strncpy(general_info->study.modality, MI_MRI, maxlen);
      (void) strncpy(general_info->study.manufacturer, 
              find_string(group_list, ACR_Manufacturer, ""), maxlen);
      (void) strncpy(general_info->study.model, 
              find_string(group_list, ACR_Manufacturer_model, ""), maxlen);
      (void) strncpy(general_info->study.institution, 
              find_string(group_list, ACR_Institution_id, ""), maxlen);
      (void) strncpy(general_info->study.station_id, 
              find_string(group_list, ACR_Station_id, ""), maxlen);
      (void) strncpy(general_info->study.ref_physician, 
              find_string(group_list, ACR_Referring_physician, ""), maxlen);
      (void) strncpy(general_info->study.procedure, 
              find_string(group_list, ACR_Procedure_description, ""), maxlen);
      (void) sprintf(general_info->study.study_id, "%d",
                     find_int(group_list, ACR_Study, 0));
      (void) sprintf(general_info->study.acquisition_id, "%d",
                     find_int(group_list, ACR_Acquisition, 0));

      /* Get acquisition information */
      (void) strncpy(general_info->acq.scan_seq,
              find_string(group_list, ACR_Scanning_sequence, ""), maxlen);
      general_info->acq.rep_time = 
         find_double(group_list, ACR_Repetition_time, -DBL_MAX) / 1000.0;
      general_info->acq.echo_time = 
         find_double(group_list, ACR_Echo_time, -DBL_MAX) / 1000.0;
      general_info->acq.inv_time = 
         find_double(group_list, ACR_Inversion_time, -DBL_MAX) / 1000.0;
      general_info->acq.flip_angle = 
         find_double(group_list, SPI_Flip_angle, -DBL_MAX);
      general_info->acq.num_avg = 
         find_double(group_list, ACR_Nr_of_averages, -DBL_MAX);
      general_info->acq.imaging_freq = 
         find_double(group_list, ACR_Imaging_frequency, -DBL_MAX) * 1000000.0;
      (void) strncpy(general_info->acq.imaged_nucl,
         find_string(group_list, ACR_Imaged_nucleus, ""), maxlen);
      (void) strncpy(general_info->acq.comments,
         find_string(group_list, ACR_Acq_comments, ""), maxlen);

      /* Copy the group list */
      general_info->group_list = acr_copy_group_list(group_list);

      /* Set initialized flag */
      general_info->initialized = TRUE;
   }           /* Set up file info */

   /* Update general info and validate file on later passes */
   else {

      /* Check for consistent data type */
      if (((general_info->datatype == NC_BYTE) &&
           (file_info->bits_alloc > 8)) || 
          ((general_info->datatype == NC_SHORT) &&
           (file_info->bits_alloc <= 8))) {
         file_info->valid = FALSE;
         return;
      }

      /* Check row and columns sizes */
      if ((nrows != general_info->nrows) &&
          (ncolumns != general_info->ncolumns))  {
         file_info->valid = FALSE;
         return;
      }

      /* Check study, acq, reconstruction and image type id's */
      if ((general_info->study_id != study_id) ||
          (general_info->acq_id != acq_id) ||
          (general_info->rec_num != rec_num) ||
          (general_info->image_type != image_type)) {
         file_info->valid = FALSE;
         return;
      }

      /* Check indices for range */
      for (imri=0; imri < MRI_NDIMS; imri++) {
         cur_index = file_info->index[imri];
         if ((cur_index < 0) || 
             (cur_index >= general_info->total_size[imri])) {
            file_info->valid = FALSE;
            return;
         }
      }

      /* Look to see if indices have changed */
      for (imri=0; imri < MRI_NDIMS; imri++) {

         /* Get current index */
         cur_index = file_info->index[imri];

         /* If we only have one index and this index is different, then 
            allocate a list */
         if ((general_info->size[imri] == 1) && 
             (cur_index != general_info->first[imri])) {
            general_info->position[imri] = 
               MALLOC(general_info->total_size[imri] * sizeof(int));
            for (index=0; index < general_info->total_size[imri]; index++)
               general_info->position[imri][index] = -1;
            general_info->position[imri][general_info->first[imri]] = 0;
            general_info->position[imri][cur_index] = 0;
            if (cur_index < general_info->first[imri])
               general_info->first[imri] = cur_index;
            general_info->size[imri]++; 
        }

         /* If we have more than one index and this one has not been found
            yet, then set it to true */
         else if ((general_info->size[imri] > 1) &&
                  (general_info->position[imri][cur_index] < 0)) {
            general_info->position[imri][cur_index] = 0;
            if (cur_index < general_info->first[imri])
               general_info->first[imri] = cur_index;
            general_info->size[imri]++;
         }

         /* Update position list */
         if (general_info->position[imri] != NULL) {
            cur_index = 0;
            for (index=0; index < general_info->total_size[imri]; index++) {
               if (general_info->position[imri][index] >= 0) {
                  general_info->position[imri][index] = cur_index;
                  cur_index++;
               }
            }
         }

      }              /* Loop over Mri_Index */

      /* Update display window info */
      if (general_info->window_min > file_info->window_min) 
         general_info->window_min = file_info->window_min;
      if (general_info->window_max < file_info->window_max)
         general_info->window_max = file_info->window_max;

   }              /* Update general info for this file */

   /* Get other slice specific information */
   default_slice_pos = general_info->slice_start + 
      file_info->index[SLICE] * general_info->slice_step;
   file_info->slice_position = 
      find_double(group_list, off_center_elid[general_info->slice_world],
                  default_slice_pos);
   file_info->dyn_begin_time = 
      find_double(group_list, SPI_dynamic_scan_begin_time, 0.0);
   file_info->echo_time = 
      find_double(group_list, ACR_Echo_time, 0.0) / 1000.0;

   /* Update slice position information for general_info. If we have a new
      first slice, then update centre and start information */
   if (file_info->index[SLICE] < general_info->sliceindex_first) {
      general_info->sliceindex_first = file_info->index[SLICE];
      general_info->slicepos_first = file_info->slice_position;

      /* Update slice centre and start info */
      general_info->centre[general_info->slice_world] = 
         general_info->slicepos_first;
      calculate_slice_start(general_info->slice_world,
                            general_info->row_world,
                            general_info->column_world,
                            general_info->nrows,
                            general_info->ncolumns,
                            general_info->centre,
                            general_info->step,
                            general_info->dircos,
                            general_info->start);
   }
   if (file_info->index[SLICE] > general_info->sliceindex_last) {
      general_info->sliceindex_last = file_info->index[SLICE];
      general_info->slicepos_last = file_info->slice_position;
   }
   if (general_info->size[SLICE] > 1) {
      general_info->step[general_info->slice_world] = 
         (general_info->slicepos_last - general_info->slicepos_first) /
            (general_info->size[SLICE] - 1);
   }

   /* If we get to here, then we have a valid file */
   file_info->valid = TRUE;

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_gyro_image
@INPUT      : group_list - input data
@OUTPUT     : image - image data structure (user must free data)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to get an image from a group list
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 25, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void get_gyro_image(Acr_Group group_list, Image_Data *image)
{
   /* Define some constants */
#define PACK_BITS 12
#define PACK_BYTES 3
#define PACK_MASK 0x0F
#define PACK_SHIFT 4

   /* Variables */
   Acr_Element element;
   int nrows, ncolumns;
   int bits_alloc;
   int bits_stored;
   int image_group;
   void *data = NULL;
   long imagepix, ipix;
   struct Acr_Element_Id elid;
   nc_type datatype;
   unsigned char *packed;
   unsigned char pixel[2][2];
   int need_byte_flip;
   unsigned char temp_byte;
   Acr_byte_order byte_order;

   /* Get the image information */
   bits_alloc = find_short(group_list, ACR_Bits_allocated, 0);
   bits_stored = find_short(group_list, ACR_Bits_stored, 0);
   nrows = find_short(group_list, ACR_Rows, 0);
   ncolumns = find_short(group_list, ACR_Columns, 0);
   if (nrows <= 0)
      nrows = find_int(group_list, SPI_Recon_resolution, 0);
   if (ncolumns <= 0)
      ncolumns = find_int(group_list, SPI_Recon_resolution, 0);
   image_group = find_short(group_list, ACR_Image_location, INT_MIN);

   /* Figure out type */
   if (bits_alloc > CHAR_BIT)
      datatype = NC_SHORT;
   else 
      datatype = NC_BYTE;

   /* Set image info */
   image->nrows = nrows;
   image->ncolumns = ncolumns;
   imagepix = nrows * ncolumns;
   image->data = (unsigned short *) MALLOC(imagepix * sizeof(short));
   image->free = TRUE;

   /* Get image pointer */
   elid.group_id = image_group;
   elid.element_id = SPI_IMAGE_ELEMENT;
   element = acr_find_group_element(group_list, &elid);
   if (element == NULL) {
      (void) memset(image->data, 0, imagepix * sizeof(short));
      return;
   }
   data = acr_get_element_data(element);

   /* Convert the data according to type */

   /* Look for byte data */
   if (datatype == NC_BYTE) {
      for (ipix=0; ipix < imagepix; ipix++) {
         image->data[ipix] = *((unsigned char *) data + ipix);
      }
   }
   else {

      /* Get byte order */
      byte_order = acr_get_element_byte_order(element);

      /* Look for unpacked short data */
      if (bits_alloc == nctypelen(datatype) * CHAR_BIT) {
         acr_get_short(byte_order, nrows*ncolumns, data, image->data);
      }

      /* Get packed short data */
      else if ((bits_alloc == PACK_BITS) && (bits_stored <= bits_alloc) &&
               ((imagepix % 2) == 0)) {
         need_byte_flip = acr_need_invert(byte_order);
         packed = (unsigned char *) data;
         for (ipix=0; ipix < imagepix; ipix+=2) {
            pixel[0][0] = packed[0];
            pixel[0][1] = packed[1] & PACK_MASK;
            pixel[1][0] = (packed[1] >> PACK_SHIFT) |
               ((packed[2] & PACK_MASK) << PACK_SHIFT);
            pixel[1][1] = packed[2] >> PACK_SHIFT;
            if (need_byte_flip) {
               temp_byte = pixel[0][0];
               pixel[0][0] = pixel[0][1];
               pixel[0][1] = temp_byte;
               temp_byte = pixel[1][0];
               pixel[1][0] = pixel[1][1];
               pixel[1][1] = temp_byte;
            }
            image->data[ipix]   = *((unsigned short *) pixel[0]);
            image->data[ipix+1] = *((unsigned short *) pixel[1]);
            packed += PACK_BYTES;
         }
      }

      /* Fill with zeros in any other case */
      else {
         (void) memset(image->data, 0, imagepix * sizeof(short));
      }
   }

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_direction_cosines
@INPUT      : angulation_ap - Angle of rotation about ap (Y) axis
              angulation_lr - Angle of rotation about lr (X) axis
              angulation_cc - Angle of rotation about cc (Z) axis
@OUTPUT     : dircos - array of direction cosines
@RETURNS    : (nothing)
@DESCRIPTION: Routine to compute direction cosines from angles
@METHOD     : The rotation matrices are designed to be pre-multiplied by row 
              vectors. The rows of the final rotation matrix are the 
              direction cosines.
@GLOBALS    : 
@CALLS      : 
@CREATED    : October 18, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void get_direction_cosines(double angulation_ap, double angulation_lr,
                                  double angulation_cc, 
                                  double dircos[WORLD_NDIMS][WORLD_NDIMS])
{
   World_Index iloop, jloop, kloop, axis_loop, axis;
   double rotation[WORLD_NDIMS][WORLD_NDIMS];
   double temp[WORLD_NDIMS][WORLD_NDIMS];
   double angle, sinangle, cosangle;
   /* Array giving order of rotations - cc, lr, ap */
   static World_Index rotation_axis[WORLD_NDIMS]={ZCOORD, XCOORD, YCOORD};

   /* Start with identity matrix */
   for (iloop=0; iloop < WORLD_NDIMS; iloop++) {
      for (jloop=0; jloop < WORLD_NDIMS; jloop++) {
         if (iloop == jloop)
            dircos[iloop][jloop] = 1.0;
         else
            dircos[iloop][jloop] = 0.0;
      }
   }

   /* Loop through angles */
   for (axis_loop=0; axis_loop < WORLD_NDIMS; axis_loop++) {

      /* Get axis */
      axis = rotation_axis[axis_loop];

      /* Get angle */
      switch (axis) {
      case XCOORD: angle = angulation_lr; break;
      case YCOORD: angle = angulation_ap; break;
      case ZCOORD: angle = angulation_cc; break;
      }
      angle = angle / 180.0 * M_PI;
      if (angle == 0.0) {
         cosangle = 1.0;
         sinangle = 0.0;
      }
      else {
         cosangle = cos(angle);
         sinangle = sin(angle);
      }

      /* Build up rotation matrix (make identity, then stuff in sin and cos) */
      for (iloop=0; iloop < WORLD_NDIMS; iloop++) {
         for (jloop=0; jloop < WORLD_NDIMS; jloop++) {
            if (iloop == jloop)
               rotation[iloop][jloop] = 1.0;
            else
               rotation[iloop][jloop] = 0.0;
         }
      }
      rotation[(YCOORD+axis)%WORLD_NDIMS][(YCOORD+axis)%WORLD_NDIMS] 
         = cosangle;
      rotation[(YCOORD+axis)%WORLD_NDIMS][(ZCOORD+axis)%WORLD_NDIMS] 
         = sinangle;
      rotation[(ZCOORD+axis)%WORLD_NDIMS][(YCOORD+axis)%WORLD_NDIMS] 
         = -sinangle;
      rotation[(ZCOORD+axis)%WORLD_NDIMS][(ZCOORD+axis)%WORLD_NDIMS] 
         = cosangle;

      /* Multiply this by the previous matrix and then save the result */
      for (iloop=0; iloop < WORLD_NDIMS; iloop++) {
         for (jloop=0; jloop < WORLD_NDIMS; jloop++) {
            temp[iloop][jloop] = 0.0;
            for (kloop=0; kloop < WORLD_NDIMS; kloop++)
               temp[iloop][jloop] += 
                  (dircos[iloop][kloop] * rotation[kloop][jloop]);
         }
      }
      for (iloop=0; iloop < WORLD_NDIMS; iloop++)
         for (jloop=0; jloop < WORLD_NDIMS; jloop++)
            dircos[iloop][jloop] = temp[iloop][jloop];

   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_orientation_info
@INPUT      : orientation         - orientation of acquisition
              dircos              - direction cosines for each world axis
@OUTPUT     : slice_world         - world axis for slices
              row_world           - world axis for rows
              column_world        - world axis for columns
              dircos              - reordered direction cosines
@RETURNS    : (nothing)
@DESCRIPTION: Routine to get the orientation information for a volume,
              taking into account rotation of the axes. We need to
              get the mapping from slice, row or column to world axis, 
              as well as re-ordering the direction cosines so that 
              they correspond to the appropriate axes.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : October 19, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void get_orientation_info(Orientation orientation,
                                 double dircos[WORLD_NDIMS][WORLD_NDIMS],
                                 World_Index *slice_world,
                                 World_Index *row_world,
                                 World_Index *column_world)
{
   World_Index original_to_rotated[WORLD_NDIMS];
   World_Index original_axis;
   static World_Index orientation_axes[NUM_ORIENTATIONS][VOL_NDIMS] = {
      ZCOORD, YCOORD, XCOORD, /* TRANSVERSE */
      XCOORD, ZCOORD, YCOORD, /* SAGITTAL */
      YCOORD, ZCOORD, XCOORD  /* CORONAL */
   };

   /* Get mapping from original world axis to rotated world axis */
   for (original_axis = XCOORD; original_axis < WORLD_NDIMS; original_axis++) {
      original_to_rotated[original_axis] = 
         get_nearest_world_axis(dircos[original_axis]);
   }

   /* Get mapping from volume axis to world axis (volume axes were defined
      for original orientation, so we have to map those axes through the
      rotation) */
   *slice_world =
      original_to_rotated[orientation_axes[orientation][VSLICE]];
   *row_world =
      original_to_rotated[orientation_axes[orientation][VROW]];
   *column_world =
      original_to_rotated[orientation_axes[orientation][VCOLUMN]];

   /* Re-order direction cosines if needed */
   if (XCOORD == original_to_rotated[YCOORD])
      swap_dircos(dircos[XCOORD], dircos[YCOORD]);
   else if (XCOORD == original_to_rotated[ZCOORD])
      swap_dircos(dircos[XCOORD], dircos[ZCOORD]);
   if (YCOORD == original_to_rotated[ZCOORD])
      swap_dircos(dircos[YCOORD], dircos[ZCOORD]);

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : swap_dircos
@INPUT      : dircos1 - first direction cosine
              dircos2 - second direction cosine
@OUTPUT     : dircos1 - first direction cosine
              dircos2 - second direction cosine
@RETURNS    : (nothing)
@DESCRIPTION: Routine to swap direction cosines in place
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : October 19, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void swap_dircos(double dircos1[WORLD_NDIMS],
                        double dircos2[WORLD_NDIMS])
{
   World_Index iaxis;
   double temp;

   for (iaxis = XCOORD; iaxis < WORLD_NDIMS; iaxis++) {
      temp = dircos1[iaxis];
      dircos1[iaxis] = dircos2[iaxis];
      dircos2[iaxis] = temp;
   }

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_nearest_world_axis
@INPUT      : dircos - direction cosines
@OUTPUT     : (none)
@RETURNS    : Index label for nearest axis (XCOORD, YCOORD or ZCOORD).
@DESCRIPTION: Routine to find the nearest axis to a direction cosine.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : October 19, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public World_Index get_nearest_world_axis(double dircos[WORLD_NDIMS])
{
   World_Index nearest_axis, iaxis;

   nearest_axis = XCOORD;
   for (iaxis = YCOORD; iaxis < WORLD_NDIMS; iaxis++) {
      if (fabs(dircos[iaxis]) > fabs(dircos[nearest_axis]))
         nearest_axis = iaxis;
   }

   return nearest_axis;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : calculate_start_from_centre
@INPUT      : slice_world - world index for slices
              row_world - world index for rows
              column_world - world index for columns
              nrows - number of rows in slice
              ncolumns - number of columns in slice
              centre - coordinate of centre of slice
              step - vector of steps (pixel separations)
              dircos - direction cosines for axes
@OUTPUT     : start - calculated start coordinate for slice
@RETURNS    : (nothing)
@DESCRIPTION: Routine to calculate the start coordinate for a slice given
              its centre, step and direction cosines.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 18, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
/* ARGSUSED */
public void calculate_slice_start(World_Index slice_world,
                                  World_Index row_world,
                                  World_Index column_world,
                                  int nrows, int ncolumns,
                                  double centre[WORLD_NDIMS],
                                  double step[WORLD_NDIMS],
                                  double dircos[WORLD_NDIMS][WORLD_NDIMS],
                                  double start[WORLD_NDIMS])
{
   World_Index iworld;
   double offset[WORLD_NDIMS];

   /* Get offsets along each axis */
   offset[slice_world] = 0.0;
   offset[row_world] = (-step[row_world]) * (nrows - 1.0) / 2.0;
   offset[column_world] = (-step[column_world]) * (ncolumns - 1.0) / 2.0;

   /* Transform offsets but don't use direction cosines since these files
      seem to give centres along the rotated axes, not along real axes. */
   for (iworld=0; iworld < WORLD_NDIMS; iworld++) {
      start[iworld] = centre[iworld] + offset[iworld];
   }

}
      
public int find_short(Acr_Group group_list, Acr_Element_Id elid, 
                      int default_value)
{
   return acr_find_short(group_list, elid, default_value);
}

public int find_int(Acr_Group group_list, Acr_Element_Id elid, 
                    int default_value)
{
   return acr_find_int(group_list, elid, default_value);
}

public double find_double(Acr_Group group_list, Acr_Element_Id elid, 
                          double default_value)
{
   return acr_find_double(group_list, elid, default_value);
}

public char *find_string(Acr_Group group_list, Acr_Element_Id elid, 
                         char *default_value)
{
   return acr_find_string(group_list, elid, default_value);
}

