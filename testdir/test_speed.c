#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <minc.h>
#include <float.h>

#define TRUE 1
#define FALSE 0

#define NORMAL_STATUS 0
#define ERROR_STATUS 1

char *nctypename(nc_type datatype)
{
    switch (datatype) {
    case NC_BYTE:
        return "byte";
    case NC_CHAR:
        return "char";
    case NC_SHORT:
        return "short";
    case NC_INT:
        return "int";
    case NC_FLOAT:
        return "float";
    case NC_DOUBLE:
        return "double";
    }
}

int
test_icv_read(char *filename, int xsize, int ysize, double image_min,
              double image_max, nc_type datatype, char *signtype)
{
   int icv, cdfid, img, ndims;
   static long coord[MAX_VAR_DIMS];
   static long count[MAX_VAR_DIMS];
   int dim[MAX_VAR_DIMS];
   long dim_size;
   unsigned char *image;
   int i;
   double min, max;
   int n;

   min = DBL_MAX;
   max = -DBL_MAX;

   image = malloc(xsize * ysize * nctypelen(datatype));
   if (image == NULL) {
       return (ERROR_STATUS);
   }

   /* Create the icv */
   icv=miicv_create();
   (void) miicv_setint(icv, MI_ICV_XDIM_DIR, MI_ICV_POSITIVE);
   (void) miicv_setint(icv, MI_ICV_YDIM_DIR, MI_ICV_POSITIVE);
   (void) miicv_setint(icv, MI_ICV_ZDIM_DIR, MI_ICV_POSITIVE);
   (void) miicv_setint(icv, MI_ICV_ADIM_SIZE, xsize);
   (void) miicv_setint(icv, MI_ICV_BDIM_SIZE, ysize);
   (void) miicv_setint(icv, MI_ICV_KEEP_ASPECT, FALSE);
   (void) miicv_setint(icv, MI_ICV_DO_DIM_CONV, TRUE);
   (void) miicv_setint(icv, MI_ICV_TYPE, datatype);
   (void) miicv_setstr(icv, MI_ICV_SIGN, signtype);
   (void) miicv_setdbl(icv, MI_ICV_VALID_MAX, image_max);
   (void) miicv_setdbl(icv, MI_ICV_VALID_MIN, image_min);

   /* Open the file, attach the image variable */
   cdfid=miopen(filename, NC_NOWRITE);

   /* Attach image variable */
   img=ncvarid(cdfid, MIimage);
   (void) miicv_attach(icv, cdfid, img);

   /* Get the number of dimensions and modify count and coord */
   (void) ncvarinq(cdfid, img, NULL, NULL, &ndims, dim, NULL);
   if (ndims!=3) {
      (void) fprintf(stderr, "File must have 3 dimensions\n");
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

      switch (datatype) {
      case NC_BYTE:
          if (!strcmp(signtype, MI_UNSIGNED)) {
              for (n = 0; n < xsize*ysize; n++) {
                  unsigned char uc = *(((unsigned char *)image) + n);
                  if (uc > max) {
                      max = uc;
                  }
                  if (uc < min) {
                      min = uc;
                  }
              }
          }
          else {
              for (n = 0; n < xsize*ysize; n++) {
                  signed char sc = *(((signed char *)image) + n);
                  if (sc > max) {
                      max = sc;
                  }
                  if (sc < min) {
                      min = sc;
                  }
              }
          }
          break;
      case NC_SHORT:
          if (!strcmp(signtype, MI_UNSIGNED)) {
              for (n = 0; n < xsize*ysize; n++) {
                  unsigned short uc = *(((unsigned short *)image) + n);
                  if (uc > max) {
                      max = uc;
                  }
                  if (uc < min) {
                      min = uc;
                  }
              }
          }
          else {
              for (n = 0; n < xsize*ysize; n++) {
                  signed short sc = *(((signed short *)image) + n);
                  if (sc > max) {
                      max = sc;
                  }
                  if (sc < min) {
                      min = sc;
                  }
              }
          }
          break;
      case NC_INT:
          if (!strcmp(signtype, MI_UNSIGNED)) {
              for (n = 0; n < xsize*ysize; n++) {
                  unsigned int uc = *(((unsigned int *)image) + n);
                  if (uc > max) {
                      max = uc;
                  }
                  if (uc < min) {
                      min = uc;
                  }
              }
          }
          else {
              for (n = 0; n < xsize*ysize; n++) {
                  signed int sc = *(((signed int *)image) + n);
                  if (sc > max) {
                      max = sc;
                  }
                  if (sc < min) {
                      min = sc;
                  }
              }
          }
          break;
      case NC_FLOAT:
          for (n = 0; n < xsize*ysize; n++) {
              float sc = *(((float *)image) + n);
              if (sc > max) {
                  max = sc;
              }
              if (sc < min) {
                  min = sc;
              }
          }
          break;
      case NC_DOUBLE:
          for (n = 0; n < xsize*ysize; n++) {
              double sc = *(((double *)image) + n);
              if (sc > max) {
                  max = sc;
              }
              if (sc < min) {
                  min = sc;
              }
          }
          break;
      }
      printf("%d %s %s %f %f\n", i, signtype, nctypename(datatype), min, max);
   }

   /* Close the file and free the icv */
   (void) miclose(cdfid);
   (void) miicv_free(icv);

   free(image);
   return (NORMAL_STATUS);
}

main(int argc, char *argv[])
{
   int xsize, ysize;
   double image_max, image_min;
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

   test_icv_read(filename, xsize, ysize, image_min, image_max, 
                 NC_BYTE, MI_UNSIGNED);
   test_icv_read(filename, xsize, ysize, image_min, image_max, 
                 NC_SHORT, MI_UNSIGNED);
   test_icv_read(filename, xsize, ysize, image_min, image_max, 
                 NC_INT, MI_UNSIGNED);

   test_icv_read(filename, xsize, ysize, image_min, image_max, 
                 NC_BYTE, MI_SIGNED);
   test_icv_read(filename, xsize, ysize, image_min, image_max, 
                 NC_SHORT, MI_SIGNED);
   test_icv_read(filename, xsize, ysize, image_min, image_max, 
                 NC_INT, MI_SIGNED);

   test_icv_read(filename, xsize, ysize, image_min, image_max, 
                 NC_FLOAT, MI_SIGNED);
   test_icv_read(filename, xsize, ysize, image_min, image_max, 
                 NC_DOUBLE, MI_SIGNED);

   return NORMAL_STATUS;
}


