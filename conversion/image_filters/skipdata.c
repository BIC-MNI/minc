/* ----------------------------- MNI Header -----------------------------------
@NAME       : skipdata.c
@INPUT      : argc - number of arguments
              argv - arguments
                 1 - number of bytes to copy
                 2 - number of bytes to skip
@OUTPUT     : (none)
@DESCRIPTION: Alternatedly copies and skips bytes read from standard input
              (writing to standard output). No leading bytes are skipped.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : July 16,1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
   int bytes_to_copy, bytes_to_skip, the_byte, i;

   /* Get arguments */
   if (argc != 3) {
      (void) fprintf(stderr, "Usage: %s <bytes to copy> <bytes to skip>\n",
                     argv[0]);
      return EXIT_FAILURE;
   }
   bytes_to_copy = atoi(argv[1]);
   bytes_to_skip = atoi(argv[2]);
   if ((bytes_to_copy < 1) || (bytes_to_skip < 1)) {
      (void) fprintf(stderr, "%s: bytes to copy and skip must be > 0\n",
                     argv[0]);
      return EXIT_FAILURE;
   }


   /* Loop, copying data and skipping data */
   do {
      for (i=0; (i < bytes_to_copy) && ((the_byte = getchar()) != EOF); i++) {
         (void) putchar(the_byte);
      }
      for (i=0; (i < bytes_to_skip) && ((the_byte = getchar()) != EOF); i++) {}
   } while (the_byte != EOF);

   return EXIT_SUCCESS;
}
