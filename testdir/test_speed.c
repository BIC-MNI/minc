#include <stdio.h>
#include <stdlib.h>
#include <minc.h>

#define TRUE 1
#define FALSE 0

#define NORMAL_STATUS 0
#define ERROR_STATUS 1

main(int argc, char *argv[])
{
   int icv, cdfid, img, ndims;
   int xsize, ysize;
   double image_max, image_min;
   static long coord[MAX_VAR_DIMS];
   static long count[MAX_VAR_DIMS];
   int dim[MAX_VAR_DIMS];
   long dim_size;
   unsigned char image[512*512];
   int i, quit;
   char *pname, *filename;

   /* Parse the command line */
   pname=argv[0];
   if (argc!=6) {
      (void) fprintf(stderr, 
         "Usage: %s <filename> <xsize> <ysize> <min> <max>\n", pname);
      return ERROR_STATUS;
   }
   filename=argv[1];
   xsize=atoi(argv[2]);
   ysize=atoi(argv[3]);
   image_min=atof(argv[4]);
   image_max=atof(argv[5]);

   /* Create the icv */
   icv=miicv_create();
   (void) miicv_setint(icv, MI_ICV_XDIM_DIR, MI_ICV_POSITIVE);
   (void) miicv_setint(icv, MI_ICV_YDIM_DIR, MI_ICV_POSITIVE);
   (void) miicv_setint(icv, MI_ICV_ZDIM_DIR, MI_ICV_POSITIVE);
   (void) miicv_setint(icv, MI_ICV_ADIM_SIZE, xsize);
   (void) miicv_setint(icv, MI_ICV_BDIM_SIZE, ysize);
   (void) miicv_setint(icv, MI_ICV_KEEP_ASPECT, FALSE);
   (void) miicv_setint(icv, MI_ICV_DO_DIM_CONV, TRUE);
   (void) miicv_setint(icv, MI_ICV_TYPE, NC_BYTE);
   (void) miicv_setstr(icv, MI_ICV_SIGN, MI_UNSIGNED);
   (void) miicv_setdbl(icv, MI_ICV_VALID_MAX, image_max);
   (void) miicv_setdbl(icv, MI_ICV_VALID_MIN, image_min);

   /* Open the file, attach the image variable */
   cdfid=ncopen(filename, NC_NOWRITE);

   /* Attach image variable */
   img=ncvarid(cdfid, MIimage);
   (void) miicv_attach(icv, cdfid, img);

   /* Get the number of dimensions and modify count and coord */
   (void) ncvarinq(cdfid, img, NULL, NULL, &ndims, dim, NULL);
   if (ndims!=3) {
      (void) fprintf(stderr, "%s: File must have 3 dimensions\n", pname);
      return ERROR_STATUS;
   }
   (void) ncdiminq(cdfid, dim[0], NULL, &dim_size);
   count[0]=1;
   count[1]=ysize;
   count[2]=xsize;
   coord[1]=0;
   coord[2]=0;

   /* Get the data */
   for (i=0; i<dim_size; i++) {
      coord[0]=i;
      (void) miicv_get(icv, coord, count, image);
   }

   /* Close the file and free the icv */
   (void) ncclose(cdfid);
   (void) miicv_free(icv);

   return NORMAL_STATUS;
}


