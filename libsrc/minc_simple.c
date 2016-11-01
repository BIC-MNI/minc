/* ----------------------------- MNI Header -----------------------------------
@NAME       : minc_simple.c
@DESCRIPTION: Simplified interface to 3D and 4D minc files
@METHOD     : Routines included in this file :
@CREATED    : August 20, 2004. (Bert Vincent, Montreal Neurological Institute)
@MODIFIED   : 
 * $Log: minc_simple.c,v $
 * Revision 6.6  2008/01/11 07:17:07  stever
 * Remove unused variables.
 *
 * Revision 6.5  2005/08/26 21:04:58  bert
 * Use #if rather than #ifdef with MINC2 symbol
 *
 * Revision 6.4  2005/05/20 21:01:52  bert
 * Declare all public functions MNCAPI
 *
 * Revision 6.3  2005/01/04 22:45:57  bert
 * Adopt Leila's changes to restructure_array() and make appropriate corrections to the rest of the code
 *
 * Revision 6.2  2004/12/14 23:53:46  bert
 * Get rid of compilation warnings
 *
 * Revision 6.1  2004/11/01 22:06:48  bert
 * Initial checkin, simplified minc interface
 *
@COPYRIGHT  :
              Copyright 2004 Robert Vincent, McConnell Brain Imaging Centre, 
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

#include "minc_private.h"
#include <math.h>               /* for sqrt */
#include <float.h>              /* for DBL_MAX */
#include "minc_simple.h"

/* Trivial MINC interface */

#define MI_S_T 0
#define MI_S_Z 1
#define MI_S_Y 2
#define MI_S_X 3
#define MI_S_NDIMS 4

static char *minc_dimnames[] = {
    MItime, 
    MIzspace,
    MIyspace,
    MIxspace
};

/* private function from from libminc2.  This function is private partially
 because it's parameters are somewhat bizarre.  It would be a good idea to
 rework them into a more rational and easily described form. 
*/

MNCAPI void restructure_array(int ndims,
                              unsigned char *array,
                              const unsigned long *lengths_perm,
                              int el_size,
                              const int *map,
                              const int *dir);

/* Structures used to represent a file in memory.
 */
struct att_info {
    char att_name[128];
    nc_type att_type;
    int att_len;
    void *att_val;
};

struct var_info {
    char var_name[128];
    nc_type var_type;
    int var_natts;
    int var_ndims;
    int var_dims[MAX_NC_DIMS];
    struct att_info *var_atts;
};

struct file_info {
    int file_ndims;
    int file_nvars;
    int file_natts;
    struct att_info *file_atts;
    struct var_info *file_vars;
};

static int
minc_simple_to_nc_type(int minctype, nc_type *nctype, char **signstr)
{
    switch (minctype) {
    case MINC_TYPE_CHAR:
        *nctype = NC_BYTE;
        *signstr = MI_SIGNED;
        break;
        
    case MINC_TYPE_UCHAR:
        *nctype = NC_BYTE;
        *signstr = MI_UNSIGNED;
        break;
        
    case MINC_TYPE_SHORT:
        *nctype = NC_SHORT;
        *signstr = MI_SIGNED;
        break;
        
    case MINC_TYPE_USHORT:
        *nctype = NC_SHORT;
        *signstr = MI_UNSIGNED;
        break;
        
    case MINC_TYPE_INT:
        *nctype = NC_INT;
        *signstr = MI_SIGNED;
        break;
        
    case MINC_TYPE_UINT:
        *nctype = NC_INT;
        *signstr = MI_UNSIGNED;
        break;
        
    case MINC_TYPE_FLOAT:
        *nctype = NC_FLOAT;
        *signstr = MI_SIGNED;
        break;
        
    case MINC_TYPE_DOUBLE:
        *nctype = NC_DOUBLE;
        *signstr = MI_SIGNED;
        break;
        
    default:
        return (MINC_STATUS_ERROR);
    }
    return (MINC_STATUS_OK);
}

MNCAPI int
minc_file_size(char *path,
               long *ct, long *cz, long *cy, long *cx,
               long *cvoxels, long *cbytes)
{
    int fd;
    nc_type nctype;
    int dim_id[MI_S_NDIMS];
    long dim_len[MI_S_NDIMS];
    int i;
    int var_id;
    int var_ndims;
    int var_dims[MAX_NC_DIMS];
    long voxel_count;
    long byte_count;
    int old_ncopts;

    fd = miopen(path, NC_NOWRITE);
    if (fd < 0) {
        return (MINC_STATUS_ERROR);
    }
    
    old_ncopts = ncopts;
    ncopts = 0;

    for (i = 0; i < MI_S_NDIMS; i++) {
        dim_id[i] = ncdimid(fd, minc_dimnames[i]);
        if (dim_id[i] >= 0) {
            ncdiminq(fd, dim_id[i], NULL, &dim_len[i]);
        }
        else {
            dim_len[i] = 0;
        }
    }

    ncopts = old_ncopts;

    if (ct != NULL) {
        *ct = dim_len[MI_S_T];
    }
    if (cz != NULL) {
        *cz = dim_len[MI_S_Z];
    }
    if (cy != NULL) {
        *cy = dim_len[MI_S_Y];
    }
    if (cx != NULL) {
        *cx = dim_len[MI_S_X];
    }

    var_id = ncvarid(fd, MIimage);

    if (cvoxels != NULL || cbytes != NULL) {
        ncvarinq(fd, var_id, NULL, &nctype, &var_ndims, var_dims, NULL);

        voxel_count = 1;

        for (i = 0; i < var_ndims; i++) {
            long length;
            ncdiminq(fd, var_dims[i], NULL, &length);
            voxel_count *= length;
        }

        byte_count = voxel_count * nctypelen(nctype);

        if (cvoxels != NULL) {
            *cvoxels = voxel_count;
        }
        if (cbytes != NULL) {
            *cbytes = byte_count;
        }
    }
    return (MINC_STATUS_OK);
}

MNCAPI int
minc_load_data(char *path, void *dataptr, int datatype,
               long *ct, long *cz, long *cy, long *cx,
               double *dt, double *dz, double *dy, double *dx,
               void **infoptr)
{
    int fd;                     /* MINC file descriptor */
    nc_type nctype;             /* netCDF type */
    char *signstr;              /* MI_SIGNED or MI_UNSIGNED */
    int length;
    int dim_id[MI_S_NDIMS];
    long dim_len[MI_S_NDIMS];
    int i, j;                   /* Generic loop counters */
    int var_id;
    int var_ndims;
    int var_dims[MAX_NC_DIMS];
    int icv;                    /* MINC image conversion variable */
    long start[MI_S_NDIMS];
    long count[MI_S_NDIMS];
    int dir[MI_S_NDIMS];        /* Dimension "directions" */
    int map[MI_S_NDIMS];        /* Dimension mapping */
    int old_ncopts;             /* For storing the old state of ncopts */
    double *p_dtmp;
    long *p_ltmp;
    struct file_info *p_file;
    struct att_info *p_att;
    int r;                      /* Generic return code */
    
    *infoptr = NULL;

    fd = miopen(path, NC_NOWRITE);
    if (fd < 0) {
        return (MINC_STATUS_ERROR);
    }

    old_ncopts = ncopts;
    ncopts = 0;

    for (i = 0; i < MI_S_NDIMS; i++) {
        dim_id[i] = ncdimid(fd, minc_dimnames[i]);
        if (dim_id[i] >= 0) {
            ncdiminq(fd, dim_id[i], NULL, &dim_len[i]);
            var_id = ncvarid(fd, minc_dimnames[i]);
            ncattinq(fd, var_id, MIstep, &nctype, &length);

            switch (i) {
            case MI_S_T:
                p_ltmp = ct;
                p_dtmp = dt;
                break;
            case MI_S_X:
                p_ltmp = cx;
                p_dtmp = dx;
                break;
            case MI_S_Y:
                p_ltmp = cy;
                p_dtmp = dy;
                break;
            case MI_S_Z:
                p_ltmp = cz;
                p_dtmp = dz;
                break;
            default:
                return (MINC_STATUS_ERROR);
            }
                
            if (nctype == NC_DOUBLE && length == 1) {
                ncattget(fd, var_id, MIstep, p_dtmp);
            }
            else {
                *p_dtmp = 0;    /* Unknown/not set */
            }
            *p_ltmp = dim_len[i];
        }
        else {
            dim_len[i] = 0;
        }
    }

    ncopts = old_ncopts;

    var_id = ncvarid(fd, MIimage);

    ncvarinq(fd, var_id, NULL, &nctype, &var_ndims, var_dims, NULL);

    if (var_ndims != 3 && var_ndims != 4) {
        return (MINC_STATUS_ERROR);
    }

    /* We want the data to wind up in t, x, y, z order. */

    for (i = 0; i < MI_S_NDIMS; i++) {
        map[i] = -1;
    }

    for (i = 0; i < var_ndims; i++) {
        if (var_dims[i] == dim_id[MI_S_T]) {
            map[MI_S_T] = i;
        }
        else if (var_dims[i] == dim_id[MI_S_X]) {
            map[MI_S_X] = i;
        }
        else if (var_dims[i] == dim_id[MI_S_Y]) {
            map[MI_S_Y] = i;
        }
        else if (var_dims[i] == dim_id[MI_S_Z]) {
            map[MI_S_Z] = i;
        }
    }

    icv = miicv_create();

    minc_simple_to_nc_type(datatype, &nctype, &signstr);
    miicv_setint(icv, MI_ICV_TYPE, nctype);
    miicv_setstr(icv, MI_ICV_SIGN, signstr);
    miicv_attach(icv, fd, var_id);

    for (i = 0; i < var_ndims; i++) {
        start[i] = 0;
    }

    for (i = 0; i < MI_S_NDIMS; i++) {
        if (map[i] >= 0) {
            count[map[i]] = dim_len[i];
        }
    }

    r = miicv_get(icv, start, count, dataptr);
    if (r < 0) {
        return (MINC_STATUS_ERROR);
    }

    if (map[MI_S_T] >= 0) {
        if (*dt < 0) {
            dir[MI_S_T] = -1;
            *dt = -*dt;
        }
        else {
            dir[MI_S_T] = 1;
        }
    }

    if (map[MI_S_X] >= 0) {
        if (*dx < 0) {
            dir[MI_S_X] = -1;
            *dx = -*dx;
        }
        else {
            dir[MI_S_X] = 1;
        }
    }

    if (map[MI_S_Y] >= 0) {
        if (*dy < 0) {
            dir[MI_S_Y] = -1;
            *dy = -*dy;
        }
        else {
            dir[MI_S_Y] = 1;
        }
    }

    if (map[MI_S_Z] >= 0) {
        if (*dz < 0) {
            dir[MI_S_Z] = -1;
            *dz = -*dz;
        }
        else {
            dir[MI_S_Z] = 1;
        }
    }

    if (var_ndims == 3) {
        for (i = 1; i < MI_S_NDIMS; i++) {
            map[i-1] = map[i];
            dir[i-1] = dir[i];
        }
    }

    j = 0;
    for (i = 0; i < MI_S_NDIMS; i++) {
        if (dim_len[i] > 0) {
            count[j++] = dim_len[i];
        }
    }

    restructure_array(var_ndims, dataptr, count, nctypelen(nctype),
                      map, dir);

    miicv_detach(icv);
    miicv_free(icv);

    old_ncopts = ncopts;
    ncopts = 0;

    /* Generate the complete infoptr array.
     * This is essentially an in-memory copy of the variables and attributes
     * in the file.
     */

    p_file = (struct file_info *) malloc(sizeof (struct file_info));

    ncinquire(fd, &p_file->file_ndims, &p_file->file_nvars,
              &p_file->file_natts, NULL);

    p_file->file_atts = (struct att_info *) malloc(sizeof (struct att_info) * 
                                                   p_file->file_natts);

    p_file->file_vars = (struct var_info *) malloc(sizeof (struct var_info) *
                                                   p_file->file_nvars);

    for (i = 0; i < p_file->file_natts; i++) {
        p_att = &p_file->file_atts[i];

        ncattname(fd, NC_GLOBAL, i, p_att->att_name);
        ncattinq(fd, NC_GLOBAL, 
                 p_att->att_name, 
                 &p_att->att_type, 
                 &p_att->att_len);

        p_att->att_val = malloc(p_att->att_len * nctypelen(p_att->att_type));

        ncattget(fd, NC_GLOBAL, p_att->att_name, p_att->att_val);
    }

    for (i = 0; i < p_file->file_nvars; i++) {
        struct var_info *p_var = &p_file->file_vars[i];

        ncvarinq(fd, i, 
                 p_var->var_name, 
                 &p_var->var_type, 
                 &p_var->var_ndims, 
                 p_var->var_dims,
                 &p_var->var_natts);

        p_var->var_atts = malloc(p_var->var_natts *
                                 sizeof (struct att_info));

        if (ncdimid(fd, p_var->var_name) >= 0) {
            /* It's a dimension variable, have to treat it specially... */
        }

        for (j = 0; j < p_var->var_natts; j++) {
            p_att = &p_var->var_atts[j];

            ncattname(fd, i, j, p_att->att_name);
            ncattinq(fd, i, 
                     p_att->att_name, 
                     &p_att->att_type, 
                     &p_att->att_len);

            p_att->att_val = malloc(p_att->att_len * nctypelen(p_att->att_type));
            ncattget(fd, i, p_att->att_name, p_att->att_val);
        }
    }

    *infoptr = p_file;

    ncopts = old_ncopts;

    miclose(fd);

    return (MINC_STATUS_OK);
}

MNCAPI void
minc_free_info(void *infoptr)
{
    struct file_info *p_file;
    int i, j;

    if ((p_file = infoptr) != NULL) {
        if (p_file->file_natts != 0 && p_file->file_atts != NULL) {
            for (i = 0; i < p_file->file_natts; i++) {
                free(p_file->file_atts[i].att_val);
            }
            free(p_file->file_atts);
        }

        if (p_file->file_nvars != 0 && p_file->file_vars != NULL) {
            for (i = 0; i < p_file->file_nvars; i++) {
                if (p_file->file_vars[i].var_natts != 0 &&
                    p_file->file_vars[i].var_atts != NULL) {
                    for (j = 0; j < p_file->file_vars[i].var_natts; j++) {
                        if (p_file->file_vars[i].var_atts[j].att_val != NULL) {
                            free(p_file->file_vars[i].var_atts[j].att_val);
                        }
                    }
                    free(p_file->file_vars[i].var_atts);
                }
            }
            free(p_file->file_vars);
        }
    }
}

/* int minc_save_start()
 *
 * Returns an opaque handle which must be passed to successive calls to
 * minc_save_data() and ultimately to minc_save_done().
 */

MNCAPI int
minc_save_start(char *path,     /* Path to the file */
                int filetype,   /* Date type as stored in the file */
                long ct,        /* Total length of time axis, in voxels */
                long cz,        /* Total length of Z axis, in voxels */
                long cy,        /* Total length of Y axis, in voxels */
                long cx,        /* Total length of X axis, in voxels */
                double dt,      /* Sample width along time axis, in seconds */
                double dz,      /* Sample width along Z axis, in mm */
                double dy,      /* Sample width along Y axis, in mm */
                double dx,      /* Sample width along X axis, in mm */
                void *infoptr,  /* Opaque file structure information */
                const char *history) /* New history information */
{
    int fd;                     /* MINC file descriptor */
    int dim_id[MI_S_NDIMS];     /* netCDF dimension ID array */
    int var_ndims;              /* Number of dimensions per variable */
    int var_dims[MI_S_NDIMS];   /* Dimension ID's per variable */
    int i, j;                   /* Generic loop counters */
    int old_ncopts;             /* For supressing fatal error messages */
    struct file_info *p_file;   /* For accessing the file structure */
    struct var_info *p_var;
    struct att_info *p_att;
    int var_id;                 /* netCDF ID for variable */
    char *signstr;
    nc_type nctype;

    old_ncopts = ncopts;
    ncopts = 0;

    fd = micreate(path, NC_CLOBBER);
    if (fd < 0) {
        return (MINC_STATUS_ERROR);
    }

    if (ct > 0) {
        dim_id[MI_S_T] = ncdimdef(fd, MItime, ct);
        micreate_std_variable(fd, MItime, NC_INT, 0, NULL);
        if (dt > 0.0) {
            miattputdbl(fd, ncvarid(fd, MItime), MIstep, dt);
        }
    }
    else {
        dim_id[MI_S_T] = -1;
    }

    if (cz > 0) {
        dim_id[MI_S_Z] = ncdimdef(fd, MIzspace, cz);
        micreate_std_variable(fd, MIzspace, NC_INT, 0, NULL);
        if (dz > 0.0) {
            miattputdbl(fd, ncvarid(fd, MIzspace), MIstep, dz);
        }
    }
    else {
        dim_id[MI_S_Z] = -1;
    }

    if (cy > 0) {
        dim_id[MI_S_Y] = ncdimdef(fd, MIyspace, cy);
        micreate_std_variable(fd, MIyspace, NC_INT, 0, NULL);
        if (dy > 0.0) {
            miattputdbl(fd, ncvarid(fd, MIyspace), MIstep, dy);
        }
    }
    else {
        return (MINC_STATUS_ERROR); /* Must define Y */
    }

    if (cx > 0) {
        dim_id[MI_S_X] = ncdimdef(fd, MIxspace, cx);
        micreate_std_variable(fd, MIxspace, NC_INT, 0, NULL);
        if (dx > 0.0) {
            miattputdbl(fd, ncvarid(fd, MIxspace), MIstep, dx);
        }
    }
    else {
        return (MINC_STATUS_ERROR); /* Must define X */
    }
    

    /* The var_dims[] array is the array of actual dimension ID's to
     * be used in defining the image variables.  Here I set it up by
     * copying all valid dimension ID's from the dim_id[] array.
     */
    var_ndims = 0;
    for (i = 0; i < MI_S_NDIMS; i++) {
        if (dim_id[i] >= 0) {
            var_dims[var_ndims] = dim_id[i];
            var_ndims++;
        }
    }

    minc_simple_to_nc_type(filetype, &nctype, &signstr);

    /* Create the image variable with the standard
     * dimension order, and the same type as the template
     * file.
     */
    micreate_std_variable(fd, MIimage, nctype, var_ndims, var_dims);
    micreate_std_variable(fd, MIimagemin, NC_DOUBLE, 1, var_dims);
    micreate_std_variable(fd, MIimagemax, NC_DOUBLE, 1, var_dims);

    /* Copy information from the infoptr to the output. 
     */
    if ((p_file = infoptr) != NULL) {
        old_ncopts = ncopts;
        ncopts = 0;

        for (i = 0; i < p_file->file_natts; i++) {
            p_att = &p_file->file_atts[i];
            if (strcmp(p_att->att_name, "ident") != 0) {
                ncattput(fd, NC_GLOBAL, p_att->att_name, p_att->att_type, 
                         p_att->att_len, p_att->att_val);
            }
        }

        for (i = 0; i < p_file->file_nvars; i++) {
            p_var = &p_file->file_vars[i];

            if ((var_id = ncvarid(fd, p_var->var_name)) < 0) {
                var_id = ncvardef(fd, p_var->var_name, p_var->var_type, 
                                  p_var->var_ndims, p_var->var_dims);
            }
            
            for (j = 0; j < p_var->var_natts; j++) {
                p_att = &p_var->var_atts[j];
                ncattput(fd, var_id, p_att->att_name, p_att->att_type,
                         p_att->att_len, p_att->att_val);
            }
        }
    }

    miattputstr(fd, ncvarid(fd, MIimage), MIcomplete, MI_FALSE);
    miattputstr(fd, ncvarid(fd, MIimage), MIsigntype, signstr);

    miappend_history(fd, history);
    ncendef(fd);
    return fd;
}

/* Internal function */
static void
find_minmax(void *dataptr, long datacount, int datatype, double *min, 
            double *max)
{
    *min = DBL_MAX;
    *max = -DBL_MAX;

    switch (datatype) {
    case MINC_TYPE_CHAR:
        {
            char *c_ptr = dataptr;
            while (datacount--) {
                if (*c_ptr > *max) {
                    *max = *c_ptr;
                }
                if (*c_ptr < *min) {
                    *min = *c_ptr;
                }
                c_ptr++;
            }
        }
        break;
    case MINC_TYPE_UCHAR:
        {
            unsigned char *c_ptr = dataptr;
            while (datacount--) {
                if (*c_ptr > *max) {
                    *max = *c_ptr;
                }
                if (*c_ptr < *min) {
                    *min = *c_ptr;
                }
                c_ptr++;
            }
        }
        break;
    case MINC_TYPE_SHORT:
        {
            short *s_ptr = dataptr;
            while (datacount--) {
                if (*s_ptr > *max) {
                    *max = *s_ptr;
                }
                if (*s_ptr < *min) {
                    *min = *s_ptr;
                }
                s_ptr++;
            }
        }
        break;
    case MINC_TYPE_USHORT:
        {
            unsigned short *s_ptr = dataptr;
            while (datacount--) {
                if (*s_ptr > *max) {
                    *max = *s_ptr;
                }
                if (*s_ptr < *min) {
                    *min = *s_ptr;
                }
                s_ptr++;
            }
        }
        break;
    case MINC_TYPE_INT:
        {
            int *i_ptr = dataptr;
            while (datacount--) {
                if (*i_ptr > *max) {
                    *max = *i_ptr;
                }
                if (*i_ptr < *min) {
                    *min = *i_ptr;
                }
                i_ptr++;
            }
        }
        break;
    case MINC_TYPE_UINT:
        {
            unsigned int *i_ptr = dataptr;
            while (datacount--) {
                if (*i_ptr > *max) {
                    *max = *i_ptr;
                }
                if (*i_ptr < *min) {
                    *min = *i_ptr;
                }
                i_ptr++;
            }
        }
        break;
    case MINC_TYPE_FLOAT:
        {
            float *f_ptr = dataptr;
            while (datacount--) {
                if (*f_ptr > *max) {
                    *max = *f_ptr;
                }
                if (*f_ptr < *min) {
                    *min = *f_ptr;
                }
                f_ptr++;
            }
        }
        break;
    case MINC_TYPE_DOUBLE:
        {
            double *d_ptr = dataptr;
            while (datacount--) {
                if (*d_ptr > *max) {
                    *max = *d_ptr;
                }
                if (*d_ptr < *min) {
                    *min = *d_ptr;
                }
                d_ptr++;
            }
        }
        break;
    default:
        return;
    }
}


MNCAPI int
minc_save_data(int fd, void *dataptr, int datatype,
               long st, long sz, long sy, long sx,
               long ct, long cz, long cy, long cx)
{
    nc_type nctype;
    char *signstr;
    int i;
    int var_id;
    int var_ndims;
    int var_dims[MAX_NC_DIMS];
    int icv;
    long start[MI_S_NDIMS];
    long count[MI_S_NDIMS];
    int old_ncopts;
    int r;
    double min, max;
    long slice_size;
    long index;
    int dtbytes;                /* Length of datatype in bytes */

    old_ncopts = ncopts;
    ncopts = 0;

    var_id = ncvarid(fd, MIimage);

    ncvarinq(fd, var_id, NULL, NULL, &var_ndims, var_dims, NULL);

    if (var_ndims < 2 || var_ndims > 4) {
        return (MINC_STATUS_ERROR);
    }
    
    r = minc_simple_to_nc_type(datatype, &nctype, &signstr);
    if (r == MINC_STATUS_ERROR) {
        return (MINC_STATUS_ERROR);
    }

    dtbytes = nctypelen(nctype);

    /* Update the image-min and image-max values */
    if (ct > 0) {
        slice_size = cz * cy * cx;
        index = st;

        for (i = 0; i < ct; i++) {
            find_minmax((char *) dataptr + (dtbytes * slice_size * i), 
                        slice_size, datatype, &min, &max);
        
            mivarput1(fd, ncvarid(fd, MIimagemin), &index, 
                      NC_DOUBLE, MI_SIGNED, &min);
            mivarput1(fd, ncvarid(fd, MIimagemax), &index, 
                      NC_DOUBLE, MI_SIGNED, &max);
            index++;
        }
    }
    else {
        slice_size = cy * cx;
        index = sz;

        for (i = 0; i < cz; i++) {
            find_minmax((char *) dataptr + (dtbytes * slice_size * i), 
                        slice_size, datatype, &min, &max);
            mivarput1(fd, ncvarid(fd, MIimagemin), &index, 
                      NC_DOUBLE, MI_SIGNED, &min);
            mivarput1(fd, ncvarid(fd, MIimagemax), &index, 
                      NC_DOUBLE, MI_SIGNED, &max);
            index++;
        }
    }

    /* We want the data to wind up in t, x, y, z order. */

    icv = miicv_create();
    if (icv < 0) {
        return (MINC_STATUS_ERROR);
    }

    r = miicv_setint(icv, MI_ICV_TYPE, nctype);
    if (r < 0) {
        return (MINC_STATUS_ERROR);
    }

    r = miicv_setstr(icv, MI_ICV_SIGN, signstr);
    if (r < 0) {
        return (MINC_STATUS_ERROR);
    }

    r = miicv_setint(icv, MI_ICV_DO_NORM, 1);
    if (r < 0) {
        return (MINC_STATUS_ERROR);
    }

    r = miicv_setint(icv, MI_ICV_DO_FILLVALUE, 1);
    if (r < 0) {
        return (MINC_STATUS_ERROR);
    }

    r = miicv_attach(icv, fd, var_id);
    if (r < 0) {
        return (MINC_STATUS_ERROR);
    }

    i = 0;
    switch (var_ndims) {
    case 4:
        count[i] = ct;
        start[i] = st;
        i++;
        /* fall through */
    case 3:
        count[i] = cz;
        start[i] = sz;
        i++;
        /* fall through */
    case 2:
        count[i] = cy;
        start[i] = sy;
        i++;
        
        count[i] = cx;
        start[i] = sx;
        i++;
        break;
    }

    r = miicv_put(icv, start, count, dataptr);
    if (r < 0) {
        return (MINC_STATUS_ERROR);
    }

    miicv_detach(icv);
    miicv_free(icv);
    return (MINC_STATUS_OK);
}

MNCAPI int 
minc_save_done(int fd)
{
    miattputstr(fd, ncvarid(fd, MIimage), MIcomplete, MI_TRUE);
    miclose(fd);
    return (MINC_STATUS_OK);
}


static void 
normalize_vector(double vector[MINC_3D])
{
    int i;
    double magnitude;

    magnitude = 0.0;
    for (i = 0; i < MINC_3D; i++) {
        magnitude += (vector[i] * vector[i]);
    }
    magnitude = sqrt(magnitude);
    if (magnitude > 0.0) {
        for (i = 0; i < MINC_3D; i++) {
            vector[i] /= magnitude;
        }
    }
}

MNCAPI int
minc_get_world_transform(char *path, double transform[MINC_3D][MINC_3D+1])
{
    int i, j;
    double dircos[MINC_3D];
    double step, start;
    char *dimensions[] = { MIxspace, MIyspace, MIzspace };
    int length;
    int fd;
    int varid;
    int old_ncopts;

    old_ncopts = ncopts;
    ncopts = 0;

    fd = miopen(path, NC_NOWRITE);
    if (fd < 0) {
        return (MINC_STATUS_ERROR);
    }

    /* Zero the matrix */
    for (i = 0; i < MINC_3D; i++) {
        for (j = 0; j < MINC_3D + 1; j++) {
          transform[i][j] = 0.0;
        }
        transform[i][i] = 1.0;
    }

   for (j = 0; j < MINC_3D; j++) {

      /* Set default values */
      step = 1.0;
      start = 0.0;
      for (i = 0; i < MINC_3D; i++) {
         dircos[i] = 0.0;
      }
      dircos[j] = 1.0;

      varid = ncvarid(fd, dimensions[j]);
      miattget(fd, varid, MIstart, NC_DOUBLE, 1, &start, &length);
      miattget(fd, varid, MIstep, NC_DOUBLE, 1, &step, &length);
      miattget(fd, varid, MIdirection_cosines, NC_DOUBLE, 3, dircos, &length);

      normalize_vector(dircos);

      /* Put them in the matrix.
       */
      for (i = 0; i < MINC_3D; i++) {
         transform[i][j] = step * dircos[i];
         transform[i][MINC_3D] += start * dircos[i];
      }

   }

   ncopts = old_ncopts;
   return (MINC_STATUS_OK);
}

#if !MINC2

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
MNCAPI void
restructure_array(int ndims,    /* Dimension count */
                  unsigned char *array, /* Raw data */
                  const unsigned long *lengths_perm, /* Permuted lengths */
                  int el_size,  /* Element size, in bytes */
                  const int *map, /* Mapping array */
                  const int *dir) /* Direction array, in permuted order */
{
    unsigned long index[MAX_VAR_DIMS]; /* Raw indices */
    unsigned long index_perm[MAX_VAR_DIMS]; /* Permuted indices */
    unsigned long lengths[MAX_VAR_DIMS]; /* Raw (unpermuted) lengths */
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
     * Permute the lengths from their "output" configuration back into
     * their "raw" or native order:
     **/
    for (i = 0; i < ndims; i++) {
      //lengths[i] = lengths_perm[map[i]];
        lengths[map[i]] = lengths_perm[i];
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
    if (bitmap == NULL) {
        return;
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

                offset_to_index(ndims, lengths_perm, offset, index_perm);
        
                /**
                 * Permute the index into the alternate arrangement.
                 **/

                for (i = 0; i < ndims; i++) {
                    if (dir[i] < 0) {
		      // index[i] = lengths[i] - index_perm[map[i]] - 1;
		      index[map[i]] = lengths[map[i]] - index_perm[i] - 1;
                    }
                    else {
		      //index[i] = index_perm[map[i]];
		      index[map[i]] = index_perm[i];
                    }
                }

                /**
                 * Calculate the next offset from the permuted index.
                 **/

                offset_next = index_to_offset(ndims, lengths, index);
#ifdef DEBUG
                if (offset_next >= total) {
                    printf("Fatal - offset %ld out of bounds!\n", offset_next);
                    printf("lengths %ld,%ld,%ld\n",
                           lengths[0],lengths[1],lengths[2]);
                    printf("index %ld,%ld,%ld\n",
                           index_perm[0], index_perm[0], index_perm[2]);
                    exit(-1);
                }
#endif
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

#endif /* !MINC2 */

#ifdef MINC_SIMPLE_TEST
/*
#define NC_TYPE MINC_TYPE_FLOAT
#define MM_TYPE float
#define MM_FMT "%20.16g"
#define EPSILON 1.0e-6
*/

#define NC_TYPE MINC_TYPE_DOUBLE
#define MM_TYPE double
#define MM_FMT "%20.16g"
#define EPSILON 1.0e-9


/*
#define NC_TYPE MINC_TYPE_CHAR
#define MM_TYPE unsigned char
#define MM_FMT "%d"
*/

/*
#define NC_TYPE MINC_TYPE_SHORT
#define MM_TYPE short
#define MM_FMT "%d"
*/

/*
#define NC_TYPE MINC_TYPE_INT
#define MM_TYPE int
#define MM_FMT "%d"
*/

#define CX 11
#define CY 12
#define CZ 9

main(int argc, char **argv)
{
    short buffer[1][256][256][256];
    long ct, cx, cy, cz;
    double dt, dx, dy, dz;
    long cvoxels, cbytes;
    int i, j, k;
    int h;

    MM_TYPE buf_xyz[CX][CY][CZ];
    MM_TYPE buf_xYz[CX][CY][CZ]; /* Negative Y */
    MM_TYPE buf_xyZ[CX][CY][CZ]; /* Negative Z */
    MM_TYPE buf_xzy[CX][CY][CZ];
    MM_TYPE buf_yxz[CX][CY][CZ];
    MM_TYPE buf_yzx[CX][CY][CZ];
    MM_TYPE buf_zyx[CX][CY][CZ];
    MM_TYPE buf_zxy[CX][CY][CZ];
    void *inf_xyz;
    void *inf_xYz;
    void *inf_xyZ;
    void *inf_xzy;
    void *inf_yxz;
    void *inf_yzx;
    void *inf_zxy;
    void *inf_zyx;
    int errors[6];

    printf("junk-xyz.mnc\n");
    minc_load_data("junk-xyz.mnc", buf_xyz, NC_TYPE, &ct, &cx, &cy, &cz,
                   &dt, &dx, &dy, &dz, &inf_xyz);

    printf("junk-xYz.mnc\n");
    minc_load_data("junk-xYz.mnc", buf_xYz, NC_TYPE, &ct, &cx, &cy, &cz,
                   &dt, &dx, &dy, &dz, &inf_xYz);

    printf("junk-xyZ.mnc\n");
    minc_load_data("junk-xyZ.mnc", buf_xyZ, NC_TYPE, &ct, &cx, &cy, &cz,
                   &dt, &dx, &dy, &dz, &inf_xyZ);

    printf("junk-xzy.mnc\n");
    minc_load_data("junk-xzy.mnc", buf_xzy, NC_TYPE, &ct, &cx, &cy, &cz,
                   &dt, &dx, &dy, &dz, &inf_xzy);

    printf("junk-yxz.mnc\n");
    minc_load_data("junk-yxz.mnc", buf_yxz, NC_TYPE, &ct, &cx, &cy, &cz,
                   &dt, &dx, &dy, &dz, &inf_yxz);

    printf("junk-yzx.mnc\n");
    minc_load_data("junk-yzx.mnc", buf_yzx, NC_TYPE, &ct, &cx, &cy, &cz,
                   &dt, &dx, &dy, &dz, &inf_yzx);

    printf("junk-zxy.mnc\n");
    minc_load_data("junk-zxy.mnc", buf_zxy, NC_TYPE, &ct, &cx, &cy, &cz,
                   &dt, &dx, &dy, &dz, &inf_zxy);

    printf("junk-zyx.mnc\n");
    minc_load_data("junk-zyx.mnc", buf_zyx, NC_TYPE, &ct, &cx, &cy, &cz,
                   &dt, &dx, &dy, &dz, &inf_zyx);

    for (i = 0; i < 6; i++) {
        errors[i] = 0;
    }

#ifdef EPSILON
#define CLOSE(a, b) (fabs(a - b) < EPSILON)
#else
#define CLOSE(a, b) (a == b)
#endif

    for (i = 0; i < CX; i++) {
        for (j = 0; j < CY; j++) {
            for (k = 0; k < CZ; k++) {
                double e;
                if (!CLOSE(buf_xyz[i][j][k], buf_xzy[i][j][k])) {
                    printf(MM_FMT, buf_xyz[i][j][k]);
                    printf(" ");
                    printf(MM_FMT, buf_xzy[i][j][k]);
                    printf("\n");
                    errors[1]++;
                }
                if (!CLOSE(buf_xyz[i][j][k], buf_xYz[i][j][k])) {
                    errors[0]++;
                }
                if (!CLOSE(buf_xyz[i][j][k], buf_xyZ[i][j][k])) {
                    errors[0]++;
                }
                if (!CLOSE(buf_xyz[i][j][k], buf_yxz[i][j][k])) {
                    errors[2]++;
                }
                if (!CLOSE(buf_xyz[i][j][k], buf_yzx[i][j][k])) {
                    errors[3]++;
                }
                if (!CLOSE(buf_xyz[i][j][k], buf_zxy[i][j][k])) {
                    errors[4]++;
                }
                if (!CLOSE(buf_xyz[i][j][k], buf_zyx[i][j][k])) {
                    errors[5]++;
                }
            }
        }
    }

    for (i = 0; i < 6; i++) {
        printf("%d - ", errors[i]);
        printf("\n");
    }

    h = minc_save_start("temp-xyz.mnc", MINC_TYPE_SHORT, ct, cx, cy, cz, 
                        dt, dx, dy, dz, inf_xyz, "testing");
    minc_save_data(h, buf_xyz, NC_TYPE, 0, 0, 0, 0, ct, cx, cy, cz);
    minc_save_done(h);

    h = minc_save_start("temp-xzy.mnc", MINC_TYPE_SHORT, ct, cx, cy, cz, 
                        dt, dx, dy, dz, inf_xzy, "testing");
    minc_save_data(h, buf_xzy, NC_TYPE, 0, 0, 0, 0, ct, cx, cy, cz);
    minc_save_done(h);

    h = minc_save_start("temp-yxz.mnc", MINC_TYPE_SHORT, ct, cx, cy, cz, 
                        dt, dx, dy, dz, inf_yxz, "testing");
    minc_save_data(h, buf_yxz, NC_TYPE, 0, 0, 0, 0, ct, cx, cy, cz);
    minc_save_done(h);

    h = minc_save_start("temp-yzx.mnc", MINC_TYPE_SHORT, ct, cx, cy, cz, 
                        dt, dx, dy, dz, inf_yzx, "testing");
    minc_save_data(h, buf_yzx, NC_TYPE, 0, 0, 0, 0, ct, cx, cy, cz);
    minc_save_done(h);

    h = minc_save_start("temp-zxy.mnc", MINC_TYPE_SHORT, ct, cx, cy, cz, 
                        dt, dx, dy, dz, inf_zxy, "testing");
    minc_save_data(h, buf_zxy, NC_TYPE, 0, 0, 0, 0, ct, cx, cy, cz);
    minc_save_done(h);

    h = minc_save_start("temp-zyx.mnc", MINC_TYPE_SHORT, ct, cx, cy, cz, 
                        dt, dx, dy, dz, inf_zyx, "testing");
    minc_save_data(h, buf_zyx, NC_TYPE, 0, 0, 0, 0, ct, cx, cy, cz);
    minc_save_done(h);
}

#endif /* MINC_SIMPLE_TEST */
