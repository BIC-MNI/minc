/* ----------------------------- MNI Header -----------------------------------
@NAME       : minc_varlists.h
@DESCRIPTION: Contains lists of minc variables for use by routines in file
              minc_convenience.c.
@METHOD     : Note that lists should be NULL terminated.
@CREATED    : Peter Neelin (August 7, 1992)
@MODIFIED   : 
@COPYRIGHT  :
              Copyright 1993 Peter Neelin, McConnell Brain Imaging Centre, 
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
@RCSID      : $Header: /private-cvsroot/minc/libsrc/minc_varlists.h,v 1.3 1993-07-20 12:18:02 neelin Exp $ MINC (MNI)
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

