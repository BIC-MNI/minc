#! /bin/sh

# Exit if error.
set -e

./test-xfm 10000 $srcdir/t1.xfm
./test-xfm 10000 $srcdir/t2.xfm

# Here's an illustration of how lousy the grid-transform inversion is:
# using a tolerance of 0.8 will fail!
./test-xfm 10000 $srcdir/t3.xfm 0.9


