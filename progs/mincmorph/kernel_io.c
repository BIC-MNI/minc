/* kernel_io.c - reads kernel files */

#include <volume_io.h>
#include "kernel_io.h"
#define MAX_KERNEL_ELEMS 1000

extern int verbose;

static const STRING KERNEL_FILE_HEADER = "MNI Morphology Kernel File";
static const STRING KERNEL_TYPE = "Kernel_Type";
static const STRING NORMAL_KERNEL = "Normal_Kernel";
static const STRING KERNEL = "Kernel";

/* inbuilt kernels */
int      n_inbuilt_kern = 4;

/* returns a new Kernel struct (pointer)                */
Kernel  *new_kernel(int nelems)
{
   int      i, j;
   Kernel  *tmp;

   ALLOC_VAR_SIZED_STRUCT(tmp, Real, 10);

   /* allocate and initialise the Kernel Array */
   SET_ARRAY_SIZE(tmp->K, 0, nelems, 10);
   for(i = 0; i < nelems; i++){
      ALLOC(tmp->K[i], KERNEL_DIMS + 1);

      for(j = 0; j < KERNEL_DIMS; j++){
         tmp->K[i][j] = 0.0;
         }
      tmp->K[i][KERNEL_DIMS] = 1.0;

      }
   tmp->nelems = nelems;

   return tmp;
   }

/* reads in a Kernel from a file                        */
Status input_kernel(const char *kernel_file, Kernel * kernel)
{
   int      i, j;

   STRING   line;
   STRING   type_name;
   STRING   str;
   Real     tmp_real;
   FILE    *file;

   /* parameter checking */
   if(kernel_file == NULL){
      print_error("input_kernel(): passed NULL FILE.\n");
      return (ERROR);
      }

   file = fopen(kernel_file, "r");
   if(file == NULL){
      print_error("input_kernel(): error opening Kernel file.\n");
      return (ERROR);
      }

   /* okay read the header */
   if(mni_input_string(file, &line, (char)0, (char)0) != OK){
      delete_string(line);
      print_error("input_kernel(): could not read header in file.\n");
      return (ERROR);
      }

   if(!equal_strings(line, KERNEL_FILE_HEADER)){
      delete_string(line);
      print_error("input_kernel(): invalid header in file.\n");
      return (ERROR);
      }

   /* --- read the type of Kernel */
   if(mni_input_keyword_and_equal_sign(file, KERNEL_TYPE, FALSE) != OK){
      return (ERROR);
      }

   if(mni_input_string(file, &type_name, (char)';', (char)0) != OK){
      print_error("input_kernel(): missing kernel type.\n");
      return (ERROR);
      }

   if(mni_skip_expected_character(file, (char)';') != OK){
      return (ERROR);
      }

   if(!equal_strings(type_name, NORMAL_KERNEL)){
      print_error("input_kernel(): invalid kernel type.\n");
      delete_string(type_name);
      return (ERROR);
      }
   delete_string(type_name);

   /* --- read the next string */
   if(mni_input_string(file, &str, (char)'=', (char)0) != OK)
      return (ERROR);

   if(!equal_strings(str, KERNEL)){
      print_error("Expected %s =\n", KERNEL);
      delete_string(str);
      return (ERROR);
      }
   delete_string(str);

   if(mni_skip_expected_character(file, (char)'=') != OK){
      return (ERROR);
      }

   /* now read the elements (lines) of the kernel */
   if(verbose){
      fprintf(stderr, "Reading [%s]", kernel_file);
      }
   for(i = 0; i < MAX_KERNEL_ELEMS; i++){

      /* allocate a bit of memory */
      SET_ARRAY_SIZE(kernel->K, kernel->nelems, kernel->nelems + 1, 10);
      ALLOC(kernel->K[i], KERNEL_DIMS + 1);

      /* get the 5 dimension vectors and the coefficient */
      for(j = 0; j < 6; j++){
         if(mni_input_real(file, &tmp_real) == OK){
            kernel->K[i][j] = tmp_real;
            }
         else {
            /* check for end */
            if(mni_skip_expected_character(file, (char)';') == OK){
               kernel->nelems = i;
               if(verbose){
                  fprintf(stderr, " %dx%d Kernel elements read\n", i, kernel->nelems);
                  }
               return (OK);
               }
            else {
               print_error("input_kernel(): error reading kernel [%d,%d]\n", i + 1,
                           j + 1);
               return (ERROR);
               }
            }
         }
      kernel->nelems++;

      if(verbose){
         fprintf(stderr, ".");
         fflush(stderr);
         }
      }

   /* SHOLDN'T BE REACHED */
   print_error("input_kernel(): Glark! Something is amiss in the State of Kansas\n");
   return (ERROR);
   }

/* pretty print a kernel */
int print_kernel(Kernel * kernel)
{
   int      i, j;

   fprintf(stderr, "           x       y       z       t       v   coeff\n");
   fprintf(stderr, "     -----------------------------------------------\n");
   for(i = 0; i < kernel->nelems; i++){
      fprintf(stderr, "[%02d]", i);
      for(j = 0; j < KERNEL_DIMS + 1; j++){
         fprintf(stderr, "%8.02f", kernel->K[i][j]);
         }
      fprintf(stderr, "\n");
      }

   return (TRUE);
   }

int setup_pad_values(Kernel * kernel)
{
   int      c, n;

   /* init padding values */
   for(n = 0; n < KERNEL_DIMS; n++){
      kernel->pre_pad[n] = 0;
      kernel->post_pad[n] = 0;
      }

   /* find the padding sizes */
   for(c = 0; c < kernel->nelems; c++){
      for(n = 0; n < KERNEL_DIMS; n++){
         if(kernel->K[c][n] < kernel->pre_pad[n]){
            kernel->pre_pad[n] = kernel->K[c][n];
            }

         if(kernel->K[c][n] > kernel->post_pad[n]){
            kernel->post_pad[n] = kernel->K[c][n];
            }
         }
      }

   return (TRUE);
   }

/* 2D 4 connectivity kernel                              */
/*            x       y       z       t       v   coeff  */
/*      -----------------------------------------------  */
/* [00]    1.00    0.00    0.00    0.00    0.00    1.00  */
/* [01]   -1.00    0.00    0.00    0.00    0.00    1.00  */
/* [02]    0.00    1.00    0.00    0.00    0.00    1.00  */
/* [03]    0.00   -1.00    0.00    0.00    0.00    1.00  */
Kernel  *get_2D04_kernel(void)
{
   Kernel  *K = new_kernel(4);

   K->K[0][0] = 1.0;
   K->K[1][0] = -1.0;

   K->K[2][1] = 1.0;
   K->K[3][1] = -1.0;

   return K;
   }

/* 2D 8 connectivity kernel                              */
/*            x       y       z       t       v   coeff  */
/*      -----------------------------------------------  */
/* [00]    1.00    1.00    0.00    0.00    0.00    1.00  */
/* [01]    1.00    0.00    0.00    0.00    0.00    1.00  */
/* [02]    1.00   -1.00    0.00    0.00    0.00    1.00  */
/*                                                       */
/* [03]    0.00    1.00    0.00    0.00    0.00    1.00  */
/* [04]    0.00   -1.00    0.00    0.00    0.00    1.00  */
/*                                                       */
/* [05]   -1.00    1.00    0.00    0.00    0.00    1.00  */
/* [06]   -1.00    0.00    0.00    0.00    0.00    1.00  */
/* [07]   -1.00   -1.00    0.00    0.00    0.00    1.00  */
Kernel  *get_2D08_kernel(void)
{
   Kernel  *K = new_kernel(8);

   K->K[0][0] = 1.0;
   K->K[0][1] = 1.0;

   K->K[1][0] = 1.0;

   K->K[2][0] = 1.0;
   K->K[2][1] = -1.0;

   K->K[3][1] = 1.0;

   K->K[4][1] = -1.0;

   K->K[5][0] = -1.0;
   K->K[5][1] = 1.0;

   K->K[6][0] = -1.0;

   K->K[7][0] = -1.0;
   K->K[7][1] = -1.0;

   return K;
   }

/* 3D 6 connectivity kernel                              */
/*            x       y       z       t       v   coeff  */
/*      -----------------------------------------------  */
/* [00]    1.00    0.00    0.00    0.00    0.00    1.00  */
/* [01]   -1.00    0.00    0.00    0.00    0.00    1.00  */
/* [02]    0.00    1.00    0.00    0.00    0.00    1.00  */
/* [03]    0.00   -1.00    0.00    0.00    0.00    1.00  */
/* [04]    0.00    0.00    1.00    0.00    0.00    1.00  */
/* [05]    0.00    0.00   -1.00    0.00    0.00    1.00  */
Kernel  *get_3D06_kernel(void)
{
   Kernel  *K = new_kernel(6);

   K->K[0][0] = 1.0;
   K->K[1][0] = -1.0;

   K->K[2][1] = 1.0;
   K->K[3][1] = -1.0;

   K->K[4][2] = 1.0;
   K->K[5][2] = -1.0;

   return K;
   }

/* 3D 26 connectivity kernel                             */
/*            x       y       z       t       v   coeff  */
/*      -----------------------------------------------  */
/* [00]    1.00    1.00    1.00    0.00    0.00    1.00  */
/* [01]    1.00    1.00    0.00    0.00    0.00    1.00  */
/* [02]    1.00    1.00   -1.00    0.00    0.00    1.00  */

/* [03]    1.00    0.00    1.00    0.00    0.00    1.00  */
/* [04]    1.00    0.00    0.00    0.00    0.00    1.00  */
/* [05]    1.00    0.00   -1.00    0.00    0.00    1.00  */

/* [06]    1.00   -1.00    1.00    0.00    0.00    1.00  */
/* [07]    1.00   -1.00    0.00    0.00    0.00    1.00  */
/* [08]    1.00   -1.00   -1.00    0.00    0.00    1.00  */

/* [09]    0.00    1.00    1.00    0.00    0.00    1.00  */
/* [10]    0.00    1.00    0.00    0.00    0.00    1.00  */
/* [11]    0.00    1.00   -1.00    0.00    0.00    1.00  */

/* [12]    0.00    0.00    1.00    0.00    0.00    1.00  */
/* ----    0.00    0.00    0.00    0.00    0.00    ----  */
/* [13]    0.00    0.00   -1.00    0.00    0.00    1.00  */

/* [14]    0.00   -1.00    1.00    0.00    0.00    1.00  */
/* [15]    0.00   -1.00    0.00    0.00    0.00    1.00  */
/* [16]    0.00   -1.00   -1.00    0.00    0.00    1.00  */

/* [17]   -1.00    1.00    1.00    0.00    0.00    1.00  */
/* [18]   -1.00    1.00    0.00    0.00    0.00    1.00  */
/* [19]   -1.00    1.00   -1.00    0.00    0.00    1.00  */

/* [20]   -1.00    0.00    1.00    0.00    0.00    1.00  */
/* [21]   -1.00    0.00    0.00    0.00    0.00    1.00  */
/* [22]   -1.00    0.00   -1.00    0.00    0.00    1.00  */

/* [23]   -1.00   -1.00    1.00    0.00    0.00    1.00  */
/* [24]   -1.00   -1.00    0.00    0.00    0.00    1.00  */
/* [25]   -1.00   -1.00   -1.00    0.00    0.00    1.00  */
Kernel  *get_3D26_kernel(void)
{
   Kernel  *K = new_kernel(26);

   K->K[0][0] = 1.0;
   K->K[0][1] = 1.0;
   K->K[0][2] = 1.0;

   K->K[1][0] = 1.0;
   K->K[1][1] = 1.0;

   K->K[2][0] = 1.0;
   K->K[2][1] = 1.0;
   K->K[2][2] = -1.0;

   K->K[3][0] = 1.0;
   K->K[3][2] = 1.0;

   K->K[4][0] = 1.0;

   K->K[5][0] = 1.0;
   K->K[5][2] = -1.0;

   K->K[6][0] = 1.0;
   K->K[6][1] = -1.0;
   K->K[6][2] = 1.0;

   K->K[7][0] = 1.0;
   K->K[7][1] = -1.0;

   K->K[8][0] = 1.0;
   K->K[8][1] = -1.0;
   K->K[8][2] = -1.0;

   K->K[9][1] = 1.0;
   K->K[9][2] = 1.0;

   K->K[10][1] = 1.0;

   K->K[11][1] = 1.0;
   K->K[11][2] = -1.0;

   K->K[12][2] = 1.0;

   K->K[13][2] = -1.0;

   K->K[14][1] = -1.0;
   K->K[14][2] = 1.0;

   K->K[15][1] = -1.0;

   K->K[16][1] = -1.0;
   K->K[16][2] = -1.0;

   K->K[17][0] = -1.0;
   K->K[17][1] = 1.0;
   K->K[17][2] = 1.0;

   K->K[18][0] = -1.0;
   K->K[18][1] = 1.0;

   K->K[19][0] = -1.0;
   K->K[19][1] = 1.0;
   K->K[19][2] = -1.0;

   K->K[20][0] = -1.0;
   K->K[20][2] = 1.0;

   K->K[21][0] = -1.0;

   K->K[22][0] = -1.0;
   K->K[22][2] = -1.0;

   K->K[23][0] = -1.0;
   K->K[23][1] = -1.0;
   K->K[23][2] = 1.0;

   K->K[24][0] = -1.0;
   K->K[24][1] = -1.0;

   K->K[25][0] = -1.0;
   K->K[25][1] = -1.0;
   K->K[25][2] = -1.0;

   return K;
   }
