#include  <def_mni.h>

#ifndef  NO_MNC_FILES
#include  <minc.h>
#define   MNC_ENDING   ".mnc"
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : start_volume_input
@INPUT      : filename               - file to input
              convert_to_byte_flag   - whether to convert volume data to byte
@OUTPUT     : volume                 - the volume data
              input_info             - information for use while inputting
@RETURNS    : OK if successful
@DESCRIPTION: Opens the file and reads the header, but does not read any
              volume data yet.  Allocates the data also.

              Note: if you wish to modify the volume file input routines,
              then look at the new_C_dev/Include/def_volume.h for the
              description of volume_struct and volume_input_struct.
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  start_volume_input(
    char                 filename[],
    Boolean              convert_to_byte_flag,
    volume_struct        *volume,
    volume_input_struct  *input_info )
{
    Status       status;
    String       expanded_filename;

    status = OK;

    expand_filename( filename, expanded_filename );

#ifndef  NO_MNC_FILES
    if( string_ends_in( expanded_filename, MNC_ENDING ) )
        input_info->file_type = MNC_FORMAT;
    else
#endif
        input_info->file_type = FREE_FORMAT;

    (void) strcpy( volume->filename, expanded_filename );

    switch( input_info->file_type )
    {
#ifndef  NO_MNC_FILES
    case  MNC_FORMAT:
        status = initialize_mnc_input( volume, input_info );
        break;
#endif

    case  FREE_FORMAT:
        input_info->convert_to_byte = convert_to_byte_flag;
        status = initialize_free_format_input( volume, input_info );
        break;
    }

    if( status == OK )
    {
        compute_transform_inverse( &volume->voxel_to_world_transform,
                                   &volume->world_to_voxel_transform );

        alloc_volume( volume );

        volume->value_scale = 1.0;
        volume->value_translation = 0.0;

        input_info->slice_index = 0;
    }

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

public  void  delete_volume_input(
    volume_input_struct   *input_info )
{
    switch( input_info->file_type )
    {
#ifndef  NO_MNC_FILES
    case  MNC_FORMAT:
        delete_mnc_input( input_info );
        break;
#endif

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

public  Boolean  input_more_of_volume(
    volume_struct         *volume,
    volume_input_struct   *input_info,
    Real                  *fraction_done )
{
    Boolean       more_to_do;

    switch( input_info->file_type )
    {
#ifndef  NO_MNC_FILES
    case  MNC_FORMAT:
        more_to_do = input_more_mnc_file( volume, input_info, fraction_done );
        break;
#endif

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

public  void  cancel_volume_input(
    volume_struct         *volume,
    volume_input_struct   *input_info )
{
    delete_volume( volume );

    delete_volume_input( input_info );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : input_volume
@INPUT      : filename
@OUTPUT     : volume
@RETURNS    : OK if loaded alright
@DESCRIPTION: Inputs the entire volume.
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  input_volume(
    char           filename[],
    volume_struct  *volume )
{
    Status               status;
    Real                 amount_done;
    volume_input_struct  input_info;

    status = start_volume_input( filename, FALSE, volume, &input_info );

    while( input_more_of_volume( volume, &input_info, &amount_done ) )
    {
    }

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : alloc_volume
@INPUT      : volume
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Assumes that the volume sizes and the data type have been
              assigned, and allocates the volume data.
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  alloc_volume(
    volume_struct  *volume )
{
    if( volume->sizes[X] > 0 && volume->sizes[Y] > 0 && volume->sizes[Z] > 0 )
    {
        switch( volume->data_type )
        {
        case UNSIGNED_BYTE:
            ALLOC3D( volume->byte_data, volume->sizes[X], volume->sizes[Y],
                     volume->sizes[Z] );
            break;

        case UNSIGNED_SHORT:
            ALLOC3D( volume->short_data, volume->sizes[X], volume->sizes[Y],
                     volume->sizes[Z] );
            break;

        default:
            HANDLE_INTERNAL_ERROR( "alloc_volume" );
        }
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : delete_volume
@INPUT      : volume
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Frees the memory associated with the volume.
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  delete_volume(
    volume_struct  *volume )
{
    if( volume->sizes[X] > 0 && volume->sizes[Y] > 0 && volume->sizes[Z] > 0 )
    {
        switch( volume->data_type )
        {
        case UNSIGNED_BYTE:
            FREE3D( volume->byte_data );
            break;

        case UNSIGNED_SHORT:
            FREE3D( volume->short_data );
            break;

        default:
            HANDLE_INTERNAL_ERROR( "alloc_volume" );
        }
    }
}
