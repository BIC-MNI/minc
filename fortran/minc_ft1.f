C------------------------------ MNI Header -----------------------------------
C@NAME       : minc_ftest.f
C@DESCRIPTION: Program to test the minc_jackets interface to the MINC library.
C@METHOD     : 
C@GLOBALS    : 
C@CALLS      : 
C@CREATED    : March 16, 1993 (Peter Neelin, Montreal Neurological Institute)
C@MODIFIED   : 
C-----------------------------------------------------------------------------
      program ftest
      implicit none

#include <netcdf.inc>
#include <minc.inc>

      parameter filename='test.mnc'

      integer icv, rcode, mincid, imgid
      integer start(MAXNCDIM), count(MAXNCDIM)
      byte vals
      
      data start/MAXNCDIM*1/, count/MAXNCDIM*1/

      icv = icvcrt(rcode)
      call icvsti(icv, MI_ICV_TYPE, NCBYTE, rcode)
      call icvsts(icv, MI_ICV_SIGN, MI_UNSIGNED, rcode)
      call icvstd(icv, MI_ICV_VALID_MAX, 200.0d0, rcode)
      call icvstd(icv, MI_ICV_VALID_MIN, 10.0d0, rcode)

      mincid = ncopn(filename, NCNOWRIT, rcode)

      imgid = ncvid(mincid, MIIMAGE, rcode)

      call icvatt(icv, mincid, imgid, rcode)

      start(1)=2
      call icvget(icv, start, count, vals, rcode)

      print *, 'val = ', vals

      stop
      end
