#include  <def_mni.h>

#ifndef  NO_MNC_FILES
#include  <minc.h>
#define   MNC_ENDING   ".mnc"
#endif

#define   FREE_ENDING   ".fre"

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
              description of Volum and volume_input_struct.
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  start_volume_input(
    char                 filename[],
    Boolean              convert_to_byte_flag,
    Volume               *volume,
    volume_input_struct  *input_info )
{
    Status          status;
    nc_type         data_type;
    static String   dim_names[N_DIMENSIONS] = { MIxspace, MIyspace, MIzspace };
    String          expanded_filename;

    status = OK;

    if( convert_to_byte_flag )
        data_type = NC_BYTE;
    else
        data_type = NC_UNSPECIFIED;

    *volume = create_volume( 3, dim_names, data_type, FALSE,
                             0.0, 0.0 );

    expand_filename( filename, expanded_filename );

#ifndef  NO_MNC_FILES
    if( !string_ends_in( expanded_filename, FREE_ENDING ) )
        input_info->file_format = MNC_FORMAT;
    else
#endif
        input_info->file_format = FREE_FORMAT;

    switch( input_info->file_format )
    {
#ifndef  NO_MNC_FILES
    case  MNC_FORMAT:
        input_info->minc_file = initialize_minc_input( expanded_filename,
                                                       *volume );
        if( input_info->minc_file == (Minc_file) NULL )
            status = ERROR;
        break;
#endif

    case  FREE_FORMAT:
        status = initialize_free_format_input( expanded_filename,
                                               *volume, input_info );
        break;
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
    switch( input_info->file_format )
    {
#ifndef  NO_MNC_FILES
    case  MNC_FORMAT:
        (void) close_minc_input( input_info->minc_file );
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
    Volume                volume,
    volume_input_struct   *input_info,
    Real                  *fraction_done )
{
    Boolean       more_to_do;

    switch( input_info->file_format )
    {
#ifndef  NO_MNC_FILES
    case  MNC_FORMAT:
        more_to_do = input_more_minc_file( input_info->minc_file,
                                           fraction_done );
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
    Volume                volume,
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
    Volume         *volume )
{
    Status               status;
    Real                 amount_done;
    volume_input_struct  input_info;
    progress_struct      progress;
    static const int     FACTOR = 1000;

    status = start_volume_input( filename, FALSE, volume, &input_info );

    if( status == OK )
    {
        initialize_progress_report( &progress, FALSE, FACTOR, "Reading Volume");

        while( input_more_of_volume( *volume, &input_info, &amount_done ) )
        {
            update_progress_report( &progress,
                                    ROUND( (Real) FACTOR * amount_done));
        }

        terminate_progress_report( &progress );
    }

    return( status );
}
