/* ----------------------------- MNI Header -----------------------------------
@NAME       : uitof.c
@INPUT      : (none)
@OUTPUT     : (none)
@DESCRIPTION: Reads binary values from standard input and writes the 
              floating point equivalent on standard output.
@METHOD     : 
@GLOBALS    : (none)
@CALLS      : 
@CREATED    : December 4,1991 (Peter Neelin)
@MODIFIED   : January 13,1991 (P.N.)
                 - read values into large array for optimization
---------------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>

#define ARRSIZE 1500
#define INTYPE unsigned short int
#define OUTTYPE float

main()
{
   INTYPE value[ARRSIZE];
   OUTTYPE output[ARRSIZE];
   int i,nread;

   while ((nread = fread(value, sizeof(INTYPE), 
                         sizeof(value)/sizeof(INTYPE), stdin)) > 0) {
      for (i=0; i < nread; i++) {
         output[i] = (OUTTYPE) value[i];
      }
      (void) fwrite(output, sizeof(OUTTYPE), nread, stdout);
   }
   return 0;
}
