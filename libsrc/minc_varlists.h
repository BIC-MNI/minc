/* ----------------------------- MNI Header -----------------------------------
@NAME       : minc_varlists.h
@DESCRIPTION: Contains lists of minc variables for use by routines in file
              minc_convenience.c.
@METHOD     : Note that lists should be NULL terminated.
@CREATED    : Peter Neelin (August 7, 1992)
@MODIFIED   : 
@RCSID      : $Header: /private-cvsroot/minc/libsrc/minc_varlists.h,v 1.2 1992-12-01 14:04:59 neelin Exp $ MINC (MNI)
---------------------------------------------------------------------------- */

/* Variables containing list of standard dimension names and variable names */

/* List of dimension variables. Note that MIvector_dimension is not included
   since it should not have an associated variable. */
static char *dimvarlist[]={MIxspace, MIyspace, MIzspace, MItime, MItfrequency,
                           MIxfrequency, MIyfrequency, MIzfrequency, NULL};

/* List of dimension width variables */
static char *dimwidthlist[]={MIxspace_width, MIyspace_width, MIzspace_width,
                             MItime_width, MItfrequency_width, 
                             MIxfrequency_width, MIyfrequency_width,
                             MIzfrequency_width, NULL};

/* List of variables */
static char *varlist[]={MIrootvariable, MIimage, MIimagemax, MIimagemin, 
                        MIpatient, MIstudy, MIacquisition, NULL};

