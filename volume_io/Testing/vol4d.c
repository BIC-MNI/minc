
#include <mni.h>
#include <minc.h>
#include <malloc.h>


char *freq_dim_names[N_DIMENSIONS+1] = { MIzspace, MIyspace, MIxspace,
   MIvector_dimension };
/*  123456789012345678901234567890 */
char *space_dim_names[N_DIMENSIONS] = { MIzspace, MIyspace, MIxspace};
char     *prog_name;

main (int argc, char *argv[] )
{   
  
  char
    *history, *outfilename;
  
  Status 
    status;
  
  int 
    i,j,k,
    data_sizes[4];
  Real zero,
    separations[4];
  
  Volume data, tmp;

  mallopt(M_DEBUG,1);

  prog_name = argv[0];



  free( malloc (4) );

 status = input_volume(argv[1], 3, freq_dim_names, NC_UNSPECIFIED,
			FALSE, 0.0, 0.0, TRUE, &tmp, (minc_input_options *)NULL);

   if( status != OK )
       print( "Error reading %s\n", argv[1] );

   delete_volume( tmp );

  free( malloc (4) );



  for_less(i,0,4) {
    data_sizes[i] = 2;
    separations[i] = 10;		
  }
  separations[3] = 1;		

				/* define  new data volume  */

  free( malloc (4) );

  data = create_volume(4, freq_dim_names, NC_FLOAT, TRUE, 0.0,0.0);
  
  free( malloc (4) );

  set_volume_sizes(data, data_sizes);
  set_volume_separations(data, separations);

  free( malloc (4) );

  alloc_volume_data(data);	/* allocate space for data */

  free( malloc (4) );

				/* init volume */
  zero = CONVERT_VALUE_TO_VOXEL( data, 0.0 );
  for_less(i, 0, data_sizes[0])
    for_less(j, 0, data_sizes[1])
      for_less(k, 0, data_sizes[2]) {
	SET_VOXEL(data, i, j, k, 0, 0, zero); /* real */
	SET_VOXEL(data, i, j, k, 1, 0, zero); /* imag */
      }

  free( malloc (4) );


  outfilename = argv[2];
  status = output_modified_volume(outfilename, NC_UNSPECIFIED, FALSE, 0.0, 0.0,
			 data, argv[1], NULL, NULL);
  
  free( malloc (4) );
  
  if (status!=OK) 
    print("Problems saving <%s>.",__FILE__, __LINE__, outfilename);
  
  delete_volume(data);
  
  return(status);
  
}





