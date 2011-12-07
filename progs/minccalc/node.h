#include "errx.h"

struct node;
struct scalar;
struct vector;
struct sym;

typedef int      ident_t;
typedef struct node    *node_t;
typedef struct scalar  *scalar_t;
typedef struct vector  *vector_t;
typedef struct sym     *sym_t;

#define SCALAR_ROUND(s)   (floor(s + 0.5))

struct scalar {
   int      width;
   double   *vals;
   int      refcnt;
};

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
   NODETYPE_PROD, 
   NODETYPE_AVG, 
   NODETYPE_LEN, 
   NODETYPE_MAX, 
   NODETYPE_MIN, 
   NODETYPE_IMAX, 
   NODETYPE_IMIN, 
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
   NODETYPE_TAN,
   NODETYPE_ASIN,
   NODETYPE_ACOS,
   NODETYPE_ATAN,
   NODETYPE_CLAMP,
   NODETYPE_SEGMENT,
   NODETYPE_EXPRLIST,
   NODETYPE_ASSIGN,
   NODETYPE_IFELSE,
   NODETYPE_FOR
};

#define RANGE_EXACT_UPPER   1
#define RANGE_EXACT_LOWER   2
#define ALLARGS_SCALAR      4
#define NODE_IS_SCALAR      8

struct node {
   enum nodetype type;
   node_t expr[3];
   ident_t ident;
   int   flags;
   double real;
   int   pos;
   int   numargs;
};

ident_t       new_ident(const char *);
const char    *ident_str(ident_t);
int           ident_is_scalar(ident_t);
ident_t       ident_lookup(char *string);

node_t      new_node(int, int);
node_t      new_scalar_node(int);
node_t      new_vector_node(int);
const char *   node_name(node_t);
int         node_is_scalar(node_t);
node_t      optimize(node_t);

vector_t    new_vector(void);
void        vector_append(vector_t, scalar_t);
void        vector_free(vector_t);
void        vector_incr_ref(vector_t);

scalar_t    new_scalar(int);
void        scalar_free(scalar_t);
void        scalar_incr_ref(scalar_t);

sym_t sym_enter_scope(sym_t sym);
void sym_leave_scope(sym_t sym);
void sym_declare_ident(ident_t id, sym_t sym);
void sym_set_scalar(int, int *, scalar_t, ident_t, sym_t);
void sym_set_vector(int, int *, vector_t, ident_t, sym_t);
scalar_t sym_lookup_scalar(ident_t id, sym_t sym);
vector_t sym_lookup_vector(ident_t id, sym_t sym);

void       lex_init(const char *);
void       lex_finalize(void);

scalar_t   eval_scalar(int, int *, node_t, sym_t);
void       show_error(int, const char *);

int      yyparse(void);
int      yylex(void);
extern node_t   root;

