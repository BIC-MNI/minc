#! /bin/sh

set -e

mincheader icv.mnc > /dev/null
mincdiff icv.mnc icv_dim.mnc > /dev/null


