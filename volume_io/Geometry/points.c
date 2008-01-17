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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_orthogonal_vector
@INPUT      : v
@OUTPUT     : ortho
@RETURNS    :
@DESCRIPTION: Creates a vector which is orthogonal to v.
@METHOD     : 
@GLOBALS    :
@CALLS      :
@CREATED    : 1993            David MacDonald
@MODIFIED   : Jul. 11, 1995   D. MacDonald    - made more numerically robust
@MODIFIED   : Feb.  8, 1996   D. MacDonald    - changed from noncolinear to
                                                orthogonal
---------------------------------------------------------------------------- */

VIOAPI  void  create_orthogonal_vector(
    Vector  *v,
    Vector  *ortho )
{
    Real   x, y, z;

    x = (Real) Vector_x(*v);
    y = (Real) Vector_y(*v);
    z = (Real) Vector_z(*v);

    fill_Vector( *ortho, y+z, -x-z, y-x );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_two_orthogonal_vectors
@INPUT      : v
@OUTPUT     : v1
              v2
@RETURNS    : 
@DESCRIPTION: Creates two vectors which are perpendicular to each other and
              to the given vector.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

VIOAPI  void  create_two_orthogonal_vectors(
    Vector   *v,
    Vector   *v1,
    Vector   *v2 )
{
    create_orthogonal_vector( v, v1 );

    CROSS_VECTORS( *v2, *v, *v1 );
}
