#include <stdio.h>
#include <minc.h>

#define TRUE 1
#define FALSE 0

main()
{
   int icv, cdfid, img, max, min, dimvar;
   static int dim[MAX_VAR_DIMS];
   static struct { long len; char *name;} diminfo[]=
      {3, MIzspace, 4, MIyspace, 5, MIxspace};
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
   int i, j, k, intype, inorm, outtype, imax, ival;

   icv=miicv_create();
   miicv_setint(icv, MI_ICV_DO_NORM, TRUE);
   cdfid=nccreate("test.mnc", NC_CLOBBER);
   for (i=0; i<numdims; i++) {
      dim[i]=ncdimdef(cdfid, diminfo[i].name, diminfo[i].len);
      dimvar=micreate_std_variable(cdfid, diminfo[i].name, NC_DOUBLE,
                                   0, &dim[i]);
      miattputdbl(cdfid, dimvar, MIstep, 0.8);
      miattputdbl(cdfid, dimvar, MIstart, 22.0);
   }
   
   img=micreate_std_variable(cdfid, MIimage, NC_INT,
                             numdims, dim);
   (void) miattputdbl(cdfid, img, MIvalid_max, 32767.0);
   (void) miattputdbl(cdfid, img, MIvalid_min, -32768.0);
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
   ncclose(cdfid);
   miicv_free(icv);
   
   return 0;
}


