#ifndef  MINC_ROUTINES_HEADER_FILE
#define  MINC_ROUTINES_HEADER_FILE

/* ----------------------------- MNI Header -----------------------------------
@NAME       : minc_routines.h
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Provides prototypes for private and semiprivate MINC routines.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 28, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */

/* MINC routines that should only be visible to the package (semiprivate) */

/* From minc_error.c */
semiprivate int MI_save_routine_name(char *name);
semiprivate int MI_return();
semiprivate int MI_return_error();
semiprivate void MI_log_pkg_error2(int p1, char *p2);
semiprivate void MI_log_pkg_error3(int p1, char *p2, char *p3);
semiprivate void MI_log_sys_error1(char *p1);

/* From value_conversion.c */
semiprivate int MI_varaccess(int operation, int cdfid, int varid, 
                             long start[], long count[],
                             nc_type datatype, int sign, void *values,
                             int *bufsize_step, mi_icv_type *icvp);
private int MI_var_action(int ndims, long var_start[], long var_count[], 
                          long nvalues, void *var_buffer, void *caller_data);
semiprivate int MI_var_loop(int ndims, long start[], long count[],
                            int value_size, int *bufsize_step,
                            long max_buffer_size,
                            void *caller_data,
                            int (*action_func) (int, long [], long [], 
                                                long, void *, void *));
semiprivate int MI_get_sign_from_string(nc_type datatype, char *sign);
private int MI_get_sign(nc_type datatype, int sign);
semiprivate int MI_convert_type(long number_of_values,
                                nc_type intype,  int insign,  void *invalues,
                                nc_type outtype, int outsign, void *outvalues,
                                mi_icv_type *icvp);

/* From netcdf_convenience.c */
private int MI_vcopy_action(int ndims, long start[], long count[], 
                            long nvalues, void *var_buffer, void *caller_data);

/* From minc_convenience.c */
private int MI_create_dim_variable(int cdfid, char *name);
private int MI_create_dimwidth_variable(int cdfid, char *name);
private int MI_create_image_variable(int cdfid, char *name, nc_type datatype,
                                     int ndims, int dim[]);
private int MI_create_imaxmin_variable(int cdfid, char *name, nc_type datatype,
                                       int ndims, int dim[]);
private int MI_verify_maxmin_dims(int cdfid,
                                  int image_ndims,  int image_dim[],
                                  int maxmin_ndims, int maxmin_dim[]);
private int MI_create_root_variable(int cdfid, char *name);
private int MI_create_simple_variable(int cdfid, char *name);
private int MI_add_stdgroup(int cdfid, int varid);
private int MI_is_in_list(char *string, char *list[]);

/* From image_conversion.c */
private int MI_icv_get_type(mi_icv_type *icvp, int cdfid, int varid);
private int MI_icv_get_vrange(mi_icv_type *icvp, int cdfid, int varid);
private double MI_get_default_range(char *what, nc_type datatype, int sign);
private int MI_icv_get_norm(mi_icv_type *icvp, int cdfid, int varid);
private int MI_icv_get_dim(mi_icv_type *icvp, int cdfid, int varid);
private int MI_get_dim_flip(mi_icv_type *icvp, int cdfid, int dimvid[], 
                           int subsc[]);
private int MI_get_dim_scale(mi_icv_type *icvp, int cdfid, int dimvid[]);
private int MI_get_dim_bufsize_step(mi_icv_type *icvp, int subsc[]);
private int MI_icv_get_dim_conversion(mi_icv_type *icvp, int subsc[]);
private int MI_icv_access(int operation, mi_icv_type *icvp, long start[], 
                          long count[], void *values);
private int MI_icv_coords_tovar(mi_icv_type *icvp, 
                                long *icv_start, long *icv_count,
                                long *var_start, long *var_count);
private int MI_icv_calc_scale(int operation, mi_icv_type *icvp, long coords[]);
private int MI_icv_dimconvert(int operation, mi_icv_type *icvp,
                              long start[], long count[], void *values,
                              long bufstart[], long bufcount[], void *buffer);
private int MI_icv_dimconv_init(int operation, mi_icv_type *icvp,
                              mi_icv_dimconv_type *dcp,
                              long start[], long count[], void *values,
                              long bufstart[], long bufcount[], void *buffer);
private int MI_icv_chkid(int icvid);


#endif
