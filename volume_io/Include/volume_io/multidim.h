#ifndef  DEF_MULTI_DIM
#define  DEF_MULTI_DIM

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

#ifndef lint
static char multidim_rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Include/volume_io/multidim.h,v 1.1 1995-11-17 21:01:08 david Exp $";
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : multidim.h
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Multidimensional variable type arrays.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Aug. 14, 1995   David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  MAX_DIMENSIONS     5

/* -------------------------- Data_types ------------------------- */

typedef  enum  { NO_DATA_TYPE,
                 UNSIGNED_BYTE,
                 SIGNED_BYTE,
                 UNSIGNED_SHORT,
                 SIGNED_SHORT,
                 UNSIGNED_LONG,
                 SIGNED_LONG,
                 FLOAT,
                 DOUBLE,
                 MAX_DATA_TYPE }   Data_types;

typedef  struct
{
    int                     n_dimensions;
    int                     sizes[MAX_DIMENSIONS];
    Data_types              data_type;
    void                    *data;
} multidim_array;

/* ------------------------- set value ---------------------------- */

/* --- private macros */

#define  SET_ONE( array, type, asterisks, subscripts, value )   \
         (((type asterisks) ((array).data))  subscripts = (type) (value))

#define  SET_GIVEN_DIM( array, asterisks, subscripts, value )   \
         switch( (array).data_type )  \
         {  \
         case UNSIGNED_BYTE:  \
             SET_ONE( array, unsigned char, asterisks, subscripts, value);\
             break;  \
         case SIGNED_BYTE:  \
             SET_ONE( array, signed char, asterisks, subscripts, value);\
             break;  \
         case UNSIGNED_SHORT:  \
             SET_ONE( array, unsigned short, asterisks, subscripts, value);\
             break;  \
         case SIGNED_SHORT:  \
             SET_ONE( array, signed short, asterisks, subscripts, value);\
             break;  \
         case UNSIGNED_LONG:  \
             SET_ONE( array, unsigned long, asterisks, subscripts, value);\
             break;  \
         case SIGNED_LONG:  \
             SET_ONE( array, signed long, asterisks, subscripts, value);\
             break;  \
         case FLOAT:  \
             SET_ONE( array, float, asterisks, subscripts, value);\
             break;  \
         case DOUBLE:  \
             SET_ONE( array, double, asterisks, subscripts, value);\
             break;  \
         }

/* --- public macros to set the [x][y]... voxel of 'array' to 'value' */

#define  SET_MULTIDIM_1D( array, x, value )       \
           SET_GIVEN_DIM( array, *, [x], value )

#define  SET_MULTIDIM_2D( array, x, y, value )       \
           SET_GIVEN_DIM( array, **, [x][y], value )

#define  SET_MULTIDIM_3D( array, x, y, z, value )       \
           SET_GIVEN_DIM( array, ***, [x][y][z], value )

#define  SET_MULTIDIM_4D( array, x, y, z, t, value )       \
           SET_GIVEN_DIM( array, ****, [x][y][z][t], value )

#define  SET_MULTIDIM_5D( array, x, y, z, t, v, value )       \
           SET_GIVEN_DIM( array, *****, [x][y][z][t][v], value )

/* --- same as previous, but don't have to know dimensions of volume */

#define  SET_MULTIDIM( array, x, y, z, t, v, value )       \
         switch( (array).n_dimensions ) \
         { \
         case 1:  SET_MULTIDIM_1D( array, x, value );              break; \
         case 2:  SET_MULTIDIM_2D( array, x, y, value );           break; \
         case 3:  SET_MULTIDIM_3D( array, x, y, z, value );        break; \
         case 4:  SET_MULTIDIM_4D( array, x, y, z, t, value );     break; \
         case 5:  SET_MULTIDIM_5D( array, x, y, z, t, v, value );  break; \
         }

/* ------------------------- get multidim value ------------------------ */

/* --- private macros */

#define  GET_ONE( value, array, type, asterisks, subscripts )   \
         (value) = (((type asterisks) ((array).data))  subscripts)

#define  GET_GIVEN_DIM( value, array, asterisks, subscripts )   \
         switch( (array).data_type )  \
         {  \
         case UNSIGNED_BYTE:  \
             GET_ONE( value, array, unsigned char, asterisks, subscripts );\
             break;  \
         case SIGNED_BYTE:  \
             GET_ONE( value, array, signed char, asterisks, subscripts );\
             break;  \
         case UNSIGNED_SHORT:  \
             GET_ONE( value, array, unsigned short, asterisks, subscripts );\
             break;  \
         case SIGNED_SHORT:  \
             GET_ONE( value, array, signed short, asterisks, subscripts );\
             break;  \
         case UNSIGNED_LONG:  \
             GET_ONE( value, array, unsigned long, asterisks, subscripts );\
             break;  \
         case SIGNED_LONG:  \
             GET_ONE( value, array, signed long, asterisks, subscripts );\
             break;  \
         case FLOAT:  \
             GET_ONE( value, array, float, asterisks, subscripts );\
             break;  \
         case DOUBLE:  \
             GET_ONE( value, array, double, asterisks, subscripts );\
             break;  \
         }

/* --- public macros to place the [x][y]...'th voxel of 'array' in 'value' */

#define  GET_MULTIDIM_1D( value, array, x )       \
           GET_GIVEN_DIM( value, array, *, [x] )

#define  GET_MULTIDIM_2D( value, array, x, y )       \
           GET_GIVEN_DIM( value, array, **, [x][y] )

#define  GET_MULTIDIM_3D( value, array, x, y, z )       \
           GET_GIVEN_DIM( value, array, ***, [x][y][z] )

#define  GET_MULTIDIM_4D( value, array, x, y, z, t )       \
           GET_GIVEN_DIM( value, array, ****, [x][y][z][t] )

#define  GET_MULTIDIM_5D( value, array, x, y, z, t, v )       \
           GET_GIVEN_DIM( value, array, *****, [x][y][z][t][v] )

/* --- same as previous, but no need to know array dimensions */

#define  GET_MULTIDIM( value, array, x, y, z, t, v )       \
         switch( (array).n_dimensions ) \
         { \
         case 1:  GET_MULTIDIM_1D( value, array, x );              break; \
         case 2:  GET_MULTIDIM_2D( value, array, x, y );           break; \
         case 3:  GET_MULTIDIM_3D( value, array, x, y, z );        break; \
         case 4:  GET_MULTIDIM_4D( value, array, x, y, z, t );     break; \
         case 5:  GET_MULTIDIM_5D( value, array, x, y, z, t, v );  break; \
         }

/* ------------------------- get multidim ptr ------------------------ */

/* --- private macros */

#define  GET_ONE_PTR( ptr, array, type, asterisks, subscripts )   \
         (ptr) = (void *) (&(((type asterisks) ((array).data))  subscripts))

#define  GET_GIVEN_DIM_PTR( ptr, array, asterisks, subscripts )   \
         switch( (array).data_type )  \
         {  \
         case UNSIGNED_BYTE:  \
             GET_ONE_PTR( ptr, array, unsigned char, asterisks, subscripts );\
             break;  \
         case SIGNED_BYTE:  \
             GET_ONE_PTR( ptr, array, signed char, asterisks, subscripts );\
             break;  \
         case UNSIGNED_SHORT:  \
             GET_ONE_PTR( ptr, array, unsigned short, asterisks, subscripts );\
             break;  \
         case SIGNED_SHORT:  \
             GET_ONE_PTR( ptr, array, signed short, asterisks, subscripts );\
             break;  \
         case UNSIGNED_LONG:  \
             GET_ONE_PTR( ptr, array, unsigned long, asterisks, subscripts );\
             break;  \
         case SIGNED_LONG:  \
             GET_ONE_PTR( ptr, array, signed long, asterisks, subscripts );\
             break;  \
         case FLOAT:  \
             GET_ONE_PTR( ptr, array, float, asterisks, subscripts );\
             break;  \
         case DOUBLE:  \
             GET_ONE_PTR( ptr, array, double, asterisks, subscripts );\
             break;  \
         }

/* --- public macros to return a pointer to the [x][y]'th voxel of the
       'array', and place it in 'ptr' */

#define  GET_MULTIDIM_PTR_1D( ptr, array, x )       \
           GET_GIVEN_DIM_PTR( ptr, array, *, [x] )

#define  GET_MULTIDIM_PTR_2D( ptr, array, x, y )       \
           GET_GIVEN_DIM_PTR( ptr, array, **, [x][y] )

#define  GET_MULTIDIM_PTR_3D( ptr, array, x, y, z )       \
           GET_GIVEN_DIM_PTR( ptr, array, ***, [x][y][z] )

#define  GET_MULTIDIM_PTR_4D( ptr, array, x, y, z, t )       \
           GET_GIVEN_DIM_PTR( ptr, array, ****, [x][y][z][t] )

#define  GET_MULTIDIM_PTR_5D( ptr, array, x, y, z, t, v )       \
           GET_GIVEN_DIM_PTR( ptr, array, *****, [x][y][z][t][v] )

/* --- same as previous, but no need to know array dimensions */

#define  GET_MULTIDIM_PTR( ptr, array, x, y, z, t, v )       \
         switch( (array).n_dimensions ) \
         { \
         case 1:  GET_MULTIDIM_PTR_1D( ptr, array, x );              break; \
         case 2:  GET_MULTIDIM_PTR_2D( ptr, array, x, y );           break; \
         case 3:  GET_MULTIDIM_PTR_3D( ptr, array, x, y, z );        break; \
         case 4:  GET_MULTIDIM_PTR_4D( ptr, array, x, y, z, t );     break; \
         case 5:  GET_MULTIDIM_PTR_5D( ptr, array, x, y, z, t, v );  break; \
         }

#endif
