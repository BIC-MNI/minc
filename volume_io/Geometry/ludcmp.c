#include  <def_mni.h>

#define TINY 1.0e-20

public  void ludcmp(
    float    **a,
    int      n,
    int      *indx,
    float    *d )
{
	int i,imax,j,k;
	float big,dum,sum,temp;
	float *vv;

        ALLOC( vv, n + 1 );
	*d=1.0;
	for (i=1;i<=n;i++) {
                indx[i] = i; /* initialize the row number */
		big=0.0;
		for (j=1;j<=n;j++)
			if ((temp=fabs((double) a[i][j])) > big) big=temp;
		if (big == 0.0) print("Singular matrix in routine LUDCMP");
		vv[i]=1.0/big;
	}
	for (j=1;j<=n;j++) {
		for (i=1;i<j;i++) {
			sum=a[i][j];
			for (k=1;k<i;k++) sum -= a[i][k]*a[k][j];
			a[i][j]=sum;
		}
		big=0.0;
		for (i=j;i<=n;i++) {
			sum=a[i][j];
			for (k=1;k<j;k++)
				sum -= a[i][k]*a[k][j];
			a[i][j]=sum;
			if ( (dum=vv[i]*fabs((double) sum)) > big) {
				big=dum;
				imax=i;
			}
		}
		if (j != imax) {
			for (k=1;k<=n;k++) {
				dum=a[imax][k];
				a[imax][k]=a[j][k];
				a[j][k]=dum;
			}
			*d = -(*d);
			vv[imax]=vv[j]; 
                  /*      temp_row = indx[j];
                        indx[j] = imax;
                        indx[imax] = temp_row; */
		}
		indx[j]=imax; 
		if (a[j][j] == 0.0) a[j][j]=TINY;
		if (j != n) {
			dum=1.0/(a[j][j]);
			for (i=j+1;i<=n;i++) a[i][j] *= dum;
		}
	}
/*        printf("This is in the ludcmp\n");
        printmatrix(a,n);*/
	FREE(vv);
}

#undef TINY
