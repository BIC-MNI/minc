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
@MODIFIED   : $Log: transformtags.c,v $
@MODIFIED   : Revision 1.2  1993-09-14 09:50:20  neelin
@MODIFIED   : changed name to from tagtransform to transformtags
@MODIFIED   :
 * Revision 1.1  93/09/13  16:16:55  neelin
 * Initial revision
 * 
---------------------------------------------------------------------------- */

#ifndef lint
static char rcsid[]="$Header: /private-cvsroot/minc/progs/xfm/transformtags.c,v 1.2 1993-09-14 09:50:20 neelin Exp $";
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <def_mni.h>
#include <ParseArgv.h>
#include <minc_def.h>

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
   if (((fp = fopen(intagfile, "r")) == NULL) || 
       (input_tag_points(fp, &n_volumes, &n_tag_points, 
                         &tags_volume1, &tags_volume2, 
                         &weights, &structure_ids, &patient_ids, 
                         &labels) != OK)) {
      (void) fprintf(stderr, "%s: Error reading tag file %s\n", 
                     pname, intagfile);
      exit(EXIT_FAILURE);
   }
   (void) fclose(fp);

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
      if (((fp=fopen(xfmfile,"r")) == NULL) ||
          (input_transform(fp, &transform) != OK)) {
         (void) fprintf(stderr, "%s: Error reading transform file %s\n",
                        pname, xfmfile);
         exit(EXIT_FAILURE);
      }
   }
   (void) fclose(fp);

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
   if (((fp=fopen(outtagfile,"w")) == NULL) ||
       (output_tag_points(fp, comment, n_volumes, n_tag_points,
                          tags_volume1, tags_volume2,
                          weights, structure_ids, patient_ids, 
                          labels) != OK)) {
      (void) fprintf(stderr, "%s: Error writing new tag file %s\n",
                     pname, outtagfile);
      exit(EXIT_FAILURE);
   }
   (void) fclose(fp);

   exit(EXIT_SUCCESS);

}


