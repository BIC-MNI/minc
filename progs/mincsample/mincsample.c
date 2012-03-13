/* mincsample.c
 * 
 * Generate samplings from MINC files
 * 
 * Andrew Janke - a.janke@gmail.com
 * Mark Griffin
 * 
 * Copyright Andrew Janke and Mark Griffin, McConnell Brain Imaging Centre
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies.  The
 * author and the University make no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 */

#include <config.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <ParseArgv.h>
#include <time_stamp.h>
#include <voxel_loop.h>
#include "mt19937ar.h"

#ifndef  FALSE
#define  FALSE  0
#endif

#ifndef  TRUE
#define  TRUE   1
#endif

#define WORLD_NDIMS 3
#define DEFAULT_INT -1

/* typedefs */
typedef enum { SAMPLE_ALL, SAMPLE_RND } Sample_enum;
typedef enum { OUTPUT_ASCII, OUTPUT_DOUBLE } Output_enum;

typedef struct {
   Sample_enum sample_type;

   /* input parameters */
   int      masking;
   double   mask_val;
   int      mask_idx;

   /* sampling */
   int      rand_samples;
   int      max_samples;

   /* output parameters */
   int      sample_mask;
   int      sample_mask_idx;

   Output_enum output_type;
   int      output_coords;
   FILE    *outFP;
   } Loop_Data;

/* function prototypes */
void     count_points(void *caller_data, long num_voxels, int input_num_buffers,
                      int input_vector_length, double *input_data[],
                      int output_num_buffers, int output_vector_length,
                      double *output_data[], Loop_Info * loop_info);
void     get_points(void *caller_data, long num_voxels, int input_num_buffers,
                    int input_vector_length, double *input_data[], int output_num_buffers,
                    int output_vector_length, double *output_data[],
                    Loop_Info * loop_info);
void     write_data(FILE * fp, double value, Output_enum ot);
void     get_minc_attribute(int mincid, char *varname, char *attname,
                            int maxvals, double vals[]);
int      get_minc_ndims(int mincid);
void     find_minc_spatial_dims(int mincid, int space_to_dim[], int dim_to_space[]);
void     get_minc_voxel_to_world(int mincid,
                                 double voxel_to_world[WORLD_NDIMS][WORLD_NDIMS + 1]);
void     normalize_vector(double vector[]);
void     transform_coord(double out_coord[],
                         double transform[WORLD_NDIMS][WORLD_NDIMS + 1],
                         double in_coord[]);

/* global vars for printing coordinates */
int      space_to_dim[WORLD_NDIMS] = { -1, -1, -1 };
int      dim_to_space[MAX_VAR_DIMS];
int      file_ndims = 0;
double   voxel_to_world[WORLD_NDIMS][WORLD_NDIMS + 1];

/* Argument variables and table */
static int verbose = FALSE;
static int quiet = FALSE;
static int clobber = FALSE;
static int max_buffer = 4 * 1024;
static char *mask_fname = NULL;
static char *sample_fname = NULL;
static char *out_fname = NULL;
static int append_output = FALSE;
static int rand_seed = DEFAULT_INT;
static Loop_Data md = {
   SAMPLE_ALL,
   FALSE, 1.0, 0,
   0, 0,
   FALSE, 0,
   OUTPUT_ASCII, FALSE,
   NULL
   };

static ArgvInfo argTable[] = {
   {NULL, ARGV_HELP, (char *)NULL, (char *)NULL,
    "General options:"},
   {"-verbose", ARGV_CONSTANT, (char *)TRUE, (char *)&verbose,
    "print out extra information."},
   {"-quiet", ARGV_CONSTANT, (char *)TRUE, (char *)&quiet,
    "be very quiet."},
   {"-clobber", ARGV_CONSTANT, (char *)TRUE, (char *)&clobber,
    "clobber existing files."},
   {"-max_buffer", ARGV_INT, (char *)1, (char *)&max_buffer,
    "maximum size of buffers (in kbytes)"},
   {"-mask", ARGV_STRING, (char *)1, (char *)&mask_fname,
    "select voxels within the specified mask"},
   {"-mask_val", ARGV_FLOAT, (char *)1, (char *)&(md.mask_val),
    "mask value to use"},

   {NULL, ARGV_HELP, (char *)NULL, (char *)NULL,
    "\nSampling Types:"},
   {"-all", ARGV_CONSTANT, (char *)SAMPLE_ALL, (char *)&md.sample_type,
    "sample all the data (Default)"},
   {"-random_seed", ARGV_INT, (char *)1, (char *)&rand_seed,
    "Random seed to use (use to get reproducible runs) Default: use tv_usec"},
   {"-random_samples", ARGV_INT, (char *)1, (char *)&md.rand_samples,
    "take # random samples from the input data"},

   {NULL, ARGV_HELP, (char *)NULL, (char *)NULL,
    "\nOutput Options:"},
   {"-sample", ARGV_STRING, (char *)1, (char *)&sample_fname,
    "Output a <mask.mnc> file of chosen points"},
   {"-outfile", ARGV_STRING, (char *)1, (char *)&out_fname,
    "<file> for output data (Default: stdout)"},
   {"-append", ARGV_CONSTANT, (char *)TRUE, (char *)&append_output,
    "append output data to existing file"},
   {"-ascii", ARGV_CONSTANT, (char *)OUTPUT_ASCII, (char *)&md.output_type,
    "Write out data as ascii strings (default)"},
   {"-double", ARGV_CONSTANT, (char *)OUTPUT_DOUBLE, (char *)&md.output_type,
    "Write out data as double precision floating-point values"},
   {"-coords", ARGV_CONSTANT, (char *)TRUE, (char *)&md.output_coords,
    "Write out world co-ordinates as well as values"},

   {NULL, ARGV_HELP, NULL, NULL, ""},
   {NULL, ARGV_END, NULL, NULL, NULL}
   };

int main(int argc, char *argv[])
{
   char   **infiles;
   char    *outfiles[1];
   int      n_infiles, n_outfiles;
   char    *arg_string;
   Loop_Options *loop_opts;
   int      mincid;
   struct timeval timer;
   int      i;

   /* Save time stamp and args */
   arg_string = time_stamp(argc, argv);

   /* get arguments */
   if(ParseArgv(&argc, argv, argTable, 0) || (argc < 2)){
      fprintf(stderr, "\nUsage: %s [options] <in1.mnc> <in2.mnc> ... \n", argv[0]);
      fprintf(stderr, "       %s -help\n\n", argv[0]);
      exit(EXIT_FAILURE);
      }

   /* check arguments */
   if(md.rand_samples != 0){
      md.sample_type = SAMPLE_RND;
      if(md.rand_samples < 0){
         fprintf(stderr, "%s: -rand_samples (%d) must be greater than 0\n\n",
                 argv[0], md.rand_samples);
         exit(EXIT_FAILURE);
         }
      }

   /* check arguments */
   if(rand_seed != DEFAULT_INT && rand_seed < 0){
      fprintf(stderr, "%s: -rand_seed (%d) must be 0 or greater\n\n", argv[0], rand_seed);
      exit(EXIT_FAILURE);
      }

   /* get infile names */
   n_infiles = argc - 1;
   infiles = (char **)malloc(sizeof(char *) * (n_infiles + 1));   /* + 1 for mask */
   for(i = 0; i < n_infiles; i++){
      infiles[i] = argv[i + 1];
      }
   if(mask_fname != NULL){
      infiles[n_infiles] = mask_fname;
      md.masking = TRUE;
      md.mask_idx = n_infiles;
      n_infiles++;
      }

   /* check for the infile(s) */
   for(i = 0; i < n_infiles; i++){
      if(access(infiles[i], F_OK) != 0){
         fprintf(stderr, "%s: Couldn't find input file: %s\n\n", argv[0], infiles[i]);
         exit(EXIT_FAILURE);
         }
      }

   /* set up and check for voxel_loop outfiles */
   if(sample_fname != NULL){
      if(access(sample_fname, F_OK) == 0 && !clobber){
         fprintf(stderr, "%s: %s exists, use -clobber to overwrite\n\n", argv[0],
                 sample_fname);
         exit(EXIT_FAILURE);
         }
      md.sample_mask = TRUE;
      md.sample_mask_idx = 0;
      n_outfiles = 1;
      outfiles[0] = sample_fname;
      }
   else {
      n_outfiles = 0;
      }

   /* set up data outfile */
   if(out_fname == NULL || strcmp(out_fname, "-") == 0){
      md.outFP = stdout;
      }
   else {
      if(!append_output && access(out_fname, F_OK) == 0 && !clobber){
         fprintf(stderr, "%s: %s exists, use -clobber to overwrite\n\n", argv[0],
                 out_fname);
         exit(EXIT_FAILURE);
         }

      if((md.outFP = fopen(out_fname, (append_output) ? "a" : "w")) == NULL){
         fprintf(stderr, "%s:  problems opening %s\n", argv[0], out_fname);
         exit(EXIT_FAILURE);
         }
      }

   /* Get some information from the first file for printing co-ordinates */
   mincid = miopen(infiles[0], NC_NOWRITE | 0x8000);
   file_ndims = get_minc_ndims(mincid);
   find_minc_spatial_dims(mincid, space_to_dim, dim_to_space);
   get_minc_voxel_to_world(mincid, voxel_to_world);

   /* set up voxel loop options */
   loop_opts = create_loop_options();
   set_loop_verbose(loop_opts, FALSE);
   set_loop_clobber(loop_opts, clobber);
   set_loop_buffer_size(loop_opts, (long)1024 * max_buffer);

   /* set up random sampling if required */
   if(md.sample_type == SAMPLE_RND){
      void    *tmp = NULL;             /* for gettimeofday */

      /* get max number of samples */
      voxel_loop(1, (md.masking) ? &mask_fname : infiles, 0, NULL, NULL,
                 loop_opts, count_points, (void *)&md);
      if(verbose){
         fprintf(stderr, " | Got max # of points: %d\n", md.max_samples);
         }

      if(md.rand_samples > md.max_samples){
         fprintf(stderr, "%s: -rand_samples (%d) must be less than max samples (%d)\n\n",
                 argv[0], md.rand_samples, md.max_samples);
         exit(EXIT_FAILURE);
         }

      /* initialise random number generator */
      if(rand_seed == DEFAULT_INT){
         gettimeofday(&timer, tmp);
         rand_seed = timer.tv_usec;
         }

      if(verbose){
         fprintf(stderr, " | Using random seed:   %d\n", rand_seed);
         }

      init_genrand((unsigned long)rand_seed);
      }

   /* do the sampling */
   voxel_loop(n_infiles, infiles, n_outfiles, outfiles, arg_string,
              loop_opts, get_points, (void *)&md);

   /* tidy up */
   fclose(md.outFP);
   free_loop_options(loop_opts);

   return (EXIT_SUCCESS);
   }

/* get points from file(s), write out to an input FP */
void get_points(void *caller_data, long num_voxels, int input_num_buffers,
                int input_vector_length, double *input_data[], int output_num_buffers,
                int output_vector_length, double *output_data[], Loop_Info * loop_info)
{
   Loop_Data *md = (Loop_Data *) caller_data;
   int      i, idim, ivox;
   int      n_infiles;
   int      do_sample;
   double   mask_value;
   int      dim_index;
   long     index[MAX_VAR_DIMS];
   double   voxel_coord[WORLD_NDIMS];
   double   world_coord[WORLD_NDIMS];

   /* shut the compiler up */
   (void)output_num_buffers;
   (void)output_vector_length;

   n_infiles = (md->masking) ? input_num_buffers - 1 : input_num_buffers;

   /* for each voxel */
   for(ivox = 0; ivox < num_voxels * input_vector_length; ivox++){

      /* nasty way that works for masking or not */
      mask_value = 0;
      if(!md->masking ||
         (md->masking && fabs(input_data[md->mask_idx][ivox] - md->mask_val) < 0.5)){

         /* flag to sample */
         do_sample = FALSE;

         switch (md->sample_type){
         case SAMPLE_ALL:
            do_sample = TRUE;
            mask_value = 1.0;
            break;

         case SAMPLE_RND:
            /* if this voxel 'qualifies', write out data */
            if(genrand_res53() < ((double)md->rand_samples / md->max_samples)){
               md->rand_samples--;
               do_sample = TRUE;
               mask_value = 1.0;
               }

            md->max_samples--;
            break;

         default:
            fprintf(stderr, "ERROR - Sample type is undefined (%d)\n", md->sample_type);
            exit(EXIT_FAILURE);
            }

         /* now write out the data */
         if(do_sample){

            /* get and convert voxel to world coordinates */
            if(md->output_coords){
               get_info_voxel_index(loop_info, ivox, file_ndims, index);
               for(idim = 0; idim < WORLD_NDIMS; idim++){
                  dim_index = space_to_dim[idim];
                  if(dim_index >= 0){
                     voxel_coord[idim] = index[dim_index];
                     }
                  }
               transform_coord(world_coord, voxel_to_world, voxel_coord);
               }

            switch (md->output_type){
            case OUTPUT_ASCII:
               if(md->output_coords){
                  fprintf(md->outFP, "%.20g\t%.20g\t%.20g\t", world_coord[0],
                          world_coord[1], world_coord[2]);
                  }

               for(i = 0; i < n_infiles; i++){
                  fprintf(md->outFP, "%.20g\t", input_data[i][ivox]);
                  }
               fprintf(md->outFP, "\n");
               break;

            case OUTPUT_DOUBLE:
               if(md->output_coords){
                  fwrite(&(world_coord[0]), sizeof(double), 1, md->outFP);
                  fwrite(&(world_coord[1]), sizeof(double), 1, md->outFP);
                  fwrite(&(world_coord[2]), sizeof(double), 1, md->outFP);
                  }

               for(i = 0; i < n_infiles; i++){
                  fwrite(&(input_data[i][ivox]), sizeof(double), 1, md->outFP);
                  }

               break;

            default:
               fprintf(stderr, "ERROR - Output type is undefined (%d)\n",
                       md->output_type);
               exit(EXIT_FAILURE);
               }
            }

         }

      /* output sampling mask */
      if(md->sample_mask){
         output_data[md->sample_mask_idx][ivox] = mask_value;
         }
      }

   }

/* count points that are within a mask */
void count_points(void *caller_data, long num_voxels, int input_num_buffers,
                  int input_vector_length, double *input_data[],
                  int output_num_buffers, int output_vector_length,
                  double *output_data[], Loop_Info * loop_info)
{
   Loop_Data *md = (Loop_Data *) caller_data;
   int      ivox;

   /* shut the compiler up */
   (void)input_num_buffers;
   (void)output_num_buffers;
   (void)output_vector_length;
   (void)output_data;
   (void)loop_info;

   for(ivox = 0; ivox < num_voxels * input_vector_length; ivox++){
      if(md->masking){
         if(fabs(input_data[0][ivox] - md->mask_val) < 0.5){
            md->max_samples++;
            }
         }
      else {
         md->max_samples++;
         }
      }
   }

inline void write_data(FILE * fp, double value, Output_enum ot)
{
   switch (ot){
   case OUTPUT_ASCII:
      fprintf(fp, "%.20g\n", value);
      break;

   case OUTPUT_DOUBLE:
      fwrite(&(value), sizeof(double), 1, fp);
      break;

   default:
      fprintf(stderr, "ERROR - Output type is undefined (%d)\n", ot);
      exit(EXIT_FAILURE);
      }
   }

void normalize_vector(double vector[])
{
   int      idim;
   double   magnitude;

   magnitude = 0.0;
   for(idim = 0; idim < WORLD_NDIMS; idim++){
      magnitude += (vector[idim] * vector[idim]);
      }
   magnitude = sqrt(magnitude);
   if(magnitude > 0.0){
      for(idim = 0; idim < WORLD_NDIMS; idim++){
         vector[idim] /= magnitude;
         }
      }
   }

/* Transforms a coordinate through a linear transform -- from mincstats */
void transform_coord(double out_coord[],
                     double transform[WORLD_NDIMS][WORLD_NDIMS + 1], double in_coord[])
{
   int      idim, jdim;
   double   homogeneous_coord[WORLD_NDIMS + 1];

   for(idim = 0; idim < WORLD_NDIMS; idim++){
      homogeneous_coord[idim] = in_coord[idim];
      }
   homogeneous_coord[WORLD_NDIMS] = 1.0;

   for(idim = 0; idim < WORLD_NDIMS; idim++){
      out_coord[idim] = 0.0;
      for(jdim = 0; jdim < WORLD_NDIMS + 1; jdim++){
         out_coord[idim] += transform[idim][jdim] * homogeneous_coord[jdim];
         }
      }
   }

/* Get the voxel to world transform (for column vectors) -- from mincstats */
void get_minc_voxel_to_world(int mincid,
                             double voxel_to_world[WORLD_NDIMS][WORLD_NDIMS + 1])
{
   int      idim, jdim;
   double   dircos[WORLD_NDIMS];
   double   step, start;
   char    *dimensions[] = { MIxspace, MIyspace, MIzspace };

   /* Zero the matrix */
   for(idim = 0; idim < WORLD_NDIMS; idim++){
      for(jdim = 0; jdim < WORLD_NDIMS + 1; jdim++)
         voxel_to_world[idim][jdim] = 0.0;
      }

   for(jdim = 0; jdim < WORLD_NDIMS; jdim++){

      /* Set default values */
      step = 1.0;
      start = 0.0;
      for(idim = 0; idim < WORLD_NDIMS; idim++)
         dircos[idim] = 0.0;
      dircos[jdim] = 1.0;

      /* Get the attributes */
      get_minc_attribute(mincid, dimensions[jdim], MIstart, 1, &start);
      get_minc_attribute(mincid, dimensions[jdim], MIstep, 1, &step);
      get_minc_attribute(mincid, dimensions[jdim], MIdirection_cosines,
                         WORLD_NDIMS, dircos);
      normalize_vector(dircos);

      /* Put them in the matrix */
      for(idim = 0; idim < WORLD_NDIMS; idim++){
         voxel_to_world[idim][jdim] = step * dircos[idim];
         voxel_to_world[idim][WORLD_NDIMS] += start * dircos[idim];
         }
      }
   }

/* Get the mapping from spatial dimension - x, y, z - to file dimensions
   and vice-versa. -- from mincstats */
void find_minc_spatial_dims(int mincid, int space_to_dim[], int dim_to_space[])
{
   int      imgid, dim[MAX_VAR_DIMS];
   int      idim, ndims, world_index;
   char     dimname[MAX_NC_NAME];

   /* Set default values */
   for(idim = 0; idim < 3; idim++)
      space_to_dim[idim] = -1;
   for(idim = 0; idim < MAX_VAR_DIMS; idim++)
      dim_to_space[idim] = -1;

   /* Get the dimension ids for the image variable */
   imgid = ncvarid(mincid, MIimage);
   (void)ncvarinq(mincid, imgid, NULL, NULL, &ndims, dim, NULL);

   /* Loop over them to find the spatial ones */
   for(idim = 0; idim < ndims; idim++){

      /* Get the name and check that this is a spatial dimension */
      (void)ncdiminq(mincid, dim[idim], dimname, NULL);
      if((dimname[0] == '\0') || (strcmp(&dimname[1], "space") != 0)){
         continue;
         }

      /* Look for the spatial dimensions */
      switch (dimname[0]){
      case 'x':
         world_index = 0;
         break;
      case 'y':
         world_index = 1;
         break;
      case 'z':
         world_index = 2;
         break;
      default:
         world_index = 0;
         break;
         }
      space_to_dim[world_index] = idim;
      dim_to_space[idim] = world_index;
      }
   }

/* Get a double attribute from a minc file -- from mincstats */
void get_minc_attribute(int mincid, char *varname, char *attname,
                        int maxvals, double vals[])
{
   int      varid;
   int      old_ncopts;
   int      att_length;

   if(!mivar_exists(mincid, varname))
      return;
   varid = ncvarid(mincid, varname);
   old_ncopts = ncopts;
   ncopts = 0;
   (void)miattget(mincid, varid, attname, NC_DOUBLE, maxvals, vals, &att_length);
   ncopts = old_ncopts;
   }

/* Get the total number of image dimensions in a minc file -- from mincstats */
int get_minc_ndims(int mincid)
{
   int      imgid;
   int      ndims;

   imgid = ncvarid(mincid, MIimage);
   (void)ncvarinq(mincid, imgid, NULL, NULL, &ndims, NULL, NULL);

   return ndims;
   }
