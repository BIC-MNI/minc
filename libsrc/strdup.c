#include <stdlib.h>
#include <string.h>
/* ----------------------------- MNI Header -----------------------------------
@NAME       : strdup
@INPUT      : string - string to duplicate
@OUTPUT     : (none)
@RETURNS    : Pointer to duplicate string or NULL if an error occurs
@DESCRIPTION: Makes a duplicate of a string and returns a pointer to it.
@METHOD     : VAX CC rtl does not have strdup, so we provide it here to
              be included in minc.olb.
@GLOBALS    : 
@CALLS      : 
@CREATED    : June 18, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
char *strdup(const char *string)
{
   int length;
   char *new_string;

   /* Get the string length */
   length = strlen(string);

   /* Allocate space */
   new_string = malloc((size_t) length+1);
   if (new_string == NULL) {
      return NULL;
   }

   /* Copy the string */
   return strcpy(new_string, string);

}
