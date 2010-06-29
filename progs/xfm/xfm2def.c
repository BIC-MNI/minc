/*
 * xfm2def.c
 * 
 * Approximates a deformation grid from an input transformation
 * 
 * Copyright Andrew Janke - a.janke@gmail.com
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies.  The
 * author makes no representations about the suitability of this software for
 * any purpose. It is provided "as is" without express or implied warranty.
 */

#include <math.h>
#include <float.h>
#include <unistd.h>
#include <sys/stat.h>
#include <volume_io.h>
#include <ParseArgv.h>
#include <time_stamp.h>

#define WORLD_NDIMS 3
#define DEF_BOOL -1

static char *std_dimorder_v[] = { MIxspace, MIyspace, MIzspace, MIvector_dimension };

/* argument variables and table */
static int verbose = FALSE;
static int clobber = FALSE;
static nc_type dtype = NC_SHORT;
static int is_signed = FALSE;
static int nelem[WORLD_NDIMS + 1] = { 100, 100, 100, 3 };
static double start[WORLD_NDIMS] = { -50.0, -50.0, -50.0 };
static double step[WORLD_NDIMS] = { 1.0, 1.0, 1.0 };
static double dircos[WORLD_NDIMS][WORLD_NDIMS] = { 
   {1.0, 0.0, 0.0},
   {0.0, 1.0, 0.0},
   {0.0, 0.0, 1.0}
   };

static ArgvInfo argTable[] = {
   {"-verbose", ARGV_CONSTANT, (char *)TRUE, (char *)&verbose,
    "Print out extra information."},
   {"-clobber", ARGV_CONSTANT, (char *)TRUE, (char *)&clobber,
    "Overwrite existing files."},

   {NULL, ARGV_HELP, NULL, NULL, "\nOuput grid Options"},
   {"-byte", ARGV_CONSTANT, (char *)NC_BYTE, (char *)&dtype,
    "Create a byte data."},
   {"-short", ARGV_CONSTANT, (char *)NC_SHORT, (char *)&dtype,
    "Create a short integer data.(Default)"},
   {"-int", ARGV_CONSTANT, (char *)NC_INT, (char *)&dtype,
    "Create a 32-bit integer"},
   {"-float", ARGV_CONSTANT, (char *)NC_FLOAT, (char *)&dtype,
    "Create a single-precision data."},
   {"-double", ARGV_CONSTANT, (char *)NC_DOUBLE, (char *)&dtype,
    "Create a double-precision data."},
   {"-signed", ARGV_CONSTANT, (char *)TRUE, (char *)&is_signed,
    "Create a signed integer data."},
   {"-unsigned", ARGV_CONSTANT, (char *)FALSE, (char *)&is_signed,
    "Create a unsigned integer data. (Default)"},
   {"-xnelements", ARGV_INT, (char *)1, (char *)&nelem[0],
    "Number of samples in x dimension."},
   {"-ynelements", ARGV_INT, (char *)1, (char *)&nelem[1],
    "Number of samples in y dimension."},
   {"-znelements", ARGV_INT, (char *)1, (char *)&nelem[2],
    "Number of samples in z dimension."},
   {"-xstart", ARGV_FLOAT, (char *)1, (char *)&start[0],
    "Starting coordinate for x dimension."},
   {"-ystart", ARGV_FLOAT, (char *)1, (char *)&start[1],
    "Starting coordinate for y dimension."},
   {"-zstart", ARGV_FLOAT, (char *)1, (char *)&start[2],
    "Starting coordinate for z dimension."},
   {"-xstep", ARGV_FLOAT, (char *)1, (char *)&step[0],
    "Step size for x dimension."},
   {"-ystep", ARGV_FLOAT, (char *)1, (char *)&step[1],
    "Step size for y dimension."},
   {"-zstep", ARGV_FLOAT, (char *)1, (char *)&step[2],
    "Step size for z dimension."},
   {"-xdircos", ARGV_FLOAT, (char *)3, (char *)dircos[0],
    "Direction cosines along the x dimension"},
   {"-ydircos", ARGV_FLOAT, (char *)3, (char *)dircos[1],
    "Direction cosines along the y dimension"},
   {"-zdircos", ARGV_FLOAT, (char *)3, (char *)dircos[2],
    "Direction cosines along the z dimension"},

   {NULL, ARGV_HELP, NULL, NULL, ""},
   {NULL, ARGV_END, NULL, NULL, NULL}
   };

int main(int argc, char *argv[])
{
   char    *xfm_fn;
   char    *out_fn;
   char    *history;
   progress_struct progress;
   Volume   def_grid;
   General_transform xfm;
   int      x, y, z, v;
   double   vcoord[4], wcoord[3], wcoord_t[3];
   double   min, max;
   double   value;
   int i;

   /* get the history string */
   history = time_stamp(argc, argv);
   
   /* get args and file names */
   if(ParseArgv(&argc, argv, argTable, 0) || (argc != 3)){
      fprintf(stderr, "\nUsage: %s [options] <input.xfm> <def_vol.mnc>\n", argv[0]);
      fprintf(stderr, "       %s [-help] %d\n\n", argv[0], argc);
      exit(EXIT_FAILURE);
      }
   xfm_fn = argv[1];
   out_fn = argv[2];

   /* check for infile and outfile */
   if(access(xfm_fn, F_OK) != 0){
      fprintf(stderr, "%s: Couldn't find input xfm %s.\n\n", argv[0], xfm_fn);
      exit(EXIT_FAILURE);
      }
   if(access(out_fn, F_OK) == 0 && !clobber){
      fprintf(stderr, "%s: %s exists, -clobber to overwrite.\n\n", argv[0], out_fn);
      exit(EXIT_FAILURE);
      }

   /* read in  the input transformation */
   if(input_transform_file(xfm_fn, &xfm) != OK){
      fprintf(stderr, "%s: Error reading in xfm %s\n\n", argv[0], xfm_fn);
      }

   /* create the def_grid volume */
   def_grid = create_volume(4, std_dimorder_v, dtype, is_signed, 0.0, 0.0);
   set_volume_sizes(def_grid, nelem);
   set_volume_starts(def_grid, start);
   set_volume_separations(def_grid, step);
   for(i = 0; i < WORLD_NDIMS; i++){
      set_volume_direction_cosine(def_grid, i, dircos[i]);
      }
   alloc_volume_data(def_grid);

   /* generate the grid itself */
   min = DBL_MAX;
   max = -DBL_MAX;
   vcoord[3] = 0;
   initialize_progress_report(&progress, FALSE, nelem[0], "Creating grid");
   for(x = nelem[0]; x--;){
      for(y = nelem[1]; y--;){
         for(z = nelem[2]; z--;){

            /* figure out where we are in world space */
            vcoord[0] = x;
            vcoord[1] = y;
            vcoord[2] = z;
            convert_voxel_to_world(def_grid,
                                   vcoord,
                                   &wcoord[0], &wcoord[1], &wcoord[2]);
            
            /* transform that */
            general_transform_point(&xfm,
                                    wcoord[0], wcoord[1], wcoord[2],
                                    &wcoord_t[0], &wcoord_t[1], &wcoord_t[2]);
            
            /* write out dx, dy and dz */
            for(v = nelem[3]; v--;){
               value = wcoord_t[v] - wcoord[v];
               if(value < min){
                  min = value;
                  }
               if(value > max){
                  max = value;
                  }
               set_volume_real_value(def_grid, x, y, z, v, 0, value);
               }
            }
         }
      update_progress_report(&progress, x + 1);
      }
   terminate_progress_report(&progress);

   /* set the range */
   if(verbose){
      fprintf(stdout, " + data range: [%g:%g]\n", min, max);
      }
   set_volume_real_range(def_grid, min, max);

   /* output the result */
   if(verbose){
      fprintf(stdout, "Outputting %s...\n", out_fn);
      }
   if(output_volume(out_fn, dtype, is_signed, 0.0, 0.0, def_grid, history, NULL) != OK){
      fprintf(stderr, "Problems outputing: %s\n\n", out_fn);
      exit(EXIT_FAILURE);
      }

   /* tidy up */
   delete_volume(def_grid);
   delete_general_transform(&xfm);
   
   return (EXIT_SUCCESS);
   }
