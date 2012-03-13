/* kernel_io.h */

#ifndef KERNEL_IO
#define KERNEL_IO

#define KERNEL_DIMS 5

/* inbuilt kernels */
int      n_inbuilt_kern;

typedef enum {
   K_NULL = 0,
   K_2D04, K_2D08, K_3D06, K_3D26
   } kern_types;

/* Structure for Kernel information */
typedef struct {
   int      nelems;
   int      pre_pad[KERNEL_DIMS];
   int      post_pad[KERNEL_DIMS];
   VIO_Real   **K;
   } Kernel;

/* returns a new B_Matrix struct (pointer) */
Kernel  *new_kernel(int nelems);

/* reads in a B_Matrix from a file (pointer) */
VIO_Status   input_kernel(const char *kernel_file, Kernel * kernel);

/* pretty print a kernel */
int      print_kernel(Kernel * kernel);

/* calculate start and step offsets for this kernel */
int      setup_pad_values(Kernel * kernel);

/* return the default kernel(s) */
Kernel  *get_2D04_kernel(void);
Kernel  *get_2D08_kernel(void);
Kernel  *get_3D06_kernel(void);
Kernel  *get_3D26_kernel(void);

#endif
