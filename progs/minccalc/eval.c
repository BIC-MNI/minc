/* Copyright David Leonard & Andrew Janke, 2000. All rights reserved. */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include "node.h"

#ifndef TRUE
#  define TRUE 1
#endif

#ifndef FALSE
#  define FALSE 0
#endif

#define INVALID_VALUE -DBL_MAX

scalar_t   eval_index(node_t, vector_t, scalar_t);
scalar_t   eval_sum(node_t, vector_t);
scalar_t   eval_prod(node_t, vector_t);
scalar_t   eval_max(node_t, vector_t, double);
vector_t   eval_vector(node_t, sym_t);
vector_t   gen_vector(node_t, sym_t);
vector_t   gen_range(node_t, sym_t);
scalar_t   for_loop(node_t n, sym_t sym);

extern int debug;
extern int propagate_nan;

void eval_error(node_t n, const char *msg){
   int pos = n->pos;
   show_error(pos, msg);
}

void show_error(int pos, const char *msg){
   extern const char *expression;
   const char *c;
   int thisline, ichar, linenum;

   if (pos != -1) {
      thisline = 0;
      linenum=1;
      for (ichar=0; ichar < pos; ichar++) {
         if (expression[ichar] == '\n') {
            thisline = ichar+1;
            linenum++;
         }
      }
      pos -= thisline;
      fprintf(stderr, "\nLine %d:\n", linenum);
      for (c = &expression[thisline]; *c && *c != '\n'; c++) {
         (void) putc((int) *c, stderr);
      }
      (void) putc((int) '\n', stderr);
      for (c = &expression[thisline]; *c; c++) {
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
   scalar_t vals[3];
   int found_invalid;
   int iarg;

   /* Check that node is of correct type */
   if (!node_is_scalar(n)) {
      eval_error(n, "Expression is not a scalar");
   }

   /* Check special case where all arguments are scalar and we can test
      for invalid values in a general way */
   if (n->flags & ALLARGS_SCALAR) {

      /* Check that we don't have too many arguments */
      if (n->numargs > (int) sizeof(vals)/sizeof(vals[0])) {
         eval_error(n, "Internal error: too many arguments");
      }

      /* Evaluate each argument and save the result, checking for invalid
         values. */
      found_invalid = FALSE;
      for (iarg=0; iarg < n->numargs; iarg++) {
         vals[iarg] = eval_scalar(n->expr[iarg], sym);
         if (vals[iarg] == INVALID_VALUE) {
            found_invalid = TRUE;
         }
      }

      /* Debug */
      if (debug) {
         (void) fprintf(stderr, "scalar %s:", node_name(n));
         for (iarg=0; iarg < n->numargs; iarg++)
            (void) fprintf(stderr, " %g", vals[iarg]);
         (void) fprintf(stderr, "\n");
      }

      /* Check for an invalid value. If we are testing for them, 
         return 1.0, otherwise return an invalid value. */
      if (found_invalid) {
         return ( (n->type == NODETYPE_ISNAN) ? 1.0 : INVALID_VALUE);
      }

      /* Do the operation */
      switch (n->type) {
      case NODETYPE_ADD:
         return vals[0] + vals[1];
   
      case NODETYPE_SUB:
         return vals[0] - vals[1];
   
      case NODETYPE_MUL:
         return vals[0] * vals[1];
   
      case NODETYPE_DIV:
         return vals[0] / vals[1];
   
      case NODETYPE_LT:
         return vals[0] < vals[1];

      case NODETYPE_LE:
         return vals[0] <= vals[1];

      case NODETYPE_GT:
         return vals[0] > vals[1];

      case NODETYPE_GE:
         return vals[0] >= vals[1];

      case NODETYPE_EQ:
         return vals[0] == vals[1];

      case NODETYPE_NE:
         return vals[0] != vals[1];

      case NODETYPE_NOT:
         return ( ! (int) vals[0] );

      case NODETYPE_AND:
         return vals[0] && vals[1];

      case NODETYPE_OR:
         return vals[0] || vals[1];

      case NODETYPE_ISNAN:
         return 0.0;      /* We only get here if the value is valid */

      case NODETYPE_POW:
         return pow(vals[0], vals[1]);

      case NODETYPE_SQRT:
         return sqrt(vals[0]);

      case NODETYPE_ABS:
         return fabs(vals[0]);

      case NODETYPE_EXP:
         return exp(vals[0]);

      case NODETYPE_LOG:
         return log(vals[0]);

      case NODETYPE_SIN:
         return sin(vals[0]);

      case NODETYPE_COS:
         return cos(vals[0]);

      case NODETYPE_CLAMP:
         if (vals[0] < vals[1]) return vals[1];
         if (vals[0] > vals[2]) return vals[2];
         return vals[0];

      case NODETYPE_SEGMENT:
         return ( (vals[0] >= vals[1] && vals[0] <= vals[2]) ? 1.0 : 0.0);

      }  /* switch on type */

   } /* If all args are scalar */

   /* If we get here then we are not doing a simple scalar operation
      and we have to handle invalid values on a case-by-case basis. */

   switch (n->type) {
   case NODETYPE_EXPRLIST:
      if (node_is_scalar(n->expr[0])) {
         s = eval_scalar(n->expr[0], sym);
      }
      else {
         v = eval_vector(n->expr[0], sym);
         vector_free(v);
      }
      return eval_scalar(n->expr[1], sym);

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
      
   case NODETYPE_PROD:
      v = eval_vector(n->expr[0], sym);
      s = eval_prod(n, v);
      vector_free(v);
      return s;
      
   case NODETYPE_AVG:
      v = eval_vector(n->expr[0], sym);
      s = eval_sum(n, v);
      if (s != INVALID_VALUE)
         s /= (scalar_t)v->len;
      vector_free(v);
      return s;
      
   case NODETYPE_LEN:
      v = eval_vector(n->expr[0], sym);
      s = (scalar_t) v->len;
      vector_free(v);
      return s;
      
   case NODETYPE_MAX:
      v = eval_vector(n->expr[0], sym);
      s = eval_max(n, v, 1.0);
      vector_free(v);
      return s;
      
   case NODETYPE_MIN:
      v = eval_vector(n->expr[0], sym);
      s = eval_max(n, v, -1.0);
      vector_free(v);
      return s;
      
   case NODETYPE_FOR:
      return for_loop(n, sym);

   case NODETYPE_IDENT:
      return sym_lookup_scalar(n->ident, sym);
      
   case NODETYPE_REAL:
      return n->real;

   case NODETYPE_ASSIGN:
      s = eval_scalar(n->expr[0], sym);
      sym_set_scalar(s, n->ident, sym);
      return s;
      
   case NODETYPE_LET:
      if (ident_is_scalar(n->ident)) {
         sym_set_scalar(eval_scalar(n->expr[0], sym), n->ident, sym);
         s = eval_scalar(n->expr[1], sym);
         return s;
      } else {
         v = eval_vector(n->expr[0], sym);
         sym_set_vector(v, n->ident, sym);
         s = eval_scalar(n->expr[1], sym);
         vector_free(v);
         return s;
      }

   case NODETYPE_IFELSE:
      s = eval_scalar(n->expr[0], sym);
      if (s != 0.0) {
         s = eval_scalar(n->expr[1], sym);
      }
      else if (n->numargs > 2) {
         s = eval_scalar(n->expr[2], sym);
      }
      else {
         s = 0.0;
      }
      return s;
      
   default:
      eval_error(n, "expected a scalar value");
      /* NOTREACHED */
      return 0;
   }
}

/* Index into a vector */
scalar_t eval_index(node_t n, vector_t v, scalar_t i){
   int idx = SCALAR_ROUND(i);
   if (idx < 0 || idx >= v->len)
      eval_error(n, "index out of bounds");
   return v->el[idx];
}

/* Perform a sum over the arguments */
scalar_t eval_sum(node_t n, vector_t v){
   int i;
   scalar_t result, s;
   int found_invalid;

   result = 0;
   found_invalid = FALSE;
   for (i = 0; i < v->len; i++) {
      s = eval_index(n, v, i);
      if (s == INVALID_VALUE) found_invalid = TRUE;
      else result += s;
   }
   return (found_invalid && propagate_nan) ? INVALID_VALUE : result;
}

/* Perform a product over the arguments */
scalar_t eval_prod(node_t n, vector_t v){
   int i;
   scalar_t result, s;
   int found_invalid;

   result = 1.0;
   found_invalid = FALSE;
   for (i = 0; i < v->len; i++) {
      s = eval_index(n, v, i);
      if (debug) {
         (void) fprintf(stderr, "Prod of %g\n", s);
      }
      if (s == INVALID_VALUE) found_invalid = TRUE;
      else result *= s;
   }
   if (debug) {
      (void) fprintf(stderr, "Prod result = %g\n", result);
   }
   return (found_invalid && propagate_nan) ? INVALID_VALUE : result;
}

/* Find the maximum of a vector. Sign should be +1.0 for maxima search
   and -1.0 for minima search */
scalar_t eval_max(node_t n, vector_t v, double sign){
   int i;
   scalar_t result, s;

   result = INVALID_VALUE;
   for (i = 0; i < v->len; i++) {
      s = eval_index(n, v, i);
      if (s != INVALID_VALUE) {
         if (result == INVALID_VALUE || (sign*(s-result) > 0.0)) {
            result = s;
         }
      }
   }
   return result;
}

/* Evaluate an expression in a vector context */
vector_t eval_vector(node_t n, sym_t sym){
   vector_t v, v2;
   scalar_t s;

   /* Check that node is of correct type */
   if (node_is_scalar(n)) {
      eval_error(n, "Expression is not a vector");
   }

   switch (n->type) {
   case NODETYPE_EXPRLIST:
      if (node_is_scalar(n->expr[0])) {
         s = eval_scalar(n->expr[0], sym);
      }
      else {
         v = eval_vector(n->expr[0], sym);
         vector_free(v);
      }
      return eval_vector(n->expr[1], sym);

   case NODETYPE_ASSIGN:
      v = eval_vector(n->expr[0], sym);
      sym_set_vector(v, n->ident, sym);
      return v;
      
   case NODETYPE_LET:
      if (ident_is_scalar(n->ident)) {
         sym_set_scalar(eval_scalar(n->expr[0], sym), n->ident, sym);
         v = eval_vector(n->expr[1], sym);
         return v;
      } else {
         v2 = eval_vector(n->expr[0], sym);
         sym_set_vector(v2, n->ident, sym);
         v = eval_vector(n->expr[1], sym);
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

   case NODETYPE_IFELSE:
      s = eval_scalar(n->expr[0], sym);
      if (s != 0.0) {
         v = eval_vector(n->expr[1], sym);
      }
      else if (n->numargs > 2) {
         v = eval_vector(n->expr[2], sym);
      }
      else {
         v = new_vector();
      }
      return v;
      
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
   vector_t v;
   vector_t els;

   ident = n->ident;
   if (!ident_is_scalar(ident))
      eval_error(n, "expected scalar (lowercase) index as 1st arg");
   els = eval_vector(n->expr[0], sym);
   expr = n->expr[1];
   v = new_vector();

   for (i = 0; i < els->len; i++) {
      sym_set_scalar(eval_index(n, els, (scalar_t) i), ident, sym);
      value = eval_scalar(expr, sym);
      vector_append(v, value);
   }
   vector_free(els);

   return v;
}

/* Implement a for loop */
scalar_t for_loop(node_t n, sym_t sym){
   int i;
   scalar_t value;
   ident_t ident;
   node_t expr;
   vector_t els;

   ident = n->ident;
   if (!ident_is_scalar(ident))
      eval_error(n, "expected scalar (lowercase) index as 1st arg");
   els = eval_vector(n->expr[0], sym);
   expr = n->expr[1];

   for (i = 0; i < els->len; i++) {
      sym_set_scalar(eval_index(n, els, (scalar_t) i), ident, sym);
      value = eval_scalar(expr, sym);
   }
   vector_free(els);

   return i;
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
