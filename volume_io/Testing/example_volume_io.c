#include  <volume_io.h>

/* ------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and
              McGill University make no representations about the
              suitability of this software for any purpose.  It is
              provided "as is" without express or implied warranty.
------------------------------------------------------------------ */

int  main()
{
    int                 v1, v2, v3, sizes[MAX_DIMENSIONS];
    Real                x_world2, y_world2, z_world2;
    Real                voxel2[MAX_DIMENSIONS];
    Real                voxel_value;
    Volume              volume1, volume2;
    int                 i, n_volumes, n_tag_points;
    int                 *structure_ids, *patient_ids;
    Real                **tags1, **tags2, *weights;
    STRING              *labels;
    General_transform   transform;

    /*--- input the two volumes */

    if( input_volume( "volume1.mnc", 3, NULL, MI_ORIGINAL_TYPE, FALSE,
            0.0, 0.0, TRUE, &volume1,
            (minc_input_options *) NULL ) != OK )
        return( 1 );

    if( input_volume( "volume2.mnc", 3, NULL, MI_ORIGINAL_TYPE, FALSE,
            0.0, 0.0, TRUE, &volume2,
            (minc_input_options *) NULL ) != OK )
        return( 1 );

    /*--- input the tag points */

    if( input_tag_file( "tags_volume1.tag", &n_volumes, &n_tag_points,
                        &tags1, &tags2, &weights, &structure_ids,
                        &patient_ids, &labels ) != OK )
        return( 1 );

    /*--- input the general transform */

    if( input_transform_file( "vol1_to_vol2.xfm", &transform ) != OK )
        return( 1 );

    /*--- convert each tag point */

    get_volume_sizes( volume2, sizes );

    for_less( i, 0, n_tag_points )
    {
        /*--- transform the tag points from volume 1 to volume 2
              world space */

        general_transform_point( &transform,
                                 tags1[i][X], tags1[i][Y], tags1[i][Z],
                                 &x_world2, &y_world2, &z_world2 );

        /*--- transform from volume 2 world space to
              volume 2 voxel space */

        convert_world_to_voxel( volume2, x_world2, y_world2, z_world2,
                                voxel2 );

        /*--- convert voxel coordinates to voxel indices */

        v1 = ROUND( voxel2[0] );
        v2 = ROUND( voxel2[1] );
        v3 = ROUND( voxel2[2] );

        /*--- check if voxel indices inside volume */
     
        if( v1 >= 0 && v1 < sizes[0] &&
            v2 >= 0 && v2 < sizes[1] &&
            v3 >= 0 && v3 < sizes[2] )
        {
            voxel_value = get_volume_real_value( volume2, v1, v2, v3,
                                                 0, 0 );

            print( "The value for tag point %d (%s) is: %g\n",
                   i, labels[i], voxel_value );
        }
        else
            print( "The tag point %d (%s) is outside.\n" );
    }

    /*--- free up memory */

    delete_volume( volume1 );
    delete_volume( volume2 );
    free_tag_points( n_volumes, n_tag_points, tags1, tags2,
                 weights, structure_ids, patient_ids, labels );
    delete_general_transform( &transform );

    return( 0 );
}
