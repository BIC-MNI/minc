/* dbh.h - Analyze 7.5 header file                                */
/*                                                                */
/* Compiled by Andrew Janke (rotor@cmr.uq.edu.au)                 */    
/* from http://www.mayo.edu/bir/analyze/AnalyzeFileInfo.html      */
/*      http://homepage2.nifty.com/peco/gpetview/gpetview.html    */
/*      Chris Rorden  - chris.rorden@nottingham.ac.uk             */
/*      Matthew Brett - matthew.brett@mrc-cbu.cam.ac.uk           */


#define DT_NONE           0      /* No data type                                                   */
#define DT_UNKNOWN        0      /* Unknown data type                                              */
#define DT_BINARY         1      /* Binary               ( 1 bit  per voxel)                       */
#define DT_UNSIGNED_CHAR  2      /* Unsigned character   ( 8 bits per voxel)                       */
#define DT_SIGNED_SHORT   4      /* Signed short         (16 bits per voxel)                       */
#define DT_SIGNED_INT     8      /* Signed integer       (32 bits per voxel)                       */
#define DT_FLOAT          16     /* Floating point       (32 bits per voxel)                       */
#define DT_COMPLEX        32     /* Complex              (64 bits per voxel; 2 floating points)    */
#define DT_DOUBLE         64     /* Double precision     (64 bits per voxel)                       */
#define DT_RGB            128    /* Uchar x 3            (24 bits per voxel)                       */
#define DT_ALL            255    /*                                                                */

struct header_key{               /* off + size                                                     */
   int  sizeof_hdr;              /*   0 +  4 - the byte size of the header file                    */
   char data_type[10];           /*   4 + 10 - the data type of the file                           */
   char db_name[18];             /*  14 + 18 -                                                     */
   int  extents;                 /*  32 +  4 - should be 16384                                     */
   short int session_error;      /*  36 +  2 -                                                     */
   char regular;                 /*  38 +  1 - 'r' indicating all images/volumes are the same size */
   char hkey_un0;                /*  39 +  1 -                                                     */
   };                            /* total=40 bytes                                                 */

struct image_dimension{          /* off + size                                                     */
   short int dim[8];             /*   0 + 16 - array of the image dimensions                       */ 
                                 /*              dim[0] # of dimensions in database; usually 4     */
                                 /*              dim[1]    X dim - pixels in an image row          */
                                 /*              dim[2]    Y dim - pixel rows in slice             */
                                 /*              dim[3]    Z dim - slices in a volume              */
                                 /*              dim[4] Time dim - volumes in database             */
   char  vox_units[4];           /*  16 +  4 - specifies the spatial units of measure for a voxel  */
   char  cal_units[8];           /*  20 +  8 - specifies the name of the calibration unit          */
   short int unused1;            /*  28 +  2                                                       */
   short int datatype;           /*  30 +  2 - datatype for this image set                         */
   short int bitpix;             /*  32 +  2 - # of bits per pixel 1, 8, 16, 32, or 64.            */
   short int dim_un0;            /*  34 +  2 -                                                     */
   float pixdim[8];              /*  36 + 32 - pixdim[] specifies the voxel dimensions:            */
                                 /*              pixdim[1] - voxel width                           */
                                 /*              pixdim[2] - voxel height                          */
                                 /*              pixdim[3] - interslice distance                   */
                                 /*              ..etc                                             */
   float vox_offset;             /*  68 +  4 - byte offset in the .img file at which voxels start. */
                                 /*              This value can be negative to specify that the    */
                                 /*              absolute value is applied for every image         */
   float scale_factor;           /*  72 +  4 = funused1; scale factor used by SPM; non standard    */
   float funused1;               /*  76 +  4                                                       */
   float funused2;               /*  80 +  4                                                       */
   float cal_max, cal_min;       /*  84 +  8 - calibrated max and min:                             */
                                 /*               www.mailbase.ac.uk/lists/spm/2000-09/0099.html   */
   float compressed;             /*  92 +  4                                                       */
   float verified;               /*  96 +  4                                                       */
   int   glmax, glmin;           /* 100 +  8 - global max and min pixel values (entire database)   */
   };                            /* total=108 bytes                                                */

struct data_history{             /* off + size                                                     */
   char descrip[80];             /*   0 + 80                                                       */
   char aux_file[24];            /*  80 + 24                                                       */
   char orient;                  /* 104 +  1 - slice orientation for this database |               */
                                 /*              0      transverse unflipped       |               */
                                 /*              1      coronal    unflipped       | disregarded   */
                                 /*              2      sagittal   unflipped       | by SPM        */
                                 /*              3      transverse flipped         |               */
                                 /*              4      coronal    flipped         |               */
                                 /*              5      sagittal   flipped         |               */
   short int originator[5];      /* 105 + 10 - origin                              |               */
                                 /*               originator[0] x-origin           | non standard  */
                                 /*               originator[1] y-origin           | SPM use only  */
                                 /*               originator[2] z-origin           |               */
   char generated[10];           /* 115 + 10                                                       */
   char scannum[10];             /* 125 + 10                                                       */
   char patient_id[10];          /* 135 + 10                                                       */
   char exp_date[10];            /* 145 + 10                                                       */
   char exp_time[10];            /* 155 + 10                                                       */
   char hist_un0[3];             /* 165 +  3                                                       */
   int  views;                   /* 168 +  4                                                       */
   int  vols_added;              /* 172 +  4                                                       */
   int  start_field;             /* 176 +  4                                                       */
   int  field_skip;              /* 180 +  4                                                       */
   int  omax, omin;              /* 184 +  8                                                       */
   int  smax, smin;              /* 192 +  8                                                       */
   };                            /* total=200 bytes                                                */
       
struct analyze75_hdr {
   struct header_key hk;         /*   0 +  40                                                      */
   struct image_dimension dime;  /*  40 + 108                                                      */
   struct data_history hist;     /* 148 + 200                                                      */
   };                            /* total= 348 bytes                                               */   

typedef struct{
	float real;
	float imag;
	} COMPLEX;
