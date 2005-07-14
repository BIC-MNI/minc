extern void get_dicom_image_data(Acr_Group group_list, Image_Data *image);
extern void parse_dicom_groups(Acr_Group group_list, Data_Object_Info *di_ptr);
extern void get_file_info(Acr_Group group_list, File_Info *file_info,
                          General_Info *general_info);

#define DICOM_POSITION_LOCAL 2
#define DICOM_POSITION_GLOBAL 1
#define DICOM_POSITION_NONE 0

extern int dicom_read_position(Acr_Group group_list, int n, double position[3]);
extern int dicom_read_orientation(Acr_Group group_list, double orientation[6]);
extern int dicom_read_pixel_size(Acr_Group group_list, double pixel_size[2]);

extern void convert_dicom_coordinate(double coord[]);

