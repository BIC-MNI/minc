/* ----------------------------- MNI Header -----------------------------------
@NAME       : atob.c
@INPUT      : (none)
@OUTPUT     : (none)
@DESCRIPTION: Reads ascii decimal values from standard input and prints the 
              binary equivalent on standard output.
@METHOD     : 
@GLOBALS    : (none)
@CALLS      : 
@CREATED    : December 4,1991 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>

#define OUTTYPE unsigned char
#define CONTROL_STRING " %d"

main()
{
   OUTTYPE value;
   int temp;

   while (scanf(CONTROL_STRING, &temp) > 0) {
      value = (OUTTYPE) temp;
      (void) fwrite(&value, sizeof(OUTTYPE), 1, stdout);
   }
   return 0;
}
