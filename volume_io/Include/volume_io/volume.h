#ifndef  DEF_VOLUME
#define  DEF_VOLUME

#include  <def_mni.h>

typedef  unsigned  char  Volume_type;

typedef  struct
{
    Volume_type   ***data;
    Real          value_scale;
    Real          value_translation;
    int           sizes[3];
    Real          thickness[3];
    Transform     world_transform;
} volume_struct;

#endif
