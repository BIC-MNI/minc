#include  <def_mni.h>

public  void  convert_voxel_to_world(
    volume_struct   *volume,
    Real            x_voxel,
    Real            y_voxel,
    Real            z_voxel,
    Real            *x_world,
    Real            *y_world,
    Real            *z_world )
{
    Point   voxel, world;

    fill_Point( voxel, x_voxel, y_voxel, z_voxel );

    transform_point( &volume->voxel_to_world_transform,
                     &voxel, &world );

    *x_world = Point_x(world);
    *y_world = Point_y(world);
    *z_world = Point_z(world);
}

public  void  convert_world_to_voxel(
    volume_struct   *volume,
    Real            x_world,
    Real            y_world,
    Real            z_world,
    Real            *x_voxel,
    Real            *y_voxel,
    Real            *z_voxel )
{
    Point   voxel, world;

    fill_Point( world, x_world, y_world, z_world );

    transform_point( &volume->world_to_voxel_transform,
                     &world, &voxel );

    *x_voxel = Point_x(voxel);
    *y_voxel = Point_y(voxel);
    *z_voxel = Point_z(voxel);
}

public  Boolean  voxel_is_within_volume(
    volume_struct   *volume,
    Real            position[N_DIMENSIONS] )
{
    return( position[X] >= -0.5 &&
            position[X] <= (Real) volume->sizes[X] - 0.5 &&
            position[Y] >= -0.5 &&
            position[Y] <= (Real) volume->sizes[Y] - 0.5 &&
            position[Z] >= -0.5 &&
            position[Z] <= (Real) volume->sizes[Z] - 0.5 );
}
