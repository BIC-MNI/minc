/* minccmp.c                                                                 */
/*                                                                           */
/* Copyright Andrew Janke - a.janke@gmail.com                                */
/* Permission to use, copy, modify, and distribute this software and its     */
/* documentation for any purpose and without fee is hereby granted,          */
/* provided that the above copyright notice appear in all copies.  The       */
/* author makes no representations about the                                 */
/* suitability of this software for any purpose.  It is provided "as is"     */
/* without express or implied warranty.                                      */
/*                                                                           */
/* calculates measures of similarity/difference between 2 or more volumes    */
/*                                                                           */
/* Measures used (sum(x) denotes the sum of x over a volume):                */
/* RMSE   - Root Mean Squared Error                                          */
/*        = sqrt( 1/n * sum((a-b)^2))                                        */
/* xcorr  - Cross Correlation                                                */
/*        =  sum((a*b)^2) / (sqrt(sum(a^2)) * sqrt(sum(b^2))                 */
/* zscore - z-score differences                                              */
/*        =  sum( |((a - mean(a)) / stdev(a)) -                              */
/*                 ((b - mean(b)) / stdev(b))| ) / nvox                      */
/*                                                                           */
/* Tue Jun 17 11:31:10 EST 2003 - initial version inspired by voldiff and    */
/*                                   peter's compare_volumes                 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <float.h>
#include <voxel_loop.h>
#include <ParseArgv.h>

#ifndef FALSE
#  define FALSE 0
#endif
#ifndef TRUE
#  define TRUE 1
#endif

#define SQR2(x) ((x) * (x))

typedef struct {
   double   nvox;
   double   sum;         /* sum of valid voxels */
   double   ssum;        /* squared sum of valid voxels */
   double   min;
   double   max;

   double   mean;
   double   var;
   double   sd;

   double   sum_prd0;    /* sum of product of file[x] with file[0] */
   double   ssum_add0;   /* squared sum of addition of file[x] with file[0] */
   double   ssum_dif0;   /* squared sum of difference of file[x] with file[0] */
   double   ssum_prd0;   /* squared sum of product of file[x] with file[0] */

   double   sum_zdif0;   /* sum of zscore differences of file[x] and file[0] */

   /* result stores */
   double   rmse;
   double   xcorr;
   double   zscore;
   double   vratio;
   } Vol_Data;

typedef struct {
   int      n_datafiles;

   int      mask;
   int      mask_idx;

   /* individual volume data */
   Vol_Data *vd;
   } Loop_Data;

/* Function prototypes */
void     pass_0(void *caller_data, long num_voxels,
                int input_num_buffers, int input_vector_length,
                double *input_data[],
                int output_num_buffers, int output_vector_length,
                double *output_data[], Loop_Info * loop_info);
void     pass_1(void *caller_data, long num_voxels,
                int input_num_buffers, int input_vector_length,
                double *input_data[],
                int output_num_buffers, int output_vector_length,
                double *output_data[], Loop_Info * loop_info);
void     print_result(char *title, double result);
void     dump_stats(Loop_Data * ld);
void     do_int_calcs(Loop_Data * ld);
void     do_final_calcs(Loop_Data * ld);

/* Argument variables and table */
static int verbose = FALSE;
static int debug = FALSE;
static int quiet = FALSE;
static int clobber = FALSE;
static int max_buffer_size_in_kb = 4 * 1024;
static int check_dim_info = TRUE;
static char *mask_fname = NULL;
static double valid_range[2] = { -DBL_MAX, DBL_MAX };

static int do_all = FALSE;
static int do_ssq = FALSE;
static int do_rmse = FALSE;
static int do_xcorr = FALSE;
static int do_zscore = FALSE;
static int do_vratio = FALSE;

ArgvInfo argTable[] = {
   {"-verbose", ARGV_CONSTANT, (char *)TRUE, (char *)&verbose,
    "be verbose"},
   {"-debug", ARGV_CONSTANT, (char *)TRUE, (char *)&debug,
    "dump all stats info"},
   {"-quiet", ARGV_CONSTANT, (char *)TRUE, (char *)&quiet,
    "print requested values only"},
   {"-clobber", ARGV_CONSTANT, (char *)TRUE, (char *)&clobber,
    "clobber existing files"},
   {"-max_buffer_size_in_kb", ARGV_INT, (char *)1, (char *)&max_buffer_size_in_kb,
    "maximum size of internal buffers."},
   {"-check_dimensions", ARGV_CONSTANT, (char *) TRUE, (char *) &check_dim_info,
    "Check that files have matching dimensions (default)."},
   {"-nocheck_dimensions", ARGV_CONSTANT, (char *) FALSE, (char *) &check_dim_info,
    "Do not check that files have matching dimensions."},

   {NULL, ARGV_HELP, (char *)NULL, (char *)NULL,
    "\nVoxel selection options (applies to first volume ONLY):"},
   {"-floor", ARGV_FLOAT, (char *)1, (char *)&valid_range[0],
    "Ignore voxels below this value. (incl)"},
   {"-ceil", ARGV_FLOAT, (char *)1, (char *)&valid_range[1],
    "Ignore voxels above this value. (incl)"},
   {"-range", ARGV_FLOAT, (char *)2, (char *)&valid_range,
    "Ignore voxels outside the range. (incl)"},

   {"-mask", ARGV_STRING, (char *)1, (char *)&mask_fname,
    "Use <mask.mnc> for calculations."},

   {NULL, ARGV_HELP, (char *)NULL, (char *)NULL,
    "\nImage Statistics (printed in this order)"},
   {"-all", ARGV_CONSTANT, (char *)TRUE, (char *)&do_all,
    "all statistics (default)."},
   {"-ssq", ARGV_CONSTANT, (char *)TRUE, (char *)&do_ssq,
    "sum of squared difference (2 volumes)"},
   {"-rmse", ARGV_CONSTANT, (char *)TRUE, (char *)&do_rmse,
    "root mean squared error (2 volumes)"},
   {"-xcorr", ARGV_CONSTANT, (char *)TRUE, (char *)&do_xcorr,
    "cross correlation (2 volumes)"},
   {"-zscore", ARGV_CONSTANT, (char *)TRUE, (char *)&do_zscore,
    "z-score (2 volumes)"},
//   {"-vr", ARGV_CONSTANT, (char *)TRUE, (char *)&do_vratio,
//    "variance ratio (2 volumes)"},

//   {NULL, ARGV_HELP, (char *)NULL, (char *)NULL,
//    "\nBinary Image Only Statistics"},
//   {"-kappa", ARGV_CONSTANT, (char *)TRUE, (char *)&do_kappa,
//    "all statistics (default)."},

   {NULL, ARGV_HELP, NULL, NULL, ""},
   {NULL, ARGV_END, NULL, NULL, NULL}
   };

int main(int argc, char *argv[]){
   char **infiles;
   int n_infiles;

   Loop_Options *loop_opt;
   Loop_Data ld;

   int i;

   /* Get arguments */
   if(ParseArgv(&argc, argv, argTable, 0) || (argc < 3)){
      fprintf(stderr, "\nUsage: %s [options] <in1.mnc> <in2.mnc> [<inn.mnc>]\n", argv[0]);
      fprintf(stderr, "       %s -help\n\n", argv[0]);
      return (EXIT_FAILURE);
      }
   n_infiles = argc - 1;
   infiles = &argv[1];

   /* check arguments */
   if(!do_rmse && !do_xcorr && !do_zscore && !do_vratio){
      do_all = TRUE;
      }
   if(do_all){
      do_ssq = do_rmse = do_xcorr = do_zscore = do_vratio = TRUE;
      }

   /* check for infiles */
   if(verbose){
      fprintf(stderr, "\n+++ infiles +++\n");
      }
   for(i = 0; i < n_infiles; i++){
      if(verbose){
         fprintf(stderr, " | [%02d]: %s\n", i, infiles[i]);
         }
      if(access(infiles[i], F_OK) != 0){
         fprintf(stderr, "%s: Couldn't find %s\n", argv[0], infiles[i]);
         exit(EXIT_FAILURE);
         }
      }

   /* set up Loop_Data struct and mask file */
   ld.n_datafiles = n_infiles;
   if(mask_fname != NULL){
      if(verbose){
         fprintf(stderr, " | mask: %s\n", mask_fname);
         }
      if(access(mask_fname, F_OK) != 0){
         fprintf(stderr, "%s: Couldn't find mask file: %s\n", argv[0], mask_fname);
         exit(EXIT_FAILURE);
         }

      ld.mask = TRUE;
      ld.mask_idx = n_infiles;

      infiles[n_infiles] = mask_fname;
      n_infiles++;
      }
   else {
      ld.mask = FALSE;
      ld.mask_idx = 0;
      }

   /* allocate space and initialise volume stats data */
   ld.vd = (Vol_Data *) malloc(sizeof(Vol_Data) * ld.n_datafiles);
   for(i = 0; i < ld.n_datafiles; i++){
      ld.vd[i].nvox = 0;
      ld.vd[i].sum = 0;
      ld.vd[i].ssum = 0;
      ld.vd[i].min = DBL_MAX;
      ld.vd[i].max = -DBL_MAX;

      ld.vd[i].sum_prd0 = 0;
      ld.vd[i].ssum_add0 = 0;
      ld.vd[i].ssum_dif0 = 0;
      ld.vd[i].ssum_prd0 = 0;

      ld.vd[i].mean = 0;
      ld.vd[i].var = 0;
      ld.vd[i].sd = 0;

      ld.vd[i].rmse = 0.0;
      ld.vd[i].xcorr = 0.0;
      ld.vd[i].zscore = 0.0;
      ld.vd[i].vratio = 0.0;
      }

   /* set up and do voxel_loop(s) */
   loop_opt = create_loop_options();
   set_loop_verbose(loop_opt, verbose);
   set_loop_buffer_size(loop_opt, (long)1024 * max_buffer_size_in_kb);
   set_loop_check_dim_info(loop_opt, check_dim_info);

   /* first pass */
   voxel_loop(n_infiles, infiles, 0, NULL, NULL, loop_opt, pass_0, (void *)&ld);

   /* intermediate calculations */
   do_int_calcs(&ld);

   /* run the second pass if we have to */
   if(do_zscore){
      voxel_loop(n_infiles, infiles, 0, NULL, NULL, loop_opt, pass_1, (void *)&ld);
      }

   /* final calculations */
   do_final_calcs(&ld);

   free_loop_options(loop_opt);

   if(debug){
      dump_stats(&ld);
      }

   /* calculate and print result(s) */
   if(do_all && !quiet){
      fprintf(stdout, "file[0]:      %s\n", infiles[0]);
      fprintf(stdout, "file[1]:      %s\n", infiles[1]);
      fprintf(stdout, "mask file:    %s\n", mask_fname);
      }
   if(do_ssq){
      print_result("ssq:          ", ld.vd[1].ssum_dif0);
      }
   if(do_rmse){
      print_result("rmse:         ", ld.vd[1].rmse);
      }
   if(do_xcorr){
      print_result("xcorr:        ", ld.vd[1].xcorr);
      }
   if(do_zscore){
      print_result("zscore:       ", ld.vd[1].zscore);
      }
   if(!quiet){
      fprintf(stdout, "\n");
      }

   return EXIT_SUCCESS;
   }

/* voxel loop function for first pass through data */
void pass_0(void *caller_data, long num_voxels,
            int input_num_buffers, int input_vector_length,
            double *input_data[],
            int output_num_buffers, int output_vector_length,
            double *output_data[], Loop_Info * loop_info){
   long ivox;
   double valuei, value0;
   int i;

   /* get pointer to loop data */
   Loop_Data *ld = (Loop_Data *)caller_data;

   /* shut the compiler up - yes I _know_ I don't use these */
   (void)output_num_buffers;
   (void)output_vector_length;
   (void)output_data;
   (void)loop_info;

   /* sanity check */
   if((input_num_buffers < 2) || (output_num_buffers != 0)){
      fprintf(stderr, "Bad arguments to pass_0\n");
      exit(EXIT_FAILURE);
      }
   
   /* for each voxel */
   for(ivox = num_voxels * input_vector_length; ivox--;){

      /* skip voxels out of the mask region */
      if(ld->mask && !(int)input_data[ld->mask_idx][ivox]){
         continue;
         }

      value0 = input_data[0][ivox];
      if(value0 >= valid_range[0] && value0 <= valid_range[1]){

         /* for each volume */
         for(i = 0; i < ld->n_datafiles; i++){

            valuei = input_data[i][ivox];

            /* various voxel sums */
            ld->vd[i].nvox++;
            ld->vd[i].sum += valuei;
            ld->vd[i].ssum += SQR2(valuei);

            if(i != 0){
               ld->vd[i].sum_prd0 += valuei * value0;

               ld->vd[i].ssum_add0 += SQR2(valuei + value0);
               ld->vd[i].ssum_dif0 += SQR2(valuei - value0);
               ld->vd[i].ssum_prd0 += SQR2(valuei * value0);
               }

            /* min and max */
            if(valuei < ld->vd[i].min){
               ld->vd[i].min = valuei;
               }
            else if(valuei > ld->vd[i].max){
               ld->vd[i].max = valuei;
               }
            }
         }
      }

   return;
   }

/* intermediate calculations */
void do_int_calcs(Loop_Data * ld){
   int i;
   double denom;

   for(i = 0; i < ld->n_datafiles; i++){
      /* mean */
      ld->vd[i].mean = ld->vd[i].sum / ld->vd[i].nvox;

      /* variance */
      ld->vd[i].var = ((ld->vd[i].nvox * ld->vd[i].ssum) - SQR2(ld->vd[i].sum)) /
         (ld->vd[i].nvox * (ld->vd[i].nvox - 1));

      /* sd */
      ld->vd[i].sd = sqrt(ld->vd[i].var);

      /* RMSE */
      ld->vd[i].rmse = sqrt((1.0 / ld->vd[0].nvox) * ld->vd[i].ssum_dif0);

      /* xcorr */
      denom = sqrt(ld->vd[0].ssum * ld->vd[i].ssum);
      ld->vd[i].xcorr = (denom == 0.0) ? 0.0 : ld->vd[i].sum_prd0 / denom;
      }
   }

/* voxel loop function for second pass through data */
void pass_1(void *caller_data, long num_voxels,
            int input_num_buffers, int input_vector_length,
            double *input_data[],
            int output_num_buffers, int output_vector_length,
            double *output_data[], Loop_Info * loop_info){
   long ivox;
   double valuei, value0;
   int i;

   /* get pointer to loop data */
   Loop_Data *ld = (Loop_Data *)caller_data;

   /* shut the compiler up - yes I _know_ I don't use these */
   (void)output_num_buffers;
   (void)output_vector_length;
   (void)output_data;
   (void)loop_info;

   /* sanity check */
   if((input_num_buffers < 2) || (output_num_buffers != 0)){
      fprintf(stderr, "Bad arguments to pass_1\n");
      exit(EXIT_FAILURE);
      }

   /* for each voxel */
   for(ivox = num_voxels * input_vector_length; ivox--;){

      /* skip voxels out of the mask region */
      if(ld->mask && !(int)input_data[ld->mask_idx][ivox]){
         continue;
         }

      value0 = input_data[0][ivox];
      if(value0 >= valid_range[0] && value0 <= valid_range[1]){

         /* for each volume */
         for(i = 0; i < ld->n_datafiles; i++){

            valuei = input_data[i][ivox];

            if(i != 0){

               /* zscore total */
               ld->vd[i].sum_zdif0 +=
                  fabs(((value0 - ld->vd[0].mean) / ld->vd[0].sd) -
                       ((valuei - ld->vd[i].mean) / ld->vd[i].sd));

               }
            }
         }
      }

   return;
   }

/* final calculations */
void do_final_calcs(Loop_Data * ld){
   int i;

   for(i = 0; i < ld->n_datafiles; i++){
      /* zscore */
      ld->vd[i].zscore = ld->vd[i].sum_zdif0 / ld->vd[i].nvox;
      }
   }

/* dirty little function to print out results */
void print_result(char *title, double result){
   if(!quiet){
      fprintf(stdout, "%s", title);
      }
   fprintf(stdout, "%.10g\n", result);
   }

/* debug function to dump stats structure */
void dump_stats(Loop_Data * ld){
   int i;

   fprintf(stdout, " + Main Loop data structure\n");
   fprintf(stdout, " | n_datafiles      %d\n", ld->n_datafiles);
   fprintf(stdout, " | mask             %d\n", ld->mask);
   fprintf(stdout, " | mask_idx         %d\n", ld->mask_idx);

   fprintf(stdout, " +++ volume data stats\n");
   for(i = 0; i < ld->n_datafiles; i++){
      fprintf(stdout, " |---------------------------------\n");
      fprintf(stdout, " | [%02d] nvox         %10g\n", i, ld->vd[i].nvox);
      fprintf(stdout, " | [%02d] sum          %.10g\n", i, ld->vd[i].sum);
      fprintf(stdout, " | [%02d] ssum         %.10g\n", i, ld->vd[i].ssum);
      fprintf(stdout, " | [%02d] min          %.10g\n", i, ld->vd[i].min);
      fprintf(stdout, " | [%02d] max          %.10g\n", i, ld->vd[i].max);
      fprintf(stdout, " | [%02d] mean         %.10g\n", i, ld->vd[i].mean);
      fprintf(stdout, " | [%02d] var          %.10g\n", i, ld->vd[i].var);
      fprintf(stdout, " | [%02d] sd           %.10g\n", i, ld->vd[i].sd);
      fprintf(stdout, " | [%02d] sum_prd0     %.10g\n", i, ld->vd[i].sum_prd0);
      fprintf(stdout, " | [%02d] ssum_add0    %.10g\n", i, ld->vd[i].ssum_add0);
      fprintf(stdout, " | [%02d] ssum_dif0    %.10g\n", i, ld->vd[i].ssum_dif0);
      fprintf(stdout, " | [%02d] ssum_prd0    %.10g\n", i, ld->vd[i].ssum_prd0);
      fprintf(stdout, " | [%02d] sum_zdif0    %.10g\n", i, ld->vd[i].sum_zdif0);

      fprintf(stdout, " | [%02d] rmse         %.10g\n", i, ld->vd[i].rmse);
      fprintf(stdout, " | [%02d] xcorr        %.10g\n", i, ld->vd[i].xcorr);
      fprintf(stdout, " | [%02d] zscore       %.10g\n", i, ld->vd[i].zscore);
      fprintf(stdout, " | [%02d] vratio       %.10g\n", i, ld->vd[i].vratio);
      }
   }
