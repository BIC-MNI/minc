/* ----------------------------- MNI Header -----------------------------------
@NAME       : extract.c
@INPUT      : argc - number of arguments
              argv - arguments
                 1 - number of bytes to skip (default = 0)
                 2 - number of bytes to copy (default = all)
                 3 - filename (if not present uses stdin)
@OUTPUT     : (none)
@DESCRIPTION: Reads a given number of bytes from a file or standard 
              input starting at a given byte. Writes those bytes to
              standard output.
@METHOD     : 
@GLOBALS    : (none)
@CALLS      : 
@CREATED    : January 13,1991 (P.N.)
@MODIFIED   : June 3,1992 (P.N.)
                 - added -h argument to print usage
---------------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>

#ifndef TRUE
#  define TRUE 1
#endif
#ifndef FALSE
#  define FALSE 0
#endif

#define ARRSIZE 1500
#define INTYPE char

main(int argc, char **argv)
{
   char *pname;
   FILE *fp1;
   INTYPE value[ARRSIZE];
   int nread,skip,pass,all;

   /* Get arguments */
   if ((argc--) > 0) pname=(*argv++);   /* program name */
   if ((argc > 0) && (strcmp(*argv,"-h") == 0)) {
      (void) fprintf(stderr,
         "Usage: %s <bytes to skip> <bytes to copy> <filename>\n",
                     pname);
      return 0;
   }
   if ((argc--)>0)                      /* bytes to skip */
      skip=atol(*argv++);
   else 
      skip=0;
   if ((argc--)>0)                      /* bytes to copy */
      pass=atol(*argv++);
   else 
      pass=(-1);
   all=(pass<0);

   if ((argc--) >0) {                   /* filename */
      fp1=fopen(*argv,"r");
      if (fp1 == NULL) {
         fprintf(stderr,"%s can't open %s\n",pname,*argv);
         exit(2);
      }

      /* Seek to right place on file */
      if(fseek(fp1, skip, 0) == -1){ 
         fprintf(stderr,"%s can't fseek on a terminal\n",pname);
         exit(2);
      }
   }
   else {
      fp1=stdin;

      /* Read to skip on stdin */
      while(skip > 0) {
         if (skip > ARRSIZE) {
            nread = fread(value, sizeof(INTYPE),ARRSIZE,fp1);
         }
         else {
            nread = fread(value, sizeof(INTYPE),skip,fp1);
         }
         skip -= nread;
         if (nread <= 0) return 0;
      }
   }

   /* Copy bytes */
   while ((pass > 0) || all) {
      if ((pass > ARRSIZE)||all) {
         nread = fread(value, sizeof(INTYPE),ARRSIZE,fp1);
      }
      else {
         nread = fread(value, sizeof(INTYPE),pass,fp1);
      }
      fwrite(value,sizeof(INTYPE),nread,stdout);
      pass -= nread;
      if (nread<=0) {
         pass = -1;
         all = FALSE;
      }
   }

   return 0;
}
