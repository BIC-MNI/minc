/** \file convert.c
 * \brief MINC 2.0 Coordinate and Voxel Conversion Functions
 * \author Bert Vincent
 *
 * Functions to convert "real" valued data to and from "voxel" valued
 * data, and to convert coordinates between "voxel" and "world" systems.
 */

#include <stdlib.h>
#include <hdf5.h>
#include <math.h>
#include "minc.h"
#include "minc2.h"
#include "minc2_private.h"

/** convert values between real (scaled) values and voxel (unscaled)
 * values.  The voxel value is the unscaled value, and corresponds to the
 * value actually stored in the file, whereas the "real" value is the
 * value at the given location after scaling has been applied.
 */
int
miconvert_real_to_voxel(mihandle_t volume,
                        const unsigned long location[],
                        int ndims,
                        double real_value,
                        double *voxel_value_ptr
                        )
{
    int result = MI_NOERROR;
    double valid_min, valid_max;
    double slice_min, slice_max;
    double voxel_range, voxel_offset;
    double real_range, real_offset;

    /* get valid min/max, image min/max 
     */
    miget_volume_valid_range(volume, &valid_max, &valid_min);
    
    /* get image min/max 
     */
    miget_slice_range(volume, location, ndims, &slice_max, &slice_min);
    
    /* Calculate the actual conversion.
     */
    voxel_offset = valid_min;
    real_offset = slice_min;
    voxel_range = valid_max - valid_min;
    real_range = slice_max - slice_min;
    
    real_value = (real_value - real_offset) / real_range;
    *voxel_value_ptr = (real_value + voxel_offset) * voxel_range;

    return (result);
}


/** convert values between real (scaled) values and voxel (unscaled)
 * values.  The voxel value is the unscaled value, and corresponds to the
 * value actually stored in the file, whereas the "real" value is the
 * value at the given location after scaling has been applied.
 */
int
miconvert_voxel_to_real(mihandle_t volume,
                        const unsigned long voxel_coords[],
                        int ndims,
                        double voxel_value,
                        double *real_value_ptr)
{
    int result = MI_NOERROR;
    double valid_min, valid_max;
    double slice_min, slice_max;
    double voxel_range, voxel_offset;
    double real_range, real_offset;

    /* get valid min/max, image min/max 
     */
    miget_volume_valid_range(volume, &valid_max, &valid_min);
    
    /* get image min/max 
     */
    miget_slice_range(volume, voxel_coords, ndims, &slice_max, &slice_min);

    /* Calculate the actual conversion.
     */
    voxel_offset = valid_min;
    real_offset = slice_min;
    voxel_range = valid_max - valid_min;
    real_range = slice_max - slice_min;
    
    voxel_value = (voxel_value - voxel_offset) / voxel_range;
    *real_value_ptr = (voxel_value + real_offset) * real_range;
    return (result);
}

/* Stuff from volume_io 
 */

/**  Computes the cross product of 2 3-dimensional vectors.
 */
static void
micross_3D_vector(const double v1[MI2_3D], /**< Input vector #1 */
                  const double v2[MI2_3D], /**< Input vector #2 */
                  double cross[MI2_3D]) /**< Output vector */
{
    cross[0] = v1[1] * v2[2] - v1[2] * v2[1];
    cross[1] = v1[2] * v2[0] - v1[0] * v2[2];
    cross[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

/** Fills in the transform with the identity matrix.
 */
static void
mimake_identity_transform(mi_lin_xfm_t transform)
{
    int i, j;

    for (i = 0; i < MI2_LIN_XFM_SIZE; i++) {
        for (j = 0; j < MI2_LIN_XFM_SIZE; j++) {
            transform[i][j] = 0.0;
        }
        transform[i][i] = 1.0;
    }
}

/** Computes a linear transform from the indices of the spatial
 dimensions, the step sizes, the start offsets, and the direction
 cosines.
*/
static void 
micompute_v2w_transform(double steps[MI2_3D],
                        double cosines[MI2_3D][MI2_3D],
                        double starts[MI2_3D],
                        mi_lin_xfm_t transform)
{
    /* These buffers are here so that we can reorder these if need be.
     * For now, they really serve no purpose.
     */
    double steps_3D[MI2_3D];
    double cosines_3D[MI2_3D][MI2_3D];
    double starts_3D[MI2_3D];
    int i, j;

    /* Find how many direction cosines are specified, and set the
     * 3D steps and starts.
     * TODO: THis is where we would reorder values into XYZ order if
     * necessary!
     */
    for ( i = 0; i < MI2_3D; i++ ) {
        steps_3D[i] = steps[i];
        starts_3D[i] = starts[i];
        cosines_3D[i][MI2_X] = cosines[i][MI2_X];
        cosines_3D[i][MI2_Y] = cosines[i][MI2_Y];
        cosines_3D[i][MI2_Z] = cosines[i][MI2_Z];
    }

    /* make sure the 3 axes are not a singular system */

    for ( i = 0; i < MI2_3D; i++ ) {
        double normal[MI2_3D];

        micross_3D_vector( cosines_3D[i], cosines_3D[(i + 1) % MI2_3D],
                           normal );
        if ( normal[0] == 0.0 && normal[1] == 0.0 && normal[2] == 0.0 ) {
            break;              /* Singular */
        }
    }

    if ( i < MI2_3D ) {
        /* Singular system, convert to identity... 
         */
        cosines_3D[0][0] = 1.0;
        cosines_3D[0][1] = 0.0;
        cosines_3D[0][2] = 0.0;
        cosines_3D[1][0] = 0.0;
        cosines_3D[1][1] = 1.0;
        cosines_3D[1][2] = 0.0;
        cosines_3D[2][0] = 0.0;
        cosines_3D[2][1] = 0.0;
        cosines_3D[2][2] = 1.0;
    }

    /* Make the linear transformation 
     */
    mimake_identity_transform( transform );

    /* Calculate the voxel-to-world transform 
     */
    for ( i = 0; i < MI2_3D; i++ ) {
        for ( j = 0; j < MI2_3D; j++ ) {
            transform[j][i] = cosines_3D[i][j] * steps_3D[i];
            transform[j][3] += cosines_3D[i][j] * starts_3D[i];
        }
    }
}

/** This is just a constant which implies that the transform is a homogenous
 * linear transform.
 */
#define W 1.0

/** Transforms the point \a in[3] by the homogenous transform
    matrix, resulting in \a out[3]
*/
static void 
mixfm_point(mi_lin_xfm_t transform,
            const double in[MI2_3D],
            double out[MI2_3D])
{
    double tmp[MI2_LIN_XFM_SIZE];
    int i;

    for (i = 0; i < MI2_LIN_XFM_SIZE; i++) {
      tmp[i] = (transform[i][0] * in[MI2_X] +
                transform[i][1] * in[MI2_Y] +
                transform[i][2] * in[MI2_Z] +
                transform[i][3] * W);
    }

    if ( tmp[3] != 0.0 && tmp[3] != 1.0 ) {
        for (i = 0; i < 3; i++) {
            tmp[i] /= tmp[3];
        }
    }

    for (i = 0; i < MI2_3D; i++) {
        out[i] = tmp[i];
    }
}

/** Performs scaled maximal pivoting gaussian elimination as a
    numerically robust method to solve systems of linear equations.
*/

static int
scaled_maximal_pivoting_gaussian_elimination(int   row[4],
                                             double  a[4][4],
                                             double solution[4][4] )
{
    int i, j, k, p, v, tmp;
    double s[4], val, best_val, m, scale_factor;
    int result;

    for ( i = 0; i < 4; i++) {
        row[i] = i;
    }

    for ( i = 0; i < 4; i++) {
        s[i] = fabs( a[i][0] );
        for ( j = 1; j < 4; j++ ) {
            if ( fabs(a[i][j]) > s[i] ) {
                s[i] = fabs(a[i][j]);
            }
        }

        if ( s[i] == 0.0 ) {
            return ( MI_ERROR );
        }
    }

    result = MI_NOERROR;

    for ( i = 0; i < 4 - 1; i++ ) {
        p = i;
        best_val = a[row[i]][i] / s[row[i]];
        best_val = fabs( best_val );
        for ( j = i + 1; j < 4; j++ ) {
            val = a[row[j]][i] / s[row[j]];
            val = fabs( val );
            if( val > best_val ) {
                best_val = val;
                p = j;
            }
        }

        if ( a[row[p]][i] == 0.0 ) {
            result = MI_ERROR;
            break;
        }

        if ( i != p ) {
            tmp = row[i];
            row[i] = row[p];
            row[p] = tmp;
        }

        for ( j = i + 1; j < 4; j++ ) {
            if ( a[row[i]][i] == 0.0 ) {
                result = MI_ERROR;
                break;
            }

            m = a[row[j]][i] / a[row[i]][i];
            for ( k = i + 1; k < 4; k++ )
                a[row[j]][k] -= m * a[row[i]][k];
            for( v = 0; v < 4; v++ )
                solution[row[j]][v] -= m * solution[row[i]][v];
        }

        if (result != MI_NOERROR)
            break;
    }

    if ( result == MI_NOERROR && a[row[4-1]][4-1] == 0.0 )
        result = MI_ERROR;

    if ( result == MI_NOERROR ) {
        for ( i = 4-1;  i >= 0;  --i ) {
            for ( j = i+1; j < 4; j++) {
                scale_factor = a[row[i]][j];
                for ( v = 0; v < 4; v++ )
                    solution[row[i]][v] -= scale_factor * solution[row[j]][v];
            }

            for( v = 0; v < 4; v++ )
                solution[row[i]][v] /= a[row[i]][i];
        }
    }

    return (result);
}

static int
scaled_maximal_pivoting_gaussian_elimination_real(double coefs[4][4],
                                                  double values[4][4])
{
    int i, j, v, row[4];
    double a[4][4], solution[4][4];
    int result;

    for ( i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++)
            a[i][j] = coefs[i][j];
        for ( v = 0; v < 4; v++ )
            solution[i][v] = values[v][i];
    }

    result = scaled_maximal_pivoting_gaussian_elimination(row, a, solution);

    if ( result == MI_NOERROR ) {
        for ( i = 0; i < 4; i++ ) {
            for ( v = 0; v < 4; v++ )
                values[v][i] = solution[row[i]][v];
        }
    }

    return ( result );
}

/** Computes the inverse of a square matrix.
 */
static int
invert_4x4_matrix(double matrix[4][4], /**< Input matrix */
                  double inverse[4][4]) /**< Output (inverted) matrix */
{
    double tmp;
    int result;
    int i, j;

    /* Start off with the identity matrix. */
    for ( i = 0; i < 4; i++ ) {
        for ( j = 0; j < 4; j++ ) {
            inverse[i][j] = 0.0;
        }
        inverse[i][i] = 1.0;
    }

    result = scaled_maximal_pivoting_gaussian_elimination_real( matrix,
                                                                inverse );

    if ( result == MI_NOERROR )  {
        for ( i = 0; i < 4 - 1; i++) {
            for ( j = i + 1; j < 4; j++ ) {
                tmp = inverse[i][j];
                inverse[i][j] = inverse[j][i];
                inverse[j][i] = tmp;
            }
        }
    }
    return (result);
}

static int 
miinvert_transform(mi_lin_xfm_t transform, mi_lin_xfm_t inverse )
{
    int result;

    result = invert_4x4_matrix( transform, inverse );
    if (result != MI_NOERROR) {
        mimake_identity_transform(inverse);
    }
    return ( result );
}

/** Converts a 3-dimensional position in voxel coordinates into a 
 * 3-dimensional position in world coordinates.
 */
int
miconvert_3D_voxel_to_world(mihandle_t volume,
                            const double voxel[MI2_3D],
                            double world[MI2_3D])
{
#if 0                           /* DMcD's way */
    int result = MI_NOERROR;    /* Return code */
    hid_t file_id;              /* HDF5 file ID */
    mi_lin_xfm_t v2w_transform; /* Voxel-to-world transform */

    file_id = miget_volume_file_handle(volume);

    miget_voxel_to_world(file_id, v2w_transform);

    mixfm_point(v2w_transform, voxel, world);

#else  /* Peter's way */
    int result = MI_NOERROR;
    hid_t file_id;
    mi_lin_xfm_t v2w_transform;

    file_id = miget_volume_file_handle(volume);
    miget_voxel_to_world(file_id, v2w_transform);
    mitransform_coord(world, v2w_transform, voxel);
#endif
    return (result);
}

/** Converts a 3-dimensional position in world coordinates into a 
 * 3-dimensional position in voxel coordinates.
 */
int miconvert_3D_world_to_voxel(mihandle_t volume,
                                const double world[MI2_3D],
                                double voxel[MI2_3D])
{
    int result = MI_NOERROR;    /* Return code */
    hid_t file_id;              /* HDF5 file ID */
    mi_lin_xfm_t v2w_transform; /* Voxel-to-world transform */
    mi_lin_xfm_t w2v_transform; /* World-to-voxel transform */

    file_id = miget_volume_file_handle(volume);

    miget_voxel_to_world(file_id, v2w_transform );

    miinvert_transform(v2w_transform, w2v_transform);

    { 
        int i, j;
        printf("Voxel to World:\n");
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                printf("%f ", v2w_transform[i][j]);
            }
            printf("\n");
        }

        printf("World to Voxel:\n");
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                printf("%f ", w2v_transform[i][j]);
            }
            printf("\n");
        }
    }

    mixfm_point(w2v_transform, world, voxel);

    return (result);
}

int 
miconvert_3D_voxel_to_spatial_frequency(mihandle_t volume,
                                        const double voxel[MI2_3D],
                                        double world[MI2_3D])

{
    return (MI_NOERROR);
}

int 
miconvert_3D_spatial_frequency_to_voxel(mihandle_t volume,
                                        const double world[MI2_3D],
                                        double voxel[MI2_3D])

{
    return (MI_NOERROR);
}

int
miconvert_voxel_to_world(midimhandle_t dimensions[],
                         int ndims,
                         const double voxel[],
                         double world[])
{
    int result = MI_NOERROR;    /* Return code */
    hid_t file_id;              /* HDF5 file ID */
    int length;                 /* Attribute length */
    mi_lin_xfm_t v2w_transform; /* Voxel-to-world transform */
    double cosines[MI2_3D][MI2_3D]; /* Direction cosines */
    double starts[MI2_3D];      /* Start positions */
    double steps[MI2_3D];       /* Step sizes */

    return (result);
}

int
miconvert_world_to_voxel(midimhandle_t dimensions[],
                         int ndims,
                         const double voxel[],
                         double world[])
{
    return (MI_NOERROR);
}

/** This function retrieves the real values of a position in the
 *  MINC volume.  The "real" value is the value at the given location 
 *  after scaling has been applied.
 */
int
miget_real_value(mihandle_t volume,
                 const unsigned long coords[],
                 int ndims,
                 double *value_ptr)
{
    int result;
    unsigned long count[ndims]; /*  */
    int i;

    for (i = 0; i < ndims; i++) {
        count[i] = 1;
    }
    result = miget_real_value_hyperslab(volume, MI_TYPE_DOUBLE,
                                        coords, count, value_ptr);
    return (result);
}

/** This function sets the  real value of a position in the MINC
 *  volume. The "real" value is the value at the given location 
 *  after scaling has been applied.
 */
int
miset_real_value(mihandle_t volume,
                 const unsigned long coords[],
                 int ndims,
                 double value)
{
    int result;
    unsigned long count[ndims]; /*  */
    int i;

    for (i = 0; i < ndims; i++) {
        count[i] = 1;
    }
    result = miset_real_value_hyperslab(volume, MI_TYPE_DOUBLE,
                                        coords, count, &value);
    return (result);
}


int
miconvert_spatial_origin_to_start( mihandle_t volume,
                                   double world[3],
                                   double starts[3])
{
    return (MI_NOERROR);
}

int
miconvert_spatial_frequency_origin_to_start( mihandle_t volume,
                                             double world[3],
                                             double starts[3])
{
    return (MI_NOERROR);
}

int
miset_spatial_origin_to_start( mihandle_t volume,
                               double world[3])
{
    return (MI_NOERROR);
}

int
miset_spatial_frequency_origin_to_start(mihandle_t volume,
                                        double world[3])
{
    return (MI_NOERROR);
}

/** This function retrieves the voxel values of a position in the
 * MINC volume. The voxel value is the unscaled value, and corresponds
 * to the value actually stored in the file.
 */
int
miget_voxel_value(mihandle_t volume,
                  const unsigned long coords[],
                  int ndims,
                  double *voxel_ptr)
{
    hid_t file_id;
    int result;

    file_id = miget_volume_file_handle(volume);
    result = mivarget1(file_id, /* The file handle */
                       MI2varid(file_id, MIimage), /* The variable ID */
                       (long *) coords, /* The voxel coordinates */
                       NC_DOUBLE, /* The datatype */
                       MI_SIGNED, /* Ignored for floating point data */
                       voxel_ptr); /* Location to store result */
    return (result);
}

/** This function sets the voxel value of a position in the MINC
 * volume.  The voxel value is the unscaled value, and corresponds to the
 * value actually stored in the file.
 */
int
miset_voxel_value(mihandle_t volume,
                  const unsigned long coords[],
                  int ndims,
                  double voxel)
{
    hid_t file_id;
    int result;

    file_id = miget_volume_file_handle(volume);
    result = mivarput1(file_id, /* The file handle */
                       MI2varid(file_id, MIimage), /* The variable ID */
                       (long *) coords, /* The voxel coordinates */
                       NC_DOUBLE, /* The datatype */
                       MI_SIGNED, /* Ignored for floating point data */
                       &voxel); /* Location from which to store result */
    return (result);
}


#ifdef M2_TEST
#define TESTRPT(msg, val) (error_cnt++, fprintf(stderr, \
                                  "Error reported on line #%d, %s: %d\n", \
                                  __LINE__, msg, val))
static int error_cnt = 0;

int
main(int argc, char **argv)
{
    int result;
    double voxel[3];
    double world[3];
    double new_voxel[3];
    unsigned long coords[3];
    unsigned long count[3] = {1,1,1};
    int i;
    mihandle_t hvol;
    double v1, v2;
    double r1, r2;
    double f;

    result = miopen_volume(argv[1], MI2_OPEN_READ, &hvol);

    coords[0] = atof(argv[2]);
    coords[1] = atof(argv[3]);
    coords[2] = atof(argv[4]);

    voxel[0] = coords[0];
    voxel[1] = coords[1];
    voxel[2] = coords[2];

    miconvert_3D_voxel_to_world(hvol, voxel, world);

    for (i = 0; i < 3; i++) {
        printf("%.20g ", world[i]);
    }
    printf("\n");

    miconvert_3D_world_to_voxel(hvol, world, new_voxel);
    for (i = 0; i < 3; i++) {
        printf("%.20g ", new_voxel[i]);
    }
    printf("\n");


    /* Get a voxel value.
     */
    miget_voxel_value_hyperslab(hvol, MI_TYPE_DOUBLE,
                                coords, count, &v1);

    /* Convert from voxel to real.
     */
    miconvert_voxel_to_real(hvol, coords, 3, v1, &r1);
    printf("voxel %f => real %f\n", v1, r1);

    /* Convert it back to voxel.
     */
    miconvert_real_to_voxel(hvol, coords, 3, r1, &v2);
    printf("real %f => voxel %f\n", r1, v2);

    /* Compare to the value read via the ICV
     */
    miget_real_value(hvol, coords, 3, &r2);

    printf("real from ICV: %f\n", r2);
    printf("\n");

    if (v1 != v2) {
        TESTRPT("Voxel value mismatch", 0);
    }
    if (r1 != r2) {
        TESTRPT("Real value mismatch", 0);
    }
    return (error_cnt);
}

#endif
