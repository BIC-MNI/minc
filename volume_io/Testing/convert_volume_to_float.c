#include  <volume_io.h>

int  main(
    int   argc,
    char  *argv[] )
{
    Volume     volume;
    char       *input_filename, *output_filename, *history;

    if( argc < 3 )
    {
        print( "Usage: %s  input_volume output_volume\n",
               argv[0] );
        return( 1 );
    }

    input_filename = argv[1];
    output_filename = argv[2];

    if( input_volume( input_filename, 3, (char **) NULL,
                      NC_UNSPECIFIED, FALSE, 0.0, 0.0,
                      TRUE, &volume, (minc_input_options *) NULL ) != OK )
        return( 1 );

    history = "Converted volume to float";

    if( output_volume( output_filename, NC_FLOAT, FALSE, 0.0, 0.0,
                       volume, history, (minc_output_options *) NULL ) != OK )
        return( 1 );

    return( 0 );
}
