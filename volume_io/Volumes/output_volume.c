#include  <volume_io.h>

private  Status   get_file_dimension_names(
    char     filename[],
    int      n_dims,
    STRING   dim_names[] )
{
    static  char   *any_names[MAX_DIMENSIONS] = { "", "", "", "", "" };
    int                   i;
    Status                status;
    volume_input_struct   volume_input;
    Volume                tmp_volume;

    status = start_volume_input( filename, n_dims, any_names,
                                 NC_UNSPECIFIED, FALSE, 0.0, 0.0,
                                 TRUE, &tmp_volume, (minc_input_options *) NULL,
                                 &volume_input );

    if( status == OK )
    {
        for_less( i, 0, n_dims )
            (void) strcpy( dim_names[i], volume_input.minc_file->dim_names[i] );

        delete_volume_input( &volume_input );
        delete_volume( tmp_volume );
    }

    return( status );
}

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
    Status               status;
    Minc_file            minc_file;
    int                  n_dims, sizes[MAX_DIMENSIONS];
    int                  vol_sizes[MAX_DIMENSIONS];
    int                  i, j, n_found;
    Real                 real_min, real_max;
    char                 **vol_dimension_names;
    BOOLEAN              done[MAX_DIMENSIONS];
    STRING               dim_names[MAX_DIMENSIONS];
    minc_output_options  used_options;

    if( file_nc_data_type == NC_UNSPECIFIED )
    {
        file_nc_data_type = volume->nc_data_type;
        file_signed_flag = volume->signed_flag;
        get_volume_voxel_range( volume, &file_voxel_min, &file_voxel_max );
    }

    get_volume_sizes( volume, vol_sizes );

    n_dims = get_volume_n_dimensions(volume);
    vol_dimension_names = get_volume_dimension_names( volume );

    /*--- either get the output dim name ordering from the original
          filename, the volume, or the options */

    if( options == NULL || strlen( options->dimension_names[0] ) == 0 )
    {
        if( original_filename != NULL )
        {
            if( get_file_dimension_names( original_filename, n_dims,
                                          dim_names ) != OK )
                return( ERROR );
        }
        else
        {
            for_less( i, 0, n_dims )
                (void) strcpy( dim_names[i], vol_dimension_names[i] );
        }
    }
    else
    {
        for_less( i, 0, n_dims )
            (void) strcpy( dim_names[i], options->dimension_names[i] );
    }

    n_found = 0;
    for_less( i, 0, n_dims )
        done[i] = FALSE;

    for_less( i, 0, n_dims )
    {
        for_less( j, 0, n_dims )
        {
            if( !done[j] &&
                strcmp( vol_dimension_names[i], dim_names[j] ) == 0 )
            {
                sizes[j] = vol_sizes[i];
                ++n_found;
                done[j] = TRUE;
            }
        }
    }

    delete_dimension_names( vol_dimension_names );

    if( n_found != n_dims )
    {
        print( "output_modified_volume: invalid dimension names option.\n");
        return( ERROR );
    }

    if( options == (minc_output_options *) NULL )
        set_default_minc_output_options( &used_options );
    else
        used_options = *options;

    if( used_options.global_image_range[0] >=
        used_options.global_image_range[1] )
    {
        get_volume_real_range( volume, &real_min, &real_max );
        set_minc_output_real_range( &used_options, real_min, real_max );
    }

    minc_file = initialize_minc_output( filename,
                                        n_dims, dim_names, sizes,
                                        file_nc_data_type, file_signed_flag,
                                        file_voxel_min, file_voxel_max,
                                        get_voxel_to_world_transform(volume),
                                        volume, &used_options );

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
        status = output_minc_volume( minc_file );

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
