/************************************************************************
 * MINC 2.0 PRIVATE UTILITY FUNCTIONS
 ************************************************************************/
#include <stdlib.h>
#include <math.h>
#include <hdf5.h>
#include <minc.h>
#include "minc2.h"
#include "minc2_private.h"

hid_t
miget_volume_file_handle(mihandle_t volume)
{
    return ((mivolume_ptr) volume)->hdf_id;
}

/*! Convert a MINC 2 datatype into a HDF5 datatype.  Actually returns a copy
 * of the datatype, so the returned value must be explicitly freed with a
 * call to H5Tclose().
 * \param mitype The MINC 2 data type to convert
 */
hid_t
mitype_to_hdftype(mitype_t mitype)
{
    hid_t type_id;

    switch (mitype) {
    case MI_TYPE_BYTE:
	type_id = H5Tcopy(H5T_NATIVE_SCHAR);
	break;
    case MI_TYPE_CHAR:
	type_id = H5Tcopy(H5T_NATIVE_SCHAR);
	break;
    case MI_TYPE_SHORT:
	type_id = H5Tcopy(H5T_NATIVE_SHORT);
	break;
    case MI_TYPE_INT:
	type_id = H5Tcopy(H5T_NATIVE_INT);
	break;
    case MI_TYPE_FLOAT:
	type_id = H5Tcopy(H5T_NATIVE_FLOAT);
	break;
    case MI_TYPE_DOUBLE:
	type_id = H5Tcopy(H5T_NATIVE_DOUBLE);
	break;
    case MI_TYPE_UBYTE:
	type_id = H5Tcopy(H5T_NATIVE_UCHAR);
	break;
    case MI_TYPE_USHORT:
	type_id = H5Tcopy(H5T_NATIVE_USHORT);
	break;
    case MI_TYPE_UINT:
	type_id = H5Tcopy(H5T_NATIVE_UINT);
	break;
    default:
        type_id = H5Tcopy(mitype); /* Else it is a standard HDF type handle */
	break;
    }
    return (type_id);
}

/*! Convert a MINC 2 datatype into a NetCDF datatype.
 * \param mitype The MINC 2 data type to convert
 * \param is_signed Set to TRUE if the data type is signed, FALSE if unsigned.
 */
int
mitype_to_nctype(mitype_t mitype, int *is_signed)
{
    int nctype;

    *is_signed = 1;		/* Assume signed by default. */

    switch (mitype) {
    case MI_TYPE_BYTE:
	nctype = NC_BYTE;
	break;
    case MI_TYPE_SHORT:
	nctype = NC_SHORT;
	break;
    case MI_TYPE_INT:
	nctype = NC_INT;
	break;
    case MI_TYPE_FLOAT:
	nctype = NC_FLOAT;
	break;
    case MI_TYPE_DOUBLE:
	nctype = NC_DOUBLE;
	break;
    case MI_TYPE_UBYTE:
	nctype = NC_BYTE;
	*is_signed = 0;
	break;
    case MI_TYPE_USHORT:
	nctype = NC_SHORT;
	*is_signed = 0;
	break;
    case MI_TYPE_UINT:
	nctype = NC_INT;
	*is_signed = 1;
	break;
    default:
	nctype = -1;		/* ERROR!! */
	break;
    }
    return (nctype);
}

/*! Return the group or dataset ID of the last item in a "path",
 * specified like a UNIX pathname /black/white/red etc.  
 * \param file_id The HDF5 handle of the file (or group) at which to start
 * the search.  
 * \param path A string consisting of a slash-separated list of 
 * HDF5 groupnames
 */
hid_t
midescend_path(hid_t file_id, const char *path)
{
    hid_t tmp_id;

    /* Put H5E_BEGIN_TRY/H5E_END_TRY around this to avoid the overzealous 
     * automatic error reporting of HDF5.
     */
    H5E_BEGIN_TRY {
        tmp_id = H5Gopen(file_id, path);
    } H5E_END_TRY;

    /* If the group open fails, try opening the object as a dataset.
     */
    if (tmp_id < 0) {
	tmp_id = H5Dopen(file_id, path);
    }
    return (tmp_id);
}

/** Get the number of voxels in the file - this is the total number,
 * not just spatial dimensions 
 */
mi_i64_t
miget_voxel_count(int mincid)
{
    int imgid;
    int dim[MAX_VAR_DIMS];
    int idim;
    int ndims;
    mi_i64_t nvoxels;
    long length;

    /* Get the dimension ids for the image variable 
     */
    imgid = ncvarid(mincid, MIimage);
    (void)ncvarinq(mincid, imgid, NULL, NULL, &ndims, dim, NULL);

    /* Loop over them to get the total number of voxels 
     */
    nvoxels = 1;
    for (idim = 0; idim < ndims; idim++) {
        (void)ncdiminq(mincid, dim[idim], NULL, &length);
        nvoxels *= length;
    }
    return (nvoxels);
}

/* Get the volume of a spatial voxel */
double
miget_voxel_volume(int mincid)
{
    int imgid;
    int dim[MAX_VAR_DIMS];
    int idim;
    int ndims;
    double volume;
    double step;
    char dimname[MAX_NC_NAME];

    /* Get the dimension ids for the image variable */
    imgid = ncvarid(mincid, MIimage);
    (void)ncvarinq(mincid, imgid, NULL, NULL, &ndims, dim, NULL);

    /* Loop over them to get the total spatial volume */
    volume = 1.0;
    for (idim = 0; idim < ndims; idim++) {

        /* Get the name and check that this is a spatial dimension */
        (void)ncdiminq(mincid, dim[idim], dimname, NULL);
        if (!strcmp(dimname, MIxspace) ||
            !strcmp(dimname, MIyspace) ||
            !strcmp(dimname, MIzspace)) {

            /* Get the step attribute of the coordinate variable */
            step = 1.0;
            miget_attribute(mincid, dimname, MIstep, 1, &step);

            /* Make sure that it is positive and calculate the volume */
            if (step < 0.0)
                step = -step;
            volume *= step;
        }
   }
   return (volume);
}

/** Get a double attribute from a minc file */
void 
miget_attribute(int mincid, char *varname, char *attname,
                int maxvals, double vals[])
{
    int varid;
    int old_ncopts;
    int length;

    if (!mivar_exists(mincid, varname))
        return;
    varid = ncvarid(mincid, varname);
    old_ncopts = ncopts;
    ncopts = 0;
    (void)miattget(mincid, varid, attname, NC_DOUBLE, maxvals, vals, &length);
    ncopts = old_ncopts;
}

/* Get the total number of image dimensions in a minc file */
int
miget_dim_count(int mincid)
{
    int ndims;

    ncvarinq(mincid, ncvarid(mincid, MIimage), NULL, NULL, &ndims, NULL, NULL);
    return (ndims);
}

/* Get the mapping from spatial dimension - x, y, z - to file dimensions
 * and vice-versa.
 */
void
mifind_spatial_dims(int mincid, int space_to_dim[], int dim_to_space[])
{
    int imgid, dim[MAX_VAR_DIMS];
    int idim, ndims, world_index;
    char dimname[MAX_NC_NAME];

    /* Set default values */
    for (idim = 0; idim < 3; idim++)
        space_to_dim[idim] = -1;
    
    for (idim = 0; idim < MAX_VAR_DIMS; idim++)
        dim_to_space[idim] = -1;

    /* Get the dimension ids for the image variable 
     */
    imgid = ncvarid(mincid, MIimage);
    (void)ncvarinq(mincid, imgid, NULL, NULL, &ndims, dim, NULL);

    /* Loop over them to find the spatial ones 
     */
    for (idim = 0; idim < ndims; idim++) {
        /* Get the name and check that this is a spatial dimension 
         */
        (void)ncdiminq(mincid, dim[idim], dimname, NULL);
        
        if (!strcmp(dimname, MIxspace)) {
            world_index = MI2_X;
        }
        else if (!strcmp(dimname, MIyspace)) {
            world_index = MI2_Y;
        }
        else if (!strcmp(dimname, MIzspace)) {
            world_index = MI2_Z;
        }
        else {
            continue;
        }

        space_to_dim[world_index] = idim;
        dim_to_space[idim] = world_index;
    }
}

/** Get the voxel to world transform (for column vectors) */
void 
miget_voxel_to_world(int mincid,
                     mi_lin_xfm_t voxel_to_world)
{
    int i;
    int j;
    double dircos[MI2_3D];
    double step;
    double start;
    static char *dimensions[] = { MIxspace, MIyspace, MIzspace };

    /* Zero the matrix */
    for (i = 0; i < MI2_LIN_XFM_SIZE; i++) {
        for (j = 0; j < MI2_LIN_XFM_SIZE; j++) {
            voxel_to_world[i][j] = 0.0;
        }
        voxel_to_world[i][i] = 1.0;
    }

    for (j = 0; j < MI2_3D; j++) {
        /* Set default values */
        step = 1.0;
        start = 0.0;
        for (i = 0; i < MI2_3D; i++)
            dircos[i] = 0.0;
        dircos[j] = 1.0;

        /* Get the attributes */
        miget_attribute(mincid, dimensions[j], MIstart, 1, &start);
        miget_attribute(mincid, dimensions[j], MIstep, 1, &step);
        miget_attribute(mincid, dimensions[j], MIdirection_cosines,
                        MI2_3D, dircos);
        minormalize_vector(dircos);

        /* Put them in the matrix */
        for (i = 0; i < MI2_3D; i++) {
            voxel_to_world[i][j] = step * dircos[i];
            voxel_to_world[i][MI2_3D] += start * dircos[i];
        }
    }
}

/** Normalize a 3 element vector */
void 
minormalize_vector(double vector[MI2_3D])
{
    int i;
    double magnitude;

    magnitude = 0.0;
    for (i = 0; i < MI2_3D; i++) {
        magnitude += (vector[i] * vector[i]);
    }
    magnitude = sqrt(magnitude);
    if (magnitude > 0.0) {
        for (i = 0; i < MI2_3D; i++) {
            vector[i] /= magnitude;
        }
    }
}

/** Transforms a coordinate through a linear transform */
void 
mitransform_coord(double out_coord[],
                  mi_lin_xfm_t transform,
                  double in_coord[])
{
    int i, j;
    double in_homogeneous[MI2_3D + 1];
    double out_homogeneous[MI2_3D + 1];

    for (i = 0; i < MI2_3D; i++) {
        in_homogeneous[i] = in_coord[i];
    }
    in_homogeneous[MI2_3D] = 1.0;

    for (i = 0; i < MI2_3D + 1; i++) {
        out_homogeneous[i] = 0.0;
        for (j = 0; j < MI2_3D + 1; j++) {
            out_homogeneous[i] += transform[i][j] * in_homogeneous[j];
        }
    }

    printf("W = %f\n", out_homogeneous[3]);

    for (i = 0; i < MI2_3D; i++) {
        out_coord[i] = out_homogeneous[i];
    }
}
