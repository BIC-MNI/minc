public void cleanup_files(int num_files, char *file_list[]);
public void free_list(int num_files, char **file_list, 
                      Data_Object_Info *file_info_list);
public int open_connection(int argc, char *argv[], 
                           Acr_File **afpin, Acr_File **afpout);
public Acr_Message gcbegin_reply(Acr_Message input_message, int *num_files);
public Acr_Message ready_reply(Acr_Message input_message);
public Acr_Message send_reply(Acr_Message input_message);
public Acr_Message gcend_reply(Acr_Message input_message);
public Acr_Message cancel_reply(Acr_Message input_message);
public Acr_Status spi_input_message(Acr_File *afp, Acr_Message *message);
public Acr_Status spi_output_message(Acr_File *afp, Acr_Message message);
public Acr_Status spi_input_data_object(Acr_File *afp, Acr_Group *group_list);
public void save_transferred_object(Acr_Group group_list, char *file_prefix,
                                    char **new_file_name,
                                    Data_Object_Info *data_info);
public void use_the_files(int num_files, char *file_list[], 
                          Data_Object_Info data_info[]);
public int gyro_to_minc(int num_files, char *file_list[], 
                        char *minc_file, int clobber,
                        char *file_prefix, char **output_file_name);
public void free_info(General_Info *general_info, File_Info *file_info, 
                      int num_files);
public Acr_Group read_gyro(char *filename, int max_group);
public void get_file_info(Acr_Group group_list, File_Info *file_info,
                          General_Info *general_info);
public void get_gyro_image(Acr_Group group_list, Image_Data *image);
public void get_direction_cosines(double angulation_ap, double angulation_lr,
                                  double angulation_cc, 
                                  double dircos[WORLD_NDIMS][WORLD_NDIMS]);
public void get_orientation_info(Orientation orientation,
                                 double dircos[WORLD_NDIMS][WORLD_NDIMS],
                                 World_Index *slice_world,
                                 World_Index *row_world,
                                 World_Index *column_world);
public void swap_dircos(double dircos1[WORLD_NDIMS],
                        double dircos2[WORLD_NDIMS]);
public World_Index get_nearest_world_axis(double dircos[WORLD_NDIMS]);
public int find_short(Acr_Group group_list, Acr_Element_Id elid, 
                      int default_value);
public int find_int(Acr_Group group_list, Acr_Element_Id elid, 
                    int default_value);
public double find_double(Acr_Group group_list, Acr_Element_Id elid, 
                          double default_value);
public char *find_string(Acr_Group group_list, Acr_Element_Id elid, 
                         char *default_value);
public int create_minc_file(char *minc_file, int clobber, 
                            General_Info *general_info,
                            char *file_prefix, char **output_file_name);
public void setup_minc_variables(int mincid, General_Info *general_info);
public void save_minc_image(int icvid, General_Info *general_info, 
                            File_Info *file_info, Image_Data *image);
public void close_minc_file(int icvid);
public void string_to_filename(char *string, char *filename, int maxlen);
