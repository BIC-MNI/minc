/* ----------------------------- MNI Header -----------------------------------
@NAME       : imageinvert.c
@INPUT      : argc - number of arguments
              argv - arguments
                 1 - image size in x (-ve means invert)
                 2 - image size in y (-ve means invert)
                 3 - number of bytes per pixel (optional - default = 1)
@OUTPUT     : (none)
@DESCRIPTION: Reads an image from standard input and copies it to standard
              output, inverting along either or both dimensions according 
              to the arguments
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : December 3,1991 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>

#define SIGN( x )  ( ((x) > (0)) ? (1) : (-1) )
#define  ABS( x )   ( ((x) > 0) ? (x) : (-(x)) )
#define  MAX( x, y )  ( ((x) >= (y)) ? (x) : (y) )

#define ERROR_STATUS -1
#define NORMAL_STATUS 0

main(int argc, char *argv[])
{
   int i,j,k,oi,image_size,offset,bytes_per_pixel,row_size,nread;
   int xsize,ysize,xstart,ystart,xstop,ystop,xstep,ystep;
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
   if ((xsize == 0) || (ysize == 0)) {
      (void) fprintf(stderr,"%s : Illegal image size\n",pname);
      (void) exit(ERROR_STATUS);
   }
   image_size = ABS(xsize*ysize);
   row_size = ABS(xsize);
   if (((buffer=malloc(image_size*bytes_per_pixel)) == NULL) ||
       ((outbuf=malloc(row_size*bytes_per_pixel)) == NULL)){
      (void) fprintf(stderr,"%s : Image too large\n",pname);
      (void) exit(ERROR_STATUS);
   }

   /* Get range of loop */
   xstart = MAX(0,-xsize-1);
   xstop  = MAX(0,xsize-1);
   xstep  = SIGN(xsize);
   ystart = MAX(0,-ysize-1);
   ystop  = MAX(0,ysize-1);
   ystep  = SIGN(ysize);

   /* Loop through images */
   while ((nread=fread(buffer, bytes_per_pixel, image_size, stdin))
                     == image_size) {

      /* Write out inverted image */
      for (j=ystart; ystep*j <= ystop; j += ystep) {
         offset=j*ABS(xsize);
         for (i=xstart, oi=0; xstep*i <= xstop; i += xstep, oi++) {
            for (k=0; k<bytes_per_pixel; k++) {
               outbuf[oi*bytes_per_pixel+k]=
                  buffer[(offset+i)*bytes_per_pixel+k];
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


