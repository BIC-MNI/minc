#include <minc.h>
#include <local_nc.h>

#define public

public long get_var_offset(int cdfid, char *variable);

int main(int argc, char *argv[])
{
   int cdfid;
   long offset;
   char *file;
   char *variable = MIimage;

   /* Check arguments */
   if (argc < 2 || argc > 3) {
      (void) fprintf(stderr, "Usage: %s [<varname>] <filename.mnc>\n", 
                     argv[0]);
      return -1;
   }
   else if (argc == 2) {
      file = argv[1];
   }
   else {
      variable = argv[1];
      file = argv[2];
   }

   /* Open minc file */
   cdfid = ncopen(file, NC_NOWRITE);

   /* Get the offset */
   offset = get_var_offset(cdfid, variable);
   if (offset == -1) {
      (void) fprintf(stderr, "Error getting offset to %s in file %s\n",
                     variable, file);
      return -1;
   }

   (void) printf("%ld\n", offset);

   return 0;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_var_offset
@INPUT      : path - name of minc file
              variable - name of variable to locate
@OUTPUT     : (none)
@RETURNS    : offset to variable data in minc file or MI_ERROR (-1) if an
              error occurs.
@DESCRIPTION: Function to return the offset to the image data in a minc file.
              WARNING: This function may be hazardous to your health since
              it directly accesses NetCDF internals!!!!! Use at your own
              risk.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : September 9, 1994 (Peter Neelin)
@MODIFIED   : August 23, 2001 (P.N.)
                 - allow use with other variables
---------------------------------------------------------------------------- */
public long get_var_offset(int cdfid, char *variable)
{
   int imgid;
   NC *handle;
   NC_var *vp;

   imgid = ncvarid(cdfid, variable);
   if (imgid == MI_ERROR) return MI_ERROR;

   handle = NC_check_id(cdfid);
   if (handle == NULL) return MI_ERROR;

   vp = NC_hlookupvar(handle, imgid);
   if (vp == NULL) return MI_ERROR;

   return (long) vp->begin;
}

