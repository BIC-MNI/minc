#include  <def_mni.h>

static   const char      *TAG_FILE_HEADER = "MNI Tag Point File";
static   const char      *VOLUMES_STRING = "Volumes";
static   const char      *TAG_POINTS_STRING = "Points";

public  Status  output_tag_points(
    FILE      *file,
    char      comments[],
    int       n_volumes,
    int       n_tag_points,
    double    **tags_volume1,
    double    **tags_volume2,
    Real      weights[],
    int       structure_ids[],
    int       patient_ids[],
    char      **labels )
{
    Status   status;
    int      i;
    Boolean  aux_present;

    /* parameter checking */

    status = OK;

    if( file == (FILE *) 0 )
    {
        (void) fprintf( stderr, "output_tag_points(): passed NULL FILE ptr.\n");
        status = ERROR;
    }

    if( n_volumes != 1 && n_volumes != 2 )
    {
        (void) fprintf( stderr, "output_tag_points():" );
        (void) fprintf( stderr, " can only support 1 or 2 volumes;\n" );
        (void) fprintf( stderr, "     you've supplied %d.\n", n_volumes );
        status = ERROR;
    }

    if( n_tag_points < 0 )
    {
        (void) fprintf( stderr, "output_tag_points():" );
        (void) fprintf( stderr, " n_tag_points must be greater than 0;\n" );
        (void) fprintf( stderr, "     you've supplied %d.\n", n_tag_points );
        status = ERROR;
    }

    if( status != OK )
        return( status );

    /* okay write the file */

    (void) fprintf( file, "%s\n", TAG_FILE_HEADER );
    (void) fprintf( file, "%s = %d;\n", VOLUMES_STRING, n_volumes );
    output_comments( file, comments );
    (void) fprintf( file, "\n" );
    (void) fprintf( file, "\n" );

    (void) fprintf( file, "%s =\n", TAG_POINTS_STRING );

    aux_present = (weights != (Real *) NULL ||
                   structure_ids != (int *) NULL ||
                   patient_ids != (int *) NULL);

    for( i = 0;  i < n_tag_points;  ++i )
    {
        (void) fprintf( file, " %10.5f %10.5f %10.5f",
                        tags_volume1[i][0],
                        tags_volume1[i][1],
                        tags_volume1[i][2] );

        if( n_volumes >= 2 )
        {
            (void) fprintf( file, " %10.5f %10.5f %10.5f",
                            tags_volume2[i][0],
                            tags_volume2[i][1],
                            tags_volume2[i][2] );
        }

        if( aux_present )
        {
            if( weights != (Real *) NULL )
                (void) fprintf( file, " %g", weights[i] );
            else
                (void) fprintf( file, " %g", 0.0 );

            if( structure_ids != (int *) NULL )
                (void) fprintf( file, " %d", structure_ids[i] );
            else
                (void) fprintf( file, " %d", -1 );

            if( patient_ids != (int *) NULL )
                (void) fprintf( file, " %d", patient_ids[i] );
            else
                (void) fprintf( file, " %d", -1 );
        }

        if( labels != (char **) NULL )
            (void) fprintf( file, " \"%s\"", labels[i] );

        if( i == n_tag_points - 1 )
            (void) fprintf( file, ";" );
        (void) fprintf( file, "\n" );
    }

    return( status );
}

private  void  add_tag_point(
    double  ***tags,
    int     n_tag_points,
    double  x,
    double  y,
    double  z )
{
    SET_ARRAY_SIZE( *tags, n_tag_points, n_tag_points+1, DEFAULT_CHUNK_SIZE );

    ALLOC( (*tags)[n_tag_points], 3 );

    (*tags)[n_tag_points][0] = x;
    (*tags)[n_tag_points][1] = y;
    (*tags)[n_tag_points][2] = z;
}

private  void  add_tag_weight(
    double  **weights,
    int     n_tag_points,
    double  weight )
{
    SET_ARRAY_SIZE( *weights, n_tag_points, n_tag_points+1, DEFAULT_CHUNK_SIZE);
    (*weights)[n_tag_points] = weight;
}

private  void  add_tag_id(
    int  **ids,
    int  n_tag_points,
    int  id )
{
    SET_ARRAY_SIZE( *ids, n_tag_points, n_tag_points+1, DEFAULT_CHUNK_SIZE);
    (*ids)[n_tag_points] = id;
}

private  void  add_tag_label(
    char    ***labels,
    int     n_tag_points,
    char    label[] )
{
    SET_ARRAY_SIZE( *labels, n_tag_points, n_tag_points+1, DEFAULT_CHUNK_SIZE);

    ALLOC( (*labels)[n_tag_points], strlen(label)+1 );
    (void) strcpy( (*labels)[n_tag_points], label );
}

private   void  free_tags(
    double  **tags,
    int     n_tag_points )
{
    int   i;

    for( i = 0;  i < n_tag_points;  ++i )
        FREE( tags[i] );

    if( n_tag_points > 0 )
        FREE( tags );
}

private  void  free_labels(
    char    **labels,
    int     n_tag_points )
{
    int   i;

    for( i = 0;  i < n_tag_points;  ++i )
        FREE( labels[i] );

    if( n_tag_points > 0 )
        FREE( labels );
}

public  void  free_tag_points(
    int       n_volumes,
    int       n_tag_points,
    double    **tags_volume1,
    double    **tags_volume2,
    Real      weights[],
    int       structure_ids[],
    int       patient_ids[],
    char      **labels )
{
    free_tags( tags_volume1, n_tag_points );

    if( n_volumes == 2 )
        free_tags( tags_volume2, n_tag_points );

    if( weights != (Real *) NULL )
        FREE( weights );

    if( structure_ids != (int *) NULL )
        FREE( structure_ids );

    if( patient_ids != (int *) NULL )
        FREE( patient_ids );

    if( labels != (char **) NULL )
        free_labels( labels, n_tag_points );
}

private  void  extract_label(
    char     str[],
    char     label[] )
{
    Boolean  quoted;
    int      i, len;

    i = 0;
    len = 0;

    while( str[i] == ' ' || str[i] == '\t' )
        ++i;

    if( str[i] == '"' )
    {
        quoted = TRUE;
        ++i;
    }
    else
        quoted = FALSE;

    while( str[i] != (char) 0 &&
           (quoted && str[i] != '"' ||
            !quoted && str[i] != ' ' && str[i] != '\t') )
    {
        label[len] = str[i];
        ++len;
        ++i;
    }

    label[len] = (char) 0;
}

public  Status  input_tag_points(
    FILE      *file,
    int       *n_volumes,
    int       *n_tag_points,
    double    ***tags_volume1,
    double    ***tags_volume2,
    Real      **weights,
    int       **structure_ids,
    int       **patient_ids,
    char      ***labels )
{
    String  line;
    double  weight;
    Boolean last_was_blank, in_quotes;
    int     n_strings, structure_id, patient_id, pos, i;
    double  x1, y1, z1, x2, y2, z2;
    String  label;

    /* parameter checking */

    if( file == (FILE *) 0 )
    {
        (void) fprintf( stderr, "input_tag_points(): passed NULL FILE ptr.\n");
        return( ERROR );
    }

    /* okay read the header */

    if( mni_input_string( file, line, MAX_STRING_LENGTH, 0, 0 ) != OK ||
        strcmp( line, TAG_FILE_HEADER ) != 0 )
    {
        (void) fprintf(stderr, "input_tag_points(): invalid header in file.\n");
        return( ERROR );
    }

    /* now read the number of volumes */

    if( mni_input_keyword_and_equal_sign( file, VOLUMES_STRING, TRUE ) != OK )
        return( ERROR );

    if( mni_input_int( file, n_volumes ) != OK )
    {
        (void) fprintf( stderr,
                 "input_tag_points(): expected # volumes after %s.\n",
                 VOLUMES_STRING );
        return( ERROR );
    }

    if( mni_skip_expected_character( file, ';' ) != OK )
        return( ERROR );

    if( *n_volumes != 1 && *n_volumes != 2 )
    {
        (void) fprintf( stderr, "input_tag_points(): invalid # volumes: %d \n",
                        *n_volumes );
        return( ERROR );
    }

    /* now read the tag points */

    if( mni_input_keyword_and_equal_sign( file, TAG_POINTS_STRING, TRUE ) != OK)
        return( ERROR );

    *n_tag_points = 0;

    while( mni_input_double( file, &x1 ) == OK )
    {
        if( mni_input_double( file, &y1 ) != OK ||
            mni_input_double( file, &z1 ) != OK ||
            (*n_volumes == 2 &&
             (mni_input_double( file, &y2 ) != OK ||
              mni_input_double( file, &x2 ) != OK ||
              mni_input_double( file, &z2 ) != OK)) )
        {
            (void) fprintf( stderr,
                      "input_tag_points(): error reading tag point %d\n",
                      *n_tag_points + 1 );
            return( ERROR );
        }

        add_tag_point( tags_volume1, *n_tag_points, x1, y1, z1 );

        if( *n_volumes == 2 )
            add_tag_point( tags_volume2, *n_tag_points, x2, y2, z2 );

        label[0] = (char) 0;
        weight = 0.0;
        structure_id = -1;
        patient_id = -1;

        n_strings = 0;
        if( mni_input_line( file, line, MAX_STRING_LENGTH ) == OK )
        {
            i = 0;
            last_was_blank = TRUE;
            in_quotes = FALSE;
            while( line[i] != (char) 0 )
            {
                if( line[i] == ' ' || line[i] == '\t' )
                {
                    last_was_blank = TRUE;
                }
                else
                {
                    if( last_was_blank && !in_quotes )
                        ++n_strings;

                    last_was_blank = FALSE;

                    if( line[i] == '\"' )
                        in_quotes = !in_quotes;
                }
                ++i;
            }

            while( i > 0 &&
                   (line[i] == ' ' || line[i] == '\t' || line[i] == (char) 0 ) )
                --i;

            if( line[i] == ';' )
            {
                (void) unget_character( file, ';' );
                line[i] = (char) 0;
            }
        }

        if( n_strings == 0 )
        {
        }
        else if( n_strings == 1 )
        {
            extract_label( line, label );
        }
        else if( n_strings < 3 || n_strings > 4 ||
                 sscanf( line, "%lf %d %d %n", &weight, &structure_id,
                         &patient_id, &pos ) != 4 )
        {
            (void) fprintf( stderr,
                  "input_tag_points(): error reading tag point %d\n",
                  *n_tag_points + 1 );
            return( ERROR );
        }
        else if( n_strings == 4 )
        {
            extract_label( &line[pos], label );
        }

        if( weights != (Real **) NULL )
            add_tag_weight( weights, *n_tag_points, weight );

        if( structure_ids != (int **) NULL )
            add_tag_id( structure_ids, *n_tag_points, structure_id );

        if( patient_ids != (int **) NULL )
            add_tag_id( patient_ids, *n_tag_points, patient_id );

        if( labels != (char ***) NULL )
            add_tag_label( labels, *n_tag_points, label );

        ++(*n_tag_points);
    }

    if( mni_skip_expected_character( file, ';' ) != OK )
        return( ERROR );

    return( OK );
}
