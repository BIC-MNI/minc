/* ----------------------------- MNI Header -----------------------------------
@NAME       : netcdf_convenience.c
@DESCRIPTION: File of convenience functions for netcdf routines. There
              is nothing MINC specific about these routines, they just
              provide convenient ways of getting netcdf data. The
              routines MI_convert_type, mivarget and mivarget1 break this
              rule by making use of the MINC variable attribute MIsigntype
              to determine the sign of an integer variable.
@METHOD     : Routines included in this file :
              public :
                 miattget
                 miattget1
                 miattgetstr
                 miattputdbl
                 miattputstr
                 mivarget
                 mivarget1
                 mivarput
                 mivarput1
                 miset_coords
                 mitranslate_coords
                 micopy_all_atts
                 micopy_var_def
                 micopy_var_values
                 micopy_all_var_defs
                 micopy_all_var_values
              private :
                 MI_vcopy_action
@CREATED    : July 27, 1992. (Peter Neelin, Montreal Neurological Institute)
@MODIFIED   : 
---------------------------------------------------------------------------- */

#include <minc_private.h>



/* ----------------------------- MNI Header -----------------------------------
@NAME       : miattget
@INPUT      : cdfid      - cdf file id
              varid      - variable id
              name       - name of attribute
              datatype   - type that calling routine wants (one of the valid
                 netcdf data types, excluding NC_CHAR)
              max_length - maximum length to return (number of elements)
@OUTPUT     : value      - value of attribute
              att_length - actual length of attribute (number of elements)
                 If NULL, then no value is returned.
@RETURNS    : MI_ERROR (=-1) when an error occurs
@DESCRIPTION: Similar to routine ncattget, but the calling routine specifies
              the form in which data should be returned (datatype), as well
              as the maximum number of elements to get. The datatype can
              only be a numeric type. If the attribute in the file is of type 
              NC_CHAR, then an error is returned. The actual length of the
              vector is returned in att_length.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines and MI_convert_type
@CREATED    : July 27, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int miattget(int cdfid, int varid, char *name, nc_type datatype,
                    int max_length, void *value, int *att_length)
{
   nc_type att_type;          /* Type of attribute */
   int actual_length;         /* Actual length of attribute */
   void *att_value;           /* Pointer to attribute value */
   int status;                /* Status of nc routine */

   MI_SAVE_ROUTINE_NAME("miattget");

   /* Inquire about the attribute */
   MI_CHK_ERR(ncattinq(cdfid, varid, name, &att_type, &actual_length))

   /* Save the actual length of the attribute */
   if (att_length != NULL)
      *att_length = actual_length;

   /* Check that the attribute type is numeric */
   if ((datatype==NC_CHAR) || (att_type==NC_CHAR)) {
      MI_LOG_PKG_ERROR2(MI_ERR_NONNUMERIC,"Non-numeric datatype");
      MI_RETURN_ERROR(MI_ERROR);
   }

   /* Check to see if the type requested is the same as the attribute type
      and that the length is less than or equal to max_length. If it is, just 
      get the value. */
   if ((datatype == att_type) && (actual_length <= max_length)) {
      MI_CHK_ERR(status=ncattget(cdfid, varid, name, value))
      MI_RETURN(status);
   }

   /* Otherwise, get space for the attribute */
   if ((att_value = MALLOC(actual_length * nctypelen(att_type), char))
                      == NULL) {
      MI_LOG_SYS_ERROR1("miattget");
      MI_RETURN_ERROR(MI_ERROR);
   }

   /* Get the attribute */
   if (ncattget(cdfid, varid, name, att_value)==MI_ERROR) {
      FREE(att_value);
      MI_RETURN_ERROR(MI_ERROR);
   }

   /* Get the values.
      Call MI_convert_type with :
         MI_convert_type(number_of_values,
                         intype,  insign,  invalues,
                         outtype, outsign, outvalues,
                         icvp) */
   status=MI_convert_type(MIN(max_length, actual_length), 
                          att_type, MI_PRIV_DEFSIGN, att_value,
                          datatype, MI_PRIV_DEFSIGN, value,
                          NULL);
   FREE(att_value);
   MI_CHK_ERR(status)
   MI_RETURN(status);
   
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : miattget1
@INPUT      : cdfid      - cdf file id
              varid      - variable id
              name       - name of attribute
              datatype   - type that calling routine wants (one of the valid
                 netcdf data types, excluding NC_CHAR)
@OUTPUT     : value      - value of attribute
@RETURNS    : MI_ERROR (=-1) when an error occurs
@DESCRIPTION: Similar to routine miattget, but the its gets only one value
              of an attribute. If the attribute is longer, then an error
              occurs.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines and miattget
@CREATED    : July 27, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int miattget1(int cdfid, int varid, char *name, nc_type datatype,
                    void *value)
{
   int att_length;      /* Actual length of the attribute */

   MI_SAVE_ROUTINE_NAME("miattget1");

   /* Get the attribute value and its actual length */
   MI_CHK_ERR(miattget(cdfid, varid, name, datatype, 1, value, &att_length))

   /* Check that the length is 1 */
   if (att_length != 1) {
      MI_LOG_PKG_ERROR2(MI_ERR_NONSCALAR,"Attribute is not a scalar value");
      MI_RETURN_ERROR(MI_ERROR);
   }

   MI_RETURN(MI_NOERROR);

}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : miattgetstr
@INPUT      : cdfid    - cdf file id
              varid    - variable id
              name     - name of attribute
              maxlen   - maximum length of string to be copied
@OUTPUT     : value    - string returned
@RETURNS    : pointer to string value or NULL if an error occurred.
@DESCRIPTION: Gets a character attribute, copying up to maxlen characters
              into value and adding a terminating '\0' if necessary. A
              pointer to the string is returned to facilitate use.
              If the attribute is non-character, a NULL pointer is returned.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines.
@CREATED    : July 28, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public char *miattgetstr(int cdfid, int varid, char *name, 
                         int maxlen, char *value)
{
   nc_type att_type;          /* Type of attribute */
   int att_length;            /* Length of attribute */
   char *att_value;           /* Pointer to attribute value */

   MI_SAVE_ROUTINE_NAME("miattgetstr");

   /* Inquire about the attribute */
   if (ncattinq(cdfid, varid, name, &att_type, &att_length)==MI_ERROR)
      MI_RETURN_ERROR(NULL);

   /* Check that the attribute type is character */
   if (att_type!=NC_CHAR) {
      MI_LOG_PKG_ERROR2(MI_ERR_NONCHAR,"Non-character datatype");
      MI_RETURN_ERROR(NULL);
   }

   /* Check to see if the attribute length is less than maxlen. 
      If it is, just get the value. */
   if (att_length <= maxlen) {
      if (ncattget(cdfid, varid, name, value) == MI_ERROR)
         MI_RETURN_ERROR(NULL);
      /* Check the last character for a '\0' */
      if (value[att_length-1] != '\0') {
         if (att_length=maxlen)
            value[att_length-1] = '\0';
         else
            value[att_length]   = '\0';
      }
      MI_RETURN(value);
   }

   /* Otherwise, get space for the attribute */
   if ((att_value = MALLOC(att_length * nctypelen(att_type), char))
                      ==NULL) {
      MI_LOG_SYS_ERROR1("miattgetstr");
      MI_RETURN_ERROR(NULL);
   }

   /* Get the attribute */
   if (ncattget(cdfid, varid, name, att_value)==MI_ERROR) {
      FREE(att_value);
      MI_RETURN_ERROR(NULL);
   }

   /* Copy the attribute */
   (void) strncpy(value, att_value, (size_t) maxlen-1);

   /* Free the string */
   FREE(att_value);

   /* Return a pointer to the string */
   MI_RETURN(value);

}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : miattputdbl
@INPUT      : cdfid    - cdf file id
              varid    - variable id
              name     - name of attribute
              value    - double value for attribute
@OUTPUT     : (none)
@RETURNS    : MI_ERROR (=-1) if an error occurs
@DESCRIPTION: Convenience routine for calling ncattput with doubles.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines.
@CREATED    : August 5, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int miattputdbl(int cdfid, int varid, char *name, double value)
{
   MI_SAVE_ROUTINE_NAME("miattputdbl");

   MI_CHK_ERR(ncattput(cdfid, varid, name, NC_DOUBLE, 1, (void *) &value))
   MI_RETURN(MI_NOERROR);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : miattputstr
@INPUT      : cdfid    - cdf file id
              varid    - variable id
              name     - name of attribute
              value    - string value for attribute
@OUTPUT     : (none)
@RETURNS    : MI_ERROR (=-1) if an error occurs
@DESCRIPTION: Convenience routine for calling ncattput with character strings.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines.
@CREATED    : July 28, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int miattputstr(int cdfid, int varid, char *name, char *value)
{
   MI_SAVE_ROUTINE_NAME("miattputstr");

   MI_CHK_ERR(ncattput(cdfid, varid, name, NC_CHAR, 
                       strlen(value) + 1, (void *) value))
   MI_RETURN(MI_NOERROR);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : mivarget
@INPUT      : cdfid    - cdf file id
              varid    - variable id
              start    - vector of coordinates of corner of hyperslab
              count    - vector of edge lengths of hyperslab
              datatype - type that calling routine wants (one of the valid
                 netcdf data types, excluding NC_CHAR)
              sign     - sign that calling routine wants (one of
                 EMPTY_STRING (or NULL) = use default sign
                 MI_SIGNED              = signed values
                 MI_UNSIGNED            = unsigned values
@OUTPUT     : values   - value of variable
@RETURNS    : MI_ERROR (=-1) when an error occurs
@DESCRIPTION: Similar to routine ncvarget, but the calling routine specifies
              the form in which data should be returned (datatype), as well
              as the sign. The datatype can only be a numeric type. If the 
              variable in the file is of type NC_CHAR, then an error is 
              returned.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF and MINC routines
@CREATED    : July 29, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int mivarget(int cdfid, int varid, long start[], long count[],
                    nc_type datatype, char *sign, void *values)
{
   MI_SAVE_ROUTINE_NAME("mivarget");

   MI_CHK_ERR(MI_varaccess(MI_PRIV_GET, cdfid, varid, start, count,
                           datatype, MI_get_sign_from_string(datatype, sign),
                           values, NULL, NULL))
   MI_RETURN(MI_NOERROR);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : mivarget1
@INPUT      : cdfid    - cdf file id
              varid    - variable id
              mindex   - vector of coordinates of value to get
              datatype - type that calling routine wants (one of the valid
                 netcdf data types, excluding NC_CHAR)
              sign     - sign that calling routine wants (one of
                 EMPTY_STRING (or NULL) = use default sign
                 MI_SIGNED              = signed values
                 MI_UNSIGNED            = unsigned values
@OUTPUT     : value    - value of variable
@RETURNS    : MI_ERROR (=-1) when an error occurs
@DESCRIPTION: Similar to routine ncvarget1, but the calling routine specifies
              the form in which data should be returned (datatype), as well
              as the sign. The datatype can only be a numeric type. If the 
              variable in the file is of type NC_CHAR, then an error is 
              returned.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF and MINC routines
@CREATED    : July 29, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int mivarget1(int cdfid, int varid, long mindex[],
                     nc_type datatype, char *sign, void *value)
{
   long count[MAX_VAR_DIMS];

   MI_SAVE_ROUTINE_NAME("mivarget1");

   MI_CHK_ERR(MI_varaccess(MI_PRIV_GET, cdfid, varid, mindex, 
                           miset_coords(MAX_VAR_DIMS, 1L, count),
                           datatype, MI_get_sign_from_string(datatype, sign),
                           value, NULL, NULL))
   MI_RETURN(MI_NOERROR);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : mivarput
@INPUT      : cdfid    - cdf file id
              varid    - variable id
              start    - vector of coordinates of corner of hyperslab
              count    - vector of edge lengths of hyperslab
              datatype - type that calling routine is passing (one of the valid
                 netcdf data types, excluding NC_CHAR)
              sign     - sign that calling routine is passing (one of
                 EMPTY_STRING (or NULL) = use default sign
                 MI_SIGNED              = signed values
                 MI_UNSIGNED            = unsigned values
              values   - value of variable
@OUTPUT     : (none)
@RETURNS    : MI_ERROR (=-1) when an error occurs
@DESCRIPTION: Similar to routine ncvarput, but the calling routine specifies
              the form in which data is passes (datatype), as well
              as the sign. The datatype can only be a numeric type. If the 
              variable in the file is of type NC_CHAR, then an error is 
              returned.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF and MINC routines
@CREATED    : July 29, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int mivarput(int cdfid, int varid, long start[], long count[],
                    nc_type datatype, char *sign, void *values)
{
   MI_SAVE_ROUTINE_NAME("mivarput");

   MI_CHK_ERR(MI_varaccess(MI_PRIV_PUT, cdfid, varid, start, count,
                           datatype, MI_get_sign_from_string(datatype, sign),
                           values, NULL, NULL))
   MI_RETURN(MI_NOERROR);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : mivarput1
@INPUT      : cdfid    - cdf file id
              varid    - variable id
              mindex   - vector of coordinates of value to put
              datatype - type that calling routine is passing (one of the valid
                 netcdf data types, excluding NC_CHAR)
              sign     - sign that calling routine is passing (one of
                 EMPTY_STRING (or NULL) = use default sign
                 MI_SIGNED              = signed values
                 MI_UNSIGNED            = unsigned values
@OUTPUT     : value    - value of variable
@RETURNS    : MI_ERROR (=-1) when an error occurs
@DESCRIPTION: Similar to routine ncvarput1, but the calling routine specifies
              the form in which data is passed (datatype), as well
              as the sign. The datatype can only be a numeric type. If the 
              variable in the file is of type NC_CHAR, then an error is 
              returned.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF and MINC routines
@CREATED    : July 29, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int mivarput1(int cdfid, int varid, long mindex[],
                     nc_type datatype, char *sign, void *value)
{
   long count[MAX_VAR_DIMS];

   MI_SAVE_ROUTINE_NAME("mivarput1");

   MI_CHK_ERR(MI_varaccess(MI_PRIV_PUT, cdfid, varid, mindex, 
                           miset_coords(MAX_VAR_DIMS, 1L, count),
                           datatype, MI_get_sign_from_string(datatype, sign),
                           value, NULL, NULL))
   MI_RETURN(MI_NOERROR);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : miset_coords
@INPUT      : nvals  - number of values in coordinate vector to set
              value  - value to which coordinates should be set
@OUTPUT     : coords - coordinate vector
@RETURNS    : pointer to coords.
@DESCRIPTION: Sets nvals entries of the vector coords to value.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : July 29, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public long *miset_coords(int nvals, long value, long coords[])
{
   int i;

   MI_SAVE_ROUTINE_NAME("miset_coords");

   for (i=0; i<nvals; i++) {
      coords[i] = value;
   }
   MI_RETURN(coords);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : mitranslate_coords
@INPUT      : cdfid     - cdf file id
              invar     - variable subscripted by incoords
              incoords  - coordinates to be copied
              outvar    - variable subscripted by outcoords
@OUTPUT     : outcoords - new coordinates
@RETURNS    : pointer to outcoords or NULL if an error occurred.
@DESCRIPTION: Translates the coordinate vector used for subscripting one
              variable (invar) to a coordinate vector that can be used to
              subscript another (outvar). This is useful when two variables
              have similar dimensions, but not necessarily the same order of
              dimensions. If invar has a dimension that is not in outvar, then
              the corresponding coordinate is ignored. If outvar has a
              dimension that is not in invar, then the corresponding coordinate
              is not modified.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines.
@CREATED    : July 29, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public long *mitranslate_coords(int cdfid, 
                                int invar,  long incoords[],
                                int outvar, long outcoords[])
{
   int in_ndims, in_dim[MAX_VAR_DIMS], out_ndims, out_dim[MAX_VAR_DIMS];
   int i,j;

   MI_SAVE_ROUTINE_NAME("mitranslate_coords");

   /* Inquire about the dimensions of the two variables */
   if ( (ncvarinq(cdfid, invar,  NULL, NULL, &in_ndims,  in_dim,  NULL)
                    == MI_ERROR) ||
        (ncvarinq(cdfid, outvar, NULL, NULL, &out_ndims, out_dim, NULL)
                    == MI_ERROR)) {
      MI_RETURN_ERROR(NULL);
   }

   /* Loop through out_dim, looking for dimensions in in_dim */
   for (i=0; i<out_ndims; i++) {
      for (j=0; j<in_ndims; j++) {
         if (out_dim[i]==in_dim[j]) break;
      }
      /* If we found the dimension, then copy it */
      if (j<in_ndims) {
         outcoords[i] = incoords[j];
      }
   }

   MI_RETURN(outcoords);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : micopy_all_atts
@INPUT      : incdfid  - input cdf file id
              invarid  - input variable id
              outcdfid - output cdf file id
              outvarid - output variable id
@OUTPUT     : (none)
@RETURNS    : MI_ERROR (=-1) if an error occurs
@DESCRIPTION: Copies all of the attributes of one variable to another.
              Attributes that already exist in outvarid are not copied.
              outcdfid must be in define mode.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines
@CREATED    : 
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int micopy_all_atts(int incdfid, int invarid, 
                           int outcdfid, int outvarid)
{
   int num_atts;             /* Number of attributes */
   char name[MAX_NC_NAME];   /* Name of attribute */
   int oldncopts;            /* Old value of ncopts */
   int status;               /* Status returned by function call */
   int i;

   MI_SAVE_ROUTINE_NAME("micopy_all_atts");

   /* Inquire about the number of input variable attributes */
   MI_CHK_ERR(ncvarinq(incdfid, invarid, NULL, NULL, NULL, NULL, &num_atts))

   /* Loop through input attributes */
   for (i=0; i<num_atts; i++){
      
      /* Get the attribute name */
      MI_CHK_ERR(ncattname(incdfid, invarid, i, name))

      /* Check to see if it is in the output file. We must set and reset
         ncopts to avoid surprises to the calling program. (This is no
         longer needed with new MI_SAVE_ROUTINE_NAME macro). */
      oldncopts=ncopts; ncopts=0;
      status=ncattinq(outcdfid, outvarid, name, NULL, NULL);
      ncopts=oldncopts;

      /* If the attribute does not exist, copy it */
      if (status == MI_ERROR) {
         MI_CHK_ERR(ncattcopy(incdfid, invarid, name, outcdfid, outvarid))
      }
   }

   MI_RETURN(MI_NOERROR);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : micopy_var_def
@INPUT      : incdfid  - input cdf file id
              invarid  - input variable id
              outcdfid - output cdf file id
@OUTPUT     : (none)
@RETURNS    : Variable id of variable created in outcdfid or MI_ERROR (=-1)
              if an error occurs.
@DESCRIPTION: Copies a variable definition (including attributes) from one 
              cdf file to another. outcdfid must be in define mode.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines.
@CREATED    : 
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int micopy_var_def(int incdfid, int invarid, int outcdfid)
{
   char varname[MAX_NC_NAME]; /* Name of variable */
   char dimname[MAX_NC_NAME]; /* Name of dimension */
   nc_type datatype;          /* Type of variable */
   int ndims;                 /* Number of dimensions of variable */
   int indim[MAX_VAR_DIMS];   /* Dimensions of input variable */
   int outdim[MAX_VAR_DIMS];  /* Dimensions of output variable */
   long insize, outsize;      /* Size of each dimension */
   int recdim;                /* Dimension that is unlimited for input file */
   int outvarid;              /* Id of newly created variable */
   int oldncopts;             /* Store ncopts */
   int i;

   MI_SAVE_ROUTINE_NAME("micopy_var_def");

   /* Get name and dimensions of variable */
   MI_CHK_ERR(ncvarinq(incdfid, invarid, varname, &datatype, 
                       &ndims, indim, NULL))

   /* Get unlimited dimension for input file */
   MI_CHK_ERR(ncinquire(incdfid, NULL, NULL, NULL, &recdim))

   /* Create any new dimensions that need creating */
   for (i=0; i<ndims; i++) {

      /* Get the dimension info */
      MI_CHK_ERR(ncdiminq(incdfid, indim[i], dimname, &insize))

      /* If it exists in the output file, check the size. */
      oldncopts=ncopts; ncopts=0;
      outdim[i]=ncdimid(outcdfid, dimname);
      ncopts=oldncopts;
      if (outdim[i]!=MI_ERROR) {
         if ( (ncdiminq(outcdfid, outdim[i], NULL, &outsize)==MI_ERROR) ||
             ((insize!=0) && (outsize!=0) && (insize != outsize)) ) {
            if ((insize!=0) && (outsize!=0) && (insize != outsize))
               MI_LOG_PKG_ERROR2(MI_ERR_DIMSIZE, 
                  "Variable already has dimension of different size");
            MI_RETURN_ERROR(MI_ERROR);
         }
      }
      /* Otherwise create it */
      else {
         /* If the dimension is unlimited then try to create it unlimited
            in the output file */
         if (indim[i]==recdim) {
            oldncopts=ncopts; ncopts=0;
            outdim[i]=ncdimdef(outcdfid, dimname, NC_UNLIMITED);
            ncopts=oldncopts;
         }
         /* If it's not meant to be unlimited, or if we cannot create it
            unlimited, then create it with the current size */
         if ((indim[i]!=recdim) || (outdim[i]==MI_ERROR)) {
            MI_CHK_ERR(outdim[i]=ncdimdef(outcdfid, dimname, MAX(1,insize)))
         }
      }
   }

   /* Create a new variable */
   MI_CHK_ERR(outvarid=ncvardef(outcdfid, varname, datatype, ndims, outdim))

   /* Copy all the attributes */
   MI_CHK_ERR(micopy_all_atts(incdfid, invarid, outcdfid, outvarid))

   MI_RETURN(outvarid);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : micopy_var_values
@INPUT      : incdfid  - input cdf file id
              invarid  - input variable id
              outcdfid - output cdf file id
              outvarid - output variable id (usually returned by 
                 micopy_var_def)
@OUTPUT     : (none)
@RETURNS    : MI_ERROR (=-1) if an error occurs.
@DESCRIPTION: Copies the values of a variable from one cdf file to another.
              outcdfid must be in data mode. The two variables must have
              the same shape.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines.
@CREATED    : 
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int micopy_var_values(int incdfid, int invarid, 
                             int outcdfid, int outvarid)
{
   nc_type intype, outtype;   /* Data types */
   int inndims, outndims;     /* Number of dimensions */
   long insize[MAX_VAR_DIMS], outsize; /* Dimension sizes */
   long start[MAX_VAR_DIMS];  /* First coordinate of variable */
   int indim[MAX_VAR_DIMS];   /* Input dimensions */
   int outdim[MAX_VAR_DIMS];  /* Output dimensions */
   mi_vcopy_type stc;
   int i;

   MI_SAVE_ROUTINE_NAME("micopy_var_values");

   /* Get the dimensions of the two variables and check for compatibility */
   if ((ncvarinq(incdfid, invarid, NULL, &intype, &inndims, indim, NULL)
                     == MI_ERROR) ||
       (ncvarinq(outcdfid, outvarid, NULL, &outtype, &outndims, outdim, NULL)
                     == MI_ERROR) ||
       (intype != outtype) || (inndims != outndims)) {
      MI_RETURN_ERROR(MI_ERROR);
   }

   /* Get the dimension sizes and check for compatibility */
   for (i=0; i<inndims; i++) {
      if ((ncdiminq(incdfid, indim[i], NULL, &insize[i]) == MI_ERROR) ||
          (ncdiminq(outcdfid, outdim[i], NULL, &outsize) == MI_ERROR) ||
          ((insize[i]!=0) && (outsize!=0) && (insize[i] != outsize))) {
         if ((insize[i]!=0) && (outsize!=0) && (insize[i] != outsize))
            MI_LOG_PKG_ERROR2(MI_ERR_DIMSIZE, 
               "Variables have dimensions of different size");
         MI_RETURN_ERROR(MI_ERROR);
      }
   }

   /* Copy the values */
   stc.incdfid =incdfid;
   stc.outcdfid=outcdfid;
   stc.invarid =invarid;
   stc.outvarid=outvarid;
   stc.value_size=nctypelen(intype);
   MI_CHK_ERR(MI_var_loop(inndims, miset_coords(MAX_VAR_DIMS, 0L, start),
                          insize, stc.value_size, NULL, 
                          MI_MAX_VAR_BUFFER_SIZE, &stc,
                          MI_vcopy_action))
   MI_RETURN(MI_NOERROR);

}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : MI_vcopy_action
@INPUT      : ndims       - number of dimensions
              var_start   - coordinate vector of corner of hyperslab
              var_count   - vector of edge lengths of hyperslab
              nvalues     - number of values in hyperslab
              var_buffer  - pointer to variable buffer
              caller_data - pointer to data from micopy_var_values
@OUTPUT     : (none)
@RETURNS    : MI_ERROR if an error occurs
@DESCRIPTION: Buffer action routine to be called by MI_var_loop, for
              use by micopy_var_values.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF and MINC routines
@CREATED    : August 3, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int MI_vcopy_action(int ndims, long start[], long count[], 
                            long nvalues, void *var_buffer, void *caller_data)
{         /* ARGSUSED */
   mi_vcopy_type *ptr;       /* Pointer to data from micopy_var_values */

   MI_SAVE_ROUTINE_NAME("MI_vcopy_action");

   ptr=(mi_vcopy_type *) caller_data;

   /* Get values from input variable */
   MI_CHK_ERR(ncvarget(ptr->incdfid, ptr->invarid, start, count, var_buffer))

   /* Put values to output variable */
   MI_CHK_ERR(ncvarput(ptr->outcdfid, ptr->outvarid, start, count, 
                       var_buffer))
   MI_RETURN(MI_NOERROR);

}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : micopy_all_var_defs
@INPUT      : incdfid       - input cdf file id
              outcdfid      - output cdf file id
              nexclude      - number of values in array excluded_vars
              excluded_vars - array of variable id's in incdfid that should
                 not be copied
@OUTPUT     : (none)
@RETURNS    : MI_ERROR (=-1) if an error occurs.
@DESCRIPTION: Copies all variable definitions in file incdfid to file outcdfid
              (including attributes), excluding the variable id's listed in
              array excluded_vars. File outcdfid must be in define mode.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines.
@CREATED    : August 3, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int micopy_all_var_defs(int incdfid, int outcdfid, int nexclude,
                               int excluded_vars[])
{
   int num_vars;          /* Number of variables in input file */
   int varid;             /* Variable id counter */
   int i;

   MI_SAVE_ROUTINE_NAME("micopy_all_var_defs");

   /* Find out how many variables there are in the file and loop through
      them */
   MI_CHK_ERR(ncinquire(incdfid, NULL, &num_vars, NULL, NULL))

   /* Loop through variables, copying them */
   for (varid=0; varid<num_vars; varid++) {

      /* Check list of excluded variables */
      for (i=0; i<nexclude; i++) {
         if (varid==excluded_vars[i]) break;
      }

      /* If the variable is not excluded, copy its definition */
      if (i>=nexclude) {
         MI_CHK_ERR(micopy_var_def(incdfid, varid, outcdfid))
      }
   }

   MI_RETURN(MI_NOERROR);
       
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : micopy_all_var_values
@INPUT      : incdfid       - input cdf file id
              outcdfid      - output cdf file id
              nexclude      - number of values in array excluded_vars
              excluded_vars - array of variable id's in incdfid that should
                 not be copied
@OUTPUT     : (none)
@RETURNS    : MI_ERROR (=-1) if an error occurs.
@DESCRIPTION: Copies all variable values in file incdfid to file outcdfid,
              excluding the variable id's listed in array excluded_vars. 
              File outcdfid must be in data mode. Usually called after
              micopy_all_var_defs with the same arguments. If a variable
              to be copied is not defined properly in outcdfid, then an
              error occurs.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines.
@CREATED    : 
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int micopy_all_var_values(int incdfid, int outcdfid, int nexclude,
                                 int excluded_vars[])
{
   int num_vars;           /* Number of variables in input file */
   int varid;              /* Variable id counter */
   int outvarid;           /* Output variable id */
   char name[MAX_NC_NAME]; /*Variable name */
   int i;

   MI_SAVE_ROUTINE_NAME("micopy_all_var_values");

   /* Find out how many variables there are in the file and loop through
      them */
   MI_CHK_ERR(ncinquire(incdfid, NULL, &num_vars, NULL, NULL))

   /* Loop through variables, copying them */
   for (varid=0; varid<num_vars; varid++) {

      /* Check list of excluded variables */
      for (i=0; i<nexclude; i++) {
         if (varid==excluded_vars[i]) break;
      }

      /* If the variable is not excluded, copy its values */
      if (i>=nexclude) {
         /* Get the input variable's name */
         MI_CHK_ERR(ncvarinq(incdfid, varid, name, NULL, NULL, NULL, NULL))
         /* Look for it in the output file */
         MI_CHK_ERR(outvarid=ncvarid(outcdfid, name))
         /* Copy the values */
         MI_CHK_ERR(micopy_var_values(incdfid, varid, outcdfid, outvarid))
      }
   }

   MI_RETURN(MI_NOERROR);
       
}
