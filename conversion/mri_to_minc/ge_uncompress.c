#include <stdio.h>
#include <stdlib.h>

/* Constants for handling byte order - First is 0 for big-endian, 
   and 1 for little endian; Second is the other way around */
int idummy = 1;
char *dummy = (char *) &idummy;
int First  = 0;
int Second = 1;

int main(int argc, char *argv[])
{
   union {
      unsigned char b[2];
      short s;
   } conv;

   int first_byte;
   int second_byte;
   int base;
   FILE *fpin, *fpout;

   /* Figure out byte order for machine */
   if ((int) dummy[0] == 1) {   /* Little-endian */
      First = 1;
      Second = 0;
   }
   else {                      /* Big-endian */
      First = 0;
      Second = 1;
   }

   /* Loop through bytes */
   base = 0;
   fpin = stdin;
   fpout = stdout;
   while ((first_byte = getc(fpin)) != EOF) {

      /* Look for single byte data */
      if (first_byte < 0x40) {
         base += first_byte;
      }
      else if (first_byte < 0x80) {
         base += first_byte - 0x80;
      }

      /* Otherwise, look for two or three-byte form */
      else {

         second_byte = getc(fpin);
         if (first_byte < 0xa0) {
            base += ((first_byte & 0x1f) << 8) + second_byte;
         }
         else if (first_byte < 0xc0) {
            base += ((first_byte | 0xc0) << 8) + second_byte;
         }

         /* We have a three-byte form */
         else {
            conv.b[First] = second_byte;
            conv.b[Second] = getc(fpin);
            base = conv.s;
         }
      }

      /* Write out the data */
      conv.s = base;
      (void) putc((int) conv.b[First], fpout);
      (void) putc((int) conv.b[Second], fpout);

   }

   return EXIT_SUCCESS;
}
