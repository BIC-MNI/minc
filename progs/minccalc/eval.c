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

scalar_t   eval_index(int, int *, node_t, vector_t, scalar_t);
scalar_t   eval_sum(int, int *, node_t, vector_t);
scalar_t   eval_prod(int, int *, node_t, vector_t);
scalar_t   eval_max(int, int *, node_t, vector_t, double, int);
vector_t   eval_vector(int, int *, node_t, sym_t);
vector_t   gen_vector(int, int *, node_t, sym_t);
vector_t   gen_range(int, int *, node_t, sym_t);
scalar_t   for_loop(int, int *, node_t n, sym_t sym);

extern int debug;
extern int propagate_nan;
extern double value_for_illegal_operations;

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
scalar_t eval_scalar(int width, int *eval_flags, node_t n, sym_t sym){
   vector_t v;
   scalar_t s, s2, result;
   scalar_t args[3];
   double vals[3];
   int *eval_flags2, *isnan_flags;
   int found_invalid, all_true, all_false;
   int iarg, ivalue;

   /* Check that node is of correct type */
   if (!node_is_scalar(n)) {
      eval_error(n, "Expression is not a scalar");
   }

   /* Check special case where all arguments are scalar and we can test
      for invalid values in a general way */
   if (n->flags & ALLARGS_SCALAR) {

      /* Check that we don't have too many arguments */
      if (n->numargs > (int) sizeof(args)/sizeof(args[0])) {
         eval_error(n, "Internal error: too many arguments");
      }

      /* Evaluate each argument and save the result. */
      for (iarg=0; iarg < n->numargs; iarg++) {
         args[iarg] = eval_scalar(width, eval_flags, n->expr[iarg], sym);
      }

      /* Set up the result scalar. We re-use the first argument if
         no one else is using it. */
      if (n->numargs > 0 && args[0]->refcnt == 1) {
         result = args[0];
         scalar_incr_ref(result);
      }
      else {
         result = new_scalar(width);
      }

      /* Loop over all values in scalar */
      for (ivalue=0; ivalue < width; ivalue++) {

         /* Check the eval flag */
         if (eval_flags != NULL && !eval_flags[ivalue]) continue;

         /* Get the values, checking for invalid values. */
         found_invalid = FALSE;
         for (iarg=0; iarg < n->numargs; iarg++) {
            vals[iarg] = args[iarg]->vals[ivalue];
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
            result->vals[ivalue] = 
               ( (n->type == NODETYPE_ISNAN) ? 1.0 : INVALID_VALUE );
            continue;
         }

         /* Do the operation */
         switch (n->type) {
         case NODETYPE_ADD:
            result->vals[ivalue] = vals[0] + vals[1]; break;
   
         case NODETYPE_SUB:
            result->vals[ivalue] = vals[0] - vals[1]; break;
      
         case NODETYPE_MUL:
            result->vals[ivalue] = vals[0] * vals[1]; break;
      
         case NODETYPE_DIV:
            if (vals[1] == 0.0)
               result->vals[ivalue] = value_for_illegal_operations;
            else
               result->vals[ivalue] = vals[0] / vals[1];
            break;

         case NODETYPE_LT:
            result->vals[ivalue] = vals[0] < vals[1]; break;
   
         case NODETYPE_LE:
            result->vals[ivalue] = vals[0] <= vals[1]; break;
   
         case NODETYPE_GT:
            result->vals[ivalue] = vals[0] > vals[1]; break;
   
         case NODETYPE_GE:
            result->vals[ivalue] = vals[0] >= vals[1]; break;
   
         case NODETYPE_EQ:
            result->vals[ivalue] = vals[0] == vals[1]; break;
   
         case NODETYPE_NE:
            result->vals[ivalue] = vals[0] != vals[1]; break;
   
         case NODETYPE_NOT:
            result->vals[ivalue] = (vals[0] == 0.0); break;
   
         case NODETYPE_AND:
            result->vals[ivalue] = (vals[0] != 0.0) && (vals[1] != 0.0); 
            break;
   
         case NODETYPE_OR:
            result->vals[ivalue] = (vals[0] != 0.0) || (vals[1] != 0.0);
            break;
   
         case NODETYPE_ISNAN:
            /* We only get here if the value is valid */
            result->vals[ivalue] = 0.0; break;
   
         case NODETYPE_POW:
            result->vals[ivalue] = pow(vals[0], vals[1]); break;
   
         case NODETYPE_SQRT:
            if (vals[0] < 0.0)
               result->vals[ivalue] = value_for_illegal_operations;
            else 
               result->vals[ivalue] = sqrt(vals[0]);
            break;
   
         case NODETYPE_ABS:
            result->vals[ivalue] = fabs(vals[0]); break;
   
         case NODETYPE_EXP:
            result->vals[ivalue] = exp(vals[0]); break;
   
         case NODETYPE_LOG:
            if (vals[0] <= 0.0)
               result->vals[ivalue] = value_for_illegal_operations;
            else
               result->vals[ivalue] = log(vals[0]);
            break;
   
         case NODETYPE_SIN:
            result->vals[ivalue] = sin(vals[0]); break;
   
         case NODETYPE_COS:
            result->vals[ivalue] = cos(vals[0]); break;
         
         case NODETYPE_TAN:
            result->vals[ivalue] = tan(vals[0]); break;
   
         case NODETYPE_ASIN:
            result->vals[ivalue] = asin(vals[0]); break;
   
         case NODETYPE_ACOS:
            result->vals[ivalue] = acos(vals[0]); break;
         
         case NODETYPE_ATAN:
            result->vals[ivalue] = atan(vals[0]); break;
   
         case NODETYPE_CLAMP:
            if (vals[0] < vals[1]) result->vals[ivalue] = vals[1];
            else if (vals[0] > vals[2]) result->vals[ivalue] = vals[2];
            else result->vals[ivalue] = vals[0];
            break;
   
         case NODETYPE_SEGMENT:
            result->vals[ivalue] = 
               ( (vals[0] >= vals[1] && vals[0] <= vals[2]) ? 1.0 : 0.0);
            break;
   
         }  /* switch on type */

      }   /* Loop over values of scalar */

      /* Free the intermediate results */
      for (iarg=0; iarg < n->numargs; iarg++) {
         scalar_free(args[iarg]);
      }

      /* Return the result vector */
      return result;

   } /* If all args are scalar */

   /* If we get here then we are not doing a simple scalar operation
      and we have to handle invalid values on a case-by-case basis. */

   switch (n->type) {
   case NODETYPE_EXPRLIST:
      if (node_is_scalar(n->expr[0])) {
         s = eval_scalar(width, eval_flags, n->expr[0], sym);
         scalar_free(s);
      }
      else {
         v = eval_vector(width, eval_flags, n->expr[0], sym);
         vector_free(v);
      }
      return eval_scalar(width, eval_flags, n->expr[1], sym);

   case NODETYPE_INDEX:
      v = eval_vector(width, eval_flags, n->expr[0], sym);
      s = eval_scalar(width, eval_flags, n->expr[1], sym);
      result = eval_index(width, eval_flags, n, v, s);
      vector_free(v);
      scalar_free(s);
      return result;
      
   case NODETYPE_SUM:
      v = eval_vector(width, eval_flags, n->expr[0], sym);
      s = eval_sum(width, eval_flags, n, v);
      vector_free(v);
      return s;
      
   case NODETYPE_PROD:
      v = eval_vector(width, eval_flags, n->expr[0], sym);
      s = eval_prod(width, eval_flags, n, v);
      vector_free(v);
      return s;
      
   case NODETYPE_AVG:
      v = eval_vector(width, eval_flags, n->expr[0], sym);
      s = eval_sum(width, eval_flags, n, v);
      for (ivalue=0; ivalue < width; ivalue++) {
         if (eval_flags != NULL && !eval_flags[ivalue]) continue;
         if (s->vals[ivalue] != INVALID_VALUE)
            s->vals[ivalue] /= (double) v->len;
      }
      vector_free(v);
      return s;
      
   case NODETYPE_LEN:
      v = eval_vector(width, eval_flags, n->expr[0], sym);
      s = new_scalar(width);
      for (ivalue=0; ivalue < width; ivalue++) {
         if (eval_flags != NULL && !eval_flags[ivalue]) continue;
         s->vals[ivalue] = (double) v->len;
      }
      if (debug) {
         (void) fprintf(stderr, "len : %d\n", v->len);
      }
      vector_free(v);
      return s;
      
   case NODETYPE_MAX:
      v = eval_vector(width, eval_flags, n->expr[0], sym);
      s = eval_max(width, eval_flags, n, v, 1.0, 0);
      vector_free(v);
      return s;
      
   case NODETYPE_MIN:
      v = eval_vector(width, eval_flags, n->expr[0], sym);
      s = eval_max(width, eval_flags, n, v, -1.0, 0);
      vector_free(v);
      return s;
      
   case NODETYPE_IMAX:
      v = eval_vector(width, eval_flags, n->expr[0], sym);
      s = eval_max(width, eval_flags, n, v, 1.0, 1);
      vector_free(v);
      return s;
      
   case NODETYPE_IMIN:
      v = eval_vector(width, eval_flags, n->expr[0], sym);
      s = eval_max(width, eval_flags, n, v, -1.0, 1);
      vector_free(v);
      return s;
      
   case NODETYPE_FOR:
      return for_loop(width, eval_flags, n, sym);

   case NODETYPE_IDENT:
      s = sym_lookup_scalar(n->ident, sym);
      if (s) {
         scalar_incr_ref(s);
      }
      return s;
      
   case NODETYPE_REAL:
      s = new_scalar(width);
      for (ivalue=0; ivalue < width; ivalue++) {
         s->vals[ivalue] = n->real;
      }
      return s;

   case NODETYPE_ASSIGN:
      s = eval_scalar(width, eval_flags, n->expr[0], sym);
      sym_set_scalar(width, eval_flags, s, n->ident, sym);
      return s;
      
   case NODETYPE_LET:
      if (ident_is_scalar(n->ident)) {
         s = eval_scalar(width, eval_flags, n->expr[0], sym);
         sym_set_scalar(width, eval_flags, s, n->ident, sym);
         scalar_free(s);
      } else {
         v = eval_vector(width, eval_flags, n->expr[0], sym);
         sym_set_vector(width, eval_flags, v, n->ident, sym);
         vector_free(v);
      }
      s = eval_scalar(width, eval_flags, n->expr[1], sym);
      return s;

   case NODETYPE_IFELSE:
      /* Do the test */
      s = eval_scalar(width, eval_flags, n->expr[0], sym);

      /* Set the eval flags based on the results. Keep track of invalid
         data in the expression - we will not evaluate either part in that
         case. */
      eval_flags2 = malloc(sizeof(eval_flags[0]) * width);
      isnan_flags = malloc(sizeof(eval_flags[0]) * width);
      all_true = TRUE;
      all_false = TRUE;
      for (ivalue=0; ivalue < width; ivalue++) {
         isnan_flags[ivalue] = (s->vals[ivalue] == INVALID_VALUE);
         eval_flags2[ivalue] = ((eval_flags == NULL ? 1 : eval_flags[ivalue])
                                && (s->vals[ivalue] != 0.0)
                                && (!isnan_flags[ivalue]));
         if (eval_flags2[ivalue])
            all_false = FALSE;
         else
            all_true = FALSE;
      }
      scalar_free(s);
      if (all_true || all_false) {
         free(eval_flags2);
         eval_flags2 = NULL;
      }

      /* Evaluate the then part */
      s = NULL;
      if (!all_false) {
         s = eval_scalar(width, eval_flags2, n->expr[1], sym);
      }

      /* Evaluate the else part if needed - remember to invert the flags */
      s2 = NULL;
      if (!all_true && n->numargs > 2) {
         if (eval_flags2 != NULL) {
            for (ivalue=0; ivalue < width; ivalue++) 
               eval_flags2[ivalue] = 
                  !eval_flags2[ivalue] && !isnan_flags[ivalue];
         }
         s2 = eval_scalar(width, eval_flags2, n->expr[2], sym);
         if (eval_flags2 != NULL) {
            for (ivalue=0; ivalue < width; ivalue++) 
               eval_flags2[ivalue] = 
                  !eval_flags2[ivalue] && !isnan_flags[ivalue];
         }
      }

      /* Make sure that we have an answer */
      if (s == NULL) {
         if (s2 != NULL) {
            s = s2;
            s2 = NULL;
         }
         else {
            s = new_scalar(width);
            for (ivalue=0; ivalue < width; ivalue++)
               s->vals[ivalue] = 0.0;
         }
      }

      /* Merge the results */
      if (eval_flags2 != NULL) {
         for (ivalue=0; ivalue < width; ivalue++) {
            if (!eval_flags2[ivalue]) {
               s->vals[ivalue] = 
                  (n->numargs > 2 ? s2->vals[ivalue] : 0.0);
            }
         }
      }

      /* Mark appropriate invalid values */
      for (ivalue=0; ivalue < width; ivalue++) {
         if (isnan_flags[ivalue]) {
            s->vals[ivalue] = value_for_illegal_operations;
         }
      }
      

      /* Free things and return */
      if (s2 != NULL) scalar_free(s2);
      if (eval_flags2 != NULL) free(eval_flags2);
      if (isnan_flags != NULL) free(isnan_flags);
      return s;

   default:
      eval_error(n, "expected a scalar value");
      /* NOTREACHED */
      return 0;
   }
}

/* Index into a vector */
scalar_t eval_index(int width, int *eval_flags, 
                    node_t n, vector_t v, scalar_t i){
   scalar_t s;
   int idx;
   int ivalue;

   s = new_scalar(width);
   for (ivalue=0; ivalue < width; ivalue++) {
      if (eval_flags != NULL && !eval_flags[ivalue]) continue;
      idx = SCALAR_ROUND(i->vals[ivalue]);
      if (idx < 0 || idx >= v->len)
         eval_error(n, "index out of bounds");
      s->vals[ivalue] = v->el[idx]->vals[ivalue];
      if (debug) (void) fprintf(stderr, "Index [%d] = %g\n", 
                                idx, s->vals[ivalue]);
   }
   return s;
}

/* Perform a sum over the arguments */
scalar_t eval_sum(int width, int *eval_flags, node_t n, vector_t v)
{
   int i, ivalue;
   scalar_t result;
   double value;
   int found_invalid, found_valid;

   result = new_scalar(width);
   for (ivalue=0; ivalue < width; ivalue++) {
      if (eval_flags != NULL && !eval_flags[ivalue]) continue;
      result->vals[ivalue] = 0.0;
      found_invalid = found_valid = FALSE;
      for (i = 0; i < v->len; i++) {
         value = v->el[i]->vals[ivalue];
         if (value == INVALID_VALUE) 
            found_invalid = TRUE;
         else {
            result->vals[ivalue] += value;
            found_valid = TRUE;
         }
      }
      if ((found_invalid && propagate_nan) || !found_valid) {
         result->vals[ivalue] = value_for_illegal_operations;
      }
   }
   return result;
}

/* Perform a product over the arguments */
scalar_t eval_prod(int width, int *eval_flags, node_t n, vector_t v)
{
   int i, ivalue;
   scalar_t result;
   double value;
   int found_invalid, found_valid;

   result = new_scalar(width);
   for (ivalue=0; ivalue < width; ivalue++) {
      if (eval_flags != NULL && !eval_flags[ivalue]) continue;
      result->vals[ivalue] = 1.0;
      found_invalid = found_valid = FALSE;
      for (i = 0; i < v->len; i++) {
         value = v->el[i]->vals[ivalue];
         if (value == INVALID_VALUE) 
            found_invalid = TRUE;
         else {
            result->vals[ivalue] *= value;
            found_valid = TRUE;
         }
      }
      if ((found_invalid && propagate_nan) || !found_valid) {
         result->vals[ivalue] = value_for_illegal_operations;
      }
   }
   return result;
}

/* Find the maximum of a vector. Sign should be +1.0 for maxima search
   and -1.0 for minima search.
   type should be 0 for value and 1 for index */
scalar_t eval_max(int width, int *eval_flags, 
                  node_t n, vector_t v, double sign, int type)
{
   int i, ivalue;
   scalar_t result;
   double value, max, idx;

   result = new_scalar(width);
   for (ivalue=0; ivalue < width; ivalue++) {
      if (eval_flags != NULL && !eval_flags[ivalue]) continue;
      result->vals[ivalue] = max = INVALID_VALUE;
      for (i = 0; i < v->len; i++) {
         value = v->el[i]->vals[ivalue];
         if (value != INVALID_VALUE) {
            if (max == INVALID_VALUE || (sign*(value-max) > 0.0)) {
               max = value;
               idx = (double)i;
            }
         }
      }
      result->vals[ivalue] = (type == 0) ? max : idx;
   }
   return result;
}

/* Evaluate an expression in a vector context */
vector_t eval_vector(int width, int *eval_flags, node_t n, sym_t sym){
   vector_t v, v2;
   scalar_t s;
   int ivalue, iel;
   int *eval_flags2, *isnan_flags;
   int all_true, all_false;

   /* Check that node is of correct type */
   if (node_is_scalar(n)) {
      eval_error(n, "Expression is not a vector");
   }

   switch (n->type) {
   case NODETYPE_EXPRLIST:
      if (node_is_scalar(n->expr[0])) {
         s = eval_scalar(width, eval_flags, n->expr[0], sym);
         scalar_free(s);
      }
      else {
         v = eval_vector(width, eval_flags, n->expr[0], sym);
         vector_free(v);
      }
      return eval_vector(width, eval_flags, n->expr[1], sym);

   case NODETYPE_ASSIGN:
      v = eval_vector(width, eval_flags, n->expr[0], sym);
      sym_set_vector(width, eval_flags, v, n->ident, sym);
      return v;
      
   case NODETYPE_LET:
      if (ident_is_scalar(n->ident)) {
         s = eval_scalar(width, eval_flags, n->expr[0], sym);
         sym_set_scalar(width, eval_flags, s, n->ident, sym);
         scalar_free(s);
      } else {
         v = eval_vector(width, eval_flags, n->expr[0], sym);
         sym_set_vector(width, eval_flags, v, n->ident, sym);
         vector_free(v);
      }
      v = eval_vector(width, eval_flags, n->expr[1], sym);
      return v;

   case NODETYPE_VEC2:
      v = eval_vector(width, eval_flags, n->expr[0], sym);
      s = eval_scalar(width, eval_flags, n->expr[1], sym);
      vector_append(v, s);
      scalar_free(s);
      return v;

   case NODETYPE_VEC1:
      s = eval_scalar(width, eval_flags, n->expr[0], sym);
      v = new_vector();
      vector_append(v, s);
      scalar_free(s);
      return v;

   case NODETYPE_GEN:
      return gen_vector(width, eval_flags, n, sym);

   case NODETYPE_RANGE:
      return gen_range(width, eval_flags, n, sym);

   case NODETYPE_IFELSE:
      /* Do the test */
      s = eval_scalar(width, eval_flags, n->expr[0], sym);

      /* Set the eval flags based on the results */
      eval_flags2 = malloc(sizeof(eval_flags[0]) * width);
      isnan_flags = malloc(sizeof(eval_flags[0]) * width);
      all_true = TRUE;
      all_false = TRUE;
      for (ivalue=0; ivalue < width; ivalue++) {
         isnan_flags[ivalue] = (s->vals[ivalue] == INVALID_VALUE);
         eval_flags2[ivalue] = ((eval_flags == NULL ? 1 : eval_flags[ivalue])
                                && (s->vals[ivalue] != 0.0)
                                && (!isnan_flags[ivalue]));
         if (eval_flags2[ivalue])
            all_false = FALSE;
         else
            all_true = FALSE;
      }
      scalar_free(s);
      if (all_true || all_false) {
         free(eval_flags2);
         eval_flags2 = NULL;
      }

      /* Evaluate the then part */
      v = NULL;
      if (!all_false) {
         v = eval_vector(width, eval_flags2, n->expr[1], sym);
      }

      /* Evaluate the else part if needed - remember to invert the flags */
      v2 = NULL;
      if (!all_true && n->numargs > 2) {
         if (eval_flags2 != NULL) {
            for (ivalue=0; ivalue < width; ivalue++) 
               eval_flags2[ivalue] = 
                  !eval_flags2[ivalue] && !isnan_flags[ivalue];
         }
         v2 = eval_vector(width, eval_flags2, n->expr[2], sym);
         if (eval_flags2 != NULL) {
            for (ivalue=0; ivalue < width; ivalue++) 
               eval_flags2[ivalue] = 
                  !eval_flags2[ivalue] && !isnan_flags[ivalue];
         }
      }

      /* Make sure that we have an answer */
      if (v == NULL) {
         if (v2 != NULL) {
            v = v2;
            v2 = NULL;
         }
         else {
            v = new_vector();
         }
      }

      /* Merge the results */
      if (v2 != NULL && v->len != v2->len) {
         eval_error(n, "Vector expressions in if-else do not have the same length");
      }
      if (eval_flags2 != NULL) {
         for (ivalue=0; ivalue < width; ivalue++) {
            if (!eval_flags2[ivalue]) {
               for (iel=0; iel < v->len; iel++) {
                  v->el[iel]->vals[ivalue] = 
                     (n->numargs > 2 ? v2->el[iel]->vals[ivalue] : 0.0);
               }
            }
         }
      }

      /* Mark appropriate invalid values */
      for (ivalue=0; ivalue < width; ivalue++) {
         if (isnan_flags[ivalue]) {
            for (iel=0; iel < v->len; iel++) {
               v->el[iel]->vals[ivalue] = value_for_illegal_operations;
            }
         }
      }

      /* Free things and return */
      if (v2 != NULL) vector_free(v2);
      if (eval_flags2 != NULL) free(eval_flags2);
      if (isnan_flags != NULL) free(isnan_flags);
      return v;

   case NODETYPE_IDENT:
      v = sym_lookup_vector(n->ident, sym);
      if (v) {
         vector_incr_ref(v);
         return v;
      }
      /* fallthrough */
   default:
      /* XXX coerce scalar to vector! */
      v = new_vector();
      s = eval_scalar(width, eval_flags, n, sym);
      vector_append(v, s);
      scalar_free(s);
      return v;
   }
}

/* Generate a vector */
vector_t gen_vector(int width, int *eval_flags, node_t n, sym_t sym){
   int i;
   scalar_t value;
   ident_t ident;
   node_t expr;
   vector_t v;
   vector_t els;

   ident = n->ident;
   if (!ident_is_scalar(ident))
      eval_error(n, "expected scalar (lowercase) index as 1st arg");
   els = eval_vector(width, eval_flags, n->expr[0], sym);
   expr = n->expr[1];
   v = new_vector();

   for (i = 0; i < els->len; i++) {
      value = els->el[i];
      scalar_incr_ref(value);
      sym_set_scalar(width, eval_flags, value, ident, sym);
      scalar_free(value);
      value = eval_scalar(width, eval_flags, expr, sym);
      vector_append(v, value);
      scalar_free(value);
   }
   vector_free(els);

   return v;
}

/* Implement a for loop */
scalar_t for_loop(int width, int *eval_flags, node_t n, sym_t sym){
   int i, ivalue;
   scalar_t value;
   ident_t ident;
   node_t expr;
   vector_t els;

   ident = n->ident;
   if (!ident_is_scalar(ident))
      eval_error(n, "expected scalar (lowercase) index as 1st arg");
   els = eval_vector(width, eval_flags, n->expr[0], sym);
   expr = n->expr[1];

   for (i = 0; i < els->len; i++) {
      if (debug) {
         (void) fprintf(stderr, "For loop iteration %d\n", i);
      }
      value = els->el[i];
      scalar_incr_ref(value);
      sym_set_scalar(width, eval_flags, value, ident, sym);
      scalar_free(value);
      value = eval_scalar(width, eval_flags, expr, sym);
      scalar_free(value);
   }
   vector_free(els);

   value = new_scalar(width);
   for (ivalue=0; ivalue < width; ivalue++) {
      value->vals[ivalue] = (double) i;
   }
   return value;
}

vector_t gen_range(int width, int *eval_flags, node_t n, sym_t sym){
   int i, ivalue;
   scalar_t start;
   scalar_t stop;
   vector_t v;
   int length;

   v = new_vector();
   start = eval_scalar(width, eval_flags, n->expr[0], sym);
   stop = eval_scalar(width, eval_flags, n->expr[1], sym);

   for (ivalue = 0; ivalue < width; ivalue++) {

      if (eval_flags != NULL && !eval_flags[ivalue]) continue;

      start->vals[ivalue] = SCALAR_ROUND(start->vals[ivalue]);
      stop->vals[ivalue] = SCALAR_ROUND(stop->vals[ivalue]);

      if (!(n->flags & RANGE_EXACT_LOWER))
         start->vals[ivalue]++;
      if (!(n->flags & RANGE_EXACT_UPPER))
         stop->vals[ivalue]--;

      if (ivalue == 0) {
         length = stop->vals[ivalue] - start->vals[ivalue];
      }
      else if (length != (int) (stop->vals[ivalue] - start->vals[ivalue])) {
         eval_error(n, "Vectors must have same size in vector generator");
      }

   }
   length++;

   scalar_free(stop);

   for (i = 0; i < length ; i++) {
      stop = new_scalar(width);
      for (ivalue = 0; ivalue < width; ivalue++) {
         if (eval_flags != NULL && !eval_flags[ivalue]) continue;
         stop->vals[ivalue] = start->vals[ivalue] + i;
         if (debug) {
            (void) fprintf(stderr, "Range %d -> %d\n", 
                           i, (int) stop->vals[ivalue]);
         }
      }
      vector_append(v, stop);
      scalar_free(stop);
   }

   scalar_free(start);

   return v;

}
