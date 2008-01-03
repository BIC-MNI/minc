#! /bin/sh

set -e

tests="minc_types icv_range icv icv_dim icv_dim1 icv_fillvalue"

for test in $tests;
do
   echo Testing $test
   ./$test > junk.out
   diff $srcdir/$test.out junk.out && rm junk.out

done

