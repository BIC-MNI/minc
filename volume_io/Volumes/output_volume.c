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

#include  <internal_volume_io.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Volumes/output_volume.c,v 1.16 1995-10-19 15:47:09 david Exp $";
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_file_dimension_names
@INPUT      : filename
              n_dims
@OUTPUT     : dim_names
@RETURNS    : OK or ERROR
@DESCRIPTION: Gets the names of the dimensions from the specified file.
              dim_names is an array of STRINGS, where the array has been
              allocated, but not each string.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status   get_file_dimension_names(
    STRING   filename,
    int      n_dims,
    STRING   dim_names[] )
{
    int                   i;
    Status                status;
    volume_input_struct   volume_input;
    Volume                tmp_volume;

    status = start_volume_input( filename, n_dims, File_order_dimension_names,
                                 NC_UNSPECIFIED, FALSE, 0.0, 0.0,
                                 TRUE, &tmp_volume, (minc_input_options *) NULL,
                                 &volume_input );

    if( status == OK )
    {
        for_less( i, 0, n_dims )
        {
            dim_names[i] = create_string( volume_input.minc_file->dim_names[i]);
        }

        delete_volume_input( &volume_input );
        delete_volume( tmp_volume );
    }

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : output_modified_volume
@INPUT      : filename
              file_nc_data_type
              file_signed_flag
              file_voxel_min
              file_voxel_max
              volume
              original_filename
              history
              options
@OUTPUT     : 
@RETURNS    : OK or ERROR
@DESCRIPTION: Creates a Minc file and outputs the volume to it.  The data
              type of the file is either specified by the second through fifth
              parameters, or by the volume, if file_nc_data_type is
              NC_UNSPECIFIED.  The volume is assumed to be derived, in some
              fashion, from an existing MINC file, and the auxiliary data
              from the existing MINC file, 'original_filename', is
              copied to the output file, along with the 'history' string.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  output_modified_volume(
    STRING                filename,
    nc_type               file_nc_data_type,
    BOOLEAN               file_signed_flag,
    Real                  file_voxel_min,
    Real                  file_voxel_max,
    Volume                volume,
    STRING                original_filename,
    STRING                history,
    minc_output_options   *options )
{
    Status               status;
    Minc_file            minc_file;
    int                  n_dims, sizes[MAX_DIMENSIONS];
    int                  vol_sizes[MAX_DIMENSIONS];
    int                  i, j, n_found;
    Real                 real_min, real_max;
    STRING               *vol_dimension_names;
    BOOLEAN              done[MAX_DIMENSIONS], copy_original_file_data;
    STRING               dim_names[MAX_DIMENSIONS];
    STRING               full_filename, full_original_filename;
    minc_output_options  used_options;

    get_volume_sizes( volume, vol_sizes );

    n_dims = get_volume_n_dimensions(volume);
    vol_dimension_names = get_volume_dimension_names( volume );

    /*--- either get the output dim name ordering from the original
          filename, the volume, or the options */

    if( options == NULL || string_length( options->dimension_names[0] ) == 0 )
    {
        if( original_filename != NULL && file_exists(original_filename) )
        {
            if( get_file_dimension_names( original_filename, n_dims,
                                          dim_names ) != OK )
                return( ERROR );
        }
        else
        {
            for_less( i, 0, n_dims )
                dim_names[i] = create_string( vol_dimension_names[i] );
        }
    }
    else
    {
        for_less( i, 0, n_dims )
            dim_names[i] = create_string( options->dimension_names[i] );
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

    delete_dimension_names( volume, vol_dimension_names );

    if( n_found != n_dims )
    {
        print_error(
                 "output_modified_volume: invalid dimension names option.\n" );

        for_less( i, 0, n_dims )
            delete_string( dim_names[i] );

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

    copy_original_file_data = FALSE;

    if( original_filename != NULL )
    {
        full_filename = expand_filename( filename );
        full_original_filename = expand_filename( original_filename );

        if( !equal_strings( full_filename, full_original_filename ) &&
            file_exists( full_original_filename ) )
        {
            copy_original_file_data = TRUE;
        }

        delete_string( full_filename );
        delete_string( full_original_filename );
    }

    if( status == OK && copy_original_file_data )
    {
        status = copy_auxiliary_data_from_minc_file( minc_file,
                                                     original_filename,
                                                     history );
    }
    else if( status == OK && history != NULL )
        status = add_minc_history( minc_file, history );

    if( status == OK )
        status = output_minc_volume( minc_file );

    if( status == OK )
        status = close_minc_output( minc_file );

    for_less( i, 0, n_dims )
        delete_string( dim_names[i] );

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : output_volume
@INPUT      : filename
              file_nc_data_type
              file_signed_flag
              file_voxel_min
              file_voxel_max
              volume
              history
              options
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sames as output_modified_volume, above, but the volume is not
              a modification of an existing MINC file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  output_volume(
    STRING                filename,
    nc_type               file_nc_data_type,
    BOOLEAN               file_signed_flag,
    Real                  file_voxel_min,
    Real                  file_voxel_max,
    Volume                volume,
    STRING                history,
    minc_output_options   *options )
{
    return( output_modified_volume( filename, file_nc_data_type,
                                    file_signed_flag,
                                    file_voxel_min, file_voxel_max,
                                    volume, NULL, history, options ) );
}
