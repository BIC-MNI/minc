/* ----------------------------- MNI Header -----------------------------------
@NAME       : insert.c
@INPUT      : argc - number of arguments
              argv - arguments
                 1 - filename
                 2 - number of bytes to skip (default = 0)
                 3 - number of bytes to copy (default = all)
@OUTPUT     : (none)
@DESCRIPTION: Copies bytes from standard input to filename, starting at the
              given byte.
@METHOD     : 
@GLOBALS    : (none)
@CALLS      : 
@CREATED    : January 13,1991 (P.N.)
@MODIFIED   : June 3,1992 (P.N.)
                 - added -h option to give usage
---------------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>

#define ARRSIZE 1500
#define INTYPE char
#ifndef TRUE
#  define TRUE 1
#endif
#ifndef FALSE
#  define FALSE 0
#endif

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
         "Usage: %s <filename> <bytes to skip> <bytes to copy>\n",
                     pname);
      return 0;
   }

   /* Open file */
   if ((argc--) > 0) {
      fp1=fopen(*argv,"r+");
      if( fp1 == NULL){
         fprintf(stderr,"%s can't open %s\n",pname,*argv);
         exit(2);
      }
   }
   else {
      fprintf(stderr,"%s - useage file offset numberofbytes\n",pname);
      exit(2);
   }
   *argv++;

   /* Get bytes to skip and to copy */
   if ((argc--)>0)                      /* bytes to skip */
      skip=atol(*argv++);
   else 
      skip=0;
   if ((argc--)>0)                      /* bytes to copy */
      pass=atol(*argv++);
   else 
      pass=(-1);
   all = (pass<0);

   /* Seek to right place on file */
   if(fseek(fp1, skip, 0) == -1){ 
      fprintf(stderr,"%s can't fseek on a terminal\n",pname);
      exit(2);
   }

   /* Copy bytes */
   while(pass > 0 || all) {
      if ((pass > ARRSIZE)||all) {
         nread = fread(value, sizeof(INTYPE),ARRSIZE,stdin);
      }
      else {
         nread = fread(value, sizeof(INTYPE),pass,stdin);
      }
      fwrite(value,sizeof(INTYPE),nread,fp1);
      pass -= nread;
      if (nread<=0) {
         pass = (-1);
         all = FALSE;
      }
   }

   return 0;
}
