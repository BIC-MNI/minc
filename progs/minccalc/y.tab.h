#define IN 257
#define TO 258
#define IDENT 259
#define REAL 260
#define AVG 261
#define SUM 262
#define LET 263
#define NEG 264
#define LEN 265
#define ISNAN 266
#define NOT 267
#define LT 268
#define LE 269
#define GT 270
#define GE 271
#define EQ 272
#define NE 273
typedef union{
int		pos;
node_t 		node;
float		real;
ident_t		ident;
} YYSTYPE;
extern YYSTYPE yylval;
