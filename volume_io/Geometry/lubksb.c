#include  <internal_volume_io.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Geometry/Attic/lubksb.c,v 1.3 1994-11-25 14:19:28 david Exp $";
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : lubksb
@INPUT      : a
              n
              indx
@OUTPUT     : b
@RETURNS    : 
@DESCRIPTION: From numerical recipes
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void lubksb(
    float **a,
    int   n,
    int   indx[],
    float b[] )
{
   int   i,ii=0,ip,j;
   float sum;

   for (i=1;i<=n;i++) {
      ip=indx[i];
      sum=b[ip];
      b[ip]=b[i];
      if (ii)
	 for (j=ii;j<=i-1;j++) sum -= a[i][j]*b[j];
      else if (sum) ii=i;
	 b[i]=sum;
   }
   for (i=n;i>=1;i--) {
      sum=b[i];
      for (j=i+1;j<=n;j++) sum -= a[i][j]*b[j];
	 b[i]=sum/a[i][i];
   }
}
