#include  <def_mni.h>

static   const char      *TRANSFORM_FILE_HEADER = "MNI Transform File";
static   const char      *TYPE_STRING = "Transform_Type";
static   const char      *LINEAR_TRANSFORM_STRING = "Linear_Transform";
static   const char      *LINEAR_TYPE = "Linear";
static   const char      *THIN_PLATE_SPLINE_STRING =
                                              "Thin_Plate_Spline_Transform";
static   const char      *INVERT_FLAG_STRING = "Invert_Flag";
static   const char      *TRUE_STRING = "True";
static   const char      *FALSE_STRING = "False";
static   const char      *N_DIMENSIONS_STRING = "Number_Dimensions";
static   const char      *POINTS_STRING = "Points";
static   const char      *DISPLACEMENTS_STRING = "Displacements";

private  void  output_one_transform(
    FILE                *file,
    Boolean             invert,
    General_transform   *transform )
{
    int        i, c, trans;
    Transform  *lin_transform;

    switch( transform->type )
    {
    case LINEAR:
        (void) fprintf( file, "%s = %s;\n", TYPE_STRING, LINEAR_TYPE );

        (void) fprintf( file, "%s =\n", LINEAR_TRANSFORM_STRING );

        if( invert )
            lin_transform = get_inverse_linear_transform_ptr( transform );
        else
            lin_transform = get_linear_transform_ptr( transform );

        for_less( i, 0, 3 )
        {
            (void) fprintf( file, " %15.8f %15.8f %15.8f %15.8f",
                                  Transform_elem(*lin_transform,i,0),
                                  Transform_elem(*lin_transform,i,1),
                                  Transform_elem(*lin_transform,i,2),
                                  Transform_elem(*lin_transform,i,3) );
            if( i == 2 )
                (void) fprintf( file, ";" );
            (void) fprintf( file, "\n" );
        }
        break;

    case THIN_PLATE_SPLINE:
        (void) fprintf( file, "%s = %s;\n", TYPE_STRING,
                        THIN_PLATE_SPLINE_STRING);

        if( transform->inverse_flag )
            invert = !invert;

        if( invert )
            (void) fprintf( file, "%s = %s\n", INVERT_FLAG_STRING, TRUE_STRING);

        (void) fprintf( file, "%s = %d\n", N_DIMENSIONS_STRING,
                        transform->n_dimensions );

        (void) fprintf( file, "%s =\n", POINTS_STRING );

        for_less( i, 0, transform->n_points )
        {
            for_less( c, 0, transform->n_dimensions )
                (void) fprintf( file, " %g", transform->points[i][c] );

            if( i == transform->n_points-1 )
                (void) fprintf( file, ";" );

            (void) fprintf( file, "\n" );
        }

        (void) fprintf( file, "%s =\n", DISPLACEMENTS_STRING );

        for_less( i, 0, transform->n_points + transform->n_dimensions + 1 )
        {
            for_less( c, 0, transform->n_dimensions )
                (void) fprintf( file, " %g", transform->displacements[i][c] );

            if( i == transform->n_points + transform->n_dimensions + 1 - 1 )
                (void) fprintf( file, ";" );

            (void) fprintf( file, "\n" );
        }
        break;

    case USER_TRANSFORM:
        print( "Cannot output user transformation.\n" );
        output_comments( file, "User transform goes here." );
        break;

    case CONCATENATED_TRANSFORM:
        
        if( transform->inverse_flag )
            invert = !invert;

        if( invert )
        {
            for( trans = get_n_concated_transforms(transform)-1;  trans >= 0;
                 --trans )
            {
                 output_one_transform( file, invert,
                               get_nth_general_transform(transform,trans) );
            }
        }
        else
        {
            for_less( trans, 0, get_n_concated_transforms(transform) )
            {
                 output_one_transform( file, invert,
                                   get_nth_general_transform(transform,trans) );
            }
        }
        break;
    }
}

public  Status  output_transform(
    FILE                *file,
    char                comments[],
    General_transform   *transform )
{
    /* parameter checking */

    if( file == (FILE *) 0 )
    {
        print( "output_transform(): passed NULL FILE ptr.\n");
        return( ERROR );
    }

    /* okay write the file */

    (void) fprintf( file, "%s\n", TRANSFORM_FILE_HEADER );

    output_comments( file, comments );
    (void) fprintf( file, "\n" );
    (void) fprintf( file, "\n" );

    output_one_transform( file, FALSE, transform );

    return( OK );
}

private  Status  input_one_transform(
    FILE                *file,
    General_transform   *transform )
{
    Status            status;
    int               i, j, n_points, n_dimensions;
    float             **points, **displacements;
    double            value, *points_1d;
    String            type_name, str;
    Transform         linear_transform;
    Transform_types   type;

    transform->inverse_flag = FALSE;

    /* --- read the type of transform */

    status = mni_input_keyword_and_equal_sign( file, TYPE_STRING, FALSE );

    if( status != OK )
        return( status );

    if( mni_input_string( file, type_name, MAX_STRING_LENGTH, ';', 0 ) != OK )
    {
        print( "input_transform(): missing transform type.\n");
        return( ERROR );
    }
    if( mni_skip_expected_character( file, ';' ) != OK )
        return( ERROR );

    if( strcmp( type_name, LINEAR_TYPE ) == 0 )
        type = LINEAR;
    else if( strcmp( type_name, THIN_PLATE_SPLINE_STRING ) == 0 )
        type = THIN_PLATE_SPLINE;
    else
    {
        print( "input_transform(): invalid transform type.\n");
        return( ERROR );
    }

    /* --- read the next string */

    if( mni_input_string( file, str, MAX_STRING_LENGTH, '=', 0 ) != OK )
        return( ERROR );

    if( strcmp( str, INVERT_FLAG_STRING ) == 0 )
    {
        if( mni_skip_expected_character( file, '=' ) != OK )
            return( ERROR );
        if( mni_input_string( file, str, MAX_STRING_LENGTH, ';', 0 ) != OK )
            return( ERROR );
        if( mni_skip_expected_character( file, ';' ) != OK )
            return( ERROR );

        if( strcmp( str, TRUE_STRING ) == 0 )
            transform->inverse_flag = TRUE;
        else if( strcmp( str, FALSE_STRING ) == 0 )
            transform->inverse_flag = FALSE;
        else
        {
            print( "Expected %s or %s after %s =\n", TRUE_STRING, FALSE_STRING,
                   INVERT_FLAG_STRING );
            return( ERROR );
        }

        if( mni_input_string( file, str, MAX_STRING_LENGTH, '=', 0 ) != OK )
            return( ERROR );
    }

    switch( type )
    {
    case LINEAR:
        if( strcmp( str, LINEAR_TRANSFORM_STRING ) != 0 )
        {
            print( "Expected %s =\n", LINEAR_TRANSFORM_STRING );
            return( ERROR );
        }

        if( mni_skip_expected_character( file, '=' ) != OK )
            return( ERROR );

        make_identity_transform( &linear_transform );

        /* now read the 3 lines of transforms */

        for_less( i, 0, 3 )
        {
            for_less( j, 0, 4 )
            {
                if( mni_input_double( file, &value ) != OK )
                {
                    print(
                    "input_transform(): error reading transform elem [%d,%d]\n",
                    i+1, j+1 );
                    return( ERROR );
                }

                Transform_elem(linear_transform,i,j) = value;
            }
        }

        if( mni_skip_expected_character( file, ';' ) != OK )
            return( ERROR );

        create_linear_transform( transform, &linear_transform );

        break;

    case THIN_PLATE_SPLINE:

        /* --- read Number_Dimensions = 3; */

        if( strcmp( str, N_DIMENSIONS_STRING ) != 0 )
        {
            print( "Expected %s =\n", N_DIMENSIONS_STRING );
            return( ERROR );
        }
        if( mni_skip_expected_character( file, '=' ) != OK )
            return( ERROR );
        if( mni_input_int( file, &n_dimensions ) != OK )
            return( ERROR );
        if( mni_skip_expected_character( file, ';' ) != OK )
            return( ERROR );

        /* --- read Points = x y z x y z .... ; */

        if( mni_input_keyword_and_equal_sign( file, POINTS_STRING, TRUE ) != OK)
            return( ERROR );
        if( mni_input_doubles( file, &n_points, &points_1d ) != OK )
            return( ERROR );

        if( n_points % n_dimensions != 0 )
        {
            print("Number of points (%d) must be multiple of number of dimensions (%d)\n",
                  n_points, n_dimensions );
            return( ERROR );
        }

        n_points = n_points / n_dimensions;

        ALLOC2D( points, n_points, n_dimensions );
        for_less( i, 0, n_points )
        {
            for_less( j, 0, n_dimensions )
            {
                points[i][j] = points_1d[IJ(i,j,n_dimensions)];
            }
        }

        /* --- allocate and input the displacements */

        ALLOC2D( displacements, n_points + n_dimensions + 1, n_dimensions );

        if( mni_input_keyword_and_equal_sign( file, DISPLACEMENTS_STRING, TRUE )
                                                                       != OK )
            return( ERROR );

        for_less( i, 0, n_points + n_dimensions + 1 )
        {
            for_less( j, 0, n_dimensions )
            {
                if( mni_input_double( file, &value ) != OK )
                {
                    print( "Expected more displacements.\n" );
                    return( ERROR );
                }
                displacements[i][j] = value;
            }
        }

        if( mni_skip_expected_character( file, ';' ) != OK )
            return( ERROR );

        create_thin_plate_transform( transform, n_dimensions, n_points, points,
                                     displacements );
    }

    return( OK );
}

public  Status  input_transform(
    FILE                *file,
    General_transform   *transform )
{
    Status              status;
    int                 n_transforms;
    String              line;
    General_transform   next, concated;

    /* parameter checking */

    if( file == (FILE *) 0 )
    {
        print( "input_transform(): passed NULL FILE ptr.\n");
        return( ERROR );
    }

    /* okay read the header */

    if( mni_input_string( file, line, MAX_STRING_LENGTH, 0, 0 ) != OK ||
        strcmp( line, TRANSFORM_FILE_HEADER ) != 0 )
    {
        print( "input_transform(): invalid header in file.\n");
        return( ERROR );
    }

    n_transforms = 0;
    while( (status = input_one_transform( file, &next )) == OK )
    {
        if( n_transforms == 0 )
            *transform = next;
        else
        {
            concat_general_transforms( transform, &next, &concated );
            delete_general_transform( transform );
            delete_general_transform( &next );
            *transform = concated;
        }
        ++n_transforms;
    }

    if( status == ERROR )
    {
        print( "input_transform: error reading transform.\n" );
        return( ERROR );
    }
    else if( n_transforms == 0 )
    {
        print( "input_transform: no transform present.\n" );
        return( ERROR );
    }

    return( OK );
}
