#!/bin/csh -f

set tests = (minc_types icv_range icv icv_dim icv_dim1 icv_fillvalue)

foreach test ($tests)

   echo Testing $test
   $test > junk.out
   diff $test.out junk.out

end

