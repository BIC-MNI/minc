/*
 *	Copyright 1990, University Corporation for Atmospheric Research
 *      See netcdf/README file for copying and redistribution conditions.
 */
/* $Header: /private-cvsroot/minc/fortran/Attic/netcdf_jacket_code.c,v 1.1 1993-03-19 10:37:17 neelin Exp $ */

/* blank fill C string to make FORTRAN string */
static void
fcdcpy (fstring, fslen, sstring)
    char *fstring;		/* output string to be blank-filled */
    int fslen;			/* length of output string */
    char *sstring;		/* input string, null-terminated */
{
    int i, len = strlen(sstring);

    for (i = 0; i < len; i++)
	*(fstring + i) = *(sstring + i);
    for (i = len; i < fslen; i++)
	*(fstring + i) = ' ';
}


static void
reverse (array, length)
    int array[];		/* array to be reversed */
    int length;			/* length of array */
{
    int temp, i, j;

    for (i = 0, j = length - 1; i < j; i++, j--) {
	temp = array[i];
	array[i] = array[j];
	array[j] = temp;
    }
}


static void
revlongs (array, length)
    long array[];		/* array to be reversed */
    int length;			/* length of array */
{
    int i, j;
    long temp;

    for (i = 0, j = length - 1; i < j; i++, j--) {
	temp = array[i];
	array[i] = array[j];
	array[j] = temp;
    }
}


/* error handling function */
static void
handle_err (pname, rcode)
    char *pname;		/* procedure name */
    int rcode;			/* error return */
{
    extern void NCadvise();
    extern char *cdf_routine_name; /* routine name in error messages */

    cdf_routine_name = pname;
    (void) NCadvise(rcode, "string won't fit in CHARACTER variable provided");
}

/* copy function used to copy strings with embedded blanks */
static void
fstrncpy (target, source, maxlen)
    char *target;		/* space to be copied into */
    char *source;		/* string to be copied */
    int maxlen;			/* maximum length of *source */
{
    while (maxlen-- && *source != '\0')
	*target++ = *source++;
    *target = '\0';
}

/* copy function used to copy strings terminated with blanks */
static void
nstrncpy (target, source, maxlen)
    char *target;		/* space to be copied into */
    char *source;		/* string to be copied */
    int maxlen;			/* maximum length of *source */
{
    while (maxlen-- && *source != ' ')
	*target++ = *source++;
    *target = '\0';
}


/*
 * Compute product of dimensions.
 */
static long
dimprod (dims, ndims)
     long *dims;			/* list of dimensions */
     int ndims;			/* number of dimensions in list */
{
    long *ip;
    long prod = 1;

    for (ip = dims; ip < &dims[ndims]; ip++)
      prod *= *ip;
    return prod;
}


#ifdef FORTRAN_HAS_NO_BYTE
/*
 * Convert multi-dimensional array of bytes stored in ints to packed array of
 * bytes, in malloc'ed space.  Returns pointer to bytes or NULL if malloc
 * failed.
 */
static char *
itob(ints, dims, ndims)
     int *ints;			/* multi-dimensional array of integers */
     long *dims;			/* list of dimensions */
     int ndims;			/* number of dimensions in list */
{
    long iocount = dimprod (dims, ndims);	/* product of dimensions */
    char *bytes = (char *) malloc (iocount * sizeof (char));
    int *ip;
    char *bp = bytes;

    if (bytes != NULL)
      for (ip = ints; iocount > 0; iocount--)
	*bp++ = (char) *ip++;
    return bytes;
}
#endif /* FORTRAN_HAS_NO_BYTE */

#ifdef FORTRAN_HAS_NO_SHORT
/*
 * Convert multi-dimensional array of shorts stored in ints to packed array of
 * shorts, in malloc'ed space.  Returns pointer to shorts or NULL if malloc
 * failed.
 */
static short *
itos(ints, dims, ndims)
     int *ints;		/* multi-dimensional array of ints */
     long *dims;			/* list of dimensions */
     int ndims;			/* number of dimensions in list */
{
    long iocount = dimprod (dims, ndims);	/* product of dimensions */
    short *shorts = (short *) malloc (iocount * sizeof (short));
    int *ip;
    short *sp = shorts;

    if (shorts != NULL)
      for (ip = ints; iocount > 0; iocount--)
	*sp++ = (short) *ip++;
    return shorts;
}
#endif /* FORTRAN_HAS_NO_SHORT */

