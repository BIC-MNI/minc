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

      integer icv, rcode, mincid, mincid2, imgid, imgid2, child
      integer dim(MAXNCDIM)
      integer start(MAXNCDIM), count(MAXNCDIM)
      integer coord1(MAXNCDIM), coord2(MAXNCDIM)
      integer length
      double precision dval
      integer ival
      integer*2 sval
      byte bval
      character*(MI_MAX_ATTSTR_LEN) cval
      
      data start/MAXNCDIM*1/, count/MAXNCDIM*1/

      icv = icvcrt(rcode)
      call icvsti(icv, MI_ICV_TYPE, NCBYTE, rcode)
      call icvsts(icv, MI_ICV_SIGN, MI_UNSIGNED, rcode)
      call icvstd(icv, MI_ICV_VALID_MAX, 200.0D0, rcode)
      call icvstd(icv, MI_ICV_VALID_MIN, 10.0D0, rcode)

      mincid = nccre(filename, NCCLOB, rcode)

      dim(1) = ncddef(mincid, MIxspace, 2, rcode)
      dim(2) = ncddef(mincid, MIyspace, 2, rcode)

C
C     Test mistdv
C
      imgid = mistdv(mincid, MIimage, NCSHORT, 2, dim, rcode)

C
C     Test migrpv
C
      child = migrpv(mincid, MIpatient, rcode)

C
C     Test michld
C
      child = ncvdef(mincid, 'testvar', NCFLOAT, 1, dim(2), rcode)
      call michld(mincid, imgid, child, rcode)

C
C     Test miaptp and miagtp
C
      call miaptp(mincid, imgid, 'pointer', child, rcode)
      if (miagtp(mincid, imgid, 'pointer', rcode).ne.child)
     +     print *, 'Error in miaptp or miagtp'

C
C     Test mistcd and mitrcd
C
      call mistcd(2, 3, coord1, rcode)
      if ((coord1(1).ne.3).or.(coord1(2).ne.3)) 
     +     print *, 'Error in mistcd'
      coord1(1)=1
      coord1(2)=2
      call mitrcd(mincid, imgid, coord1, child, coord2, rcode)
      if (coord2(1).ne.2) print *, 'Error in mitrcd'

C
C     Test miagt and miagt1
C
      call ncapt(mincid, imgid, 'total_counts', NCFLOAT, 1, 163.0, 
     +           rcode)
      call miagt(mincid, imgid, 'total_counts', NCLONG, 1, ival, 
     +           length, rcode)
      if ((ival.ne.163).or.(length.ne.1)) print *, 'Error in miagt'
      ival = 0
      call miagt1(mincid, imgid, 'total_counts', NCLONG, ival, rcode)
      if (ival.ne.163) print *, 'Error in miagt1'

      call ncendf(mincid, rcode)

C
C     Test mivpt and mivgt
C
      call mivpt(mincid, imgid, start, count, NCFLOAT, MI_SIGNED,
     +           47.0, rcode)
      call mivgt(mincid, imgid, start, count, NCLONG, MI_UNSIGNED,
     +           ival, rcode)
      if (ival.ne. 47) print *, 'Error in mivpt/mivgt'

C
C     Test mivpt1 and mivgt1
C
      call mivpt1(mincid, imgid, start, NCFLOAT, MI_SIGNED,
     +           38.0, rcode)
      call mivgt1(mincid, imgid, start, NCLONG, MI_UNSIGNED,
     +           ival, rcode)
      if (ival.ne. 38) print *, 'Error in mivpt/mivgt'

C
C     test icvnda, icvdet and icvatt
C
      call icvnda(icv, mincid, imgid, rcode)
      call icvdet(icv, rcode)
      call icvatt(icv, mincid, imgid, rcode)

C
C     Test icvnq?
C
      call icvnqd(icv, MI_ICV_VALID_MAX, dval, rcode)
      if (dval.ne.200.0D0) print *, 'Error in icvnqd'
      call icvnqi(icv, MI_ICV_TYPE, ival, rcode)
      if (ival.ne.NCBYTE) print *, 'Error in icvnqi'
      call icvnqs(icv, MI_ICV_SIGN, cval, MI_MAX_ATTSTR_LEN, rcode)
      if (cval.ne.MI_UNSIGNED) print *, 'Error in icvnqs'

C
C     Test icvput
C
      bval = 10
      call icvput(icv, start, count, bval, rcode)
      call ncvgt1(mincid, imgid, start, sval, rcode)
      if (sval.ne.-32768) print *, 'Error in icvput'

C
C     Test icvget
C
      bval=0
      call icvget(icv, start, count, bval, rcode)
      if (bval.ne.10) print *, 'Error in icvget'

C
C     Test icvfre
C
      call icvfre(icv, rcode)

C
C     Test copy routines
C
      mincid2 = nccre('test2.mnc', NCCLOB, rcode)

      call micavd(mincid, mincid2, 1, imgid, rcode)

      imgid2 = micvd(mincid, imgid, mincid2, rcode)

      call micaat(mincid, child, mincid2, imgid2, rcode)

      call ncendf(mincid2, rcode)

      call micavv(mincid, mincid2, 1, imgid, rcode)

      call micvv(mincid, imgid, mincid2, imgid2, rcode)

 10   continue

      call ncclos(mincid, rcode)
      call ncclos(mincid2, rcode)
      

      stop
      end
