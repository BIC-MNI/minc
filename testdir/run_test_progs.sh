#! /bin/sh

set -e

root=`pwd`/..
progs=${root}/progs

PATH=${root}:${progs}/mincdiff::${progs}/mincpik:${progs}/mincheader:${progs}/minchistory:${progs}/mincview:${PATH}
export PATH

mincheader icv.mnc > /dev/null
mincdiff icv.mnc icv.mnc > /dev/null
