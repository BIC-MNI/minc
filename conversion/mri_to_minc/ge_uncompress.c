#include <stdio.h>
#include <stdlib.h>

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
            conv.b[0] = second_byte;
            conv.b[1] = getc(fpin);
            base = conv.s;
         }
      }

      /* Write out the data */
      conv.s = base;
      (void) putc((int) conv.b[0], fpout);
      (void) putc((int) conv.b[1], fpout);

   }

   return EXIT_SUCCESS;
}
