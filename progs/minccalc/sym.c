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

void sym_set_scalar(scalar_t sc, ident_t id, sym_t sym){
   sym_t newsym = sym_lookup(id, sym);
   if (newsym == NULL) {
      newsym = new_sym(id, sym);
      newsym->type = SYM_SCALAR;
   }
   if (newsym->type == SYM_UNKNOWN) newsym->type = SYM_SCALAR;
   if (newsym->type != SYM_SCALAR) {
      /* errx(1, "%s is not a scalar", ident_str(id)); */
      fprintf(stderr, "%s is not a scalar(lowercase)\n", ident_str(id));
      exit(1);
   }
   newsym->scalar = sc;
   return;
}

void sym_set_vector(vector_t v, ident_t id, sym_t sym){
   sym_t newsym = sym_lookup(id, sym);
   if (newsym == NULL) {
      newsym = new_sym(id, sym);
      newsym->type = SYM_VECTOR;
   }
   else if (newsym->type == SYM_VECTOR) {
      vector_free(newsym->vector);
   }
   if (newsym->type == SYM_UNKNOWN) newsym->type = SYM_VECTOR;
   if (newsym->type != SYM_VECTOR) {
      /* errx(1, "%s is not a vector", ident_str(id)); */
      fprintf(stderr, "%s is not a vector\n", ident_str(id));
      exit(1);
   }
   newsym->vector = v;
   v->refcnt++;
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

