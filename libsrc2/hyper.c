/** \file hyper.c
 * \brief MINC 2.0 Hyperslab Functions
 * \author Bert Vincent
 *
 * Functions to manipulate hyperslabs of volume image data.
 ************************************************************************/
#include <stdlib.h>
#include <hdf5.h>
#include "minc.h"
#include "minc2.h"
#include "minc2_private.h"

#define MIRW_OP_READ 1
#define MIRW_OP_WRITE 2

typedef unsigned long mioffset_t;

/** In-place array dimension restructuring.
 *
 * Based on Chris H.Q. Ding, "An Optimal Index Reshuffle Algorithm for
 * Multidimensional Arrays and its Applications for Parallel Architectures"
 * IEEE Transactions on Parallel and Distributed Systems, Vol.12, No.3,
 * March 2001, pp.306-315.
 * 
 * I rewrote the algorithm in "C" an generalized it to N dimensions.
 *
 * Guaranteed to do the minimum number of memory moves, but requires
 * that we allocate a bitmap of nelem/8 bytes.  The paper suggests
 * ways to eliminate the bitmap - I'll work on it.
 */

/**
 * Map a set of array coordinates to a linear offset in the array memory.
 */
static mioffset_t
index_to_offset(int ndims, 
                const unsigned long sizes[], 
                const unsigned long index[])
{
    mioffset_t offset = index[0];
    int i;

    for (i = 1; i < ndims; i++) {
        offset *= sizes[i];
        offset += index[i];
    }
    return (offset);
}

/**
 * Map a linear offset to a set of coordinates in a multidimensional array.
 */
static void
offset_to_index(int ndims, 
                const unsigned long sizes[], 
                mioffset_t offset, 
                unsigned long index[])
{
    int i;

    for (i = ndims - 1; i > 0; i--) {
        index[i] = offset % sizes[i];
        offset /= sizes[i];
    }
    index[0] = offset;
}

/* Trivial bitmap test & set.
 */
#define BIT_TST(bm, i) (bm[(i) / 8] & (1 << ((i) % 8)))
#define BIT_SET(bm, i) (bm[(i) / 8] |= (1 << ((i) % 8)))

/** The main restructuring code.
 */
void
restructure_array(int ndims, 
                  unsigned char *array, 
                  const unsigned long *lengths, 
                  int el_size,
                  const int *map,
                  const int *dir)
{
    unsigned long index[MI2_MAX_VAR_DIMS];
    unsigned long index_perm[MI2_MAX_VAR_DIMS];
    unsigned long lengths_perm[MI2_MAX_VAR_DIMS];
    unsigned char *temp;
    mioffset_t offset_start;
    mioffset_t offset_next;
    mioffset_t offset;
    unsigned char *bitmap;
    size_t total;
    int i;

    if ((temp = malloc(el_size)) == NULL) {
        return;
    }

    /**
     * Calculate the total size of the array, in elements.
     **/
    total = 1;
    for (i = 0; i < ndims; i++) {
        total *= lengths[i];
    }

    /**
     * Allocate a bitmap with enough space to hold one bit for each
     * element in the array.
     **/
    bitmap = calloc((total + 8 - 1) / 8, 1); /* bit array */

    /**
     * Permute the lengths into their "output" configuration.
     **/
    for (i = 0; i < ndims; i++) {
        lengths_perm[i] = lengths[map[i]];
    }

    for (offset_start = 0; offset_start < total; offset_start++) {

        /**
         * Look for an unset bit - that's where we start the next
         * cycle.
         **/

        if (!BIT_TST(bitmap, offset_start)) {

            /**
             * Found a cycle we have not yet performed.
             **/

            offset_next = -1;   /* Initialize. */

#ifdef DEBUG
            printf("%ld", offset_start);
#endif /* DEBUG */

            /** 
             * Save the first element in this cycle.
             **/

            memcpy(temp, array + (offset_start * el_size), el_size);

            /**
             * We've touched this location.
             **/

            BIT_SET(bitmap, offset_start);

            offset = offset_start;

            /**
             * Do until the cycle repeats.
             **/

            while (offset_next != offset_start) {

                /**
                 * Compute the index from the offset and permuted length.
                 **/

                offset_to_index(ndims, lengths_perm, offset, index);
        
                /**
                 * Permute the index into the alternate arrangement.
                 **/

                for (i = 0; i < ndims; i++) {
                    if (dir[i] < 0) {
                        index_perm[map[i]] = lengths_perm[i] - index[i] - 1;
                    }
                    else {
                        index_perm[map[i]] = index[i];
                    }
                }

                /**
                 * Calculate the next offset from the permuted index.
                 **/

                offset_next = index_to_offset(ndims, lengths, index_perm);

                /**
                 * If we are not at the end of the cycle...
                 **/

                if (offset_next != offset_start) {

                    /**
                     * Note that we've touched a new location.
                     **/

                    BIT_SET(bitmap, offset_next);

#ifdef DEBUG
                    printf(" - %ld", offset_next);
#endif /* DEBUG */
                    
                    /**
                     * Move from old to new location.
                     **/

                    memcpy(array + (offset * el_size), 
                           array + (offset_next * el_size), 
                           el_size);

                    /** 
                     * Advance offset to the next location in the cycle.
                     **/

                    offset = offset_next;
                }
            }

            /**
             * Store the first value in the cycle, which we saved in
             * 'tmp', into the last offset in the cycle.
             **/

            memcpy(array + (offset * el_size), temp, el_size);

#ifdef DEBUG
            printf("\n");
#endif /* DEBUG */
        }
    }

    free(bitmap);               /* Get rid of the bitmap. */
    free(temp);
}

/** Calculates and returns the number of bytes required to store the
 * hyperslab specified by the \a n_dimensions and the 
 * \a count parameters.
 */
int
miget_hyperslab_size(mitype_t volume_data_type, /**< Data type of a voxel. */
                     int n_dimensions, /**< Dimensionality */
                     const unsigned long count[], /**< Dimension lengths  */
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

/** "semiprivate" function for translating coordinates.
 */
int
mitranslate_hyperslab_origin(mihandle_t volume,
                             const unsigned long start[],
                             const unsigned long count[],
                             hssize_t hdf_start[],
                             hsize_t hdf_count[],
                             int dir[])
{
    int n_different = 0;
    int file_i;
    int ndims = volume->number_of_dims;

    for (file_i = 0; file_i < ndims; file_i++) {
        midimhandle_t hdim;
        int user_i;

        /* Set up the basic translations of dimensions, for
         * flipping directions and swapping indices.
         */
        if (volume->dim_indices != NULL) {
            /* Have to swap indices */
            user_i = volume->dim_indices[file_i];  
            if (user_i != file_i) {
                n_different++;
            }
        }
        else {
            user_i = file_i;
        }

        hdim = volume->dim_handles[file_i];

        switch (hdim->flipping_order) {
        case MI_FILE_ORDER:
            hdf_start[file_i] = start[user_i];
            dir[file_i] = 1;
            break;

        case MI_COUNTER_FILE_ORDER:
            hdf_start[file_i] = hdim->length - start[user_i] - count[user_i];
            dir[file_i] = -1;
            break;
            
        case MI_POSITIVE:
            if (hdim->step > 0) { /* Positive? */
                hdf_start[file_i] = start[user_i]; /* Use raw file order. */
                dir[file_i] = 1;
            }
            else {
                hdf_start[file_i] = hdim->length - start[user_i] - count[user_i];
                dir[file_i] = -1;
            }
            break;

        case MI_NEGATIVE:
            if (hdim->step < 0) { /* Negative? */
                hdf_start[file_i] = start[user_i]; /* Use raw file order */
                dir[file_i] = 1;
            }
            else {
                hdf_start[file_i] = hdim->length - start[user_i] - count[user_i];
                dir[file_i] = -1;
            }
            break;
        }
        hdf_count[file_i] = count[user_i];
    }
    return (n_different);
}

/** Read/write a hyperslab of data.  This is the simplified function
 * which performs no value conversion.  It is much more efficient than
 * mirw_hyperslab_icv()
 */
static int
mirw_hyperslab_raw(int opcode, 
                   mihandle_t volume, 
                   mitype_t midatatype, 
                   const unsigned long start[], 
                   const unsigned long count[],
                   void *buffer)
{
    hid_t file_id;
    hid_t type_id = -1;
    hid_t dset_id = -1;
    hid_t mspc_id = -1;
    hid_t fspc_id = -1;
    char path[MI2_MAX_PATH];
    int result = MI_ERROR;
    hssize_t hdf_start[MI2_MAX_VAR_DIMS];
    hsize_t hdf_count[MI2_MAX_VAR_DIMS];
    int dir[MI2_MAX_VAR_DIMS];
    int ndims;
    int n_different = 0;

    file_id = volume->hdf_id;

    if (midatatype == MI_TYPE_UNKNOWN) {
        type_id = H5Tcopy(volume->type_id);
    }
    else {
        type_id = mitype_to_hdftype(midatatype);
    }

    sprintf(path, "/minc-2.0/image/%d/image", volume->selected_resolution);

    dset_id = H5Dopen(file_id, path);
    if (dset_id < 0) {
        goto cleanup;
    }

    fspc_id = H5Dget_space(dset_id);
    if (fspc_id < 0) {
        goto cleanup;
    }

    ndims = volume->number_of_dims;

    if (ndims == 0) {
        /* A scalar volume is possible but extremely unlikely, not to 
         * mention useless!
         */
        mspc_id = H5Screate(H5S_SCALAR);
    }
    else {
        n_different = mitranslate_hyperslab_origin(volume, 
                                                   start,
                                                   count,
                                                   hdf_start,
                                                   hdf_count,
                                                   dir);

        mspc_id = H5Screate_simple(ndims, hdf_count, NULL);
        if (mspc_id < 0) {
            goto cleanup;
        }
    }

    result = H5Sselect_hyperslab(fspc_id, H5S_SELECT_SET, hdf_start, NULL, 
                                 hdf_count, NULL);
    if (result < 0) {
        goto cleanup;
    }

    if (opcode == MIRW_OP_READ) {
        result = H5Dread(dset_id, type_id, mspc_id, fspc_id, H5P_DEFAULT, 
                         buffer);
        /* Restructure the array after reading the data in file orientation.
         * Count must be in raw order here.
         */
        if (n_different != 0) {
            restructure_array(ndims, buffer, count, H5Tget_size(type_id), 
                              volume->dim_indices, dir);
        }
    }
    else {
        /* Restructure array before writing to file.
         * Count must be in raw order here.
         */
        if (n_different != 0) {
            restructure_array(ndims, buffer, count, H5Tget_size(type_id), 
                              volume->dim_indices, dir);
        }
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
 * and data rescaling as needed.
 */
static int 
mirw_hyperslab_icv(int opcode, 
                   mihandle_t volume,
                   int icv,
                   const unsigned long start[], 
                   const unsigned long count[], 
                   void *buffer)
{
    int ndims;
    int nbytes;
    int nc_type;
    int file_id;
    int result = MI_ERROR;
    long icv_start[MI2_MAX_VAR_DIMS];
    long icv_count[MI2_MAX_VAR_DIMS];
    int dir[MI2_MAX_VAR_DIMS];  /* Direction, either +1 or -1 */
    int n_different = 0;

    miicv_inqint(icv, MI_ICV_TYPE, &nc_type);

    nbytes = MI2typelen(nc_type);

    file_id = volume->hdf_id;

    ndims = volume->number_of_dims;

    if (ndims != 0) {
        int i;
        hssize_t hdf_start[MI2_MAX_VAR_DIMS];
        hsize_t hdf_count[MI2_MAX_VAR_DIMS];

        n_different = mitranslate_hyperslab_origin(volume, 
                                                   start,
                                                   count,
                                                   hdf_start,
                                                   hdf_count,
                                                   dir);
        for (i = 0; i < ndims; i++) {
            icv_start[i] = hdf_start[i];
            icv_count[i] = hdf_count[i];
        }
    }

    if (opcode == MIRW_OP_READ) {
        result = miicv_get(icv, icv_start, icv_count, buffer);

        /* Now we have to restructure the array.
         * Count must be in raw order here.
         */
        if (result == MI_NOERROR && n_different != 0) {
            restructure_array(ndims, buffer, count, nbytes,
                              volume->dim_indices, dir);
        }
    }
    else {
        /* Restructure the data before writing.
         * Count must be in raw order here.
         */
        if (n_different != 0) {
            restructure_array(ndims, buffer, count, nbytes,
                              volume->dim_indices, dir);
        }

        result = miicv_put(icv, icv_start, icv_count, buffer);
    }

    return (result);
}

/** Reads the real values in the volume from the interval min through
 *  max, mapped to the maximum representable range for the requested
 *  data type. Float type is NOT an allowed data type.  
 */
int
miget_hyperslab_normalized(mihandle_t volume, 
                           mitype_t buffer_data_type,
                           const unsigned long start[], 
                           const unsigned long count[],
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

    file_id = volume->hdf_id;
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
        result = mirw_hyperslab_icv(MIRW_OP_READ, volume, icv, start, count, 
                                    buffer);
	miicv_detach(icv);
    }
    miicv_free(icv);
    return (result);
}

/** Get a hyperslab from the file, with the assistance of a MINC image
 * conversion variable (ICV).
 */
int
miget_hyperslab_with_icv(mihandle_t volume, /**< A MINC 2.0 volume handle */
			  int icv, /**< The ICV to use */
			  mitype_t buffer_data_type, /**< Output datatype */
			  const unsigned long start[], /**< Start coordinates  */
			  const unsigned long count[], /**< Lengths of edges  */
			  void *buffer) /**< Output memory buffer */
{
    hid_t file_id;
    int var_id;
    int result;
    int is_signed;
    int nctype;

    file_id = volume->hdf_id;

    var_id = ncvarid(file_id, MIimage);

    nctype = mitype_to_nctype(buffer_data_type, &is_signed);

    miicv_setint(icv, MI_ICV_TYPE, nctype);
    miicv_setstr(icv, MI_ICV_SIGN, is_signed ? MI_SIGNED : MI_UNSIGNED);

    result = miicv_attach(icv, file_id, var_id);
    if (result == MI_NOERROR) {
        result = mirw_hyperslab_icv(MIRW_OP_READ, volume, icv, start, count, 
                                    buffer);
	miicv_detach(icv);
    }
    return (result);
}

/** Write a hyperslab to the file, with the assistance of a MINC image
 * conversion variable (ICV).
 */
int
miset_hyperslab_with_icv(mihandle_t volume, /**< A MINC 2.0 volume handle */
			  int icv, /**< The ICV to use */
			  mitype_t buffer_data_type, /**< Output datatype */
			  const unsigned long start[], /**< Start coordinates  */
			  const unsigned long count[], /**< Lengths of edges  */
			  const void *buffer) /**< Output memory buffer */
{
    hid_t file_id;
    int var_id;
    int result;
    int is_signed;
    int nctype;

    file_id = volume->hdf_id;

    var_id = ncvarid(file_id, MIimage);

    nctype = mitype_to_nctype(buffer_data_type, &is_signed);

    miicv_setint(icv, MI_ICV_TYPE, nctype);
    miicv_setstr(icv, MI_ICV_SIGN, is_signed ? MI_SIGNED : MI_UNSIGNED);

    result = miicv_attach(icv, file_id, var_id);
    if (result == MI_NOERROR) {
	result = mirw_hyperslab_icv(MIRW_OP_WRITE, 
                                    volume,
                                    icv, 
                                    start, 
                                    count, 
                                    (void *) buffer);
	miicv_detach(icv);
    }
    return (result);
}

/** Read a hyperslab from the file into the preallocated buffer,
 *  converting from the stored "voxel" data range to the desired
 * "real" (float or double) data range.
 */
int
miget_real_value_hyperslab(mihandle_t volume,
			    mitype_t buffer_data_type,
			    const unsigned long start[],
			    const unsigned long count[],
			    void *buffer)
{
    hid_t file_id;
    int var_id;
    int icv;
    int result;
    int is_signed;
    int nctype;

    file_id = volume->hdf_id;

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
	result = mirw_hyperslab_icv(MIRW_OP_READ,
                                    volume,
                                    icv, 
                                    start, 
                                    count, 
                                    (void *) buffer);
	miicv_detach(icv);
    }
    miicv_free(icv);
    return (result);
}

/** Write a hyperslab to the file from the preallocated buffer,
 *  converting from the stored "voxel" data range to the desired
 * "real" (float or double) data range.
 */
int
miset_real_value_hyperslab(mihandle_t volume,
			    mitype_t buffer_data_type,
			    const unsigned long start[],
			    const unsigned long count[],
			    const void *buffer)
{
    hid_t file_id;
    int var_id;
    int icv;
    int result;
    int is_signed;
    int nctype;

    file_id = volume->hdf_id;

    var_id = ncvarid(file_id, MIimage);

    nctype = mitype_to_nctype(buffer_data_type, &is_signed);

    if ((icv = miicv_create()) < 0) {
	return (MI_ERROR);
    }

    miicv_setint(icv, MI_ICV_TYPE, nctype);
    miicv_setstr(icv, MI_ICV_SIGN, is_signed ? MI_SIGNED : MI_UNSIGNED);

    result = miicv_attach(icv, file_id, var_id);
    if (result == MI_NOERROR) {
	result = mirw_hyperslab_icv(MIRW_OP_WRITE, 
                                    volume,
                                    icv, 
                                    start, 
                                    count, 
                                    (void *) buffer);
	miicv_detach(icv);
    }
    miicv_free(icv);
    return (result);
}

/** Read a hyperslab from the file into the preallocated buffer,
 * with no range conversions or normalization.  Type conversions will
 * be performed if necessary.
 */
int
miget_voxel_value_hyperslab(mihandle_t volume,
			     mitype_t buffer_data_type,
			     const unsigned long start[],
			     const unsigned long count[],
			     void *buffer)
{
    return mirw_hyperslab_raw(MIRW_OP_READ, volume, buffer_data_type, 
                              start, count, buffer);
}

/** Write a hyperslab to the file from the preallocated buffer,
 * with no range conversions or normalization.  Type conversions will
 * be performed if necessary.
 */
int
miset_voxel_value_hyperslab(mihandle_t volume,
			     mitype_t buffer_data_type,
			     const unsigned long start[],
			     const unsigned long count[],
			     const void *buffer)
{
    return mirw_hyperslab_raw(MIRW_OP_WRITE, volume, buffer_data_type, 
                              start, count, (void *) buffer);
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
    unsigned long start[NDIMS];
    unsigned long count[NDIMS] = {CX,CY,CZ};
    double dtemp[CX][CY][CZ];
    unsigned short stemp[CX][CY][CZ];
    unsigned char btemp[CX][CY][CZ];
    unsigned short stmp2[CX][CY][CZ];
    int i,j,k;
    hid_t file_id;
    long imap[NDIMS];
    long offset;
    long sizes[NDIMS] = {CX,CY,CZ};
    midimhandle_t hdims[NDIMS];
    char *dimnames[] = {"xspace", "yspace", "zspace"};

    result = miopen_volume("hyper.mnc", MI2_OPEN_RDWR, &hvol);
    if (result < 0) {
        TESTRPT("Unable to open input file", result);
    }

    result = miget_volume_dimensions(hvol, MI_DIMCLASS_ANY, MI_DIMATTR_ALL, 
                                     MI_DIMORDER_FILE, 3, hdims);
    if (result < 0) {
        TESTRPT("Unable to get volume dimensions", result);
    }

    miset_apparent_dimension_order_by_name(hvol, 3, dimnames);

    miset_dimension_apparent_voxel_order(hdims[2], MI_COUNTER_FILE_ORDER);

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

    start[0] = start[1] = start[2] = 0;
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
                                   0.0, 0.001, btemp);
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

    i = miget_voxel_value_hyperslab(hvol, MI_TYPE_USHORT, start, count, stemp);
    if (i < 0) {
        printf("oops\n");
    }
    else {
        for (i = 0; i < CX; i++) {
            for (j = 0; j < CY; j++) {
                for (k = 0; k < CZ; k++) {
                    stemp[i][j][k] = stemp[i][j][k] ^ 0x55;
                }
            }
        }
    }
    i = miset_voxel_value_hyperslab(hvol, MI_TYPE_USHORT, start, count, stemp);
    if (i < 0) {
        printf("oops\n");
    }
    else {
        for (i = 0; i < CX; i++) {
            for (j = 0; j < CY; j++) {
                for (k = 0; k < CZ; k++) {
                    stemp[i][j][k] = stemp[i][j][k] ^ 0x55;
                }
            }
        }
    }
    i = miset_voxel_value_hyperslab(hvol, MI_TYPE_USHORT, start, count, stemp);
    miclose_volume(hvol);
    return (error_cnt);
}
#endif /* M2_TEST */
