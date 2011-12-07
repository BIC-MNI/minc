%{
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <float.h>
#include "node.h"

#define INVALID_VALUE (-DBL_MAX)

/* Avoid problems with conflicting declarations */
void yyerror(const char *msg);
%}

%union{
int      pos;
node_t   node;
double   real;
ident_t  ident;
}

%token      NAN
%token      IN TO IDENT REAL AVG PROD SUM LET NEG LEN MAX MIN IMAX IMIN
%token      ISNAN SQRT ABS EXP LOG SIN COS TAN ASIN ACOS ATAN CLAMP SEGMENT
%token      LT LE GT GE EQ NE NOT AND OR
%token      IF ELSE FOR

%type<ident>   IDENT 
%type<real>    REAL
%type<pos>     IN TO AVG SUM PROD LET NEG LEN IF ELSE FOR
%type<pos>     ISNAN SQRT ABS MAX MIN IMAX IMIN EXP LOG SIN COS TAN ASIN ACOS ATAN
%type<pos>     CLAMP SEGMENT
%type<pos>     NOT LT LE GT GE EQ NE AND OR
%type<pos>     '+' '-' '*' '/' '(' ')' '[' ']' '.' '=' '^' '{' '}' ',' '|' ';'
%type<pos>     ':' '?'
%type<node>    exprlist expr letexpr vector

%right   '='
%right   LET
%right   '?'
%left    OR
%left    AND
%left    EQ NE
%left    LT LE GT GE 
%left    '-' '+'
%left    '*' '/'
%right   NEG NOT
%right   '^'
%right   AVG SUM PROD LEN ISNAN SQRT ABS MAX MIN IMAX IMIN EXP LOG SIN COS TAN ASIN ACOS ATAN

%%

top   : exprlist
      { root = $1; }
   ;

exprlist : expr ';' exprlist
      { $$ = new_node(2, node_is_scalar($3));
        $$->type = NODETYPE_EXPRLIST;
        $$->pos = $2;
        $$->expr[0] = $1;
        $$->expr[1] = $3; }

   |   expr ';'
      { $$ = $1; }

   |   expr
      { $$ = $1; }
   ;

expr   :   '(' expr ')'
      { $$ = $2; }

   |   '{' exprlist '}'
      { $$ = $2; }

   |   '[' vector ']'
      { $$ = $2; }

   |   '{' IDENT IN expr '|' expr '}'
      { $$ = new_vector_node(2);
        $$->type = NODETYPE_GEN;
        $$->pos = $7;
        $$->ident = $2;
        $$->expr[0] = $4;
        $$->expr[1] = $6; }

   |   '(' expr ':' expr ')'
      { $$ = new_vector_node(2);
        $$->type = NODETYPE_RANGE;
        $$->flags = 0;
        $$->pos = $5;
        $$->expr[0] = $2;
        $$->expr[1] = $4; }

   |   '(' expr ':' expr ']'
      { $$ = new_vector_node(2);
        $$->type = NODETYPE_RANGE;
        $$->flags = RANGE_EXACT_UPPER;
        $$->pos = $5;
        $$->expr[0] = $2;
        $$->expr[1] = $4; }

   |   '[' expr ':' expr ')'
      { $$ = new_vector_node(2);
        $$->type = NODETYPE_RANGE;
        $$->flags = RANGE_EXACT_LOWER;
        $$->pos = $5;
        $$->expr[0] = $2;
        $$->expr[1] = $4; }

   |   '[' expr ':' expr ']'
      { $$ = new_vector_node(2);
        $$->type = NODETYPE_RANGE;
        $$->flags = RANGE_EXACT_UPPER | RANGE_EXACT_LOWER;
        $$->pos = $5;
        $$->expr[0] = $2;
        $$->expr[1] = $4; }

   |   expr '+' expr
      { $$ = new_scalar_node(2);
        $$->type = NODETYPE_ADD;
        $$->flags |= ALLARGS_SCALAR;
        $$->pos = $2;
        $$->expr[0] = $1;
        $$->expr[1] = $3; }

   |   expr '-' expr
      { $$ = new_scalar_node(2);
        $$->type = NODETYPE_SUB;
        $$->flags |= ALLARGS_SCALAR;
        $$->pos = $2;
        $$->expr[0] = $1;
        $$->expr[1] = $3; }

   |    '-' expr   %prec NEG
      { $$ = new_scalar_node(2);
        $$->type = NODETYPE_SUB;
        $$->pos = $1;
        $$->flags |= ALLARGS_SCALAR;
        $$->expr[0] = new_scalar_node(0);
        $$->expr[0]->type = NODETYPE_REAL;
        $$->expr[0]->real = 0.0;
        $$->expr[1] = $2; }

   |   expr '*' expr
      { $$ = new_scalar_node(2);
        $$->pos = $2;
        $$->type = NODETYPE_MUL;
        $$->flags |= ALLARGS_SCALAR;
        $$->expr[0] = $1;
        $$->expr[1] = $3; }

   |   expr '/' expr
      { $$ = new_scalar_node(2);
        $$->pos = $2;
        $$->type = NODETYPE_DIV;
        $$->flags |= ALLARGS_SCALAR;
        $$->expr[0] = $1;
        $$->expr[1] = $3; }

   |   expr '^' expr
      { $$ = new_scalar_node(2);
        $$->pos = $2;
        $$->type = NODETYPE_POW;
        $$->flags |= ALLARGS_SCALAR;
        $$->expr[0] = $1;
        $$->expr[1] = $3; }

   |   expr LT expr
      { $$ = new_scalar_node(2);
        $$->type = NODETYPE_LT;
        $$->flags |= ALLARGS_SCALAR;
        $$->pos = $2;
        $$->expr[0] = $1;
        $$->expr[1] = $3; }

   |   expr LE expr
      { $$ = new_scalar_node(2);
        $$->type = NODETYPE_LE;
        $$->flags |= ALLARGS_SCALAR;
        $$->pos = $2;
        $$->expr[0] = $1;
        $$->expr[1] = $3; }

   |   expr GT expr
      { $$ = new_scalar_node(2);
        $$->type = NODETYPE_GT;
        $$->flags |= ALLARGS_SCALAR;
        $$->pos = $2;
        $$->expr[0] = $1;
        $$->expr[1] = $3; }

   |   expr GE expr
      { $$ = new_scalar_node(2);
        $$->type = NODETYPE_GE;
        $$->flags |= ALLARGS_SCALAR;
        $$->pos = $2;
        $$->expr[0] = $1;
        $$->expr[1] = $3; }

   |   expr EQ expr
      { $$ = new_scalar_node(2);
        $$->type = NODETYPE_EQ;
        $$->flags |= ALLARGS_SCALAR;
        $$->pos = $2;
        $$->expr[0] = $1;
        $$->expr[1] = $3; }

   |   expr NE expr
      { $$ = new_scalar_node(2);
        $$->type = NODETYPE_NE;
        $$->flags |= ALLARGS_SCALAR;
        $$->pos = $2;
        $$->expr[0] = $1;
        $$->expr[1] = $3; }

   |   expr AND expr
      { $$ = new_scalar_node(2);
        $$->type = NODETYPE_AND;
        $$->flags |= ALLARGS_SCALAR;
        $$->pos = $2;
        $$->expr[0] = $1;
        $$->expr[1] = $3; }

   |   expr OR expr
      { $$ = new_scalar_node(2);
        $$->type = NODETYPE_OR;
        $$->flags |= ALLARGS_SCALAR;
        $$->pos = $2;
        $$->expr[0] = $1;
        $$->expr[1] = $3; }

   |   expr '[' expr ']'
      { $$ = new_scalar_node(2);
        $$->type = NODETYPE_INDEX;
        $$->pos = $4;
        $$->expr[0] = $1;
        $$->expr[1] = $3; }

   |   IDENT '=' expr
      { $$ = new_node(1, node_is_scalar($3));
        $$->type = NODETYPE_ASSIGN;
        $$->pos = $2;
        $$->ident = $1;
        $$->expr[0] = $3; }

   |   LET letexpr 
      { $$ = $2; }

   |   NOT expr
      { $$ = new_scalar_node(1);
        $$->pos = $1;
        $$->type = NODETYPE_NOT;
        $$->flags |= ALLARGS_SCALAR;
        $$->expr[0] = $2; }

   |   SUM expr
      { $$ = new_scalar_node(1);
        $$->pos = $1;
        $$->type = NODETYPE_SUM;
        $$->expr[0] = $2; }

   |   PROD expr
      { $$ = new_scalar_node(1);
        $$->pos = $1;
        $$->type = NODETYPE_PROD;
        $$->expr[0] = $2; }

   |   AVG expr
      { $$ = new_scalar_node(1);
        $$->pos = $1;
        $$->type = NODETYPE_AVG;
        $$->expr[0] = $2; }

   |   LEN expr
      { $$ = new_scalar_node(1);
        $$->pos = $1;
        $$->type = NODETYPE_LEN;
        $$->expr[0] = $2; }

   |   MAX expr
      { $$ = new_scalar_node(1);
        $$->pos = $1;
        $$->type = NODETYPE_MAX;
        $$->expr[0] = $2; }

   |   MIN expr
      { $$ = new_scalar_node(1);
        $$->pos = $1;
        $$->type = NODETYPE_MIN;
        $$->expr[0] = $2; }

   |   IMAX expr
      { $$ = new_scalar_node(1);
        $$->pos = $1;
        $$->type = NODETYPE_IMAX;
        $$->expr[0] = $2; }

   |   IMIN expr
      { $$ = new_scalar_node(1);
        $$->pos = $1;
        $$->type = NODETYPE_IMIN;
        $$->expr[0] = $2; }

   |   ISNAN expr
      { $$ = new_scalar_node(1);
        $$->pos = $1;
        $$->type = NODETYPE_ISNAN;
        $$->flags |= ALLARGS_SCALAR;
        $$->expr[0] = $2; }

   |   SQRT expr
      { $$ = new_scalar_node(1);
        $$->pos = $1;
        $$->type = NODETYPE_SQRT;
        $$->flags |= ALLARGS_SCALAR;
        $$->expr[0] = $2; }

   |   ABS expr
      { $$ = new_scalar_node(1);
        $$->pos = $1;
        $$->type = NODETYPE_ABS;
        $$->flags |= ALLARGS_SCALAR;
        $$->expr[0] = $2; }

   |   EXP expr
      { $$ = new_scalar_node(1);
        $$->pos = $1;
        $$->type = NODETYPE_EXP;
        $$->flags |= ALLARGS_SCALAR;
        $$->expr[0] = $2; }

   |   LOG expr
      { $$ = new_scalar_node(1);
        $$->pos = $1;
        $$->type = NODETYPE_LOG;
        $$->flags |= ALLARGS_SCALAR;
        $$->expr[0] = $2; }

   |   SIN expr
      { $$ = new_scalar_node(1);
        $$->pos = $1;
        $$->type = NODETYPE_SIN;
        $$->flags |= ALLARGS_SCALAR;
        $$->expr[0] = $2; }

   |   COS expr
      { $$ = new_scalar_node(1);
        $$->pos = $1;
        $$->type = NODETYPE_COS;
        $$->flags |= ALLARGS_SCALAR;
        $$->expr[0] = $2; }

   |   TAN expr
      { $$ = new_scalar_node(1);
        $$->pos = $1;
        $$->type = NODETYPE_TAN;
        $$->flags |= ALLARGS_SCALAR;
        $$->expr[0] = $2; }

   |   ASIN expr
      { $$ = new_scalar_node(1);
        $$->pos = $1;
        $$->type = NODETYPE_ASIN;
        $$->flags |= ALLARGS_SCALAR;
        $$->expr[0] = $2; }

   |   ACOS expr
      { $$ = new_scalar_node(1);
        $$->pos = $1;
        $$->type = NODETYPE_ACOS;
        $$->flags |= ALLARGS_SCALAR;
        $$->expr[0] = $2; }

   |   ATAN expr
      { $$ = new_scalar_node(1);
        $$->pos = $1;
        $$->type = NODETYPE_ATAN;
        $$->flags |= ALLARGS_SCALAR;
        $$->expr[0] = $2; }

   |   CLAMP '(' expr ',' expr ',' expr ')'
      { $$ = new_scalar_node(3);
        $$->pos = $1;
        $$->type = NODETYPE_CLAMP;
        $$->flags |= ALLARGS_SCALAR;
        $$->expr[0] = $3;
        $$->expr[1] = $5;
        $$->expr[2] = $7; }

   |   SEGMENT '(' expr ',' expr ',' expr ')'
      { $$ = new_scalar_node(3);
        $$->pos = $1;
        $$->type = NODETYPE_SEGMENT;
        $$->flags |= ALLARGS_SCALAR;
        $$->expr[0] = $3;
        $$->expr[1] = $5;
        $$->expr[2] = $7; }

   |   expr '?' expr ':' expr
      { $$ = new_node(3, node_is_scalar($3));
        $$->pos = $2;
        $$->type = NODETYPE_IFELSE;
        $$->expr[0] = $1;
        $$->expr[1] = $3;
        $$->expr[2] = $5; }

   |   IF '(' expr ')' expr ELSE expr
      { $$ = new_node(3, node_is_scalar($5));
        $$->pos = $1;
        $$->type = NODETYPE_IFELSE;
        $$->expr[0] = $3;
        $$->expr[1] = $5;
        $$->expr[2] = $7; }

   |   IF '(' expr ')' expr
      { $$ = new_node(2, node_is_scalar($5));
        $$->pos = $1;
        $$->type = NODETYPE_IFELSE;
        $$->expr[0] = $3;
        $$->expr[1] = $5; }

   |   FOR '{' IDENT IN expr '}' expr
      { $$ = new_scalar_node(2);
        $$->pos = $1;
        $$->type = NODETYPE_FOR;
        $$->ident = $3;
        $$->expr[0] = $5;
        $$->expr[1] = $7; }

   |   IDENT
      { $$ = new_node(0, ident_is_scalar($1));
        $$->type = NODETYPE_IDENT;
        $$->pos = -1;
        $$->ident = $1; }
      
   |   REAL
      { $$ = new_scalar_node(0);
        $$->pos = -1;
        $$->type = NODETYPE_REAL;
        $$->real = $1; }
      
   |   NAN
      { $$ = new_scalar_node(0);
        $$->pos = -1;
        $$->type = NODETYPE_REAL;
        $$->real = INVALID_VALUE; }
      
   ;

   
letexpr   :   IDENT '=' expr ',' letexpr
      { $$ = new_scalar_node(2);
        $$->type = NODETYPE_LET;
        $$->pos = $2;
        $$->ident = $1;
        $$->expr[0] = $3;
        $$->expr[1] = $5; }
   |   IDENT '=' expr IN expr
      { $$ = new_scalar_node(2);
        $$->pos = $2;
        $$->type = NODETYPE_LET;
        $$->ident = $1;
        $$->expr[0] = $3;
        $$->expr[1] = $5; }
   ;

vector   :   expr
      { $$ = new_vector_node(1);
        $$->pos = $1->pos;
        $$->type = NODETYPE_VEC1;
        $$->expr[0] = $1; }
   |   vector ',' expr
      { $$ = new_vector_node(2);
        $$->pos = $2;
        $$->type = NODETYPE_VEC2;
        $$->expr[0] = $1;
        $$->expr[1] = $3; }
   ;

%%

node_t root;

void
yyerror(msg)
   const char *msg;
{
   extern int lexpos;

   show_error(lexpos, msg);
}
