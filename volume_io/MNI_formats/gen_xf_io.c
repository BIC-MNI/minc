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

public  Status  output_transform(
    FILE                *file,
    char                comments[],
    General_transform   *transform )
{
    int        i, c;
    Transform  *lin_transform;

    /* parameter checking */

    if( file == (FILE *) 0 )
    {
        (void) fprintf( stderr, "output_transform(): passed NULL FILE ptr.\n");
        return( ERROR );
    }

    /* okay write the file */

    (void) fprintf( file, "%s\n", TRANSFORM_FILE_HEADER );

    output_comments( file, comments );
    (void) fprintf( file, "\n" );
    (void) fprintf( file, "\n" );

    switch( transform->type )
    {
    case LINEAR6:
    case LINEAR7:
    case LINEAR9:
    case LINEAR12:
        (void) fprintf( file, "%s = %s\n", TYPE_STRING, LINEAR_TYPE );
        if( transform->inverse_flag )
            (void) fprintf( file, "%s = %s\n", INVERT_FLAG_STRING, TRUE_STRING);

        (void) fprintf( file, "%s =\n", LINEAR_TRANSFORM_STRING );

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
        (void) fprintf( file, "%s = %s\n", TYPE_STRING,
                        THIN_PLATE_SPLINE_STRING);
        if( transform->inverse_flag )
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
    }

    return( OK );
}

public  Status  input_transform(
    FILE                *file,
    General_transform   *transform )
{
    int               i, j, n_points, n_dimensions;
    float             **points, **displacements;
    double            value, *points_1d;
    String            line, type_name, str;
    Transform_types   type;

    /* parameter checking */

    if( file == (FILE *) 0 )
    {
        (void) fprintf( stderr, "input_transform(): passed NULL FILE ptr.\n");
        return( ERROR );
    }

    transform->inverse_flag = FALSE;

    /* okay read the header */

    if( mni_input_string( file, line, MAX_STRING_LENGTH, 0, 0 ) != OK ||
        strcmp( line, TRANSFORM_FILE_HEADER ) != 0 )
    {
        (void) fprintf(stderr, "input_transform(): invalid header in file.\n");
        return( ERROR );
    }

    /* --- read the type of transform */

    if( mni_input_keyword_and_equal_sign( file, TYPE_STRING ) != OK )
        return( ERROR );

    if( mni_input_string( file, type_name, MAX_STRING_LENGTH, ';', 0 ) != OK )
    {
        (void) fprintf(stderr, "input_transform(): missing transform type.\n");
        return( ERROR );
    }
    if( mni_skip_expected_character( file, ';' ) != OK )
        return( ERROR );

    if( strcmp( type_name, LINEAR_TRANSFORM_STRING ) == 0 )
        type = LINEAR12;
    else if( strcmp( type_name, THIN_PLATE_SPLINE_STRING ) == 0 )
        type = THIN_PLATE_SPLINE;
    else
    {
        (void) fprintf(stderr, "input_transform(): invalid transform type.\n");
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
    case LINEAR12:
        if( strcmp( str, LINEAR_TRANSFORM_STRING ) != 0 )
        {
            print( "Expected %s =\n", LINEAR_TRANSFORM_STRING );
            return( ERROR );
        }

        if( mni_skip_expected_character( file, '=' ) != OK )
            return( ERROR );

        create_linear_transform( transform, (Transform *) NULL );

        /* now read the 3 lines of transforms */

        for_less( i, 0, 3 )
        {
            for_less( j, 0, 4 )
            {
                if( mni_input_double( file, &value ) != OK )
                {
                    (void) fprintf( stderr,
                          "input_transform(): error reading transform elem [%d,%d]\n",
                          i+1, j+1 );
                    return( ERROR );
                }

                Transform_elem(*get_linear_transform_ptr(transform),i,j) =
                                                                        value;
            }
        }
        if( mni_skip_expected_character( file, ';' ) != OK )
            return( ERROR );

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

        if( mni_input_keyword_and_equal_sign( file, POINTS_STRING ) != OK )
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

        if( mni_input_keyword_and_equal_sign( file, DISPLACEMENTS_STRING ) != OK )
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
