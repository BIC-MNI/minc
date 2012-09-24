#! /bin/sh

if [ ! -r m4/mni_REQUIRE_LIB.m4 ]; then
    cat <<EOF
The required m4 files were not found.
EOF
    exit 1
fi

# some systems need libtoolize, some glibtoolize 
echo -n "testing for glibtoolize ... "
if glibtoolize --version >/dev/null 2>&1; then
   LIBTOOLIZE=glibtoolize
   echo using glibtoolize 
else
   LIBTOOLIZE=libtoolize
   echo using libtoolize 
fi


aclocal -I m4
autoheader
$LIBTOOLIZE --automake
automake --add-missing --copy --force-missing
autoconf

