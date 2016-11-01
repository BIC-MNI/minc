/* ----------------------------- MNI Header -----------------------------------
@NAME       : mincconvert
@INPUT      : argc, argv - command line arguments
@OUTPUT     : (none)
@RETURNS    : status
@DESCRIPTION: Program to convert between MINC 1.0 and MINC 2.0 formats.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 2003-12-17
@MODIFIED   : 
 * $Log: mincconvert.c,v $
 * Revision 1.10  2008/01/17 02:33:02  rotor
 *  * removed all rcsids
 *  * removed a bunch of ^L's that somehow crept in
 *  * removed old (and outdated) BUGS file
 *
 * Revision 1.9  2008/01/12 19:08:15  stever
 * Add __attribute__ ((unused)) to all rcsid variables.
 *
 * Revision 1.8  2007/12/10 13:25:12  rotor
 *  * few more fixes for CMake build
 *  * started adding static to globals for a wierd zlib bug
 *
 * Revision 1.7  2007/08/09 17:05:25  rotor
 *  * added some fixes of Claudes for chunking and internal compression
 *
 * Revision 1.6  2006/07/28 16:49:55  baghdadi
 * added message to disallow conversion to itself and exit with success
 *
 * Revision 1.6  2006/06/21 11:30:00  Leila
 * added return value for main function
 * Revision 1.5  2006/04/10 11:30:00  Leila
 * check the version of file and Abort if converting to itself!
 * Revision 1.4  2005/08/26 21:07:17  bert
 * Use #if rather than #ifdef with MINC2 symbol, and be sure to include config.h whereever MINC2 is used
 *
 * Revision 1.3  2004/11/01 22:38:38  bert
 * Eliminate all references to minc_def.h
 *
 * Revision 1.2  2004/09/09 19:25:32  bert
 * Force V1 file format creation if -2 not specified
 *
 * Revision 1.1  2004/04/27 15:27:57  bert
 * Initial checkin, MINC 1 <-> MINC 2 converter
 *
 * 
@COPYRIGHT  :
              Copyright 2003 Robert Vincent, McConnell Brain Imaging Centre, 
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <minc.h>
#include <ParseArgv.h>
#include <time_stamp.h>

static int clobber = 0;
static int v2format = 0;
static int do_template = 0;
static int compress = -1;
static int chunking = -1;

ArgvInfo argTable[] = {
    {"-clobber", ARGV_CONSTANT, (char *) 1, (char *) &clobber, 
     "Overwrite existing file."},
    {"-2", ARGV_CONSTANT, (char *) 1, (char *) &v2format,
     "Create a MINC 2 output file."},
    {"-template", ARGV_CONSTANT, (char *) 1, (char *)&do_template,
     "Create a template file."},
    {"-compress", ARGV_INT, (char *) 1, (char *)&compress, 
     "Set the compression level, from 0 (disabled) to 9 (maximum)."},
    {"-chunk", ARGV_INT, (char *) 1, (char *)&chunking,
     "Set the target block size for chunking (-1 unknown, 0 default, >1 block size)."},
    {NULL, ARGV_END, NULL, NULL, NULL}
};

int
micopy(int old_fd, int new_fd, char *new_history, int is_template)
{
    if (is_template) {
        /* Tell NetCDF that we don't want to allocate the data until written.
         */
        ncsetfill(new_fd, NC_NOFILL);
    }

    /* Copy all variable definitions (and global attributes).
     */
    micopy_all_var_defs(old_fd, new_fd, 0, NULL);

    /* Append the updated history.
     */
    miappend_history(new_fd, new_history);

    if (!is_template) {
        ncendef(new_fd);
        micopy_all_var_values(old_fd, new_fd, 0, NULL);
    }
    else {
        /* This isn't really standard, but flag this as a template file. 
         */
        miattputstr(new_fd, NC_GLOBAL, "class", "template");
    }
    return (MI_NOERROR);
}

/* Main program */

int
main(int argc, char **argv)
{
    char *old_fname;
    char *new_fname;
    int old_fd;
    int new_fd;
    int flags;
#if MINC2
    struct mi2opts opts;
#endif /* MINC2 */

    char *new_history = time_stamp(argc, argv);

    /* Check arguments */
    if (ParseArgv(&argc, argv, argTable, 0) || (argc != 3)) {
        fprintf(stderr, 
                "\nUsage: %s [<options>] <infile> <outfile>\n", argv[0]);
        fprintf(stderr,
                "       %s -help\n\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    old_fname  = argv[1];
    new_fname = argv[2];

    old_fd = miopen(old_fname, NC_NOWRITE);
    if (old_fd < 0) {
        perror(old_fname);
        exit(EXIT_FAILURE);
    }
    
    /* check the version of file and Abort if converting to itself */
    if (MI2_ISH5OBJ(old_fd)) {
      if (v2format && compress <= 0 ) {
      
        /* If already a minc2 file, allow to do compression if
           requested by the user. Ideally, one should call the
           HDF5 function H5Pget_deflate to query the compression
           level of the input image, but this function does not
           seem to be supported anymore in HDF5. */

	fprintf(stderr,"Abort: Converting Version 2 (HDF5) to itself!! \n");
	exit(EXIT_FAILURE);
      }
    }
    else {
       if (!v2format) {
	 fprintf(stderr,"Abort: Converting Version 1 (netCDF) to itself!! \n");
	 exit(EXIT_FAILURE);
       }
    }
    
    flags = 0;

    if (clobber) {
        flags |= NC_CLOBBER;
    }
    else {
        flags |= NC_NOCLOBBER;
    }

#if MINC2
    if (v2format) {
        flags |= MI2_CREATE_V2;
    }
    else {
        flags |= MI2_CREATE_V1; /* Force V1 format */
    }

    opts.struct_version = MI2_OPTS_V1;
    if (compress == -1) {
        opts.comp_type = MI2_COMP_UNKNOWN;
    }
    else if (compress == 0) {
        opts.comp_type = MI2_COMP_NONE;
    }
    else {
        opts.comp_type = MI2_COMP_ZLIB;
        opts.comp_param = compress;
    }

    if (chunking == -1) {
        opts.chunk_type = MI2_CHUNK_UNKNOWN;
    }
    else if (chunking == 0) {
        opts.chunk_type = MI2_CHUNK_OFF;
    }
    else {
        opts.chunk_type = MI2_CHUNK_ON;
        opts.chunk_param = chunking;
    }

    new_fd = micreatex(new_fname, flags, &opts);
    if (new_fd < 0) {
        perror(new_fname);
        exit(EXIT_FAILURE);
    }
#else
    new_fd = micreate(new_fname, flags);
#endif /* not MINC2 */

    micopy(old_fd, new_fd, new_history, do_template);

    miclose(old_fd);
    miclose(new_fd);
    free(new_history);

    exit(EXIT_SUCCESS);
}
