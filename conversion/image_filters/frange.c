/* ----------------------------- MNI Header -----------------------------------
@NAME       : frange.c
@INPUT      : argc - number of arguments
              argv - arguments
                 1 - lower bound
                 2 - upper bound
                 3 - null value
@OUTPUT     : (none)
@DESCRIPTION: Reads binary floating point values from standard input, 
              checks to see if they are in the allowed range and copies
              the result to standard output (in binary form). If the value
              is outside of the range, it is set to the null value. If the
              lower bound is greater than the upper bound, then values
              between upper and lower bound are set to the null value.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 6,1992 (Peter Neelin)
@MODIFIED   : January 13,1991 (P.N.)
                 - reads large blocks for greater efficiency
---------------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>

#define SIGN( x )  ( ((x) > (0)) ? (1) : (-1) )

#define ARRSIZE 1500
#define ERROR_STATUS -1
#define NORMAL_STATUS 0

main(int argc, char *argv[])
{
   float low,high,null,value[ARRSIZE];
   int within,i,nread;
   char *pname;

   /* Check arguments */
   pname=argv[0];
   if (argc != 4) {
      (void) fprintf(stderr,
                     "Usage : %s low high null\n",
                     pname);
      (void) exit(ERROR_STATUS);
   }
   low  = atof(argv[1]);
   high = atof(argv[2]);
   null = atof(argv[3]);

   /* If low <= high, then values should be between low and high, 
      otherwise, they should be outside the range (remember that 
      low >= high) */
   within = (low <= high);

   /* Read in values, do calculation and write out result */
   while ((nread=fread(value, sizeof(&value[0]), ARRSIZE, stdin)) > 0) {
      for (i=0; i<nread; i++) {
         if (within) {
            value[i] = ( ( (value[i]>=low) && (value[i]<=high) ) ? 
                        value[i] : null );
         }
         else {
            value[i] = ( ( (value[i]>=low) || (value[i]<=high) ) ? 
                        value[i] : null );
         }
      }
      (void) fwrite(value, sizeof(&value[0]), nread, stdout);
   }

   return NORMAL_STATUS;

}
