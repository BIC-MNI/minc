extern void get_siemens_dicom_image(Acr_Group group_list, Image_Data *image);
extern void parse_dicom_groups(Acr_Group group_list, Data_Object_Info *di_ptr);
extern void get_file_info(Acr_Group group_list, File_Info *file_info,
                          General_Info *general_info);
