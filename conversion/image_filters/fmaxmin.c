/* ----------------------------- MNI Header -----------------------------------
@NAME       : fmaxmin.c
@INPUT      : 
@OUTPUT     : (none)
@DESCRIPTION: Reads binary floating point values from standard input, 
              finds the maximum and minimum and prints them in ascii
              form on standard output (first maximum, then minimum).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 16,1991 (Peter Neelin)
@MODIFIED   : January 13,1991 (P.N.)
                 - reads large blocks for greater efficiency
---------------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>

#define SIGN( x )  ( ((x) > (0)) ? (1) : (-1) )
#define  MAX( x, y )  ( ((x) >= (y)) ? (x) : (y) )
#define  MIN( x, y )  ( ((x) <= (y)) ? (x) : (y) )

#define ARRSIZE 1500
#define ERROR_STATUS -1
#define NORMAL_STATUS 0

main(int argc, char *argv[])
{
   float maximum,minimum,value[ARRSIZE];
   int i,nread;
   char *pname;

   /* Check arguments */
   pname=argv[0];
   if (argc != 1) {
      (void) fprintf(stderr,
                     "Usage : %s \n",
                     pname);
      (void) exit(ERROR_STATUS);
   }

   /* Initialize values */
   maximum = -FLT_MAX;
   minimum = FLT_MAX;

   /* Read in values and test for max and min */
   while ((nread=fread(value, sizeof(&value[0]), ARRSIZE, stdin)) > 0) {
      for (i=0; i<nread; i++) {
         maximum = MAX(maximum,value[i]);
         minimum = MIN(minimum,value[i]);
      }
   }

   if (maximum<minimum) {
      maximum = 0.0;
      minimum = 0.0;
   }

   (void) printf("%g\n%g\n",maximum,minimum);

   return NORMAL_STATUS;

}
