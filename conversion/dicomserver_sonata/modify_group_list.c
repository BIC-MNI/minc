public void multi_image_modify_group_list(Acr_Group group_list, Acr_Element *big_image,Acr_Element *small_image,int iimage)
{
   int irow, ibyte, idim, nbyte;
   int isub, jsub;
   char *new, *old;
   long old_offset, new_offset;
   double position[3], distance;
   double old_position[3], old_step[3], normal[3];
   char string[256];

   int slices_in_file;
   int num_mosaic_rows;
   int num_mosaic_cols;
   int pixel_size;
   long new_image_size;
   Acr_Element element;
   int big_cols, big_rows;
   int small_cols, small_rows;
   void *data;
   double RowColVec[6];
   double dircos[VOL_NDIMS][WORLD_NDIMS];

   // get info about file:

   slices_in_file=acr_find_int(group_list, EXT_Slices_in_file, 999);
   num_mosaic_rows=acr_find_int(group_list,EXT_Mosaic_rows, 999);
   num_mosaic_cols = acr_find_int(group_list,EXT_Mosaic_columns,999);

   // Check the image number
   if ((iimage < 0) || (iimage > slices_in_file)) {
      (void) fprintf(stderr, "Invalid image number to send: %d of %d\n",iimage, slices_in_file);
      exit(EXIT_FAILURE);
   }

   // Figure out the sub-image indices
   isub = iimage % num_mosaic_rows;
   jsub = iimage / num_mosaic_cols;

   // Get pointers:
   
   old = acr_get_element_data(*big_image);
   new = acr_get_element_data(*small_image);

   /* Copy the image */
   nbyte = small_cols * pixel_size;
   for (irow=0; irow < small_rows; irow++) {
      old_offset = isub * small_cols +(jsub * small_rows + irow) * big_rows;
      old_offset *= pixel_size;
      new_offset = (irow * small_cols) * pixel_size;
      for (ibyte=0; ibyte < nbyte; ibyte++) {
         new[new_offset + ibyte] = old[old_offset + ibyte];
      }
   }

   /* Reset the byte order and VR encoding. This will be modified on each
      send according to what the connection needs. */
   acr_set_element_byte_order(*small_image,acr_get_element_byte_order(*big_image));
   acr_set_element_vr_encoding(*small_image,acr_get_element_vr_encoding(*big_image));

   // Update the slice index
   acr_insert_numeric(&group_list, SPI_Current_slice_number,(double) (iimage + 1));

   if (file_type == N3DCM || file_type == IMA) {
     // get the image normals
     element = acr_find_group_element(group_list, SPI_Image_normal);
     acr_get_element_numeric_array(element, 3, normal);

     // get the old image position
     element = acr_find_group_element(group_list, SPI_Image_position);
     acr_get_element_numeric_array(element, 3, old_position);

   } else {
     element = acr_find_group_element(group_list,ACR_Image_orientation_patient);
     acr_get_element_numeric_array(element, 6, RowColVec);

     memcpy(dircos[VCOLUMN],RowColVec,sizeof(RowColVec[0])*3);
     memcpy(dircos[VROW],&RowColVec[3],sizeof(RowColVec[0])*3);

     convert_dicom_coordinate(dircos[VROW]);
     convert_dicom_coordinate(dircos[VCOLUMN]);

     //     dircos[VSLICE][0] = 
     normal[0] = dircos[VCOLUMN][1] * dircos[VROW][2] - dircos[VCOLUMN][2] * dircos[VROW][1];

     //     dircos[VSLICE][1] = 
     normal[1] = dircos[VCOLUMN][2] * dircos[VROW][0] - dircos[VCOLUMN][0] * dircos[VROW][2];

     //     dircos[VSLICE][2] = 
     normal[2] = dircos[VCOLUMN][0] * dircos[VROW][1] - dircos[VCOLUMN][1] * dircos[VROW][0];

     element = acr_find_group_element(group_list, ACR_Image_position_patient);
     acr_get_element_numeric_array(element, WORLD_NDIMS, old_position);
     convert_dicom_coordinate(old_position);

   }

   printf("here1\n");

   /* Update the position */
   distance = 0.0;
   for (idim=0; idim < 3; idim++) {
      position[idim] = old_position[idim] + (double) iimage * old_step[idim];
      distance += position[idim] * normal[idim];
   }

   (void) sprintf(string, "%.15g\\%.15g\\%.15g",position[0], position[1], position[2]);
   acr_insert_string(&group_list, SPI_Image_position, string);
   
   // call function to fix DICOM header to match Siemens header
   update_coordinate_info(group_list);

}

public int multi_image_init(Acr_Group group_list, Acr_Element *big_image, Acr_Element *small_image)

{
  int big_cols, big_rows;
  int small_cols, small_rows;
  int num_mosaic_rows;
  int num_mosaic_cols;
  int pixel_size;
  long new_image_size;
  void *data;
  int group_id, element_id;

  // Steal the image element from the group list
  *big_image = acr_find_group_element(group_list, ACR_Image);
  
  group_id = acr_get_element_group(*big_image);
  element_id = acr_get_element_element(*big_image);
  acr_group_steal_element(acr_find_group(group_list, group_id),*big_image);
  
  // Add a small image, if needed
  
  big_cols = acr_find_int(group_list, ACR_Columns, 1) * num_mosaic_cols;
  big_rows = acr_find_int(group_list, ACR_Rows, 1) * num_mosaic_rows;
  
  small_cols = acr_find_int(group_list, ACR_Columns, 1);
  small_rows = acr_find_int(group_list, ACR_Rows, 1);
  
  pixel_size = 
    (acr_find_int(group_list, ACR_Bits_allocated, 16)-1) / 8 + 1;
  
  new_image_size = acr_find_int(group_list, ACR_Columns, 1) * acr_find_int(group_list, ACR_Rows, 1) * pixel_size;
  
  data = malloc((size_t) new_image_size);
  
  *small_image = acr_create_element(group_id, element_id,acr_get_element_vr(*big_image),new_image_size, data);
  
  acr_set_element_vr(*small_image,acr_get_element_vr(*big_image));
  acr_set_element_byte_order(*small_image,acr_get_element_byte_order(*big_image));
  acr_set_element_vr_encoding(*small_image,acr_get_element_vr_encoding(*big_image));
  acr_insert_element_into_group_list(&group_list, *small_image);

  return 0;  
}

