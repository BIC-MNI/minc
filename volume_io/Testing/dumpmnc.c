#include <volume_io.h>

main (int argc, char *argv[] )
{   
  Minc_file   file;
  Volume vol;
  
  Real vol_val, vox_val, fract;
  Real
    min,max,
    step[3];
  int
    i,
    sizes[3];

/*
  input_volume(argv[1],NULL,FALSE,&vol);
*/

  vol = create_volume( 3, (char **) NULL, NC_UNSPECIFIED, FALSE, 0.0, 0.0 );

  file = initialize_minc_input( argv[1], vol, (minc_input_options *) NULL );

  while( input_more_minc_file( file, &fract ) )
  {
  }

  (void) close_minc_input( file );
  
  get_volume_sizes(vol, sizes);
  get_volume_separations(vol, step);


  printf ("dim = %d\n",vol->n_dimensions);
  for_less(i,0,vol->n_dimensions) 
    printf ("dim[%d] = %s\n",i,vol->dimension_names[i]);
  printf ("datatype = %d\n",vol->data_type);
  printf ("nc_type  = %d\n",vol->nc_data_type);
  printf ("signed   = %s\n", vol->signed_flag ? "TRUE" : "FALSE");
  printf ("min val  = %f\n", get_volume_voxel_min(vol));
  printf ("max val  = %f\n", get_volume_voxel_max(vol));
  printf ("scale    = %f\n", vol->real_value_scale);
  printf ("translati= %f\n", vol->real_value_translation);
  printf ("sizes    = ");
  for_less(i,0,vol->n_dimensions) 
    printf ("%d ",sizes[i]);
  printf ("\n");
  printf ("steps    = ");
  for_less(i,0,vol->n_dimensions) 
    printf ("%f ",step[i]);
  printf ("\n");

  get_volume_real_range(vol, &min, &max);
  printf ("volume range  = %f %f\n", min,max);
  get_volume_voxel_range(vol, &min, &max);
  printf ("voxel range  = %f %f\n", min,max);

  vox_val = 0.0;
  vol_val = CONVERT_VOXEL_TO_VALUE(vol, vox_val);
  printf ("voxel %f -> value %f\n", vox_val, vol_val);

  vol_val = 0.0;
  vox_val = CONVERT_VALUE_TO_VOXEL(vol, vol_val);
  printf ("voxel %f <- value %f\n", vox_val, vol_val);


  GET_VOXEL_3D( vox_val, vol, 0, 0, 0 );
  vol_val = CONVERT_VOXEL_TO_VALUE(vol, vox_val);

  printf( "Voxel[0][0][0] = %g %g\n", vox_val, vol_val );

}
