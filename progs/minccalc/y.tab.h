#ifndef BISON_Y_TAB_H
# define BISON_Y_TAB_H

#ifndef YYSTYPE
typedef union{
int      pos;
node_t   node;
double   real;
ident_t  ident;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
# define	NAN	257
# define	IN	258
# define	TO	259
# define	IDENT	260
# define	REAL	261
# define	AVG	262
# define	PROD	263
# define	SUM	264
# define	LET	265
# define	NEG	266
# define	LEN	267
# define	MAX	268
# define	MIN	269
# define	ISNAN	270
# define	SQRT	271
# define	ABS	272
# define	EXP	273
# define	LOG	274
# define	SIN	275
# define	COS	276
# define	TAN	277
# define	ASIN	278
# define	ACOS	279
# define	ATAN	280
# define	CLAMP	281
# define	SEGMENT	282
# define	LT	283
# define	LE	284
# define	GT	285
# define	GE	286
# define	EQ	287
# define	NE	288
# define	NOT	289
# define	AND	290
# define	OR	291
# define	IF	292
# define	ELSE	293
# define	FOR	294


extern YYSTYPE yylval;

#endif /* not BISON_Y_TAB_H */
