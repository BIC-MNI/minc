#ifndef  DEF_VOLUME
#define  DEF_VOLUME

#include  <def_mni.h>

/* recognized file formats */

typedef  enum  { MNC_FORMAT, FREE_FORMAT }       Volume_file_types;

/* internal storage of a volume is one of these types */

typedef  enum  { UNSIGNED_BYTE, UNSIGNED_SHORT } Data_types;

/* -------------------------- volume struct --------------------- */

typedef  struct
{
    String          filename;                    /* name of volume file */
    Data_types      data_type;                   /* BYTE or SHORT */

    unsigned char   ***byte_data;                /* only one of these is used */
    unsigned short  ***short_data;               /* at a time (could be union)*/

    Real            value_scale;                 /* if values scaled on input,*/
    Real            value_translation;           /* orig = new * scale + trans*/
    int             sizes[N_DIMENSIONS];         /* n voxels in 3 directions */
    Real            thickness[N_DIMENSIONS];     /* voxel thickness in 3 dirs */
                                                 /* can be + or -ve           */
    Transform       world_to_voxel_transform;    /* transform from world space*/
    Transform       voxel_to_world_transform;    /* inverse of above */

    int             axis_index_from_file[N_DIMENSIONS];
} volume_struct;

#define  ASSIGN_VOLUME_DATA( volume, x, y, z, val )                           \
         if( (volume).data_type == UNSIGNED_BYTE )                            \
             (volume).byte_data[x][y][z] = (unsigned char) (val);             \
         else                                                                 \
             (volume).short_data[x][y][z] = (unsigned short) (val)

#define  GET_VOLUME_DATA( volume, x, y, z )                                   \
         ( ((volume).data_type == UNSIGNED_BYTE) ?                            \
             ((volume).byte_data[x][y][z]) : ((volume).short_data[x][y][z]) )

#define  GET_VOLUME_DATA_PTR( volume, x, y, z )                               \
         ( ((volume).data_type == UNSIGNED_BYTE) ?                            \
             ((void *)(&(volume).byte_data[x][y][z])) :                       \
             ((void *)(&(volume).short_data[x][y][z])) )

/* -------------- volume input struct (during input only) -------------- */

typedef  struct
{
    Volume_file_types  file_type;

    int                sizes_in_file[N_DIMENSIONS];
    int                slice_index;

    unsigned char      *byte_slice_buffer;
    unsigned short     *short_slice_buffer;

/* for mnc files only */

    int                cdfid;
    int                icv;

/* for free format files only */

    FILE               *volume_file;
    Data_types         file_data_type;
    Boolean            convert_to_byte;
    Boolean            one_file_per_slice;
    String             *slice_filenames;
    int                *slice_byte_offsets;

} volume_input_struct;

/* --------------------- resample struct (used during resampling) ------- */

typedef struct
{
    int            x, y;
    volume_struct  *src_volume;
    volume_struct  *dest_volume;
    Transform      transform;
} resample_struct;

#endif
