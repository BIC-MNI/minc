#include  <def_mni.h>
#include  <minc.h>

#define  DEFAULT_SUFFIX  "fre"

#define  NUM_VALUES      256

public  Status  initialize_free_format_input(
    volume_struct        *volume,
    volume_input_struct  *volume_input )
{
    Status         status, file_status;
    String         volume_filename, abs_volume_filename, directory;
    int            n_bytes_per_voxel, min_value, max_value, i, n_voxels;
    unsigned short value;
    char           ch;
    Real           thickness[N_DIMENSIONS];
    Point          origin;
    Real           trans[N_DIMENSIONS], scale_axis[N_DIMENSIONS];
    FILE           *file;
    Boolean        flip_axis[N_DIMENSIONS], axis_valid;
    int            axis;

    status = OK;

    file_status = open_file_with_default_suffix(
                         volume->filename, DEFAULT_SUFFIX,
                         READ_FILE, ASCII_FORMAT, &file );

    status = file_status;

    if( status == OK )
        status = input_string( file, volume_filename, MAX_STRING_LENGTH, ' ' );

    if( status == OK )
        status = input_int( file, &n_bytes_per_voxel );

    if( status == OK )
    {
        if( n_bytes_per_voxel == 1 )
            volume_input->file_data_type = UNSIGNED_BYTE;
        else if( n_bytes_per_voxel == 2 )
            volume_input->file_data_type = UNSIGNED_SHORT;
        else
        {
            print( "Must be either 1 or 2 bytes per voxel.\n" );
            status = ERROR;
        }

        if( volume_input->convert_to_byte )
            volume->data_type = UNSIGNED_BYTE;
        else
            volume->data_type = volume_input->file_data_type;
    }

    if( status == OK )
    for_less( axis, 0, N_DIMENSIONS )
    {
        status = ERROR;

        if( input_int( file, &volume_input->sizes_in_file[axis] ) != OK )
            break;

        if( input_real( file, &thickness[axis] ) != OK )
            break;

        if( input_nonwhite_character( file, &ch ) != OK )
            break;

        flip_axis[axis] = (ch == '-');

        if( ch == '-' || ch == '+' )
            if( input_nonwhite_character( file, &ch ) != OK )
                break;

        axis_valid = TRUE;

        switch( ch )
        {
        case 'x':
        case 'X':  volume->axis_index_from_file[axis] = X;
                   break;

        case 'y':
        case 'Y':  volume->axis_index_from_file[axis] = Y;
                   break;

        case 'z':
        case 'Z':  volume->axis_index_from_file[axis] = Z;
                   break;

        default:   axis_valid = FALSE;    break;
        }

        if( !axis_valid )
        {
            print( "Invalid axis.\n" );
            break;
        }

        status = OK;
    }

    if( status == OK &&
        volume->axis_index_from_file[X] == volume->axis_index_from_file[Y] ||
        volume->axis_index_from_file[X] == volume->axis_index_from_file[Z] ||
        volume->axis_index_from_file[Y] == volume->axis_index_from_file[Z] )
    {
        print( "Two axis indices are equal.\n" );
        status = ERROR;
    }

    if( status == OK )
    {
        for_less( axis, 0, N_DIMENSIONS )
        {
            volume->sizes[volume->axis_index_from_file[axis]] =
                                 volume_input->sizes_in_file[axis];
            volume->thickness[volume->axis_index_from_file[axis]] =
                                                     thickness[axis];
            volume->flip_axis[volume->axis_index_from_file[axis]] =
                                                     flip_axis[axis];
        }

        for_less( axis, 0, N_DIMENSIONS )
        {
            if( volume->flip_axis[axis] )
            {
                scale_axis[axis] = -volume->thickness[axis];
                trans[axis] = volume->thickness[axis] *
                                  (Real) volume->sizes[axis];
            }
            else
            {
                scale_axis[axis] = volume->thickness[axis];
                trans[axis] = 0.0;
            }
        }

        make_scale_transform( scale_axis[X],
                              scale_axis[Y],
                              scale_axis[Z],
                              &volume->voxel_to_world_transform );
        fill_Point( origin, trans[X], trans[Y], trans[Z] );
        set_transform_origin( &volume->voxel_to_world_transform, &origin );

        extract_directory( volume->filename, directory );
        get_absolute_filename( volume_filename, directory,
                               abs_volume_filename );
        status = open_file( abs_volume_filename, READ_FILE, BINARY_FORMAT,
                            &volume_input->volume_file );
    }

    if( status == OK && volume_input->convert_to_byte &&
        volume_input->file_data_type != UNSIGNED_BYTE )
    {
        n_voxels = volume->sizes[X] * volume->sizes[Y] * volume->sizes[Z];

        min_value = 0;
        max_value = 0;

        for_less( i, 0, n_voxels )
        {
            if( io_binary_data( volume_input->volume_file,
                                READ_FILE, (void *) &value,
                                sizeof(value), 1 ) != OK )
            {
                print( "End of file in volume file.\n" );
                status = ERROR;
                break;
            }

            if( i == 0 || value < min_value )
                min_value = value;
            if( i == 0 || value > max_value )
                max_value = value;
        }

        if( min_value == max_value )
        {
            volume->value_scale = 1.0;
            volume->value_translation = (Real) min_value;
        }
        else
        {
            volume->value_scale = (Real) (NUM_VALUES - 1) /
                                  (Real) (max_value - min_value);
            volume->value_translation = (Real) min_value;
        }

        if( status == OK )
            status = close_file( volume_input->volume_file );

        if( status == OK )
            status = open_file( abs_volume_filename, READ_FILE, BINARY_FORMAT,
                                &volume_input->volume_file );
    }
    else
    {
        volume->value_scale = 1.0;
        volume->value_translation = 0.0;
    }

    if( status == OK )
    if( volume_input->file_data_type == UNSIGNED_BYTE )
    {
        ALLOC( volume_input->byte_slice_buffer,
               volume_input->sizes_in_file[1]*volume_input->sizes_in_file[2] );
    }
    else
    {
        ALLOC( volume_input->short_slice_buffer,
               volume_input->sizes_in_file[1]*volume_input->sizes_in_file[2] );
    }

    volume_input->slice_index = 0;

    return( status );
}

public  void  delete_free_format_input(
    volume_input_struct   *volume_input )
{
    if( volume_input->file_data_type == UNSIGNED_BYTE )
    {
        FREE( volume_input->byte_slice_buffer );
    }
    else
    {
        FREE( volume_input->short_slice_buffer );
    }

    (void) close_file( volume_input->volume_file );
}

public  Boolean  input_more_free_format_file(
    volume_struct         *volume,
    volume_input_struct   *volume_input,
    Real                  *fraction_done )
{
    Status          status;
    Boolean         more_to_do;
    int             i, j, indices[N_DIMENSIONS];
    unsigned char   *byte_buffer_ptr;
    unsigned short  *short_buffer_ptr;

    if( volume_input->slice_index < volume_input->sizes_in_file[0] )
    {
        if( volume_input->file_data_type == UNSIGNED_BYTE )
        {
            status = io_binary_data( volume_input->volume_file, READ_FILE,
                                 (void *) volume_input->byte_slice_buffer,
                                 sizeof(volume_input->byte_slice_buffer[0]),
                                 volume_input->sizes_in_file[1] *
                                 volume_input->sizes_in_file[2] );

            if( status == OK )
            {
                indices[volume->axis_index_from_file[0]] =
                                  volume_input->slice_index;

                byte_buffer_ptr = volume_input->byte_slice_buffer;
                for_less( i, 0, volume_input->sizes_in_file[1] )
                {
                    indices[volume->axis_index_from_file[1]] = i;
                    for_less( j, 0, volume_input->sizes_in_file[2] )
                    {
                        indices[volume->axis_index_from_file[2]] = j;
                        ASSIGN_VOLUME_DATA( *volume,
                             indices[X], indices[Y], indices[Z],
                             ((Real) (*byte_buffer_ptr) -
                             volume->value_translation) / volume->value_scale );

                        ++byte_buffer_ptr;
                    }
                }
            }
        }
        else
        {
            status = io_binary_data( volume_input->volume_file, READ_FILE,
                                 (void *) volume_input->short_slice_buffer,
                                 sizeof(volume_input->short_slice_buffer[0]),
                                 volume_input->sizes_in_file[1] *
                                 volume_input->sizes_in_file[2] );

            if( status == OK )
            {
                indices[volume->axis_index_from_file[0]] =
                                  volume_input->slice_index;

                short_buffer_ptr = volume_input->short_slice_buffer;
                for_less( i, 0, volume_input->sizes_in_file[1] )
                {
                    indices[volume->axis_index_from_file[1]] = i;
                    for_less( j, 0, volume_input->sizes_in_file[2] )
                    {
                        indices[volume->axis_index_from_file[2]] = j;
                        ASSIGN_VOLUME_DATA( *volume,
                             indices[X], indices[Y], indices[Z],
                             ((Real) (*short_buffer_ptr) -
                             volume->value_translation) / volume->value_scale );

                        ++short_buffer_ptr;
                    }
                }
            }
        }

        ++volume_input->slice_index;
    }

    *fraction_done = (Real) volume_input->slice_index /
                     (Real) volume->sizes[volume->axis_index_from_file[0]];

    more_to_do = TRUE;

    if( volume_input->slice_index ==
        volume->sizes[volume->axis_index_from_file[0]] )
    {
        more_to_do = FALSE;
        delete_volume_input( volume_input );
    }

    return( more_to_do );
}
