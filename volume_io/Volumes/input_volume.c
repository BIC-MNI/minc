#include  <def_mni.h>
#include  <minc.h>

#define  N_VALUES  256

#define   MNC_ENDING   ".mnc"

public  Status  start_volume_input(
    char                 filename[],
    volume_struct        *volume,
    volume_input_struct  *input )
{
    Status       status;
    String       expanded_filename;

    status = OK;

    expand_filename( filename, expanded_filename );

    if( string_ends_in( expanded_filename, MNC_ENDING ) )
        input->file_type = MNC_FORMAT;
    else
        input->file_type = FREE_FORMAT;

    (void) strcpy( volume->filename, expanded_filename );

    switch( input->file_type )
    {
    case  MNC_FORMAT:
        status = initialize_mnc_input( volume, input );
        break;

    case  FREE_FORMAT:
        status = initialize_free_format_input( volume, input );
        break;
    }

    compute_transform_inverse( &volume->voxel_to_world_transform,
                               &volume->world_to_voxel_transform );

    ALLOC3D( volume->data, volume->sizes[X], volume->sizes[Y],
             volume->sizes[Z] );

    volume->value_scale = 1.0;
    volume->value_translation = 0.0;

    input->slice_index = 0;

    return( status );
}

public  void  delete_volume_input(
    volume_input_struct   *input )
{
    switch( input->file_type )
    {
    case  MNC_FORMAT:
        delete_mnc_input( input );
        break;

    case  FREE_FORMAT:
        delete_free_format_input( input );
        break;
    }
}

public  Boolean  input_more_of_volume(
    volume_struct         *volume,
    volume_input_struct   *input,
    Real                  *fraction_done )
{
    Boolean       more_to_do;

    switch( input->file_type )
    {
    case  MNC_FORMAT:
        more_to_do = input_more_mnc_file( volume, input, fraction_done );
        break;

    case  FREE_FORMAT:
        more_to_do = input_more_free_format_file( volume, input,
                                                  fraction_done );
        break;
    }

    return( more_to_do );
}

public  void  cancel_volume_input(
    volume_struct         *volume,
    volume_input_struct   *input )
{
    delete_volume( volume );

    delete_volume_input( input );
}

public  Status  input_volume(
    char           filename[],
    volume_struct  *volume )
{
    Status               status;
    Real                 amount_done;
    volume_input_struct  volume_input;

    status = start_volume_input( filename, volume, &volume_input );

    while( input_more_of_volume( volume, &volume_input, &amount_done ) )
    {
    }

    return( status );
}

public  void  delete_volume(
    volume_struct  *volume )
{
    FREE3D( volume->data );
}
