/* ----------------------------- MNI Header -----------------------------------
@NAME       : image_conversion.c
@DESCRIPTION: File of functions to manipulate image conversion variables
              (icv). These variables allow conversion of netcdf variables
              (the MINC image variable, in particular) to a form more
              convenient for a program.
@METHOD     : Routines included in this file :
              public :
                 miicv_create
                 miicv_free
                 miicv_setdbl
                 miicv_setint
                 miicv_set
                 miicv_inq
                 miicv_attach
                 miicv_detach
                 miicv_get
                 miicv_put
              private :
                 MI_icv_chkid
                 MI_icv_get_type
                 MI_icv_get_vrange
                 MI_icv_get_norm
                 MI_get_default_range
@CREATED    : July 27, 1992. (Peter Neelin, Montreal Neurological Institute)
@MODIFIED   : 
---------------------------------------------------------------------------- */

#include <type_limits.h>
#include <minc_private.h>

/* Array of pointers to image conversion structures */
static mi_icv_type *minc_icv_list[MI_MAX_NUM_ICV] = {
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};




/* ----------------------------- MNI Header -----------------------------------
@NAME       : miicv_create
@INPUT      : (none)
@OUTPUT     : (none)
@RETURNS    : icv id or MI_ERROR when an error occurs
@DESCRIPTION: Creates an image conversion variable (icv) and returns
              a handle to it.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 7, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int miicv_create()
{
   int new_icv;       /* Id of newly created icv */
   mi_icv_type *icvp;  /* Pointer to new icv structure */
   int i;

   MI_SAVE_ROUTINE_NAME("miicv_create");

   /* Look for free slot */
   for (new_icv=0; new_icv<MI_MAX_NUM_ICV; new_icv++)
      if (minc_icv_list[new_icv]==NULL) break;
   if (new_icv>=MI_MAX_NUM_ICV) {
      MI_LOG_PKG_ERROR2(MI_ERR_NOICV,"No more icv's available");
      MI_RETURN_ERROR(MI_ERROR);
   }

   /* Allocate a new structure */
   if ((minc_icv_list[new_icv]=MALLOC(1, mi_icv_type))==NULL) {
      MI_LOG_SYS_ERROR1("miicv_create");
      MI_RETURN_ERROR(MI_ERROR);
   }
   icvp=minc_icv_list[new_icv];

   /* Fill in defaults */

   /* Stuff for calling MI_varaccess */
   icvp->do_scale = FALSE;
   icvp->scale = 1.0;
   icvp->offset = 0.0;
   icvp->do_dimconvert = FALSE;
   icvp->dimconvert_func = MI_icv_dimconvert;

   /* User defaults */
   icvp->user_type = NC_SHORT;
   icvp->user_sign = MI_PRIV_SIGNED;
   icvp->user_do_range = TRUE;
   icvp->user_vmax = MI_get_default_range(MIvalid_max, icvp->user_type,
                                            icvp->user_sign);
   icvp->user_vmin = MI_get_default_range(MIvalid_min, icvp->user_type,
                                            icvp->user_sign);
   icvp->user_do_norm = FALSE;
   icvp->user_user_norm = FALSE;
   icvp->user_imgmax = 1.0;
   icvp->user_imgmin = 0.0;
   icvp->user_do_scalar = TRUE;
   icvp->user_xdim_dir = MI_ICV_ANYDIR;
   icvp->user_ydim_dir = MI_ICV_ANYDIR;
   icvp->user_zdim_dir = MI_ICV_ANYDIR;
   icvp->user_keep_aspect = TRUE;

   /* Variable values */
   icvp->cdfid = MI_ERROR;            /* Set so that we can recognise an */
   icvp->varid = MI_ERROR;            /* unattached icv */
   icvp->imgmaxid = MI_ERROR;
   icvp->imgminid = MI_ERROR;
   icvp->var_ndims = 0;
   for (i=0; i<MAX_VAR_DIMS; i++)
      icvp->var_dim[i]=MI_ERROR;
   icvp->var_type = NC_SHORT;
   icvp->var_sign = MI_PRIV_DEFSIGN;
   icvp->var_vmax = SHRT_MAX;
   icvp->var_vmin = SHRT_MIN;
   icvp->var_is_vector=FALSE;

   /* Derived values */
   icvp->derv_imgmax = 1.0;
   icvp->derv_imgmin = 0.0;
   icvp->derv_firstdim = -1;
   for (i=0; i<MAX_VAR_DIMS; i++)
      icvp->derv_bufsize_step[i] = 1;

   /* Initialize dimensions stuff */
   for (i=0; i< MI_PRIV_IMGDIMS; i++) {
      icvp->user_dim_size[i] = MI_ICV_ANYSIZE;
      icvp->var_dim_size[i] = MI_ICV_ANYSIZE;
      icvp->derv_dim_flip[i] = FALSE;
      icvp->derv_dim_grow[i] = TRUE;
      icvp->derv_dim_scale[i] = 1;
      icvp->derv_dim_off[i] = 0;
      icvp->derv_dim_step[i] = 0.0;
      icvp->derv_dim_start[i] = 0.0;
   }

   MI_RETURN(new_icv);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : miicv_free
@INPUT      : icvid
@OUTPUT     : (none)
@RETURNS    : MI_ERROR if an error occurs
@DESCRIPTION: Frees the image conversion variable (icv)
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 7, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int miicv_free(int icvid)
{
   MI_SAVE_ROUTINE_NAME("miicv_free");

   /* Check icv id */
   {MI_CHK_ERR(MI_icv_chkid(icvid))}

   /* Detach the icv if it is attached */
   if (minc_icv_list[icvid]->cdfid != MI_ERROR)
      {MI_CHK_ERR(miicv_detach(icvid))}

   /* Free the structure */
   FREE(minc_icv_list[icvid]);
   minc_icv_list[icvid]=NULL;

   MI_RETURN(MI_NOERROR);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : miicv_setdbl
@INPUT      : icvid        - icv id
              icv_property - property of icv to set
              value        - value to set it to
@OUTPUT     : (none)
@RETURNS    : MI_ERROR if an error occurs
@DESCRIPTION: Sets a property of an icv to a given double value
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 7, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int miicv_setdbl(int icvid, int icv_property, double value)
{
   MI_SAVE_ROUTINE_NAME("miicv_setdbl");

   MI_CHK_ERR(miicv_set(icvid, icv_property, &value));

   MI_RETURN(MI_NOERROR);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : miicv_setint
@INPUT      : icvid        - icv id
              icv_property - property of icv to set
              value        - value to set it to
@OUTPUT     : (none)
@RETURNS    : MI_ERROR if an error occurs
@DESCRIPTION: Sets a property of an icv to a given integer value. Note
              that MI_ICV_ADIM_SIZE and MI_ICV_BDIM_SIZE are converted
              to long.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 7, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int miicv_setint(int icvid, int icv_property, int value)
{
   long lvalue;

   MI_SAVE_ROUTINE_NAME("miicv_setint");

   if ((icv_property==MI_ICV_ADIM_SIZE) || (icv_property==MI_ICV_BDIM_SIZE)) {
      lvalue = (long) value;
      MI_CHK_ERR(miicv_set(icvid, icv_property, &lvalue))
   }
   else
      MI_CHK_ERR(miicv_set(icvid, icv_property, &value))

   MI_RETURN(MI_NOERROR);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : miicv_set
@INPUT      : icvid        - icv id
              icv_property - property of icv to set
              value        - value to set it to (note that this is a pointer
                 so that different types can be passed to the routine, no
                 value is returned).
@OUTPUT     : (none)
@RETURNS    : MI_ERROR if an error occurs
@DESCRIPTION: Sets a property of an icv to a given value. Properties cannot
              be modified while the icv is attached to a cdf file and 
              variable (see miicv_attach and miicv_detach).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 7, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int miicv_set(int icvid, int icv_property, void *value)
{
   int *ip, ival;
   mi_icv_type *icvp;

   MI_SAVE_ROUTINE_NAME("miicv_set");

   /* Check icv id */
   MI_CHK_ERR(MI_icv_chkid(icvid))
   icvp=minc_icv_list[icvid];

   /* Check that the icv is not attached to a file */
   if (icvp->cdfid != MI_ERROR) {
      MI_LOG_PKG_ERROR2(MI_ERR_ICVATTACHED, 
         "Attempt to modify an attached image conversion variable");
      MI_RETURN_ERROR(MI_ERROR);
   }

   /* Set the property */
   switch (icv_property) {
   case MI_ICV_TYPE:
      icvp->user_type   = *((nc_type *) value);
      icvp->user_vmax   = MI_get_default_range(MIvalid_max, icvp->user_type,
                                               icvp->user_sign);
      icvp->user_vmin   = MI_get_default_range(MIvalid_min, icvp->user_type,
                                               icvp->user_sign);
      break;
   case MI_ICV_SIGN:
      icvp->user_sign   = MI_get_sign_from_string(icvp->user_type, value);
      icvp->user_vmax   = MI_get_default_range(MIvalid_max, icvp->user_type,
                                               icvp->user_sign);
      icvp->user_vmin   = MI_get_default_range(MIvalid_min, icvp->user_type,
                                               icvp->user_sign);
      break;
   case MI_ICV_DO_RANGE:
      icvp->user_do_range = *((int *) value); break;
   case MI_ICV_VALID_MAX:
      icvp->user_vmax   = *((double *) value); break;
   case MI_ICV_VALID_MIN:
      icvp->user_vmin   = *((double *) value); break;
   case MI_ICV_DO_NORM:
      icvp->user_do_norm = *((int *) value); break;
   case MI_ICV_USER_NORM:
      icvp->user_user_norm = *((int *) value); break;
   case MI_ICV_IMAGE_MAX:
      icvp->user_imgmax = *((double *) value); break;
   case MI_ICV_IMAGE_MIN:
      icvp->user_imgmin = *((double *) value); break;
   case MI_ICV_DO_SCALAR:
      icvp->user_do_scalar = *((int *) value); break;
   case MI_ICV_XDIM_DIR: ip = &(icvp->user_xdim_dir);
   case MI_ICV_YDIM_DIR: ip = &(icvp->user_ydim_dir);
   case MI_ICV_ZDIM_DIR: ip = &(icvp->user_zdim_dir);
      ival = *((int *) value);
      *ip = ((ival==MI_ICV_POSITIVE) || (ival==MI_ICV_NEGATIVE)) ?
                   ival : MI_ICV_ANYSIZE;
      break;
   case MI_ICV_ADIM_SIZE:
      icvp->user_dim_size[0] = *((long *) value); break;
   case MI_ICV_BDIM_SIZE:
      icvp->user_dim_size[1] = *((long *) value); break;
   case MI_ICV_KEEP_ASPECT:
      icvp->user_keep_aspect = *((int *) value); break;
   default:
      MI_LOG_PKG_ERROR2(MI_ERR_BADPROP,
                        "Tried to set unknown or illegal icv property");
      MI_RETURN_ERROR(MI_ERROR);
   }

   MI_RETURN(MI_NOERROR);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : miicv_inq
@INPUT      : icvid        - icv id
              icv_property - icv property to get
@OUTPUT     : value        - value returned
@RETURNS    : MI_ERROR if an error occurs
@DESCRIPTION: Gets the value of an icv property
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int miicv_inq(int icvid, int icv_property, void *value)
{
   int *ip;
   mi_icv_type *icvp;

   MI_SAVE_ROUTINE_NAME("miicv_inq");

   /* Check icv id */
   MI_CHK_ERR(MI_icv_chkid(icvid))
   icvp=minc_icv_list[icvid];

   /* Set the property */
   switch (icv_property) {
   case MI_ICV_TYPE:
      *((nc_type *) value) = icvp->user_type; break;
   case MI_ICV_SIGN:
      *((char **) value) = (icvp->user_sign==MI_PRIV_SIGNED)   ? MI_SIGNED :
                           (icvp->user_sign==MI_PRIV_UNSIGNED) ? MI_UNSIGNED :
                                                             MI_EMPTY_STRING;
      break;
   case MI_ICV_DO_RANGE:
      *((int *) value) = icvp->user_do_range; break;
   case MI_ICV_VALID_MAX:
      *((double *) value) = icvp->user_vmax; break;
   case MI_ICV_VALID_MIN:
      *((double *) value) = icvp->user_vmin; break;
   case MI_ICV_DO_NORM:
      *((int *) value) = icvp->user_do_norm; break;
   case MI_ICV_USER_NORM:
      *((int *) value) = icvp->user_user_norm; break;
   case MI_ICV_IMAGE_MAX:
      *((double *) value) = icvp->user_imgmax; break;
   case MI_ICV_IMAGE_MIN:
      *((double *) value) = icvp->user_imgmin; break;
   case MI_ICV_DO_SCALAR:
      *((int *) value) = icvp->user_do_scalar; break;
   case MI_ICV_XDIM_DIR: ip = &(icvp->user_xdim_dir);
   case MI_ICV_YDIM_DIR: ip = &(icvp->user_ydim_dir);
   case MI_ICV_ZDIM_DIR: ip = &(icvp->user_zdim_dir);
      *((int *) value) = *ip;
      break;
   case MI_ICV_ADIM_SIZE:
      *((long *) value) = icvp->user_dim_size[0]; break;
   case MI_ICV_BDIM_SIZE:
      *((long *) value) = icvp->user_dim_size[1]; break;
   case MI_ICV_ADIM_STEP:
      *((double *) value) = icvp->derv_dim_step[0]; break;
   case MI_ICV_BDIM_STEP:
      *((double *) value) = icvp->derv_dim_step[1]; break;
   case MI_ICV_ADIM_START:
      *((double *) value) = icvp->derv_dim_start[0]; break;
   case MI_ICV_BDIM_START:
      *((double *) value) = icvp->derv_dim_start[1]; break;
   case MI_ICV_KEEP_ASPECT:
      *((int *) value) = icvp->user_keep_aspect; break;
   default:
      MI_LOG_PKG_ERROR2(MI_ERR_BADPROP,
                        "Tried to inquire about unknown icv property");
      MI_RETURN_ERROR(MI_ERROR);
   }

   MI_RETURN(MI_NOERROR);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : miicv_attach
@INPUT      : icvid - icv id
              cdfid - cdf file id
              varid - cdf variable id
@OUTPUT     : (none)
@RETURNS    : MI_ERROR if an error occurs
@DESCRIPTION: Attaches an open cdf file and variable to an image conversion
              variable for subsequent access through miicvget and miicvput.
              File must be in data mode.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines
@CREATED    : 
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int miicv_attach(int icvid, int cdfid, int varid)
{
   mi_icv_type *icvp;         /* Pointer to icv structure */
   long size_diff;
   int idim;

   MI_SAVE_ROUTINE_NAME("miicv_attach");

   /* Check icv id */
   MI_CHK_ERR(MI_icv_chkid(icvid))
   icvp=minc_icv_list[icvid];

   /* If the icv is attached, then detach it */
   if (icvp->cdfid != MI_ERROR)
      MI_CHK_ERR(miicv_detach(icvid))

   /* Inquire about the variable's type, sign and number of dimensions */
   MI_CHK_ERR(MI_icv_get_type(icvp, cdfid, varid))

   /* Get valid range */
   if (icvp->user_do_range) {
      MI_CHK_ERR(MI_icv_get_vrange(icvp, cdfid, varid))

      /* Get normalization info */
      MI_CHK_ERR(MI_icv_get_norm(icvp, cdfid, varid))
   }

   /* Get dimensions info */
   MI_CHK_ERR(MI_icv_get_dim(icvp, cdfid, varid))

   /* Set the do_scale and do_dimconvert fields of icv structure
      We have to scale only if do_range is TRUE. If ranges don't
      match, or we have to do user normalization, or if we are normalizing
      and MIimagemax or MIimagemin vary over the variable.
      We do dimension conversion if any dimension needs flipping, scaling
      or offset, or if we have to convert from vector to scalar. */

   icvp->do_scale = 
      (icvp->user_do_range && 
       ((icvp->user_vmax!=icvp->var_vmax) ||
        (icvp->user_vmin!=icvp->var_vmin) ||
        (icvp->user_do_norm && icvp->user_user_norm) ||
        (icvp->user_do_norm && (icvp->derv_firstdim>=0))) );

   icvp->do_dimconvert = (icvp->user_do_scalar && icvp->var_is_vector);
   for (idim=0; idim<MI_PRIV_IMGDIMS; idim++) {
      if (icvp->derv_dim_flip[idim] || (icvp->derv_dim_scale[idim] != 1) ||
          (icvp->derv_dim_off[idim] != 0))
         icvp->do_dimconvert = TRUE;
   }

   /* Check if we have to zero user's buffer on GETs */
   icvp->derv_do_zero = FALSE;
   for (idim=0; idim<MI_PRIV_IMGDIMS; idim++) {
      if (icvp->derv_dim_grow[idim])
         size_diff = icvp->user_dim_size[idim] -
                     icvp->var_dim_size[idim] * icvp->derv_dim_scale[idim];
      else
         size_diff = icvp->user_dim_size[idim] * icvp->derv_dim_scale[idim] -
                     icvp->var_dim_size[idim];
      if ((icvp->derv_dim_off[idim]!=0) || (size_diff!=0))
         icvp->derv_do_zero = TRUE;
   }

   /* Set the cdfid and varid fields */
   icvp->cdfid = cdfid;
   icvp->varid = varid;

   MI_RETURN(MI_NOERROR);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : MI_icv_get_type
@INPUT      : icvp  - pointer to icv structure
              cdfid - cdf file id
              varid - variable id
@OUTPUT     : (none)
@RETURNS    : MI_ERROR if an error occurs
@DESCRIPTION: Gets the type and sign of a variable for miicv_attach.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines
@CREATED    : August 10, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int MI_icv_get_type(mi_icv_type *icvp, int cdfid, int varid)
{
   int oldncopts;            /* For saving value of ncopts */
   char stringa[MI_MAX_ATTSTR_LEN];
   char *string=stringa;     /* String for sign info */

   MI_SAVE_ROUTINE_NAME("MI_icv_get_type");

   /* Inquire about the variable */
   MI_CHK_ERR(ncvarinq(cdfid, varid, NULL, &(icvp->var_type), 
                       &(icvp->var_ndims), icvp->var_dim, NULL))

   /* Check that the variable type is numeric */
   if (icvp->var_type==NC_CHAR) {
      MI_LOG_PKG_ERROR2(MI_ERR_NONNUMERIC,"Non-numeric datatype");
      MI_RETURN_ERROR(MI_ERROR);
   }

   /* Try to find out the sign of the variable using MIsigntype. */
   oldncopts = ncopts; ncopts = 0;
   string=miattgetstr(cdfid, varid, MIsigntype, MI_MAX_ATTSTR_LEN, string);
   ncopts = oldncopts;
   icvp->var_sign  = MI_get_sign_from_string(icvp->var_type, string);

   /* Get type lengths */
   icvp->var_typelen = nctypelen(icvp->var_type);
   icvp->user_typelen = nctypelen(icvp->user_type);

   MI_RETURN(MI_NOERROR);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : MI_icv_get_vrange
@INPUT      : icvp  - pointer to icv structure
              cdfid - cdf file id
              varid - variable id
@OUTPUT     : (none)
@RETURNS    : MI_ERROR if an error occurs
@DESCRIPTION: Gets the valid range of a variable
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines
@CREATED    : August 10, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int MI_icv_get_vrange(mi_icv_type *icvp, int cdfid, int varid)
{
   int oldncopts;            /* For saving value of ncopts */
   int status;
   double vrange[2];         /* Valid range buffer */
   int length;               /* Number of elements in attribute */

   MI_SAVE_ROUTINE_NAME("MI_icv_get_vrange");

   /* Look for valid range info */
   oldncopts = ncopts; ncopts = 0;
   status=miattget(cdfid, varid, MIvalid_range, NC_DOUBLE, 2, vrange, &length);
   if ((status!=MI_ERROR) && (length==2)) {
      icvp->var_vmax=MAX(vrange[0],vrange[1]);
      icvp->var_vmin=MIN(vrange[0],vrange[1]);
   }
   else {
      status=miattget1(cdfid, varid, MIvalid_max, NC_DOUBLE, 
                       &(icvp->var_vmax));
      if (status==MI_ERROR) 
         icvp->var_vmax=MI_get_default_range(MIvalid_max, 
                                             icvp->var_type, icvp->var_sign);
      status=miattget1(cdfid, varid, MIvalid_min, NC_DOUBLE, 
                       &(icvp->var_vmin));
      if (status==MI_ERROR) 
         icvp->var_vmin=MI_get_default_range(MIvalid_min, 
                                             icvp->var_type, icvp->var_sign);
   }
   ncopts = oldncopts;

   MI_RETURN(MI_NOERROR);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : MI_get_default_range
@INPUT      : what     - MIvalid_min means get default min, MIvalid_min means 
                 get default min
              datatype - type of variable
              sign     - sign of variable
@OUTPUT     : (none)
@RETURNS    : default maximum or minimum for the datatype
@DESCRIPTION: Return the defaults maximum or minimum for a given datatype
              and sign.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines
@CREATED    : August 10, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private double MI_get_default_range(char *what, nc_type datatype, int sign)
{
   double limit;

   MI_SAVE_ROUTINE_NAME("MI_get_default_range");

   if (STRINGS_EQUAL(what, MIvalid_max)) {
      switch (datatype) {
      case NC_LONG:
         limit = (sign == MI_PRIV_SIGNED) ? LONG_MAX : ULONG_MAX; break;
      case NC_SHORT:
         limit = (sign == MI_PRIV_SIGNED) ? SHRT_MAX : USHRT_MAX; break;
      case NC_BYTE:
         limit = (sign == MI_PRIV_SIGNED) ? SCHAR_MAX : UCHAR_MAX; break;
      default:
         limit = 1.0; break;
      }
   }
   else if (STRINGS_EQUAL(what, MIvalid_min)) {
      switch (datatype) {
      case NC_LONG:
         limit = (sign == MI_PRIV_SIGNED) ? LONG_MIN : 0; break;
      case NC_SHORT:
         limit = (sign == MI_PRIV_SIGNED) ? SHRT_MIN : 0; break;
      case NC_BYTE:
         limit = (sign == MI_PRIV_SIGNED) ? SCHAR_MIN : 0; break;
      default:
         limit = 0.0; break;
      }
   }
   else {
      ncopts = NC_VERBOSE | NC_FATAL;
      MI_LOG_PKG_ERROR2(-1,"MINC bug - this line should never be printed");
      limit = 0.0;
   }

   MI_RETURN(limit);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : MI_icv_get_norm
@INPUT      : icvp  - pointer to icv structure
              cdfid - cdf file id
              varid - variable id
@OUTPUT     : (none)
@RETURNS    : MI_ERROR if an error occurs
@DESCRIPTION: Gets the normalization info for a variable
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines
@CREATED    : August 10, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int MI_icv_get_norm(mi_icv_type *icvp, int cdfid, int varid)
{             /* ARGSUSED */
   int oldncopts;             /* For saving value of ncopts */
   int vid[2];                /* Variable ids for max and min */
   double *immptr[2];         /* Pointers to imgmax and imgmin */
   int ndims;                 /* Number of dimensions for image max and min */
   int dim[MAX_VAR_DIMS];     /* Dimensions */
   long start[MAX_VAR_DIMS];  /* Start of variable */
   long count[MAX_VAR_DIMS];  /* Dimensions sizes */
   long size;                 /* Size of max and min variables */
   int imm;                   /* Counter for looping through max and min */
   double *buffer;            /* Pointer to buffer for max/min values */
   int idim, i;
   long ientry;

   MI_SAVE_ROUTINE_NAME("MI_icv_get_norm");

   /* Initialize first dimension over which MIimagemax or MIimagemin
      vary - assume that they don't vary at all */
   icvp->derv_firstdim=(-1);

   /* Look for image max, image min variables */
   if (icvp->user_do_norm) {
      oldncopts=ncopts; ncopts=0;
      icvp->imgmaxid=ncvarid(cdfid, MIimagemax);
      icvp->imgminid=ncvarid(cdfid, MIimagemin);
      ncopts = oldncopts;
   }

   /* Check to see if normalization to variable max, min should be done */
   if (!icvp->user_do_norm) {
      icvp->derv_imgmax = 1.0;
      icvp->derv_imgmin = 0.0;
   }
   else if (icvp->user_user_norm) {
      icvp->derv_imgmax = icvp->user_imgmax;
      icvp->derv_imgmin = icvp->user_imgmin;
   }
   else {
      /* Get image max, min variable ids */
      vid[0]=icvp->imgmaxid;
      vid[1]=icvp->imgminid;

      /* No max/min variables, so use valid_range values for float/double
         and 0, 1 for integer types */
      if ((vid[0] == MI_ERROR) || (vid[1] == MI_ERROR)) {
         switch (icvp->var_type) {
         case NC_DOUBLE:
         case NC_FLOAT:
            icvp->derv_imgmax = icvp->var_vmax;
            icvp->derv_imgmin = icvp->var_vmin;
            break;
         default:
            icvp->derv_imgmax = 1.0;
            icvp->derv_imgmin = 0.0;
            break;
         }
      }

      /* If the variables are there then get the max and min */
      else {
         immptr[0] = &(icvp->derv_imgmax);
         immptr[1] = &(icvp->derv_imgmin);
         for (imm=0; imm<2; imm++) {    /* Loop through max, then min */
            MI_CHK_ERR(ncvarinq(cdfid, vid[imm], NULL, NULL, 
                                &ndims, dim, NULL))

            /* Loop through dimensions, checking dimensions against image,
               getting dimension sizes and total max/min variable size */
            size=1;     /* Size of MIimagemax/min variable */
            icvp->derv_firstdim = -1;  /* Doesn't vary with image */
            for (idim=0; idim<ndims; idim++) {
               /* Look to see where this dimension falls as an image 
                  dimension */
               for (i=0; i<icvp->var_ndims; i++) {
                  if (icvp->var_dim[i]==dim[idim])
                     icvp->derv_firstdim = MAX(icvp->derv_firstdim, i);
               }
               /* Get the dimension size */
               MI_CHK_ERR(ncdiminq(cdfid, dim[idim], NULL, &(count[i])))
               size *= count[i];
            }
            /* Get space */
            if ((buffer=MALLOC(size, double))==NULL) {
               MI_LOG_SYS_ERROR1("MI_icv_get_norm");
               MI_RETURN_ERROR(MI_ERROR);
            }
            /* Get values */
            if (mivarget(cdfid, vid[imm], 
                         miset_coords(ndims, 0L, start),
                         count, NC_DOUBLE, NULL, buffer)==MI_ERROR) {
               FREE(buffer);
               MI_RETURN_ERROR(MI_ERROR);
            }
            /* Loop through values, getting max/min */
            *immptr[imm] = buffer[0];
            for (ientry=1; ientry<size; ientry++) {
               if (imm==0)
                  *immptr[imm] = MAX(*immptr[imm], buffer[ientry]);
               else
                  *immptr[imm] = MIN(*immptr[imm], buffer[ientry]);
            }
            FREE(buffer);
         }
      }
   }

   MI_RETURN(MI_NOERROR);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : MI_icv_get_dim
@INPUT      : icvp  - pointer to icv structure
              cdfid - cdf file id
              varid - variable id
@OUTPUT     : (none)
@RETURNS    : MI_ERROR if an error occurs
@DESCRIPTION: Gets dimension info for the icv
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines
@CREATED    : August 10, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int MI_icv_get_dim(mi_icv_type *icvp, int cdfid, int varid)
{        /* ARGSUSED */
   int oldncopts;             /* For saving value of ncopts */
   char dimname[MAX_NC_NAME]; /* Dimensions name */
   int idim;                  /* Looping counter for fastest image dims */
   int subsc[MI_PRIV_IMGDIMS]; /* Subscripts for fastest image dims */
   int dimvid[MI_PRIV_IMGDIMS]; /* Variable ids for dimensions */

   MI_SAVE_ROUTINE_NAME("MI_icv_get_dim");

   /* Check that the variable has at least MI_PRIV_IMGDIMS dimensions */
   if (icvp->var_ndims < MI_PRIV_IMGDIMS) {
      MI_LOG_PKG_ERROR2(MI_ERR_TOOFEWDIMS, 
                       "Variable has too few dimensions to be an image");
      MI_RETURN_ERROR(MI_ERROR);
   }

   /* Check the first dimensions of the variable */
   MI_CHK_ERR(ncdiminq(cdfid, icvp->var_dim[icvp->var_ndims-1], dimname, 
                       &(icvp->var_vector_size)))
   icvp->var_is_vector = STRINGS_EQUAL(dimname, MIvector_dimension);

   /* Check that the variable has at least MI_PRIV_IMGDIMS+1 dimensions if 
      it is a vector field */
   if (icvp->var_is_vector && (icvp->var_ndims < MI_PRIV_IMGDIMS+1)) {
      MI_LOG_PKG_ERROR2(MI_ERR_TOOFEWDIMS, 
                        "Variable has too few dimensions to be an image");
      MI_RETURN_ERROR(MI_ERROR);
   }

   /* Check for dimension flipping and get dimension sizes */

   /* Get subscripts for first MI_PRIV_IMGDIMS dimensions */
   subsc[0] = (icvp->var_is_vector) ? icvp->var_ndims-2 : icvp->var_ndims-1;
   for (idim=1; idim < MI_PRIV_IMGDIMS; idim++)
      subsc[idim]=subsc[idim-1]-1;

   /* Get dimension variable ids */
   for (idim=0; idim < MI_PRIV_IMGDIMS; idim++) {
      oldncopts = ncopts; ncopts = 0;
      dimvid[idim] = ncvarid(cdfid, dimname);
      ncopts = oldncopts;
   }

   /* Check for flipping */
   {MI_CHK_ERR(MI_get_dim_flip(icvp, cdfid, dimvid, subsc))}

   /* Check for scaling of dimension */
   {MI_CHK_ERR(MI_get_dim_scale(icvp, cdfid, dimvid))}

   /* Check for variable buffer size increments */
   {MI_CHK_ERR(MI_get_dim_bufsize_step(icvp, subsc))}

   /* Get information for dimension conversion */
   {MI_CHK_ERR(MI_icv_get_dim_conversion(icvp, subsc))}

   MI_RETURN(MI_NOERROR);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : MI_icv_get_dim_flip
@INPUT      : icvp  - icv pointer
              cdfid - cdf file id
              dimvid - variable id
              subsc - array of dimension subscripts for fastest varying 
                 dimensions
@OUTPUT     : (none)
@RETURNS    : MI_ERROR if an error occurs
@DESCRIPTION: Checks for flipping of icv.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines
@CREATED    : September 1, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int MI_get_dim_flip(mi_icv_type *icvp, int cdfid, int dimvid[], 
                           int subsc[])
{
   int oldncopts;             /* For saving value of ncopts */
   char dimname[MAX_NC_NAME]; /* Dimensions name */
   int dim_dir;               /* Desired direction for current dimension */
   double dimstep;            /* Dimension step size (and direction) */
   int idim;
   int status;

   MI_SAVE_ROUTINE_NAME("MI_get_dim_flip");

   /* Loop through fast dimensions */

   for (idim=0; idim < MI_PRIV_IMGDIMS; idim++) {

      /* Get name of the dimension */
      {MI_CHK_ERR(ncdiminq(cdfid, icvp->var_dim[subsc[idim]], dimname, 
                           &(icvp->var_dim_size[idim])))}

      /* Should we look for dimension flipping? */
      icvp->derv_dim_flip[idim]=FALSE;
      if (STRINGS_EQUAL(dimname, MIxspace) || 
          STRINGS_EQUAL(dimname, MIxfrequency))
         dim_dir = icvp->user_xdim_dir;
      else if (STRINGS_EQUAL(dimname, MIyspace) || 
               STRINGS_EQUAL(dimname, MIyfrequency))
         dim_dir = icvp->user_ydim_dir;
      else if (STRINGS_EQUAL(dimname, MIzspace) || 
               STRINGS_EQUAL(dimname, MIzfrequency))
         dim_dir = icvp->user_zdim_dir;
      else
         dim_dir = MI_ICV_ANYDIR;

      /* Look for variable corresponding to dimension */
      if (dim_dir != MI_ICV_ANYDIR) {   /* Look for flipping? */

         /* Get the value of the MIstep attribute to determine whether flipping
            is needed */
         if (dimvid[idim] != MI_ERROR) {   /* if dimension exists */
            oldncopts = ncopts; ncopts = 0;
            status=miattget1(cdfid, dimvid[idim], MIstep, NC_DOUBLE, &dimstep);
            ncopts = oldncopts;
            if (status != MI_ERROR) {   /* Found dimstep */
               if (dim_dir == MI_ICV_POSITIVE)
                  icvp->derv_dim_flip[idim] = (dimstep<0.0);
               else if (dim_dir == MI_ICV_NEGATIVE)
                  icvp->derv_dim_flip[idim] = (dimstep>0.0);
            }                           /* if found dimstep */
         }                           /* if dimension exists */
      }                          /* if look for flipping */

   }                           /* for each dimension */


   MI_RETURN(MI_NOERROR);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : MI_icv_get_dim_scale
@INPUT      : icvp  - icv pointer
              cdfid - cdf file id
              dimvid - dimension variable id
@OUTPUT     : (none)
@RETURNS    : MI_ERROR if an error occurs
@DESCRIPTION: Checks for scaling of images
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines
@CREATED    : September 1, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int MI_get_dim_scale(mi_icv_type *icvp, int cdfid, int dimvid[])
{
   int oldncopts;             /* For saving value of ncopts */
   int min_grow, dim_grow;
   int min_scale, dim_scale;
   double dimstep, dimstart;
   int idim;

   MI_SAVE_ROUTINE_NAME("MI_get_dim_scale");

   /* Loop through dimensions, calculating scale and looking for smallest 
      one. For each dimension, check to see if we need to shrink or grow the 
      image. (This is get-oriented: grow is TRUE if the variable dimension 
      has to be expanded to fit the user's dimensions). */

   for (idim=0; idim < MI_PRIV_IMGDIMS; idim++) {

      /* Check to see if we user wants resize */
      if (icvp->user_dim_size[idim] <= 0) {
         icvp->derv_dim_grow[idim]=TRUE;
         icvp->derv_dim_scale[idim]=1;
      }
      else {

         /* Check for growing or shrinking */
         icvp->derv_dim_grow[idim] =
            (icvp->var_dim_size[idim] <= icvp->user_dim_size[idim]);

         /* If growing */
         if (icvp->derv_dim_grow[idim]) {
            /* Get scale so that whole image fits in user array */
            icvp->derv_dim_scale[idim] = 
               icvp->user_dim_size[idim] / icvp->var_dim_size[idim];
         }

         /* Otherwise, shrinking. Things are complicated by the fact that
            the external variable must fit completely in the user's array */
         else {

            icvp->derv_dim_scale[idim] = 
               (icvp->var_dim_size[idim] + icvp->user_dim_size[idim] - 1) 
                      / icvp->user_dim_size[idim];
         }
      }           /* if user wants resizing */

      /* Check for smallest scale */
      if (idim==0) {
         min_grow = icvp->derv_dim_grow[idim];
         min_scale = icvp->derv_dim_scale[idim];
      }
      else {
         dim_grow  = icvp->derv_dim_grow[idim];
         dim_scale = icvp->derv_dim_scale[idim];
         /* Check for one of three conditions :
               (1) smallest yet is growing, but this dim is shrinking
               (2) both are growing and this dim has smaller scale
               (3) both are shrinking and this dim has larger scale */
         if ((min_grow && !dim_grow) ||
             ((min_grow && dim_grow) &&
              (min_scale > dim_scale)) ||
             ((!min_grow && !dim_grow) &&
              (min_scale < dim_scale))) {
            min_grow = dim_grow;
            min_scale = dim_scale;
         }
      }           /* if not first dim */
      
   }           /* for each dimension, get scale */

   /* Loop through dimensions, resetting scale if needed, setting offset 
      and pixel step and start */

   for (idim=0; idim < MI_PRIV_IMGDIMS; idim++) {

      /* Check for aspect ratio */
      if (icvp->user_keep_aspect) {
         icvp->derv_dim_grow[idim]  = min_grow;
         icvp->derv_dim_scale[idim] = min_scale;
      }

      /* Set offset of variable into user's image */

      /* If growing */
      if (icvp->derv_dim_grow[idim]) {
         /* Calculate remainder and split it in half */
         icvp->derv_dim_off[idim] = 
            ( icvp->user_dim_size[idim] -
             icvp->derv_dim_grow[idim] * icvp->derv_dim_scale[idim] )
                                      / 2;
      }
      /* Otherwise, shrinking. Things are complicated by the fact that
         the external variable must fit completely in the user's array */
      else {
         /* Calculate remainder and split it in half */
         icvp->derv_dim_off[idim] = 
            ( icvp->user_dim_size[idim] -
             (icvp->var_dim_size[idim] + icvp->user_dim_size[idim] - 1) 
                              / icvp->derv_dim_scale[idim] ) / 2 ;
      }

      /* Get pixel step and start for variable and calculate for user.
         Look for them in the dimension variable (if MIstep is not
         there, then leave values at 0.0) */
      oldncopts = ncopts; ncopts = 0;
      if (miattget1(cdfid, dimvid[idim], MIstep, NC_DOUBLE, &dimstep) 
                          != MI_ERROR) {
         /* Get step size for user's image */
         icvp->derv_dim_step[idim] = icvp->derv_dim_grow[idim] ?
            dimstep / icvp->derv_dim_scale[idim] :
            dimstep * icvp->derv_dim_scale[idim];
         /* Get start position for user's image - if not MIstart for
            dimension, then assume 0.0 */
         if (miattget1(cdfid, dimvid[idim], MIstep, NC_DOUBLE, &dimstart)
                             == MI_ERROR)
            dimstart=0.0;
         icvp->derv_dim_start[idim] = dimstart + 
            (icvp->derv_dim_step[idim] - dimstep) / 2.0 -
               icvp->derv_dim_off[idim] * icvp->derv_dim_step[idim];
      }             /* if found MIstep */
      ncopts = oldncopts;

   }                 /* for each dimension */

   MI_RETURN(MI_NOERROR);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : MI_icv_get_dim_bufsize_step
@INPUT      : icvp  - icv pointer
              subsc - array of dimension subscripts for fastest varying 
                 dimensions
@OUTPUT     : (none)
@RETURNS    : MI_ERROR if an error occurs
@DESCRIPTION: Sets the variables giving variable buffer size
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines
@CREATED    : September 3, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int MI_get_dim_bufsize_step(mi_icv_type *icvp, int subsc[])
{
   int idim;

   MI_SAVE_ROUTINE_NAME("MI_get_dim_bufsize_step");

   /* Set default buffer size step */
   for (idim=0; idim < MAX_VAR_DIMS; idim++)
      icvp->derv_bufsize_step[idim]=1;

   /* Check for converting vector to scalar */
   icvp->derv_do_bufsize_step = (icvp->var_is_vector && icvp->user_do_scalar);
   if (icvp->derv_do_bufsize_step)
      icvp->derv_bufsize_step[icvp->var_ndims-1] = icvp->var_vector_size;
      
   /* Check each dimension to see if we need to worry about the variable
      buffer size. This only occurs if we are shrinking the dimension from 
      the variable buffer to the user buffer. */
   for (idim=0; idim < MI_PRIV_IMGDIMS; idim++) {
      if (!icvp->derv_dim_grow[idim])
         icvp->derv_bufsize_step[subsc[idim]]=icvp->derv_dim_scale[idim];
      if (icvp->derv_bufsize_step[subsc[idim]] != 1)
         icvp->derv_do_bufsize_step = TRUE;
   }

   MI_RETURN(MI_NOERROR);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : MI_icv_get_dim_conversion
@INPUT      : icvp  - icv pointer
              subsc - array of dimension subscripts for fastest varying 
                 dimensions
@OUTPUT     : (none)
@RETURNS    : MI_ERROR if an error occurs
@DESCRIPTION: Sets the variables for dimensions converions
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines
@CREATED    : September 8, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int MI_icv_get_dim_conversion(mi_icv_type *icvp, int subsc[])
{            /* ARGSUSED */
   int idim;

   MI_SAVE_ROUTINE_NAME("MI_icv_get_dim_conversion");

   /* Find out whether we need to compress variable or user buffer */
   icvp->derv_var_compress = (icvp->var_is_vector && icvp->user_do_scalar);
   icvp->derv_usr_compress = FALSE;
   for (idim=0; idim<MI_PRIV_IMGDIMS; idim++) {
      if (icvp->derv_dim_scale[idim]!=1) {
         if (icvp->derv_dim_grow[idim])
            icvp->derv_usr_compress = TRUE;
         else
            icvp->derv_var_compress = TRUE;
      }
   }

   /* Get the fastest varying dimension in user's buffer */
   icvp->derv_dimconv_fastdim = icvp->var_ndims-1;
   if (icvp->var_is_vector && icvp->user_do_scalar)
      icvp->derv_dimconv_fastdim--;

   /* Find out how many pixels to compress/expand for variable and user
      buffers and allocate arrays */
   if (icvp->var_is_vector && icvp->user_do_scalar)
      icvp->derv_var_pix_num=icvp->var_vector_size;
   else
      icvp->derv_var_pix_num=1;
   icvp->derv_usr_pix_num=1;
   for (idim=0; idim<MI_PRIV_IMGDIMS; idim++) {
      if (icvp->derv_dim_grow[idim])
         icvp->derv_usr_pix_num *= icvp->derv_dim_scale[idim];
      else
         icvp->derv_var_pix_num *= icvp->derv_dim_scale[idim];
   }
   icvp->derv_var_pix_off = MALLOC(icvp->derv_var_pix_num, long);
   icvp->derv_usr_pix_off = MALLOC(icvp->derv_usr_pix_num, long);
   if ((icvp->derv_var_pix_off == NULL) || (icvp->derv_usr_pix_off == NULL)) {
      MI_LOG_SYS_ERROR1("MI_icv_get_dim_conversion");
      MI_RETURN_ERROR(MI_ERROR);
   }

   MI_RETURN(MI_NOERROR);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : miicv_detach
@INPUT      : icvid - icv id
@OUTPUT     : (none)
@RETURNS    : MI_ERROR if an error occurs
@DESCRIPTION: Detaches the cdf file and variable from the image conversion
              variable, allowing modifications to the icv.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines
@CREATED    : August 10, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int miicv_detach(int icvid)
{
   mi_icv_type *icvp;

   MI_SAVE_ROUTINE_NAME("miicv_detach");

   /* Check icv id */
   {MI_CHK_ERR(MI_icv_chkid(icvid))}
   icvp=minc_icv_list[icvid];

   /* Free the pixel offset arrays */
   if (icvp->derv_var_pix_off != NULL) FREE(icvp->derv_var_pix_off);
   if (icvp->derv_usr_pix_off != NULL) FREE(icvp->derv_usr_pix_off);

   /* Set cdfid field to MI_ERROR to indicate that icv is detached */
   minc_icv_list[icvid]->cdfid = MI_ERROR;

   MI_RETURN(MI_NOERROR);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : miicv_get
@INPUT      : icvid  - icv id
              start  - coordinates of start of hyperslab (see ncvarget)
              count  - size of hyperslab (see ncvarget)
@OUTPUT     : values - array of values returned
@RETURNS    : MI_ERROR if an error occurs
@DESCRIPTION: Gets a hyperslab of values from a netcdf variable through
              the image conversion variable (icvid) 
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines
@CREATED    : August 10, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int miicv_get(int icvid, long start[], long count[], void *values)
{
   mi_icv_type *icvp;
   double zeroval, zerobuf;
   void *zerostart;
   int zerolen, idim, ndims;
   char *bufptr, *bufend, *zeroptr, *zeroend;
   long buflen;

   MI_SAVE_ROUTINE_NAME("miicv_get");

   /* Check icv id */
   MI_CHK_ERR(MI_icv_chkid(icvid))
   icvp=minc_icv_list[icvid];

   /* Zero the user's buffer, if needed */

   if (icvp->derv_do_zero) {

      /* Create a zero pixel and get its size */
      zerostart = (void *) (&zerobuf);
      if (icvp->do_scale)
         zeroval = icvp->offset;
      else
         zeroval = 0.0;
      {MI_FROM_DOUBLE(zeroval, icvp->user_type, icvp->user_sign, zerostart)}
      zerolen = icvp->user_typelen;

      /* Get the buffer size */
      ndims = icvp->var_ndims;
      if (icvp->var_is_vector && icvp->user_do_scalar)
         ndims--;
      buflen = zerolen;
      for (idim=0; idim<ndims; idim++)
         buflen *= count[idim];

      /* Loop through the buffer, copying the zero pixel */
      bufend = (char *) values + buflen;
      zeroend = (char *) zerostart + zerolen;
      for (bufptr = (char *) values, zeroptr = (char *) zerostart;
           bufptr < bufend; bufptr++, zeroptr++) {
         if (zeroptr >= zeroend)
            zeroptr = (char *) zerostart;
         *bufptr = *zeroptr;
      }
      
   }

   /* Get the data */
   MI_CHK_ERR(MI_icv_access(MI_PRIV_GET, icvp, start, count, values))

   MI_RETURN(MI_NOERROR);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : miicv_put
@INPUT      : icvid  - icv id
              start  - coordinates of start of hyperslab (see ncvarput)
              count  - size of hyperslab (see ncvarput)
              values - array of values to store
@OUTPUT     : (none)
@RETURNS    : MI_ERROR if an error occurs
@DESCRIPTION: Stores a hyperslab of values in a netcdf variable through
              the image conversion variable (icvid)
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines
@CREATED    : 
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int miicv_put(int icvid, long start[], long count[], void *values)
{
   mi_icv_type *icvp;

   MI_SAVE_ROUTINE_NAME("miicv_put");

   /* Check icv id */
   MI_CHK_ERR(MI_icv_chkid(icvid))
   icvp=minc_icv_list[icvid];

   MI_CHK_ERR(MI_icv_access(MI_PRIV_PUT, icvp, start, count, values))

   MI_RETURN(MI_NOERROR);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : MI_icv_access
@INPUT      : operation - MI_PRIV_GET or MI_PRIV_PUT
              icvid     - icv id
              start     - coordinates of start of hyperslab (see ncvarput)
              count     - size of hyperslab (see ncvarput)
              values    - array of values to put
@OUTPUT     : values    - array of values to get
@RETURNS    : MI_ERROR if an error occurs
@DESCRIPTION: Does the work of getting or putting values from an icv.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines
@CREATED    : August 11, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int MI_icv_access(int operation, mi_icv_type *icvp, long start[], 
                          long count[], void *values)
{
   int *bufsize_step;                /* Pointer to array giving increments
                                        for allocating variable buffer
                                        (NULL if we don't care) */
   long chunk_count[MAX_VAR_DIMS];   /* Number of elements to get for chunk */
   long chunk_start[MAX_VAR_DIMS];   /* Starting index for getting a chunk */
   long var_start[MAX_VAR_DIMS];     /* Coordinates of first var element */
   long var_count[MAX_VAR_DIMS];     /* Edge lengths in variable */
   long var_end[MAX_VAR_DIMS];       /* Coordinates of last var element */
   int firstdim;
   int idim;

   MI_SAVE_ROUTINE_NAME("MI_icv_access");

   /* Check that icv is attached to a variable */
   if (icvp->cdfid == MI_ERROR) {
      MI_LOG_PKG_ERROR2(MI_ERR_ICVNOTATTACHED, 
                        "ICV is not attached to an image variable");
      MI_RETURN_ERROR(MI_ERROR);
   }

   /* Translate icv coordinates to variable coordinates */
   {MI_CHK_ERR(MI_icv_coords_tovar(icvp, start, count, var_start, var_count))}

   /* Do we care about getting variable in convenient increments ? 
      Only if we are getting data and the icv structure wants it */
   if ((operation==MI_PRIV_GET) && (icvp->derv_do_bufsize_step))
      bufsize_step = icvp->derv_bufsize_step;
   else
      bufsize_step = NULL;

   /* Set up variables for looping through variable. The biggest chunk that
      we can get in one call is determined by the subscripts of MIimagemax
      and MIimagemin. These must be constant over the chunk that we get if
      we are doing normalization. */
   (void) miset_coords(icvp->var_ndims, 0L, chunk_start);
   for (idim=0; idim<icvp->var_ndims; idim++)
      var_end[idim]=var_start[idim]+var_count[idim];
   (void) miset_coords(icvp->var_ndims, 1L, chunk_count);
   for (idim=MAX(icvp->derv_firstdim+1,0); idim < icvp->var_ndims; idim++)
      chunk_count[idim]=var_count[idim];
   firstdim= (icvp->user_do_norm) ? MAX(icvp->derv_firstdim, 0) : 0;

   /* Loop through variable */
   while (chunk_start[0] < var_end[0]) {

      /* Calculate scale factor */
      if (icvp->do_scale) {
         MI_CHK_ERR(MI_icv_calc_scale(operation, icvp, chunk_start))
      }

      /* Get the values */
      MI_CHK_ERR(MI_varaccess(operation, icvp->cdfid, icvp->varid,
                              chunk_start, chunk_count,
                              icvp->user_type, icvp->user_sign,
                              values, bufsize_step, icvp))

      /* Increment the start counter */
      chunk_start[firstdim] += chunk_count[firstdim];
      for (idim=firstdim; 
           (idim>0) && (chunk_start[idim]>var_end[idim]); idim--) {
         chunk_start[idim]=var_start[idim];
         chunk_start[idim-1]++;
      }
   }

   MI_RETURN(MI_NOERROR);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : MI_icv_coords_tovar
@INPUT      : icvp      - icv structure pointer
              icv_start - start vector for icv (if NULL, don't do conversion)
              icv_count - count vector for icv (if NULL, don't do conversion)
@OUTPUT     : var_start - start vector for variable
              var_count - count vector for variable
@RETURNS    : MI_ERROR if an error occurs
@DESCRIPTION: Converts a start and count vector for referencing an icv
              to the corresponding vectors for referencing a NetCDF variable.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines
@CREATED    : September 1, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int MI_icv_coords_tovar(mi_icv_type *icvp, 
                                long *icv_start, long *icv_count,
                                long *var_start, long *var_count)
{
   int i, j;
   int num_non_img_dims;
   long coord;

   MI_SAVE_ROUTINE_NAME("MI_icv_coords_tovar");

   /* Get the number of non image dimensions */
   num_non_img_dims=icvp->var_ndims-MI_PRIV_IMGDIMS;
   if (icvp->var_is_vector)
      num_non_img_dims--;

   /* Set the start values */
   if (icv_start!=NULL) {
      for (i=0; i < num_non_img_dims; i++)
         var_start[i] = icv_start[i];
      for (i=num_non_img_dims, j=MI_PRIV_IMGDIMS-1; 
           i < num_non_img_dims+MI_PRIV_IMGDIMS; i++, j--) {
         coord = icv_start[i];
         if (icvp->derv_dim_flip[j])
            coord = icvp->var_dim_size[j]-1-coord;
         if (icvp->derv_dim_grow[j])
            var_start[i] = coord*icvp->derv_dim_scale[j] + 
                                    icvp->derv_dim_off[j];
         else
            var_start[i] = coord/icvp->derv_dim_scale[j] +
                                    icvp->derv_dim_off[j];
      }
      if (icvp->var_is_vector) {
         var_start[icvp->var_ndims-1] = (icvp->user_do_scalar ?
                                       0 : icv_start[icvp->var_ndims-1]);
      }
   }

   /* Set the count value */
   if (icv_count!=NULL) {
      for (i=0; i < num_non_img_dims; i++)
         var_count[i] = icv_count[i];
      for (i=num_non_img_dims, j=MI_PRIV_IMGDIMS-1; 
           i < num_non_img_dims+MI_PRIV_IMGDIMS; i++, j--) {
         coord = icv_count[i];
         if (icvp->derv_dim_grow[j])
            var_count[i] = coord*icvp->derv_dim_scale[j];
         else
            var_count[i] = coord/icvp->derv_dim_scale[j];
      }
      if (icvp->var_is_vector) {
         var_count[icvp->var_ndims-1] = (icvp->user_do_scalar ?
                                         icvp->var_vector_size : 
                                         icv_count[icvp->var_ndims-1]);

      }
   }

   MI_RETURN(MI_NOERROR);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : MI_icv_calc_scale
@INPUT      : operation - MI_PRIV_GET or MI_PRIV_PUT
              icvp      - icv structure pointer
              coords    - coordinates of first value to get or put
@OUTPUT     : icvp      - fields scale and offset set
@RETURNS    : MI_ERROR if an error occurs
@DESCRIPTION: Calculates the scale and offset needed for getting or putting
              values, starting at index coords (assumes that scale is constant
              over that range).
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines
@CREATED    : August 10, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int MI_icv_calc_scale(int operation, mi_icv_type *icvp, long coords[])
{
   double usr_imgmax, usr_imgmin;
   double var_imgmax, var_imgmin;
   double usr_scale;
   double denom;

   MI_SAVE_ROUTINE_NAME("MI_icv_calc_scale");

   usr_imgmax=icvp->derv_imgmax;
   usr_imgmin=icvp->derv_imgmin;
   if ((icvp->imgmaxid==MI_ERROR) || (icvp->imgminid==MI_ERROR)) {
      var_imgmax = icvp->var_vmax;
      var_imgmin = icvp->var_vmin;
   }
   else {
      MI_CHK_ERR(mivarget1(icvp->cdfid, icvp->imgmaxid, coords,
                           NC_DOUBLE, NULL, &var_imgmax))
      MI_CHK_ERR(mivarget1(icvp->cdfid, icvp->imgminid, coords,
                           NC_DOUBLE, NULL, &var_imgmin))
   }

   /* Calculate scale and offset for MI_PRIV_GET */

   /* Scale */
   denom = usr_imgmax - usr_imgmin;
   if (denom!=0.0)
      usr_scale=(icvp->user_vmax - icvp->user_vmin) / denom;
   else
      usr_scale=0.0;
   denom = icvp->var_vmax - icvp->var_vmin;
   if (denom!=0.0)
      icvp->scale = usr_scale * (var_imgmax - var_imgmin) / denom;
   else
      icvp->scale = 0.0;

   /* Offset */
   icvp->offset = icvp->user_vmin - icvp->scale * icvp->var_vmin
                + usr_scale * (var_imgmin - usr_imgmin);

   /* If we want a MI_PRIV_PUT, invert */
   if (operation==MI_PRIV_PUT) {
      if (icvp->scale!=0.0) {
         icvp->offset = (-icvp->offset) / icvp->scale;
         icvp->scale  = 1.0/icvp->scale;
      }
      else {
         icvp->offset = icvp->user_vmin;
         icvp->scale  = 0.0;
      }
   }

   MI_RETURN(MI_NOERROR);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : MI_icv_dimconvert
@INPUT      : operation  - MI_PRIV_GET or MI_PRIV_PUT
              icvp       - icv structure pointer
              start      - start passed by user
              count      - count passed by user
              values     - pointer to user's data area (for put)
              bufstart   - start of variable buffer
              bufcount   - count of variable buffer
              buffer     - pointer to variable buffer (for get)
@OUTPUT     : values     - pointer to user's data area (for get)
              buffer     - pointer to variable buffer (for put)
@RETURNS    : MI_ERROR if an error occurs
@DESCRIPTION: Converts values and dimensions from an input buffer to the 
              user's buffer. Called by MI_var_action.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines
@CREATED    : August 27, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int MI_icv_dimconvert(int operation, mi_icv_type *icvp,
                              long start[], long count[], void *values,
                              long bufstart[], long bufcount[], void *buffer)
{
   mi_icv_dimconv_type dim_conv_struct;
   mi_icv_dimconv_type *dcp;
   double sum0, sum1;           /* Counters for averaging values */
   double dvalue;               /* Pixel value */
   long counter[MAX_VAR_DIMS];  /* Dimension loop counter */
   void *ptr, *iptr, *optr;     /* Pointers for looping through fastest dim */
   void *ivecptr[MAX_VAR_DIMS]; /* Pointers to start of each dimension */
   void *ovecptr[MAX_VAR_DIMS];
   long *end;                   /* Pointer to array of dimension ends */
   int fastdim;                 /* Dimension that varies fastest */
   long i;                      /* Buffer subscript */
   int idim;                    /* Dimension subscript */
   int notmodified;             /* First dimension not reset */

   MI_SAVE_ROUTINE_NAME("MI_icv_dimconvert");

   /* Initialize variables */
   dcp = &dim_conv_struct;
   {MI_CHK_ERR(MI_icv_dimconv_init(operation, icvp, dcp, start, count, values,
                                   bufstart, bufcount, buffer))}

   /* Initialize local variables */
   iptr    = dcp->istart;
   optr    = dcp->ostart;
   end     = dcp->end;
   fastdim = icvp->derv_dimconv_fastdim;

   /* Initialize counters */
   for (idim=0; idim<=fastdim; idim++) {
      counter[idim] = 0;
      ivecptr[idim] = iptr;
      ovecptr[idim] = optr;
   }

   /* Loop through data */

   while (counter[0] < dcp->end[0]) {

      /* Compress data by averaging if needed */
      if (!dcp->do_compress) {
         {MI_TO_DOUBLE(dvalue, dcp->intype, dcp->insign, iptr)}
      }
      else {
         sum1 = 0.0;
         sum0 = 0.0;
         for (i=0; i<dcp->in_pix_num; i++) {
            ptr=(void *) ((char *)iptr + dcp->in_pix_off[i]);

            /* Check if we are outside the buffer.
               If we are looking before the buffer, then we need to
               add in the previous result to do averaging properly. If
               we are looking after the buffer, then break. */
            if (ptr<dcp->in_pix_first) {
               /* Get the output value and re-scale it */
               {MI_TO_DOUBLE(dvalue, dcp->outtype, dcp->outsign, optr)}
               if (icvp->do_scale) {
                  dvalue = ((icvp->scale==0.0) ?
                            0.0 : (dvalue - icvp->offset) / icvp->scale);
               }
            }
            else if (ptr>dcp->in_pix_last) {
               break;
            }
            else {
               {MI_TO_DOUBLE(dvalue, dcp->intype, dcp->insign, ptr)}
            }    /* If out of buffer */

            /* Add in the value */
            sum1 += dvalue;
            sum0++;
         }         /* Foreach pixel to compress */

         /* Average values */
         if (sum0!=0.0)
            dvalue = sum1/sum0;
         else
            dvalue = 0.0;
      }           /* If compress */

      /* Scale result */
      if (icvp->do_scale) {
         dvalue = icvp->scale * dvalue + icvp->offset;
      }

      /* Expand data if needed */
      if (!dcp->do_expand) {
         {MI_FROM_DOUBLE(dvalue, dcp->outtype, dcp->outsign, optr)}
      }
      else {
         for (i=0; i<dcp->out_pix_num; i++) {
            ptr=(void *) ((char *)optr + dcp->out_pix_off[i]);

            /* Check if we are outside the buffer. */
            if (ptr>dcp->out_pix_last) {
               break;
            }
            else if (ptr>=dcp->out_pix_first) {
               {MI_FROM_DOUBLE(dvalue, dcp->outtype, dcp->outsign, ptr)}
            }    /* If out of buffer */

         }         /* Foreach pixel to expand */
      }         /* if expand */

      /* Increment the counter and the pointers */
      if ((++counter[fastdim]) < dcp->end[fastdim]) {
         optr = (void *) ((char *) optr + dcp->ostep[fastdim]);
         iptr = (void *) ((char *) iptr + dcp->istep[fastdim]);
      }
      else {
         /* If we reach the end of fastdim, then reset the counter and
            increment the next dimension down - keep going as needed.
            The vectors ovecptr and ivecptr give the starting values of optr 
            and iptr for that dimension. */
         idim = fastdim;
         while ((idim>0) && (counter[idim] >= end[idim])) {
            counter[idim] = 0;
            idim--;
            counter[idim]++;
            ovecptr[idim] = (void *)((char *)ovecptr[idim]+dcp->ostep[idim]);
            ivecptr[idim] = (void *)((char *)ivecptr[idim]+dcp->istep[idim]);
         }
         notmodified = idim;

         /* Copy the starting index up the vector */
         for (idim=notmodified+1; idim<=fastdim; idim++) {
            ovecptr[idim]=ovecptr[notmodified];
            ivecptr[idim]=ivecptr[notmodified];
         }

         optr = ovecptr[fastdim];
         iptr = ivecptr[fastdim];
      }      /* if at end of row */

   }      /* while more pixels to process */


   MI_RETURN(MI_NOERROR);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : MI_icv_dimconv_init
@INPUT      : operation  - MI_PRIV_GET or MI_PRIV_PUT
              icvp       - icv structure pointer
              dcp        - dimconvert structure pointer
              start      - start passed by user
              count      - count passed by user
              values     - pointer to user's data area (for put)
              bufstart   - start of variable buffer
              bufcount   - count of variable buffer
              buffer     - pointer to variable buffer (for get)
@OUTPUT     : 
@RETURNS    : MI_ERROR if an error occurs
@DESCRIPTION: Sets up stuff for MI_icv_dimconvert.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines
@CREATED    : September 4, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int MI_icv_dimconv_init(int operation, mi_icv_type *icvp,
                              mi_icv_dimconv_type *dcp,
                              long start[], long count[], void *values,
                              long bufstart[], long bufcount[], void *buffer)
{
   long buffer_len, values_len; /* Buffer lengths, offsets and indices */
   long buffer_off, values_off;
   long buffer_index, values_index;
   int imgdim_high, imgdim_low; /* Range of subscripts of image dimensions */
   int scale, offset;            /* Dimension scale and offset */
   int idim, jdim;
   int fastdim;
   /* Variables for calculating pixel offsets for compress/expand */
   long var_dcount[MI_PRIV_IMGDIMS+1], var_dend[MI_PRIV_IMGDIMS];
   long usr_dcount[MI_PRIV_IMGDIMS+1], usr_dend[MI_PRIV_IMGDIMS];
   long pixcount;
   int var_fd, usr_fd, dshift;
   long ipix;

   MI_SAVE_ROUTINE_NAME("MI_icv_dimconv_init");

   /* Check to see if any compression or expansion needs to be done.
      Work it out for a GET and then swap if a PUT. */
   if (operation==MI_PRIV_GET) {
      dcp->do_compress = icvp->derv_var_compress;
      dcp->do_expand   = icvp->derv_usr_compress;
   }
   else {
      dcp->do_expand   = icvp->derv_var_compress;
      dcp->do_compress = icvp->derv_usr_compress;
   }

   fastdim = icvp->derv_dimconv_fastdim;

   /* Get the buffer sizes */
   buffer_len = icvp->var_typelen;
   values_len = icvp->user_typelen;
   for (idim=0; idim < icvp->var_ndims; idim++) {
      buffer_len *= bufcount[idim];
      if (idim<=dcp->fastdim)
         values_len *= count[idim];
   }

   /* Calculate step size for variable and user buffers */
   dcp->buf_step[fastdim] = icvp->var_typelen;
   if (icvp->var_is_vector && icvp->user_do_scalar)
      dcp->buf_step[fastdim] *= bufcount[icvp->var_ndims-1];
   dcp->usr_step[fastdim] = icvp->user_typelen;
   for (idim=fastdim-1; idim>=0; idim++) {
      dcp->buf_step[idim] = dcp->buf_step[idim+1] * bufcount[idim+1];
      dcp->usr_step[idim] = dcp->usr_step[idim+1] * count[idim+1];
   }

   /* Get the pointers to the start of buffers and the number of pixels
      in each dimension (count a pixel as one expansion/compression -
      one time through the loop below) */
   imgdim_high=icvp->var_ndims-1;
   if (icvp->var_is_vector && icvp->user_do_scalar) imgdim_high--;
   imgdim_low = imgdim_high - MI_PRIV_IMGDIMS;
   buffer_off = 0;
   values_off = 0;
   for (idim=0; idim <= fastdim; idim++) {
      if ((idim < imgdim_low) || (idim > imgdim_high)) {
         dcp->end[idim] = bufcount[idim];
         buffer_index = 0;
         values_index = bufstart[idim] - start[idim];
      }
      else {
         scale = icvp->derv_dim_scale[idim];
         offset = icvp->derv_dim_off[idim];
         if (icvp->derv_dim_grow[idim]) {
            dcp->end[idim] = bufcount[idim];
            buffer_index = 0;
            values_index = bufstart[idim]*scale - start[idim] + offset;
         }
         else {
            dcp->end[idim] = (bufcount[idim] + scale - 1 + 
                              bufstart[idim]%scale); 
            buffer_index = -(bufstart[idim] % scale);
            values_index = bufstart[idim]/scale - start[idim] + offset;
         }
      }
      buffer_off += buffer_index * dcp->buf_step[idim];
      values_off += values_index * dcp->usr_step[idim];
   }

   /* Calculate arrays of offsets for compress/expand. */
   if (dcp->do_compress || dcp->do_expand) {
      /* Initialize counters */
      var_fd = MI_PRIV_IMGDIMS-1;
      usr_fd = MI_PRIV_IMGDIMS-1;
      if (icvp->var_is_vector && icvp->user_do_scalar) {
         var_fd++;
         var_dcount[var_fd]=0;
         var_dend[var_fd]=icvp->var_vector_size;
      }
      for (idim=0; idim<MI_PRIV_IMGDIMS; idim++) {
         jdim=MI_PRIV_IMGDIMS - idim - 1;
         var_dcount[jdim] = 0;
         usr_dcount[jdim] = 0;
         var_dend[jdim] = (icvp->derv_dim_grow[idim] ?
                           1 : icvp->derv_dim_scale[idim]);
         usr_dend[jdim] = (icvp->derv_dim_grow[idim] ?
                           icvp->derv_dim_scale[idim] : 1);
      }

      /* Loop through variable buffer pixels */
      pixcount=0;
      dshift = icvp->var_ndims - var_fd - 1;
      for (ipix=0; ipix<icvp->derv_var_pix_num; ipix++) {
         icvp->derv_var_pix_off[ipix] = pixcount;
         pixcount += dcp->buf_step[var_fd+dshift];
         if ((var_dcount[var_fd]++) > var_dend[var_fd]) {
            idim=var_fd;
            while ((idim>0) && (var_dcount[idim]>=var_dend[idim])) {
               var_dcount[idim]=0;
               idim--;
               var_dcount[idim]++;
            }
            pixcount = var_dcount[idim] * dcp->buf_step[idim+dshift];
         }
      }

      /* Loop through user buffer pixels */
      pixcount=0;
      dshift = icvp->var_ndims - usr_fd - 1;
      for (ipix=0; ipix<icvp->derv_usr_pix_num; ipix++) {
         icvp->derv_usr_pix_off[ipix] = pixcount;
         pixcount += dcp->usr_step[usr_fd+dshift];
         if ((++usr_dcount[usr_fd]) > usr_dend[usr_fd]) {
            idim=usr_fd;
            while ((idim>0) && (usr_dcount[idim]>=usr_dend[idim])) {
               usr_dcount[idim]=0;
               idim--;
               usr_dcount[idim]++;
            }
            pixcount = usr_dcount[idim] * dcp->usr_step[idim+dshift];
         }
      }

   }           /* if compress/expand */

   /* Set input and output variables */
   if (operation==MI_PRIV_GET) {          /* For a GET */
      dcp->in_pix_num = icvp->derv_var_pix_num;
      dcp->in_pix_off = icvp->derv_var_pix_off;
      dcp->in_pix_first = buffer;
      dcp->in_pix_last = (void *) ((char *)buffer + buffer_len);
      dcp->out_pix_num = icvp->derv_usr_pix_num;
      dcp->out_pix_off = icvp->derv_usr_pix_off;
      dcp->out_pix_first = values;
      dcp->out_pix_last = (void *) ((char *)values + values_len);
      dcp->intype = icvp->var_type;
      dcp->insign = icvp->var_sign;
      dcp->outtype = icvp->user_type;
      dcp->outsign = icvp->user_sign;
      dcp->istep = dcp->buf_step;
      dcp->ostep = dcp->usr_step;
      dcp->istart = (void *) ((char *) buffer + buffer_off);
      dcp->ostart = (void *) ((char *) values + values_off);
   }                   /* if GET */
   else {                                 /* For a PUT */
      dcp->out_pix_num = icvp->derv_var_pix_num;
      dcp->out_pix_off = icvp->derv_var_pix_off;
      dcp->out_pix_first = buffer;
      dcp->out_pix_last = (void *) ((char *)buffer + buffer_len);
      dcp->in_pix_num = icvp->derv_usr_pix_num;
      dcp->in_pix_off = icvp->derv_usr_pix_off;
      dcp->in_pix_first = values;
      dcp->in_pix_last = (void *) ((char *)values + values_len);
      dcp->outtype = icvp->var_type;
      dcp->outsign = icvp->var_sign;
      dcp->intype = icvp->user_type;
      dcp->insign = icvp->user_sign;
      dcp->ostep = dcp->buf_step;
      dcp->istep = dcp->usr_step;
      dcp->ostart = (void *) ((char *) buffer + buffer_off);
      dcp->istart = (void *) ((char *) values + values_off);
   }                   /* if PUT */

   MI_RETURN(MI_NOERROR);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : MI_icv_chkid
@INPUT      : icvid  - icv id
@OUTPUT     : (none)
@RETURNS    : MI_ERROR if an error occurs
@DESCRIPTION: Checks that an icv id is valid
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines
@CREATED    : August 7, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int MI_icv_chkid(int icvid)
{
   MI_SAVE_ROUTINE_NAME("MI_icv_chkid");

   /* Check icv id */
   if ((icvid<0) || (icvid>MI_MAX_NUM_ICV) || (minc_icv_list[icvid]==NULL)) {
      MI_LOG_PKG_ERROR2(MI_ERR_BADICV,"Illegal icv identifier");
      MI_RETURN_ERROR(MI_ERROR);
   }

   MI_RETURN(MI_NOERROR);
}

