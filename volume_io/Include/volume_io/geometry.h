#ifndef  DEF_GEOMETRY
#define  DEF_GEOMETRY

#include  <def_mni.h>
#include  <def_geom_structs.h>
#include  <def_math.h>

/* point and vector operations */

#define  POINT_SCALAR_EXP( result, v, op, scalar ) \
         { \
           Point_x(result) = Point_x(v) op (scalar); \
           Point_y(result) = Point_y(v) op (scalar); \
           Point_z(result) = Point_z(v) op (scalar); \
         }

#define  POINT_EXP2( result, v1, op, v2 ) \
         { \
           Point_x(result) = Point_x(v1) op Point_x(v2); \
           Point_y(result) = Point_y(v1) op Point_y(v2); \
           Point_z(result) = Point_z(v1) op Point_z(v2); \
         }

#define  POINT_EXP3( result, v1, op1, v2, op2, v3 ) \
         { \
           Point_x(result) = Point_x(v1) op1 Point_x(v2) op2 Point_x(v3); \
           Point_y(result) = Point_y(v1) op1 Point_y(v2) op2 Point_y(v3); \
           Point_z(result) = Point_z(v1) op1 Point_z(v2) op2 Point_z(v3); \
         }

#define  POINT_VECTOR_EXP2( result, p, op, v ) \
         { \
           Point_x(result) = Point_x(p) op Vector_x(v); \
           Point_y(result) = Point_y(p) op Vector_y(v); \
           Point_z(result) = Point_z(p) op Vector_z(v); \
         }

#define  VECTOR_SCALAR_EXP( result, v, op, scalar ) \
         { \
           Vector_x(result) = Vector_x(v) op (scalar); \
           Vector_y(result) = Vector_y(v) op (scalar); \
           Vector_z(result) = Vector_z(v) op (scalar); \
         }

#define  VECTOR_EXP2( result, v1, op, v2 ) \
         { \
           Vector_x(result) = Vector_x(v1) op Vector_x(v2); \
           Vector_y(result) = Vector_y(v1) op Vector_y(v2); \
           Vector_z(result) = Vector_z(v1) op Vector_z(v2); \
         }

#define  VECTOR_EXP3( result, v1, op1, v2, op2, v3 ) \
         { \
           Vector_x(result) = Vector_x(v1) op1 Vector_x(v2) op2 Vector_x(v3); \
           Vector_y(result) = Vector_y(v1) op1 Vector_y(v2) op2 Vector_y(v3); \
           Vector_z(result) = Vector_z(v1) op1 Vector_z(v2) op2 Vector_z(v3); \
         }

#define  INTERPOLATE_POINTS( interp, p1, p2, alpha ) \
         { \
           Point_x(interp) = (1.0-(alpha))*Point_x(p1) + (alpha)*Point_x(p2);\
           Point_y(interp) = (1.0-(alpha))*Point_y(p1) + (alpha)*Point_y(p2);\
           Point_z(interp) = (1.0-(alpha))*Point_z(p1) + (alpha)*Point_z(p2);\
         }

#define  INTERPOLATE_VECTORS( interp, v1, v2, alpha ) \
         { \
           Vector_x(interp)= (1.0-(alpha))*Vector_x(v1) + (alpha)*Vector_x(v2);\
           Vector_y(interp)= (1.0-(alpha))*Vector_y(v1) + (alpha)*Vector_y(v2);\
           Vector_z(interp)= (1.0-(alpha))*Vector_z(v1) + (alpha)*Vector_z(v2);\
         }

#define  GET_POINT_ON_RAY( point, origin, direction, distance ) \
         { \
           Vector_x(point)= Point_x(origin) + (distance)*Vector_x(direction);\
           Vector_y(point)= Point_y(origin) + (distance)*Vector_y(direction);\
           Vector_z(point)= Point_z(origin) + (distance)*Vector_z(direction);\
         }

#define  ADD_POINTS( sum, p1, p2 )           POINT_EXP2( sum, p1, +, p2 )
#define  ADD_VECTORS( sum, v1, v2 )          VECTOR_EXP2( sum, v1, +, v2 )
#define  ADD_POINT_VECTOR( sum, p, v )       POINT_VECTOR_EXP2( sum, p, +, v )
#define  SUB_POINTS( diff, p1, p2 )          POINT_EXP2( diff, p1, -, p2 )
#define  SUB_VECTORS( diff, v1, v2 )         VECTOR_EXP2( diff, v1, -, v2 )
#define  SUB_POINT_VECTOR( diff, p, v )      POINT_VECTOR_EXP2( diff, p, -, v )

#define  DOT_POINTS( p1, p2 ) \
            ( Point_x(p1)*Point_x(p2) + \
              Point_y(p1)*Point_y(p2) + \
              Point_z(p1)*Point_z(p2) )

#define  DOT_VECTORS( v1, v2 ) \
            ( Vector_x(v1)*Vector_x(v2) + \
              Vector_y(v1)*Vector_y(v2) + \
              Vector_z(v1)*Vector_z(v2) )

#define  DOT_POINT_VECTOR( p, v ) \
            ( Point_x(p)*Vector_x(v) + \
              Point_y(p)*Vector_y(v) + \
              Point_z(p)*Vector_z(v) )

#define  SCALE_POINT( ps, p, scale )  POINT_SCALAR_EXP( ps, p, *, scale )

#define  SCALE_VECTOR( vs, v, scale )  VECTOR_SCALAR_EXP( vs, v, *, scale )

#define  MAGNITUDE( v ) (Real) sqrt( (double) DOT_VECTORS(v,v) )

#define  NORMALIZE_VECTOR( vn, v ) \
         { \
             Real  _mag_; \
 \
             _mag_ = MAGNITUDE( v ); \
             if( _mag_ != 0.0 ) \
                 SCALE_VECTOR( vn, v, 1.0 / _mag_ ); \
         }

#define  CROSS_VECTORS( c, v1, v2 ) \
         { \
             Vector_x(c) = Vector_y(v1) * Vector_z(v2) - \
                           Vector_y(v2) * Vector_z(v1); \
             Vector_y(c) = Vector_z(v1) * Vector_x(v2) - \
                           Vector_z(v2) * Vector_x(v1); \
             Vector_z(c) = Vector_x(v1) * Vector_y(v2) - \
                           Vector_x(v2) * Vector_y(v1); \
         }

#define  EQUAL_POINTS( p1, p2 ) \
         ( Point_x(p1) == Point_x(p2) && \
           Point_y(p1) == Point_y(p2) && \
           Point_z(p1) == Point_z(p2) )

#define  DIST_FROM_PLANE( normal, d, point ) \
         ( DOT_POINT_VECTOR(point,normal) - (d) )

#define  CONVERT_POINT_TO_VECTOR( vector, point ) \
            fill_Point( point, \
                        Vector_x(vector), Vector_y(vector), Vector_z(vector) )

#define  CONVERT_VECTOR_TO_POINT( point, vector ) \
            fill_Vector( vector, \
                         Point_x(point), Point_y(point), Point_z(point) )
#endif
