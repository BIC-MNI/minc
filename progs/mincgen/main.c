/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /private-cvsroot/minc/progs/mincgen/main.c,v 1.6 2008-01-11 07:17:08 stever Exp $
 *********************************************************************/

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>

#ifdef __hpux
#include <locale.h>
#endif
    
#include <minc.h>
#include <ParseArgv.h>

#include "generic.h"
#include "ncgen.h"
#include "genlib.h"

extern int	yyparse(void);

const char *progname;			/* for error messages */
const char *cdlname;

int c_flag;
int fortran_flag;
int netcdf_flag;
char *netcdf_name = NULL;	/* name of output netCDF file to write */

extern FILE *yyin;

static const char* ubasename ( const char* av0 );
static void usage ( void );
int main ( int argc, char** argv );


/* strip off leading path */
static const char *
ubasename(
	const char *av0)
{
	const char *logident ;
#ifdef VMS
#define SEP	']'
#endif
#ifdef MSDOS
#define SEP	'\\'
#endif
#ifndef SEP
#define SEP	'/'
#endif
	if ((logident = strrchr(av0, SEP)) == NULL)
		logident = av0 ;
	else
	    logident++ ;
	return logident ;
}


static void usage(void)
{
    derror("Usage: %s [ -b ] [ -c ] [ -f ] [ -o outfile]  [ file ... ]",
	   progname);
    derror("netcdf library version %s", nc_inq_libvers());
}


int
main(
	int argc,
	char *argv[])
{
    FILE *fp;
    static ArgvInfo argTable[] = {
        { "-b", ARGV_CONSTANT, (char *) 1, (char *) &netcdf_flag,
          "Select binary netCDF output" },
        { "-c", ARGV_CONSTANT, (char *) 1, (char *) &c_flag,
          "Select 'C' output" },
        { "-f", ARGV_CONSTANT, (char *) 1, (char *) &fortran_flag,
          "Select FORTRAN output" },
        { "-o", ARGV_STRING, (char *) 1, (char *) &netcdf_name,
          "Select name for netCDF output" },
        { NULL, ARGV_END, NULL, NULL, NULL }
    };

#ifdef __hpux
    setlocale(LC_CTYPE,"");
#endif
    
#ifdef MDEBUG
	malloc_debug(2) ;	/* helps find malloc/free errors on Sun */
#endif /* MDEBUG */

    progname = ubasename(argv[0]);
    cdlname = "-";

    c_flag = 0;
    fortran_flag = 0;
    netcdf_flag = 0;

#if _CRAYMPP && 0
    /* initialize CRAY MPP parallel-I/O library */
    (void) par_io_init(32, 32);
#endif

    if (ParseArgv(&argc, argv, argTable, 0)) {
        usage();
        return (8);
    }

    if (fortran_flag && c_flag) {
	derror("Only one of -c or -f may be specified");
	return(8);
    }

    if (netcdf_name != NULL) {
      netcdf_flag = 1;
    }

    argc -= 1;
    argv += 1;

    if (argc > 1) {
	derror ("%s: only one input file argument permitted",progname);
	return(6);
    }

    fp = stdin;
    if (argc > 0 && strcmp(argv[0], "-") != 0) {
	if ((fp = fopen(argv[0], "r")) == NULL) {
	    derror ("can't open file %s for reading: ", argv[0]);
	    perror("");
	    return(7);
	}
	cdlname = argv[0];
    }
    yyin = fp;
    return (yyparse());
}
