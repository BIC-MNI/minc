#ifndef  DEF_VOLUME
#define  DEF_VOLUME

#include  <netcdf.h>
#include  <minc.h>

#define  MAX_DIMENSIONS     5

/* -------------------------- volume struct --------------------- */

#define  LABEL_BIT                 128
#define  ACTIVE_BIT                64
#define  LOWER_AUXILIARY_BITS      63


typedef  enum  { NO_DATA_TYPE,
                 UNSIGNED_BYTE,
                 SIGNED_BYTE,
                 UNSIGNED_SHORT,
                 SIGNED_SHORT,
                 UNSIGNED_LONG,
                 SIGNED_LONG,
                 FLOAT,
                 DOUBLE,
                 MAX_DATA_TYPE }   Data_types;

#define  ANY_SPATIAL_DIMENSION   "any_spatial_dimension"

typedef  struct
{
    int             n_dimensions;
    String          dimension_names[MAX_DIMENSIONS];
    Data_types      data_type;
    nc_type         nc_data_type;
    Boolean         signed_flag;

    void            *data;

    Real            min_voxel;
    Real            max_voxel;
    Real            value_scale;
    Real            value_translation;
    int             sizes[MAX_DIMENSIONS];
    Real            separation[MAX_DIMENSIONS];

    Transform       world_to_voxel_transform;
    Transform       voxel_to_world_transform;

    unsigned char   ***labels;
} volume_struct;

typedef  volume_struct  *Volume;

/* ------------------------- set voxel value ------------------------ */

#define  SET_ONE( volume, type, asterisks, subscripts, value )   \
         (((type asterisks) ((volume)->data))  subscripts = (type) (value))

#define  SET_GIVEN_DIM( volume, asterisks, subscripts, value )   \
         switch( (volume)->data_type )  \
         {  \
         case UNSIGNED_BYTE:  \
             SET_ONE( volume, unsigned char, asterisks, subscripts, value);\
             break;  \
         case SIGNED_BYTE:  \
             SET_ONE( volume, signed char, asterisks, subscripts, value);\
             break;  \
         case UNSIGNED_SHORT:  \
             SET_ONE( volume, unsigned short, asterisks, subscripts, value);\
             break;  \
         case SIGNED_SHORT:  \
             SET_ONE( volume, signed short, asterisks, subscripts, value);\
             break;  \
         case UNSIGNED_LONG:  \
             SET_ONE( volume, unsigned long, asterisks, subscripts, value);\
             break;  \
         case SIGNED_LONG:  \
             SET_ONE( volume, signed long, asterisks, subscripts, value);\
             break;  \
         case FLOAT:  \
             SET_ONE( volume, float, asterisks, subscripts, value);\
             break;  \
         case DOUBLE:  \
             SET_ONE( volume, double, asterisks, subscripts, value);\
             break;  \
         }

#define  SET_VOXEL_1D( volume, x, value )       \
           SET_GIVEN_DIM( volume, *, [x], value )

#define  SET_VOXEL_2D( volume, x, y, value )       \
           SET_GIVEN_DIM( volume, **, [x][y], value )

#define  SET_VOXEL_3D( volume, x, y, z, value )       \
           SET_GIVEN_DIM( volume, ***, [x][y][z], value )

#define  SET_VOXEL_4D( volume, x, y, z, t, value )       \
           SET_GIVEN_DIM( volume, ****, [x][y][z][t], value )

#define  SET_VOXEL_5D( volume, x, y, z, t, v, value )       \
           SET_GIVEN_DIM( volume, *****, [x][y][z][t][v], value )

#define  SET_VOXEL( volume, x, y, z, t, v, value )       \
         switch( (volume)->n_dimensions ) \
         { \
         case 1:  SET_VOXEL_1D( volume, x, value );              break; \
         case 2:  SET_VOXEL_2D( volume, x, y, value );           break; \
         case 3:  SET_VOXEL_3D( volume, x, y, z, value );        break; \
         case 4:  SET_VOXEL_4D( volume, x, y, z, t, value );     break; \
         case 5:  SET_VOXEL_5D( volume, x, y, z, t, v, value );  break; \
         }

/* ------------------------- get voxel value ------------------------ */

#define  GET_ONE( value, volume, type, asterisks, subscripts )   \
         (value) = (((type asterisks) ((volume)->data))  subscripts)

#define  GET_GIVEN_DIM( value, volume, asterisks, subscripts )   \
         switch( (volume)->data_type )  \
         {  \
         case UNSIGNED_BYTE:  \
             GET_ONE( value, volume, unsigned char, asterisks, subscripts );\
             break;  \
         case SIGNED_BYTE:  \
             GET_ONE( value, volume, signed char, asterisks, subscripts );\
             break;  \
         case UNSIGNED_SHORT:  \
             GET_ONE( value, volume, unsigned short, asterisks, subscripts );\
             break;  \
         case SIGNED_SHORT:  \
             GET_ONE( value, volume, signed short, asterisks, subscripts );\
             break;  \
         case UNSIGNED_LONG:  \
             GET_ONE( value, volume, unsigned long, asterisks, subscripts );\
             break;  \
         case SIGNED_LONG:  \
             GET_ONE( value, volume, signed long, asterisks, subscripts );\
             break;  \
         case FLOAT:  \
             GET_ONE( value, volume, float, asterisks, subscripts );\
             break;  \
         case DOUBLE:  \
             GET_ONE( value, volume, double, asterisks, subscripts );\
             break;  \
         }

#define  GET_VOXEL_1D( value, volume, x )       \
           GET_GIVEN_DIM( value, volume, *, [x] )

#define  GET_VOXEL_2D( value, volume, x, y )       \
           GET_GIVEN_DIM( value, volume, **, [x][y] )

#define  GET_VOXEL_3D( value, volume, x, y, z )       \
           GET_GIVEN_DIM( value, volume, ***, [x][y][z] )

#define  GET_VOXEL_4D( value, volume, x, y, z, t )       \
           GET_GIVEN_DIM( value, volume, ****, [x][y][z][t] )

#define  GET_VOXEL_5D( value, volume, x, y, z, t, v )       \
           GET_GIVEN_DIM( value, volume, *****, [x][y][z][t][v] )

/* ------------------------- get voxel ptr ------------------------ */

#define  GET_ONE_PTR( ptr, volume, type, asterisks, subscripts )   \
         (ptr) = (void *) (&(((type asterisks) ((volume)->data))  subscripts))

#define  GET_GIVEN_DIM_PTR( ptr, volume, asterisks, subscripts )   \
         switch( (volume)->data_type )  \
         {  \
         case UNSIGNED_BYTE:  \
             GET_ONE_PTR( ptr, volume, unsigned char, asterisks, subscripts );\
             break;  \
         case SIGNED_BYTE:  \
             GET_ONE_PTR( ptr, volume, signed char, asterisks, subscripts );\
             break;  \
         case UNSIGNED_SHORT:  \
             GET_ONE_PTR( ptr, volume, unsigned short, asterisks, subscripts );\
             break;  \
         case SIGNED_SHORT:  \
             GET_ONE_PTR( ptr, volume, signed short, asterisks, subscripts );\
             break;  \
         case UNSIGNED_LONG:  \
             GET_ONE_PTR( ptr, volume, unsigned long, asterisks, subscripts );\
             break;  \
         case SIGNED_LONG:  \
             GET_ONE_PTR( ptr, volume, signed long, asterisks, subscripts );\
             break;  \
         case FLOAT:  \
             GET_ONE_PTR( ptr, volume, float, asterisks, subscripts );\
             break;  \
         case DOUBLE:  \
             GET_ONE_PTR( ptr, volume, double, asterisks, subscripts );\
             break;  \
         }

#define  GET_VOXEL_PTR_1D( ptr, volume, x )       \
           GET_GIVEN_DIM_PTR( ptr, volume, *, [x] )

#define  GET_VOXEL_PTR_2D( ptr, volume, x, y )       \
           GET_GIVEN_DIM_PTR( ptr, volume, **, [x][y] )

#define  GET_VOXEL_PTR_3D( ptr, volume, x, y, z )       \
           GET_GIVEN_DIM_PTR( ptr, volume, ***, [x][y][z] )

#define  GET_VOXEL_PTR_4D( ptr, volume, x, y, z, t )       \
           GET_GIVEN_DIM_PTR( ptr, volume, ****, [x][y][z][t] )

#define  GET_VOXEL_PTR_5D( ptr, volume, x, y, z, t, v )       \
           GET_GIVEN_DIM_PTR( ptr, volume, *****, [x][y][z][t][v] )

#define  GET_VOXEL_PTR( ptr, volume, x, y, z, t, v )       \
         switch( (volume)->n_dimensions ) \
         { \
         case 1:  GET_VOXEL_PTR_1D( ptr, volume, x );              break; \
         case 2:  GET_VOXEL_PTR_2D( ptr, volume, x, y );           break; \
         case 3:  GET_VOXEL_PTR_3D( ptr, volume, x, y, z );        break; \
         case 4:  GET_VOXEL_PTR_4D( ptr, volume, x, y, z, t );     break; \
         case 5:  GET_VOXEL_PTR_5D( ptr, volume, x, y, z, t, v );  break; \
         }

#define  CONVERT_VOXEL_TO_VALUE( volume, voxel )    \
            ((volume)->value_scale * (Real) (voxel) + \
             (volume)->value_translation)

#define  CONVERT_VALUE_TO_VOXEL( volume, value )    \
          (((Real) value - (volume)->value_translation) / (volume)->value_scale)

#define  GET_VALUE_1D( value, volume, x )       \
         { \
             GET_VOXEL_1D( value, volume, x ); \
             value = CONVERT_VOXEL_TO_VALUE( volume, value ); \
         }

#define  GET_VALUE_2D( value, volume, x, y )       \
         { \
             GET_VOXEL_2D( value, volume, x, y ); \
             value = CONVERT_VOXEL_TO_VALUE( volume, value ); \
         }

#define  GET_VALUE_3D( value, volume, x, y, z )       \
         { \
             GET_VOXEL_3D( value, volume, x, y, z ); \
             value = CONVERT_VOXEL_TO_VALUE( volume, value ); \
         }

#define  GET_VALUE_4D( value, volume, x, y, z, t )       \
         { \
             GET_VOXEL_4D( value, volume, x, y, z, t ); \
             value = CONVERT_VOXEL_TO_VALUE( volume, value ); \
         }

#define  GET_VALUE_5D( value, volume, x, y, z, t, v )       \
         { \
             GET_VOXEL_5D( value, volume, x, y, z, t, v ); \
             value = CONVERT_VOXEL_TO_VALUE( volume, value ); \
         }

/* -------------------- minc file struct -------------------- */

typedef  struct
{
    Boolean            file_is_being_read;

    /* input and output */

    int                cdfid;
    int                icv;
    int                n_file_dimensions;
    int                sizes_in_file[MAX_VAR_DIMS];
    long               indices[MAX_VAR_DIMS];

    /* input only */

    Boolean            end_volume_flag;
    int                n_volumes_in_file;
    Volume             volume;

    int                axis_index_in_file[MAX_VAR_DIMS];
    int                valid_file_axes[MAX_DIMENSIONS];

    int                n_slab_dims;

    /* output only */

    int                img_var_id;
    int                min_id;
    int                max_id;
    double             image_range[2];
    Boolean            end_def_done;
    Boolean            variables_written;
    int                dim_ids[MAX_VAR_DIMS];
    String             dim_names[MAX_VAR_DIMS];
} minc_file_struct;

typedef  minc_file_struct  *Minc_file;

typedef  struct
{
    int       arent_any_yet;
} volume_creation_options;

typedef  struct
{
    Boolean     promote_invalid_to_min_flag;
} minc_input_options;

typedef  struct
{
    int         duh;
} minc_output_options;

/* recognized file formats */

typedef  enum  { MNC_FORMAT, FREE_FORMAT }       Volume_file_formats;

typedef struct
{
    Volume_file_formats  file_format;

    Minc_file            minc_file;

    /* for free format files only */

    FILE                 *volume_file;
    int                  slice_index;
    int                  sizes_in_file[MAX_DIMENSIONS];
    int                  axis_index_from_file[MAX_DIMENSIONS];
    Data_types           file_data_type;
    Boolean              one_file_per_slice;
    String               directory;
    String               *slice_filenames;
    int                  *slice_byte_offsets;
    unsigned char        *byte_slice_buffer;
    unsigned short       *short_slice_buffer;

} volume_input_struct;

/* --------------------- resample struct (used during resampling) ------- */

typedef struct
{
    int            x, y;
    Volume         src_volume;
    Volume         dest_volume;
    Transform      transform;
} resample_struct;

/* --------------------- filter types -------------------------------- */

typedef enum {
               NEAREST_NEIGHBOUR,
               LINEAR_INTERPOLATION,
               BOX_FILTER,
               TRIANGLE_FILTER,
               GAUSSIAN_FILTER } Filter_types;

#endif
