#include  <def_mni.h>
#include  <minc.h>

#define  DEFAULT_SUFFIX  ".fre"

#define  NUM_VALUES      256

public  Status  initialize_free_format_input(
    volume_struct        *volume,
    volume_input_struct  *volume_input )
{
    Status         status, file_status;
    String         volume_filename;
    short          value;
    int            min_value, max_value;
    char           ch;
    Real           thickness[N_DIMENSIONS];
    FILE           *file;
    Boolean        flip_axis[N_DIMENSIONS], axis_valid;
    int            axis, i, n_voxels, file_axis_index[N_DIMENSIONS];

    status = OK;

    file_status = open_file_with_default_suffix(
                         volume->filename, DEFAULT_SUFFIX,
                         READ_FILE, ASCII_FORMAT, &file );

    status = file_status;

    if( status == OK )
        status = input_string( file, volume_filename, MAX_STRING_LENGTH, ' ' );


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
        case 'X':  file_axis_index[X] = axis;
                   volume_input->axis_index[axis] = X;
                   break;

        case 'y':
        case 'Y':  file_axis_index[Y] = axis;
                   volume_input->axis_index[axis] = Y;
                   break;

        case 'z':
        case 'Z':  file_axis_index[Z] = axis;
                   volume_input->axis_index[axis] = Z;
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

    if( volume_input->axis_index[X] == volume_input->axis_index[Y] ||
        volume_input->axis_index[X] == volume_input->axis_index[Z] ||
        volume_input->axis_index[Y] == volume_input->axis_index[Z] )
    {
        print( "Two axis indices are equal.\n" );
        status = ERROR;
    }

    if( status == OK )
    {
        for_less( axis, 0, N_DIMENSIONS )
        {
            volume->sizes[volume_input->axis_index[axis]] =
                                 volume_input->sizes_in_file[axis];
            volume->thickness[volume_input->axis_index[axis]] = thickness[axis];
            volume->flip_axis[volume_input->axis_index[axis]] = flip_axis[axis];
        }

        make_scale_transform( volume->thickness[X],
                              volume->thickness[Y],
                              volume->thickness[Z],
                              &volume->voxel_to_world_transform );

        status = open_file( volume_filename, READ_FILE, BINARY_FORMAT,
                            &volume_input->volume_file );
    }

    if( status == OK )
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

        status = close_file( volume_input->volume_file );
    }

    if( status == OK )
    {
        status = open_file( volume_filename, READ_FILE, BINARY_FORMAT,
                            &volume_input->volume_file );
    }

    ALLOC( volume_input->short_slice_buffer, volume_input->sizes_in_file[1] *
                                             volume_input->sizes_in_file[2] );
    volume_input->slice_index = 0;

    return( status );
}

public  void  delete_free_format_input(
    volume_input_struct   *volume_input )
{
    FREE( volume_input->short_slice_buffer );

    (void) close_file( volume_input->volume_file );
}

public  Boolean  input_more_free_format_file(
    volume_struct         *volume,
    volume_input_struct   *volume_input,
    Real                  *fraction_done )
{
    Status        status;
    Boolean       more_to_do;
    int           i, j, indices[N_DIMENSIONS];
    short         *buffer_ptr;

    if( volume_input->slice_index < volume_input->sizes_in_file[0] )
    {
        status = io_binary_data( volume_input->volume_file, READ_FILE,
                                 (void *) volume_input->short_slice_buffer,
                                 sizeof(volume_input->short_slice_buffer[0]),
                                 volume_input->sizes_in_file[1] *
                                 volume_input->sizes_in_file[2] );

        if( status == OK )
        {
            indices[volume_input->axis_index[0]] = volume_input->slice_index;

            buffer_ptr = volume_input->short_slice_buffer;
            for_less( i, 0, volume_input->sizes_in_file[1] )
            {
                indices[volume_input->axis_index[1]] = i;
                for_less( j, 0, volume_input->sizes_in_file[2] )
                {
                    indices[volume_input->axis_index[2]] = j;
                    volume->data[indices[X]][indices[Y]][indices[Z]] =
                         (Volume_type) ((Real) (*buffer_ptr) -
                             volume->value_translation) / volume->value_scale;

                    ++buffer_ptr;
                }
            }
        }

        ++volume_input->slice_index;
    }

    *fraction_done = (Real) volume_input->slice_index /
                     (Real) volume->sizes[volume_input->axis_index[0]];

    more_to_do = TRUE;

    if( volume_input->slice_index ==
        (Real) volume->sizes[volume_input->axis_index[0]] )
    {
        more_to_do = FALSE;
        delete_volume_input( volume_input );
    }

    return( more_to_do );
}
