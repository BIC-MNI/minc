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
                 miicv_ndattach
                 miicv_detach
                 miicv_get
                 miicv_put
              semiprivate :
                 MI_icv_chkid
              private :
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
   int idim;

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
   icvp->do_dimconvert = FALSE;

   /* User defaults */
   icvp->user_type = NC_SHORT;
   icvp->user_typelen = nctypelen(icvp->user_type);
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
   icvp->user_do_dimconv = FALSE;
   icvp->user_do_scalar = TRUE;
   icvp->user_xdim_dir = MI_ICV_POSITIVE;
   icvp->user_ydim_dir = MI_ICV_POSITIVE;
   icvp->user_zdim_dir = MI_ICV_POSITIVE;
   icvp->user_keep_aspect = TRUE;
   for (idim=0; idim<MI_PRIV_IMGDIMS; idim++) {
      icvp->user_dim_size[idim]=MI_ICV_ANYSIZE;
   }

   /* Variable values */
   icvp->cdfid = MI_ERROR;            /* Set so that we can recognise an */
   icvp->varid = MI_ERROR;            /* unattached icv */

   /* Values that can be read by user */
   for (idim=0; idim<MI_PRIV_IMGDIMS; idim++) {
      icvp->derv_dim_step[idim] = 0.0;
      icvp->derv_dim_start[idim] = 0.0;
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
   if (MI_icv_chkid(icvid) == NULL) MI_RETURN_ERROR(MI_ERROR);

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
   if ((icvp=MI_icv_chkid(icvid)) == NULL) MI_RETURN_ERROR(MI_ERROR);

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
      icvp->user_typelen= nctypelen(icvp->user_type);
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
   case MI_ICV_DO_DIM_CONV:
      icvp->user_do_dimconv = *((int *) value); break;
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
   if ((icvp=MI_icv_chkid(icvid)) == NULL) MI_RETURN_ERROR(MI_ERROR);

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
   case MI_ICV_DO_DIM_CONV:
      *((int *) value) = icvp->user_do_dimconv; break;
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
@NAME       : miicv_ndattach
@INPUT      : icvid - icv id
              cdfid - cdf file id
              varid - cdf variable id
@OUTPUT     : (none)
@RETURNS    : MI_ERROR if an error occurs
@DESCRIPTION: Attaches an open cdf file and variable to an image conversion
              variable for subsequent access through miicvget and miicvput.
              File must be in data mode. This routine differs from 
              miicv_attach in that no dimension conversions will be made
              on the variable (avoids linking in a significant amount
              of code).
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines
@CREATED    : September 9, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int miicv_ndattach(int icvid, int cdfid, int varid)
{
   mi_icv_type *icvp;         /* Pointer to icv structure */
   int idim;

   MI_SAVE_ROUTINE_NAME("miicv_attach");

   /* Check icv id */
   if ((icvp=MI_icv_chkid(icvid)) == NULL) MI_RETURN_ERROR(MI_ERROR);

   /* If the icv is attached, then detach it */
   if (icvp->cdfid != MI_ERROR)
      {MI_CHK_ERR(miicv_detach(icvid))}

   /* Inquire about the variable's type, sign and number of dimensions */
   {MI_CHK_ERR(MI_icv_get_type(icvp, cdfid, varid))}

   /* Get valid range */
   {MI_CHK_ERR(MI_icv_get_vrange(icvp, cdfid, varid))}
          
   /* Get normalization info */
   {MI_CHK_ERR(MI_icv_get_norm(icvp, cdfid, varid))}

   /* Set other fields to defaults */
   icvp->var_is_vector = FALSE;
   icvp->var_vector_size = 1;
   icvp->derv_do_zero = FALSE;
   icvp->derv_do_bufsize_step = FALSE;
   icvp->derv_var_pix_off = NULL;
   icvp->derv_usr_pix_off = NULL;
   for (idim=0; idim<MI_PRIV_IMGDIMS; idim++) {
      icvp->derv_dim_flip[idim] = FALSE;
      icvp->derv_dim_grow[idim] = TRUE;
      icvp->derv_dim_scale[idim] = 1;
      icvp->derv_dim_off[idim] = 0;
      icvp->derv_dim_step[idim] = 0.0;
      icvp->derv_dim_start[idim] = 0.0;
   }

   /* Set the do_scale and do_dimconvert fields of icv structure
      We have to scale only if do_range is TRUE. If ranges don't
      match, or we have to do user normalization, or if we are normalizing
      and MIimagemax or MIimagemin vary over the variable. */

   icvp->do_scale = 
      (icvp->user_do_range && 
       ((icvp->user_vmax!=icvp->var_vmax) ||
        (icvp->user_vmin!=icvp->var_vmin) ||
        (icvp->user_do_norm && icvp->user_user_norm) ||
        (icvp->user_do_norm && (icvp->derv_firstdim>=0))) );

   icvp->do_dimconvert = FALSE;

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
   if ((icvp=MI_icv_chkid(icvid)) == NULL) MI_RETURN_ERROR(MI_ERROR);

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

   MI_SAVE_ROUTINE_NAME("miicv_get");

   /* Check icv id */
   if ((icvp=MI_icv_chkid(icvid)) == NULL) MI_RETURN_ERROR(MI_ERROR);

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
   if ((icvp=MI_icv_chkid(icvid)) == NULL) MI_RETURN_ERROR(MI_ERROR);

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

   /* Zero the user's buffer if needed */
   if ((operation == MI_PRIV_GET) && (icvp->derv_do_zero))
      {MI_CHK_ERR(MI_icv_zero_buffer(icvp, count, values))}

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
@NAME       : MI_icv_zero_buffer
@INPUT      : icvp      - icv structure pointer
              count     - count vector
              values    - pointer to user's buffer
@OUTPUT     : 
@RETURNS    : MI_ERROR if an error occurs
@DESCRIPTION: Zeros the user's buffer, with a size given by the vector count.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines
@CREATED    : September 9, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int MI_icv_zero_buffer(mi_icv_type *icvp, long count[], void *values)
{
   double zeroval, zerobuf;
   void *zerostart;
   int zerolen, idim, ndims;
   char *bufptr, *bufend, *zeroptr, *zeroend;
   long buflen;

   MI_SAVE_ROUTINE_NAME("MI_icv_zero_buffer");

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
@NAME       : MI_icv_chkid
@INPUT      : icvid  - icv id
@OUTPUT     : (none)
@RETURNS    : Pointer to icv structure if it exists, otherwise NULL.
@DESCRIPTION: Checks that an icv id is valid and returns a pointer to the
              structure.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines
@CREATED    : August 7, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
semiprivate mi_icv_type *MI_icv_chkid(int icvid)
{
   MI_SAVE_ROUTINE_NAME("MI_icv_chkid");

   /* Check icv id */
   if ((icvid<0) || (icvid>MI_MAX_NUM_ICV) || (minc_icv_list[icvid]==NULL)) {
      MI_LOG_PKG_ERROR2(MI_ERR_BADICV,"Illegal icv identifier");
      MI_RETURN_ERROR(NULL);
   }

   MI_RETURN(minc_icv_list[icvid]);
}
