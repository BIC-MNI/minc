/* Copyright David Leonard and Andrew Janke, 2000. All rights reserved. */

#include <stdio.h>
#include <stdlib.h>
#include "node.h"

#define NEW_SCOPE (-1)

typedef enum {SYM_UNKNOWN, SYM_SCALAR, SYM_VECTOR} sym_type_t;

struct sym {
   ident_t ident;
   sym_type_t type;
   scalar_t scalar;
   vector_t vector;
   sym_t   next;
};

static sym_t new_sym(ident_t id, sym_t sym){
   sym_t newsym;

   newsym = malloc(sizeof *newsym);
   newsym->ident = id;
   newsym->type = SYM_UNKNOWN;

   /* We put new scope nodes at the head of the list. For regular symbols,
      we add the symbol to the head of the list only in the case where 
      the head of the current symbol table is not a new scope. */
   if ((id == NEW_SCOPE) || (sym==NULL) || (sym->ident != NEW_SCOPE)) {
      newsym->next = sym;
   }

   /* Usually for regular symbols, we add the symbol to an existing scope, 
      so we put it as the second element of the list and return the
      head of the list. */
   else {
      newsym->next = sym->next;
      sym->next = newsym;
   }

   return newsym;
}

sym_t sym_enter_scope(sym_t sym) {
   sym_t newsym = new_sym(NEW_SCOPE, sym);
   return newsym;
}

void sym_leave_scope(sym_t sym) {
   sym_t osym;
   if (sym==NULL) return;
   do {
      osym = sym->next;
      if (sym->type == SYM_VECTOR) {
         vector_free(sym->vector);
      }
      free(sym);
      sym = osym;
   } while ((sym != NULL) && (sym->ident != NEW_SCOPE));
   return;
}

void sym_declare_ident(ident_t id, sym_t sym) {
   (void) new_sym(id, sym);
   return;
}

static sym_t sym_lookup(ident_t id, sym_t sym){
   while (sym) {
      if (sym->ident == id)
         break;
      sym = sym->next;
   }
   return sym;
}

void sym_set_scalar(int width, int *eval_flags, 
                    scalar_t sc, ident_t id, sym_t sym){
   int ivalue;

   /* Find the symbol - it is does not exist make a new one */
   sym_t newsym = sym_lookup(id, sym);
   if (newsym == NULL) {
      newsym = new_sym(id, sym);
   }

   /* Make sure that any existing one is of the right type */
   if (newsym->type == SYM_VECTOR) {
      /* errx(1, "%s is not a scalar", ident_str(id)); */
      fprintf(stderr, "%s is not a scalar(lowercase)\n", ident_str(id));
      exit(1);
   }

   /* Create a new scalar if needed */
   if (newsym->type == SYM_UNKNOWN || newsym->scalar->width < width) {
      if (newsym->type == SYM_SCALAR)
         scalar_free(newsym->scalar);
      newsym->type = SYM_SCALAR;
      newsym->scalar = new_scalar(width);
   }

   /* Copy in the values */
   for (ivalue=0; ivalue < width; ivalue++) {
      if (eval_flags != NULL && !eval_flags[ivalue]) continue;
      newsym->scalar->vals[ivalue] = sc->vals[ivalue];
   }
   return;
}

void sym_set_vector(int width, int *eval_flags, 
                    vector_t v, ident_t id, sym_t sym){
   int ivalue, iel;
   scalar_t sc;

   /* Find the symbol - it is does not exist make a new one */
   sym_t newsym = sym_lookup(id, sym);
   if (newsym == NULL) {
      newsym = new_sym(id, sym);
   }

   /* Make sure that any existing one is of the right type */
   if (newsym->type == SYM_SCALAR) {
      /* errx(1, "%s is not a vector", ident_str(id)); */
      fprintf(stderr, "%s is not a vector\n", ident_str(id));
      exit(1);
   }

   /* Create a new vector if needed - either it does not exist or the
      length is changing or the width is increasing*/
   if (newsym->type == SYM_UNKNOWN || newsym->vector->len != v->len ||
      (newsym->vector->len > 0 && newsym->vector->el[0]->width < width) ) {

      /* Free an existing vector. If eval_flags is set, then we cannot
         change the length of the vector */
      if (newsym->type == SYM_VECTOR) {
         if (eval_flags != NULL && newsym->vector->len != v->len) {
            /* errx(1, "assigned vector must match length of %s in if", 
                    ident_str(id)); */
            fprintf(stderr, "assigned vector must match length of %s in if", 
                    ident_str(id));
            exit(1);
         }
         vector_free(newsym->vector);
      }
      newsym->type = SYM_VECTOR;
      newsym->vector = new_vector();
      for (iel=0; iel < v->len; iel++) {
         sc = new_scalar(width);
         vector_append(newsym->vector, sc);
         scalar_free(sc);
      }
   }

   /* Copy in the values */
   for (ivalue=0; ivalue < width; ivalue++) {
      if (eval_flags != NULL && !eval_flags[ivalue]) continue;
      for (iel=0; iel < v->len; iel++) {
         newsym->vector->el[iel]->vals[ivalue] = v->el[iel]->vals[ivalue];
      }
   }

   return;
}

scalar_t sym_lookup_scalar(ident_t id, sym_t sym){
   sym_t s = sym_lookup(id, sym);
   if (!s) {
      /* errx(1, "%s undefined", ident_str(id)); */
      fprintf(stderr, "%s undefined\n", ident_str(id));
      exit(1);
   }
   if (s->type != SYM_SCALAR) {
      /* errx(1, "%s is not scalar (lowercase)", ident_str(id)); */
      fprintf(stderr, "%s is not scalar (lowercase)\n", ident_str(id));
      exit(1);
   }
   return s->scalar;
}

vector_t sym_lookup_vector(ident_t id, sym_t sym){
   sym_t s = sym_lookup(id, sym);
   if (!s) {
      /* errx(1, "%s undefined", ident_str(id)); */
      fprintf(stderr, "%s undefined\n", ident_str(id));
      exit(1);
   }
   if (s->type != SYM_VECTOR)
      return NULL;
   return s->vector;
}

