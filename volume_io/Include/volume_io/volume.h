#ifndef  DEF_VOLUME
#define  DEF_VOLUME

/* ----------------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

#ifndef lint
static char volume_rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Include/volume_io/volume.h,v 1.42 1995-11-20 12:33:40 david Exp $";
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : volume.h
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Types for use in dealing with volumes.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  cmode  nc_cmode
#include  <minc.h>
#undef  cmode
#include  <transforms.h>
#include  <multidim.h>

typedef  struct
{
    Real     global_image_range[2];
    STRING   dimension_names[MAX_DIMENSIONS];
} minc_output_options;

#include  <volume_cache.h>

extern  STRING   XYZ_dimension_names[];
extern  STRING   File_order_dimension_names[];

/* -------------------------- volume struct --------------------- */

#define  ANY_SPATIAL_DIMENSION   "any_spatial_dimension"

typedef  struct volume_struct
{
    BOOLEAN                 is_cached_volume;
    volume_cache_struct     cache;

    multidim_array          array;

    STRING                  dimension_names[MAX_DIMENSIONS];
    int                     spatial_axes[N_DIMENSIONS];
    nc_type                 nc_data_type;
    BOOLEAN                 signed_flag;
    BOOLEAN                 is_rgba_data;

    Real                    (*get_1d)( struct volume_struct *, int );
    Real                    (*get_2d)( struct volume_struct *, int, int );
    Real                    (*get_3d)( struct volume_struct *, int, int, int );
    Real                    (*get_4d)( struct volume_struct *, int, int, int,
                                       int );
    Real                    (*get_5d)( struct volume_struct *, int, int, int,
                                       int, int );
    Real                    (*get)( struct volume_struct *, int, int, int,
                                       int, int );

    void                    (*set_1d)( struct volume_struct *, int, Real );
    void                    (*set_2d)( struct volume_struct *, int, int, Real );
    void                    (*set_3d)( struct volume_struct *, int, int, int,
                                       Real );
    void                    (*set_4d)( struct volume_struct *, int, int, int,
                                       int, Real );
    void                    (*set_5d)( struct volume_struct *, int, int, int,
                                       int, int, Real );
    void                    (*set)( struct volume_struct *, int, int, int,
                                    int, int, Real );

    Real                    voxel_min;
    Real                    voxel_max;
    BOOLEAN                 real_range_set;
    Real                    real_value_scale;
    Real                    real_value_translation;

    Real                    separations[MAX_DIMENSIONS];
    Real                    translation_voxel[MAX_DIMENSIONS];
    Real                    direction_cosines[MAX_DIMENSIONS][N_DIMENSIONS];

    Real                    world_space_for_translation_voxel[N_DIMENSIONS];

    General_transform       voxel_to_world_transform;
} volume_struct;

typedef  volume_struct  *Volume;

/* ---- macro for stepping through entire volume */

#define  BEGIN_ALL_VOXELS( volume, v0, v1, v2, v3, v4 )                       \
         {                                                                    \
             int  _i_, _sizes_[MAX_DIMENSIONS];                               \
             int  _size0_, _size1_, _size2_, _size3_, _size4_;                \
                                                                              \
             get_volume_sizes( volume, _sizes_ );                             \
             for_less( _i_, get_volume_n_dimensions(volume), MAX_DIMENSIONS ) \
                 _sizes_[_i_] = 1;                                            \
             _size0_ = _sizes_[0];                                            \
             _size1_ = _sizes_[1];                                            \
             _size2_ = _sizes_[2];                                            \
             _size3_ = _sizes_[3];                                            \
             _size4_ = _sizes_[4];                                            \
                                                                              \
             for_less( v4, 0, _size4_ )                                       \
             for_less( v3, 0, _size3_ )                                       \
             for_less( v2, 0, _size2_ )                                       \
             for_less( v1, 0, _size1_ )                                       \
             for_less( v0, 0, _size0_ )                                       \
             {

#define  END_ALL_VOXELS                                                       \
             }                                                                \
         }

#define  CONVERT_VOXEL_TO_VALUE( volume, voxel ) \
         ( (volume)->real_range_set ? \
               (volume)->real_value_scale * (voxel) + \
               (volume)->real_value_translation : (voxel) )

#define  CONVERT_VALUE_TO_VOXEL( volume, value ) \
         ( (volume)->real_range_set ? \
               ((value) - (volume)->real_value_translation) / \
                (volume)->real_value_scale : (value) )

/* ------------------------- set voxel value ------------------------ */

/* --- public macros to set the [x][y]... voxel of 'volume' to 'value' */

#define  SET_VOXEL_1D( volume, x, value )       \
           (volume)->set_1d( volume, x, value )

#define  SET_VOXEL_2D( volume, x, y, value )       \
           (volume)->set_2d( volume, x, y, value )

#define  SET_VOXEL_3D( volume, x, y, z, value )       \
           (volume)->set_3d( volume, x, y, z, value )

#define  SET_VOXEL_4D( volume, x, y, z, t, value )       \
           (volume)->set_4d( volume, x, y, z, t, value )

#define  SET_VOXEL_5D( volume, x, y, z, t, v, value )       \
           (volume)->set_5d( volume, x, y, z, t, v, value )

/* --- same as previous, but don't have to know dimensions of volume */

#define  SET_VOXEL( volume, x, y, z, t, v, value )       \
           (volume)->set( volume, x, y, z, t, v, value )

/* --- public macros to place the [x][y]...'th voxel of 'volume' in 'value' */

#define  GET_VOXEL_1D( value, volume, x )       \
           (value) = (volume)->get_1d( volume, x )

#define  GET_VOXEL_2D( value, volume, x, y )       \
           (value) = (volume)->get_2d( volume, x, y )

#define  GET_VOXEL_3D( value, volume, x, y, z )       \
           (value) = (volume)->get_3d( volume, x, y, z )

#define  GET_VOXEL_4D( value, volume, x, y, z, t )       \
           (value) = (volume)->get_4d( volume, x, y, z, t )

#define  GET_VOXEL_5D( value, volume, x, y, z, t, v )       \
           (value) = (volume)->get_5d( volume, x, y, z, t, v )

/* --- same as previous, but no need to know volume dimensions */

#define  GET_VOXEL( value, volume, x, y, z, t, v )       \
           (value) = (volume)->get( volume, x, y, z, t, v )

#define  get_volume_voxel_value( volume, v0, v1, v2, v3, v4 ) \
           (volume)->get( volume, v0, v1, v2, v3, v4 )
#define  set_volume_voxel_value( volume, v0, v1, v2, v3, v4, value ) \
           (volume)->set( volume, v0, v1, v2, v3, v4, value )
#define  get_volume_real_value( volume, v0, v1, v2, v3, v4 ) \
           CONVERT_VOXEL_TO_VALUE( volume, \
                                (volume)->get( volume, v0, v1, v2, v3, v4 ) )
#define  set_volume_real_value( volume, v0, v1, v2, v3, v4, value ) \
           (volume)->set( volume, v0, v1, v2, v3, v4, \
                              CONVERT_VALUE_TO_VOXEL( volume, value ) )

/* ------------------------- get voxel ptr ------------------------ */

/* --- public macros to return a pointer to the [x][y]'th voxel of the
       'volume', and place it in 'ptr' */

#define  GET_VOXEL_PTR_1D( ptr, volume, x )       \
           if( (volume)->is_cached_volume ) \
/*              handle_internal_error( "Cannot get pointer to cached Volume.\n");\
           else */ \
              GET_MULTIDIM_PTR_1D( ptr, (volume)->array, x )

#define  GET_VOXEL_PTR_2D( ptr, volume, x, y )       \
           if( (volume)->is_cached_volume ) \
              handle_internal_error( "Cannot get pointer to cached Volume.\n");\
           else \
              GET_MULTIDIM_PTR_2D( ptr, (volume)->array, x, y )

#define  GET_VOXEL_PTR_3D( ptr, volume, x, y, z )       \
           if( (volume)->is_cached_volume ) \
              handle_internal_error( "Cannot get pointer to cached Volume.\n");\
           else \
              GET_MULTIDIM_PTR_3D( ptr, (volume)->array, x, y, z )

#define  GET_VOXEL_PTR_4D( ptr, volume, x, y, z, t )       \
           if( (volume)->is_cached_volume ) \
              handle_internal_error( "Cannot get pointer to cached Volume.\n");\
           else \
              GET_MULTIDIM_PTR_4D( ptr, (volume)->array, x, y, z, t )

#define  GET_VOXEL_PTR_5D( ptr, volume, x, y, z, t, v )       \
           if( (volume)->is_cached_volume ) \
              handle_internal_error( "Cannot get pointer to cached Volume.\n");\
           else \
              GET_MULTIDIM_PTR_5D( ptr, (volume)->array, x, y, z, t, v )

/* --- same as previous, but no need to know voxel dimensions */

#define  GET_VOXEL_PTR( ptr, volume, x, y, z, t, v )       \
           if( (volume)->is_cached_volume ) \
              handle_internal_error( "Cannot get pointer to cached Volume.\n");\
           else \
              GET_MULTIDIM_PTR( ptr, (volume)->array, x, y, z, t, v )

/* --- assigns 'value' the value of the [x][y]...'th voxel of 'volume' */

#define  GET_VALUE_1D( value, volume, x )       \
         { \
             GET_VOXEL_1D( value, volume, x ); \
             (value) = CONVERT_VOXEL_TO_VALUE( volume, value ); \
         }

#define  GET_VALUE_2D( value, volume, x, y )       \
         { \
             GET_VOXEL_2D( value, volume, x, y ); \
             (value) = CONVERT_VOXEL_TO_VALUE( volume, value ); \
         }

#define  GET_VALUE_3D( value, volume, x, y, z )       \
         { \
             GET_VOXEL_3D( value, volume, x, y, z ); \
             (value) = CONVERT_VOXEL_TO_VALUE( volume, value ); \
         }

#define  GET_VALUE_4D( value, volume, x, y, z, t )       \
         { \
             GET_VOXEL_4D( value, volume, x, y, z, t ); \
             (value) = CONVERT_VOXEL_TO_VALUE( volume, value ); \
         }

#define  GET_VALUE_5D( value, volume, x, y, z, t, v )       \
         { \
             GET_VOXEL_5D( value, volume, x, y, z, t, v ); \
             (value) = CONVERT_VOXEL_TO_VALUE( volume, value ); \
         }

/* --- same as previous, without knowing number of dimensions of volume */

#define  GET_VALUE( value, volume, x, y, z, t, v )       \
         switch( (volume)->n_dimensions ) \
         { \
         case 1:  GET_VALUE_1D( value, volume, x );              break; \
         case 2:  GET_VALUE_2D( value, volume, x, y );           break; \
         case 3:  GET_VALUE_3D( value, volume, x, y, z );        break; \
         case 4:  GET_VALUE_4D( value, volume, x, y, z, t );     break; \
         case 5:  GET_VALUE_5D( value, volume, x, y, z, t, v );  break; \
         }

/* -------------------- minc file struct -------------------- */

typedef  struct
{
    int         arent_any_yet;
} volume_creation_options;

typedef  struct
{
    BOOLEAN     promote_invalid_to_min_flag;
    BOOLEAN     convert_vector_to_scalar_flag;
    BOOLEAN     convert_vector_to_colour_flag;
    int         dimension_size_for_colour_data;
    int         rgba_indices[4];
} minc_input_options;

typedef  struct
{
    BOOLEAN            file_is_being_read;

    /* input and output */

    int                cdfid;
    int                img_var;
    int                n_file_dimensions;
    int                sizes_in_file[MAX_VAR_DIMS];
    long               indices[MAX_VAR_DIMS];
    STRING             dim_names[MAX_VAR_DIMS];
    Volume             volume;
    int                to_volume_index[MAX_VAR_DIMS];
    int                to_file_index[MAX_DIMENSIONS];
    int                minc_icv;
    STRING             filename;

    /* input only */

    BOOLEAN            end_volume_flag;
    BOOLEAN            converting_to_colour;
    int                rgba_indices[4];
    int                n_volumes_in_file;

    int                valid_file_axes[MAX_DIMENSIONS];

    int                n_slab_dims;

    int                spatial_axes[N_DIMENSIONS];
    General_transform  voxel_to_world_transform;
    minc_input_options original_input_options;

    /* output only */

    int                img_var_id;
    int                min_id;
    int                max_id;
    double             image_range[2];
    BOOLEAN            end_def_done;
    BOOLEAN            ignoring_because_cached;
    BOOLEAN            variables_written;
    int                dim_ids[MAX_VAR_DIMS];
    BOOLEAN            outputting_in_order;
    BOOLEAN            entire_file_written;
} minc_file_struct;

typedef  minc_file_struct  *Minc_file;

#define   MNC_ENDING   "mnc"

/* --- recognized file formats */

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
    BOOLEAN              one_file_per_slice;
    STRING               directory;
    STRING               *slice_filenames;
    int                  *slice_byte_offsets;
    unsigned char        *byte_slice_buffer;
    unsigned short       *short_slice_buffer;

} volume_input_struct;

/* --------------------- filter types -------------------------------- */

typedef enum {
               NEAREST_NEIGHBOUR,
               LINEAR_INTERPOLATION,
               BOX_FILTER,
               TRIANGLE_FILTER,
               GAUSSIAN_FILTER } Filter_types;

#endif
