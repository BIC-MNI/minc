#ifndef  DEF_TAG_IO
#define  DEF_TAG_IO

typedef enum { LINEAR6, LINEAR9, LINEAR12, BOOKSTEIN } Transform_types;

int  output_tag_points(
    FILE      *file,
    char      comments[],
    int       n_volumes,
    int       n_tag_points,
    double    **tags_volume1,
    double    **tags_volume2,
    char      **labels );

int  input_tag_points(
    FILE      *file,
    int       *n_volumes,
    int       *n_tag_points,
    double    ***tags_volume1,
    double    ***tags_volume2,
    char      ***labels );

void  free_tag_points(
    int       n_volumes,
    int       n_tag_points,
    double    **tags_volume1,
    double    **tags_volume2,
    char      **labels );

int  input_transform(
    FILE      *file,
    double    transform[3][4] );

int  output_transform(
    FILE      *file,
    char      comments[],
    double    transform[3][4] );

#endif
