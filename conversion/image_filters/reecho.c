/* ----------------------------- MNI Header -----------------------------------
@NAME       : reecho.c
@INPUT      : argc - number of arguments
              argv - arguments
                 1 - number of bytes to read from input
                 2 - number of times to repeat
@OUTPUT     : (none)
@DESCRIPTION: Reads a given number of bytes from standard input and
              repeatedly writes out those bytes a given number of times.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 10,1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>

#define ERROR_STATUS -1
#define NORMAL_STATUS 0

main(int argc, char *argv[])
{
   int nread,nrepeat,i;
   char *input;
   char *pname;

   /* Check arguments */
   pname=argv[0];
   if (argc != 3) {
      (void) fprintf(stderr,
                     "Usage : %s nread nrepeat \n",
                     pname);
      (void) exit(ERROR_STATUS);
   }
   nread   = atol(argv[1]);
   nrepeat = atol(argv[2]);

   /* Get buffer for input */
   if (nread <= 0) {
      (void) fprintf(stderr,
                     "%s: number of bytes to read must be positive\n",
                     pname);
      (void) exit(ERROR_STATUS);
   }
   if ((input = malloc(nread)) == NULL) {
      (void) fprintf(stderr,"%s: unable to allocate memory\n",pname);
      (void) exit(ERROR_STATUS);
   }

   /* Read input */
   if (fread(input, sizeof(*input), nread, stdin) != nread ) {
      (void) fprintf(stderr,"%s: too few bytes on input\n",pname);
      (void) exit(ERROR_STATUS);
   }

   /* Write out input nrepeat times */
   for (i=0; i<nrepeat; i++) {
      (void) fwrite(input, sizeof(*input), nread, stdout);
   }

   return NORMAL_STATUS;

}
