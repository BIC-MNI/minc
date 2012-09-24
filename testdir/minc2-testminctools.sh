#! /bin/sh
#
# Test minc tools on minc2 file.

set -e

../../mincconvert ../../volume_io/Testing/t3_grid_0.mnc t3_grid_0_2.mnc -2 -clobber && echo "Converted file to MINC2.0 format." || exit 1
 
../../mincinfo -minc_version -image_info t3_grid_0_2.mnc

../../mincmath -const 1 -mult t3_grid_0_2.mnc t32.mnc -clobber || exit 1

echo "Statistics on image"
../../mincstats -mean -std t32.mnc || exit 1

echo "Adding const 2 to all three channels."
../../mincmath -const 2 -add t32.mnc t32_added.mnc -clobber || exit 1

echo "Statistics on image with added constant"
../../mincstats -mean -std t32_added.mnc || exit 1

echo "Resampling file with t1.xfm transform"
../../mincresample -transform ../../volume_io/Testing/t1.xfm -tfm_input_sampling t32_added.mnc t32_transformed.mnc -clobber || exit 1

echo "Transformed file"
../../mincinfo t32_transformed.mnc

exit 0

