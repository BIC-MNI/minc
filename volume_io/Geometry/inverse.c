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
static char rcsid[] = "$Header: /private-cvsroot/minc/volume_io/Geometry/inverse.c,v 1.8 1995-07-31 13:44:28 david Exp $";
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : compute_transform_inverse
@INPUT      : transform
@OUTPUT     : inverse
@RETURNS    : TRUE if successful
@DESCRIPTION: Computes the inverse of the given transformation matrix.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  BOOLEAN   compute_transform_inverse(
    Transform  *transform,
    Transform  *inverse )
{
    int        i, j;
    Real       **t, **inv;
    BOOLEAN    success;

    /* --- copy the transform to a numerical recipes type matrix */

    ALLOC2D( t, 4, 4 );
    ALLOC2D( inv, 4, 4 );

    for_less( i, 0, 4 )
    {
        for_less( j, 0, 4 )
            t[i][j] = Transform_elem(*transform,i,j);
    }

    success = invert_square_matrix( 4, t, inv );

    if( success )
    {
        /* --- copy the resulting numerical recipes matrix to the
               output argument */

        for_less( i, 0, 4 )
        {
            for_less( j, 0, 4 )
            {
                Transform_elem(*inverse,i,j) = inv[i][j];
            }
        }

#ifdef  DEBUG
        /* --- check if this really is an inverse, by multiplying */

        {
            Transform  ident;

            concat_transforms( &ident, transform, inverse );

            if( !close_to_identity(&ident) )
            {
                print_error( "Error in compute_transform_inverse\n" );
            }
        }
#endif
    }
    else
        make_identity_transform( inverse );

    FREE2D( t );
    FREE2D( inv );

    return( success );
}
