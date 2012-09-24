#include <stdio.h>
#include "minc2.h"

#define TESTRPT(msg, val) (error_cnt++, fprintf(stderr, \
                                  "Error reported on line #%d, %s: %d\n", \
                                  __LINE__, msg, val))

static int error_cnt = 0;


int main(int argc, char **argv)
{
  mihandle_t vol;
  mivolumeprops_t  props;
  int r;
  micompression_t compression_type;
  miboolean_t enable_flag;
  int zlib_level;
  int depth;
  int edge_lengths[MI2_MAX_VAR_DIMS];
  int edge_count;
  int i;

  r = minew_volume_props(&props);

  if (r < 0) {
    TESTRPT("failed", r);
  }
  
  r = miset_props_multi_resolution(props, 1 , 2);

  if (r < 0) {
    TESTRPT("failed", r);
   }

  r = miget_props_multi_resolution(props, &enable_flag, &depth);
  if (r < 0) {
    TESTRPT("failed", r);
  }
  else {
    printf("Multiresolution enabled: %d depth: %d \n", enable_flag, depth);
  }

  r = miset_props_compression_type(props, MI_COMPRESS_NONE);
  if (r < 0) {
    TESTRPT("failed", r);
  }
  else {
      printf("Set compression type to %d\n", MI_COMPRESS_NONE);
  }

  r = miget_props_compression_type(props,&compression_type);
  if (r < 0 || compression_type != MI_COMPRESS_NONE) {
    TESTRPT("failed", r);
  }
  else {
    printf("Got compression type %d \n", compression_type);
 }  

  r = miset_props_zlib_compression(props,4);
  if (r < 0) {
    TESTRPT("failed", r);
  }
  else {
    printf("Set zlib level to %d\n", 4);
  }

  r = miget_props_zlib_compression(props,&zlib_level);
  if (r < 0 || zlib_level != 4) {
    TESTRPT("failed", r);
  }
  else {
    printf("Got zlib level %d \n", zlib_level);
  }

  mifree_volume_props(props);

  while (--argc > 0) {
      r = miopen_volume(*++argv, MI2_OPEN_RDWR, &vol);
      if (r < 0) {
	  TESTRPT("failed", r);
      }
      r = miget_volume_props(vol, &props);
      if (r < 0) {
	  TESTRPT("failed", r);
      }
      r = miget_props_blocking(props, &edge_count, edge_lengths, 
			       MI2_MAX_VAR_DIMS);
      if (r < 0) {
	  TESTRPT("failed", r);
      }
      printf("edge_count %d\n", edge_count);
      for (i = 0; i < edge_count; i++) {
	  printf("  %d", edge_lengths[i]);
      }
      printf("\n");
      mifree_volume_props(props);
      miclose_volume(vol);
  }

 if (error_cnt != 0) {
    fprintf(stderr, "%d error%s reported\n", 
	    error_cnt, (error_cnt == 1) ? "" : "s");
  }
  else {
    fprintf(stderr, "No errors\n");
  }
  return (error_cnt);
}

