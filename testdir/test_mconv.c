#if HAVE_CONFIG_H
#include "config.h"
#endif
#include <minc.h>

#ifndef NULL
#  define NULL ((void *) 0)
#endif

int main()
{
   char *file = "test.mnc";
   int cdfid;
   int dim[MAX_VAR_DIMS];
   int imgid;

   cdfid=nccreate(file, NC_CLOBBER);
   dim[0]=ncdimdef(cdfid, MIzspace, 3L);
   dim[1]=ncdimdef(cdfid, MIyspace, 5L);
   dim[2]=ncdimdef(cdfid, MIxspace, 6L);
   imgid=micreate_std_variable(cdfid, MIimage, NC_SHORT, 3, dim);
   (void) micreate_std_variable(cdfid, MIimagemax, NC_DOUBLE, 1, dim);
   (void) micreate_std_variable(cdfid, MIimagemin, NC_DOUBLE, 1, dim);
   (void) micreate_std_variable(cdfid, MIzspace, NC_DOUBLE, 1, NULL);
   (void) micreate_std_variable(cdfid, MIzspace_width, NC_DOUBLE, 1, &dim[0]);
   (void) micreate_group_variable(cdfid, MIpatient);
   (void) micreate_group_variable(cdfid, MIstudy);
   (void) micreate_group_variable(cdfid, MIacquisition);
   (void) ncclose(cdfid);

   return 0;
}
