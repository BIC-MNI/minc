/* ----------------------------- MNI Header -----------------------------------
   @NAME       : dicom_to_minc.c
   @DESCRIPTION: Code to convert a list of DICOM files to minc 
   format.
   @METHOD     : 
   @GLOBALS    : 
   @CALLS      : 
   @CREATED    : January 28, 1997 (Peter Neelin)
   @MODIFIED   : 
   * $Log: dicom_to_minc.c,v $
   * Revision 1.22  2007-08-13 16:34:52  ilana
   * mods to handle naming scheme of more diffusion sequences
   *
   * Revision 1.21  2007/06/08 20:28:57  ilana
   * added several fields to mincheader (dicom elements and found in ASCONV header)
   *
   * Revision 1.20  2007/05/30 15:17:34  ilana
   * fix so that diffusion images all written into 1 4d volume, gradient directions and bvalues are written to mincheader, some fixes for TIM diffusion images
   *
   * Revision 1.19  2006/05/11 14:45:14  bert
   * Fix endian-ness issues when parsing Siemens proprietary fields
   *
   * Revision 1.18  2006/04/09 15:39:04  bert
   * Improve support for DTI
   *
   * Revision 1.17  2006/02/09 20:54:29  bert
   * More changes to dcm2mnc
   *
   * Revision 1.16  2005/12/13 17:31:13  bert
   * Ignore DICOM protocol errors. This change was necessitated by images from a Philips Intera scanner version 'NT 10.4.1\\PIIM V2.1.4.1 MIMIT MCS' that appears to set the DICOM length field incorrectly.
   *
   * Revision 1.15  2005/08/26 21:25:54  bert
   * Latest changes ported from 1.0 branch
   *
   * Revision 1.13.2.5  2005/08/18 18:18:35  bert
   * Implement the -usecoordinates option, also some minor cleanup and fixes for some warning messages.
   *
   * Revision 1.13.2.4  2005/08/18 16:38:44  bert
   * Minor updates for dealing w/older numaris data
   *
   * Revision 1.13.2.3  2005/07/14 16:47:55  bert
   * Handle additional classes of Numa3 mosaics by using the Siemens 'base raw matrix size' element
   *
   * Revision 1.13.2.2  2005/06/20 22:01:15  bert
   * Add basic support for multiframe DICOM images
   *
   * Revision 1.13.2.1  2005/05/12 21:16:47  bert
   * Initial checkin
   *
   * Revision 1.13  2005/05/09 15:33:20  bert
   * Fix handing of descending mosaic sequences; don't print GEMS field missing warnings by default
   *
   * Revision 1.12  2005/04/28 17:38:06  bert
   * Insert and retrieve width information when sorting a dimension
   *
   * Revision 1.11  2005/04/21 22:31:29  bert
   * Copy SPI_Magnetic_field_strength to standard field in copy_spi_to_acr(); Relax some tests to avoid spurious warnings
   *
   * Revision 1.10  2005/04/20 23:25:50  bert
   * Add copy_spi_to_acr() function, minor name and comment changes
   *
   * Revision 1.9  2005/04/20 17:48:16  bert
   * Copy SPI_Number_of_slices_nominal to ACR_Image_in_acquisition for Siemens
   *
   * Revision 1.8  2005/04/18 21:04:25  bert
   * Get rid of old is_reversed behavior, always use most natural sort of the image. Also tried to fix listing function somewhat.
   *
   * Revision 1.7  2005/04/05 21:55:33  bert
   * Update handling of Siemens ASCCONV to reflect value of uc2DInterpolation field for mosaic images.  Additional cleanup of mosaic code and minor tweak to suppress GEMS warning message for PET data.
   *
   * Revision 1.6  2005/03/29 20:20:42  bert
   * Add checks for GE Medical Systems proprietary files
   *
   * Revision 1.5  2005/03/14 22:26:40  bert
   * Dump the entire coordinate array for each MRI dimension after sorting.  Also detect GE scans.
   *
   * Revision 1.4  2005/03/13 19:35:11  bert
   * Lots of changes for dealing with some proprietary Philips stuff
   *
   * Revision 1.3  2005/03/03 18:59:15  bert
   * Fix handling of image position so that we work with the older field (0020, 0030) as well as the new (0020, 0032)
   *
   * Revision 1.2  2005/03/02 20:16:24  bert
   * Latest changes and cleanup
   *
   * Revision 1.1  2005/02/17 16:38:10  bert
   * Initial checkin, revised DICOM to MINC converter
   *
   * Revision 1.1.1.1  2003/08/15 19:52:55  leili
   * Leili's dicom server for sonata
   *
   * Revision 1.18  2002/09/26 15:24:33  rhoge
   * Before was only skipping time sort for multi-slice N4 mosaics.  Turns out
   * this was also causing failure on single-slice scans.
   * Changed slices>1 to slices>0 so that now N4 dicom scans never get sorted
   * on their (apparently nonsensical) time value.  The if statement should
   * really be reworked, and should keep an eye on this.  Seems like EPI
   * time series sequences never sort properly on 'time'.
   *
   * Revision 1.17  2002/09/25 17:25:43  rhoge
   * changed public void's to public int's
   *
   * Revision 1.16  2002/05/08 19:32:40  rhoge
   * fixed handling of diffusion scans with separate series for each average
   *
   * Revision 1.15  2002/05/01 21:29:34  rhoge
   * removed MrProt from minc header - encountered files with large strings,
   * causing seg faults
   *
   * Revision 1.14  2002/04/30 12:36:35  rhoge
   * fixes to handle current (and hopefully final) diffusion sequence
   *
   * Revision 1.13  2002/04/26 03:27:03  rhoge
   * fixed MrProt problem - replaced fixed lenght char array with malloc
   *
   * Revision 1.12  2002/04/08 03:40:56  rhoge
   * fixed mosaic extraction for non-square scans and 3D scans.
   * added some new dicom elements
   *
   * Revision 1.11  2002/03/27 19:38:08  rhoge
   * small comment change
   *
   * Revision 1.10  2002/03/27 18:57:50  rhoge
   * added diffusion b value
   *
   * Revision 1.9  2002/03/23 13:17:53  rhoge
   * added support for Bourget network pushed dicom files, cleaned up
   * file check and read_numa4_dicom vr check/assignment
   *
   * Revision 1.8  2002/03/22 19:19:36  rhoge
   * Numerous fixes -
   * - handle Numaris 4 Dicom patient name
   * - option to cleanup input files
   * - command option
   * - list-only option
   * - debug mode
   * - user supplied name, idstr
   * - anonymization
   *
   * Revision 1.7  2002/03/21 13:31:56  rhoge
   * updated comments
   *
   * Revision 1.6  2002/03/19 22:10:16  rhoge
   * removed time sorting for N4DCM mosaics - time is random for mosaics
   *
   * Revision 1.5  2002/03/19 13:13:56  rhoge
   * initial working mosaic support - I think time is scrambled though.
   *
   * Revision 1.4  2001/12/31 18:27:21  rhoge
   * modifications for dicomreader processing of Numaris 4 dicom files - at
   * this point code compiles without warning, but does not deal with
   * mosaiced files.  Also will probably not work at this time for Numaris
   * 3 .ima files.  dicomserver may also not be functional...
   *
   * Revision 1.3  2000/12/14 21:37:11  rhoge
   * log message cleanup
   *
   * Revision 1.2  2000/12/14 21:36:22  rhoge
   * changes to restore measurement loop support that was broken by changes
   * to provide acquisition loop support
   *
   * Revision 1.1.1.1  2000/11/30 02:13:15  rhoge
   * imported sources to CVS repository on amoeba
   * -now support Siemens acquisition loop scans with and without correction
   *  on sending side
   *
   * Revision 6.1  1999/10/29 17:51:59  neelin
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
   @COPYRIGHT : Copyright 1997 Peter Neelin, McConnell Brain Imaging
   Centre, Montreal Neurological Institute, McGill University.
   Permission to use, copy, modify, and distribute this software and
   its documentation for any purpose and without fee is hereby
   granted, provided that the above copyright notice appear in all
   copies.  The author and McGill University make no representations
   about the suitability of this software for any purpose.  It is
   provided "as is" without express or implied warranty.
   ---------------------------------------------------------------------------- */

static const char rcsid[] = "$Header: /private-cvsroot/minc/conversion/dcm2mnc/dicom_to_minc.c,v 1.22 2007-08-13 16:34:52 ilana Exp $";
#include "dcm2mnc.h"

const char *World_Names[WORLD_NDIMS] = { "X", "Y", "Z" };
const char *Volume_Names[VOL_NDIMS] = { "Slice", "Row", "Column" };
const char *Mri_Names[MRI_NDIMS] = {"Slice", "Echo", "Time", "Phase", "ChmSh"};

/* Private structure definitions. */

/* multi-image (mosaic) info */
typedef struct {
    int packed;
    mosaic_seq_t mosaic_seq;
    int size[2];
    int big[2];
    int grid[2];
    int pixel_size;
    Acr_Element big_image;
    Acr_Element small_image;
    int sub_images;
    int slice_count;
    double normal[WORLD_NDIMS];
    double step[WORLD_NDIMS];
    double position[WORLD_NDIMS];
} Mosaic_Info;

/* DICOM Multiframe information.  NOTE: This represents at best a very
 * partial implementation of the DICOM multiframe specification, barely 
 * adequate for conversion of basic 3D files. More work is needed to 
 * support dynamic scans and other more complex objects.
 */
typedef struct {
    int frame_count;
    int frame_size;
    Acr_Element big_image;
    Acr_Element sub_image;
    double normal[WORLD_NDIMS];
    double step[WORLD_NDIMS];
    double position[WORLD_NDIMS];
} Multiframe_Info;    

/* For dicom_to_minc(), these codes specify whether we are dealing
 * with a "normal" DICOM sequence, a Siemens mosaic sequence, or a
 * DICOM multiframe sequence.
 */
#define SUBIMAGE_TYPE_NONE 0
#define SUBIMAGE_TYPE_MOSAIC 1
#define SUBIMAGE_TYPE_MULTIFRAME 2

/* Structure for sorting dimensions */
typedef struct {
   int identifier;
   int original_index;
   double value;
   double width;
} Sort_Element;

/* Private function definitions */
static int mosaic_init(Acr_Group, Mosaic_Info *, int);
static void mosaic_cleanup(Mosaic_Info *);
static int mosaic_insert_subframe(Acr_Group, Mosaic_Info *, int, int);

/* DICOM Multiframe conversion functions (see NOTE: above) */
static void multiframe_init(Acr_Group, Multiframe_Info *, int);
static void multiframe_cleanup(Multiframe_Info *);
static void multiframe_insert_subframe(Acr_Group, Multiframe_Info *, int, int);

static void free_info(General_Info *gi_ptr, File_Info *fi_ptr, 
                      int num_files);
static int dimension_sort_function(const void *v1, const void *v2);
static void sort_dimensions(General_Info *gi_ptr);
static int prot_find_string(Acr_Element Protocol, const char *name,
                            char *value);
static char *dump_protocol_text(Acr_Element Protocol);

/* ----------------------------- MNI Header -----------------------------------
   @NAME       : dicom_to_minc
   @INPUT      : num_files - number of image files
   file_list - list of file names
   minc_file - name of minc file to create, or NULL to make one up.
   clobber - if TRUE, then open the output with NC_CLOBBER
   file_prefix - string providing any directory or prefix 
   for internally generated filename (if it is a directory,
   then it must contain the last "/")
   @OUTPUT     : output_file_name - returns a pointer to an internal area
   containing the file name of the created file if minc_file
   is NULL, or simply a pointer to minc_file. If NULL, then
   nothing is returned.
   @RETURNS    : EXIT_SUCCESS if no error, EXIT_FAILURE on error.
   @DESCRIPTION: Routine to convert a list of Siemens dicom files to minc 
   format.
   @METHOD     : 
   @GLOBALS    :
   @CALLS      : 
   @CREATED    : November 25, 1993 (Peter Neelin)
   @MODIFIED   : 
   ---------------------------------------------------------------------------- */

int
dicom_to_minc(int num_files, 
              const char *file_list[], 
              const char *minc_file,
              int clobber,
              const char *file_prefix, 
              char **output_file_name)
{
    Acr_Group group_list;     /* List of ACR/NEMA groups & elements */
    File_Info *fi_ptr;          /* Array of per-file information */
    General_Info gi;     /* General (common) DICOM file information */
    int max_group;              /* Maximum group number to read */
    Image_Data image;           /* Actual image data */
    int icvid;                  /* MINC Image Conversion Variable */
    int ifile;                  /* File index */
    Mri_Index imri;             /* MRI axis index */
    char *out_file_name;        /* Output MINC filename */
    int isep;                   /* Loop counter */
    const Loop_Type loop_type = NONE; /* MINC loop type always none for now */
    int subimage;               /* Loop counter for MOSAIC images per file */
    int iimage;                 /* Loop counter for all files/images */
    int num_images;             /* Total number of slices (>= # files) */
    Mosaic_Info mi;             /* Mosaic (multi-image) information */
    Multiframe_Info mfi;        /* Multiframe information */
    int n_slices_in_file;       /* Number of slices in file */
    int subimage_type;          /* Subimage type */

    subimage_type = SUBIMAGE_TYPE_NONE;

    /* Allocate space for the file information */
    fi_ptr = malloc(num_files * sizeof(*fi_ptr));
    CHKMEM(fi_ptr);

    num_images = num_files;

    /* Last group needed for first pass
     */
    max_group = ACR_IMAGE_GID - 1;

    /* Add all control characters as numeric array separators to handle 
     * odd behaviour with Siemens dicom files
     */
    for (isep = 0; isep < 31; isep++) {
        acr_element_numeric_array_separator(isep);
    }

    /* Initialize some values for general info */
    gi.initialized = FALSE;
    gi.group_list = NULL;
    for (imri = 0; imri < MRI_NDIMS; imri++) {
        gi.indices[imri] = NULL;
        gi.coordinates[imri] = NULL;
        gi.widths[imri] = NULL;
    }

    /* Loop through file list getting information
     * (note that we have to duplicate the handling
     * of multiple images per file in this loop
     * to accumulate dimension sizes correctly)

     * need separate counter for images, since some files may
     * contain more than one image!
     */
    iimage = 0;

    for (ifile = 0; ifile < num_files; ifile++) {
        if (G.Debug >= HI_LOGGING) {
            printf("\nFile %s\n", file_list[ifile]);
        }

        if (!G.Debug) {
            progress(ifile, num_files, "-Parsing series info");
        }

        /* Read the file
         */
        if (G.file_type == N4DCM) {
            group_list = read_numa4_dicom(file_list[ifile], max_group);
        } 
        else if (G.file_type == IMA) {
            group_list = siemens_to_dicom(file_list[ifile], max_group);
        }

        if (group_list == NULL) {
            fprintf(stderr, "Error parsing file '%s' on 1st pass.\n",
                    file_list[ifile]);
            exit(-1);
        }

        if (G.opts & OPTS_NO_MOSAIC) {
            n_slices_in_file = 1;
        }
        else {
            n_slices_in_file = acr_find_int(group_list, EXT_Slices_in_file, 1);
        }

        /* initialize big and small images, if mosaic
         */
        if (n_slices_in_file > 1) {

            subimage_type = SUBIMAGE_TYPE_MOSAIC;

            mosaic_init(group_list, &mi, FALSE);

            num_images += n_slices_in_file - 1;

            fi_ptr = realloc(fi_ptr, num_images * sizeof(*fi_ptr));
            CHKMEM(fi_ptr);
        }
        else {
            /* See if we have an DICOM multiframe image, or at least a
             * reasonable facsimile of one such as the files produced
             * by the Shimadzu PET scanner.
             * 
             * It is probably not correct to rely on the presence of the
             * "Number of Frames" (0x0028, 0x0008) field, but the Shimadzu
             * multiframe implementation is far from complete and does not
             * seem to implement any of the other components of the 
             * specification.
             */

            n_slices_in_file = acr_find_int(group_list, ACR_Number_of_frames,
                                            1);

            if (n_slices_in_file > 1) {
                subimage_type = SUBIMAGE_TYPE_MULTIFRAME;
            
                multiframe_init(group_list, &mfi, FALSE);

                num_images += n_slices_in_file - 1;
                
                fi_ptr = realloc(fi_ptr, num_images * sizeof(*fi_ptr));
                CHKMEM(fi_ptr);
            }
        }
        

        /* loop over subimages in mosaic
         */
        for (subimage = 0; subimage < n_slices_in_file; subimage++) {

            /* Modify the group list for this image if mosaic or multiframe
             */
            switch (subimage_type) {
            case SUBIMAGE_TYPE_MOSAIC:
                mosaic_insert_subframe(group_list, &mi, subimage, FALSE);
                break;
            case SUBIMAGE_TYPE_MULTIFRAME:
                multiframe_insert_subframe(group_list, &mfi, subimage, FALSE);
                break;
            default:
                break;
            }

            /* Get file-specific information
             */
            get_file_info(group_list, &fi_ptr[iimage], &gi);
	    
	    //ilana debug
	    /*int acq=acr_find_int(group_list, ACR_Acquisition, 1);
	    printf("****WE HAVE ACQUISITION# %i",acq);*/

            /* increment iimage here
             */
            iimage++;
        }

        /* Delete the group list
         */
        acr_delete_group_list(group_list);

        /* cleanup mosaic struct if used
         */
        switch (subimage_type) {
        case SUBIMAGE_TYPE_MOSAIC:
            mosaic_cleanup(&mi);
            break;

        case SUBIMAGE_TYPE_MULTIFRAME:
            multiframe_cleanup(&mfi);
            break;

        default:
            break;
        }
    }

    /* Sort the dimensions */
    sort_dimensions(&gi);

    /* Create the output file
     */
    if (gi.initialized) {
        icvid = create_minc_file(minc_file,
                                 clobber,
                                 &gi,
                                 file_prefix,
                                 &out_file_name,
                                 loop_type);
    }
    if (output_file_name != NULL) {
        *output_file_name = out_file_name;
    }

    /* Check that we found the general info and that the minc file was
     * created okay
     */
    if ((!gi.initialized) || (icvid == MI_ERROR)) {
        if (gi.initialized) {
            fprintf(stderr, "Error creating MINC file %s.\n", out_file_name);
        }
        free_info(&gi, fi_ptr, num_files);
        free(fi_ptr);
        return EXIT_FAILURE;
    }

    if (G.Debug) {
        printf("Writing %d images to MINC file\n", num_files);
    }	  

    /* Last group needed for second pass
     * we now have to read up to and including the image, 
     * since image pointers are needed in mosaic_init
     */
    max_group = ACR_IMAGE_GID;

    /* Loop through the files again and put images into the minc file
     */
    iimage = 0;
    for (ifile = 0; ifile < num_files; ifile++) {

        if (!G.Debug) {
            progress(ifile, num_files, "-Creating minc file");
        }

        /* Check that we have a valid file 
         */
        if (!fi_ptr[ifile].valid) {
            printf("WARNING: file %s was marked invalid\n", 
                   file_list[ifile]);
            continue;
        }
     
        /* Read the file 
         */
        if (G.file_type == N4DCM) {
            group_list = read_numa4_dicom(file_list[ifile], max_group);
        }
        else if (G.file_type == IMA) {
            group_list = siemens_to_dicom(file_list[ifile], max_group);
        }
       
        if (group_list == NULL) {
            fprintf(stderr, "Error parsing file '%s' during 2nd pass.\n",
                    file_list[ifile]);
            exit(-1);
        }

        /* initialize big and small images, if mosaic
         */
        switch (subimage_type) {
        case SUBIMAGE_TYPE_MOSAIC:
            mosaic_init(group_list, &mi, TRUE);
            break;

        case SUBIMAGE_TYPE_MULTIFRAME:
            multiframe_init(group_list, &mfi, TRUE);
            break;

        default:
            break;
        }

        /* loop over subimages in mosaic
         */
        for (subimage = 0; subimage < n_slices_in_file; subimage++) {

            /* Modify the group list for this image if mosaic
             */
            switch (subimage_type) {
            case SUBIMAGE_TYPE_MOSAIC:
                mosaic_insert_subframe(group_list, &mi, subimage, TRUE);
                break;

            case SUBIMAGE_TYPE_MULTIFRAME:
                multiframe_insert_subframe(group_list, &mfi, subimage, TRUE);
                break;

            default:
                break;
            }
       
            /* Get image
             */
            get_dicom_image_data(group_list, &image);
       
            /* Save the image and any other information
             */
            save_minc_image(icvid, &gi, &fi_ptr[iimage], &image);

            /* Free the image data */
            if (image.data != NULL) {
                free(image.data);
                image.data = NULL;
            }

            /* increment image counter
             */
            iimage++;
        }
     
        /* Delete the group list
         */
        acr_delete_group_list(group_list);

        /* cleanup mosaic struct if used
         */
        switch (subimage_type) {
        case SUBIMAGE_TYPE_MOSAIC:
            mosaic_cleanup(&mi);
            break;

        case SUBIMAGE_TYPE_MULTIFRAME:
            multiframe_cleanup(&mfi);
            break;

        default:
            break;
        }
    }

    /* Close the output file */
    close_minc_file(icvid);

    /* Free the gi and fi_ptr stuff */
    free_info(&gi, fi_ptr, num_files);
    free(fi_ptr);

    return EXIT_SUCCESS;
}

/* ----------------------------- MNI Header -----------------------------------
   @NAME       : read_std_dicom
   @INPUT      : filename - name of siemens Numaris 4 `dicom' file to read
                 max_group - maximum group number to read
   @OUTPUT     : (none)
   @RETURNS    : group list read in from file
   @DESCRIPTION: Routine to read in a group list from a file.
   @METHOD     : 
   @GLOBALS    : 
   @CALLS      : 
   @CREATED    : December 18, 2001 (Rick Hoge)
   @MODIFIED   : 
   ---------------------------------------------------------------------------- */

Acr_Group
read_std_dicom(const char *filename, int max_group)
{
    FILE *fp;
    Acr_File *afp;
    Acr_Group group_list;
    int status;

    /* Open the file
     */
    fp = fopen(filename, "rb");
    if (fp == NULL) {
        return NULL;
    }

    /* Connect to input stream
     */
    afp = acr_file_initialize(fp, 0, acr_stdio_read);
    if (afp == NULL) {
        return NULL;
    }

    acr_set_ignore_errors(afp, 1); /* ignore protocol errors */

    if (acr_test_dicom_file(afp) != ACR_OK) {
        return NULL;
    }

    // Read in group list
    status = acr_input_group_list(afp, &group_list, max_group);
    if (status != ACR_END_OF_INPUT && status != ACR_OK) {
        return NULL;
    }

    // Close the file
    acr_file_free(afp);
    fclose(fp);

    return (group_list);
}

/* Return non-zero if this image looks like it is a mosaic image.
 */
int
is_siemens_mosaic(Acr_Group group_list)
{
    char *str_ptr;
    int num_slices;

    str_ptr = acr_find_string(group_list, ACR_Image_type, "");
    if (strstr(str_ptr, "MOSAIC") != NULL)
        return 1;               /* slam dunk, this is mosaic data */

    /* For some reason, some Localizer scans look like MOSAICs in that
     * they appear to be upsampled in such a way that their
     * rows/columns are larger than the acquisition matrix. The
     * details of this are probably buried in the Siemens ASCCONV dump
     * and therefore I have not been able to figure out how to tweak
     * the MOSAIC stuff to get this right. Instead I am taking the
     * cheesy way out and declaring that anything that is NOT
     * explictly marked MOSAIC, and IS marked a localizer, should not
     * be treated as a mosaic.
     */
    str_ptr = acr_find_string(group_list, ACR_Series_description, "");
    if (strstr(str_ptr, "localizer") || strstr(str_ptr, "LOCALIZER")) {
        return 0;
    }

    /* OK, we did not find the word "mosaic" in the image type.  But this
     * could still be a mosaic image.  Let's look for some other clues.
     */
    if (acr_find_int(group_list, SPI_Number_of_slices_nominal, 0) > 1)
        return 1;               /* probably mosaic. */

    if (acr_find_int(group_list, SPI_Number_of_3D_raw_partitions_nominal, 0) > 1)
        return 1;               /* probably mosaic */

    return 0;                   /* probably not mosaic */
}

#define MAXVM 32

Acr_Group
parse_siemens_proto2(Acr_Group group_list, Acr_Element element)
{
    int byte_cnt;
    int byte_pos;
    unsigned char *byte_ptr;
    int n_items;
    int n_values;
    int vm;
    char vr[4];
    int syngodt;
    int i;
    char name[64];
    char *value[MAXVM];
    int len;

    byte_cnt = element->data_length;
    byte_ptr = element->data_pointer;
    byte_pos = 0;

    /* See if the magic 8-byte header is present. If not, we start
     * right off with the number of elements.
     */
    if (byte_ptr[0] == 'S' && byte_ptr[1] == 'V' &&
        byte_ptr[2] == '1' && byte_ptr[3] == '0') {
        byte_pos += 8;          /* Skip header */
    }

    acr_get_long(ACR_LITTLE_ENDIAN, 1, byte_ptr + byte_pos, &n_items);
    byte_pos += 8;              /* Skip # items and 4 bytes of unknown junk */

    while (n_items-- > 0) {
        strncpy(name, (byte_ptr + byte_pos), 64);
        byte_pos += 64;

        acr_get_long(ACR_LITTLE_ENDIAN, 1, byte_ptr + byte_pos, &vm);
        byte_pos += 4;

        strncpy(vr, (byte_ptr + byte_pos), 4);
        byte_pos += 4;

        acr_get_long(ACR_LITTLE_ENDIAN, 1, byte_ptr + byte_pos, &syngodt);
        byte_pos += 4;

        acr_get_long(ACR_LITTLE_ENDIAN, 1, byte_ptr + byte_pos, &n_values);
        byte_pos += 4;

        byte_pos += 4;          /* skip dummy */

        if (n_values > 0) {
            for (i = 0; i < n_values; i++) {
                byte_pos += 4;      /* skip */

                acr_get_long(ACR_LITTLE_ENDIAN, 1, byte_ptr + byte_pos, &len);

                byte_pos += 4;

                byte_pos += 8;

                if (i < vm && i < MAXVM) {
                    value[i] = (byte_ptr + byte_pos);
                }

                byte_pos += ((len + 3) / 4) * 4;
            }
        }

        if (!strcmp(name, "DiffusionDirectionality")) {
            acr_insert_string(&group_list, ACR_Diffusion_directionality,
                              value[0]);
        }
        else if (!strcmp(name, "B_value")) {
		
            /*double tmp = atof(value[0]); this atof makes the value null!  ilana*/
	     double tmp = atoi(value[0]);
	     /*need a hack for ICBM scan, see below ilana*/
            acr_insert_double(&group_list, ACR_Diffusion_b_value, 1, &tmp); 
        }
        else if (!strcmp(name, "DiffusionGradientDirection")) {
            double tmp[3];
            if (vm == 3 && n_values >= vm) {
		/*For the ICBM WIP scan, the b0 images do not have
		B_value=0 or DiffusionGradientDirection=0 0 0, they
		actually have DiffusionGradientDirection=-1.00010000 -1.00010000 -1.00010000
		Use this to detect the b=0 images and hopefully this doesn't ever correspond
		to a real gradient direction (have to change bvalues correspondingly)!   ilana*/
		
		if(!strcmp(value[0],"-1.00010000") && !strcmp(value[1],"-1.00010000")  && !strcmp(value[2],"-1.00010000")){
			value[0] = value[1] = value[2] = "0"; /*grad directions should be 0*/
			double tmp2 = atoi("0");
			acr_insert_double(&group_list, ACR_Diffusion_b_value, 1, &tmp2); /*also have to modify B values ilana*/
		}
		                  
		tmp[0] = atof(value[0]);
                tmp[1] = atof(value[1]);
                tmp[2] = -1*atof(value[2]); /*dicom z = -minc z ilana*/
		
		
                acr_insert_double(&group_list, 
                                  ACR_Diffusion_gradient_orientation,
                                  3,
                                  tmp);
            }
        }

#if 0
        if (G.Debug >= HI_LOGGING) {
            printf("%s VM=%d VR=%s %d ", name, vm, vr, n_values);

            if (n_values != 0) {
                for (i = 0; i < vm && i < MAXVM; i++) {
                    printf("%s ", value[i]);
                }
            }
            printf("\n");
        }
#endif
    }
    return (group_list);
}

static Acr_Group 
add_siemens_info(Acr_Group group_list)
{
    /* needed for group repair - some essential info
     * only available in ascii dump of MrProt structure
     */
    Acr_Element protocol; 
    Acr_Element element;
    int num_slices, num_partitions;
    string_t str_buf, str_buf2; 
    char *str_ptr=NULL;
    char *str_ptr2=NULL;
    int interpolation_flag;
    int enc_ix, num_encodings, num_b0; /*added by ilana*/
    char DiffSeqName[50];
    int i=0;

    element = acr_find_group_element(group_list, SPI_Protocol2);
    if (element != NULL) {
        group_list = parse_siemens_proto2(group_list, element);
    }

    /* now fix the group list to provide essential info
     * via standard dicom elements
     * (this lets the rest of the code be more reusable)

     * Note that these parameters are mostly dimension lengths,
     * and are not usually supplied in standard DICOM implementations.
     * We could do without them, except that the use of mosaics for
     * multi-slice data makes at least the number of slices necessary.
     * For such elements, we will spoof our own `private' entries
     * using Numaris 3.5 coordinates.  These should not be used elsewhere
     * in the code unless there's no other way!  Basically things
     * should be done as follows:

     * 1) use actual element in public dicom group, if possible
     * 2) if not, then get info from MrProt and insert as 
     *    correct public dicom element
     * 3) if no public element exists, use an SPI element (careful!)
     * 4) if no SPI element exists, use and EXT element (careful!)
     */

    /* read in Protocol group */

    protocol = acr_find_group_element(group_list, SPI_Protocol);
    if (protocol != NULL) {
        /* parse_siemens_junk(protocol); */

        if (G.Debug >= HI_LOGGING) {
            printf("Incorporating Siemens protocol structure...\n");
        }

        /* Add number of dynamic scans:
         */
        prot_find_string(protocol, "lRepetitions", str_buf);
        acr_insert_numeric(&group_list, ACR_Acquisitions_in_series, 
                           atoi(str_buf) + 1);

        /* add number of echoes:
         */
        prot_find_string(protocol, "lContrasts", str_buf);
        acr_insert_numeric(&group_list, SPI_Number_of_echoes, atoi(str_buf));

        /* Add receiving coil (for some reason this isn't in generic groups)
         */
        prot_find_string(protocol,
                         "sCOIL_SELECT_MEAS.asList[0].sCoilElementID.tCoilID",
                         str_buf);
        acr_insert_string(&group_list, ACR_Receive_coil_name, str_buf);

        /* add MrProt dump
         */
        str_ptr = dump_protocol_text(protocol);
        acr_insert_string(&group_list, EXT_MrProt_dump, str_ptr);
        free(str_ptr);

        /* add number of slices: (called `Partitions' for 3D) */
        prot_find_string(protocol, "sSliceArray.lSize", str_buf);
        num_slices = atoi(str_buf);
        
        prot_find_string(protocol, "sKSpace.lPartitions", str_buf);
        num_partitions = atoi(str_buf);

        /* This is a hack based upon the observation that for at least some
         * conversions, this value seems to give the true number of slices
         * rather than the sKSpace.lPartitions value (bert)
         */
        prot_find_string(protocol, "sKSpace.lImagesPerSlab", str_buf);
        if (str_buf[0] != '\0') {
            int num_images_per_slab = atoi(str_buf);
            if (num_images_per_slab > num_partitions) {
                num_partitions = num_images_per_slab;
            }
        }
        
        /* NOTE:  for some reason, lPartitions > 1 even for 2D scans
         * (e.g. EPI, scouts)
         */
        if (!strncmp(acr_find_string(group_list, ACR_MR_acquisition_type,""),
                     "3D", 2)) {
            /* Use partitions if 3D.
             * (note that this gets more complicated if the 3D scan
             *  is mosaiced - see below)
             */
            acr_insert_numeric(&group_list, SPI_Number_of_slices_nominal, 1);
            acr_insert_numeric(&group_list, 
                               SPI_Number_of_3D_raw_partitions_nominal, 
                               num_partitions);
        } 
        else {
            /* use slices for 2D
             */
            acr_insert_numeric(&group_list, SPI_Number_of_slices_nominal, 
                               num_slices);
            acr_insert_numeric(&group_list, 
                               SPI_Number_of_3D_raw_partitions_nominal, 1);
        }

        /* See if the field of view is set, if not, see if we can find
         * it in the ASCCONV dump and copy it into the proprietary
         * fields.
         */
        if (acr_find_group_element(group_list, SPI_Field_of_view) == NULL) {
            /* TODO: This assumes a symmetric field of view.  We need to
             * figure out what to do if this is not true!!
             */
            prot_find_string(protocol, "sSliceArray.asSlice[0].sReadoutFOV",
                             str_buf);
            if (str_buf[0] != '\0') {
                int fov = atoi(str_buf);

                sprintf(str_buf, "%d\\%d", fov, fov);
                acr_insert_string(&group_list, SPI_Field_of_view, str_buf);
            }
        }

        prot_find_string(protocol, "sKSpace.uc2DInterpolation", str_buf);
        interpolation_flag = strtol(str_buf, NULL, 0);

	/*find Delay time in TR in ASCONV header ilana*/
	prot_find_string(protocol, "lDelayTimeInTR", str_buf);
	acr_insert_numeric(&group_list, EXT_Delay_in_TR,
                               atol(str_buf));
	
	/*Find slice acquisition mode in ASCONV header, 
	can't seem to find is in a standard dicom field
	sSliceArray.ucMode takes on 3 values:
	0x1 means ASCENDING
	0x2 means DESCENDING
	0x4 means INTERLEAVED*/
	prot_find_string(protocol, "sSliceArray.ucMode", str_buf);
	acr_insert_string(&group_list,EXT_Slice_order,str_buf);

			
        /*Modified by ilana to handle the common types of diffusion scans (ref: siemens_dicom_to_minc for dicomserver)
	
	/* correct dynamic scan info if *MGH* diffusion scan:
         *
         * assumptions:
         *
         *  - diffusion protocol indicated by sDiffusion.ulMode = 0x100
	 *  - bvalue is in sDiffusionalBValue[1]
         *  - there are 10 b=0 scans and a user defined number of diffusion directions
         *  - b=0 scans have sequence name "ep_b0#0, ep_b0#1... etc"
         *  - encoded scans have seq names "ep_b1000#1, ep_b1000#2, ...,ep_b1300#1, ep_b1300#2, ..." etc.
         *
         * actions:
         * 
         *  - change number of dynamic scans to sDiffusion.lDiffDirections  + num b0 images
	 *  - use sWiPMemBlock.alFree[8] for number of b=0 scans
         *  - modify dynamic scan index to encoding index
         */
	 /* correct dynamic scan info if standard *ep2d_diff* diffusion scan
         *
         * assumptions:
         *
         *  - diffusion protocol indicated by sDiffusion.ulMode = 0x100
	 *  - bvalue is in sDiffusionalBValue[0]
         *  - there is 1 b=0 scans and 12 diffusion directions
         *  - b=0 scan havs sequence name "ep_b0"
         *  - encoded scans have seq names "ep_b1000#1, ep_b1000#2, ..." etc.
         *
         * actions:
         * 
         *  - change number of dynamic scans to sDiffusion.lDiffDirections + num b0 images
         *  - modify dynamic scan index to encoding index
         */
        /* correct dynamic scan info if standard *ICBM_WIP* diffusion scan
         *
         * assumptions:
         *
         *  - diffusion protocol indicated by sDiffusion.ulMode = 0x80
	 *  - bvalue is in sDiffusionalBValue[1]
         *  - there is 1 b=0 scans and user defined number of diffusion directions
         *  - b=0 scan has sequence name "ep_b0"
         *  - encoded scans have seq names "ep_b1000#1, ep_b1000#2, ..." etc.
         *
         * actions:
         * 
         *  - change number of dynamic scans to sDiffusion.lDiffDirections + num b0 images
         *  - modify dynamic scan index to encoding index
         */

	prot_find_string(protocol, "sDiffusion.ulMode", str_buf);
	if (!strcmp(str_buf, "0x100") | !strcmp(str_buf, "0x80")) { 
          /*we have a diffusion scan; flag it*/
          acr_insert_string(&group_list, ACR_Acquisition_contrast, "DIFFUSION");
	  /*----MGH-----*/
	  prot_find_string(protocol,"sWiPMemBlock.alFree[8]", str_buf);
	  if ((atoi ((char*)str_buf))!= 0 ) { /*num b0 images for MGH sequence*/
            
	    /* get number of b=0 images*/
            num_b0 = atoi ((char*)str_buf);
		  
	    /* try to get b value */
            prot_find_string(protocol, "sDiffusion.alBValue[1]", str_buf);

	    acr_insert_numeric(&group_list, EXT_Diffusion_b_value,
                               atoi(str_buf));


          }
	  else
	  {
		prot_find_string(protocol, "sDiffusion.ulMode", str_buf);
	  	/*-----ep2d_diff-----OR-----
		ep2d_diff_WIP_ICBM with "Diffusion mode"=MDDW & DiffusionWeightings=2-----------*/
		if (!strcmp(str_buf, "0x100")) { 
	            	/* try to get b value */
        	    	prot_find_string(protocol, "sDiffusion.alBValue[1]", str_buf);
            		acr_insert_numeric(&group_list, EXT_Diffusion_b_value,
                               atoi(str_buf));
		    	num_b0=1;
	  
          	}
	   	/*-----ICBM_WIP  with "Diffusion mode"=Free (5 b=0 scans) -----*/
	  	else if(!strcmp(str_buf, "0x80")) {
        	    	/* try to get b value */
            		prot_find_string(protocol, "sDiffusion.alBValue[0]", str_buf);
	            	acr_insert_numeric(&group_list, EXT_Diffusion_b_value,
                               atoi(str_buf));
		    	num_b0=0; 
			/*there are 5 b=0 scans but they are not identified 
			any differently than the diffusion weighted images, 
			sDiffusion.lDiffDirections includes the b=0 images*/	  
          	}
	   }  
	    
          /* if all averages in one series: */
	  prot_find_string(protocol,"ucDixon",str_buf);
          if (!strcmp(str_buf,"0x1")) {
            prot_find_string(protocol, "sDiffusion.lDiffDirections", str_buf);
            num_encodings = atoi ((char*)str_buf) + num_b0;
             
	    /* number of 'time points' */
            acr_insert_numeric(&group_list, ACR_Acquisitions_in_series, 
                                     num_encodings *
                                     acr_find_double(group_list,
                                                     ACR_Nr_of_averages, 1));

            /* time index of current scan: */

            /* For multi-series scans, we DO USE THIS BECAUSE global
            * image number may be broken!!
            */
	    
	    /*Have to also take care of numbered b=0 images (ep_b0#0, etc...) ilana*/
	    /*the Siemems-based sequences, in MDDW mode with 2 diff weightings have b=0 images called ep_b0*/

	    str_ptr = strstr(acr_find_string(group_list, ACR_Sequence_name, ""), "b");
	    str_ptr2 = strstr(acr_find_string(group_list, ACR_Sequence_name, ""), "#");
			    
            if (str_ptr == NULL || str_ptr2 == NULL) {
                enc_ix = 0;
            }
	    else if(atoi(str_ptr + sizeof(char)) == 0){ /*a 0 after the b means b=0 image*/
			enc_ix = atoi(str_ptr2 + sizeof(char));
			}
	    else{
		enc_ix = atoi(str_ptr2 + sizeof(char)) + num_b0; /*should be in diffusion weighted images now*/
            }
	  
	    /* however with the current sequence, we get usable
            * time indices from floor(global_image_num/num_slices)*/ /*i'm not sure that works here ilana*/
	    
	    acr_insert_numeric(&group_list, ACR_Acquisition, enc_ix);
            /*acr_insert_numeric(&group_list, ACR_Acquisition, 
                                   (acr_find_int(group_list, ACR_Image, 1)-1) / 
                                   num_slices);*/

          } 
          else { /* averages in different series - no special handling needed? */
	    
            prot_find_string(protocol, "sDiffusion.lDiffDirections", str_buf);
            num_encodings = atoi((char*)str_buf) + num_b0; 
            //num_encodings = 7; /* for now assume 7 shots in diffusion scan */

             /* number of 'time points' */
             acr_insert_numeric(&group_list, ACR_Acquisitions_in_series,
                                   num_encodings);
                
                /* For multi-series scans, we DO USE THIS BECAUSE global
                 * image number may be broken!!
                 */
	    /*Have to also take care of numbered b=0 images (ep_b0#0, etc...) ilana*/
	    
	    str_ptr = strstr(acr_find_string(group_list, ACR_Sequence_name, ""), "b");
	    str_ptr2 = strstr(acr_find_string(group_list, ACR_Sequence_name, ""), "#");
	    
            if (str_ptr == NULL || str_ptr2 == NULL){
                enc_ix = 0;
            } 
            else if(atoi(str_ptr + sizeof(char)) == 0){ /*a 0 after the b means b=0 image*/
		enc_ix = atoi(str_ptr2 + sizeof(char));
	    }
	    else{
		enc_ix = atoi(str_ptr2 + sizeof(char)) + num_b0; /*should be in diffusion weighted images now*/
            }
                acr_insert_numeric(&group_list, ACR_Acquisition, enc_ix);
          }
	  
	   /* BUG! TODO! FIXME!  In dcm2mnc.c the sequence name is
             * used as one of the criteria for starting a new
             * file. For a DTI sequence, we don't want this to
             * happen. For now I replace the sequence name with a
             * bogus value in order to keep the DTI scan from being
             * split into pieces.  This isn't right. We should deal
             * with this in a more graceful way.
             */
            acr_insert_string(&group_list, ACR_Sequence_name, "DTI");

            /* Reset acquisition time to series time plus the series
             * index.  Otherwise each slice may get its own
             * time. This is also probably wrong and in the ideal
             * world we should deal with this gracefully. But we have
             * no good way of storing per-slice timing information
             * right now.
             */
            acr_insert_numeric(&group_list, ACR_Acquisition_time, acr_find_double(group_list, ACR_Series_time, 0) + enc_ix);
        /* end of diffusion scan handling */


        /* There is another whole class of (probably newer) diffusion
         * weighted images that use a very different arrangement and
         * need better handling.
         */
        
    }  
} 
    else {
        /* If no protocol dump was found we have to assume no
         * interpolation since at the momemnt I have no idea where it
         * would live...
         */
        interpolation_flag = 0;
        num_partitions = acr_find_int(group_list, 
                                      SPI_Number_of_3D_raw_partitions_nominal, 0);
        num_slices = acr_find_int(group_list, SPI_Number_of_slices_nominal, 0);
    }

    /* Handle mosaic images if necessary.
     */
    if (is_siemens_mosaic(group_list)) {
        
        int mosaic_rows, mosaic_cols;
        short subimage_size[4];
        int subimage_rows, subimage_cols;

        /* Now figure out mosaic rows and columns, and put in EXT
         * shadow group. Check for interpolation - will require 2x
         * scaling of rows and columns.
         */

        /* Assign defaults in case something goes wrong below...
         */
        subimage_rows = subimage_cols = acr_find_int(group_list, 
                                                     SPI_Base_raw_matrix_size, 
                                                     0);

        /* If we don't have an "SPI_Base_raw_matrix_size" field, try using
         * the acquisition matrix.
         */
        if (subimage_rows == 0) {

            /* Compute mosaic rows and columns
             * Here is a hack to handle non-square mosaiced images
             *
             * WARNING: as far as I can tell, the phase-encoding dir (row/col)
             * is reversed for mosaic EPI scans (don't know if this is a
             * mosaic thing, an EPI thing, or whatever). 
	     * (Something is wrong here, it seems that it's not reversed, but not sure
	     * obviously doesn't show up when the acquisition matrix is square ilana)
             * Get the array of sizes: freq row/freq col/phase row/phase col
             */

            element = acr_find_group_element(group_list, 
                                             ACR_Acquisition_matrix);
            if (element == NULL) {
                printf("WARNING: Can't find acquisition matrix\n");
            }
            else if (acr_get_element_short_array(element, 4, 
                                                 subimage_size) != 4) {
                printf("WARNING: Can't read acquisition matrix\n");
            }
            else {
                if (G.Debug >= HI_LOGGING) {
                    printf(" * Acquisition matrix %d %d %d %d\n",
                           subimage_size[0],
                           subimage_size[1],
                           subimage_size[2],
                           subimage_size[3]);
                }
                /* Get subimage dimensions, assuming the OPPOSITE of the
                 * reported phase-encode direction!!
                 */
                str_ptr = acr_find_string(group_list, 
                                          ACR_Phase_encoding_direction,
                                          "");
                if (!strncmp(str_ptr, "COL", 3)) { /*TODO test that this is right for non-square acquisition matrices ilana*/
                    subimage_rows = subimage_size[3];
                    subimage_cols = subimage_size[0];
                }
                else if (!strncmp(str_ptr, "ROW", 3)) {
		/*not sure the subimage dimensions should be opposite of the phase encoding direction, 
		running into some problems, so I changed it to be the same
		but I also might be breaking for other cases! ilana*/
                    /*subimage_rows = subimage_size[2];
                    subimage_cols = subimage_size[1];*/
		    subimage_rows = subimage_size[1];
                    subimage_cols = subimage_size[2];
			
                }
                else {
                    printf("WARNING: Unknown phase encoding direction '%s'\n",
                           str_ptr);
                }

                /* If interpolation, multiply rows and columns by 2 */
                if (interpolation_flag) {
                    subimage_rows *= 2;
                    subimage_cols *= 2;
                }
            }
        }

        /* If these are not set or still zero, assume this is NOT a mosaic
         * format file.
         */
        if (subimage_rows == 0 || subimage_cols == 0) {
            subimage_rows = acr_find_int(group_list, ACR_Rows, 1);
            subimage_cols = acr_find_int(group_list, ACR_Columns, 1);
            mosaic_rows = 1;
            mosaic_cols = 1;
        }
        else {
            if (G.Debug >= HI_LOGGING) {
                printf("Assuming %dx%d mosaic\n", subimage_rows, subimage_cols);
            }
            mosaic_rows = acr_find_int(group_list, ACR_Rows, 1) / subimage_rows;
            mosaic_cols = acr_find_int(group_list, ACR_Columns, 1) / subimage_cols;
        }

        acr_insert_numeric(&group_list, EXT_Mosaic_rows, mosaic_rows);
        acr_insert_numeric(&group_list, EXT_Mosaic_columns, mosaic_cols);

        if (mosaic_rows * mosaic_cols > 1) {

            str_ptr = acr_find_string(group_list, ACR_MR_acquisition_type, "");
             
            /* assume any mosaiced file contains all slices
             * (we now support mosaics for 2D and 3D acquisitions,
             *  so we may need to use partitions instead of slices)
             */

            if (!strncmp(str_ptr, "2D", 2)) {
                acr_insert_numeric(&group_list, EXT_Slices_in_file, num_slices);
                acr_insert_numeric(&group_list, 
                                   SPI_Number_of_slices_nominal, num_slices);
            } 

            /* if 3D mosaiced scan, write number of partitions to number of 
             * slices in dicom group 
             */
            else if (!strncmp(str_ptr, "3D", 2)) {
                acr_insert_numeric(&group_list, EXT_Slices_in_file, 
                                   num_partitions);
                acr_insert_numeric(&group_list, SPI_Number_of_slices_nominal, 
                                   num_partitions);
                /* also have to provide slice spacing - in case of 3D it's same
                 * as slice thickness (and not provided in dicom header!)
                 */
                acr_insert_numeric(&group_list, ACR_Spacing_between_slices,
                                   acr_find_double(group_list, 
                                                   ACR_Slice_thickness, 1.0));
            }
        } 
        else {
            acr_insert_numeric(&group_list, EXT_Slices_in_file, 1);
        }

        /* Correct the rows and columns values -
         * These will reflect those of the subimages in the mosaics
         * NOT the total image dimensions
         */
        acr_insert_short(&group_list, EXT_Sub_image_columns, subimage_cols);
        acr_insert_short(&group_list, EXT_Sub_image_rows, subimage_rows);

        /* TODO: should also correct the image position here? */
    }

    /* Hacks for specific numaris3 sequences.
     */
    if (is_numaris3(group_list)) {
        int tmp;

        str_ptr = acr_find_string(group_list, ACR_Sequence_name, "");

        /* This first hack is for Sebastian's Numaris-3 DTI scans.
         */
        if (!strcmp(str_ptr, "ep_d33a ")) {
            /* Numaris-3 DTI scan.  Individual scans are identified by
             * echo number.  We need to change echos to time.
             */
            tmp = acr_find_int(group_list, SPI_Number_of_echoes, 1);
            acr_insert_numeric(&group_list, SPI_Number_of_echoes, 1);

            element = acr_find_group_element(group_list,
                                             ACR_Acquisitions_in_series);
            if (element != NULL) {
                int grp_id = acr_get_element_group(element);
                int elm_id = acr_get_element_element(element);
                acr_group_remove_element(acr_find_group(group_list, grp_id),
                                        elm_id);
            }
            acr_insert_short(&group_list, ACR_Number_of_time_slices, tmp);

            tmp = acr_find_int(group_list, ACR_Echo_number, 0);
            acr_insert_numeric(&group_list, ACR_Echo_number, 1);
            acr_insert_numeric(&group_list, ACR_Frame_reference_time, 
                               tmp * 1000);

            /* Just use defaults for these values. */
            acr_insert_numeric(&group_list, ACR_Actual_frame_duration, 1000);
        }

        /* An additional hack required for Sebastian's Numaris-3 FMRI scans.
         */
        if (!strcmp(str_ptr, "ep_fid")) {
            /* Numaris-3 FMRI scan.  Each time slice uses a different 
             * series number!!
             */
            tmp = acr_find_int(group_list, ACR_Series_time, 0);
            acr_insert_numeric(&group_list, ACR_Series, tmp);

            tmp = acr_find_int(group_list, ACR_Acquisitions_in_series, 0);
            acr_insert_short(&group_list, ACR_Number_of_time_slices, tmp);

            element = acr_find_group_element(group_list,
                                             ACR_Acquisitions_in_series);
            if (element != NULL) {
                int grp_id = acr_get_element_group(element);
                int elm_id = acr_get_element_element(element);
                acr_group_remove_element(acr_find_group(group_list, grp_id),
                                        elm_id);
            }
        }
    }
    return (group_list);
}

#define PMS_SET_CREATOR(el, cr) \
     (el)->element_id = ((el)->element_id & 0xff) + ((cr)->element_id << 8)

static Acr_Group
add_philips_info(Acr_Group group_list)
{
    struct Acr_Element_Id creator_id;
    Acr_Group pms_group;
    Acr_Element pms_element;
    Acr_Element pms_element_list;
    char *str_ptr;
    int slice_count;
    int slice_index;
    char str_buf[128];

    /* To use the Philips proprietary group, we have to figure out the
     * DICOM private creator ID in use.  The group ID is always 0x2001,
     * but the upper eight bits of the element ID are somewhat variable.
     * To tell what they are, we have to search through the element ID's
     * from 0x0001-0x00ff and find one that contains the text
     * "PHILIPS IMAGING DD 001" or "Philips Imaging DD 001".  The value
     * of this element is then used for the upper eight bits of all
     * subsequent Philips private element ID's.
     */
    pms_group = acr_find_group(group_list, PMS_PRIVATE_GROUP_ID);
    if (pms_group != NULL) {
        pms_element_list = acr_get_group_element_list(pms_group);
        creator_id.group_id = PMS_PRIVATE_GROUP_ID;
        creator_id.vr_code = ACR_VR_LO;
        for (creator_id.element_id = 0x0001; 
             creator_id.element_id <= 0x00ff; 
             creator_id.element_id++) {
            pms_element = acr_find_element_id(pms_element_list, &creator_id);
            if (pms_element != NULL) {
                str_ptr = acr_get_element_string(pms_element);
                if (str_ptr != NULL && 
                    (!strcmp(str_ptr, "Philips Imaging DD 001") ||
                     !strcmp(str_ptr, "PHILIPS IMAGING DD 001"))) {
                    /* Found it!!! */
                    break;
                }
            }
        }
    }
    else {
        creator_id.element_id = 0;
    }
    if (creator_id.element_id > 0xff || creator_id.element_id < 0x01) {
        printf("WARNING: Can't find Philips private creator ID.\n");

        /* OK, this may be an old Philips file with the SPI stuff in it.
         */
        str_ptr = acr_find_string(group_list, SPI_PMS_grp19_tag, "");
        if (!strncmp(str_ptr, "PHILIPS MR", 10)) {
            if (G.Debug >= HI_LOGGING) {
                printf("Found Philips SPI information\n");
            }

            str_ptr = acr_find_string(group_list, ACR_Image_position_patient, "");
            if (*str_ptr == '\0') {
                double x, y, z;

                x = acr_find_double(group_list, SPI_PMS_lr_position2, 0);
                y = acr_find_double(group_list, SPI_PMS_ap_position2, 0);
                z = acr_find_double(group_list, SPI_PMS_cc_position2, 0);
                sprintf(str_buf, "%.15g\\%.15g\\%.15g", x, y, z);
                printf("New position: %s\n", str_buf);
                acr_insert_string(&group_list, ACR_Image_position_patient, 
                                  str_buf);

            }
            str_ptr = acr_find_string(group_list, ACR_Number_of_slices, "");
            if (*str_ptr == '\0') {
                short n;
                n = acr_find_short(group_list, SPI_PMS_slice_count, 0);
                acr_insert_short(&group_list, ACR_Number_of_slices, n);
            }
            str_ptr = acr_find_string(group_list, 
                                      ACR_Image_orientation_patient, "");
            if (strchr(str_ptr, '\\') == NULL) {
                int orientation = acr_find_int(group_list, 
                                               SPI_PMS_slice_orientation,
                                               1);
                switch (orientation) {
                case 1:         /* transverse, slice=Z */
                    strcpy(str_buf, "1\\0\\0\\0\\1\\0");
                    break;
                case 2:         /* sagittal, slice=X */
                    strcpy(str_buf, "0\\1\\0\\0\\0\\1");
                    break;
                case 3:         /* coronal, slice=Y */
                    strcpy(str_buf, "1\\0\\0\\0\\0\\1");
                    break;
                }

                acr_insert_string(&group_list, ACR_Image_orientation_patient,
                                  str_buf);
            }
#if 0 /* not clear that this is needed */
            str_ptr = acr_find_string(group_list, ACR_Pixel_size, "");
            if (*str_ptr == '\0') {
                pms_element = acr_find_group_element(group_list, 
                                                     SPI_PMS_field_of_view);
                if (pms_element != NULL) {
                    double fov[2];
                    acr_get_element_numeric_array(pms_element, 2, fov);
                    if (fov[0] != 0.0 && fov[1] != 0.0) {
                        double derived_spacing[2];
                        int rows = acr_find_int(group_list, ACR_Rows, 1);
                        int cols = acr_find_int(group_list, ACR_Columns, 1);

                        derived_spacing[0] = fov[0] / cols;
                        derived_spacing[1] = fov[1] / rows;

                        sprintf(str_buf, "%.15g\\%.15g", 
                                derived_spacing[0], derived_spacing[1]);

                        printf("PHILIPS: New pixel size %s\n", str_buf);

                        acr_insert_string(&group_list, ACR_Pixel_size, 
                                          str_buf);
                    }
                }
            }
#endif
        }
    }
    else {
        if (G.Debug >= HI_LOGGING) {
            printf("Found Philips private creator ID at %#x\n", 
                   creator_id.element_id);
        }

        PMS_SET_CREATOR(PMS_Number_of_Slices_MR, &creator_id);
        slice_count = acr_find_int(group_list, PMS_Number_of_Slices_MR, -1);
        if (slice_count < 0) {
            printf("WARNING: Can't find Philips slice count\n");
        }
        else {
            acr_insert_short(&group_list, ACR_Images_in_acquisition, 
                             slice_count);
        }
        PMS_SET_CREATOR(PMS_Slice_Number_MR, &creator_id);
        slice_index = acr_find_int(group_list, PMS_Slice_Number_MR, -1);
        if (slice_index < 0) {
            printf("WARNING: Can't find Philips slice index\n");
        }
        else {
            /* TODO: It is really quite gross that we have to resort to
             * using a Siemens-proprietary field to tell the rest of the
             * code which slice we are on.  But such is life, for now...
             */
            acr_insert_numeric(&group_list, SPI_Current_slice_number,
                               (double) slice_index);
        }
    }
    return (group_list);
}

Acr_Group
add_gems_info(Acr_Group group_list)
{
    char *tmp_str;
    int image_count;
    int image_index;
    double tr;
    int ok;

    ok = 1;
    tmp_str = acr_find_string(group_list, GEMS_Acqu_private_creator_id, "");
    if (strcmp(tmp_str, "GEMS_ACQU_01")) {
        ok = 0;
    }

    tmp_str = acr_find_string(group_list, GEMS_Sers_private_creator_id, "");
    if (strcmp(tmp_str, "GEMS_SERS_01")) {
        ok = 0;
    }

    if (!ok) {
        /* Warn only for non-PET things.  We know the PET scanner doesn't
         * rely on this proprietary nonsense.
         */
        tmp_str = acr_find_string(group_list, ACR_Modality, "");
        if (strcmp(tmp_str, "PT") && G.Debug) {
            printf("WARNING: GEMS data not found\n");
        }
    }

    tmp_str = acr_find_string(group_list, ACR_Image_type, "");
    image_index = acr_find_int(group_list, ACR_Image, -1);
    image_count = acr_find_int(group_list, GEMS_Images_in_series, -1);
    tr = acr_find_double(group_list, ACR_Repetition_time, 0.0);

    /* If we found a valid image count in the proprietary field, copy it
     * into the standard field if the standard field is not already set.
     */
    if (image_count > 0 &&
        acr_find_int(group_list, ACR_Images_in_acquisition, -1) < 0) {
        acr_insert_long(&group_list, ACR_Images_in_acquisition, image_count);
    }

    /* Do this only for EPI images for now 
     */
    if (strstr(tmp_str, "\\EPI") != NULL &&
        image_index >= 0 &&
        image_count > 0 &&
        tr != 0.0) {
        int frame_count = acr_find_int(group_list, 
                                       GEMS_Fast_phases, -1);
        if (frame_count > 0) {
            if (acr_find_int(group_list, ACR_Acquisitions_in_series, -1) < 0) {
                acr_insert_long(&group_list, ACR_Acquisitions_in_series,
                                frame_count);
            }
        }

        if (acr_find_double(group_list, ACR_Frame_reference_time, -1.0) < 0) {
            acr_insert_numeric(&group_list, ACR_Frame_reference_time,
                               ((image_index - 1) / image_count) * tr);
        }
        if (acr_find_double(group_list, ACR_Actual_frame_duration, -1.0) < 0) {
            acr_insert_numeric(&group_list, ACR_Actual_frame_duration,
                               tr);
        }

        if (G.Debug >= HI_LOGGING) {
            printf("EPI slice %d timing information: %f, %f\n", 
                   image_index, ((image_index - 1) / image_count) * tr,
                   tr);
        }
    }
    return (group_list);
}


/* ----------------------------- MNI Header -----------------------------------
   @NAME       : copy_spi_to_acr()
   @INPUT      : group_list - read a standard DICOM file
   @OUTPUT     : (none)
   @RETURNS    : modified group list 
   @DESCRIPTION: Routine to copy Siemens-specific (SPI) fields to generic
                 ACR/NEMA fields.
   @METHOD     : 
   @GLOBALS    : 
   @CALLS      : 
   @CREATED    : 2005 (Bert Vincent)
   @MODIFIED   : 
   ---------------------------------------------------------------------------- */

Acr_Group 
copy_spi_to_acr(Acr_Group group_list)
{
    int itemp;
    double dtemp;

    itemp = acr_find_int(group_list, SPI_Number_of_slices_nominal, 0);
    if (itemp != 0) {
        acr_insert_numeric(&group_list, ACR_Images_in_acquisition, itemp);
    }

    itemp = acr_find_int(group_list, SPI_Number_of_echoes, 0);
    if (itemp != 0) {
        acr_insert_numeric(&group_list, ACR_Echo_train_length, itemp);
    }

    dtemp = acr_find_double(group_list, SPI_Magnetic_field_strength, 0);
    if (dtemp != 0.0) {
        acr_insert_numeric(&group_list, ACR_Magnetic_field_strength, dtemp);
    }
    return (group_list);
}

Acr_Group 
add_shimadzu_info(Acr_Group group_list)
{
    char *str_ptr;

    /* TODO: Figure out what can be done here...
     */
    str_ptr = acr_find_string(group_list, ACR_Series_instance_UID, "");
    if (*str_ptr != '\0') {
    }
    return (group_list);
}

/* ----------------------------- MNI Header -----------------------------------
   @NAME       : read_numa4_dicom
   @INPUT      : filename - read a standard DICOM file
   max_group - maximum group number to read
   @OUTPUT     : (none)
   @RETURNS    : group list read in from file
   @DESCRIPTION: Routine to read in a group list from a file.
   @METHOD     : 
   @GLOBALS    : 
   @CALLS      : 
   @CREATED    : December 18, 2001 (Rick Hoge)
   @MODIFIED   : 
   ---------------------------------------------------------------------------- */

Acr_Group
read_numa4_dicom(const char *filename, int max_group)
{
    Acr_Group group_list;
    char *str_ptr;

    group_list = read_std_dicom(filename, max_group);
    if (group_list == NULL) {
        return NULL;
    }

    /* Check the manufacturer.  If it is one we know, try to interpret
     * the private/proprietary data if present.  This is converted to
     * standard DICOM fields whereever it makes sense to do so.
     */
    str_ptr = acr_find_string(group_list, ACR_Manufacturer, "");
    if (strstr(str_ptr, "SIEMENS") != NULL ||
        strstr(str_ptr, "Siemens") != NULL) {

        group_list = add_siemens_info(group_list);

        /* Now copy proprietary fields into the correct places in the 
         * standard groups.
         */
        group_list = copy_spi_to_acr(group_list);

    }
    else if (strstr(str_ptr, "Philips") != NULL) {
        group_list = add_philips_info(group_list);
    }
    else if (strstr(str_ptr, "GEMS") != NULL ||
             strstr(str_ptr, "GE MEDICAL") != NULL) {
        group_list = add_gems_info(group_list);
    }
    else if (strstr(str_ptr, "shimadzu") != NULL) {
        group_list = add_shimadzu_info(group_list);
    }
    return (group_list);
}

/* ----------------------------- MNI Header -----------------------------------
   @NAME       : free_info
   @INPUT      : gi_ptr
   fi_ptr
   num_files
   @OUTPUT     : (none)
   @RETURNS    : (nothing)
   @DESCRIPTION: Routine to free contents of general and file info structures.
   @METHOD     : 
   @GLOBALS    : 
   @CALLS      : 
   @CREATED    : November 26, 1993 (Peter Neelin)
   @MODIFIED   : 
   ---------------------------------------------------------------------------- */

static void
free_info(General_Info *gi_ptr, File_Info *fi_ptr, int num_files)
{
    Mri_Index imri;

    /* Free the general info pointers */
    for (imri = 0; imri < MRI_NDIMS; imri++) {
        if (gi_ptr->indices[imri] != NULL) {
            free(gi_ptr->indices[imri]);
        }
        if (gi_ptr->coordinates[imri] != NULL) {
            free(gi_ptr->coordinates[imri]);
        }
        if (gi_ptr->widths[imri] != NULL) {
            free(gi_ptr->widths[imri]);
        }
    }

    /* Free the group list */
    if (gi_ptr->group_list != NULL) {
        acr_delete_group_list(gi_ptr->group_list);
    }

    return;

}

/* ----------------------------- MNI Header -----------------------------------
   @NAME       : search_list
   @INPUT      : value
   list_ptr
   list_length
   start_index - point from which search should start
   @OUTPUT     : (none)
   @RETURNS    : Index in list where value is found, or -1 is value not found.
   @DESCRIPTION: Routine to search a list for a value, returning the index
   into the list. If the value is not found, then -1 is returned.
   @METHOD     : 
   @GLOBALS    : 
   @CALLS      : 
   @CREATED    : February 28, 1997 (Peter Neelin)
   @MODIFIED   : 
   ---------------------------------------------------------------------------- */
int
search_list(int value, const int *list_ptr, int list_length, int start_index)
{
    int index;

    /* If nothing on list, just return. */
    if (list_length <= 0) {
        return (-1);
    }

    /* If starting point is invalid, start at zero.
     */
    if ((start_index >= list_length) || (start_index < 0)) {
        start_index = 0;
    }

    /* Loop over indices, wrapping at the end of the list */
    index = start_index;
    do {
        if (list_ptr[index] == value) {
            return index;       /* Found it. */
        }
        index++;
        if (index >= list_length) {
            index = 0;
        }
    } while (index != start_index);

    return -1;                  /* Search failed. */
}

/* ----------------------------- MNI Header -----------------------------------
   @NAME       : sort_dimensions
   @INPUT      : gi_ptr
   @OUTPUT     : gi_ptr
   @RETURNS    : (nothing)
   @DESCRIPTION: Routine to sort the MRI dimensions according to their 
   coordinates. It also fills in the step and start values for 
   the SLICE dimension.
   @METHOD     : 
   @GLOBALS    : 
   @CALLS      : 
   @CREATED    : February 28, 1997 (Peter Neelin)
   @MODIFIED   : 
   ---------------------------------------------------------------------------- */
static void
sort_dimensions(General_Info *gi_ptr)
{
    Mri_Index imri;
    Sort_Element *sort_array;
    int nvalues;
    int i;

    /* Sort the dimensions, if needed.
     */
    for (imri = 0; imri < MRI_NDIMS; imri++) {
        /* Sort each dimension iff the size is greater than 1 and it is not
         * a time dimension in an N4 mosaic.
         */
        if (gi_ptr->cur_size[imri] <= 1 ||
            /* Don't sort on time for N4 mosaics (TODO: Why not??)
             */
            ((G.file_type == N4DCM) &&
             (imri == TIME) && 
             (gi_ptr->num_slices_in_file > 1))) {
            if (G.Debug >= HI_LOGGING) {
                printf("Not sorting %s dimension\n", Mri_Names[imri]);
            }
            continue;
        }
            
        if (G.Debug >= HI_LOGGING) {
            printf("Sorting %s dimension\n", Mri_Names[imri]);
        }

        /* Set up the array for sorting.
         */
        nvalues = gi_ptr->cur_size[imri];
        sort_array = malloc(nvalues * sizeof(*sort_array));
        CHKMEM(sort_array);
        for (i = 0; i < nvalues; i++) {
            sort_array[i].identifier = gi_ptr->indices[imri][i];
            sort_array[i].original_index = i;
            sort_array[i].value = gi_ptr->coordinates[imri][i];
            sort_array[i].width = gi_ptr->widths[imri][i];
        }

        /* Sort the array.
         */
        qsort((void *) sort_array, (size_t) nvalues, sizeof(*sort_array), 
              dimension_sort_function);

        /* Copy the information back into the appropriate arrays 
         */
        for (i = 0; i < nvalues; i++) {
            gi_ptr->indices[imri][i] = sort_array[i].identifier;
            gi_ptr->coordinates[imri][i] = sort_array[i].value;
            gi_ptr->widths[imri][i] = sort_array[i].width;
        }

        if (G.Debug >= HI_LOGGING) {
            /* Print out all of the information about this dimension.
             */
            printf(" nvalues %d min %f max %f\n",
                   nvalues, 
                   gi_ptr->coordinates[imri][0],
                   gi_ptr->coordinates[imri][nvalues - 1]);
            for (i = 0; i < nvalues; i++) {
                printf("%3d %6d %8.3f\n", 
                       i,
                       gi_ptr->indices[imri][i],
                       gi_ptr->coordinates[imri][i]);
            }
        }
        
        /* Free the temporary array we used to sort the coordinate axis.
         */
        free(sort_array);

        /* Now verify that the slice coordinate array looks sane.  We
         * don't complain about things like missing time slices, since
         * many PET scanners produce irregular timings.
         */
        if (nvalues >= 2 && imri == SLICE) {
            /* Calculate the spacing between the first and second slice.
             */
            double delta = (gi_ptr->coordinates[imri][1] - 
                            gi_ptr->coordinates[imri][0]);

            for (i = 1; i < nvalues; i++) {
                /* Check that each successive slice has roughly the same
                 * spacing, to within 2 percent of the initial delta.
                 */
                if (!fcmp(delta, 
                          (gi_ptr->coordinates[imri][i] - 
                           gi_ptr->coordinates[imri][i - 1]),
                          2.0 * (delta / 100.0))) {
                    /* TODO: Perhaps this message could be improved?? */
                    printf("WARNING: Missing %s data at index %d, %g %g\n",
                           Mri_Names[imri],
                           i, 
                           delta,
                           (gi_ptr->coordinates[imri][i] -
                            gi_ptr->coordinates[imri][i - 1]));
                }
            }
        }

        /* Update slice step and start.
         */
        if (imri == SLICE) {
            if (gi_ptr->coordinates[imri][0] != 
                gi_ptr->coordinates[imri][nvalues-1]) {
                double dbl_tmp1;
                double dbl_tmp2;

                dbl_tmp1 = gi_ptr->step[gi_ptr->slice_world];
                dbl_tmp2 = (gi_ptr->coordinates[imri][nvalues - 1] -
                            gi_ptr->coordinates[imri][0]) / 
                    ((double) gi_ptr->cur_size[imri] - 1.0);

                /* OK, so now we have to figure out who wins the great
                 * battle to become the slice step value.  If the value
                 * we have been assuming up until now is the same as the
                 * calculated value with only a sign change, or if the
                 * assumed value is the default (1.0), we adopt the 
                 * calculated value.
                 */
                if (!fcmp(dbl_tmp1, dbl_tmp2, (dbl_tmp1 / 1000.0))) {
                    printf("WARNING: Coordinate spacing (%g) differs from DICOM slice spacing (%g)\n", dbl_tmp2, dbl_tmp1);
                    if (!G.prefer_coords) {
                        printf(" (perhaps you should consider the -usecoordinates option)\n");
                    }
                    if (dbl_tmp1 == 1.0 || G.prefer_coords || !fcmp(dbl_tmp1, -dbl_tmp2, (dbl_tmp1 / 1000.0))) { 
			    /*Although in the comment above it says that we should use the
			    calculated value if the assumed and calculated value only differ
			    by a sign change, this was not included in this if statement, added 
			    by ilana because default was wrong (i.e. w/o using -usecoordinates option)*/
                        gi_ptr->step[gi_ptr->slice_world] = dbl_tmp2;
                    }
                    printf(" Using %g for the slice spacing value.\n", 
                           gi_ptr->step[gi_ptr->slice_world]);
                }
                
            }
            gi_ptr->start[gi_ptr->slice_world] = gi_ptr->coordinates[imri][0];
            if (G.Debug >= HI_LOGGING) {
                printf("Set slice %d step to %f, start to %f\n",
                       gi_ptr->slice_world,
                       gi_ptr->step[gi_ptr->slice_world],
                       gi_ptr->start[gi_ptr->slice_world]);
            }
        } /* If slice dimension */
    } /* for all mri dimensions */
}

/* ----------------------------- MNI Header -----------------------------------
   @NAME       : dimension_sort_function
   @INPUT      : v1, v2 - values to compare
   @OUTPUT     : (none)
   @RETURNS    : -1, 0 or 1 if v1 < v2, v1 == v2 or v1 > v2
   @DESCRIPTION: Function to compare to array elements for sorting. Elements are
   compared first on value, then on their original array index
   (this tries to preserve the original sequence).
   @METHOD     : 
   @GLOBALS    : 
   @CALLS      : 
   @CREATED    : February 28, 1997 (Peter Neelin)
   @MODIFIED   : 
   ---------------------------------------------------------------------------- */
static int
dimension_sort_function(const void *v1, const void *v2)
{
    Sort_Element *value1, *value2;

    value1 = (Sort_Element *) v1;
    value2 = (Sort_Element *) v2;

    if (value1->value < value2->value)
        return -1;
    else if (value1->value > value2->value)
        return 1;
    else if (value1->original_index < value2->original_index)
        return -1;
    else if (value1->original_index > value2->original_index)
        return 1;
    else
        return 0;
}

int
prot_find_string(Acr_Element elem_ptr, const char *name_str, char *value)
{
    static const char prot_head[] = "### ASCCONV BEGIN ###";
    long cur_offset;
    long max_offset;
    char *field_ptr;
    int  ix1, ix2;

    max_offset = elem_ptr->data_length - sizeof(prot_head);

    // Scan through the element containing the protocol, to find the 
    // ASCII dump of the MrProt structure.
    //
    for (cur_offset = 0; cur_offset < max_offset; cur_offset++) {
        if (!memcmp(elem_ptr->data_pointer + cur_offset,
                    prot_head, sizeof(prot_head) - 1)) {
            break;
        }
    }

    /* bail if we didn't find the protocol
     */
    if (cur_offset == max_offset) {
        return (0);
    }

    field_ptr = strstr(elem_ptr->data_pointer + cur_offset, name_str);
    if (field_ptr != NULL) {
        sscanf(field_ptr, "%*s %*s %s", value);
        ix1 = 0;
        for (ix2 = 0; value[ix2] != '\0'; ix2++) {
            if (value[ix2] != '"') {
                value[ix1++] = value[ix2];
            }
        }
    } 
    else {
        strcpy(value, "0");
    }
    return (1);
}

static char *
dump_protocol_text(Acr_Element elem_ptr)
{
    const char prot_head[] = "### ASCCONV BEGIN ###";
    const char prot_tail[] = "### ASCCONV END ###";
    char *output = malloc(elem_ptr->data_length);
    int prot_found = FALSE;
    long cur_offset;
    long max_offset;

    CHKMEM(output);

    // scan throught the group containing the protocol, to find the 
    // ascii dump of the MrProt structure
    max_offset = elem_ptr->data_length - sizeof (prot_head);

    for (cur_offset = 0; cur_offset < max_offset; cur_offset++) {
        if (!memcmp(elem_ptr->data_pointer + cur_offset,
                    prot_head, sizeof(prot_head) - 1)) {
            prot_found = TRUE;
            break;
        }
    }

    if (prot_found) {
        int ix1 = 0;
        char *tmp_ptr = elem_ptr->data_pointer + cur_offset;

        for ( ; cur_offset < max_offset; cur_offset++) {
            if (!memcmp(tmp_ptr, prot_tail, sizeof(prot_tail) - 1)) {
                break;      
            }
            if (*tmp_ptr != '"') {
                output[ix1++] = *tmp_ptr;
            }
            tmp_ptr++;
        }
        output[ix1] = '\0';
    }
    return (output);
}

static int
copy_element_properties(Acr_Element new_element, Acr_Element old_element)
{
    acr_set_element_byte_order(new_element,
                               acr_get_element_byte_order(old_element));

    acr_set_element_vr_encoding(new_element,
                                acr_get_element_vr_encoding(old_element));
}

static int 
mosaic_init(Acr_Group group_list, Mosaic_Info *mi_ptr, int load_image)
{
    int group_id, element_id;
    int grid_size;
    long new_image_size;
    void *data;
    Acr_Element element;
    int i;
    double pixel_spacing[2];
    double separation;
    double RowColVec[6];
    double dircos[VOL_NDIMS][WORLD_NDIMS];
    char *str_tmp;

    if (G.Debug >= HI_LOGGING) {
        printf("mosaic_init(%lx, %lx, %d)\n",
               (unsigned long) group_list, (unsigned long) mi_ptr,
               load_image);
    }

    str_tmp = acr_find_string(group_list, SPI_Order_of_slices, "");
    if (!strncmp(str_tmp, "INTERLEAVED", 11)) {
        mi_ptr->mosaic_seq = MOSAIC_SEQ_INTERLEAVED;
    }
    else if (!strncmp(str_tmp, "ASCENDING", 9)) {
        mi_ptr->mosaic_seq = MOSAIC_SEQ_ASCENDING;
    }
    else if (!strncmp(str_tmp, "DESCENDING", 10)) {
        mi_ptr->mosaic_seq = MOSAIC_SEQ_DESCENDING;
    }
    else {
        mi_ptr->mosaic_seq = G.mosaic_seq;
    }

    if (G.Debug >= HI_LOGGING) {
        printf(" ordering is %s\n", str_tmp);
    }

    /* Get some basic image information.
     * big[0/1] is number of columns/rows in whole mosaic.
     */
    mi_ptr->big[0] = acr_find_int(group_list, ACR_Columns, 1);
    mi_ptr->big[1] = acr_find_int(group_list, ACR_Rows, 1);
    mi_ptr->pixel_size = 
        (acr_find_int(group_list, ACR_Bits_allocated, 16) - 1) / 8 + 1;

    /* Get the image size
     * (size[0/1] is number of columns/rows in a single slice)
     */
    mi_ptr->size[0] = acr_find_short(group_list, EXT_Sub_image_columns, 1);
    mi_ptr->size[1] = acr_find_short(group_list, EXT_Sub_image_rows, 1);

    // Get the grid shape, checking that it is not too big if specified
    mi_ptr->grid[0] = mi_ptr->big[0] / mi_ptr->size[0];
    mi_ptr->grid[1] = mi_ptr->big[1] / mi_ptr->size[1];
    if ((mi_ptr->grid[0] < 1) || (mi_ptr->grid[0] < 1)) {
        fprintf(stderr, "Grid too small: %d x %d, size %d x %d, big %d x %d\n",
                mi_ptr->grid[0], mi_ptr->grid[1],
                mi_ptr->size[0], mi_ptr->size[1],
                mi_ptr->big[0], mi_ptr->big[1]);
        exit(EXIT_FAILURE);
    }

    // Check whether we need to do anything (1x1 grid may be the whole image)
    grid_size = mi_ptr->grid[0] * mi_ptr->grid[1];
    if ((grid_size == 1) &&
        (mi_ptr->size[0] == mi_ptr->big[0]) &&
        (mi_ptr->size[1] == mi_ptr->big[1])) {
        /* had to remove this as now ANY images acquired with 
           the mosaic sequence need special treatment */
        mi_ptr->packed = FALSE;
        return 1;
    }

    /* Update the number of image rows and columns
     */
    acr_insert_short(&group_list, ACR_Rows, mi_ptr->size[1]);
    acr_insert_short(&group_list, ACR_Columns, mi_ptr->size[0]);

    /* Get image image index info (number of slices in file)
     */
    mi_ptr->slice_count = acr_find_int(group_list, EXT_Slices_in_file, 1);

    /* sub_images is now just the number of mosaic elements, even if
     * they don't all contain slices 
     */
    mi_ptr->sub_images = mi_ptr->grid[0] * mi_ptr->grid[1];


    /* Get the pixel size.
     */
    dicom_read_pixel_size(group_list, pixel_spacing);

    /* Get step between slices
     */
    separation = acr_find_double(group_list, ACR_Slice_thickness, 0.0);
    if (separation == 0.0) {
        separation = acr_find_double(group_list, ACR_Spacing_between_slices, 
                                     1.0);
    }

    /* get image normal vector
     * (need to compute based on dicom field, which gives
     *  unit vectors for row and column direction)
     * TODO: This code (and other code in this function) could probably
     * be broken out and made redundant with other code in the converter.
     */
    if (dicom_read_orientation(group_list, RowColVec)) {
        memcpy(dircos[VCOLUMN], RowColVec, sizeof(*RowColVec) * WORLD_NDIMS);
        memcpy(dircos[VROW], &RowColVec[3], sizeof(*RowColVec) * WORLD_NDIMS);

        /* compute slice normal as cross product of row/column unit vectors
         * (should check for unit length?)
         */
        mi_ptr->normal[XCOORD] = 
            dircos[VCOLUMN][YCOORD] * dircos[VROW][ZCOORD] -
            dircos[VCOLUMN][ZCOORD] * dircos[VROW][YCOORD];
   
        mi_ptr->normal[YCOORD] = 
            dircos[VCOLUMN][ZCOORD] * dircos[VROW][XCOORD] -
            dircos[VCOLUMN][XCOORD] * dircos[VROW][ZCOORD];
   
        mi_ptr->normal[ZCOORD] = 
            dircos[VCOLUMN][XCOORD] * dircos[VROW][YCOORD] -
            dircos[VCOLUMN][YCOORD] * dircos[VROW][XCOORD];
    }

    /* compute slice-to-slice step vector
     */
    for (i = 0; i < WORLD_NDIMS; i++) {
        mi_ptr->step[i] = separation * mi_ptr->normal[i];
    }

    /* Get position and correct to first slice
     */
    if (!dicom_read_position(group_list, 0, mi_ptr->position)) {
        if (G.Debug) {
            printf("WARNING: No image position found\n");
        }
        mi_ptr->position[XCOORD] = mi_ptr->position[YCOORD] = 
            mi_ptr->position[ZCOORD] = 0.0;
    }

    if (G.Debug >= HI_LOGGING) {
        printf(" step %.3f %.3f %.3f pos %.3f %.3f %.3f normal %.3f,%.3f,%.3f\n",
               mi_ptr->step[0],
               mi_ptr->step[1],
               mi_ptr->step[2],
               mi_ptr->position[0],
               mi_ptr->position[1],
               mi_ptr->position[2],
               mi_ptr->normal[0],
               mi_ptr->normal[1],
               mi_ptr->normal[2]
               );
    }

    if (mi_ptr->mosaic_seq != MOSAIC_SEQ_INTERLEAVED) {
        if (is_numaris3(group_list)) {
            for (i = 0; i < WORLD_NDIMS; i++) {
                mi_ptr->position[i] -= 
                    (double) (mi_ptr->sub_images-1) * mi_ptr->step[i];
            } 
        }
        else {
            /* Numaris 4 mosaic correction:
             * - position given is edge of huge slice constructed as if 
             *   real slice was at center of mosaic
             * - mi_ptr->big[0,1] are number of columns and rows of mosaic
             * - mi_ptr->size[0,1] are number of columns and rows of sub-image
             */

            for (i = 0; i < WORLD_NDIMS; i++) {
                /* Correct offset from mosaic Center
                 */
                mi_ptr->position[i] += (double)
                    ((dircos[VCOLUMN][i] * mi_ptr->big[0] * pixel_spacing[0]/2.0) +
                     (dircos[VROW][i] * mi_ptr->big[1] * pixel_spacing[1]/2));
                
                /* Move from center to corner of slice
                 */
                mi_ptr->position[i] -= 
                    ((dircos[VCOLUMN][i] * mi_ptr->size[0] * pixel_spacing[0]/2.0) +
                     (dircos[VROW][i] * mi_ptr->size[1] * pixel_spacing[1]/2.0));
            }
        }
    }

    if (G.Debug >= HI_LOGGING) {
        printf(" corrected position %.3f %.3f %.3f\n",
               mi_ptr->position[0],
               mi_ptr->position[1],
               mi_ptr->position[2]);
    }

    /* Now that we've corrected the _position_, we still might need to correct
     * the pixel spacing.  For some gadawful reason, some (but not all) Siemens
     * mosaic files store the pixel spacing such that it is scaled by the ratio
     * between the mosaic image size and the subimage size.  The best way I can 
     * think of to detect this is by comparing the field of view to the product
     * of the pixel spacing and the actual subimage size, and if they are 
     * wildly different, assume that we need to perform the scaling...
     */
    element = acr_find_group_element(group_list, SPI_Field_of_view);
    if (element != NULL) {
        double fov[2];          /* field of view (row/column) */

        acr_get_element_numeric_array(element, 2, fov);
        if (fov[0] != 0.0 && fov[1] != 0.0) {
            double derived_spacing[2];
            int ratio[2];
            char str_buf[128];

            /* Calculate the actual spacing required to cover the field of view.
             */
            derived_spacing[0] = fov[0] / mi_ptr->size[0];
            derived_spacing[1] = fov[1] / mi_ptr->size[1];

            ratio[0] = (int) rint(derived_spacing[0] / pixel_spacing[0]);
            ratio[1] = (int) rint(derived_spacing[1] / pixel_spacing[1]);

            /* If the ratio of the derived spacing to the pixel spacing is
             * the same as the ratio between the edge length of the large 
             * and small images, adopt the derived spacing as the correct
             * value.
             */
            if (ratio[0] == (mi_ptr->big[0] / mi_ptr->size[0]) ||
                ratio[1] == (mi_ptr->big[1] / mi_ptr->size[1])) {

                if (G.Debug) {
                    printf("Updating mosaic pixel spacing from %f,%f to %f,%f\n",
                           pixel_spacing[0],pixel_spacing[1],
                           derived_spacing[0],derived_spacing[1]);
                }

                /* Store the updated pixel spacing in the group list.
                 */
                sprintf(str_buf, "%.15g\\%.15g", 
                        derived_spacing[0], derived_spacing[1]);
                acr_insert_string(&group_list, ACR_Pixel_size, str_buf);
            }
        }
    }

    if (!load_image) {
        mi_ptr->big_image = NULL;
        mi_ptr->small_image = NULL;
    }
    else {

        /* Steal the image element from the group list
         */
        mi_ptr->big_image = acr_find_group_element(group_list, ACR_Pixel_data);
        if (mi_ptr->big_image == NULL) {
            fprintf(stderr, "Couldn't find an image\n");
            exit(EXIT_FAILURE);
        }
        group_id = acr_get_element_group(mi_ptr->big_image);
        element_id = acr_get_element_element(mi_ptr->big_image);
        acr_group_steal_element(acr_find_group(group_list, group_id),
                                mi_ptr->big_image);
        
        /* Add a small image
         */
        new_image_size = mi_ptr->size[0] * mi_ptr->size[1] * mi_ptr->pixel_size;
        data = malloc(new_image_size);
        CHKMEM(data);
        mi_ptr->small_image = acr_create_element(group_id, element_id,
                                                 acr_get_element_vr(mi_ptr->big_image),
                                                 new_image_size, data);
        acr_set_element_vr(mi_ptr->small_image,
                           acr_get_element_vr(mi_ptr->big_image));

        copy_element_properties(mi_ptr->small_image, mi_ptr->big_image);

        acr_insert_element_into_group_list(&group_list, mi_ptr->small_image);
    }

    /* Return number of sub-images in this image */
    return mi_ptr->sub_images;
}

static int 
mosaic_insert_subframe(Acr_Group group_list, Mosaic_Info *mi_ptr,
                       int iimage, int load_image)
{
    int irow;
    int idim;
    int nbyte;
    int isub;
    int jsub;
    char *new;
    char *old;
    long old_offset;
    long new_offset;
    double position[WORLD_NDIMS];
    string_t string;
    int islice;

    if (G.Debug >= HI_LOGGING) {
        printf("mosaic_insert_subframe(%lx, %lx, %d, %d)\n",
               (unsigned long)group_list, (unsigned long)mi_ptr,
               iimage, load_image);
    }

    /* Figure out what to do based upon the mosaic sequencing.
     */
    switch (mi_ptr->mosaic_seq) {
    case MOSAIC_SEQ_INTERLEAVED:
        /* For interleaved sequences, we have to map the odd slices to
         * the range slice_count/2..slice_count-1 and the even slices
         * from zero to slice_count/2-1
         */
        if (iimage & 1) {       /* Odd?? */
            islice = (mi_ptr->slice_count / 2) + (iimage / 2);
        }
        else {
            islice = iimage / 2;
        }
        break;

    default:
        /* Otherwise, just use the image number without modification for
         * ascending or unknown slice ordering.
         */
        islice = iimage;
        break;
    }

#if 0
    if (is_numaris3(group_list)) {
        islice = mi_ptr->slice_count - islice - 1;
    }
#endif

    /* Check the image number 
     */
    if ((iimage < 0) || (iimage > mi_ptr->sub_images)) {
        fprintf(stderr, "Invalid image number to send: %d of %d\n",
                iimage, mi_ptr->sub_images);
        exit(EXIT_FAILURE);
    }

    /* Update the index
     */
    acr_insert_numeric(&group_list, SPI_Current_slice_number, (double) iimage);

    /* Update the position
     */
    for (idim = 0; idim < WORLD_NDIMS; idim++) {
        position[idim] = mi_ptr->position[idim] + 
            (double) iimage * mi_ptr->step[idim];
    }

    /* If the sequence is descending, invert the slice coordinate.
     * This involves subtracting the step * index from the mosaic
     * slice position.
     */
    if (mi_ptr->mosaic_seq == MOSAIC_SEQ_DESCENDING) {
        position[ZCOORD] = mi_ptr->position[ZCOORD] - 
            (double) islice * mi_ptr->step[ZCOORD];
    }

    sprintf(string, "%.15g\\%.15g\\%.15g", 
            position[XCOORD], position[YCOORD], position[ZCOORD]);

    acr_insert_string(&group_list, ACR_Image_position_patient, string);
    acr_insert_string(&group_list, SPI_Image_position, string);

    /* HMM - is this necessary?? */
    if (is_numaris3(group_list)) {
        update_coordinate_info(group_list);
    }

    if (G.Debug >= HI_LOGGING) {
        printf(" position %s\n", string);
    }

    if (load_image) {
        /* Figure out the sub-image indices 
         */
        isub = islice % mi_ptr->grid[0];
        jsub = islice / mi_ptr->grid[0];

        /* Get pointers
         */
        old = acr_get_element_data(mi_ptr->big_image);
        new = acr_get_element_data(mi_ptr->small_image);

        /* Copy the image
         */
        nbyte = mi_ptr->size[0] * mi_ptr->pixel_size;
        for (irow = 0; irow < mi_ptr->size[1]; irow++) {
            old_offset = isub * mi_ptr->size[0] +
                (jsub * mi_ptr->size[1] + irow) * mi_ptr->big[0];
            old_offset *= mi_ptr->pixel_size;
            new_offset = (irow * mi_ptr->size[0]) * mi_ptr->pixel_size;
            memcpy(&new[new_offset], &old[old_offset], nbyte);
        }

        /* Reset the byte order and VR encoding. This will be modified on each
         * send according to what the connection needs.
         */
        copy_element_properties(mi_ptr->small_image, mi_ptr->big_image);
    }
    return 1;

}

static void 
mosaic_cleanup(Mosaic_Info *mi_ptr)
{
    if (mi_ptr->packed && mi_ptr->big_image != NULL) {
        acr_delete_element(mi_ptr->big_image);
    }
}

/************************************************************************
 * Multiframe functions, which mimic the behavior of the older mosaic
 * functions.  This is not yet a complete or correct implementation of
 * multiframe DICOM - see the NOTE: above!
 */
/* ----------------------------- MNI Header -----------------------------------
   @NAME       : multiframe_init()
   @INPUT      : group_list - the list of DICOM groups/elements that make up
                 this file.
                 int load_image - a boolean value, non-zero if the function
                 should actually load the data into memory.
   @OUTPUT     : mfi_ptr - a pointer to a Multiframe_Info structure that will
                 contain information used to expand this multiframe file into
                 a series of slices.
   @RETURNS    : void
   @DESCRIPTION: Initialize a multiframe conversion process.
   @METHOD     : 
   @GLOBALS    :
   @CALLS      : 
   @CREATED    : June 3, 2005 Bert Vincent
   @MODIFIED   : 
---------------------------------------------------------------------------- */

static void
multiframe_init(Acr_Group group_list, Multiframe_Info *mfi_ptr, int load_image)
{
    int grp_id;
    int elm_id;
    void *data_ptr;
    Acr_Element element;
    int i;
    double spacing;
    double RowColVec[6];
    double dircos[VOL_NDIMS][WORLD_NDIMS];
    int rows;
    int cols;
    int pixel_size;

    if (G.Debug >= HI_LOGGING) {
        printf("multiframe_init(%lx, %lx, %d)\n",
               (unsigned long) group_list, (unsigned long) mfi_ptr,
               load_image);
    }

    cols = acr_find_int(group_list, ACR_Columns, 1);
    rows = acr_find_int(group_list, ACR_Rows, 1);
    pixel_size = 
        (acr_find_int(group_list, ACR_Bits_allocated, 16) - 1) / 8 + 1;

    /* Get image image index info (number of slices in file)
     */
    mfi_ptr->frame_count = acr_find_int(group_list, ACR_Number_of_frames, 1);

    /* Get spacing between slices
     */
    spacing = acr_find_double(group_list, ACR_Slice_thickness, 0.0);
    if (spacing == 0.0) {
        spacing = acr_find_double(group_list, ACR_Spacing_between_slices, 1.0);
    }

    /* get image normal vector
     * (need to compute based on dicom field, which gives
     *  unit vectors for row and column direction)
     * TODO: This code (and other code in this function) could probably
     * be broken out and made redundant with other code in the converter.
     */
    if (dicom_read_orientation(group_list, RowColVec)) {
        memcpy(dircos[VCOLUMN], RowColVec, sizeof(*RowColVec) * WORLD_NDIMS);
        memcpy(dircos[VROW], &RowColVec[3], sizeof(*RowColVec) * WORLD_NDIMS);

        convert_dicom_coordinate(dircos[VROW]);
        convert_dicom_coordinate(dircos[VCOLUMN]);

        /* compute slice normal as cross product of row/column unit vectors
         * (should check for unit length?)
         */
        mfi_ptr->normal[XCOORD] = 
            dircos[VCOLUMN][YCOORD] * dircos[VROW][ZCOORD] -
            dircos[VCOLUMN][ZCOORD] * dircos[VROW][YCOORD];
   
        mfi_ptr->normal[YCOORD] = 
            dircos[VCOLUMN][ZCOORD] * dircos[VROW][XCOORD] -
            dircos[VCOLUMN][XCOORD] * dircos[VROW][ZCOORD];
   
        mfi_ptr->normal[ZCOORD] = 
            dircos[VCOLUMN][XCOORD] * dircos[VROW][YCOORD] -
            dircos[VCOLUMN][YCOORD] * dircos[VROW][XCOORD];
    }

    /* If the normal is unreliable, use a default value.
     */
    if (mfi_ptr->normal[XCOORD] == mfi_ptr->normal[YCOORD] &&
        mfi_ptr->normal[XCOORD] == mfi_ptr->normal[ZCOORD]) {
        mfi_ptr->normal[ZCOORD] = -1.0;
        mfi_ptr->normal[YCOORD] = 0.0;
        mfi_ptr->normal[XCOORD] = 0.0;
    }

    /* Compute slice-to-slice step vector
     */
    for (i = 0; i < WORLD_NDIMS; i++) {
        mfi_ptr->step[i] = spacing * mfi_ptr->normal[i];
    }

    /* Get position and correct to first slice
     */
    if (!dicom_read_position(group_list, 0, mfi_ptr->position)) {
        if (G.Debug) {
            printf("WARNING: No image position found\n");
        }
        mfi_ptr->position[XCOORD] = mfi_ptr->position[YCOORD] = 
            mfi_ptr->position[ZCOORD] = 0.0;
    }
    convert_dicom_coordinate(mfi_ptr->position);

    if (G.Debug >= HI_LOGGING) {
        printf(" step %.3f %.3f %.3f position %.3f %.3f %.3f\n",
               mfi_ptr->step[0],
               mfi_ptr->step[1],
               mfi_ptr->step[2],
               mfi_ptr->position[0],
               mfi_ptr->position[1],
               mfi_ptr->position[2]);
    }

    if (!load_image) {
        mfi_ptr->big_image = NULL;
        mfi_ptr->sub_image = NULL;
    }
    else {
        /* We need to load the image (we're probably on the second pass).
         */

        /* Steal the image element from the group list
         */
        element = acr_find_group_element(group_list, ACR_Pixel_data);
        if (element == NULL) {
            fprintf(stderr, "Couldn't find an image\n");
            exit(EXIT_FAILURE);
        }

        mfi_ptr->big_image = element; /* Save pointer to pixel data element. */

        grp_id = acr_get_element_group(element);
        elm_id = acr_get_element_element(element);
        acr_group_steal_element(acr_find_group(group_list, grp_id), element);
        
        /* Add a small image
         */
        mfi_ptr->frame_size = rows * cols * pixel_size;
        data_ptr = malloc(mfi_ptr->frame_size);
        CHKMEM(data_ptr);

        mfi_ptr->sub_image = acr_create_element(grp_id,
                                                elm_id,
                                                acr_get_element_vr(element),
                                                mfi_ptr->frame_size, 
                                                data_ptr);

        copy_element_properties(mfi_ptr->sub_image, mfi_ptr->big_image);
        
        acr_insert_element_into_group_list(&group_list, mfi_ptr->sub_image);
    }
}

/* ----------------------------- MNI Header -----------------------------------
   @NAME       : multiframe_insert_subframe()
   @INPUT      : group_list - the list of DICOM groups/elements that make up
                 this file.
                 mfi_ptr - a pointer to a Multiframe_Info structure that will
                 contain information used to expand this multiframe file into
                 a series of slices.
                 int iimage - the index of the subimage to parse and possibly
                 to load.
                 int load_image - a boolean value, non-zero if the function
                 should actually load the data into memory.
   @OUTPUT     : mfi_ptr - may be modified by the function.
   @RETURNS    : void
   @DESCRIPTION: This function decomposes a multiframe DICOM image
                 into a series of single-frame images. Modifies the
                 group_list to include updated image and position
                 information.
   @METHOD     : 
   @GLOBALS    :
   @CALLS      : 
   @CREATED    : June 3, 2005 Bert Vincent
   @MODIFIED   : 
---------------------------------------------------------------------------- */
static void
multiframe_insert_subframe(Acr_Group group_list, Multiframe_Info *mfi_ptr,
                           int iframe, int load_image)
{
    int idim;
    char *new_ptr;
    char *old_ptr;
    double position[WORLD_NDIMS];
    string_t string;
    int result;

    if (G.Debug >= HI_LOGGING) {
        printf("multiframe_insert_subframe(%lx, %lx, %d, %d)\n",
               (unsigned long)group_list, (unsigned long)mfi_ptr,
               iframe, load_image);
    }

    /* Check the frame number 
     */
    if ((iframe < 0) || (iframe > mfi_ptr->frame_count)) {
        fprintf(stderr, "Invalid image number to send: %d of %d\n",
                iframe, mfi_ptr->frame_count);
        exit(EXIT_FAILURE);
    }

    /* Update the index in the file's group list.
     */
    acr_insert_numeric(&group_list, SPI_Current_slice_number, (double) iframe);

    result = dicom_read_position(group_list, iframe, position);
    convert_dicom_coordinate(position);

    if (result != DICOM_POSITION_LOCAL) {
        /* If either no position was found for this frame number, or if
         * only a global position was found, we need to update the 
         * position for this particular frame number.
         * 
         * If a local position is found, as in some multiframe files,
         * this step is unnecessary and possibly wrong.
         */
        for (idim = 0; idim < WORLD_NDIMS; idim++) {
            position[idim] = mfi_ptr->position[idim] + 
                (double) iframe * mfi_ptr->step[idim];
        }
    }

    sprintf(string, "%.15g\\%.15g\\%.15g", 
            position[XCOORD], position[YCOORD], position[ZCOORD]);

    acr_insert_string(&group_list, ACR_Image_position_patient, string);

    if (G.Debug >= HI_LOGGING) {
        printf(" position %s\n", string);
    }

    if (load_image) {
        /* Get pointers
         */
        old_ptr = acr_get_element_data(mfi_ptr->big_image);
        new_ptr = acr_get_element_data(mfi_ptr->sub_image);

        /* Copy the image
         */
        memcpy(new_ptr,         /* destination */
               old_ptr + (iframe * mfi_ptr->frame_size), /* source */
               mfi_ptr->frame_size); /* length */
        
        /* Reset the byte order and VR encoding. This will be modified
         * on each send according to what the connection needs.
         */
        copy_element_properties(mfi_ptr->sub_image, mfi_ptr->big_image);
    }
}

static void 
multiframe_cleanup(Multiframe_Info *mfi_ptr)
{
    if (mfi_ptr->big_image != NULL) {
        acr_delete_element(mfi_ptr->big_image);
        mfi_ptr->big_image = NULL;
    }
}

