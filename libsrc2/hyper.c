/** \file hyper.c
 * \brief MINC 2.0 Hypercube Functions
 * \author Bert Vincent
 *
 * Functions to manipulate hypercubes of volume image data.  Significant
 * portions of this code have been cut and pasted from netcdf 3.5.
 ************************************************************************/
#include <stdlib.h>
#include <hdf5.h>
#include "minc.h"
#include "minc2.h"
#include "minc2_private.h"

#define MIRW_OP_READ 1
#define MIRW_OP_WRITE 2

/** Given a set of dimensions, and a re-ordering dimension, figure out
 * how to remap the dimensions
 */
static int
micalc_mapping(int ndims, 
               const long lengths[],
               const int dimorder[], 
               const int dimdir[],
               long output_imap[],
               long *output_offset)
{
    int i;
    long input_imap[MAX_VAR_DIMS];

    /* Set up hypothetical imap variable for input.
     */
    input_imap[ndims - 1] = 1;
    for (i = ndims - 2; i >= 0; i--) {
        input_imap[i] = input_imap[i + 1] * lengths[i + 1];
    }

    *output_offset = 0;
    for (i = 0; i < ndims; i++) {
        int j = dimorder[i];
        if (dimdir[j] > 0) {
            output_imap[i] = input_imap[j];
        }
        else {
            output_imap[i] = -input_imap[j];
            *output_offset = (*output_offset) - 
                ((lengths[i] - 1) * output_imap[i]);
        }
    }
    return (MI_NOERROR);
}


/** Calculates and returns the number of bytes required to store the
 * hypercube specified by the \a n_dimensions and the 
 * \a count parameters.
 */
int
miget_hyperslab_size(mitype_t volume_data_type, /**< Data type of a voxel. */
		      int n_dimensions, /**< Dimensionality */
		      const long count[], /**< Dimension lengths  */
		      misize_t *size_ptr) /**< Returned byte count */
{
    int voxel_size;
    misize_t temp;
    int i;
    hid_t type_id;

    type_id = mitype_to_hdftype(volume_data_type);
    if (type_id < 0) {
        return (MI_ERROR);
    }

    voxel_size = H5Tget_size(type_id);

    temp = 1;
    for (i = 0; i < n_dimensions; i++) {
        temp *= count[i];
    }
    *size_ptr = (temp * voxel_size);
    H5Tclose(type_id);
    return (MI_NOERROR);
}

/** Read/write a hyperslab of data.  This is the simplified function
 * which performs no dimension reordering or value conversion.  It is
 * much more efficient than mirw_hyperslab_mapped() or (heaven forbid)
 * mirw_hyperslab_mapped_icv()
 */
static int
mirw_hyperslab(int opcode, 
               mihandle_t volume, 
               mitype_t midatatype, 
               const long start[], 
               const long count[],
               void *buffer)
{
    hid_t file_id;
    hid_t type_id = -1;
    hid_t dset_id = -1;
    hid_t mspc_id = -1;
    hid_t fspc_id = -1;
    char path[128];
    int result = MI_ERROR;
    hsize_t hdf_start[MAX_VAR_DIMS];
    hsize_t hdf_count[MAX_VAR_DIMS];
    int ndims;
    int i;

    file_id = miget_volume_file_handle(volume);

    type_id = mitype_to_hdftype(midatatype);

    /* TODO: select current resolution. */
    strcpy(path, "/minc-2.0/image/0/image");

    dset_id = H5Dopen(file_id, path);
    if (dset_id < 0) {
        goto cleanup;
    }

    fspc_id = H5Dget_space(dset_id);
    if (fspc_id < 0) {
        goto cleanup;
    }

    ndims = H5Sget_simple_extent_ndims(fspc_id);

    if (ndims == 0) {
        mspc_id = H5Screate(H5S_SCALAR);
    }
    else {
        for (i = 0; i < ndims; i++) {
            hdf_start[i] = start[i];
            hdf_count[i] = count[i];
        }
        mspc_id = H5Screate_simple(ndims, hdf_count, NULL);
        if (mspc_id < 0) {
            goto cleanup;
        }
    }

    H5Sselect_hyperslab(fspc_id, H5S_SELECT_SET, hdf_start, NULL, hdf_count,
                        NULL);

    if (opcode == MIRW_OP_READ) {
        result = H5Dread(dset_id, type_id, mspc_id, fspc_id, H5P_DEFAULT, 
                         buffer);
    }
    else {
        result = H5Dwrite(dset_id, type_id, mspc_id, fspc_id, H5P_DEFAULT, 
                          buffer);
    }

 cleanup:
    if (type_id >= 0) {
        H5Tclose(type_id);
    }
    if (dset_id >= 0) {
        H5Dclose(dset_id);
    }
    if (mspc_id >= 0) {
        H5Sclose(mspc_id);
    }
    if (fspc_id >= 0) {
        H5Sclose(fspc_id);
    }
    return (result);
}

/** Read/write a hyperslab of data, performing dimension remapping
 * as needed.
 */
static int 
mirw_hyperslab_mapped(int opcode, 
                      mihandle_t volume, 
                      mitype_t midatatype, 
                      const long *start, 
                      const long *edges, 
                      const long *stride, 
                      const long *map, 
                      void *buffer)
{
    int status = MI_ERROR;      /* Assume guilty */
    int maxidim;		/* maximum dimensional index */
    int idim;
    int64_t *mystart = NULL;
    int64_t *myedges;
    int64_t *iocount;	/* count vector */
    int64_t *stop;	/* stop indexes */
    int64_t *length;	/* edge lengths in bytes */
    int64_t *mystride;
    int64_t *mymap;
    int64_t *dims;
    int file_id = -1;
    int dset_id = -1;
    int type_id = -1;
    int fspc_id = -1;
    int mspc_id = -1;
    int ndims;
    char path[128];
    int nbytes;
    void *base;

    /* If stride and map not specified, do the more efficient thing...
     */
    if (stride == NULL && map == NULL) {
        return mirw_hyperslab(opcode, volume, midatatype, start, edges, 
                              buffer);
    }

    file_id = miget_volume_file_handle(volume);

    type_id = mitype_to_hdftype(midatatype);

    nbytes = H5Tget_size(type_id);

    /* TODO: select current resolution. */
    strcpy(path, "/minc-2.0/image/0/image");

    if ((dset_id = H5Dopen(file_id, path)) < 0) {
        goto cleanup;
    }

    if ((fspc_id = H5Dget_space(dset_id)) < 0) {
        goto cleanup;
    }

    ndims = H5Sget_simple_extent_ndims(fspc_id);

    maxidim = (int) ndims - 1;

    if (maxidim < 0) {
	/*
	 * TODO: The variable is a scalar!
	 */
        goto cleanup;
    }


    /*
     * Verify stride argument.
     */
    if (stride != NULL) {
	for (idim = 0; idim <= maxidim; idim++) {
	    if (stride[idim] == 0) {
                goto cleanup;
	    }
	}
    }

    mystart = (int64_t *)calloc(ndims * 8, sizeof(int64_t));
    if (mystart == NULL) {
        goto cleanup;
    }

    myedges = mystart + ndims;
    iocount = myedges + ndims;
    stop = iocount + ndims;
    length = stop + ndims;
    mystride = length + ndims;
    mymap = mystride + ndims;
    dims = mymap + ndims;

    H5Sget_simple_extent_dims(fspc_id, dims, NULL);

    /*
     * Initialize I/O parameters.
     */
    for (idim = maxidim; idim >= 0; --idim) {
        if (start != NULL) {
            mystart[idim] = start[idim];
        }
        else {
            mystart[idim] = 0;
        }

	if (edges != NULL) {
	    myedges[idim] = edges[idim];
            if (edges[idim] == 0) {
                goto cleanup;   /* Nothing to do here */
            }
	}
	else {
	    myedges[idim] = dims[idim] - mystart[idim];
	}

	if (stride != NULL) {
	    mystride[idim] = stride[idim];
	}
	else {
	    mystride[idim] = 1;
	}

	if (map != NULL) {
	    mymap[idim] = map[idim];
	}
	else if (idim == maxidim) {
	    mymap[idim] = 1;
	}
	else {
	    mymap[idim] = mymap[idim+1] * myedges[idim+1];
	}

	iocount[idim] = 1;
	length[idim] = mymap[idim] * myedges[idim];
	stop[idim] = mystart[idim] + myedges[idim] * mystride[idim];
    }

    /*
     * Check start, edges
     */
    for (idim = 0; idim < maxidim; idim++) {
	if (mystart[idim] >= dims[idim]) {
	    status = MI_ERROR;
	    goto cleanup;
	}
	if (mystart[idim] + myedges[idim] > dims[idim]) {
	    status = MI_ERROR;
	    goto cleanup;
	}

        if (mymap[idim] < 0 && mystart[idim] != 0) {
            printf("flipping %d\n", idim);
            mystart[idim] = (dims[idim] - 1) - mystart[idim];
        }
    }
    /*
     * As an optimization, adjust I/O parameters when the fastest 
     * dimension has unity stride both externally and internally.
     * In this case, the user could have called a simpler routine
     * (i.e. ncvarnc_put_vara_uchar()
     */
    if (mystride[maxidim] == 1 && mymap[maxidim] == 1) {
        printf("optimizing...\n");
	iocount[maxidim] = myedges[maxidim];
	mystride[maxidim] = myedges[maxidim];
	mymap[maxidim] = length[maxidim];
    }

    printf("start stop mystart iocount length mymap\n");
    for (idim = 0; idim < ndims; idim++) {
        printf("%5ld %4lld %7lld %7lld %6lld %5d\n", start[idim], stop[idim],
               mystart[idim], iocount[idim], length[idim], mymap[idim]);

    }

    mspc_id = H5Screate_simple(ndims, iocount, NULL);
    if (mspc_id < 0) {
        printf("can't create dataspace\n");
        goto cleanup;
    }

    /*
     * Perform I/O.  Exit when done.
     */
    
    base = buffer;

    for (;;) {
        status = H5Sselect_hyperslab(fspc_id, H5S_SELECT_SET, mystart, NULL, 
                                     iocount, NULL);
        if (status < 0) {
            printf("select failed\n");
            goto cleanup;
        }

        if (opcode == MIRW_OP_READ) {
            status = H5Dread(dset_id, type_id, mspc_id, fspc_id, H5P_DEFAULT, 
                             buffer);
        }
        else {
            status = H5Dwrite(dset_id, type_id, mspc_id, fspc_id, H5P_DEFAULT, 
                              buffer);
        }
        if (status < 0) {
            printf("I/O failed\n");
            goto cleanup;
        }

	/*
	 * The following code permutes through the variable's
	 * external start-index space and it's internal address
	 * space.  At the UPC, this algorithm is commonly
	 * called "odometer code".
	 */
	idim = maxidim;
    carry:
	buffer += (mymap[idim] * nbytes);
	mystart[idim] += mystride[idim];
	if (mystart[idim] == stop[idim]) {
	    mystart[idim] = start[idim];
	    buffer -= (length[idim] * nbytes);
	    if (--idim < 0)
		break; /* normal return */
	    goto carry;
	}
    } /* I/O loop */

 cleanup:
    if (mystart != NULL) {
        free(mystart);
    }
    if (mspc_id >= 0) {
        H5Sclose(mspc_id);
    }
    if (fspc_id >= 0) {
        H5Sclose(fspc_id);
    }
    if (type_id >= 0) {
        H5Tclose(type_id);
    }
    if (dset_id >= 0) {
        H5Dclose(dset_id);
    }
    return (status);
}

/** Read/write a hyperslab of data, performing dimension remapping
 * and data rescaling as needed.
 */
static int 
mirw_hyperslab_mapped_icv(int opcode, 
                          int icv,
                          const long *start, 
                          const long *edges, 
                          const long *stride, 
                          const long *map, 
                          void *buffer)
{
    int status = MI_ERROR;      /* Assume guilty */
    int maxidim;		/* maximum dimensional index */
    int idim;
    long *mystart = NULL;
    long *myedges;
    long *iocount;	/* count vector */
    long *stop;	/* stop indexes */
    long *length;	/* edge lengths in bytes */
    ptrdiff_t *mystride;
    ptrdiff_t *mymap;
    long *dims;
    int file_id = -1;
    int ndims;
    int nbytes;
    int var_id;
    int nc_type;
    int dimids[MAX_VAR_DIMS];

    if (stride == NULL && map == NULL) {
        if (opcode == MIRW_OP_READ) {
            return miicv_get(icv, (long *) start, (long *) edges, buffer);
        }
        else {
            return miicv_put(icv, (long *) start, (long *) edges, buffer);
        }
    }

    miicv_inqint(icv, MI_ICV_VARID, &var_id);
    miicv_inqint(icv, MI_ICV_TYPE, &nc_type);
    miicv_inqint(icv, MI_ICV_CDFID, &file_id);

    nbytes = MI2typelen(nc_type);

    ncvarinq(file_id, var_id, NULL, NULL, &ndims, dimids, NULL);

    maxidim = (int) ndims - 1;

    if (maxidim < 0) {
	/*
	 * TODO: The variable is a scalar!
	 */
        goto cleanup;
    }


    /*
     * Verify stride argument.
     */
    if (stride != NULL) {
	for (idim = 0; idim <= maxidim; idim++) {
	    if (stride[idim] == 0) {
                goto cleanup;
	    }
	}
    }

    mystart = (long *)calloc(ndims * 8, sizeof (ptrdiff_t));
    if (mystart == NULL) {
        goto cleanup;
    }

    myedges = mystart + ndims;
    iocount = myedges + ndims;
    stop = iocount + ndims;
    length = stop + ndims;
    mystride = (ptrdiff_t *)(length + ndims);
    mymap = mystride + ndims;
    dims = (long *) (mymap + ndims);

    /* Get the lengths of all of the dimensions.
     */
    for (idim = 0; idim < ndims; idim++) {
        ncdiminq(file_id, dimids[idim], NULL, &dims[idim]);
    }
               
    /*
     * Initialize I/O parameters.
     */
    for (idim = maxidim; idim >= 0; --idim) {
        if (start != NULL) {
            mystart[idim] = start[idim];
        }
        else {
            mystart[idim] = 0;
        }

	if (edges != NULL) {
	    myedges[idim] = edges[idim];
            if (edges[idim] == 0) {
                goto cleanup;   /* Nothing to do here */
            }
	}
	else {
	    myedges[idim] = dims[idim] - mystart[idim];
	}

	if (stride != NULL) {
	    mystride[idim] = stride[idim];
	}
	else {
	    mystride[idim] = 1;
	}

	if (map != NULL) {
	    mymap[idim] = map[idim];
	}
	else if (idim == maxidim) {
	    mymap[idim] = 1;
	}
	else {
	    mymap[idim] = mymap[idim+1] * (ptrdiff_t) myedges[idim+1];
	}

	iocount[idim] = 1;
	length[idim] = mymap[idim] * myedges[idim];
	stop[idim] = mystart[idim] + myedges[idim] * mystride[idim];
    }

    /*
     * Check start, edges
     */
    for (idim = 0; idim < maxidim; idim++) {
	if (mystart[idim] >= dims[idim]) {
	    status = MI_ERROR;
	    goto cleanup;
	}
	if (mystart[idim] + myedges[idim] > dims[idim]) {
	    status = MI_ERROR;
	    goto cleanup;
	}
    }
    /*
     * As an optimization, adjust I/O parameters when the fastest 
     * dimension has unity stride both externally and internally.
     * In this case, the user could have called a simpler routine
     * (i.e. ncvarnc_put_vara_uchar()
     */
    if (mystride[maxidim] == 1 && mymap[maxidim] == 1) {
	iocount[maxidim] = myedges[maxidim];
	mystride[maxidim] = (ptrdiff_t) myedges[maxidim];
	mymap[maxidim] = (ptrdiff_t) length[maxidim];
    }

    /*
     * Perform I/O.  Exit when done.
     */
    for (;;) {
        if (opcode == MIRW_OP_READ) {
            status = miicv_get(icv, mystart, iocount, buffer);
        }
        else {
            status = miicv_put(icv, mystart, iocount, buffer);
        }
        if (status < 0) {
            break;
        }

	/*
	 * The following code permutes through the variable's
	 * external start-index space and it's internal address
	 * space.  At the UPC, this algorithm is commonly
	 * called "odometer code".
	 */
	idim = maxidim;
    carry:
	buffer += (mymap[idim] * nbytes);
	mystart[idim] += mystride[idim];
	if (mystart[idim] == stop[idim]) {
	    mystart[idim] = start[idim];
	    buffer -= (length[idim] * nbytes);
	    if (--idim < 0)
		break; /* normal return */
	    goto carry;
	}
    } /* I/O loop */

 cleanup:
    if (mystart != NULL) {
        free(mystart);
    }
    return (status);
}

/** Reads the real values in the volume from the interval min through
 *  max, mapped to the maximum representable range for the requested
 *  data type. Float type is NOT an allowed data type.  
 */
int
miget_hyperslab_normalized(mihandle_t volume, 
                            mitype_t buffer_data_type,
                            const long start[], 
                            const long count[],
                            double min, 
                            double max, 
                            void *buffer) 
{ 
    hid_t file_id;
    int var_id;
    int icv;
    int result;
    int is_signed;
    int nctype;

    if (min > max) {
        return (MI_ERROR);
    }

    file_id = miget_volume_file_handle(volume);
    if (file_id < 0) {
        return (MI_ERROR);
    }

    var_id = ncvarid(file_id, MIimage);
    if (var_id < 0) {
        return (MI_ERROR);
    }

    nctype = mitype_to_nctype(buffer_data_type, &is_signed);

    if (nctype == NC_FLOAT || nctype == NC_DOUBLE) {
        return (MI_ERROR);
    }

    if ((icv = miicv_create()) < 0) {
	return (MI_ERROR);
    }

    result = miicv_setint(icv, MI_ICV_TYPE, nctype);
    result = miicv_setstr(icv, MI_ICV_SIGN, is_signed ? MI_SIGNED : MI_UNSIGNED);
    result = miicv_setdbl(icv, MI_ICV_IMAGE_MIN, min);
    result = miicv_setdbl(icv, MI_ICV_IMAGE_MAX, max);
    result = miicv_setint(icv, MI_ICV_USER_NORM, TRUE);
    result = miicv_setint(icv, MI_ICV_DO_NORM, TRUE);

    result = miicv_attach(icv, file_id, var_id);
    if (result == MI_NOERROR) {
        result = mirw_hyperslab_mapped_icv(MIRW_OP_READ, icv, start, count, 
                                           NULL, NULL, buffer);
	miicv_detach(icv);
    }
    miicv_free(icv);
    return (result);
}

/** Get a hypercube from the file, with the assistance of a MINC image
 * conversion variable (ICV).
 */
int
miget_hyperslab_with_icv(mihandle_t volume, /**< A MINC 2.0 volume handle */
			  int icv, /**< The ICV to use */
			  mitype_t buffer_data_type, /**< Output datatype */
			  const long start[], /**< Start coordinates  */
			  const long count[], /**< Lengths of edges  */
			  void *buffer) /**< Output memory buffer */
{
    hid_t file_id;
    int var_id;
    int result;
    int is_signed;
    int nctype;

    file_id = miget_volume_file_handle(volume);

    var_id = ncvarid(file_id, MIimage);

    nctype = mitype_to_nctype(buffer_data_type, &is_signed);

    miicv_setint(icv, MI_ICV_TYPE, nctype);
    miicv_setstr(icv, MI_ICV_SIGN, is_signed ? MI_SIGNED : MI_UNSIGNED);

    result = miicv_attach(icv, file_id, var_id);
    if (result == MI_NOERROR) {
        result = mirw_hyperslab_mapped_icv(MIRW_OP_READ, icv, start, count, 
                                           NULL, NULL, buffer);
	miicv_detach(icv);
    }
    return (result);
}

/** Write a hypercube to the file, with the assistance of a MINC image
 * conversion variable (ICV).
 */
int
miset_hyperslab_with_icv(mihandle_t volume, /**< A MINC 2.0 volume handle */
			  int icv, /**< The ICV to use */
			  mitype_t buffer_data_type, /**< Output datatype */
			  const long start[], /**< Start coordinates  */
			  const long count[], /**< Lengths of edges  */
			  const void *buffer) /**< Output memory buffer */
{
    hid_t file_id;
    int var_id;
    int result;
    int is_signed;
    int nctype;

    file_id = miget_volume_file_handle(volume);

    var_id = ncvarid(file_id, MIimage);

    nctype = mitype_to_nctype(buffer_data_type, &is_signed);

    miicv_setint(icv, MI_ICV_TYPE, nctype);
    miicv_setstr(icv, MI_ICV_SIGN, is_signed ? MI_SIGNED : MI_UNSIGNED);

    result = miicv_attach(icv, file_id, var_id);
    if (result == MI_NOERROR) {
	result = mirw_hyperslab_mapped_icv(MIRW_OP_WRITE, 
                                           icv, 
                                           start, 
                                           count, 
                                           NULL, 
                                           NULL, 
                                           (void *) buffer);
	miicv_detach(icv);
    }
    return (result);
}

/** Read a hypercube from the file into the preallocated buffer,
 *  converting from the stored "voxel" data range to the desired
 * "real" (float or double) data range.
 */
int
miget_real_value_hyperslab(mihandle_t volume,
			    mitype_t buffer_data_type,
			    const long start[],
			    const long count[],
			    void *buffer)
{
    hid_t file_id;
    int var_id;
    int icv;
    int result;
    int is_signed;
    int nctype;

    file_id = miget_volume_file_handle(volume);

    var_id = ncvarid(file_id, MIimage);

    nctype = mitype_to_nctype(buffer_data_type, &is_signed);

    if ((icv = miicv_create()) < 0) {
        return (MI_ERROR);
    }

    miicv_setint(icv, MI_ICV_TYPE, nctype);
    miicv_setstr(icv, MI_ICV_SIGN, is_signed ? MI_SIGNED : MI_UNSIGNED);
    miicv_setint(icv, MI_ICV_DO_RANGE, TRUE);
    miicv_setint(icv, MI_ICV_DO_NORM, FALSE);

    result = miicv_attach(icv, file_id, var_id);
    if (result == MI_NOERROR) {
	result = mirw_hyperslab_mapped_icv(MIRW_OP_READ,
                                           icv, 
                                           (long *) start, 
                                           (long *) count, 
                                           NULL,
                                           NULL,
                                           (void *) buffer);
	miicv_detach(icv);
    }
    miicv_free(icv);
    return (result);
}

/** Write a hypercube to the file from the preallocated buffer,
 *  converting from the stored "voxel" data range to the desired
 * "real" (float or double) data range.
 */
int
miset_real_value_hyperslab(mihandle_t volume,
			    mitype_t buffer_data_type,
			    const long start[],
			    const long count[],
			    const void *buffer)
{
    hid_t file_id;
    int var_id;
    int icv;
    int result;
    int is_signed;
    int nctype;

    file_id = miget_volume_file_handle(volume);

    var_id = ncvarid(file_id, MIimage);

    nctype = mitype_to_nctype(buffer_data_type, &is_signed);

    if ((icv = miicv_create()) < 0) {
	return (MI_ERROR);
    }

    miicv_setint(icv, MI_ICV_TYPE, nctype);
    miicv_setstr(icv, MI_ICV_SIGN, is_signed ? MI_SIGNED : MI_UNSIGNED);

    result = miicv_attach(icv, file_id, var_id);
    if (result == MI_NOERROR) {
	result = mirw_hyperslab_mapped_icv(MIRW_OP_WRITE, 
                                           icv, 
                                           (long *) start, 
                                           (long *) count, 
                                           NULL, 
                                           NULL,
                                           (void *) buffer);
	miicv_detach(icv);
    }
    miicv_free(icv);
    return (result);
}

/** Read a hypercube from the file into the preallocated buffer,
 * with no range conversions or normalization.  Type conversions will
 * be performed if necessary.
 */
int
miget_voxel_value_hyperslab(mihandle_t volume,
			     mitype_t buffer_data_type,
			     const long start[],
			     const long count[],
			     void *buffer)
{
    return mirw_hyperslab_mapped(MIRW_OP_READ, volume, buffer_data_type, 
                                 start, count, NULL, NULL, buffer);
}

/** Write a hypercube to the file from the preallocated buffer,
 * with no range conversions or normalization.  Type conversions will
 * be performed if necessary.
 */
int
miset_voxel_value_hyperslab(mihandle_t volume,
			     mitype_t buffer_data_type,
			     const long start[],
			     const long count[],
			     const void *buffer)
{
    return mirw_hyperslab_mapped(MIRW_OP_WRITE, volume, buffer_data_type, 
                                 start, count, NULL, NULL, (void *) buffer);
}


#ifdef M2_TEST

#define NDIMS 3

#define CX 3
#define CY 3
#define CZ 3

#define TESTRPT(msg, val) (error_cnt++, fprintf(stderr, \
                                  "Error reported on line #%d, %s: %d\n", \
                                  __LINE__, msg, val))
static int error_cnt = 0;

int 
main(int argc, char **argv)
{
    mihandle_t hvol;
    int result;
    long start[NDIMS];
    static long count[NDIMS] = {CX,CY,CZ};
    int itemp[CX][CY][CZ];
    double dtemp[CX][CY][CZ];
    int itemp2[CX][CY][CZ];
    double dtemp2[CX][CY][CZ];
    unsigned short stemp[CX][CY][CZ];
    unsigned char btemp[CX][CY][CZ];
    unsigned short stmp2[CX][CY][CZ];
    int i,j,k;
    int n;
    misize_t nbytes;
    hid_t file_id;
    long imap[NDIMS];
    long offset;
    long sizes[3] = {3,3,3};
    int dimorder[3] = {0,1,2};
    int dimdir[3] = { -1,-1,-1};
    unsigned short stmp3[2][2][2];

    result = miopen_volume("hyper.mnc", MI2_OPEN_READ, &hvol);
    if (result < 0) {
        TESTRPT("Unable to open input file", result);
    }
    else {
        start[0] = start[1] = start[2] = 0;
        i = miget_real_value_hyperslab(hvol, MI_TYPE_DOUBLE, start, count,
                                        dtemp);
        
        printf("miget_real_value_hyperslab()\n");
        if (i < 0) {
            printf("oops\n");
        }
        else {
            for (i = 0; i < CX; i++) {
                for (j = 0; j < CY; j++) {
                    printf("[%d][%d]: ", i, j);
                    for (k = 0; k < CZ; k++) {
                        printf("%f ", dtemp[i][j][k]);
                    }
                    printf("\n");
                }
            }
        }

        printf("miget_voxel_value_hyperslab()\n");

        i = miget_voxel_value_hyperslab(hvol, MI_TYPE_USHORT, start, count,
                                         stemp);
        if (i < 0) {
            printf("oops\n");
        }
        else {
            for (i = 0; i < CX; i++) {
                for (j = 0; j < CY; j++) {
                    printf("[%d][%d]: ", i, j);
                    for (k = 0; k < CZ; k++) {
                        printf("%u ", stemp[i][j][k]);
                    }
                    printf("\n");
                }
            }
        }

        printf("miget_hyperslab_normalized()\n");

        i = miget_hyperslab_normalized(hvol, MI_TYPE_UBYTE, start, count,
                                        0.0, 4096.0 * 102.0, btemp);
        if (i < 0) {
            printf("oops\n");
        }
        else {
            for (i = 0; i < CX; i++) {
                for (j = 0; j < CY; j++) {
                    printf("[%d][%d]: ", i, j);
                    for (k = 0; k < CZ; k++) {
                        printf("%u ", btemp[i][j][k]);
                    }
                    printf("\n");
                }
            }
        }

        printf("mirw_hyperslab_mapped()\n");

        micalc_mapping(3, sizes, dimorder, dimdir, imap, &offset);
        printf("offset %ld ", offset);
        for (i = 0; i < 3; i++) {
            printf("imap[%d] = %ld ", i, imap[i]);
        }
        printf("\n");
        fflush(stdout);
  

        file_id = miget_volume_file_handle(hvol);
        i = mirw_hyperslab_mapped(MIRW_OP_READ, hvol, MI_TYPE_USHORT, start, 
                                  count, NULL, imap, 
                                  (short *)stmp2 + offset);
        if (i < 0) {
            fprintf(stderr, "oops\n");
        }
        else {
            int d[3];
            for (d[0] = 0; d[0] < CX; d[0]++) {
                for (d[1] = 0; d[1] < CY; d[1]++) {
                    printf("[%02d][%02d]: ", d[0], d[1]);
                    for (d[2] = 0; d[2] < CZ; d[2]++) {
                        printf("%04u", stmp2[d[0]][d[1]][d[2]]);
                        if (stemp[d[dimorder[0]]][d[dimorder[1]]][d[dimorder[2]]] != 
                            stmp2[d[0]][d[1]][d[2]]) {
                            printf("!");
                        }
                        printf(" ");
                    }
                    printf("\n");
                }
            }
        }


        printf("mirw_hyperslab_mapped()\n");

        sizes[0] = sizes[1] = sizes[2] = 2;
        micalc_mapping(3, sizes, dimorder, dimdir, imap, &offset);
        printf("offset %ld ", offset);
        for (i = 0; i < 3; i++) {
            printf("imap[%d] = %ld ", i, imap[i]);
        }
        printf("\n");
        fflush(stdout);
  

        file_id = miget_volume_file_handle(hvol);
        start[0] = start[1] = start[2] = 1;
        count[0] = count[1] = count[2] = 2;
        i = mirw_hyperslab_mapped(MIRW_OP_READ, hvol, MI_TYPE_USHORT, start, 
                                  count, NULL, imap, 
                                  (short *)stmp3 + offset);
        if (i < 0) {
            fprintf(stderr, "oops\n");
        }
        else {
            int d[3];
            for (d[0] = 0; d[0] < 2; d[0]++) {
                for (d[1] = 0; d[1] < 2; d[1]++) {
                    printf("[%02d][%02d]: ", d[0], d[1]);
                    for (d[2] = 0; d[2] < 2; d[2]++) {
                        printf("%04u", stmp3[d[0]][d[1]][d[2]]);
                        printf(" ");
                    }
                    printf("\n");
                }
            }
        }

	miclose_volume(hvol);
    }
    return (error_cnt);
}
#endif /* M2_TEST */
