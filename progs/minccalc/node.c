/* Copyright David Leonard and Andrew Janke, 2000. All rights reserved. */

#include <stdlib.h>
#include "node.h"

struct nodename { enum nodetype type; const char *name; } 
nodenames[] = {
        { NODETYPE_ADD,      "add" },
        { NODETYPE_SUB,      "sub" },
        { NODETYPE_MUL,      "mul" },
        { NODETYPE_DIV,      "div" },
        { NODETYPE_POW,      "pow" },
        { NODETYPE_INDEX,    "index" },
        { NODETYPE_SUM,      "sum" },
        { NODETYPE_AVG,      "avg" },
        { NODETYPE_LEN,      "len" },
        { NODETYPE_MAX,      "max" },
        { NODETYPE_MIN,      "min" },
        { NODETYPE_IDENT,    "ident" },
        { NODETYPE_REAL,     "real" },
        { NODETYPE_LET,      "let" },
        { NODETYPE_VEC1,     "vec1" },
        { NODETYPE_VEC2,     "vec2" },
        { NODETYPE_GEN,      "gen" },
        { NODETYPE_RANGE,    "range" },
        { NODETYPE_LT,       "lt" },
        { NODETYPE_LE,       "le" },
        { NODETYPE_GT,       "gt" },
        { NODETYPE_GE,       "ge" },
        { NODETYPE_EQ,       "eq" },
        { NODETYPE_NE,       "ne" },
        { NODETYPE_NOT,      "not" },
        { NODETYPE_AND,      "and" },
        { NODETYPE_OR,       "or" },
        { NODETYPE_ISNAN,    "isnan" },
        { NODETYPE_SQRT,     "sqrt" },
        { NODETYPE_ABS,      "abs" },
        { NODETYPE_EXP,      "exp" },
        { NODETYPE_LOG,      "log" },
        { NODETYPE_SIN,      "sin" },
        { NODETYPE_COS,      "cos" },
        { NODETYPE_CLAMP,    "clamp" },
        { NODETYPE_SEGMENT,  "segment" },
        { 0, NULL }
};

node_t new_node(int numargs){
   node_t n;
   n = malloc(sizeof *n);
   n->numargs = numargs;
   n->flags = 0;
   return n;
}

const char *node_name(node_t n){
   struct nodename *p;

   for (p = nodenames; p->name; p++)
      if (p->type == n->type)
         return p->name;
   return "unknown";
}
