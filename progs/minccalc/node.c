/* Copyright David Leonard and Andrew Janke, 2000. All rights reserved. */

#include <stdlib.h>
#include "node.h"

struct nodename { enum nodetype type; const char *name; } 
nodenames[] = {
        { NODETYPE_ADD,      "add" },
        { NODETYPE_SUB,      "sub" },
        { NODETYPE_MUL,      "mul" },
        { NODETYPE_DIV,      "div" },
        { NODETYPE_EXP,      "exp" },
        { NODETYPE_INDEX,    "index" },
        { NODETYPE_SUM,      "sum" },
        { NODETYPE_AVG,      "avg" },
        { NODETYPE_LEN,      "len" },
        { NODETYPE_IDENT,    "ident" },
        { NODETYPE_REAL,     "real" },
        { NODETYPE_LET,      "let" },
        { NODETYPE_VEC1,     "vec1" },
        { NODETYPE_VEC2,     "vec2" },
        { 0, NULL }
};

node_t new_node(){
   node_t n;
   n = malloc(sizeof *n);
   return n;
}

const char *node_name(node_t n){
   struct nodename *p;

   for (p = nodenames; p->name; p++)
      if (p->type == n->type)
         return p->name;
   return "unknown";
}
