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
static char rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Volumes/evaluate.c,v 1.25 1995-11-20 12:37:58 david Exp $";
#endif

/*--- invalid get voxel functions */

#define  INVALID_GET_FUNC_BODY \
    { \
        print_error( "Invalid get voxel function.\n" ); \
        return( 0 ); \
    }

/* ARGSUSED */

private  Real  invalid_get_voxel_1d(
    Volume  volume,
    int     v0 )
{
    INVALID_GET_FUNC_BODY
}

/* ARGSUSED */

private  Real  invalid_get_voxel_2d(
    Volume  volume,
    int     v0,
    int     v1 )
{
    INVALID_GET_FUNC_BODY
}

/* ARGSUSED */

private  Real  invalid_get_voxel_3d(
    Volume  volume,
    int     v0,
    int     v1,
    int     v2 )
{
    INVALID_GET_FUNC_BODY
}

/* ARGSUSED */

private  Real  invalid_get_voxel_4d(
    Volume  volume,
    int     v0,
    int     v1,
    int     v2,
    int     v3 )
{
    INVALID_GET_FUNC_BODY
}

/* ARGSUSED */

private  Real  invalid_get_voxel_5d(
    Volume  volume,
    int     v0,
    int     v1,
    int     v2,
    int     v3,
    int     v4 )
{
    INVALID_GET_FUNC_BODY
}

/*--- invalid set voxel functions */

#define  INVALID_SET_FUNC_BODY \
    { \
        print_error( "Invalid set voxel function.\n" ); \
    }

/* ARGSUSED */

private  void  invalid_set_voxel_1d(
    Volume  volume,
    int     v0,
    Real    value )
{
    INVALID_SET_FUNC_BODY
}

/* ARGSUSED */

private  void  invalid_set_voxel_2d(
    Volume  volume,
    int     v0,
    int     v1,
    Real    value )
{
    INVALID_SET_FUNC_BODY
}

/* ARGSUSED */

private  void  invalid_set_voxel_3d(
    Volume  volume,
    int     v0,
    int     v1,
    int     v2,
    Real    value )
{
    INVALID_SET_FUNC_BODY
}

/* ARGSUSED */

private  void  invalid_set_voxel_4d(
    Volume  volume,
    int     v0,
    int     v1,
    int     v2,
    int     v3,
    Real    value )
{
    INVALID_SET_FUNC_BODY
}

/* ARGSUSED */

private  void  invalid_set_voxel_5d(
    Volume  volume,
    int     v0,
    int     v1,
    int     v2,
    int     v3,
    int     v4,
    Real    value )
{
    INVALID_SET_FUNC_BODY
}

private  void  initialize_volume_functions(
    Volume   volume )
{
    volume->get_1d = invalid_get_voxel_1d;
    volume->get_2d = invalid_get_voxel_2d;
    volume->get_3d = invalid_get_voxel_3d;
    volume->get_4d = invalid_get_voxel_4d;
    volume->get_5d = invalid_get_voxel_5d;
    volume->get = invalid_get_voxel_5d;

    volume->set_1d = invalid_set_voxel_1d;
    volume->set_2d = invalid_set_voxel_2d;
    volume->set_3d = invalid_set_voxel_3d;
    volume->set_4d = invalid_set_voxel_4d;
    volume->set_5d = invalid_set_voxel_5d;
    volume->set = invalid_set_voxel_5d;
}

#define  FUNCS_1D( type_name, type ) \
    private   Real  GLUE3(get_,type_name,_1d)( \
        Volume  volume, \
        int     v0 ) \
    { \
        return( GET_MULTIDIM_TYPE_1D( (volume)->array, type, v0 ) ); \
    } \
    private   void  GLUE3(set_,type_name,_1d)( \
        Volume  volume, \
        int     v0, \
        Real    value ) \
    { \
        SET_MULTIDIM_TYPE_1D( (volume)->array, type, v0, value ); \
    } \
    /* ARGSUSED */ \
    private   Real  GLUE3(get_,type_name,_1d_5)( \
        Volume  volume, \
        int     v0, \
        int     v1, \
        int     v2, \
        int     v3, \
        int     v4 ) \
    { \
        return( GET_MULTIDIM_TYPE_1D( (volume)->array, type, v0 ) ); \
    } \
    /* ARGSUSED */ \
    private   void  GLUE3(set_,type_name,_1d_5)( \
        Volume  volume, \
        int     v0, \
        int     v1, \
        int     v2, \
        int     v3, \
        int     v4, \
        Real    value ) \
    { \
        SET_MULTIDIM_TYPE_1D( (volume)->array, type, v0, value ); \
    }

#define  FUNCS_2D( type_name, type ) \
    private   Real  GLUE3(get_,type_name,_2d)( \
        Volume  volume, \
        int     v0, \
        int     v1 ) \
    { \
        return( GET_MULTIDIM_TYPE_2D( (volume)->array, type, v0, v1 ) ); \
    } \
    private   void  GLUE3(set_,type_name,_2d)( \
        Volume  volume, \
        int     v0, \
        int     v1, \
        Real    value ) \
    { \
        SET_MULTIDIM_TYPE_2D( (volume)->array, type, v0, v1, value ); \
    } \
    /* ARGSUSED */ \
    private   Real  GLUE3(get_,type_name,_2d_5)( \
        Volume  volume, \
        int     v0, \
        int     v1, \
        int     v2, \
        int     v3, \
        int     v4 ) \
    { \
        return( GET_MULTIDIM_TYPE_2D( (volume)->array, type, v0,v1 ) ); \
    } \
    /* ARGSUSED */ \
    private   void  GLUE3(set_,type_name,_2d_5)( \
        Volume  volume, \
        int     v0, \
        int     v1, \
        int     v2, \
        int     v3, \
        int     v4, \
        Real    value ) \
    { \
        SET_MULTIDIM_TYPE_2D( (volume)->array, type, v0,v1, value ); \
    }


#define  FUNCS_3D( type_name, type ) \
    private   Real  GLUE3(get_,type_name,_3d)( \
        Volume  volume, \
        int     v0, \
        int     v1, \
        int     v2 ) \
    { \
        return( GET_MULTIDIM_TYPE_3D( (volume)->array, type, v0, v1, v2 ) ); \
    } \
    private   void  GLUE3(set_,type_name,_3d)( \
        Volume  volume, \
        int     v0, \
        int     v1, \
        int     v2, \
        Real    value ) \
    { \
        SET_MULTIDIM_TYPE_3D( (volume)->array, type, v0, v1, v2, value ); \
    } \
    /* ARGSUSED */ \
    private   Real  GLUE3(get_,type_name,_3d_5)( \
        Volume  volume, \
        int     v0, \
        int     v1, \
        int     v2, \
        int     v3, \
        int     v4 ) \
    { \
        return( GET_MULTIDIM_TYPE_3D( (volume)->array, type, v0,v1,v2 ) ); \
    } \
    /* ARGSUSED */ \
    private   void  GLUE3(set_,type_name,_3d_5)( \
        Volume  volume, \
        int     v0, \
        int     v1, \
        int     v2, \
        int     v3, \
        int     v4, \
        Real    value ) \
    { \
        SET_MULTIDIM_TYPE_3D( (volume)->array, type, v0,v1,v2, value ); \
    }


#define  FUNCS_4D( type_name, type ) \
    private   Real  GLUE3(get_,type_name,_4d)( \
        Volume  volume, \
        int     v0, \
        int     v1, \
        int     v2, \
        int     v3 ) \
    { \
        return( GET_MULTIDIM_TYPE_4D( (volume)->array, type, v0,v1,v2,v3 ) ); \
    } \
    private   void  GLUE3(set_,type_name,_4d)( \
        Volume  volume, \
        int     v0, \
        int     v1, \
        int     v2, \
        int     v3, \
        Real    value ) \
    { \
        SET_MULTIDIM_TYPE_4D( (volume)->array, type, v0, v1, v2, v3, value );\
    } \
    /* ARGSUSED */ \
    private   Real  GLUE3(get_,type_name,_4d_5)( \
        Volume  volume, \
        int     v0, \
        int     v1, \
        int     v2, \
        int     v3, \
        int     v4 ) \
    { \
        return( GET_MULTIDIM_TYPE_4D( (volume)->array, type, v0,v1,v2,v3 ) ); \
    } \
    /* ARGSUSED */ \
    private   void  GLUE3(set_,type_name,_4d_5)( \
        Volume  volume, \
        int     v0, \
        int     v1, \
        int     v2, \
        int     v3, \
        int     v4, \
        Real    value ) \
    { \
        SET_MULTIDIM_TYPE_4D( (volume)->array, type, v0,v1,v2,v3, value ); \
    }


#define  FUNCS_5D( type_name, type ) \
    private   Real  GLUE3(get_,type_name,_5d)( \
        Volume  volume, \
        int     v0, \
        int     v1, \
        int     v2, \
        int     v3, \
        int     v4 ) \
    { \
        return( GET_MULTIDIM_TYPE_5D( (volume)->array, type, v0,v1,v2,v3,v4 ));\
    } \
    private   void  GLUE3(set_,type_name,_5d)( \
        Volume  volume, \
        int     v0, \
        int     v1, \
        int     v2, \
        int     v3, \
        int     v4, \
        Real    value ) \
    { \
        SET_MULTIDIM_TYPE_5D( (volume)->array, type, v0,v1,v2,v3,v4, value );\
    }

#define FUNCS_TYPE( type_name, type ) \
     FUNCS_1D( type_name, type ) \
     FUNCS_2D( type_name, type ) \
     FUNCS_3D( type_name, type ) \
     FUNCS_4D( type_name, type ) \
     FUNCS_5D( type_name, type )

FUNCS_TYPE( unsigned_byte, unsigned char )
FUNCS_TYPE( signed_byte, signed char )
FUNCS_TYPE( unsigned_short, unsigned short )
FUNCS_TYPE( signed_short, signed short )
FUNCS_TYPE( unsigned_long, unsigned long )
FUNCS_TYPE( signed_long, signed long )
FUNCS_TYPE( float, float )
FUNCS_TYPE( double, double )

#define  INSTALL_FUNCTIONS( volume, type_name, n_dims ) \
     { \
         switch( n_dims ) \
         { \
         case 1: \
             (volume)->get_1d = GLUE3(get_,type_name,_1d); \
             (volume)->get = GLUE3(get_,type_name,_1d_5); \
             (volume)->set_1d = GLUE3(set_,type_name,_1d); \
             (volume)->set = GLUE3(set_,type_name,_1d_5); \
             break; \
         case 2: \
             (volume)->get_2d = GLUE3(get_,type_name,_2d); \
             (volume)->get = GLUE3(get_,type_name,_2d_5); \
             (volume)->set_2d = GLUE3(set_,type_name,_2d); \
             (volume)->set = GLUE3(set_,type_name,_2d_5); \
             break; \
         case 3: \
             (volume)->get_3d = GLUE3(get_,type_name,_3d); \
             (volume)->get = GLUE3(get_,type_name,_3d_5); \
             (volume)->set_3d = GLUE3(set_,type_name,_3d); \
             (volume)->set = GLUE3(set_,type_name,_3d_5); \
             break; \
         case 4: \
             (volume)->get_4d = GLUE3(get_,type_name,_4d); \
             (volume)->get = GLUE3(get_,type_name,_4d_5); \
             (volume)->set_4d = GLUE3(set_,type_name,_4d); \
             (volume)->set = GLUE3(set_,type_name,_4d_5); \
             break; \
         case 5: \
             (volume)->get_5d = GLUE3(get_,type_name,_5d); \
             (volume)->get = GLUE3(get_,type_name,_5d); \
             (volume)->set_5d = GLUE3(set_,type_name,_5d); \
             (volume)->set = GLUE3(set_,type_name,_5d); \
             break; \
         } \
     }

private  void   set_volume_array_functions(
    Volume   volume )
{
    int   n_dims;

    n_dims = get_volume_n_dimensions( volume );

    switch( get_volume_data_type(volume) )
    {
    case UNSIGNED_BYTE:
        INSTALL_FUNCTIONS( volume, unsigned_byte, n_dims )
        break;
    case SIGNED_BYTE:
        INSTALL_FUNCTIONS( volume, signed_byte, n_dims )
        break;
    case UNSIGNED_SHORT:
        INSTALL_FUNCTIONS( volume, unsigned_short, n_dims )
        break;
    case SIGNED_SHORT:
        INSTALL_FUNCTIONS( volume, signed_short, n_dims )
        break;
    case UNSIGNED_LONG:
        INSTALL_FUNCTIONS( volume, unsigned_long, n_dims )
        break;
    case SIGNED_LONG:
        INSTALL_FUNCTIONS( volume, signed_long, n_dims )
        break;
    case FLOAT:
        INSTALL_FUNCTIONS( volume, float, n_dims )
        break;
    case DOUBLE:
        INSTALL_FUNCTIONS( volume, double, n_dims )
        break;
    }
}

public  void   set_volume_functions(
    Volume   volume )
{
    initialize_volume_functions( volume );

    if( !volume_is_alloced( volume ) )
        return;

    if( !volume->is_cached_volume )
        set_volume_array_functions( volume );
    else
        set_volume_cache_functions( volume );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_voxel_to_value
@INPUT      : volume
              voxel
@OUTPUT     : 
@RETURNS    : real value
@DESCRIPTION: Converts a voxel value to a real value.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Sep. 1, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Real  convert_voxel_to_value(
    Volume   volume,
    Real     voxel )
{
    return( CONVERT_VOXEL_TO_VALUE( volume, voxel ) );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_value_to_voxel
@INPUT      : volume
              value
@OUTPUT     : 
@RETURNS    : voxel value
@DESCRIPTION: Converts a real value to a voxel value.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Sep. 1, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Real  convert_value_to_voxel(
    Volume   volume,
    Real     value )
{
    return( CONVERT_VALUE_TO_VOXEL( volume, value ) );
}
