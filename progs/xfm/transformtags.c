/* ----------------------------- MNI Header -----------------------------------
@NAME       : transformtags.c
@INPUT      : argc, argv - command line arguments
@OUTPUT     : (none)
@RETURNS    : status
@DESCRIPTION: Program to transform a tag file using a transform file
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : September 13, 1993 (Peter Neelin)
@MODIFIED   : 
 * $Log: transformtags.c,v $
 * Revision 6.3  2008-01-12 19:08:15  stever
 * Add __attribute__ ((unused)) to all rcsid variables.
 *
 * Revision 6.2  2004/11/01 22:38:39  bert
 * Eliminate all references to minc_def.h
 *
 * Revision 6.1  1999/10/19 14:45:31  neelin
 * Fixed Log subsitutions for CVS
 *
 * Revision 6.0  1997/09/12 13:23:28  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:24:29  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:00:08  neelin
 * Release of minc version 0.4
 *
 * Revision 3.0  1995/05/15  19:31:03  neelin
 * Release of minc version 0.3
 *
 * Revision 2.0  1994/09/28  10:33:24  neelin
 * Release of minc version 0.2
 *
 * Revision 1.5  94/09/28  10:33:21  neelin
 * Pre-release
 * 
 * Revision 1.4  93/10/12  12:52:16  neelin
 * Replaced def_mni.h with volume_io.h
 * 
 * Revision 1.3  93/09/16  09:39:00  neelin
 * Use dave's open_file_with_default_suffix and input_transform_file and
 * output_transform_file to add suffixes to file names.
 * 
 * Revision 1.2  93/09/14  09:50:20  neelin
 * changed name to from tagtransform to transformtags
 * 
 * Revision 1.1  93/09/13  16:16:55  neelin
 * Initial revision
 * 
---------------------------------------------------------------------------- */

#ifndef lint
static char rcsid[] __attribute__ ((unused))="$Header: /private-cvsroot/minc/progs/xfm/transformtags.c,v 6.3 2008-01-12 19:08:15 stever Exp $";
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

/* Variables for argument parsing */
int volume_to_transform = 2;
char *xfmfile = NULL;

/* Argument table */
ArgvInfo argTable[] = {
   {"-vol1", ARGV_CONSTANT, (char *) 1, (char *) &volume_to_transform,
       "Transform tags for volume 1."},
   {"-vol2", ARGV_CONSTANT, (char *) 2, (char *) &volume_to_transform,
       "Transform tags for volume 2 (default)."},
   {"-transformation", ARGV_STRING, (char *) NULL, (char *) &xfmfile,
       "Name of transformation file (default = identity)."},
   {NULL, ARGV_END, NULL, NULL, NULL}
};

/* Main program */

int main(int argc, char *argv[])
{
   char *pname, *intagfile, *outtagfile;
   int n_volumes, n_tag_points, ipoint;
   Real **tags_volume1, **tags_volume2, **tag_list;
   General_transform transform;
   FILE *fp;
   char comment_string[512];
   char *comment = comment_string;
   Real *weights;
   int *structure_ids;
   int *patient_ids;
   char **labels;

   /* Parse arguments */
   pname = argv[0];
   if (ParseArgv(&argc, argv, argTable, 0) || (argc != 3)) {
      (void) fprintf(stderr, 
                     "\nUsage: %s [<options>] infile.tag outfile.tag\n\n",
                     argv[0]);
      exit(EXIT_FAILURE);
   }
   intagfile  = argv[1];
   outtagfile = argv[2];

   /* Read in tag file */
   if ((open_file_with_default_suffix(intagfile,
                  get_default_tag_file_suffix(),
                  READ_FILE, ASCII_FORMAT, &fp) != OK) ||
       (input_tag_points(fp, &n_volumes, &n_tag_points, 
                         &tags_volume1, &tags_volume2, 
                         &weights, &structure_ids, &patient_ids, 
                         &labels) != OK)) {
      (void) fprintf(stderr, "%s: Error reading tag file %s\n", 
                     pname, intagfile);
      exit(EXIT_FAILURE);
   }
   (void) close_file(fp);

   /* Check number of volumes */
   if (n_volumes > 2) {
      (void) fprintf(stderr, "%s: Wrong number of volumes in %s\n", 
                     pname, intagfile);
      exit(EXIT_FAILURE);
   }

   /* Get the list of tags to transform */
   if (n_volumes == 1) volume_to_transform = 1;
   if (volume_to_transform == 1) {
      tag_list = tags_volume1;
   }
   else {
      tag_list = tags_volume2;
   }

   /* Get the transform */
   if (xfmfile == NULL) {
      create_linear_transform(&transform, NULL);
   }
   else {
      if (input_transform_file(xfmfile, &transform) != OK) {
         (void) fprintf(stderr, "%s: Error reading transform file %s\n",
                        pname, xfmfile);
         exit(EXIT_FAILURE);
      }
   }

   /* Transform the points */
   for (ipoint=0; ipoint < n_tag_points; ipoint++) {
      general_transform_point(&transform,
                              tag_list[ipoint][0],
                              tag_list[ipoint][1],
                              tag_list[ipoint][2],
                              &tag_list[ipoint][0],
                              &tag_list[ipoint][1],
                              &tag_list[ipoint][2]);
   }

   /* Create a comment for the new file */
   if (xfmfile != NULL) {
      (void) sprintf(comment, 
                     " Volume %d points transformed using xfm file %s",
                     volume_to_transform, xfmfile);
   }
   else {
      comment = NULL;
   }

   /* Write out the new tag file */
   if ((open_file_with_default_suffix(outtagfile,
                  get_default_tag_file_suffix(),
                  WRITE_FILE, ASCII_FORMAT, &fp) != OK) ||
       (output_tag_points(fp, comment, n_volumes, n_tag_points,
                          tags_volume1, tags_volume2,
                          weights, structure_ids, patient_ids, 
                          labels) != OK)) {
      (void) fprintf(stderr, "%s: Error writing new tag file %s\n",
                     pname, outtagfile);
      exit(EXIT_FAILURE);
   }
   (void) close_file(fp);

   exit(EXIT_SUCCESS);

}


