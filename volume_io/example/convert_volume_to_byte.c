#include  <volume_io.h>

int  main(
    int   argc,
    char  *argv[] )
{
    Volume     volume;
    Status     status;
    char       *input_filename, *output_filename, *history;

    if( argc < 3 )
    {
        print( "Need args.\n" );
        return( 1 );
    }

    input_filename = argv[1];
    output_filename = argv[2];

    status = input_volume( input_filename, 3, (char **) NULL,
                      NC_UNSPECIFIED, FALSE, 0.0, 0.0,
                      TRUE, &volume, (minc_input_options *) NULL ) ;

    if( status != OK )
        return( 1 );

    history = "Converted volume to byte";

    status = output_volume( output_filename, NC_BYTE, FALSE, 0.0, 255.0,
                            volume, history, (minc_output_options *) NULL );

    return( 0 );
}
