/* ----------------------------- MNI Header -----------------------------------
@NAME       : fscale.c
@INPUT      : argc - number of arguments
              argv - arguments
                 1 - scale factor
                 2 - offset
@OUTPUT     : (none)
@DESCRIPTION: Reads binary floating point values from standard input, 
              scales them according to the arguments and writes the
              result to standard output (in binary form).
              Scale and offset are used as follows :
                    result = value * scale + offset
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 4,1991 (Peter Neelin)
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
   float scale,offset,value[ARRSIZE];
   int i,nread;
   char *pname;

   /* Check arguments */
   pname=argv[0];
   if (argc != 3) {
      (void) fprintf(stderr,
                     "Usage : %s scale offset (o = i*scale + offset)\n",
                     pname);
      (void) exit(ERROR_STATUS);
   }
   scale  = atof(argv[1]);
   offset = atof(argv[2]);

   /* Read in values, do calculation and write out result */
   while ((nread=fread(value, sizeof(&value[0]), ARRSIZE, stdin)) > 0) {
      for (i=0; i<nread; i++) {
         value[i] = value[i] * scale + offset;
      }
      (void) fwrite(value, sizeof(&value[0]), nread, stdout);
   }

   return NORMAL_STATUS;

}
