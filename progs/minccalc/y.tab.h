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
#define	PROD	262
#define	SUM	263
#define	LET	264
#define	NEG	265
#define	LEN	266
#define	MAX	267
#define	MIN	268
#define	ISNAN	269
#define	SQRT	270
#define	ABS	271
#define	EXP	272
#define	LOG	273
#define	SIN	274
#define	COS	275
#define	CLAMP	276
#define	SEGMENT	277
#define	LT	278
#define	LE	279
#define	GT	280
#define	GE	281
#define	EQ	282
#define	NE	283
#define	NOT	284
#define	AND	285
#define	OR	286
#define	IF	287
#define	ELSE	288
#define	FOR	289


extern YYSTYPE yylval;
