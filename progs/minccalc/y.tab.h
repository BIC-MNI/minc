typedef union{
int      pos;
node_t   node;
double   real;
ident_t  ident;
} YYSTYPE;
#define	NAN	257
#define	IN	258
#define	TO	259
#define	IDENT	260
#define	REAL	261
#define	AVG	262
#define	PROD	263
#define	SUM	264
#define	LET	265
#define	NEG	266
#define	LEN	267
#define	MAX	268
#define	MIN	269
#define	ISNAN	270
#define	SQRT	271
#define	ABS	272
#define	EXP	273
#define	LOG	274
#define	SIN	275
#define	COS	276
#define	CLAMP	277
#define	SEGMENT	278
#define	LT	279
#define	LE	280
#define	GT	281
#define	GE	282
#define	EQ	283
#define	NE	284
#define	NOT	285
#define	AND	286
#define	OR	287
#define	IF	288
#define	ELSE	289
#define	FOR	290


extern YYSTYPE yylval;
