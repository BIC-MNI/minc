/*
 *	Copyright 1990, University Corporation for Atmospheric Research
 *      See netcdf/README file for copying and redistribution conditions.
 */
/* $Header: /private-cvsroot/minc/fortran/Attic/netcdf_jacket_code.c,v 6.0 1997-09-12 13:24:34 neelin Rel $ */

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

