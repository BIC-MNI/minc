#ifndef  DEF_GEOM_STRUCTS
#define  DEF_GEOM_STRUCTS

#ifndef lint
static char geom_structs_rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Include/volume_io/geom_structs.h,v 1.14 1995-03-21 19:01:41 david Exp $";
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : geom_structs.h
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Types and macros for accessing points, vectors, colours, etc.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  N_DIMENSIONS  3

#define  X             0
#define  Y             1
#define  Z             2

/* ----------------------------- MNI Header -----------------------------------
@NAME       : Point   type
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: A 3D point type and macros for manipulation.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

typedef  struct
{
    float   coords[N_DIMENSIONS];
} Point;

/* --- access the given coordinate of the point */

#define  Point_coord( point, coord ) ((point).coords[coord])

/* --- access x, y, or z coordinate of the point */

#define  Point_x( point ) Point_coord( point, X )
#define  Point_y( point ) Point_coord( point, Y )
#define  Point_z( point ) Point_coord( point, Z )

/* --- assign all 3 coordinates of the point */

#define  fill_Point( point, x, y, z ) \
            { \
                Point_x(point) = (x); \
                Point_y(point) = (y); \
                Point_z(point) = (z); \
            }

/* ----------------------------- MNI Header -----------------------------------
@NAME       : Vector   type
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: A 3D vector type and macros for manipulation.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

typedef  struct
{
    float   coords[N_DIMENSIONS];
} Vector;

/* --- access the given coordinate of the vector */

#define  Vector_coord( vector, coord ) ((vector).coords[coord])

/* --- access x, y, or z coordinate of the vector */

#define  Vector_x( vector ) Vector_coord( vector, X )
#define  Vector_y( vector ) Vector_coord( vector, Y )
#define  Vector_z( vector ) Vector_coord( vector, Z )

/* --- assign all 3 coordinates of the vector */

#define  fill_Vector( vector, x, y, z ) \
            { \
                Vector_x(vector) = (x); \
                Vector_y(vector) = (y); \
                Vector_z(vector) = (z); \
            }

/* ----------------------------- MNI Header -----------------------------------
@NAME       : Colour   type
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: A 4 by 8-bit component colour type and macros for manipulation.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

typedef  unsigned  long    Colour;

/* --- return a colour, given four values, each ranging from 0 and 255 */

#define  make_rgba_Colour( r, g, b, a )            \
          ( (Colour) (r) |                         \
           ((Colour) (g) << (Colour) 8) |          \
           ((Colour) (b) << (Colour) 16) |         \
           ((Colour) (a) << (Colour) 24) )

/* --- return a colour, given three rgb values */

#define  make_Colour( r, g, b )  make_rgba_Colour( r, g, b, 255 )

/* --- return the 4 components of the colour, in the range 0 to 255 */

#define  get_Colour_r( colour ) ((colour) & 255)
#define  get_Colour_g( colour ) (((colour) >> 8) & 255)
#define  get_Colour_b( colour ) (((colour) >> 16) & 255)
#define  get_Colour_a( colour ) (((colour) >> 24) & 255)

/* --- convert a 0 to 255 colour component to a 0 to 1 range */

#define  COLOUR_256_TO_0_1( val ) ( (val) / 255.0 )

/* --- convert a 0 to 1 range colour component to the range 0 to 255 */

#define  COLOUR_0_1_TO_256( val ) (int) ( (val) * 255.0 )

/* --- return the 4 components of the colour in the range 0.0 to 1.0 */

#define  get_Colour_r_0_1( colour )  COLOUR_256_TO_0_1( get_Colour_r(colour) )
#define  get_Colour_g_0_1( colour )  COLOUR_256_TO_0_1( get_Colour_g(colour) )
#define  get_Colour_b_0_1( colour )  COLOUR_256_TO_0_1( get_Colour_b(colour) )
#define  get_Colour_a_0_1( colour )  COLOUR_256_TO_0_1( get_Colour_a(colour) )

#ifdef lint
#define  make_Colour_0_1( r, g, b ) ((Colour) ( r + g + b ))
#define  make_rgba_Colour_0_1( r, g, b, a ) ((Colour) ( r + g + b + a))
#else

/* --- create a colour from 3 rgb components, each in the range 0 to 1 */

#define  make_Colour_0_1( r, g, b )                       \
            make_Colour( COLOUR_0_1_TO_256(r),            \
                         COLOUR_0_1_TO_256(g),            \
                         COLOUR_0_1_TO_256(b) )

/* --- create a colour from 4 rgba components, each in the range 0 to 1 */

#define  make_rgba_Colour_0_1( r, g, b, a )              \
            make_rgba_Colour( COLOUR_0_1_TO_256(r),            \
                              COLOUR_0_1_TO_256(g),            \
                              COLOUR_0_1_TO_256(b),            \
                              COLOUR_0_1_TO_256(a) )
#endif

/* --- converts a colour to a luminance (a gray scale intensity) */

#define  get_Colour_luminance( c )                                            \
            POSITIVE_ROUND( 0.299 * (Real) get_Colour_r(c) +                  \
                            0.587 * (Real) get_Colour_g(c) +                  \
                            0.114 * (Real) get_Colour_b(c) )

/* --- returns a colour which is the given colour multiplied by the factor */

#define  SCALE_COLOUR( col, factor )                  \
          make_rgba_Colour( get_Colour_r(col) * (factor),    \
                            get_Colour_g(col) * (factor),    \
                            get_Colour_b(col) * (factor),    \
                            get_Colour_a(col) )

/* --- component-wise multiplication of two colours, returned in prod */

#define  MULT_COLOURS( prod, c1, c2 )                  \
        { \
            Real r, g, b, r1, g1, b1, r2, g2, b2; \
            r1 = get_Colour_r_0_1(c1); \
            g1 = get_Colour_g_0_1(c1); \
            b1 = get_Colour_b_0_1(c1); \
            r2 = get_Colour_r_0_1(c2); \
            g2 = get_Colour_g_0_1(c2); \
            b2 = get_Colour_b_0_1(c2); \
            r = r1 * r2; \
            g = g1 * g2; \
            b = b1 * b2; \
            (prod) = make_rgba_Colour_0_1( r, g, b, get_Colour_a_0_1(c1) ); \
        }

/* --- component-wise sum of two colours, returned in sum */

#define  ADD_COLOURS( sum, c1, c2 )                  \
        { \
            int _r, _g, _b, _r1, _g1, _b1, _r2, _g2, _b2; \
            _r1 = get_Colour_r(c1); \
            _g1 = get_Colour_g(c1); \
            _b1 = get_Colour_b(c1); \
            _r2 = get_Colour_r(c2); \
            _g2 = get_Colour_g(c2); \
            _b2 = get_Colour_b(c2); \
            _r = _r1 + _r2; \
            _g = _g1 + _g2; \
            _b = _b1 + _b2; \
            if( _r > 255 ) _r = 255; \
            if( _g > 255 ) _g = 255; \
            if( _b > 255 ) _b = 255; \
            (sum) = make_rgba_Colour( _r, _g, _b, get_Colour_a(c1) ); \
        }

/* ----------------------------- MNI Header -----------------------------------
@NAME       : Surfprop   type
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: A Surface property type and macros for manipulation.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

typedef  struct
{
    float   a, d, s;
    float   se;
    float   t;
} Surfprop;

/* --- access the given element of the surface property */

#define  Surfprop_a( surfprop )  ((surfprop).a)
#define  Surfprop_d( surfprop )  ((surfprop).d)
#define  Surfprop_s( surfprop )  ((surfprop).s)
#define  Surfprop_se( surfprop )  ((surfprop).se)
#define  Surfprop_t( surfprop )  ((surfprop).t)

/* --- assign all elements of the structure, s */

#define  fill_Surfprop( s, amb, diff, spec, spec_exp, trans ) \
         { \
             Surfprop_a(s) = (amb); \
             Surfprop_d(s) = (diff); \
             Surfprop_s(s) = (spec); \
             Surfprop_se(s) = (spec_exp); \
             Surfprop_t(s) = (trans); \
         }

/* ----------------------------- MNI Header -----------------------------------
@NAME       : Transform_2d   type
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: A 2D transform type and macros for element access.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

typedef  struct
{
    double    m2d[2][3];
} Transform_2d;

#define  Transform_2d_elem( t, i, j ) ((t).m2d[i][j])

/* ----------------------------- MNI Header -----------------------------------
@NAME       : Transform   type
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: A 3D transform type and macros for element access.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

typedef  struct
{
    double    m[4][4];
} Transform;

#define  Transform_elem( t, i, j ) ((t).m[j][i])

#endif
