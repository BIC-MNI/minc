extern int create_minc_file(const char *minc_file, 
                            int clobber, 
                            General_Info *general_info,
                            const char *file_prefix, 
                            char **output_file_name,
			    Loop_Type loop_type);
extern void setup_minc_variables(int mincid, General_Info *general_info,
				 Loop_Type loop_type);
extern void save_minc_image(int icvid, General_Info *general_info, 
                            File_Info *file_info, Image_Data *image);
extern void close_minc_file(int icvid);
