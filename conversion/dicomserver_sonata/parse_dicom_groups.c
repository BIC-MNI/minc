/* ----------------------------- MNI Header -----------------------------------
@NAME       : parse_dicom_groups.c
@DESCRIPTION: Routine to parse dicom file - replicates postconditions
              of save_transferred_object.c
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June 2001 (Rick Hoge)
@MODIFIED   : 
 * $Log: parse_dicom_groups.c,v $
 * Revision 1.1  2003-08-15 19:52:55  leili
 * Initial revision
 *
 * Revision 1.2  2002/03/19 13:13:56  rhoge
 * initial working mosaic support - I think time is scrambled though.
 *
 * Revision 1.1  2001/12/31 17:27:01  rhoge
 * adding file to repository - works for numa4 non-mos files now
 *
---------------------------------------------------------------------------- */

#include <dicomserver.h>

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
public void parse_dicom_groups(Acr_Group group_list, Data_Object_Info *data_info)
{
   Acr_Group group;
   Acr_Element element;
   char patient_name[256];

   unsigned short AcqMat[4];
   unsigned short freq_rows;
   unsigned short freq_cols;
   unsigned short phase_rows;
   unsigned short phase_cols;

   int maxlen = sizeof(Cstring) - 1;

   // Get info to construct unique identifiers for study, series/acq
   // for file processing
   get_identification_info(group_list,
                           &(data_info->study_id), &(data_info->acq_id),
                           &(data_info->rec_num), &(data_info->image_type));

   // Get number of echos, echo number, number of dynamic scans and 
   // dynamic_scan_number
   data_info->num_echoes =
      acr_find_int(group_list, SPI_Number_of_echoes, 999);
   data_info->echo_number =
      acr_find_int(group_list, ACR_Echo_number, 999);
   data_info->num_dyn_scans =
      acr_find_int(group_list, ACR_Acquisitions_in_series, 999);
   data_info->dyn_scan_number =
      acr_find_int(group_list, ACR_Acquisition, 999);
   data_info->global_image_number =
      acr_find_int(group_list, ACR_Image, 999);

   /* rhoge:
      new info added to data_info by rhoge: nominal number of slices;
      this is used in detection of a stream of files with the same
      acquisition ID number in which there are more files than
      slices.  If the number of signal averages is greater than one,
      we will assume that this means the acquisition loop was used for
      dynamic scanning.  

      WARNINGS:  the same thing may need to be done with `number of
      partitions' for it to work with 3D scans  */

   data_info->num_slices_nominal =
      acr_find_int(group_list, SPI_Number_of_slices_nominal, 999);
   data_info->slice_number = 999;

   // identification info needed to generate unique session id
   // for file names
   data_info->study_date =
     acr_find_int(group_list, ACR_Study_date, 999); 
   data_info->study_time =
     acr_find_int(group_list, ACR_Study_time, 999); 
   data_info->scanner_serialno =
     acr_find_int(group_list, ACR_Device_serial_number, 999); 

   // identification info needed to determine if mosaics used 

   element = acr_find_group_element(group_list,ACR_Acquisition_matrix);
   acr_get_element_short_array(element,4,AcqMat);

   freq_rows = AcqMat[0];
   freq_cols = AcqMat[1];

   phase_rows = AcqMat[2];
   phase_cols = AcqMat[3];

   // rows in acq matrix is larger of freq rows and freq columns:
   data_info->acq_rows = ( freq_rows > freq_cols ? freq_rows : freq_cols );
   // all images are square, at this time
   data_info->acq_cols = data_info->acq_rows;

   data_info->rec_rows = acr_find_int(group_list,ACR_Rows, 999);
   data_info->rec_cols = acr_find_int(group_list,ACR_Columns, 999);

   data_info->num_mosaic_rows=acr_find_int(group_list,EXT_Mosaic_rows, 999);
   data_info->num_mosaic_cols=acr_find_int(group_list,EXT_Mosaic_columns,999);
   data_info->num_slices_in_file=
     acr_find_int(group_list,EXT_Slices_in_file,999);

   // sequence, protocol names (useful for debugging):

   (void) strncpy(data_info->sequence_name,
		  acr_find_string(group_list,ACR_Sequence_name,""),maxlen);
   (void) strncpy(data_info->protocol_name,
		  acr_find_string(group_list,ACR_Protocol_name,""),maxlen);

   return;

}


