
#ifndef  MINC_BASIC_HEADER_FILE
#define  MINC_BASIC_HEADER_FILE

/* ----------------------------- MNI Header -----------------------------------
@NAME       : minc_basic.h
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Constants and macros for private use by MINC routines.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 28, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */

/* --------- MINC specific constants ------------- */

/* Maximum buffer size for conversions. Should not be a power of 2 - this
   can cause poor performance on some systems (e.g. SGI) due to caching-
   related inefficiencies */
#define MI_MAX_VAR_BUFFER_SIZE 10000

/* Possible values for sign of a value */
#define MI_PRIV_DEFSIGN   0
#define MI_PRIV_SIGNED    1
#define MI_PRIV_UNSIGNED  2

/* Operations for MI_varaccess */
#define MI_PRIV_GET 10
#define MI_PRIV_PUT 11

/* Number of image dimensions for image conversion */
#define MI_PRIV_IMGDIMS 2

/* Suffix for dimension width variable names */
#define MI_WIDTH_SUFFIX "-width"

/* NetCDF routine name variable (for error logging) */
extern char *cdf_routine_name ; /* defined in globdef.c */
#define MI_NC_ROUTINE_VAR cdf_routine_name

/* MINC routine name variable, call depth counter (for keeping track of
   minc routines calling minc routines) and variable for keeping track
   of callers ncopts. All of these are for error logging. They are
   defined in minc_globdef.c */
extern char *minc_routine_name;
extern int minc_call_depth;
extern int minc_callers_ncopts;
extern int minc_trash_var;        /* Just for getting rid of lint messages */
#define MI_ROUTINE_VAR minc_routine_name
#define MI_CALL_DEPTH minc_call_depth
#define MI_CALLERS_NCOPTS minc_callers_ncopts
#define MI_TRASH_VAR minc_trash_var

/* Macros for logging errors. All routines should start with MI_SAVE_ROUTINE
   and exit with MI_RETURN (which includes MI_RETURN_ERROR and 
   MI_CHK_ERROR). All the macros except MI_CHK_ERROR are single line
   commands. MI_CHK_ERROR is in a block and so should not be followed by
   a ';' */
#define MI_SAVE_ROUTINE_NAME(name) \
   (MI_TRASH_VAR = (((MI_CALL_DEPTH++)==0) ? \
       MI_save_routine_name(name) : MI_NOERROR))
#define MI_RETURN(value) \
   return( (((--MI_CALL_DEPTH)!=0) || MI_return()) ? (value) : (value))
#define MI_RETURN_ERROR(error) \
   return( (((--MI_CALL_DEPTH)!=0) || MI_return_error()) ? (error) : (error))
#define MI_LOG_PKG_ERROR2(p1,p2) MI_log_pkg_error2(p1, p2)
#define MI_LOG_PKG_ERROR3(p1,p2,p3) MI_log_pkg_error3(p1, p2, p3)
#define MI_LOG_SYS_ERROR1(p1) MI_log_sys_error1(p1)
#define MI_CHK_ERR(expr) {if ((expr)==MI_ERROR) MI_RETURN_ERROR(MI_ERROR);}

/* Macros for converting data types. These macros are compound statements, 
   so don't put a semi-colon after them. dvalue should be a double, type
   is an int NetCDF type, sign is one of MI_PRIV_UNSIGNED and 
   MI_PRIV_SIGNED and ptr is a void pointer */

#define MI_TO_DOUBLE(dvalue, type, sign, ptr) \
   switch (type) { \
   case NC_BYTE : \
      switch (sign) { \
      case MI_PRIV_UNSIGNED : \
         dvalue = (double) *((unsigned char *) ptr); break; \
      case MI_PRIV_SIGNED : \
         dvalue = (double) *((signed char *) ptr); break; \
      } \
      break; \
   case NC_SHORT : \
      switch (sign) { \
      case MI_PRIV_UNSIGNED : \
         dvalue = (double) *((unsigned short *) ptr); break; \
      case MI_PRIV_SIGNED : \
         dvalue = (double) *((signed short *) ptr); break; \
      } \
      break; \
   case NC_LONG : \
      switch (sign) { \
      case MI_PRIV_UNSIGNED : \
         dvalue = (double) *((unsigned long *) ptr); break; \
      case MI_PRIV_SIGNED : \
         dvalue = (double) *((signed long *) ptr); break; \
      } \
      break; \
   case NC_FLOAT : \
      dvalue = (double) *((float *) ptr); \
      break; \
   case NC_DOUBLE : \
      dvalue = (double) *((double *) ptr); \
      break; \
   } 

#define MI_FROM_DOUBLE(dvalue, type, sign, ptr) \
   switch (type) { \
   case NC_BYTE : \
      switch (sign) { \
      case MI_PRIV_UNSIGNED : \
         dvalue = MAX(0, dvalue); \
         dvalue = MIN(UCHAR_MAX, dvalue); \
         *((unsigned char *) ptr) = ROUND(dvalue); \
         break; \
      case MI_PRIV_SIGNED : \
         dvalue = MAX(SCHAR_MIN, dvalue); \
         dvalue = MIN(SCHAR_MAX, dvalue); \
         *((signed char *) ptr) = ROUND(dvalue); \
         break; \
      } \
      break; \
   case NC_SHORT : \
      switch (sign) { \
      case MI_PRIV_UNSIGNED : \
         dvalue = MAX(0, dvalue); \
         dvalue = MIN(USHRT_MAX, dvalue); \
         *((unsigned short *) ptr) = ROUND(dvalue); \
         break; \
      case MI_PRIV_SIGNED : \
         dvalue = MAX(SHRT_MIN, dvalue); \
         dvalue = MIN(SHRT_MAX, dvalue); \
         *((signed short *) ptr) = ROUND(dvalue); \
         break; \
      } \
      break; \
   case NC_LONG : \
      switch (sign) { \
      case MI_PRIV_UNSIGNED : \
         dvalue = MAX(0, dvalue); \
         dvalue = MIN(ULONG_MAX, dvalue); \
         *((unsigned long *) ptr) = ROUND(dvalue); \
         break; \
      case MI_PRIV_SIGNED : \
         dvalue = MAX(LONG_MIN, dvalue); \
         dvalue = MIN(LONG_MAX, dvalue); \
         *((signed long *) ptr) = ROUND(dvalue); \
         break; \
      } \
      break; \
   case NC_FLOAT : \
      dvalue = MAX(-FLT_MAX,dvalue); \
      *((float *) ptr) = MIN(FLT_MAX,dvalue); \
      break; \
   case NC_DOUBLE : \
      *((double *) ptr) = dvalue; \
      break; \
   }


#endif
