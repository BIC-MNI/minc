/* ----------------------------- MNI Header -----------------------------------
@NAME       : siemens_dicom_to_minc.c
@DESCRIPTION: Code to convert a list of Siemens dicom files to minc 
              format.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 28, 1997 (Peter Neelin)
@MODIFIED   : 
 * $Log: siemens_dicom_to_minc.c,v $
 * Revision 1.1  2003-08-15 19:52:55  leili
 * Initial revision
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

extern int Do_logging;
char *pname;
File_Type file_type; /* type of input files */
int Fork;
int N4_OFFSET;

/* ----------------------------- MNI Header -----------------------------------
@NAME       : siemens_dicom_to_minc
@INPUT      : num_files - number of image files
              file_list - list of file names
              minc_file - name of output minc file (NULL means make one
                 up)
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
@GLOBALS    : Do_logging
@CALLS      : 
@CREATED    : November 25, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int siemens_dicom_to_minc(int num_files, char *file_list[], 
                        char *minc_file, int clobber,
                        char *file_prefix, char **output_file_name)
{
   Acr_Group group_list;
   int max_group;
   File_Info *file_info;
   General_Info general_info;
   General_Info general_info_orig;
   Image_Data image;
   int icvid;
   int ifile;
   Mri_Index imri;
   char *out_file_name;
   int isep;

   // variables added by rhoge

   int num_files_expected;
   int num_avg_nominal;
   int num_frames_nominal;
   int idim;
   int frame;
   int num_frames;
   int index;
   Loop_Type loop_type = NONE;
   int subimage;
   int iimage;
   int num_images_allocated;
   Acr_Element big_image;
   Acr_Element small_image;
   Multi_Image multi_image;

   /* Allocate space for the file information */
   file_info = MALLOC(num_files * sizeof(*file_info));
   num_images_allocated = num_files;

   // Last group needed for first pass
   //   max_group = ACR_ACTUAL_IMAGE_GID - 1;
   // we now have to read up to and including the image, 
   // since image pointers are needed in multi_image_init
   max_group = ACR_ACTUAL_IMAGE_GID;

   /* Add all control characters as numeric array separators to handle 
      odd behaviour with Siemens dicom files */
   for (isep=0; isep < 31; isep++) {
      (void) acr_element_numeric_array_separator(isep);
   }

   /* Initialize some values for general info */
   general_info.initialized = FALSE;
   general_info.group_list = NULL;
   for (imri=0; imri < MRI_NDIMS; imri++) {
      general_info.indices[imri] = NULL;
      general_info.coordinates[imri] = NULL;
   }

   // Loop through file list getting information
   // (note that we have to duplicate the handling
   // of multiple images per file in this loop
   // to accumulate dimension sizes correctly)

   // need separate counter for images, since some files may
   // contain more than one image!
   iimage = 0;

   for (ifile=0; ifile < num_files; ifile++) {

     if (!Fork) {
       progress(ifile, num_files, "-Parsing series info");
     }

     // Read the file
     if (file_type == N4DCM) {
       group_list = read_numa4_dicom(file_list[ifile], max_group);
     } else if (file_type == IMA) {
       group_list = read_siemens_dicom(file_list[ifile], max_group);
     }

     // initialize big and small images, if mosaic
     if (acr_find_int(group_list, EXT_Slices_in_file,1)>1) {

       multi_image_init(group_list, &multi_image);

       // if multi images in file, extend file_info list

       num_images_allocated += 
	 acr_find_int(group_list, EXT_Slices_in_file,1) - 1;

       file_info = REALLOC(file_info, 
			   num_images_allocated * sizeof(*file_info));

     }

     // loop over subimages in mosaic
     for(subimage = 0; 
     	 subimage < acr_find_int(group_list, EXT_Slices_in_file,1);
     	 subimage++) {

       // Modify the group list for this image if mosaic
       if (acr_find_int(group_list, EXT_Slices_in_file,1)>1) {
	 multi_image_modify_group_list(group_list,&multi_image,subimage);
       }

       // Get file-specific information
       get_file_info(group_list, &file_info[iimage], &general_info);

       // increment iimage here
       iimage++;
     }

     // Delete the group list
     acr_delete_group_list(group_list);
     // cleanup multi_image struct if used
     if (general_info.num_slices_in_file > 1) {
       multi_image_cleanup(group_list, &multi_image);
     }

   }

   // Sort the dimensions
   sort_dimensions(&general_info);

   // Create the output file
   if (general_info.initialized) {
      icvid = create_minc_file(minc_file, clobber, &general_info,
                               file_prefix, &out_file_name,
			       loop_type);
   }
   if (output_file_name != NULL)
      *output_file_name = out_file_name;

   // Check that we found the general info and that the minc file was
   // created okay
   if ((!general_info.initialized) || (icvid == MI_ERROR)) {
      if (general_info.initialized) {
         (void) fprintf(stderr, "Error creating minc file %s.\n",
                        out_file_name);
      }
      free_info(&general_info, file_info, num_files);
      FREE(file_info);
      return EXIT_FAILURE;
   }

   if (Do_logging > HIGH_LOGGING) { /* rhoge */
     fprintf(stderr,"\nAbout to enter minc write loop...\n");
   }	  


   // Loop through the files again and put images into the minc file
   iimage = 0;
   for (ifile=0; ifile < num_files; ifile++) {

     if (!Fork) {
       progress(ifile, num_files, "-Creating minc file");
     }

     // Check that we have a valid file 
     if (!file_info[ifile].valid) {
       continue;
     }
     
     // Read the file 
     if (file_type == N4DCM) {
       group_list = read_numa4_dicom(file_list[ifile], max_group);
     } else if (file_type == IMA) {
       group_list = read_siemens_dicom(file_list[ifile], max_group);
     }

     // initialize big and small images, if mosaic
     if (general_info.num_slices_in_file > 1) {
       multi_image_init(group_list, &multi_image);
     }

     // loop over subimages in mosaic
     for(subimage = 0; 
	 subimage < general_info.num_slices_in_file; 
	 subimage++) {

       // Modify the group list for this image if mosaic
       if (general_info.num_slices_in_file > 1) {
	 multi_image_modify_group_list(group_list,&multi_image,subimage);
       }
       
       // Get image
       get_siemens_dicom_image(group_list, &image);
       
       // Save the image and any other information
       save_minc_image(icvid, &general_info, &file_info[iimage], &image);

       // increment image counter
       iimage++;
       
     }
     
     // Delete the group list
     acr_delete_group_list(group_list);
     // cleanup multi_image struct if used
     if (general_info.num_slices_in_file > 1) {
       multi_image_cleanup(group_list, &multi_image);
     }
     
     /* Free the image data */
     if ((image.data != NULL) && (image.free)) FREE(image.data);
     
   }

   /* Close the output file */
   close_minc_file(icvid);

   /* Free the general_info and file_info stuff */
   free_info(&general_info, file_info, num_files);
   FREE(file_info);

   return EXIT_SUCCESS;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_siemens_dicom
@INPUT      : filename - name of siemens dicom file to read
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
public Acr_Group read_siemens_dicom(char *filename, int max_group)
{
   FILE *fp;
   Acr_File *afp;
   Acr_Group group_list;
   Acr_byte_order byte_order;
   Acr_VR_encoding_type vr_encoding;

   /* Open the file */
   fp = fopen(filename, "r");
   if (fp == NULL) {
     fprintf(stderr,"Error opening file %s!\n",filename);
     return NULL;
   }

   /* Connect to input stream */
   afp=acr_file_initialize(fp, 0, acr_stdio_read);

   /* should be:  if file type is numa 4 dicom, and if read from file
      with first 128 bytes blank, then do required skipping and setup */

   if (1) {

     /* set byte ordering to explicit LE */

     byte_order = ACR_LITTLE_ENDIAN;
     vr_encoding = ACR_EXPLICIT_VR;
     acr_set_byte_order(afp, byte_order);
     acr_set_vr_encoding(afp, vr_encoding);

     /* skip 1st 128 bytes - are empty in storage class 
	+ DICM (4 chars) 128+4 = 132 bytes to skip */

     acr_skip_input_data(afp, 132);

   }

   /* Read in group list */
   (void) acr_input_group_list(afp, &group_list, max_group);

   /* Close the file */
   acr_file_free(afp);
   (void) fclose(fp);

   return group_list;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_numa4_dicom
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
public Acr_Group read_numa4_dicom(char *filename, int max_group)
{
   FILE *fp;
   Acr_File *afp;
   Acr_Group group_list;
   Acr_byte_order byte_order;
   Acr_VR_encoding_type vr_encoding;

   // needed for group repair - some essential info
   // only available in ascii dump of MrProt structure
   Acr_Element Protocol; 
   Acr_Element element;

   int mosaic_rows, mosaic_cols;
   short subimage_size[4];
   int subimage_rows, subimage_cols;
   int num_slices, num_partitions;
   char *field_ptr;
   int num_encodings, enc_ix;
   int average, files_per_average;

   // Open the file
   fp = fopen(filename, "r");
   if (fp == NULL) {
     fprintf(stderr,"Error opening file %s!\n",filename);
     return NULL;
   }

   // Connect to input stream
   afp=acr_file_initialize(fp, 0, acr_stdio_read);

   /* should be:  if file type is numa 4 dicom, and if read from file
      with first 128 bytes blank, then do required skipping and setup */

   // set byte ordering to explicit LE
   
   byte_order = ACR_LITTLE_ENDIAN;
   // vr_encoding = ACR_EXPLICIT_VR;  // CD format
   // vr_encoding = ACR_IMPLICIT_VR;  // Bourget format

   vr_encoding = acr_get_vr_encoding(afp); // doesn't seem to work on Export/CD
   acr_set_byte_order(afp, byte_order);
   acr_set_vr_encoding(afp, vr_encoding);

   if (N4_OFFSET) {
     // skip 1st 128 bytes - are empty in Syngo CD/Local Export files
     // DICM (4 chars) 128+4 = 132 bytes to skip
     vr_encoding = ACR_EXPLICIT_VR;  // CD format
     acr_set_vr_encoding(afp, vr_encoding);
     acr_skip_input_data(afp, 132);
   } 
   // Read in group list
   (void) acr_input_group_list(afp, &group_list, max_group);

   // Close the file
   acr_file_free(afp);
   (void) fclose(fp);

   // now fix the group list to provide essential info
   // via standard dicom elements
   // (this lets the rest of the code be more reusable)

   // Note that these parameters are mostly dimension lengths,
   // and are not usually supplied in standard DICOM implementations.
   // We could do without them, except that the use of mosaics for
   // multi-slice data makes at least the number of slices necessary.
   // For such elements, we will spoof our own `private' entries
   // using Numaris 3.5 coordinates.  These should not be used elsewhere
   // in the code unless there's no other way!  Basically things
   // should be done as follows:

   // 1) use actual element in public dicom group, if possible
   // 2) if not, then get info from MrProt and insert as 
   //    correct public dicom element
   // 3) if no public element exists, use an SPI element (careful!)
   // 4) if no SPI element exists, use and EXT element (careful!)

   // WOULD RETURN HERE FOR NORMAL DICOM

   // read in Protocol group
   Protocol = acr_find_group_element(group_list,SPI_Protocol);

   // add number of dynamic scans:
   acr_insert_numeric(&group_list, ACR_Acquisitions_in_series,
	      atoi((char*)prot_find_string(Protocol,"lRepetitions"))+1);

   // add number of echoes:
   acr_insert_numeric(&group_list, SPI_Number_of_echoes,
	      atoi((char*)prot_find_string(Protocol,"lContrasts")));

   // add receiving coil (for some reason this isn't in generic groups)
   acr_insert_string(&group_list, ACR_Receiving_coil,
	      prot_find_string(Protocol,
		       "sCOIL_SELECT_MEAS.asList[0].sCoilElementID.tCoilID"));
   // add MrProt dump
   acr_insert_string(&group_list,EXT_MrProt_dump,dump_protocol_text(Protocol));

   // add number of slices:
   // (called `Partitions' for 3D)
   num_slices = atoi((char*)prot_find_string(Protocol,"sSliceArray.lSize"));
   num_partitions = 
     atoi((char*)prot_find_string(Protocol,"sKSpace.lPartitions"));
   // NOTE:  for some reason, lPartitions > 1 even for 2D scans
   // (e.g. EPI, scouts)

   if (!strncmp(acr_find_string(group_list,ACR_MR_acquisition_type,
					   ""),"3D",2)) {
     // use partitions if 3D
     // (note that this gets more complicated if the 3D scan
     //  is mosaiced - see below)

     acr_insert_numeric(&group_list, SPI_Number_of_slices_nominal,
			1);
     acr_insert_numeric(&group_list, SPI_Number_of_3D_raw_partitions_nominal,
     			num_partitions);
   } else {
     // use slices for 2D
     acr_insert_numeric(&group_list, SPI_Number_of_slices_nominal,num_slices);
     acr_insert_numeric(&group_list, 
			SPI_Number_of_3D_raw_partitions_nominal,1);
   }

   // now figure out mosaic rows and columns, and put in EXT shadow group

   // check for interpolation - not supported for mosaics yet
   if (strcmp(prot_find_string(Protocol,"sKSpace.uc2DInterpolation"),"0")){
     // if interpolated image, assume no mosaic
     acr_insert_numeric(&group_list, EXT_Mosaic_rows, 1);
     acr_insert_numeric(&group_list, EXT_Mosaic_columns, 1);
     acr_insert_numeric(&group_list, EXT_Slices_in_file, 1);

   } else {
     // compute mosaic rows and columns

     // here is a hack to handle non-square mosaiced images
     // WARNING:  as far as I can tell, the phase-encoding dir
     // (row/col) is reversed for mosaic EPI scans (don't know
     // if this is a mosaic thing, an EPI thing, or whatever)

     // get the array of sizes:
     //    freq row/freq col/phase row/phase col
     element = acr_find_group_element(group_list, ACR_Acquisition_matrix);
     acr_get_element_short_array(element, 4, subimage_size);

     // get subimage dimensions, assuming the OPPOSITE of the
     // reported phase-encode direction!!
     if (!strncmp(acr_find_string(group_list,ACR_Phase_encoding_direction,""),
		"COL",3)) {

       subimage_rows = subimage_size[3];
       subimage_cols = subimage_size[0];

     } else if (!strncmp(acr_find_string(group_list,
                ACR_Phase_encoding_direction,""),"ROW",3)) {

       subimage_rows = subimage_size[2];
       subimage_cols = subimage_size[1];

     }

     mosaic_rows = acr_find_int(group_list,ACR_Rows, 1)/subimage_rows;
     mosaic_cols = acr_find_int(group_list,ACR_Columns, 1)/subimage_cols;

     acr_insert_numeric(&group_list, EXT_Mosaic_rows,mosaic_rows);
     acr_insert_numeric(&group_list, EXT_Mosaic_columns,mosaic_cols);

     if (mosaic_rows * mosaic_cols > 1) {

       // if 3D mosaiced scan, write number of partitions to number of 
       // slices in dicom group  THIS LOOKS REDUNDANT!!!
       if (!strncmp(acr_find_string(group_list,ACR_MR_acquisition_type,
				    ""),"3D",2)) {
	 acr_insert_numeric(&group_list, SPI_Number_of_slices_nominal,
			    num_partitions);
       }

       // assume any mosaiced file contains all slices
       // (we now support mosaics for 2D and 3D acquisitions,
       //  so we may need to use partitions instead of slices)
       
       if (!strncmp(acr_find_string(group_list,ACR_MR_acquisition_type,""),
		    "2D",2)) {
	 acr_insert_numeric(&group_list, EXT_Slices_in_file, num_slices);
	 acr_insert_numeric(&group_list, 
			    SPI_Number_of_slices_nominal,num_slices);
       } else if (!strncmp(acr_find_string(group_list,ACR_MR_acquisition_type,
					   ""),"3D",2)) {
	 acr_insert_numeric(&group_list, EXT_Slices_in_file, 
			    num_partitions);
	 acr_insert_numeric(&group_list, SPI_Number_of_slices_nominal, 
			    num_partitions);
	 // also have to provide slice spacing - in case of 3D it's same
	 // as slice thickness (and not provided in dicom header!)
	 acr_insert_numeric(&group_list, ACR_Spacing_between_slices,
		    acr_find_double(group_list,ACR_Slice_thickness,1.0));
       }
     } else {
       acr_insert_numeric(&group_list, EXT_Slices_in_file, 1);
     }

     // correct the rows and columns values -
     // these will reflect those of the subimages in the mosaics
     // NOT the total image dimensions
     acr_insert_short(&group_list, EXT_Sub_image_columns,subimage_cols);
     acr_insert_short(&group_list, EXT_Sub_image_rows,subimage_rows);

     // should also correct the image position here?

   }

   // correct dynamic scan info if diffusion scan:
   //
   // assumptions:
   //
   //  - diffusion protocol indicated by sDiffusion.ucMode = 0x4
   //  - there are 7 shots for DTI (b=0 + 6 encodings)
   //  - b=0 scan has sequence name "ep_b0"
   //  - encoded scans have seq names "ep_b700#1, ep_b700#2, ..." etc.
   //
   // actions:
   // 
   //  - change number of dynamic scans to 7
   //  - modify dynamic scan index to encoding index

   if (!strcmp(prot_find_string(Protocol,"sDiffusion.ucMode"),"0x4")) {

     // try to get b value
     acr_insert_numeric(&group_list, 
			EXT_Diffusion_b_value,
			atoi((char*)prot_find_string(Protocol,
					     "sDiffusion.alBValue[1]")));

     // if all averages in one series:
     if (!strcmp(prot_find_string(Protocol,"ucOneSeriesForAllMeas"),"0x1")){

       num_encodings = 7; // for now assume 7 shots in diffusion scan

       // number of 'time points'
       acr_insert_numeric(&group_list, ACR_Acquisitions_in_series, 
			  num_encodings*
			  acr_find_double(group_list,ACR_Nr_of_averages,1));

       // time index of current scan:

       // In the current scheme, the unencoded
       // scan has a sequence name like "ep_b0" while the subsequent
       // six diffusion encodings have names like "ep_b700#1"
       // we could use this to come up with indices for an encoding dimension
       field_ptr=strstr(acr_find_string(group_list,ACR_Sequence_name,""),"#");
       if (field_ptr == NULL) {
	 enc_ix = 0;
       } else {
	 enc_ix = atoi(field_ptr+sizeof(char));
       }

       // however with the current sequence, we get usable
       // time indices from floor(global_image_num/num_slices)
       acr_insert_numeric(&group_list, ACR_Acquisition, 
			  (acr_find_int(group_list, ACR_Image, 1)-1) / 
			  num_slices);

     } else { // averages in different series - no special handling needed?

       num_encodings = 7; // for now assume 7 shots in diffusion scan

       // number of 'time points'
       acr_insert_numeric(&group_list, ACR_Acquisitions_in_series,
			  num_encodings);

       // For multi-series scans, we DO USE THIS BECAUSE global
       // image number may be broken!!
       field_ptr=strstr(acr_find_string(group_list,ACR_Sequence_name,""),"#");
       if (field_ptr == NULL) {
	 enc_ix = 0;
       } else {
	 enc_ix = atoi(field_ptr+sizeof(char));
       }

       acr_insert_numeric(&group_list, ACR_Acquisition, enc_ix);

       
     }
   } // end of diffusion scan handling

   return group_list;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : free_info
@INPUT      : general_info
              file_info
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
/* ARGSUSED */
public void free_info(General_Info *general_info, File_Info *file_info, 
                      int num_files)
{
   Mri_Index imri;

   /* Free the general info pointers */
   for (imri=0; imri < MRI_NDIMS; imri++) {
      if (general_info->indices[imri] != NULL) {
         FREE(general_info->indices[imri]);
      }
      if (general_info->coordinates[imri] != NULL) {
         FREE(general_info->coordinates[imri]);
      }
   }

   /* Free the group list */
   if (general_info->group_list != NULL) {
      acr_delete_group_list(general_info->group_list);
  }

   return;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : search_list
@INPUT      : value
              list
              list_length
              starting_point - point from which search should start
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
public int search_list(int value, int list[], int list_length, 
                       int starting_point)
{
   int index;

   /* Check list length and starting point */
   if (list_length <= 0) return -1;
   if ((starting_point >= list_length) || (starting_point < 0)) {
      starting_point = 0;
   }

   /* Loop over indices, wrapping at the end of the list */
   index = starting_point;
   do {
      if (list[index] == value) return index;
      index++;
      if (index >= list_length) index = 0;
   } while (index != starting_point);

   /* If we get to here, we didn't find the value */
   return -1;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : sort_dimensions
@INPUT      : general_info
@OUTPUT     : general_info
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
public void sort_dimensions(General_Info *general_info)
{
   Mri_Index imri;
   Sort_Element *sort_array;
   int nvalues, ival, jval;
   int reverse_array;

   /* Sort the dimensions, if needed */
   for (imri = 0; imri < MRI_NDIMS; imri++) {
      if (general_info->size[imri] > 1 &&
	  !((file_type == N4DCM) && // don't sort on time for N4 mosaics!
	    (imri == TIME) && 
	    (general_info->num_slices_in_file > 0))) { // also fails on 1 slice

         /* Set up the array for sorting */
         nvalues = general_info->size[imri];
         sort_array = MALLOC(nvalues * sizeof(*sort_array));
         for (ival=0; ival < nvalues; ival++) {
            sort_array[ival].identifier = general_info->indices[imri][ival];
            sort_array[ival].original_index = ival;
            sort_array[ival].value = general_info->coordinates[imri][ival];
         }

         /* Sort the array */
         qsort((void *) sort_array, (size_t) nvalues, sizeof(*sort_array), 
               dimension_sort_function);

         /* Figure out if we should reverse the array to keep something 
            similar to the original ordering */
         reverse_array = (sort_array[0].original_index > 
                          sort_array[nvalues-1].original_index);

         /* Copy the information back into the appropriate arrays */
         for (ival=0; ival < nvalues; ival++) {
            jval = (reverse_array ? nvalues - ival - 1 : ival);
            general_info->indices[imri][ival] = sort_array[jval].identifier;
            general_info->coordinates[imri][ival] = sort_array[jval].value;
         }

         /* Free the array */
         FREE(sort_array);

         /* Update slice step and start */
         if (imri == SLICE) {
            if (general_info->coordinates[imri][0] != 
                general_info->coordinates[imri][nvalues-1]) {
               general_info->step[general_info->slice_world] = 
                  (general_info->coordinates[imri][nvalues-1] -
                   general_info->coordinates[imri][0]) / 
                      ((double) general_info->size[imri] - 1.0);
            }
            general_info->start[general_info->slice_world] =
               general_info->coordinates[imri][0];
         }

      }       /* If size > 1 */
   }          /* Loop over dimensions */

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
public int dimension_sort_function(const void *v1, const void *v2)
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

public char *prot_find_string(Acr_Element Protocol, char *Field) {

  char FieldName[512];
  char Separator[512];
  char FieldValue[512];
  char *Output = calloc(512,sizeof(char)); // return value

  char ProtHead[] = "### ASCCONV BEGIN ###";
  long prot_offset;
  char *field_ptr;
  char DefaultValue = '0';
  int  ix1,ix2;

  // scan throught the group containing the protocol, to find the 
  // ascii dump of the MrProt structure
  for (prot_offset = 0; 
       prot_offset < Protocol->data_length - sizeof(ProtHead); 
       prot_offset++) {
    if (!memcmp(Protocol->data_pointer+prot_offset,
		ProtHead,sizeof(ProtHead)-1)) {
      break;
    }
  }

  //  bail if we didn't find the protocol
  if (prot_offset == Protocol->data_length-sizeof(ProtHead)) {
    (void) fprintf(stderr,
		   "ERROR:  could not find protocol dump in group\n");
    exit(EXIT_FAILURE);
  }

  // try to find the Field name 
  // (should bracket with newline and white space)
  field_ptr = strstr(Protocol->data_pointer+prot_offset,Field);

  if (field_ptr != NULL) {
    sscanf(field_ptr,"%s %s %s",FieldName,Separator,FieldValue);
  } else {
    sprintf(FieldValue,"0");
  }

  // copy FieldValue to output, skipping quotation marks in strings

  ix1 = 0;
  for (ix2 = 0; ix2 < strlen(FieldValue); ix2++) {
    if (FieldValue[ix2] != '"')
      Output[ix1++] = FieldValue[ix2];
  }

  // terminate string
  Output[ix1] = '\0';
  
  return (char*) Output;

}

public char *dump_protocol_text(Acr_Element Protocol) {

  char ProtHead[] = "### ASCCONV BEGIN ###";
  char ProtTail[] = "### ASCCONV END ###";
  char *Output = calloc(Protocol->data_length,sizeof(char)); // return value
  int  prot_found = FALSE;
  long prot_offset;
  int  ix1;

  // scan throught the group containing the protocol, to find the 
  // ascii dump of the MrProt structure
  ix1 = 0;
  for (prot_offset = 0; 
       prot_offset < Protocol->data_length - sizeof(ProtHead); 
       prot_offset++) {
    if (!memcmp(Protocol->data_pointer+prot_offset,
		ProtHead,sizeof(ProtHead)-1)) {
      prot_found = TRUE;
    }
    if (!memcmp(Protocol->data_pointer+prot_offset,
		ProtTail,sizeof(ProtTail)-1)) {
      break;      
    }
    if (prot_found) {
      if (*(Protocol->data_pointer+prot_offset) != '"')
	Output[ix1++] = *(Protocol->data_pointer+prot_offset);
    }
  }

  // terminate string
  Output[ix1] = '\0';

  //  bail if we didn't find the protocol
  if (prot_offset == Protocol->data_length-sizeof(ProtHead)) {
    (void) fprintf(stderr,
		   "ERROR:  could not find protocol dump in group\n");
    exit(EXIT_FAILURE);
  }

  return (char*) Output;

}

public int multi_image_init(Acr_Group group_list, 
                            Multi_Image *multi_image)
{
   int group_id, element_id;
   int last_image, grid_size;
   long new_image_size;
   void *data;
   Acr_Element element;
   char string[256];
   int idim;
   double pixel_spacing[2], separation;
   char *protocol;
   double normal[3];
   double RowColVec[6];
   double dircos[VOL_NDIMS][WORLD_NDIMS];

   // Get some basic image information
   // (big[0/1] is number of columns/rows in whole mosaic)
   multi_image->big[0] = acr_find_int(group_list, ACR_Columns, 1);
   multi_image->big[1] = acr_find_int(group_list, ACR_Rows, 1);
   multi_image->pixel_size = 
      (acr_find_int(group_list, ACR_Bits_allocated, 16)-1) / 8 + 1;

   // Get the image size
   // (size[0/1] is number of columns/rows in a single slice)
   multi_image->size[0] = acr_find_short(group_list,EXT_Sub_image_columns,1);
   multi_image->size[1] = acr_find_short(group_list,EXT_Sub_image_rows,1);

   // Get the grid shape, checking that it is not too big if specified
   multi_image->grid[0] = multi_image->big[0] / multi_image->size[0];
   multi_image->grid[1] = multi_image->big[1] / multi_image->size[1];
   if ((multi_image->grid[0] < 1) || (multi_image->grid[0] < 1)) {
      (void) fprintf(stderr, "Grid too small: %d x %d\n",
                     multi_image->grid[0], multi_image->grid[1]);
      exit(EXIT_FAILURE);
   }

   // Check whether we need to do anything (1x1 grid may be the whole image)
   grid_size = multi_image->grid[0] * multi_image->grid[1];
   if ((grid_size == 1) &&
       (multi_image->size[0] == multi_image->big[0]) &&
       (multi_image->size[1] == multi_image->big[1])) {
     /* had to remove this as now ANY images acquired with 
	the mosaic sequence need special treatment */
     multi_image->packed = FALSE;
      return 1;
   }

   // Steal the image element from the group list
   multi_image->big_image = acr_find_group_element(group_list, ACR_Pixel_data);

   if (multi_image->big_image == NULL) {
      (void) fprintf(stderr, "Couldn't find an image\n");
      exit(EXIT_FAILURE);
   }
   group_id = acr_get_element_group(multi_image->big_image);
   element_id = acr_get_element_element(multi_image->big_image);
   acr_group_steal_element(acr_find_group(group_list, group_id),
                           multi_image->big_image);

   // Add a small image
   new_image_size = 
      multi_image->size[0] * multi_image->size[1] * multi_image->pixel_size;
   data = malloc((size_t) new_image_size);
   multi_image->small_image = 
      acr_create_element(group_id, element_id,
                         acr_get_element_vr(multi_image->big_image),
                         new_image_size, data);
   acr_set_element_vr(multi_image->small_image,
      acr_get_element_vr(multi_image->big_image));
   acr_set_element_byte_order(multi_image->small_image,
      acr_get_element_byte_order(multi_image->big_image));
   acr_set_element_vr_encoding(multi_image->small_image,
      acr_get_element_vr_encoding(multi_image->big_image));
   acr_insert_element_into_group_list(&group_list, multi_image->small_image);

   // Update the number of image rows and columns
   acr_insert_short(&group_list, ACR_Rows, multi_image->size[1]);
   acr_insert_short(&group_list, ACR_Columns, multi_image->size[0]);

   // Get image image index info (number of slices in file)
   last_image = acr_find_int(group_list, EXT_Slices_in_file,1);

   // sub_images is now just the number of mosaic elements, even if
   // they don't all contain slices 
   multi_image->sub_images = multi_image->grid[0] * multi_image->grid[1];

   // unlike Numaris 3.5, last_image should always be correct
   multi_image->first_image = last_image -
     multi_image->sub_images + 1;

   // get the pixel size
   element = acr_find_group_element(group_list, ACR_Pixel_size);
   if ((element != NULL) &&
       (acr_get_element_numeric_array(element, 2, pixel_spacing) == 2)) {
     
     // adjust pixel size for old Numaris 3.5 data
     if (file_type == IMA ||
	 file_type == N3DCM) {
       
       pixel_spacing[0] *= 
         (double) multi_image->big[0] / (double) multi_image->size[0];
       pixel_spacing[1] *= 
         (double) multi_image->big[1] / (double) multi_image->size[1];
       (void) sprintf(string, "%.15g\\%.15g", 
		      pixel_spacing[0], pixel_spacing[1]);
       acr_insert_string(&group_list, ACR_Pixel_size, string);
     }
   }

   // Get step between slices
   separation = acr_find_double(group_list, ACR_Spacing_between_slices, 1.0);

   // get image normal vector
   // (need to compute based on dicom field, which gives
   //  unit vectors for row and column direction)
   element = acr_find_group_element(group_list,
				    ACR_Image_orientation_patient);
   acr_get_element_numeric_array(element, 6, RowColVec);
   
   memcpy(dircos[VCOLUMN],RowColVec,sizeof(RowColVec[0])*3);
   memcpy(dircos[VROW],&RowColVec[3],sizeof(RowColVec[0])*3);
   
   // used to convert x/y flips here...
   // convert_dicom_coordinate(dircos[VROW]);
   // convert_dicom_coordinate(dircos[VCOLUMN]);

   // compute slice normal as cross product of row/column unit vectors
   // (should check for unit length?)
   multi_image->normal[0] = 
     dircos[VCOLUMN][1] * dircos[VROW][2] -
     dircos[VCOLUMN][2] * dircos[VROW][1];
   
   multi_image->normal[1] = 
     dircos[VCOLUMN][2] * dircos[VROW][0] -
     dircos[VCOLUMN][0] * dircos[VROW][2];
   
   multi_image->normal[2] = 
     dircos[VCOLUMN][0] * dircos[VROW][1] -
     dircos[VCOLUMN][1] * dircos[VROW][0];

   // compute slice-to-slice step vector
   for (idim=0; idim < 3; idim++) {
      multi_image->step[idim] = separation * multi_image->normal[idim];
   }

   // Get position and correct to first slice
   element = acr_find_group_element(group_list, ACR_Image_position_patient);
   acr_get_element_numeric_array(element, WORLD_NDIMS,multi_image->position);

   if (file_type == IMA ||
       file_type == N3DCM) {
     // Numaris 3.5 style correction:
     // (position in file is for last slice, we want first)
     for (idim=0; idim < 3; idim++) {
       multi_image->position[idim] -= 
	 (double) (multi_image->sub_images-1) * multi_image->step[idim];
     } 
   } else {
     // Numaris 4 mosaic correction:
     // - position given is edge of huge slice constructed as if 
     //   real slice was at center of mosaic
     // - multi_image->big[0,1] are number of columns and rows of mosaic
     // - multi_image->size[0,1] are number of columns and rows of sub-image

     for (idim=0; idim < 3; idim++) {

       // correct offset from mosaic Center
       multi_image->position[idim] += (double)
	 ((dircos[VCOLUMN][idim]*multi_image->big[0]*pixel_spacing[0]/2.0) +
	  (dircos[VROW][idim] * multi_image->big[1] * pixel_spacing[1]/2));

       // move from center to corner of slice
       multi_image->position[idim] -= 
	 dircos[VCOLUMN][idim] * multi_image->size[0] * pixel_spacing[0]/2.0 +
	 dircos[VROW][idim] * multi_image->size[1] * pixel_spacing[1]/2.0;
	 
     } 

   }

   /* Return number of sub-images in this image */
   return multi_image->sub_images;
}

public int multi_image_modify_group_list(Acr_Group group_list, 
                                          Multi_Image *multi_image,
                                          int iimage)
{
   int irow, ibyte, idim, nbyte;
   int isub, jsub;
   char *new, *old;
   long old_offset, new_offset;
   double position[3], distance;
   char string[256];

   // slice order in mosaic is bottom->top under Numaris 4,
   // unlike Numaris 3.5 in which slices were top->bottom
   if (!(file_type == IMA ||
	 file_type == N3DCM)) {
     iimage = acr_find_int(group_list, EXT_Slices_in_file,1) - iimage - 1;
   } 

   // Check the image number 
   if ((iimage < 0) || (iimage > multi_image->sub_images)) {
      (void) fprintf(stderr, "Invalid image number to send: %d of %d\n",
                     iimage, multi_image->sub_images);
      exit(EXIT_FAILURE);
   }

   // Figure out the sub-image indices 
   isub = iimage % multi_image->grid[0];
   jsub = iimage / multi_image->grid[0];

   // Get pointers
   old = acr_get_element_data(multi_image->big_image);
   new = acr_get_element_data(multi_image->small_image);

   // Copy the image
   nbyte = multi_image->size[0] * multi_image->pixel_size;
   for (irow=0; irow < multi_image->size[1]; irow++) {
      old_offset = isub * multi_image->size[0] +
         (jsub * multi_image->size[1] + irow) * multi_image->big[0];
      old_offset *= multi_image->pixel_size;
      new_offset = (irow * multi_image->size[0]) * multi_image->pixel_size;
      for (ibyte=0; ibyte < nbyte; ibyte++) {
         new[new_offset + ibyte] = old[old_offset + ibyte];
      }
   }

   // Reset the byte order and VR encoding. This will be modified on each
   // send according to what the connection needs.
   acr_set_element_byte_order(multi_image->small_image,
      acr_get_element_byte_order(multi_image->big_image));
   acr_set_element_vr_encoding(multi_image->small_image,
      acr_get_element_vr_encoding(multi_image->big_image));

   // Update the index
   acr_insert_numeric(&group_list, SPI_Current_slice_number, 
		      (double) iimage);

   // Update the position

   for (idim=0; idim < 3; idim++) {
     position[idim] = multi_image->position[idim] + 
       (double) iimage * multi_image->step[idim];
   }

   (void) sprintf(string, "%.15g\\%.15g\\%.15g",
                  position[0], position[1], position[2]);
   acr_insert_string(&group_list, SPI_Image_position, string);
   acr_insert_string(&group_list, ACR_Image_position_patient, string);

   if (file_type == IMA ||
       file_type == N3DCM) {
     // this will convert Siemens SPI info into dicom compliant info
     // not needed for Numaris 4
     update_coordinate_info(group_list);
   }

   return 1;

}

public int multi_image_cleanup(Acr_Group group_list, 
                                Multi_Image *multi_image)
/* ARGSUSED */
{

   if (!multi_image->packed) return;

   acr_delete_element(multi_image->big_image);

   return 1;
}



