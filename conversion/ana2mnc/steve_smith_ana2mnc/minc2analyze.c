/* {{{ Copyright etc. */

/**********************************************\

  minc2analyze 0.4 by Stephen Smith

  Copyright 1997-1998 Stephen Smith 

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
  minctoraw (setup the macro MINCTORAW below)
  mincinfo  (setup the macro MINCINFO below)

  Compile with:
  cc -o minc2analyze minc2analyze.c

  History:
  0.4 January 20 1999 fixed L-R error
  0.3 September 28 1998 altered reversing and added reordering
  0.2 July 17 1998 added 4D functionality and more data types
  0.1 June 1 1998

\**********************************************/

/* }}} */

#define MINCTORAW "/usr/local/mni/bin/minctoraw"
#define MINCINFO  "/usr/local/mni/bin/mincinfo"

/* {{{ includes and defines */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/file.h>
#include <malloc.h>
#include "dbh.h"

typedef unsigned char uchar;

#define FTOI(a)    ( (a) < 0   ? ((int)((a)-0.5)) : ((int)((a)+0.5)) )

/* }}} */
/* {{{ num_test */

float num_test(sinput,fdefault)
  char  *sinput;
  float fdefault;
{
float finput=atof(sinput);

  if ( (finite(finput)==1) && (strcmp(sinput,"unknown")!=0) )
  {
    /*printf("string=%s atof=%f default=%f return=%f\n",sinput,finput,fdefault,finput);*/
    return(finput);
  }
  else
  {
    /*printf("string=%s atof=%f default=%f return=%f\n",sinput,finput,fdefault,fdefault);*/
    return(fdefault);
  }
}

/* }}} */
/* {{{ usage */

void usage()
{
  printf("Usage: minc2analyze <minc_input_root> [-z]\n");
  printf("E.g.: if input file is image.mnc, use \"minc2analyze image\"\n");
  printf("By default, co-ordinates and voxel dimensions will be the same after conversion;\nalternatively, -z zeros origin offset and makes all voxel dimensions positive\n");

  exit(1);
}

/* }}} */

int main(argc,argv)
     int argc;
     char *argv[];
{
  /* {{{ vars */

uchar  *buffera, *bufferb;
char   string_a[1000], string_array_a[100][1000], type_string[1000], *tcp;
short  short_array_a[1000], x_dim_short, y_dim_short, z_dim_short;
int    x,y,z, i, ko=1, datatype, volumes=1, bitpix, bytepix, argindex, order[3], glmin, glmax, pos=0;
static int DataTypeSizes[32] = {0,1,8,0,16,0,0,0,32,0,0,0,0,0,0,0,32};
float  x_pix_dim_float, y_pix_dim_float, z_pix_dim_float, x_origin_float, y_origin_float, z_origin_float;
struct dsr hdr;
FILE   *fd, *fd2;

/* }}} */

  /* {{{ usage */

if (argc==1)
     usage();

argindex=2;
     
while (argindex < argc)
{
  tcp = argv[argindex];
  if (*tcp == '-')
    switch (*++tcp)
      {
      case 'z': /* zero origin offset */
	ko=0;
	break;
      case 'p': /* make voxel dimensions positive */
	pos=1;
	break;
      default:
	usage();
      }
  else
    usage();
  argindex++;
}

/* }}} */
  /* {{{ read file type */

  sprintf(string_a,"%s %s.mnc | tail +2 | head -1 > %s.tmp",MINCINFO,argv[1],argv[1]);
  system(string_a);
  sprintf(string_a,"%s.tmp",argv[1]);
  fd2=fopen(string_a,"r");
  fscanf(fd2,"%s %s %s",string_a,(char *)&string_array_a[0],(char *)&string_array_a[1]);
  fclose(fd2);

  datatype=DT_SIGNED_SHORT;
  sprintf(type_string," ");
  if (strncmp(string_array_a[1],"byt",3)==0)
  {
    if (strncmp(string_array_a[0],"sig",3)==0) { datatype=DT_SIGNED_SHORT;  sprintf(type_string,"-short -signed ");}
    else                                       { datatype=DT_UNSIGNED_CHAR; sprintf(type_string,"-byte ");}
  }
  if (strncmp(string_array_a[1],"sho",3)==0)
  {
    if (strncmp(string_array_a[0],"sig",3)!=0) { datatype=DT_SIGNED_INT;    sprintf(type_string,"-long -signed ");}
    else                                       { datatype=DT_SIGNED_SHORT;  sprintf(type_string,"-short -signed ");}
  }
  if (strncmp(string_array_a[1],"lon",3)==0)
  {
    if (strncmp(string_array_a[0],"sig",3)==0) { datatype=DT_SIGNED_INT;    sprintf(type_string,"-long -signed ");}
    else                                       { datatype=DT_FLOAT;         sprintf(type_string,"float ");}
  }
  if (strncmp(string_array_a[1],"flo",3)==0)   { datatype=DT_FLOAT;         sprintf(type_string,"-float "); }

  printf("datatype=%d\n",datatype);
  bitpix=DataTypeSizes[datatype];
  bytepix=bitpix/8;

/* }}} */
  /* {{{ read x,y,z,t dimensions and image range etc. */

  sprintf(string_a,"%s %s.mnc | grep image: > %s.tmp",MINCINFO,argv[1],argv[1]);
  system(string_a);
  sprintf(string_a,"%s.tmp",argv[1]);
  fd2=fopen(string_a,"r");
  fscanf(fd2,"%s %s %s %s %s %s",(char *)&string_array_a[0],(char *)&string_array_a[1],(char *)&string_array_a[2],
	                         (char *)&string_array_a[3],(char *)&string_array_a[4],(char *)&string_array_a[5]);
  if (strcmp("to",string_array_a[4])==0)
     i=4;
  else
     i=3;
  glmin=atoi(string_array_a[i-1]);
  glmax=atoi(string_array_a[i+1]);
  printf("Image range = %d to %d\n",glmin,glmax);

  fscanf(fd2,"%s %s %s %s",string_a,(char *)&string_array_a[0],(char *)&string_array_a[1],(char *)&string_array_a[2]);
  z_dim_short=num_test(string_array_a[0],(float)256);
  z_pix_dim_float=num_test(string_array_a[1],(float)1);
  z_origin_float=-((float)num_test(string_array_a[2],(float)(z_dim_short/2)))/fabs(z_pix_dim_float);
  fclose(fd2);

  sprintf(string_a,"%s %s.mnc | tail -3 | grep zspace > %s.tmp",MINCINFO,argv[1],argv[1]);
  system(string_a);
  sprintf(string_a,"%s.tmp",argv[1]);
  fd2=fopen(string_a,"r");
  fscanf(fd2,"%s %s %s %s",string_a,(char *)&string_array_a[0],(char *)&string_array_a[1],(char *)&string_array_a[2]);
  z_dim_short=num_test(string_array_a[0],(float)256);
  z_pix_dim_float=num_test(string_array_a[1],(float)1);
  z_origin_float=-((float)num_test(string_array_a[2],(float)(z_dim_short/2)))/fabs(z_pix_dim_float);
  fclose(fd2);

  sprintf(string_a,"%s %s.mnc | tail -3 | grep yspace > %s.tmp",MINCINFO,argv[1],argv[1]);
  system(string_a);
  sprintf(string_a,"%s.tmp",argv[1]);
  fd2=fopen(string_a,"r");
  fscanf(fd2,"%s %s %s %s",string_a,(char *)&string_array_a[0],(char *)&string_array_a[1],(char *)&string_array_a[2]);
  y_dim_short=num_test(string_array_a[0],(float)256);
  y_pix_dim_float=num_test(string_array_a[1],(float)1);
  y_origin_float=-((float)num_test(string_array_a[2],(float)(y_dim_short/2)))/fabs(y_pix_dim_float);
  fclose(fd2);

  sprintf(string_a,"%s %s.mnc | tail -3 | grep xspace > %s.tmp",MINCINFO,argv[1],argv[1]);
  system(string_a);
  sprintf(string_a,"%s.tmp",argv[1]);
  fd2=fopen(string_a,"r");
  fscanf(fd2,"%s %s %s %s",string_a,(char *)&string_array_a[0],(char *)&string_array_a[1],(char *)&string_array_a[2]);
  x_dim_short=num_test(string_array_a[0],(float)256);
  x_pix_dim_float=num_test(string_array_a[1],(float)1);
  x_origin_float=-((float)num_test(string_array_a[2],(float)(x_dim_short/2)))/fabs(x_pix_dim_float);
  fclose(fd2);

  sprintf(string_a,"%s %s.mnc | grep dimensions | grep time > %s.tmp",MINCINFO,argv[1],argv[1]);
  system(string_a);
  sprintf(string_a,"%s.tmp",argv[1]);
  fd2=fopen(string_a,"r");
  fscanf(fd2,"%s %s %s",string_a,(char *)&string_array_a[0],(char *)&string_array_a[1]);
  if (strncmp(string_array_a[1],"time",4)==0)
  {
    sprintf(string_a,"%s %s.mnc | tail -4 | grep time > %s.tmp",MINCINFO,argv[1],argv[1]);
    system(string_a);
    sprintf(string_a,"%s.tmp",argv[1]);
    fd2=fopen(string_a,"r");
    fscanf(fd2,"%s %s %s %s",string_a,(char *)&string_array_a[0],(char *)&string_array_a[1],(char *)&string_array_a[2]);
    volumes=num_test(string_array_a[0],(float)1);
    fclose(fd2);
    /*printf("4D file of %d volumes\n",volumes);*/
  }

  sprintf(string_a,"/bin/rm %s.tmp",argv[1]);
  system(string_a);

/* }}} */
  /* {{{ create raw image */

  sprintf(string_a,"%s -normalize %s %s.mnc > %s.img",MINCTORAW,type_string,argv[1],argv[1]);
  printf("%s\n",string_a);
  system(string_a);

/* }}} */
  /* {{{ input image */

buffera = (uchar*)malloc(volumes*x_dim_short*y_dim_short*z_dim_short*bytepix);
sprintf(string_a,"%s.img",argv[1]);
fd=fopen(string_a,"r");
fread(buffera,bytepix,volumes*x_dim_short*y_dim_short*z_dim_short,fd);
fclose(fd);

bufferb = (uchar*)malloc(volumes*x_dim_short*y_dim_short*z_dim_short*bytepix);

/* }}} */
  /* {{{ reorder x,y,z ? */

/* {{{ read x,y,z,t ordering */

sprintf(string_a,"%s %s.mnc | tail -3 > %s.tmp",MINCINFO,argv[1],argv[1]);
system(string_a);
sprintf(string_a,"%s.tmp",argv[1]);
fd2=fopen(string_a,"r");
fscanf(fd2,"%s %s %s %s",(char *)&string_array_a[2],string_a,string_a,string_a);
fscanf(fd2,"%s %s %s %s",(char *)&string_array_a[1],string_a,string_a,string_a);
fscanf(fd2,"%s %s %s %s",(char *)&string_array_a[0],string_a,string_a,string_a);
fclose(fd2);

for(i=0; i<3; i++)
{
  if (strncmp(string_array_a[i],"z",1)==0)
    order[i]=2;
  else
    {
      if (strncmp(string_array_a[i],"y",1)==0)
	order[i]=1;
      else
	order[i]=0;
    }
}
printf("Order: %s (%d) %s (%d) %s (%d)\n",string_array_a[0],order[0],string_array_a[1],order[1],string_array_a[2],order[2]);

sprintf(string_a,"/bin/rm %s.tmp",argv[1]);
system(string_a);

/* }}} */

if ( (order[0]!=0) || (order[1]!=1) || (order[2]!=2) )
{
  int counters[3];
  short dims[3];

  dims[0]=x_dim_short;
  dims[1]=y_dim_short;
  dims[2]=z_dim_short;

  printf("Reordering dimensions.\n");

  for(i=0; i<volumes; i++)
    for(counters[2]=0; counters[2]<z_dim_short; counters[2]++)
      for(counters[1]=0; counters[1]<y_dim_short; counters[1]++)
	for(counters[0]=0; counters[0]<x_dim_short; counters[0]++)
	  memcpy(bufferb+( ( i*z_dim_short*y_dim_short*x_dim_short +
			       counters[2]*y_dim_short*x_dim_short +
			       counters[1]*x_dim_short +
			       counters[0]) * bytepix ),
		 buffera +( ( i*z_dim_short*y_dim_short*x_dim_short +
			       counters[order[2]]*dims[order[1]]*dims[order[0]] +
			       counters[order[1]]*dims[order[0]] +
			       counters[order[0]]) * bytepix ),
		 bytepix);
  
  memcpy(buffera,bufferb,bytepix*volumes*x_dim_short*y_dim_short*z_dim_short);
}

/* }}} */
  /* {{{ COMMENT reverse x ? ORIG */

#ifdef FoldingComment

if (x_pix_dim_float<0)
{
  printf("Reversing x direction.\n");

  x_origin_float += ((float)x_dim_short);
  x_pix_dim_float = -x_pix_dim_float;

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
  
  memcpy(buffera,bufferb,bytepix*volumes*x_dim_short*y_dim_short*z_dim_short);
}

#endif

/* }}} */
  /* {{{ reverse x ? */

if (x_pix_dim_float>0) /* this is necessary because of the MINC<->MEDx L<->R inconsistency */
{
  printf("Reversing x direction (data).\n");

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
  
  memcpy(buffera,bufferb,bytepix*volumes*x_dim_short*y_dim_short*z_dim_short);
}
else
{
  printf("Reversing x direction (header).\n");

  x_origin_float += ((float)x_dim_short);
  x_pix_dim_float = -x_pix_dim_float;

  /* this used to have data reversal in here but moved it up as in this case we'd want to reverse twice */
}

if (ko) /* this is necessary because of the MINC<->MEDx L<->R inconsistency */
  x_pix_dim_float = -x_pix_dim_float;

/* }}} */
  /* {{{ reverse y ? */

if (y_pix_dim_float<0)
{
  printf("Reversing y direction.\n");

  y_origin_float += ((float)y_dim_short);
  y_pix_dim_float = -y_pix_dim_float;

  for(i=0; i<volumes; i++)
    for(z=0; z<z_dim_short; z++)
      for(y=0; y<y_dim_short; y++)
	memcpy(bufferb+( ( i*z_dim_short*y_dim_short*x_dim_short +
			     z*y_dim_short*x_dim_short +
			     y*x_dim_short ) * bytepix ),
	       buffera +( ( i*z_dim_short*y_dim_short*x_dim_short +
			     z*y_dim_short*x_dim_short + 
			     (y_dim_short-1-y)*x_dim_short ) * bytepix ),
	       bytepix*x_dim_short);

  memcpy(buffera,bufferb,bytepix*volumes*x_dim_short*y_dim_short*z_dim_short);
}

/* }}} */
  /* {{{ reverse z ? */

if (z_pix_dim_float<0)
{
  printf("Reversing z direction.\n");

  z_origin_float += ((float)z_dim_short);
  z_pix_dim_float = -z_pix_dim_float;

  for(i=0; i<volumes; i++)
    for(z=0; z<z_dim_short; z++)
	memcpy(bufferb+( ( i*z_dim_short*y_dim_short*x_dim_short +
			     z*y_dim_short*x_dim_short ) * bytepix ),
	       buffera +( ( i*z_dim_short*y_dim_short*x_dim_short +
			     (z_dim_short-1-z)*y_dim_short*x_dim_short ) * bytepix ),
	       bytepix*x_dim_short*y_dim_short);

  memcpy(buffera,bufferb,bytepix*volumes*x_dim_short*y_dim_short*z_dim_short);
}

/* }}} */
  /* {{{ output image */

sprintf(string_a,"%s.img",argv[1]);
fd=fopen(string_a,"w");
fwrite(buffera,bytepix,volumes*x_dim_short*y_dim_short*z_dim_short,fd);
fclose(fd);

/* }}} */
  /* {{{ open output (header) file */

  sprintf(string_a,"%s.hdr",argv[1]);

  if((fd=fopen(string_a,"w"))==0)
    {
        printf("unable to create: %s\n",argv[1]);
        exit(0);
    }

/* }}} */
  /* {{{ create header struc */

    hdr.hk.sizeof_hdr = sizeof(struct dsr);

    sprintf(hdr.hk.data_type,"dsr      ");
    hdr.hk.data_type[9]=0;

    sprintf(string_a,"%s                               ",argv[1]);
    string_a[17]=0;
    strcpy(hdr.hk.db_name,string_a);

    hdr.hk.extents=0;

    hdr.hk.session_error=0;

    hdr.hk.regular = 'r';

    hdr.hk.hkey_un0 = '0';

    hdr.dime.dim[0] = 4;            /* all Analyze images are taken as 4 dimensional */
    hdr.dime.dim[1] = x_dim_short;  /* slice width  in pixels */
    hdr.dime.dim[2] = y_dim_short;  /* slice height in pixels */
    hdr.dime.dim[3] = z_dim_short;  /* volume depth in slices */
    hdr.dime.dim[4] = volumes;      /* number of volumes per file */
    hdr.dime.dim[5] = 0;
    hdr.dime.dim[6] = 0;
    hdr.dime.dim[7] = 0;

    /* up to 3 characters for the voxels units label; i.e. mm., um., cm. */
    strcpy(hdr.dime.vox_units,"mm");

    /*   up to 7 characters for the calibration units label; i.e. HU */
    strcpy(hdr.dime.cal_units," ");  

    hdr.dime.unused1=0;

    hdr.dime.datatype = datatype;

    hdr.dime.bitpix = bitpix;

    hdr.dime.dim_un0=0;

    /* 1,2,3 = pixel dimensions */
    hdr.dime.pixdim[0]=0;
    hdr.dime.pixdim[1]=x_pix_dim_float;
    hdr.dime.pixdim[2]=y_pix_dim_float;
    hdr.dime.pixdim[3]=z_pix_dim_float;
    hdr.dime.pixdim[4]=0;
    hdr.dime.pixdim[5]=0;
    hdr.dime.pixdim[6]=0;
    hdr.dime.pixdim[7]=0;

    hdr.dime.vox_offset = 0.0;

    hdr.dime.funused1    = 1.0; /* scaling */

    hdr.dime.funused2    = 0.0;

    hdr.dime.funused3    = 0.0;

    hdr.dime.cal_max     = 0.0;

    hdr.dime.cal_min     = 0.0;
    
    hdr.dime.compressed=0;

    hdr.dime.verified=0;

    hdr.dime.glmax  = glmax;
    hdr.dime.glmin  = glmin;
    
    /*   Planar Orientation;
         Movie flag OFF: 0 = transverse, 1 = coronal, 2 = sagittal
         Movie flag ON:  3 = transverse, 4 = coronal, 5 = sagittal  */  
    hdr.hist.orient     = 0;  

    /* origin */
    if (ko==1)
    {
      short_array_a[0]=(short)FTOI(x_origin_float);
      short_array_a[1]=(short)FTOI(y_origin_float);
      short_array_a[2]=(short)FTOI(z_origin_float);
      printf("origin = %d %d %d\n",short_array_a[0],short_array_a[1],short_array_a[2]);
    }
    else
    {
      short_array_a[0]=0;
      short_array_a[1]=0;
      short_array_a[2]=0;
    }
    short_array_a[3]=0;
    short_array_a[4]=0;
    memcpy(hdr.hist.originator,short_array_a,5*sizeof(short));

/* }}} */
  /* {{{ output header */

  fwrite(&hdr,sizeof(struct dsr),1,fd);
  fclose(fd);

/* }}} */
  return(0);
}
