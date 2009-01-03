#! /bin/sh
#
# Test application of concatenated transforms.

set -e

# _t1.xfm is linear transform.
# _t2.xfm is inverse of _t1
# _t3.xfm is grid transform.
# _t4.xfm is inverse of _t3

cp $srcdir/t1.xfm _t1.xfm
../xfminvert -clobber _t1.xfm _t2.xfm
dd if=/dev/zero | ../rawtominc -vector 3 -byte -clobber _grid.mnc 8 8 8
./create_grid_xfm _grid.mnc _t3.xfm
../xfminvert -clobber _t3.xfm _t4.xfm


# Test primary transforms.
#
./test_xfm 10000 _t1.xfm
./test_xfm 10000 _t2.xfm
./test_xfm 10000 _t3.xfm
./test_xfm 10000 _t4.xfm


# Test concatenations.
#
../xfmconcat -clobber _t1.xfm _t3.xfm _t5.xfm
./test_xfm 10000 _t5.xfm

../xfmconcat -clobber _t1.xfm _t4.xfm _t6.xfm
./test_xfm 10000 _t6.xfm

../xfmconcat -clobber _t5.xfm _t6.xfm _t7.xfm
./test_xfm 10000 _t7.xfm

../xfminvert -clobber _t7.xfm _t8.xfm
./test_xfm 10000 _t8.xfm

../xfmconcat -clobber _t8.xfm _t2.xfm _t4.xfm _t7.xfm _t9.xfm
./test_xfm 10000 _t9.xfm
