/* Copyright David Leonard & Andrew Janke, 2000. All rights reserved. */

#include <stdio.h>
#include <math.h>
#include "node.h"

scalar_t   eval_index(node_t, vector_t, scalar_t);
scalar_t   eval_sum(node_t, vector_t);
vector_t   eval_vector(node_t, sym_t);
vector_t   gen_vector(node_t, sym_t);
vector_t   gen_range(node_t, sym_t);

void eval_error(node_t n, const char *msg){
   int pos = n->pos;
   show_error(pos, msg);
}

void show_error(int pos, const char *msg){
   extern const char *expression;
   const char *c;

   if (pos != -1) {
      fprintf(stderr, "%s\n", expression);
      for (c = expression; *c; c++) {
         if (pos-- == 0)
            break;
         if (*c == '\t') fprintf(stderr, "\t");
         else          fprintf(stderr, " ");
         }
      fprintf(stderr, "^\n");
      }
   fprintf(stderr, "%s\n", msg);
   exit(1);
   }

/* Try to evaluate an expression in a scalar context */
scalar_t eval_scalar(node_t n, sym_t sym){
   vector_t v;
   scalar_t s;
   sym_t y;

   switch (n->type) {
   case NODETYPE_ADD:
      return eval_scalar(n->expr[0], sym) + eval_scalar(n->expr[1], sym);
      
   case NODETYPE_SUB:
      return eval_scalar(n->expr[0], sym) - eval_scalar(n->expr[1], sym);
      
   case NODETYPE_MUL:
      return eval_scalar(n->expr[0], sym) * eval_scalar(n->expr[1], sym);
      
   case NODETYPE_DIV:
      return eval_scalar(n->expr[0], sym) / eval_scalar(n->expr[1], sym);
      
   case NODETYPE_EXP:
      return pow(eval_scalar(n->expr[0], sym), 
            eval_scalar(n->expr[1], sym));
      
   case NODETYPE_INDEX:
      v = eval_vector(n->expr[0], sym);
      s = eval_index(n, v, eval_scalar(n->expr[1], sym));
      vector_free(v);
      return s;
      
   case NODETYPE_SUM:
      v = eval_vector(n->expr[0], sym);
      s = eval_sum(n, v);
      vector_free(v);
      return s;
      
   case NODETYPE_AVG:
      v = eval_vector(n->expr[0], sym);
      s = eval_sum(n, v) / (scalar_t)v->len;
      vector_free(v);
      return s;
      
   case NODETYPE_LEN:
      v = eval_vector(n->expr[0], sym);
      s = (scalar_t) v->len;
      vector_free(v);
      return s;
      
   case NODETYPE_IDENT:
      return sym_lookup_scalar(n->ident, sym);
      
   case NODETYPE_REAL:
      return n->real;
      
   case NODETYPE_LET:
      if (ident_is_scalar(n->ident)) {
         y = sym_enter_scalar(
             eval_scalar(n->expr[0], sym), n->ident, sym);
         s = eval_scalar(n->expr[1], y);
         sym_leave(y);
         return s;
      } else {
         v = eval_vector(n->expr[0], sym);
         y = sym_enter_vector(v, n->ident, sym);
         s = eval_scalar(n->expr[1], y);
         sym_leave(y);
         vector_free(v);
         return s;
      }

   default:
      eval_error(n, "expected a scalar value");
      /* NOTREACHED */
      return 0;
   }
}

/* Index into a vector */
scalar_t eval_index(node_t n, vector_t v, scalar_t i){
   int idx = SCALAR_ROUND(i);
   if (idx < 0 || idx > v->len)
      eval_error(n, "index out of bounds");
   return v->el[idx];
}

/* Perform a sum over the arguments */
scalar_t eval_sum(node_t n, vector_t v){
   int i;
   scalar_t result;

   result = 0;
   for (i = 0; i < v->len; i++)
      result += eval_index(n, v, i);
   return result;
}

/* Evaluate an expression in a vector context */
vector_t eval_vector(node_t n, sym_t sym){
   vector_t v, v2;
   scalar_t s;
   sym_t y;

   switch (n->type) {
   case NODETYPE_LET:
      if (ident_is_scalar(n->ident)) {
         y = sym_enter_scalar(eval_scalar(n->expr[0], sym), 
            n->ident, sym);
         v = eval_vector(n->expr[1], y);
         sym_leave(y);
         return v;
      } else {
         v2 = eval_vector(n->expr[0], sym);
         y = sym_enter_vector(v2, n->ident, sym);
         v = eval_vector(n->expr[1], y);
         sym_leave(y);
         vector_free(v2);
         return v;
      }

   case NODETYPE_VEC2:
      v = eval_vector(n->expr[0], sym);
      s = eval_scalar(n->expr[1], sym);
      vector_append(v, s);
      return v;

   case NODETYPE_VEC1:
      s = eval_scalar(n->expr[0], sym);
      v = new_vector();
      vector_append(v, s);
      return v;

   case NODETYPE_GEN:
      return gen_vector(n, sym);

   case NODETYPE_RANGE:
      return gen_range(n, sym);

   case NODETYPE_IDENT:
      v = sym_lookup_vector(n->ident, sym);
      if (v) {
         v->refcnt++;
         return v;
      }
      /* fallthrough */
   default:
      /* XXX coerce scalar to vector! */
      v = new_vector();
      vector_append(v, eval_scalar(n, sym));
      return v;
   }
}

/* Generate a vector */
vector_t gen_vector(node_t n, sym_t sym){
   int i;
   scalar_t value;
   ident_t ident;
   node_t expr;
   sym_t s;
   vector_t v;
   vector_t els;

   ident = n->ident;
   if (!ident_is_scalar(ident))
      eval_error(n, "expected scalar (lowercase) index as 1st arg");
   els = eval_vector(n->expr[0], sym);
   expr = n->expr[1];
   v = new_vector();

   for (i = 0; i < els->len; i++) {
      s = sym_enter_scalar((scalar_t)i, ident, sym);
      value = eval_scalar(expr, s);
      sym_leave(s);
      vector_append(v, value);
   }
   return v;
}

vector_t gen_range(node_t n, sym_t sym){
   int i;
   scalar_t start;
   scalar_t stop;
   vector_t v;

   v = new_vector();
   start = SCALAR_ROUND(eval_scalar(n->expr[0], sym));
   stop = SCALAR_ROUND(eval_scalar(n->expr[1], sym));

   if (!(n->flags & RANGE_EXACT_LOWER))
      start++;
   if (!(n->flags & RANGE_EXACT_UPPER))
      stop--;

   for (i = start; i <= stop; i++) {
      vector_append(v, (scalar_t)i);
   }

   return v;

}
