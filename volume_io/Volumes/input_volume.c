#include  <def_mni.h>

#ifndef  NO_MNC_FILES
#include  <minc.h>
#define   MNC_ENDING   ".mnc"
#endif

#define   FREE_ENDING   ".fre"
#define   MNI_ENDING    ".mni"

private  void  setup_input_mni_as_free_format(
    char   filename[] );
private  void  get_mni_scaling(
    char   filename[],
    Real   *scale,
    Real   *translation );

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
    int             len;
    Real            mni_scale, mni_translation;
    Boolean         mni_format;
    nc_type         data_type;
    static String   dim_names[N_DIMENSIONS] = { MIxspace, MIyspace, MIzspace };
    String          expanded_filename, filename_no_z;

    (void) strcpy( filename_no_z, filename );
    len = strlen( filename );

    if( filename[len-2] == '.' && filename[len-1] == 'Z' )
        filename_no_z[len-2] = (char) 0;

    status = OK;

    if( convert_to_byte_flag )
        data_type = NC_BYTE;
    else
        data_type = NC_UNSPECIFIED;

    *volume = create_volume( 3, dim_names, data_type, FALSE,
                             0.0, 0.0 );

    expand_filename( filename_no_z, expanded_filename );
    mni_format = FALSE;

#ifndef  NO_MNC_FILES
    if( string_ends_in( expanded_filename, MNI_ENDING ) )
    {
        get_mni_scaling( expanded_filename, &mni_scale, &mni_translation );
        setup_input_mni_as_free_format( expanded_filename );
        input_info->file_format = FREE_FORMAT;
        mni_format = TRUE;
    }
    else if( !string_ends_in( expanded_filename, FREE_ENDING ) )
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

    if( mni_format )
    {
        (*volume)->value_scale = mni_scale;
        (*volume)->value_translation = - mni_scale * mni_translation;
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

#define  MNI_NX            256
#define  MNI_NY            256
#define  MNI_NZ            80
#define  MNI_BYTE_OFFSET   1536

private  void  setup_input_mni_as_free_format(
    char   filename[] )
{
    Status  status;
    FILE    *file;
    String  tmp_name;

    (void) tmpnam( tmp_name );

    status = open_file( tmp_name, WRITE_FILE, ASCII_FORMAT, &file );

    status = output_string( file, "1\n" );
    status = output_string( file, "-86.095 -110.510 -37.5\n" );
    status = output_string( file, "80   1.5    z\n" );
    status = output_string( file, "256  0.86   y\n" );
    status = output_string( file, "256  0.67   x\n" );
    status = output_string( file, filename );
    status = output_int( file, MNI_BYTE_OFFSET );
    status = output_string( file, "\n" );

    status = close_file( file );

    if( status ) {}

    (void) strcpy( filename, tmp_name );
}

private  Status  input_vax_real(
    FILE    *file,
    Real    *real )
{
    char    *byte_ptr, tmp;
    Status  status;

    byte_ptr = (char *) (void *) real;

    status = io_binary_data( file, READ_FILE, (void *) byte_ptr,
                             sizeof(char), 4 );

    if( status == OK )
    {
        tmp = byte_ptr[0];
        byte_ptr[0] = byte_ptr[1];
        byte_ptr[1] = tmp;
        tmp = byte_ptr[2];
        byte_ptr[2] = byte_ptr[3];
        byte_ptr[3] = tmp;
        *real /= 4.0;
    }

    return( status );
}

private  Status  input_vax_short(
    FILE      *file,
    short     *s )
{
    char    *byte_ptr, tmp;
    Status  status;

    byte_ptr = (char *) (void *) s;

    status = io_binary_data( file, READ_FILE, (void *) byte_ptr,
                             sizeof(char), 2 );

    if( status == OK )
    {
        tmp = byte_ptr[0];
        byte_ptr[0] = byte_ptr[1];
        byte_ptr[1] = tmp;
    }

    return( status );
}

private  void  get_mni_scaling(
    char   filename[],
    Real   *scale_factor,
    Real   *trans_factor )
{
    Status  status;
    short   short_trans_factor;
    FILE    *file;

    status = open_file( filename, READ_FILE, BINARY_FORMAT, &file );

    status = set_file_position( file, (long) MNI_BYTE_OFFSET );

    if( status == OK )
        status = input_vax_real( file, scale_factor );

    if( status == OK )
    {
        status = set_file_position( file, (long) MNI_BYTE_OFFSET + MNI_NX + 4 );
    }

    if( status == OK )
        status = input_vax_short( file, &short_trans_factor );

    if( status == OK )
        *trans_factor = (Real) short_trans_factor;

    status = close_file( file );
}
