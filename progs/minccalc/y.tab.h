typedef union{
int      pos;
node_t   node;
double   real;
ident_t  ident;
} YYSTYPE;
#define	NAN	258
#define	IN	259
#define	TO	260
#define	IDENT	261
#define	REAL	262
#define	AVG	263
#define	PROD	264
#define	SUM	265
#define	LET	266
#define	NEG	267
#define	LEN	268
#define	MAX	269
#define	MIN	270
#define	ISNAN	271
#define	SQRT	272
#define	ABS	273
#define	EXP	274
#define	LOG	275
#define	SIN	276
#define	COS	277
#define	TAN	278
#define	ASIN	279
#define	ACOS	280
#define	ATAN	281
#define	CLAMP	282
#define	SEGMENT	283
#define	LT	284
#define	LE	285
#define	GT	286
#define	GE	287
#define	EQ	288
#define	NE	289
#define	NOT	290
#define	AND	291
#define	OR	292
#define	IF	293
#define	ELSE	294
#define	FOR	295


extern YYSTYPE yylval;
