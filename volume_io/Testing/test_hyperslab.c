#include  <volume_io.h>

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

int  main(
    int    argc,
    char   *argv[] )
{
    char                *input_filename;
    Volume              volume, volume2;
    volume_input_struct volume_input;
    int                 sizes[MAX_DIMENSIONS];
    int                 dim, to_array[MAX_DIMENSIONS];
    int                 file_start[MAX_DIMENSIONS], file_count[MAX_DIMENSIONS];
    int                 array_start[MAX_DIMENSIONS];
    int                 array_sizes[MAX_DIMENSIONS];
    int                 file_v[MAX_DIMENSIONS];
    int                 v[MAX_DIMENSIONS];
    int                 voxel[MAX_DIMENSIONS];
    int                 a1, a2, a3, start1, start2, start3;
    int                 count1, count2, count3, n_array_dims;
    int                 test_value, correct_value;
    multidim_array      array;

    input_filename = argv[1];

    /*--- input the two volumes */

    if( input_volume( input_filename, 3, File_order_dimension_names,
            NC_UNSPECIFIED, FALSE,
            0.0, 0.0, TRUE, &volume, (minc_input_options *) NULL ) != OK )
        return( 1 );

    get_volume_sizes( volume, sizes );

    print( "Sizes: %d %d %d\n", sizes[X], sizes[Y], sizes[Z] );

    if( start_volume_input( input_filename, 3, File_order_dimension_names,
            NC_UNSPECIFIED, FALSE,
            0.0, 0.0, TRUE, &volume2, (minc_input_options *) NULL,
            &volume_input ) != OK )
    {
        return( 1 );
    }

    print( "Enter 3 axes, 3 starts, and 3 counts: " );

    while( input_int( stdin, &a1 ) == OK &&
           input_int( stdin, &a2 ) == OK &&
           input_int( stdin, &a3 ) == OK &&
           input_int( stdin, &start1 ) == OK &&
           input_int( stdin, &start2 ) == OK &&
           input_int( stdin, &start3 ) == OK &&
           input_int( stdin, &count1 ) == OK &&
           input_int( stdin, &count2 ) == OK &&
           input_int( stdin, &count3 ) == OK )
    {
        for_less( dim, 0, MAX_DIMENSIONS )
        {
            array_start[dim] = 0;
            array_sizes[dim] = 1;
            file_start[dim] = 0;
            file_count[dim] = 1;
            to_array[dim] = -1;
        }

        n_array_dims = 0;

        if( a1 >= 0 )
        {
            array_sizes[n_array_dims] = count1;
            file_start[a1] = start1;
            file_count[a1] = count1;
            to_array[a1] = n_array_dims;
            ++n_array_dims;
        }

        if( a2 >= 0 )
        {
            array_sizes[n_array_dims] = count2;
            file_start[a2] = start2;
            file_count[a2] = count2;
            to_array[a2] = n_array_dims;
            ++n_array_dims;
        }

        if( a3 >= 0 )
        {
            array_sizes[n_array_dims] = count3;
            file_start[a3] = start3;
            file_count[a3] = count3;
            to_array[a3] = n_array_dims;
            ++n_array_dims;
        }

        create_multidim_array( &array, n_array_dims, array_sizes, 
                               get_volume_data_type(volume) );

        if( input_minc_hyperslab( get_volume_input_minc_file(&volume_input),
                                  &array, array_start, to_array, file_start,
                                  file_count ) != OK )
            return( 1 );

        for_less( file_v[4], file_start[4], file_start[4] + file_count[4] )
        for_less( file_v[3], file_start[3], file_start[3] + file_count[3] )
        for_less( file_v[2], file_start[2], file_start[2] + file_count[2] )
        for_less( file_v[1], file_start[1], file_start[1] + file_count[1] )
        for_less( file_v[0], file_start[0], file_start[0] + file_count[0] )
        {
            for_less( dim, 0, N_DIMENSIONS )
                voxel[dim] = 0;

            for_less( dim, 0, N_DIMENSIONS )
            {
                if( to_array[dim] >= 0 )
                {
                    v[to_array[dim]] = file_v[dim] - file_start[dim];
                    voxel[dim] = file_v[dim];
                }
            }

            correct_value = get_volume_voxel_value( volume,
                          voxel[0], voxel[1], voxel[2], voxel[3], voxel[4] );
            GET_MULTIDIM( test_value, array, v[0], v[1], v[2], v[3], v[4] );

            if( test_value != correct_value )
            {
                print( "Error %d != %d.\n", test_value, correct_value );
            }
        }

        delete_multidim_array( &array );

        (void) input_newline( stdin );
        print( "Enter 3 axes, 3 starts, and 3 counts: " );
    }

    (void) input_newline( stdin );

    return( 0 );
}
