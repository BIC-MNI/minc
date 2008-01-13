#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <float.h>
#include <minc.h>

#define TRUE 1
#define FALSE 0

int main(int argc, char **argv)
{
   int icv, mincid, img, i;
   static int dim[MAX_VAR_DIMS];
   static struct { long len; char *name;} diminfo[] = {
       { 4, MIyspace }, 
       { 5, MIxspace}
   };
   static int numdims=sizeof(diminfo)/sizeof(diminfo[0]);
   static long coord[]={0,0};
   static long count[]={4,5};
   static short int ivalue[]={
      111, 113, 115, 117, 119,
      121, 123, 125, 127, 129,
      131, 133, 135, 137, 139,
      141, 143, 145, 147, -5
   };
   static int ivallen = sizeof(ivalue)/sizeof(ivalue[0]);
   int cflag = 0;

#if MINC2
   if (argc == 2 && !strcmp(argv[1], "-2")) {
       cflag = MI2_CREATE_V2;
   }
#endif /* MINC2 */

   icv = miicv_create();
   (void) miicv_setint(icv, MI_ICV_VALID_MAX, 200);
   (void) miicv_setint(icv, MI_ICV_VALID_MIN, 0);
   (void) miicv_setint(icv, MI_ICV_DO_FILLVALUE, TRUE);
   mincid=micreate("test.mnc", NC_CLOBBER | cflag);
   for (i=0; i<2; i++) {
      dim[i]=ncdimdef(mincid, diminfo[i].name, diminfo[i].len);
   }
   img=micreate_std_variable(mincid, MIimage, NC_SHORT,
                             numdims, dim);
   (void) miattputint(mincid, img, MIvalid_max, 200);
   (void) miattputint(mincid, img, MIvalid_min, 0);
   (void) ncendef(mincid);
   (void) miicv_attach(icv, mincid, img);

   (void) miicv_put(icv, coord, count, ivalue);

   for (i=0; i<ivallen; i++)
      ivalue[i]=SHRT_MAX;
   (void) miicv_get(icv, coord, count, ivalue);
   (void) printf("\nNo dimconv, default fillvalue:\n");
   for (i=0; i<ivallen; i++) {
      (void) printf("%d ", (int) ivalue[i]);
      if ((i % 5) == 4) (void) printf("\n");
   }

   for (i=0; i<ivallen; i++)
      ivalue[i]=SHRT_MAX;
   (void) miicv_detach(icv);
   (void) miicv_setdbl(icv, MI_ICV_FILLVALUE, -32.0);
   (void) miicv_attach(icv, mincid, img);
   (void) miicv_get(icv, coord, count, ivalue);
   (void) printf("\nNo dimconv, fillvalue=-32:\n");
   for (i=0; i<ivallen; i++) {
      (void) printf("%d ", (int) ivalue[i]);
      if ((i % 5) == 4) (void) printf("\n");
   }

   for (i=0; i<ivallen; i++)
      ivalue[i]=SHRT_MAX;
   (void) miicv_detach(icv);
   (void) miicv_setint(icv, MI_ICV_DO_DIM_CONV, TRUE);
   (void) miicv_setint(icv, MI_ICV_KEEP_ASPECT, FALSE);
   (void) miicv_setint(icv, MI_ICV_BDIM_SIZE, 2);
   (void) miicv_attach(icv, mincid, img);
   count[0]=2;
   (void) miicv_get(icv, coord, count, ivalue);
   (void) printf("\nWith dimconv, fillvalue=-32:\n");
   for (i=0; i<ivallen; i++) {
      (void) printf("%d ", (int) ivalue[i]);
      if ((i % 5) == 4) (void) printf("\n");
   }

   (void) miclose(mincid);

}
