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
static char rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Volumes/output_volume.c,v 1.19 1995-11-17 20:25:43 david Exp $";
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
    int      *n_dims,
    STRING   *dim_names[] )
{
    int                   i;
    Status                status;
    volume_input_struct   volume_input;
    Volume                tmp_volume;

    status = start_volume_input( filename, -1, File_order_dimension_names,
                                 NC_UNSPECIFIED, FALSE, 0.0, 0.0,
                                 TRUE, &tmp_volume, (minc_input_options *) NULL,
                                 &volume_input );

    if( status == OK )
    {
        *n_dims = get_volume_n_dimensions( tmp_volume );

        ALLOC( *dim_names, *n_dims );

        for_less( i, 0, *n_dims )
        {
            (*dim_names)[i] = create_string(
                           volume_input.minc_file->dim_names[i]);
        }

        delete_volume_input( &volume_input );
        delete_volume( tmp_volume );
    }

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_output_dim_names
@INPUT      : volume
              original_filename
              options
@OUTPUT     : file_sizes
@RETURNS    : array of names
@DESCRIPTION: Creates an array of dimension names for file output.  If the
              options contains a set of names, they are used.  Otherwise,
              if the original_filename is specified (non-NULL), then the
              dimension names from it are used, if they contain all the
              dimension names in the volume.  Otherwise, those from the
              volume are used.  The file_sizes[] array is set to match
              the sizes in the volume cross-referenced with the dimension
              names.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Nov. 4, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  STRING  *create_output_dim_names(
    Volume                volume,
    STRING                original_filename,
    minc_output_options   *options,
    int                   file_sizes[] )
{
    int                  n_dims, n_file_dims, dim_index;
    int                  vol_sizes[MAX_DIMENSIONS];
    int                  i, j, n_found;
    STRING               *file_dim_names;
    STRING               *vol_dimension_names;
    STRING               *dim_names;

    get_volume_sizes( volume, vol_sizes );

    n_dims = get_volume_n_dimensions(volume);
    vol_dimension_names = get_volume_dimension_names( volume );

    ALLOC( dim_names, n_dims );

    /*--- either get the output dim name ordering from the original
          filename, the volume, or the options */

    if( options != NULL && string_length( options->dimension_names[0] ) > 0 )
    {
        for_less( i, 0, n_dims )
            dim_names[i] = create_string( options->dimension_names[i] );
    }
    else
    {
        if( original_filename != NULL && file_exists(original_filename) &&
            get_file_dimension_names( original_filename, &n_file_dims,
                                      &file_dim_names ) == OK )
        {
            /*--- extract the dimension names from the file which match
                  those of the volume, so a 3D volume of z, y, x with a 4D file
                  of x y z t, will generate a list of dim_names: x y z */

            dim_index = 0;

            for_less( i, 0, n_file_dims )
            {
                for_less( j, 0, n_dims )
                {
                    if( equal_strings( vol_dimension_names[j],
                                       file_dim_names[i] ) )
                    {
                        dim_names[dim_index] = create_string(
                                                  vol_dimension_names[j] );
                        ++dim_index;
                        break;
                    }
                }

                if( dim_index == n_dims )  /*--- save time */
                    break;
            }

            /*--- check that all volume dimensions exist in the file */

            if( dim_index != n_dims )
            {
                for_less( i, 0, dim_index )
                    delete_string( dim_names[i] );

                for_less( i, 0, n_dims )
                    dim_names[i] = create_string( vol_dimension_names[i] );
            }

            /*--- free up the file dimension names */

            for_less( i, 0, n_file_dims )
                delete_string( file_dim_names[i] );

            FREE( file_dim_names );
        }
        else   /*--- no original file specified, use the volumes own list */
        {
            for_less( i, 0, n_dims )
                dim_names[i] = create_string( vol_dimension_names[i] );
        }
    }

    /*--- check that the set of dimension names are simply a permutation
          of the ones in the volume */

    n_found = 0;

    for_less( i, 0, n_dims )
    {
        for_less( j, 0, n_dims )
        {
            if( equal_strings( dim_names[j], vol_dimension_names[i] ) )
            {
                file_sizes[j] = vol_sizes[i];
                ++n_found;
            }
        }
    }

    /*--- act on whether the set of names was a permutation or not */

    if( n_found != n_dims )
    {
        print_error(
               "create_output_dim_names: dimension name mismatch.\n" );

        delete_dimension_names( volume, dim_names );

        dim_names = NULL;
    }

    /*--- no longer need the volume dimension names */

    delete_dimension_names( volume, vol_dimension_names );

    return( dim_names );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : copy_volume_auxiliary_and_history
@INPUT      : minc_file
              filename
              original_filename
              history
@OUTPUT     : 
@RETURNS    : OK or ERROR
@DESCRIPTION: If the original_filename is specified, Copies the auxiliary
              data from it.  If the history is specified, adds the history
              line.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Oct. 24, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status   copy_volume_auxiliary_and_history(
    Minc_file   minc_file,
    STRING      filename,
    STRING      original_filename,
    STRING      history )
{
    Status    status;
    BOOLEAN   copy_original_file_data;
    STRING    full_filename, full_original_filename;

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

    status = OK;

    if( copy_original_file_data )
    {
        status = copy_auxiliary_data_from_minc_file( minc_file,
                                                     original_filename,
                                                     history );
    }
    else if( history != NULL )
        status = add_minc_history( minc_file, history );

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
    Real                 real_min, real_max;
    STRING               *dim_names;
    minc_output_options  used_options;

    dim_names = create_output_dim_names( volume, original_filename,
                                         options, sizes );

    if( dim_names == NULL )
        return( ERROR );

    n_dims = get_volume_n_dimensions(volume);

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

    if( minc_file == (Minc_file) NULL )
        return( ERROR );

    status = copy_volume_auxiliary_and_history( minc_file, filename,
                                                original_filename, history );

    if( status == OK )
        status = output_minc_volume( minc_file );

    if( status == OK )
        status = close_minc_output( minc_file );

    delete_dimension_names( volume, dim_names );

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
