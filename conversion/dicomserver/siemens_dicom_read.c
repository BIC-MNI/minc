/* ----------------------------- MNI Header -----------------------------------
@NAME       : siemens_dicom_read.c
@DESCRIPTION: Code to read siemens dicom files and get info from them.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 28, 1997 (Peter Neelin)
@MODIFIED   : 
 * $Log: siemens_dicom_read.c,v $
 * Revision 6.2  1999-10-29 17:51:58  neelin
 * Fixed Log keyword
 *
 * Revision 6.1  1999/08/05 20:00:34  neelin
 * Get acquisition id from series or study element, depending on the
 * version of the Siemens software.
 *
 * Revision 6.0  1997/09/12  13:24:27  neelin
 * Release of minc version 0.6
 *
 * Revision 5.1  1997/09/10  19:36:13  neelin
 * Small fix to set default direction cosines when they are absent from the
 * dicom data.
 *
 * Revision 5.0  1997/08/21  13:25:26  neelin
 * Release of minc version 0.5
 *
 * Revision 4.1  1997/06/13  12:51:21  neelin
 * Changed definition of time index and acquisition id to match change
 * in Siemens dicom software.
 *
 * Revision 4.0  1997/05/07  20:06:20  neelin
 * Release of minc version 0.4
 *
 * Revision 1.2  1997/03/11  13:10:48  neelin
 * Working version of dicomserver.
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
#include <math.h>

extern int SPI_Vision_version_pre33A;

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
   Mri_Index imri;
   World_Index iworld, jworld;
   Volume_Index ivolume;
   int nrows, ncolumns, spatial_sizes[VOL_NDIMS];
   int study_id, acq_id, rec_num;
   int cur_index;
   int index;
   int number_of_3D_partitions;
   Orientation orientation;
   World_Index volume_to_world[VOL_NDIMS];
   double coordinate[WORLD_NDIMS], dircos[VOL_NDIMS][WORLD_NDIMS];
   double steps[VOL_NDIMS], starts[VOL_NDIMS], slice_index;
   Acr_Element_Id mri_index_list[MRI_NDIMS];
   Acr_Element_Id mri_total_list[MRI_NDIMS];

   /* Array of elements for mri dimensions */
   mri_index_list[SLICE] = SPI_Current_slice_number;
   mri_index_list[ECHO] = ACR_Echo_number;
   mri_index_list[TIME] = (SPI_Vision_version_pre33A ? ACR_Study : ACR_Series);
   mri_index_list[PHASE] = NULL;
   mri_index_list[CHEM_SHIFT] = NULL;
   mri_total_list[SLICE] = SPI_Number_of_slices_nominal;
   mri_total_list[ECHO] = SPI_Number_of_echoes;
   mri_total_list[TIME] = ACR_Acquisitions_in_series;
   mri_total_list[PHASE] = NULL;
   mri_total_list[CHEM_SHIFT] = NULL;

   /* Get image dimensions */
   nrows = acr_find_short(group_list, ACR_Rows, 0);
   ncolumns = acr_find_short(group_list, ACR_Columns, 0);
   spatial_sizes[VROW] = nrows;
   spatial_sizes[VCOLUMN] = ncolumns;
   spatial_sizes[VSLICE] = 1;

   /* Get intensity information */
   get_intensity_info(group_list, file_info);

   /* Check for necessary values not found */
   if ((nrows <= 0) || (ncolumns <= 0) ||
       (file_info->bits_stored <= 0) ||
       (file_info->bits_alloc <= 0)) {
      file_info->valid = FALSE;
      return;
   }

   /* Get study, acq, rec, image type id's */
   get_identification_info(group_list, &study_id, &acq_id, &rec_num, NULL);

   /* Get number of 3D partitions for working out number of slices */
   number_of_3D_partitions = 
      acr_find_int(group_list, SPI_Number_of_3D_raw_partitions_nominal, 1);
   if (number_of_3D_partitions < 1)
      number_of_3D_partitions = 1;

   /* Get image indices */
   for (imri=0; imri < MRI_NDIMS; imri++) {
      if (mri_index_list[imri] != NULL) {
         file_info->index[imri] = 
            acr_find_int(group_list, mri_index_list[imri], 1);
      }
      else {
         file_info->index[imri] = 1;
      }
   }

   /* Get coordinate information */
   get_coordinate_info(group_list, file_info, &orientation, volume_to_world,
                       spatial_sizes, dircos, steps, starts, coordinate);

   /* Replace slice index with slice position in hundredths of millimetres if
      we have more than one partition */
   if (number_of_3D_partitions > 1) {
      slice_index = file_info->coordinate[SLICE] * 100.0;
      if (slice_index >= 0.0)
         slice_index += 0.5;
      else
         slice_index -= 0.5;
      slice_index = (int) slice_index;
      file_info->index[SLICE] = slice_index;
   }

   /* Set up general info on first pass */
   if (!general_info->initialized) {

      /* Get row and columns sizes */
      general_info->nrows = nrows;
      general_info->ncolumns = ncolumns;

      /* Save the study, acquisition, reconstruction and image type 
         identifiers */
      general_info->study_id = study_id;
      general_info->acq_id = acq_id;
      general_info->rec_num = rec_num;

      /* No image type is available */
      general_info->image_type_string[0] = '\0';

      /* Get dimension information */
      for (imri=0; imri < MRI_NDIMS; imri++) {

         /* Gets sizes */
         general_info->size[imri] = 1;
         if (mri_total_list[imri] != NULL) {
            general_info->total_size[imri] = 
               acr_find_int(group_list, mri_total_list[imri], 1);
         }
         else
            general_info->total_size[imri] = 1;
         if (general_info->total_size[imri] < 1)
            general_info->total_size[imri] = 1;

         /* Check for 3D partitions for slice dimensions */
         if (imri == SLICE) {
            general_info->total_size[imri] *= number_of_3D_partitions;
         }

         /* Set initial values */
         general_info->default_index[imri] = file_info->index[imri];
         general_info->image_index[imri] = -1;

         /* Allocate space for index and coordinate arrays if total_size > 1.
            Set the first values. */
         if (general_info->total_size[imri] > 1) {
            general_info->indices[imri] = 
               MALLOC(general_info->total_size[imri] * sizeof(int));
            general_info->coordinates[imri] = 
               MALLOC(general_info->total_size[imri] * sizeof(double));
            for (index=0; index < general_info->total_size[imri]; index++) {
               general_info->indices[imri][index] = -1;
               general_info->coordinates[imri][index] = 0;
            }
            general_info->search_start[imri] = 0;
            general_info->indices[imri][0] = file_info->index[imri];
            general_info->coordinates[imri][0] = file_info->coordinate[imri];
         }
      }          /* Loop over dimensions */

      /* Get spatial coordinate information */
      general_info->slice_world = volume_to_world[VSLICE];
      general_info->row_world = volume_to_world[VROW];
      general_info->column_world = volume_to_world[VCOLUMN];
      for (ivolume=0; ivolume < VOL_NDIMS; ivolume++) {
         iworld = volume_to_world[ivolume];
         general_info->step[iworld] = steps[ivolume];
         general_info->start[iworld] = starts[ivolume];
         for (jworld=0; jworld < WORLD_NDIMS; jworld++) {
            general_info->dircos[volume_to_world[ivolume]][jworld]
               = dircos[ivolume][jworld];
         }
      }

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

      /* Get the rest of the header information */
      get_general_header_info(group_list, general_info);

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

      /* Check study and acquisition id's */
      if ((general_info->study_id != study_id) ||
          (general_info->acq_id != acq_id)) {
         file_info->valid = FALSE;
         return;
      }

      /* Look to see if indices have changed */
      for (imri=0; imri < MRI_NDIMS; imri++) {

         /* Get current index */
         cur_index = file_info->index[imri];

         /* Check whether this index is in the list */
         if (general_info->size[imri] == 1) {
            index = 
               ((cur_index == general_info->default_index[imri]) ? 0 : -1);
         }
         else {
            index = search_list(cur_index, general_info->indices[imri],
                                general_info->size[imri],
                                general_info->search_start[imri]);
         }

         /* If it is not, then add it */
         if (index < 0) {

            /* Check whether we can add a new index */
            if (general_info->size[imri] >= general_info->total_size[imri]) {
               file_info->valid = FALSE;
               return;
            }

            /* Add the index and coordinate to the lists */
            index = general_info->size[imri];
            general_info->search_start[imri] = index;
            general_info->indices[imri][index] = cur_index;
            general_info->coordinates[imri][index] = 
               file_info->coordinate[imri];
            general_info->size[imri]++;

         }


      }              /* Loop over Mri_Index */

      /* Update display window info */
      if (general_info->window_min > file_info->window_min) 
         general_info->window_min = file_info->window_min;
      if (general_info->window_max < file_info->window_max)
         general_info->window_max = file_info->window_max;

   }              /* Update general info for this file */

   /* If we get to here, then we have a valid file */
   file_info->valid = TRUE;

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_identification_info
@INPUT      : group_list - input data
@OUTPUT     : study_id
              acq_id
              rec_num
              image_type
@RETURNS    : (nothing)
@DESCRIPTION: Routine to get image identification information.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 28, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void get_identification_info(Acr_Group group_list, 
                                    int *study_id, int *acq_id, 
                                    int *rec_num, int *image_type)
{
   int number_of_frames;

   if (study_id != NULL) {
      *study_id = acr_find_int(group_list, ACR_Study_date, 0);
   }
   if (acq_id != NULL) {
      *acq_id = acr_find_int(group_list, 
                             (SPI_Vision_version_pre33A ? 
                              ACR_Study : ACR_Series), 0);
      number_of_frames = 
         acr_find_int(group_list, ACR_Acquisitions_in_series, 1);
      if ((number_of_frames > 1) || (*acq_id == 0)) {
         *acq_id = acr_find_int(group_list, ACR_Study_time, 0);
      }
   }
   if (rec_num != NULL)
      *rec_num = 0;
   if (image_type != NULL)
      *image_type = 0;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_intensity_info
@INPUT      : group_list - input data
@OUTPUT     : file_info - file-specific info
@RETURNS    : (nothing)
@DESCRIPTION: Routine to get intensity information from a group list
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 28, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void get_intensity_info(Acr_Group group_list, File_Info *file_info)
{
   double window_centre, window_width;

   /* Get pixel storage information */
   file_info->bits_alloc = acr_find_short(group_list, ACR_Bits_allocated, 0);
   file_info->bits_stored = acr_find_short(group_list, ACR_Bits_stored, 0);

   /* Get pixel value information */
   file_info->pixel_min = 
      acr_find_short(group_list, ACR_Smallest_pixel_value, 0);
   file_info->pixel_max = acr_find_short(group_list, ACR_Largest_pixel_value, 
                                         (1 << file_info->bits_stored) - 1);
   file_info->slice_min = file_info->pixel_min;
   file_info->slice_max = file_info->pixel_max;

   /* Get window min and max */
   window_centre = (file_info->slice_max + file_info->slice_min) / 2.0;
   window_width  = file_info->slice_max - file_info->slice_min;
   window_centre = 
      acr_find_double(group_list, ACR_Window_centre, window_centre);
   window_width = 
      acr_find_double(group_list, ACR_Window_width, window_width);
   window_width /= 2.0;
   file_info->window_min = window_centre - window_width;
   file_info->window_max = window_centre + window_width; 

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_coordinate_info
@INPUT      : group_list - input data
              sizes - size of each spatial dimension
@OUTPUT     : file_info - file-specific info
              volume_to_world - volume index to world coordinate index mapping
              dircos - direction cosines for spatial dimensions
              steps - step sizes for spatial dimensions
              starts - start positions for spatial dimensions (for a slice)
              coordinate - coordinate of centre of slice
@RETURNS    : (nothing)
@DESCRIPTION: Routine to get coordinate information for a slice from 
              a group list
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 28, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void get_coordinate_info(Acr_Group group_list, File_Info *file_info,
                                Orientation *orientation,
                                World_Index volume_to_world[VOL_NDIMS],
                                int sizes[VOL_NDIMS],
                                double dircos[VOL_NDIMS][WORLD_NDIMS],
                                double steps[VOL_NDIMS],
                                double starts[VOL_NDIMS],
                                double coordinate[WORLD_NDIMS])
{
   Volume_Index ivolume;
   World_Index iworld;
   Acr_Element_Id dircos_elid[VOL_NDIMS];
   Acr_Element element;
   int found_dircos[VOL_NDIMS], found_coordinate;
   double frame_time, start_time;
   double magnitude, largest, centre;
   double darray[2];
   static Orientation orientation_list[WORLD_NDIMS] = 
      {SAGITTAL, CORONAL, TRANSVERSE};

   /* Set direction cosine element ids. Note that the reversal of rows and
      columns is intentional - their idea of the meaning of theses labels is
      different from ours. (Their row vector points along the row and not
      along the row dimension.) */
   dircos_elid[VSLICE] = SPI_Image_normal;
   dircos_elid[VROW] = SPI_Image_column;
   dircos_elid[VCOLUMN] = SPI_Image_row;

   /* Get direction cosines */
   for (ivolume=0; ivolume < VOL_NDIMS; ivolume++) {
      found_dircos[ivolume] = FALSE;
      element = acr_find_group_element(group_list, dircos_elid[ivolume]);
      if (element == NULL) continue;
      if (acr_get_element_numeric_array(element, WORLD_NDIMS, dircos[ivolume])
          != WORLD_NDIMS) continue;
      convert_coordinate(dircos[ivolume]);
      found_dircos[ivolume] = TRUE;
   }

   /* Normalize the direction cosines */
   for (ivolume=0; ivolume < VOL_NDIMS; ivolume++) {
      magnitude = 0.0;
      for (iworld=0; iworld < WORLD_NDIMS; iworld++) {
         magnitude += dircos[ivolume][iworld] * dircos[ivolume][iworld];
      }
      if (magnitude <= 0) {
         found_dircos[ivolume] = FALSE;
         continue;
      }
      magnitude = sqrt(magnitude);
      for (iworld=0; iworld < WORLD_NDIMS; iworld++) {
         dircos[ivolume][iworld] /= magnitude;
      }
   }

   /* If we don't find direction cosines, then assume transverse volume */
   if (!found_dircos[VSLICE] || !found_dircos[VROW] || 
       !found_dircos[VCOLUMN]) {
      for (ivolume=0; ivolume < VOL_NDIMS; ivolume++) {
         for (iworld=0; iworld < WORLD_NDIMS; iworld++) {
            dircos[ivolume][iworld] = 
               ((ivolume == (WORLD_NDIMS-iworld-1)) ? -1.0 : 0.0);
         }
      }
   }

   /* Figure out volume index to world index mapping and sign of direction
      cosines */
   for (ivolume=0; ivolume < VOL_NDIMS; ivolume++) {
      largest = -1.0;
      for (iworld=0; iworld < WORLD_NDIMS; iworld++) {
         magnitude = dircos[ivolume][iworld];
         if (magnitude < 0.0) magnitude = -magnitude;
         if (magnitude > largest) {
            largest = magnitude;
            volume_to_world[ivolume] = iworld;
         }
      }
   }

   /* Get orientation */
   *orientation = orientation_list[volume_to_world[VSLICE]];

   /* Get step information. */
   for (ivolume=0; ivolume < sizeof(darray)/sizeof(darray[0]); ivolume++)
      darray[ivolume] = -DBL_MAX;
   element = acr_find_group_element(group_list, ACR_Pixel_size);
   if (element != NULL)
      (void) acr_get_element_numeric_array(element, 
         sizeof(darray)/sizeof(darray[0]), darray);
   if (darray[0] == -DBL_MAX) darray[0] = 1.0;
   if (darray[1] == -DBL_MAX) darray[1] = darray[0];
   steps[VCOLUMN] = darray[0];
   steps[VROW] = darray[0];
   steps[VSLICE] = acr_find_double(group_list, ACR_Slice_thickness, 1.0);

   /* Make sure that direction cosines point the right way (dot product
      of direction cosine and axis is positive) and that step has proper
      sign */
   for (ivolume = 0; ivolume < VOL_NDIMS; ivolume++) {
      iworld = volume_to_world[ivolume];
      if (dircos[ivolume][iworld] < 0.0) {
         steps[ivolume] *= -1.0;
         for (iworld = 0; iworld < WORLD_NDIMS; iworld++) {
            dircos[ivolume][iworld] *= -1.0;
         }
      }
   }

   /* Find 3D coordinate of slice */
   element = acr_find_group_element(group_list, SPI_Image_position);
   if ((element == NULL) ||
       (acr_get_element_numeric_array(element, WORLD_NDIMS, coordinate) 
        != WORLD_NDIMS)) {
      found_coordinate = FALSE;
      for (iworld=0; iworld < WORLD_NDIMS; iworld++)
         coordinate[iworld] = 0.0;
   }
   else
      found_coordinate = TRUE;
   convert_coordinate(coordinate);

   /* Work out start positions */
   for (ivolume=0; ivolume < VOL_NDIMS; ivolume++) {

      if (found_coordinate && found_dircos[VSLICE] && found_dircos[VROW] &&
          found_dircos[VCOLUMN]) {
         centre = 
            coordinate[XCOORD] * dircos[ivolume][XCOORD] +
            coordinate[YCOORD] * dircos[ivolume][YCOORD] +
            coordinate[ZCOORD] * dircos[ivolume][ZCOORD];
      }
      else {
         centre = 0.0;
      }
      starts[ivolume] = centre -
         (steps[ivolume] * (sizes[ivolume] - 1.0) / 2.0);
   }

   /* Find position along each dimension */
   file_info->coordinate[SLICE] = starts[VSLICE];
   file_info->coordinate[ECHO] = 
      acr_find_double(group_list, ACR_Echo_time, 0.0) / (double) MS_PER_SECOND;
   start_time = acr_find_double(group_list, ACR_Study_time, 0.0);
   frame_time = acr_find_double(group_list, ACR_Acquisition_time, start_time);
   start_time = convert_time_to_seconds(start_time);
   frame_time = convert_time_to_seconds(frame_time) - start_time;
   if (frame_time < 0.0) frame_time += (double) SECONDS_PER_DAY;
   file_info->coordinate[TIME] = frame_time;
   file_info->coordinate[PHASE] = 0.0;
   file_info->coordinate[CHEM_SHIFT] = 0.0;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_coordinate
@INPUT      : coordinate
@OUTPUT     : coordinate
@RETURNS    : (nothing)
@DESCRIPTION: Routine to convert a coordinate to the correct orientation
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 28, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void convert_coordinate(double coordinate[WORLD_NDIMS])
{
   coordinate[XCOORD] = -coordinate[XCOORD];
   coordinate[ZCOORD] = -coordinate[ZCOORD];
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_general_header_info
@INPUT      : group_list - input data
@OUTPUT     : general_info - general information about files
@RETURNS    : (nothing)
@DESCRIPTION: Routine to extract general header information from a group list
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 28, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void get_general_header_info(Acr_Group group_list, 
                                    General_Info *general_info)
{
   int maxlen, length;
   char *string, *ptr;

   /* Maximum length for strings */
   maxlen = sizeof(Cstring) - 1;

   /* Get intensity units */
   (void) strncpy(general_info->units, "", maxlen);

   /* Get patient info */
   (void) strncpy(general_info->patient.name,
                  acr_find_string(group_list, ACR_Patient_name, ""), maxlen);
   (void) strncpy(general_info->patient.identification,
      acr_find_string(group_list, ACR_Patient_identification, ""), maxlen);
   (void) strncpy(general_info->patient.birth_date,
      acr_find_string(group_list, ACR_Patient_birth_date, ""), maxlen);
   string = acr_find_string(group_list, ACR_Patient_sex, "");
   if (*string == 'M') 
      (void) strncpy(general_info->patient.sex, MI_MALE, maxlen);
   else if (*string == 'F') 
      (void) strncpy(general_info->patient.sex, MI_FEMALE, maxlen);
   else if (*string == 'O') 
      (void) strncpy(general_info->patient.sex, MI_OTHER, maxlen);
   else 
      (void) strncpy(general_info->patient.sex, "", maxlen);
   general_info->patient.weight = 
      acr_find_double(group_list, ACR_Patient_weight, -DBL_MAX);

   /* Get study info */
   (void) strncpy(general_info->study.start_time, 
      acr_find_string(group_list, ACR_Study_date, ""), maxlen - 1);
   length = strlen(general_info->study.start_time);
   general_info->study.start_time[length] = ' ';
   length++;
   (void) strncpy(&general_info->study.start_time[length], 
      acr_find_string(group_list, ACR_Study_time, ""), maxlen - length);
   string = acr_find_string(group_list, ACR_Modality, "");
   if (strcmp(string, ACR_MODALITY_MR) == 0)
      (void) strncpy(general_info->study.modality, MI_MRI, maxlen);
   (void) strncpy(general_info->study.manufacturer, 
      acr_find_string(group_list, ACR_Manufacturer, ""), maxlen);
   (void) strncpy(general_info->study.model, 
      acr_find_string(group_list, ACR_Manufacturer_model, ""), maxlen);
   (void) strncpy(general_info->study.institution, 
      acr_find_string(group_list, ACR_Institution_id, ""), maxlen);
   (void) strncpy(general_info->study.station_id, 
      acr_find_string(group_list, ACR_Station_id, ""), maxlen);
   (void) strncpy(general_info->study.ref_physician, 
      acr_find_string(group_list, ACR_Referring_physician, ""), maxlen);
   (void) strncpy(general_info->study.procedure, 
      acr_find_string(group_list, ACR_Procedure_description, ""), maxlen);
   (void) sprintf(general_info->study.study_id, "%d", general_info->study_id);
   (void) sprintf(general_info->study.acquisition_id, "%d_%d",
      acr_find_int(group_list, ACR_Series, 0), general_info->acq_id);

   /* Get acquisition information */
   string = acr_find_string(group_list, SPI_Sequence_file_name, "");
   ptr = string + strlen(string) - 1;
   while ((ptr > string) && (*ptr != '/')) {ptr--;}
   if ((ptr >= string) && (*ptr == '/')) string = ptr + 1;
   (void) strncpy(general_info->acq.scan_seq, string, maxlen);
   ptr = general_info->acq.scan_seq;
   while (*ptr != '\0') {
      if (*ptr == '.') *ptr = '\0';
      ptr++;
   }
   general_info->acq.rep_time = 
      acr_find_double(group_list, ACR_Repetition_time, -DBL_MAX);
   if (general_info->acq.rep_time != -DBL_MAX)
      general_info->acq.rep_time /= 1000.0;
   general_info->acq.echo_time = 
      acr_find_double(group_list, ACR_Echo_time, -DBL_MAX);
   if (general_info->acq.echo_time != -DBL_MAX)
      general_info->acq.echo_time /= 1000.0;
   general_info->acq.inv_time = 
      acr_find_double(group_list, ACR_Inversion_time, -DBL_MAX);
   if (general_info->acq.inv_time != -DBL_MAX)
      general_info->acq.inv_time /= 1000.0;
   general_info->acq.flip_angle = 
      acr_find_double(group_list, ACR_Flip_angle, -DBL_MAX);
   general_info->acq.num_avg = 
      acr_find_double(group_list, ACR_Nr_of_averages, -DBL_MAX);
   general_info->acq.imaging_freq = 
      acr_find_double(group_list, ACR_Imaging_frequency, -DBL_MAX);
   if (general_info->acq.imaging_freq != -DBL_MAX)
      general_info->acq.imaging_freq *= 1e6;
   (void) strncpy(general_info->acq.imaged_nucl,
      acr_find_string(group_list, ACR_Imaged_nucleus, ""), maxlen);
   (void) strncpy(general_info->acq.comments, "", maxlen);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_time_to_seconds
@INPUT      : dicom_time
@OUTPUT     : (none)
@RETURNS    : real time in seconds from beginning of day
@DESCRIPTION: Routine to convert dicom seconds (decimal hhmmss.xxxxx) to 
              real seconds since the start of day.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 28, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public double convert_time_to_seconds(double dicom_time)
{
   /* Constants */
#define DICOM_SECONDS_PER_HOUR 10000
#define DICOM_SECONDS_PER_MINUTE 100

   /* Variables */
   double real_time, hours, minutes, seconds;

   /* Get the components of the time */
   hours = (int) (dicom_time / (double) DICOM_SECONDS_PER_HOUR);
   dicom_time -= hours * DICOM_SECONDS_PER_HOUR;
   minutes = (int) (dicom_time / (double) DICOM_SECONDS_PER_MINUTE);
   dicom_time -= minutes * DICOM_SECONDS_PER_MINUTE;
   seconds = dicom_time;

   /* Work out the number of seconds */
   real_time = hours * SECONDS_PER_HOUR + minutes * SECONDS_PER_MINUTE
      + seconds;

   return real_time;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_siemens_dicom_image
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
public void get_siemens_dicom_image(Acr_Group group_list, Image_Data *image)
{

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

   /* Get the image information */
   bits_alloc = acr_find_short(group_list, ACR_Bits_allocated, 0);
   bits_stored = acr_find_short(group_list, ACR_Bits_stored, bits_alloc);
   nrows = acr_find_short(group_list, ACR_Rows, 0);
   ncolumns = acr_find_short(group_list, ACR_Columns, 0);
   image_group = acr_find_short(group_list, ACR_Image_location, 
                                ACR_ACTUAL_IMAGE_GID);

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

      /* Look for unpacked short data */
      if (bits_alloc == nctypelen(datatype) * CHAR_BIT) {
         acr_get_short(acr_get_element_byte_order(element), 
                       nrows*ncolumns, data, image->data);
      }

      /* Fill with zeros in any other case */
      else {
         (void) memset(image->data, 0, imagepix * sizeof(short));
      }
   }

   return;
}

