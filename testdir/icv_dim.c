#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <minc.h>

#define TRUE 1
#define FALSE 0

main(int argc, char **argv)
{
   int icv, cdfid, img, max, min, dimvar;
   static int dim[MAX_VAR_DIMS];
   static struct { long len; char *name;} diminfo[]=
      {3, MIzspace, 9, MIyspace, 2, MIxspace};
/*   static struct { long len; char *name;} diminfo[]=
      {3, MIzspace, 4, MIyspace, 5, MIxspace}; */
   static int numdims=sizeof(diminfo)/sizeof(diminfo[0]);
   static long coord[]={0,0,0};
   static long count[]={3,4,5};
   double dvalue;
   short int ivalue[]={
      111, 113, 115, 117, 119,
      121, 123, 125, 127, 129,
      131, 133, 135, 137, 139,
      141, 143, 145, 147, 149,
      211, 213, 215, 217, 219,
      221, 223, 225, 227, 229,
      231, 233, 235, 237, 239,
      241, 243, 245, 247, 249,
      311, 313, 315, 317, 319,
      321, 323, 325, 327, 329,
      331, 333, 335, 337, 339,
      341, 343, 345, 347, 349
   };
   int i, j, k;
   int cflag = 0;

#if MINC2
   if (argc == 2 && !strcmp(argv[1], "-2")) {
       cflag = MI2_CREATE_V2;
   }
#endif /* MINC2 */

   icv=miicv_create();
   miicv_setint(icv, MI_ICV_XDIM_DIR, MI_ICV_NEGATIVE);
   miicv_setint(icv, MI_ICV_YDIM_DIR, MI_ICV_NEGATIVE);
   miicv_setint(icv, MI_ICV_ZDIM_DIR, MI_ICV_NEGATIVE);
   miicv_setint(icv, MI_ICV_ADIM_SIZE, 5);
   miicv_setint(icv, MI_ICV_BDIM_SIZE, 4);
   miicv_setint(icv, MI_ICV_DO_DIM_CONV, TRUE);
   miicv_setint(icv, MI_ICV_KEEP_ASPECT, FALSE);
   miicv_setint(icv, MI_ICV_DO_NORM, TRUE);
   cdfid=micreate("test.mnc", NC_CLOBBER | cflag);
   for (i=0; i<numdims; i++) {
      dim[i]=ncdimdef(cdfid, diminfo[i].name, diminfo[i].len);
      dimvar=micreate_std_variable(cdfid, diminfo[i].name, NC_DOUBLE,
                                   0, &dim[i]);
      miattputdbl(cdfid, dimvar, MIstep, 0.8);
      miattputdbl(cdfid, dimvar, MIstart, 22.0);
   }
   
   img=micreate_std_variable(cdfid, MIimage, NC_SHORT,
                             numdims, dim);
   max=micreate_std_variable(cdfid, MIimagemax, NC_DOUBLE, 1, dim);
   min=micreate_std_variable(cdfid, MIimagemin, NC_DOUBLE, 1, dim);
   ncendef(cdfid);
   for (i=0; i<diminfo[0].len; i++) {
      dvalue = 200;
      coord[0]=i;
      ncvarput1(cdfid, max, coord, &dvalue);
      dvalue = -dvalue;
      ncvarput1(cdfid, min, coord, &dvalue);
   }
   coord[0]=0;
   miicv_attach(icv, cdfid, img);
   miicv_inqdbl(icv, MI_ICV_ADIM_START, &dvalue);
   printf("adim start = %g", dvalue);
   miicv_inqdbl(icv, MI_ICV_ADIM_STEP, &dvalue);
   printf(", step = %g\n", dvalue);
   miicv_inqdbl(icv, MI_ICV_BDIM_START, &dvalue);
   printf("bdim start = %g", dvalue);
   miicv_inqdbl(icv, MI_ICV_BDIM_STEP, &dvalue);
   printf(", step = %g\n", dvalue);
   miicv_inqdbl(icv, MI_ICV_NORM_MAX, &dvalue);
   printf("norm : max = %g", dvalue);
   miicv_inqdbl(icv, MI_ICV_NORM_MIN, &dvalue);
   printf(", min = %g\n", dvalue);
   miicv_put(icv, coord, count, ivalue);
   for (i=0; i< sizeof(ivalue)/sizeof(ivalue[0]); i++)
      ivalue[i] = 0;
   miicv_get(icv, coord, count, ivalue);
   for (i=0; i<3; i++) {
      for (j=0; j<4; j++) {
         for (k=0; k<5; k++) {
            printf("%5d",ivalue[i*20+j*5+k]);
         }
         printf("\n");
      }
   }
   miclose(cdfid);
   miicv_free(icv);
   
   return 0;
}


