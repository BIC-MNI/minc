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
@MODIFIED   : $Log: minc_routines.h,v $
@MODIFIED   : Revision 5.0  1997-08-21 13:25:53  neelin
@MODIFIED   : Release of minc version 0.5
@MODIFIED   :
 * Revision 4.0  1997/05/07  20:07:52  neelin
 * Release of minc version 0.4
 *
 * Revision 3.0  1995/05/15  19:33:12  neelin
 * Release of minc version 0.3
 *
 * Revision 2.2  1995/02/08  19:01:06  neelin
 * Moved private function declarations from minc_routines.h to appropriate file.
 *
 * Revision 2.1  1995/01/20  15:21:20  neelin
 * Added midecompress_file with ability to decompress only the header of a file.
 *
 * Revision 2.0  94/09/28  10:38:08  neelin
 * Release of minc version 0.2
 * 
 * Revision 1.9  94/09/28  10:37:29  neelin
 * Pre-release
 * 
 * Revision 1.8  93/08/11  12:06:41  neelin
 * Added RCS logging in source.
 * 
@COPYRIGHT  :
              Copyright 1993 Peter Neelin, McConnell Brain Imaging Centre, 
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
@RCSID      : $Header: /private-cvsroot/minc/libsrc/minc_routines.h,v 5.0 1997-08-21 13:25:53 neelin Rel $ MINC (MNI)
---------------------------------------------------------------------------- */

/* MINC routines that should only be visible to the package (semiprivate) */

/* From minc_error.c */
semiprivate int MI_save_routine_name(char *name);
semiprivate int MI_return(void);
semiprivate int MI_return_error(void);
semiprivate void MI_log_pkg_error2(int p1, char *p2);
semiprivate void MI_log_pkg_error3(int p1, char *p2, char *p3);
semiprivate void MI_log_sys_error1(char *p1);

/* From value_conversion.c */
semiprivate int MI_varaccess(int operation, int cdfid, int varid, 
                             long start[], long count[],
                             nc_type datatype, int sign, void *values,
                             int *bufsize_step, mi_icv_type *icvp);
semiprivate int MI_var_loop(int ndims, long start[], long count[],
                            int value_size, int *bufsize_step,
                            long max_buffer_size,
                            void *caller_data,
                            int (*action_func) (int, long [], long [], 
                                                long, void *, void *));
semiprivate int MI_get_sign_from_string(nc_type datatype, char *sign);
semiprivate int MI_convert_type(long number_of_values,
                                nc_type intype,  int insign,  void *invalues,
                                nc_type outtype, int outsign, void *outvalues,
                                mi_icv_type *icvp);

/* From image_conversion.c */
semiprivate mi_icv_type *MI_icv_chkid(int icvid);


#endif
