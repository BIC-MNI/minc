/* kernel_ops.c */

#include <float.h>
#include <limits.h>
#include "kernel_ops.h"

extern int verbose;

/* function prototypes */
void     split_kernel(Kernel * K, Kernel * k1, Kernel * k2);
int      compare_ints(const void *a, const void *b);
int      compare_groups(const void *a, const void *b);

/* structure for group information */
typedef struct {
   unsigned int orig_label;
   unsigned int count;
   } group_info_struct;

typedef group_info_struct *Group_info;

int compare_ints(const void *a, const void *b)
{
   return (*(int *)a - *(int *)b);
   }

int compare_groups(const void *a, const void *b)
{
   return (*(Group_info *) b)->count - (*(Group_info *) a)->count;
   }

void split_kernel(Kernel * K, Kernel * k1, Kernel * k2)
{
   int      c, k1c, k2c;

   /* fill the two sub kernels */
   k1c = k2c = 0;
   for(c = 0; c < K->nelems; c++){
      if((K->K[c][2] < 0) ||
         (K->K[c][1] < 0 && K->K[c][2] <= 0) ||
         (K->K[c][0] < 0 && K->K[c][1] <= 0 && K->K[c][2] <= 0)){

         k1->K[k1c] = K->K[c];
         k1c++;
         }
      else {
         k2->K[k2c] = K->K[c];
         k2c++;
         }
      }
   k1->nelems = k1c;
   k2->nelems = k2c;
   }

/* binarise a volume between a range */
Volume  *binarise(Volume * vol, double floor, double ceil, double fg, double bg)
{
   int      x, y, z;
   int      sizes[MAX_VAR_DIMS];
   double   value;
   progress_struct progress;

   if(verbose){
      fprintf(stdout, "Binarising, range: [%g:%g] fg/bg: [%g:%g]\n", floor, ceil, fg, bg);
      }
   get_volume_sizes(*vol, sizes);

   initialize_progress_report(&progress, FALSE, sizes[2], "Binarise");
   for(z = sizes[0]; z--;){
      for(y = sizes[1]; y--;){
         for(x = sizes[2]; x--;){

            value = get_volume_voxel_value(*vol, z, y, x, 0, 0);
            if((value >= floor) && (value <= ceil)){
               set_volume_voxel_value(*vol, z, y, x, 0, 0, fg);
               }
            else {
               set_volume_voxel_value(*vol, z, y, x, 0, 0, bg);
               }

            }
         }
      update_progress_report(&progress, z + 1);
      }
   terminate_progress_report(&progress);

   return (vol);
   }

/* clamp a volume between a range */
Volume  *clamp(Volume * vol, double floor, double ceil, double bg)
{
   int      x, y, z;
   int      sizes[MAX_VAR_DIMS];
   double   value;
   progress_struct progress;

   if(verbose){
      fprintf(stdout, "Clamping, range: [%g:%g] bg: %g\n", floor, ceil, bg);
      }

   get_volume_sizes(*vol, sizes);

   initialize_progress_report(&progress, FALSE, sizes[2], "Clamping");
   for(z = sizes[0]; z--;){
      for(y = sizes[1]; y--;){
         for(x = sizes[2]; x--;){
            value = get_volume_voxel_value(*vol, z, y, x, 0, 0);
            if((value < floor) || (value > ceil)){
               set_volume_voxel_value(*vol, z, y, x, 0, 0, bg);
               }
            }
         }
      update_progress_report(&progress, z + 1);
      }

   terminate_progress_report(&progress);
   return (vol);
   }

/* pad a volume using the background value */
Volume  *pad(Kernel * K, Volume * vol, double bg)
{
   int      x, y, z;
   int      sizes[MAX_VAR_DIMS];

   get_volume_sizes(*vol, sizes);

   /* z */
   for(y = 0; y < sizes[1]; y++){
      for(x = 0; x < sizes[2]; x++){
         for(z = 0; z < -K->pre_pad[2]; z++){
            set_volume_real_value(*vol, z, y, x, 0, 0, bg);
            }
         for(z = sizes[0] - K->post_pad[2]; z < sizes[0]; z++){
            set_volume_real_value(*vol, z, y, x, 0, 0, bg);
            }
         }
      }

   /* y */
   for(z = 0; z < sizes[0]; z++){
      for(x = 0; x < sizes[2]; x++){
         for(y = 0; y < -K->pre_pad[1]; y++){
            set_volume_real_value(*vol, z, y, x, 0, 0, bg);
            }
         for(y = sizes[1] - K->post_pad[1]; y < sizes[1]; y++){
            set_volume_real_value(*vol, z, y, x, 0, 0, bg);
            }
         }
      }

   /* x */
   for(z = 0; z < sizes[0]; z++){
      for(y = 0; y < sizes[1]; y++){
         for(x = 0; x < -K->pre_pad[0]; x++){
            set_volume_real_value(*vol, z, y, x, 0, 0, bg);
            }
         for(x = sizes[2] - K->post_pad[0]; x < sizes[2]; x++){
            set_volume_real_value(*vol, z, y, x, 0, 0, bg);
            }
         }
      }

   return (vol);
   }

/* perform a dilation on a volume */
Volume  *dilation_kernel(Kernel * K, Volume * vol)
{
   int      x, y, z, c;
   double   value;
   int      sizes[MAX_VAR_DIMS];
   progress_struct progress;
   Volume   tmp_vol;

   if(verbose){
      fprintf(stdout, "Dilation kernel\n");
      }
   get_volume_sizes(*vol, sizes);
   initialize_progress_report(&progress, FALSE, sizes[2], "Dilation");

   /* copy the volume */
   tmp_vol = copy_volume(*vol);

   for(z = -K->pre_pad[2]; z < sizes[0] - K->post_pad[2]; z++){
      for(y = -K->pre_pad[1]; y < sizes[1] - K->post_pad[1]; y++){
         for(x = -K->pre_pad[0]; x < sizes[2] - K->post_pad[0]; x++){

            value = get_volume_real_value(tmp_vol, z, y, x, 0, 0);
            for(c = 0; c < K->nelems; c++){
               if(get_volume_real_value(*vol,
                                        z + K->K[c][2],
                                        y + K->K[c][1],
                                        x + K->K[c][0],
                                        0 + K->K[c][3], 0 + K->K[c][4]) < value){
                  set_volume_real_value(*vol,
                                        z + K->K[c][2],
                                        y + K->K[c][1],
                                        x + K->K[c][0],
                                        0 + K->K[c][3],
                                        0 + K->K[c][4], value * K->K[c][5]);
                  }
               }
            }
         }

      update_progress_report(&progress, z + 1);
      }

   delete_volume(tmp_vol);
   terminate_progress_report(&progress);
   return (vol);
   }

/* perform a median kernel operation on a volume */
Volume  *median_dilation_kernel(Kernel * K, Volume * vol)
{
   int      x, y, z, c, i;
   int      sizes[MAX_VAR_DIMS];
   progress_struct progress;
   Volume   tmp_vol;
   double   value;

   unsigned int kvalue;
   unsigned int neighbours[K->nelems];

   if(verbose){
      fprintf(stdout, "Median Dilation kernel\n");
      }
   get_volume_sizes(*vol, sizes);
   initialize_progress_report(&progress, FALSE, sizes[2], "Median Dilation");

   /* copy the volume */
   tmp_vol = copy_volume(*vol);

   for(z = -K->pre_pad[2]; z < sizes[0] - K->post_pad[2]; z++){
      for(y = -K->pre_pad[1]; y < sizes[1] - K->post_pad[1]; y++){
         for(x = -K->pre_pad[0]; x < sizes[2] - K->post_pad[0]; x++){

            /* only modify background voxels */
            value = get_volume_voxel_value(tmp_vol, z, y, x, 0, 0);
            if(value == 0.0){

               i = 0;
               for(c = 0; c < K->nelems; c++){

                  kvalue = (unsigned int)get_volume_voxel_value(tmp_vol,
                                                                z + K->K[c][2],
                                                                y + K->K[c][1],
                                                                x + K->K[c][0],
                                                                0 + K->K[c][3],
                                                                0 + K->K[c][4]);
                  if(kvalue != 0){
                     neighbours[i] = kvalue;
                     i++;
                     }
                  }

               /* only run this for adjacent voxels */
               if(i > 0){

                  /* find the median of our little array */
                  qsort(&neighbours[0], (size_t) i, sizeof(unsigned int), &compare_ints);

                  /* store the median value */
                  set_volume_voxel_value(*vol, z, y, x, 0, 0,
                                         (double)neighbours[(int)floor((i - 1) / 2)]);
                  }
               }

            /* else just copy the original value over */
            else {
               set_volume_voxel_value(*vol, z, y, x, 0, 0, value);
               }
            }
         }

      update_progress_report(&progress, z + 1);
      }

   delete_volume(tmp_vol);
   terminate_progress_report(&progress);
   return (vol);
   }

/* perform an erosion on a volume */
Volume  *erosion_kernel(Kernel * K, Volume * vol)
{
   int      x, y, z, c;
   double   value;
   int      sizes[MAX_VAR_DIMS];
   progress_struct progress;
   Volume   tmp_vol;

   if(verbose){
      fprintf(stdout, "Erosion kernel\n");
      }
   get_volume_sizes(*vol, sizes);
   initialize_progress_report(&progress, FALSE, sizes[2], "Erosion");

   /* copy the volume */
   tmp_vol = copy_volume(*vol);

   for(z = -K->pre_pad[2]; z < sizes[0] - K->post_pad[2]; z++){
      for(y = -K->pre_pad[1]; y < sizes[1] - K->post_pad[1]; y++){
         for(x = -K->pre_pad[0]; x < sizes[2] - K->post_pad[0]; x++){

            value = get_volume_real_value(tmp_vol, z, y, x, 0, 0);
            for(c = 0; c < K->nelems; c++){
               if(get_volume_real_value(*vol,
                                        z + K->K[c][2],
                                        y + K->K[c][1],
                                        x + K->K[c][0],
                                        0 + K->K[c][3], 0 + K->K[c][4]) > value){
                  set_volume_real_value(*vol,
                                        z + K->K[c][2],
                                        y + K->K[c][1],
                                        x + K->K[c][0],
                                        0 + K->K[c][3],
                                        0 + K->K[c][4], value * K->K[c][5]);
                  }
               }

            value = get_volume_real_value(tmp_vol, z, y, x, 0, 0);
            }
         }
      update_progress_report(&progress, z + 1);
      }

   delete_volume(tmp_vol);
   terminate_progress_report(&progress);
   return (vol);
   }

/* convolve a volume with a input kernel */
Volume  *convolve_kernel(Kernel * K, Volume * vol)
{
   int      x, y, z, c;
   double   value;
   int      sizes[MAX_VAR_DIMS];
   progress_struct progress;
   Volume   tmp_vol;

   if(verbose){
      fprintf(stdout, "Convolve kernel\n");
      }
   get_volume_sizes(*vol, sizes);
   initialize_progress_report(&progress, FALSE, sizes[2], "Convolve");

   /* copy the volume */
   tmp_vol = copy_volume(*vol);

   for(z = -K->pre_pad[2]; z < sizes[0] - K->post_pad[2]; z++){
      for(y = -K->pre_pad[1]; y < sizes[1] - K->post_pad[1]; y++){
         for(x = -K->pre_pad[0]; x < sizes[2] - K->post_pad[0]; x++){

            value = 0;
            for(c = 0; c < K->nelems; c++){
               value += get_volume_real_value(tmp_vol,
                                              z + K->K[c][2],
                                              y + K->K[c][1],
                                              x + K->K[c][0], 0 + K->K[c][3],
                                              0 + K->K[c][4]) * K->K[c][5];
               }
            set_volume_real_value(*vol, z, y, x, 0, 0, value);
            }
         }

      update_progress_report(&progress, z + 1);
      }

   delete_volume(tmp_vol);
   terminate_progress_report(&progress);
   return (vol);
   }

/* should really only work on binary images    */
/* from the original 2 pass Borgefors alg      */
Volume  *distance_kernel(Kernel * K, Volume * vol, double bg)
{
   int      x, y, z, c;
   double   value, min;
   int      sizes[MAX_VAR_DIMS];
   progress_struct progress;
   Kernel  *k1, *k2;

   /* split the Kernel */
   k1 = new_kernel(K->nelems);
   k2 = new_kernel(K->nelems);
   split_kernel(K, k1, k2);

   setup_pad_values(k1);
   setup_pad_values(k2);

   if(verbose){
      fprintf(stdout, "Distance kernel - background %g\n", bg);
      fprintf(stdout, "forward direction kernel:\n");
      print_kernel(k1);
      fprintf(stdout, "\nreverse direction kernel:\n");
      print_kernel(k2);
      }

   get_volume_sizes(*vol, sizes);
   initialize_progress_report(&progress, FALSE, sizes[2] * 2, "Distance");

   /* forward raster direction */
   for(z = -K->pre_pad[2]; z < sizes[0] - K->post_pad[2]; z++){
      for(y = -K->pre_pad[1]; y < sizes[1] - K->post_pad[1]; y++){
         for(x = -K->pre_pad[0]; x < sizes[2] - K->post_pad[0]; x++){

            if(get_volume_real_value(*vol, z, y, x, 0, 0) != bg){

               /* find the minimum */
               min = DBL_MAX;
               for(c = 0; c < k1->nelems; c++){
                  value = get_volume_real_value(*vol,
                                                z + k1->K[c][2],
                                                y + k1->K[c][1],
                                                x + k1->K[c][0],
                                                0 + k1->K[c][3], 0 + k1->K[c][4]) + 1;
                  if(value < min){
                     min = value;
                     }
                  }

               set_volume_real_value(*vol, z, y, x, 0, 0, min);
               }
            }
         }
      update_progress_report(&progress, z + 1);
      }

   /* reverse raster direction */
   for(z = sizes[0] - k2->post_pad[2] - 1; z >= -k2->pre_pad[2]; z--){
      for(y = sizes[1] - k2->post_pad[1] - 1; y >= -k2->pre_pad[1]; y--){
         for(x = sizes[2] - k2->post_pad[0] - 1; x >= -k2->pre_pad[0]; x--){

            min = get_volume_real_value(*vol, z, y, x, 0, 0);
            if(min != bg){

               /* find the minimum distance to bg in the neighbouring vectors */
               for(c = 0; c < k2->nelems; c++){
                  value = get_volume_real_value(*vol,
                                                z + k2->K[c][2],
                                                y + k2->K[c][1],
                                                x + k2->K[c][0],
                                                0 + k2->K[c][3], 0 + k2->K[c][4]) + 1;
                  if(value < min){
                     min = value;
                     }
                  }

               set_volume_real_value(*vol, z, y, x, 0, 0, min);
               }
            }
         }
      update_progress_report(&progress, sizes[2] + z + 1);
      }

   free(k1);
   free(k2);
   terminate_progress_report(&progress);
   return (vol);
   }

/* do connected components labelling on a volume */
/* resulting groups are sorted WRT size          */
Volume  *group_kernel(Kernel * K, Volume * vol, double bg)
{
   int      x, y, z;
   int      sizes[MAX_VAR_DIMS];
   progress_struct progress;
   Volume   tmp_vol;
   Kernel  *k1, *k2;

   unsigned int *equiv;
   unsigned int *counts;
   unsigned int *trans;
   unsigned int neighbours[K->nelems];

   /* counters */
   unsigned int c;
   unsigned int value;
   unsigned int group_idx;             /* label for the next group     */
   unsigned int num_groups;

   unsigned int min_label;
   unsigned int curr_label;
   unsigned int prev_label;
   unsigned int num_matches;

   /* structure for group data */
   Group_info *group_data;

   /* split the Kernel into forward and backwards kernels */
   k1 = new_kernel(K->nelems);
   k2 = new_kernel(K->nelems);
   split_kernel(K, k1, k2);

   setup_pad_values(k1);
   setup_pad_values(k2);

   if(verbose){
      fprintf(stdout, "Group kernel - background %g\n", bg);
      fprintf(stdout, "forward direction kernel:\n");
      print_kernel(k1);
      fprintf(stdout, "\nreverse direction kernel:\n");
      print_kernel(k2);
      }

   get_volume_sizes(*vol, sizes);
   initialize_progress_report(&progress, FALSE, sizes[2], "Groups");

   /* copy and then zero out the original volume */
   tmp_vol = copy_volume(*vol);
   for(z = sizes[0]; z--;){
      for(y = sizes[1]; y--;){
         for(x = sizes[2]; x--;){
            set_volume_voxel_value(*vol, z, y, x, 0, 0, 0);
            }
         }
      }

   /* pass 1 - forward direction (we assume a symmetric kernel) */

   /* our first group is given the label 1 */
   group_idx = 1;

   /* initialise the equiv and counts arrays */
   SET_ARRAY_SIZE(equiv, 0, group_idx, 500);
   equiv[0] = 0;

   SET_ARRAY_SIZE(counts, 0, group_idx, 500);
   counts[0] = 0;

   for(z = -k1->pre_pad[2]; z < sizes[0] - k1->post_pad[2]; z++){
      for(y = -k1->pre_pad[1]; y < sizes[1] - k1->post_pad[1]; y++){
         for(x = -k1->pre_pad[0]; x < sizes[2] - k1->post_pad[0]; x++){

            if(get_volume_voxel_value(tmp_vol, z, y, x, 0, 0) != bg){

               /* search this voxels neighbours */
               num_matches = 0;
               min_label = INT_MAX;

               for(c = 0; c < k1->nelems; c++){
                  value = (unsigned int)get_volume_voxel_value(*vol,
                                                               z + k1->K[c][2],
                                                               y + k1->K[c][1],
                                                               x + k1->K[c][0],
                                                               0 + k1->K[c][3],
                                                               0 + k1->K[c][4]);
                  if(value != 0){
                     if(value < min_label){
                        min_label = value;
                        }
                     neighbours[num_matches] = value;
                     num_matches++;
                     }
                  }

               switch (num_matches){
               case 0:
                  /* no neighbours, make a new label and increment */
                  set_volume_voxel_value(*vol, z, y, x, 0, 0, (Real) group_idx);

                  SET_ARRAY_SIZE(equiv, group_idx, group_idx + 1, 500);
                  equiv[group_idx] = group_idx;

                  SET_ARRAY_SIZE(counts, group_idx, group_idx + 1, 500);
                  counts[group_idx] = 1;

                  group_idx++;
                  break;

               case 1:
                  /* only one neighbour, no equivalences needed */
                  set_volume_voxel_value(*vol, z, y, x, 0, 0, (Real) min_label);
                  counts[min_label]++;
                  break;

               default:
                  /* more than one neighbour */

                  /* first sort the neighbours array */
                  qsort(&neighbours[0], (size_t) num_matches, sizeof(unsigned int),
                        &compare_ints);

                  /* find the minimum possible label for this voxel,    */
                  /* this is done by descending through each neighbours */
                  /* equivalences until an equivalence equal to itself  */
                  /* is found                                           */
                  prev_label = -1;
                  for(c = 0; c < num_matches; c++){
                     curr_label = neighbours[c];

                     /* recurse this label if we haven't yet */
                     if(curr_label != prev_label){
                        while(equiv[curr_label] != equiv[equiv[curr_label]]){
                           curr_label = equiv[curr_label];
                           }

                        /* check against the current minimum value */
                        if(equiv[curr_label] < min_label){
                           min_label = equiv[curr_label];
                           }
                        }

                     prev_label = neighbours[c];
                     }

                  /* repeat, setting equivalences to the min_label */
                  prev_label = -1;
                  for(c = 0; c < num_matches; c++){
                     curr_label = neighbours[c];

                     if(curr_label != prev_label){
                        while(equiv[curr_label] != equiv[equiv[curr_label]]){
                           curr_label = equiv[curr_label];

                           equiv[curr_label] = min_label;
                           }

                        /* set the label itself */
                        if(equiv[neighbours[c]] != min_label){
                           equiv[neighbours[c]] = min_label;
                           }
                        }

                     prev_label = neighbours[c];
                     }

                  /* finally set the voxel in question to the minimum value */
                  set_volume_voxel_value(*vol, z, y, x, 0, 0, (Real) min_label);
                  counts[min_label]++;
                  break;
                  }                       /* end case */

               }
            }
         }
      update_progress_report(&progress, z + 1);
      }
   terminate_progress_report(&progress);

   /* reduce the equiv and counts array */
   num_groups = 0;
   for(c = 0; c < group_idx; c++){

      /* if this equivalence is not resolved yet */
      if(c != equiv[c]){

         /* find the min label value */
         min_label = equiv[c];
         while(min_label != equiv[min_label]){
            min_label = equiv[min_label];
            }

         /* update the label and its counters */
         equiv[c] = min_label;
         counts[min_label] += counts[c];
         counts[c] = 0;
         }
      else {
         num_groups++;
         }
      }

   /* Allocate space for the array of groups */
   group_data = (Group_info *) malloc(num_groups * sizeof(Group_info));

   num_groups = 0;
   for(c = 0; c < group_idx; c++){
      if(counts[c] > 0){
         /* allocate space for this element */
         group_data[num_groups] = malloc(sizeof(group_info_struct));

         group_data[num_groups]->orig_label = equiv[c];
         group_data[num_groups]->count = counts[c];
         num_groups++;
         }
      }

   /* sort the groups by the count size */
   if(verbose){
      fprintf(stdout, "Found %d unique groups from %d, sorting...\n", num_groups,
              group_idx);
      }
   qsort(group_data, num_groups, sizeof(Group_info), &compare_groups);

   /* set up the transpose array */
   trans = (unsigned int *)malloc(sizeof(unsigned int) * group_idx);
   for(c = 0; c < num_groups; c++){
      trans[group_data[c]->orig_label] = c + 1; /* +1 to bump past 0 */
      }

   /* pass 2 - resolve equivalences in the output data */
   if(verbose){
      fprintf(stdout, "Resolving equivalences...\n");
      }
   for(z = sizes[0]; z--;){
      for(y = sizes[1]; y--;){
         for(x = sizes[2]; x--;){
            value = (unsigned int)get_volume_voxel_value(*vol, z, y, x, 0, 0);
            if(value != 0){
               value = trans[equiv[value]];
               set_volume_voxel_value(*vol, z, y, x, 0, 0, (Real) value);
               }
            }
         }
      }

   /* tidy up */
   delete_volume(tmp_vol);
   for(c = 0; c < num_groups; c++){
      free(group_data[c]);
      }
   free(group_data);
   free(trans);
   free(k1);
   free(k2);

   return (vol);
   }

/* do local correlation to another volume                    */
/* xcorr = sum((a*b)^2) / (sqrt(sum(a^2)) * sqrt(sum(b^2))   */
VIO_Volume *lcorr_kernel(Kernel * K, VIO_Volume * vol, VIO_Volume *cmp)
{
   int      x, y, z, c;
   double   value, v1, v2;
   double   ssum_v1, ssum_v2, sum_prd, denom;
   int      sizes[MAX_VAR_DIMS];
   progress_struct progress;
   Volume   tmp_vol;
   
   if(verbose){
      fprintf(stdout, "Local Correlation kernel\n");
      }
   get_volume_sizes(*vol, sizes);
   initialize_progress_report(&progress, FALSE, sizes[2], "Local Correlation");

   /* copy the volume */
   tmp_vol = copy_volume(*vol);
   
   /* zero the output volume */
   for(z = sizes[0]; z--;){
      for(y = sizes[1]; y--;){
         for(x = sizes[2]; x--;){
            set_volume_voxel_value(*vol, z, y, x, 0, 0, 0);
            }
         }
      }
   
   /* set output range */
   set_volume_real_range(*vol, 0.0, 1.0);
   
   for(z = -K->pre_pad[2]; z < sizes[0] - K->post_pad[2]; z++){
      for(y = -K->pre_pad[1]; y < sizes[1] - K->post_pad[1]; y++){
         for(x = -K->pre_pad[0]; x < sizes[2] - K->post_pad[0]; x++){
            
            /* init counters */
            ssum_v1 = ssum_v2 = sum_prd = 0;
            for(c = 0; c < K->nelems; c++){
               v1 = get_volume_real_value(tmp_vol,
                                          z + K->K[c][2],
                                          y + K->K[c][1],
                                          x + K->K[c][0], 0 + K->K[c][3],
                                          0 + K->K[c][4]) * K->K[c][5];
               v2 = get_volume_real_value(*cmp,
                                          z + K->K[c][2],
                                          y + K->K[c][1],
                                          x + K->K[c][0], 0 + K->K[c][3],
                                          0 + K->K[c][4]) * K->K[c][5];
               
               /* increment counters */
               ssum_v1 += v1*v1;
               ssum_v2 += v2*v2;
               sum_prd += v1*v2;
               }
            
            denom = sqrt(ssum_v1 * ssum_v2);
            value = (denom == 0.0) ? 0.0 : sum_prd / denom;
            
            set_volume_real_value(*vol, z, y, x, 0, 0, value);
            }
         }
      update_progress_report(&progress, z + 1);
      }
   terminate_progress_report(&progress);
   
   /* tidy up */
   delete_volume(tmp_vol);
   
   return (vol);
   }
