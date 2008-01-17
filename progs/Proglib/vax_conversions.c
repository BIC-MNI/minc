/* ----------------------------- MNI Header -----------------------------------
@NAME       : vax_conversions.c
@DESCRIPTION: File containing routines to convert machine values to vax format
              values.
@METHOD     : 
@CREATED    : December 10, 1992 (Peter Neelin)
@MODIFIED   : 
 * $Log: vax_conversions.c,v $
 * Revision 6.4  2008-01-17 02:33:02  rotor
 *  * removed all rcsids
 *  * removed a bunch of ^L's that somehow crept in
 *  * removed old (and outdated) BUGS file
 *
 * Revision 6.3  2008/01/12 19:08:15  stever
 * Add __attribute__ ((unused)) to all rcsid variables.
 *
 * Revision 6.2  1999/10/19 15:57:18  neelin
 * Fixed log message containing log substitution
 *
 * Revision 6.1  1999/10/19 14:45:15  neelin
 * Fixed Log subsitutions for CVS
 *
 * Revision 6.0  1997/09/12 13:23:41  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:24:41  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:00:50  neelin
 * Release of minc version 0.4
 *
 * Revision 3.0  1995/05/15  19:31:35  neelin
 * Release of minc version 0.3
 *
 * Revision 2.0  1994/09/28  10:34:32  neelin
 * Release of minc version 0.2
 *
 * Revision 1.5  94/09/28  10:34:18  neelin
 * Pre-release
 * 
 * Revision 1.4  93/08/04  13:04:01  neelin
 * Added RCS Log to keep track of modifications in source
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
---------------------------------------------------------------------------- */

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_vax_short
@INPUT      : nvals - number of values to convert
              vax_value - pointer to array of shorts in vax format
@OUTPUT     : mach_value - pointer to array of shorts in current machine format
@RETURNS    : (nothing)
@DESCRIPTION: Converts vax short integers to short integers in the format of
              the current machine.
@METHOD     : 
@GLOBALS    : (none)
@CALLS      : memcpy
@CREATED    : December 10, 1992.
@MODIFIED   : 
---------------------------------------------------------------------------- */
void get_vax_short(int nvals, void *vax_value, short *mach_value)
{
   int i;
   char *ptr1, *ptr2, v0, v1;

#ifdef vax
   memcpy((void *) mach_value, vax_value, nvals*sizeof(short));
#else
   ptr1 = (char *) vax_value;
   ptr2 = (char *) mach_value;
   for (i=0; i<nvals; i++) {
      v0 = ptr1[1];
      v1 = ptr1[0];
      ptr2[0] = v0;
      ptr2[1] = v1;
      ptr1 += sizeof(mach_value[0]);
      ptr2 += sizeof(mach_value[0]);
   }
#endif

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_vax_long
@INPUT      : nvals - number of values to convert
              vax_value - pointer to array of longs in vax format
@OUTPUT     : mach_value - pointer to array of longs in current machine format
@RETURNS    : (nothing)
@DESCRIPTION: Converts vax long integers to long integers in the format of
              the current machine.
@METHOD     : 
@GLOBALS    : (none)
@CALLS      : memcpy
@CREATED    : December 10, 1992.
@MODIFIED   : 
---------------------------------------------------------------------------- */
void get_vax_long(int nvals, void *vax_value, long *mach_value)
{
   int i;
   char *ptr1, *ptr2, v0, v1;

#ifdef vax
   memcpy((void *) mach_value, vax_value, nvals*sizeof(long));
#else
   ptr1 = (char *) vax_value;
   ptr2 = (char *) mach_value;
   for (i=0; i<nvals; i++) {
      v0 = ptr1[3];
      v1 = ptr1[0];
      ptr2[0] = v0;
      ptr2[3] = v1;
      v0 = ptr1[2];
      v1 = ptr1[1];
      ptr2[1] = v0;
      ptr2[2] = v1;
      ptr1 += sizeof(mach_value[0]);
      ptr2 += sizeof(mach_value[0]);
   }
#endif

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_vax_float
@INPUT      : nvals - number of values to convert
              vax_value - pointer to array of floats in vax format
@OUTPUT     : mach_value - pointer to array of floats in current machine format
@RETURNS    : (nothing)
@DESCRIPTION: Converts vax floats to floats in the format of
              the current machine.
@METHOD     : 
@GLOBALS    : (none)
@CALLS      : memcpy
@CREATED    : December 10, 1992.
@MODIFIED   : 
---------------------------------------------------------------------------- */
void get_vax_float(int nvals, void *vax_value, float *mach_value)
{
   int i;
   char *ptr1, *ptr2, v0, v1;

#ifdef vax
   memcpy((void *) mach_value, vax_value, nvals*sizeof(float));
#else
   ptr1 = (char *) vax_value;
   ptr2 = (char *) mach_value;
   for (i=0; i<nvals; i++) {
      v0 = ptr1[1];
      v1 = ptr1[0];
      ptr2[0] = v0;
      ptr2[1] = v1;
      v0 = ptr1[3];
      v1 = ptr1[2];
      ptr2[2] = v0;
      ptr2[3] = v1;
      mach_value[i] /= 4.0;
      ptr1 += sizeof(mach_value[0]);
      ptr2 += sizeof(mach_value[0]);
   }
#endif

   return;
}

