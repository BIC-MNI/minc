/* ----------------------------- MNI Header -----------------------------------
@NAME       : minc_convenience.c
@DESCRIPTION: File of convenience functions following the minc standard.
@METHOD     : Routines included in this file :
              public :
                 miattput_pointer
                 miattget_pointer
                 miadd_child
                 micreate_std_variable
                 micreate_group_variable
              private :
                 MI_create_dim_variable
                 MI_create_dimwidth_variable
                 MI_create_image_variable
                 MI_create_imaxmin_variable
                 MI_create_root_variable
                 MI_create_patient_variable
                 MI_create_study_variable
                 MI_create_acq_variable
                 MI_add_stdgroup
                 MI_is_in_list
@CREATED    : July 27, 1992. (Peter Neelin, Montreal Neurological Institute)
@MODIFIED   : 
---------------------------------------------------------------------------- */

#include <minc_private.h>
#include <minc_varlists.h>


/* ----------------------------- MNI Header -----------------------------------
@NAME       : miattput_pointer
@INPUT      : cdfid    - cdf file id
              varid    - variable id
              name     - name of attribute to point to variable
              ptrvarid - variable id of existing variable to which name 
                 should point
@OUTPUT     : (none)
@RETURNS    : MI_ERROR when an error occurs
@DESCRIPTION: Creates an variable attribute which points to another variable
              (generally a multi-dimensional attribute that must be stored
              as a variable). The variable to which the attribute points must
              already exist in the file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF and MINC routines.
@CREATED    : August 5, 1992
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int miattput_pointer(int cdfid, int varid, char *name, int ptrvarid)
{
   /* String to hold pointer to variable */
   char pointer_string[MAX_NC_NAME+sizeof(MI_VARATT_POINTER_PREFIX)];
   int index;           /* Index into string */

   MI_SAVE_ROUTINE_NAME("miattput_pointer");

   /* Set the first part of the string */
   index=strlen(strcpy(pointer_string,MI_VARATT_POINTER_PREFIX));

   /* Get the name of the variable to which we should point */
   MI_CHK_ERR(ncvarinq(cdfid, ptrvarid, &(pointer_string[index]), NULL,
                       NULL, NULL, NULL))

   /* Set the attribute of the parent */
   MI_CHK_ERR(miattputstr(cdfid, varid, name, pointer_string))

   /* Get the name of the parent variable */
   MI_CHK_ERR(ncvarinq(cdfid, varid, pointer_string, NULL,
                       NULL, NULL, NULL))

   /* Set the attribute of the variable to which we point */
   MI_CHK_ERR(miattputstr(cdfid, ptrvarid, MIparent, pointer_string))

   /* Set the MIvartype attribute for ptrvarid */
   MI_CHK_ERR(miattputstr(cdfid, ptrvarid, MIvartype, MI_VARATT))

   MI_RETURN(MI_NOERROR);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : miattget_pointer
@INPUT      : cdfid - cdf file id
              varid - variable id
              name  - attribute name that should contain a pointer to
                 a variable
@OUTPUT     : (none)
@RETURNS    : variable id pointed to by name, MI_ERROR if an error occurs.
@DESCRIPTION: 
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF and MINC routines
@CREATED    : August 5, 1992
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int miattget_pointer(int cdfid, int varid, char *name)
{
   /* Character string to hold attribute */
   char pointer_string[MAX_NC_NAME+sizeof(MI_VARATT_POINTER_PREFIX)];
   int index;           /* Index into string */
   char *prefix_string=MI_VARATT_POINTER_PREFIX;  /* Prefix string */
   int ptrvarid;        /* Id of variable pointed to by name */

   MI_SAVE_ROUTINE_NAME("miattget_pointer");

   /* Get the attribute */
   if (miattgetstr(cdfid, varid, name, sizeof(pointer_string), 
                   pointer_string) == NULL)
      MI_RETURN_ERROR(MI_ERROR);

   /* Check for the prefix */
   for (index=0; prefix_string[index]!='\0'; index++) {
      if (pointer_string[index]!=prefix_string[index]) {
         MI_LOG_PKG_ERROR3(MI_ERR_NOTPOINTER,
                           "Attribute %s is not a pointer to a variable",
                           name);
         MI_RETURN_ERROR(MI_ERROR);
      }
   }

   /* Get the variable id */
   MI_CHK_ERR(ptrvarid=ncvarid(cdfid, pointer_string[index]))

   MI_RETURN(ptrvarid);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : miadd_child
@INPUT      : cdfid        - cdf file id
              parent_varid - variable id of parent variable
              child_varid  - variable id of child variable
@OUTPUT     : (none)
@RETURNS    : MI_ERROR if an error occurs.
@DESCRIPTION: Adds the name of child_varid to the children attribute of
              parent_varid and sets the parent attribute of child_varid
              to the name of parent_varid.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF and MINC routines
@CREATED    : August 5, 1992
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int miadd_child(int cdfid, int parent_varid, int child_varid)
{
   char *child_list;           /* Pointer to string list of children */
   int child_list_size;        /* Length of child list string */
   int oldncopts;              /* To set and reset ncopts */
   nc_type datatype;           /* Type of attribute */
   int status;                 /* Status of function call */

   MI_SAVE_ROUTINE_NAME("miadd_child");

   /* Get the size of the child list in the parent. Handle the case where the
      child list does not exist. */
   oldncopts=ncopts; ncopts=0;
   status=ncattinq(cdfid, parent_varid, MIchildren, &datatype, 
                   &child_list_size);
   ncopts=oldncopts;
   if ((status == MI_ERROR) || (datatype != NC_CHAR)) 
      child_list_size=0;

   /* Allocate space for new child list */
   if ((child_list = MALLOC(child_list_size+MAX_NC_NAME+
                            strlen(MI_CHILD_SEPARATOR), char)) == NULL) {
      MI_LOG_SYS_ERROR1("miadd_child");
      MI_RETURN_ERROR(MI_ERROR);
   }

   /* Get the old list if needed and check for terminating null character 
      (child_list_size should point to the next spot in child_list, 
      overwriting any null character) */
   if (child_list_size>0) {
      if (ncattget(cdfid, parent_varid, MIchildren, child_list) == MI_ERROR) {
         FREE(child_list);
         MI_RETURN_ERROR(MI_ERROR);
      }
      if (child_list[child_list_size-1] == '\0')
         child_list_size--;

      /* Copy the child list element separator (only if there are other
         elements in the list */
      (void) strcpy(&child_list[child_list_size], MI_CHILD_SEPARATOR);
      child_list_size += strlen(MI_CHILD_SEPARATOR);
   }

   /* Add the new child name to the list */
   if (ncvarinq(cdfid, child_varid, &child_list[child_list_size], NULL,
                NULL, NULL, NULL) == MI_ERROR) {
      FREE(child_list);
      MI_RETURN_ERROR(MI_ERROR);
   }

   /* Put the attribute MIchildren */
   if (miattputstr(cdfid, parent_varid, MIchildren, child_list) == MI_ERROR) {
      FREE(child_list);
      MI_RETURN_ERROR(MI_ERROR);
   }

   /* Get the parent variable name */
   if (ncvarinq(cdfid, parent_varid, child_list, NULL, NULL, NULL, NULL)
                       == MI_ERROR) {
      FREE(child_list);
      MI_RETURN_ERROR(MI_ERROR);
   }

   /* Put the attribute MIparent */
   if (miattputstr(cdfid, child_varid, MIparent, child_list) == MI_ERROR) {
      FREE(child_list);
      MI_RETURN_ERROR(MI_ERROR);
   }

   FREE(child_list);
   MI_RETURN(MI_NOERROR);

}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : micreate_std_variable
@INPUT      : cdfid    - cdf file id
              name     - name of standard variable to create
              datatype - type of data to store (see ncvardef)
              ndims    - number of dimensions of variable (see ncvardef)
              dim      - vector of variable dimensions (see ncvardef)
@OUTPUT     : (none)
@RETURNS    : id of created variable, or MI_ERROR if an error occurs
@DESCRIPTION: Creates a standard MINC variable by calling ncvardef
              and then sets default attributes. The standard variables are 
              identified by name, so an unrecognised name produces an error.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF and MINC routines
@CREATED    : August 5, 1992
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int micreate_std_variable(int cdfid, char *name, nc_type datatype, 
                                 int ndims, int dim[])
{
   int varid;                /* Created variable id */

   MI_SAVE_ROUTINE_NAME("micreate_std_variable");

   /* Check to see if it is a standard dimension */
   if (MI_is_in_list(name, dimvarlist)) {
      MI_CHK_ERR(varid=MI_create_dim_variable(cdfid, name))
   }

   /* Check for a dimension width */
   else if (MI_is_in_list(name, dimwidthlist)) {
      MI_CHK_ERR(varid=MI_create_dimwidth_variable(cdfid, name))
   }

   /* Check for a standard variable or group */
   else if (MI_is_in_list(name, varlist)) {
      if (STRINGS_EQUAL(name, MIimage))
         MI_CHK_ERR(varid=MI_create_image_variable(cdfid, name, datatype,
                                                   ndims, dim))
      else if ((STRINGS_EQUAL(name, MIimagemax)) ||
               (STRINGS_EQUAL(name, MIimagemin)))
         MI_CHK_ERR(varid=MI_create_imaxmin_variable(cdfid, name, datatype,
                                                     ndims, dim))
      else if (STRINGS_EQUAL(name, MIrootvariable))
         MI_CHK_ERR(varid=MI_create_root_variable(cdfid, name))
      else if (STRINGS_EQUAL(name, MIpatient))
         MI_CHK_ERR(varid=MI_create_simple_variable(cdfid, name))
      else if (STRINGS_EQUAL(name, MIstudy))
         MI_CHK_ERR(varid=MI_create_simple_variable(cdfid, name))
      else if (STRINGS_EQUAL(name, MIacquisition))
         MI_CHK_ERR(varid=MI_create_simple_variable(cdfid, name))
      else {
         MI_LOG_PKG_ERROR3(MI_ERR_BAD_STDVAR, 
                           "%s is not recognised as a standard MINC variable",
                           name);
         MI_RETURN_ERROR(MI_ERROR);
      }
   }

   /* If not in any list, then return an error */
   else {
      MI_LOG_PKG_ERROR3(MI_ERR_BAD_STDVAR,
                        "%s is not a standard MINC variable", name);
      MI_RETURN_ERROR(MI_ERROR);
   }

   MI_RETURN(varid);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : MI_create_dim_variable
@INPUT      : cdfid    - cdf file id
              name     - name of standard variable to create
@OUTPUT     : (none)
@RETURNS    : id of created variable, or MI_ERROR if an error occurs
@DESCRIPTION: Creates a standard MINC dimension variable by calling ncvardef
              and then sets default attributes. The standard variables are 
              identified by name, so an unrecognised name produces an error.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF and MINC routines
@CREATED    : August 5, 1992
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int MI_create_dim_variable(int cdfid, char *name)
{
   int dimid;                /* Dimension id (for dimensions variables) */
   int varid;                /* Created variable id */
   /* Vector of direction cosines */
   double dircos[MI_NUM_SPACE_DIMS];

   MI_SAVE_ROUTINE_NAME("MI_create_dim_variable");

   /* Check for MIvector_dimension - no associated variable */
   if (STRINGS_EQUAL(name, MIvector_dimension)) {
      MI_LOG_PKG_ERROR3(MI_ERR_BAD_STDVAR,
                        "%s is not a standard MINC variable", name);
      MI_RETURN_ERROR(MI_ERROR);
   }

   /* Look for dimension and create the variable */
   MI_CHK_ERR(dimid=ncdimid(cdfid, name))
   MI_CHK_ERR(varid=ncvardef(cdfid, name, NC_DOUBLE, 1, &dimid))

   /* Standard attributes */
   MI_CHK_ERR(miattputstr(cdfid, varid, MIvarid, MI_STDVAR))
   MI_CHK_ERR(miattputstr(cdfid, varid, MIvartype, MI_DIMENSION))
   MI_CHK_ERR(miattputstr(cdfid, varid, MIversion, MI_CURRENT_VERSION))

   /* Dimension attributes */
   dircos[0] = dircos[1] = dircos[2] = 0.0;
   MI_CHK_ERR(miattputstr(cdfid, varid, MIspacing, MI_REGULAR))
   MI_CHK_ERR(miattputdbl(cdfid, varid, MIstep, 1.0))
   MI_CHK_ERR(miattputdbl(cdfid, varid, MIstart, 0.0))
   MI_CHK_ERR(miattputstr(cdfid, varid, MIspacetype, MI_NATIVE))
   if (STRINGS_EQUAL(name, MItime))
      MI_CHK_ERR(miattputstr(cdfid, varid, MIalignment, MI_START))
   else
      MI_CHK_ERR(miattputstr(cdfid, varid, MIalignment, MI_CENTRE))

   /* Direction cosines */
   if ((STRINGS_EQUAL(name, MIxspace)) || 
       (STRINGS_EQUAL(name, MIxfrequency))) {
      dircos[0]=1.0;
      MI_CHK_ERR(ncattput(cdfid, varid, MIdirection_cosines, NC_DOUBLE,
                          3, dircos))
   }
   else if ((STRINGS_EQUAL(name, MIyspace)) || 
           (STRINGS_EQUAL(name, MIyfrequency))) {
      dircos[1]=1.0;
      MI_CHK_ERR(ncattput(cdfid, varid, MIdirection_cosines, NC_DOUBLE,
                          3, dircos))
   }
   else if ((STRINGS_EQUAL(name, MIzspace)) || 
            (STRINGS_EQUAL(name, MIzfrequency))) {
      dircos[1]=1.0;
      MI_CHK_ERR(ncattput(cdfid, varid, MIdirection_cosines, NC_DOUBLE,
                          3, dircos))
   }

   MI_RETURN(varid);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : MI_create_dimwidth_variable
@INPUT      : cdfid    - cdf file id
              name     - name of standard variable to create
@OUTPUT     : (none)
@RETURNS    : id of created variable, or MI_ERROR if an error occurs
@DESCRIPTION: Creates a standard MINC dimension width variable by calling 
              ncvardef and then sets default attributes. The standard 
              variables are identified by name, so an unrecognised name 
              produces an error.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF and MINC routines
@CREATED    : August 5, 1992
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int MI_create_dimwidth_variable(int cdfid, char *name)
{
   int dimid;                /* Dimension id (for dimensions variables) */
   int varid;                /* Created variable id */
   char string[MAX_NC_NAME]; /* String for dimension name */
   char *str;

   MI_SAVE_ROUTINE_NAME("MI_create_dimwidth_variable");

   /* Look for dimension name in name (remove width suffix) */
   if ((str=strstr(strcpy(string, name),MI_WIDTH_SUFFIX)) == NULL) {
      MI_LOG_PKG_ERROR2(MI_ERR_BADSUFFIX,"Bad dimension width suffix");
      MI_RETURN_ERROR(MI_ERROR);
   }
   *str='\0';

   /* Look for the dimension */
   MI_CHK_ERR(dimid=ncdimid(cdfid, string))
   /* Create the variable and set defaults */
   MI_CHK_ERR(varid=ncvardef(cdfid, name, NC_DOUBLE, 1, &dimid))
   MI_CHK_ERR(miattputstr(cdfid, varid, MIvarid, MI_STDVAR))
   MI_CHK_ERR(miattputstr(cdfid, varid, MIvartype, MI_DIM_WIDTH))
   MI_CHK_ERR(miattputstr(cdfid, varid, MIversion, MI_CURRENT_VERSION))
   MI_CHK_ERR(miattputstr(cdfid, varid, MIspacing, MI_REGULAR))
   MI_CHK_ERR(miattputdbl(cdfid, varid, MIwidth, 1.0))
   MI_CHK_ERR(miattputstr(cdfid, varid, MIfiltertype, MI_SQUARE))

   MI_RETURN(varid);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : MI_create_image_variable
@INPUT      : cdfid    - cdf file id
              name     - name of standard variable to create
              datatype - type of data to store (see ncvardef)
              ndims    - number of dimensions of variable (see ncvardef)
              dim      - vector of variable dimensions (see ncvardef)
@OUTPUT     : (none)
@RETURNS    : id of created variable, or MI_ERROR if an error occurs
@DESCRIPTION: Creates a standard MINC image variable by calling ncvardef
              and then sets default attributes.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF and MINC routines
@CREATED    : August 6, 1992
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int MI_create_image_variable(int cdfid, char *name, nc_type datatype,
                                     int ndims, int dim[])
{
   int varid;                /* Created variable id */
   int max_varid;            /* Variable id for dimensional attribute */
   int min_varid;            /* Variable id for dimensional attribute */
   int maxmin_ndims;         /* Number of dimensions in max/min variable */
   int maxmin_dim[MAX_VAR_DIMS];  /* Dimensions of max/min variable */
   int oldncopts;            /* For saving and restoring ncopts */

   MI_SAVE_ROUTINE_NAME("MI_create_image_variable");

   /* Look to see if MIimagemax or MIimagemin exist for dimension checking 
      and pointers */
   oldncopts=ncopts; ncopts=0;
   max_varid=ncvarid(cdfid, MIimagemax);
   min_varid=ncvarid(cdfid, MIimagemin);
   ncopts=oldncopts;
   if (max_varid != MI_ERROR) {
      /* Get MIimagemax dimensions */
      MI_CHK_ERR(ncvarinq(cdfid, max_varid, NULL, NULL, &maxmin_ndims,
                          maxmin_dim, NULL))
      MI_CHK_ERR(MI_verify_maxmin_dims(cdfid, ndims, dim, 
                                       maxmin_ndims, maxmin_dim))
   }
   if (min_varid != MI_ERROR) {
      /* Get MIimagemin dimensions */
      MI_CHK_ERR(ncvarinq(cdfid, min_varid, NULL, NULL, &maxmin_ndims,
                          maxmin_dim, NULL))
      MI_CHK_ERR(MI_verify_maxmin_dims(cdfid, ndims, dim, 
                                       maxmin_ndims, maxmin_dim))
   }

   /* Create the variable */
   MI_CHK_ERR(varid=ncvardef(cdfid, name, datatype, ndims, dim))

   /* Standard attributes */
   MI_CHK_ERR(MI_add_stdgroup(cdfid, varid))

   /* Create pointers to MIimagemax and MIimagemin if they exist */
   if (max_varid!=MI_ERROR) 
      MI_CHK_ERR(miattput_pointer(cdfid, varid, MIimagemax, max_varid))
   if (min_varid!=MI_ERROR) 
      MI_CHK_ERR(miattput_pointer(cdfid, varid, MIimagemin, min_varid))

   MI_RETURN(varid);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : MI_create_imaxmin_variable
@INPUT      : cdfid    - cdf file id
              name     - name of standard variable to create
              datatype - type of data to store (see ncvardef)
              ndims    - number of dimensions of variable (see ncvardef)
              dim      - vector of variable dimensions (see ncvardef)
@OUTPUT     : (none)
@RETURNS    : id of created variable, or MI_ERROR if an error occurs
@DESCRIPTION: Creates a standard MINC image maximum or minimum dimensional
              attribute variable by calling ncvardef and then sets default 
              attributes. If MIimage exists, then dimensions are checked
              (MIimagemax and MIimagemin cannot vary over the first two
              dimensions of MIimage (or first three if the first is
              MIvector_dimension)), and a pointer attribute is added to
              MIimage.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF and MINC routines
@CREATED    : August 6, 1992
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int MI_create_imaxmin_variable(int cdfid, char *name, nc_type datatype,
                                       int ndims, int dim[])
{
   int varid;                /* Created variable id */
   int image_varid;          /* Variable id for image */
   int image_ndims;          /* Number of image dimensions */
   int image_dim[MAX_VAR_DIMS]; /* Image dimensions */
   int oldncopts;            /* For saving and restoring ncopts */

   MI_SAVE_ROUTINE_NAME("MI_create_imaxmin_variable");

   /* Look to see if MIimage exists for dimension checking and pointers */
   oldncopts=ncopts; ncopts=0;
   image_varid=ncvarid(cdfid, MIimage);
   ncopts=oldncopts;
   if (image_varid != MI_ERROR) {
      /* Get image dimensions */
      MI_CHK_ERR(ncvarinq(cdfid, image_varid, NULL, NULL, &image_ndims,
                          image_dim, NULL))
      MI_CHK_ERR(MI_verify_maxmin_dims(cdfid, image_ndims, image_dim, 
                                       ndims, dim))
   }

   /* Create the variable */
   MI_CHK_ERR(varid=ncvardef(cdfid, name, datatype, ndims, dim))

   /* Standard attributes */
   MI_CHK_ERR(miattputstr(cdfid, varid, MIvarid, MI_STDVAR))
   MI_CHK_ERR(miattputstr(cdfid, varid, MIvartype, MI_VARATT))
   MI_CHK_ERR(miattputstr(cdfid, varid, MIversion, MI_CURRENT_VERSION))

   /* Create pointer from MIimage to max or min if MIimage exists */
   if (image_varid != MI_ERROR) 
      MI_CHK_ERR(miattput_pointer(cdfid, image_varid, name, varid))

   MI_RETURN(varid);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : MI_verify_maxmin_dims
@INPUT      : cdfid        - cdf file id
              image_ndims  - number of MIimage dimensions
              image_dim    - image dimensions
              maxmin_ndims - number of MIimagemax or MIimagemin dimensions
              maxmin_dim   - max/min dimensions
@OUTPUT     : (none)
@RETURNS    : MI_ERROR if dimensions don't agree
@DESCRIPTION: Verifies that MIimage dimensions and MIimagemax/MIimagemin
              dimensions agree. MIimagemax/MIimagemin cannot vary over the
              two fastest varying (last) dimensions of MIimage - three
              fastest dimensions if MIvector_dimension is the fastest varying
              dimension of MIimage (this maintains the image nature of MIimage
              and its dimensional attributes MIimagemax and MIimagemin).
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF and MINC routines
@CREATED    : August 7, 1992
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int MI_verify_maxmin_dims(int cdfid,
                                  int image_ndims,  int image_dim[],
                                  int maxmin_ndims, int maxmin_dim[])
{
   char dimname[MAX_NC_NAME];
   int i,j;
   int nbaddims = 2;         /* Number of dimension over which max/min
                                should not vary */

   MI_SAVE_ROUTINE_NAME("MI_verify_maxmin_dims");

   /* Check to see if last dimension is MIvectordimension */
   MI_CHK_ERR(ncdiminq(cdfid, image_dim[image_ndims-1], dimname, NULL))
   if (STRINGS_EQUAL(dimname, MIvector_dimension))
      nbaddims++;

   /* Loop through illegal image dimensions (last nbaddims) checking 
      dimensions against maxmin_dim */
   for (i=MAX(0,image_ndims-nbaddims); i<image_ndims; i++)
      for (j=0; j<maxmin_ndims; j++)
         MI_CHK_ERR((image_dim[i]==maxmin_dim[j]) ? MI_ERROR : MI_NOERROR)

   MI_RETURN(MI_NOERROR);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : MI_create_root_variable
@INPUT      : cdfid    - cdf file id
              name     - name of standard variable to create
@OUTPUT     : (none)
@RETURNS    : id of created variable, or MI_ERROR if an error occurs
@DESCRIPTION: Creates a standard MINC root variable by calling ncvardef
              and then sets default attributes.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF and MINC routines
@CREATED    : August 6, 1992
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int MI_create_root_variable(int cdfid, char *name)
{
   int varid;                /* Created variable id */

   MI_SAVE_ROUTINE_NAME("MI_create_root_variable");

   /* Create the variable */
   MI_CHK_ERR(varid=ncvardef(cdfid, name, NC_LONG, 0, NULL))

   /* Standard attributes */
   MI_CHK_ERR(miattputstr(cdfid, varid, MIvarid, MI_STDVAR))
   MI_CHK_ERR(miattputstr(cdfid, varid, MIvartype, MI_GROUP))
   MI_CHK_ERR(miattputstr(cdfid, varid, MIversion, MI_CURRENT_VERSION))

   /* Add empty parent pointer */
   MI_CHK_ERR(miattputstr(cdfid, varid, MIparent, MI_EMPTY_STRING))

   MI_RETURN(varid);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : MI_create_simple_variable
@INPUT      : cdfid    - cdf file id
              name     - name of standard variable to create
@OUTPUT     : (none)
@RETURNS    : id of created variable, or MI_ERROR if an error occurs
@DESCRIPTION: Creates a standard MINC variable by calling ncvardef
              and then sets default attributes (only standard ones)
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF and MINC routines
@CREATED    : August 6, 1992
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int MI_create_simple_variable(int cdfid, char *name)
{
   int varid;                /* Created variable id */

   MI_SAVE_ROUTINE_NAME("MI_create_simple_variable");

   /* Create the variable */
   MI_CHK_ERR(varid=ncvardef(cdfid, name, NC_LONG, 0, NULL))

   /* Standard attributes */
   MI_CHK_ERR(MI_add_stdgroup(cdfid, varid))

   MI_RETURN(varid);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : MI_add_stdgroup
@INPUT      : cdfid    - cdf file id
              varid    - id of variable
@OUTPUT     : (none)
@RETURNS    : MI_ERROR if an error occurs
@DESCRIPTION: Adds an MI standard variable to the MIchildren list of 
              MIrootvariable and sets some standard attributes. If 
              MIrootvariable does not exist, it is created.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF and MINC routines
@CREATED    : August 6, 1992
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int MI_add_stdgroup(int cdfid, int varid)
{
   int root_varid;          /* Id of root variable */
   int oldncopts;           /* Old value of ncopts */

   MI_SAVE_ROUTINE_NAME("MI_add_stdgroup");

   /* Check for root variable, and add it if it is not there */
   oldncopts=ncopts; ncopts=0;
   root_varid=ncvarid(cdfid, MIrootvariable);
   ncopts=oldncopts;
   if (root_varid==MI_ERROR) {
      MI_CHK_ERR(root_varid=MI_create_root_variable(cdfid, MIrootvariable))
   }

   /* Add group as child of root */
   MI_CHK_ERR(miadd_child(cdfid, root_varid, varid))

   /* Standard attributes */
   MI_CHK_ERR(miattputstr(cdfid, varid, MIvarid, MI_STDVAR))
   MI_CHK_ERR(miattputstr(cdfid, varid, MIvartype, MI_GROUP))
   MI_CHK_ERR(miattputstr(cdfid, varid, MIversion, MI_CURRENT_VERSION))

   MI_RETURN(MI_NOERROR);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : micreate_group_variable
@INPUT      : cdfid - cdf file id
              name  - name of standard variable to create
@OUTPUT     : (none)
@RETURNS    : id of created variable or MI_ERROR if an error occurs
@DESCRIPTION: Creates a standard MINC variable whose values and dimensions
              are unimportant by calling ncvardef and then sets default 
              attributes. The standard variables are identified by name, so
              an unrecognised name produces an error.
@METHOD     : 
@GLOBALS    : 
@CALLS      : NetCDF routines
@CREATED    : August 6, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int micreate_group_variable(int cdfid, char *name)
{
   int varid;

   MI_SAVE_ROUTINE_NAME("micreate_group_variable");

   MI_CHK_ERR(varid=micreate_std_variable(cdfid, name, NC_LONG, 0, NULL))

   MI_RETURN(varid);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : MI_is_in_list
@INPUT      : string    - string for which to look
              list      - list in which to look (must be NULL terminated)
@OUTPUT     : (none)
@RETURNS    : TRUE if found, FALSE if not
@DESCRIPTION: Searches a list of character strings for string and returns
              TRUE if the string is in the list.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 5, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private int MI_is_in_list(char *string, char *list[])
{
   int i;

   MI_SAVE_ROUTINE_NAME("MI_is_in_list");

   for (i=0; list[i] != NULL; i++) {
      if (STRINGS_EQUAL(string, list[i])) MI_RETURN(TRUE);
   }

   MI_RETURN(FALSE);
}

