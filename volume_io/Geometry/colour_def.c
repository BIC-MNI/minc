#include  <internal_volume_io.h>

public  Colour  make_rgba_Colour(
    int    r,
    int    g,
    int    b,
    int    a )
{
    Colour          c;
    unsigned  char  *byte_ptr;

    byte_ptr = (void *) &c;

    byte_ptr[0] = (unsigned char) a;
    byte_ptr[1] = (unsigned char) b;
    byte_ptr[2] = (unsigned char) g;
    byte_ptr[3] = (unsigned char) r;

    return( c );
}

public  int  get_Colour_r(
    Colour   colour )
{
    unsigned  char  *b;

    b = (void *) &colour;

    return( (int) b[3] );
}

public  int  get_Colour_g(
    Colour   colour )
{
    unsigned  char  *b;

    b = (void *) &colour;

    return( (int) b[2] );
}

public  int  get_Colour_b(
    Colour   colour )
{
    unsigned  char  *b;

    b = (void *) &colour;

    return( (int) b[1] );
}

public  int  get_Colour_a(
    Colour   colour )
{
    unsigned  char  *b;

    b = (void *) &colour;

    return( (int) b[0] );
}
