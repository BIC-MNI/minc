#ifndef  DEF_vol_io_prototypes
#define  DEF_vol_io_prototypes

public  Real  convert_voxel_to_value(
    Volume   volume,
    Real     voxel );

public  Real  convert_value_to_voxel(
    Volume   volume,
    Real     value );

public  Real  get_volume_voxel_value(
    Volume   volume,
    int      v0,
    int      v1,
    int      v2,
    int      v3,
    int      v4 );

public  Real  get_volume_real_value(
    Volume   volume,
    int      v0,
    int      v1,
    int      v2,
    int      v3,
    int      v4 );

public  void  set_volume_voxel_value(
    Volume   volume,
    int      v0,
    int      v1,
    int      v2,
    int      v3,
    int      v4,
    Real     voxel );

public  void  set_volume_real_value(
    Volume   volume,
    int      v0,
    int      v1,
    int      v2,
    int      v3,
    int      v4,
    Real     value );

public  void  set_volume_interpolation_tolerance(
    Real   tolerance );

public  int   evaluate_volume(
    Volume         volume,
    Real           voxel[],
    BOOLEAN        interpolating_dimensions[],
    int            degrees_continuity,
    BOOLEAN        use_linear_at_edge,
    Real           outside_value,
    Real           values[],
    Real           **first_deriv,
    Real           ***second_deriv );

public  void   evaluate_volume_in_world(
    Volume         volume,
    Real           x,
    Real           y,
    Real           z,
    int            degrees_continuity,
    BOOLEAN        use_linear_at_edge,
    Real           outside_value,
    Real           values[],
    Real           deriv_x[],
    Real           deriv_y[],
    Real           deriv_z[],
    Real           deriv_xx[],
    Real           deriv_xy[],
    Real           deriv_xz[],
    Real           deriv_yy[],
    Real           deriv_yz[],
    Real           deriv_zz[] );

public  void  convert_voxels_to_values(
    Volume   volume,
    int      n_voxels,
    Real     voxels[],
    Real     values[] );

public  void  get_volume_value_hyperslab(
    Volume   volume,
    int      v0,
    int      v1,
    int      v2,
    int      v3,
    int      v4,
    int      n0,
    int      n1,
    int      n2,
    int      n3,
    int      n4,
    Real     values[] );

public  void  get_volume_value_hyperslab_5d(
    Volume   volume,
    int      v0,
    int      v1,
    int      v2,
    int      v3,
    int      v4,
    int      n0,
    int      n1,
    int      n2,
    int      n3,
    int      n4,
    Real     values[] );

public  void  get_volume_value_hyperslab_4d(
    Volume   volume,
    int      v0,
    int      v1,
    int      v2,
    int      v3,
    int      n0,
    int      n1,
    int      n2,
    int      n3,
    Real     values[] );

public  void  get_volume_value_hyperslab_3d(
    Volume   volume,
    int      v0,
    int      v1,
    int      v2,
    int      n0,
    int      n1,
    int      n2,
    Real     values[] );

public  void  get_volume_value_hyperslab_2d(
    Volume   volume,
    int      v0,
    int      v1,
    int      n0,
    int      n1,
    Real     values[] );

public  void  get_volume_value_hyperslab_1d(
    Volume   volume,
    int      v0,
    int      n0,
    Real     values[] );

public  void  get_voxel_values_5d(
    Data_types  data_type,
    void        *void_ptr,
    int         steps[],
    int         counts[],
    Real        values[] );

public  void  get_voxel_values_4d(
    Data_types  data_type,
    void        *void_ptr,
    int         steps[],
    int         counts[],
    Real        values[] );

public  void  get_voxel_values_3d(
    Data_types  data_type,
    void        *void_ptr,
    int         steps[],
    int         counts[],
    Real        values[] );

public  void  get_voxel_values_2d(
    Data_types  data_type,
    void        *void_ptr,
    int         steps[],
    int         counts[],
    Real        values[] );

public  void  get_voxel_values_1d(
    Data_types  data_type,
    void        *void_ptr,
    int         step0,
    int         n0,
    Real        values[] );

public  void  get_volume_voxel_hyperslab_5d(
    Volume   volume,
    int      v0,
    int      v1,
    int      v2,
    int      v3,
    int      v4,
    int      n0,
    int      n1,
    int      n2,
    int      n3,
    int      n4,
    Real     values[] );

public  void  get_volume_voxel_hyperslab_4d(
    Volume   volume,
    int      v0,
    int      v1,
    int      v2,
    int      v3,
    int      n0,
    int      n1,
    int      n2,
    int      n3,
    Real     values[] );

public  void  get_volume_voxel_hyperslab_3d(
    Volume   volume,
    int      v0,
    int      v1,
    int      v2,
    int      n0,
    int      n1,
    int      n2,
    Real     values[] );

public  void  get_volume_voxel_hyperslab_2d(
    Volume   volume,
    int      v0,
    int      v1,
    int      n0,
    int      n1,
    Real     values[] );

public  void  get_volume_voxel_hyperslab_1d(
    Volume   volume,
    int      v0,
    int      n0,
    Real     values[] );

public  void  get_volume_voxel_hyperslab(
    Volume   volume,
    int      v0,
    int      v1,
    int      v2,
    int      v3,
    int      v4,
    int      n0,
    int      n1,
    int      n2,
    int      n3,
    int      n4,
    Real     voxels[] );

public  Status  initialize_free_format_input(
    STRING               filename,
    Volume               volume,
    volume_input_struct  *volume_input );

public  void  delete_free_format_input(
    volume_input_struct   *volume_input );

public  BOOLEAN  input_more_free_format_file(
    Volume                volume,
    volume_input_struct   *volume_input,
    Real                  *fraction_done );

public  int   get_minc_file_n_dimensions(
    STRING   filename );

public  Minc_file  initialize_minc_input_from_minc_id(
    int                  minc_id,
    Volume               volume,
    minc_input_options   *options );

public  Minc_file  initialize_minc_input(
    STRING               filename,
    Volume               volume,
    minc_input_options   *options );

public  int  get_n_input_volumes(
    Minc_file  file );

public  Status  close_minc_input(
    Minc_file   file );

public  Status  input_minc_hyperslab(
    Minc_file        file,
    Data_types       data_type,
    int              n_array_dims,
    int              array_sizes[],
    void             *array_data_ptr,
    int              to_array[],
    int              start[],
    int              count[] );

public  BOOLEAN  input_more_minc_file(
    Minc_file   file,
    Real        *fraction_done );

public  BOOLEAN  advance_input_volume(
    Minc_file   file );

public  void  reset_input_volume(
    Minc_file   file );

public  int  get_minc_file_id(
    Minc_file  file );

public  void  set_default_minc_input_options(
    minc_input_options  *options );

public  void  set_minc_input_promote_invalid_to_zero_flag(
    minc_input_options  *options,
    BOOLEAN             flag );

public  void  set_minc_input_promote_invalid_to_min_flag(
    minc_input_options  *options,
    BOOLEAN             flag );

public  void  set_minc_input_vector_to_scalar_flag(
    minc_input_options  *options,
    BOOLEAN             flag );

public  void  set_minc_input_vector_to_colour_flag(
    minc_input_options  *options,
    BOOLEAN             flag );

public  void  set_minc_input_colour_dimension_size(
    minc_input_options  *options,
    int                 size );

public  void  set_minc_input_colour_max_dimension_size(
    minc_input_options  *options,
    int                 size );

public  void  set_minc_input_colour_indices(
    minc_input_options  *options,
    int                 indices[4] );

public  Status  start_volume_input(
    STRING               filename,
    int                  n_dimensions,
    STRING               dim_names[],
    nc_type              volume_nc_data_type,
    BOOLEAN              volume_signed_flag,
    Real                 volume_voxel_min,
    Real                 volume_voxel_max,
    BOOLEAN              create_volume_flag,
    Volume               *volume,
    minc_input_options   *options,
    volume_input_struct  *input_info );

public  void  delete_volume_input(
    volume_input_struct   *input_info );

public  BOOLEAN  input_more_of_volume(
    Volume                volume,
    volume_input_struct   *input_info,
    Real                  *fraction_done );

public  void  cancel_volume_input(
    Volume                volume,
    volume_input_struct   *input_info );

public  Status  input_volume(
    STRING               filename,
    int                  n_dimensions,
    STRING               dim_names[],
    nc_type              volume_nc_data_type,
    BOOLEAN              volume_signed_flag,
    Real                 volume_voxel_min,
    Real                 volume_voxel_max,
    BOOLEAN              create_volume_flag,
    Volume               *volume,
    minc_input_options   *options );

public  Minc_file   get_volume_input_minc_file(
    volume_input_struct   *volume_input );

public   void   create_empty_multidim_array(
    multidim_array  *array,
    int             n_dimensions,
    Data_types      data_type );

public  Data_types  get_multidim_data_type(
    multidim_array       *array );

public  void  set_multidim_data_type(
    multidim_array       *array,
    Data_types           data_type );

public  int  get_type_size(
    Data_types   type );

public  void  get_type_range(
    Data_types   type,
    Real         *min_value,
    Real         *max_value );

public  void  set_multidim_sizes(
    multidim_array   *array,
    int              sizes[] );

public  void  get_multidim_sizes(
    multidim_array   *array,
    int              sizes[] );

public  BOOLEAN  multidim_array_is_alloced(
    multidim_array   *array );

public  void  alloc_multidim_array(
    multidim_array   *array );

public   void   create_multidim_array(
    multidim_array  *array,
    int             n_dimensions,
    int             sizes[],
    Data_types      data_type );

public  void  delete_multidim_array(
    multidim_array   *array );

public  int  get_multidim_n_dimensions(
    multidim_array   *array );

public  void  copy_multidim_data_reordered(
    int                 type_size,
    void                *void_dest_ptr,
    int                 n_dest_dims,
    int                 dest_sizes[],
    void                *void_src_ptr,
    int                 n_src_dims,
    int                 src_sizes[],
    int                 counts[],
    int                 to_dest_index[],
    BOOLEAN             use_src_order );

public  void  copy_multidim_reordered(
    multidim_array      *dest,
    int                 dest_ind[],
    multidim_array      *src,
    int                 src_ind[],
    int                 counts[],
    int                 to_dest_index[] );

public  Minc_file  initialize_minc_output(
    STRING                 filename,
    int                    n_dimensions,
    STRING                 dim_names[],
    int                    sizes[],
    nc_type                file_nc_data_type,
    BOOLEAN                file_signed_flag,
    Real                   file_voxel_min,
    Real                   file_voxel_max,
    General_transform      *voxel_to_world_transform,
    Volume                 volume_to_attach,
    minc_output_options    *options );

public  Status  copy_auxiliary_data_from_minc_file(
    Minc_file   file,
    STRING      filename,
    STRING      history_string );

public  Status  copy_auxiliary_data_from_open_minc_file(
    Minc_file   file,
    int         src_cdfid,
    STRING      history_string );

public  Status  add_minc_history(
    Minc_file   file,
    STRING      history_string );

public  Status  set_minc_output_random_order(
    Minc_file   file );

public  Status  output_minc_hyperslab(
    Minc_file           file,
    Data_types          data_type,
    int                 n_array_dims,
    int                 array_sizes[],
    void                *array_data_ptr,
    int                 to_array[],
    int                 file_start[],
    int                 file_count[] );

public  Status  output_volume_to_minc_file_position(
    Minc_file   file,
    Volume      volume,
    int         volume_count[],
    long        file_start[] );

public  Status  output_minc_volume(
    Minc_file   file );

public  Status  close_minc_output(
    Minc_file   file );

public  void  set_default_minc_output_options(
    minc_output_options  *options           );

public  void  copy_minc_output_options(
    minc_output_options  *src,
    minc_output_options  *dest );

public  void  delete_minc_output_options(
    minc_output_options  *options           );

public  void  set_minc_output_dimensions_order(
    minc_output_options  *options,
    int                  n_dimensions,
    STRING               dimension_names[] );

public  void  set_minc_output_real_range(
    minc_output_options  *options,
    Real                 real_min,
    Real                 real_max );

public  void  set_minc_output_use_volume_starts_and_steps_flag(
    minc_output_options  *options,
    BOOLEAN              flag );

public  Status   get_file_dimension_names(
    STRING   filename,
    int      *n_dims,
    STRING   *dim_names[] );

public  STRING  *create_output_dim_names(
    Volume                volume,
    STRING                original_filename,
    minc_output_options   *options,
    int                   file_sizes[] );

public  Status   copy_volume_auxiliary_and_history(
    Minc_file   minc_file,
    STRING      filename,
    STRING      original_filename,
    STRING      history );

public  Status  output_modified_volume(
    STRING                filename,
    nc_type               file_nc_data_type,
    BOOLEAN               file_signed_flag,
    Real                  file_voxel_min,
    Real                  file_voxel_max,
    Volume                volume,
    STRING                original_filename,
    STRING                history,
    minc_output_options   *options );

public  Status  output_volume(
    STRING                filename,
    nc_type               file_nc_data_type,
    BOOLEAN               file_signed_flag,
    Real                  file_voxel_min,
    Real                  file_voxel_max,
    Volume                volume,
    STRING                history,
    minc_output_options   *options );

public  void  convert_values_to_voxels(
    Volume   volume,
    int      n_voxels,
    Real     values[],
    Real     voxels[] );

public  void  set_volume_value_hyperslab(
    Volume   volume,
    int      v0,
    int      v1,
    int      v2,
    int      v3,
    int      v4,
    int      n0,
    int      n1,
    int      n2,
    int      n3,
    int      n4,
    Real     values[] );

public  void  set_volume_value_hyperslab_5d(
    Volume   volume,
    int      v0,
    int      v1,
    int      v2,
    int      v3,
    int      v4,
    int      n0,
    int      n1,
    int      n2,
    int      n3,
    int      n4,
    Real     values[] );

public  void  set_volume_value_hyperslab_4d(
    Volume   volume,
    int      v0,
    int      v1,
    int      v2,
    int      v3,
    int      n0,
    int      n1,
    int      n2,
    int      n3,
    Real     values[] );

public  void  set_volume_value_hyperslab_3d(
    Volume   volume,
    int      v0,
    int      v1,
    int      v2,
    int      n0,
    int      n1,
    int      n2,
    Real     values[] );

public  void  set_volume_value_hyperslab_2d(
    Volume   volume,
    int      v0,
    int      v1,
    int      n0,
    int      n1,
    Real     values[] );

public  void  set_volume_value_hyperslab_1d(
    Volume   volume,
    int      v0,
    int      n0,
    Real     values[] );

public  void  set_volume_voxel_hyperslab_5d(
    Volume   volume,
    int      v0,
    int      v1,
    int      v2,
    int      v3,
    int      v4,
    int      n0,
    int      n1,
    int      n2,
    int      n3,
    int      n4,
    Real     values[] );

public  void  set_volume_voxel_hyperslab_4d(
    Volume   volume,
    int      v0,
    int      v1,
    int      v2,
    int      v3,
    int      n0,
    int      n1,
    int      n2,
    int      n3,
    Real     values[] );

public  void  set_volume_voxel_hyperslab_3d(
    Volume   volume,
    int      v0,
    int      v1,
    int      v2,
    int      n0,
    int      n1,
    int      n2,
    Real     values[] );

public  void  set_volume_voxel_hyperslab_2d(
    Volume   volume,
    int      v0,
    int      v1,
    int      n0,
    int      n1,
    Real     values[] );

public  void  set_volume_voxel_hyperslab_1d(
    Volume   volume,
    int      v0,
    int      n0,
    Real     values[] );

public  void  set_volume_voxel_hyperslab(
    Volume   volume,
    int      v0,
    int      v1,
    int      v2,
    int      v3,
    int      v4,
    int      n0,
    int      n1,
    int      n2,
    int      n3,
    int      n4,
    Real     voxels[] );

public  void  set_n_bytes_cache_threshold(
    int  threshold );

public  int  get_n_bytes_cache_threshold( void );

public  void  set_default_max_bytes_in_cache(
    int   max_bytes );

public  int  get_default_max_bytes_in_cache( void );

public  void  set_default_cache_block_sizes(
    int                      block_sizes[] );

public  void  set_cache_block_sizes_hint(
    Cache_block_size_hints  hint );

public  void  initialize_volume_cache(
    volume_cache_struct   *cache,
    Volume                volume );

public  BOOLEAN  volume_cache_is_alloced(
    volume_cache_struct   *cache );

public  void  flush_volume_cache(
    Volume                volume );

public  void  delete_volume_cache(
    volume_cache_struct   *cache,
    Volume                volume );

public  void  set_volume_cache_block_sizes(
    Volume    volume,
    int       block_sizes[] );

public  void  set_volume_cache_size(
    Volume    volume,
    int       max_memory_bytes );

public  void  set_cache_output_volume_parameters(
    Volume                      volume,
    STRING                      filename,
    nc_type                     file_nc_data_type,
    BOOLEAN                     file_signed_flag,
    Real                        file_voxel_min,
    Real                        file_voxel_max,
    STRING                      original_filename,
    STRING                      history,
    minc_output_options         *options )
;

public  void  open_cache_volume_input_file(
    volume_cache_struct   *cache,
    Volume                volume,
    STRING                filename,
    minc_input_options    *options );

public  void  cache_volume_range_has_changed(
    Volume   volume );

public  void  set_cache_volume_file_offset(
    volume_cache_struct   *cache,
    Volume                volume,
    long                  file_offset[] );

public  Real  get_cached_volume_voxel(
    Volume   volume,
    int      x,
    int      y,
    int      z,
    int      t,
    int      v );

public  void  set_cached_volume_voxel(
    Volume   volume,
    int      x,
    int      y,
    int      z,
    int      t,
    int      v,
    Real     value );

public  BOOLEAN  cached_volume_has_been_modified(
    volume_cache_struct  *cache );

public  BOOLEAN  volume_is_cached(
    Volume  volume );

public  void   set_volume_cache_debugging(
    Volume   volume,
    int      output_every );

public  STRING  *get_default_dim_names(
    int    n_dimensions );

public  BOOLEAN  convert_dim_name_to_spatial_axis(
    STRING  name,
    int     *axis );

public   Volume   create_volume(
    int         n_dimensions,
    STRING      dimension_names[],
    nc_type     nc_data_type,
    BOOLEAN     signed_flag,
    Real        voxel_min,
    Real        voxel_max );

public  void  set_volume_type(
    Volume       volume,
    nc_type      nc_data_type,
    BOOLEAN      signed_flag,
    Real         voxel_min,
    Real         voxel_max );

public  nc_type  get_volume_nc_data_type(
    Volume       volume,
    BOOLEAN      *signed_flag );

public  Data_types  get_volume_data_type(
    Volume       volume );

public  void  set_rgb_volume_flag(
    Volume   volume,
    BOOLEAN  flag );

public  BOOLEAN  is_an_rgb_volume(
    Volume   volume );

public  void  alloc_volume_data(
    Volume   volume );

public  BOOLEAN  volume_is_alloced(
    Volume   volume );

public  void  free_volume_data(
    Volume   volume );

public  void  delete_volume(
    Volume   volume );

public  int  get_volume_n_dimensions(
    Volume   volume );

public  void  get_volume_sizes(
    Volume   volume,
    int      sizes[] );

public  void  set_volume_sizes(
    Volume       volume,
    int          sizes[] );

public  unsigned int  get_volume_total_n_voxels(
    Volume    volume );

public  void  compute_world_transform(
    int                 spatial_axes[N_DIMENSIONS],
    Real                separations[],
    Real                direction_cosines[][N_DIMENSIONS],
    Real                starts[],
    General_transform   *world_transform );

public  void  convert_transform_to_starts_and_steps(
    General_transform  *transform,
    int                n_volume_dimensions,
    Real               step_signs[],
    int                spatial_axes[],
    Real               starts[],
    Real               steps[],
    Real               dir_cosines[][N_DIMENSIONS] );

public  void  set_voxel_to_world_transform(
    Volume             volume,
    General_transform  *transform );

public  General_transform  *get_voxel_to_world_transform(
    Volume   volume );

public  STRING  *get_volume_dimension_names(
    Volume   volume );

public  void  delete_dimension_names(
    Volume   volume,
    STRING   dimension_names[] );

public  STRING  get_volume_space_type(
    Volume   volume );

public  void  set_volume_space_type(
    Volume   volume,
    STRING   name );

public  void  get_volume_separations(
    Volume   volume,
    Real     separations[] );

public  void  set_volume_separations(
    Volume   volume,
    Real     separations[] );

public  void  set_volume_starts(
    Volume  volume,
    Real    starts[] );

public  void  get_volume_starts(
    Volume  volume,
    Real    starts[] );

public  void  set_volume_direction_unit_cosine(
    Volume   volume,
    int      axis,
    Real     dir[] );

public  void  set_volume_direction_cosine(
    Volume   volume,
    int      axis,
    Real     dir[] );

public  void  get_volume_direction_cosine(
    Volume   volume,
    int      axis,
    Real     dir[] );

public  void  reorder_voxel_to_xyz(
    Volume   volume,
    Real     voxel[],
    Real     xyz[] );

public  void  reorder_xyz_to_voxel(
    Volume   volume,
    Real     xyz[],
    Real     voxel[] );

public  void  convert_voxel_to_world(
    Volume   volume,
    Real     voxel[],
    Real     *x_world,
    Real     *y_world,
    Real     *z_world );

public  void  convert_3D_voxel_to_world(
    Volume   volume,
    Real     voxel1,
    Real     voxel2,
    Real     voxel3,
    Real     *x_world,
    Real     *y_world,
    Real     *z_world );

public  void  convert_voxel_normal_vector_to_world(
    Volume          volume,
    Real            voxel_vector[],
    Real            *x_world,
    Real            *y_world,
    Real            *z_world );

public  void  convert_voxel_vector_to_world(
    Volume          volume,
    Real            voxel_vector[],
    Real            *x_world,
    Real            *y_world,
    Real            *z_world );

public  void  convert_world_vector_to_voxel(
    Volume          volume,
    Real            x_world,
    Real            y_world,
    Real            z_world,
    Real            voxel_vector[] );

public  void  convert_world_to_voxel(
    Volume   volume,
    Real     x_world,
    Real     y_world,
    Real     z_world,
    Real     voxel[] );

public  void  convert_3D_world_to_voxel(
    Volume   volume,
    Real     x_world,
    Real     y_world,
    Real     z_world,
    Real     *voxel1,
    Real     *voxel2,
    Real     *voxel3 );

public  Real  get_volume_voxel_min(
    Volume   volume );

public  Real  get_volume_voxel_max(
    Volume   volume );

public  void  get_volume_voxel_range(
    Volume     volume,
    Real       *voxel_min,
    Real       *voxel_max );

public  void  set_volume_voxel_range(
    Volume   volume,
    Real     voxel_min,
    Real     voxel_max );

public  void  get_volume_real_range(
    Volume     volume,
    Real       *min_value,
    Real       *max_value );

public  Real  get_volume_real_min(
    Volume     volume );

public  Real  get_volume_real_max(
    Volume     volume );

public  void  set_volume_real_range(
    Volume   volume,
    Real     real_min,
    Real     real_max );

public  Volume   copy_volume_definition_no_alloc(
    Volume   volume,
    nc_type  nc_data_type,
    BOOLEAN  signed_flag,
    Real     voxel_min,
    Real     voxel_max );

public  Volume   copy_volume_definition(
    Volume   volume,
    nc_type  nc_data_type,
    BOOLEAN  signed_flag,
    Real     voxel_min,
    Real     voxel_max );

public  Volume  copy_volume(
    Volume   volume );

public  void  grid_transform_point(
    General_transform   *transform,
    Real                x,
    Real                y,
    Real                z,
    Real                *x_transformed,
    Real                *y_transformed,
    Real                *z_transformed );

public  void  grid_inverse_transform_point(
    General_transform   *transform,
    Real                x,
    Real                y,
    Real                z,
    Real                *x_transformed,
    Real                *y_transformed,
    Real                *z_transformed );

public  void  grid_inverse_transform_point(
    General_transform   *transform,
    Real                x,
    Real                y,
    Real                z,
    Real                *x_transformed,
    Real                *y_transformed,
    Real                *z_transformed );

public  Status  mni_get_nonwhite_character(
    FILE   *file,
    char   *ch );

public  Status  mni_skip_expected_character(
    FILE   *file,
    char   expected_ch );

public  Status  mni_input_line(
    FILE     *file,
    STRING   *string );

public  Status  mni_input_string(
    FILE     *file,
    STRING   *string,
    char     termination_char1,
    char     termination_char2 );

public  Status  mni_input_keyword_and_equal_sign(
    FILE         *file,
    const char   keyword[],
    BOOLEAN      print_error_message );

public  Status  mni_input_real(
    FILE    *file,
    Real    *d );

public  Status  mni_input_reals(
    FILE    *file,
    int     *n,
    Real    *reals[] );

public  Status  mni_input_int(
    FILE    *file,
    int     *i );

public  void  output_comments(
    FILE     *file,
    STRING   comments );

public  STRING  get_default_tag_file_suffix( void );

public  Status  initialize_tag_file_output(
    FILE      *file,
    STRING    comments,
    int       n_volumes );

public  Status  output_one_tag(
    FILE      *file,
    int       n_volumes,
    Real      tag_volume1[],
    Real      tag_volume2[],
    Real      *weight,
    int       *structure_id,
    int       *patient_id,
    STRING    label );

public  void  terminate_tag_file_output(
    FILE    *file );

public  Status  output_tag_points(
    FILE      *file,
    STRING    comments,
    int       n_volumes,
    int       n_tag_points,
    Real      **tags_volume1,
    Real      **tags_volume2,
    Real      weights[],
    int       structure_ids[],
    int       patient_ids[],
    STRING    *labels );

public  void  free_tag_points(
    int       n_volumes,
    int       n_tag_points,
    Real      **tags_volume1,
    Real      **tags_volume2,
    Real      weights[],
    int       structure_ids[],
    int       patient_ids[],
    char      **labels );

public  Status  initialize_tag_file_input(
    FILE      *file,
    int       *n_volumes_ptr );

public  Status  output_tag_file(
    STRING    filename,
    STRING    comments,
    int       n_volumes,
    int       n_tag_points,
    Real      **tags_volume1,
    Real      **tags_volume2,
    Real      weights[],
    int       structure_ids[],
    int       patient_ids[],
    STRING    labels[] );

public  Status  input_tag_file(
    STRING    filename,
    int       *n_volumes,
    int       *n_tag_points,
    Real      ***tags_volume1,
    Real      ***tags_volume2,
    Real      **weights,
    int       **structure_ids,
    int       **patient_ids,
    STRING    *labels[] );

public  BOOLEAN  input_one_tag(
    FILE      *file,
    int       n_volumes,
    Real      tag_volume1[],
    Real      tag_volume2[],
    Real      *weight,
    int       *structure_id,
    int       *patient_id,
    STRING    *label,
    Status    *status );

public  Status  input_tag_points(
    FILE      *file,
    int       *n_volumes_ptr,
    int       *n_tag_points,
    Real      ***tags_volume1,
    Real      ***tags_volume2,
    Real      **weights,
    int       **structure_ids,
    int       **patient_ids,
    STRING    *labels[] );

public  void  evaluate_thin_plate_spline(
    int     n_dims,
    int     n_values,
    int     n_points,
    Real    **points,
    Real    **weights,
    Real    pos[],
    Real    values[],
    Real    **derivs );

public  void  thin_plate_spline_transform(
    int     n_dims,
    int     n_points,
    Real    **points,
    Real    **weights,
    Real    x,
    Real    y,
    Real    z,
    Real    *x_transformed,
    Real    *y_transformed,
    Real    *z_transformed );

public  void  thin_plate_spline_inverse_transform(
    int     n_dims,
    int     n_points,
    Real    **points,
    Real    **weights,
    Real    x,
    Real    y,
    Real    z,
    Real    *x_transformed,
    Real    *y_transformed,
    Real    *z_transformed );

public  Real  thin_plate_spline_U(
    Real   pos[],
    Real   landmark[],
    int    n_dims );

public  STRING  get_default_transform_file_suffix( void );

public  Status  output_transform(
    FILE                *file,
    STRING              filename,
    int                 *volume_count_ptr,
    STRING              comments,
    General_transform   *transform );

public  Status  input_transform(
    FILE                *file,
    STRING              filename,
    General_transform   *transform );

public  Status  output_transform_file(
    STRING              filename,
    STRING              comments,
    General_transform   *transform );

public  Status  input_transform_file(
    STRING              filename,
    General_transform   *transform );

public  void  create_linear_transform(
    General_transform   *transform,
    Transform           *linear_transform );

public  void  create_thin_plate_transform_real(
    General_transform    *transform,
    int                  n_dimensions,
    int                  n_points,
    Real                 **points,
    Real                 **displacements );

public  void  create_thin_plate_transform(
    General_transform    *transform,
    int                  n_dimensions,
    int                  n_points,
    float                **points,
    float                **displacements );

public  void  create_grid_transform(
    General_transform    *transform,
    Volume               displacement_volume );

public  void  create_grid_transform_no_copy(
    General_transform    *transform,
    Volume               displacement_volume );

public  void  create_user_transform(
    General_transform         *transform,
    void                      *user_data,
    size_t                    size_user_data,
    User_transform_function   transform_function,
    User_transform_function   inverse_transform_function );

public  Transform_types  get_transform_type(
    General_transform   *transform );

public  int  get_n_concated_transforms(
    General_transform   *transform );

public  General_transform  *get_nth_general_transform(
    General_transform   *transform,
    int                 n );

public  Transform  *get_linear_transform_ptr(
    General_transform   *transform );

public  Transform  *get_inverse_linear_transform_ptr(
    General_transform   *transform );

public  void  general_transform_point(
    General_transform   *transform,
    Real                x,
    Real                y,
    Real                z,
    Real                *x_transformed,
    Real                *y_transformed,
    Real                *z_transformed );

public  void  general_inverse_transform_point(
    General_transform   *transform,
    Real                x,
    Real                y,
    Real                z,
    Real                *x_transformed,
    Real                *y_transformed,
    Real                *z_transformed );

public  void  copy_general_transform(
    General_transform   *transform,
    General_transform   *copy );

public  void  invert_general_transform(
    General_transform   *transform );

public  void  create_inverse_general_transform(
    General_transform   *transform,
    General_transform   *inverse );

public  void  concat_general_transforms(
    General_transform   *first,
    General_transform   *second,
    General_transform   *result );

public  void  delete_general_transform(
    General_transform   *transform );

public  Colour  make_rgba_Colour(
    int    r,
    int    g,
    int    b,
    int    a );

public  int  get_Colour_r(
    Colour   colour );

public  int  get_Colour_g(
    Colour   colour );

public  int  get_Colour_b(
    Colour   colour );

public  int  get_Colour_a(
    Colour   colour );

public  Colour  make_Colour(
    int   r,
    int   g,
    int   b );

public  Real  get_Colour_r_0_1(
    Colour   colour );

public  Real  get_Colour_g_0_1(
    Colour   colour );

public  Real  get_Colour_b_0_1(
    Colour   colour );

public  Real  get_Colour_a_0_1(
    Colour   colour );

public  Colour  make_Colour_0_1(
    Real   r,
    Real   g,
    Real   b );

public  Colour  make_rgba_Colour_0_1(
    Real   r,
    Real   g,
    Real   b,
    Real   a );

public  BOOLEAN  scaled_maximal_pivoting_gaussian_elimination(
    int   n,
    int   row[],
    Real  **a,
    int   n_values,
    Real  **solution );

public  BOOLEAN  solve_linear_system(
    int   n,
    Real  **coefs,
    Real  values[],
    Real  solution[] );

public  BOOLEAN  invert_square_matrix(
    int   n,
    Real  **matrix,
    Real  **inverse );

public  BOOLEAN  newton_root_find(
    int    n_dimensions,
    void   (*function) ( void *, Real [],  Real [], Real ** ),
    void   *function_data,
    Real   initial_guess[],
    Real   desired_values[],
    Real   solution[],
    Real   function_tolerance,
    Real   delta_tolerance,
    int    max_iterations );

public  void  create_orthogonal_vector(
    Vector  *v,
    Vector  *ortho );

public  void  create_two_orthogonal_vectors(
    Vector   *v,
    Vector   *v1,
    Vector   *v2 );

public  BOOLEAN   compute_transform_inverse(
    Transform  *transform,
    Transform  *inverse );

public  void  get_linear_spline_coefs(
    Real  **coefs );

public  void  get_quadratic_spline_coefs(
    Real  **coefs );

public  void  get_cubic_spline_coefs(
    Real  **coefs );

public  Real  cubic_interpolate(
    Real   u,
    Real   v0,
    Real   v1,
    Real   v2,
    Real   v3 );

public  void  evaluate_univariate_interpolating_spline(
    Real    u,
    int     degree,
    Real    coefs[],
    int     n_derivs,
    Real    derivs[] );

public  void  evaluate_bivariate_interpolating_spline(
    Real    u,
    Real    v,
    int     degree,
    Real    coefs[],
    int     n_derivs,
    Real    derivs[] );

public  void  evaluate_trivariate_interpolating_spline(
    Real    u,
    Real    v,
    Real    w,
    int     degree,
    Real    coefs[],
    int     n_derivs,
    Real    derivs[] );

public  void  evaluate_interpolating_spline(
    int     n_dims,
    Real    parameters[],
    int     degree,
    int     n_values,
    Real    coefs[],
    int     n_derivs,
    Real    derivs[] );

public  void  spline_tensor_product(
    int     n_dims,
    Real    positions[],
    int     degrees[],
    Real    *bases[],
    int     n_values,
    Real    coefs[],
    int     n_derivs[],
    Real    results[] );

public  void  make_identity_transform( Transform   *transform );

public  BOOLEAN  close_to_identity(
    Transform   *transform );

public  void  get_transform_origin(
    Transform   *transform,
    Point       *origin );

public  void  set_transform_origin(
    Transform   *transform,
    Point       *origin );

public  void  get_transform_origin_real(
    Transform   *transform,
    Real        origin[] );

public  void  get_transform_x_axis(
    Transform   *transform,
    Vector      *x_axis );

public  void  get_transform_x_axis_real(
    Transform   *transform,
    Real        x_axis[] );

public  void  set_transform_x_axis(
    Transform   *transform,
    Vector      *x_axis );

public  void  set_transform_x_axis_real(
    Transform   *transform,
    Real        x_axis[] );

public  void  get_transform_y_axis(
    Transform   *transform,
    Vector      *y_axis );

public  void  get_transform_y_axis_real(
    Transform   *transform,
    Real        y_axis[] );

public  void  set_transform_y_axis(
    Transform   *transform,
    Vector      *y_axis );

public  void  set_transform_y_axis_real(
    Transform   *transform,
    Real        y_axis[] );

public  void  get_transform_z_axis(
    Transform   *transform,
    Vector      *z_axis );

public  void  get_transform_z_axis_real(
    Transform   *transform,
    Real        z_axis[] );

public  void  set_transform_z_axis(
    Transform   *transform,
    Vector      *z_axis );

public  void  set_transform_z_axis_real(
    Transform   *transform,
    Real        z_axis[] );

public  void   make_change_to_bases_transform(
    Point      *origin,
    Vector     *x_axis,
    Vector     *y_axis,
    Vector     *z_axis,
    Transform  *transform );

public  void   make_change_from_bases_transform(
    Point      *origin,
    Vector     *x_axis,
    Vector     *y_axis,
    Vector     *z_axis,
    Transform  *transform );

public  void   concat_transforms(
    Transform   *result,
    Transform   *t1,
    Transform   *t2 );

public  void  transform_point(
    Transform  *transform,
    Real       x,
    Real       y,
    Real       z,
    Real       *x_trans,
    Real       *y_trans,
    Real       *z_trans );

public  void  transform_vector(
    Transform  *transform,
    Real       x,
    Real       y,
    Real       z,
    Real       *x_trans,
    Real       *y_trans,
    Real       *z_trans );

public  void  *alloc_memory_in_bytes(
    size_t       n_bytes
    _ALLOC_SOURCE_LINE_ARG_DEF );

public  void  *alloc_memory_1d(
    size_t       n_elements,
    size_t       type_size
    _ALLOC_SOURCE_LINE_ARG_DEF );

public  void  *alloc_memory_2d(
    size_t       n1,
    size_t       n2,
    size_t       type_size
    _ALLOC_SOURCE_LINE_ARG_DEF );

public  void  *alloc_memory_3d(
    size_t       n1,
    size_t       n2,
    size_t       n3,
    size_t       type_size
    _ALLOC_SOURCE_LINE_ARG_DEF );

public  void  *alloc_memory_4d(
    size_t       n1,
    size_t       n2,
    size_t       n3,
    size_t       n4,
    size_t       type_size
    _ALLOC_SOURCE_LINE_ARG_DEF );

public  void  *alloc_memory_5d(
    size_t       n1,
    size_t       n2,
    size_t       n3,
    size_t       n4,
    size_t       n5,
    size_t       type_size
    _ALLOC_SOURCE_LINE_ARG_DEF );

public  void  realloc_memory(
    void      **ptr,
    size_t    n_elements,
    size_t    type_size
    _ALLOC_SOURCE_LINE_ARG_DEF );

public  void  free_memory_1d(
    void   **ptr
    _ALLOC_SOURCE_LINE_ARG_DEF );

public  void  free_memory_2d(
    void   ***ptr
    _ALLOC_SOURCE_LINE_ARG_DEF );

public  void  free_memory_3d(
    void   ****ptr
    _ALLOC_SOURCE_LINE_ARG_DEF );

public  void  free_memory_4d(
    void   *****ptr
    _ALLOC_SOURCE_LINE_ARG_DEF );

public  void  free_memory_5d(
    void   ******ptr
    _ALLOC_SOURCE_LINE_ARG_DEF );

public  size_t  get_total_memory_alloced( void );

public  BOOLEAN  alloc_checking_enabled( void );

public  void  set_alloc_checking( BOOLEAN state );

public  void  record_ptr_alloc_check(
    void      *ptr,
    size_t    n_bytes,
    STRING    source_file,
    int       line_number );

public  void  change_ptr_alloc_check(
    void      *old_ptr,
    void      *new_ptr,
    size_t    n_bytes,
    STRING    source_file,
    int       line_number );

public  BOOLEAN  unrecord_ptr_alloc_check(
    void     *ptr,
    STRING   source_file,
    int      line_number );

public  void  output_alloc_to_file(
    STRING   filename );

public  void  print_alloc_source_line(
    STRING  filename,
    int     line_number );

public  void  set_array_size(
    void      **array,
    size_t    type_size,
    size_t    previous_n_elems,
    size_t    new_n_elems,
    size_t    chunk_size
    _ALLOC_SOURCE_LINE_ARG_DEF );

public  BOOLEAN  real_is_double( void );

public  BOOLEAN  file_exists(
    STRING        filename );

public  BOOLEAN  file_directory_exists(
    STRING        filename );

public  BOOLEAN  check_clobber_file(
    STRING   filename );

public  BOOLEAN  check_clobber_file_default_suffix(
    STRING   filename,
    STRING   default_suffix );

public  Status  make_backup_file(
    STRING   filename,
    STRING   *backup_filename );

public  void  cleanup_backup_file(
    STRING   filename,
    STRING   backup_filename,
    Status   status_of_write );

public  void  remove_file(
    STRING  filename );

public  Status  copy_file(
    STRING  src,
    STRING  dest );

public  Status  move_file(
    STRING  src,
    STRING  dest );

public  STRING  expand_filename(
    STRING  filename );

public  BOOLEAN  filename_extension_matches(
    STRING   filename,
    STRING   extension );

public  STRING  remove_directories_from_filename(
    STRING  filename );

public  BOOLEAN  file_exists_as_compressed(
    STRING       filename,
    STRING       *compressed_filename );

public  STRING  get_temporary_filename( void );

public  Status  open_file(
    STRING             filename,
    IO_types           io_type,
    File_formats       file_format,
    FILE               **file );

public  Status  open_file_with_default_suffix(
    STRING             filename,
    STRING             default_suffix,
    IO_types           io_type,
    File_formats       file_format,
    FILE               **file );

public  Status  set_file_position(
    FILE     *file,
    long     byte_position );

public  Status  close_file(
    FILE     *file );

public  STRING  extract_directory(
    STRING    filename );

public  STRING  get_absolute_filename(
    STRING    filename,
    STRING    directory );

public  Status  flush_file(
    FILE     *file );

public  Status  input_character(
    FILE  *file,
    char   *ch );

public  Status  unget_character(
    FILE  *file,
    char  ch );

public  Status  input_nonwhite_character(
    FILE   *file,
    char   *ch );

public  Status  output_character(
    FILE   *file,
    char   ch );

public  Status   skip_input_until(
    FILE   *file,
    char   search_char );

public  Status  output_string(
    FILE    *file,
    STRING  str );

public  Status  input_string(
    FILE    *file,
    STRING  *str,
    char    termination_char );

public  Status  input_quoted_string(
    FILE            *file,
    STRING          *str );

public  Status  input_possibly_quoted_string(
    FILE            *file,
    STRING          *str );

public  Status  output_quoted_string(
    FILE            *file,
    STRING          str );

public  Status  input_binary_data(
    FILE            *file,
    void            *data,
    size_t          element_size,
    int             n );

public  Status  output_binary_data(
    FILE            *file,
    void            *data,
    size_t          element_size,
    int             n );

public  Status  input_newline(
    FILE            *file );

public  Status  output_newline(
    FILE            *file );

public  Status  input_line(
    FILE    *file,
    STRING  *line );

public  Status  input_boolean(
    FILE            *file,
    BOOLEAN         *b );

public  Status  output_boolean(
    FILE            *file,
    BOOLEAN         b );

public  Status  input_short(
    FILE            *file,
    short           *s );

public  Status  output_short(
    FILE            *file,
    short           s );

public  Status  input_unsigned_short(
    FILE            *file,
    unsigned short  *s );

public  Status  output_unsigned_short(
    FILE            *file,
    unsigned short  s );

public  Status  input_int(
    FILE  *file,
    int   *i );

public  Status  output_int(
    FILE            *file,
    int             i );

public  Status  input_real(
    FILE            *file,
    Real            *r );

public  Status  output_real(
    FILE            *file,
    Real            r );

public  Status  input_float(
    FILE            *file,
    float           *f );

public  Status  output_float(
    FILE            *file,
    float           f );

public  Status  input_double(
    FILE            *file,
    double          *d );

public  Status  output_double(
    FILE            *file,
    double          d );

public  Status  io_binary_data(
    FILE            *file,
    IO_types        io_flag,
    void            *data,
    size_t          element_size,
    int             n );

public  Status  io_newline(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format );

public  Status  io_quoted_string(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format,
    STRING          *str );

public  Status  io_boolean(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format,
    BOOLEAN         *b );

public  Status  io_short(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format,
    short           *short_int );

public  Status  io_unsigned_short(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format,
    unsigned short  *unsigned_short );

public  Status  io_unsigned_char(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format,
    unsigned  char  *c );

public  Status  io_int(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format,
    int             *i );

public  Status  io_real(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format,
    Real            *r );

public  Status  io_float(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format,
    float           *f );

public  Status  io_double(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format,
    double          *d );

public  Status  io_ints(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format,
    int             n,
    int             *ints[] );

public  Status  io_unsigned_chars(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format,
    int             n,
    unsigned char   *unsigned_chars[] );

public  void  set_print_function( void  (*function) ( STRING ) );

public  void  push_print_function( void );

public  void  pop_print_function( void );

public  void  print( STRING format, ... );

public  void  set_print_error_function( void  (*function) ( char [] ) );

public  void  push_print_error_function( void );

public  void  pop_print_error_function( void );

public  void  print_error( char format[], ... );

public  void   handle_internal_error( char  str[] );

public  void  abort_if_allowed( void );

public  void  initialize_progress_report(
    progress_struct   *progress,
    BOOLEAN           one_line_only,
    int               n_steps,
    STRING            title );

public  void  update_progress_report(
    progress_struct   *progress,
    int               current_step );

public  void  terminate_progress_report(
    progress_struct   *progress );

public  STRING  alloc_string(
    int   length );

public  STRING  create_string(
    STRING    initial );

public  void  delete_string(
    STRING   string );

public  STRING  concat_strings(
    STRING   str1,
    STRING   str2 );

public  void  replace_string(
    STRING   *string,
    STRING   new_string );

public  void  concat_char_to_string(
    STRING   *string,
    char     ch );

public  void  concat_to_string(
    STRING   *string,
    STRING   str2 );

public  int  string_length(
    STRING   string );

public  BOOLEAN  equal_strings(
    STRING   str1,
    STRING   str2 );

public  BOOLEAN  is_lower_case(
    char  ch );

public  BOOLEAN  is_upper_case(
    char  ch );

public  char  get_lower_case(
    char   ch );

public  char  get_upper_case(
    char   ch );

public  BOOLEAN  string_ends_in(
    STRING   string,
    STRING   ending );

public    STRING   strip_outer_blanks(
    STRING  str );

public  int  find_character(
    STRING    string,
    char      ch );

public  void  make_string_upper_case(
    STRING    string );

public  BOOLEAN  blank_string(
    STRING   string );

public  Real  current_cpu_seconds( void );

public  Real  current_realtime_seconds( void );

public  STRING  format_time(
    STRING   format,
    Real     seconds );

public  void  print_time(
    STRING   format,
    Real     seconds );

public  STRING  get_clock_time( void );

public  void  sleep_program( Real seconds );

public  STRING  get_date( void );
#endif
