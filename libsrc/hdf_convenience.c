#ifdef MINC2                    /* Ignore this file if not MINC2 */

/* #define NC_FILL_INT 1 */
#include "minc.h"
#include "minc_private.h"
#include "hdf_convenience.h"

#define MI2_STD_DIM_COUNT 8
#define MI2_DIMORDER "dimorder"
#define MI2_LENGTH "length"
#define MI2_CLASS "class"

/************************************************************************
 * Structures for files, variables, and dimensions.
 ************************************************************************/

struct m2_var {
    char name[NC_MAX_NAME];
    char path[NC_MAX_NAME];
    int id;
    int ndims;
    hsize_t *dims;
    hid_t dset_id;
    hid_t ftyp_id;              /* File type */
    hid_t mtyp_id;              /* Memory type */
    hid_t fspc_id;
};

struct m2_dim {
    struct m2_dim *link;
    int id;
    long length;
    char name[NC_MAX_NAME];
};

struct m2_file {
    struct m2_file *link;
    hid_t fd;
    int resolution;		/* Resolution setting. */
    int nvars;
    int ndims;
    struct m2_var *vars[NC_MAX_VARS];
    struct m2_dim *dims[NC_MAX_DIMS];
    hid_t grp_id;               /* Root group ID */
    int comp_type;              /* Compression type */
    int comp_param;             /* Compression parameter */
    int chunk_type;             /* Chunking enabled */
    int chunk_param;            /* Chunk length */
} *_m2_list;


static struct m2_file *
hdf_id_check(int fd)
{
    struct m2_file *curr;

    for (curr = _m2_list; curr != NULL; curr = curr->link) {
	if (fd == curr->fd) {
	    return (curr);
	}
    }
    return (NULL);
}

static struct m2_file *
hdf_id_add(int fd)
{
    struct m2_file *new;

    new = (struct m2_file *) malloc(sizeof (struct m2_file));
    if (new != NULL) {
	new->fd = fd;
	new->resolution = 0;
	new->nvars = 0;
	new->ndims = 0;
	new->link =_m2_list;
        new->grp_id = H5Gopen(fd, MI2_GRPNAME);
        new->comp_type = MI2_COMP_UNKNOWN;
        new->comp_param = 0;
        new->chunk_type = MI2_CHUNK_UNKNOWN;
        new->chunk_param = 0;
	_m2_list = new;
    }
    else {
	milog_message(MI_MSG_OUTOFMEM, sizeof(struct m2_file));
	exit(-1);
    }
    return (new);
}

static int 
hdf_id_del(int fd)
{
    struct m2_file *curr, *prev;
    int i;

    for (prev = NULL, curr = _m2_list; curr != NULL; 
	 prev = curr, curr = curr->link) {
	if (fd == curr->fd) {

	    /* Unlink it from the global list.
	     */
	    if (prev == NULL) {
		_m2_list = curr->link;
	    }
	    else {
		prev->link = curr->link;
	    }

	    /* Delete the variable list.
	     */
	    for (i = 0; i < curr->nvars; i++) {
		struct m2_var *tmp = curr->vars[i];
		if (tmp->dims != NULL) {
		    free(tmp->dims);
		}
                /* Close the HDF5 handles we were holding open.
                 */
                H5Dclose(tmp->dset_id);
                H5Tclose(tmp->ftyp_id);
                H5Tclose(tmp->mtyp_id);
                H5Sclose(tmp->fspc_id);
		free(tmp);
	    }

	    /* Delete the dimension list.
	     */
            for (i = 0; i < curr->ndims; i++) {
		struct m2_dim *tmp = curr->dims[i];
		free(tmp);
	    }

            H5Gclose(curr->grp_id);
	    free(curr);
	    return (MI_NOERROR);
	}
    }
    return (MI_ERROR);
}

struct m2_var *
hdf_var_byname(struct m2_file *file, const char *name)
{
    int i;

    for (i = 0; i < file->nvars; i++) {
	if (!strcmp(file->vars[i]->name, name)) {
	    return (file->vars[i]);
	}
    }
    return (NULL);
}

struct m2_var *
hdf_var_byid(struct m2_file *file, int varid)
{
    if (varid >= 0 && varid < file->nvars) {
        return (file->vars[varid]);
    }
    return (NULL);
}

struct m2_var *
hdf_var_add(struct m2_file *file, const char *name, const char *path, 
	    int ndims, hsize_t *dims)
{
    struct m2_var *new;

    if (file->nvars >= NC_MAX_VARS) {
        return (NULL);
    }

    new = (struct m2_var *) malloc(sizeof(struct m2_var));
    if (new != NULL) {
        new->id = file->nvars++;
	strncpy(new->name, name, NC_MAX_NAME - 1);
	strncpy(new->path, path, NC_MAX_NAME - 1);
        new->dset_id = H5Dopen(file->fd, path);
        new->ftyp_id = H5Dget_type(new->dset_id);
        new->mtyp_id = H5Tget_native_type(new->ftyp_id, H5T_DIR_ASCEND);
        new->fspc_id = H5Dget_space(new->dset_id);
	new->ndims = ndims;
	if (ndims != 0) {
	    new->dims = (hsize_t *) malloc(sizeof (hsize_t) * ndims);
	    if (new->dims != NULL) {
		int i;
		for (i = 0; i < ndims; i++) {
		    new->dims[i] = dims[i];
		}
	    }
	    else {
		milog_message(MI_MSG_OUTOFMEM, sizeof(hsize_t) * ndims);
	    }
	}
	else {
	    new->dims = NULL;
	}
        file->vars[new->id] = new;
    }
    else {
	milog_message(MI_MSG_OUTOFMEM, sizeof (struct m2_var));
	exit(-1);
    }
    return (new);
}

/** Find a dimension by name.
 */
struct m2_dim *
hdf_dim_byname(struct m2_file *file, const char *name)
{
    int i;

    for (i = 0; i < file->ndims; i++) {
        if (!strcmp(file->dims[i]->name, name)) {
	    return (file->dims[i]);
	}
    }
    return (NULL);
}

/** Find a dimension by ID number.
 */
struct m2_dim *
hdf_dim_byid(struct m2_file *file, int dimid)
{
    if (dimid >= 0 && dimid < file->ndims) {
        return (file->dims[dimid]);
    }
    return (NULL);
}

struct m2_dim *
hdf_dim_add(struct m2_file *file, const char *name, long length)
{
    struct m2_dim *new;

    if (file->ndims >= NC_MAX_DIMS) {
        return (NULL);
    }

    new = (struct m2_dim *) malloc(sizeof(struct m2_dim));
    if (new != NULL) {
        new->id = file->ndims++;
	new->length = length;
	strncpy(new->name, name, NC_MAX_NAME - 1);
	file->dims[new->id] = new;
    }
    else {
        milog_message(MI_MSG_OUTOFMEM, sizeof(struct m2_dim));
	exit(-1);
    }
    return (new);
}

/************************************************************************
 * Other helper functions
 ************************************************************************/
static int
hdf_is_dimension_name(struct m2_file *file, const char *varnm)
{
    static char *dimnms[MI2_STD_DIM_COUNT] = {
	MIxspace,
	MIyspace,
	MIzspace,
	MItime,
	MIxfrequency,
	MIyfrequency,
	MIzfrequency,
	MItfrequency
    };
    int i;

    if (hdf_dim_byname(file, varnm) != NULL) {
	return (1);
    }

    for (i = 0; i < MI2_STD_DIM_COUNT; i++) {
	if (!strcmp(varnm, dimnms[i])) {
	    return (1);
	}
    }

    return (0);			/* Not a dimension (yet?) */
}

/** Find the path of a variable, given its name.  The variable
 * may not yet exist in the file.
 */
static hid_t
hdf_path_from_name(struct m2_file *file, const char *varnm, char *varpath)
{
    if (!strcmp(varnm, MIimage) ||
	!strcmp(varnm, MIimagemax) ||
	!strcmp(varnm, MIimagemin)) {
	sprintf(varpath, "/minc-2.0/image/%d/", file->resolution);
    }
    else if (hdf_is_dimension_name(file, varnm)) {
	strcpy(varpath, "/minc-2.0/dimensions/");
    }
    else {
	strcpy(varpath, "/minc-2.0/info/");
    }
    strcat(varpath, varnm);
    return (MI_NOERROR);
}

/* map NetCDF types onto HDF types */
hid_t 
nc_to_hdf5_type(nc_type dtype, int is_signed)
{
    switch (dtype) {
    case NC_CHAR:
	return (is_signed ? H5T_STD_I8LE : H5T_STD_U8LE);
    case NC_BYTE:
	return (is_signed ? H5T_STD_I8LE : H5T_STD_U8LE);
    case NC_SHORT:
	return (is_signed ? H5T_STD_I16LE : H5T_STD_U16LE);
    case NC_INT:
	return (is_signed ? H5T_STD_I32LE : H5T_STD_U32LE);
    case NC_FLOAT:
	return (H5T_IEEE_F32LE);
    case NC_DOUBLE:
	return (H5T_IEEE_F64LE);
    }
    return (-1);
}

private void
hdf_get_diminfo(hid_t dst_id, int *ndims, hsize_t dims[])
{
    hid_t spc_id;
    spc_id = H5Dget_space(dst_id);
    if (spc_id < 0) {
        milog_message(MI_MSG_SNH);
    }
    else {
	*ndims = H5Sget_simple_extent_ndims(spc_id);
	if (*ndims > 0) {
	    H5Sget_simple_extent_dims(spc_id, dims, NULL);
	}
    }
}

private int
hdf_size(hid_t spc_id, hid_t typ_id)
{
    int typ_size = H5Tget_size(typ_id);
    int spc_size = H5Sget_simple_extent_npoints(spc_id);

    if (typ_size <= 0 || spc_size <= 0) {
	milog_message(MI_MSG_SNH);
	return (-1);
    }
    return (typ_size * spc_size);
}

/** Given a numeric variable ID, get the text name of a MINC/HDF5 variable.
 * Equivalent of ncvarname()
 */
public int
hdf_varname(int fd, int varid, char *varnm)
{
    struct m2_file *file;
    struct m2_var *var;

    /* Emulate rootvariable.
     */
    if (varid == MI_ROOTVARIABLE_ID) {
        strcpy(varnm, MIrootvariable);
        return (MI_NOERROR);
    }

    if ((file = hdf_id_check(fd)) != NULL && 
        (var = hdf_var_byid(file, varid)) != NULL) {
        strcpy(varnm, var->name);
        return (MI_NOERROR);
    }
    return (MI_ERROR);
}

/** Given a text name, get the numeric ID of a MINC/HDF5 variable. 
 * Equivalent of ncvarid().
 */
public int
hdf_varid(int fd, const char *varnm)
{
    struct m2_file *file;
    struct m2_var *var;

    if (!strcmp(varnm, MIrootvariable)) {
        return (MI_ROOTVARIABLE_ID);
    }

    file = hdf_id_check(fd);
    if (file != NULL) {
	var = hdf_var_byname(file, varnm);
	if (var != NULL) {
	    return (var->id);
	}
    }
    return (MI_ERROR);
}

/** Given a variable ID and attribute number return the attribute's text
 * name.  The variable ID may be NC_GLOBAL.  Equivalent to ncattname().
 */
public int
hdf_attname(int fd, int varid, int attnum, char *name)
{
    hid_t loc_id;
    hid_t att_id;
    int status;
    struct m2_file *file;
    struct m2_var *var;

    if ((file = hdf_id_check(fd)) == NULL) {
        return (MI_ERROR);
    }

    if (varid == NC_GLOBAL) {
        var = NULL;
        loc_id = file->grp_id;
    } 
    else {
        if ((var = hdf_var_byid(file, varid)) == NULL) {
            return (MI_ERROR);
        }
        loc_id = var->dset_id;
    }

    H5E_BEGIN_TRY {
        att_id = H5Aopen_idx(loc_id, attnum);
    } H5E_END_TRY;
    if (att_id >= 0) {
        status = H5Aget_name(att_id, MAX_NC_NAME, name);
        H5Aclose(att_id);
    }
    else {
        /* See if this is the magic emulated signtype attribute.
         */
        if (var != NULL &&
            !strcmp(var->name, MIimage) &&
            attnum == H5Aget_num_attrs(loc_id)) {
            strcpy(name, MIsigntype);
            status = MI_NOERROR;
        }
        else {
            status = MI_ERROR;
        }
    }
    return (status);
}

/** This function provides emulation for the "ncattinq()" call which
 * is part of netCDF.  Unlike a number of netCDF functions, this 
 * function is expected to return 1 on success, rather than zero.
 */
public int 
hdf_attinq(int fd, int varid, const char *attnm, nc_type *type_ptr, 
	   int *length_ptr)
{
  hid_t att_id = -1;
  hid_t typ_id = -1;
  hid_t spc_id = -1;
  hid_t loc_id;
  int status = MI_ERROR;
  size_t typ_size;
  H5T_class_t typ_class;
  struct m2_file *file;
  struct m2_var *var;

  if ((file = hdf_id_check(fd)) == NULL) {
      return (MI_ERROR);
  }

  if (varid == NC_GLOBAL) {
      var = NULL;
      loc_id = file->grp_id;
  } 
  else {
      if ((var = hdf_var_byid(file, varid)) == NULL) {
          return (MI_ERROR);
      }
      loc_id = var->dset_id;
  }

  /* Special case - emulate the signtype attribute.
   */
  if (!strcmp(attnm, MIsigntype)) {
      if (var != NULL && H5Tget_class(var->ftyp_id) == H5T_INTEGER) {
          if (type_ptr != NULL) {
              *type_ptr = NC_CHAR;
          }
          if (length_ptr != NULL) {
              /* Signed and unsigned are the same length,
               */
              *length_ptr = sizeof(MI_UNSIGNED);
          }
          return (1);           /* 1 -> success here */
      }
      else {
          return (MI_ERROR);
      }
  }
  else {
      H5E_BEGIN_TRY {
          att_id = H5Aopen_name(loc_id, attnm);
      } H5E_END_TRY;

      if (att_id < 0)
	  goto cleanup;
  
      if ((spc_id = H5Aget_space(att_id)) < 0)
	  goto cleanup;

      if ((typ_id = H5Aget_type(att_id)) < 0)
	  goto cleanup;
 
      typ_class = H5Tget_class(typ_id);
      typ_size = H5Tget_size(typ_id);

      if (type_ptr != NULL) {
	  if (typ_class == H5T_INTEGER) {
	      if (typ_size == 1)
		  *type_ptr = NC_BYTE;
	      else if (typ_size == 2)
		  *type_ptr = NC_SHORT;
	      else if (typ_size == 4) 
		  *type_ptr = NC_INT;
	      else {
		  milog_message(MI_MSG_INTSIZE, typ_size);
	      }
	  }
	  else if (typ_class == H5T_FLOAT) {
	      if (typ_size == 4) {
		  *type_ptr = NC_FLOAT;
	      }
	      else if (typ_size == 8) {
		  *type_ptr = NC_DOUBLE;
	      }
	      else {
		  milog_message(MI_MSG_FLTSIZE, typ_size);
	      }
	  }
	  else if (typ_class == H5T_STRING) {
	      *type_ptr = NC_CHAR;
	  }
	  else {
	      milog_message(MI_MSG_TYPECLASS, typ_class);
	  }
      }

      if (length_ptr != NULL) {
	  if (typ_class == H5T_STRING) {
	      *length_ptr = typ_size;
	  }
	  else {
	      *length_ptr = H5Sget_simple_extent_npoints(spc_id);
	  }
      }

      status = 1;               /* 1 -> success here */

  cleanup:
      if (typ_id >= 0)
          H5Tclose(typ_id);
      if (spc_id >= 0)
          H5Sclose(spc_id);
      if (att_id >= 0)
          H5Aclose(att_id);
  }

  return (status);
}

private int
hdf_put_dimorder(struct m2_file *file, int dst_id, int ndims, 
		 const int *dims_ptr)
{
    int i;
    hid_t att_id;
    hid_t spc_id;
    hid_t typ_id;
    char str_buf[NC_MAX_NAME];

    /* Don't bother */
    if (ndims == 0) {
	return (MI_NOERROR);
    }

    str_buf[0] = '\0';
    for (i = 0; i < ndims; i++) {
	struct m2_dim *dim = hdf_dim_byid(file, dims_ptr[i]);
	if (dim != NULL) {
	    strcat(str_buf, dim->name);
	}
	if (i != ndims - 1) {
	    strcat(str_buf, ",");
	}
    }

    typ_id = H5Tcopy(H5T_C_S1);
    H5Tset_size(typ_id, strlen(str_buf) + 1);

    spc_id = H5Screate(H5S_SCALAR);

    att_id = H5Acreate(dst_id, MI2_DIMORDER, typ_id, spc_id, H5P_DEFAULT);

    if (att_id >= 0) {
	H5Awrite(att_id, typ_id, str_buf);
    }
  
    H5Aclose(att_id);
    H5Sclose(spc_id);
    H5Tclose(typ_id);
    return (MI_NOERROR);
}

private int 
hdf_get_dimorder(struct m2_file *file, int dst_id, int ndims, int *dims_ptr)
{
    char *str_ptr;
    char *tmp_ptr;
    hid_t att_id;
    hid_t typ_id;
    char buf[NC_MAX_NAME];
    int status;
    struct m2_dim *dim;
    int n;
    int done;
    int length;

    /* Don't bother */
    if (ndims == 0) {
	return (MI_NOERROR);
    }

    att_id = H5Aopen_name(dst_id, MI2_DIMORDER);
    if (att_id < 0) {
	return (MI_ERROR);
    }
    typ_id = H5Aget_type(att_id);

    length = H5Tget_size(typ_id);
    if (length > NC_MAX_NAME) {
        return (MI_ERROR);
    }

    status = H5Aread(att_id, typ_id, buf);
    if (status < 0) {
        return (MI_ERROR);
    }
    H5Tclose(typ_id);
    H5Aclose(att_id);

    buf[length] = '\0';         /* Make certain string is terminated. */

    str_ptr = &buf[0];

    n = 0;
    done = 0;
    while (!done && n < ndims) {

	tmp_ptr = str_ptr;

	while (*tmp_ptr != ',' && *tmp_ptr != '\0') {
	    tmp_ptr++;
	}

	if (*tmp_ptr == '\0') {	/* Real end of string? */
	    done = 1;		/* We're finished after this iteration. */
	}
	else {
	    *tmp_ptr++ = '\0';	/* Terminate the string. */
	}

	dim = hdf_dim_byname(file, str_ptr);
	if (dim != NULL) {
	    dims_ptr[n++] = dim->id;
	}

	str_ptr = tmp_ptr;
    }

    return (MI_NOERROR);
}

public int
hdf_inquire(int fd, int *ndims_ptr, int *nvars_ptr, int *natts_ptr,
	    int *unlimdim_ptr)
{
    struct m2_file *file;

    if ((file = hdf_id_check(fd)) == NULL) {
	return (MI_ERROR);
    }

    if (ndims_ptr != NULL) {
	*ndims_ptr = file->ndims;
    } 

    if (unlimdim_ptr != NULL) {
	/* We don't support unlimited dimensions. */
	*unlimdim_ptr = -1;
    }

    if (nvars_ptr != NULL) {
	*nvars_ptr = file->nvars;
    }

    if (natts_ptr != NULL) {
        *natts_ptr = H5Aget_num_attrs(file->grp_id);
    }

    return (MI_NOERROR);
}

public int 
hdf_varinq(int fd, int varid, char *varnm_ptr, nc_type *type_ptr, 
	   int *ndims_ptr, int *dims_ptr, int *natts_ptr)
{
    hid_t dst_id;
    hid_t typ_id;
    size_t size;
    H5T_class_t class;
    int ndims;
    struct m2_file *file;
    struct m2_var *var;

    /* Emulate rootvariable */
    if (varid == MI_ROOTVARIABLE_ID) {
        if (varnm_ptr != NULL) {
            strcpy(varnm_ptr, MIrootvariable);
        }
        if (type_ptr != NULL) {
            *type_ptr = NC_INT;
        }
        if (ndims_ptr != NULL) {
            *ndims_ptr = 0;
        }
        if (natts_ptr != NULL) {
            *natts_ptr = 0;
        }
        return (MI_NOERROR);
    }

    if ((file = hdf_id_check(fd)) == NULL) {
	return (MI_ERROR);
    }
    if ((var = hdf_var_byid(file, varid)) == NULL) {
        return (MI_ERROR);
    }

    dst_id = var->dset_id;
    typ_id = var->ftyp_id;
 
    class = H5Tget_class(typ_id);
    size = H5Tget_size(typ_id);

    if (type_ptr != NULL) {
	if (class == H5T_INTEGER) {
	    if (size == 1)
		*type_ptr = NC_BYTE;
	    else if (size == 2)
		*type_ptr = NC_SHORT;
	    else if (size == 4) 
		*type_ptr = NC_INT;
	    else {
		milog_message(MI_MSG_INTSIZE, size);
		exit(-1);
	    }
	}
	else if (class == H5T_FLOAT) {
	    if (size == 4) {
		*type_ptr = NC_FLOAT;
	    }
	    else if (size == 8) {
		*type_ptr = NC_DOUBLE;
	    }
	    else {
		milog_message(MI_MSG_FLTSIZE, size);
		exit(-1);
	    }
	}
	else if (class == H5T_STRING) {
	    *type_ptr = NC_CHAR;
	}
	else {
	    milog_message(MI_MSG_TYPECLASS, class);
	    exit(-1);
	}
    }

    if (class == H5T_STRING) {
	ndims = 0;
    }
    else {
	ndims = var->ndims;
    }
    
    if (dims_ptr != NULL && ndims != 0) {
	hdf_get_dimorder(file, dst_id, ndims, dims_ptr);
    }

    if (ndims_ptr != NULL) {
	*ndims_ptr = ndims;
    }

    if (natts_ptr != NULL) {
        int natts = H5Aget_num_attrs(dst_id);

        /* Emulate the signtype attribute for the image variable.
         */
        if (!strcmp(var->name, MIimage)) {
            natts++;
        }

        *natts_ptr = natts;
    }

    if (varnm_ptr != NULL) {
        strcpy(varnm_ptr, var->name);
    }

    return (MI_NOERROR);
}

public int
hdf_dimrename(int fd, int dimid, const char *new_name)
{
    milog_message(MI_MSG_NOTIMPL, "dimrename");
    return (MI_NOERROR);
}

public int
hdf_dimid(int fd, const char *dimnm)
{
    struct m2_file *file;
    struct m2_dim *dim;

    if ((file = hdf_id_check(fd)) == NULL) {
	return (MI_ERROR);
    }

    if ((dim = hdf_dim_byname(file, dimnm)) == NULL) {
	return (MI_ERROR);
    }
    return (dim->id);
}

	
public int
hdf_diminq(int fd, int dimid, char *dimnm_ptr, long *len_ptr)
{
    struct m2_file *file;
    struct m2_dim *dim;

    if ((file = hdf_id_check(fd)) == NULL) {
	return (MI_ERROR);
    }

    if ((dim = hdf_dim_byid(file, dimid)) == NULL) {
	return (MI_ERROR);
    }

    /* Copy the dimension name, if appropriate.
     */
    if (dimnm_ptr != NULL) {
	strcpy(dimnm_ptr, dim->name);
    }

    if (len_ptr != NULL) {
	*len_ptr = dim->length;
    }

    return (MI_NOERROR);
}

public int
hdf_dimdef(int fd, const char *dimnm, long length)
{
    int status = MI_ERROR;
    struct m2_file *file;
    struct m2_dim *dim;

    if ((file = hdf_id_check(fd)) != NULL &&
        (dim = hdf_dim_add(file, dimnm, length)) != NULL) {
        status = dim->id;
    }
    return (status);
}

/** Like hdf_attinq, this function must return one for success, not
 * zero!
 */
public int
hdf_attget(int fd, int varid, const char *attnm, void *value)
{
    hid_t att_id;
    hid_t ftyp_id;
    hid_t mtyp_id;
    hid_t loc_id;
    int status = MI_ERROR;
    struct m2_file *file;
    struct m2_var *var;

    if ((file = hdf_id_check(fd)) == NULL) {
        return (MI_ERROR);
    }

    if (varid == NC_GLOBAL) {
        var = NULL;
        loc_id = file->grp_id;
    } 
    else {
        if ((var = hdf_var_byid(file, varid)) == NULL) {
            return (MI_ERROR);
        }
        loc_id = var->dset_id;
    }

    /* Special case - emulate the signtype attribute.
     */
    if (!strcmp(attnm, MIsigntype)) {
        if (H5Tget_class(var->ftyp_id) == H5T_INTEGER) {
            if (H5Tget_sign(var->ftyp_id) == H5T_SGN_NONE) {
                strcpy((char *) value, MI_UNSIGNED);
            }
            else {
                strcpy((char *) value, MI_SIGNED);
            }
            status = 1;         /* 1 -> success */
        }
    }
    else if (!strcmp(attnm, MI_FillValue)) {
        hid_t plist_id = H5Dget_create_plist(loc_id);
        if (plist_id >= 0) {
            if (H5Pget_fill_value(plist_id, var->mtyp_id, value) >= 0) {
                status = MI_NOERROR;
            }
            H5Pclose(plist_id);
        }
    }
    else {
        H5E_BEGIN_TRY {
            att_id = H5Aopen_name(loc_id, attnm);
        } H5E_END_TRY;
        if (att_id >= 0) {
            if ((ftyp_id = H5Aget_type(att_id)) >= 0) {
                mtyp_id = H5Tget_native_type(ftyp_id, H5T_DIR_ASCEND);
                if (mtyp_id >= 0) {
                    if (H5Aread(att_id, mtyp_id, value) >= 0) {
                        status = 1; /* 1 -> success */
                    }
                    H5Tclose(mtyp_id);
                }
                H5Tclose(ftyp_id);
            }
            H5Aclose(att_id);
        }
    }
    return (status);
}

public int
hdf_attput(int fd, int varid, const char *attnm, nc_type val_typ, 
	   int val_len, void *val_ptr)
{
    hid_t att_id = -1;
    hid_t mtyp_id = -1;         /* Memory type */
    hid_t ftyp_id = -1;         /* File type */
    hid_t spc_id = -1;
    hid_t loc_id;
    int status = MI_ERROR;
    struct m2_file *file;
    struct m2_var *var;

    /* Ignore deprecated variables. */
    if (varid == MI_ROOTVARIABLE_ID) {
        return (MI_NOERROR);    /* Pretend all is OK. */
    }

    /* Ignore deprecated attributes. */
    if (!strcmp(attnm, MIparent) ||
        !strcmp(attnm, MIchildren) ||
        !strcmp(attnm, MIimagemin) ||
        !strcmp(attnm, MIimagemax) ||
        !strcmp(attnm, MI_FillValue)) {
        return (MI_NOERROR);	/* Pretend we created it. */
    }

    if ((file = hdf_id_check(fd)) == NULL) {
        return (MI_ERROR);
    }

    if (varid == NC_GLOBAL) {
        var = NULL;
        loc_id = file->grp_id;
    } 
    else {
        if ((var = hdf_var_byid(file, varid)) == NULL) {
            return (MI_ERROR);
        }
        loc_id = var->dset_id;
    }

    if (!strcmp(attnm, MIsigntype)) { /* Emulate 'signtype' */
        int new_signed;

        /* Need to recreate dataset with new type.  Sigh.
         */

        if (!strcmp(val_ptr, MI_SIGNED)) {
            new_signed = 1;
        }
        else if (!strcmp(val_ptr, MI_UNSIGNED)) {
            new_signed = 0;
        }
        else {
            return (MI_ERROR);
        }
        
        if ((H5Tget_sign(var->ftyp_id) == H5T_SGN_NONE && new_signed) ||
            (H5Tget_sign(var->ftyp_id) == H5T_SGN_2 && !new_signed)) {
            hid_t new_type_id;
            hid_t new_dset_id;
            hid_t new_plst_id;
            char temp[128];
            unsigned int i;

            sprintf(temp, "junkXXXX");

            new_type_id = H5Tcopy(var->ftyp_id);
            if (new_type_id < 0) {
                milog_message(MI_MSG_SNH);
            }
            if (H5Tset_sign(new_type_id, (new_signed) ? H5T_SGN_2 : H5T_SGN_NONE) < 0) {
                milog_message(MI_MSG_SNH);
            }

            new_plst_id = H5Dget_create_plist(var->dset_id);

            new_dset_id = H5Dcreate(file->grp_id, temp, new_type_id, 
                                    var->fspc_id, new_plst_id);

            /* Iterate over all attributes, copying from old to new. */
            i = 0;
            H5Aiterate(var->dset_id, &i, hdf_copy_attr, (void *) new_dset_id);

            H5Dclose(var->dset_id);
            H5Tclose(var->ftyp_id);
            H5Tclose(var->mtyp_id);
            H5Tclose(new_type_id);
            H5Pclose(new_plst_id);
            H5Sclose(var->fspc_id);

            if (H5Gunlink(fd, var->path) < 0) {
                milog_message(MI_MSG_SNH);
            }

            if (H5Gmove2(file->grp_id, temp, fd, var->path) < 0) {
                milog_message(MI_MSG_SNH);
            }

            var->dset_id = new_dset_id;
            var->ftyp_id = H5Dget_type(var->dset_id);
            var->mtyp_id = H5Tget_native_type(var->ftyp_id, H5T_DIR_ASCEND);
            var->fspc_id = H5Dget_space(var->dset_id);
        }
        return (MI_NOERROR);
    }

    if (val_typ == NC_CHAR) {
        ftyp_id = H5Tcopy(H5T_C_S1);
        H5Tset_size(ftyp_id, val_len);
        mtyp_id = H5Tcopy(ftyp_id);
        spc_id = H5Screate(H5S_SCALAR);
    }
    else {
        if (val_len == 1) {
            spc_id = H5Screate(H5S_SCALAR);
        }
        else {
            hsize_t temp_size = val_len;
            spc_id = H5Screate_simple(1, &temp_size, NULL);
        }

        switch (val_typ) {
        case NC_BYTE:
            mtyp_id = H5T_NATIVE_UCHAR;
            ftyp_id = H5T_STD_U8LE;
            break;
        case NC_SHORT:
            mtyp_id = H5T_NATIVE_USHORT;
            ftyp_id = H5T_STD_U16LE;
            break;
        case NC_INT:
            mtyp_id = H5T_NATIVE_UINT;
            ftyp_id = H5T_STD_U32LE;
            break;
        case NC_FLOAT:
            mtyp_id = H5T_NATIVE_FLOAT;
            ftyp_id = H5T_IEEE_F32LE;
            break;
        case NC_DOUBLE:
            mtyp_id = H5T_NATIVE_DOUBLE;
            ftyp_id = H5T_IEEE_F64LE;
            break;
        default:
            milog_message(MI_MSG_BADTYPE, val_typ);
            return (MI_ERROR);
        }

        mtyp_id = H5Tcopy(mtyp_id);
        ftyp_id = H5Tcopy(ftyp_id);
    }

    /* If the attribute already exists, delete it.  It is not possible
     * to change the size of an existing attribute.
     */
    H5E_BEGIN_TRY {
        H5Adelete(loc_id, attnm);
    } H5E_END_TRY;

    /* Create the attribute anew.
     */
    att_id = H5Acreate(loc_id, attnm, ftyp_id, spc_id, H5P_DEFAULT);

    if (att_id < 0)
        goto cleanup;

    /* Save the value.
     */
    status = H5Awrite(att_id, mtyp_id, val_ptr);
    if (status >= 0) {
        status = MI_NOERROR;
    }

 cleanup:
    if (spc_id >= 0) 
        H5Sclose(spc_id);
    if (ftyp_id >= 0)
        H5Tclose(ftyp_id);
    if (mtyp_id >= 0)
        H5Tclose(mtyp_id);
    if (att_id >= 0)
        H5Aclose(att_id);

    return (status);
}

/** This function simply sweeps through all of the dimensions defined
 * in a file, and if a dimension variable has not yet been saved to the 
 * file, it creates one here.
 */
private void
hdf_dim_commit(int fd)
{
    struct m2_file *file;
    struct m2_dim *dim;
    int i;

    if ((file = hdf_id_check(fd)) != NULL) {
        for (i = 0; i < file->ndims; i++) {
            if ((dim = hdf_dim_byid(file, i)) != NULL) {
                if (hdf_var_byname(file, dim->name) == NULL) {
                    hdf_vardef(fd, dim->name, NC_INT, 0, NULL);
                }
            }
        }
    }
}

/** This function is called when the "definition" phase of NetCDF file
 * creation is completed.  The HDF5 library really doesn't need this, 
 * since the mode doesn't exist in HDF5.  But this is a convenient 
 * time to make certain all of the dimension variables have actually
 * been defined.
 */
public void
hdf_enddef(int fd)
{
    hdf_dim_commit(fd);         /* Make sure all dimensions were saved. */
}


/** This function provides the HDF5 emulation of the function ncvardef
 */
public int
hdf_vardef(int fd, const char *varnm, nc_type vartype, int ndims, 
           const int *dimids)
{
    int dst_id = -1;
    int typ_id = -1;
    int spc_id = -1;
    int prp_id = -1;
    int status = MI_ERROR;
    int i;
    long length;
    hsize_t dims[MAX_NC_DIMS];
    hsize_t chkdims[MAX_NC_DIMS];
    char varpath[NC_MAX_NAME];
    struct m2_file *file;
    struct m2_var *var;
    struct m2_dim *dim;
    int chunk_length;
    int comp_level;

    /* Ignore deprecated variables */
    if (!strcmp(varnm, MIrootvariable)) {
	return (MI_ROOTVARIABLE_ID);
    }

    if ((file = hdf_id_check(fd)) == NULL) {
	return (MI_ERROR);
    }

    if (hdf_path_from_name(file, varnm, varpath) < 0) {
	return (MI_ERROR);
    }

    prp_id = H5Pcreate(H5P_DATASET_CREATE);
    if (prp_id < 0) {
        goto cleanup;
    }

    if (ndims == 0) {
	spc_id = H5Screate(H5S_SCALAR);

        /* For any scalar datasets, use compact dataset layout to 
         * minimize file overhead.
         */
        H5Pset_layout(prp_id, H5D_COMPACT);
    }
    else {
	for (i = 0; i < ndims; i++) {
	    status = hdf_diminq(fd, dimids[i], NULL, &length);
	    if (status < 0) {
		return (status);
	    }
            dims[i] = length;
	}

	spc_id = H5Screate_simple(ndims, dims, NULL);

        if (file->chunk_type == MI2_CHUNK_UNKNOWN) {
            chunk_length = miget_cfg_int(MICFG_CHUNKING);
        }
        else {
            chunk_length = file->chunk_param;
        }

        if (file->comp_type == MI2_COMP_UNKNOWN) {
            comp_level = miget_cfg_int(MICFG_COMPRESS);
        }
        else {
            if (file->comp_type == MI2_COMP_ZLIB) {
                comp_level = file->comp_param;
            }
            else {
                comp_level = 0;
            }
        }

        if (comp_level != 0) {
            /* If compression is specified, chunking must be enabled.
             */
            if (chunk_length == 0) {
                chunk_length = 32;  /* TODO: define somewhere??? */
            }
            H5Pset_deflate(prp_id, comp_level);
        }

        if (chunk_length > 4) {
            for (i = 0; i < ndims; i++) {
                if ((chkdims[i] = dims[i]) > chunk_length) {
                    chkdims[i] = chunk_length;
                }
            }
            H5Pset_chunk(prp_id, ndims, chkdims);
        }
    }

    if (spc_id < 0) {
	goto cleanup;
    }

    typ_id = H5Tcopy(nc_to_hdf5_type(vartype, TRUE));
    if (typ_id < 0) {
	goto cleanup;
    }
    
    H5E_BEGIN_TRY {
        dst_id = H5Dcreate(fd, varpath, typ_id, spc_id, prp_id);
    } H5E_END_TRY;

    if (dst_id < 0) {
        milog_message(MI_MSG_OPENDSET, varnm);
	goto cleanup;
    }

    /* If this is a dimension variable, we have to define the length
     * attribute now.
     */
    if ((dim = hdf_dim_byname(file, varnm)) != NULL) {
        hid_t att_id;
        hid_t aspc_id;

        aspc_id = H5Screate(H5S_SCALAR);
        if (aspc_id >= 0) {
            att_id = H5Acreate(dst_id, MI2_LENGTH, H5T_STD_U32LE, aspc_id, 
                               H5P_DEFAULT);
            if (att_id >= 0) {
                status = H5Awrite(att_id, H5T_NATIVE_LONG, 
                                  (void *) &dim->length);
                H5Aclose(att_id);
            }
            H5Sclose(aspc_id);
        }
    }

    /* Add the dimension order information.
     */
    hdf_put_dimorder(file, dst_id, ndims, dimids);

    /* Add the variable to the internal table.
     */
    var = hdf_var_add(file, varnm, varpath, ndims, dims);
    if (var == NULL) {
	goto cleanup;
    }

    status = var->id;

 cleanup:
    if (prp_id >= 0) {
	H5Pclose(prp_id);
    }
    if (dst_id >= 0) {
	H5Dclose(dst_id);
    }
    if (typ_id >= 0) {
	H5Tclose(typ_id);
    }
    if (spc_id >= 0) {
	H5Sclose(spc_id);
    }
    return (status);
}

/**
 */
public int
hdf_var_declare(int fd, char *varnm, char *varpath, int ndims, hsize_t *sizes)
{
    struct m2_file *file;

    if ((file = hdf_id_check(fd)) == NULL) {
        return (MI_ERROR);
    }

    hdf_var_add(file, varnm, varpath, ndims, sizes);
    return (MI_NOERROR);
}

public int
hdf_varget(int fd, int varid, const long *start_ptr, const long *length_ptr,
	   void *val_ptr)
{
  int status = MI_ERROR;
  int dst_id = -1;
  int typ_id = -1;
  int fspc_id = -1;
  int mspc_id = -1;
  int i;
  int ndims;
  hssize_t fstart[MAX_VAR_DIMS];
  hsize_t count[MAX_VAR_DIMS];
  struct m2_file *file;
  struct m2_var *var;

  /* Emulate the obsolete "rootvariable"
   */
  if (varid == MI_ROOTVARIABLE_ID) {
      *((int *)val_ptr) = 0;
      return (MI_NOERROR);
  }

  if ((file = hdf_id_check(fd)) == NULL) {
      return (MI_ERROR);
  }

  if ((var = hdf_var_byid(file, varid)) == NULL) {
      return (MI_ERROR);
  }

  dst_id = var->dset_id;
  typ_id = var->mtyp_id;
  fspc_id = var->fspc_id;

  ndims = var->ndims;

  if (ndims == 0) {
      mspc_id = H5Screate(H5S_SCALAR);
  }
  else {
    for (i = 0; i < ndims; i++) {
      fstart[i] = start_ptr[i];
      count[i] = length_ptr[i];
    }

    status = H5Sselect_hyperslab(fspc_id, H5S_SELECT_SET, fstart, NULL, count,
				 NULL);
    if (status < 0) {
        milog_message(MI_MSG_SNH);
	goto cleanup;
    }

    mspc_id = H5Screate_simple(ndims, count, 0);
    if (mspc_id < 0) {
        milog_message(MI_MSG_SNH);
	goto cleanup;
    }
  }

  status = H5Dread(dst_id, typ_id, mspc_id, fspc_id, H5P_DEFAULT, val_ptr);
  if (status < 0) {
      milog_message(MI_MSG_READDSET, var->path);
  }

 cleanup:

  if (mspc_id >= 0)
    H5Sclose(mspc_id);
  return (status);
}

public int 
hdf_varputg(int fd, int varid, const long *start, 
            const long *edges, const long *stride, 
            const long *map, const void *value)
{
    int status = MI_ERROR;      /* Assume guilty */
    int maxidim;		/* maximum dimensional index */
    int idim;
    hssize_t *mystart = NULL;
    hsize_t *myedges;
    hsize_t *iocount;	/* count vector */
    hsize_t *stop;	/* stop indexes */
    hsize_t *length;	/* edge lengths in bytes */
    ptrdiff_t *mystride;
    ptrdiff_t *mymap;

    struct m2_var *varp;
    struct m2_file *file;
    int dst_id = -1;
    int typ_id = -1;
    int fspc_id = -1;
    int mspc_id = -1;

    if ((file = hdf_id_check(fd)) == NULL) {
	return (MI_ERROR);
    }

    if ((varp = hdf_var_byid(file, varid)) == NULL) {
	return (MI_ERROR);
    }
    
    dst_id = varp->dset_id;
    typ_id = varp->mtyp_id;
    fspc_id = varp->fspc_id;

    mspc_id = H5Scopy(fspc_id);

    maxidim = (int) varp->ndims - 1;

    if (maxidim < 0) {
	/*
	 * The variable is a scalar!
	 */
        milog_message(MI_MSG_SNH);
        goto cleanup;
    }


    /*
     * Verify stride argument.
     */
    if (stride != NULL) {
	for (idim = 0; idim <= maxidim; idim++) {
	    /* cast needed for braindead systems with signed size_t */
	    if (stride[idim] == 0) {
                goto cleanup;
	    }
	}
    }

    mystart = (hssize_t *)calloc(varp->ndims * 7, sizeof(hsize_t));
    if (mystart == NULL) {
        goto cleanup;
    }

    myedges = (hsize_t *)(mystart + varp->ndims);
    iocount = myedges + varp->ndims;
    stop = iocount + varp->ndims;
    length = stop + varp->ndims;
    mystride = (ptrdiff_t *)(length + varp->ndims);
    mymap = mystride + varp->ndims;

    /*
     * Initialize I/O parameters.
     */
    for (idim = maxidim; idim >= 0; --idim) {
	mystart[idim] = start != NULL ? start[idim] : 0;

	/* Hmm - should this even _BE_ here? */
	if (edges[idim] == 0) {
	    status = MI_NOERROR; /* nothing to do here */
	    goto cleanup;
	}

	if (edges != NULL) {
	    myedges[idim] = edges[idim];
	}
	else {
	    myedges[idim] = varp->dims[idim] - mystart[idim];
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
	if (mystart[idim] >= varp->dims[idim]) {
	    status = MI_ERROR;
	    goto cleanup;
	}
	if (mystart[idim] + myedges[idim] > varp->dims[idim]) {
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

    mspc_id = H5Screate_simple(varp->ndims, iocount, NULL);

    /*
     * Perform I/O.  Exit when done.
     */
    for (;;) {
        status = H5Sselect_hyperslab(fspc_id, H5S_SELECT_SET, mystart, NULL, 
                                     iocount, NULL);
        if (status < 0) {
            milog_message(MI_MSG_SNH);
            goto cleanup;
        }

        status = H5Dwrite(dst_id, typ_id, mspc_id, fspc_id, H5P_DEFAULT, 
                          value);
        if (status < 0) {
            milog_message(MI_MSG_WRITEDSET, varp->path);
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
	value = ((char *)value) + mymap[idim];
	mystart[idim] += mystride[idim];
	if (mystart[idim] == stop[idim]) {
	    mystart[idim] = start[idim];
	    value = ((char *)value) - length[idim];
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
    return (status);
}

public int 
hdf_vargetg(int fd, int varid, const long *start, 
            const long *edges, const long *stride, 
            const long *map, void *value)
{
    int status = MI_NOERROR;
    struct m2_var *varp;
    int maxidim;		/* maximum dimensional index */
    struct m2_file *file;
    int idim;
    long *mystart = NULL;
    long *myedges;
    long *iocount;	/* count vector */
    long *stop;	/* stop indexes */
    long *length;	/* edge lengths in bytes */
    long *mystride;
    long *mymap;

    file = hdf_id_check(fd);
    if (file == NULL) {
	return (MI_ERROR);
    }

    varp = hdf_var_byid(file, varid);
    if (varp == NULL) {
	return (MI_ERROR);
    }

    maxidim = (int) varp->ndims - 1;

    if (maxidim < 0) {
	/*
	 * The variable is a scalar!
	 */
        milog_message(MI_MSG_SNH);
	return (MI_ERROR);
    }


    /*
     * Verify stride argument.
     */
    if (stride != NULL) {
	for (idim = 0; idim <= maxidim; idim++) {
	    if (stride[idim] == 0) {
		return MI_ERROR;
	    }
	}
    }

    mystart = (long *)calloc(varp->ndims * 7, sizeof(long));
    if (mystart == NULL) {
	return (MI_ERROR);
    }
    myedges = mystart + varp->ndims;
    iocount = myedges + varp->ndims;
    stop = iocount + varp->ndims;
    length = stop + varp->ndims;
    mystride = length + varp->ndims;
    mymap = mystride + varp->ndims;

    /*
     * Initialize I/O parameters.
     */
    for (idim = maxidim; idim >= 0; --idim) {
	mystart[idim] = start != NULL ? start[idim] : 0;

	/* Hmm - should this even _BE_ here? */
	if (edges[idim] == 0) {
	    status = MI_NOERROR; /* nothing to do here */
	    goto done;
	}

	if (edges != NULL) {
	    myedges[idim] = edges[idim];
	}
	else {
	    myedges[idim] = varp->dims[idim] - mystart[idim];
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
	if (mystart[idim] >= varp->dims[idim]) {
	    status = MI_ERROR;
	    goto done;
	}
	if (mystart[idim] + myedges[idim] > varp->dims[idim]) {
	    status = MI_ERROR;
	    goto done;
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
	int lstatus = hdf_varget(fd, varid, mystart, iocount, value);
	if (lstatus != MI_NOERROR && status == MI_NOERROR) {
	    status = lstatus;
	}

	/*
	 * The following code permutes through the variable's
	 * external start-index space and it's internal address
	 * space.  At the UPC, this algorithm is commonly
	 * called "odometer code".
	 */
	idim = maxidim;
    carry:
	value = ((char *)value) + mymap[idim];
	mystart[idim] += mystride[idim];
	if (mystart[idim] == stop[idim]) {
	    mystart[idim] = start[idim];
	    value = ((char *)value) - length[idim];
	    if (--idim < 0)
		break; /* normal return */
	    goto carry;
	}
    } /* I/O loop */
 done:
    free(mystart);
    return (status);
}

public int
hdf_varput(int fd, int varid, const long *start_ptr, const long *length_ptr,
	   const void *val_ptr)
{
  int status = MI_ERROR;
  int dst_id;
  int typ_id;
  int fspc_id;
  int mspc_id = -1;
  int i;
  int ndims;
  hssize_t fstart[MAX_VAR_DIMS];
  hsize_t count[MAX_VAR_DIMS];
  struct m2_file *file;
  struct m2_var *var;

  if (varid == MI_ROOTVARIABLE_ID) {
      return (MI_NOERROR);
  }

  if ((file = hdf_id_check(fd)) == NULL) {
      return (MI_ERROR);
  }

  if ((var = hdf_var_byid(file, varid)) == NULL) {
      return (MI_ERROR);
  }

  dst_id = var->dset_id;
  typ_id = var->mtyp_id;
  fspc_id = var->fspc_id;

  ndims = var->ndims;
  if (ndims == 0) {
      mspc_id = H5Screate(H5S_SCALAR);
  }
  else {
    for (i = 0; i < ndims; i++) {
      fstart[i] = start_ptr[i];
      count[i] = length_ptr[i];
    }

    status = H5Sselect_hyperslab(fspc_id, H5S_SELECT_SET, fstart, NULL, count,
				 NULL);
    if (status < 0) {
        milog_message(MI_MSG_SNH);
        goto cleanup;
    }

    mspc_id = H5Screate_simple(ndims, count, NULL);
    if (mspc_id < 0) {
        milog_message(MI_MSG_SNH);
      goto cleanup;
    }
  }

  status = H5Dwrite(dst_id, typ_id, mspc_id, fspc_id, H5P_DEFAULT, val_ptr);
  if (status < 0) {
      milog_message(MI_MSG_WRITEDSET, var->path);
      goto cleanup;
  }

 cleanup:

  if (mspc_id >= 0)
    H5Sclose(mspc_id);

  return (status);
}

public int
hdf_varput1(int fd, int varid, const long *mindex_ptr, const void *val_ptr)
{
  long length[MAX_VAR_DIMS];
  int i;

  /* Rather than querying the number of dimensions, just fill the whole
   * darn thing with 1's.
   */
  for (i = 0; i < MAX_VAR_DIMS; i++) {
    length[i] = 1;
  }

  return hdf_varput(fd, varid, mindex_ptr, length, val_ptr);
}

/** Emulates ncattdel().  Like many of the netCDF attribute functions,
 * success here is indicated by a return value of one.
 */
public int
hdf_attdel(int fd, int varid, const char *attnm)
{
    hid_t loc_id;
    struct m2_file *file;
    struct m2_var *var;

    if ((file = hdf_id_check(fd)) == NULL) {
        return (MI_ERROR);
    }

    if (varid == NC_GLOBAL) {
        var = NULL;
        loc_id = file->grp_id;
    } 
    else {
        if ((var = hdf_var_byid(file, varid)) == NULL) {
            return (MI_ERROR);
        }
        loc_id = var->dset_id;
    }
    H5Adelete(loc_id, attnm);
    return 1;                   /* success */
}

/* Get the current size of a variable. This is needed in HDF5 since 
 * dimensions are associated with individual variable's dataspaces,
 * and can be independent, rather than set by a file-wide "dimension"
 * object.
 */
public int
hdf_varsize(int fd, int varid, long *size_ptr)
{
    int i;
    hsize_t dims[MAX_VAR_DIMS];

    struct m2_file *file;
    struct m2_var *var;

    if (varid == MI_ROOTVARIABLE_ID) {
        *size_ptr = 1;
        return (MI_NOERROR);
    }

    if ((file = hdf_id_check(fd)) == NULL) {
        return (MI_ERROR);
    }
    if ((var = hdf_var_byid(file, varid)) == NULL) {
        return (MI_ERROR);
    }

    if (var->ndims > MAX_VAR_DIMS) {
        milog_message(MI_MSG_TOOMANYDIMS, MAX_VAR_DIMS);
        exit(-1);
    }

    H5Sget_simple_extent_dims(var->fspc_id, dims, NULL);

    for (i = 0; i < var->ndims; i++) {
        size_ptr[i] = dims[i];
    }
    return (MI_NOERROR);
}

public herr_t
hdf_copy_attr(hid_t in_id, const char *attr_name, void *op_data)
{
   hid_t out_id = (hid_t) op_data;
   hid_t inatt_id = -1;
   hid_t outatt_id = -1;
   hid_t spc_id = -1;
   hid_t typ_id = -1;
   void *val_ptr = NULL;
   int status = MI_ERROR;

   if ((inatt_id = H5Aopen_name(in_id, attr_name)) < 0)
     goto cleanup;

   if ((spc_id = H5Aget_space(inatt_id)) < 0)
     goto cleanup;

   if ((typ_id = H5Aget_type(inatt_id)) < 0)
     goto cleanup;

   outatt_id = H5Acreate(out_id, attr_name, typ_id, spc_id, H5P_DEFAULT);
   if (outatt_id < 0) {
     /* This can happen if the attribute already exists.  If it does, we
      * don't overwrite the existing value.
      */
     status = MI_NOERROR;
     goto cleanup;
   }
   
   if ((val_ptr = malloc(hdf_size(spc_id, typ_id))) == NULL)
     goto cleanup;

   if (H5Aread(inatt_id, typ_id, val_ptr) < 0)
     goto cleanup;

   if (H5Awrite(outatt_id, typ_id, val_ptr) < 0)
     goto cleanup;

   status = MI_NOERROR;

 cleanup:

   if (val_ptr != NULL)
     free(val_ptr);

   if (spc_id >= 0)
     H5Sclose(spc_id);

   if (typ_id >= 0)
     H5Tclose(typ_id);

   if (inatt_id >= 0)
     H5Aclose(inatt_id);

   if (outatt_id >= 0)
     H5Aclose(outatt_id);

   return (status);
}

int
hdf_open_dsets(struct m2_file *file, hid_t grp_id, char *cpath, int is_dim)
{
    hsize_t nobjs;
    hsize_t idx;
    char temp[NC_MAX_NAME];
    char tpath[NC_MAX_NAME];
    hid_t new_id;
    herr_t result;

    result = H5Gget_num_objs(grp_id, &nobjs);
    if (result < 0) {
	return (MI_ERROR);
    }

    for (idx = 0; idx < nobjs; idx++) {
	switch (H5Gget_objtype_by_idx(grp_id, idx)) {
	case H5G_GROUP:
	    H5Gget_objname_by_idx(grp_id, idx, temp, sizeof(temp));
	    strcpy(tpath, cpath);
	    strcat(tpath, temp);
	    strcat(tpath, "/");
	    new_id = H5Gopen(grp_id, temp);
	    if (new_id >= 0) {
		hdf_open_dsets(file, new_id, tpath, is_dim);
		H5Gclose(new_id);
	    }
	    break;

	case H5G_DATASET:
	    H5Gget_objname_by_idx(grp_id, idx, temp, sizeof(temp));
	    strcpy(tpath, cpath);
	    strcat(tpath, temp);
	    new_id = H5Dopen(grp_id, temp);
	    if (new_id >= 0) {
		hid_t spc_id;
		spc_id = H5Dget_space(new_id);
		if (spc_id < 0) {
                    milog_message(MI_MSG_SNH);
		}
		else {
		    hsize_t dims[MAX_NC_DIMS];
		    int ndims;
		    hdf_get_diminfo(new_id, &ndims, dims);
		    hdf_var_add(file, temp, tpath, ndims, dims);
		}

		if (is_dim) {
		    long length;
		    hid_t att_id;

		    att_id = H5Aopen_name(new_id, MI2_LENGTH);
                    if (att_id > 0) {
                        H5Aread(att_id, H5T_NATIVE_LONG, (void *) &length);
                        H5Aclose(att_id);
                    }
                    else {
                        milog_message(MI_MSG_SNH);
                    }

		    hdf_dim_add(file, temp, length);
		}
		H5Dclose(new_id);
	    }
	    break;
	}
    }
    return (MI_NOERROR);
}

int
hdf_open(const char *path, int mode)
{
    hid_t fd;
    hid_t grp_id;
    hid_t dset_id;
    struct m2_file *file;
    hsize_t dims[MAX_NC_DIMS];
    int ndims;

    H5E_BEGIN_TRY {
        fd = H5Fopen(path, mode, H5P_DEFAULT);
    } H5E_END_TRY;

    if (fd < 0) {
	return (MI_ERROR);
    }

    file = hdf_id_add(fd);	/* Add it to the list */

    /* Open the image variables.
     */
    H5E_BEGIN_TRY {
        dset_id = H5Dopen(fd, "/minc-2.0/image/0/image");
        if (dset_id >= 0) {
            hdf_get_diminfo(dset_id, &ndims, dims);
            hdf_var_add(file, MIimage, "/minc-2.0/image/0/image", 
                        ndims, dims);
            H5Dclose(dset_id);
        }
    
        dset_id = H5Dopen(fd, "/minc-2.0/image/0/image-min");
        if (dset_id >= 0) {
            hdf_get_diminfo(dset_id, &ndims, dims);
            hdf_var_add(file, MIimagemin, "/minc-2.0/image/0/image-min", 
                        ndims, dims);
            H5Dclose(dset_id);
        }

        dset_id = H5Dopen(fd, "/minc-2.0/image/0/image-max");
        if (dset_id >= 0) {
            hdf_get_diminfo(dset_id, &ndims, dims);
            hdf_var_add(file, MIimagemax, "/minc-2.0/image/0/image-max", 
                        ndims, dims);
            H5Dclose(dset_id);
        }
    } H5E_END_TRY;

    /* Open all of the datasets in the "dimensions" category.
     */
    grp_id = H5Gopen(fd, "/minc-2.0/dimensions");
    hdf_open_dsets(file, grp_id, "/minc-2.0/dimensions/", 1);
    H5Gclose(grp_id);

    /* Open all of the datasets in the "info" category.
     */
    grp_id = H5Gopen(fd, "/minc-2.0/info");
    hdf_open_dsets(file, grp_id, "/minc-2.0/info/", 0);
    H5Gclose(grp_id);
    return (fd);
}

/** Create an HDF5 file. */
int
hdf_create(const char *path, int cmode, struct mi2opts *opts_ptr)
{
    hid_t grp_id;
    hid_t fd;
    hid_t tmp_id;
    struct m2_file *file;

    /* Convert the MINC (NetCDF) mode to a HDF5 mode.
     */
    if (cmode & NC_NOCLOBBER) {
	cmode = H5F_ACC_EXCL;
    }
    else {
	cmode = H5F_ACC_TRUNC;
    }

    H5E_BEGIN_TRY {
        fd = H5Fcreate(path, cmode, H5P_DEFAULT, H5P_DEFAULT);
    } H5E_END_TRY;
    if (fd < 0) {
	return (MI_ERROR);
    }

    /* Create the default groups.
     * Should we use a non-zero value for size_hint (parameter 3)???
     */
    if ((grp_id = H5Gcreate(fd, MI2_GRPNAME, 0)) < 0) {
	return (MI_ERROR);
    }

    if ((tmp_id = H5Gcreate(grp_id, "dimensions", 0)) < 0) {
	return (MI_ERROR);
    }
    H5Gclose(tmp_id);

    if ((tmp_id = H5Gcreate(grp_id, "info", 0)) < 0) {
	return (MI_ERROR);
    }
    H5Gclose(tmp_id);

    if ((tmp_id = H5Gcreate(grp_id, "image", 0)) < 0) {
	return (MI_ERROR);
    }
    H5Gclose(tmp_id);

    if ((tmp_id = H5Gcreate(grp_id, "image/0", 0)) < 0) {
	return (MI_ERROR);
    }
    H5Gclose(tmp_id);

    H5Gclose(grp_id);

    file = hdf_id_add(fd);      /* Add it to the list */
    if (file == NULL) {
        return (MI_ERROR);      /* Should not happen?? */
    }

    if (opts_ptr != NULL && opts_ptr->struct_version == MI2_OPTS_V1) {
        file->comp_type = opts_ptr->comp_type;
        file->comp_param = opts_ptr->comp_param;
        file->chunk_type = opts_ptr->chunk_type;
        file->chunk_param = opts_ptr->chunk_param;
    }
    return ((int) fd);
}

int
hdf_close(int fd)
{
    hdf_dim_commit(fd);         /* Make sure all dimensions were saved. */
    hdf_id_del(fd);		/* Delete it from the list. */
    H5Fclose(fd);
    return (MI_NOERROR);
}

#ifdef HDF_TEST
main(int argc, char **argv)
{
  int dims[5] = {0,0,0,0,0};
  int i;
  int n;

  while (--argc > 0) {
    n = hdf_parse_dimorder(*++argv, 5, dims);
    for (i = 0; i < n; i++) {
      printf("%d ", dims[i]);
    }
    printf("\n");
  }
}
#endif /* HDF_TEST defined */
#endif /* MINC2 defined */
