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
    char      **labels )
{
    Status   status;
    int      i;

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
    if( n_tag_points == 0 )
        *tags = (double **) malloc( sizeof(double*) );
    else
        *tags = (double **) realloc( (void *) (*tags),
                                     (n_tag_points+1)*sizeof(double*));

    (*tags)[n_tag_points] = (double *) malloc( 3 * sizeof(double) );

    (*tags)[n_tag_points][0] = x;
    (*tags)[n_tag_points][1] = y;
    (*tags)[n_tag_points][2] = z;
}

private  void  add_tag_label(
    char    ***labels,
    int     n_tag_points,
    char    label[] )
{
    if( n_tag_points == 0 )
        *labels = (char **) malloc( sizeof(char*) );
    else
        *labels = (char **) realloc( (void *) (*labels),
                                     (n_tag_points+1) * sizeof(char*));

    (*labels)[n_tag_points] = (char *) malloc( strlen(label)+1 );
    (void) strcpy( (*labels)[n_tag_points], label );
}

private   void  free_tags(
    double  **tags,
    int     n_tag_points )
{
    int   i;

    for( i = 0;  i < n_tag_points;  ++i )
        free( (void *) (tags[i]) );

    if( n_tag_points > 0 )
        free( (void *) tags );
}

private  void  free_labels(
    char    **labels,
    int     n_tag_points )
{
    int   i;

    for( i = 0;  i < n_tag_points;  ++i )
        free( (void *) (labels[i]) );

    if( n_tag_points > 0 )
        free( (void *) labels );
}

public  void  free_tag_points(
    int       n_volumes,
    int       n_tag_points,
    double    **tags_volume1,
    double    **tags_volume2,
    char      **labels )
{
    free_tags( tags_volume1, n_tag_points );

    if( n_volumes == 2 )
        free_tags( tags_volume2, n_tag_points );

    if( labels != (char **) NULL )
        free_labels( labels, n_tag_points );
}

public  Status  input_tag_points(
    FILE      *file,
    int       *n_volumes,
    int       *n_tag_points,
    double    ***tags_volume1,
    double    ***tags_volume2,
    char      ***labels )
{
    String  line;
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

    if( mni_input_keyword_and_equal_sign( file, VOLUMES_STRING ) != OK )
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

    if( mni_input_keyword_and_equal_sign( file, TAG_POINTS_STRING ) != OK )
        return( ERROR );

    *n_tag_points = 0;

    while( mni_input_double( file, &x1 ) == OK )
    {
        if( mni_input_double( file, &y1 ) != OK ||
            mni_input_double( file, &z1 ) != OK ||
            (*n_volumes == 2 &&
             (mni_input_double( file, &y2 ) != OK ||
              mni_input_double( file, &x2 ) != OK ||
              mni_input_double( file, &z2 ) != OK)) ||
            mni_input_string( file, label, MAX_STRING_LENGTH, ' ', ';' ) != OK )
        {
            (void) fprintf( stderr,
                      "input_tag_points(): error reading tag point %d\n",
                      *n_tag_points + 1 );

            free_tag_points( *n_volumes, *n_tag_points, *tags_volume1,
                             *tags_volume2, *labels );
            return( ERROR );
        }

        add_tag_point( tags_volume1, *n_tag_points, x1, y1, z1 );

        if( *n_volumes == 2 )
            add_tag_point( tags_volume2, *n_tag_points, x2, y2, z2 );

        if( labels != (char ***) NULL )
            add_tag_label( labels, *n_tag_points, label );

        ++(*n_tag_points);
    }

    if( mni_skip_expected_character( file, ';' ) != OK )
        return( ERROR );

    return( OK );
}
