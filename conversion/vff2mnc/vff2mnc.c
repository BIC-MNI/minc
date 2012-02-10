/* ----------------------------- MNI Header -----------------------------------
@NAME       : vff2mnc.c
@DESCRIPTION: Program to convert vff file(s) to minc
@GLOBALS    : 
@CREATED    : Jul 2006 (Leila Baghdadi)
@MODIFIED   : 
 * $Log: vff2mnc.c,v $
 * Revision 1.7  2010-03-27 15:10:59  rotor
 *  * removed default volume_io caching
 *  * fixed a few warnings in vff2mnc (fgets)
 *
 * Revision 1.6  2008/09/23 02:18:08  alex
 * Fixed typo on line 141 which prevented minc to be built
 *
 * Revision 1.5  2008/09/04 16:15:01  baghdadi
 * corrected start value to world coordinate system
 *
 * Revision 1.4  2008/01/11 07:17:07  stever
 * Remove unused variables.
 *
 * Revision 1.3  2007/12/11 12:43:01  rotor
 *  * added static to all global variables in main programs to avoid linking
 *       problems with libraries (compress in mincconvert and libz for example)
 *
 * Revision 1.2  2007/01/19 17:52:56  baghdadi
 * Added minor revision for string manipulations.
 *
 * Revision 1.1  2007/01/16 18:37:57  baghdadi
 * To convert vff files produced by the CT scanner to MINC2.0
 *
 * Adopted from vfftominc (perl script) of John G. Sled.
---------------------------------------------------------------------------- */
#include "vff2mnc.h"

#include <sys/stat.h>

#if HAVE_DIRENT_H
#include <dirent.h>
#endif

#if defined (_MSC_VER)
#include <windows.h>
#endif

#include <time_stamp.h>
#include <ParseArgv.h>


/* Function Prototypes */
static void computeScalarRange(int datatype,double range[2],
			       long count,void *buffer);
static int usage(void);
static void free_list(int num_files, const char **file_list);
void read_2Dvff_files_header(const char **file_list, int num_files,
			     struct mnc_vars *m2, struct vff_attrs *vattrs);
void read_2Dvff_files_image(mihandle_t hvol, const char **file_list, 
			    int num_files, struct mnc_vars m2, 
			    struct vff_attrs vattrs,double range[2]);
void read_3Dvff_file_header(char *filename, struct mnc_vars *m2, 
			    struct vff_attrs *vattrs);
void read_3Dvff_file_image(mihandle_t hvol, char *filename, 
			   struct mnc_vars m2, struct vff_attrs vattrs, 
			   double range[2]);
int
add_vff_attribute_to_file(mihandle_t hvol,const struct vff_attrs *vattrs);
int
add_attributes_from_files(mihandle_t hvol);
int
find_filenames_first(string_t fullpath_pro,string_t fullpath_des,string_t fullpath_par,
		     int *found_protocol,int *found_description, int *found_parameters);
struct globals G;

#define VERSION_STRING "2.0.12 built " __DATE__ " " __TIME__

static ArgvInfo argTable[] = {
    {NULL, ARGV_VERINFO, VERSION_STRING, NULL, NULL },
    {"-noswap", ARGV_CONSTANT, (char *) FALSE, (char *) &G.little_endian,
     "Change to noswap default is swap"},
    {"-addattrs", ARGV_STRING, (char *) 1, (char *) &G.dirname,
     "Add attributes from files in the given directory"},
    {"-list", ARGV_CONSTANT, (char *) TRUE, (char *) &G.List,
     "Print list of series (don't create files)"},
    {NULL, ARGV_END, NULL, NULL, NULL}

};

int 
main(int argc, char *argv[])
{   
    int r;
    
    const char **file_list = NULL;     /* List of file names */
    struct vff_attrs vffattrs;
    struct mnc_vars mnc2;
    char out_str[1024];         /* Big string for filename */
    midimhandle_t hdim[MAX_VFF_DIMS];
    mihandle_t hvol;
   
    double mnc_vrange[2];       /* MINC valid min/max */
    int num_file_args;          /* Number of files on command line */
    string_t out_dir;           /* Output directory */
    int length;
    struct stat st;
    int ifile;
    int num_files;              /* Total number of files */
    int is_file=0;
    int is_list=0;
    int ival;
    char *extension;

    mnc2.mnc_srange[0]= -1;
    mnc2.mnc_srange[1]= -1;

    G.pname = argv[0];          /* get program name */
    G.dirname = NULL;
    G.little_endian = 1; /*default is little endian unless otherwise*/
    G.minc_history = time_stamp(argc, argv); /* Create minc history string */

    if (ParseArgv(&argc, argv, argTable, 0) || argc < 2) {
        usage();
	exit(EXIT_FAILURE);
    }
     
    if (G.dirname != NULL) {
#if HAVE_DIRENT_H
      
      if (stat(G.dirname, &st) != 0 || !S_ISDIR(st.st_mode)) {
        fprintf(stderr,"Option -addattrs requires directory as argument!!!\n");
        exit(EXIT_FAILURE);
      }
#endif 
    }

    if (G.List) 
      {
      num_file_args = argc - 1; 
      }
    else 
      {
	strcpy(out_str, argv[1]);
        extension = strrchr(out_str, '.');
        if (extension != NULL )
	  {
	    extension++;
            if (strcmp(extension, "mnc") !=0)
	      {
		usage();
		exit(EXIT_FAILURE);
	      }
	  }
	if (argc == 3)
	  {
	  /* check if last argument is dir */
	  num_file_args = argc - 2; 
	  strcpy(out_dir, argv[argc - 1]); 
	  
	  /* make sure path ends with slash 
	   */
	  length = strlen(out_dir);
	  if (out_dir[length - 1] != '/') 
	    {
	    out_dir[length++] = '/';
	    out_dir[length++] = '\0';
	    }

	  if (stat(out_dir, &st) != 0 || !S_ISDIR(st.st_mode)) 
	    {/* assume filename */
	    is_file =1;
	    }
	  }
	else
	  { //list of 2d files must check!
	  num_file_args = argc - 2;
	  is_list = 1;
	  }
      }
	    
    if (!is_file || G.List)
      {
	
      /* Get space for file lists */
      /* Allocate the array of pointers used to implement the
       * list of filenames.
       */
      file_list = malloc(1 * sizeof(char *));
      CHKMEM(file_list);

      /* Go through the list of files, expanding directories where they
       * are encountered...
       */
      num_files = 0;
      for (ifile = 1 ; ifile <= num_file_args; ifile++) 
	{
#if HAVE_DIRENT_H    
	
	if (stat(argv[ifile + 1], &st) == 0 && S_ISDIR(st.st_mode)) 
	  {
	  DIR *dp;
	  struct dirent *np;
	  char *tmp_str;

	  length = strlen(argv[ifile + 1]);

	  dp = opendir(argv[ifile + 1]);
	  if (dp != NULL) 
	    {
	    while ((np = readdir(dp)) != NULL) 
	      {
	      /* Generate the full path to the file.
	       */
	      tmp_str = malloc(length + strlen(np->d_name) + 2);
	      strcpy(tmp_str, argv[ifile + 1]);
	      if (tmp_str[length-1] != '/') 
	       {
	       tmp_str[length] = '/';
	       tmp_str[length+1] = '\0';
	       }
	      strcat(&tmp_str[length], np->d_name);
	      if (stat(tmp_str, &st) == 0 && S_ISREG(st.st_mode)) 
		{
		file_list = realloc(file_list,
				    (num_files + 1) * sizeof(char *));
		file_list[num_files++] = tmp_str;
		}
	      else 
		{
		free(tmp_str);
		}
	      }
	    closedir(dp);
	  }
	  else 
	    {
	      fprintf(stderr, "Error opening directory '%s'\n", 
		      argv[ifile + 1]);
	    }
	  }
	else 
	  {
	  file_list = realloc(file_list, (num_files + 1) * sizeof(char *));
	  file_list[num_files++] = strdup(argv[ifile + 1]);
	  }
#else
	file_list = realloc(file_list, (num_files + 1) * sizeof(char *));
        file_list[num_files++] = strdup(argv[ifile + 1]);
#endif
	}
      }
 
    if (G.List)
      {
      exit(EXIT_SUCCESS);
      }

    if (is_file)
      {
      read_3Dvff_file_header(argv[2],&mnc2,&vffattrs);
      }
    else
      {
      read_2Dvff_files_header(file_list,num_files,&mnc2,&vffattrs);
      }

    /* ok starting to create minc2.0 file assuming 3D must take care of 2D*/

    r = micreate_dimension("zspace", MI_DIMCLASS_SPATIAL, 
                           MI_DIMATTR_REGULARLY_SAMPLED, mnc2.mnc_count[2], &hdim[0]);
    if (r != 0) {
      TESTRPT("failed create_dimension zspace", r);
      return (1);
    }
    r = micreate_dimension("yspace", MI_DIMCLASS_SPATIAL, 
                           MI_DIMATTR_REGULARLY_SAMPLED, mnc2.mnc_count[1], &hdim[1]);
    if (r != 0) {
     TESTRPT("failed create_dimension yspace", r);
      return (1);
    }
    r = micreate_dimension("xspace", MI_DIMCLASS_SPATIAL, 
                           MI_DIMATTR_REGULARLY_SAMPLED, mnc2.mnc_count[0], &hdim[2]);
    if (r != 0) {
      TESTRPT("failed create_dimension xspace", r);
      return (1);
    }
    
    r = miset_dimension_start(hdim[0], mnc2.mnc_starts[2]);
    if (r < 0) {
      TESTRPT("failed dimension start xspace", r);
      return (1);
    }
    r = miset_dimension_start(hdim[1], mnc2.mnc_starts[1]);
    if (r < 0) {
      TESTRPT("failed dimension start yspace", r);
      return (1);
    }
    /* create negative start for xspace to correct orientation */
    r = miset_dimension_start(hdim[2], mnc2.mnc_starts[0] * -1);
    if (r < 0) {
      TESTRPT("failed dimension start zspace", r);
      return (1);
    }
    /* create negative spacing for zspace to correct orientation */
    r = miset_dimension_separation(hdim[0], mnc2.mnc_steps[2] * -1);
    if (r < 0) {
      TESTRPT("failed dimension separation xspace", r);
      return (1);
    }
    /* create negative spacing for yspace to correct orientation */
    r = miset_dimension_separation(hdim[1], mnc2.mnc_steps[1] * -1);
    if (r < 0) {
      TESTRPT("failed dimension separation yspace", r);
      return (1);
    }
    
    r = miset_dimension_separation(hdim[2], mnc2.mnc_steps[0]);
    if (r < 0) {
      TESTRPT("failed dimension separation zspace", r);
      return (1);
    }
    
    r = micreate_volume(out_str,MAX_VFF_DIMS, hdim, mnc2.mnc_type, 
                        MI_CLASS_REAL, NULL, &hvol);
    if (r != 0) {
      TESTRPT("error creating volume", r);
      return (1);
    }

    r = micreate_volume_image(hvol);
    if (r != 0) {
      TESTRPT("error creating volume", r);
      return (1);
    }
    
    // read image slice by slice
    if (is_file)
      {
	read_3Dvff_file_image(hvol, argv[2], mnc2, vffattrs, mnc_vrange);
      }
    else 
      {
	read_2Dvff_files_image(hvol,file_list,num_files, mnc2, vffattrs, mnc_vrange);
      }
    miset_volume_valid_range(hvol,mnc_vrange[1], mnc_vrange[0]);

    if (mnc2.mnc_srange[0] == -1 || mnc2.mnc_srange[1] == -1) {
      /* min and max are not specified in the file
         voxel range is set same as real range */
      mnc2.mnc_srange[0] = mnc_vrange[0];
      mnc2.mnc_srange[1] = mnc_vrange[1];
    }
    miset_volume_range(hvol,mnc2.mnc_srange[1], mnc2.mnc_srange[0]);
    
    if (is_file) {
      /* create minc history 3D */
      strcat(vffattrs.cmd_line,G.minc_history);
      G.minc_history = vffattrs.cmd_line;
      /* attributes from vff file itself 3D case*/
      add_vff_attribute_to_file(hvol,&vffattrs);
    }

    if (G.dirname != NULL) {
      /* attributes from external files 3D case*/
      add_attributes_from_files(hvol);
    }
    else if (!is_file) {
      /* just afew attributes from 2D case */
      ival =  vffattrs.year;
      r = miset_attr_values(hvol, MI_TYPE_INT, "study",
			    MIstart_year,1 , &ival);
      if (r < 0) {
	TESTRPT("failed to add date:year attribute", r);
      }
      ival = vffattrs.month;
      r = miset_attr_values(hvol, MI_TYPE_INT, "study",
			    MIstart_month,1 , &ival);
      if (r < 0) {
	TESTRPT("failed to add date:month attribute", r);
      }
      ival = vffattrs.day;
      r = miset_attr_values(hvol, MI_TYPE_INT, "study",
			    MIstart_day,1 , &ival);
      if (r < 0) {
	TESTRPT("failed to add date:day attribute", r);
      }
    }

    /* add history attribute */
    r = miadd_history_attr(hvol,strlen(G.minc_history), G.minc_history);
    if (r < 0) {
	TESTRPT("error creating history", r);
	return (1);
      }
    
    if (file_list != NULL) {
      free_list(num_files, file_list);
      free(file_list);
    }
    miclose_volume(hvol);

    exit(EXIT_SUCCESS);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : computeScalarRange
@INPUT      : datatype
              range (max, min)
	      count (dim1 * dim2)
	      buffer (actual data)
@OUTPUT     : voxel range max and min
@RETURNS    : (nothing)
@DESCRIPTION: Function to scan data for voxel min and max
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Jul 2006 (Leila Baghdadi)
@MODIFIED   : Adopted from Bert's code for DICOM files.
---------------------------------------------------------------------------- */
static void
computeScalarRange(int datatype,double range[2],long count,void *buffer)
{
  int i;
  
  range[0] = DBL_MAX;
  range[1] = -DBL_MAX;
  
  for(i=0; i < count; i++)
    {
      double tmp;
      switch (datatype) {
      case MI_TYPE_UBYTE:
	tmp = (double) ((unsigned char *)buffer)[i];
	break;
      case MI_TYPE_BYTE:
	tmp = (double) ((char *)buffer)[i];
	break;
      case MI_TYPE_SHORT:
	tmp = (double) ((short *)buffer)[i];
	break;
      case MI_TYPE_USHORT:
	tmp = (double) ((unsigned short *)buffer)[i];
	break;
      case MI_TYPE_FLOAT:
	tmp = (double) ((float *)buffer)[i];
	break;
      default:
	printf("Data type %d not handled\n", datatype);
	break;
      }
      
      if (tmp < range[0]) {
	range[0] = tmp;
      }
      if (tmp > range[1]) {
	range[1] = tmp;
      }
    }
}

static int
usage(void)
{
 static const char msg[] = {
        "vff2mnc: Convert VFF file(s) to MINC2.0 format\n"
        "usage: vff2mnc [options] <output.mnc> <vff_file1> [<file2> ...]\n"
    };

    fprintf(stderr,"%s", msg);
    return (-1);
}

static void 
free_list(int num_files, 
          const char **file_list)
{
    int i;

    for (i = 0; i < num_files; i++) {
        if (file_list[i] != NULL) {
            free((void *) file_list[i]);
        }
    }
}
/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_2Dvff_files_header
@INPUT      : file_list (list of files with directory)
              num_files number of filenames
	      m2 minc2 variables (struct)
	      vattrs vff attributes to be added to minc2.0
@OUTPUT     : (nothing)
@RETURNS    : 
@DESCRIPTION: Function to read 2D vff files header information
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Jul 2006 (Leila Baghdadi)
@MODIFIED   : 
---------------------------------------------------------------------------- */
void
read_2Dvff_files_header(const char **file_list, int num_files,
			struct mnc_vars *m2, struct vff_attrs *vattrs)
{
  FILE *fp ;
  int i;
  char linebuf[1024];
  char temp[10];
  char *pch;
  long rawsize;
  
  double element_size = 1;
  
  m2->mnc_steps[0] = 1;
  m2->mnc_steps[1] = 1;
  m2->mnc_steps[2] = 1;

  m2->mnc_starts[0] = 0;
  m2->mnc_starts[1] = 0;
  m2->mnc_starts[2] = 0;

  // set z dimension
  m2->mnc_count[2] = num_files;

  /* check files */
  
  for (i=0; i < num_files; i++) {
    fp = fopen(file_list[i]  , "rb" ) ;
    if( fp == NULL ) {
      exit(EXIT_FAILURE);  /* bad open? */
    }
    /* ensure file is VFF */
    if(fgets(linebuf, sizeof(linebuf), fp) == NULL){
       fprintf(stderr, "Error with fgets on line %d of %s\n", 
          __LINE__, __FILE__);
       }
    if (strncmp(linebuf, "ncaa", 4) != 0) {
      fclose(fp);
      printf("File is not in vff format!!!\n");
      exit(EXIT_FAILURE);
    }
    /* create a dictionary */
    while (linebuf[0] != '\f') {
      if(fgets(linebuf, sizeof(linebuf), fp) == NULL){
         fprintf(stderr, "Error with fgets on line %d of %s\n", 
            __LINE__, __FILE__);
         }
      pch=strchr(linebuf, '=');
      if (pch != NULL && pch[1] !=';') {
	strncpy(temp, linebuf, pch-linebuf);
	temp[pch-linebuf]='\0';
	if (strcmp(temp, "rank") == 0) {
	  if (atoi(pch+1) != 2) {
	    printf("Looking for 2D file got %d\n",atoi(pch+1));
	    exit(EXIT_FAILURE);
	  } 
	  m2->mnc_ndims = atoi(pch+1);
	}
	else if (strcmp(temp, "size") == 0) {
	  m2->mnc_count[0] = atoi(pch+1);
	  pch = (char*) memchr (pch+1, ' ', strlen(linebuf));
	  m2->mnc_count[1] = atoi(pch+1);
	}
	else if (strcmp(temp, "rawsize") == 0) {
	  rawsize = atoi(pch+1); 
	}
	else if (strcmp(temp, "spacing") == 0) {
	  m2->mnc_steps[0] = atof(pch+1);
	  pch = (char*) memchr (pch+1, ' ', strlen(linebuf));
	  m2->mnc_steps[1] = atoi(pch+1);
	}
	else if (strcmp(temp, "origin") == 0) {
	  m2->mnc_starts[0] = atof(pch+1); 
	  pch = (char*) memchr (pch+1, ' ', strlen(linebuf));
	  m2->mnc_starts[1] = atof(pch+1);
	}
	else if (strcmp(temp, "bands") == 0) {
	  vattrs->bands = atoi(pch+1); 
	}
	else if (strcmp(temp, "bits") == 0) {
	  switch(atoi(pch+1)) {
	  case 8:
	    m2->mnc_type = MI_TYPE_UBYTE;
	    vattrs->bits = 8;
	    break;
	  case 16:   
	    m2->mnc_type = MI_TYPE_SHORT;
	    vattrs->bits = 16;
	    break;
	  default:
	    printf("Could not determine data type!!!\n");
	    exit(EXIT_FAILURE);
	  }
	}
	else if (strcmp(temp, "date") == 0) {
	  vattrs->day = atoi(pch+7);
	  pch[7]='\0';
	  vattrs->month = atoi(pch+5);
	  pch[5]='\0';
	  vattrs->year = atoi(pch+1);
	}
	else if (strcmp(temp, "min") == 0) {
	  m2->mnc_srange[0] = atof(pch+1);
	}
	else if (strcmp(temp, "max") == 0) {
	  m2->mnc_srange[1] = atof(pch+1);
	}
	else if (strcmp(temp, "elementsize") == 0) {
	element_size = atof(pch+1);
	for(i=0; i<MAX_VFF_DIMS;i++)
	  {
	  m2->mnc_steps[i] *= element_size;
          m2->mnc_starts[i] *= element_size;
	  }
	}
      }
    }

    fclose(fp);
    
    } 
}
/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_2Dvff_files_image
@INPUT      : hvol minc volume handle
              file_list (list of files with directory)
	      num_files number of filenames
	      m2 minc2 variables (struct)
	      vattrs vff attributes to be added to minc2.0
	      range (data min and max range)
@OUTPUT     : (nothing)
@RETURNS    :
@DESCRIPTION: Function to read the 2D vff files image
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Jan 2007 (Leila Baghdadi)
@MODIFIED   : 
---------------------------------------------------------------------------- */
void
read_2Dvff_files_image(mihandle_t hvol, const char **file_list, int num_files,
		       struct mnc_vars m2, struct vff_attrs vattrs,
		       double range[2])
{
  FILE *fp ;
  int i,counts,r;
  void *buffer;
  int number_of_bits = vattrs.bits/8;
  unsigned long start[MAX_VFF_DIMS]; /* MINC data starts */
  unsigned long count[MAX_VFF_DIMS];
  double valid_range[2];

  range[0] = DBL_MAX;
  range[1] = -DBL_MAX;
  
  start[1] = 0;
  start[2] = 0;
  
  count[1] = m2.mnc_count[1];
  count[2] = m2.mnc_count[0];
  
  counts = m2.mnc_count[0]*m2.mnc_count[1];
  // allocate enough memory for one image (i.e, one slice)
  buffer = malloc(counts * number_of_bits);
  CHKMEM(buffer);

  for (i=0; i < num_files; i++) {
    // set start to the current slice
    start[0] = i;  
    count[0] = 1;
    // open file
    fp = fopen(file_list[i]  , "rb" ) ;
    if( fp == NULL ) {
      exit(EXIT_FAILURE);  /* bad open? */
    }
  
    // Set file position indicator to beginning of data
    r = fseek(fp,-counts* number_of_bits,SEEK_END);
    if ( r != 0) {
      printf(" fseek is reporting a problem!!\n");
      exit(EXIT_FAILURE);
    }
    
    r = fread(buffer,sizeof(char),counts * number_of_bits,fp);
    if ( r == 0) {
      printf(" fread is reporting a problem.\n");
      exit(EXIT_FAILURE);
    }  

    if (G.little_endian && vattrs.bits==16) {
      /* default switch byte order of 16bit data */
      swab(buffer, buffer, counts * number_of_bits);
    }

     // write the slice
    r = miset_voxel_value_hyperslab(hvol, m2.mnc_type,
				    start, count, buffer);
    if (r != 0) {
      TESTRPT("can not write data with hperslab function",r);
      exit(EXIT_FAILURE);
    }
    //calculate min and max of slice
    
    computeScalarRange(m2.mnc_type,valid_range,count[1] * count[2],buffer);
   
    if (valid_range[0] < range[0]) {
      range[0] = valid_range[0];
    }
    if (valid_range[1] > range[1]) {
      range[1] = valid_range[1];
      }
    
    fclose(fp);
  }
    
  free(buffer); 
    
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_3Dvff_file_header
@INPUT      : filename (vff filename)
	      m2 minc2 variables (struct)
	      vattrs vff attributes to be added to minc2.0
@OUTPUT     : (nothing)
@RETURNS    :
@DESCRIPTION: Function to read the 3D vff file header information.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Jul 2006 (Leila Baghdadi)
@MODIFIED   : 
---------------------------------------------------------------------------- */
void
read_3Dvff_file_header(char *filename, struct mnc_vars *m2, struct vff_attrs *vattrs)
{
  FILE *fp ;
  char linebuf[1024];
  char temp[20];
  
  double element_size;
  char *pch;
  int counter;
  int i;
 
  
  m2->mnc_steps[0]=1;
  m2->mnc_steps[1]=1;
  m2->mnc_steps[2]=1;

  /* open file */
  
  fp = fopen(filename  , "rb" ) ;

  if( fp == NULL ) {
    exit(EXIT_FAILURE);  /* bad open? */
  }
  /* ensure file is VFF */
  if(fgets(linebuf, sizeof(linebuf), fp) == NULL){
     fprintf(stderr, "Error with fgets on line %d of %s\n", 
        __LINE__, __FILE__);
     }
  if (strncmp(linebuf, "ncaa", 4) != 0) {
    fclose(fp);
    printf("File is not in vff format!!!\n");
    exit(EXIT_FAILURE);
  }

  /* create a dictionary */
  while (linebuf[0] != '\f') {
    counter =0;
    if(fgets(linebuf, sizeof(linebuf), fp) == NULL){
       fprintf(stderr, "Error with fgets on line %d of %s\n", 
          __LINE__, __FILE__);
       }
    pch=strchr(linebuf, '=');
    if (pch != NULL && pch[1] !=';') {
      strncpy(temp, linebuf, pch-linebuf);
      temp[pch-linebuf]='\0';
      if (strcmp(temp, "rank") == 0) {
	m2->mnc_ndims = atoi(pch+1);
      }
      else if (strcmp(temp, "size") == 0) {
	while(pch != NULL) {
	  m2->mnc_count[counter] = atoi(pch+1); 
	  pch = (char*) memchr (pch+1, ' ', strlen(linebuf));
	  counter++;
	}
      }
      else if (strcmp(temp, "origin") == 0) {
	while(pch != NULL) {
	  m2->mnc_starts[counter] = atof(pch+1); 
	  pch = (char*) memchr (pch+1, ' ', strlen(linebuf));
	  counter++;
	}
      }
      else if (strcmp(temp, "y_bin") == 0) {
	vattrs->y_bin = atoi(pch+1); 
      }
      else if (strcmp(temp, "z_bin") == 0) {
	vattrs->z_bin = atoi(pch+1); 
      }
      else if (strcmp(temp, "bands") == 0) {
	vattrs->bands = atoi(pch+1); 
      }
      else if (strcmp(temp, "center_of_rotation") == 0) {
	vattrs->center_of_rotation = atof(pch+1); 
      }
      else if (strcmp(temp, "central_slice") == 0) {
	vattrs->central_slice = atof(pch+1); 
      }
      else if (strcmp(temp, "rfan_y") == 0) {
	vattrs->rfan_y = atof(pch+1); 
      }
      else if (strcmp(temp, "rfan_z") == 0) {
	vattrs->rfan_z = atof(pch+1); 
      }
      else if (strcmp(temp, "angle_increment") == 0) {
	vattrs->angle_increment = atof(pch+1); 
      }    
      else if (strcmp(temp, "bits") == 0) {
	switch(atoi(pch+1)) {
	case 8:
	  m2->mnc_type = MI_TYPE_UBYTE;
	  vattrs->bits = 8;
	  break;
	case 16:
	  m2->mnc_type = MI_TYPE_SHORT;
	  vattrs->bits = 16;
	  break;
	default:
	  printf("Could not determine data type!!!\n");
	  exit(EXIT_FAILURE);
	}
      }
      else if (strcmp(temp, "date") == 0) {
	vattrs->day = atoi(pch+7);
	pch[7]='\0';
	vattrs->month = atoi(pch+5);
	pch[5]='\0';
	vattrs->year = atoi(pch+1);
      }
      else if (strcmp(temp, "reverse_order") == 0) {
	if (strcmp(pch+1,"no"))
	  vattrs->reverse_order = 0;
	else
	  vattrs->reverse_order = 1;
      }
      else if (strcmp(temp, "min") == 0) {
	m2->mnc_srange[0] = atof(pch+1);
      }
      else if (strcmp(temp, "max") == 0) {
	m2->mnc_srange[1] = atof(pch+1);
      }
      else if (strcmp(temp, "spacing") == 0) {
	while(pch != NULL) {
	  m2->mnc_steps[counter] = atof(pch+1); 
	  pch = (char*) memchr (pch+1, ' ', strlen(linebuf));
	  counter++;
	}
      }
      else if (strcmp(temp, "elementsize") == 0) {
	element_size = atof(pch+1);
	for(i=0; i<MAX_VFF_DIMS;i++)
	  {
	  m2->mnc_steps[i] *= element_size;
          m2->mnc_starts[i] *= element_size;
	  }
      }
      else if (strcmp(temp, "cmdLine") == 0) {
	strcpy(vattrs->cmd_line,linebuf);
      }
    }
  }
  
  fclose(fp);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_3Dvff_file_image
@INPUT      : hvol minc volume handle
              filename (vff filename)
	      m2 minc2 variables (struct)
	      vattrs vff attributes to be added to minc2.0
	      range (data min and max range)
	     
@OUTPUT     : (nothing)
@RETURNS    :  
@DESCRIPTION: Function to read the 3D vff file image
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Jan 2007 (Leila Baghdadi)
@MODIFIED   : 
---------------------------------------------------------------------------- */
void
read_3Dvff_file_image(mihandle_t hvol, char *filename, 
		      struct mnc_vars m2, struct vff_attrs vattrs, 
		      double range[2])
{
  FILE *fp ;
  int i,counts,r;
  void *buffer;
  int number_of_bits = vattrs.bits/8;
  unsigned long start[MAX_VFF_DIMS]; /* MINC data starts */
  unsigned long count[MAX_VFF_DIMS];
  double valid_range[2];
 
  range[0] = DBL_MAX;
  range[1] = -DBL_MAX;

  start[1] = 0;
  start[2] = 0;

  count[1] = m2.mnc_count[1];
  count[2] = m2.mnc_count[0];
  
  counts = m2.mnc_count[0]*m2.mnc_count[1];

  /* open file */
  fp = fopen(filename  , "rb" );
  if( fp == NULL ) {
    exit(EXIT_FAILURE);  /* bad open? */
  }
    
  // Set file position indicator to beginning of image
  r = fseek(fp,-counts * m2.mnc_count[2] * number_of_bits,SEEK_END);
  if ( r != 0) {
    printf(" fseek is reporting a problem!!\n");
    exit(EXIT_FAILURE);
  }
     
  // allocate memory for one slice only
  buffer = malloc( counts * number_of_bits);
  CHKMEM(buffer);
  
  for (i = 0; i < m2.mnc_count[2]; i++)
    {
    // set start to the current slice
    start[0] = i;  
    count[0] = 1;
    
    // read data one slice at a time
    r = fread(buffer,sizeof(char),counts * number_of_bits,fp);
    if ( r == 0) {
      printf(" fread is reporting a problem leila.\n");
      exit(EXIT_FAILURE);
    }
    
    if (G.little_endian && vattrs.bits==16) {
     
      swab(buffer, buffer, counts * number_of_bits);
    }
    // write the slice
    r = miset_voxel_value_hyperslab(hvol, m2.mnc_type,
				    start, count, buffer);
    if (r != 0) {
      TESTRPT("can not write data with hperslab function",r);
      exit(EXIT_FAILURE);
    }
 
    //calculate min and max of slice
    
    computeScalarRange(m2.mnc_type,valid_range,counts,buffer);
   
    if (valid_range[0] < range[0]) {
      range[0] = valid_range[0];
    }
    if (valid_range[1] > range[1]) {
      range[1] = valid_range[1];
      }
    
    } 
  free(buffer);

  fclose(fp);
} 

/* ----------------------------- MNI Header -----------------------------------
@NAME       : add_vff_attributes_to_file
@INPUT      : hvol volume handle
	      vattrs vff attributes to be added to minc2.0
@OUTPUT     : (nothing)
@RETURNS    :  
@DESCRIPTION: adds various attributes to minc file
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Jul 2006 (Leila Baghdadi)
@MODIFIED   : 
---------------------------------------------------------------------------- */
int
add_vff_attribute_to_file(mihandle_t hvol,const struct vff_attrs *vattrs)
{
  int ival;
  double dval;
  int r;
  ival = vattrs->y_bin;
  r = miset_attr_values(hvol, MI_TYPE_INT, "acquisition",
			"ybin",1 , &ival);
  if (r < 0) {
    TESTRPT("failed to add y_bin attribute", r);
  }
  ival = vattrs->z_bin;
  r = miset_attr_values(hvol, MI_TYPE_INT, "acquisition",
			"z_bin",1 , &ival);
  if (r < 0) {
    TESTRPT("failed to add z_bin attribute", r);
  }
  ival = vattrs->bands;
  r = miset_attr_values(hvol, MI_TYPE_INT, "acquisition",
			"bands",1 , &ival);
  if (r < 0) {
   TESTRPT("failed to add bands attribute", r);
  }
  dval = vattrs->center_of_rotation;
  r = miset_attr_values(hvol, MI_TYPE_DOUBLE, "acquisition",
			"center_of_rotation",1 , &dval);
  if (r < 0) {
    TESTRPT("failed to add center_of_rotation attribute", r);
  }
  dval = vattrs->central_slice;
  r = miset_attr_values(hvol, MI_TYPE_DOUBLE, "acquisition",
			"central_slice",1 , &dval);
  if (r < 0) {
    TESTRPT("failed to add central_slice attribute", r);
  }
  dval = vattrs->rfan_y;
  r = miset_attr_values(hvol, MI_TYPE_DOUBLE, "acquisition",
			"rfan_y",1 , &dval);
  if (r < 0) {
    TESTRPT("failed to add rfan_y attribute", r);
  }
  dval = vattrs->rfan_z;
  r = miset_attr_values(hvol, MI_TYPE_DOUBLE, "acquisition",
			"rfan_z",1 , &dval);
  if (r < 0) {
    TESTRPT("failed to add rfan_z attribute", r);
  }
  dval = vattrs->angle_increment;
  r = miset_attr_values(hvol, MI_TYPE_DOUBLE, "acquisition",
			"angle_increment",1 , &dval);
  if (r < 0) {
    TESTRPT("failed to add angle_increment attribute", r);
  }
  ival = vattrs->reverse_order;
  r = miset_attr_values(hvol, MI_TYPE_INT, "acquisition",
			"reverse_order",1 , &ival);
  if (r < 0) {
    TESTRPT("failed to add reverse_order attribute", r);
  }
  ival =  vattrs->year;
  r = miset_attr_values(hvol, MI_TYPE_INT, "study",
			MIstart_year,1 , &ival);
  if (r < 0) {
    TESTRPT("failed to add date:year attribute", r);
  }
  ival = vattrs->month;
  r = miset_attr_values(hvol, MI_TYPE_INT, "study",
			MIstart_month,1 , &ival);
  if (r < 0) {
    TESTRPT("failed to add date:month attribute", r);
  }
  ival = vattrs->day;
  r = miset_attr_values(hvol, MI_TYPE_INT, "study",
			MIstart_day,1 , &ival);
  if (r < 0) {
    TESTRPT("failed to add date:day attribute", r);
  }

  return(0);
}
/* ----------------------------- MNI Header -----------------------------------
@NAME       : add_attributes_from_files
@INPUT      : hvol volume handle
	      
@OUTPUT     : (nothing)
@RETURNS    :  
@DESCRIPTION: adds a few files generated with vff file to minc file
@METHOD     : This method assumes the txt files are in the top directory
@GLOBALS    : 
@CALLS      : 
@CREATED    : Jul 2006 (Leila Baghdadi)
@MODIFIED   : This code is a bit ugly thanks for the wodnerful work of windows.
---------------------------------------------------------------------------- */
int
add_attributes_from_files(mihandle_t hvol)
{
  FILE *inf;
  char *strbuf;
  char *buffer;
  string_t fullpath_pro;
  string_t fullpath_des;
  string_t fullpath_par;
  char *p;
  int r,i;
  char **str;
  int found_protocol=0;
  int found_description=0;
  int found_parameters=0;

  
  /* first find files names --> different platforms */
  find_filenames_first(fullpath_pro,fullpath_des,fullpath_par,
		       &found_protocol,&found_description, &found_parameters); 

  if (!found_protocol) {
    printf("could not find file with extension protocol\n");
  }
  else {
    /* add *.protocol to "acquisition" protocol attribute */
    inf = fopen(fullpath_pro,"r");
    if (inf == NULL) {
      printf("Could not open file %s \n", fullpath_pro);
      exit(EXIT_FAILURE);
    }
    else {
      strbuf = malloc(MAX_BUF_LINE + 1);
      CHKMEM(strbuf);
      strbuf[0]='\0';
      buffer = malloc(MAX_BUF_TEXT + 1);
      CHKMEM(buffer);
      buffer[0]='\0';
      while (fgets(strbuf, MAX_BUF_LINE, inf) != NULL) {
	strcat(buffer,strbuf);
      }
    }
    fclose(inf);
    
    r = miset_attr_values(hvol, MI_TYPE_STRING, "acquisition",
			  "protocol", strlen(buffer) ,buffer);
    free(buffer);
    free(strbuf);
    if (r < 0) {
      TESTRPT("failed to add protocol attribute", r);
    }
  }


  if (!found_description) {
    printf("Could not find file Description.txt\n");
  }
  else {
    /* add Description.txt to patient name and study id */
    inf = fopen(fullpath_des,"r");		       
    if (inf == NULL) {
      printf("Could not open file %s \n", fullpath_des);
      exit(EXIT_FAILURE);
    }
    else {
      /* just need the first two lines of this file */
      str = malloc(MAX_DESCRIPTION);
      CHKMEM(str);
      for (i=0; i < MAX_DESCRIPTION; i++) {
	str[i] = malloc(MAX_BUF_LINE + 1);
	CHKMEM(str[i]);
	if(fgets(str[i], MAX_BUF_LINE, inf) == NULL){
      fprintf(stderr, "Error with fgets on line %d of %s\n", 
         __LINE__, __FILE__);
      }
	for (p = str[i]; *p != '\0'; p++) {
	  if (*p == '\r') {
	    *p = '\0';
	  }
	}
      }
      r = miset_attr_values(hvol, MI_TYPE_STRING, "patient",
			    "full_name",strlen(str[0]) , str[0]);
      if (r < 0) {
	TESTRPT("failed to add full_name attribute", r);
      }
      
      r = miset_attr_values(hvol, MI_TYPE_STRING, "study",
			    "study_id",strlen(str[1]) , str[1]);
      if (r < 0) {
	TESTRPT("failed to add study_id attribute", r);
      }
      fclose(inf);
      for (i=0; i < MAX_DESCRIPTION; i++) {
	free(str[i]);
      }
      free(str);
    }
  }

  if (!found_parameters) {
    printf("Could not find file Parameters.txt\n");
  }
  else {
    /* add Parameters.txt to "acquisition" scan_parameters */
    inf = fopen(fullpath_par,"r");
    if (inf == NULL) {
      printf("Could not open file %s \n", fullpath_par);
      exit(EXIT_FAILURE);
    }
   else {
     strbuf = malloc(MAX_BUF_LINE + 1);
     CHKMEM(strbuf);
     strbuf[0]='\0';
     buffer = malloc(MAX_BUF_TEXT + 1);
     CHKMEM(buffer);
     buffer[0]='\0';
     while (fgets(strbuf, MAX_BUF_LINE, inf) != NULL) {
       strcat(buffer,strbuf);
     }
   }
    fclose(inf);
    
    r = miset_attr_values(hvol, MI_TYPE_STRING, "acquisition",
			  "scan_parameters",strlen(buffer) ,buffer);
    free(buffer);
    free(strbuf);
    if (r < 0) {
      TESTRPT("failed to add scan_parameters attribute", r);
    }
  }

  return(0);
}

int
find_filenames_first(string_t fullpath_pro,string_t fullpath_des,string_t fullpath_par,
		     int *found_protocol,int *found_description, int *found_parameters)
{ 
  char *ptr;
#if HAVE_DIRENT_H
  DIR *dp;  
  struct dirent *np;
  
  dp = opendir(G.dirname);
  
  if (dp != NULL) {
    
    while ((np = readdir(dp)) != NULL) {
	  if (np->d_name != "." && np->d_name != "..") {

	    /* add *.protocol to "acquisition" protocol attribute */
	    ptr = strstr(np->d_name,"protocol");
	    if (ptr != NULL) {
	      *found_protocol = 1;
	      strcpy(fullpath_pro,np->d_name);
	    }
	   
	    /* add Description.txt to patient name and study id */
	    ptr = strstr(np->d_name,"Description.txt");
	    if (ptr != NULL) {
	      *found_description = 1;
	      strcpy(fullpath_des,np->d_name);
	    }
	    
	    /* add Parameters.txt to "acquisition" scan_parameters */
	    ptr = strstr(np->d_name,"Parameters.txt");
	    if (ptr != NULL) {
	      *found_parameters=1;
	      strcpy(fullpath_par,np->d_name);
	    }  
	  }
    }
 }
  closedir(dp);
#endif

#if defined (_MSC_VER)
  WIN32_FIND_DATA FindFileData;
  HANDLE hFind = INVALID_HANDLE_VALUE;
  
  char DirSpec[MAX_BUF_LINE + 1];
  char dirname[MAX_BUF_LINE + 1];
  DWORD dwError;
  
  strncpy(DirSpec, G.dirname, MAX_BUF_LINE);
  DirSpec[MAX_BUF_LINE]='\0';
  strncat(DirSpec, "\\*", 3);
  /* make sure the directory ends with '/' */
  strncpy(dirname, G.dirname, MAX_BUF_LINE);
  dirname[MAX_BUF_LINE]='\0';
  if (G.dirname[strlen(G.dirname)-1] != '/') {
	strcat(dirname,"/");
  }

  hFind = FindFirstFile(DirSpec, &FindFileData);

  if (hFind == INVALID_HANDLE_VALUE) {
    printf(" Invalid file handle with Error %u \n", GetLastError());
    return(1);
  }
  else {
    
    do {
      /* add *.protocol to "acquisition" protocol attribute */
      ptr = strstr(FindFileData.cFileName,"protocol");
      if (ptr != NULL) {
	*found_protocol = 1;
	strcpy(fullpath_pro, dirname);
	strcat(fullpath_pro,FindFileData.cFileName);
      }

      /* add Description.txt to patient name and study id */
      ptr = strstr(FindFileData.cFileName,"Description.txt");
      if (ptr != NULL) {
	*found_description = 1;
	strcpy(fullpath_des, dirname);
	strcat(fullpath_des,FindFileData.cFileName);
      }
	    
      /* add Parameters.txt to "acquisition" scan_parameters */
      ptr = strstr(FindFileData.cFileName,"Parameters.txt");
      if (ptr != NULL) {
	*found_parameters=1;
	strcpy(fullpath_par, dirname);
	strcat(fullpath_par,FindFileData.cFileName);
      }  

    } while (FindNextFile(hFind, &FindFileData) != 0);

    dwError = GetLastError();
    FindClose(hFind);
    if (dwError != ERROR_NO_MORE_FILES) {
      printf(" Find next file %u\n", dwError);
      return(1);
    }
  }
#endif

  return(0);
}
