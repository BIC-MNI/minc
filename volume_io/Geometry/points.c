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
static char rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Geometry/points.c,v 1.11 1995-12-19 15:47:10 david Exp $";
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_noncolinear_vector
@INPUT      : v
@OUTPUT     : not_v
@RETURNS    :
@DESCRIPTION: Creates a vector which is not colinear with the given vector.
              This is used, for instance, when creating a vector which is
              perpendicular to a given vector.
@METHOD     : Copies v to not_v and interchanges the largest absolute
              component with the smallest absolute component.
@GLOBALS    :
@CALLS      :
@CREATED    : 1993            David MacDonald
@MODIFIED   : Jul. 11, 1995   D. MacDonald    - made more numerically robust
---------------------------------------------------------------------------- */

public  void  create_noncolinear_vector(
    Vector  *v,
    Vector  *not_v )
{
    int    max_index, min_index;
    Real   abs_x, abs_y, abs_z, min_abs, max_abs;

    abs_x = ABS( Vector_x(*v) );
    abs_y = ABS( Vector_y(*v) );
    abs_z = ABS( Vector_z(*v) );

    if( abs_x > abs_y )
    {
        max_index = X;
        min_index = Y;
        max_abs = abs_x;
        min_abs = abs_y;
    }
    else
    {
        max_index = Y;
        min_index = X;
        max_abs = abs_y;
        min_abs = abs_x;
    }

    if( abs_z > max_abs )
        max_index = Z;
    else if( abs_z < min_abs )
        min_index = Z;

    *not_v = *v;
    Vector_coord( *not_v, max_index ) = Vector_coord( *v, min_index );
    Vector_coord( *not_v, min_index ) = Vector_coord( *v, max_index );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_orthogonal_vector
@INPUT      : v1
              v2
@OUTPUT     : v
@RETURNS    : 
@DESCRIPTION: Creates a vector which is orthogonal to the given two vectors,
              simply by performing a cross product.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  create_orthogonal_vector(
    Vector   *v1,
    Vector   *v2,
    Vector   *v )
{
    CROSS_VECTORS( *v, *v1, *v2 );
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

public  void  create_two_orthogonal_vectors(
    Vector   *v,
    Vector   *v1,
    Vector   *v2 )
{
    create_noncolinear_vector( v, v1 );

    create_orthogonal_vector( v, v1, v2 );

    create_orthogonal_vector( v2, v, v1 );
}
