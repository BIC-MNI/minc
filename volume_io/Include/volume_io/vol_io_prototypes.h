#ifndef  DEF_VOLUME_IO_PROTOTYPES
#define  DEF_VOLUME_IO_PROTOTYPES

public  void  alloc_memory(
    void   **ptr,
    int    n_bytes );

public  void  realloc_memory(
    void   **ptr,
    int    n_bytes );

public  void  free_memory( void   **ptr );

public  void  abort_if_allowed( void );

public  int  get_total_memory_alloced( void );

public  void  set_alloc_checking( BOOLEAN state );

public  void  record_ptr(
    void   *ptr,
    int    n_bytes,
    char   source_file[],
    int    line_number );

public  void  change_ptr(
    void   *old_ptr,
    void   *new_ptr,
    int    n_bytes,
    char   source_file[],
    int    line_number );

public  BOOLEAN  unrecord_ptr(
    void   *ptr,
    char   source_file[],
    int    line_number );

public  void  output_alloc_to_file(
    char   filename[] );

public  BOOLEAN  real_is_double();

public  BOOLEAN  file_exists(
    char        filename[] );

public  void  remove_file(
    char  filename[] );

public  void  unlink_file(
    char  filename[] );

public  void  expand_filename(
    char  filename[],
    char  expanded_filename[] );

public  BOOLEAN  filename_extension_matches(
    char   filename[],
    char   extension[] );

public  void  remove_directories_from_filename(
    char  filename[],
    char  filename_no_directories[] );

public  Status  open_file(
    char               filename[],
    IO_types           io_type,
    File_formats       file_format,
    FILE               **file );

public  Status  open_file_with_default_suffix(
    char               filename[],
    char               default_suffix[],
    IO_types           io_type,
    File_formats       file_format,
    FILE               **file );

public  Status  set_file_position(
    FILE     *file,
    long     byte_position );

public  Status  close_file(
    FILE     *file );

public  void  extract_directory(
    char    filename[],
    char    directory[] );

public  void  get_absolute_filename(
    char    filename[],
    char    directory[],
    char    abs_filename[] );

public  void  strip_off_directories(
    char    filename[],
    char    no_dirs[] );

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
    FILE  *file,
    char  str[] );

public  Status  input_string(
    FILE  *file,
    char  str[],
    int   string_length,
    char  termination_char );

public  Status  input_quoted_string(
    FILE            *file,
    char            str[],
    int             str_length );

public  Status  output_quoted_string(
    FILE            *file,
    char            str[] );

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
    char    line[],
    int     str_length );

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

public  Status  input_int( FILE * file, int * i );

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
    char            str[],
    int             str_length );

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

public  void  set_print_function( void (*function) ( char [] ) );

public  void  push_print_function();

public  void  pop_print_function();

public  void  print( char format[], ... );

public  void  initialize_progress_report(
    progress_struct   *progress,
    BOOLEAN           one_line_only,
    int               n_steps,
    char              title[] );

public  void  update_progress_report(
    progress_struct   *progress,
    int               current_step );

public  void  terminate_progress_report(
    progress_struct   *progress );

public  BOOLEAN  string_ends_in(
    char   string[],
    char   ending[] );

public    void   strip_blanks(
    char  str[],
    char  stripped[] );

public  int  find_character(
    char    string[],
    char    ch );

public  void  get_upper_case_string(
    char    string[],
    char    upper_case[] );

public  BOOLEAN  blank_string(
    char   string[] );

public  Real  current_cpu_seconds( void );

public  Real  current_realtime_seconds( void );

public  void  format_time(
    char   str[],
    char   format[],
    Real   seconds );

public  void  print_time(
    char   format[],
    Real   seconds );

public  void  get_clock_time(
    char  time_str[] );

public  void  sleep_program( Real seconds );

public  void  get_date(
    char  date_str[] );

public  Status  initialize_free_format_input(
    char                 filename[],
    Volume               volume,
    volume_input_struct  *volume_input );

public  void  delete_free_format_input(
    volume_input_struct   *volume_input );

public  BOOLEAN  input_more_free_format_file(
    Volume                volume,
    volume_input_struct   *volume_input,
    Real                  *fraction_done );

public  Minc_file  initialize_minc_input(
    char                 filename[],
    Volume               volume,
    minc_input_options   *options );

public  int  get_n_input_volumes(
    Minc_file  file );

public  Status  close_minc_input(
    Minc_file   file );

public  BOOLEAN  input_more_minc_file(
    Minc_file   file,
    Real        *fraction_done );

public  BOOLEAN  advance_input_volume(
    Minc_file   file );

public  void  reset_input_volume(
    Minc_file   file );

public  BOOLEAN  is_spatial_dimension(
    char   dimension_name[],
    int    *axis );

public  int  get_minc_file_id(
    Minc_file  file );

public  void  set_default_minc_input_options(
    minc_input_options  *options );

public  void  set_minc_input_promote_invalid_to_min_flag(
    minc_input_options  *options,
    BOOLEAN             flag );

public  void  set_minc_input_vector_to_scalar_flag(
    minc_input_options  *options,
    BOOLEAN             flag );

public  Status  start_volume_input(
    char                 filename[],
    int                  n_dimensions,
    char                 *dim_names[],
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
    char                 filename[],
    int                  n_dimensions,
    char                 *dim_names[],
    nc_type              volume_nc_data_type,
    BOOLEAN              volume_signed_flag,
    Real                 volume_voxel_min,
    Real                 volume_voxel_max,
    BOOLEAN              create_volume_flag,
    Volume               *volume,
    minc_input_options   *options );

public  Minc_file  initialize_minc_output(
    char                   filename[],
    int                    n_dimensions,
    char                   *dim_names[],
    int                    sizes[],
    nc_type                file_nc_data_type,
    BOOLEAN                file_signed_flag,
    Real                   file_voxel_min,
    Real                   file_voxel_max,
    Real                   real_min,
    Real                   real_max,
    General_transform      *voxel_to_world_transform,
    minc_output_options    *options );

public  Status  copy_auxiliary_data_from_minc_file(
    Minc_file   file,
    char        filename[],
    char        history_string[] );

public  Status  copy_auxiliary_data_from_open_minc_file(
    Minc_file   file,
    int         src_cdfid,
    char        history_string[] );

public  Status  add_minc_history(
    Minc_file   file,
    char        history_string[] );

public  Status  output_minc_volume(
    Minc_file   file,
    Volume      volume );

public  Status  close_minc_output(
    Minc_file   file );

public  void  set_default_minc_output_options(
    minc_output_options  *options           /* ARGSUSED */ );

public  Status  output_modified_volume(
    char                  filename[],
    nc_type               file_nc_data_type,
    BOOLEAN               file_signed_flag,
    Real                  file_voxel_min,
    Real                  file_voxel_max,
    Volume                volume,
    char                  original_filename[],
    char                  history[],
    minc_output_options   *options );

public  Status  output_volume(
    char                  filename[],
    nc_type               file_nc_data_type,
    BOOLEAN               file_signed_flag,
    Real                  file_voxel_min,
    Real                  file_voxel_max,
    Volume                volume,
    char                  history[],
    minc_output_options   *options );

public  char  **get_default_dim_names(
    int    n_dimensions );

public  BOOLEAN  convert_dim_name_to_axis(
    char    name[],
    int     *axis );

public   Volume   create_volume(
    int         n_dimensions,
    char        *dimension_names[],
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

public  int  get_type_size(
    Data_types   type );

public  void  alloc_volume_data(
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

public  int  get_volume_total_n_voxels(
    Volume    volume );

public  void  set_voxel_to_world_transform(
    Volume             volume,
    General_transform  *transform );

public  General_transform  *get_voxel_to_world_transform(
    Volume   volume );

public  void  compute_world_transform(
    int                 spatial_axes[N_DIMENSIONS],
    Real                separations[],
    Real                translation_voxel[],
    Real                world_space_for_translation_voxel[N_DIMENSIONS],
    Real                direction_cosines[][N_DIMENSIONS],
    General_transform   *world_transform );

public  char  **get_volume_dimension_names(
    Volume   volume );

public  void  delete_dimension_names(
    char   **dimension_names );

public  void  get_volume_separations(
    Volume   volume,
    Real     separations[] );

public  void  set_volume_separations(
    Volume   volume,
    Real     separations[] );

public  void  set_volume_translation(
    Volume  volume,
    Real    voxel[],
    Real    world_space_voxel_maps_to[] );

public  void  get_volume_translation(
    Volume  volume,
    Real    voxel[],
    Real    world_space_voxel_maps_to[] );

public  void  set_volume_direction_cosine(
    Volume   volume,
    int      axis,
    Real     dir[] );

public  void  convert_voxel_to_world(
    Volume   volume,
    Real     voxel[],
    Real     *x_world,
    Real     *y_world,
    Real     *z_world );

public  void  convert_3D_voxel_to_world(
    Volume   volume,
    Real     x_voxel,
    Real     y_voxel,
    Real     z_voxel,
    Real     *x_world,
    Real     *y_world,
    Real     *z_world );

public  void  convert_voxel_normal_vector_to_world(
    Volume          volume,
    Real            x_voxel,
    Real            y_voxel,
    Real            z_voxel,
    Real            *x_world,
    Real            *y_world,
    Real            *z_world );

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
    Real     *x_voxel,
    Real     *y_voxel,
    Real     *z_voxel );

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

public  Volume   copy_volume_definition(
    Volume   volume,
    nc_type  nc_data_type,
    BOOLEAN  signed_flag,
    Real     voxel_min,
    Real     voxel_max );

public  Status  mni_get_nonwhite_character(
    FILE   *file,
    char   *ch );

public  Status  mni_skip_expected_character(
    FILE   *file,
    char   expected_ch );

public  Status  mni_input_line(
    FILE   *file,
    char   string[],
    int    max_length );

public  Status  mni_input_string(
    FILE   *file,
    char   string[],
    int    max_length,
    char   termination_char1,
    char   termination_char2 );

public  Status  mni_input_keyword_and_equal_sign(
    FILE         *file,
    const char   keyword[],
    BOOLEAN      print_error_message );

public  Status  mni_input_double(
    FILE    *file,
    double  *d );

public  Status  mni_input_doubles(
    FILE    *file,
    int     *n,
    double  *doubles[] );

public  Status  mni_input_int(
    FILE    *file,
    int     *i );

public  void  output_comments(
    FILE   *file,
    char   comments[] );

public  char  *get_default_tag_file_suffix();

public  Status  output_tag_points(
    FILE      *file,
    char      comments[],
    int       n_volumes,
    int       n_tag_points,
    Real      **tags_volume1,
    Real      **tags_volume2,
    Real      weights[],
    int       structure_ids[],
    int       patient_ids[],
    char      **labels );

public  void  free_tag_points(
    int       n_volumes,
    int       n_tag_points,
    Real      **tags_volume1,
    Real      **tags_volume2,
    Real      weights[],
    int       structure_ids[],
    int       patient_ids[],
    char      **labels );

public  Status  input_tag_points(
    FILE      *file,
    int       *n_volumes,
    int       *n_tag_points,
    Real      ***tags_volume1,
    Real      ***tags_volume2,
    Real      **weights,
    int       **structure_ids,
    int       **patient_ids,
    char      ***labels );

public  Status  output_tag_file(
    char      filename[],
    char      comments[],
    int       n_volumes,
    int       n_tag_points,
    Real      **tags_volume1,
    Real      **tags_volume2,
    Real      weights[],
    int       structure_ids[],
    int       patient_ids[],
    char      **labels );

public  Status  input_tag_file(
    char      filename[],
    int       *n_volumes,
    int       *n_tag_points,
    Real      ***tags_volume1,
    Real      ***tags_volume2,
    Real      **weights,
    int       **structure_ids,
    int       **patient_ids,
    char      ***labels );

public  void  thin_plate_spline_transform(
    int     n_dimensions,
    int     n_points,
    float   **points,
    float   **displacements,
    Real    x,
    Real    y,
    Real    z,
    Real    *x_transformed,
    Real    *y_transformed,
    Real    *z_transformed );

public  void  thin_plate_spline_inverse_transform(
    int     n_dimensions,
    int     n_points,
    float   **points,
    float   **displacements,
    Real    x,
    Real    y,
    Real    z,
    Real    *x_transformed,
    Real    *y_transformed,
    Real    *z_transformed );

public  char  *get_default_transform_file_suffix();

public  Status  output_transform(
    FILE                *file,
    char                comments[],
    General_transform   *transform );

public  Status  input_transform(
    FILE                *file,
    General_transform   *transform );

public  Status  output_transform_file(
    char                filename[],
    char                comments[],
    General_transform   *transform );

public  Status  input_transform_file(
    char                filename[],
    General_transform   *transform );

public  void  create_linear_transform(
    General_transform   *transform,
    Transform           *linear_transform );

public  void  create_thin_plate_transform(
    General_transform    *transform,
    int                  n_dimensions,
    int                  n_points,
    float                **points,
    float                **displacements );

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

public  void  create_inverse_general_transform(
    General_transform   *transform,
    General_transform   *inverse );

public  void  concat_general_transforms(
    General_transform   *first,
    General_transform   *second,
    General_transform   *result );

public  void  delete_general_transform(
    General_transform   *transform );

public  BOOLEAN  null_Point(
    Point   *point );

public  BOOLEAN  null_Vector(
    Vector   *vector );

public  Real  distance_between_points(
    Point  *p1,
    Point  *p2 );

public  BOOLEAN  points_within_distance(
    Point  *p1,
    Point  *p2,
    Real   distance );

public  void  create_noncolinear_vector(
    Vector  *v,
    Vector  *not_v );

public  void  create_orthogonal_vector(
    Vector   *v1,
    Vector   *v2,
    Vector   *v );

public  void  create_two_orthogonal_vectors(
    Vector   *v,
    Vector   *v1,
    Vector   *v2 );

public  void  apply_point_to_min_and_max(
    Point   *point,
    Point   *min_point,
    Point   *max_point );

public  void  expand_min_and_max_points(
    Point   *min_point,
    Point   *max_point,
    Point   *min_to_check,
    Point   *max_to_check );

public  void  get_range_points(
    int                n_points,
    Point              points[],
    Point              *min_corner,
    Point              *max_corner );

public  void  get_points_centroid(
    int     n_points,
    Point   points[],
    Point   *centroid );

public   void     reverse_vectors(
    int       n_vectors,
    Vector    vectors[] );

public  void lubksb(
    float **a,
    int   n,
    int   *indx,
    float *b );

public  void ludcmp(
    float    **a,
    int      n,
    int      *indx,
    float    *d );

public  BOOLEAN   compute_transform_inverse(
    Transform  *transform,
    Transform  *inverse );

public  void  make_identity_transform( Transform   *transform );

public  BOOLEAN  close_to_identity(
    Transform   *transform );

public  void  make_scale_transform( 
    Real        sx,
    Real        sy,
    Real        sz,
    Transform   *transform );

public  void  get_transform_origin(
    Transform   *transform,
    Point       *origin );

public  void  set_transform_origin(
    Transform   *transform,
    Point       *origin );

public  void  get_transform_x_axis(
    Transform   *transform,
    Vector      *x_axis );

public  void  set_transform_x_axis(
    Transform   *transform,
    Vector      *x_axis );

public  void  get_transform_y_axis(
    Transform   *transform,
    Vector      *y_axis );

public  void  set_transform_y_axis(
    Transform   *transform,
    Vector      *y_axis );

public  void  get_transform_z_axis(
    Transform   *transform,
    Vector      *z_axis );

public  void  set_transform_z_axis(
    Transform   *transform,
    Vector      *z_axis );

public  void  set_transform_x_and_z_axes(
    Transform   *transform,
    Vector      *x_axis,
    Vector      *z_axis );

public  void  make_translation_transform(
    Real        x_trans,
    Real        y_trans,
    Real        z_trans,
    Transform   *transform );

public  void  make_origin_transform(
    Point      *origin,
    Transform   *transform );

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

public  void   compute_inverse_of_orthogonal_transform(
    Transform  *transform,
    Transform  *inverse );

public  void   concat_transforms(
    Transform   *result,
    Transform   *t1,
    Transform   *t2 );

public  void  make_rotation_transform(
    Real       radians,
    int        axis,
    Transform  *transform );

public  void  make_transform_relative_to_point(
    Point      *point,
    Transform  *transform,
    Transform  *rel_transform );

public  void  make_transform_in_coordinate_system(
    Point      *origin,
    Vector     *x_axis,
    Vector     *y_axis,
    Vector     *z_axis,
    Transform  *transform,
    Transform  *rel_transform );

public  void  make_rotation_about_axis(
    Vector     *axis,
    Real       angle,
    Transform  *transform );

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

public  void  inverse_transform_point(
    Transform  *transform,
    Real       x,
    Real       y,
    Real       z,
    Real       *x_trans,
    Real       *y_trans,
    Real       *z_trans );

public  void  inverse_transform_vector(
    Transform  *transform,
    Real       x,
    Real       y,
    Real       z,
    Real       *x_trans,
    Real       *y_trans,
    Real       *z_trans );

public  void  convert_2d_transform_to_rotation_translation(
    Transform  *transform,
    Real       *degrees_clockwise,
    Real       *x_trans,
    Real       *y_trans );

public  Real  compute_clockwise_rotation( Real x, Real y );

public  void  make_identity_transform_2d( Transform_2d *transform );

public  void  get_inverse_transform_2d(
    Transform_2d   *transform,
    Transform_2d   *inverse );

public  void  transform_point_2d(
    Transform_2d   *transform,
    Real           x,
    Real           y,
    Real           *x_trans,
    Real           *y_trans );

public  Status  write_transform_file(
    char       filename[],
    char       comments[],
    Transform  *transform );

public  Status  read_transform_file(
    char       filename[],
    Transform  *transform );

public  void  get_least_squares_transform_2d(
    int           n_points,
    Real          x[],
    Real          y[],
    Real          x_trans[],
    Real          y_trans[],
    Transform_2d  *transform_2d );
#endif
