#include  <internal_volume_io.h>

public  Colour  make_Colour(
    int   r,
    int   g,
    int   b )
{
    return( make_rgba_Colour( r, g, b, 255 ) );
}

public  Real  get_Colour_r_0_1(
    Colour   colour )
{
    return( get_Colour_r(colour) / 255.0 );
}

public  Real  get_Colour_g_0_1(
    Colour   colour )
{
    return( get_Colour_g(colour) / 255.0 );
}

public  Real  get_Colour_b_0_1(
    Colour   colour )
{
    return( get_Colour_b(colour) / 255.0 );
}

public  Real  get_Colour_a_0_1(
    Colour   colour )
{
    return( get_Colour_a(colour) / 255.0 );
}

public  Colour  make_Colour_0_1(
    Real   r,
    Real   g,
    Real   b )
{
    return( make_Colour( (int) (r * 255.0 + 0.5),
                         (int) (g * 255.0 + 0.5),
                         (int) (b * 255.0 + 0.5) ) );
}

public  Colour  make_rgba_Colour_0_1(
    Real   r,
    Real   g,
    Real   b,
    Real   a )
{
    return( make_rgba_Colour( (int) (r * 255.0 + 0.5),
                              (int) (g * 255.0 + 0.5),
                              (int) (b * 255.0 + 0.5),
                              (int) (a * 255.0 + 0.5) ) );
}
