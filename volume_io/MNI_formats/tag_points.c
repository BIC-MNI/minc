#include  <def_mni.h>
#include  <def_tag_io.h>

static   const char      COMMENT_CHAR1 = '%';
static   const char      COMMENT_CHAR2 = '#';

static   const char      *TAG_FILE_HEADER = "MNI Tag Point File";
static   const char      *VOLUMES_EQUAL = "Volumes = ";

static   const char      *TRANSFORM_FILE_HEADER = "MNI Transform File";

static  void  output_comments( FILE *, char [] );

#define  STRING_LENGTH  213

public  int  output_tag_points(
    FILE      *file,
    char      comments[],
    int       n_volumes,
    int       n_tag_points,
    double    **tags_volume1,
    double    **tags_volume2,
    char      **labels )
{
    int   i, okay;

    /* parameter checking */

    okay = 1;

    if( file == (FILE *) 0 )
    {
        (void) fprintf( stderr, "output_tag_points(): passed NULL FILE ptr.\n");
        okay = 0;
    }

    if( n_volumes != 1 && n_volumes != 2 )
    {
        (void) fprintf( stderr, "output_tag_points():" );
        (void) fprintf( stderr, " can only support 1 or 2 volumes;\n" );
        (void) fprintf( stderr, "     you've supplied %d.\n", n_volumes );
        okay = 0;
    }

    if( n_tag_points < 0 )
    {
        (void) fprintf( stderr, "output_tag_points():" );
        (void) fprintf( stderr, " n_tag_points must be greater than 0;\n" );
        (void) fprintf( stderr, "     you've supplied %d.\n", n_tag_points );
        okay = 0;
    }

    if( !okay )
        return( 0 );

    /* okay write the file */

    (void) fprintf( file, "%s\n", TAG_FILE_HEADER );
    (void) fprintf( file, "%s%d\n", VOLUMES_EQUAL, n_volumes );
    output_comments( file, comments );
    (void) fprintf( file, "\n" );

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

        (void) fprintf( file, "\n" );
    }

    return( 1 );
}

private  int  line_is_comment(
    char   line[] )
{
    return( line[0] == COMMENT_CHAR1 || line[0] == COMMENT_CHAR2 );
}

private  int  line_is_blank(
    char   line[] )
{
    int   i, blank;

    blank = 1;

    for( i = 0;  i < strlen(line);  ++i )
    {
        if( line[i] != ' ' && line[i] != '\t' && line[i] != '\n' )
        {
            blank = 0;
            break;
        }
    }

    return( blank );
}

private  int  input_a_line(
    FILE     *file,
    char     line[] )
{
    int   len;
    char  *ret;

    do
    {
        ret = fgets( line, STRING_LENGTH, file );
    }
    while( ret != (char *) NULL &&
           (line_is_comment( line ) || line_is_blank( line )) );

    len = strlen(line);
    if( len > 0 && line[len-1] == '\n' )
        line[len-1] = (char) 0;        /* remove the '\n' at the end */

    return( ret != (char *) NULL );
}

private  int  extract_tag_points(
    int      n_volumes,
    char     line[],
    double   *x1,
    double   *y1,
    double   *z1,
    double   *x2,
    double   *y2,
    double   *z2,
    char     *label )
{
    int   i, pos, quoted;

    if( sscanf( line, "%lf %lf %lf %n", x1, y1, z1, &pos ) != 3 )
        return( 0 );

    line = &line[pos];

    if( n_volumes == 2 )
    {
        if( sscanf( line, "%lf %lf %lf %n", x2, y2, z2, &pos ) != 3 )
            return( 0 );

        line = &line[pos];
    }

    /* skip leading white space of label */

    pos = 0;
    while( line[pos] == ' ' || line[pos] == '\t' )
        ++pos;

    if( line[pos] == '"' )
    {
        quoted = 1;
        ++pos;
    }
    else
        quoted = 0;

    i = 0;   

    while( line[pos] != (char) 0 && (!quoted || line[pos] != '"') )
    {
        label[i] = line[pos];
        ++pos;
        ++i;
    }

    label[i] = (char) 0;

    /* remove trailing white space */

    while( i > 0 && (label[i-1] == ' ' || label[i-1] == '\t' ) )
    {
        --i;
        label[i] = (char) 0;
    }

    return( 1 );
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

public  int  input_tag_points(
    FILE      *file,
    int       *n_volumes,
    int       *n_tag_points,
    double    ***tags_volume1,
    double    ***tags_volume2,
    char      ***labels )
{
    char    line[STRING_LENGTH+1];
    double  x1, y1, z1, x2, y2, z2;
    char    label[STRING_LENGTH+1];

    /* parameter checking */

    if( file == (FILE *) 0 )
    {
        (void) fprintf( stderr, "input_tag_points(): passed NULL FILE ptr.\n");
        return( 0 );
    }

    /* okay read the header */

    if( !input_a_line( file, line ) ||
        strcmp( line, TAG_FILE_HEADER ) != 0 )
    {
        (void) fprintf(stderr, "input_tag_points(): invalid header in file.\n");
        return( 0 );
    }

    /* now read the number of volumes */

    if( !input_a_line( file, line ) ||
        strncmp( line, VOLUMES_EQUAL, strlen(VOLUMES_EQUAL) ) != 0 )
    {
        (void) fprintf(stderr, "input_tag_points(): invalid # volumes line.\n");
        return( 0 );
    }

    (void) sscanf( &line[strlen(VOLUMES_EQUAL)], "%d", n_volumes );

    if( *n_volumes != 1 && *n_volumes != 2 )
    {
        (void) fprintf( stderr, "input_tag_points(): invalid # volumes: %d \n",
                        *n_volumes );
        return( 0 );
    }

    *n_tag_points = 0;

    while( input_a_line( file, line ) )
    {
        if( !extract_tag_points( *n_volumes, line,
                                 &x1, &y1, &z1, &x2, &y2, &z2, label ) )
        {
            (void) fprintf( stderr,
                      "input_tag_points(): error reading tag point %d\n",
                      *n_tag_points + 1 );

            free_tag_points( *n_volumes, *n_tag_points, *tags_volume1,
                             *tags_volume2, *labels );
            return( 0 );
        }

        add_tag_point( tags_volume1, *n_tag_points, x1, y1, z1 );

        if( *n_volumes == 2 )
            add_tag_point( tags_volume2, *n_tag_points, x2, y2, z2 );

        if( labels != (char ***) NULL )
            add_tag_label( labels, *n_tag_points, label );

        ++(*n_tag_points);
    }

    return( 1 );
}

public  int  output_transform(
    FILE      *file,
    char      comments[],
    double    transform[3][4] )
{
    int   i;

    /* parameter checking */

    if( file == (FILE *) 0 )
    {
        (void) fprintf( stderr, "output_transform(): passed NULL FILE ptr.\n");
        return( 0 );
    }

    /* okay write the file */

    (void) fprintf( file, "%s\n", TRANSFORM_FILE_HEADER );
    output_comments( file, comments );
    (void) fprintf( file, "\n" );

    for( i = 0;  i < 3;  ++i )
    {
        (void) fprintf( file, " %15.8f %15.8f %15.8f %15.8f",
                              transform[i][0],
                              transform[i][1],
                              transform[i][2],
                              transform[i][3] );
        (void) fprintf( file, "\n" );
    }

    return( 1 );
}

public  int  input_transform(
    FILE      *file,
    double    transform[3][4] )
{
    int     i;
    char    line[STRING_LENGTH+1];

    /* parameter checking */

    if( file == (FILE *) 0 )
    {
        (void) fprintf( stderr, "input_transform(): passed NULL FILE ptr.\n");
        return( 0 );
    }

    /* okay read the header */

    if( !input_a_line( file, line ) ||
        strcmp( line, TRANSFORM_FILE_HEADER ) != 0 )
    {
        (void) fprintf(stderr, "input_transform(): invalid header in file.\n");
        return( 0 );
    }

    /* now read the 3 lines of transforms */

    for( i = 0;  i < 3;  ++i )
    {
        if( !input_a_line( file, line ) ||
            sscanf( line, "%lf %lf %lf %lf",
                    &transform[i][0], &transform[i][1],
                    &transform[i][2], &transform[i][3] ) != 4 )
        {
            (void) fprintf( stderr,
                      "input_transform(): error reading transform line %d\n",
                      i + 1 );
            return( 0 );
        }
    }

    return( 1 );
}

private  void  output_comments(
    FILE   *file,
    char   comments[] )
{
    int   i, len;

    if( comments != (char *) NULL )
    {
        len = strlen( comments );

        (void) fputc( COMMENT_CHAR1, file );
        for( i = 0;  i < len;  ++i )
        {
            (void) fputc( comments[i], file );
            if( comments[i] == '\n' )
                (void) fputc( COMMENT_CHAR1, file );
        }
    }
}
