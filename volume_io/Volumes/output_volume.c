#include  <volume_io.h>

public  Status  output_modified_volume(
    char                  filename[],
    nc_type               file_nc_data_type,
    BOOLEAN               file_signed_flag,
    Real                  file_voxel_min,
    Real                  file_voxel_max,
    Volume                volume,
    char                  original_filename[],
    char                  history[],
    minc_output_options   *options )
{
    Status       status;
    Minc_file    minc_file;
    int          sizes[MAX_DIMENSIONS];
    Real         min_value, max_value;
    char         **dimension_names;

    if( file_nc_data_type == NC_UNSPECIFIED )
    {
        file_nc_data_type = volume->nc_data_type;
        file_signed_flag = volume->signed_flag;
        get_volume_voxel_range( volume, &file_voxel_min, &file_voxel_max );
    }

    get_volume_real_range( volume, &min_value, &max_value );
    get_volume_sizes( volume, sizes );

    dimension_names = get_volume_dimension_names( volume );

    minc_file = initialize_minc_output( filename,
                                        get_volume_n_dimensions(volume),
                                        dimension_names, sizes,
                                        file_nc_data_type, file_signed_flag,
                                        file_voxel_min, file_voxel_max,
                                        min_value, max_value,
                                        get_voxel_to_world_transform(volume),
                                        options );

    delete_dimension_names( dimension_names );

    status = OK;

    if( minc_file == (Minc_file) NULL )
        status = ERROR;

    if( status == OK && original_filename != (char *) NULL )
        status = copy_auxiliary_data_from_minc_file( minc_file,
                                                     original_filename,
                                                     history );

    else if( status == OK && history != (char *) NULL )
        status = add_minc_history( minc_file, history );

    if( status == OK )
        status = output_minc_volume( minc_file, volume );

    if( status == OK )
        status = close_minc_output( minc_file );

    return( status );
}

public  Status  output_volume(
    char                  filename[],
    nc_type               file_nc_data_type,
    BOOLEAN               file_signed_flag,
    Real                  file_voxel_min,
    Real                  file_voxel_max,
    Volume                volume,
    char                  history[],
    minc_output_options   *options )
{
    return( output_modified_volume( filename, file_nc_data_type,
                                    file_signed_flag,
                                    file_voxel_min, file_voxel_max,
                                    volume, (char *) NULL, history, options ) );
}
