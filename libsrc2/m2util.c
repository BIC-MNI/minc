/************************************************************************
 * MINC 2.0 PRIVATE UTILITY FUNCTIONS
 ************************************************************************/
#include <stdlib.h>
#include <math.h>
#include <hdf5.h>
#include <minc.h>
#include "minc2.h"
#include "minc2_private.h"

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

/** Generic HDF5 integer-to-double converter.
 */
static herr_t 
mi2_int_to_dbl(hid_t src_id,
               hid_t dst_id,
               H5T_cdata_t *cdata,
               hsize_t nelements,
               size_t buf_stride,
               size_t bkg_stride,
               void *buf_ptr,
               void *bkg_ptr,
               hid_t dset_xfer_plist)
{
    double *dptr;
    unsigned char *sptr;
    int i;
    int nb;
    H5T_sign_t sg;
    double t;

    switch (cdata->command) {
    case H5T_CONV_INIT:
	nb = H5Tget_size(src_id);
	if (nb != 1 && nb != 2 && nb != 4) {
	    return (-1);
	}
	break;

    case H5T_CONV_CONV:
	nb = H5Tget_size(src_id);
	sg = H5Tget_sign(src_id);

        /* Convert starting from "far side" of buffer... (Hope this works!)
         */
        dptr = ((double *) buf_ptr) + nelements;
	sptr = ((unsigned char *) buf_ptr) + (nelements * nb);

	if (sg == H5T_SGN_2) {
	    for (i = 0; i < nelements; i++) {
		sptr -= nb;
		if (nb == 4) {
		    t = *((int *)sptr);
		}
		else if (nb == 2) {
		    t = *((short *)sptr);
		}
		else if (nb == 1) {
		    t = *((char *)sptr);
		}
                *--dptr = t;
	    }
	}
	else {
	    for (i = 0; i < nelements; i++) {
		sptr -= nb;
		if (nb == 4) {
		    t = *((unsigned int *)sptr);
		}
		else if (nb == 2) {
		    t = *((unsigned short *)sptr);
		}
		else if (nb == 1) {
		    t = *((unsigned char *)sptr);
		}
                *--dptr = t;
	    }
	}
	break;

    case H5T_CONV_FREE:
	break;

    default:
	/* Unknown command */
	return (-1);
    }
    return (0);
}

/** Generic HDF5 double-to-integer converter.
 */
static herr_t 
mi2_dbl_to_int(hid_t src_id,
               hid_t dst_id,
               H5T_cdata_t *cdata,
               hsize_t nelements,
               size_t buf_stride,
               size_t bkg_stride,
               void *buf_ptr,
               void *bkg_ptr,
               hid_t dset_xfer_plist)
{
    unsigned char *uptr;
    double *dptr;
    int i;
    int nb;
    H5T_sign_t sg;
    double t;                   /* Signed data */

    switch (cdata->command) {
    case H5T_CONV_INIT:
	nb = H5Tget_size(src_id);
        fprintf(stderr, "d2i(INIT): %d\n", nb);
	if (nb != 8) {
	    return (-1);
	}
	break;

    case H5T_CONV_CONV:
	nb = H5Tget_size(dst_id);
	sg = H5Tget_sign(dst_id);
	uptr = (unsigned char *) buf_ptr;
	dptr = (double *) buf_ptr;

	if (sg == H5T_SGN_2) {
	    for (i = 0; i < nelements; i++) {
                t = rint(*dptr++);
		if (nb == 4) {
		    *((int *)uptr) = (int) t;
		}
		else if (nb == 2) {
		    *((short *)uptr) = (short) t;
		}
		else if (nb == 1) {
                    *((char *)uptr) = (char) t;
		}
		uptr += nb;
	    }
	}
	else {
	    for (i = 0; i < nelements; i++) {
                t = rint(*dptr++);
		if (nb == 4) {
		    *((unsigned int *)uptr) = (unsigned int) t;
		}
		else if (nb == 2) {
		    *((unsigned short *)uptr) = (unsigned short) t;
		}
		else if (nb == 1) {
		    *((unsigned char *)uptr) = (unsigned char) t;
		}
		uptr += nb;
	    }
	}
	break;

    case H5T_CONV_FREE:
	break;

    default:
	/* Unknown command */
	return (-1);
    }
    return (0);
}

/** Initialize some critical pieces of the library.
 */
void
miinit(void)
{
    H5Tregister(H5T_PERS_SOFT, "i2d", H5T_NATIVE_INT, H5T_NATIVE_DOUBLE,
                mi2_int_to_dbl);
    
    H5Tregister(H5T_PERS_SOFT, "d2i", H5T_NATIVE_DOUBLE, H5T_NATIVE_INT,
                mi2_dbl_to_int);
}


int
minc_create_thumbnail(hid_t file_id, int grp)
{
    char path[MAX_PATH];
    hid_t grp_id;

    /* Don't handle negative or overly large numbers!
     */
    if (grp <= 0 || grp > MAX_RESOLUTION_GROUP) {
	return (MI_ERROR);
    }

    sprintf(path, "/minc-2.0/image/%d", grp);
    grp_id = H5Gcreate(file_id, path, 0);
    if (grp_id < 0) {
        return (MI_ERROR);
    }
    H5Gclose(grp_id);
    return (MI_NOERROR);
}

/** Function to downsample a single slice of an image.
 * \param in_ptr the 3D input slice, scale x isize[1] x isize[2]
 * \param out_ptr the 2D output slice, osize[1] x osize[2]
 */
void
midownsample_slice(double *in_ptr, double *out_ptr, hsize_t isize[], 
                   hsize_t osize[], int scale)
{
    int j, k;
    int x, y, z;
    double d;
    hsize_t total;

    total = scale * scale * scale;

    /* These two loops iterate over all of the voxels in the 2D output
     * image.
     */
    for (j = 0; j < osize[1]; j++) {
        for (k = 0; k < osize[2]; k++) {
            /* The three inner loops iterate all scale^3 
             * voxels in the input image which will be averaged
             * to form the output image.
             */
            d = 0;
            for (x = 0; x < scale; x++) {
                for (y = 0; y < scale; y++) {
                    for (z = 0; z < scale; z++) {
                        int x1,y1,z1;
                        double t;

                        x1 = x;
                        y1 = y + (j * scale);
                        z1 = z + (k * scale);

                        t = in_ptr[((x1 * isize[1]) + y1) * isize[2] + z1];
                        d += t;
                    }
                }
            }

            d /= total;
            out_ptr[(j * osize[1]) + k] = d;
        }
    }
}

int
minc_update_thumbnail(hid_t loc_id, int igrp, int ogrp)
{
    hsize_t isize[MAX_VAR_DIMS];
    hsize_t osize[MAX_VAR_DIMS];
    hsize_t count[MAX_VAR_DIMS];
    hsize_t start[MAX_VAR_DIMS];
    hid_t idst_id;              /* Input dataset */
    hid_t odst_id;              /* Output dataset */
    hid_t ifspc_id;             /* Input "file" dataspace */
    hid_t ofspc_id;             /* Output "file" dataspace */
    hid_t typ_id;               /* Type ID */
    hid_t imspc_id;
    hid_t omspc_id;
    char path[MAX_PATH];
    int ndims;
    int scale;
    int i;
    double *in_ptr;
    double *out_ptr;
    int slice;
    int in_bytes;
    int out_bytes;
    


    miinit();

    /* Calculate scale factor (always a power of 2) */
    for (i = igrp, scale = 1; i < ogrp; i++, scale <<= 1)
	;

    /* Open the input path.
     */
    sprintf(path, "%d/image", igrp);
    idst_id = H5Dopen(loc_id, path);
    if (idst_id < 0) {
        return (MI_ERROR);
    }

    /* Get the input type.
     */
    typ_id = H5Dget_type(idst_id);

    /* Get the input dataspace.
     */
    ifspc_id = H5Dget_space(idst_id);

    ndims = H5Sget_simple_extent_ndims(ifspc_id);
    H5Sget_simple_extent_dims(ifspc_id, isize, NULL);

    /* Calculate the size of the new thumbnail.
     */
    for (i = 0; i < ndims; i++) {
	osize[i] = isize[i] / scale; 
	if (osize[i] == 0) {	/* Too small? */
	    return (MI_ERROR);
	}
    }

    ofspc_id = H5Screate_simple(ndims, osize, NULL);

    sprintf(path, "%d/image", ogrp);
    /* TODO: Non-default properties */
    odst_id = H5Dcreate(loc_id, path, typ_id, ofspc_id, H5P_DEFAULT);
    if (odst_id < 0) {
        odst_id = H5Dopen(loc_id, path);
    }

    /* Calculate the input buffer size - scale slices.
     */
    in_bytes = scale * isize[1] * isize[2] * sizeof(double);
    in_ptr = malloc(in_bytes);

    out_bytes = osize[1] * osize[2] * sizeof(double);
    out_ptr = malloc(out_bytes);

    count[0] = scale;
    count[1] = isize[1];
    count[2] = isize[2];
    imspc_id = H5Screate_simple(ndims, count, NULL);

    count[0] = 1;
    count[1] = osize[1];
    count[2] = osize[2];
    omspc_id = H5Screate_simple(ndims, count, NULL);

    //
    // read image & TODO: convert to "real" range.
    //
    for (slice = 0; slice < osize[0]; slice++) {
        
	fprintf(stderr, "Slice # %d\n", slice);
	start[0] = slice * scale;
	start[1] = 0;
	start[2] = 0;
	count[0] = scale;
	count[1] = isize[1];
	count[2] = isize[2];

	H5Sselect_hyperslab(ifspc_id, H5S_SELECT_SET, start, NULL, count, NULL);
	H5Dread(idst_id, H5T_NATIVE_DOUBLE, imspc_id, ifspc_id, H5P_DEFAULT, in_ptr);

        midownsample_slice(in_ptr, out_ptr, isize, osize, scale);

	start[0] = slice;
	start[1] = 0;
	start[2] = 0;
	count[0] = 1;
	count[1] = osize[1];
	count[2] = osize[2];
	H5Sselect_hyperslab(ofspc_id, H5S_SELECT_SET, start, NULL, count, NULL);
	H5Dwrite(odst_id, H5T_NATIVE_DOUBLE, omspc_id, ofspc_id, H5P_DEFAULT, 
                 out_ptr);
    }

    free(in_ptr);
    free(out_ptr);
    H5Sclose(omspc_id);
    H5Sclose(imspc_id);
    H5Dclose(odst_id);
    H5Tclose(typ_id);
    H5Sclose(ofspc_id);
    H5Sclose(ifspc_id);
    return (MI_NOERROR);
}

int
minc_update_thumbnails(hid_t file_id)
{
    int grp_no, prv_grp_no;
    hid_t grp_id;
    hsize_t n;
    hsize_t i;
    char name[128];
    size_t length;

    grp_id = H5Gopen(file_id, "/minc-2.0/image");
    if (grp_id >= 0) {
        if (H5Gget_num_objs(grp_id, &n) != 0) {
            for (i = 0; i < n; i++) {
                length = sizeof(name);
                H5Gget_objname_by_idx(grp_id, i, name, length);
                fprintf(stderr, "Found group %s\n", name);
                prv_grp_no = grp_no;
                grp_no = atoi(name);
                if (grp_no != 0) {
                    fprintf(stderr, "Updating group #%d from #%d\n", 
                            grp_no, prv_grp_no);
                    minc_update_thumbnail(grp_id, prv_grp_no, grp_no);
                }
            }
        }
        else {
            fprintf(stderr, "error getting object count?\n");
        }

        H5Gclose(grp_id);
    }
    else {
        fprintf(stderr, "error opening group?\n");
	
    }
    return (MI_NOERROR);

}

