#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define GLOBAL_ELEMENT_DEFINITION

#include <acr_nema.h>

GLOBAL_ELEMENT(ACR_Recognition_code, 0x0, 0x10, LO);
GLOBAL_ELEMENT(Shadow_Recognition_code, 0x1, 0x10, LO);
GLOBAL_ELEMENT(Shadow_Command_code, 0x1, 0x18, US);

int main(int argc, char *argv[])
     /* ARGSUSED */
{
   Acr_File *afpin, *afpout;
   Acr_Message message;
   Acr_Status status;
   Acr_Element element;
   char *error_type, *error_string;

   afpin=acr_file_initialize(stdin, 0, acr_stdio_read);
   (void) acr_test_byte_order(afpin);
   acr_set_io_watchpoint(afpin, 102382L);
#if 0
   afpout=acr_file_initialize(stdout, 0, acr_stdio_write);
#else
   afpout=acr_initialize_dicom_output(stdout, 0, acr_stdio_write);
#endif
   error_type = "reading";
   while ((status=acr_input_message(afpin, &message)) == ACR_OK) {
#if 0
      if ((status=acr_output_message(afpout, message)) != ACR_OK) {
#else
      acr_set_dicom_pres_context_id(afpout, 19);
      if ((status=acr_output_dicom_message(afpout, message)) != ACR_OK) {
#endif
         error_type = "writing";
         break;
      }
      element = acr_find_group_element(acr_get_message_group_list(message),
                                       ACR_Recognition_code);
      if (element != NULL)
         (void) fprintf(stderr, "ACR-NEMA recognition code: '%s'\n",
                        acr_get_element_string(element));
      element = acr_find_group_element(acr_get_message_group_list(message),
                                       Shadow_Recognition_code);
      if (element != NULL)
         (void) fprintf(stderr, "Shadow recognition code: '%s'\n",
                        acr_get_element_string(element));
      element = acr_find_group_element(acr_get_message_group_list(message),
                                       Shadow_Command_code);
      if (element != NULL)
         (void) fprintf(stderr, "Shadow command code: 0x%x\n",
                        (int) acr_get_element_short(element));
   }

   error_string = acr_status_string(status);
   (void) fprintf(stderr, "Terminated while %s: %s\n", 
                  error_type, error_string);

   /* Free the afp */
   acr_file_free(afpin);
#if 0
   acr_file_free(afpout);
#else
   acr_close_dicom_file(afpout);
#endif

   exit(EXIT_SUCCESS);
}
