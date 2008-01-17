/* ----------------------------- MNI Header -----------------------------------
@NAME       : siemens_to_dicom.c
@DESCRIPTION: File containing routines to read in a Siemens vision internal
              file and convert it to dicom.
@METHOD     : 
@GLOBALS    : 
@CREATED    : July 8, 1997 (Peter Neelin)
@MODIFIED   : $Log: siemens_to_dicom.c,v $
@MODIFIED   : Revision 1.3  2008-01-17 02:33:01  rotor
@MODIFIED   :  * removed all rcsids
@MODIFIED   :  * removed a bunch of ^L's that somehow crept in
@MODIFIED   :  * removed old (and outdated) BUGS file
@MODIFIED   :
@MODIFIED   : Revision 1.2  2008/01/12 19:08:14  stever
@MODIFIED   : Add __attribute__ ((unused)) to all rcsid variables.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DS_FUNCTIONS
#include <ds_include_files.h>

#include <acr_nema.h>

/* Constants */
#ifndef public
#  define public
#endif
#ifndef private
#  define private static
#endif
#ifndef EXIT_SUCCESS
#  define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
#  define EXIT_FAILURE 1
#endif
#ifndef SEEK_SET
#  define SEEK_SET 0
#endif

#define ACR_IMAGE_GID 0x7fe0
#define ACR_IMAGE_EID 0x10
#define SIEMENS_IMAGE_OFFSET (6*1024)

#define WORLD_NDIMS 3
#define IMAGE_NDIMS 2

/* Macros */
#define MALLOC(size) ((void *) malloc(size))
#define FREE(ptr) free(ptr)
#define REALLOC(ptr, size) ((void *) realloc(ptr, size))
#define CALLOC(nelem, elsize) ((void *) calloc(nelem, elsize))

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
/* #define create_order_of_slices_t_element NULL */
#define create_patient_orientation_t_element NULL
#define create_patient_phase_t_element NULL
#define create_patient_position_t_element NULL
#define create_patient_region_t_element NULL
#define create_pixel_quality_code_t_element NULL
#define create_pixel_quality_value_t_element NULL
#define create_reference_t_element NULL
#define create_rest_direction_t_element NULL
#define create_rotation_direction_t_element NULL
#define create_sar_sed_t_element NULL
#define create_save_code_t_element NULL
#define create_storage_mode_t_element NULL
#define create_study_type_t_element NULL
#define create_target_point_t_element NULL
#define create_view_direction_t_element NULL
#define create_window_style_t_element NULL

/* Define element id's */
DEFINE_ELEMENT(static, ACR_Rows                       , 0x0028, 0x0010, US);
DEFINE_ELEMENT(static, ACR_Columns                    , 0x0028, 0x0011, US);
DEFINE_ELEMENT(static, ACR_Flip_angle                 , 0x0018, 0x1314, DS);
DEFINE_ELEMENT(static, SPI_Flip_angle                 , 0x0019, 0x1260, DS);
DEFINE_ELEMENT(static, ACR_Series                     , 0x0020, 0x0011, IS);
DEFINE_ELEMENT(static, ACR_Image_position             , 0x0020, 0x0032, DS);
DEFINE_ELEMENT(static, ACR_Image_orientation          , 0x0020, 0x0037, DS);
DEFINE_ELEMENT(static, ACR_Image_location             , 0x0028, 0x0200, IS);
DEFINE_ELEMENT(static, SPI_Image_position             , 0x0021, 0x1160, DS);
DEFINE_ELEMENT(static, SPI_Image_row                  , 0x0021, 0x116a, DS);
DEFINE_ELEMENT(static, SPI_Image_column               , 0x0021, 0x116b, DS);
DEFINE_ELEMENT(static, ACR_Pixel_spacing              , 0x0028, 0x0030, DS);


/* Types */
typedef Acr_Element (*Create_Element_Function)
     (int group_id, int element_id, void *data, int length);
typedef struct {
   int group_id;
   int element_id;
   void *data;
   Create_Element_Function function;
   int length;
} Siemens_header_entry;

/* Functions */
public Acr_Group siemens_to_dicom(char *filename, int read_image);
public void update_coordinate_info(Acr_Group group_list);
private Acr_Element_Id get_elid(int group_id, int element_id, 
                                Acr_VR_Type vr_code);
private Acr_Element create_char_element(int group_id, int element_id,
                                        void *data, int length);
private Acr_Element create_long_element(int group_id, int element_id,
                                        void *data, int length);
private Acr_Element create_short_element(int group_id, int element_id,
                                         void *data, int length);
private Acr_Element create_double_element(int group_id, int element_id,
                                        void *data, int length);
private Acr_Element create_ds_date_t_element(int group_id, int element_id,
                                             void *data, int length);
private Acr_Element create_ds_time_t_element(int group_id, int element_id,
                                             void *data, int length);
private Acr_Element create_modality_t_element(int group_id, int element_id,
                                                 void *data, int length);
private Acr_Element create_sex_t_element(int group_id, int element_id,
                                            void *data, int length);
private Acr_Element create_order_of_slices_t_element(int group_id, 
						     int element_id,
						     void *data, int length);
private Acr_Element create_pixel_size_t_element(int group_id, int element_id,
                                                void *data, int length);
private Acr_Element create_windows_t_element(int group_id, int element_id,
                                             void *data, int length);
private Acr_Element create_image_location_t_element(int group_id, 
                                                    int element_id,
                                                    void *data, int length);

/* Define the table of header values */
header_t Siemens_header;
#include <siemens_header_table.h>

/* flag to print offset table, useful in debugging byte pad issues
   with Linux/sun porting */
/* #define PRINT_OFFSET_TABLE */

/* ----------------------------- MNI Header -----------------------------------
@NAME       : siemens_to_dicom
@INPUT      : filename - name of Siemens internal file
              read_image - if TRUE, then the image is added to the group list,
                 otherwise it is not read in.
@OUTPUT     : (none)
@RETURNS    : Acr-nema group list containing contents of Siemens file
@DESCRIPTION: Function to read in a siemens internal format file and
              store it in an ACR-NEMA group list
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : July 8, 1997 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public Acr_Group siemens_to_dicom(char *filename, int read_image)
{
   FILE *fp;
   Siemens_header_entry *entry;
   Acr_Group group_list;
   Acr_Element element;
   long image_size, pixel_size;
   void *image;
   double flip_angle;

   short rows_in;
   short cols_in;

   short rows;
   short cols;

#ifdef PRINT_OFFSET_TABLE
   void *header_ptr; /* debug junk */
   void *data_ptr;   /* debug junk */
   long offset;       /* debug junk */
#endif

   /* Open the file */
   if ((fp = fopen(filename, "r")) == NULL) {
      (void) fprintf(stderr, "Error opening file %s\n", filename);
      return NULL;
   }

   /* Read in the header */
   if (fread(&Siemens_header, sizeof(Siemens_header), 1, fp) != 1) {
      (void) fprintf(stderr, "Error reading header in %s\n", filename);
      (void) fclose(fp);
      return NULL;
   }

   /* Get the image if it is needed */
   if (read_image) {

      /* Figure out how much space we need for the image */
      pixel_size = 2;

      rows_in = Siemens_header.G28.Pre.Rows;
      acr_get_short(ACR_BIG_ENDIAN,1, (short *) &rows_in, &rows);
      cols_in = Siemens_header.G28.Pre.Columns;
      acr_get_short(ACR_BIG_ENDIAN,1, (short *) &cols_in, &cols);

      /* need to byte swap row/col values if needed */

      image_size = rows*cols;

      image = MALLOC((size_t) pixel_size * image_size);

      /* Read in the image */
      if (fseek(fp, (long) SIEMENS_IMAGE_OFFSET, SEEK_SET)) {
         (void) fprintf(stderr, "Error finding image in %s\n", filename);
         (void) fclose(fp);
         return NULL;
      }
      if (fread(image, pixel_size, image_size, fp) != image_size) {
         (void) fprintf(stderr, "Error reading image in %s\n", filename);
         (void) fclose(fp);
         return NULL;
      }

   }         /* If read_image */

   /* Close the file */
   (void) fclose(fp);

   /* Loop through the header table, creating a header */
   group_list = NULL;
   for (entry = Siemens_header_table; entry->data != NULL; entry++) {

#ifdef PRINT_OFFSET_TABLE
     data_ptr = entry->data;
     header_ptr = &Siemens_header;
     offset = (long) data_ptr - (long) header_ptr;
     printf("DEBUG:  group = 0x%x, element = 0x%x, offset = 0x%x, length = 0x%x\n",
     entry->group_id,entry->element_id,offset,entry->length);
#endif

     if (entry->function == NULL) {
       continue;
     }
      element = entry->function(entry->group_id, entry->element_id,
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
      element = acr_create_element(ACR_IMAGE_GID, ACR_IMAGE_EID, 
                                   ACR_VR_OW,
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
public void update_coordinate_info(Acr_Group group_list)
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
      row[0] = 1.0; row[1] = 0.0; row[2] = 0.0;
   }

   /* Look for the column vector */
   element = acr_find_group_element(group_list, SPI_Image_column);
   if ((element == NULL) ||
       (acr_get_element_numeric_array(element, WORLD_NDIMS, column) 
        != WORLD_NDIMS)) {
      column[0] = 0.0; column[1] = 1.0; column[2] = 0.0;
   }

   /* Put in the dicom orientation (patient) field */
   (void) sprintf(string, "%.15g\\%.15g\\%.15g\\%.15g\\%.15g\\%.15g",
                  row[0], -row[1], -row[2], 
                  column[0], -column[1], -column[2]);
   acr_insert_string(&group_list, ACR_Image_orientation, string);

   /* Look for the position */
   element = acr_find_group_element(group_list, SPI_Image_position);
   if ((element == NULL) ||
       (acr_get_element_numeric_array(element, WORLD_NDIMS, coord) 
        != WORLD_NDIMS)) {
      coord[0] = 0.0; coord[1] = 0.0; coord[2] = 0.0;
   }

   /* Get the number of rows and columns */
   nrows = acr_find_int(group_list, ACR_Rows, 0);
   ncolumns = acr_find_int(group_list, ACR_Columns, 0);
   if ((nrows <= 0) || (ncolumns <= 0)) {
      (void) fprintf(stderr, "Illegal image size in Siemens file\n");
      exit(EXIT_FAILURE);
   }

   /* Get the pixel size */
   element = acr_find_group_element(group_list, ACR_Pixel_spacing);
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
   (void) sprintf(string, "%.15g\\%.15g\\%.15g",
                  coord[0], -coord[1], -coord[2]);
   acr_insert_string(&group_list, ACR_Image_position, string);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_<type>_element
@INPUT      : group_id
              element_id
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

private Acr_Element_Id get_elid(int group_id, int element_id, 
                                Acr_VR_Type vr_code)
{
   static struct Acr_Element_Id elid_struct = {0, 0, ACR_VR_UNKNOWN};
   elid_struct.group_id = group_id;
   elid_struct.element_id = element_id;
   elid_struct.vr_code = vr_code;
   return &elid_struct;
}

private Acr_Element create_char_element(int group_id, int element_id,
                                        void *data, int length)
/* ARGSUSED */
{
   char *old, *new;
   int oldsize, newsize, i;

   /* Get a pointer to the old string */
   old = (char *) data;

   /* Figure out the length of the new string up to the first NUL. Make 
      sure that there is a room for an additional NUL if necessary */
   for (i=0; (i < length-1) && (old[i] != '\0'); i++) {}
   newsize = ((old[i] == '\0') ? i+1 : length + 1);
   oldsize = newsize - 1;
   if ((newsize % 2) != 1) newsize++;

   /* Copy the string, making sure that there is a NUL on the end */
   new = MALLOC(newsize);
   for (i=0; i < newsize-1; i++) {
      if (i < oldsize)
         new[i] = old[i];
      else
         new[i] = ' ';
   }
   new[newsize-1] = '\0';

   /* Create the element */
   return acr_create_element(group_id, element_id, ACR_VR_ST, 
                             (long) newsize-1, (void *) new);
}

private Acr_Element create_long_element(int group_id, int element_id,
                                        void *data, int length)

/* modified by rhoge to byte swap if needed */

/* ARGSUSED */
{
  long data_out;

  acr_get_long(ACR_BIG_ENDIAN,1, data, &data_out);

   return acr_create_element_numeric
      (get_elid(group_id, element_id, ACR_VR_IS), 
       (long) data_out); 
}

private Acr_Element create_short_element(int group_id, int element_id,
                                         void *data, int length)

/* modified by rhoge to byte swap if needed */

/* ARGSUSED */
{
  unsigned short data_out;

  acr_get_short(ACR_BIG_ENDIAN,1, data, &data_out);

  return acr_create_element_short
    (get_elid(group_id, element_id, ACR_VR_US), 
     (unsigned short) data_out);
}

private Acr_Element create_double_element(int group_id, int element_id,
                                        void *data, int length)

/* modified by rhoge to byte swap if needed */

/* ARGSUSED */
{
  double data_out;
  
  acr_get_double(ACR_BIG_ENDIAN,1, data, &data_out);

   return acr_create_element_numeric
     (get_elid(group_id, element_id, ACR_VR_DS), (double) data_out);
}

private Acr_Element create_ds_date_t_element(int group_id, int element_id,
                                             void *data, int length)

/* modified by rhoge to byte swap if needed */

/* ARGSUSED */
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
   acr_get_long(ACR_BIG_ENDIAN,1, (long *) &year_in, &year); 
	
   month_in = ptr->Month;
   acr_get_long(ACR_BIG_ENDIAN,1, (long *) &month_in, &month);
	
   day_in = ptr->Day;
   acr_get_long(ACR_BIG_ENDIAN,1, (long *) &day_in, &day);

   if ((year < 0) || (year > 9999)) return NULL;
   if ((month < 0) || (month > 12)) return NULL;
   if ((day < 0) || (day > 40)) return NULL;
   (void) sprintf(string, "%04d%02d%02d", 
                  (int) year, (int) month, (int) day);

   return acr_create_element_string
      (get_elid(group_id, element_id, ACR_VR_DA), string);
}

private Acr_Element create_ds_time_t_element(int group_id, int element_id,
                                             void *data, int length)
/* modified by rhoge to byte swap if needed */

/* ARGSUSED */
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
   acr_get_long(ACR_BIG_ENDIAN,1, (long *) &hour_in, &hour); 
   minute_in = ptr->Minute;
   acr_get_long(ACR_BIG_ENDIAN,1, (long *) &minute_in, &minute); 
   second_in = ptr->Second;
   acr_get_long(ACR_BIG_ENDIAN,1, (long *) &second_in, &second); 
   fraction_in = ptr->Fraction;
   acr_get_long(ACR_BIG_ENDIAN,1, (long *) &fraction_in, &fraction); 
   if ((hour < 0) || (hour > 24)) return NULL;
   if ((minute < 0) || (minute > 60)) return NULL;
   if ((second < 0) || (second > 60)) return NULL;
   if ((fraction < 0) || (fraction > 999)) return NULL;
   (void) sprintf(string, "%02d%02d%02d.%03d", hour, minute, second, fraction);
   return acr_create_element_string
      (get_elid(group_id, element_id, ACR_VR_TM), string);
}

private Acr_Element create_modality_t_element(int group_id, int element_id,
                                                 void *data, int length)

/* modified by rhoge to byte swap if needed */

/* ARGSUSED */
{
   char *string;
   modality_t *ptr_in;
   modality_t modality;
   /*   modality_t *ptr; */

   /* Get the appropriate string */

   ptr_in = (modality_t *) data;
   acr_get_long(ACR_BIG_ENDIAN,1, (long *) ptr_in, (long *) &modality); 

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
   return acr_create_element_string
      (get_elid(group_id, element_id, ACR_VR_CS), string);
}

private Acr_Element create_sex_t_element(int group_id, int element_id,
                                            void *data, int length)

/* modified by rhoge to byte swap if needed */
/* ARGSUSED */
{
   char *string;
   sex_t *ptr_in;
   sex_t sex;

   /* Get the appropriate string */
   ptr_in = (sex_t *) data;
   acr_get_long(ACR_BIG_ENDIAN,1, (long *) ptr_in, (long *) &sex); 
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
   return acr_create_element_string
      (get_elid(group_id, element_id, ACR_VR_CS), string);
}

private Acr_Element create_order_of_slices_t_element(int group_id, 
						   int element_id,
						   void *data, int length)

/* modified by rhoge to byte swap if needed */
/* ARGSUSED */
{
   char *string;
   order_of_slices_t *ptr_in;
   order_of_slices_t order_of_slices;

   /* Get the appropriate string */
   ptr_in = (order_of_slices_t *) data;
   acr_get_long(ACR_BIG_ENDIAN,1, (long *) ptr_in, (long *) &order_of_slices); 
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
   return acr_create_element_string
      (get_elid(group_id, element_id, ACR_VR_CS), string);
}

private Acr_Element create_pixel_size_t_element(int group_id, int element_id,
                                                void *data, int length)

/* modified by rhoge to byte swap if needed */

/* ARGSUSED */
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
   acr_get_double(ACR_BIG_ENDIAN,1, (double *) &row_in, &row); 
   col_in = ptr->Col;
   acr_get_double(ACR_BIG_ENDIAN,1, (double *) &col_in, &col); 

   (void) sprintf(string, "%.15g\\%.15g", row, col);

   return acr_create_element_string
      (get_elid(group_id, element_id, ACR_VR_DS), string);
}

private Acr_Element create_windows_t_element(int group_id, int element_id,
                                             void *data, int length)

/* modified by rhoge to byte swap if needed */

/* ARGSUSED */
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
   acr_get_long(ACR_BIG_ENDIAN,1, (long *) &x_in, &x); 
   y_in = ptr->Y;
   acr_get_long(ACR_BIG_ENDIAN,1, (long *) &y_in, &y); 

   (void) sprintf(string, "%ld\\%ld", x, y);

   return acr_create_element_string
      (get_elid(group_id, element_id, ACR_VR_IS), string);
}

private Acr_Element create_image_location_t_element(int group_id, 
                                                    int element_id,
                                                    void *data, int length)
/* modified by rhoge to byte swap if needed */
/* ARGSUSED */
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
   acr_get_double(ACR_BIG_ENDIAN,1, (double *) &sag_in, &sag); 
   cor_in = ptr->Cor;
   acr_get_double(ACR_BIG_ENDIAN,1, (double *) &cor_in, &cor); 
   tra_in = ptr->Tra;
   acr_get_double(ACR_BIG_ENDIAN,1, (double *) &tra_in, &tra); 

   (void) sprintf(string, "%.15g\\%.15g\\%.15g", sag, cor, tra);

   return acr_create_element_string
      (get_elid(group_id, element_id, ACR_VR_DS), string);
}





