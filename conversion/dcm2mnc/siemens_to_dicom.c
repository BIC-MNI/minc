/* ----------------------------- MNI Header -----------------------------------
@NAME       : siemens_to_dicom.c
@DESCRIPTION: File containing routines to read in a Siemens vision internal
              file (.IMA extension) and convert it to a DICOM representation.
@METHOD     : 
@GLOBALS    : 
@CREATED    : July 8, 1997 (Peter Neelin)
@MODIFIED   : $Log: siemens_to_dicom.c,v $
@MODIFIED   : Revision 1.1  2005-02-17 16:38:11  bert
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

#ifndef lint
static char rcsid[]="$Header: /private-cvsroot/minc/conversion/dcm2mnc/siemens_to_dicom.c,v 1.1 2005-02-17 16:38:11 bert Exp $";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dcm2mnc.h"
#include "siemens_header_defs.h"

/* Constants */

#define SIEMENS_IMAGE_OFFSET (6*1024)

#define IMAGE_NDIMS 2

/* Conversion functions that are not defined */
#define create_calculation_mode_t_element NULL
#define create_compression_code_t_element NULL
#define create_contrast_t_element NULL
#define create_data_object_subtype_t_element NULL
#define create_data_set_subtype_t_element NULL
#define create_field_of_view_t_element NULL
#define create_filter_parameter_t_element NULL
#define create_filter_type_image_t_element NULL
#define create_filter_type_t_element NULL
#define create_gate_phase_t_element NULL
#define create_geometry_t_element NULL
#define create_gradient_delay_time_t_element NULL
#define create_image_format_t_element NULL
#define create_laterality_t_element NULL
#define create_measurement_mode_t_element NULL
#define create_object_orientation_t_element NULL
#define create_object_threshold_t_element NULL
#define create_patient_orientation_t_element NULL
#define create_patient_phase_t_element NULL
#define create_patient_position_t_element NULL
#define create_patient_region_t_element NULL
#define create_pixel_quality_code_t_element NULL
#define create_pixel_quality_value_t_element NULL
#define create_rest_direction_t_element NULL
#define create_rotation_direction_t_element NULL
#define create_sar_sed_t_element NULL
#define create_save_code_t_element NULL
#define create_storage_mode_t_element NULL
#define create_target_point_t_element NULL
#define create_view_direction_t_element NULL
#define create_window_style_t_element NULL
#define create_study_type_t_element NULL

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
} Siemens_header_entry;

/* Functions */

#if 0
DEFINE_ELEMENT_FUNC(create_data_set_subtype_t_element)
{
    int i;

    fprintf(stderr, "create_data_set_subtype_t_element(");

    fprintf(stderr, "grp_id %#x elm_id %#x data %#x length %d)\n",
            grp_id, elm_id, data, length);

    length *= sizeof (data_set_subtype_t);
    for (i = 0; i < length; i++) {
        fprintf(stderr, "%02x", *((unsigned char *)data + i));
    }
    fprintf(stderr, " | ");
    for (i = 0; i < length; i++) {
        int c = *((unsigned char *)data + i);
        fprintf(stderr, "%c", isprint(c) ? c : '.');
    }
    fprintf(stderr, "\n");
    return (0);
}

DEFINE_ELEMENT_FUNC(create_contrast_t_element)
{
    int i;

    fprintf(stderr, "create_contrast_t_element(");

    fprintf(stderr, "grp_id %#x elm_id %#x data %#x length %d)\n",
            grp_id, elm_id, data, length);

    length *= sizeof (contrast_t);
    for (i = 0; i < length; i++) {
        fprintf(stderr, "%02x", *((unsigned char *)data + i));
    }
    fprintf(stderr, " | ");
    for (i = 0; i < length; i++) {
        int c = *((unsigned char *)data + i);
        fprintf(stderr, "%c", isprint(c) ? c : '.');
    }
    fprintf(stderr, "\n");
    return (0);
}
#endif

static Acr_Element_Id get_elid(int grp_id, int elm_id, Acr_VR_Type vr_code);

DECLARE_ELEMENT_FUNC(create_char_element);
DECLARE_ELEMENT_FUNC(create_long_element);
DECLARE_ELEMENT_FUNC(create_short_element);
DECLARE_ELEMENT_FUNC(create_double_element);
DECLARE_ELEMENT_FUNC(create_ds_date_t_element);
DECLARE_ELEMENT_FUNC(create_ds_time_t_element);
DECLARE_ELEMENT_FUNC(create_modality_t_element);
DECLARE_ELEMENT_FUNC(create_sex_t_element);
DECLARE_ELEMENT_FUNC(create_order_of_slices_t_element);
DECLARE_ELEMENT_FUNC(create_pixel_size_t_element);
DECLARE_ELEMENT_FUNC(create_windows_t_element);
DECLARE_ELEMENT_FUNC(create_image_location_t_element);

/* Define the table of header values */
siemens_header_t Siemens_header; /* Must define this first */
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
siemens_to_dicom(const char *filename, int read_image)
{
    FILE *fp;
    Siemens_header_entry *entry;
    Acr_Group group_list;
    Acr_Element element;
    long image_size;
    long pixel_size;
    void *image;
    double flip_angle;
    short rows_in;
    short cols_in;
    short rows;
    short cols;

#ifdef PRINT_OFFSET_TABLE
    void *header_ptr;           /* debug junk */
    void *data_ptr;             /* debug junk */
    long offset;                /* debug junk */
#endif

    /* Open the file */
    if ((fp = fopen(filename, "rb")) == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        return NULL;
    }

    /* Read in the header */
    if (fread(&Siemens_header, sizeof(Siemens_header), 1, fp) != 1) {
        fprintf(stderr, "Error reading header in %s\n", filename);
        fclose(fp);
        return NULL;
    }

    /* Get the image if it is needed */
    if (read_image) {

        /* Figure out how much space we need for the image */
        pixel_size = 2;         /* Apparently this never changes?? */

        /* need to byte swap row/col values if needed */

        rows_in = Siemens_header.G28.Pre.Rows;
        acr_get_short(ACR_BIG_ENDIAN, 1, &rows_in, &rows);
        cols_in = Siemens_header.G28.Pre.Columns;
        acr_get_short(ACR_BIG_ENDIAN, 1, &cols_in, &cols);


        image_size = rows * cols;

        image = malloc(pixel_size * image_size);
        CHKMEM(image);

        /* Read in the image */
        if (fseek(fp, (long) SIEMENS_IMAGE_OFFSET, SEEK_SET)) {
            fprintf(stderr, "Error finding image in %s\n", filename);
            fclose(fp);
            return NULL;
        }
        if (fread(image, pixel_size, image_size, fp) != image_size) {
            fprintf(stderr, "Error reading image in %s\n", filename);
            fclose(fp);
            return NULL;
        }

    }         /* If read_image */

    /* Close the file */
    fclose(fp);

    /* Loop through the header table, creating a header */
    group_list = NULL;
    for (entry = Siemens_header_table; entry->data != NULL; entry++) {

#ifdef PRINT_OFFSET_TABLE
        data_ptr = entry->data;
        header_ptr = &Siemens_header;
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

    if (acr_find_int(group_list, SPI_Number_of_slices_nominal, 1) > 1) {
        short acq_cols;

        acq_cols = acr_find_short(group_list, SPI_Acquisition_columns,
                                  acr_find_short(group_list, ACR_Columns, 1));

        acr_insert_long(&group_list, EXT_Mosaic_rows, 
                        acr_find_int(group_list, ACR_Rows, 1));

        acr_insert_long(&group_list, EXT_Mosaic_columns, 
                        acr_find_int(group_list, ACR_Columns, 1));

        acr_insert_long(&group_list, EXT_Slices_in_file, 
                        acr_find_int(group_list, SPI_Number_of_slices_nominal, 1));
        acr_insert_short(&group_list, EXT_Sub_image_rows, acq_cols);
        acr_insert_short(&group_list, EXT_Sub_image_columns, acq_cols);

        /* We need to set up the ACR_Acquisition (and 
         * ACR_Acquisitions_in_series) objects to make everyone happy.
         *
         * TODO: This appears to work for SOME IMA files, but it may
         * not be correct for all sequences.
         */
        acr_insert_long(&group_list, ACR_Acquisition, 
                        acr_find_int(group_list, ACR_Image, 1));

        acr_insert_long(&group_list, ACR_Acquisitions_in_series, 
                        acr_find_int(group_list, ACR_Nr_of_averages, 1));
    }

    /* Insert a series number */
    acr_insert_numeric(&group_list, ACR_Series, 1.0);

    /* Insert appropriate image position and orientation information */
    update_coordinate_info(group_list);


    /* Add the image if it is needed */
    if (read_image) {

        /* Insert the image location */
        acr_insert_short(&group_list, ACR_Image_location, ACR_IMAGE_GID);

        /* Add the image. We don't byte-swap here since it will be done
           automatically when the data is written out. */
        element = acr_create_element(ACR_IMAGE_GID, ACR_IMAGE_EID, ACR_VR_OW,
                                     image_size * pixel_size, image);
        acr_insert_element_into_group_list(&group_list, element);

        /* explicitly label image data as big-endian */
        acr_set_element_byte_order(element, ACR_BIG_ENDIAN);

    }        /* If read_image */

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
    int nrows, ncolumns, idim;
    double coord[WORLD_NDIMS], row[WORLD_NDIMS], column[WORLD_NDIMS];
    double pixel_spacing[IMAGE_NDIMS];
    char string[256];

    /* Look for the row vector */
    element = acr_find_group_element(group_list, SPI_Image_row);
    if ((element == NULL) ||
        (acr_get_element_numeric_array(element, WORLD_NDIMS, row) 
         != WORLD_NDIMS)) {
        row[0] = 1.0; 
        row[1] = 0.0; 
        row[2] = 0.0;
    }

    /* Look for the column vector */
    element = acr_find_group_element(group_list, SPI_Image_column);
    if ((element == NULL) ||
        (acr_get_element_numeric_array(element, WORLD_NDIMS, column) 
         != WORLD_NDIMS)) {
        column[0] = 0.0; 
        column[1] = 1.0; 
        column[2] = 0.0;
    }

    /* Put in the dicom orientation (patient) field */
    sprintf(string, "%.15g\\%.15g\\%.15g\\%.15g\\%.15g\\%.15g",
            row[0], -row[1], -row[2], 
            column[0], -column[1], -column[2]);
    acr_insert_string(&group_list, ACR_Image_orientation_patient, string);
    
    /* Look for the position */
    element = acr_find_group_element(group_list, SPI_Image_position);
    if ((element == NULL) ||
        (acr_get_element_numeric_array(element, WORLD_NDIMS, coord) 
         != WORLD_NDIMS)) {
        coord[0] = 0.0; 
        coord[1] = 0.0; 
        coord[2] = 0.0;
    }

    /* Get the number of rows and columns */
    nrows = acr_find_int(group_list, ACR_Rows, 0);
    ncolumns = acr_find_int(group_list, ACR_Columns, 0);
    if ((nrows <= 0) || (ncolumns <= 0)) {
        fprintf(stderr, "Illegal image size in Siemens file\n");
        exit(1);
    }

    /* Get the pixel size */
    element = acr_find_group_element(group_list, ACR_Pixel_size);
    if ((element == NULL) ||
        (acr_get_element_numeric_array(element, IMAGE_NDIMS, pixel_spacing) 
         != IMAGE_NDIMS)) {
        pixel_spacing[0] = pixel_spacing[1] = 1.0;
    }

    /* Calculate the position of the first pixel. This coordinate is still in 
       the Siemens space, not dicom space and will need to be flipped. Note 
       that ncolumns is used with row, since they are the size and unit 
       vector of the same dimension. */
    for (idim = 0; idim < WORLD_NDIMS; idim++) {
        coord[idim] -= 
            pixel_spacing[0] * ((double) ncolumns - 1.0) / 2.0 * row[idim] +
            pixel_spacing[1] * ((double) nrows - 1.0) / 2.0 * column[idim];
    }
    sprintf(string, "%.15g\\%.15g\\%.15g", coord[0], -coord[1], -coord[2]);
    acr_insert_string(&group_list, ACR_Image_position_patient, string);
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
       sure that there is a room for an additional NUL if necessary */
    for (i=0; (i < length-1) && (old[i] != '\0'); i++) 
        ;
    newsize = ((old[i] == '\0') ? i+1 : length + 1);
    oldsize = newsize - 1;
    if ((newsize % 2) != 1) 
        newsize++;

    /* Copy the string, making sure that there is a NUL on the end */
    new = malloc(newsize);
    CHKMEM(new);
    for (i=0; i < newsize-1; i++) {
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
                                      (long) data_out); 
}

DEFINE_ELEMENT_FUNC(create_short_element)
{
    unsigned short data_out;

    acr_get_short(ACR_BIG_ENDIAN, 1, data, &data_out);

    return acr_create_element_short(get_elid(grp_id, elm_id, ACR_VR_US), 
                                    (unsigned short) data_out);
}

DEFINE_ELEMENT_FUNC(create_double_element)
{
    double data_out;
  
    acr_get_double(ACR_BIG_ENDIAN, 1, data, &data_out);

    return acr_create_element_numeric(get_elid(grp_id, elm_id, ACR_VR_DS),
                                      (double) data_out);
}

DEFINE_ELEMENT_FUNC(create_ds_date_t_element)
{
    char string[20];
    ds_date_t *ptr;

    long year;
    long month;
    long day;

    long year_in;
    long month_in;
    long day_in;

    ptr = (ds_date_t *) data;

    year_in = ptr->Year;
    acr_get_long(ACR_BIG_ENDIAN, 1, (long *) &year_in, &year); 
	
    month_in = ptr->Month;
    acr_get_long(ACR_BIG_ENDIAN, 1, (long *) &month_in, &month);
	
    day_in = ptr->Day;
    acr_get_long(ACR_BIG_ENDIAN, 1, (long *) &day_in, &day);

    if ((year < 0) || (year > 9999)) 
        return NULL;
    if ((month < 0) || (month > 12)) 
        return NULL;
    if ((day < 0) || (day > 40)) 
        return NULL;

    sprintf(string, "%04d%02d%02d", (int) year, (int) month, (int) day);

   return acr_create_element_string(get_elid(grp_id, elm_id, ACR_VR_DA),
                                    string);
}

DEFINE_ELEMENT_FUNC(create_ds_time_t_element)
{
    char string[20];
    ds_time_t *ptr;

    long hour;
    long minute;
    long second;
    long fraction;

    long hour_in;
    long minute_in;
    long second_in;
    long fraction_in;

    ptr = (ds_time_t *) data;
    hour_in = ptr->Hour;
    acr_get_long(ACR_BIG_ENDIAN, 1, (long *) &hour_in, &hour); 
    minute_in = ptr->Minute;
    acr_get_long(ACR_BIG_ENDIAN, 1, (long *) &minute_in, &minute); 
    second_in = ptr->Second;
    acr_get_long(ACR_BIG_ENDIAN, 1, (long *) &second_in, &second); 
    fraction_in = ptr->Fraction;
    acr_get_long(ACR_BIG_ENDIAN, 1, (long *) &fraction_in, &fraction); 

    if ((hour < 0) || (hour > 24)) 
        return NULL;
    if ((minute < 0) || (minute > 60)) 
        return NULL;
    if ((second < 0) || (second > 60)) 
        return NULL;
    if ((fraction < 0) || (fraction > 999)) 
        return NULL;

    sprintf(string, "%02d%02d%02d.%03d", (int) hour, (int) minute, 
            (int) second, (int) fraction);
    return acr_create_element_string(get_elid(grp_id, elm_id, ACR_VR_TM), 
                                     string);
}

DEFINE_ELEMENT_FUNC(create_modality_t_element)
{
    char *string;
    modality_t *ptr_in;
    modality_t modality;

    /* Get the appropriate string */

    ptr_in = (modality_t *) data;
    acr_get_long(ACR_BIG_ENDIAN, 1, (long *) ptr_in, (long *) &modality); 

    switch (modality) {
    case Modality_CT:
        string = "CT";
        break;
    case Modality_MR:
        string = "MR";
        break;
    default:
        return NULL;
    }
    
    /* Return a new element */
    return acr_create_element_string(get_elid(grp_id, elm_id, ACR_VR_CS), 
                                     string);
}

DEFINE_ELEMENT_FUNC(create_sex_t_element)
{
    char *string;
    sex_t *ptr_in;
    sex_t sex;

    /* Get the appropriate string */
    ptr_in = (sex_t *) data;
    acr_get_long(ACR_BIG_ENDIAN, 1, (long *) ptr_in, (long *) &sex); 
    switch (sex) {
    case Sex_F:
        string = "F ";
        break;
    case Sex_M:
        string = "M ";
        break;
    case Sex_O:
        string = "O ";
        break;
    default:
        return NULL;
    }

    /* Return a new element */
    return acr_create_element_string(get_elid(grp_id, elm_id, ACR_VR_CS), 
                                     string);
}

DEFINE_ELEMENT_FUNC(create_order_of_slices_t_element)
{
    char *string;
    order_of_slices_t *ptr_in;
    order_of_slices_t order_of_slices;

    /* Get the appropriate string */
    ptr_in = (order_of_slices_t *) data;
    acr_get_long(ACR_BIG_ENDIAN, 1, (long *) ptr_in, (long *) &order_of_slices); 
    switch (order_of_slices) {
    case Slice_Order_ASCENDING:
        string = "ASCENDING ";
        break;
    case Slice_Order_DECREASING:
        string = "DESCENDING ";
        break;
    case Slice_Order_INTERLEAVED:
        string = "INTERLEAVED ";
        break;
    case Slice_Order_NONE:
        string = "NONE ";
        break;
    case Slice_Order_UNDEFINED:
        string = "UNDEFINED ";
        break;
    default:
        return NULL;
    }

    /* Return a new element */
    return acr_create_element_string(get_elid(grp_id, elm_id, ACR_VR_CS), 
                                     string);
}

DEFINE_ELEMENT_FUNC(create_pixel_size_t_element)
{
    pixel_size_t *ptr;
    char string[64];

    double row_in;
    double col_in;

    double row;
    double col;

    /* Get the pixel sizes */
    ptr = (pixel_size_t *) data;

    row_in = ptr->Row;
    acr_get_double(ACR_BIG_ENDIAN, 1, (double *) &row_in, &row); 
    col_in = ptr->Col;
    acr_get_double(ACR_BIG_ENDIAN, 1, (double *) &col_in, &col); 

    sprintf(string, "%.15g\\%.15g", row, col);
    
    return acr_create_element_string(get_elid(grp_id, elm_id, ACR_VR_DS), 
                                     string);
}

DEFINE_ELEMENT_FUNC(create_windows_t_element)
{
    windows_t *ptr;
    char string[64];

    long x_in;
    long y_in;

    long x;
    long y;

    /* Get the window info */
    ptr = (windows_t *) data;

    x_in = ptr->X;
    acr_get_long(ACR_BIG_ENDIAN, 1, (long *) &x_in, &x); 
    y_in = ptr->Y;
    acr_get_long(ACR_BIG_ENDIAN, 1, (long *) &y_in, &y); 
    
    sprintf(string, "%ld\\%ld", x, y);

    return acr_create_element_string(get_elid(grp_id, elm_id, ACR_VR_IS), 
                                     string);
}

DEFINE_ELEMENT_FUNC(create_image_location_t_element)
{
    image_location_t *ptr;
    char string[64];

    double sag;
    double cor;
    double tra;

    double sag_in;
    double cor_in;
    double tra_in;

    /* Get the coordinate */
    ptr = (image_location_t *) data;
   
    sag_in = ptr->Sag;
    acr_get_double(ACR_BIG_ENDIAN, 1, (double *) &sag_in, &sag);
    cor_in = ptr->Cor;
    acr_get_double(ACR_BIG_ENDIAN, 1, (double *) &cor_in, &cor);
    tra_in = ptr->Tra;
    acr_get_double(ACR_BIG_ENDIAN, 1, (double *) &tra_in, &tra);

    sprintf(string, "%.15g\\%.15g\\%.15g", sag, cor, tra);

    return acr_create_element_string(get_elid(grp_id, elm_id, ACR_VR_DS),
                                     string);
}





