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
static char rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Volumes/input_volume.c,v 1.42 2004-10-04 20:23:52 bert Exp $";
#endif

#include  <minc.h>

#define   FREE_ENDING   "fre"

/* ----------------------------- MNI Header -----------------------------------
@NAME       : start_volume_input
@INPUT      : filename               - file to input
              dim_names              - names of dimensions, or null
              convert_to_byte_flag   - whether to convert volume data to byte
@OUTPUT     : volume                 - the volume data
              input_info             - information for use while inputting
@RETURNS    : OK if successful
@DESCRIPTION: Opens the file and reads the header, but does not read any
              volume data yet.  Allocates the data also.

              Note: if you wish to modify the volume file input routines,
              then look at the new_C_dev/Include/volume.h for the
              description of Volum and volume_input_struct.
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

VIOAPI  Status  start_volume_input(
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
    volume_input_struct  *input_info )
{
    Status          status;
    int             d;
    STRING          expanded_filename;

    status = OK;

    if( create_volume_flag || *volume == (Volume) NULL )
    {
        if( n_dimensions < 1 || n_dimensions > MAX_DIMENSIONS )
            n_dimensions = get_minc_file_n_dimensions( filename );

        if( n_dimensions < 1 )
            return( ERROR );

        if( dim_names == (STRING *) NULL )
           dim_names = get_default_dim_names( n_dimensions );

        *volume = create_volume( n_dimensions, dim_names, volume_nc_data_type,
                                 volume_signed_flag,
                                 volume_voxel_min, volume_voxel_max );
    }
    else if( n_dimensions != get_volume_n_dimensions( *volume ) &&
             volume_is_alloced( *volume ) )
        free_volume_data( *volume );

    expanded_filename = expand_filename( filename );

    if( !filename_extension_matches( expanded_filename, FREE_ENDING ) )
        input_info->file_format = MNC_FORMAT;
    else
        input_info->file_format = FREE_FORMAT;

    switch( input_info->file_format )
    {
    case  MNC_FORMAT:
        if( !file_exists( expanded_filename ) )
        {
            (void) file_exists_as_compressed( expanded_filename,
                                              &expanded_filename );
        }

        input_info->minc_file = initialize_minc_input( expanded_filename,
                                                       *volume, options );
        if( input_info->minc_file == (Minc_file) NULL )
            status = ERROR;
        else
        {
            for_less( d, 0, MAX_DIMENSIONS )
                input_info->axis_index_from_file[d] = d;
        }

        break;

    case  FREE_FORMAT:
        status = initialize_free_format_input( expanded_filename,
                                               *volume, input_info );
        break;
    }

    if( status != OK && create_volume_flag )
        delete_volume( *volume );

    delete_string( expanded_filename );

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : delete_volume_input
@INPUT      : input_info
@OUTPUT     :
@RETURNS    :
@DESCRIPTION: Frees up any memory allocated for the volume input, i.e., any
              temporary_buffer.
@CREATED    :                      David MacDonald
@MODIFIED   :
---------------------------------------------------------------------------- */

VIOAPI  void  delete_volume_input(
    volume_input_struct   *input_info )
{
    switch( input_info->file_format )
    {
    case  MNC_FORMAT:
        (void) close_minc_input( input_info->minc_file );
        break;

    case  FREE_FORMAT:
        delete_free_format_input( input_info );
        break;
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : input_more_of_volume
@INPUT      : volume
              input_info
@OUTPUT     : fraction_done    - number between 0 and 1
@RETURNS    : TRUE - if there is remains more to input after this call
@DESCRIPTION: Reads in more of the volume file.  This routine is provided,
              rather than a read_entire_volume(), so that programs can
              multiprocess loading with other tasks.
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

VIOAPI  BOOLEAN  input_more_of_volume(
    Volume                volume,
    volume_input_struct   *input_info,
    Real                  *fraction_done )
{
    BOOLEAN       more_to_do;

    switch( input_info->file_format )
    {
    case  MNC_FORMAT:
        more_to_do = input_more_minc_file( input_info->minc_file,
                                           fraction_done );
        break;

    case  FREE_FORMAT:
        more_to_do = input_more_free_format_file( volume, input_info,
                                                  fraction_done );
        break;
    }

    return( more_to_do );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : cancel_volume_input
@INPUT      : volume
              input_info
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Cancels loading the volume.  Merely deletes the volume, then
              deletes the input buffer.
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

VIOAPI  void  cancel_volume_input(
    Volume                volume,
    volume_input_struct   *input_info )
{
    delete_volume( volume );

    delete_volume_input( input_info );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : input_volume
@INPUT      : filename
              dim_names
              convert_to_byte_flag
@OUTPUT     : volume
@RETURNS    : OK if loaded alright
@DESCRIPTION: Inputs the entire volume.
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

VIOAPI  Status  input_volume(
    STRING               filename,
    int                  n_dimensions,
    STRING               dim_names[],
    nc_type              volume_nc_data_type,
    BOOLEAN              volume_signed_flag,
    Real                 volume_voxel_min,
    Real                 volume_voxel_max,
    BOOLEAN              create_volume_flag,
    Volume               *volume,
    minc_input_options   *options )
{
    Status               status;
    Real                 amount_done;
    volume_input_struct  input_info;
    progress_struct      progress;
    static const int     FACTOR = 1000;

    status = start_volume_input( filename, n_dimensions, dim_names,
                                 volume_nc_data_type, volume_signed_flag,
                                 volume_voxel_min, volume_voxel_max,
                                 create_volume_flag, volume, options,
                                 &input_info );

    if( status == OK )
    {
        initialize_progress_report( &progress, FALSE, FACTOR, "Reading Volume");

        while( input_more_of_volume( *volume, &input_info, &amount_done ) )
        {
            update_progress_report( &progress,
                                    ROUND( (Real) FACTOR * amount_done));
        }

        terminate_progress_report( &progress );

        delete_volume_input( &input_info );
    }

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_volume_input_minc_file
@INPUT      : volume_input
@OUTPUT     : 
@RETURNS    : Minc_file
@DESCRIPTION: Returns the minc file attached to a particular volume
              input structure.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Jun 15, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

VIOAPI  Minc_file   get_volume_input_minc_file(
    volume_input_struct   *volume_input )
{
    return( volume_input->minc_file );
}
