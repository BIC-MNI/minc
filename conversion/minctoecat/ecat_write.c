#include "ecat_write.h"
#include "machine_indep.h"

#define	MatBLKSIZE 512
#define	MatFirstDirBlk 2
#define NameLen 32
#define IDLen 16
#define OK 0
#define ERROR   -1
#define V7 70


char* dstypecode[NumDataSetTypes] =
	{ "u","s","i","a","n","pm","v8","v","p8","p","i8","S","S8","N", "FS"};

static char* magicNumber = "MATRIX";

struct	MatDir {
  int matnum;
  int strtblk;
  int endblk;
  int matstat; 
};

struct matdir { 
  int	nmats, nmax;
  struct MatDir *entry;
};

typedef struct matdirblk {
  int	nfree, nextblk, prvblk, nused ;
  struct MatDir matdir[31] ;
} MatDirBlk ;


FILE *mat_create(const char  *fname,Main_header *mhead);
FILE *mat_open( const char *fname, char *fmode);
int mat_write_main_header(FILE *fptr, Main_header *header);
int map_main_header(char *bufr,Main_header *header);
int mat_wblk(FILE *fptr,int blkno, char *bufr,int nblks);
MatDirList *mat_read_directory(MatrixFile *mptr);
MatDirBlk *mat_rdirblk(MatrixFile *file, int blknum);
int write_host_data(MatrixFile *mptr, int matnum, MatrixData *data);
int matrix_find(MatrixFile *matfile, int matnum,struct MatDir *matdir);
int mat_enter(FILE *fptr, Main_header *mhptr, int matnum, int nblks);
int insert_mdir(struct MatDir matdir, MatDirList *dirlist);
int mat_write_image_subheader(FILE *fptr, Main_header *mhptr,int blknum, Image_subheader *header);
int map_image_header(char *buf, Image_subheader *header);
int matrix_freelist(MatDirList *matdirlist);
void swaw( short *from, short *to, int length);
int mat_close(FILE *fptr);
int mat_rblk(FILE *fptr, int blkno, char *bufr, int nblks);

MatrixFile     *matrix_create(const char *fname, Main_header * proto_mhptr) {
  MatrixFile     *mptr = NULL;
  FILE           *fptr, *mat_create();
  
  matrix_errno = MAT_OK;
  matrix_errtxt[0] = '\0';
  
  fptr = mat_create(fname, proto_mhptr);
  if (!fptr) return( NULL );
  
  mptr = (MatrixFile *) calloc(1, sizeof(MatrixFile));
  if (!mptr) {
    fclose( fptr );
    return( NULL );
  }
  
  mptr->fptr = fptr;
  
  mptr->fname = (char *) malloc(strlen(fname) + 1);
  if (!mptr->fname) {
    free( mptr );
    fclose( fptr );
    return( NULL );
  }
  
  strcpy(mptr->fname, fname);
  mptr->mhptr = (Main_header *) malloc(sizeof(Main_header));
  if (!mptr->mhptr) {
    free( mptr->fname );
    free( mptr );
    fclose( fptr );
    return( NULL );
  }
  
  memcpy(mptr->mhptr, proto_mhptr, sizeof(Main_header));
  mptr->dirlist = mat_read_directory(mptr);
  if (!mptr->dirlist) {
    free( mptr->fname );
    free( mptr->mhptr );
    free( mptr );
    fclose( fptr );
    return( NULL );
  }
  return mptr;
}

FILE *mat_create(const char  *fname,Main_header *mhead) {
  FILE *fptr;
  int bufr[MatBLKSIZE/sizeof(int)];
  int ret;
  
  fptr = mat_open( fname, "w+");
  if (!fptr) return( NULL );
  ret = mat_write_main_header( fptr, mhead );
  if( ret != 0 ) {
    mat_close( fptr);
    return( NULL );
  }
  memset(bufr,0,MatBLKSIZE);
  bufr[0] = 31;
  bufr[1] = 2;
  
  ret = write_matrix_data(fptr,MatFirstDirBlk,1,(char*)bufr,SunLong);
  
  if( ret != 0 ) {
    mat_close( fptr);
    return( NULL );
  }
  return (fptr);
}

FILE *mat_open( const char *fname, char *fmode) {
  FILE *fopen(), *fptr;
  
  matrix_errno = MAT_OK;
  matrix_errtxt[0] = '\0';
  fptr = fopen(fname, fmode);
  return (fptr);
}

int mat_write_main_header(FILE *fptr, Main_header *header) {
  char bufr[MatBLKSIZE];
  
  map_main_header(bufr, header);
  return mat_wblk(fptr, 1, bufr, 1);	/* write main header at block 1 */
}

int map_main_header(char *bufr,Main_header *header) {

  int i = 0, j = 0;
  char mn[20];
  /* set magic number */
  sprintf(mn,"%s%d%s", magicNumber,header->sw_version,
	  dstypecode[header->file_type]);
  bufWrite(mn, bufr, &i, 14);
  
  /* copy buffer into struct */
  bufWrite(header->original_file_name, bufr, &i, NameLen);
  bufWrite_s(header->sw_version, bufr, &i);
  bufWrite_s(header->system_type, bufr, &i);
  bufWrite_s(header->file_type, bufr, &i);
  bufWrite(header->serial_number, bufr, &i, 10);
  bufWrite_u(header->scan_start_time, bufr, &i);
  bufWrite(header->isotope_code, bufr, &i, 8);
  bufWrite_f(header->isotope_halflife, bufr, &i);
  bufWrite(header->radiopharmaceutical, bufr, &i, NameLen);
  bufWrite_f(header->gantry_tilt, bufr, &i);
  bufWrite_f(header->gantry_rotation, bufr, &i);
  bufWrite_f(header->bed_elevation, bufr, &i);
  bufWrite_f(header->intrinsic_tilt, bufr, &i);
  bufWrite_s(header->wobble_speed, bufr, &i);
  bufWrite_s(header->transm_source_type, bufr, &i);
  bufWrite_f(header->distance_scanned, bufr, &i);
  bufWrite_f(header->transaxial_fov, bufr, &i);
  bufWrite_s(header->angular_compression, bufr, &i);
  bufWrite_s(header->coin_samp_mode, bufr, &i);
  bufWrite_s(header->axial_samp_mode, bufr, &i);
  bufWrite_f(header->calibration_factor, bufr, &i);
  bufWrite_s(header->calibration_units, bufr, &i);
  bufWrite_s(header->calibration_units_label, bufr, &i);
  bufWrite_s(header->compression_code, bufr, &i);
  bufWrite(header->study_name, bufr, &i, 12);
  bufWrite(header->patient_id, bufr, &i, IDLen);
  bufWrite(header->patient_name, bufr, &i, NameLen);
  bufWrite(header->patient_sex, bufr, &i, 1);
  bufWrite(header->patient_dexterity, bufr, &i, 1);
  bufWrite_f(header->patient_age, bufr, &i);
  bufWrite_f(header->patient_height, bufr, &i);
  bufWrite_f(header->patient_weight, bufr, &i);
  bufWrite_i(header->patient_birth_date, bufr, &i);
  bufWrite(header->physician_name, bufr, &i, NameLen);
  bufWrite(header->operator_name, bufr, &i, NameLen);
  bufWrite(header->study_description, bufr, &i, NameLen);
  bufWrite_s(header->acquisition_type, bufr, &i);
  bufWrite_s(header->patient_orientation, bufr, &i);
  bufWrite(header->facility_name, bufr, &i, 20);
  bufWrite_s(header->num_planes, bufr, &i);
  bufWrite_s(header->num_frames, bufr, &i);
  bufWrite_s(header->num_gates, bufr, &i);
  bufWrite_s(header->num_bed_pos, bufr, &i);
  bufWrite_f(header->init_bed_position, bufr, &i);
  for(j = 0; j < 15; j++)
  	bufWrite_f(header->bed_offset[j], bufr, &i);
  bufWrite_f(header->plane_separation, bufr, &i);
  bufWrite_s(header->lwr_sctr_thres, bufr, &i);
  bufWrite_s(header->lwr_true_thres, bufr, &i);
  bufWrite_s(header->upr_true_thres, bufr, &i);
  bufWrite(header->user_process_code, bufr, &i, 10);
  bufWrite_s(header->acquisition_mode, bufr, &i);
  bufWrite_f(header->bin_size, bufr, &i);
  bufWrite_f(header->branching_fraction, bufr, &i);
  bufWrite_u(header->dose_start_time, bufr, &i);
  bufWrite_f(header->dosage, bufr, &i);
  bufWrite_f(header->well_counter_factor, bufr, &i);
  bufWrite(header->data_units, bufr, &i, 32);
  bufWrite_s(header->septa_state, bufr, &i);
  return 1;
}

int mat_wblk(FILE *fptr,int blkno, char *bufr,int nblks) {
  int err;
  
  matrix_errno = MAT_OK;
  matrix_errtxt[0] = '\0';
  
  /* seek to position in file */
  err = fseek(fptr, (blkno - 1) * MatBLKSIZE, 0);
  if (err) return (ERROR);
  
  /* write matrix data */
  err = fwrite(bufr, 1, nblks * MatBLKSIZE, fptr);
  if (err == -1) return (ERROR);
  if (err != nblks * MatBLKSIZE) {
    matrix_errno = MAT_WRITE_ERROR;
    return (ERROR);
  }
  return (0);
}

MatDirList *mat_read_directory(MatrixFile *mptr) {
  struct MatDir   matdir;
  MatDirList     *dirlist;
  MatDirBlk      *matdirblk;
  int             i, blknum;
  
  matrix_errno = MAT_OK;
  matrix_errtxt[0] = '\0';
  
  dirlist = (MatDirList *) calloc(1, sizeof(MatDirList));
  if (dirlist == NULL) return (NULL);

  blknum = MatFirstDirBlk;
  do {
    matdirblk = mat_rdirblk(mptr, blknum);
    if (matdirblk == NULL) {
      free(dirlist);
      return (NULL);
    }
    for (i = 0; i < matdirblk->nused; i++) {
      matdir.matnum = matdirblk->matdir[i].matnum;
      matdir.strtblk = matdirblk->matdir[i].strtblk;
      matdir.endblk = matdirblk->matdir[i].endblk;
      matdir.matstat = matdirblk->matdir[i].matstat;
      insert_mdir(matdir, dirlist);
    }
    blknum = matdirblk->nextblk;
    free(matdirblk);
  }
  while (blknum != MatFirstDirBlk);
  return (dirlist);
}

MatDirBlk *mat_rdirblk(MatrixFile *file, int blknum) {
  MatDirBlk      *matdirblk;
  int             i, j, err, ndirs;
  int             dirbufr[MatBLKSIZE / 4];
  FILE           *fptr = file->fptr;
  
  matrix_errno = MAT_OK;
  matrix_errtxt[0] = '\0';
  
  matdirblk = (MatDirBlk *) malloc(MatBLKSIZE);
  if (matdirblk == NULL)
    return (NULL);
  
  err = read_matrix_data(fptr, blknum, 1, (char *) dirbufr, SunLong);
  
  if (err == ERROR) {
    free(matdirblk);
    return (NULL);
  }
  matdirblk->nfree = dirbufr[0];
  matdirblk->nextblk = dirbufr[1];
  matdirblk->prvblk = dirbufr[2];
  matdirblk->nused = dirbufr[3];
  
  if (matdirblk->nused > 31) {
    matrix_errno = MAT_INVALID_DIRBLK;
    free(matdirblk);
    return (NULL);
  }
  ndirs = (MatBLKSIZE / 4 - 4) / 4;
  for (i = 0; i < ndirs; i++) {
    matdirblk->matdir[i].matnum = 0;
    matdirblk->matdir[i].strtblk = 0;
    matdirblk->matdir[i].endblk = 0;
    matdirblk->matdir[i].matstat = 0;
  }
  
  for (i = 0; i < matdirblk->nused; i++) {
    j = i + 1;
    matdirblk->matdir[i].matnum = dirbufr[j * 4 + 0];
    matdirblk->matdir[i].strtblk = dirbufr[j * 4 + 1];
    matdirblk->matdir[i].endblk = dirbufr[j * 4 + 2];
    matdirblk->matdir[i].matstat = dirbufr[j * 4 + 3];
  }
  return (matdirblk);
}

int matrix_write(MatrixFile *mptr, int matnum,MatrixData *data) {
  int   slice ;
  
  matrix_errno = MAT_OK;
  matrix_errtxt[0] = '\0';
  if (mptr == NULL) matrix_errno = MAT_READ_FROM_NILFPTR ;
  else if (mptr->mhptr == NULL) matrix_errno = MAT_NOMHD_FILE_OBJECT ;
  else if (data->shptr == NULL) matrix_errno = MAT_NIL_SHPTR ;
  else if (data->data_ptr == NULL) matrix_errno = MAT_NIL_DATA_PTR ;
  if (matrix_errno != OK) return (ERROR) ;
  return  write_host_data(mptr, matnum, data);
} 

int write_host_data(MatrixFile *mptr, int matnum, MatrixData *data) {
  struct MatDir matdir, dir_entry ;
  Image_subheader *imagesub ;
  int   status, blkno, nblks ;
  
  matrix_errno = MAT_OK;
  matrix_errtxt[0] = '\0';
  status = OK ;
  nblks = (data->data_size+511)/512;
  
  if (matrix_find(mptr, matnum, &matdir) == ERROR) {
    blkno = mat_enter(mptr->fptr, mptr->mhptr, matnum, nblks) ;
    if( blkno == ERROR ) return( ERROR );
    dir_entry.matnum = matnum ;
    dir_entry.strtblk = blkno ;
    
    /*le 24 Avril 2001, j'ai enleve le -1*/
    dir_entry.endblk = dir_entry.strtblk + nblks /*- 1*/ ;
    dir_entry.matstat = 1 ;
    insert_mdir(dir_entry, mptr->dirlist) ;
    matdir = dir_entry ;
  }
  
  imagesub = (Image_subheader *) data->shptr ;
  if (imagesub == NULL) {
    imagesub = (Image_subheader *) calloc(1, MatBLKSIZE);
    data->shptr = (caddr_t)imagesub;
  }                                                     /* use MatrixData info */
  imagesub->x_pixel_size = data->pixel_size;
  imagesub->y_pixel_size = data->y_size;
  imagesub->z_pixel_size = data->z_size;
  imagesub->num_dimensions = 3;
  imagesub->x_dimension = data->xdim;
  imagesub->y_dimension = data->ydim;
  imagesub->z_dimension = data->zdim;
  imagesub->image_max = (int)(data->data_max/data->scale_factor);
  imagesub->image_min = (int)(data->data_min/data->scale_factor);
  imagesub->scale_factor = data->scale_factor;
  imagesub->data_type = data->data_type;
  if( mat_write_image_subheader(mptr->fptr,mptr->mhptr,matdir.strtblk, imagesub) == ERROR ) return( ERROR );
  status = write_matrix_data(mptr->fptr, matdir.strtblk+1, nblks, data->data_ptr, imagesub->data_type) ;
  if( status == ERROR ) return( ERROR );
  return(status) ;
}

int matrix_find(MatrixFile *matfile, int matnum,struct MatDir *matdir) {
  MatDirNode    *node ;
  
  matrix_errno = MAT_OK;
  matrix_errtxt[0] = '\0';
  if (matfile == NULL) return(ERROR) ;
  if (matfile->dirlist == NULL) return(ERROR) ;   
  node = matfile->dirlist->first ;
  while (node != NULL)
    {
      if (node->matnum == matnum)
	{
	  matdir->matnum = node->matnum ;
	  matdir->strtblk = node->strtblk ;
	  matdir->endblk = node->endblk ;
	  matdir->matstat = node->matstat ;
	  break ;
	}
      node = node->next ;
    }
  if (node != NULL) return(OK) ;
  else return(ERROR) ;
}

int mat_enter(FILE *fptr, Main_header *mhptr, int matnum, int nblks) {
  int dirblk, dirbufr[128], i, nxtblk, busy, oldsize;
  short sw_version = mhptr->sw_version;
  
  matrix_errno = MAT_OK;
  matrix_errtxt[0] = '\0';
  dirblk = MatFirstDirBlk;
  if( fseek(fptr, 0, 0) ) return( ERROR );
  /*
   * nfs locks are very time consuming lockf( fileno(fptr), F_LOCK, 0);
   */
  if (read_matrix_data(fptr, dirblk, 1, (char *) dirbufr, SunLong) == ERROR) return (ERROR);
  
  busy = 1;
  while (busy) {
    nxtblk = dirblk + 1;
    for (i = 4; i < 128; i += 4) {
      if (dirbufr[i] == 0) {
	busy = 0;
	break;
      } else if (dirbufr[i] == matnum) {
	oldsize = dirbufr[i + 2] - dirbufr[i + 1] + 1;
	if (oldsize < nblks) {
	  dirbufr[i] = 0xFFFFFFFF;
	  write_matrix_data(fptr, dirblk, 1, (char*)dirbufr, SunLong);
	  nxtblk = dirbufr[i + 2] + 1;
	} else {
	  nxtblk = dirbufr[i + 1];
	  dirbufr[0]++;
	  dirbufr[3]--;
	  busy = 0;
	  break;
	}
      } else {
	nxtblk = dirbufr[i + 2] + 1;
      }
    }
    if (!busy) break;
    if (dirbufr[1] != MatFirstDirBlk) {
      dirblk = dirbufr[1];
      if (read_matrix_data(fptr, dirblk, 1, (char *) dirbufr, SunLong) == ERROR) return (ERROR);
    } else {
      dirbufr[1] = nxtblk;
      if (write_matrix_data(fptr, dirblk, 1, (char *) dirbufr, SunLong) == ERROR) return (ERROR);
      
      dirbufr[0] = 31;
      dirbufr[1] = MatFirstDirBlk;
      dirbufr[2] = dirblk;
      dirbufr[3] = 0;
      dirblk = nxtblk;
      for (i = 4; i < 128; i++)
	dirbufr[i] = 0;
    }
  }
  dirbufr[i] = matnum;
  dirbufr[i + 1] = nxtblk;
  dirbufr[i + 2] = nxtblk + nblks;
  dirbufr[i + 3] = 1;
  dirbufr[0]--;
  dirbufr[3]++;
  if (write_matrix_data(fptr, dirblk, 1, (char*)dirbufr, SunLong) == ERROR) return (ERROR);
  
  if( fseek(fptr, 0, 0) ) return( ERROR );
  /*
   * nfs locks are very time consuming lockf( fileno(fptr), F_UNLOCK, 0);
   */
  return (nxtblk);
}

int insert_mdir(struct MatDir matdir, MatDirList *dirlist) {
  MatDirNode    *node ;
  
  matrix_errno = MAT_OK;
  matrix_errtxt[0] = '\0';
  if (dirlist == NULL)
    {
      dirlist = (MatDirList *) malloc(sizeof(MatDirList)) ;
      if (dirlist == NULL) return(ERROR) ;
      dirlist->nmats = 0 ;
      dirlist->first = NULL ;
      dirlist->last = NULL ;
    }
  node = (MatDirNode *) malloc(sizeof(MatDirNode)) ;
  if (node == NULL) return(ERROR) ;
  
  node->matnum = matdir.matnum ;
  node->strtblk = matdir.strtblk ;
  node->endblk = matdir.endblk ;
  node->matstat = matdir.matstat;
  node->next = NULL ;
  
  if (dirlist->first == NULL)     /* if list was empty, add first node */
    {
      dirlist->first = node ;
      dirlist->last = node ;
      dirlist->nmats = 1 ;
    }
  else
    {
      (dirlist->last)->next = node ;
      dirlist->last = node ;
      ++(dirlist->nmats) ;
    }
  return OK;
}

int mat_write_image_subheader(FILE *fptr, Main_header *mhptr,int blknum, Image_subheader *header) {
  char buf[MatBLKSIZE];
  map_image_header(buf, header);
  return mat_wblk(fptr, blknum, buf, 1);
}

int map_image_header(char *buf, Image_subheader *header) {
  int i = 0;
  bufWrite_s(header->data_type, buf, &i);
  bufWrite_s(header->num_dimensions, buf, &i);
  bufWrite_s(header->x_dimension, buf, &i);
  bufWrite_s(header->y_dimension, buf, &i);
  bufWrite_s(header->z_dimension, buf, &i);
  bufWrite_f(header->z_offset, buf, &i);
  bufWrite_f(header->x_offset, buf, &i);
  bufWrite_f(header->y_offset, buf, &i);
  bufWrite_f(header->recon_zoom, buf, &i);
  bufWrite_f(header->scale_factor, buf, &i);
  bufWrite_s(header->image_min, buf, &i);
  bufWrite_s(header->image_max, buf, &i);
  bufWrite_f(header->x_pixel_size, buf, &i);
  bufWrite_f(header->y_pixel_size, buf, &i);
  bufWrite_f(header->z_pixel_size, buf, &i);
  bufWrite_u(header->frame_duration, buf, &i);
  bufWrite_u(header->frame_start_time, buf, &i);
  bufWrite_s(header->filter_code, buf, &i);
  bufWrite_f(header->x_resolution, buf, &i);
  bufWrite_f(header->y_resolution, buf, &i);
  bufWrite_f(header->z_resolution, buf, &i);
  bufWrite_f(header->num_r_elements, buf, &i);
  bufWrite_f(header->num_angles, buf, &i);
  bufWrite_f(header->z_rotation_angle, buf, &i);
  bufWrite_f(header->decay_corr_fctr, buf, &i);
  bufWrite_i(header->processing_code, buf, &i);
  bufWrite_u(header->gate_duration, buf, &i);
  bufWrite_i(header->r_wave_offset, buf, &i);
  bufWrite_i(header->num_accepted_beats, buf, &i);
  bufWrite_f(header->filter_cutoff_frequency, buf, &i);
  bufWrite_f(header->filter_resolution, buf, &i);
  bufWrite_f(header->filter_ramp_slope, buf, &i);
  bufWrite_s(header->filter_order, buf, &i);
  bufWrite_f(header->filter_scatter_fraction, buf, &i);
  bufWrite_f(header->filter_scatter_slope, buf, &i);
  bufWrite(header->annotation, buf, &i, 40);
  bufWrite_f(header->mt_1_1, buf, &i);
  bufWrite_f(header->mt_1_2, buf, &i);
  bufWrite_f(header->mt_1_3, buf, &i);
  bufWrite_f(header->mt_2_1, buf, &i);
  bufWrite_f(header->mt_2_2, buf, &i);
  bufWrite_f(header->mt_2_3, buf, &i);
  bufWrite_f(header->mt_3_1, buf, &i);
  bufWrite_f(header->mt_3_2, buf, &i);
  bufWrite_f(header->mt_3_3, buf, &i);
  bufWrite_f(header->rfilter_cutoff, buf, &i);
  bufWrite_f(header->rfilter_resolution, buf, &i);
  bufWrite_s(header->rfilter_code, buf, &i);
  bufWrite_s(header->rfilter_order, buf, &i);
  bufWrite_f(header->zfilter_cutoff, buf, &i);
  bufWrite_f(header->zfilter_resolution, buf, &i);
  bufWrite_s(header->zfilter_code, buf, &i);
  bufWrite_s(header->zfilter_order, buf, &i);
  bufWrite_f(header->mt_1_4, buf, &i);
  bufWrite_f(header->mt_2_4, buf, &i);
  bufWrite_f(header->mt_3_4, buf, &i);
  bufWrite_s(header->scatter_type, buf, &i);
  bufWrite_s(header->recon_type, buf, &i);
  bufWrite_s(header->recon_views, buf, &i);
  return 1;
}

int matrix_close(MatrixFile *mptr) {
  int status = OK;
  matrix_errno = MAT_OK;
  if (mptr->fname) strcpy(matrix_errtxt,mptr->fname);
  else matrix_errtxt[0] = '\0';
  if (mptr == NULL) return status;
  if (mptr->mhptr != NULL) free(mptr->mhptr) ;
  if (mptr->dirlist != NULL) matrix_freelist(mptr->dirlist) ;
  if (mptr->fptr) status = fclose(mptr->fptr);
  if (mptr->fname) free(mptr->fname);
  free(mptr);
  return status;
}

int matrix_freelist(MatDirList *matdirlist) {
  MatDirNode    *node, *next ;
  
  if (matdirlist == NULL) return OK;
  if (matdirlist->first != NULL) {
    node = matdirlist->first ;
    do {
      next = node->next ;
      free(node) ;
      node = next ;
    }
    while(next != NULL) ;
  }
  free(matdirlist) ;
  return OK;
}


int mat_numcod(int frame, int plane, int gate, int data, int bed) {
  return ((frame)|((bed&0xF)<<12)|((plane&0xFF)<<16)|(((plane&0x300)>>8)<<9)|
	  ((gate&0x3F)<<24)|((data&0x3)<<30)|((data&0x4)<<9));
}

void swaw( short *from, short *to, int length) {
  short int temp;
  int i;
  
  for (i=0;i<length; i+=2)
    {  temp = from[i+1];
    to[i+1]=from[i];
    to[i] = temp;
    }
}

int mat_close(FILE *fptr) {
  matrix_errno = MAT_OK;
  matrix_errtxt[0] = '\0';
  return fclose( fptr);
}

int mat_rblk(FILE *fptr, int blkno, char *bufr, int nblks) {
  int err;
  
  matrix_errno = MAT_OK;
  matrix_errtxt[0] = '\0';
  if( fseek( fptr, (blkno-1)*MatBLKSIZE, 0) ) return( ERROR );
  err = fread( bufr, 1, nblks*MatBLKSIZE, fptr);
  if( err == ERROR ) {
    return( ERROR );
  }
  /*  some applications write pixel instead of block count
      ==> check if byte count less than (nblks-1) (M. Sibomana 23-oct-1997) */
  else if( err < (nblks-1)*MatBLKSIZE ) {
    matrix_errno = MAT_READ_ERROR;
    return( ERROR );
  }
  return( 0 );
}
