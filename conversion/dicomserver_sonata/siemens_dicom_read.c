/* ----------------------------- MNI Header -----------------------------------
@NAME       : siemens_dicom_read.c
@DESCRIPTION: Code to read siemens dicom files and get info from them.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 28, 1997 (Peter Neelin)
@MODIFIED   : 
 * $Log: siemens_dicom_read.c,v $
 * Revision 1.1  2003-08-15 19:52:55  leili
 * Initial revision
 *
 * Revision 1.12  2002/05/01 21:29:34  rhoge
 * removed MrProt from minc header - encountered files with large strings,
 * causing seg faults
 *
 * Revision 1.11  2002/04/26 03:27:03  rhoge
 * fixed MrProt problem - replaced fixed lenght char array with malloc
 *
 * Revision 1.10  2002/04/08 17:26:34  rhoge
 * added additional sequence info to minc header
 *
 * Revision 1.9  2002/03/27 18:57:50  rhoge
 * added diffusion b value
 *
 * Revision 1.8  2002/03/19 13:13:56  rhoge
 * initial working mosaic support - I think time is scrambled though.
 *
 * Revision 1.7  2001/12/31 18:27:21  rhoge
 * modifications for dicomreader processing of Numaris 4 dicom files - at
 * this point code compiles without warning, but does not deal with
 * mosaiced files.  Also will probably not work at this time for Numaris
 * 3 .ima files.  dicomserver may also not be functional...
 *
 * Revision 1.6  2000/12/14 21:33:13  rhoge
 * code modifications to restore measurement loop support that was broken
 * by changes to support acqusition loop scanning
 *
 * Revision 1.5  2000/12/13 13:25:36  rhoge
 * removed dummy printf from convert_time_to_seconds - turns out that
 * buggy behaviour was an optimization problem
 *
 * Revision 1.4  2000/12/12 19:27:52  rhoge
 * changed atof(acr_find_string) back to acr_find_double after realizing
 * that these functions assume string representation
 *
 * Revision 1.3  2000/12/12 14:43:22  rhoge
 * fixed syntax error (dangling comment symbol) from removal of debug
 * printfs - compiles now
 *
 * Revision 1.2  2000/12/11 20:01:44  rhoge
 * fixed code for frame time computation - ACR vals are strings.  Also
 * inserted dummy fprintf statement in convert_time_to_seconds as this
 * seems to salvage Linux problems
 *
 * Revision 1.1.1.1  2000/11/30 02:13:15  rhoge
 * imported sources to CVS repository on amoeba
 * -now always use ACR_Series for run number (seemed to be
 *  problems with test introduced in 6.1)
 * -added code to detect use of acquisition loop and also to handle
 *  `corrected' siemens acq loop scans
 * -changed code to use registration time instead of scan time for 
 *  session id
 * -got rid of extraneous acquisition id digit
 * -added technical information about data acquisition
 *
 * Revision 6.2  1999/10/29 17:51:58  neelin
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
File_Type file_type;

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
@MODIFIED   : Modified Feb. 2000 by Rick Hoge to handle Acquisition loop mode
            : if assume_acq_loop is FALSE the other added variables don't 
            : matter 
---------------------------------------------------------------------------- */
public void get_file_info(Acr_Group group_list, File_Info *file_info,
                          General_Info *general_info)
{
   Mri_Index imri;
   World_Index iworld, jworld;
   Volume_Index ivolume;
   int nrows, ncolumns, spatial_sizes[VOL_NDIMS];
   double study_id;
   int acq_id, rec_num;
   int cur_index;
   int index;
   int number_of_3D_partitions;
   Orientation orientation;
   World_Index volume_to_world[VOL_NDIMS];
   double coordinate[WORLD_NDIMS], dircos[VOL_NDIMS][WORLD_NDIMS];
   double steps[VOL_NDIMS], starts[VOL_NDIMS], slice_index;
   Acr_Element_Id mri_index_list[MRI_NDIMS];
   Acr_Element_Id mri_total_list[MRI_NDIMS];

   // Array of elements for mri dimensions
   mri_index_list[SLICE] = SPI_Current_slice_number;
   mri_index_list[ECHO] = ACR_Echo_number;
   // dicom time element may be different on old systems
   mri_index_list[TIME] = ACR_Acquisition;
   mri_index_list[PHASE] = NULL;
   mri_index_list[CHEM_SHIFT] = NULL;
   mri_total_list[SLICE] = SPI_Number_of_slices_nominal;
   mri_total_list[ECHO] = SPI_Number_of_echoes;
   mri_total_list[TIME] = ACR_Acquisitions_in_series;
   mri_total_list[PHASE] = NULL;
   mri_total_list[CHEM_SHIFT] = NULL;

   // Get image dimensions
   nrows = acr_find_short(group_list, ACR_Rows, 0);
   ncolumns = acr_find_short(group_list, ACR_Columns, 0);
   spatial_sizes[VROW] = nrows;
   spatial_sizes[VCOLUMN] = ncolumns;
   spatial_sizes[VSLICE] = 1;

   // Get intensity information
   get_intensity_info(group_list, file_info);

   // Check for necessary values not found
   if ((nrows <= 0) || (ncolumns <= 0) ||
       (file_info->bits_stored <= 0) ||
       (file_info->bits_alloc <= 0)) {
      file_info->valid = FALSE;
      return;
   }

   // Get study, acq, rec, image type id's
   get_identification_info(group_list, &study_id, &acq_id, &rec_num, NULL);

   // Get number of 3D partitions for working out number of slices
   number_of_3D_partitions = 
      acr_find_int(group_list, SPI_Number_of_3D_raw_partitions_nominal, 1);
   if (number_of_3D_partitions < 1)
      number_of_3D_partitions = 1;

   // Get indices for image in current file
   for (imri=0; imri < MRI_NDIMS; imri++) {
     
     if (mri_index_list[imri] != NULL) {
       file_info->index[imri] = 
	 /* note that for TIME this will use ACR_Acqusition,
	    which does not work for measurement loop scans */
	 acr_find_int(group_list, mri_index_list[imri], 1);
     }
     else {
       file_info->index[imri] = 1;
     }
   }

   // Get coordinate information
   get_coordinate_info(group_list, file_info, &orientation, volume_to_world,
                       spatial_sizes, dircos, steps, starts, coordinate);

   // Replace slice index with slice position in hundredths of millimetres if
   //   we have more than one partition
   if (number_of_3D_partitions > 1 | 1) { // ALWAYS USE POSITION (rhoge)
      slice_index = file_info->coordinate[SLICE] * 100.0;
      if (slice_index >= 0.0) {
         slice_index += 0.5;
      } else {
         slice_index -= 0.5;
      }
      slice_index = (int) slice_index;
      file_info->index[SLICE] = slice_index;
   }

   // Set up general info on first pass
   if (!general_info->initialized) {

      // Get row and columns sizes
      general_info->nrows = nrows;
      general_info->ncolumns = ncolumns;

      // Save the study, acquisition, reconstruction and image type 
      //   identifiers
      general_info->study_id = study_id;
      general_info->acq_id = acq_id;
      general_info->rec_num = rec_num;

      // No image type is available 
      // (rhoge:  no longer true?)
      general_info->image_type_string[0] = '\0';

      /* Get dimension information */
      for (imri=0; imri < MRI_NDIMS; imri++) {

	/* Gets sizes */
	general_info->size[imri] = 1;
	if (mri_total_list[imri] != NULL) {
	  general_info->total_size[imri] = 
	    acr_find_int(group_list, mri_total_list[imri], 1);
	} else {
	  general_info->total_size[imri] = 1;
	}

	if (general_info->total_size[imri] < 1) {
	  general_info->total_size[imri] = 1;
	}

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
   else { //--

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

     // note that number of slices will be wrong here for mosaics
     // we add some other mosaic-relevant fields...

     general_info->num_mosaic_rows =
       acr_find_int(group_list,EXT_Mosaic_rows,1);
     general_info->num_mosaic_cols =
       acr_find_int(group_list,EXT_Mosaic_columns,1);
     general_info->num_slices_in_file = 
       acr_find_int(group_list,EXT_Slices_in_file,1);
     general_info->sub_image_rows =
       acr_find_short(group_list,EXT_Sub_image_rows,
		      acr_find_short(group_list, ACR_Rows, 0));
     general_info->sub_image_columns =
       acr_find_short(group_list,EXT_Sub_image_columns,
		      acr_find_short(group_list, ACR_Rows, 0));

     // Update display window info
     if (general_info->window_min > file_info->window_min) 
       general_info->window_min = file_info->window_min;
     if (general_info->window_max < file_info->window_max)
       general_info->window_max = file_info->window_max;
     
   }  // Update general info for this file

   // If we get to here, then we have a valid file
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
                                    double *study_id, int *acq_id, 
                                    int *rec_num, int *image_type)
{
   int number_of_frames;
   int number_of_averages;

   if (study_id != NULL) {
     // generate a study ID number from date & time:  yyyymmdd.hhmmss
     // (should be unique enough for our application)
     *study_id = (((float)acr_find_int(group_list, ACR_Study_date, 0)) +
       ((float)acr_find_int(group_list, ACR_Study_time, 0))/1e6);
   }
   if (acq_id != NULL) {

     *acq_id = acr_find_int(group_list, ACR_Series, 0);

      number_of_frames = 
         acr_find_int(group_list, ACR_Acquisitions_in_series, 1);

      number_of_averages = 
         acr_find_int(group_list, ACR_Nr_of_averages, 1);

      /* Determine if measurement loop was used (rhoge) -

	 if so, we replace the different series numbers with
	 ACR_Study_time, which is the same for all frames in a run.
	 This will aid in grouping the files later on.  

	 Criteria used for identification of meast loop:

	 1) more than one dynamic scan

	 2) number of dynamic scans NOT equal to nominal number of signal
	 averages (if they're equal, we assume acquisition loop scan)

	 WARNING:  it is possible that someone might use the
	 measurement loop do serial scans which each have multiple signal
	 averages.  If NSA = the number of measts. in this case, then
	 the scan will not be recognized as a Meast loop scan and the
	 different frames will be placed in different series.  To fix
	 this, we'd really need to look at the series numbers of
	 future and past files.  It's also unlikely to happen but I'm
	 sure it will... 

         This also means we should NOT correct the number of signal
	 averages on the sending side */


      /*      if ((number_of_frames > 1) || (*acq_id == 0)) { (orig test) */

      if ( (file_type == N3DCM) &&
	   (number_of_frames > 1) &&
	   (number_of_frames != number_of_averages)) { 

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

#ifdef USE_DICOM_PIXEL_MIN_MAX

   // Get pixel value information
   // I think this might wrongly assume that the ACR values min/max
   // apply to the whole volume - it actually appears that they apply
   // to the current slice.

   file_info->pixel_min = 
     acr_find_short(group_list, ACR_Smallest_pixel_value, 0);

   file_info->pixel_max = acr_find_short(group_list,ACR_Largest_pixel_value, 
                                         (1 << file_info->bits_stored) - 1);

#else

   // for now, use bits_stored to determine dynamic range
   // DICOM info on largest pixel applies to first slice, 
   // not whole volume - this caused problems (roundoff?)
   // in Siemens Numaris 4 scans

   file_info->pixel_min = 0;
   file_info->pixel_max = (1 << file_info->bits_stored) - 1;

#endif

   file_info->slice_min = file_info->pixel_min;
   file_info->slice_max = file_info->pixel_max;

   /* Get window min and max */
   window_centre = (file_info->slice_max + file_info->slice_min) / 2.0;
   window_width  = file_info->slice_max - file_info->slice_min;
   window_centre = 
      acr_find_double(group_list, ACR_Window_centre, window_centre);
   window_width = 
      acr_find_double(group_list, ACR_Window_width, window_width);
   file_info->window_min = window_centre - window_width / 2.0;
   file_info->window_max = window_centre + window_width / 2.0; 

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

   // row/column unit vectors in public dicom element
   double RowColVec[6];

   if (file_type == N3DCM || file_type == IMA) {

     /* Set direction cosine element ids. Note that the reversal of rows and
	columns is intentional - their idea of the meaning of theses labels is
	different from ours. (Their row vector points along the row and not
	along the row dimension.) */
     dircos_elid[VSLICE] = SPI_Image_normal;
     dircos_elid[VROW] = SPI_Image_column;
     dircos_elid[VCOLUMN] = SPI_Image_row;
     
     // Get direction cosines
     for (ivolume=0; ivolume < VOL_NDIMS; ivolume++) {
       found_dircos[ivolume] = FALSE;
       element = acr_find_group_element(group_list, dircos_elid[ivolume]);
       if (element == NULL) continue;
       if (acr_get_element_numeric_array(element, WORLD_NDIMS, dircos[ivolume])
	   != WORLD_NDIMS) continue;
       // negate the X and Z coordinates
       convert_numa3_coordinate(dircos[ivolume]);
       found_dircos[ivolume] = TRUE;
     }
   } else {
     // in normal dicom file, you just have row and column unit vectors
     
     // read in row/col vectors:
     element = acr_find_group_element(group_list,
				      ACR_Image_orientation_patient);
     
     acr_get_element_numeric_array(element, 6, RowColVec);

     memcpy(dircos[VCOLUMN],RowColVec,sizeof(RowColVec[0])*3);
     memcpy(dircos[VROW],&RowColVec[3],sizeof(RowColVec[0])*3);
     found_dircos[VCOLUMN] = TRUE;
     found_dircos[VROW] = TRUE;

     convert_dicom_coordinate(dircos[VROW]);
     convert_dicom_coordinate(dircos[VCOLUMN]);

     // slice direction unit vector is cross product of row, col vectors:
     dircos[VSLICE][0] = 
       dircos[VCOLUMN][1] * dircos[VROW][2] -
       dircos[VCOLUMN][2] * dircos[VROW][1];

     dircos[VSLICE][1] = 
       dircos[VCOLUMN][2] * dircos[VROW][0] -
       dircos[VCOLUMN][0] * dircos[VROW][2];

     dircos[VSLICE][2] = 
       dircos[VCOLUMN][0] * dircos[VROW][1] -
       dircos[VCOLUMN][1] * dircos[VROW][0];
     found_dircos[VSLICE] = TRUE;

     // slice does not need to be converted because 
     // it was computed as the cross product of two
     // corrected vectors
     // convert_dicom_coordinate(dircos[VSLICE]);

   }

   // Normalize the direction cosines
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

   // If we don't find direction cosines, then assume transverse volume
   if (!found_dircos[VSLICE] || !found_dircos[VROW] || 
       !found_dircos[VCOLUMN]) {
      for (ivolume=0; ivolume < VOL_NDIMS; ivolume++) {
         for (iworld=0; iworld < WORLD_NDIMS; iworld++) {
            dircos[ivolume][iworld] = 
               ((ivolume == (WORLD_NDIMS-iworld-1)) ? -1.0 : 0.0);
         }
      }
   }

   // Figure out volume index to world index mapping and sign of direction
   // cosines - the code below figures out the primary direction in x,y,z
   // of each volume coordinate (row,col,slice)
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

   // Get orientation (depends on primary direction of slice normal)
   *orientation = orientation_list[volume_to_world[VSLICE]];

   // Get step information
   for (ivolume=0; ivolume < sizeof(darray)/sizeof(darray[0]); ivolume++)
      darray[ivolume] = -DBL_MAX;
   // note ACR_Pixel_size should now be called Pixel_spacing
   element = acr_find_group_element(group_list, ACR_Pixel_size);
   if (element != NULL)
      (void) acr_get_element_numeric_array(element, 
         sizeof(darray)/sizeof(darray[0]), darray);
   if (darray[0] == -DBL_MAX) darray[0] = 1.0;
   if (darray[1] == -DBL_MAX) darray[1] = darray[0];
   steps[VCOLUMN] = darray[0];
   steps[VROW] = darray[1];  // anisotropic resolution?

   // steps[VSLICE] = acr_find_double(group_list, ACR_Slice_thickness, 1.0);
   steps[VSLICE] = acr_find_double(group_list,ACR_Spacing_between_slices,1.0);

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

   if (file_type == N3DCM || file_type == IMA) {

     // Find 3D coordinate of slice - SPI_Image_position gives
     // the *centre* of the slice!
     element = acr_find_group_element(group_list, SPI_Image_position);
     if ((element == NULL) ||
	 (acr_get_element_numeric_array(element, WORLD_NDIMS, coordinate) 
	  != WORLD_NDIMS)) {
       found_coordinate = FALSE;
       for (iworld=0; iworld < WORLD_NDIMS; iworld++)
         coordinate[iworld] = 0.0;
     } else {
       found_coordinate = TRUE;
     }
     
     convert_numa3_coordinate(coordinate);
     
     // Work out start positions in volume coordinates
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
     
   } else { // normal dicom
     
     // Find 3D coordinate of slice - ACR_Image_position_patient gives
     // the *corner* of the slice!
     element = acr_find_group_element(group_list, ACR_Image_position_patient);
     if ((element == NULL) ||
	 (acr_get_element_numeric_array(element, WORLD_NDIMS, coordinate) 
	  != WORLD_NDIMS)) {
       found_coordinate = FALSE;
       for (iworld=0; iworld < WORLD_NDIMS; iworld++)
         coordinate[iworld] = 0.0;
     } else {
       found_coordinate = TRUE;
     }
     
     convert_dicom_coordinate(coordinate);

     // Work out start positions in volume coordinates
     for (ivolume=0; ivolume < VOL_NDIMS; ivolume++) {
       
       if (found_coordinate && found_dircos[VSLICE] && found_dircos[VROW] &&
	   found_dircos[VCOLUMN]) {
	 starts[ivolume] = 
	   coordinate[XCOORD] * dircos[ivolume][XCOORD] +
	   coordinate[YCOORD] * dircos[ivolume][YCOORD] +
	   coordinate[ZCOORD] * dircos[ivolume][ZCOORD];
       }
       else {
	 starts[ivolume] = 0.0;
       }
     }

   }

   // Find position along each dimension
   file_info->coordinate[SLICE] = starts[VSLICE];
   file_info->coordinate[ECHO] = 
      acr_find_double(group_list, ACR_Echo_time, 0.0) / (double) MS_PER_SECOND;

   // time section (rhoge)
   // now assume that time has been fixed when file was read

   start_time = acr_find_double(group_list,ACR_Series_time, 0.0);
   frame_time = 
     acr_find_double(group_list, ACR_Acquisition_time, 0.0); 
   start_time = convert_time_to_seconds(start_time);
   frame_time = convert_time_to_seconds(frame_time) - start_time;

   // check for case where scan starts right before midnight,
   // but frame is after midnight
   if (frame_time < 0.0) frame_time += (double) SECONDS_PER_DAY;
   file_info->coordinate[TIME] = frame_time;

   /* end of time section */

   file_info->coordinate[PHASE] = 0.0;
   file_info->coordinate[CHEM_SHIFT] = 0.0;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_numa3_coordinate
@INPUT      : coordinate
@OUTPUT     : coordinate
@RETURNS    : (nothing)
@DESCRIPTION: Routine to convert a coordinate to the correct orientation
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 28, 1997 (Peter Neelin)
@MODIFIED   : made version specific to Numaris 3 SPI data (rhoge)
---------------------------------------------------------------------------- */
public void convert_numa3_coordinate(double coordinate[WORLD_NDIMS])
{
  coordinate[XCOORD] = -coordinate[XCOORD];
  coordinate[ZCOORD] = -coordinate[ZCOORD];
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_dicom_coordinate
@INPUT      : coordinate
@OUTPUT     : coordinate
@RETURNS    : (nothing)
@DESCRIPTION: Routine to convert a coordinate to the correct orientation
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : February 28, 1997 (Peter Neelin)
@MODIFIED   : made new dicom version (rhoge)
---------------------------------------------------------------------------- */
public void convert_dicom_coordinate(double coordinate[WORLD_NDIMS])
{
   coordinate[XCOORD] = -coordinate[XCOORD];
   coordinate[YCOORD] = -coordinate[YCOORD];
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
   (void) strncpy(general_info->patient.age,
      acr_find_string(group_list, ACR_Patient_age, ""), maxlen);
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
   /* added by rhoge - registration timing info */
   (void) strncpy(general_info->patient.reg_date,
      acr_find_string(group_list, ACR_Study_date, ""), maxlen);
   (void) strncpy(general_info->patient.reg_time, 
      acr_find_string(group_list, ACR_Study_time, ""), maxlen);

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
   general_info->study.field_value = 
      acr_find_double(group_list, ACR_Magnetic_field_strength, -DBL_MAX);
   (void) strncpy(general_info->study.software_version, 
      acr_find_string(group_list, ACR_Software_versions, ""), maxlen);
   (void) strncpy(general_info->study.serial_no, 
      acr_find_string(group_list, ACR_Device_serial_number, ""), maxlen);
   (void) strncpy(general_info->study.calibration_date, 
      acr_find_string(group_list, SPI_Calibration_date, ""), maxlen);
   (void) strncpy(general_info->study.institution, 
      acr_find_string(group_list, ACR_Institution_id, ""), maxlen);
   (void) strncpy(general_info->study.station_id, 
      acr_find_string(group_list, ACR_Station_id, ""), maxlen);
   (void) strncpy(general_info->study.referring_physician, 
      acr_find_string(group_list, ACR_Referring_physician, ""), maxlen);
   (void) strncpy(general_info->study.performing_physician, 
      acr_find_string(group_list, ACR_Performing_physician, ""), maxlen);
   (void) strncpy(general_info->study.operator, 
      acr_find_string(group_list, ACR_Operators_name, ""), maxlen);
   (void) strncpy(general_info->study.procedure, 
      acr_find_string(group_list, ACR_Procedure_description, ""), maxlen);
   (void) sprintf(general_info->study.study_id, "%.6f",general_info->study_id);
   /*   acquisition id modified by rhoge to get rid of first digit that 
	is not required for identification of run */
   /*   (void) sprintf(general_info->study.acquisition_id, "%d_%d",
	acr_find_int(group_list, ACR_Series, 0), general_info->acq_id); */
   (void) sprintf(general_info->study.acquisition_id, "%d",
		  general_info->acq_id);

   /* Get acquisition information */
   /*   string = acr_find_string(group_list, SPI_Sequence_file_name, "");
	ptr = string + strlen(string) - 1;
	while ((ptr > string) && (*ptr != '/')) {ptr--;}
	if ((ptr >= string) && (*ptr == '/')) string = ptr + 1;
	(void) strncpy(general_info->acq.scan_seq, string, maxlen);
	ptr = general_info->acq.scan_seq;
	while (*ptr != '\0') {
	if (*ptr == '.') *ptr = '\0';
	ptr++;
	} */

   (void) strncpy(general_info->acq.scan_seq, 
      acr_find_string(group_list, ACR_Sequence_name, ""), maxlen);
   (void) strncpy(general_info->acq.seq_owner, 
      acr_find_string(group_list, SPI_Sequence_file_owner, ""), maxlen);
   (void) strncpy(general_info->acq.seq_descr, 
      acr_find_string(group_list, SPI_Sequence_description, ""), maxlen);
   (void) strncpy(general_info->acq.protocol_name, 
      acr_find_string(group_list, ACR_Protocol_name, ""), maxlen);
   (void) strncpy(general_info->acq.receive_coil, 
      acr_find_string(group_list, ACR_Receiving_coil, ""), maxlen);
   (void) strncpy(general_info->acq.transmit_coil, 
      acr_find_string(group_list, ACR_Transmitting_coil, ""), maxlen);
   general_info->acq.rep_time = 
      acr_find_double(group_list, ACR_Repetition_time, -DBL_MAX);
   if (general_info->acq.rep_time != -DBL_MAX)
      general_info->acq.rep_time /= 1000.0;
   general_info->acq.echo_time = 
      acr_find_double(group_list, ACR_Echo_time, -DBL_MAX);
   if (general_info->acq.echo_time != -DBL_MAX)
      general_info->acq.echo_time /= 1000.0;
   general_info->acq.echo_number = 
      acr_find_double(group_list, ACR_Echo_number, -DBL_MAX);
   general_info->acq.inv_time = 
      acr_find_double(group_list, ACR_Inversion_time, -DBL_MAX);
   if (general_info->acq.inv_time != -DBL_MAX)
      general_info->acq.inv_time /= 1000.0;
   general_info->acq.b_value = 
      acr_find_double(group_list, EXT_Diffusion_b_value, -DBL_MAX);
   general_info->acq.flip_angle = 
      acr_find_double(group_list, ACR_Flip_angle, -DBL_MAX);
   general_info->acq.slice_thickness = 
      acr_find_double(group_list, ACR_Slice_thickness, -DBL_MAX);
   general_info->acq.num_slices = 
      acr_find_double(group_list, SPI_Number_of_slices_nominal, -DBL_MAX);
   general_info->acq.num_dyn_scans = 
      acr_find_double(group_list, ACR_Acquisitions_in_series, -DBL_MAX);
   general_info->acq.num_avg = 
      acr_find_double(group_list, ACR_Nr_of_averages, -DBL_MAX);
   general_info->acq.scan_dur = 
      acr_find_double(group_list, SPI_Total_measurement_time_cur, -DBL_MAX);
   general_info->acq.ky_lines = 
      acr_find_double(group_list, SPI_Number_of_fourier_lines_current, 
		      -DBL_MAX);
   general_info->acq.kymax_ix = 
      acr_find_double(group_list, SPI_Number_of_fourier_lines_after_zero, 
		      -DBL_MAX);
   general_info->acq.kymin_ix = 
      acr_find_double(group_list, SPI_First_measured_fourier_line, 
		      -DBL_MAX);
   general_info->acq.kz_lines = 
      acr_find_double(group_list, SPI_Number_of_3d_raw_part_cur, 
		      -DBL_MAX);
   general_info->acq.dummy_scans = 
      acr_find_double(group_list, SPI_Number_of_prescans, 
		      -DBL_MAX);
   general_info->acq.imaging_freq = 
      acr_find_double(group_list, ACR_Imaging_frequency, -DBL_MAX);
   if (general_info->acq.imaging_freq != -DBL_MAX)
      general_info->acq.imaging_freq *= 1e6;
   (void) strncpy(general_info->acq.imaged_nucl,
      acr_find_string(group_list, ACR_Imaged_nucleus, ""), maxlen);
   general_info->acq.adc_voltage = 
      acr_find_double(group_list, SPI_ADC_voltage, -DBL_MAX);
   general_info->acq.adc_offset = 
      acr_find_double(group_list, SPI_ADC_offset, -DBL_MAX);
   general_info->acq.transmit_ampl = 
      acr_find_double(group_list, SPI_Transmitter_amplitude, -DBL_MAX);
   general_info->acq.rec_amp_gain = 
      acr_find_double(group_list, SPI_Receiver_amplifier_gain, -DBL_MAX);
   general_info->acq.rec_preamp_gain = 
      acr_find_double(group_list, SPI_Receiver_preamplifier_gain, -DBL_MAX);
   general_info->acq.win_center = 
      acr_find_double(group_list, SPI_Window_center, -DBL_MAX);
   general_info->acq.win_width = 
      acr_find_double(group_list, SPI_Window_width, -DBL_MAX);
   general_info->acq.gy_ampl = 
      acr_find_double(group_list, SPI_Phase_gradient_amplitude, -DBL_MAX);
   general_info->acq.gx_ampl = 
      acr_find_double(group_list, SPI_Readout_gradient_amplitude, -DBL_MAX);
   general_info->acq.gz_ampl = 
     acr_find_double(group_list, SPI_Selection_gradient_amplitude, -DBL_MAX);

   general_info->acq.num_phase_enc_steps = 
     acr_find_double(group_list, ACR_Number_of_phase_encoding_steps, -DBL_MAX);
   general_info->acq.percent_sampling = 100 * 
     acr_find_double(group_list, ACR_Percent_sampling, -DBL_MAX);
   general_info->acq.percent_phase_fov = 100 *
     acr_find_double(group_list, ACR_Percent_phase_field_of_view, -DBL_MAX);
   general_info->acq.pixel_bandwidth = 
     acr_find_double(group_list, ACR_Pixel_bandwidth, -DBL_MAX);
   general_info->acq.sar = 
     acr_find_double(group_list, ACR_SAR, -DBL_MAX);
   strncpy(general_info->acq.mr_acq_type,
	   acr_find_string(group_list,ACR_MR_acquisition_type,""),4);
   strncpy(general_info->acq.image_type,
	   acr_find_string(group_list,ACR_Image_type,""),128);

   strncpy(general_info->acq.phase_enc_dir, 
	   acr_find_string(group_list,ACR_Phase_encoding_direction,""),3);
   (void) strncpy(general_info->acq.comments, "", maxlen);
   // Siemens Numaris 4 specific!

   if (0) { // this causes problems with large MrProt fields
     //     printf("allocating %d bytes for MrProt...\n",
     //	    strlen(acr_find_string(group_list, EXT_MrProt_dump, "")));
     general_info->acq.MrProt =
       MALLOC(strlen(acr_find_string(group_list, EXT_MrProt_dump, "")) * 
	      sizeof(char));
     (void) strcpy(general_info->acq.MrProt, 
		   acr_find_string(group_list, EXT_MrProt_dump, ""));
   } else {
     general_info->acq.MrProt =
       MALLOC(strlen("disabled")*sizeof(char));
     (void) strcpy(general_info->acq.MrProt, "disabled");
   }

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
#define DICOM_SECONDS_PER_HOUR 10000.0
#define DICOM_SECONDS_PER_MINUTE 100.0

  /* Variables */
  double real_time, Hours, Minutes, Seconds;

  /* Get the components of the time */

  Hours = (int) (dicom_time / (double) DICOM_SECONDS_PER_HOUR);
  dicom_time -= Hours * DICOM_SECONDS_PER_HOUR;
  Minutes = (int) (dicom_time / (double) DICOM_SECONDS_PER_MINUTE);
  dicom_time -= Minutes * (double) DICOM_SECONDS_PER_MINUTE;
  Seconds = dicom_time;

  /* Work out the number of seconds */

  real_time = (Hours * 3600.0) + (Minutes * 60.0) + Seconds;

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


