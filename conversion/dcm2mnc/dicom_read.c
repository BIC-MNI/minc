/* ----------------------------- MNI Header -----------------------------------
@NAME       : siemens_dicom_read.c
@DESCRIPTION: Code to read siemens dicom files and get info from them.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 28, 1997 (Peter Neelin)
@MODIFIED   : 
 * $Log: dicom_read.c,v $
 * Revision 1.1  2005-02-17 16:38:10  bert
 * Initial checkin, revised DICOM to MINC converter
 *
 * Revision 1.1.1.1  2003/08/15 19:52:55  leili
 * Leili's dicom server for sonata
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

#include "dcm2mnc.h"

#include <math.h>

static double convert_time_to_seconds(double dicom_time);
static void get_intensity_info(Acr_Group group_list, File_Info *file_info);
static void get_coordinate_info(Acr_Group group_list, File_Info *file_info,
                                Orientation *orientation,
                                World_Index volume_to_world[VOL_NDIMS],
                                const int sizes[VOL_NDIMS],
                                double dircos[VOL_NDIMS][WORLD_NDIMS],
                                double steps[VOL_NDIMS],
                                double starts[VOL_NDIMS],
                                double coordinate[WORLD_NDIMS]);
static void get_general_header_info(Acr_Group group_list,
                                    General_Info *general_info);
static void convert_numa3_coordinate(double coordinate[WORLD_NDIMS]);
static void convert_dicom_coordinate(double coordinate[WORLD_NDIMS]);
static void get_identification_info(Acr_Group group_list, 
                                    double *study_id, int *acq_id, 
                                    int *rec_num, int *image_type);

static int irnd(double x)
{
    if (x > 0.0) {
        x += 0.5;
    }
    else {
        x -= 0.5;
    }
    return (int) floor(x);
}



/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_file_info
@INPUT      : group_list - input data
@OUTPUT     : file_inf_p - file-specific info
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
void 
get_file_info(Acr_Group group_list, 
              File_Info *file_inf_p,
              General_Info *gen_inf_p)
{
    Mri_Index imri;             /* MRI index (SLICE, ECHO, TIME, PHASE...) */
    World_Index iworld; /* World coordinate index (XCOORD, YCOORD...) */
    World_Index jworld;
    Volume_Index ivolume; /* Voxel coordinate index (VROW, VCOLUMN...) */
    int nrows;                  /* Row count in this file */
    int ncolumns;               /* Column count in this file */
    int spatial_sizes[VOL_NDIMS]; /* Voxel coordinate extents */
    double study_id;            /* Study identifier */
    int acq_id;                 /* Acquisition identifier */
    int rec_num;                /* ? Seems to be a dummy */
    int cur_index;              /* Index of slice(s) in current file */
    int index;                  /* General index value */
    int number_of_3D_partitions;
    Orientation orientation;    /* TRANSVERSE, SAGITTAL, or CORONAL */
    World_Index volume_to_world[VOL_NDIMS]; /* Maps voxel to world indices */
    double coordinate[WORLD_NDIMS]; /* Slice coordinates */
    double dircos[VOL_NDIMS][WORLD_NDIMS]; /* Direction cosines */
    double steps[VOL_NDIMS];    /* Step (spacing) coordinates */
    double starts[VOL_NDIMS];   /* Start (origin) coordinates */
    Acr_Element_Id mri_index_list[MRI_NDIMS];
    Acr_Element_Id mri_total_list[MRI_NDIMS];

    // Initialize array of elements for MRI positions (indices)
    //
    mri_index_list[SLICE] = SPI_Current_slice_number;
    mri_index_list[ECHO] = ACR_Echo_number;
    mri_index_list[TIME] = ACR_Acquisition;
    mri_index_list[PHASE] = NULL;
    mri_index_list[CHEM_SHIFT] = NULL;

    // Initialize array for MRI dimension lengths
    //
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

    /* Get intensity information
     */
    get_intensity_info(group_list, file_inf_p);

    // Check for necessary values not found
    if ((nrows <= 0) || (ncolumns <= 0) ||
        (file_inf_p->bits_stored <= 0) ||
        (file_inf_p->bits_alloc <= 0)) {
        if (G.Debug) {
            printf("Needed values missing, marking invalid\n");
        }
        file_inf_p->valid = FALSE;
        return;
    }

    /* Get study, acq, rec, image type id's
     */
    get_identification_info(group_list, &study_id, &acq_id, &rec_num, NULL);

    /* Get number of 3D partitions for working out number of slices
     */
    number_of_3D_partitions =  
        acr_find_int(group_list, SPI_Number_of_3D_raw_partitions_nominal, 1);
    if (number_of_3D_partitions < 1) {
        number_of_3D_partitions = 1;
    }

    /* Get indices for image in current file
     */
    for (imri = 0; imri < MRI_NDIMS; imri++) {
        if (mri_index_list[imri] != NULL) {
            file_inf_p->index[imri] = acr_find_int(group_list,
                                                   mri_index_list[imri], 1);
        }
        else {
            file_inf_p->index[imri] = 1;
        }
    }

    /* Get coordinate information
     */
    get_coordinate_info(group_list, file_inf_p, &orientation, volume_to_world,
                        spatial_sizes, dircos, steps, starts, coordinate);

    /*
     * Use the coordinate information rather than the slice or time
     * position derived above.  This seems to be much more reliable.
     */

    file_inf_p->index[SLICE] = irnd(file_inf_p->coordinate[SLICE] * 100.0);
    
    /* For non-IMA files, use the time coordinate to order the time.
     * index.  IMA files do not seem to have a reliable slice time
     * indicator.
     */
    if (G.file_type != IMA) {
        file_inf_p->index[TIME] = irnd(file_inf_p->coordinate[TIME] * 100.0);
    }

    // Set up general info on first pass
    if (!gen_inf_p->initialized) {

        // Get row and columns sizes
        gen_inf_p->nrows = nrows;
        gen_inf_p->ncolumns = ncolumns;

        // Save the study, acquisition, reconstruction and image type 
        //   identifiers
        gen_inf_p->study_id = study_id;
        gen_inf_p->acq_id = acq_id;
        gen_inf_p->rec_num = rec_num;

        // No image type is available 
        // (rhoge:  no longer true?)
        gen_inf_p->image_type_string[0] = '\0';

        /* Get dimension information */
        for (imri = 0; imri < MRI_NDIMS; imri++) {

            /* Gets sizes */
            gen_inf_p->cur_size[imri] = 1;

            if (mri_total_list[imri] != NULL) {
                int def_val = 1;

                /* Special case for slice index - need to look at the 
                 * new standard element 0x0020:0x1002 "Images in 
                 * Acquisition".  We use this as a default, but override
                 * it with the Siemens-specific value if present.
                 */
                if (imri == SLICE) {
                    /* Look for the standard slice count field first.
                     */
                    def_val = acr_find_int(group_list,
                                           ACR_Images_in_acquisition,
                                           1);
                }
                gen_inf_p->max_size[imri] = acr_find_int(group_list,
                                                         mri_total_list[imri],
                                                         def_val);
            }
            else {
                gen_inf_p->max_size[imri] = 1;
            }

            if (gen_inf_p->max_size[imri] < 1) {
                gen_inf_p->max_size[imri] = 1;
            }

            /* Check for 3D partitions for slice dimensions */
            if (imri == SLICE) {
                gen_inf_p->max_size[imri] *= number_of_3D_partitions;
            }

            /* Set initial values */
            gen_inf_p->default_index[imri] = file_inf_p->index[imri];
            gen_inf_p->image_index[imri] = -1;
	
            /* Allocate space for index and coordinate arrays if 
             * max_size > 1.
             *
             * Set the first values. 
             */

            if (gen_inf_p->max_size[imri] > 1) {
                gen_inf_p->indices[imri] = 
                    malloc(gen_inf_p->max_size[imri] * sizeof(int));

                gen_inf_p->coordinates[imri] = 
                    malloc(gen_inf_p->max_size[imri] * sizeof(double));

                for (index = 0; index < gen_inf_p->max_size[imri]; index++) {
                    gen_inf_p->indices[imri][index] = -1;
                    gen_inf_p->coordinates[imri][index] = 0;
                }
                gen_inf_p->search_start[imri] = 0;
                gen_inf_p->indices[imri][0] = file_inf_p->index[imri];
                gen_inf_p->coordinates[imri][0] = file_inf_p->coordinate[imri];
            }
        }          /* Loop over dimensions */

        /* Get spatial coordinate information */
        gen_inf_p->slice_world = volume_to_world[VSLICE];
        gen_inf_p->row_world = volume_to_world[VROW];
        gen_inf_p->column_world = volume_to_world[VCOLUMN];
        for (ivolume = 0; ivolume < VOL_NDIMS; ivolume++) {
            iworld = volume_to_world[ivolume];
            gen_inf_p->step[iworld] = steps[ivolume];
            gen_inf_p->start[iworld] = starts[ivolume];
            for (jworld = 0; jworld < WORLD_NDIMS; jworld++) {
                gen_inf_p->dircos[iworld][jworld] = dircos[ivolume][jworld];
            }

            if (G.Debug) {
                printf("%s axis step %f, start %f, cosines %f,%f,%f\n",
                       World_Names[iworld],
                       gen_inf_p->step[iworld],
                       gen_inf_p->start[iworld],
                       gen_inf_p->dircos[iworld][XCOORD],
                       gen_inf_p->dircos[iworld][YCOORD],
                       gen_inf_p->dircos[iworld][ZCOORD]
                       );
            }
        }

        /* Set data type and range */
        if (file_inf_p->bits_alloc <= 8) {
            gen_inf_p->datatype = NC_BYTE;
        }
        else {
            gen_inf_p->datatype = NC_SHORT;
        }
        gen_inf_p->is_signed = ((gen_inf_p->datatype == NC_SHORT) &&
                                (file_inf_p->bits_stored < 16));
        gen_inf_p->pixel_min = file_inf_p->pixel_min;
        gen_inf_p->pixel_max = file_inf_p->pixel_max;

        /* Save display window info */
        gen_inf_p->window_min = file_inf_p->window_min;
        gen_inf_p->window_max = file_inf_p->window_max;

        /* Get the rest of the header information */
        get_general_header_info(group_list, gen_inf_p);

        /* Copy the group list */
        gen_inf_p->group_list = acr_copy_group_list(group_list);

        /* Set initialized flag */
        gen_inf_p->initialized = TRUE;
    }

    /* Set up file info */

    /* Update general info and validate file on later passes 
     */
    else {

        /* Check for consistent data type */
        if (((gen_inf_p->datatype == NC_BYTE) && 
             (file_inf_p->bits_alloc > 8)) || 
            ((gen_inf_p->datatype == NC_SHORT) &&
             (file_inf_p->bits_alloc <= 8))) {
            if (G.Debug) {
                printf("Inconsistent datatype, marking invalid\n");
            }
            file_inf_p->valid = FALSE;
            return;
        }
     
        /* Check row and columns sizes */
        if ((nrows != gen_inf_p->nrows) &&
            (ncolumns != gen_inf_p->ncolumns))  {
            if (G.Debug) {
                printf("Mismatched rows/columns, marking invalid\n");
            }
            file_inf_p->valid = FALSE;
            return;
        }
     
        /* Check study and acquisition id's */
        if ((gen_inf_p->study_id != study_id) ||
            (gen_inf_p->acq_id != acq_id)) {
            if (G.Debug) {
                printf("Mismatched acquisition/study, marking invalid\n");
            }
            file_inf_p->valid = FALSE;
            return;
        }
     
        /* Look to see if indices have changed */
        for (imri = 0; imri < MRI_NDIMS; imri++) {
       
            /* Get current index */
            cur_index = file_inf_p->index[imri];
       
            /* Check whether this index is in the list.
             */
            if (gen_inf_p->cur_size[imri] == 1) {
                index = ((cur_index == gen_inf_p->default_index[imri]) ? 0 : -1);
            }
            else {
                /* Search list of indices for 'cur_index'.  Search is 
                   started at search_start[] and has maximum length of
                   size[imri].
                */
                index = search_list(cur_index, 
                                    gen_inf_p->indices[imri],
                                    gen_inf_p->cur_size[imri],
                                    gen_inf_p->search_start[imri]);
            }

            /* If it is not, then add it */
            if (index < 0) {
	 
                /* Check whether we can add a new index */
                if (gen_inf_p->cur_size[imri] >= gen_inf_p->max_size[imri]) {
                    gen_inf_p->max_size[imri]++;
                    gen_inf_p->indices[imri] = 
                        realloc(gen_inf_p->indices[imri],
                                gen_inf_p->max_size[imri] * sizeof(int));

                    gen_inf_p->coordinates[imri] = 
                        realloc(gen_inf_p->coordinates[imri],
                                gen_inf_p->max_size[imri] * sizeof(double));
                }

	 
                /* Add the index and coordinate to the lists */
                index = gen_inf_p->cur_size[imri];
                gen_inf_p->search_start[imri] = index;
                gen_inf_p->indices[imri][index] = cur_index;
                gen_inf_p->coordinates[imri][index] = file_inf_p->coordinate[imri];
                gen_inf_p->cur_size[imri]++;
	 
            }
        }              /* Loop over Mri_Index */

        // note that number of slices will be wrong here for mosaics
        // we add some other mosaic-relevant fields...

        gen_inf_p->num_mosaic_rows =
            acr_find_int(group_list, EXT_Mosaic_rows, 1);
        gen_inf_p->num_mosaic_cols =
            acr_find_int(group_list, EXT_Mosaic_columns, 1);
        gen_inf_p->num_slices_in_file = 
            acr_find_int(group_list, EXT_Slices_in_file, 1);
        gen_inf_p->sub_image_rows =
            acr_find_short(group_list, EXT_Sub_image_rows,
                           acr_find_short(group_list, ACR_Rows, 0));
        gen_inf_p->sub_image_columns =
            acr_find_short(group_list, EXT_Sub_image_columns,
                           acr_find_short(group_list, ACR_Rows, 0));

        // Update display window info
        if (gen_inf_p->window_min > file_inf_p->window_min) 
            gen_inf_p->window_min = file_inf_p->window_min;
        if (gen_inf_p->window_max < file_inf_p->window_max)
            gen_inf_p->window_max = file_inf_p->window_max;
     
    }  // Update general info for this file

    // If we get to here, then we have a valid file
    file_inf_p->valid = TRUE;
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
static void 
get_identification_info(Acr_Group group_list, 
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

        if ( (G.file_type == N3DCM) &&
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
   @OUTPUT     : file_inf_p - file-specific info
   @RETURNS    : (nothing)
   @DESCRIPTION: Routine to get intensity information from a group list
   @METHOD     : 
   @GLOBALS    : 
   @CALLS      : 
   @CREATED    : February 28, 1997 (Peter Neelin)
   @MODIFIED   : 
   ---------------------------------------------------------------------------- */
static void
get_intensity_info(Acr_Group group_list, File_Info *file_inf_p)
{
    double window_centre, window_width;

    /* Get pixel storage information */
    file_inf_p->bits_alloc = acr_find_short(group_list, ACR_Bits_allocated, 0);
    file_inf_p->bits_stored = acr_find_short(group_list, ACR_Bits_stored, 0);

    if (G.useMinMax) {
        // Get pixel value information
        // I think this might wrongly assume that the ACR values min/max
        // apply to the whole volume - it actually appears that they apply
        // to the current slice.

        file_inf_p->pixel_min = acr_find_short(group_list, 
                                               ACR_Smallest_pixel_value, 0);

        file_inf_p->pixel_max = acr_find_short(group_list,
                                               ACR_Largest_pixel_value, 
                                               (1 << file_inf_p->bits_stored)-1);

    }
    else {
        // for now, use bits_stored to determine dynamic range
        // DICOM info on largest pixel applies to first slice, 
        // not whole volume - this caused problems (roundoff?)
        // in Siemens Numaris 4 scans

        file_inf_p->pixel_min = 0;
        file_inf_p->pixel_max = (1 << file_inf_p->bits_stored) - 1;
    }

    file_inf_p->slice_min = file_inf_p->pixel_min;
    file_inf_p->slice_max = file_inf_p->pixel_max;

    /* Get window min and max */
    window_centre = (file_inf_p->slice_max + file_inf_p->slice_min) / 2.0;
    window_width  = file_inf_p->slice_max - file_inf_p->slice_min;
    window_centre = 
        acr_find_double(group_list, ACR_Window_centre, window_centre);
    window_width = 
        acr_find_double(group_list, ACR_Window_width, window_width);
    file_inf_p->window_min = window_centre - window_width / 2.0;
    file_inf_p->window_max = window_centre + window_width / 2.0; 

}

/* ----------------------------- MNI Header -----------------------------------
   @NAME       : get_coordinate_info
   @INPUT      : group_list - input data
                 sizes - size of each spatial dimension
   @OUTPUT     : file_inf_p - file-specific info
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

#define DARRAY_SIZE 2

static void
get_coordinate_info(Acr_Group group_list, 
                    File_Info *file_inf_p,
                    Orientation *orientation,
                    World_Index volume_to_world[VOL_NDIMS],
                    const int sizes[VOL_NDIMS],
                    double dircos[VOL_NDIMS][WORLD_NDIMS],
                    double steps[VOL_NDIMS],
                    double starts[VOL_NDIMS],
                    double coordinate[WORLD_NDIMS])
{
    Volume_Index ivolume;
    World_Index iworld;
    Acr_Element_Id dircos_elid[VOL_NDIMS];
    Acr_Element element;
    int found_dircos[VOL_NDIMS];
    int found_coordinate;
    double frame_time;
    double start_time;
    double magnitude;
    double largest;
    double centre;
    double darray[DARRAY_SIZE];
    double dbl_tmp1, dbl_tmp2;

    double RowColVec[6]; /* row/column unit vectors in public dicom element */

    const Orientation orientation_list[WORLD_NDIMS] = {
        SAGITTAL, CORONAL, TRANSVERSE
    };

    if (G.file_type == N3DCM || G.file_type == IMA) {

        /* Set direction cosine element ids. Note that the reversal of
           rows and columns is intentional - their idea of the meaning
           of theses labels is different from ours. (Their row vector
           points along the row and not along the row dimension.) */

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
    }
    else {
        // in normal dicom file, you just have row and column unit vectors
     
        // read in row/col vectors:
        element = acr_find_group_element(group_list,
                                         ACR_Image_orientation_patient);
        if (element == NULL) {
            /* If we failed to find the newer, better patient orientation
             * information, try to use the obsolete information if present.
             */
            element = acr_find_group_element(group_list,
                                             ACR_Image_orientation_patient_old);
            if (element == NULL) {
                return;
            }
        }
     
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
    for (ivolume=0; ivolume < DARRAY_SIZE; ivolume++) {
        darray[ivolume] = -DBL_MAX;
    }

    // note ACR_Pixel_size should now be called Pixel_spacing
    element = acr_find_group_element(group_list, ACR_Pixel_size);
    if (element != NULL) {
        acr_get_element_numeric_array(element, DARRAY_SIZE, darray);
    }

    if (darray[0] == -DBL_MAX) 
        darray[0] = 1.0;
    if (darray[1] == -DBL_MAX) 
        darray[1] = darray[0];

    steps[VCOLUMN] = darray[0];
    steps[VROW] = darray[1];    // anisotropic resolution?

    /* Figure out the slice thickness.  It could be from either one of
     * two possible places in the file.
     */
    dbl_tmp1 = acr_find_double(group_list, ACR_Slice_thickness, 0.0);
    dbl_tmp2 = acr_find_double(group_list, ACR_Spacing_between_slices, 0.0);

    if (dbl_tmp1 == 0.0) {
        if (dbl_tmp2 == 0.0) {
            if (G.Debug > 1) {
                printf("Using default slice thickness of 1.0\n");
            }
            steps[VSLICE] = 1.0;
        }
        else {
            if (G.Debug > 1) {
                printf("Using (0018,0088) for slice thickness\n");
            }
            steps[VSLICE] = dbl_tmp2;
        }
    }
    else if (dbl_tmp2 == 0.0) {
        if (G.Debug > 1) {
            printf("Using (0018,0050) for slice thickness\n");
        }
        steps[VSLICE] = dbl_tmp1;
    }
    else {
        if (dbl_tmp1 != dbl_tmp2) {
            printf("WARNING: slice thickness value conflict:\n");
            printf("(0018,0050) = %f, (0018,0088) = %f\n",
                   dbl_tmp1, dbl_tmp2);
            /* Choose the maximum for now - seems reasonable, maybe?? */
            steps[VSLICE] = (dbl_tmp1 > dbl_tmp2) ? dbl_tmp1 : dbl_tmp2;
        }
        else {
            steps[VSLICE] = dbl_tmp1;
        }
    }

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

    if (G.file_type == N3DCM || G.file_type == IMA) {
        //
        // Find 3D coordinate of slice - SPI_Image_position gives
        // the *centre* of the slice!
        //
        element = acr_find_group_element(group_list, SPI_Image_position);
        if ((element == NULL) ||
            (acr_get_element_numeric_array(element, WORLD_NDIMS, coordinate)
             != WORLD_NDIMS)) {
            found_coordinate = FALSE;
            for (iworld = 0; iworld < WORLD_NDIMS; iworld++) {
                coordinate[iworld] = 0.0;
            }
        } 
        else {
            found_coordinate = TRUE;
        }
     
        convert_numa3_coordinate(coordinate);
     
        // Work out start positions in volume coordinates
        for (ivolume = 0; ivolume < VOL_NDIMS; ivolume++) {
       
            if (found_coordinate && 
                found_dircos[VSLICE] && 
                found_dircos[VROW] &&
                found_dircos[VCOLUMN]) {
                centre = 
                    coordinate[XCOORD] * dircos[ivolume][XCOORD] +
                    coordinate[YCOORD] * dircos[ivolume][YCOORD] +
                    coordinate[ZCOORD] * dircos[ivolume][ZCOORD];
            }
            else {
                centre = 0.0;
            }
            if (G.Debug > 1 && ivolume == VSLICE) {
                printf("centre = %f, steps = %f, sizes %d XYZ = %f,%f,%f\n",
                       centre, steps[VSLICE], sizes[VSLICE],
                       coordinate[XCOORD],
                       coordinate[YCOORD],
                       coordinate[ZCOORD]);
            }
            starts[ivolume] = centre -
                (steps[ivolume] * (sizes[ivolume] - 1.0) / 2.0);
        }
     
    } 
    else {                      // normal dicom
     
        // Find 3D coordinate of slice - ACR_Image_position_patient gives
        // the *corner* of the slice!
        element = acr_find_group_element(group_list, ACR_Image_position_patient);
        if ((element == NULL) ||
            (acr_get_element_numeric_array(element, WORLD_NDIMS, coordinate) 
             != WORLD_NDIMS)) {
            found_coordinate = FALSE;
            for (iworld = 0; iworld < WORLD_NDIMS; iworld++) {
                coordinate[iworld] = 0.0;
            }
        } 
        else {
            found_coordinate = TRUE;
        }
     
        convert_dicom_coordinate(coordinate);

        // Work out start positions in volume coordinates
        for (ivolume=0; ivolume < VOL_NDIMS; ivolume++) {
       
            if (found_coordinate && 
                found_dircos[VSLICE] && 
                found_dircos[VROW] &&
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
    file_inf_p->coordinate[SLICE] = starts[VSLICE];
    file_inf_p->coordinate[ECHO] = 
        acr_find_double(group_list, ACR_Echo_time, 0.0) / MS_PER_SECOND;

    // time section (rhoge)
    // now assume that time has been fixed when file was read

    start_time = acr_find_double(group_list, ACR_Series_time, 0.0);
    frame_time = acr_find_double(group_list, ACR_Acquisition_time, 0.0);
    start_time = convert_time_to_seconds(start_time);
    frame_time = convert_time_to_seconds(frame_time) - start_time;

    // check for case where scan starts right before midnight,
    // but frame is after midnight
    if (frame_time < 0.0) {
        frame_time += SECONDS_PER_DAY;
    }
    file_inf_p->coordinate[TIME] = frame_time;

    /* end of time section */

    file_inf_p->coordinate[PHASE] = 0.0;
    file_inf_p->coordinate[CHEM_SHIFT] = 0.0;

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
static void 
convert_numa3_coordinate(double coordinate[WORLD_NDIMS])
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
static void
convert_dicom_coordinate(double coordinate[WORLD_NDIMS])
{
    coordinate[XCOORD] = -coordinate[XCOORD];
    coordinate[YCOORD] = -coordinate[YCOORD];
}

/* ----------------------------- MNI Header -----------------------------------
   @NAME       : get_general_header_info
   @INPUT      : group_list - input data
   @OUTPUT     : gen_inf_p - general information about files
   @RETURNS    : (nothing)
   @DESCRIPTION: Routine to extract general header information from a group list
   @METHOD     : 
   @GLOBALS    : 
   @CALLS      : 
   @CREATED    : February 28, 1997 (Peter Neelin)
   @MODIFIED   : 
   ---------------------------------------------------------------------------- */

static void
get_string_field(char *out_str, Acr_Group group_list, 
                 Acr_Element_Id element_id)
{
    strncpy(out_str, acr_find_string(group_list, element_id, ""), 
            STRING_T_LEN);
}
                 
void
get_general_header_info(Acr_Group group_list, General_Info *gen_inf_p)
{
    int length;
    char *string;

    if (G.Debug) {
        printf("SOP Class UID: %s\n",
               acr_find_string(group_list, ACR_SOP_Class_UID, ""));
        printf("Images in acquisition: %d\n", 
               acr_find_int(group_list, ACR_Images_in_acquisition, -1));
        printf("Acquisitions in series: %d\n", 
               acr_find_int(group_list, ACR_Acquisitions_in_series, -1));
        printf("3D raw partitions: %d\n", 
               acr_find_int(group_list, SPI_Number_of_3D_raw_partitions_nominal, -1));
    }
    /* Get intensity units */
    strncpy(gen_inf_p->units, "", STRING_T_LEN);

    /* Get patient info */
    get_string_field(gen_inf_p->patient.name, group_list, ACR_Patient_name);

    get_string_field(gen_inf_p->patient.identification,
                     group_list, ACR_Patient_identification);
    get_string_field(gen_inf_p->patient.birth_date,
                     group_list, ACR_Patient_birth_date);

    get_string_field(gen_inf_p->patient.age,
                     group_list, ACR_Patient_age);

    string = acr_find_string(group_list, ACR_Patient_sex, "");
    if (*string == 'M') 
        strncpy(gen_inf_p->patient.sex, MI_MALE, STRING_T_LEN);
    else if (*string == 'F') 
        strncpy(gen_inf_p->patient.sex, MI_FEMALE, STRING_T_LEN);
    else if (*string == 'O') 
        strncpy(gen_inf_p->patient.sex, MI_OTHER, STRING_T_LEN);
    else 
        strncpy(gen_inf_p->patient.sex, "", STRING_T_LEN);

    gen_inf_p->patient.weight = 
        acr_find_double(group_list, ACR_Patient_weight, -DBL_MAX);

    /* added by rhoge - registration timing info */
    get_string_field(gen_inf_p->patient.reg_date,
                     group_list, ACR_Study_date);

    get_string_field(gen_inf_p->patient.reg_time, 
                     group_list, ACR_Study_time);

    /* Get study info */
    get_string_field(gen_inf_p->study.start_time, 
                     group_list, ACR_Study_date);

    length = strlen(gen_inf_p->study.start_time);
    gen_inf_p->study.start_time[length] = ' ';
    length++;
    strncpy(&gen_inf_p->study.start_time[length], 
            acr_find_string(group_list, ACR_Study_time, ""), STRING_T_LEN - length);
    string = acr_find_string(group_list, ACR_Modality, "");
    if (strcmp(string, ACR_MODALITY_MR) == 0)
        strncpy(gen_inf_p->study.modality, MI_MRI, STRING_T_LEN);
    get_string_field(gen_inf_p->study.manufacturer, 
                     group_list, ACR_Manufacturer);
    get_string_field(gen_inf_p->study.model, 
                     group_list, ACR_Manufacturer_model);
    gen_inf_p->study.field_value = 
        acr_find_double(group_list, ACR_Magnetic_field_strength, -DBL_MAX);
    get_string_field(gen_inf_p->study.software_version, 
                     group_list, ACR_Software_versions);
    get_string_field(gen_inf_p->study.serial_no, 
                     group_list, ACR_Device_serial_number);
    get_string_field(gen_inf_p->study.calibration_date, 
                     group_list, ACR_Calibration_date);
    get_string_field(gen_inf_p->study.institution, 
                     group_list, ACR_Institution_id);
    get_string_field(gen_inf_p->study.station_id, 
                     group_list, ACR_Station_id);
    get_string_field(gen_inf_p->study.referring_physician, 
                     group_list, ACR_Referring_physician);
    get_string_field(gen_inf_p->study.performing_physician, 
                     group_list, ACR_Performing_physician);
    get_string_field(gen_inf_p->study.operator, 
                     group_list, ACR_Operators_name);
    get_string_field(gen_inf_p->study.procedure, 
                     group_list, ACR_Procedure_description);
    sprintf(gen_inf_p->study.study_id, "%.6f",gen_inf_p->study_id);
    /* Acquisition id modified by rhoge to get rid of first digit that 
       is not required for identification of run */
    /*   sprintf(gen_inf_p->study.acquisition_id, "%d_%d",
         acr_find_int(group_list, ACR_Series, 0), gen_inf_p->acq_id); */
    sprintf(gen_inf_p->study.acquisition_id, "%d",
            gen_inf_p->acq_id);

    /* Get acquisition information */

    get_string_field(gen_inf_p->acq.scan_seq, group_list, ACR_Sequence_name);
    get_string_field(gen_inf_p->acq.seq_owner,
                     group_list, SPI_Sequence_file_owner);
    get_string_field(gen_inf_p->acq.seq_descr, 
                     group_list, SPI_Sequence_description);
    get_string_field(gen_inf_p->acq.protocol_name, 
                     group_list, ACR_Protocol_name);
    get_string_field(gen_inf_p->acq.receive_coil, 
                     group_list, ACR_Receiving_coil);
    get_string_field(gen_inf_p->acq.transmit_coil, 
                     group_list, ACR_Transmitting_coil);

    gen_inf_p->acq.rep_time = 
        acr_find_double(group_list, ACR_Repetition_time, -DBL_MAX);
    if (gen_inf_p->acq.rep_time != -DBL_MAX)
        gen_inf_p->acq.rep_time /= 1000.0;

    gen_inf_p->acq.echo_time = 
        acr_find_double(group_list, ACR_Echo_time, -DBL_MAX);
    if (gen_inf_p->acq.echo_time != -DBL_MAX)
        gen_inf_p->acq.echo_time /= 1000.0;

    gen_inf_p->acq.echo_number = 
        acr_find_double(group_list, ACR_Echo_number, -DBL_MAX);

    gen_inf_p->acq.inv_time = 
        acr_find_double(group_list, ACR_Inversion_time, -DBL_MAX);
    if (gen_inf_p->acq.inv_time != -DBL_MAX)
        gen_inf_p->acq.inv_time /= 1000.0;
    gen_inf_p->acq.b_value = 
        acr_find_double(group_list, EXT_Diffusion_b_value, -DBL_MAX);
    gen_inf_p->acq.flip_angle = 
        acr_find_double(group_list, ACR_Flip_angle, -DBL_MAX);
    gen_inf_p->acq.slice_thickness = 
        acr_find_double(group_list, ACR_Slice_thickness, -DBL_MAX);
    gen_inf_p->acq.num_slices = 
        acr_find_double(group_list, SPI_Number_of_slices_nominal, -DBL_MAX);
    gen_inf_p->acq.num_dyn_scans = 
        acr_find_double(group_list, ACR_Acquisitions_in_series, -DBL_MAX);
    gen_inf_p->acq.num_avg = 
        acr_find_double(group_list, ACR_Nr_of_averages, -DBL_MAX);
    gen_inf_p->acq.scan_dur = 
        acr_find_double(group_list, SPI_Total_measurement_time_cur, -DBL_MAX);
    gen_inf_p->acq.ky_lines = 
        acr_find_double(group_list, SPI_Number_of_fourier_lines_current, 
                        -DBL_MAX);
    gen_inf_p->acq.kymax_ix = 
        acr_find_double(group_list, SPI_Number_of_fourier_lines_after_zero, 
                        -DBL_MAX);
    gen_inf_p->acq.kymin_ix = 
        acr_find_double(group_list, SPI_First_measured_fourier_line, -DBL_MAX);
    gen_inf_p->acq.kz_lines = 
        acr_find_double(group_list, SPI_Number_of_3d_raw_part_cur, -DBL_MAX);
    gen_inf_p->acq.dummy_scans = 
        acr_find_double(group_list, SPI_Number_of_prescans, -DBL_MAX);
    gen_inf_p->acq.imaging_freq = 
        acr_find_double(group_list, ACR_Imaging_frequency, -DBL_MAX);
    if (gen_inf_p->acq.imaging_freq != -DBL_MAX)
        gen_inf_p->acq.imaging_freq *= 1e6;
    get_string_field(gen_inf_p->acq.imaged_nucl, 
                     group_list, ACR_Imaged_nucleus);
    gen_inf_p->acq.adc_voltage = 
        acr_find_double(group_list, SPI_ADC_voltage, -DBL_MAX);
    gen_inf_p->acq.adc_offset = 
        acr_find_double(group_list, SPI_ADC_offset, -DBL_MAX);
    gen_inf_p->acq.transmit_ampl = 
        acr_find_double(group_list, SPI_Transmitter_amplitude, -DBL_MAX);
    gen_inf_p->acq.rec_amp_gain = 
        acr_find_double(group_list, SPI_Receiver_amplifier_gain, -DBL_MAX);
    gen_inf_p->acq.rec_preamp_gain = 
        acr_find_double(group_list, SPI_Receiver_preamplifier_gain, -DBL_MAX);
    gen_inf_p->acq.win_center = 
        acr_find_double(group_list, ACR_Window_centre, -DBL_MAX);
    gen_inf_p->acq.win_width = 
        acr_find_double(group_list, ACR_Window_width, -DBL_MAX);
    gen_inf_p->acq.gy_ampl = 
        acr_find_double(group_list, SPI_Phase_gradient_amplitude, -DBL_MAX);
    gen_inf_p->acq.gx_ampl = 
        acr_find_double(group_list, SPI_Readout_gradient_amplitude, -DBL_MAX);
    gen_inf_p->acq.gz_ampl = 
        acr_find_double(group_list, SPI_Selection_gradient_amplitude, -DBL_MAX);

    gen_inf_p->acq.num_phase_enc_steps = 
        acr_find_double(group_list, ACR_Number_of_phase_encoding_steps, -DBL_MAX);
    gen_inf_p->acq.percent_sampling = 100 * 
        acr_find_double(group_list, ACR_Percent_sampling, -DBL_MAX);

    gen_inf_p->acq.percent_phase_fov = 100 *
        acr_find_double(group_list, ACR_Percent_phase_field_of_view, -DBL_MAX);

    gen_inf_p->acq.pixel_bandwidth = 
        acr_find_double(group_list, ACR_Pixel_bandwidth, -DBL_MAX);

    gen_inf_p->acq.sar = acr_find_double(group_list, ACR_SAR, -DBL_MAX);

    get_string_field(gen_inf_p->acq.mr_acq_type, 
                     group_list, ACR_MR_acquisition_type);

    get_string_field(gen_inf_p->acq.image_type, group_list, ACR_Image_type);

    get_string_field(gen_inf_p->acq.phase_enc_dir, 
                     group_list, ACR_Phase_encoding_direction);

    strncpy(gen_inf_p->acq.comments, "", STRING_T_LEN);

    // Siemens Numaris 4 specific!

#if 0
    gen_inf_p->acq.MrProt = strdup(acr_find_string(group_list,
                                                   EXT_MrProt_dump,
                                                   ""));
#else
    gen_inf_p->acq.MrProt = strdup("");
#endif
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
static double
convert_time_to_seconds(double dicom_time)
{
    /* Constants */
#define DICOM_SECONDS_PER_HOUR 10000.0
#define DICOM_SECONDS_PER_MINUTE 100.0

    /* Variables */
    double hh, mm, ss;

    /* Get the components of the time */

    hh = (int) (dicom_time / DICOM_SECONDS_PER_HOUR);
    dicom_time -= hh * DICOM_SECONDS_PER_HOUR;
    mm = (int) (dicom_time / DICOM_SECONDS_PER_MINUTE);
    dicom_time -= mm * DICOM_SECONDS_PER_MINUTE;
    ss = dicom_time;

    /* Work out the number of seconds */

    return (hh * 3600.0) + (mm * 60.0) + ss;
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
void 
get_siemens_dicom_image(Acr_Group group_list, Image_Data *image)
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
                                 ACR_IMAGE_GID);

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
        memset(image->data, 0, imagepix * sizeof(short));
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
            memset(image->data, 0, imagepix * sizeof(short));
        }
    }

    return;
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : parse_dicom_groups
@INPUT      : group_list - list of acr-nema groups that make up object
@OUTPUT     : data_info - information about data object
@RETURNS    : (nothing)
@DESCRIPTION: Routine to parse dicom object
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June 2001 (Rick Hoge)
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define IDEFAULT (-1)

void
parse_dicom_groups(Acr_Group group_list, Data_Object_Info *di_ptr)
{
    Acr_Element element;
    unsigned short AcqMat[4];
    unsigned short freq_rows;
    unsigned short freq_cols;
    unsigned short phase_rows;
    unsigned short phase_cols;

    // Get info to construct unique identifiers for study, series/acq
    // for file processing
    get_identification_info(group_list,
                            &(di_ptr->study_id), &(di_ptr->acq_id),
                            &(di_ptr->rec_num), &(di_ptr->image_type));

    // Get number of echos, echo number, number of dynamic scans and 
    // dynamic_scan_number
    //

    di_ptr->num_echoes = acr_find_int(group_list, 
                                      SPI_Number_of_echoes, 
                                      IDEFAULT);
    if (di_ptr->num_echoes == IDEFAULT) {
        di_ptr->num_echoes = acr_find_int(group_list,
                                          ACR_Echo_train_length, IDEFAULT);
    }

    di_ptr->echo_number = acr_find_int(group_list, 
                                       ACR_Echo_number, 
                                       IDEFAULT);

    di_ptr->num_dyn_scans = acr_find_int(group_list, 
                                         ACR_Acquisitions_in_series, 
                                         IDEFAULT);

    di_ptr->dyn_scan_number = acr_find_int(group_list, 
                                           ACR_Acquisition, 
                                           IDEFAULT);

    di_ptr->global_image_number = acr_find_int(group_list, 
                                               ACR_Image, 
                                               IDEFAULT);

    /* rhoge:
       new info added to di_ptr by rhoge: nominal number of slices;
       this is used in detection of a stream of files with the same
       acquisition ID number in which there are more files than
       slices.  If the number of signal averages is greater than one,
       we will assume that this means the acquisition loop was used for
       dynamic scanning.  

       WARNINGS:  the same thing may need to be done with `number of
       partitions' for it to work with 3D scans  */

    di_ptr->num_slices_nominal = acr_find_int(group_list, 
                                              SPI_Number_of_slices_nominal,
                                              IDEFAULT);
    if (di_ptr->num_slices_nominal == IDEFAULT) {
        di_ptr->num_slices_nominal = acr_find_int(group_list,
                                                  ACR_Images_in_acquisition,
                                                  IDEFAULT);
    }

    di_ptr->slice_number = acr_find_int(group_list,
                                        SPI_Current_slice_number,
                                        IDEFAULT);
    if (di_ptr->slice_number == IDEFAULT) {
        di_ptr->slice_number = rint(acr_find_double(group_list,
                                                    ACR_Slice_location,
                                                    0.0) * 100.0);
    }

    /* identification info needed to generate unique session id
     * for file names
     */
    di_ptr->study_date = acr_find_int(group_list, ACR_Study_date, 
                                      IDEFAULT); 

    di_ptr->study_time = acr_find_int(group_list, ACR_Study_time, 
                                      IDEFAULT); 

    di_ptr->scanner_serialno = acr_find_int(group_list, 
                                            ACR_Device_serial_number, 
                                            IDEFAULT); 

    // identification info needed to determine if mosaics used 

    element = acr_find_group_element(group_list, ACR_Acquisition_matrix);

    if (element != NULL) {
        acr_get_element_short_array(element, 4, AcqMat);

        freq_rows = AcqMat[0];
        freq_cols = AcqMat[1];
    
        phase_rows = AcqMat[2];
        phase_cols = AcqMat[3];

        /* rows in acq matrix is larger of freq rows and freq columns:
         */
        di_ptr->acq_rows = ( freq_rows > freq_cols ? freq_rows : freq_cols );

        /* all images are square, at this time 
         */
        di_ptr->acq_cols = di_ptr->acq_rows;
    }
    else {
        di_ptr->acq_rows = IDEFAULT;
        di_ptr->acq_cols = IDEFAULT;
    }

    di_ptr->rec_rows = acr_find_int(group_list, ACR_Rows, IDEFAULT);
    di_ptr->rec_cols = acr_find_int(group_list, ACR_Columns, IDEFAULT);

    di_ptr->num_mosaic_rows = acr_find_int(group_list, EXT_Mosaic_rows, 
                                           IDEFAULT);
    di_ptr->num_mosaic_cols = acr_find_int(group_list, EXT_Mosaic_columns, 
                                           IDEFAULT);
    di_ptr->num_slices_in_file = acr_find_int(group_list, EXT_Slices_in_file,
                                              IDEFAULT);

    // sequence, protocol names (useful for debugging):

    get_string_field(di_ptr->sequence_name, group_list, ACR_Sequence_name);
    get_string_field(di_ptr->protocol_name, group_list, ACR_Protocol_name);
}


