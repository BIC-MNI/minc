/* {{{ Copyright etc. */

/**********************************************\

  analyze2minc 0.3 by Stephen Smith

  Copyright 1997-1999 Stephen Smith 

  Some elements are Copyright 1986-1995
  Biomedical Imaging Resource Mayo Foundation

  Oxford Centre for Functional Magnetic Resonance Imaging of the Brain,
  Department of Clinical Neurology, Oxford University, Oxford, UK
  Email:    steve@fmrib.ox.ac.uk
  WWW:      http://www.fmrib.ox.ac.uk/~steve

  This program should be considered a beta test version and must not
  be used for any clinical purposes.

  This code is written using an emacs folding mode, making moving
  around the different sections very easy. This is why there are
  various marks within comments and why comments are indented.

  Requires:
  rawtominc (setup the macro RAWTOMINC below)

  Compile with:
  cc -o analyze2minc analyze2minc.c

  History:
  0.3 January 20 1999 fixed L-R error
  0.2 July 17 1998 added 4D functionality
  0.1 June 1 1998

\**********************************************/

/* }}} */

#define RAWTOMINC "/usr/local/mni/bin/rawtominc"

/* {{{ includes and defines */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/file.h>
#include <malloc.h>
#include "dbh.h"
typedef unsigned char uchar;

/* }}} */
/* {{{ routines from avw_read */

/* {{{ swap_long */

void swap_long(pntr)
     unsigned char *pntr;
{
  unsigned char b0, b1, b2, b3;

  b0 = *pntr;
  b1 = *(pntr+1);
  b2 = *(pntr+2);
  b3 = *(pntr+3);
  
  *pntr = b3;
        *(pntr+1) = b2;
        *(pntr+2) = b1;
        *(pntr+3) = b0;
}

/* }}} */
/* {{{ swap_short */

void swap_short(pntr)
     unsigned char *pntr;
{
  unsigned char b0, b1;
  
  b0 = *pntr;
  b1 = *(pntr+1);
  
  *pntr = b1;
  *(pntr+1) = b0;
}

/* }}} */
/* {{{ swap_hdr */

void swap_hdr(pntr)
     struct dsr *pntr;
{
        swap_short(&pntr->hist.originator[0]) ;
        swap_short(&pntr->hist.originator[1]) ;
        swap_short(&pntr->hist.originator[2]) ;
        swap_short(&pntr->hist.originator[3]) ;
        swap_short(&pntr->hist.originator[4]) ;
  swap_long(&pntr->hk.sizeof_hdr) ;
  swap_long(&pntr->hk.extents) ;
  swap_short(&pntr->hk.session_error) ;
  swap_short(&pntr->dime.dim[0]) ;
  swap_short(&pntr->dime.dim[1]) ;
  swap_short(&pntr->dime.dim[2]) ;
  swap_short(&pntr->dime.dim[3]) ;
  swap_short(&pntr->dime.dim[4]) ;
  swap_short(&pntr->dime.dim[5]) ;
  swap_short(&pntr->dime.dim[6]) ;
  swap_short(&pntr->dime.dim[7]) ;
  swap_short(&pntr->dime.unused1) ;
  swap_short(&pntr->dime.datatype) ;
  swap_short(&pntr->dime.bitpix) ;
  swap_long(&pntr->dime.pixdim[0]) ;
  swap_long(&pntr->dime.pixdim[1]) ;
  swap_long(&pntr->dime.pixdim[2]) ;
  swap_long(&pntr->dime.pixdim[3]) ;
  swap_long(&pntr->dime.pixdim[4]) ;
  swap_long(&pntr->dime.pixdim[5]) ;
  swap_long(&pntr->dime.pixdim[6]) ;
  swap_long(&pntr->dime.pixdim[7]) ;
  swap_long(&pntr->dime.vox_offset) ;
  swap_long(&pntr->dime.funused1) ;
  swap_long(&pntr->dime.funused2) ;
  swap_long(&pntr->dime.cal_max) ;
  swap_long(&pntr->dime.cal_min) ;
  swap_long(&pntr->dime.compressed) ;
  swap_long(&pntr->dime.verified) ;
  swap_short(&pntr->dime.dim_un0) ;
  swap_long(&pntr->dime.glmax) ;
  swap_long(&pntr->dime.glmin) ;
}

/* }}} */
/* {{{ ShowHdr */

void ShowHdr(fileName,hdr)
struct dsr *hdr;
char *fileName;
{
int i;
char string[128];
printf("Analyze Header Dump of: <%s> \n", fileName);
/* Header Key */
printf("sizeof_hdr: <%d> \n", hdr->hk.sizeof_hdr);
printf("data_type:  <%s> \n", hdr->hk.data_type);
printf("db_name:    <%s> \n", hdr->hk.db_name);
printf("extents:    <%d> \n", hdr->hk.extents);
printf("session_error: <%d> \n", hdr->hk.session_error);
printf("regular:  <%c> \n", hdr->hk.regular);
printf("hkey_un0: <%c> \n", hdr->hk.hkey_un0);

/* Image Dimension */
for(i=0;i<8;i++)
	printf("dim[%d]: <%d> \n", i, hdr->dime.dim[i]);
	
	strncpy(string,hdr->dime.vox_units,4);
	printf("vox_units:  <%s> \n", string);
	
	strncpy(string,hdr->dime.cal_units,8);
	printf("cal_units: <%s> \n", string);
	printf("unused1:   <%d> \n", hdr->dime.unused1);
	printf("datatype:  <%d> \n", hdr->dime.datatype);
	printf("bitpix:    <%d> \n", hdr->dime.bitpix);
	
for(i=0;i<8;i++)
	printf("pixdim[%d]: <%6.4f> \n",i, hdr->dime.pixdim[i]);
	
printf("vox_offset: <%6.4> \n",  hdr->dime.vox_offset);
printf("funused1:   <%6.4f> \n", hdr->dime.funused1);
printf("funused2:   <%6.4f> \n", hdr->dime.funused2);
printf("funused3:   <%6.4f> \n", hdr->dime.funused3);
printf("cal_max:    <%6.4f> \n", hdr->dime.cal_max);
printf("cal_min:    <%6.4f> \n", hdr->dime.cal_min);
printf("compressed: <%d> \n", hdr->dime.compressed);
printf("verified:   <%d> \n", hdr->dime.verified);
printf("glmax:      <%d> \n", hdr->dime.glmax);
printf("glmin:      <%d> \n", hdr->dime.glmin);

/* Data History */
strncpy(string,hdr->hist.descrip,80);
printf("descrip:  <%s> \n", string);
strncpy(string,hdr->hist.aux_file,24);
printf("aux_file: <%s> \n", string);
printf("orient:   <%d> \n", hdr->hist.orient);

strncpy(string,hdr->hist.originator,10);
printf("originator: <%s> \n", string);

strncpy(string,hdr->hist.generated,10);
printf("generated: <%s> \n", string);


strncpy(string,hdr->hist.scannum,10);
printf("scannum: <%s> \n", string);

strncpy(string,hdr->hist.patient_id,10);
printf("patient_id: <%s> \n", string);

strncpy(string,hdr->hist.exp_date,10);
printf("exp_date: <%s> \n", string);

strncpy(string,hdr->hist.exp_time,10);
printf("exp_time: <%s> \n", string);

strncpy(string,hdr->hist.hist_un0,10);
printf("hist_un0: <%s> \n", string);

printf("views:      <%d> \n", hdr->hist.views);
printf("vols_added: <%d> \n", hdr->hist.vols_added);
printf("start_field:<%d> \n", hdr->hist.start_field);
printf("field_skip: <%d> \n", hdr->hist.field_skip);
printf("omax: <%d> \n", hdr->hist.omax);
printf("omin: <%d> \n", hdr->hist.omin);
printf("smin: <%d> \n", hdr->hist.smax);
printf("smin: <%d> \n", hdr->hist.smin);

}

/* }}} */

/* }}} */

main(argc,argv)
  int argc;
  char *argv[];
{
/* {{{ vars */

FILE *fd, *fd2;
struct dsr hdr;
int int_a, size, rev=0, volumes=1, swap=0, bytepix, i, x, y, z, revxdata=1;
double cmax, cmin;
short datatype, short_a, short_array_a[1000], x_dim_short, y_dim_short, z_dim_short;
uchar uchar_a, *buffera, *bufferb;
float float_a, float_array_a[1000], x_pix_dim_float, y_pix_dim_float, z_pix_dim_float, x_origin_float, y_origin_float, z_origin_float;
char data_file[1000], string_a[1000], string_a2[1000], string_array_a[100][1000];
static char type_string[100];
static int DataTypeSizes[32] = {0,1,8,0,16,0,0,0,32,0,0,0,0,0,0,0,32};

/* }}} */

  /* {{{ COMMENT usage */

#ifdef FoldingComment

  if (argc==1)
  {
    printf("Usage: analyze2minc <analyze_input_root> [-rev]\n");
    printf("E.g.: if input file is image.hdr and image.img, use \"analyze2minc image\"\n");
    printf("(-rev causes reversal in the x direction.)\n");
    exit(0);
  }

  if (argc==3)
    rev=1;

#endif

/* }}} */
  /* {{{ usage */

  if (argc==1)
  {
    printf("Usage: analyze2minc <analyze_input_root>\n");
    printf("E.g.: if input file is image.hdr and image.img, use \"analyze2minc image\"\n");
    exit(0);
  }

/* }}} */
  /* {{{ open input file and read header */

  sprintf(string_a,"%s.hdr",argv[1]);

  if((fd=fopen(string_a,"r"))==NULL)
  {
    fprintf(stderr,"Can't open:<%s>\n", argv[1]);
    exit(0);
  }

  fread(&hdr,1,sizeof(struct dsr),fd);
  fclose(fd);

  if(hdr.dime.dim[0] < 0 || hdr.dime.dim[0] > 15)
  {
    printf("Byte swapping\n");
    swap=1;
    swap_hdr(&hdr);
  }

/* }}} */
  /* {{{ read image_dimension */

x_dim_short=hdr.dime.dim[1];
y_dim_short=hdr.dime.dim[2];
z_dim_short=hdr.dime.dim[3];
volumes=hdr.dime.dim[4];

x_pix_dim_float=hdr.dime.pixdim[1];
if (x_pix_dim_float<0)
{
  x_pix_dim_float = -x_pix_dim_float;
  /*  revxdata=0;*/ /* don't need this */
}
y_pix_dim_float=hdr.dime.pixdim[2];
z_pix_dim_float=hdr.dime.pixdim[3];

datatype=hdr.dime.datatype;
sprintf(type_string,"");
if (datatype==DT_UNSIGNED_CHAR) sprintf(type_string,"-byte ");
if (datatype==DT_SIGNED_SHORT) sprintf(type_string,"-short -signed ");
if (datatype==DT_SIGNED_INT) sprintf(type_string,"-long -signed ");
if (datatype==DT_FLOAT) sprintf(type_string,"-float ");
if (datatype==DT_DOUBLE) sprintf(type_string,"-double ");

bytepix=DataTypeSizes[datatype]/8;

buffera = (uchar*)malloc(volumes*x_dim_short*y_dim_short*z_dim_short*bytepix);
bufferb = (uchar*)malloc(volumes*x_dim_short*y_dim_short*z_dim_short*bytepix);

/* }}} */
  /* {{{ read data_history */

/* origin */
memcpy(short_array_a,hdr.hist.originator,5*sizeof(short));
x_origin_float=((float)-short_array_a[0])*x_pix_dim_float;
y_origin_float=((float)-short_array_a[1])*y_pix_dim_float;
z_origin_float=((float)-short_array_a[2])*z_pix_dim_float;

/* }}} */
  /* {{{ debug prints */

  printf("%d %d %d\n",x_dim_short,y_dim_short,z_dim_short);
  printf("%f %f %f\n",x_pix_dim_float,y_pix_dim_float,z_pix_dim_float);
  printf("%f %f %f\n",x_origin_float,y_origin_float,z_origin_float);

/* }}} */
  /* {{{ reverse x */

if (revxdata)
{
  sprintf(string_a,"%s.img",argv[1]);
  fd=fopen(string_a,"r");
  fread(buffera,bytepix,volumes*x_dim_short*y_dim_short*z_dim_short,fd);
  fclose(fd);

  for(i=0; i<volumes; i++)
    for(z=0; z<z_dim_short; z++)
      for(y=0; y<y_dim_short; y++)
	for(x=0; x<x_dim_short; x++)
	  memcpy(bufferb+( ( i*z_dim_short*y_dim_short*x_dim_short +
			       z*y_dim_short*x_dim_short +
			       y*x_dim_short +
			       x) * bytepix ),
		 buffera +( ( i*z_dim_short*y_dim_short*x_dim_short +
			       z*y_dim_short*x_dim_short +
			       y*x_dim_short +
			       (x_dim_short-x-1) ) * bytepix ),
		 bytepix);
  
  sprintf(data_file,"%s.tmp.img",argv[1]);
  fd=fopen(data_file,"w");
  fwrite(bufferb,bytepix,volumes*x_dim_short*y_dim_short*z_dim_short,fd);
  fclose(fd);
}
else
  sprintf(data_file,"%s.img",argv[1]);

/* }}} */
  /* {{{ swap bytes on image files if necessary */

if ( swap )
{
  if((fd=fopen(data_file,"r"))==NULL)
  {
    fprintf(stderr,"Can't open:<%s>\n", argv[1]);
    exit(0);
  }

  sprintf(string_a,"%s.tmp.tmp.img",argv[1]);
  if((fd2=fopen(string_a,"w"))==NULL)
  {
    fprintf(stderr,"Can't open:<%s> for writing\n", argv[1]);
    exit(0);
  }

  for(i=0; i<z_dim_short*y_dim_short*x_dim_short; i++)
    {
      fread(buffera,bytepix,1,fd);

      if (bytepix==2)
	swap_short(buffera);

      if (bytepix==4)
	swap_long(buffera);

      fwrite(buffera,bytepix,1,fd2);
    }

  fclose(fd);
  fclose(fd2);

  sprintf(string_a,"/bin/mv %s.tmp.tmp.img %s.tmp.img",argv[1],argv[1]);
  system(string_a);
}

/* }}} */
  /* {{{ write minc file */

if ( rev == 1 )
{
  printf("Reversing x\n");
  x_origin_float = x_origin_float+(x_pix_dim_float*((float)x_dim_short));
  x_pix_dim_float = -x_pix_dim_float;
}

/* several MNI programs won't work if volumes is set to 1 rather than not set at all */
if ( volumes > 1 )
     sprintf(string_a2,"%d",volumes);
else
     sprintf(string_a2,"");

sprintf(string_a,
	"%s %s -transverse -scan_range -input %s -xstep %f -ystep %f -zstep %f -xstart %f -ystart %f -zstart %f %s.mnc %s %d %d %d",
	RAWTOMINC, type_string, data_file,
	x_pix_dim_float,y_pix_dim_float,z_pix_dim_float,
	x_origin_float,y_origin_float,z_origin_float,
	argv[1],
	string_a2,z_dim_short,y_dim_short,x_dim_short);

system(string_a);

/* }}} */
  /* {{{ remove temp data file */

if (revxdata)
{
  sprintf(string_a,"/bin/rm %s",data_file);
  system(string_a);
}

/* }}} */
}
