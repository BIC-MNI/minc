/* ----------------------------- MNI Header -----------------------------------
@NAME       : minctoecat
@INPUT      : argc, argv - command line arguments
@OUTPUT     : (none)
@RETURNS    : error status
@DESCRIPTION: Converts a minc format file to a CTI ECAT file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 31, 2005 (Anthonin Reilhac)
@MODIFIED   : 
@COPYRIGHT  :
              Copyright 2005 Anthonin Reilhac, McConnell Brain Imaging Centre, 
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */


#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <minc.h>
#include <time.h>
#include <ParseArgv.h>
#include <volume_io.h>  
#include "ecat_write.h"    
#define ShortMax 32767
#define BECQUEREL_PER_NCURIE 37

typedef enum MODALITY {
  UNKNOWN_MODALITY,
  PET,
  MRI,
  NUM_MODALITY
} modality;

typedef enum DATA_UNIT {
  UNKNOWN_DATA_UNIT,
  LABEL,
  NCIPERCC
} data_unit;


typedef struct MINC_INFO {
  int dim;
  long x_size;
  long y_size;
  long z_size;
  long time_length;
  double x_step;
  double y_step;
  double z_step;
  int x_start_flag;
  int y_start_flag;
  int z_start_flag;
  double x_start;
  double y_start;
  double z_start;
  double *time_points;
  double *time_widths;
  data_unit dunit;
  modality mod;
  float isotope_halflife;
} minc_info;


/*Fuction declarations*/
double decay_correction(double scan_time, double measure_time, double start_time, double half_life);
minc_info *get_minc_info(char *file);
void free_minc_info(minc_info *mi);
void get_patient_info(char *file, Main_header *mh);
void get_study_info(char *file, Main_header *mh);
void get_acquisition_info(char *file, Main_header *mh);
void get_ecat_acquisition_info(char *file, Main_header *mh);
void get_ecat_main_header_info(char *file, Main_header *mh);
void get_ecat_subheader_info(char *file, Image_subheader *sh);
Main_header * init_main_header(char *file, minc_info *mi);
Image_subheader * init_image_subheader(char *file, minc_info *mi, Main_header *mh);
void write_ecat_frame(MatrixFile *mf, Image_subheader *sh, short *ptr,int fr, float min, float max);
void usage_error(char *progname);

int main ( int argc, char **argv) {
  /* Variables for arguments */
  static int include_patient_info = TRUE;
  static int include_study_info = TRUE;
  static int include_acquisition_info = TRUE;
  static int include_ecat_acquisition_info = TRUE;
  static int include_ecat_main_info = TRUE;
  static int include_ecat_subheader_info = TRUE;
  static int do_decay_corr_factor = TRUE;
  static int label_data = FALSE;
  
  /* Argument option table */
  static ArgvInfo argTable[] = {
    {"-ignore_patient_variable", ARGV_CONSTANT, (char *) FALSE, (char *) &include_patient_info,"Ignore informations from the minc patient variable."},
    {"-ignore_study_variable", ARGV_CONSTANT, (char *) FALSE, (char *) &include_study_info,"Ignore informations from the minc study variable."},
    {"-ignore_acquisition_variable", ARGV_CONSTANT, (char *) FALSE, (char *) &include_acquisition_info,"Ignore informations from the minc acquisition variable."},
    {"-ignore_ecat_acquisition_variable", ARGV_CONSTANT, (char *) FALSE, (char *) &include_ecat_acquisition_info,"Ignore informations from the minc ecat_acquisition variable."},
    {"-ignore_ecat_main", ARGV_CONSTANT, (char *) FALSE, (char *) &include_ecat_main_info,"Ignore informations from the minc ecat-main variable."},
    {"-ignore_ecat_subheader_variable", ARGV_CONSTANT, (char *) FALSE, (char *) &include_ecat_subheader_info,"Ignore informations from the minc ecat-subhdr variable."},
    {"-no_decay_corr_fctr", ARGV_CONSTANT, (char *) FALSE, (char *) &do_decay_corr_factor,"Do not compute the decay correction factors"},
    {"-label", ARGV_CONSTANT, (char *) TRUE, (char *) &label_data,"Voxel values are treated as integers, scale and calibration factors are set to unity"},
    {NULL, ARGV_END, NULL, NULL, NULL} 
  };

  /*other variables*/
  char *pname; /*name of the present command ->argv[0]*/
  char *minc_file_name = NULL; /*name of the input file (minc format)*/
  char *ecat_file_name = NULL; /*name of the output file (to be created into the ecat format*/
  minc_info *mi = NULL;
  Volume volume;
  short int *ecat_short_ptr = NULL;
  float ecat_scale_factor = 1;
  progress_struct progress;
  STRING DimOrder[MAX_DIMENSIONS];
  MatrixFile *out_matrix_file = NULL;
  Main_header *out_main_header = NULL;
  Image_subheader *out_image_subheader = NULL;
  int fr;
  int incre = 0;
  int x_ind, y_ind, z_ind;
  double cal_factor;  

   /* Check arguments */
   pname = argv[0];
   if (ParseArgv(&argc, argv, argTable, 0) || (argc != 3)) {
      usage_error(pname);
   }

   /* Get file names */
   minc_file_name = argv[1];
   ecat_file_name = argv[2];

  /*getting informations about the dimensions from the minc file*/
  if((mi = get_minc_info(minc_file_name)) == NULL) {
    fprintf(stderr, "Can not get informations from %s.\n", minc_file_name);
    return (1);
  }

  /*checking the number of dimension*/
  if((mi->dim != 3) && (mi->dim != 4)) {
    fprintf(stderr, "input file has not a valid number of dimension (%d), should be either 3 (static file) or 4 (dynamic file)");
    return (-1);
  }

  
  /***************************************************************************************
   *       filling the ecat header fields with the available informations                *
   ***************************************************************************************/
  /*MAIN HEADER: filling compulsory fields*/
  if((out_main_header = init_main_header(minc_file_name, mi)) == NULL) {
    fprintf(stderr, "Can not initilize the ecat main header.\n");
    return (1);
  }
  /*additional informations if available and requested*/
  /*getting patient informations from the MIpatient variable*/
  
  if(include_patient_info)
    get_patient_info(minc_file_name, out_main_header);
  /*getting study information from the MIstudy variable*/
  if(include_study_info)
    get_study_info(minc_file_name, out_main_header);
  /*getting acquisition information from the MIacquisition variable*/
  if(include_acquisition_info)
    get_acquisition_info(minc_file_name, out_main_header);  
  /*getting acquisition information from the ecat_acquisition variable*/
  if(include_ecat_acquisition_info)
    get_ecat_acquisition_info(minc_file_name, out_main_header);
  /*getting ecat main header informations from the ecat-main variable*/
  if(include_ecat_main_info)
    get_ecat_main_header_info(minc_file_name, out_main_header);

  /*SUBHEADER TEMPLATE: filling compulsory fields*/
 if((out_image_subheader = init_image_subheader(minc_file_name, mi, out_main_header)) == NULL) {
    fprintf(stderr, "Can not initialize the ecat image subheader.\n");
    return (1);
  }
 /*getting additionnal informations from the ecat-subhdr variable*/
 if(include_ecat_subheader_info)
   get_ecat_subheader_info(minc_file_name, out_image_subheader);
  

  /*writting the main header*/
  if((out_matrix_file = matrix_create(ecat_file_name, out_main_header)) == NULL) {
    fprintf(stderr, "cannot create %s \n", ecat_file_name);
    return(-1);
  }
  
  /*extracting the mincvolume in the order time, X, Y, Z*/
  if(mi->dim == 4) {
    DimOrder[0] = MItime;
    DimOrder[1] = MIzspace;
    DimOrder[2] = MIyspace;
    DimOrder[3] = MIxspace;
  } else {
    DimOrder[0] = MIzspace;
    DimOrder[1] = MIyspace;
    DimOrder[2] = MIxspace;   
  }

 
  /*input the volume*/
  if(input_volume(minc_file_name,
		  mi->dim,
		  DimOrder,
		  NC_UNSPECIFIED,
		  FALSE,
		  1,
		  1,
		  TRUE,
		  &volume,
		  (minc_input_options *) NULL) != OK) return (1);
  
   /*initializing the progress report*/
  initialize_progress_report(&progress,
                             FALSE,
                             mi->time_length,
                             "Converting data");
  cal_factor = (mi->dunit == NCIPERCC)?BECQUEREL_PER_NCURIE:1;
  cal_factor /= out_main_header->calibration_factor;

  /*getting the values*/
  for(fr = 0; fr < ((mi->time_length)?mi->time_length:1); fr++) {
    float max_val = 0, min_val = 0;
    int vx, vy, vz;
    Real value;

    if((ecat_short_ptr = (short int *)calloc((mi->x_size) * (mi->y_size) * (mi->z_size), sizeof(short int))) == NULL) {
      fprintf(stderr, "allocation error\n");
      return 1;
    }
    
    for(vz = 0; vz < mi->z_size; vz++) 
      for(vy = 0; vy < mi->y_size; vy++) 
	for(vx = 0; vx < mi->x_size; vx++) {
	  value = ((mi->time_length)?get_volume_real_value(volume, fr, vz, vy, vx, 0):get_volume_real_value(volume, vz, vy, vx, 0,0)) * cal_factor;
	  if(value > max_val) max_val = value;
	  if(value < min_val) min_val = value;
	}
    if((mi->dunit != LABEL) && (!label_data))
      ecat_scale_factor =  ((max_val > fabs(min_val))?max_val:fabs(min_val))/ShortMax;

    for(vz = 0; vz < mi->z_size; vz++) {
      for(vy = 0; vy < mi->y_size; vy++) {
	for(vx = 0; vx < mi->x_size; vx++) {
    	  value = ((mi->time_length)?get_volume_real_value(volume, fr, vz, vy, vx, 0):get_volume_real_value(volume, vz, vy, vx, 0,0));
	  x_ind = ((mi->x_step > 0)?mi->x_size - 1 - vx:vx);
	  y_ind = ((mi->y_step > 0)?mi->y_size - 1 - vy:vy);
	  z_ind = ((mi->z_step > 0)?mi->z_size - 1 - vz:vz);
	  ecat_short_ptr[IJK(z_ind, y_ind, x_ind, mi->y_size, mi->x_size)] = (short int)ROUND(value * cal_factor/(ecat_scale_factor));
	}
      }
    }
       
    /*filling image subheader*/
    if(mi->dim == 4) {
      out_image_subheader->frame_start_time = (unsigned int)(mi->time_points[fr] * 1000);
      out_image_subheader->frame_duration = (unsigned int)(mi->time_widths[fr] * 1000);
    }
    if((mi->dunit == NCIPERCC) && (do_decay_corr_factor))
      out_image_subheader->decay_corr_fctr = decay_correction(mi->time_points[fr],mi->time_widths[fr] , 0.0, mi->isotope_halflife);
    out_image_subheader->scale_factor = ecat_scale_factor;
    write_ecat_frame(out_matrix_file, out_image_subheader, ecat_short_ptr, fr, min_val, max_val);
    free(ecat_short_ptr);
    ecat_short_ptr = NULL;
    update_progress_report(&progress, incre);
    incre++;
  }
  free_minc_info(mi);
  matrix_close(out_matrix_file);
  free(out_main_header);
  free(out_image_subheader);
  
  terminate_progress_report(&progress);
  return (0);
}



double decay_correction(double scan_time, double measure_time, double start_time, double half_life) {
  double mean_life;
  double measure_correction;
  double decay;
  
  /* Check for negative half_life and calculate mean life */
  if (half_life <= 0.0) return 1.0;
  mean_life = half_life/ log(2.0);
  
  /* Normalize scan time and measure_time */
  scan_time = (scan_time - start_time) / mean_life;
  measure_time /= mean_life;
  
  /* Calculate correction for decay over measuring time (assuming a
     constant activity). Check for possible rounding errors. */
  if ((measure_time*measure_time/2.0) < DBL_EPSILON) {
    measure_correction = 1.0 - measure_time/2.0;
  }
  else {
    measure_correction = (1.0 - exp(-measure_time)) / fabs(measure_time);
  }
  
  /* Calculate decay */
  decay = exp(-scan_time) * measure_correction;
  if (decay<=0.0) decay = DBL_MAX;
  else decay = 1.0/decay;
  
  return decay;
}

minc_info *get_minc_info(char *file) {
  int minc_fd; /*minc file descriptor*/
  minc_info *mi = NULL; /*minc info structures*/
  int var_id;
  int num_att;
  char attname[NC_MAX_NAME+1];
  char buff[NC_MAX_NAME+1]; 
  int i;

 /* we first open the netcdf file*/
  minc_fd = miopen(file, NC_NOWRITE);

  if((mi = (minc_info *)calloc(1, sizeof(minc_info))) == NULL) return NULL;

  /*initlaization*/
  mi->time_length = 0;
  mi->mod = UNKNOWN_MODALITY;
  mi->dunit = UNKNOWN_DATA_UNIT;
  mi->x_start_flag = 0;
  mi->y_start_flag = 0;
  mi->z_start_flag = 0;

  /* first we check out the number of dimension. we suppose that the time dimension is the 4th*/
  ncinquire(minc_fd, &(mi->dim), NULL, NULL, NULL);
 
  /*getting infos about the x, y and and z sizes and width*/
  ncdiminq(minc_fd, ncdimid(minc_fd, MIxspace), NULL, &(mi->x_size));
  ncdiminq(minc_fd, ncdimid(minc_fd, MIyspace), NULL, &(mi->y_size));
  ncdiminq(minc_fd, ncdimid(minc_fd, MIzspace), NULL, &(mi->z_size)); 

  /*getting information about each dimensions*/
  /*MIxspace*/
  var_id = ncvarid(minc_fd, MIxspace);
  ncvarinq(minc_fd,var_id, NULL, NULL, NULL, NULL, &num_att);
  for(i = 0; i < num_att; i++) {
    ncattname(minc_fd,var_id,i,attname);
    if(strcmp(MIstart, attname) == 0) {
      mi->x_start_flag = 1;
      miattget1(minc_fd,var_id, MIstart, NC_DOUBLE, &(mi->x_start));
    }
    if(strcmp(MIstep, attname) == 0) 
      miattget1(minc_fd,var_id, MIstep, NC_DOUBLE, &(mi->x_step));
  }
  /*MIyspace*/
  var_id = ncvarid(minc_fd, MIyspace);
  ncvarinq(minc_fd,var_id, NULL, NULL, NULL, NULL, &num_att);
  for(i = 0; i < num_att; i++) {
    ncattname(minc_fd,var_id,i,attname);
    if(strcmp(MIstart, attname) == 0) {
      mi->y_start_flag = 1;
      miattget1(minc_fd,var_id, MIstart, NC_DOUBLE, &(mi->y_start));
    }
    if(strcmp(MIstep, attname) == 0) 
      miattget1(minc_fd,var_id, MIstep, NC_DOUBLE, &(mi->y_step));
  }

  /*MIzspace*/
  var_id = ncvarid(minc_fd, MIzspace);
  ncvarinq(minc_fd,var_id, NULL, NULL, NULL, NULL, &num_att);
  for(i = 0; i < num_att; i++) {
    ncattname(minc_fd,var_id,i,attname);
    if(strcmp(MIstart, attname) == 0) {
      mi->z_start_flag = 1;
      miattget1(minc_fd,var_id, MIstart, NC_DOUBLE, &(mi->z_start));
    }
    if(strcmp(MIstep, attname) == 0) 
      miattget1(minc_fd,var_id, MIstep, NC_DOUBLE, &(mi->z_step));
  }

  /* if it is a dynamic file, then we ask for the number of frames, the frame durations and start time*/
 if(mi->dim == 4){
    long start_time_vector[1];
    long count_time_vector[1];

    var_id = ncdimid(minc_fd, MItime);
    ncdiminq(minc_fd, var_id, NULL, &(mi->time_length));
    if((mi->time_points = (double *) calloc(mi->time_length, sizeof(double))) == NULL) return NULL;
    if((mi->time_widths = (double *) calloc(mi->time_length, sizeof(double))) == NULL) return NULL;
    start_time_vector[0] = 0;
    count_time_vector[0] = mi->time_length;

    mivarget(minc_fd, var_id, start_time_vector, count_time_vector, NC_DOUBLE, MI_SIGNED, mi->time_points);
    mivarget(minc_fd, ncvarid(minc_fd, MItime_width), start_time_vector, count_time_vector, NC_DOUBLE, MI_SIGNED, mi->time_widths);
  }

  /*defining the modality*/
  if(mivar_exists(minc_fd, MIstudy)) {
    var_id = ncvarid(minc_fd, MIstudy);
    ncvarinq(minc_fd, var_id, NULL, NULL, NULL, NULL, &num_att);
    for(i = 0; i < num_att; i++) {
      ncattname(minc_fd,var_id,i,attname);
      if(strcmp(MImodality, attname) == 0) { 
	miattgetstr(minc_fd, var_id, MImodality, NC_MAX_NAME, buff);
	if(strcmp(buff, MI_PET) == 0) mi->mod = PET;
	if(strcmp(buff, MI_MRI) == 0) mi->mod = MRI;
	if(strcmp(buff, MI_LABEL) == 0) mi->dunit = LABEL;
      }
    }
  }
  if(mivar_exists(minc_fd, MIimagemin)) {
    var_id = ncvarid(minc_fd, MIimagemin);
    ncvarinq(minc_fd, var_id, NULL, NULL, NULL, NULL, &num_att);
    for(i = 0; i < num_att; i++) {
      ncattname(minc_fd,var_id,i,attname);
      if(strcmp(MIunits, attname) == 0) {
	miattgetstr(minc_fd, var_id, MIunits, NC_MAX_NAME, buff);
	if(strcmp(buff, "nCi/cc") == 0) mi->dunit = NCIPERCC;
      }
    }
  }

  /*getting isotope halflife if available*/
  if(mivar_exists(minc_fd, MIacquisition)) {
    var_id = ncvarid(minc_fd, MIacquisition);
    ncvarinq(minc_fd, var_id, NULL, NULL, NULL, NULL, &num_att);
    for(i = 0; i < num_att; i++) {
      ncattname(minc_fd,var_id,i,attname);      
      if(strcmp(MIradionuclide_halflife, attname) == 0)
	miattget1(minc_fd, var_id, MIradionuclide_halflife, NC_FLOAT, &(mi->isotope_halflife));
    }
  }

  miclose(minc_fd);
  return mi;
}

void free_minc_info(minc_info *mi) {
  if(mi->time_points != NULL) free(mi->time_points);
  if(mi->time_widths != NULL) free(mi->time_widths);
}
void get_patient_info(char *file, Main_header *mh) {
  int minc_fd; /*minc file descriptor*/

 /* we first open the netcdf file*/
  minc_fd = miopen(file, NC_NOWRITE);

  if(mivar_exists(minc_fd, MIpatient)) {
    int var_id;
    int num_att;
    int i;
    
    var_id = ncvarid(minc_fd, MIpatient);
    ncvarinq(minc_fd, var_id, NULL, NULL, NULL, NULL, &num_att);
    for(i = 0; i < num_att; i++) {
      char attname[NC_MAX_NAME+1];
      char buffer_val[NC_MAX_NAME+1];
      
      ncattname(minc_fd,var_id,i,attname);
      if(strcmp(MIfull_name, attname) == 0) 
	miattgetstr(minc_fd, var_id, MIfull_name, 32, mh->patient_name);
      if(strcmp(MIage, attname) == 0) 
	miattget1(minc_fd, var_id, MIage, NC_FLOAT, &(mh->patient_age));
      if(strcmp(MIweight, attname) == 0) 
	miattget1(minc_fd, var_id, MIweight, NC_FLOAT, &(mh->patient_weight));
      if(strcmp(MIsize, attname) == 0) 
	miattget1(minc_fd, var_id, MIsize, NC_FLOAT, &(mh->patient_height));
      if(strcmp(MIsex, attname) == 0) {
	miattgetstr(minc_fd, var_id, MIsex, NC_MAX_NAME+1, buffer_val);	
	if(strcmp(buffer_val, MI_MALE) == 0)
	  mh->patient_sex[0] = 'M';
	if(strcmp(buffer_val, MI_FEMALE) == 0)
	  mh->patient_sex[0] = 'F';
      }
      if(strcmp(MIidentification, attname) == 0) 
	miattgetstr(minc_fd, var_id, MIidentification, 16, mh->patient_id);
  
      /*I don't kown how to deal with the patient birth date yet
	in minc, it is a string, in ecat a int ...
	I guess this is not important as long as we have the age of the patient
	Might be filled below if the ecat-header variable exists
      */
    }
  }
  miclose(minc_fd);
}

void get_study_info(char *file, Main_header *mh) {
  int minc_fd; /*minc file descriptor*/

 /* we first open the netcdf file*/
  minc_fd = miopen(file, NC_NOWRITE);

  if(mivar_exists(minc_fd, MIstudy)) {
    int var_id;
    int i;
    struct tm scan_time;
    int num_att;

    var_id = ncvarid(minc_fd, MIstudy);
    ncvarinq(minc_fd, var_id, NULL, NULL, NULL, NULL, &num_att);
    for(i = 0; i < num_att; i++) {
      char attname[NC_MAX_NAME+1];
      
      ncattname(minc_fd,var_id,i,attname);
      if(strcmp(MIstart_year, attname) == 0) { 
	miattget1(minc_fd, var_id, MIstart_year, NC_INT, &scan_time.tm_year);	
	scan_time.tm_year -= 1900;
      }
      if(strcmp(MIstart_month, attname) == 0) {
	miattget1(minc_fd, var_id, MIstart_month, NC_INT, &scan_time.tm_mon);
	scan_time.tm_mon--;
      }
      if(strcmp(MIstart_day, attname) == 0) 
	miattget1(minc_fd, var_id, MIstart_day, NC_INT, &scan_time.tm_mday);
      
      if(strcmp(MIstart_hour, attname) == 0) 
	miattget1(minc_fd, var_id, MIstart_hour, NC_INT, &scan_time.tm_hour);
      
      if(strcmp(MIstart_minute, attname) == 0) 
	miattget1(minc_fd, var_id, MIstart_minute, NC_INT, &scan_time.tm_min);
      
      if(strcmp(MIstart_seconds, attname) == 0) 
	miattget1(minc_fd, var_id, MIstart_seconds, NC_INT, &scan_time.tm_sec);

      if(strcmp(MIidentification, attname) == 0) 
	miattgetstr(minc_fd, var_id, MIidentification, 16, mh->patient_id);

      if(strcmp(MIstudy_id, attname) == 0)
	miattgetstr(minc_fd, var_id, MIstudy_id, 12, mh->study_name);

      if(strcmp(MIreferring_physician, attname) == 0)
	miattgetstr(minc_fd, var_id, MIreferring_physician, 32, mh->physician_name);

      if(strcmp(MIoperator, attname) == 0)
	miattgetstr(minc_fd, var_id, MIoperator, 32, mh->operator_name);      
    }
    mh->scan_start_time = mktime(&scan_time);
  }
  miclose(minc_fd);
}

void get_acquisition_info(char *file, Main_header *mh) {
  int minc_fd; /*minc file descriptor*/

 /* we first open the netcdf file*/
  minc_fd = miopen(file, NC_NOWRITE);

  if(mivar_exists(minc_fd, MIacquisition)) {
    int var_id;
    int num_att;
    int i;
    struct tm injection_time;
    
    var_id = ncvarid(minc_fd, MIacquisition);
    ncvarinq(minc_fd, var_id, NULL, NULL, NULL, NULL, &num_att);
    for(i = 0; i < num_att; i++) {
      char attname[NC_MAX_NAME+1];
      char buffer_val[NC_MAX_NAME+1];
      
      ncattname(minc_fd,var_id,i,attname);      
      
      if(strcmp(MIradionuclide, attname) == 0)
	miattgetstr(minc_fd, var_id, MIradionuclide, 8, mh->isotope_code);

      if(strcmp(MIradionuclide_halflife, attname) == 0)
	miattget1(minc_fd, var_id, MIradionuclide_halflife, NC_FLOAT, &(mh->isotope_halflife));

      if(strcmp(MItracer, attname) == 0)
	miattgetstr(minc_fd, var_id, MItracer, 32, mh->radiopharmaceutical);

      if(strcmp(MIinjection_year, attname) == 0) { 
	miattget1(minc_fd, var_id, MIinjection_year, NC_INT, &injection_time.tm_year);	
	injection_time.tm_year -= 1900;
      }
      if(strcmp(MIinjection_month, attname) == 0) {
	miattget1(minc_fd, var_id, MIinjection_month, NC_INT, &injection_time.tm_mon);
	injection_time.tm_mon--;
      }
      if(strcmp(MIinjection_day, attname) == 0) 
	miattget1(minc_fd, var_id, MIinjection_day, NC_INT, &injection_time.tm_mday);
      
      if(strcmp(MIinjection_hour, attname) == 0) 
	miattget1(minc_fd, var_id, MIinjection_hour, NC_INT, &injection_time.tm_hour);
      
      if(strcmp(MIinjection_minute, attname) == 0) 
	miattget1(minc_fd, var_id, MIinjection_minute, NC_INT, &injection_time.tm_min);
      
      if(strcmp(MIinjection_seconds, attname) == 0) 
	miattget1(minc_fd, var_id, MIinjection_seconds, NC_INT, &injection_time.tm_sec);

      if(strcmp(MIinjection_dose, attname) == 0) 
	miattget1(minc_fd, var_id, MIinjection_dose, NC_FLOAT, &(mh->dosage));

      if(strcmp(MIdose_units, attname) == 0) 
	miattgetstr(minc_fd, var_id, MIdose_units, 32, mh->data_units);
    }
    mh->dose_start_time = mktime(&injection_time);
  }
  miclose(minc_fd);
}

void get_ecat_acquisition_info(char *file, Main_header *mh) {
  int minc_fd; /*minc file descriptor*/

 /* we first open the netcdf file*/
  minc_fd = miopen(file, NC_NOWRITE);

  if(mivar_exists(minc_fd, "ecat_acquisition")) {
    int var_id;
    int num_att;
    int i;
    char attname[NC_MAX_NAME+1];
    
    var_id = ncvarid(minc_fd, "ecat_acquisition");
    ncvarinq(minc_fd, var_id, NULL, NULL, NULL, NULL, &num_att);
    for(i = 0; i < num_att; i++) {
      ncattname(minc_fd,var_id,i,attname);  
      if(strcmp("septa_retracted", attname) == 0) {
	char buff[NC_MAX_NAME+1];
	
	miattgetstr(minc_fd, var_id, "septa_retracted", NC_MAX_NAME+1, buff);	
	if(strcmp(buff, MI_TRUE) == 0)
	  mh->septa_state = 1;
	if(strcmp(buff, MI_FALSE) == 0)
	  mh->septa_state = 0;
      }
    }
  }
  miclose(minc_fd);
}

void get_ecat_main_header_info(char *file, Main_header *mh) {
  int minc_fd;

 /* we first open the netcdf file*/
  minc_fd = miopen(file, NC_NOWRITE);

  if(mivar_exists(minc_fd, "ecat-main")) {
    int var_id;
    char buffer[NC_MAX_NAME+1];    


    var_id = ncvarid(minc_fd, "ecat-main");

    miattgetstr(minc_fd, var_id, "Magic_Number", 14, mh->magic_number);
   
    miattgetstr(minc_fd, var_id, "Original_Filename", 32, mh->original_file_name);
    
    miattgetstr(minc_fd, var_id, "System_Type", sizeof(buffer), buffer);
    sscanf(buffer, "%hd", &(mh->system_type));
    
    miattgetstr(minc_fd, var_id, "Serial_Number", 10, mh->serial_number);
    
    miattgetstr(minc_fd, var_id, "Scan_Start_Time", sizeof(buffer), buffer);
    sscanf(buffer, "%u", &(mh->scan_start_time));  
    
    miattgetstr(minc_fd, var_id, "Isotope_Name", 8, mh->isotope_code);
    
    miattgetstr(minc_fd, var_id, "Isotope_Halflife", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(mh->isotope_halflife));  
    
    miattgetstr(minc_fd, var_id, "Radiopharmaceutical", 32, mh->radiopharmaceutical);
    
    miattgetstr(minc_fd, var_id, "Gantry_Tilt", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(mh->gantry_tilt));
    
    miattgetstr(minc_fd, var_id, "Gantry_Rotation", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(mh->gantry_rotation));
    
    miattgetstr(minc_fd, var_id, "Bed_Elevation", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(mh->bed_elevation));
    
    miattgetstr(minc_fd, var_id, "Intrinsic_Tilt", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(mh->intrinsic_tilt));
    
    miattgetstr(minc_fd, var_id, "Wobble_Speed", sizeof(buffer), buffer);
    sscanf(buffer, "%hd", &(mh->wobble_speed));
    
    miattgetstr(minc_fd, var_id, "Transm_Source_Type", sizeof(buffer), buffer);
    sscanf(buffer, "%hd", &(mh->transm_source_type));
    
    miattgetstr(minc_fd, var_id, "Distance_Scanned", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(mh->distance_scanned));

    miattgetstr(minc_fd, var_id, "Transaxial_Fov", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(mh->transaxial_fov));
    
    miattgetstr(minc_fd, var_id, "Angular_Compression", sizeof(buffer), buffer);
    sscanf(buffer, "%hd", &(mh->angular_compression));
    
    miattgetstr(minc_fd, var_id, "Coin_Samp_Mode", sizeof(buffer), buffer);
    sscanf(buffer, "%hd", &(mh->coin_samp_mode));

    miattgetstr(minc_fd, var_id, "Axial_Samp_Mode", sizeof(buffer), buffer);
    sscanf(buffer, "%hd", &(mh->axial_samp_mode));

    miattgetstr(minc_fd, var_id, "Calibration_Factor", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(mh->calibration_factor));

    miattgetstr(minc_fd, var_id, "Study_Type", 12, mh->study_name);
      
    miattgetstr(minc_fd, var_id, "Patient_Id", 16, mh->patient_id);
    
    miattgetstr(minc_fd, var_id, "Patient_Name", 32, mh->patient_name);
    
    miattgetstr(minc_fd, var_id, "Patient_Sex", sizeof(buffer), buffer);
    sscanf(buffer, "%c", mh->patient_sex);
    
    miattgetstr(minc_fd, var_id, "Patient_Dexterity", sizeof(buffer), buffer);
    sscanf(buffer, "%c", mh->patient_dexterity);
    
    miattgetstr(minc_fd, var_id, "Patient_Age", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(mh->patient_age));
    
    miattgetstr(minc_fd, var_id, "Patient_Height", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(mh->patient_height));
    
    miattgetstr(minc_fd, var_id, "Patient_Weight", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(mh->patient_weight));
    
    miattgetstr(minc_fd, var_id, "Patient_Birth_Date", sizeof(buffer), buffer);
    sscanf(buffer, "%d", &(mh->patient_birth_date));

    miattgetstr(minc_fd, var_id, "Physician_Name", 32, mh->physician_name);
    
    miattgetstr(minc_fd, var_id, "Operator_Name", 32, mh->operator_name);
    
    miattgetstr(minc_fd, var_id, "Study_Description", 32, mh->study_description); 

   /*bugg -> in minc lib, acquisition should be replaced by acquisition*/
    miattgetstr(minc_fd, var_id, "Acquision_Type", sizeof(buffer), buffer);
    sscanf(buffer, "%hd", &(mh->acquisition_type));
    
    miattgetstr(minc_fd, var_id, "Patient_Orientation", sizeof(buffer), buffer);
    sscanf(buffer, "%hd", &(mh->patient_orientation));
    
    miattgetstr(minc_fd, var_id, "Facility_Name", 20, mh->facility_name);
        
    miattgetstr(minc_fd, var_id, "Lwr_Sctr_Thres", sizeof(buffer), buffer);
    sscanf(buffer, "%hd", &(mh->lwr_sctr_thres)); 
    
    miattgetstr(minc_fd, var_id, "Lwr_True_Thres", sizeof(buffer), buffer);
    sscanf(buffer, "%hd", &(mh->lwr_true_thres));
    
    miattgetstr(minc_fd, var_id, "Upr_True_Thres", sizeof(buffer), buffer);
    sscanf(buffer, "%hd", &(mh->upr_true_thres));
    
    miattgetstr(minc_fd, var_id, "User_Process_Code", 10, mh->user_process_code);
    
    miattgetstr(minc_fd, var_id, "Acquisition_Mode", sizeof(buffer), buffer);
    sscanf(buffer, "%hd", &(mh->acquisition_mode));
    
    miattgetstr(minc_fd, var_id, "Bin_Size", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(mh->bin_size));
    
    miattgetstr(minc_fd, var_id, "Branching_Fraction", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(mh->branching_fraction));
    
    miattgetstr(minc_fd, var_id, "Dose_Start_Time", sizeof(buffer), buffer);
    sscanf(buffer, "%u", &(mh->dose_start_time)); 
    
    miattgetstr(minc_fd, var_id, "Dosage", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(mh->dosage));
    
    miattgetstr(minc_fd, var_id, "Well_Counter_Corr_Factor", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(mh->well_counter_factor));
    
    miattgetstr(minc_fd, var_id, "Data_Units", 32, mh->data_units);
    
    miattgetstr(minc_fd, var_id, "Septa_State", sizeof(buffer), buffer);
    sscanf(buffer, "%hd", &(mh->septa_state));
  }
  miclose(minc_fd);
}

void get_ecat_subheader_info(char *file, Image_subheader *sh) {
  int minc_fd; /*minc file descriptor*/

 /* we first open the netcdf file*/
  minc_fd = miopen(file, NC_NOWRITE);

  if(mivar_exists(minc_fd, "ecat-subhdr")) {
    int var_id;
    char buffer[NC_MAX_NAME];

    var_id = ncvarid(minc_fd, "ecat-subhdr");
    
    miattgetstr(minc_fd, var_id, "Recon_Zoom", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(sh->recon_zoom));
    
    miattgetstr(minc_fd, var_id, "Filter_Code", sizeof(buffer), buffer);
    sscanf(buffer, "%hd", &(sh->filter_code));
    
    miattgetstr(minc_fd, var_id, "X_Resolution", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(sh->x_resolution));
    
    miattgetstr(minc_fd, var_id, "Y_Resolution", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(sh->y_resolution));
    
    miattgetstr(minc_fd, var_id, "Z_Resolution", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(sh->z_resolution));
    
    miattgetstr(minc_fd, var_id, "X_Rotation_Angle", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(sh->num_r_elements));
    
    miattgetstr(minc_fd, var_id, "Y_Rotation_Angle", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(sh->num_angles));
    
    miattgetstr(minc_fd, var_id, "Z_Rotation_Angle", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(sh->z_rotation_angle));
    
    miattgetstr(minc_fd, var_id, "Decay_Corr_Fctr", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(sh->decay_corr_fctr));
  
    miattgetstr(minc_fd, var_id, "Corrections_Applied", sizeof(buffer), buffer);
    sscanf(buffer, "%d", &(sh->processing_code));

    miattgetstr(minc_fd, var_id, "Gate_Duration", sizeof(buffer), buffer);
    sscanf(buffer, "%u", &(sh->gate_duration));
    
    miattgetstr(minc_fd, var_id, "R_Wave_Offset", sizeof(buffer), buffer);
    sscanf(buffer, "%d", &(sh->r_wave_offset));
    
    miattgetstr(minc_fd, var_id, "Num_Accepted_Beats", sizeof(buffer), buffer);
    sscanf(buffer, "%d", &(sh->num_accepted_beats));
    
    miattgetstr(minc_fd, var_id, "Filter_Cutoff_Frequency", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(sh->filter_cutoff_frequency));
    
    miattgetstr(minc_fd, var_id, "Filter_Dc_Component", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(sh->filter_resolution));
    
    miattgetstr(minc_fd, var_id, "Filter_Ramp_Slope", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(sh->filter_ramp_slope));
    
    miattgetstr(minc_fd, var_id, "Filter_Order", sizeof(buffer), buffer);
    sscanf(buffer, "%hd", &(sh->filter_order));
    
    miattgetstr(minc_fd, var_id, "Annotation", 40, sh->annotation);
    
    miattgetstr(minc_fd, var_id, "Da_X_Rotation_Angle", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(sh->mt_1_1));
    
    miattgetstr(minc_fd, var_id, "Da_Y_Rotation_Angle", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(sh->mt_1_2));
    
    miattgetstr(minc_fd, var_id, "Da_Z_Rotation_Angle", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(sh->mt_1_3));
    
    miattgetstr(minc_fd, var_id, "Da_X_Translation", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(sh->mt_2_1));
    
    miattgetstr(minc_fd, var_id, "Da_Y_Translation", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(sh->mt_2_2));
    
    miattgetstr(minc_fd, var_id, "Da_Z_Translation", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(sh->mt_2_3));
    
    miattgetstr(minc_fd, var_id, "Da_X_Scale_Factor", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(sh->mt_3_1));
    
    miattgetstr(minc_fd, var_id, "Da_Y_Scale_Factor", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(sh->mt_3_2));
    
    miattgetstr(minc_fd, var_id, "Da_Z_Scale_Factor", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(sh->mt_3_3));
    
    miattgetstr(minc_fd, var_id, "Rfilter_Cutoff", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(sh->rfilter_cutoff));
    
    miattgetstr(minc_fd, var_id, "Rfilter_Resolution", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(sh->rfilter_resolution));
    
    miattgetstr(minc_fd, var_id, "Rfilter_Code", sizeof(buffer), buffer);
    sscanf(buffer, "%hd", &(sh->rfilter_code));
    
    miattgetstr(minc_fd, var_id, "Rfilter_Order", sizeof(buffer), buffer);
    sscanf(buffer, "%hd", &(sh->rfilter_order));
    
    miattgetstr(minc_fd, var_id, "Zfilter_Cutoff", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(sh->zfilter_cutoff));
    
    miattgetstr(minc_fd, var_id, "Zfilter_Resolution", sizeof(buffer), buffer);
    sscanf(buffer, "%f", &(sh->zfilter_resolution));
    
    miattgetstr(minc_fd, var_id, "Zfilter_Code", sizeof(buffer), buffer);
    sscanf(buffer, "%hd", &(sh->zfilter_code));

    miattgetstr(minc_fd, var_id, "Zfilter_Order", sizeof(buffer), buffer);
    sscanf(buffer, "%hd", &(sh->zfilter_order));
  }
  miclose(minc_fd);
}

Main_header * init_main_header(char *file, minc_info *mi) {
  Main_header *mh = NULL;

  if((mh = (Main_header *)calloc(1, sizeof(Main_header))) == NULL) return NULL;

  /****************************************************************************
   * first, we filled the compulsory fields                                   *
   ****************************************************************************/

  /*software version 7*/
  mh->sw_version = 72;  

  /*well ecat is a format for PET data, so MRI data in the minc format will have the PetVolume flag in ecat ....*/
  mh->file_type = PetVolume; 
 
  mh->num_planes = mi->z_size;

  mh->num_frames = (mi->dim == 4)?mi->time_length:1;

  mh->num_gates = 1; /*not handled for now*/

  mh->num_bed_pos = 0; /*not handles for now*/

  /*inital bed position, I do the backward calculation of ecattominc - !! MIstart may not exist*/
  mh->init_bed_position = -mi->z_start/10.;
  mh->init_bed_position = (mi->z_step < 0)?-mi->z_start/10.:-(mi->z_step * (mi->z_size - 1) + mi->z_start)/10.;
  mh->bed_offset[0] = 0;

  /*for now, the distance_scanned is equal to the number of axial planes times the plane width*/
  mh->distance_scanned = fabs(mi->z_step) * (mi->z_size + 1)/10.0; /*cm*/

  mh->plane_separation = fabs(mi->z_step)/10.;

  mh->calibration_factor = 1; /*forced*/

  mh->calibration_units = 2; /*2 = processed, by default*/
 
  mh->calibration_units_label = 0; /*by default*/

  if(mi->dunit == NCIPERCC) {
    mh->calibration_units = 1; 
    mh->calibration_units_label = 1; 
  }
  mh->patient_sex[0] = 'U'; /*for now unknown*/

  mh->patient_dexterity[0] = 'U'; /*unknown for now*/;

  return mh;
}

Image_subheader * init_image_subheader(char *file, minc_info *mi, Main_header *mh) {
  Image_subheader *sh = NULL;
 
  if((sh = (Image_subheader *)calloc(1, sizeof(Image_subheader))) == NULL) return NULL;
 
  sh->data_type = SunShort; /*forced*/
  sh->num_dimensions = 3; /*forced*/

  sh->x_dimension = mi->x_size;
  sh->y_dimension = mi->y_size;
  sh->z_dimension = mi->z_size;
  sh->x_pixel_size = fabs(mi->x_step)/10.0; /*in cm*/
  sh->y_pixel_size = fabs(mi->y_step)/10.0;
  sh->z_pixel_size = fabs(mi->z_step)/10.0;
  sh->x_offset = 0;
  if(mi->x_start_flag)
    sh->x_offset = -(mi->x_start + (mi->x_size - 1.0)/2.0 * mi->x_step)/10.;
  sh->y_offset = 0;
  if(mi->y_start_flag)
    sh->y_offset = (mi->y_start + (mi->y_size - 1.0)/2.0 * mi->y_step)/10.;
  sh->z_offset = 0;
  return sh;
}
void write_ecat_frame(MatrixFile *mf, Image_subheader *sh, short *ptr,int fr, float min, float max) {
  MatrixData md = {0};

  md.matfile = mf;
  md.mat_type = mf->mhptr->file_type;
  md.data_type = sh->data_type;
  md.shptr = (caddr_t)sh;
  md.data_ptr = (caddr_t)ptr;
  md.xdim = sh->x_dimension;
  md.ydim = sh->y_dimension;
  md.zdim = sh->z_dimension;
    
  md.scale_factor = sh->scale_factor;
  md.pixel_size = sh->x_pixel_size;
  md.y_size = sh->y_pixel_size;
  md.z_size = sh->z_pixel_size;
  md.data_min = min;
  md.data_max = max;
  md.data_size = (sh->x_dimension) * (sh->y_dimension) * (sh->z_dimension) * sizeof(short int);
  matrix_write(mf,mat_numcod(fr + 1, 1, 1, 0, 0) , &md);
}


void usage_error(char *progname)
{
   (void) fprintf(stderr, 
             "\nUsage: %s [<options>] <infile.mnc> <outfile.v>\n", progname);
   (void) fprintf(stderr,   
               "       %s [-help]\n\n", progname);

   exit(EXIT_FAILURE);
}



