typedef union{
int      pos;
node_t   node;
float    real;
ident_t  ident;
} YYSTYPE;
#define	IN	257
#define	TO	258
#define	IDENT	259
#define	REAL	260
#define	AVG	261
#define	SUM	262
#define	LET	263
#define	NEG	264
#define	LEN	265
#define	MAX	266
#define	MIN	267
#define	ISNAN	268
#define	SQRT	269
#define	ABS	270
#define	EXP	271
#define	LOG	272
#define	SIN	273
#define	COS	274
#define	CLAMP	275
#define	SEGMENT	276
#define	LT	277
#define	LE	278
#define	GT	279
#define	GE	280
#define	EQ	281
#define	NE	282
#define	NOT	283
#define	AND	284
#define	OR	285


extern YYSTYPE yylval;
