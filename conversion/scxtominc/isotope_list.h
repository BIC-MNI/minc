/* ----------------------------- MNI Header -----------------------------------
@NAME       : isotope_list.h
@DESCRIPTION: Header file containing list of isotopes and their half-lives.
@METHOD     : 
@GLOBALS    : isotope_list
@CALLS      : 
@CREATED    : January 13, 1993 (Peter Neelin)
@MODIFIED   : $Log: isotope_list.h,v $
@MODIFIED   : Revision 1.3  1994-09-28 10:33:32  neelin
@MODIFIED   : Pre-release
@MODIFIED   :
 * Revision 1.2  93/08/11  15:27:48  neelin
 * Added RCS logging in source.
 * 
---------------------------------------------------------------------------- */

struct {
   char *name;
   double half_life;      /* Half life in seconds */
} isotope_list[] = {
   {"GA-68", 68.3*60.0},
   {"C-11", 20.3*60.0},
   {"F-18", 109.8*60.0},
   {"O-15", 2.03*60.0},
   {"N-13", 9.96*60.0},
   {"GE-68", 414720*60.0},
   {"CU-64", 762*60.0},
   {"CS-137", 999999*60.0},
   {NULL, -1.0}
};
