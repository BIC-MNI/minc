public void timeout_handler(int sig);
public Acr_Group skip_command_groups(Acr_Group group_list);
public void cleanup_files(int num_files, char *file_list[]);
public void free_list(int num_files, char **file_list, 
                      Data_Object_Info **file_info_list);
public int create_minc_file(char *minc_file, int clobber, 
                            General_Info *general_info,
                            char *file_prefix, char **output_file_name,
			    Loop_Type loop_type);
public void setup_minc_variables(int mincid, General_Info *general_info,
				 Loop_Type loop_type);
public void save_minc_image(int icvid, General_Info *general_info, 
                            File_Info *file_info, Image_Data *image);
public void close_minc_file(int icvid);
public void open_connection(int argc, char *argv[], 
                            Acr_File **afpin, Acr_File **afpout);
public int read_project_file(char *project_name, 
                             char *file_prefix, 
                             int *output_uid, int *output_gid,
                             char *command_line, int maxlen_command);
public void get_project_option_string(char *project_option_string,
                                      int maxlen_project_option);
public Acr_Message associate_reply(Acr_Message input_message, 
                                   char **project_name,
                                   int *pres_context_id,
                                   Acr_byte_order *byte_order,
                                   Acr_VR_encoding_type *vr_encoding,
                                   long *maximum_length);
public Acr_Message associate_reply_reject(Acr_Message input_message, 
                                          int reason);
public Acr_Message release_reply(Acr_Message input_message);
public Acr_Message abort_reply(Acr_Message input_message);
public Acr_Message data_reply(Acr_Message input_message);
public void save_transferred_object(Acr_Group group_list, char *file_prefix,
                                    char **new_file_name,
                                    Data_Object_Info *data_info);
public void get_file_info(Acr_Group group_list, File_Info *file_info,
                          General_Info *general_info);
public void get_identification_info(Acr_Group group_list, 
                                    double *study_id, int *acq_id, 
                                    int *rec_num, int *image_type);
public void get_intensity_info(Acr_Group group_list, File_Info *file_info);
public void get_coordinate_info(Acr_Group group_list, File_Info *file_info,
                                Orientation *orientation,
                                World_Index volume_to_world[VOL_NDIMS],
                                int sizes[VOL_NDIMS],
                                double dircos[VOL_NDIMS][WORLD_NDIMS],
                                double steps[VOL_NDIMS],
                                double starts[VOL_NDIMS],
                                double coordinate[WORLD_NDIMS]);
public void convert_numa3_coordinate(double coordinate[WORLD_NDIMS]);
public void convert_dicom_coordinate(double coordinate[WORLD_NDIMS]);
public void get_general_header_info(Acr_Group group_list, 
                                    General_Info *general_info);
public double convert_time_to_seconds(double dicom_time);
public void get_siemens_dicom_image(Acr_Group group_list, Image_Data *image);
public int siemens_dicom_to_minc(int num_files, char *file_list[], 
                        char *minc_file, int clobber,
                        char *file_prefix, char **output_file_name);
public Acr_Group read_siemens_dicom(char *filename, int max_group);
public void free_info(General_Info *general_info, File_Info *file_info, 
                      int num_files);
public int search_list(int value, int list[], int list_length, 
                       int starting_point);
public void usage(void);
public void sort_dimensions(General_Info *general_info);
public int dimension_sort_function(const void *v1, const void *v2);
public void string_to_filename(char *string, char *filename, int maxlen);
public void use_the_files(char *project_name,
                          int num_files, char *file_list[], 
                          Data_Object_Info *data_info[],
			  int UseArgDir,char *OutDir);
public Acr_Group siemens_to_dicom(char *filename, int read_image);

// MGH specific stuff
public void string_to_initials(char *string, char *filename, int maxlen);

// Numaris 4 specific stuff
public Acr_Group read_numa4_dicom(char *filename, int max_group);
public char *prot_find_string(Acr_Element Protocol, char *Field);
public char *dump_protocol_text(Acr_Element Protocol);

