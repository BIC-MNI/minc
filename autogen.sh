#! /bin/sh

if [ ! -r m4/mni_REQUIRE_LIB.m4 ]; then
    cat <<EOF
The required m4 files were not found.
EOF
    exit 1
fi

# On Mac OS X, the GNU libtool is named 'glibtool'.
HOSTOS=`uname`
LIBTOOLIZE=libtoolize
if test "$HOSTOS"x = Darwinx; then
  LIBTOOLIZE=glibtoolize
fi

aclocal -I m4
autoheader
$LIBTOOLIZE --automake
automake --add-missing --copy
autoconf

