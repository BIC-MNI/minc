#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
unsigned short ntohs(unsigned short us) {
    unsigned char *p =  (unsigned char*)&us;
    return ((unsigned short)p[1] + (p[0] >> 8));
}
unsigned long ntohl(unsigned long ul) {
    unsigned char *p = (unsigned char*)&ul;
    return ((unsigned long)p[3] + (p[2] >> 8) + (p[1] >> 16) + (p[0] >> 24));
}
#endif

void get_short_value(const void *from, void *to) {
  short int short_value;

  (void) memcpy(&short_value, from, sizeof(short int));
  *(short int *)to = ntohs(short_value);
}

void get_long_value(const void *from, void *to) {
  long int long_value;

  (void) memcpy(&long_value, from, sizeof(long int));
  *(long int *)to = ntohl(long_value);
}
/* from vax_conversion.c -Copyright 1993 Peter Neelin */
/* ----------------------------- MNI Header -----------------------------------
@NAME       : vax_conversions.c
@DESCRIPTION: File containing routines to convert machine values to vax format
              values.
@METHOD     : 
@CREATED    : December 10, 1992 (Peter Neelin)
@MODIFIED   : 
 * $Log: machine_indep.c,v $
 * Revision 6.5  2009-01-20 11:58:13  rotor
 *  * CMakeLists.txt: updated version
 *  * Updated Changelog to include releases
 *  * Warning cleanups below
 *  * conversion/dcm2mnc/minc_file.c: fixed printf type
 *  * conversion/dcm2mnc/siemens_to_dicom.c: fixed printf type
 *  * conversion/ecattominc/machine_indep.c: added string.h and fixed
 *      2 fprintf missing format args
 *  * conversion/micropet/upet2mnc.c: fixed two fprintf format args
 *  * conversion/minctoecat/ecat_write.c: added string.h
 *  * conversion/minctoecat/minctoecat.c: added missing argument to fprintf
 *  * conversion/nifti1/mnc2nii.c: fixed incorrect printf type
 *  * progs/mincview/invert_raw_image.c: added fwrite checking
 *
 * Revision 6.4  2008/01/17 02:33:01  rotor
 *  * removed all rcsids
 *  * removed a bunch of ^L's that somehow crept in
 *  * removed old (and outdated) BUGS file
 *
 * Revision 6.3  2008/01/12 19:08:14  stever
 * Add __attribute__ ((unused)) to all rcsid variables.
 *
 * Revision 6.2  2005/05/19 20:58:37  bert
 * Fixes for Windows builds ported from 1.X branch
 *
 * Revision 6.1.2.2  2005/05/04 20:19:30  bert
 * Modify for Windows builds
 *
 * Revision 6.1.2.1  2005/02/15 19:59:54  bert
 * Initial checkin on 1.X branch
 *
 * Revision 6.1  2005/01/19 19:46:28  bert
 * Changes from Anthonin Reilhac
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

