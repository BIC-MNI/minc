#! /bin/sh

# Exit if error.
set -e

./test-xfm 10000 $srcdir/t1.xfm
./test-xfm 10000 $srcdir/t2.xfm

# Transformation t3 is non-injective, so we have to set the tolerance
# fairly large.
./test-xfm 10000 $srcdir/t3.xfm 0.9


