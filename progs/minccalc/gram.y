%{
#include <stdio.h>
#include <errno.h>
#include "node.h"
%}

%union{
int		pos;
node_t 		node;
float		real;
ident_t		ident;
}

%token		IN TO IDENT REAL AVG SUM LET NEG LEN

%type<ident>	IDENT 
%type<real>	REAL
%type<pos>	AVG SUM LET NEG LEN ISNAN
%type<pos>	IN TO
%type<pos>  NOT
%type<pos>  LT LE GT GE EQ NE
%type<pos>	'+' '-' '*' '/' '(' ')' '[' ']' '.' '=' '^' '{' '}' ',' '|'
%type<node>	expr letexpr vector

%right	'='
%right	LET
%left NOT
%left LT LE GT GE EQ NE
%left	'-' '+'
%left	'*' '/'
%left	NEG
%right	'^'
%right	AVG SUM LEN

%%

top	:	expr
		{ root = $1; }
	;

expr	:	'(' expr ')'
		{ $$ = $2; }

	|	'{' vector '}'
		{ $$ = $2; }

	|	'{' IDENT IN expr '|' expr '}'
		{ $$ = new_node(2);
		  $$->type = NODETYPE_GEN;
		  $$->pos = $7;
		  $$->ident = $2;
		  $$->expr[0] = $4;
		  $$->expr[1] = $6; }

	|	'(' expr ',' expr ')'
		{ $$ = new_node(2);
		  $$->type = NODETYPE_RANGE;
		  $$->flags = 0;
		  $$->pos = $5;
		  $$->expr[0] = $2;
		  $$->expr[1] = $4; }

	|	'(' expr ',' expr ']'
		{ $$ = new_node(2);
		  $$->type = NODETYPE_RANGE;
		  $$->flags = RANGE_EXACT_UPPER;
		  $$->pos = $5;
		  $$->expr[0] = $2;
		  $$->expr[1] = $4; }

	|	'[' expr ',' expr ')'
		{ $$ = new_node(2);
		  $$->type = NODETYPE_RANGE;
		  $$->flags = RANGE_EXACT_LOWER;
		  $$->pos = $5;
		  $$->expr[0] = $2;
		  $$->expr[1] = $4; }

	|	'[' expr ',' expr ']'
		{ $$ = new_node(2);
		  $$->type = NODETYPE_RANGE;
		  $$->flags = RANGE_EXACT_UPPER | RANGE_EXACT_LOWER;
		  $$->pos = $5;
		  $$->expr[0] = $2;
		  $$->expr[1] = $4; }

	|	expr '+' expr
		{ $$ = new_node(2);
		  $$->type = NODETYPE_ADD;
        $$->flags |= ALLARGS_SCALAR;
		  $$->pos = $2;
		  $$->expr[0] = $1;
		  $$->expr[1] = $3; }

	|	expr '-' expr
		{ $$ = new_node(2);
		  $$->type = NODETYPE_SUB;
        $$->flags |= ALLARGS_SCALAR;
		  $$->pos = $2;
		  $$->expr[0] = $1;
		  $$->expr[1] = $3; }

	|	 '-' expr	%prec NEG
		{ $$ = new_node(2);
		  $$->type = NODETYPE_SUB;
		  $$->pos = $1;
        $$->flags |= ALLARGS_SCALAR;
		  $$->expr[0] = new_node(0);
		  $$->expr[0]->type = NODETYPE_REAL;
		  $$->expr[0]->real = 0.0;
		  $$->expr[1] = $2; }

	|	expr '*' expr
		{ $$ = new_node(2);
		  $$->pos = $2;
		  $$->type = NODETYPE_MUL;
        $$->flags |= ALLARGS_SCALAR;
		  $$->expr[0] = $1;
		  $$->expr[1] = $3; }

	|	expr '/' expr
		{ $$ = new_node(2);
		  $$->pos = $2;
		  $$->type = NODETYPE_DIV;
        $$->flags |= ALLARGS_SCALAR;
		  $$->expr[0] = $1;
		  $$->expr[1] = $3; }

	|	expr '^' expr
		{ $$ = new_node(2);
		  $$->pos = $2;
		  $$->type = NODETYPE_EXP;
        $$->flags |= ALLARGS_SCALAR;
		  $$->expr[0] = $1;
		  $$->expr[1] = $3; }

	|	expr LT expr
		{ $$ = new_node(2);
		  $$->type = NODETYPE_LT;
        $$->flags |= ALLARGS_SCALAR;
		  $$->pos = $2;
		  $$->expr[0] = $1;
		  $$->expr[1] = $3; }

	|	expr LE expr
		{ $$ = new_node(2);
		  $$->type = NODETYPE_LE;
        $$->flags |= ALLARGS_SCALAR;
		  $$->pos = $2;
		  $$->expr[0] = $1;
		  $$->expr[1] = $3; }

	|	expr GT expr
		{ $$ = new_node(2);
		  $$->type = NODETYPE_GT;
        $$->flags |= ALLARGS_SCALAR;
		  $$->pos = $2;
		  $$->expr[0] = $1;
		  $$->expr[1] = $3; }

	|	expr GE expr
		{ $$ = new_node(2);
		  $$->type = NODETYPE_GE;
        $$->flags |= ALLARGS_SCALAR;
		  $$->pos = $2;
		  $$->expr[0] = $1;
		  $$->expr[1] = $3; }

	|	expr EQ expr
		{ $$ = new_node(2);
		  $$->type = NODETYPE_EQ;
        $$->flags |= ALLARGS_SCALAR;
		  $$->pos = $2;
		  $$->expr[0] = $1;
		  $$->expr[1] = $3; }

	|	expr NE expr
		{ $$ = new_node(2);
		  $$->type = NODETYPE_NE;
        $$->flags |= ALLARGS_SCALAR;
		  $$->pos = $2;
		  $$->expr[0] = $1;
		  $$->expr[1] = $3; }

	|	expr '[' expr ']'
		{ $$ = new_node(2);
		  $$->type = NODETYPE_INDEX;
        $$->flags |= ALLARGS_SCALAR;
		  $$->pos = $4;
		  $$->expr[0] = $1;
		  $$->expr[1] = $3; }

	|	LET letexpr 
		{ $$ = $2; }

	|	NOT expr
		{ $$ = new_node(1);
		  $$->pos = $1;
		  $$->type = NODETYPE_NOT;
        $$->flags |= ALLARGS_SCALAR;
		  $$->expr[0] = $2; }

	|	SUM expr
		{ $$ = new_node(1);
		  $$->pos = $1;
		  $$->type = NODETYPE_SUM;
		  $$->expr[0] = $2; }

	|	AVG expr
		{ $$ = new_node(1);
		  $$->pos = $1;
		  $$->type = NODETYPE_AVG;
		  $$->expr[0] = $2; }

	|	LEN expr
		{ $$ = new_node(1);
		  $$->pos = $1;
		  $$->type = NODETYPE_LEN;
		  $$->expr[0] = $2; }

	|	ISNAN expr
		{ $$ = new_node(1);
		  $$->pos = $1;
		  $$->type = NODETYPE_ISNAN;
        $$->flags |= ALLARGS_SCALAR;
		  $$->expr[0] = $2; }

	|	IDENT
		{ $$ = new_node(0);
		  $$->type = NODETYPE_IDENT;
		  $$->pos = -1;
		  $$->ident = $1; }
		
	|	REAL
		{ $$ = new_node(0);
		  $$->pos = -1;
		  $$->type = NODETYPE_REAL;
		  $$->real = $1; }
		
	;

	
letexpr	:	IDENT '=' expr ',' letexpr
		{ $$ = new_node(2);
		  $$->type = NODETYPE_LET;
		  $$->pos = $2;
		  $$->ident = $1;
		  $$->expr[0] = $3;
		  $$->expr[1] = $5; }
	|	IDENT '=' expr IN expr
		{ $$ = new_node(2);
		  $$->pos = $2;
		  $$->type = NODETYPE_LET;
		  $$->ident = $1;
		  $$->expr[0] = $3;
		  $$->expr[1] = $5; }
	;

vector	:	expr
		{ $$ = new_node(1);
		  $$->pos = $1->pos;
		  $$->type = NODETYPE_VEC1;
		  $$->expr[0] = $1; }
	|	vector ',' expr
		{ $$ = new_node(2);
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
