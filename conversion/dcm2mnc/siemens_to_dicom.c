/* ----------------------------- MNI Header -----------------------------------
@NAME       : siemens_to_dicom.c
@DESCRIPTION: File containing routines to read in a Siemens vision internal
              file (.IMA extension) and convert it to a DICOM representation.
@METHOD     : 
@GLOBALS    : 
@CREATED    : July 8, 1997 (Peter Neelin)
@MODIFIED   : $Log: siemens_to_dicom.c,v $
@MODIFIED   : Revision 1.7  2006-02-09 20:54:29  bert
@MODIFIED   : More changes to dcm2mnc
@MODIFIED   :
@MODIFIED   : Revision 1.6  2005/04/21 22:32:15  bert
@MODIFIED   : Continue Siemens IMA code cleanup
@MODIFIED   :
@MODIFIED   : Revision 1.5  2005/04/18 16:21:16  bert
@MODIFIED   : Fix definition of siemens_to_dicom
@MODIFIED   :
@MODIFIED   : Revision 1.4  2005/04/05 21:56:47  bert
@MODIFIED   : Add some conversion functions, remove some more proprietary junk, and improve range-checking on some functions
@MODIFIED   :
@MODIFIED   : Revision 1.3  2005/03/03 18:59:16  bert
@MODIFIED   : Fix handling of image position so that we work with the older field (0020, 0030) as well as the new (0020, 0032)
@MODIFIED   :
@MODIFIED   : Revision 1.2  2005/03/02 20:06:23  bert
@MODIFIED   : Update conversions to reflect simplified header structures and types
@MODIFIED   :
@MODIFIED   : Revision 1.1  2005/02/17 16:38:11  bert
@MODIFIED   : Initial checkin, revised DICOM to MINC converter
@MODIFIED   :
@MODIFIED   : Revision 1.1.1.1  2003/08/15 19:52:55  leili
@MODIFIED   : Leili's dicom server for sonata
@MODIFIED   :
@MODIFIED   : Revision 1.1  2001/12/31 17:28:34  rhoge
@MODIFIED   : adding file to repos - now needed for reading .ima files in directly
@MODIFIED   :
@MODIFIED   : Revision 1.2  2000/12/17 01:05:24  rhoge
@MODIFIED   : temporary activation of offset table printing macro
@MODIFIED   :
@MODIFIED   : Revision 1.1.1.1  2000/11/30 02:05:54  rhoge
@MODIFIED   : imported sources to CVS repository on amoeba
@MODIFIED   :
 * Revision 1.4  1998/11/16  19:54:15  neelin
 * Added definitions for SunOS.
 *
 * Revision 1.3  1998/11/13  16:02:09  neelin
 * Modifications to support packed images and asynchronous transfer.
 *
 * Revision 1.2  1997/11/04  14:31:30  neelin
 * *** empty log message ***
 *
 * Revision 1.1  1997/08/11  12:50:53  neelin
 * Initial revision
 *
---------------------------------------------------------------------------- */

static const char rcsid[]="$Header: /private-cvsroot/minc/conversion/dcm2mnc/siemens_to_dicom.c,v 1.7 2006-02-09 20:54:29 bert Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "dcm2mnc.h"
#include "siemens_header_defs.h"

/* Constants */

#define SIEMENS_IMAGE_OFFSET 6144 /* From dclunie.com */

#define IMAGE_NDIMS 2

/* Types */
#define ELEMENT_FUNC_ARGS \
    (int grp_id, int elm_id, void *data, int length)

#define DEFINE_ELEMENT_FUNC(name) \
    static Acr_Element name ELEMENT_FUNC_ARGS

#define DECLARE_ELEMENT_FUNC(name) \
    static Acr_Element name ELEMENT_FUNC_ARGS

typedef Acr_Element (*Create_Element_Function) ELEMENT_FUNC_ARGS;

typedef struct {
    int grp_id;
    int elm_id;
    void *data;
    Create_Element_Function function;
    int length;
} Siemens_hdr_entry;

/* Functions */

static Acr_Element_Id get_elid(int grp_id, int elm_id, Acr_VR_Type vr_code);

DECLARE_ELEMENT_FUNC(create_char_element);
DECLARE_ELEMENT_FUNC(create_long_element);
DECLARE_ELEMENT_FUNC(create_short_element);
DECLARE_ELEMENT_FUNC(create_double_element);
DECLARE_ELEMENT_FUNC(create_ima_date_t_element);
DECLARE_ELEMENT_FUNC(create_ima_time_t_element);
DECLARE_ELEMENT_FUNC(create_modality_element);
DECLARE_ELEMENT_FUNC(create_sex_element);
DECLARE_ELEMENT_FUNC(create_age_element);
DECLARE_ELEMENT_FUNC(create_slice_order_element);
DECLARE_ELEMENT_FUNC(create_pixel_spacing_t_element);
DECLARE_ELEMENT_FUNC(create_window_t_element);
DECLARE_ELEMENT_FUNC(create_ima_vector_t_element);
DECLARE_ELEMENT_FUNC(create_laterality_element);
DECLARE_ELEMENT_FUNC(create_ima_position_t_element);
DECLARE_ELEMENT_FUNC(create_rest_direction_t_element);
DECLARE_ELEMENT_FUNC(create_view_direction_t_element);
DECLARE_ELEMENT_FUNC(create_ima_orientation_t_element);
DECLARE_ELEMENT_FUNC(create_field_of_view_t_element);

/* Define the table of header values */
ima_header_t IMA_hdr;           /* Must define this first */
#include "siemens_header_table.h" /* Now include the table */

/* flag to print offset table, useful in debugging byte pad issues
   with Linux/sun porting */
/* #define PRINT_OFFSET_TABLE 1 */

/* ----------------------------- MNI Header -----------------------------------
@NAME       : siemens_to_dicom
@INPUT      : filename - name of Siemens internal file
              read_image - if TRUE, then the image is added to the group list,
                           otherwise it is not read in.
@OUTPUT     : (none)
@RETURNS    : Acr-nema group list containing contents of Siemens file
@DESCRIPTION: Function to read in a siemens internal format file (.IMA) 
              and store it in an ACR-NEMA group list.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : July 8, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */

Acr_Group
siemens_to_dicom(const char *filename, int max_group)
{
    FILE *fp;
    Siemens_hdr_entry *entry;
    Acr_Group group_list;
    Acr_Element element;
    long image_size;
    long pixel_size;
    void *image;
    double flip_angle;
    short rows;
    short cols;
    int n_slices;

#ifdef PRINT_OFFSET_TABLE
    void *header_ptr;           /* debug junk */
    void *data_ptr;             /* debug junk */
    long offset;                /* debug junk */
#endif

    /* Check the structure offsets */
    assert(((char *)&IMA_hdr.G08 - (char *)&IMA_hdr) == 0x0000);
    assert(((char *)&IMA_hdr.G10 - (char *)&IMA_hdr) == 0x0300);
    assert(((char *)&IMA_hdr.G18 - (char *)&IMA_hdr) == 0x0600);
    assert(((char *)&IMA_hdr.G19 - (char *)&IMA_hdr) == 0x0780);
    assert(((char *)&IMA_hdr.G20 - (char *)&IMA_hdr) == 0x0C80);
    assert(((char *)&IMA_hdr.G21 - (char *)&IMA_hdr) == 0x0E80);
    assert(((char *)&IMA_hdr.G28 - (char *)&IMA_hdr) == 0x1380);

    if (G.Debug >= HI_LOGGING) {
        printf("siemens_to_dicom(%s, %x)\n", filename, max_group);
    }

    /* Open the file */
    if ((fp = fopen(filename, "rb")) == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        return NULL;
    }

    /* Read in the header */
    if (fread(&IMA_hdr, sizeof(IMA_hdr), 1, fp) != 1) {
        fprintf(stderr, "Error reading header in %s\n", filename);
        fclose(fp);
        return NULL;
    }

    /* Get the image if it is needed */
    if (max_group >= ACR_IMAGE_GID) {

        /* Figure out how much space we need for the image */
        pixel_size = 2;         /* Apparently this never changes?? */

        /* Need to byte swap row/col values if needed 
         */
        acr_get_short(ACR_BIG_ENDIAN, 1, &IMA_hdr.G28.Rows, &rows);
        acr_get_short(ACR_BIG_ENDIAN, 1, &IMA_hdr.G28.Columns, &cols);

        image_size = rows * cols;

        image = malloc(pixel_size * image_size);
        CHKMEM(image);

        /* Read in the image */
        if (fseek(fp, (long) SIEMENS_IMAGE_OFFSET, SEEK_SET)) {
            printf("ERROR: Error finding image in %s\n", filename);
            fclose(fp);
            return NULL;
        }
        if (fread(image, pixel_size, image_size, fp) != image_size) {
            printf("ERROR: Error reading image in %s\n", filename);
            fclose(fp);
            return NULL;
        }

    }         /* If (max_group >= ACR_IMAGE_GID) */

    /* Close the file */
    fclose(fp);

    /* Loop through the header table, creating a header */
    group_list = NULL;
    for (entry = Siemens_hdr_table; entry->data != NULL; entry++) {

#ifdef PRINT_OFFSET_TABLE
        data_ptr = entry->data;
        header_ptr = &IMA_hdr;
        offset = (long) data_ptr - (long) header_ptr;
        printf("DEBUG:  group = 0x%x, element = 0x%x, offset = 0x%lx, length = 0x%x\n",
               entry->grp_id, entry->elm_id, offset, entry->length);
#endif

        if (entry->function == NULL) {
            continue;
        }
        element = entry->function(entry->grp_id, entry->elm_id,
                                  entry->data, entry->length);
        if (element == NULL) {
            continue;
        }

        acr_insert_element_into_group_list(&group_list, element);
    }

    /* Insert flip angle element */
    element = acr_find_group_element(group_list, SPI_Flip_angle);
    if (element != NULL) {
        flip_angle = acr_get_element_numeric(element);
        if (flip_angle >= 0.0) {
            acr_insert_numeric(&group_list, ACR_Flip_angle, flip_angle);
        }
    }

    n_slices = acr_find_int(group_list, SPI_Number_of_slices_nominal, 1);
    if (n_slices > 1) {
        short acq_cols;
        int n_acq;
        int n_avg;

        acq_cols = acr_find_short(group_list, SPI_Acquisition_columns,
                                  acr_find_short(group_list, ACR_Columns, 1));

        rows = acr_find_int(group_list, ACR_Rows, 1);
        acr_insert_long(&group_list, EXT_Mosaic_rows, rows);
                        
        cols = acr_find_int(group_list, ACR_Columns, 1);
        acr_insert_long(&group_list, EXT_Mosaic_columns, cols);

        /* Don't allow acq_cols greater than actual size! */
        if (acq_cols > rows) { 
            n_slices = 1;
            acq_cols = rows;
        }
        if (acq_cols > cols) {
            n_slices = 1;
            acq_cols = cols;
        }
        acr_insert_long(&group_list, EXT_Slices_in_file, n_slices);
        acr_insert_short(&group_list, EXT_Sub_image_rows, acq_cols);
        acr_insert_short(&group_list, EXT_Sub_image_columns, acq_cols);

        /* We need to set up the ACR_Acquisition (and 
         * ACR_Acquisitions_in_series) objects to make everyone happy.
         *
         * TODO: This appears to work for SOME IMA files, but it may
         * not be correct for all sequences.
         *
         * BERT: My latest change here is to examine both values and
         * use the larger of the two. Jens Pruessner had some IMA files
         * that had a value for acquisitions_in_series that was correctly
         * giving the number of time steps, while the nr_averages was 1.
         * Other files apparently contradict this!
         */
        acr_insert_long(&group_list, ACR_Acquisition, 
                        acr_find_int(group_list, ACR_Image, 1));

        n_avg = acr_find_int(group_list, ACR_Nr_of_averages, 1);
        n_acq = acr_find_int(group_list, ACR_Acquisitions_in_series, 1);
        if (n_avg > n_acq) {
            acr_insert_long(&group_list, ACR_Acquisitions_in_series, n_avg);
        }
    }

    /* Insert a series number */
    acr_insert_numeric(&group_list, ACR_Series, 1.0);

    /* Insert appropriate image position and orientation information */
    update_coordinate_info(group_list);


    /* Add the image if it is needed */
    if (max_group >= ACR_IMAGE_GID) {

        /* Insert the image location */
        acr_insert_short(&group_list, ACR_Image_location, ACR_IMAGE_GID);

        /* Add the image. We don't byte-swap here since it will be done
           automatically when the data is written out. */
        element = acr_create_element(ACR_IMAGE_GID, ACR_IMAGE_EID, ACR_VR_OW,
                                     image_size * pixel_size, image);
        acr_insert_element_into_group_list(&group_list, element);

        /* explicitly label image data as big-endian */
        acr_set_element_byte_order(element, ACR_BIG_ENDIAN);

    } /* if (max_group >= ACR_IMAGE_GID) */

    /* Return the group list */
    return group_list;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : update_coordinate_info
@INPUT      : group_list
@OUTPUT     : group_list
@RETURNS    : (nothing)
@DESCRIPTION: Function to modify the DICOM coordinate information to match
              the Siemens info.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 9, 1998 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
void
update_coordinate_info(Acr_Group group_list)
{
    Acr_Element element;
    int nrows, ncolumns;
    int i;
    double coord[WORLD_NDIMS];
    double row[WORLD_NDIMS];
    double column[WORLD_NDIMS];
    double normal[WORLD_NDIMS];
    double pixel_spacing[IMAGE_NDIMS];
    string_t string;
    int n_slices;

    if (G.Debug >= HI_LOGGING) {
        printf("update_coordinate_info(%lx)\n", (unsigned long) group_list);
    }

    /* Look for the normal vector 
     */
    element = acr_find_group_element(group_list, SPI_Image_normal);
    if ((element == NULL) ||
        (acr_get_element_numeric_array(element, WORLD_NDIMS, normal)
         != WORLD_NDIMS)) {
        normal[XCOORD] = 0.0;
        normal[YCOORD] = 0.0;
        normal[ZCOORD] = 1.0;
    }

    /* Look for the row vector.
     */
    element = acr_find_group_element(group_list, SPI_Image_row);
    if ((element == NULL) ||
        (acr_get_element_numeric_array(element, WORLD_NDIMS, row) 
         != WORLD_NDIMS)) {
        row[XCOORD] = 1.0; 
        row[YCOORD] = 0.0; 
        row[ZCOORD] = 0.0;
    }

    /* Look for the column vector 
     */
    element = acr_find_group_element(group_list, SPI_Image_column);
    if ((element == NULL) ||
        (acr_get_element_numeric_array(element, WORLD_NDIMS, column) 
         != WORLD_NDIMS)) {
        column[XCOORD] = 0.0; 
        column[YCOORD] = 1.0; 
        column[ZCOORD] = 0.0;
    }

    if (G.Debug >= HI_LOGGING) {
        printf("R %.3f %.3f %.3f C %.3f %.3f %.3f N %.3f %.3f %.3f\n",
               row[0], row[1], row[2],
               column[0], column[1], column[2],
               normal[0], normal[1], normal[2]);
    }

    /* Put in the dicom orientation (patient) field 
     */
    sprintf(string, "%.15g\\%.15g\\%.15g\\%.15g\\%.15g\\%.15g",
            row[XCOORD], -row[YCOORD], -row[ZCOORD], 
            column[XCOORD], -column[YCOORD], -column[ZCOORD]);
    acr_insert_string(&group_list, ACR_Image_orientation_patient, string);

    /* Look for the position.
     */
    element = acr_find_group_element(group_list, SPI_Image_position);
    if ((element == NULL) ||
        (acr_get_element_numeric_array(element, WORLD_NDIMS, coord) 
         != WORLD_NDIMS)) {
        coord[XCOORD] = 0.0; 
        coord[YCOORD] = 0.0; 
        coord[ZCOORD] = 0.0;
    }
    else {
        if (G.Debug >= HI_LOGGING) {
            printf(" old %.3f %.3f %.3f, ", coord[0], coord[1], coord[2]);
        }
    }

    /* Get the number of rows and columns.
     */

    nrows = acr_find_int(group_list, ACR_Rows, 0);
    ncolumns = acr_find_int(group_list, ACR_Columns, 0);
    if ((nrows <= 0) || (ncolumns <= 0)) {
        printf("ERROR: Illegal image size in Siemens IMA file\n");
        exit(1);
    }


    /* Get the pixel size */
    element = acr_find_group_element(group_list, ACR_Pixel_size);
    if ((element == NULL) ||
        (acr_get_element_numeric_array(element, IMAGE_NDIMS, pixel_spacing) 
         != IMAGE_NDIMS)) {
        pixel_spacing[0] = pixel_spacing[1] = 1.0;
    }

    /* Calculate the position of the first pixel. This coordinate
     * is still in the Siemens space, not dicom space and will
     * need to be flipped. Note that ncolumns is used with row,
     * since they are the size and unit vector of the same
     * dimension.
     */
    for (i = 0; i < WORLD_NDIMS; i++) {
        coord[i] -= 
            pixel_spacing[0] * ((double) ncolumns - 1.0) / 2.0 * row[i] +
            pixel_spacing[1] * ((double) nrows - 1.0) / 2.0 * column[i];
    }
    sprintf(string, "%.15g\\%.15g\\%.15g", coord[0], -coord[1], -coord[2]);
    acr_insert_string(&group_list, ACR_Image_position_patient, string);
    if (G.Debug >= HI_LOGGING) {
        printf(" new %.3f %.3f %.3f\n", coord[0], -coord[1], -coord[2]);
    }

    /* Copy non-standard fields to standard fields.
     */
    group_list = copy_spi_to_acr(group_list);

    /* If this is a Mosaic image, we need to adjust the pixel spacing
     * to reflect the ratio between the number of mosaic columns
     * divided the number of image columns.
     */
    n_slices = acr_find_int(group_list, EXT_Slices_in_file, 1);
    if (n_slices != 1) {
        int acq_cols = acr_find_short(group_list, SPI_Acquisition_columns, 
                                      ncolumns);
        if (G.Debug >= HI_LOGGING) {
            printf("Hmmm... This appears to be a mosaic image %d %d %d\n",
                   acq_cols, ncolumns, n_slices);
        }

        /* Mosaic images in IMA format appear to need to have their
         * pixel spacing scaled up.  I don't fully understand why this
         * should be necessary, but there it is...
         */
        pixel_spacing[0] *= (double) nrows / (double) acq_cols;
        pixel_spacing[1] *= (double) ncolumns / (double) acq_cols;
        sprintf(string, "%.15g\\%.15g", pixel_spacing[0], pixel_spacing[1]);
        acr_insert_string(&group_list, ACR_Pixel_size, string);
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_<type>_element
@INPUT      : grp_id
              elm_id
              data - pointer to data in Siemens header
              length - number of values in array (if appropriate)
@OUTPUT     : (none)
@RETURNS    : New element containing data
@DESCRIPTION: Series of functions to convert Siemens vision header types
              to ACR-NEMA elements
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : July 8, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */

static Acr_Element_Id 
get_elid(int grp_id, int elm_id, Acr_VR_Type vr_code)
{
    static struct Acr_Element_Id elid_struct = {0, 0, ACR_VR_UNKNOWN};
    elid_struct.group_id = grp_id;
    elid_struct.element_id = elm_id;
    elid_struct.vr_code = vr_code;
    return &elid_struct;
}

DEFINE_ELEMENT_FUNC(create_char_element)
{
    char *old, *new;
    int oldsize, newsize, i;

    /* Get a pointer to the old string */
    old = (char *) data;

    /* Figure out the length of the new string up to the first NUL. Make 
     * sure that there is a room for an additional NUL if necessary 
     */
    for (i = 0; (i < length - 1) && (old[i] != '\0'); i++) 
        ;
    newsize = ((old[i] == '\0') ? i + 1 : length + 1);
    oldsize = newsize - 1;
    if ((newsize % 2) != 1) {   /* Assure even length overall */
        newsize++;
    }

    /* Copy the string, making sure that there is a NUL on the end */
    new = malloc(newsize);
    CHKMEM(new);
    for (i = 0; i < newsize-1; i++) {
        if (i < oldsize)
            new[i] = old[i];
        else
            new[i] = ' ';
    }
    new[newsize-1] = '\0';

    /* Create the element */
    return acr_create_element(grp_id, elm_id, ACR_VR_ST, 
                              (long) newsize-1, (void *) new);
}

DEFINE_ELEMENT_FUNC(create_long_element)
{
    long data_out;

    acr_get_long(ACR_BIG_ENDIAN, 1, data, &data_out);

    return acr_create_element_numeric(get_elid(grp_id, elm_id, ACR_VR_IS), 
                                      data_out); 
}

DEFINE_ELEMENT_FUNC(create_short_element)
{
    unsigned short data_out;

    acr_get_short(ACR_BIG_ENDIAN, 1, data, &data_out);

    return acr_create_element_short(get_elid(grp_id, elm_id, ACR_VR_US), 
                                    data_out);
}

DEFINE_ELEMENT_FUNC(create_double_element)
{
    double data_out;
  
    acr_get_double(ACR_BIG_ENDIAN, 1, data, &data_out);

    return acr_create_element_numeric(get_elid(grp_id, elm_id, ACR_VR_DS),
                                      data_out);
}

DEFINE_ELEMENT_FUNC(create_ima_date_t_element)
{
    string_t string;
    ima_date_t *ptr;
    long year;
    long month;
    long day;

    ptr = (ima_date_t *) data;

    acr_get_long(ACR_BIG_ENDIAN, 1, (long *) &ptr->year, &year); 
    acr_get_long(ACR_BIG_ENDIAN, 1, (long *) &ptr->month, &month);
    acr_get_long(ACR_BIG_ENDIAN, 1, (long *) &ptr->day, &day);

    if ((year < 1900) || (year > 9999)) 
        return NULL;
    if ((month < 1) || (month > 12)) 
        return NULL;
    if ((day < 1) || (day > 31)) 
        return NULL;

    sprintf(string, "%04d%02d%02d", (int) year, (int) month, (int) day);

    return acr_create_element_string(get_elid(grp_id, elm_id, ACR_VR_DA),
                                     string);
}

DEFINE_ELEMENT_FUNC(create_ima_time_t_element)
{
    string_t string;
    ima_time_t *ptr;
    long hour;
    long minute;
    long second;
    long msec;

    ptr = (ima_time_t *) data;

    /* Convert data from big endian to native:
     */
    acr_get_long(ACR_BIG_ENDIAN, 1, (long *) &ptr->hour, &hour); 
    acr_get_long(ACR_BIG_ENDIAN, 1, (long *) &ptr->minute, &minute); 
    acr_get_long(ACR_BIG_ENDIAN, 1, (long *) &ptr->second, &second); 
    acr_get_long(ACR_BIG_ENDIAN, 1, (long *) &ptr->msec, &msec); 

    if ((hour < 0) || (hour >= 24)) 
        return NULL;
    if ((minute < 0) || (minute >= 60)) 
        return NULL;
    if ((second < 0) || (second >= 60)) 
        return NULL;
    if ((msec < 0) || (msec > 999)) 
        return NULL;

    sprintf(string, "%02d%02d%02d.%03d", (int) hour, (int) minute, 
            (int) second, (int) msec);
    return acr_create_element_string(get_elid(grp_id, elm_id, ACR_VR_TM), 
                                     string);
}

DEFINE_ELEMENT_FUNC(create_modality_element)
{
    char *string;
    int32_t modality;

    /* Get the appropriate string */

    acr_get_long(ACR_BIG_ENDIAN, 1, (long *) data, (long *) &modality); 
    switch (modality) {
    case 1:
        string = "CT";
        break;
    case 2:
        string = "MR";
        break;
    default:
        return NULL;
    }
    
    /* Return a new element */
    return acr_create_element_string(get_elid(grp_id, elm_id, ACR_VR_CS), 
                                     string);
}

DEFINE_ELEMENT_FUNC(create_sex_element)
{
    char *string;
    int32_t sex;

    /* Get the appropriate string */
    acr_get_long(ACR_BIG_ENDIAN, 1, (long *) data, (long *) &sex); 
    switch (sex) {
    case 1:
        string = "F ";
        break;
    case 2:
        string = "M ";
        break;
    case 3:
        string = "O ";
        break;
    default:
        return NULL;
    }

    /* Return a new element */
    return acr_create_element_string(get_elid(grp_id, elm_id, ACR_VR_CS), 
                                     string);
}

DEFINE_ELEMENT_FUNC(create_age_element)
{
    string_t string;
    int i;
    int is_ok;

    is_ok = 1;

    /* The age string has a fixed length of 4 */
    memcpy(string, data, 4);
    string[4] = '\0';

    for (i = 0; i < 3; i++) {
        if (string[i] < '0' || string[i] > '9') {
            is_ok = 0;
        }
    }
    if (string[3] != 'Y' &&
        string[3] != 'M' && 
        string[3] != 'W' &&
        string[3] != 'D') {
        is_ok = 0;
    }
    if (!is_ok) {
        printf("WARNING: Invalid age field '%s'\n", string);
        return NULL;
    }

    return acr_create_element_string(get_elid(grp_id, elm_id, ACR_VR_AS), 
                                     string);
}

DEFINE_ELEMENT_FUNC(create_slice_order_element)
{
    char *string;
    ima_slice_order_t slice_order;

    /* Get the appropriate string */
    acr_get_long(ACR_BIG_ENDIAN, 1, (long *) data, (long *) &slice_order); 
    switch (slice_order) {
    case SO_ASCENDING:
        string = "ASCENDING ";
        break;
    case SO_DESCENDING:
        string = "DESCENDING ";
        break;
    case SO_INTERLEAVED:
        string = "INTERLEAVED ";
        break;
    case SO_NONE:
        string = "NONE ";
        break;
    default:
        string = "UNDEFINED ";
        break;
    }

    /* Return a new element */
    return acr_create_element_string(get_elid(grp_id, elm_id, ACR_VR_CS), 
                                     string);
}

DEFINE_ELEMENT_FUNC(create_pixel_spacing_t_element)
{
    pixel_spacing_t *ptr;
    string_t string;
    double row;
    double col;

    /* Get the pixel sizes */
    ptr = (pixel_spacing_t *) data;

    /* Convert from big endian to native format */
    acr_get_double(ACR_BIG_ENDIAN, 1, (double *) &ptr->row, &row); 
    acr_get_double(ACR_BIG_ENDIAN, 1, (double *) &ptr->col, &col); 

    sprintf(string, "%.15g\\%.15g", row, col);
    
    return acr_create_element_string(get_elid(grp_id, elm_id, ACR_VR_DS), 
                                     string);
}

DEFINE_ELEMENT_FUNC(create_window_t_element)
{
    window_t *ptr;
    string_t string;
    long x;
    long y;

    ptr = (window_t *) data;    /* Get the window info */

    /* Convert from big endian to native format */
    acr_get_long(ACR_BIG_ENDIAN, 1, (long *) &ptr->x, &x); 
    acr_get_long(ACR_BIG_ENDIAN, 1, (long *) &ptr->y, &y); 
    
    sprintf(string, "%ld\\%ld", x, y);

    return acr_create_element_string(get_elid(grp_id, elm_id, ACR_VR_IS), 
                                     string);
}

DEFINE_ELEMENT_FUNC(create_ima_vector_t_element)
{
    ima_vector_t *ptr;
    string_t string;
    double x, y, z;

    /* Get the coordinate */
    ptr = (ima_vector_t *) data;
   
    acr_get_double(ACR_BIG_ENDIAN, 1, (double *) &ptr->x, &x);
    acr_get_double(ACR_BIG_ENDIAN, 1, (double *) &ptr->y, &y);
    acr_get_double(ACR_BIG_ENDIAN, 1, (double *) &ptr->z, &z);

    sprintf(string, "%.15g\\%.15g\\%.15g", x, y, z);

    return acr_create_element_string(get_elid(grp_id, elm_id, ACR_VR_DS),
                                     string);
}

DEFINE_ELEMENT_FUNC(create_laterality_element)
{
    long laterality;
    char *string;

    acr_get_long(ACR_BIG_ENDIAN, 1, (long *) data, &laterality);

    switch (laterality) {
    case 1:
        string = "L "; break;
    case 2:
        string = ""; break;
    case 3:
        string = "R "; break;
    default:
        return NULL;
    }

    return acr_create_element_string(get_elid(grp_id, elm_id, ACR_VR_CS), 
                                     string);
}

DEFINE_ELEMENT_FUNC(create_ima_position_t_element)
{
    ima_position_t position;
    char *string;

    acr_get_long(ACR_BIG_ENDIAN, 1, (long *) data, (long *) &position);
    switch (position) {
    case PP_LEFT:
        string = "HFL"; break;
    case PP_RIGHT:
        string = "HFR"; break;
    case PP_PRONE:
        string = "HFP"; break;
    case PP_SUPINE:
        string = "HFS"; break;
    default:
        string = ""; break;
    }
    return acr_create_element_string(get_elid(grp_id, elm_id, ACR_VR_CS),
                                     string);
}

DEFINE_ELEMENT_FUNC(create_rest_direction_t_element)
{
    ima_rest_direction_t dir;
    char *string;

    acr_get_long(ACR_BIG_ENDIAN, 1, (long *) data, (long *) &dir);
    switch (dir) {
    case RD_HEAD:
        string = "HEAD";
        break;
    case RD_FEET:
        string = "FEET";
        break;
    default:
        string = "";
        break;
    }
    return acr_create_element_string(get_elid(grp_id, elm_id, ACR_VR_CS),
                                     string);
}

DEFINE_ELEMENT_FUNC(create_view_direction_t_element)
{
    ima_view_direction_t dir;
    char *string;

    acr_get_long(ACR_BIG_ENDIAN, 1, (long *) data, (long *) &dir);
    switch (dir) {
    case VD_HEAD:
        string = "HEAD";
        break;
    case VD_FEET:
        string = "FEET";
        break;
    case VD_AtoP:
        string = "AtoP";
        break;
    case VD_LtoR:
        string = "LtoR";
        break;
    case VD_PtoA:
        string = "PtoA";
        break;
    case VD_RtoL:
        string = "RtoL";
        break;
    default:
        string = "";
        break;
    }
    return acr_create_element_string(get_elid(grp_id, elm_id, ACR_VR_CS),
                                     string);
}

static void
copy_to_space(char *dst_ptr, char *src_ptr, int max_chr)
{
    while (*src_ptr != ' ' && *src_ptr != '\0' && max_chr > 0) {
        *dst_ptr++ = *src_ptr++;
        max_chr--;
    }
    *dst_ptr = '\0';
}

DEFINE_ELEMENT_FUNC(create_ima_orientation_t_element)
{
    ima_orientation_t *po_ptr = (ima_orientation_t *) data;
    char y[N_ORIENTATION + 1];
    char x[N_ORIENTATION + 1];
    char z[N_ORIENTATION + 1];
    string_t string;

    copy_to_space(y, po_ptr->y, N_ORIENTATION);
    copy_to_space(x, po_ptr->x, N_ORIENTATION);
    copy_to_space(z, po_ptr->z, N_ORIENTATION);

    sprintf(string, "%s\\%s\\%s", y, x, z);

    return acr_create_element_string(get_elid(grp_id, elm_id, ACR_VR_CS),
                                     string);
}

DEFINE_ELEMENT_FUNC(create_field_of_view_t_element)
{
    ima_field_of_view_t *ptr;
    string_t string;
    double height;
    double width;

    ptr = (ima_field_of_view_t *) data;

    acr_get_double(ACR_BIG_ENDIAN, 1, (double *) &ptr->height, &height);
    acr_get_double(ACR_BIG_ENDIAN, 1, (double *) &ptr->width, &width);

    sprintf(string, "%.15g\\%.15g", height, width);
    return acr_create_element_string(get_elid(grp_id, elm_id, ACR_VR_DS),
                                     string);
}
