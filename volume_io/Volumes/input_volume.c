#include  <volume_io.h>

#ifndef  NO_MNC_FILES
#include  <minc.h>
#define   MNC_ENDING   "mnc"
#endif

#define   FREE_ENDING   "fre"
#define   MNI_ENDING    "mni"

private  void  setup_input_mni_as_free_format(
    char   filename[] );
private  void  get_mni_scaling(
    char   filename[],
    Real   *scale,
    Real   *translation );

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
    volume_input_struct  *input_info )
{
    Status          status;
    int             d;
    Real            mni_scale, mni_translation;
    Real            min_voxel, max_voxel, real_min, real_max;
    BOOLEAN         mni_format;
    STRING          expanded_filename;

    if( dim_names == (char **) NULL )
        dim_names = get_default_dim_names( n_dimensions );

    status = OK;

    if( create_volume_flag || *volume == (Volume) NULL )
    {
        *volume = create_volume( n_dimensions, dim_names, volume_nc_data_type,
                                 volume_signed_flag,
                                 volume_voxel_min, volume_voxel_max );
    }
    else if( n_dimensions != get_volume_n_dimensions( *volume ) &&
             (*volume)->data != (void *) NULL )
        free_volume_data( *volume );

    expand_filename( filename, expanded_filename );
    mni_format = FALSE;

#ifndef  NO_MNC_FILES
    if( filename_extension_matches( expanded_filename, MNI_ENDING ) )
    {
        get_mni_scaling( expanded_filename, &mni_scale, &mni_translation );
        setup_input_mni_as_free_format( expanded_filename );
        set_volume_voxel_range( *volume, 0.0, 255.0 );
        input_info->file_format = FREE_FORMAT;
        mni_format = TRUE;
    }
    else if( !filename_extension_matches( expanded_filename, FREE_ENDING ) )
        input_info->file_format = MNC_FORMAT;
    else
#endif
        input_info->file_format = FREE_FORMAT;

    switch( input_info->file_format )
    {
#ifndef  NO_MNC_FILES
    case  MNC_FORMAT:
        input_info->minc_file = initialize_minc_input( expanded_filename,
                                                       *volume, options );
        if( input_info->minc_file == (Minc_file) NULL )
            status = ERROR;

        for_less( d, 0, MAX_DIMENSIONS )
            input_info->axis_index_from_file[d] = d;
        break;
#endif

    case  FREE_FORMAT:
        status = initialize_free_format_input( expanded_filename,
                                               *volume, input_info );
        break;
    }

    if( mni_format )
    {
        get_volume_voxel_range( *volume, &min_voxel, &max_voxel );
        real_min = mni_scale * (min_voxel - mni_translation);
        real_max = mni_scale * (max_voxel - mni_translation);
        set_volume_real_range( *volume, real_min, real_max );
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

public  BOOLEAN  input_more_of_volume(
    Volume                volume,
    volume_input_struct   *input_info,
    Real                  *fraction_done )
{
    BOOLEAN       more_to_do;

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
              dim_names
              convert_to_byte_flag
@OUTPUT     : volume
@RETURNS    : OK if loaded alright
@DESCRIPTION: Inputs the entire volume.
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

#define  MNI_NX            256
#define  MNI_NY            256
#define  MNI_NZ            80
#define  MNI_BYTE_OFFSET   1536

private  void  setup_input_mni_as_free_format(
    char   filename[] )
{
    Status  status;
    FILE    *file;
    STRING  tmp_name;
    STRING  abs_filename;
    char    *getwd();

    if( filename[0] != '/' )
    {
        (void) getwd( abs_filename );
        (void) strcat( abs_filename, "/" );
        (void) strcat( abs_filename, filename );
    }
    else
        (void) strcpy( abs_filename, filename );

    (void) tmpnam( tmp_name );

    status = open_file( tmp_name, WRITE_FILE, ASCII_FORMAT, &file );

    status = output_string( file, "1\n" );
    status = output_string( file, "-86.095 -126.510 -37.5\n" );
    status = output_string( file, "80   1.5    z\n" );
    status = output_string( file, "256  0.86   y\n" );
    status = output_string( file, "256  0.67   x\n" );
    status = output_string( file, abs_filename );
    status = output_int( file, MNI_BYTE_OFFSET );
    status = output_string( file, "\n" );

    status = close_file( file );

    if( status ) {}

    (void) strcpy( filename, tmp_name );
}

private  Status  input_vax_real(
    FILE     *file,
    float    *real )
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
    float   float_scale_factor;
    short   short_trans_factor;
    FILE    *file;

    status = open_file( filename, READ_FILE, BINARY_FORMAT, &file );

    status = set_file_position( file, (long) MNI_BYTE_OFFSET );

    if( status == OK )
        status = input_vax_real( file, &float_scale_factor );

    if( status == OK )
    {
        status = set_file_position( file, (long) MNI_BYTE_OFFSET + MNI_NX + 4 );
    }

    if( status == OK )
        status = input_vax_short( file, &short_trans_factor );

    if( status == OK )
    {
        *scale_factor = (Real) float_scale_factor;
        *trans_factor = (Real) short_trans_factor;
    }

    status = close_file( file );
}
