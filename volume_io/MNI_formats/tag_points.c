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

#include  <internal_volume_io.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/minc/volume_io/MNI_formats/tag_points.c,v 1.17 1995-10-18 16:26:00 david Exp $";
#endif

static   const char      *TAG_FILE_HEADER = "MNI Tag Point File";
static   const char      *VOLUMES_STRING = "Volumes";
static   const char      *TAG_POINTS_STRING = "Points";

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_default_tag_file_suffix
@INPUT      : 
@OUTPUT     : 
@RETURNS    : "tag"
@DESCRIPTION: Returns the default tag file suffix.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  STRING  get_default_tag_file_suffix()
{
    return( "tag" );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : output_tag_points
@INPUT      : file
              comments       - may be null
              n_volumes
              n_tag_points
              tags_volume1
              tags_volume2
              weights
              structure_ids
              patient_ids
              labels
@OUTPUT     : 
@RETURNS    : OK or ERROR
@DESCRIPTION: Outputs the tag points in MNI tag point format.  If weights,
              structure_ids, and patient_ids are all NULL, they are not
              written to the file.  If labels is NULL, it is not written.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  output_tag_points(
    FILE      *file,
    STRING    comments,
    int       n_volumes,
    int       n_tag_points,
    Real      **tags_volume1,
    Real      **tags_volume2,
    Real      weights[],
    int       structure_ids[],
    int       patient_ids[],
    STRING    *labels )
{
    Status   status;
    int      i;
    BOOLEAN  aux_present;

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

    (void) fprintf( file, "%s =\n", TAG_POINTS_STRING );

    aux_present = (weights != (Real *) NULL ||
                   structure_ids != (int *) NULL ||
                   patient_ids != (int *) NULL);

    for( i = 0;  i < n_tag_points;  ++i )
    {
        (void) fprintf( file, " %.8g %.8g %.8g",
                        tags_volume1[i][0],
                        tags_volume1[i][1],
                        tags_volume1[i][2] );

        if( n_volumes >= 2 )
        {
            (void) fprintf( file, " %.8g %.8g %.8g",
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

        if( labels != (STRING *) NULL )
            (void) fprintf( file, " \"%s\"", labels[i] );

        if( i == n_tag_points - 1 )
            (void) fprintf( file, ";" );
        (void) fprintf( file, "\n" );
    }

    if( n_tag_points == 0 )
        (void) fprintf( file, ";\n" );

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : add_tag_point
@INPUT      : n_tag_points
              x
              y
              z
@OUTPUT     : tags
@RETURNS    : 
@DESCRIPTION: Adds the tag point to the tags, after increasing the size of the
              array.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  void  add_tag_point(
    Real    ***tags,
    int     n_tag_points,
    Real    x,
    Real    y,
    Real    z )
{
    SET_ARRAY_SIZE( *tags, n_tag_points, n_tag_points+1, DEFAULT_CHUNK_SIZE );

    ALLOC( (*tags)[n_tag_points], 3 );

    (*tags)[n_tag_points][0] = x;
    (*tags)[n_tag_points][1] = y;
    (*tags)[n_tag_points][2] = z;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : add_tag_weight
@INPUT      : n_tag_points
              weight
@OUTPUT     : weights
@RETURNS    : 
@DESCRIPTION: Adds the weight to the array, increasing size as needed.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  void  add_tag_weight(
    Real    **weights,
    int     n_tag_points,
    Real    weight )
{
    SET_ARRAY_SIZE( *weights, n_tag_points, n_tag_points+1, DEFAULT_CHUNK_SIZE);
    (*weights)[n_tag_points] = weight;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : add_tag_id
@INPUT      : n_tag_points
              id
@OUTPUT     : ids
@RETURNS    : 
@DESCRIPTION: Adds the id to the array, increasing size as needed.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  void  add_tag_id(
    int  **ids,
    int  n_tag_points,
    int  id )
{
    SET_ARRAY_SIZE( *ids, n_tag_points, n_tag_points+1, DEFAULT_CHUNK_SIZE);
    (*ids)[n_tag_points] = id;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : add_tag_label
@INPUT      : n_tag_points
              label
@OUTPUT     : labels
@RETURNS    : 
@DESCRIPTION: Adds the label to the array, increasing size as needed.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  void  add_tag_label(
    STRING  *labels[],
    int     n_tag_points,
    STRING  label )
{
    SET_ARRAY_SIZE( *labels, n_tag_points, n_tag_points+1, DEFAULT_CHUNK_SIZE);

    (*labels)[n_tag_points] = create_string( label );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : free_tags
@INPUT      : tags
              n_tag_points
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Frees the tag x,y,z positions.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private   void  free_tags(
    Real    **tags,
    int     n_tag_points )
{
    int   i;

    for( i = 0;  i < n_tag_points;  ++i )
        FREE( tags[i] );

    if( n_tag_points > 0 )
        FREE( tags );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : free_tag_points
@INPUT      : n_volumes
              n_tag_points
              tags_volume1
              tags_volume2
              weights
              structure_ids
              patient_ids
              labels
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Frees the tag point data.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  free_tag_points(
    int       n_volumes,
    int       n_tag_points,
    Real      **tags_volume1,
    Real      **tags_volume2,
    Real      weights[],
    int       structure_ids[],
    int       patient_ids[],
    char      **labels )
{
    int   i;

    if( n_tag_points > 0 )
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
        {
            for( i = 0;  i < n_tag_points;  ++i )
                delete_string( labels[i] );

            if( n_tag_points > 0 )
                FREE( labels );
        }
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : extract_label
@INPUT      : str
@OUTPUT     : label
@RETURNS    : 
@DESCRIPTION: Extracts the label from the string, by either taking the
              first space delimited word, or first quoted string.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  STRING  extract_label(
    STRING     str )
{
    BOOLEAN  quoted;
    int      i;
    STRING   label;

    i = 0;

    /* --- skip leading space */

    while( str[i] == ' ' || str[i] == '\t' )
        ++i;

    if( str[i] == '"' )
    {
        quoted = TRUE;
        ++i;
    }
    else
        quoted = FALSE;

    /* --- copy characters until either closing quote is found (if quoted),
           or white space or end of string is found */

    label = create_string( NULL );

    while( str[i] != END_OF_STRING &&
           (quoted && str[i] != '"' ||
            !quoted && str[i] != ' ' && str[i] != '\t') )
    {
        concat_char_to_string( &label, str[i] );
        ++i;
    }

    return( label );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : input_tag_points
@INPUT      : file
@OUTPUT     : n_volumes
              n_tag_points
              tags_volume1
              tags_volume2
              weights
              structure_ids
              patient_ids
              labels
@RETURNS    : OK or ERROR
@DESCRIPTION: Inputs the file and passes back the data.  The last four arguments
              can each be set to NULL if the corresponding information is not
              desired.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  input_tag_points(
    FILE      *file,
    int       *n_volumes,
    int       *n_tag_points,
    Real      ***tags_volume1,
    Real      ***tags_volume2,
    Real      **weights,
    int       **structure_ids,
    int       **patient_ids,
    STRING    *labels[] )
{
    STRING  line;
    Real    weight;
    BOOLEAN last_was_blank, in_quotes;
    int     n_strings, structure_id, patient_id, pos, i;
    Real    x1, y1, z1, x2, y2, z2;
    STRING  label;

    /* parameter checking */

    if( file == NULL )
    {
        (void) fprintf( stderr, "input_tag_points(): passed NULL FILE ptr.\n");
        return( ERROR );
    }

    /* okay read the header */

    if( mni_input_string( file, &line, (char) 0, (char) 0 ) != OK ||
        !equal_strings( line, (STRING) TAG_FILE_HEADER ) )
    {
        (void) fprintf(stderr, "input_tag_points(): invalid header in file.\n");
        delete_string( line );
        return( ERROR );
    }

    delete_string( line );

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

    if( mni_skip_expected_character( file, (char) ';' ) != OK )
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

    while( mni_input_real( file, &x1 ) == OK )
    {
        if( mni_input_real( file, &y1 ) != OK ||
            mni_input_real( file, &z1 ) != OK ||
            (*n_volumes == 2 &&
             (mni_input_real( file, &x2 ) != OK ||
              mni_input_real( file, &y2 ) != OK ||
              mni_input_real( file, &z2 ) != OK)) )
        {
            (void) fprintf( stderr,
                      "input_tag_points(): error reading tag point %d\n",
                      *n_tag_points + 1 );
            return( ERROR );
        }

        add_tag_point( tags_volume1, *n_tag_points, x1, y1, z1 );

        if( *n_volumes == 2 && tags_volume2 != NULL )
            add_tag_point( tags_volume2, *n_tag_points, x2, y2, z2 );

        label = NULL;
        weight = 0.0;
        structure_id = -1;
        patient_id = -1;

        n_strings = 0;
        if( mni_input_line( file, &line ) == OK )
        {
            i = 0;
            last_was_blank = TRUE;
            in_quotes = FALSE;
            while( line[i] != END_OF_STRING )
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
                   (line[i] == ' ' || line[i] == '\t' ||
                    line[i] == END_OF_STRING) )
                --i;

            if( line[i] == ';' )
            {
                (void) unget_character( file, (char) ';' );
                line[i] = END_OF_STRING;
            }
        }

        if( n_strings != 0 )
        {
            if( n_strings == 1 )
            {
                label = extract_label( line );
            }
            else if( n_strings < 3 || n_strings > 4 ||
                     sscanf( line, "%lf %d %d %n", &weight, &structure_id,
                             &patient_id, &pos ) != 3 )
            {
                (void) fprintf( stderr,
                      "input_tag_points(): error reading tag point %d\n",
                      *n_tag_points + 1 );
                return( ERROR );
            }
            else if( n_strings == 4 )
            {
                label = extract_label( &line[pos] );
            }
        }

        delete_string( line );

        if( weights != (Real **) NULL )
            add_tag_weight( weights, *n_tag_points, weight );

        if( structure_ids != (int **) NULL )
            add_tag_id( structure_ids, *n_tag_points, structure_id );

        if( patient_ids != (int **) NULL )
            add_tag_id( patient_ids, *n_tag_points, patient_id );

        if( labels != (STRING **) NULL )
            add_tag_label( labels, *n_tag_points, label );

        delete_string( label );

        ++(*n_tag_points);
    }

    if( mni_skip_expected_character( file, (char) ';' ) != OK )
        return( ERROR );

    return( OK );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : output_tag_file
@INPUT      : filename
              comments
              n_volumes
              n_tag_points
              tags_volume1
              tags_volume2
              weights
              structure_ids
              patient_ids
              labels
@OUTPUT     : 
@RETURNS    : OK or ERROR
@DESCRIPTION: Opens the file, outputs the tag points, and closes the file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  output_tag_file(
    STRING    filename,
    STRING    comments,
    int       n_volumes,
    int       n_tag_points,
    Real      **tags_volume1,
    Real      **tags_volume2,
    Real      weights[],
    int       structure_ids[],
    int       patient_ids[],
    STRING    labels[] )
{
    Status  status;
    FILE    *file;

    status = open_file_with_default_suffix( filename,
                                            get_default_tag_file_suffix(),
                                            WRITE_FILE, ASCII_FORMAT, &file );

    if( status == OK )
        status = output_tag_points( file, comments, n_volumes, n_tag_points,
                                    tags_volume1, tags_volume2, weights,
                                    structure_ids, patient_ids, labels );

    if( status == OK )
        status = close_file( file );

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : input_tag_file
@INPUT      : filename
@OUTPUT     : n_volumes
              n_tag_points
              tags_volume1
              tags_volume2
              weights
              structure_ids
              patient_ids
              labels
@RETURNS    : OK or ERROR
@DESCRIPTION: Opens the file, inputs the tag points, and closes the file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  input_tag_file(
    STRING    filename,
    int       *n_volumes,
    int       *n_tag_points,
    Real      ***tags_volume1,
    Real      ***tags_volume2,
    Real      **weights,
    int       **structure_ids,
    int       **patient_ids,
    STRING    *labels[] )
{
    Status  status;
    FILE    *file;

    status = open_file_with_default_suffix( filename,
                                            get_default_tag_file_suffix(),
                                            READ_FILE, ASCII_FORMAT, &file );

    if( status == OK )
        status = input_tag_points( file, n_volumes, n_tag_points,
                                   tags_volume1, tags_volume2, weights,
                                   structure_ids, patient_ids, labels );

    if( status == OK )
        status = close_file( file );

    return( status );
}
