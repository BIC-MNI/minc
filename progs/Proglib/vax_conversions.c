/* ----------------------------- MNI Header -----------------------------------
@NAME       : vax_conversions.c
@DESCRIPTION: File containing routines to convert machine values to vax format
              values.
@METHOD     : 
@CREATED    : December 10, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */

#ifndef lint
static char rcsid[]="$Header: /private-cvsroot/minc/progs/Proglib/vax_conversions.c,v 1.2 1993-02-01 15:55:44 neelin Exp $";
#endif

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

