/* Copyright David Leonard and Andrew Janke, 2000. All rights reserved. */

#include <stdio.h>
#include <stdlib.h>
#include "node.h"

struct sym {
   ident_t ident;
   char    is_scalar;
   scalar_t scalar;
   vector_t vector;
   sym_t   next;
};

static sym_t new_sym(ident_t id, sym_t sym){
   sym_t newsym;

   newsym = malloc(sizeof *newsym);
   newsym->ident = id;
   newsym->next = sym;
   return newsym;
}

sym_t sym_enter_scalar(scalar_t sc, ident_t id, sym_t sym){
   sym_t newsym = new_sym(id, sym);
   newsym->is_scalar = 1;
   newsym->scalar = sc;
   return newsym;
}

sym_t sym_enter_vector(vector_t v, ident_t id, sym_t sym){
   sym_t newsym = new_sym(id, sym);
   newsym->is_scalar = 0;
   newsym->vector = v;
   return newsym;
}

static sym_t sym_lookup(ident_t id, sym_t sym){
   while (sym) {
      if (sym->ident == id)
         break;
      sym = sym->next;
   }
   return sym;
}

scalar_t sym_lookup_scalar(ident_t id, sym_t sym){
   sym_t s = sym_lookup(id, sym);
   if (!s)
      /* errx(1, "%s undefined", ident_str(id)); */
      fprintf(stderr, "%s undefined", ident_str(id));
   if (!s->is_scalar)
      /* errx(1, "%s is not scalar (lowercase)", ident_str(id)); */
      fprintf(stderr, "%s is not scalar (lowercase)", ident_str(id));
   return s->scalar;
}

vector_t sym_lookup_vector(ident_t id, sym_t sym){
   sym_t s = sym_lookup(id, sym);
   if (!s)
      /* errx(1, "%s undefined", ident_str(id)); */
      fprintf(stderr, "%s undefined", ident_str(id));
   if (s->is_scalar)
      return NULL;
   return s->vector;
}

sym_t sym_leave(sym_t sym){
   sym_t osym = sym->next;
   free(sym);
   return osym;
}
