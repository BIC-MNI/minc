public void cleanup_files(int num_files, char *file_list[]);
public void free_list(int num_files, char **file_list);
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
                                    char **new_file_name);
public void use_the_files(int num_files, char *file_list[]);
