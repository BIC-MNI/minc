/* ----------------------------- MNI Header -----------------------------------
@NAME       : byte_swap.c
@INPUT      : (none)
@OUTPUT     : (none)
@DESCRIPTION: Reads bytes from standard input and writes the 
              swapped bytes standard output.
@METHOD     : 
@GLOBALS    : (none)
@CALLS      : 
@CREATED    : January 13,1991 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>

#define BYTES 2
#define ARRSIZE 800*BYTES
#define VALTYPE char

main()
{
   VALTYPE input[ARRSIZE];
   VALTYPE output[ARRSIZE];
   int i,nread,extra;

   while ((nread = fread(input, sizeof(VALTYPE), 
                         sizeof(input)/sizeof(VALTYPE), stdin)) > 0) {
      if ((extra = nread % BYTES) != 0) nread = nread-extra;
      for (i=0; i < nread; i+=BYTES) {
         output[i] = (VALTYPE) input[i+1];
         output[i+1] = (VALTYPE) input[i];
      }
      (void) fwrite(output, sizeof(VALTYPE), nread, stdout);
   }
   return 0;
}
