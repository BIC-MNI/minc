#!/bin/csh -f

echo Testing minc_test_types
minc_test_types > junk.out
diff minc_test_types.out junk.out

echo Testing test_icv_range
test_icv_range > junk.out
diff test_icv_range.out junk.out

echo Testing test_icv_dim
test_icv_dim > junk.out
diff test_icv_dim.out junk.out
diff test_icv_dim.mnc test.mnc

echo Testing test_icv_dim1
test_icv_dim1 > junk.out
diff test_icv_dim1.out junk.out
diff test_icv_dim1.mnc test.mnc

echo Testing test_icv_fillvalue
test_icv_fillvalue > junk.out
diff test_icv_fillvalue.out junk.out
diff test_icv_fillvalue.mnc test.mnc

