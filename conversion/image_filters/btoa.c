/* ----------------------------- MNI Header -----------------------------------
@NAME       : btoa.c
@INPUT      : (none)
@OUTPUT     : (none)
@DESCRIPTION: Reads binary values from standard input and prints the ascii
              decimal equivalent on standard output.
@METHOD     : 
@GLOBALS    : (none)
@CALLS      : 
@CREATED    : December 4,1991 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>

#define INTYPE unsigned char
#define CONTROL_STRING "%d\n"
#define PRINTF_TYPE int

main()
{
   INTYPE value;

   while (fread(&value, sizeof(INTYPE), 1, stdin) == 1) {
      (void) printf(CONTROL_STRING, (PRINTF_TYPE) value);
   }
   return 0;
}
