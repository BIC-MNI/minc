/* Copyright David Leonard and Andrew Janke, 2000. All rights reserved. */

#include <stdio.h>
#include <stdlib.h>
#include "node.h"

#define INIT_SIZE 20

scalar_t new_scalar(int width){
   scalar_t s;

   s = malloc(sizeof *s);
   s->width = width;
   s->vals = malloc(width * sizeof(s->vals[0]));
   s->refcnt = 1;
   return s;
}

void scalar_incr_ref(scalar_t s) {
   s->refcnt++;
}

void scalar_free(scalar_t s){
   if (s->refcnt <= 0) {
      (void) fprintf(stderr, "Internal error: scalar freed too often\n");
      exit(1);
   }
   if (--s->refcnt == 0) {
      free(s->vals);
      s->vals = NULL;
      free(s);
   }
}
