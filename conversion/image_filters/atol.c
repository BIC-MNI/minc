/* ----------------------------- MNI Header -----------------------------------
@NAME       : atol.c
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

#define OUTTYPE signed long int
#define CONTROL_STRING " %d"

main()
{
   OUTTYPE value;

   while (scanf(CONTROL_STRING, &value) > 0) {
      (void) fwrite(&value, sizeof(OUTTYPE), 1, stdout);
   }
   return 0;
}
