/* ----------------------------- MNI Header -----------------------------------
@NAME       : imagetranspose.c
@INPUT      : argc - number of arguments
              argv - arguments
                 1 - image size in x
                 2 - image size in y
                 3 - number of bytes per pixel (optional - default = 1)
@OUTPUT     : (none)
@DESCRIPTION: Reads a series of images from standard input and copies them
              to standard output, transposing the x and y axes.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : July 16, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>

#define SIGN( x )  ( ((x) > (0)) ? (1) : (-1) )

#define ERROR_STATUS -1
#define NORMAL_STATUS 0

main(int argc, char *argv[])
{
   int i,j,k,image_size,bytes_per_pixel,row_size,nread;
   int xsize,ysize;
   char *pname;
   char *buffer,*outbuf;

   /* Check arguments */
   pname=argv[0];
   if ((argc != 3)&&(argc != 4)) {
      (void) fprintf(stderr,"Usage : %s xsize ysize <bytesperpixel>\n",pname);
      (void) exit(ERROR_STATUS);
   }
   xsize = atol(argv[1]);
   ysize = atol(argv[2]);
   if (argc == 4) {
      bytes_per_pixel = atol(argv[3]);
      if (bytes_per_pixel <=0) {
         (void) fprintf(stderr,"%s : Negative bytes per pixel\n",pname);
      }
   }
   else {
      bytes_per_pixel = 1;
   }
   if ((xsize <= 0) || (ysize <= 0)) {
      (void) fprintf(stderr,"%s : Illegal image size\n",pname);
      (void) exit(ERROR_STATUS);
   }
   image_size = xsize*ysize;
   row_size = ysize;
   if (((buffer=malloc(image_size*bytes_per_pixel)) == NULL) ||
       ((outbuf=malloc(row_size*bytes_per_pixel)) == NULL)){
      (void) fprintf(stderr,"%s : Image too large\n",pname);
      (void) exit(ERROR_STATUS);
   }

   /* Loop through images */
   while ((nread=fread(buffer, bytes_per_pixel, image_size, stdin))
                     == image_size) {

      /* Write out transposed image */
      for (i=0; i < xsize; i++) {
         for (j=0; j < ysize; j++) {
            for (k=0; k<bytes_per_pixel; k++) {
               outbuf[j*bytes_per_pixel+k]=
                  buffer[(j*xsize+i)*bytes_per_pixel+k];
            }
         }
         (void) fwrite(outbuf, bytes_per_pixel, row_size, stdout);
      }

   }

   /* Check that input corresponds to complete images */
   if (nread>0) {
      (void) fprintf(stderr,"%s : Insufficient data\n",pname);
      (void) exit(ERROR_STATUS);
   }

   return NORMAL_STATUS;

}

