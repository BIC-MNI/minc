#define IN 257
#define TO 258
#define IDENT 259
#define REAL 260
#define AVG 261
#define SUM 262
#define LET 263
#define NEG 264
#define LEN 265
typedef union{
int		pos;
node_t 		node;
float		real;
ident_t		ident;
} YYSTYPE;
extern YYSTYPE yylval;
