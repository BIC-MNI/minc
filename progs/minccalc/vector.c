/* Copyright David Leonard and Andrew Janke, 2000. All rights reserved. */

#include <stdio.h>
#include <stdlib.h>
#include "node.h"

#define INIT_SIZE 20

vector_t new_vector(){
   vector_t v;

   v = malloc(sizeof *v);
   v->maxlen = INIT_SIZE;
   v->el = malloc(INIT_SIZE * sizeof v->el[0]);
   v->len = 0;
   v->refcnt = 1;
   return v;
}

void vector_incr_ref(vector_t v) {
   v->refcnt++;
}

void vector_append(vector_t v, scalar_t s){
   if (v->len + 1 == v->maxlen) {
      v->maxlen *= 2;
      v->el = realloc(v->el, v->maxlen * sizeof v->el[0]);
   }
   v->el[v->len++] = s;
   scalar_incr_ref(s);
}

void vector_free(vector_t v){
   int i;

   if (v->refcnt <= 0) {
      (void) fprintf(stderr, "Internal error: vector freed too often\n");
      exit(1);
   }
   if (--v->refcnt == 0) {
      for (i=0; i < v->len; i++) {
         scalar_free(v->el[i]);
      }
      free(v->el);
      v->el = NULL;
      free(v);
   }
}
