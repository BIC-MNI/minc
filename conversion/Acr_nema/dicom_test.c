#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include <acr_nema.h>
#include <minc_def.h>

#if 0
#define WRITING
#endif

int main(int argc, char *argv[])
     /* ARGSUSED */
{
   Acr_File *afpin, *afpout;
   Acr_Message message;
   Acr_Status status;
   Acr_Element element;
   char *error_type, *error_string;

   afpin=acr_initialize_dicom_input(stdin, 0, acr_stdio_read);
#ifdef WRITING
   afpout=acr_initialize_dicom_output(stdout, 0, acr_stdio_write);
#endif
   error_type = "reading";

   /* Loop over messages */
   while ((status=acr_input_dicom_message(afpin, &message)) == ACR_OK) {

      /* Dump the values */
      acr_dump_group_list(stderr, acr_get_message_group_list(message));

#ifdef WRITING
      /* Write out message */
      if ((status=acr_output_dicom_message(afpout, message)) != ACR_OK) {
         error_type = "writing";
         break;
      }
#endif

      acr_delete_message(message);

   }

   error_string = acr_status_string(status);
   (void) fprintf(stderr, "Terminated while %s: %s\n", 
                  error_type, error_string);

   /* Free the afp */
   acr_close_dicom_file(afpin);
#ifdef WRITING
   acr_close_dicom_file(afpout);
#endif

   exit(EXIT_SUCCESS);
}
