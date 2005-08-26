#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <minc.h>

#define TRUE 1
#define FALSE 0

#define MAX_IN_TYPES 2
#define MAX_NORM 2
#define MAX_OUT_TYPES 2
#define MAX_MAX 2
#define MAX_VAL 2

int main(int argc, char **argv)
{
   int icv, cdfid, img, max, min;
   static char *typenm[]={"short", "double"};
   static char *boolnm[] = {"true", "false"};
   static nc_type intypes[] = {NC_SHORT, NC_DOUBLE};
   static int norms[] = {TRUE, FALSE};
   static nc_type outtypes[] = {NC_SHORT, NC_DOUBLE};
   static int maxpresent[] = {TRUE, FALSE};
   static int valpresent[] = {TRUE, FALSE};
   static int dim[MAX_VAR_DIMS];
   static struct { long len; char *name;} diminfo[]=
      {3, MIzspace, 1, MIyspace, 1, MIxspace};
   static int numdims=sizeof(diminfo)/sizeof(diminfo[0]);
   static long coord[]={0,0,0};
   static long count[]={1,1,1};
   static double max_values[] = {0.4, 0.6, 0.8};
   double dvalue;
   short int ivalue;
   int i, intype, inorm, outtype, imax, ival;
   int cflag = 0;

#if MINC2
   if (argc == 2 && !strcmp(argv[1], "-2")) {
       cflag = MI2_CREATE_V2;
   }
#endif /* MINC2 */

   for (intype=0; intype<MAX_IN_TYPES; intype++) {
      for (inorm=0; inorm<MAX_NORM; inorm++) {
         icv=miicv_create();
         miicv_setint(icv, MI_ICV_TYPE, intypes[intype]);
         miicv_setint(icv, MI_ICV_DO_NORM, norms[inorm]);
         miicv_setdbl(icv, MI_ICV_VALID_MAX, 20000.0);
         miicv_setdbl(icv, MI_ICV_VALID_MIN, 0.0);
         for (outtype=0; outtype<MAX_OUT_TYPES; outtype++) {
            for (imax=0; imax<MAX_MAX; imax++) {
               for (ival=0; ival<MAX_VAL; ival++) {
                  printf(
                     "in : %s, out : %s, norm : %s, imgmax : %s, valid : %s\n",
                         typenm[intype], typenm[outtype], boolnm[inorm],
                         boolnm[imax], boolnm[ival], dvalue);
                  cdfid=micreate("test.mnc", NC_CLOBBER | cflag);
                  for (i=0; i<numdims; i++) 
                     dim[i]=ncdimdef(cdfid, diminfo[i].name, diminfo[i].len);
                  img=micreate_std_variable(cdfid, MIimage, outtypes[outtype],
                                            numdims, dim);
                  if (maxpresent[imax]) {
                     max=micreate_std_variable(cdfid, MIimagemax, NC_DOUBLE,
                                               1, dim);
                     min=micreate_std_variable(cdfid, MIimagemin, NC_DOUBLE,
                                               1, dim);
                  }
                  if (valpresent[ival]) {
                     dvalue = 32000;
                     ncattput(cdfid, img, MIvalid_max, NC_DOUBLE, 1, &dvalue);
                     dvalue = 0;
                     ncattput(cdfid, img, MIvalid_min, NC_DOUBLE, 1, &dvalue);
                  }
                  ncendef(cdfid);
                  if (maxpresent[imax]) {
                     for (i=0; i<3; i++) {
                        dvalue = max_values[i];
                        coord[0]=i;
                        ncvarput1(cdfid, max, coord, &dvalue);
                        dvalue = -dvalue;
                        ncvarput1(cdfid, min, coord, &dvalue);
                     }
                     coord[0]=0;
                  }
                  miicv_attach(icv, cdfid, img);
                  if (intypes[intype]==NC_DOUBLE) {
                     dvalue = 0.2;
                     miicv_put(icv, coord, count, &dvalue);
                  }
                  else {
                     ivalue = 12500;
                     miicv_put(icv, coord, count, &ivalue);
                  }
                  dvalue = 0;
                  mivarget1(cdfid, img, coord, NC_DOUBLE, MI_SIGNED, &dvalue);
                  printf("   file value = %g\n", dvalue);
                  if (intypes[intype]==NC_DOUBLE) {
                     miicv_get(icv, coord, count, &dvalue);
                  }
                  else {
                     miicv_get(icv, coord, count, &ivalue);
                     dvalue=ivalue;
                  }
                  printf("   icv value = %g\n", dvalue);
                  miclose(cdfid);
               }
            }
         }
         miicv_free(icv);
      }
   }
   
   return 0;
}
