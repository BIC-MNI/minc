#include  <minc.h>
#include  <def_mni.h>

int main(
    int  argc,
    char *argv[] )
{
    int                 img_var, dim_vars[MAX_VAR_DIMS];
    int                 min_dim_vars[MAX_VAR_DIMS], max_dim_vars[MAX_VAR_DIMS];
    int                 icv, cdfid, n_file_dimensions;
    long                long_size, mindex[MAX_VAR_DIMS];
    BOOLEAN             converted_sign;
    nc_type             converted_type;
    STRING              dim_names[MAX_VAR_DIMS];
    nc_type             file_datatype, min_datatype, max_datatype;
    int                 n_min_dimensions, n_max_dimensions;
    double              separation[MAX_VAR_DIMS];
    double              start_position[MAX_VAR_DIMS];
    double              dir_cosines[MAX_VAR_DIMS][MI_NUM_SPACE_DIMS];
    double              min_value, max_value, real_min, real_max;
    int                 d, dimvar, min_var, max_var;
    STRING              min_name, max_name;                
    char                *filename = "/nil/david/mr_data/indiv_mr.mnc";

    if( argc > 1 )
        filename = argv[1];

    ncopts = NC_VERBOSE;
    cdfid =  ncopen( filename, NC_NOWRITE );

    if( cdfid == MI_ERROR )
    {
        print( "Error: opening MINC file \"%s\".\n", filename );
        return( 1 );
    }

    img_var = ncvarid( cdfid, MIimage );

    ncvarinq( cdfid, img_var, (char *) NULL, &file_datatype,
              &n_file_dimensions, dim_vars, (int *) NULL );

    for_less( d, 0, n_file_dimensions )
    {
        (void) ncdiminq( cdfid, dim_vars[d], dim_names[d], &long_size );
    }

    for_less( d, 0, n_file_dimensions )
    {
        dimvar = ncvarid( cdfid, dim_names[d] );
        if( dimvar != MI_ERROR )
        {
            (void) miattget1( cdfid, dimvar, MIstep, NC_DOUBLE,
                              (void *) (&separation[d]) );

             (void) miattget1( cdfid, dimvar, MIstart, NC_DOUBLE,
                               (void *) (&start_position[d]) );
/*
             (void) miattget( cdfid, dimvar, MIdirection_cosines,
                              NC_DOUBLE, MI_NUM_SPACE_DIMS,
                              (void *) (dir_cosines[d]), (int *) NULL );
*/
        }
    }

    min_var = ncvarid( cdfid, MIimagemin );
    max_var = ncvarid( cdfid, MIimagemax );

    print( "%d %d\n", min_var, max_var );

    ncvarinq( cdfid, min_var, (char *) NULL, &min_datatype,
              &n_min_dimensions, min_dim_vars, (int *) NULL );
    ncvarinq( cdfid, max_var, (char *) NULL, &max_datatype,
              &n_max_dimensions, max_dim_vars, (int *) NULL );

    print( "%d %d %d %d %d\n", min_datatype, n_min_dimensions,
           min_dim_vars[0], min_dim_vars[1], min_dim_vars[2] );
    print( "%d %d %d %d %d\n", max_datatype, n_max_dimensions,
           max_dim_vars[0], max_dim_vars[1], max_dim_vars[2] );

    mindex[0] = 0;
    mindex[1] = 0;
    mindex[2] = 0;

    (void) mivarget1( cdfid, min_var, mindex, NC_DOUBLE, MI_SIGNED,
                      (void *) (&real_min) );
    (void) mivarget1( cdfid, max_var, mindex, NC_DOUBLE, MI_SIGNED,
                      (void *) (&real_max) );

    print( "%g %g\n", real_min, real_max );
    

    converted_type = NC_SHORT;
    converted_sign = FALSE;

    icv = miicv_create();

    (void) miicv_setint( icv, MI_ICV_TYPE, converted_type );
    (void) miicv_setstr( icv, MI_ICV_SIGN,
                         converted_sign ? MI_SIGNED : MI_UNSIGNED );
    (void) miicv_setint( icv, MI_ICV_DO_NORM, TRUE );
    (void) miicv_attach( icv, cdfid, img_var );
    (void) miicv_inqdbl( icv, MI_ICV_VALID_MIN, &min_value );
    (void) miicv_inqdbl( icv, MI_ICV_VALID_MAX, &max_value );

/*
    for_less( d, 0, n_file_dimensions )
        mindex[d] = 0;
{
    char    att[100];
    (void) ncattget( cdfid, img_var, MIimagemin, (void *) (att) );
    (void) ncvarget1( cdfid, ncvarid(cdfid,att), mindex, (void *) (&real_min) );
    (void) ncattget( cdfid, img_var, MIimagemax, (void *) (att) );
    (void) ncvarget1( cdfid, ncvarid(cdfid,att), mindex, (void *) (&real_max) );
print( "%g %g\n", real_min, real_max );
}

    (void) ncattget( cdfid, img_var, MIimagemin, (void *) (&min_var_id) );
    (void) ncattget( cdfid, img_var, MIimagemax, (void *) (&max_var_id) );


    if( mivarget1( cdfid, min_var_id, mindex, NC_DOUBLE, MI_SIGNED,
                      (void *) (&real_min) ) == MI_ERROR )
        real_min = 0.0;
    if( mivarget1( cdfid, max_var_id, mindex, NC_DOUBLE, MI_SIGNED,
                      (void *) (&real_max) ) )
        real_max = 1.0;
*/

    print( "%g %g\n", min_value, max_value );
    print( "%g %g\n", real_min, real_max );

    ncopts = NC_VERBOSE | NC_FATAL;

    return( 0 );
}
