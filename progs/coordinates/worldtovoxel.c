/* ----------------------------- MNI Header -----------------------------------
@NAME       : worldtovoxel
@INPUT      : argc, argv - command line arguments
@OUTPUT     : (none)
@RETURNS    : status
@DESCRIPTION: Program to convert world coordinates to voxel coordinates
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June 13, 1994 (Peter Neelin)
@MODIFIED   : 
 * $Log: worldtovoxel.c,v $
 * Revision 6.8  2008/01/17 02:33:02  rotor
 *  * removed all rcsids
 *  * removed a bunch of ^L's that somehow crept in
 *  * removed old (and outdated) BUGS file
 *
 * Revision 6.7  2008/01/12 19:08:15  stever
 * Add __attribute__ ((unused)) to all rcsid variables.
 *
 * Revision 6.6  2006/05/19 00:35:58  bert
 * Add config.h to several files that might need it
 *
 * Revision 6.5  2004/11/01 22:38:38  bert
 * Eliminate all references to minc_def.h
 *
 * Revision 6.4  2004/02/02 18:27:51  bert
 * Call ParseArgv() so that version information can be output
 *
 * Revision 6.3  2001/04/24 13:38:41  neelin
 * Replaced NC_NAT with MI_ORIGINAL_TYPE.
 *
 * Revision 6.2  2001/04/17 18:40:16  neelin
 * Modifications to work with NetCDF 3.x
 * In particular, changed NC_LONG to NC_INT (and corresponding longs to ints).
 * Changed NC_UNSPECIFIED to NC_NAT.
 * A few fixes to the configure script.
 *
 * Revision 6.1  1999/10/19 14:45:17  neelin
 * Fixed Log subsitutions for CVS
 *
 * Revision 6.0  1997/09/12 13:24:08  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:25:07  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:01:38  neelin
 * Release of minc version 0.4
 *
 * Revision 3.0  1995/05/15  19:32:23  neelin
 * Release of minc version 0.3
 *
 * Revision 2.0  1994/09/28  10:36:36  neelin
 * Release of minc version 0.2
 *
 * Revision 1.3  94/09/28  10:36:31  neelin
 * Pre-release
 * 
 * Revision 1.2  94/09/26  10:05:26  neelin
 * Changed vx,vy,vz to v0, v1, v2.
 * 
 * Revision 1.1  94/06/13  10:22:04  neelin
 * Initial revision
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <volume_io.h>
#include <ParseArgv.h>

/* Constants */
#ifndef TRUE
#  define TRUE 1
#  define FALSE 0
#endif
#ifndef public
#  define public
#  define private static
#endif

/* Function to print to stderr */
void print_to_stderr(char *string)
{
   (void) fprintf(stderr, "%s", string);
   return;
}

/* Argument table */
ArgvInfo argTable[] = {
   {NULL, ARGV_END, NULL, NULL, NULL}
};

/* Main program */

int main(int argc, char *argv[])
{
   Volume volume;
   volume_input_struct input_info;
   char *filename;
   double v0, v1, v2, wx, wy, wz;
   static char *dim_names[] =
      {ANY_SPATIAL_DIMENSION, ANY_SPATIAL_DIMENSION, ANY_SPATIAL_DIMENSION};

   /* Check arguments */
   if (ParseArgv(&argc, argv, argTable, 0) || argc != 5) {
      (void) fprintf(stderr, 
         "Usage: %s <image file> <world x coord> <y coord> <z coord>\n",
                     argv[0]);
      exit(EXIT_FAILURE);
   }
   filename = argv[1];
   wx = atof(argv[2]);
   wy = atof(argv[3]);
   wz = atof(argv[4]);

   /* Open the image file */
   set_print_function(print_to_stderr);
   if (start_volume_input(filename, 3, dim_names, MI_ORIGINAL_TYPE, TRUE,
                          0.0, 0.0, TRUE, &volume, NULL, &input_info) != OK) {
      (void) fprintf(stderr, "Error opening file %s for input.\n",
                     filename);
      exit(EXIT_FAILURE);
   }

   /* Convert the voxel to world coordinates */
   convert_3D_world_to_voxel(volume, wx, wy, wz, &v0, &v1, &v2);

   /* Write out the result */
   (void) printf("%.20g %.20g %.20g\n", v0, v1, v2);

   exit(EXIT_SUCCESS);
}

