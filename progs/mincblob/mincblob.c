/* mincblob.c                                                                 */
/*                                                                            */
/* Andrew Janke - a.janke@gmail.com                                           */
/* Center for Magnetic Resonance                                              */
/* University of Queensland                                                   */
/*                                                                            */
/* Copyright Andrew Janke, The University of Queensland.                      */
/* Permission to use, copy, modify, and distribute this software and its      */
/* documentation for any purpose and without fee is hereby granted,           */
/* provided that the above copyright notice appear in all copies.  The        */
/* author and the University of Queensland make no representations about the  */
/* suitability of this software for any purpose.  It is provided "as is"      */
/* without express or implied warranty.                                       */
/*                                                                            */
/* The bloberiser                                                             */
/*                                                                            */
/* Mon Aug 21 16:18:36 EST 2000 - Original version                            */
/* Wed Nov  1 17:47:35 EST 2000 - rewrote translation option (new equation)   */
/* Thu Feb  7 23:42:40 EST 2002 - complete rewrite to use volume_io           */
/* Mon May  6 21:07:18 EDT 2002 - added -determinant option (jacobian)        */

/* TRACE */
/* Compute the areas within the deformation field that equate to volume       */
/* increase or decrease (+ve or -ve dilation)                                 */
/* Dilation is defined as the trace of the deformation field                  */
/* Thus it should range between -1..1 with -1 being compression and           */
/* 1 being dilation.                                                          */

/* TRANSLATION */
/* Compute the areas within the deformation field that equate to translation  */
/* Translation is defined as: trans = arccos( A.B / |A|.|B| ) * e^- (|A|-|B|) */
/* Thus it should range between 0..1 with 1 being "translation".              */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <volume_io.h>
#include <time_stamp.h>
#include <ParseArgv.h>

typedef enum { NO_OP, TRACE, DETERMINANT, TRANSLATION, MAGNITUDE } op;

/* function prototypes */
double   fdiv(double num, double denom);
double   farccos(double a0, double b0, double c0, double a1, double b1, double c1);
double   cindex(double a0, double b0, double c0, double a1, double b1, double c1);
double   feuc(double a, double b, double c);
void     clear_borders(Volume v, int sizes[3]);
void     fill_slice0(Volume v, Real value, int v0, int v1_size, int v2_size);
void     fill_slice1(Volume v, Real value, int v0_size, int v1, int v2_size);
void     fill_slice2(Volume v, Real value, int v0_size, int v1_size, int v2);
void     print_version_info(void);

/* argument variables */
static int verbose = FALSE;
static int clobber = FALSE;
static op operation = NO_OP;

/* argument table */
static ArgvInfo argTable[] = {
   {NULL, ARGV_HELP, (char *)NULL, (char *)NULL,
    "General options:"},
   {"-version", ARGV_FUNC, (char *)print_version_info, (char *)NULL,
    "print version info and exit"},
   {"-verbose", ARGV_CONSTANT, (char *)TRUE, (char *)&verbose,
    "print out extra information"},
   {"-clobber", ARGV_CONSTANT, (char *)TRUE, (char *)&clobber,
    "clobber existing files"},
   {NULL, ARGV_HELP, (char *)NULL, (char *)NULL,
    "\nOperations:"},
   {"-trace", ARGV_CONSTANT, (char *)TRACE, (char *)&operation,
    "compute the trace (approximate growth and shrinkage) -- FAST"},
   {"-determinant", ARGV_CONSTANT, (char *)DETERMINANT, (char *)&operation,
    "compute the determinant (exact growth and shrinkage) -- SLOW"},
   {"-translation", ARGV_CONSTANT, (char *)TRANSLATION, (char *)&operation,
    "compute translation (structure displacement)"},
   {"-magnitude", ARGV_CONSTANT, (char *)MAGNITUDE, (char *)&operation,
    "compute the magnitude of the displacement vector"},
   {NULL, ARGV_HELP, (char *)NULL, (char *)NULL, ""},
   {NULL, ARGV_END, NULL, NULL, NULL}
   };

int main(int argc, char **argv)
{
   char    *arg_string;
   char    *infile;
   char    *outfile;
   Volume   in_vol;
   Volume   out_vol;
   nc_type  datatype;
   BOOLEAN  signed_flag;
   Real     steps[MAX_VAR_DIMS];
   Real     starts[MAX_VAR_DIMS];
   int      sizes[MAX_VAR_DIMS];

   double   out_real_max, out_real_min;

   char    *in_axis_order[4] = { MIvector_dimension, MIzspace, MIyspace, MIxspace };
   char    *out_axis_order[3] = { MIzspace, MIyspace, MIxspace };

   int      x, y, z;
   double   value;
   progress_struct progress;

   /* Jacobian matrix */
   Real     J[3][3];

   /* Save list of arguments as strings  */
   arg_string = time_stamp(argc, argv);

   /* Check arguments   */
   if(ParseArgv(&argc, argv, argTable, 0) || (argc != 3)){
      fprintf(stderr, "\nUsage: %s [options] <vec_in.mnc> <out.mnc>\n", argv[0]);
      fprintf(stderr, "       %s -help\n\n", argv[0]);
      exit(EXIT_FAILURE);
      }
   infile = argv[1];
   outfile = argv[2];

   /* check for an operation */
   if(operation == NO_OP){
      fprintf(stderr, "%s: You need to specify an operation!\n\n", argv[0]);
      exit(EXIT_FAILURE);
      }

   /* check for the infile and outfile */
   if(access(infile, F_OK) != 0){
      fprintf(stderr, "%s: Couldn't find %s\n\n", argv[0], infile);
      exit(EXIT_FAILURE);
      }
   if(access(outfile, F_OK) == 0 && !clobber){
      fprintf(stderr, "%s: %s exists! (use -clobber to overwrite)\n\n", argv[0], outfile);
      exit(EXIT_FAILURE);
      }

   /* read in the input volume and a few other things.... */
   if(input_volume(infile, 4, in_axis_order,
                   NC_UNSPECIFIED, TRUE, 0.0, 0.0, TRUE, &in_vol, NULL) != 0){
      fprintf(stderr, "%s: Error reading input volume (%s)\n\n", argv[0], infile);
      exit(EXIT_FAILURE);
      }
   datatype = get_volume_nc_data_type(in_vol, &signed_flag);
   get_volume_sizes(in_vol, sizes);
   get_volume_starts(in_vol, starts);
   get_volume_separations(in_vol, steps);

   switch (operation){
   default:
   case NO_OP:
      fprintf(stderr, "%s: GNARKLE! this shouldn't happen!\n\n", argv[0]);
      exit(EXIT_FAILURE);
      break;

   case TRACE:
   case DETERMINANT:
      out_real_min = -1.0;
      out_real_max = 1.0;
      break;

   case MAGNITUDE:
   case TRANSLATION:
      out_real_min = 0.0;
      out_real_max = 1.0;
      break;
      }

   /* set up the output volume */
   out_vol = create_volume(3, out_axis_order, NC_DOUBLE, TRUE, 0.0, 0.0);
   set_volume_sizes(out_vol, &sizes[1]);
   set_volume_starts(out_vol, &starts[1]);
   set_volume_separations(out_vol, &steps[1]);
   alloc_volume_data(out_vol);

   /* set the surrounding voxels to 0 */
   clear_borders(out_vol, &sizes[1]);

   /* start to do some stuff */
   initialize_progress_report(&progress, FALSE, sizes[2] - 2, "Blobberising");
   for(z = 1; z < sizes[1] - 1; z++){
      for(y = 1; y < sizes[2] - 1; y++){
         for(x = 1; x < sizes[3] - 1; x++){

            switch (operation){
            default:
            case NO_OP:
               fprintf(stderr, "%s: GNARKLE! this shouldn't happen!\n\n", argv[0]);
               exit(EXIT_FAILURE);
               break;

            case TRACE:
               value =
                  ((get_volume_real_value(in_vol, 0, z, y, x + 1, 0) -
                    get_volume_real_value(in_vol, 0, z, y, x - 1, 0)) / (steps[3] * 2))
                  +
                  ((get_volume_real_value(in_vol, 1, z, y + 1, x, 0) -
                    get_volume_real_value(in_vol, 1, z, y - 1, x, 0)) / (steps[2] * 2))
                  +
                  ((get_volume_real_value(in_vol, 2, z + 1, y, x, 0) -
                    get_volume_real_value(in_vol, 2, z - 1, y, x, 0)) / (steps[1] * 2));
               break;

            case DETERMINANT:
               /* compute the Jacobian matrix */
               J[0][0] = 1 + ((get_volume_real_value(in_vol, 0, z, y, x + 1, 0) -
                               get_volume_real_value(in_vol, 0, z, y, x - 1,
                                                     0)) / (steps[3] * 2));
               J[0][1] =
                  (get_volume_real_value(in_vol, 0, z, y + 1, x, 0) -
                   get_volume_real_value(in_vol, 0, z, y - 1, x, 0)) / (steps[2] * 2);
               J[0][2] =
                  (get_volume_real_value(in_vol, 0, z + 1, y, x, 0) -
                   get_volume_real_value(in_vol, 0, z - 1, y, x, 0)) / (steps[1] * 2);

               J[1][0] = (get_volume_real_value(in_vol, 1, z, y, x + 1, 0) -
                          get_volume_real_value(in_vol, 1, z, y, x - 1,
                                                0)) / (steps[3] * 2);
               J[1][1] =
                  1 +
                  ((get_volume_real_value(in_vol, 1, z, y + 1, x, 0) -
                    get_volume_real_value(in_vol, 1, z, y - 1, x, 0)) / (steps[2] * 2));
               J[1][2] =
                  (get_volume_real_value(in_vol, 1, z + 1, y, x, 0) -
                   get_volume_real_value(in_vol, 1, z - 1, y, x, 0)) / (steps[1] * 2);

               J[2][0] = (get_volume_real_value(in_vol, 2, z, y, x + 1, 0) -
                          get_volume_real_value(in_vol, 2, z, y, x - 1,
                                                0)) / (steps[3] * 2);
               J[2][1] =
                  (get_volume_real_value(in_vol, 2, z, y + 1, x, 0) -
                   get_volume_real_value(in_vol, 2, z, y - 1, x, 0)) / (steps[2] * 2);
               J[2][2] =
                  1 +
                  ((get_volume_real_value(in_vol, 2, z + 1, y, x, 0) -
                    get_volume_real_value(in_vol, 2, z - 1, y, x, 0)) / (steps[1] * 2));

               value = (J[0][0] * ((J[1][1] * J[2][2]) - (J[1][2] * J[2][1])) -
                        J[0][1] * ((J[1][0] * J[2][2]) - (J[1][2] * J[2][0])) +
                        J[0][2] * ((J[1][0] * J[2][1]) - (J[1][1] * J[2][0]))
                  ) - 1;
               break;

            case TRANSLATION:
               value = (
                          /* x direction */
                          cindex(get_volume_real_value(in_vol, 0, z, y, x, 0),
                                 get_volume_real_value(in_vol, 1, z, y, x, 0),
                                 get_volume_real_value(in_vol, 2, z, y, x, 0),
                                 get_volume_real_value(in_vol, 0, z, y, x - 1, 0),
                                 get_volume_real_value(in_vol, 1, z, y, x - 1, 0),
                                 get_volume_real_value(in_vol, 2, z, y, x - 1, 0))
                          +
                          cindex(get_volume_real_value(in_vol, 0, z, y, x, 0),
                                 get_volume_real_value(in_vol, 1, z, y, x, 0),
                                 get_volume_real_value(in_vol, 2, z, y, x, 0),
                                 get_volume_real_value(in_vol, 0, z, y, x + 1, 0),
                                 get_volume_real_value(in_vol, 1, z, y, x + 1, 0),
                                 get_volume_real_value(in_vol, 2, z, y, x + 1, 0))
                          +
                          /* y direction */
                          cindex(get_volume_real_value(in_vol, 0, z, y, x, 0),
                                 get_volume_real_value(in_vol, 1, z, y, x, 0),
                                 get_volume_real_value(in_vol, 2, z, y, x, 0),
                                 get_volume_real_value(in_vol, 0, z, y - 1, x, 0),
                                 get_volume_real_value(in_vol, 1, z, y - 1, x, 0),
                                 get_volume_real_value(in_vol, 2, z, y - 1, x, 0))
                          +
                          cindex(get_volume_real_value(in_vol, 0, z, y, x, 0),
                                 get_volume_real_value(in_vol, 1, z, y, x, 0),
                                 get_volume_real_value(in_vol, 2, z, y, x, 0),
                                 get_volume_real_value(in_vol, 0, z, y + 1, x, 0),
                                 get_volume_real_value(in_vol, 1, z, y + 1, x, 0),
                                 get_volume_real_value(in_vol, 2, z, y + 1, x, 0))
                          +
                          /* z direction */
                          cindex(get_volume_real_value(in_vol, 0, z, y, x, 0),
                                 get_volume_real_value(in_vol, 1, z, y, x, 0),
                                 get_volume_real_value(in_vol, 2, z, y, x, 0),
                                 get_volume_real_value(in_vol, 0, z - 1, y, x, 0),
                                 get_volume_real_value(in_vol, 1, z - 1, y, x, 0),
                                 get_volume_real_value(in_vol, 2, z - 1, y, x, 0))
                          +
                          cindex(get_volume_real_value(in_vol, 0, z, y, x, 0),
                                 get_volume_real_value(in_vol, 1, z, y, x, 0),
                                 get_volume_real_value(in_vol, 2, z, y, x, 0),
                                 get_volume_real_value(in_vol, 0, z + 1, y, x, 0),
                                 get_volume_real_value(in_vol, 1, z + 1, y, x, 0),
                                 get_volume_real_value(in_vol, 2, z + 1, y, x, 0))
                  ) / 6;
               break;

            case MAGNITUDE:
               value =
                  sqrt((get_volume_real_value(in_vol, 0, z, y, x, 0) *
                        get_volume_real_value(in_vol, 0, z, y, x, 0)) +
                       (get_volume_real_value(in_vol, 1, z, y, x, 0) *
                        get_volume_real_value(in_vol, 1, z, y, x, 0)) +
                       (get_volume_real_value(in_vol, 2, z, y, x, 0) *
                        get_volume_real_value(in_vol, 2, z, y, x, 0)));
               break;
               }

            set_volume_real_value(out_vol, z, y, x, 0, 0, value);

            /* check the min and max */
            if(value < out_real_min){
               out_real_min = value;
               }
            else if(value > out_real_max){
               out_real_max = value;
               }
            }
         }
      update_progress_report(&progress, z + 1);
      }
   terminate_progress_report(&progress);

   if(verbose){
      fprintf(stdout, "%s: Found output range of [%g:%g]\n", argv[0], out_real_min,
              out_real_max);
      }
   set_volume_real_range(out_vol, out_real_min, out_real_max);
   output_volume(outfile, datatype, signed_flag, 0.0, 0.0, out_vol, arg_string, NULL);
   return (EXIT_SUCCESS);
   }

double fdiv(double num, double denom)
{
   if(fabs(denom) < 0.0005){
      return 0.0;
      }
   else {
      return num / denom;
      }
   }

double feuc(double a, double b, double c)
{
   return sqrt((a * a) + (b * b) + (c * c));
   }

double farccos(double a0, double b0, double c0, double a1, double b1, double c1)
{
   return
      acos(fdiv((a0 * a1) + (b0 * b1) + (c0 * c1), feuc(a0, b0, c0) * feuc(a1, b1, c1)));
   }

double cindex(double a0, double b0, double c0, double a1, double b1, double c1)
{
   return (1.0 - farccos(a0, b0, c0, a1, b1, c1))
      * exp(-1.0 * (fabs(feuc(a0, b0, c0) - feuc(a1, b1, c1))));
   }

/* steve's super-dooper padding function(s) */
void clear_borders(Volume v, int sizes[3])
{
   fill_slice0(v, 0, 0, sizes[1], sizes[2]);
   fill_slice0(v, 0, sizes[0] - 1, sizes[1], sizes[2]);

   fill_slice1(v, 0, sizes[0], 0, sizes[2]);
   fill_slice1(v, 0, sizes[0], sizes[1] - 1, sizes[2]);

   fill_slice2(v, 0, sizes[0], sizes[1], 0);
   fill_slice2(v, 0, sizes[0], sizes[1], sizes[2] - 1);
   }

void fill_slice0(Volume v, Real value, int v0, int v1_size, int v2_size)
{
   int      v1, v2;

   for(v1 = 0; v1 < v1_size; ++v1){
      for(v2 = 0; v2 < v2_size; ++v2){
         set_volume_real_value(v, v0, v1, v2, 0, 0, value);
         }
      }
   }

void fill_slice1(Volume v, Real value, int v0_size, int v1, int v2_size)
{
   int      v0, v2;

   for(v0 = 0; v0 < v0_size; ++v0){
      for(v2 = 0; v2 < v2_size; ++v2){
         set_volume_real_value(v, v0, v1, v2, 0, 0, value);
         }
      }
   }

void fill_slice2(Volume v, Real value, int v0_size, int v1_size, int v2)
{
   int      v0, v1;

   for(v0 = 0; v0 < v0_size; ++v0){
      for(v1 = 0; v1 < v1_size; ++v1){
         set_volume_real_value(v, v0, v1, v2, 0, 0, value);
         }
      }
   }

void print_version_info(void)
{
   fprintf(stdout, "%s version %s\n", PACKAGE_STRING, PACKAGE_VERSION);
   fprintf(stdout, "Comments to %s\n", PACKAGE_BUGREPORT);
   fprintf(stdout, "\n");
   exit(EXIT_SUCCESS);
   }
