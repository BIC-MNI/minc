/* ----------------------------- MNI Header -----------------------------------
@NAME       : thin_plate_spline.c
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: library of routines for warping/mapping transformations.
@METHOD     : The original source code for these routines are taken from
              program VOI, written by Weiqian Dai.
@GLOBALS    : 
@CALLS      : 
@CREATED    : Dec 2, 1991 LC
@MODIFIED   : Mon Apr  5 09:00:54 EST 1993 louis 
                - building new library routines, with prototypes
@MODIFIED   : Wed Jul  14 1993  david 
                - incorporated into libmni.c
---------------------------------------------------------------------------- */


#include <def_mni.h>

private  void mnewt(int ntrial,float x[], int dim, float tolx, float tolf,
                    float **bdefor, float **INVMLY, int num_marks);
private  void 
  usrfun(float x[], float **alpha, float bet[], float **bdefor, float **INVMLY, 
	 int num_marks, int dim, float *xout);
private  float return_r(float *cor1, float *cor2, int dim);
private  float FU(float r, int dim);

/* ----------------------------- MNI Header -----------------------------------
@NAME       : mappingf
@INPUT      : bdefor - numerical recipes array with 'num_marks' rows, and 'dim' cols,
                       contains the list of landmarks points in the 'source' volume
	      INVMLY - numerical recipes array with 'num_marks+dim+1' rows, and 'dim' cols,
	               contains the deformation vectors that define the thin plate spline
	      num_marks - number of landmark points
              icor   - array of float [1..dim] for the input coordinate in the
                       'source' volume space.
	      dim    - number of dimensions (either 2 or 3).
@OUTPUT     : rcor   - array of float [1..dim] of the output coordinate in the 
                       'target' volume.
@RETURNS    : nothing
@DESCRIPTION: 
              INVMLY to 'in' to get 'out'
@METHOD     : 
@GLOBALS    : none
@CALLS      : return_r, vector, free_vector
@CREATED    : Mon Apr  5 09:00:54 EST 1993
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  thin_plate_spline_transform(
    int     n_dimensions,
    int     n_points,
    float   **points,
    float   **displacements,
    Real    x,
    Real    y,
    Real    z,
    Real    *x_transformed,
    Real    *y_transformed,
    Real    *z_transformed )
{
    Boolean   markpoint;
    int       i, j;
    float     r,some_number,*tempcor,*coord_flt;
    float     input_point[N_DIMENSIONS], output_point[N_DIMENSIONS];

    /* f(x,y) =a_{1} + a_{x}x + a_{y}y + sum_{1}^{n}
     *          w_{i}U(|P_{i} - (x,y)|) 
     */

    ALLOC( tempcor, n_dimensions );
    ALLOC( coord_flt, n_dimensions );

    for (j=0;j<n_dimensions;j++){
        tempcor[j] = 0;
    }

    input_point[0] = x;
    input_point[1] = y;
    input_point[2] = z;

    for (i=0; i<n_points; i++){

        markpoint = TRUE;            /* set the point as the landmark point */
		                   /* check the point where is landmark */

        for (j=0;j<n_dimensions;j++)
	    coord_flt[j] = (float)points[i][j];

        for (j=0;(j<n_dimensions)&&markpoint;j++){
            if ((coord_flt[j] != input_point[j])){
                markpoint = FALSE;
            }
        }

        /* because the point at the landmark is not deformable, we use 
         * the closest point at landmark.
         */
        if (markpoint){
            input_point[0] = input_point[0] + 0.00001;
        }

        r = return_r(coord_flt,input_point,n_dimensions); 
        some_number = FU(r,n_dimensions);

        for (j=0;j<n_dimensions;j++){
            tempcor[j] = displacements[i][j]*some_number + tempcor[j];
        }
    } 

    for (j=0;j<n_dimensions;j++){
        output_point[j] = displacements[n_points][j]+tempcor[j];
    }

    for (j = 0; j<n_dimensions; j++){   
        for (i = 0; i<n_dimensions; i++){
            output_point[j] = displacements[n_points+i+1][j]*input_point[i] +
                              output_point[j];
        }
    }

    *x_transformed = output_point[0];
    *y_transformed = output_point[1];

    if( n_dimensions >= 3 )
        *z_transformed = output_point[2];

    FREE( tempcor );
    FREE( coord_flt );
}

public  void  thin_plate_spline_inverse_transform(
    int     n_dimensions,
    int     n_points,
    float   **points,
    float   **displacements,
    Real    x,
    Real    y,
    Real    z,
    Real    *x_transformed,
    Real    *y_transformed,
    Real    *z_transformed )
{
    float temp[N_DIMENSIONS];
  
    temp[0] = x;
    temp[1] = y;
    temp[2] = z;

    mnewt( 100, temp, n_dimensions, 0.01, 0.05, points, displacements,
           n_points ); 
    *x_transformed = temp[0];
    *y_transformed = temp[1];
    *z_transformed = temp[2];
}



/* ----------------------------- MNI Header -----------------------------------
@NAME       : mnewt
@INPUT      : ntrial - the upper limit on iterations 
              x      - array of float [1..dim] for the input coordinate in the
                       'target' volume space.
		  ---> x is changed, see @OUTPUT
	      dim    - number of dimensions (either 2 or 3).
	      num_marks - number of landmark points
	      tolx   - tolerance for fitting (routine stops when the point guessed
	               is closer than tolx to the target point).
	      tolf   - tolerance for fitting (routine stops when the point guessed
	               is closer than tolx to the target point).
              bdefor - numerical recipes array with 'num_marks' rows, and 'dim' cols,
                       contains the list of landmarks points in the 'source' volume
	      INVMLY - numerical recipes array with 'num_marks+dim+1' rows, and 'dim' cols,
	               contains the deformation vectors that define the thin plate spline
@OUTPUT     : x      - array of float [1..dim] of the output coordinate in the 
                       'source' volume.
@RETURNS    : nothing
@DESCRIPTION: calls mnewt, to nummerically calculate the inverse transformation stored in
              INVMLY to 'in' to get 'out'
@METHOD     : Given an initial guess in x,  for a root in 'dim' dimensions, take ntrial
              Newton-Raphson steps to improve the root.  Stop if the root converges in 
	      either summed variable increments tolx or summed function values tolf
              
@GLOBALS    : none
@CALLS      : ludcmp, lubksb, usrfun
@CREATED    : Mon Apr  5 09:00:54 EST 1993
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define FREERETURN {FREE2D(alpha);FREE(bet);FREE(indx);return;}

private  void mnewt(int ntrial,float x[], int dim, float tolx, float tolf,
                    float **bdefor, float **INVMLY, int num_marks)

{
  int k,i,*indx;
  float 
    errx,errf,d,
    *bet,**alpha,
    xout[N_DIMENSIONS];
  
   xout[0] = x[0];          /* assume that the result is equal to the input, */
   xout[1] = x[1];	    /* as if the transformation was identity         */
   if (dim>2)
     xout[2] = x[2];

   ALLOC( indx, dim + 1 );
   ALLOC( bet, dim + 1 );
   ALLOC2D( alpha, dim + 1, dim + 1 );

   for (k=1;k<=ntrial;k++) {   
			/* usrfun will build the matrix coefficients for
			   the linear approximation to the local function */
      usrfun(x, alpha, bet, bdefor, INVMLY, num_marks, dim, xout);

      errf=0.0;			/* Check for function convergence */
      for (i=1;i<=dim;i++) errf += fabs(bet[i]);
      if (errf <= tolf) FREERETURN
      ludcmp(alpha,dim,indx,&d); /* Solve the linear eqs using LU decomposition */
      lubksb(alpha,dim,indx,bet);
      errx=0.0;
      for (i=1;i<=dim;i++) {	/* check for root convergence */
	 errx += fabs(bet[i]);
	 x[i-1] += bet[i];	/* Update the solution */
      }
      if (errx <= tolx) FREERETURN
   }
   FREERETURN
}

#undef FREERETURN


/* ----------------------------- MNI Header -----------------------------------
@NAME       : usrfun
@INPUT      : x[]    - array of float [1..dim] of current guess of coordinate in
                       source volume space.
              bdefor - numerical recipes array with 'num_marks' rows, and 'dim' cols,
                       contains the list of landmarks points in the 'source' volume
	      
	      dim    - number of dimensions (either 2 or 3).
	      INVMLY - numerical recipes array with 'num_marks+dim+1' rows, and 'dim' cols,
	               contains the deformation vectors that define the thin plate spline
	      num_marks - number of landmark points
	      xout   - array of float [1..dim] of true answer in target space.
@OUTPUT     : alpha  - matrix[1..dim][1..dim] - linear matric coefficients.
	      bet    - vector[1..dim] - negative of the function values
@RETURNS    : nothing
@DESCRIPTION: 
@METHOD     : 
@GLOBALS    : none
@CALLS      : 
@CREATED    : Mon Apr  5 09:00:54 EST 1993
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  void 
  usrfun(float x[], float **alpha, float bet[], float **bdefor, float **INVMLY, 
	 int num_marks, int dim, float *xout)

/* NOTE: Now this function will only work for 3-D case */
{
   register int i, j, k;
   float *d;

   ALLOC( d, num_marks );

   for (i=1; i<=dim; i++){	/* build up the matrix of linear parameters */
      for (j=1; j<=dim; j++){
         alpha[i][j] = 0;
      }
   }


   for (i=0; i<num_marks; i++)  /* build distances array */
       d[i] = return_r( x, bdefor[i], dim );

   for (i = 1; i<= dim; i++){	/* Estimate the linear parameters for the error function */
      for (j = 1; j <= dim; j++){
         alpha[i][j] = 0;
         for (k = 1; k <= num_marks; k++){
            alpha[i][j] += (x[j-1] - bdefor[k-1][j-1])/d[k-1] *INVMLY[k-1][i-1];
         }
         alpha[i][j] += INVMLY[k+j-1][i-1];
      }
   }

   for (i=1; i<=dim; i++){
      bet[i] = 0;
      for (k=1; k<=num_marks; k++){
         bet[i] += d[k-1]*INVMLY[k-1][i-1];
      }
      bet[i] += INVMLY[k-1][i-1] +
                INVMLY[k+1-1][i-1]*x[0] +
                INVMLY[k+2-1][i-1]*x[1] +
                INVMLY[k+3-1][i-1]*x[2] - xout[i-1];
      bet[i] = -bet[i];
   }

   FREE( d );
}



/*===============================================================*/



private  float return_r(float *cor1, float *cor2, int dim)
{
   float r1,r2,r3;


   if (dim == 1){
      r1 = cor1[0] - cor2[0]; 
      r1 = fabs(r1);
      return(r1);
   }
   else if (dim == 2){
      r1 = cor1[0] - cor2[0];
      r2 = cor1[1] - cor2[1];
      return(r1*r1+r2*r2);
   }
   else if (dim == 3){
      r1 = cor1[0] - cor2[0];
      r2 = cor1[1] - cor2[1];
      r3 = cor1[2] - cor2[2];
      return( (float) sqrt((double)(r1*r1 + r2*r2 + r3*r3)) );
   } 
   else {
      HANDLE_INTERNAL_ERROR( " impossible error in return_r" );
      return( 0.0 );
   }
}

private  float FU(float r, int dim)
/* This function will calculate the U(r) function.
 * if dim = 1, the funtion returns |r|^3 
 * if dim = 2, the funtion retruns r^2*log r^2
 * if dim = 3, the funtion returns |r|
 */ 
{
   float z;

   if (dim==1){
      z = r*r*r;
      return(fabs(z));
   }
   else if (dim==2){/* r is stored as r^2 */
      z = r * log((double) r);
      return(z);
   }
   else if (dim==3){
      return(fabs(r));
   }
   else {
      HANDLE_INTERNAL_ERROR( " impossible error in FU" );
      return( 0.0 );
   }
}
