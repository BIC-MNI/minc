#! /bin/sh

if [ ! -r m4/mni_REQUIRE_LIB.m4 ]; then
    cat <<EOF
The required m4 files were not found.
EOF
    exit 1
fi

aclocal -I m4
autoheader
automake --add-missing --copy --force-missing
autoconf

