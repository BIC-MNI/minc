
#include  <volume_io.h>

public  BOOLEAN   compute_transform_inverse(
    Transform  *transform,
    Transform  *inverse )
{
    int        i, j, ind[5];
    float      **t, **inv, col[5], d;
    Transform  ident;

    ALLOC2D( t, 5, 5 );
    ALLOC2D( inv, 5, 5 );

    for_less( i, 0, 4 )
    {
        for_less( j, 0, 4 )
        {
            t[i+1][j+1] = Transform_elem(*transform,i,j);
        }
    }

    ludcmp( t, 4, ind, &d );

    if( d != 0.0 )
    {
        for_inclusive( j, 1, 4 )
        {
            for_inclusive( i, 1, 4 )
                col[i] = 0.0;
            col[j] = 1.0;
            lubksb( t, 4, ind, col );
            for_inclusive( i, 1, 4 )
                inv[i][j] = col[i];
        }

        for_less( i, 0, 4 )
        {
            for_less( j, 0, 4 )
            {
                Transform_elem(*inverse,i,j) = inv[i+1][j+1];
            }
        }

        concat_transforms( &ident, transform, inverse );

        if( !close_to_identity(&ident) )
        {
            print( "Error in compute_transform_inverse\n" );
        }
    }

    FREE2D( t );
    FREE2D( inv );

    return( d != 0.0 );
}
