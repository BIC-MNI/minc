#ifndef  DEF_GEOM_STRUCTS
#define  DEF_GEOM_STRUCTS

#define  N_DIMENSIONS  3

#define  X             0
#define  Y             1
#define  Z             2

typedef  struct
{
    float   coords[N_DIMENSIONS];
} Point;

#define  Point_coord( point, coord ) ((point).coords[coord])
#define  Point_x( point ) Point_coord( point, X )
#define  Point_y( point ) Point_coord( point, Y )
#define  Point_z( point ) Point_coord( point, Z )
#define  fill_Point( point, x, y, z ) \
            { \
                Point_x(point) = (x); \
                Point_y(point) = (y); \
                Point_z(point) = (z); \
            }

typedef  struct
{
    float   coords[N_DIMENSIONS];
} Vector;

#define  Vector_coord( vector, coord ) ((vector).coords[coord])
#define  Vector_x( vector ) Vector_coord( vector, X )
#define  Vector_y( vector ) Vector_coord( vector, Y )
#define  Vector_z( vector ) Vector_coord( vector, Z )
#define  fill_Vector( vector, x, y, z ) \
            { \
                Vector_x(vector) = (x); \
                Vector_y(vector) = (y); \
                Vector_z(vector) = (z); \
            }

typedef  unsigned  long    Colour;

#define  make_Colour( r, g, b )                  \
          ( (unsigned long) (r) |                \
           ((unsigned long) (g) << 8) |          \
           ((unsigned long) (b) << 16) |         \
           ((unsigned long) (255 << 24)) )

#define  get_Colour_r( colour ) ((colour) & 255)
#define  get_Colour_g( colour ) (((colour) >> 8) & 255)
#define  get_Colour_b( colour ) (((colour) >> 16) & 255)

#define  COLOUR_256_TO_0_1( val ) ( ((val) + 0.5) / 256.0 )
#define  COLOUR_0_1_TO_256( val ) (int) ( (val) * 256.0 - 0.5 )

#define  get_Colour_r_0_1( colour )  COLOUR_256_TO_0_1( get_Colour_r(colour) )
#define  get_Colour_g_0_1( colour )  COLOUR_256_TO_0_1( get_Colour_g(colour) )
#define  get_Colour_b_0_1( colour )  COLOUR_256_TO_0_1( get_Colour_b(colour) )

#define  make_Colour_0_1( r, g, b )                       \
            make_Colour( COLOUR_0_1_TO_256(r),            \
                         COLOUR_0_1_TO_256(g),            \
                         COLOUR_0_1_TO_256(b) )

#define  SCALE_COLOUR( col, factor )                  \
          make_Colour( get_Colour_r(col) * factor,    \
                       get_Colour_g(col) * factor,    \
                       get_Colour_b(col) * factor )

typedef  struct
{
    float   a, d, s;
    float   se;
    float   t;
} Surfprop;

#define  Surfprop_a( surfprop )  ((surfprop).a)
#define  Surfprop_d( surfprop )  ((surfprop).d)
#define  Surfprop_s( surfprop )  ((surfprop).s)
#define  Surfprop_se( surfprop )  ((surfprop).se)
#define  Surfprop_t( surfprop )  ((surfprop).t)

#define  fill_Surfprop( s, amb, diff, spec, spec_exp, trans ) \
         { \
             Surfprop_a(s) = (amb); \
             Surfprop_d(s) = (diff); \
             Surfprop_s(s) = (spec); \
             Surfprop_se(s) = (spec_exp); \
             Surfprop_t(s) = (trans); \
         }

typedef  struct
{
    float    m2d[2][3];
} Transform_2d;

#define  Transform_2d_elem( t, i, j ) ((t).m2d[i][j])

typedef  struct
{
    float    m[4][4];
} Transform;

#define  Transform_elem( t, i, j ) ((t).m[j][i])

#endif
