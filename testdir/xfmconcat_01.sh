#! /bin/sh
#
# Test concatenation of grid transforms. 

set -e

# Create volume to use as displacement grid.
#
dd if=/dev/zero | ../rawtominc -vector 3 -byte -clobber _grid.mnc 8 8 8 

./create_grid_xfm _grid.mnc _t1.xfm
./create_grid_xfm _grid.mnc _t2.xfm
../xfmconcat -clobber _t1.xfm _t2.xfm _t3.xfm

# Make sure the displacement volumes have unique filenames.
#
test -n "`grep Displacement_Volume _t3.xfm |uniq -u`" || exit 1

exit 0

