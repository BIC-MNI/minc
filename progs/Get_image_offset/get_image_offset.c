#include <minc.h>
#include <local_nc.h>

#define public

public long get_image_offset(int cdfid);

int main(int argc, char *argv[])
{
   int cdfid;
   long offset;

   /* Check arguments */
   if (argc != 2) {
      (void) fprintf(stderr, "Usage: %s <filename.mnc>\n", argv[0]);
      return -1;
   }

   /* Open minc file */
   cdfid = ncopen(argv[1], NC_NOWRITE);

   /* Get the offset */
   offset = get_image_offset(cdfid);
   if (offset == -1) {
      (void) fprintf(stderr, "Error getting offset to image in file %s\n",
                     argv[1]);
      return -1;
   }

   (void) printf("%ld\n", offset);

   return 0;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_image_offset
@INPUT      : path - name of minc file
@OUTPUT     : (none)
@RETURNS    : offset to image data in minc file or MI_ERROR (-1) if an
              error occurs.
@DESCRIPTION: Function to return the offset to the image data in a minc file.
              WARNING: This function may be hazardous to your health since
              it directly accesses NetCDF internals!!!!! Use at your own
              risk.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : September 9, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public long get_image_offset(int cdfid)
{
   int imgid;
   NC *handle;
   NC_var *vp;

   imgid = ncvarid(cdfid, MIimage);
   if (imgid == MI_ERROR) return MI_ERROR;

   handle = NC_check_id(cdfid);
   if (handle == NULL) return MI_ERROR;

   vp = NC_hlookupvar(handle, imgid);
   if (vp == NULL) return MI_ERROR;

   return (long) vp->begin;
}

