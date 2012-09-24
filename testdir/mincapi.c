#define _GNU_SOURCE 1
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <minc.h>
#include <limits.h>
#include <float.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#define FUNC_ERROR(x) (fprintf(stderr, "On line %d, function %s failed unexpectedly\n", __LINE__, x), ++errors)

#define TST_X 0
#define TST_Y 1
#define TST_Z 2

long errors = 0;

extern void icv_tests(void);

#define XSIZE 20
#define YSIZE 30
#define ZSIZE 40

#define YBOOST 10
#define ZBOOST 20

static struct dimdef {
  char * name;
  int length;
} dimtab1[3] = { 
  { MIxspace, XSIZE }, 
  { MIyspace, YSIZE },
  { MIzspace, ZSIZE }
};

struct testinfo {
  char *name;
  int fd;
  int maxid;
  int minid;
  int imgid;
  int dim[3];
};

/* Test case 1 - file creation & definition. 
 */
int
test1(struct testinfo *ip, struct dimdef *dims, int ndims)
{
  int fd2;
  int varid;
  int stat;
  int i;

  /* Test case #1 - file creation 
   */
  ip->name = micreate_tempfile();
  if (ip->name == NULL) {
    FUNC_ERROR("micreate_tempfile\n");
  }

  ip->fd = micreate(ip->name, NC_CLOBBER);
  if (ip->fd < 0) {
    FUNC_ERROR("micreate");
  }

  /* Try to create another file of the same name - should fail. 
   */
  fd2 = micreate(ip->name, NC_NOCLOBBER);
  if (fd2 >= 0) {
    FUNC_ERROR("micreate");
  }

  /* Try to open the file for write - should fail. 
   */
  /* VF: it doesn't fail!
  fd2 = miopen(ip->name, NC_WRITE);
  if (fd2 >= 0) {
    FUNC_ERROR("miopen");
  } */   

  /* Have to use ncdimdef() here since there is no MINC equivalent.  Sigh. 
   */
  for (i = 0; i < ndims; i++) {

    /* Define the dimension 
     */
    ip->dim[i] = ncdimdef(ip->fd, dims[i].name, dims[i].length);
    if (ip->dim[i] < 0) {
      FUNC_ERROR("ncdimdef");
    }

    /* Create the dimension variable.
     */
    varid = micreate_std_variable(ip->fd, dims[i].name, NC_DOUBLE, 0, 
				  &ip->dim[i]);
    if (varid < 0) {
      FUNC_ERROR("micreate_std_variable");
    }
    stat = miattputdbl(ip->fd, varid, MIstep, 0.8);
    if (stat < 0) {
      FUNC_ERROR("miattputdbl");
    }
    stat = miattputdbl(ip->fd, varid, MIstart, 22.0);
    if (stat < 0) {
      FUNC_ERROR("miattputdbl");
    }
  }

  /* Try to create a bogus variable.  This should trigger an error.
   */
  varid = micreate_std_variable(ip->fd, "xyzzy", NC_DOUBLE, 0, NULL);
  if (varid >= 0) {
    FUNC_ERROR("micreate_std_variable");
  }

  /* Create the image-max variable.
   */
  ip->maxid = micreate_std_variable(ip->fd, MIimagemax, NC_FLOAT, 0, NULL);
  if (ip->maxid < 0) {
    FUNC_ERROR("micreate_std_variable");
  }

  /* Create the image-min variable.
   */
  ip->minid = micreate_std_variable(ip->fd, MIimagemin, NC_FLOAT, 0, NULL);
  if (ip->minid < 0) {
    FUNC_ERROR("micreate_std_variable");
  }

  ip->imgid = micreate_std_variable(ip->fd, MIimage, NC_INT, ndims, ip->dim);
  if (ip->imgid < 0) {
    FUNC_ERROR("micreate_std_variable");
  }
  return (0);
}

int 
test2(struct testinfo *ip, struct dimdef *dims, int ndims)
{
  int i, j, k;
  int stat;
  long coords[3];
  float flt;

  stat = miattputdbl(ip->fd, ip->imgid, MIvalid_max, (XSIZE * 10000.0));
  if (stat < 0) {
    FUNC_ERROR("miattputdbl");
  }

  stat = miattputdbl(ip->fd, ip->imgid, MIvalid_min, -(XSIZE * 10000.0));
  if (stat < 0) {
    FUNC_ERROR("miattputdbl");
  }

  ncendef(ip->fd);		/* End definition mode. */

  coords[0] = 0;

  flt = -(XSIZE * 100000.0);
  stat = mivarput1(ip->fd, ip->minid, coords, NC_FLOAT, MI_SIGNED, &flt);
  if (stat < 0) {
    FUNC_ERROR("mivarput1");
  }
    
  flt = XSIZE * 100000.0;
  stat = mivarput1(ip->fd, ip->maxid, coords, NC_FLOAT, MI_SIGNED, &flt);
  if (stat < 0) {
    FUNC_ERROR("mivarput1");
  }

  for (i = 0; i < dims[TST_X].length; i++) {
    for (j = 0; j < dims[TST_Y].length; j++) {
      for (k = 0; k < dims[TST_Z].length; k++) {
	int tmp = (i * 10000) + (j * 100) + k;

	coords[TST_X] = i;
	coords[TST_Y] = j;
	coords[TST_Z] = k;

	stat = mivarput1(ip->fd, ip->imgid, coords, NC_INT, MI_SIGNED, &tmp);
	if (stat < 0) {
	  fprintf(stderr, "At (%d,%d,%d), status %d: ", i,j,k,stat);
	  FUNC_ERROR("mivarput1");
	}
      }
    }
  }

  return (0);
}

int
test3(struct testinfo *ip, struct dimdef *dims, int ndims)
{
  /* Try to read the data back. */
  size_t total;
  long coords[3];
  long lengths[3];
  void *buf_ptr;
  int *int_ptr;
  int i, j, k;
  int stat;

  total = 1;
  for (i = 0; i < ndims; i++) {
    total *= dims[i].length;
  }

  buf_ptr = malloc(total * sizeof (int));

  if (buf_ptr == NULL) {
    fprintf(stderr, "Oops, malloc failed\n");
    return (-1);
  }

  coords[TST_X] = 0;
  coords[TST_Y] = 0;
  coords[TST_Z] = 0;
  lengths[TST_X] = dims[TST_X].length;
  lengths[TST_Y] = dims[TST_Y].length;
  lengths[TST_Z] = dims[TST_Z].length;

  stat = mivarget(ip->fd, ip->imgid, coords, lengths, NC_INT, MI_SIGNED, 
		  buf_ptr);
  if (stat < 0) {
    FUNC_ERROR("mivarget");
  }

  int_ptr = (int *) buf_ptr;
  for (i = 0; i < dims[TST_X].length; i++) {
    for (j = 0; j < dims[TST_Y].length; j++) {
      for (k = 0; k < dims[TST_Z].length; k++) {
	int tmp = (i * 10000) + (j * 100) + k;
	if (*int_ptr != tmp) {
	  fprintf(stderr, "1. Data error at (%d,%d,%d)\n", i,j,k);
	  errors++;
	}
	int_ptr++;
      }
    }
  }
  free(buf_ptr);
  return (0);
}


int
test4(struct testinfo *ip, struct dimdef *dims, int ndims)
{
  /* Get the same variable again, but this time use an ICV to scale it.
   */
  size_t total;
  long coords[3];
  long lengths[3];
  double range[2];

  void *buf_ptr;
  float *flt_ptr;
  int i, j, k;
  int stat;
  int icv;
  double dbl;

  total = 1;
  for (i = 0; i < ndims; i++) {
    total *= dims[i].length;
  }

  buf_ptr = malloc(total * sizeof (float));

  if (buf_ptr == NULL) {
    fprintf(stderr, "Oops, malloc failed\n");
    return (-1);
  }

  coords[TST_X] = 0;
  coords[TST_Y] = 0;
  coords[TST_Z] = 0;
  lengths[TST_X] = dims[TST_X].length;
  lengths[TST_Y] = dims[TST_Y].length;
  lengths[TST_Z] = dims[TST_Z].length;

  icv = miicv_create();
  if (icv < 0) {
    FUNC_ERROR("miicv_create");
  }

  stat = miicv_setint(icv, MI_ICV_TYPE, NC_FLOAT);
  if (stat < 0) {
    FUNC_ERROR("miicv_setint");
  }

  stat = miicv_setint(icv, MI_ICV_DO_NORM, 1);
  if (stat < 0) {
    FUNC_ERROR("miicv_setint");
  }

  stat = miicv_attach(icv, ip->fd, ip->imgid);
  if (stat < 0) {
    FUNC_ERROR("miicv_attach");
  }

  /* This next call _should_ fail, since the ICV has been attached.
   */
  stat = miicv_setint(icv, MI_ICV_DO_NORM, 0);
  if (stat >= 0) {
    FUNC_ERROR("miicv_setint");
  }

  stat = miicv_inqdbl(icv, MI_ICV_DO_NORM, &dbl);
  if (stat < 0) {
    FUNC_ERROR("miicv_inqdbl");
  }

  if (dbl != 1.0) {
    fprintf(stderr, "miicv_inqdbl: Bad value returned\n");
    errors++;
  }

  stat = miicv_get(icv, coords, lengths, buf_ptr);
  if (stat < 0) {
    FUNC_ERROR("miicv_get");
  }

  stat = miget_image_range(ip->fd, range);
  if (stat < 0) {
    FUNC_ERROR("miget_image_range");
  }

  if (range[0] != -(XSIZE * 100000.0) || range[1] != (XSIZE * 100000.00)) {
    fprintf(stderr, "miget_image_range: bad result\n");
    errors++;
  }

  stat = miget_valid_range(ip->fd, ip->imgid, range);
  if (stat < 0) {
    FUNC_ERROR("miget_valid_range");
  }

  if (range[0] != -(XSIZE * 10000.0) || range[1] != (XSIZE * 10000.0)) {
    fprintf(stderr, "miget_valid_range: bad result\n");
    errors++;
  }

  flt_ptr = (float *) buf_ptr;
  for (i = 0; i < dims[TST_X].length; i++) {
    for (j = 0; j < dims[TST_Y].length; j++) {
      for (k = 0; k < dims[TST_Z].length; k++) {
	float tmp = (i * 10000) + (j * 100) + k;
	if (*flt_ptr != (float) tmp * 10.0) {
	  fprintf(stderr, "2. Data error at (%d,%d,%d) %f != %f\n", 
		  i,j,k, *flt_ptr, tmp);
	  errors++;
	}
	flt_ptr++;
      }
    }
  }

  stat = miicv_detach(icv);
  if (stat < 0) {
    FUNC_ERROR("miicv_detach");
  }

  /* Try it again, to make certain we fail gracefully. 
   */
  stat = miicv_detach(icv);
  if (stat < 0) {
    FUNC_ERROR("miicv_detach");
  }

  /* Try to detach a completely random number.
   */
  stat = miicv_detach(rand());
  if (stat >= 0) {
    FUNC_ERROR("miicv_detach");
  }

  stat = miicv_free(icv);
  if (stat < 0) {
    FUNC_ERROR("miicv_free");
  }

  free(buf_ptr);

  return (0);
}


int
test5(struct testinfo *ip, struct dimdef *dims, int ndims)
{
  /* Get the same variable again, but this time use an ICV to scale it.
   */
  size_t total;
  long coords[3];
  long lengths[3];

  void *buf_ptr;
  int *int_ptr;
  int i, j, k;
  int stat;
  int icv;

  total = 1;
  total *= dims[TST_X].length;
  total *= dims[TST_Y].length + YBOOST;
  total *= dims[TST_Z].length + ZBOOST;

  buf_ptr = malloc(total * sizeof (int));

  if (buf_ptr == NULL) {
    fprintf(stderr, "Oops, malloc failed\n");
    return (-1);
  }

  icv = miicv_create();
  if (icv < 0) {
    FUNC_ERROR("miicv_create");
  }

  /* Now set up a dimension conversion. */
  stat = miicv_setint(icv, MI_ICV_DO_NORM, 0);
  if (stat < 0) {
    FUNC_ERROR("miicv_setint");
  }

  stat = miicv_setint(icv, MI_ICV_DO_RANGE, 0);
  if (stat < 0) {
    FUNC_ERROR("miicv_setint");
  }

  stat = miicv_setint(icv, MI_ICV_TYPE, NC_INT);
  if (stat < 0) {
    FUNC_ERROR("miicv_setint");
  }

  stat = miicv_setint(icv, MI_ICV_YDIM_DIR, MI_ICV_NEGATIVE);
  if (stat < 0) {
    FUNC_ERROR("miicv_setint");
  }

  stat = miicv_setint(icv, MI_ICV_ZDIM_DIR, MI_ICV_NEGATIVE);
  if (stat < 0) {
    FUNC_ERROR("miicv_setint");
  }

  stat = miicv_setint(icv, MI_ICV_BDIM_SIZE, dims[TST_Y].length + YBOOST);
  if (stat < 0) {
    FUNC_ERROR("miicv_setint");
  }

  stat = miicv_setint(icv, MI_ICV_ADIM_SIZE, dims[TST_Z].length + ZBOOST);
  if (stat < 0) {
    FUNC_ERROR("miicv_setint");
  }

  stat = miicv_setint(icv, MI_ICV_DO_DIM_CONV, 1);
  if (stat < 0) {
    FUNC_ERROR("miicv_setint");
  }

  stat = miicv_attach(icv, ip->fd, ip->imgid);
  if (stat < 0) {
    FUNC_ERROR("miicv_attach");
  }

  coords[TST_X] = 0;
  coords[TST_Y] = 0;
  coords[TST_Z] = 0;
  lengths[TST_X] = dims[TST_X].length;
  lengths[TST_Y] = dims[TST_Y].length + YBOOST;
  lengths[TST_Z] = dims[TST_Z].length + ZBOOST;

  stat = miicv_get(icv, coords, lengths, buf_ptr);
  if (stat < 0) {
    FUNC_ERROR("miicv_get");
  }

  int_ptr = (int *) buf_ptr;
  for (i = 0; i < dims[TST_X].length; i++) {
    for (j = 0; j < dims[TST_Y].length + YBOOST; j++) {
      for (k = 0; k < dims[TST_Z].length + ZBOOST; k++, int_ptr++) {
	int x;
	int y;
	int z;
	int tmp;

	if (j < YBOOST/2 || j >= dims[TST_Y].length + YBOOST/2) 
	  continue;

	if (k < ZBOOST/2 || k >= dims[TST_Z].length + ZBOOST/2) 
	  continue;

	x = i;
	y = (YSIZE + YBOOST - 1) - j;
	z = (ZSIZE + ZBOOST - 1) - k;

	y -= YBOOST / 2;
	z -= ZBOOST / 2;

	tmp = (x * 10000) + (y * 100) + (z);

	if (*int_ptr != (int) tmp) {
	  fprintf(stderr, "3. Data error at (%d,%d,%d) %d != %d\n", 
		  i,j,k, *int_ptr, tmp);
	  errors++;
	}
      }
    }
  }

  stat = miicv_detach(icv);
  if (stat < 0) {
    FUNC_ERROR("miicv_detach");
  }

  stat = miicv_free(icv);
  if (stat < 0) {
    FUNC_ERROR("miicv_free");
  }

  free(buf_ptr);

  return (0);
}

int
test6(struct testinfo *ip, struct dimdef *dims, int ndims)
{
  size_t total;
  long coords[3];
  long lengths[3];

  void *buf_ptr;
  int *int_ptr;
  int i, j, k;
  int stat;
  int icv;

  total = 1;
  total *= dims[TST_X].length;
  total *= dims[TST_Y].length - YBOOST;
  total *= dims[TST_Z].length - ZBOOST;

  buf_ptr = malloc(total * sizeof (int));

  if (buf_ptr == NULL) {
    fprintf(stderr, "Oops, malloc failed\n");
    return (-1);
  }

  icv = miicv_create();
  if (icv < 0) {
    FUNC_ERROR("miicv_create");
  }
  /* Now try reading the image with reduced size.
   */

  stat = miicv_setint(icv, MI_ICV_BDIM_SIZE, dims[TST_Y].length - YBOOST);
  if (stat < 0) {
    FUNC_ERROR("miicv_setint");
  }

  stat = miicv_setint(icv, MI_ICV_ADIM_SIZE, dims[TST_Z].length - ZBOOST);
  if (stat < 0) {
    FUNC_ERROR("miicv_setint");
  }

  stat = miicv_setint(icv, MI_ICV_DO_DIM_CONV, 1);
  if (stat < 0) {
    FUNC_ERROR("miicv_setint");
  }

  stat = miicv_attach(icv, ip->fd, ip->imgid);
  if (stat < 0) {
    FUNC_ERROR("miicv_attach");
  }

  coords[TST_X] = 0;
  coords[TST_Y] = 0;
  coords[TST_Z] = 0;
  lengths[TST_X] = dims[TST_X].length;
  lengths[TST_Y] = dims[TST_Y].length - YBOOST;
  lengths[TST_Z] = dims[TST_Z].length - ZBOOST;

  stat = miicv_get(icv, coords, lengths, buf_ptr);
  if (stat < 0) {
    FUNC_ERROR("miicv_get");
  }

  int_ptr = (int *) buf_ptr;
  for (i = 0; i < dims[TST_X].length; i++) {
    for (j = 0; j < dims[TST_Y].length - YBOOST; j++) {
      for (k = 0; k < dims[TST_Z].length - ZBOOST; k++, int_ptr++) {
	int tmp;

	tmp = (i * 10000) + (j * 100) + (k);

	if (*int_ptr != (int) tmp) {
	  fprintf(stderr, "4. Data error at (%d,%d,%d) %d != %d\n", 
		  i,j,k, *int_ptr, tmp);
	  errors++;
	}
      }
    }
  }

  stat = miicv_detach(icv);
  if (stat < 0) {
    FUNC_ERROR("miicv_detach");
  }

  stat = miicv_free(icv);
  if (stat < 0) {
    FUNC_ERROR("miicv_free");
  }

  free(buf_ptr);

  return (0);
}

int
test7(struct testinfo *ip, struct dimdef *dims, int ndims)
{
  size_t total;
  long coords[3];
  long lengths[3];

  void *buf_ptr;
  int *int_ptr;
  int i, j, k;
  int stat;
  int icv;

  total = 1;
  for (i = 0; i < ndims; i++) {
    total *= dims[i].length;
  }

  buf_ptr = malloc(total * sizeof (float));

  if (buf_ptr == NULL) {
    fprintf(stderr, "Oops, malloc failed\n");
    return (-1);
  }

  icv = miicv_create();
  if (icv < 0) {
    FUNC_ERROR("miicv_create");
  }

  /* Test range conversion. */
  stat = miicv_setint(icv, MI_ICV_DO_DIM_CONV, 0);
  if (stat < 0) {
    FUNC_ERROR("miicv_setint");
  }
  stat = miicv_setint(icv, MI_ICV_DO_RANGE, 1);
  if (stat < 0) {
    FUNC_ERROR("miicv_setint");
  }
  stat = miicv_setstr(icv, MI_ICV_SIGN, MI_UNSIGNED);
  if (stat < 0) {
    FUNC_ERROR("miicv_setstr");
  }
  stat = miicv_setint(icv, MI_ICV_TYPE, NC_INT);
  if (stat < 0) {
    FUNC_ERROR("miicv_setint");
  }
  stat = miicv_setint(icv, MI_ICV_VALID_MAX, 1000);
  if (stat < 0) {
    FUNC_ERROR("miicv_setint");
  }
  stat = miicv_setint(icv, MI_ICV_VALID_MIN, -1000);
  if (stat < 0) {
    FUNC_ERROR("miicv_setint");
  }

  stat = miicv_attach(icv, ip->fd, ip->imgid);
  if (stat < 0) {
    FUNC_ERROR("miicv_attach");
  }

  coords[TST_X] = 0;
  coords[TST_Y] = 0;
  coords[TST_Z] = 0;
  lengths[TST_X] = dims[TST_X].length;
  lengths[TST_Y] = dims[TST_Y].length;
  lengths[TST_Z] = dims[TST_Z].length;

  stat = miicv_get(icv, coords, lengths, buf_ptr);
  if (stat < 0) {
    FUNC_ERROR("miicv_get");
  }

  int_ptr = (int *) buf_ptr;
  for (i = 0; i < dims[TST_X].length; i++) {
    for (j = 0; j < dims[TST_Y].length; j++) {
      for (k = 0; k < dims[TST_Z].length; k++, int_ptr++) {
	int tmp = (i * 10000) + (j * 100) + (k);
	int rng = (XSIZE * 10000) / 1000;
	/* Round tmp properly. */
	tmp = (tmp + (rng / 2)) / rng;
	if (*int_ptr != tmp) {
	  fprintf(stderr, "5. Data error at (%d,%d,%d) %d != %d\n", 
		  i,j,k, *int_ptr, tmp);
	  errors++;
	}
      }
    }
  }

  stat = miicv_detach(icv);
  if (stat < 0) {
    FUNC_ERROR("miicv_detach");
  }

  /* Free the ICV */
  stat = miicv_free(icv);
  if (stat < 0) {
    FUNC_ERROR("miicv_free");
  }

  free(buf_ptr);
  return (0);
}

/* Test MINC API's 
 */
int
main(int argc, char **argv)
{
  int stat;
  struct testinfo info;

  milog_init("mincapi");      /* Disable error messages completely. */
  milog_set_verbosity(0);

  ncopts &= ~(NC_FATAL | NC_VERBOSE);

  test1(&info, dimtab1, 3);

  test2(&info, dimtab1, 3);

  test3(&info, dimtab1, 3);

  test4(&info, dimtab1, 3);

  test5(&info, dimtab1, 3);

  /* test6(&info, dimtab1, 3); */

  test7(&info, dimtab1, 3);

  stat = miicv_free(rand());
  if (stat >= 0) {
    FUNC_ERROR("miicv_free");
  }

  if (miclose(info.fd) != MI_NOERROR) {
    FUNC_ERROR("miclose");
  }

  if (miclose(info.fd) != MI_ERROR) {
    FUNC_ERROR("miclose");
  }

  if (miclose(rand()) != MI_ERROR) {
    FUNC_ERROR("miclose");
  }

  unlink(info.name);		/* Delete the temporary file. */

  free(info.name);		/* Free the temporary filename */

  icv_tests();

  fprintf(stderr, "**** Tests completed with ");
  if (errors == 0) {
      fprintf(stderr, "no errors\n");
  }
  else {
      fprintf(stderr, "%ld error%s\n", errors, (errors == 1) ? "" : "s");
  }
  return (errors);
}


void icv_tests(void)
{
  /* Some random ICV tests */
  int icv;
  int stat;
  int i;
  double min, max;

  icv = miicv_create();
  if (icv < 0) {
    FUNC_ERROR("miicv_create");
  }

  for (i = NC_BYTE; i <= NC_DOUBLE; i++) {
    stat = miicv_setint(icv, MI_ICV_TYPE, i);
    if (stat < 0) {
      FUNC_ERROR("miicv_setint");
    }

    stat = miicv_setstr(icv, MI_ICV_SIGN, MI_UNSIGNED);

    stat = miicv_inqdbl(icv, MI_ICV_VALID_MAX, &max);
    if (stat < 0) {
      FUNC_ERROR("miicv_inqdbl");
    }

    stat = miicv_inqdbl(icv, MI_ICV_VALID_MIN, &min);
    if (stat < 0) {
      FUNC_ERROR("miicv_inqdbl");
    }

    switch (i) {
    case NC_BYTE:
      if (min != 0 || max != UCHAR_MAX) {
	fprintf(stderr, "Type %d min %g max %g\n", i, min, max);
	errors++;
      }
      break;
    case NC_SHORT:
      if (min != 0 || max != USHRT_MAX) {
	fprintf(stderr, "Type %d min %g max %g\n", i, min, max);
	errors++;
      }
      break;
    case NC_INT:
      if (min != 0 || max != UINT_MAX) {
	fprintf(stderr, "Type %d min %g max %g\n", i, min, max);
	errors++;
      }
      break;
    case NC_FLOAT:
      if (min != -FLT_MAX || max != FLT_MAX) {
	fprintf(stderr, "Type %d min %g max %g, header min %g max %g\n",
		i, min, max, FLT_MIN, FLT_MAX);
	errors++;
      }
      break;
    case NC_DOUBLE:
      if (min != -DBL_MAX || max != DBL_MAX) {
	fprintf(stderr, "Type %d min %g max %g, header min %g max %g\n",
		i, min, max, DBL_MIN, DBL_MAX);
	errors++;
      }
      break;
    }

    stat = miicv_setstr(icv, MI_ICV_SIGN, MI_SIGNED);
    if (stat < 0) {
      FUNC_ERROR("miicv_setstr");
    }

    stat = miicv_inqdbl(icv, MI_ICV_VALID_MAX, &max);
    if (stat < 0) {
      FUNC_ERROR("miicv_inqdbl");
    }

    stat = miicv_inqdbl(icv, MI_ICV_VALID_MIN, &min);
    if (stat < 0) {
      FUNC_ERROR("miicv_inqdbl");
    }

    switch (i) {
    case NC_BYTE:
      if (min != SCHAR_MIN || max != SCHAR_MAX) {
	fprintf(stderr, "Type %d min %g max %g\n", i, min, max);
	errors++;
      }
      break;
    case NC_SHORT:
      if (min != SHRT_MIN || max != SHRT_MAX) {
	fprintf(stderr, "Type %d min %g max %g\n", i, min, max);
	errors++;
      }
      break;
    case NC_INT:
      if (min != INT_MIN || max != INT_MAX) {
	fprintf(stderr, "Type %d min %g max %g\n", i, min, max);
	errors++;
      }
      break;
    case NC_FLOAT:
      if (min != -FLT_MAX || max != FLT_MAX) {
	fprintf(stderr, "Type %d min %g max %g\n", i, min, max);
	errors++;
      }
      break;
    case NC_DOUBLE:
      if (min != -DBL_MAX || max != DBL_MAX) {
	fprintf(stderr, "Type %d min %g max %g\n", i, min, max);
	errors++;
      }
      break;
    }
  }

#if 0
  /* For some reason we're allowed to set MI_ICV_TYPE to an illegal value.
   */
  stat = miicv_setint(icv, MI_ICV_TYPE, 1000);
  if (stat < 0) {
    FUNC_ERROR("miicv_setint");
  }
#endif

  stat = miicv_setint(icv, MI_ICV_NUM_IMGDIMS, MI_MAX_IMGDIMS + 1);
  if (stat >= 0) {
    FUNC_ERROR("miicv_setint");
  }
  
  stat = miicv_setint(icv, MI_ICV_NUM_IMGDIMS, MI_MAX_IMGDIMS);
  if (stat < 0) {
    FUNC_ERROR("miicv_setint");
  }

  miicv_free(icv);
}
