#include "errx.h"

struct node;
struct vector;
struct sym;

typedef double   scalar_t;
typedef int      ident_t;
typedef struct node    *node_t;
typedef struct vector  *vector_t;
typedef struct sym     *sym_t;

#define SCALAR_ROUND(s)   (floorf(s + 0.5))

struct vector {
   int      len;
   scalar_t *el;
   int      maxlen;
   int      refcnt;
};

enum nodetype {
   NODETYPE_ADD, 
   NODETYPE_SUB, 
   NODETYPE_MUL, 
   NODETYPE_DIV, 
   NODETYPE_POW, 
   NODETYPE_INDEX, 
   NODETYPE_SUM, 
   NODETYPE_AVG, 
   NODETYPE_LEN, 
   NODETYPE_MAX, 
   NODETYPE_MIN, 
   NODETYPE_IDENT, 
   NODETYPE_REAL, 
   NODETYPE_LET,
   NODETYPE_VEC1,
   NODETYPE_VEC2,
   NODETYPE_GEN,
   NODETYPE_RANGE,
   NODETYPE_LT,
   NODETYPE_LE,
   NODETYPE_GT,
   NODETYPE_GE,
   NODETYPE_EQ,
   NODETYPE_NE,
   NODETYPE_NOT,
   NODETYPE_AND,
   NODETYPE_OR,
   NODETYPE_ISNAN,
   NODETYPE_SQRT,
   NODETYPE_ABS,
   NODETYPE_EXP,
   NODETYPE_LOG,
   NODETYPE_SIN,
   NODETYPE_COS,
   NODETYPE_CLAMP,
   NODETYPE_SEGMENT
};

#define RANGE_EXACT_UPPER   1
#define RANGE_EXACT_LOWER   2
#define ALLARGS_SCALAR      4

struct node {
   enum nodetype type;
   node_t expr[3];
   ident_t ident;
   int   flags;
   float real;
   int   pos;
   int   numargs;
};

ident_t       new_ident(const char *);
const char    *ident_str(ident_t);
int           ident_is_scalar(ident_t);

node_t      new_node(int numargs);
const char *   node_name(node_t);
node_t      optimize(node_t);

vector_t    new_vector(void);
void        vector_append(vector_t, scalar_t);
void        vector_free(vector_t);

sym_t       sym_enter_scalar(scalar_t, ident_t, sym_t);
sym_t       sym_enter_vector(vector_t, ident_t, sym_t);
sym_t       sym_leave(sym_t);
scalar_t    sym_lookup_scalar(ident_t, sym_t);
vector_t    sym_lookup_vector(ident_t, sym_t);

void       lex_init(const char *);
void       lex_finalize(void);

scalar_t   eval_scalar(node_t, sym_t);
void       show_error(int, const char *);

int      yyparse(void);
int      yylex(void);
extern node_t   root;

