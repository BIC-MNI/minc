
#include  <internal_volume_io.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Geometry/points.c,v 1.9 1994-12-08 08:49:46 david Exp $";
#endif

/* ----------------------------- MNI Header -----------------------------------@NAME       : create_noncolinear_vector
@INPUT      : v
@OUTPUT     : not_v
@RETURNS    :
@DESCRIPTION: Creates a vector which is not colinear with the given vector.
              This is used, for instance, when creating a vector which is
              perpendicular to a given vector.
@METHOD     :
@GLOBALS    :
@CALLS      :
@CREATED    : 1993            David MacDonald
@MODIFIED   :
---------------------------------------------------------------------------- */

public  void  create_noncolinear_vector(
    Vector  *v,
    Vector  *not_v )
{
    Real   x, y, z;

    x = Vector_x(*v);
    y = Vector_y(*v);
    z = Vector_z(*v);
    if( x != 0.0 || y != 0.0 )
    {
        fill_Vector( *not_v, x, y, z + 1.0 );
    }
    else
    {
        fill_Vector( *not_v, 1.0, y, z );
    }
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
