/* ----------------------------- MNI Header -----------------------------------
@NAME       : test
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: 
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 
@MODIFIED   : 
---------------------------------------------------------------------------- */
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <limits.h>
#include <minc.h>

struct {
   nc_type type;
   char *sign;
   char *ctype;
} types[]={NC_BYTE,   MI_UNSIGNED, "byte",
           NC_BYTE,   MI_SIGNED,   "byte",
           NC_SHORT,  MI_UNSIGNED, "short",
           NC_SHORT,  MI_SIGNED,   "short",
           NC_INT,    MI_UNSIGNED, "int",
           NC_INT,    MI_SIGNED,   "int",
           NC_FLOAT,  MI_SIGNED,   "float",
           NC_DOUBLE, MI_SIGNED,   "double"};
int ntypes = sizeof(types)/sizeof(types[0]);

main(int argc, char **argv)
{
   int cdf;
   int img, img2;
   int dim[MAX_VAR_DIMS];
   long start[MAX_VAR_DIMS];
   long count[MAX_VAR_DIMS];
   double image[256*256];
   long limage[256*256];
   int i, j, k, ioff, itype, jtype;
   int cflag = 0;

#ifdef MINC2
   if (argc == 2 && !strcmp(argv[1], "-2")) {
       cflag = MI2_CREATE_V2;
   }
#endif /* MINC2 */

   ncopts=NC_VERBOSE|NC_FATAL;
   for (itype=0; itype<ntypes; itype++) {
      for (jtype=0; jtype<ntypes; jtype++) {
         cdf=micreate("test.mnc",NC_CLOBBER | cflag);
         count[2]=256;
         count[1]=20;
         count[0]=7;
         dim[2]=ncdimdef(cdf, MIzspace, count[2]);
         dim[1]=ncdimdef(cdf, MIxspace, count[1]);
         dim[0]=ncdimdef(cdf, MIyspace, count[0]);
         img=ncvardef(cdf, MIimage, types[itype].type, 1, dim);
         (void) miattputstr(cdf, img, MIsigntype, types[itype].sign);
         img2=ncvardef(cdf, "image2", types[jtype].type, 1, dim);
         (void) miattputstr(cdf, img2, MIsigntype, types[jtype].sign);
         image[0]=10.0;
         image[1]=2.0*(-FLT_MAX);
         image[2]=2.0*FLT_MAX;
         image[3]=3.2;
         image[4]=3.7;
         image[5]=(-3.2);
         image[6]=(-3.7);
#if 0
   for (j=0; j<count[0]; j++) {
      for (i=0; i<count[1]; i++) {
         ioff=(j*count[1]+i)*count[2];
         for (k=0; k<count[2]; k++)
            image[ioff+k]=ioff+k+10;
      }
   }
#endif
         (void) ncendef(cdf);
         (void) miset_coords(3,0L,start);
         (void) mivarput(cdf, img, start, count, NC_DOUBLE, NULL, image);

         (void) mivarget(cdf, img, start, count, types[jtype].type,
                         types[jtype].sign, image);
         (void) mivarput(cdf, img2, start, count, types[jtype].type,
                         types[jtype].sign, image);

         (void) mivarget(cdf, img2, start, count, NC_DOUBLE, NULL, image);
         (void) printf("Image 1 : %s %s\n",types[itype].sign,
                       types[itype].ctype);
         (void) printf("Image 2 : %s %s\n",types[jtype].sign,
                       types[jtype].ctype);
         for (i=0;i<count[0];i++)
            (void) printf("   image[%d] = %g\n",(int) i, image[i]);
         (void) miclose(cdf);
      }
   }
   return(0);
}
