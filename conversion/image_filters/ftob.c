/* ----------------------------- MNI Header -----------------------------------
@NAME       : ftob.c
@INPUT      : (none)
@OUTPUT     : (none)
@DESCRIPTION: Reads floating point values from standard input and writes the 
              integer equivalent on standard output.
@METHOD     : 
@GLOBALS    : (none)
@CALLS      : 
@CREATED    : December 4,1991 (Peter Neelin)
@MODIFIED   : January 13,1991 (P.N.)
                 - read values into large array for optimization
---------------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define  MAX( x, y )  ( ((x) >= (y)) ? (x) : (y) )
#define  MIN( x, y )  ( ((x) <= (y)) ? (x) : (y) )

#define ARRSIZE 1500
#define INTYPE float
#define OUTTYPE unsigned char
#define ROUND( x ) ( (signed long int) ( ((x) > (0)) ? (x)+0.5 : (x)-0.5 ))
#define MAXVAL UCHAR_MAX
#define MINVAL 0

main()
{
   INTYPE value[ARRSIZE], temp;
   OUTTYPE output[ARRSIZE];
   int i,nread;

   while ((nread = fread(value, sizeof(INTYPE), 
                         sizeof(value)/sizeof(INTYPE), stdin)) > 0) {
      for (i=0; i < nread; i++) {
         temp = ROUND(value[i]);
         temp = MIN(MAXVAL, temp);
         temp = MAX(MINVAL, temp);
         output[i] = (OUTTYPE) temp;
      }
      (void) fwrite(output, sizeof(OUTTYPE), nread, stdout);
   }
   return 0;
}
