#!/bin/csh -ef

set tests = (minc_types icv_range icv icv_dim icv_dim1 icv_fillvalue)

foreach test ($tests)

   echo Testing $test
   ./$test -2 > junk.out
   diff $srcdir/$test.out junk.out && rm junk.out

end

