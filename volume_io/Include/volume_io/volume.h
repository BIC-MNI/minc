#ifndef  DEF_VOLUME
#define  DEF_VOLUME

#include  <def_mni.h>

typedef  enum  { MNC_FORMAT, FREE_FORMAT }       Volume_file_types;
typedef  enum  { UNSIGNED_BYTE, UNSIGNED_SHORT } Data_types;

typedef  unsigned  char  Volume_type;

typedef  struct
{
    String          filename;
    Data_types      data_type;

    unsigned char   ***byte_data;
    unsigned short  ***short_data;

    Real            value_scale;
    Real            value_translation;
    int             sizes[N_DIMENSIONS];
    Real            thickness[N_DIMENSIONS];
    Boolean         flip_axis[N_DIMENSIONS];
    Transform       world_to_voxel_transform;
    Transform       voxel_to_world_transform;
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

typedef  struct
{
    Volume_file_types  file_type;

    int                sizes_in_file[N_DIMENSIONS];
    int                axis_index[N_DIMENSIONS];
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

} volume_input_struct;

typedef struct
{
    int            x, y;
    volume_struct  *src_volume;
    volume_struct  *dest_volume;
    Transform      transform;
} resample_struct;

#endif
